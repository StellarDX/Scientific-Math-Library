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

#include <stdexcept>
#ifndef __INTEGER__
#define __INTEGER__

#include <SciMath/StellarDX-GMP/SMLDefs.h>
#include <SciMath/StellarDX-GMP/Container.h>
#include <concepts>
#include <memory>
#include <format>
#include <charconv>
#include <meta>

_80000_BEGIN
_CONTAINER_BEGIN

/**
 * @brief 整数容器基类
 * @ingroup IPZ
 */
class IntegerContainer : public NumericContainer
{
public:
    using Mybase = NumericContainer;

    /// @brief 类型名称标识
    constexpr static const auto NType = "Integer";

    /**
    * @brief 字符串进制
    */
    enum ToStringBase
    {
        StrBinary  = 2,     ///< 二进制
        StrOctal   = 8,     ///< 八进制
        StrDecimal = 10,    ///< 十进制
        StrHexadecimal = 16 ///< 十六进制
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
    void Adjust(std::size_t Partition, PartitionSizeType Size)
    {
        Panic<std::runtime_error>("BasicIntegerContainer：此类型不支持扩/缩容");
    }
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
     * @details 初始化元数据
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
     * @param StrInput 输入字符串，支持"0b"(二进制)，"0"(八进制)和"0x"(十六进制)前缀
     * @details 自动检测前缀并解析数值
     */
    BasicIntegerContainer(std::string StrInput) 
        : BasicIntegerContainer()
    {
        if (StrInput.empty())
        {
            _Data.RawData = 0;
            return;
        }

        std::size_t StartPos = 0;
        int Base = 10;

        if (StrInput.front() == '+' || StrInput.front() == '-') {StartPos = 1;}

        if (StrInput.substr(StartPos, 2) == "0x")
        {
            Base = 16;
            StrInput.erase(StartPos, 2);
        }
        else if (StrInput.substr(StartPos, 2) == "0b")
        {
            Base = 2;
            StrInput.erase(StartPos, 2);
        }
        else if (StrInput.size() >= 2 && StrInput.at(StartPos) == '0')
        {
            Base = 8;
            StrInput.erase(StartPos, 1);
        }
        
        if (!StrInput.empty())
        {
            auto Result = std::from_chars(
                StrInput.c_str(), StrInput.c_str() + StrInput.size(), 
                _Data.RawData, Base);
            if (Result.ec != std::errc())
            {
                Panic("无效的整数字符串");
            }
        }
        else {Panic("无效的整数字符串");}
    }

    BlockArrayView GetRawData()override{return _Data.Blocks;}
    BlockArrayConstView GetRawData()const override{return _Data.Blocks;}
    BlockArrayConstView GetConstRawData()const override{return _Data.Blocks;}
    MetadataType GetMetadata()const override{return Metadata;}
    constexpr size_t size()const override{return BlkArrSize;}
    constexpr size_t size_byte()const override{return sizeof(ValueType);}
    constexpr size_t size_bit()const override{return size_byte() * 8;}

    /**
     * @brief 转换为指定进制的字符串
     * @param Base 目标进制
     * @return std::string 转换后的字符串，包含相应的前缀（如 0x, 0b）
     */
    std::string ToString(ToStringBase Base)const
    {
        std::size_t BufSize = std::size_t(size_bit() + 10);
        std::shared_ptr<char[]> Buffer(new char[BufSize]);
        auto [Ptr, Err] = std::to_chars(Buffer.get(), Buffer.get() + BufSize, _Data.RawData, int(Base));
        *Ptr = '\0';
        std::string Result{Buffer.get()};
        std::size_t InsertPrefixPos = 0;
        if (Result.front() == '-') {InsertPrefixPos = 1;}

        switch (Base)
        {
        case IntegerContainer::StrBinary:
            Result.insert(1, "0b");
            break;
        case IntegerContainer::StrOctal:
            Result.insert(1, "0");
            break;
        case IntegerContainer::StrDecimal:
            return Result;
            break;
        case IntegerContainer::StrHexadecimal:
            std::transform(Result.begin(), Result.end(), Result.begin(),
                [](char c){return toupper(c);});
            Result.insert(1, "0x");
            break;
        }

        return Result;
    }

