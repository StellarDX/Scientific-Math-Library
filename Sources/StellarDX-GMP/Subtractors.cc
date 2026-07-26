/**
 * @file Subtractors.cc
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

#include "StellarMath/StellarDX-GMP/Subtractors.h"

_ALU_BEGIN

void DEC(BlockArrayView AX, BlockType BX, SBlockType* CF)
{
    SBlockType CI = -BX; // 这里要变号
    for (auto& i : AX)
    {
        ExtBlockType A = ExtBlockType(i) + CI;
        i = BlockType(A);
        CI = A >> BSIZE;
    }
    if (CF) {*CF = CI;}
}

_SUBTRACTOR_BEGIN

void GMP_SerialSubtractor(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, SBlockType CI, SBlockType* CF)
{
    auto AN = AX.size(), BN = BX.size();
    if (AN < BN) // 设置分类讨论以适应全部的情况，始终以长的那个为准，负数表示为补码
    {
        for (size_t i = 0; i < AN; ++i)
        {
            ExtBlockType S = ExtBlockType(AX.at(i)) - BX.at(i) + CI; // 因为CI为负数，此处要变号
            DST[i] = BlockType(S);
            CI = S >> BSIZE;
        }
        for (size_t i = AN; i < BN; ++i)
        {
            ExtBlockType S = ExtBlockType(0) - BX.at(i) + CI;
            DST[i] = BlockType(S);
            CI = S >> BSIZE;
        }
    }
    else
    {
        for (size_t i = 0; i < BN; ++i)
        {
            ExtBlockType S = ExtBlockType(AX.at(i)) - BX.at(i) + CI;
            DST[i] = BlockType(S);
            CI = S >> BSIZE;
        }
        if (AN > BN)
        {
            std::copy(AX.begin() + BN, AX.end(), DST.begin() + BN);
            DEC(DST.subspan(BN, AN - BN), -CI, &CI);
        }
    }

    if (CF) {*CF = CI;}
}

_SUBTRACTOR_END

void SUB(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, SBlockType CI, SBlockType* CF, SubtractorFunc SUBER)
{
    if (SUBER) // 指定自定义减法器
    {
        SUBER(DST, AX, BX, CI, CF);
        return;
    }

    // 未指定自定义减法器时智能选择
    _SUBTRACTOR GMP_SerialSubtractor(DST, AX, BX, CI, CF);
}

_ALU_END