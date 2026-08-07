/**
 * @file Integer.h
 * @defgroup IPZ 无限长度整数
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 无限长度整数类模板
 * @version 26H2
 * @date 2026-08-04
 */

/*
    Copyright (c) 2026 StellarDX Astronomy

    Scientific Math Library is free software; you can redistribute it and/or 
    modify it under the terms of either:

      * the GNU Lesser General Public License as published by the Free
        Software Foundation; either version 3 of the License, or (at your
        option) any later version.

    or

      * the GNU General Public License as published by the Free Software
        Foundation; either version 2 of the License, or (at your option) any
        later version.

    or both in parallel, as here.

    Scientific Math Library is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received copies of the GNU General Public License and the
    GNU Lesser General Public License along with the Scientific Math Library.
    If not, see https://www.gnu.org/licenses/.
*/

#pragma once

#ifndef __INTEGER__
#define __INTEGER__

#include <SciMath/StellarDX-GMP/SMLDefs.h>
#include <SciMath/StellarDX-GMP/Container.h>
#include <SciMath/StellarDX-GMP/Memory.h>
#include <type_traits>
#include <concepts>
#include <memory>
#include <format>
#include <charconv>
#include <meta>
#include <ranges>
#include <string_view>

_80000_BEGIN
_CONTAINER_BEGIN

template <typename T>
concept IsIntegerReference = std::is_reference_v<T> && 
    std::is_integral_v<std::remove_reference_t<T>>;

/**
 * @brief 整数容器基类
 * @ingroup IPZ
 */
class IntegerContainer : public NumericContainer
{
public:
    using Mybase             = NumericContainer;
    /**
     * @brief 转换为字符串的函数签名模板
     * @tparam ITy 输入数据类型
     */
    template<typename ITy> 
    requires std::integral<ITy> || std::is_same<ITy, BlockArrayConstView>::value
    using ToStringFuncType   = std::to_chars_result(char*, char*, ITy, int);
    /**
     * @brief 从字符串解析的函数签名模板
     * @tparam IRef 引用或视图类型
     */
    template<typename IRef> 
    requires IsIntegerReference<IRef> || std::is_same<IRef, BlockArrayView>::value
    using FromStringFuncType = std::from_chars_result(const char*, const char*, IRef, int);

    /// @brief 类型名称标识
    constexpr static const auto NType = "Integer";

    /**
    * @brief 字符串进制
    */
    enum ToStringBase
    {
        StrBinary      = 2,  ///< 二进制
        StrOctal       = 8,  ///< 八进制
        StrDecimal     = 10, ///< 十进制
        StrHexadecimal = 16  ///< 十六进制
    };

protected:
    MetadataType Metadata; ///< 元数据

public:
    /**
     * @brief 调整容器大小（整数类型通常不支持动态扩缩容）
     * @param Partition 分区索引
     * @param Size 新的大小
     * @throws std::runtime_error 总是抛出异常，因为基本整数容器不支持此操作
     */
    void Adjust(std::size_t Partition, size_t Size, AllocSizeUnit Unit)override
    {
        Panic<std::runtime_error>("BasicIntegerContainer：此类型不支持扩/缩容");
    }

    MetadataType GetMetadata()const override{return Metadata;}

    /**
     * @brief 从字符串解析整数（通用实现）
     * @tparam ITy 目标整数类型或视图
     * @param Dst 目标存储位置
     * @param StrInput 输入字符串
     * @param Func 具体的解析函数（可以自行实现）
     * @throws std::invalid_argument 如果字符串格式无效
     */
    template<typename ITy>
    static void IntegerFromString(ITy Dst, std::string StrInput, FromStringFuncType<ITy> Func)
        requires IsIntegerReference<ITy> || std::is_same<ITy, BlockArrayView>::value
    {
        if (StrInput.empty())
        {
            if constexpr (IsIntegerReference<ITy>) {Dst = 0;}
            return;
        }

        std::size_t StartPos = 0;
        int Base = 10;

        if (StrInput.front() == '+' || StrInput.front() == '-') {StartPos = 1;}

        if (StrInput.substr(StartPos, 2) == "0x" || 
            StrInput.substr(StartPos, 2) == "0X")
        {
            Base = 16;
            StrInput.erase(StartPos, 2);
        }
        else if (StrInput.substr(StartPos, 2) == "0b" ||
            StrInput.substr(StartPos, 2) == "0B")
        {
            Base = 2;
            StrInput.erase(StartPos, 2);
        }
        else if (StrInput.substr(StartPos, 2) == "0o" ||
            StrInput.substr(StartPos, 2) == "0O")
        {
            Base = 8;
            StrInput.erase(StartPos, 2);
        }
        else if (StrInput.size() >= 2 && StrInput.at(StartPos) == '0')
        {
            Base = 8;
            StrInput.erase(StartPos, 1);
        }
        
        if (!StrInput.empty())
        {
            auto Result = Func(
                StrInput.c_str(), StrInput.c_str() + StrInput.size(), Dst, Base);
            if (Result.ec != std::errc())
            {
                Panic("无效的整数字符串");
            }
        }
        else {Panic("无效的整数字符串");}
    }

