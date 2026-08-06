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

#include <algorithm>
#include <type_traits>
#ifndef __INTEGER__
#define __INTEGER__

#include <SciMath/StellarDX-GMP/SMLDefs.h>
#include <SciMath/StellarDX-GMP/Container.h>
#include <concepts>
#include <memory>
#include <format>

_80000_BEGIN
_CONTAINER_BEGIN

class IntegerContainer : public NumericContainer
{
public:
    using Mybase = NumericContainer;

    constexpr static const auto NType = "Integer";

    enum ToStringBase
    {
        StrBinary, StrOctal, StrDecimal, StrHexadecimal
    };

protected:
    MetadataType Metadata;

public:
    void Adjust(std::size_t Partition, PartitionSizeType Size)
    {
        Panic<std::runtime_error>("BasicIntegerContainer：此类型不支持扩/缩容");
    }

    template<std::integral Ity>
    static std::string ToBinaryString(Ity AX)
    {
        if (!AX) {return "0b0";}
        static const auto ChTbl = "01";
        std::string Result;
        while(AX)
        {
            Result.push_back(ChTbl[AX & 1]);
            AX >>= 1;
        }
        std::reverse(Result.begin(), Result.end());
        return "0b" + Result;
    }

    template<std::integral Ity>
    static std::string ToOctalString(Ity AX)
    {
        if (!AX) {return "00";}
        static const auto ChTbl = "01234567";
        std::string Result;
        while(AX)
        {
            Result.insert(Result.begin(), ChTbl[AX & 0b111]);
            AX >>= 3;
        }
        return "0" + Result;
    }

    template<std::integral Ity>
    static std::string ToDecimalString(Ity AX)
    {
        return std::format("{}", AX);
    }

    template<std::integral Ity>
    static std::string ToHexadecimalString(Ity AX)
    {
        return std::format("0x{:X}", AX);
    }
};

template<std::integral ContainerType>
class BasicIntegerContainer : public IntegerContainer
{
public:
    using Mybase    = IntegerContainer;
    using ValueType = ContainerType;

    static const size_t BlkArrSize = BlocksAsneeded<ValueType>();

    union DetailedType
    {
        ValueType RawData;
        BlockType Blocks[BlkArrSize];
    };

protected:
    DetailedType _Data;

public:
    BasicIntegerContainer()
    {
        Metadata.TypeName = "BasicInteger";
        Metadata.Format = this->NType;
        Metadata.IsDynamicSized = 0;
        Metadata.IsSigned = std::is_signed_v<ValueType>;
        std::size_t BlockSz = BlkArrSize - 1;
        std::size_t Offset = (sizeof(ValueType) * 8) % BSIZE;
        Metadata.Partitions = {{{0, 0}, {BlockSz, Offset}, "WholeStorage"}};
    }
    explicit BasicIntegerContainer(ValueType Data) : BasicIntegerContainer()
    {
        _Data.RawData = Data;
    }

    BlockArrayView GetRawData()override{return _Data.Blocks;}

    BlockArrayConstView GetRawData()const override{return _Data.Blocks;}

    BlockArrayConstView GetConstRawData()const override{return _Data.Blocks;}

    MetadataType GetMetaData()const override{return Metadata;}

    size_t size()const override{return BlkArrSize;}

    size_t size_byte()const override{return sizeof(ValueType);}

    size_t size_bit()const override{return size_byte() * 8;}

    std::string ToString(ToStringBase Base)const
    {
        switch (Base)
        {
        case StrBinary:
            return ToBinaryString(_Data.RawData);
        case StrOctal:
            return ToOctalString(_Data.RawData);
        case StrDecimal:
            return ToDecimalString(_Data.RawData);
        case StrHexadecimal:
            return ToHexadecimalString(_Data.RawData);
        }
    }

    std::string ToString()const override{return ToString(StrDecimal);}
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