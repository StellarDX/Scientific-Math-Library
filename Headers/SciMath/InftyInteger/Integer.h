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
#include <concepts>
#include <memory>
#include <format>
#include <charconv>
#include <string_view>

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
        Metadata.IsDynamicSized = 0;
        Metadata.IsSigned = std::is_signed_v<ValueType>;
        std::size_t BlockSz = BlkArrSize - 1;
        std::size_t Offset = (sizeof(ValueType) * 8 - 1) % BSIZE;
        Metadata.Partitions = {{{0, 0}, {BlockSz, Offset}, "WholeStorage"}};
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
    BasicIntegerContainer(std::string_view StrInput) 
        : BasicIntegerContainer()
    {
        int Base = 10;
        auto First = StrInput.begin();
        if (StrInput.substr(0, 2) == "0x")
        {
            Base = 16;
            First += 2;
        }
        else if (StrInput.substr(0, 2) == "0b")
        {
            Base = 2;
            First += 2;
        }
        else if (StrInput.size() >= 2 && StrInput.front() == '0')
        {
            Base = 8;
            First += 1;
        }
        std::from_chars(First, StrInput.end(), _Data.RawData, Base);
    }

    BlockArrayView GetRawData()override{return _Data.Blocks;}
    BlockArrayConstView GetRawData()const override{return _Data.Blocks;}
    BlockArrayConstView GetConstRawData()const override{return _Data.Blocks;}
    MetadataType GetMetaData()const override{return Metadata;}
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
        std::size_t BufSize = std::size_t(double(size_bit()) * 0.30103 + 2.);
        std::shared_ptr<char[]> Buffer(new char[BufSize]);
        std::to_chars(Buffer.get(), Buffer.get() + BufSize, _Data.RawData, int(Base));
        std::string Result{Buffer.get()};
        switch (Base)
        {
        case IntegerContainer::StrBinary:
            return "0b" + Result;
        case IntegerContainer::StrOctal:
            return "0" + Result;
        case IntegerContainer::StrDecimal:
            return Result;
        case IntegerContainer::StrHexadecimal:
            std::transform(Result.begin(), Result.end(), Result.begin(),
                [](char c){return toupper(c);});
            return "0x" + Result;
        }
    }

    std::string ToString()const override {return ToString(StrDecimal);}
};

template<std::ranges::contiguous_range ContainerType>
class InftyIntegerContainer : public IntegerContainer
{
public:
    // TODO
    // virtual BlockArrayView GetRawData() = 0;
    // virtual BlockArrayConstView GetRawData()const = 0;
    // virtual BlockArrayConstView GetConstRawData()const = 0;
    // virtual Metadata GetMetaData()const = 0;
    // virtual size_t size()const = 0;
    // virtual size_t size_byte()const = 0;
    // virtual size_t size_bit()const = 0;
    // virtual std::string ToString()const = 0;
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