    /**
     * @brief 转换为指定进制的字符串（通用函数）
     * @param Src 输入，可以是一个原生整型数字或一个块数组的View
     * @param Base 目标进制 (2, 8, 10, 16)
     * @param Func 转换用的函数（可以自行实现）
     * @return std::string 转换后的字符串，包含相应的前缀（如 0x, 0b）
     */
    template<typename ITy>
    static std::string IntegerToString(ITy Src, ToStringBase Base, ToStringFuncType<ITy> Func)
        requires std::integral<ITy> || std::is_same<ITy, BlockArrayConstView>::value // 大数存储全部抽象为view
    {
        std::size_t SizeBit;
        if constexpr (std::integral<ITy>) {SizeBit = 8 * sizeof(ITy);}
        else {SizeBit = BSIZE * Src.size();}

        std::size_t BufSize = std::size_t(SizeBit + 10);
        std::shared_ptr<char[]> Buffer(new char[BufSize]);
        auto [Ptr, Err] = Func(Buffer.get(), Buffer.get() + BufSize, Src, int(Base));
        *Ptr = '\0';
        std::string Result{Buffer.get()};
        std::size_t InsertPrefixPos = 0;
        if (Result.front() == '-') {InsertPrefixPos = 1;}

        switch (Base)
        {
        case IntegerContainer::StrBinary:
            Result.insert(InsertPrefixPos, "0b");
            break;
        case IntegerContainer::StrOctal:
            Result.insert(InsertPrefixPos, "0");
            break;
        case IntegerContainer::StrDecimal:
            return Result;
            break;
        case IntegerContainer::StrHexadecimal:
            std::transform(Result.begin(), Result.end(), Result.begin(),
                [](char c){return toupper(c);});
            Result.insert(InsertPrefixPos, "0x");
            break;
        }

        return Result;
    }

    /**
     * @brief 转换为指定进制的字符串
     * @param Base 目标进制 (2, 8, 10, 16)
     * @return std::string 转换后的字符串，包含相应的前缀（如 0x, 0b）
     */
    virtual std::string ToString(ToStringBase Base)const = 0;
};

/**
 * @brief 基础整数容器
 * @ingroup IPZ
 * @tparam ContainerType 底层存储的整数类型，必须为整数类型
 * @details 封装原始数据类型表示的整数
 */
template<std::integral ContainerType>
class BasicIntegerContainer : public IntegerContainer
{
public:
    using Mybase    = IntegerContainer;
    using ValueType = ContainerType;

    /// @brief 根据值类型计算所需的块数组长度
    static const size_t BlkArrSize = BlocksAsneeded<ValueType>();

    /**
     * @brief 联合存储类型
     * @details 通过原始值或块数组表示存储的值
     */
    union DetailedType
    {
        ValueType RawData;
        BlockType Blocks[BlkArrSize];
    };

protected:
    DetailedType _Data; ///< 数据

public:
    /**
     * @brief 默认构造函数
     * @details 填写元数据
     */
    BasicIntegerContainer()
    {
        Metadata.TypeName = "BasicInteger";
        Metadata.Format = this->NType;
        Metadata.IsExternal = 0;
        Metadata.IsDynamicSized = 0;
        Metadata.IsSigned = std::is_signed_v<ValueType>;
        Metadata.Partitions = {__Balloc(sizeof(ValueType), AllocByte)};
    }

    /**
     * @brief 从整数值构造
     * @param Data 初始整数值
     */
    explicit BasicIntegerContainer(ValueType Data) 
        : BasicIntegerContainer()
    {
        _Data.RawData = Data;
    }

