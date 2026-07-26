/**
 * @file IO.cc
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 字节数组输入输出
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

#include "StellarMath/StellarDX-GMP/IO.h"
#include <algorithm>

_ALU_BEGIN

void MOV(BlockArrayView DST, BlockArraySrcView SRC, bool STD)
{
    if (STD)
    {
        std::copy_backward(SRC.begin(), SRC.end(), DST.begin());
    }
    else {std::copy(SRC.begin(), SRC.end(), DST.begin());}
}

std::strong_ordering CMP(BlockArraySrcView AX, BlockArraySrcView BX)
{
    auto ASize = BSR(AX), BSize = BSR(BX);
    if (ASize != BSize) {return ASize <=> BSize;}
    for (auto i = AX.begin() + ASize, j = BX.begin() + BSize; i != AX.begin() || j != BX.begin(); --i, --j)
    {
        if (*i != *j) {return *i <=> *j;}
    }
    return AX[0] <=> BX[0];
}

size_t BSR(BlockArraySrcView AX)
{
    auto it = std::find_if(AX.rbegin(), AX.rend(), [](BlockType x){return x != 0;});
    return AX.size() - std::distance(AX.rbegin(), it);
}

bool TEST(BlockArraySrcView AX)
{
    return std::all_of(AX.begin(), AX.end(), [](BlockType x){return x == 0;});
}

void STOS(BlockArrayView AX)
{
    std::fill(AX.begin(), AX.end(), 0);
}

_ALU_END