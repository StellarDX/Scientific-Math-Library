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
#include <SciMath/StellarDX-GMP/Adders.h>
#include <SciMath/StellarDX-GMP/Subtractors.h>
#include <SciMath/StellarDX-GMP/Multipliers.h>
#include <SciMath/StellarDX-GMP/Dividers.h>
#include <SciMath/StellarDX-GMP/Logics.h>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <concepts>
#include <memory>
#include <format>
#include <charconv>
#include <meta>
#include <ranges>
#include <string_view>

_80000_BEGIN

// 默认运算器

/**
 * @brief 默认加法器
 * @ingroup IPZ
 */
extern std::shared_ptr<ALU::Adder> DefaultAdder;

/**
 * @brief 默认减法器
 * @ingroup IPZ
 */
extern std::shared_ptr<ALU::Subtractor> DefaultSubtractor;

/**
 * @brief 默认单块乘法器
 * @ingroup IPZ
 */
extern std::shared_ptr<ALU::SingleBlockMultiplier> DefaultSingleBlockMultiplier;

/**
 * @brief 默认乘法器
 * @ingroup IPZ
 */
extern std::shared_ptr<ALU::Multiplier> DefaultMultiplier;

/**
 * @brief 默认除法器
 * @ingroup IPZ
 */
extern std::shared_ptr<ALU::Divider> DefaultDivider;

_CONTAINER_BEGIN