    /**
     * @brief 从字符串构造
     * @param StrInput 输入字符串
     * @details 支持以下前缀自动检测：
     *          - "0x" 或 "0X": 十六进制
     *          - "0b" 或 "0B": 二进制
     *          - "0": 八进制 (仅当后续字符存在时)
     *          - 其他: 十进制
     * @throws std::invalid_argument 如果字符串格式无效或超出类型范围
     */
    BasicIntegerContainer(std::string StrInput) 
        : BasicIntegerContainer()
    {
        IntegerFromString<ValueType&>(_Data.RawData, StrInput, std::from_chars);
    }

    BlockArrayView GetRawData()override{return _Data.Blocks;}
    BlockArrayConstView GetRawData()const override{return _Data.Blocks;}
    BlockArrayConstView GetConstRawData()const override{return _Data.Blocks;}
    constexpr size_t size()const override{return BlkArrSize;}
    constexpr size_t size_byte()const override{return sizeof(ValueType);}
    constexpr size_t size_bit()const override{return size_byte() * 8;}

    void Write(BlockArrayConstView NewData)override
    {
        if (NewData.size() < BlkArrSize)
        {
            ALU::MOV(_Data.Blocks, NewData);
        }
        else {ALU::MOV(_Data.Blocks, NewData.subspan(0, BlkArrSize));}
        std::size_t EffectiveEnd = Metadata.Partitions.front().End.Offset + 1;
        if (EffectiveEnd < BSIZE) 
        {
            _Data.Blocks[BlkArrSize - 1] &= ((1 << EffectiveEnd) - 1);
        }
    }

    std::string ToString(ToStringBase Base)const override
    {
        return IntegerToString(_Data.RawData, Base, std::to_chars);
    }

    std::string ToString()const override
    {
        return IntegerToString(_Data.RawData, StrDecimal, std::to_chars);
    }
};

/**
 * @brief 无限长度整数容器
 * @ingroup IPZ
 * @tparam ContainerType 底层存储容器，需要满足存储用的容器是连续内存块
 * @details 支持动态大小的整数存储，适用于大数运算。
 * @note 使用非变长容器（含外部链接）构造时，强制32位对齐，变长容器不受此限制
 */
template<std::ranges::contiguous_range ContainerType>
class InftyIntegerContainer : public IntegerContainer
{
public:
    using Mybase    = IntegerContainer;
    using ValueType = ContainerType;

protected:
    ValueType _Data; ///< 动态数据存储

    /**
     * @brief 调整大小实现（非动态容器）
     * @throws std::runtime_error 总是抛出异常，因为非动态容器不支持扩缩容
     */
    void _Resize(std::size_t Partition, size_t Size, AllocSizeUnit Unit)
    {
        Panic<std::runtime_error>(
            std::format("InftyIntegerContainer: 当前存储类型为{}，此类型不支持扩/缩容",
            std::meta::display_string_of(^^ContainerType)).c_str());
    }

    /**
     * @brief 调整大小实现（动态容器）
     * @details 重新分配内存并更新元数据分区信息（保留数据）
     */
    void _Resize(std::size_t Partition, size_t Size, AllocSizeUnit Unit)
        requires IsDynamicContainer<ContainerType>
    {
        Metadata.Partitions.front() = __Bmalloc(&_Data, Size, Unit);
    }

    /**
     * @brief 根据字符串预计算需要的位数
     * @param Input 字符串
     * @return std::size_t 位数
     */
    std::size_t __Precompute_Size_From_String(const std::string_view& Input)
    {
        auto II = Input.begin();
        auto IE = Input.end();
    }

public:
    /**
     * @brief 填写元数据
     * @param Signed 是否有符号
     */
    InftyIntegerContainer(bool Signed)
    {
        Metadata.TypeName = "InftyInteger";
        Metadata.Format = this->NType;
        Metadata.IsExternal = 
            std::bool_constant<std::ranges::view<ContainerType>>::value;
        Metadata.IsDynamicSized = 
            std::bool_constant<IsDynamicContainer<ContainerType>>::value;
        Metadata.IsSigned = Signed;
    }