    std::string ToString()const override {return ToString(StrDecimal);}
};

template<typename ContainerType>
concept IsDynamicContainer = requires(ContainerType Container, std::size_t Size)
{
    {Container.resize(Size)} -> std::same_as<void>;
};

/**
 * @brief 无限长度整数容器
 * @ingroup IPZ
 * @tparam ContainerType 底层存储容器，需要满足存储用的容器是连续内存块
 * @details 支持动态大小的整数存储，适用于大数运算。
 */
template<std::ranges::contiguous_range ContainerType>
class InftyIntegerContainer : public IntegerContainer
{
public:
    using Mybase    = IntegerContainer;
    using ValueType = ContainerType;

protected:
    ValueType _Data; ///< 动态数据存储

    void _Resize(std::size_t Partition, PartitionSizeType Size)
    {
        Panic<std::runtime_error>(
            std::format("InftyIntegerContainer: 当前存储类型为{}，此类型不支持扩/缩容",
            std::meta::display_string_of(^^ContainerType)).c_str());
    }

public:
    /**
     * @brief 默认构造函数
     * @param Value 初始值容器
     * @param Signed 是否为有符号数
     */
    InftyIntegerContainer(const ValueType& Value = ValueType(), bool Signed = 1)
    {
        Metadata.TypeName = "InftyInteger";
        Metadata.Format = this->NType;
        Metadata.IsExternal = 
            std::bool_constant<std::ranges::view<ContainerType>>::value;
        Metadata.IsDynamicSized = 
            std::bool_constant<IsDynamicContainer<ContainerType>>::value;
        Metadata.IsSigned = Signed;
        Metadata.Partitions = {__Balloc(std::size(Value), AllocBlock)};
    }

    /**
     * @brief 构造指定大小的容器
     * @param AllocSize 分配大小
     * @param Unit 分配单位
     * @param Signed 是否为有符号数
     * @note ContainerType必须支持resize时才能使用
     */
    InftyIntegerContainer(std::size_t AllocSize, AllocSizeUnit Unit = AllocBlock, bool Signed = 1)
        requires IsDynamicContainer<ContainerType>
    {
        Metadata.TypeName = "InftyInteger";
        Metadata.Format = this->NType;
        Metadata.IsExternal = 0;
        Metadata.IsDynamicSized = 1;
        Metadata.IsSigned = Signed;
        auto Part = __Balloc(AllocSize, Unit);
        Metadata.Partitions = {Part};
        _Data.resize(Part.End.BlockSize + 1);
    }

    /**
     * @brief 从字符串构造
     * @param InputStr 输入字符串
     * @todo 实现大数字符串解析逻辑
     */
    InftyIntegerContainer(std::string InputStr)
    {
        // TODO
    }
    
    BlockArrayView GetRawData()override{return _Data;}
    BlockArrayConstView GetRawData()const override{return _Data;}
    BlockArrayConstView GetConstRawData()const override{return _Data;}
    MetadataType GetMetadata()const override{return Metadata;}
    size_t size()const override{return std::size(_Data);}
    size_t size_byte()const override
    {
        auto EndPoint = Metadata.Partitions.back().End;
        EndPoint.Offset += 1;
        return (EndPoint.BlockSize * BBYTE) + (EndPoint.Offset / 8) + 
            (EndPoint.Offset % 8 ? 1 : 0);
    }
    size_t size_bit()const override
    {
        auto EndPoint = Metadata.Partitions.back().End;
        return EndPoint.BlockSize * BSIZE + EndPoint.Offset + 1;
    }

    void Adjust(std::size_t Partition, PartitionSizeType Size)override
    {
        _Resize(Partition, Size);
    }

    std::string ToString()const override
    {
        return std::string(); // 占位符，TODO
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