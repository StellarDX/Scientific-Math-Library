/**
 * @file Logics.cc
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 逻辑运算
 * @version 26H2
 * @date 2026-06-20
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

#include "SciMath/StellarDX-GMP/Logics.h"
#include "SciMath/StellarDX-GMP/Memory.h"
#include "SciMath/StellarDX-GMP/SMLDefs.h"
#include <algorithm>
#include <cstddef>
#include <iterator>

_80000_BEGIN
_ALU_BEGIN

void AND(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX)
{
    const auto& [SX, LX] = AX.size() > BX.size() ? std::tie(BX, AX) : std::tie(AX, BX);
    for (size_t i = 0; i < SX.size(); ++i)
    {
        DST.at(i) = SX.at(i) & LX.at(i);
    }
}

void OR(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX)
{
    const auto& [SX, LX] = AX.size() > BX.size() ? std::tie(BX, AX) : std::tie(AX, BX);
    for (size_t i = 0; i < SX.size(); ++i)
    {
        DST.at(i) = SX.at(i) | LX.at(i);
    }
    std::copy(LX.begin() + SX.size(), LX.end(), DST.begin() + SX.size());
}

void XOR(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX)
{
    const auto& [SX, LX] = AX.size() > BX.size() ? std::tie(BX, AX) : std::tie(AX, BX);
    for (size_t i = 0; i < SX.size(); ++i)
    {
        DST.at(i) = SX.at(i) ^ LX.at(i);
    }
    std::copy(LX.begin() + SX.size(), LX.end(), DST.begin() + SX.size());
}

void NOT(BlockArrayView DST, BlockArraySrcView AX)
{
    for (size_t i = 0; i < AX.size(); ++i)
    {
        DST.at(i) = ~AX.at(i);
    }
}

void SHL(BlockArrayView DST, BlockArraySrcView AX, std::size_t BX, BlockType* CF)
{
    if (BX > BSIZE)
    {
        Panic("SHL：移位块数不能超过块大小，整块移位请使用MOV(+STD)");
    }

    if (BX == 0)
    {
        MOV(DST, AX);
        return;
    }

    std::size_t TNC = BSIZE - BX;
    BlockType LB = AX.back();
    BlockType C = LB >> TNC;
    BlockType HB = LB << BX;

    auto DI = std::reverse_iterator{DST.begin() + AX.size()};
    for (auto i = AX.rbegin() + 1; i != AX.rend(); ++i)
    {
        LB = *i;
        *DI = HB | (LB >> TNC);
        ++DI;
        HB = LB << BX;
    }
    DST.front() = HB;
    if (CF) {*CF = C;}
}

void SHR(BlockArrayView DST, BlockArraySrcView AX, std::size_t BX, BlockType* CF)
{
    if (BX > BSIZE)
    {
        Panic("SHR：移位块数不能超过块大小，整块移位请使用MOV");
    }

    if (BX == 0)
    {
        MOV(DST, AX);
        return;
    }

    std::size_t TNC = BSIZE - BX;
    BlockType HB = AX.front();
    BlockType C = HB << TNC;
    BlockType LB = HB >> BX;

    auto DI = DST.begin();
    for (auto i = AX.begin() + 1; i != AX.end(); ++i)
    {
        HB = *i;
        *DI = LB | (HB << TNC);
        ++DI;
        LB = HB >> BX;
    }
    *(DST.begin() + AX.size() - 1) = LB;
    if (CF) {*CF = C;}
}

_ALU_END
_80000_END