/**
 * @file Multipliers.cc
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 
 * @version 26H2
 * @date 2026-06-20
 * 
 * @copyright Copyright (c) 2026 StellarDX Astronomy
 * Scientific Math Library is free software; you can redistribute it and/or 
 * modify it under the terms of either:
 * 
 *   * the GNU Lesser General Public License as published by the Free
 *     Software Foundation; either version 3 of the License, or (at your
 *     option) any later version.
 * 
 * or
 * 
 *   * the GNU General Public License as published by the Free Software
 *     Foundation; either version 2 of the License, or (at your option) any
 *     later version.
 * 
 * or both in parallel, as here.
 * 
 * Scientific Math Library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received copies of the GNU General Public License and the
 * GNU Lesser General Public License along with the Scientific Math Library.
 * If not, see https://www.gnu.org/licenses/.
 */

#include <ranges>
#include "SciMath/StellarDX-GMP/整数乘法器的长征.h"

_ALU_BEGIN

void MADD(BlockArrayView DX, BlockArraySrcView AX, BlockType BX, BlockType* CF)
{
    BlockType C = 0;
    for (size_t i = 0; i < AX.size(); ++i)
    {
        ExtBlockType P64 = ExtBlockType(AX.at(i)) * BX + C;
        C = P64 >> BSIZE;

        ExtBlockType S64 = ExtBlockType(DX.at(i)) + (P64 & BMASK);
        C += S64 >> BSIZE;

        DX.at(i) = BlockType(S64);
    }
    if (CF) {*CF = C;}
}

void MSUB(BlockArrayView DX, BlockArraySrcView AX, BlockType BX, BlockType* CF)
{
    BlockType C = 0;
    for (size_t i = 0; i < AX.size(); ++i)
    {
        ExtBlockType P64 = ExtBlockType(AX.at(i)) * BX + C;
        C = P64 >> BSIZE;

        ExtBlockType S64 = ExtBlockType(DX.at(i)) - (P64 & BMASK);
        C -= S64 >> BSIZE; // 借位溢出到高位会得到一个-1，因此需要改成减

        DX.at(i) = BlockType(S64);
    }
    if (CF) {*CF = C;}
}

_MULTIPLIER_BEGIN

void LongMultiplier::Run(BlockArrayView DST, BlockArraySrcView AX, BlockType BX)const
{
    BlockType C = 0;
    for (size_t i = 0; i < AX.size(); ++i)
    {
        ExtBlockType P64 = ExtBlockType(AX.at(i)) * BX + C;
        C = P64 >> BSIZE;
        DST.at(i) = BlockType(P64);
    }
    DST.at(AX.size()) = C;
}

void LongMultiplier::Run(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX)const
{
    auto DSLIDE = DST | std::ranges::views::slide(AX.size() + 1);
    auto DI = DSLIDE.begin();
    Run(*DI, AX, BX.front());
    for (size_t i = 1; i < BX.size(); ++i)
    {
        ++DI;
        MADD(*DI, AX, BX.at(i), &((*DI).back()));
    }
}

_MULTIPLIER_END

void MUL1(BlockArrayView DST, BlockArraySrcView AX, BlockType BX, const SingleBlockMultiplier* MULER)
{
    if (MULER)
    {
        MULER->Run(DST, AX, BX);
        return;
    }

    _MULTIPLIER LongMultiplier Mul;
    Mul.Run(DST, AX, BX);
}

void MUL(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, const Multiplier* MULER)
{
    if (MULER)
    {
        MULER->Run(DST, AX, BX);
        return;
    }

    _MULTIPLIER LongMultiplier Mul;
    Mul.Run(DST, AX, BX);
}

_ALU_END