    /**
     * @brief 默认构造函数
     * @param Value 初始值容器
     * @param Signed 是否为有符号数
     */
    InftyIntegerContainer(const ValueType& Value = ValueType(), bool Signed = 1)
        : InftyIntegerContainer(Signed)
    {
        Metadata.Partitions = {__Balloc(std::size(Value), AllocBlock)};
        if constexpr (IsDynamicContainer<ContainerType> || std::ranges::view<ContainerType>)
        {
            _Data = Value;
        }
        else
        {
            std::copy(std::begin(Value), std::end(Value), std::begin(_Data));
        }
    }

    /**
     * @brief 构造指定大小的容器
     * @param AllocSize 分配大小
     * @param Unit 分配单位
     * @param Signed 是否为有符号数
     * @note ContainerType必须支持resize时才能使用
     */
    InftyIntegerContainer(std::size_t AllocSize, AllocSizeUnit Unit = AllocBlock, bool Signed = 1)
        requires IsDynamicContainer<ContainerType> : InftyIntegerContainer(Signed)
    {
        Metadata.Partitions = {__Bmalloc(&_Data, AllocSize, Unit)};
    }

    /**
     * @brief 从字符串构造
     * @param InputStr 输入字符串
     * @param Signed 是否有符号
     * @todo 实现大数字符串解析逻辑
     */
    InftyIntegerContainer(std::string InputStr, bool Signed = 1)
        : InftyIntegerContainer(Signed)
    {
        if constexpr (IsDynamicContainer<ValueType>)
        {
            Metadata.Partitions = {__Bmalloc(&_Data, __Precompute_Size_From_String(InputStr), AllocBit)};
        }
        IntegerFromString(BlockArrayView{_Data}, InputStr, BlockArrayFromString);
        if constexpr (IsDynamicContainer<ValueType>)
        {
            if (_Data.size() > 1 && _Data.back() == 0) [[unlikely]]
            {
                Adjust(0, _Data.size() - 1, AllocBlock);
            }
        }
    }
    
    BlockArrayView GetRawData()override{return _Data;}
    BlockArrayConstView GetRawData()const override{return _Data;}
    BlockArrayConstView GetConstRawData()const override{return _Data;}

    size_t size()const override{return std::size(_Data);}

    size_t size_byte()const override
    {
        auto EndPoint = Metadata.Partitions.back().End;
        EndPoint.Offset += 1;
        return (EndPoint.BlockIndex * BBYTE) + (EndPoint.Offset / 8) + 
            (EndPoint.Offset % 8 ? 1 : 0);
    }

    size_t size_bit()const override
    {
        auto EndPoint = Metadata.Partitions.back().End;
        return EndPoint.BlockIndex * BSIZE + EndPoint.Offset + 1;
    }

    void Adjust(std::size_t Partition, size_t Size, AllocSizeUnit Unit)override
    {
        _Resize(Partition, Size, Unit);
    }

    void Write(BlockArrayConstView NewData)override
    {
        if (NewData.size() < std::size(_Data))
        {
            ALU::MOV(_Data, NewData);
        }
        else {ALU::MOV(_Data, NewData.subspan(0, std::size(_Data)));}
        std::size_t EffectiveEnd = Metadata.Partitions.front().End.Offset + 1;
        if (EffectiveEnd < BSIZE) 
        {
            _Data[std::size(_Data) - 1] &= ((1 << EffectiveEnd) - 1);
        }
    }

    std::string ToString()const override
    {
        return IntegerToString(BlockArrayConstView{_Data}, StrDecimal, BlockArrayToString);
    }

    std::string ToString(ToStringBase Base)const override
    {
        return IntegerToString(BlockArrayConstView{_Data}, Base, BlockArrayToString);
    }

    static std::to_chars_result BlockArrayToString(char* __first, char* __last, 
        BlockArraySrcView __value, int __base = 10)
    {
        *__first = '0'; // 占位符，TODO
        return std::to_chars_result{__first + 1};
    }

    static std::from_chars_result BlockArrayFromString(const char* __first, const char* __last, 
        BlockArrayView __value, int __base = 10)
    {
        return std::from_chars_result{__first}; // 占位符，TODO
    }
};

_CONTAINER_END

class Integer
{
public:
    using ValueType = _CONTAINER IntegerContainer;
    using Pointer   = std::shared_ptr<ValueType>;
    using Reference = ValueType&;

protected:
    Pointer PData;

public:
    // TODO
};

_80000_END

#endif