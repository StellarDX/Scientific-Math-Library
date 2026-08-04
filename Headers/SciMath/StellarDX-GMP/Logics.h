/**
 * @file Logics.h
 * @defgroup Logics 逻辑运算
 * @ingroup IPN
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

#pragma once

#ifndef __LOGIC__
#define __LOGIC__

#include <SciMath/StellarDX-GMP/SMLDefs.h>

_80000_BEGIN
_ALU_BEGIN

/**
 * @brief 与运算，结果长度取决于短的那个操作数（0 & 任何数 = 0）
 * @ingroup Logics
 * @param DST 结果
 * @param AX 操作数1
 * @param BX 操作数2
 */
void AND(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX);

/**
 * @brief 或运算，结果长度取决于长的那个操作数（0 | 任何数 = 任何数）
 * @ingroup Logics
 * @param DST 结果
 * @param AX 操作数1
 * @param BX 操作数2
 */
void OR(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX);

/**
 * @brief 异或运算，结果长度取决于长的那个操作数（0 ^ 任何数 = 任何数）
 * @ingroup Logics
 * @param DST 结果
 * @param AX 操作数1
 * @param BX 操作数2
 */
void XOR(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX);

/**
 * @brief 非运算
 * @ingroup Logics
 * @param DST 结果
 * @param AX 操作数1
 */
void NOT(BlockArrayView DST, BlockArraySrcView AX);

/**
 * @brief 左移位
 * @ingroup Logics
 * @param DST 结果
 * @param AX 操作数1
 * @param BX 移位数，不能超过块大小，整块移位请使用MOV（+STD）
 */
void SHL(BlockArrayView DST, BlockArraySrcView AX, std::size_t BX, BlockType* CF = nullptr);

/**
 * @brief 右移位
 * @ingroup Logics
 * @param DST 结果
 * @param AX 操作数1
 * @param BX 移位数，不能超过块大小，整块移位请使用MOV
 */
void SHR(BlockArrayView DST, BlockArraySrcView AX, std::size_t BX, BlockType* CF = nullptr);

_ALU_END
_80000_END

#endif