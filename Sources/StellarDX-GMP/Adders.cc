/**
 * @file Adders.cc
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

#include "StellarMath/StellarDX-GMP/Adders.h"
//#include <simd> // SIMD
#include <stdexcept>
#include <immintrin.h> // AVX-512

_ALU_BEGIN

void INC(BlockArrayView AX, BlockType BX, BlockType* CF)
{
    BlockType CI = BX;
    // 这里不设置提前退出，因为提前退出会增加一步判断从而不利于SIMD
    for (auto& i : AX)
    {
        ExtBlockType A = ExtBlockType(i) + CI;
        i = BlockType(A);
        CI = A >> BSIZE;
    }
    if (CF) {*CF = CI;}
}

_ADDER_BEGIN

void GMP_SerialAdder(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI, BlockType* CF)
{
    const auto& [SX, LX] = AX.size() > BX.size() ? std::tie(BX, AX) : std::tie(AX, BX);
    for (size_t i = 0; i < SX.size(); ++i)
    {
        ExtBlockType S = ExtBlockType(LX.at(i)) + SX.at(i) + CI;
        DST[i] = BlockType(S);
        CI = S >> BSIZE;
    }
    if (LX.size() > SX.size())
    {
        std::copy(LX.begin() + SX.size(), LX.end(), DST.begin() + SX.size());
        INC(DST.subspan(SX.size(), LX.size() - SX.size()), CI, &CI);
    }
    if (CF) {*CF = CI;}
}

void CarryLookaheadParallelAdder(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI, BlockType* CF)
{
    Panic<std::runtime_error>("CarryLookaheadParallelAdder：已废弃特性");
}

void SIMD_ParallelAdder(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI, BlockType* CF)
{
    Panic<std::runtime_error>("SIMD_ParallelAdder：待实现");
}

void AVX512_ParallelAdder(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI, BlockType* CF)
{
    Panic<std::runtime_error>("AVX512_ParallelAdder：待实现");
}

_ADDER_END

void ADD(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI, BlockType* CF, AdderFunc ADDER)
{
    if (ADDER) // 指定自定义加法器
    {
        ADDER(DST, AX, BX, CI, CF);
        return;
    }

    // 未指定自定义加法器时智能选择
    _ADDER GMP_SerialAdder(DST, AX, BX, CI, CF);
}

_ALU_END