/**
 * @brief 概念：判断类型是否为整数引用
 * @ingroup IPZ
 * @tparam T 待检查的类型
 */
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

        auto Prefix = StrInput.substr(StartPos, 2);
        if (Prefix == "0x" ||  Prefix == "0X")
        {
            Base = 16;
            StrInput.erase(StartPos, 2);
        }
        else if (Prefix == "0b" || Prefix == "0B")
        {
            Base = 2;
            StrInput.erase(StartPos, 2);
        }
        else if (Prefix == "0o" || Prefix == "0O")
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
     * @brief 计算基数的2的幂次指数
     * @param b 基数
     * @return constexpr auto 指数，如果不是2的幂则返回0
     */
    static constexpr auto __Base_Pow_Of_2(unsigned b)
    {
        switch (b)
        {
            case 2: return 1;
            case 4: return 2;
            case 8: return 3;
            case 16: return 4;
            case 32: return 5;
            case 64: return 6;
            case 128: return 7;
            case 256: return 8;
            default: return 0;
        }
    };

    /**
     * @brief 生成基数信息用于非2幂进制的转换
     * @param b 基数
     * @return std::tuple<size_t, BlockType> {exp is the corresponding exponent., P is the largest power of the base which fits in one limb}
     */
    static auto __generate_base_info (BlockType b)
    {
        size_t Exp = 1; // exp is the corresponding exponent.
        BlockType M = BMASK / b, P = b; // P is the largest power of the base which fits in one limb
        for (; P <= M; ++Exp)
        {
            P *= b;
        }
        return std::tuple{Exp, P};
    }

    /**
     * @brief 根据字符串预计算需要的块数
     * @param Input 字符串
     * @return std::size_t 块数
     */
    static std::size_t __Precompute_Size_From_String(const std::string_view& Input)
    {
        auto II = std::find_if(Input.begin(), Input.end(), [](char c){return !(::isspace(c));});
        auto IE = Input.end();
        if (II == IE) {return 0;} // 说明字符串里只有空格

        if (*II == '+' || *II == '-') {++II;}
        if (II == IE) {return 0;} // 说明字符串里只有一个+或一个-

        int Base = 10;
        if (*II == '0')
        {
            if (II + 1 != IE && (*(II + 1) == 'x' || *(II + 1) == 'X'))
            {
                II += 2;
                if (II == IE) {return 0;} // 说明字符串里只有一个0x
                Base = 16;
            }
            else if (II + 1 != IE && (*(II + 1) == 'b' || *(II + 1) == 'B'))
            {
                II += 2;
                if (II == IE) {return 0;} // 说明字符串里只有一个0b
                Base = 2;
            }
            else if (II + 1 != IE && (*(II + 1) == 'o' || *(II + 1) == 'O'))
            {
                II += 2;
                if (II == IE) {return 0;} // 说明字符串里只有一个0o
                Base = 8;
            }
            else
            {
                II += 1;
                if (II == IE) {return 1;} // 说明字符串里只有一个0
                Base = 8;
            }
        }
        
        // static constexpr unsigned BasePowOf2[17] // sorry, unimplemented: 不平凡的代理初始值设定不受支持
        // {
        //     [2] = 1, [4] = 2, [8] = 3, [16] = 4
        //     //[32] = 5, [64] = 6, [128] = 7, [256] = 8 // 到不了这么大
        // };

        auto DN = std::distance(II, IE);
        auto Bits = __Base_Pow_Of_2(Base);

        if (Bits > 0)
        {
            return (DN * Bits + BSIZE - 1) / BSIZE;
        }
        else [[likely]]
        {
            auto [Exp, BB] = __generate_base_info(Base);
            return (DN + Exp - 1) / Exp;
        }
    }

    /**
     * @brief 从2的幂进制字符串解析到块数组
     * @param __first 起始指针
     * @param __last 结束指针
     * @param __val 目标块数组视图
     * @param __bwidth 每个字符代表的位宽
     */
    static constexpr void 
        __from_chars_pow2_base(const char* __first, const char* __last, BlockArrayView __val,
        int __bwidth)
    {
        // 使用GMP实现
        BlockType Block = 0;
        std::size_t Shift = 0;
        auto OI = __val.begin();
        for (auto II = std::reverse_iterator{__last}; II != std::reverse_iterator{__first}; ++II)
        {
            Block |= BlockType(*II) << Shift;
            Shift += __bwidth;
            if (Shift >= BSIZE)
            {
                Shift -= BSIZE;
                *OI = Block;
                ++OI;
                // Next line is correct also if shift == 0,
                // bits == 8, and mp_limb_t == unsigned char.
                Block = (unsigned int)(*II) >> (__bwidth - Shift);
            }
        }
        if (Block) {*OI = Block;} // 是0的情况后续会自动处理
    }
    
    /**
     * @brief 从字母数字进制字符串解析到块数组
     * @param __first 起始指针
     * @param __last 结束指针
     * @param __val 目标块数组视图
     * @param __base 基数
     * @param __exp __generate_base_info预计算的指数
     * @param __bb __generate_base_info预计算的基数幂值
     * @see __generate_base_info
     * @note 这个是十进制专用的，性能相对2^n进制会很拉，因此开优化（当然再怎么优化也夯不过标准库）
     */
    static constexpr void __declspec(optimize("O2")) 
    // 这个是十进制专用的，性能相对2^n进制会很拉，因此开优化（当然再怎么优化也夯不过标准库）
        __from_chars_alnum(const char* __first, const char* __last, BlockArrayView __val,
        int __base, unsigned __exp, BlockType __bb)
    {
        // 使用GMP实现
        auto sn = std::distance(__first, __last);
        unsigned k = 1 + (sn - 1) % __exp;
        BlockType w = *__first;
        ++__first;
        --k;
        while (k != 0)
        {
            w = w * __base + (*__first);
            ++__first;
            --k;
        }

        __val.front() = w;
        size_t rn = 1;

        while (__first != __last)
        {
            BlockType cy, cym, cya;
            w = *__first;
            ++__first;
            for (k = 1; k < __exp; ++k)
            {
                w = w * __base + (*__first);
                ++__first;
            }
            ALU::MUL1(__val.subspan(0, rn), __val.subspan(0, rn), __bb, &cym, 
                DefaultSingleBlockMultiplier.get());
            ALU::INC(__val.subspan(0, rn), w, &cya);
            cy = cym + cya;
            if (cy > 0)
            {
                __val.at(rn) = cy;
                ++rn;
            }
        }
    }

    /**
     * @brief 将块数组转换为字符串
     * @param __first 输出缓冲区起始
     * @param __last 输出缓冲区结束
     * @param __value 源块数组视图
     * @param __base 目标进制
     * @return std::to_chars_result 结果状态
     */
    static std::to_chars_result BlockArrayToString(char* __first, char* __last, 
        BlockArraySrcView __value, int __base = 10)
    {

        *__first = '0'; // 占位符，TODO
        return std::to_chars_result{__first + 1};
    }

    /**
     * @brief 从字符串解析到块数组
     * @param __first 输入起始
     * @param __last 输入结束
     * @param __value 目标块数组视图
     * @param __base 目标进制
     * @return std::from_chars_result 解析结果
     */
    static std::from_chars_result BlockArrayFromString(const char* __first, const char* __last, 
        BlockArrayView __value, int __base = 10)
    {
        // 查找第一个无效字符作为结束位置
        std::from_chars_result Res;
        switch (__base)
        {
        case 2:
            Res.ptr = std::find_if(__first, __last, [](char c)
            {
                return !(c == '+' || c == '-' || c == '0' || c == '1');
            });
            break;
        case 8:
            Res.ptr = std::find_if(__first, __last, [](char c)
            {
                return !(c == '+' || c == '-' || (c >= '0' && c <= '7'));
            });
            break;
        case 10:
            Res.ptr = std::find_if(__first, __last, [](char c)
            {
                return !(c == '+' || c == '-' || (c >= '0' && c <= '9'));
            });
            break;
        case 16:
            Res.ptr = std::find_if(__first, __last, [](char c)
            {
                return !(c == '+' || c == '-' || (c >= '0' && c <= '9') || 
                    (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
            });
            break;
        }
        __last = Res.ptr;
        if (__first == __last)
        {
            Res.ec = std::errc::invalid_argument;
            return Res;
        }

        // 预处理字符串
        bool Negative = 0;
        if (*__first == '+' || *__first == '-')
        {
            Negative = *__first == '-';
            ++__first;
        }
        std::string PreprocessedString(__first, __last);
        if (PreprocessedString.empty())
        {
            Res.ec = std::errc::invalid_argument;
            return Res;
        }
        for (auto sp = PreprocessedString.begin(); sp != PreprocessedString.end(); ++sp)
        {
            unsigned digit;
            if (::isspace((unsigned char)(*sp))) {continue;}
            else if (*sp >= '0' && *sp <= '9') {digit = *sp - '0';}
            else if (*sp >= 'a' && *sp <= 'f') {digit = *sp - 'a';}
            else if (*sp >= 'A' && *sp <= 'F') {digit = *sp - 'A' + 10;}
            else {digit = __base;} // fail
            *sp = digit;
        }

        // 转换
        auto First = PreprocessedString.c_str();
        auto Last = PreprocessedString.c_str() + PreprocessedString.size();
        auto bits = __Base_Pow_Of_2(__base);
        if (bits)
        {
            __from_chars_pow2_base(First, Last, __value, bits);
        }
        else
        {
            auto [Exp, BB] = __generate_base_info(__base);
            __from_chars_alnum(First, Last, __value, __base, Exp, BB);
        }

        if (Negative) {ALU::NEG(__value, __value);}

        return Res;
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
     * @note 使用字符串构造的数字强制32位对齐
     */
    InftyIntegerContainer(std::string InputStr, bool Signed = 1)
        : InftyIntegerContainer(Signed)
    {
        if constexpr (IsDynamicContainer<ValueType>)
        {
            Metadata.Partitions = {__Bmalloc(&_Data, __Precompute_Size_From_String(InputStr))};
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
        if constexpr (IsDynamicContainer<ContainerType>)
        {
            Metadata.Partitions.front() = __Bmalloc(&_Data, Size, Unit);
        }
        else
        {
            Panic<std::runtime_error>(
                std::format("InftyIntegerContainer: 当前存储类型为{}，此类型不支持扩/缩容",
                std::meta::display_string_of(^^ContainerType)).c_str());
        }
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