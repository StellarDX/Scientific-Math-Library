/**
 * @file Subtractors.h
 * @defgroup Subtractors 减法器
 * @ingroup IPN
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 一些无限精度减法器
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

#ifndef __STEL_GMP_SUBTRACTORS__
#define __STEL_GMP_SUBTRACTORS__

#include <SciMath/StellarDX-GMP/SMLDefs.h>

#define _SUBTRACTOR_BEGIN namespace Subtractors {
#define _SUBTRACTOR_END }
#define _SUBTRACTOR Subtractors::

_ALU_BEGIN

/**
 * @brief 减量器
 * @ingroup Subtractors
 *
 * @details 从由数组AX表示的大整数中减去标量值BX。
 *          此操作直接修改AX的内容。
 * 
 * @param[in,out] AX   目标操作数数组视图。输入为被减数，输出为减法结果。
 * @param[in]     BX   要减去的标量值。默认为1，即执行自减操作。
 * @param[out]    CF   借位标志指针。
 *                     - 若非 nullptr，则存储最终的借位状态（Borrow Out）（可能为0或-1）。
 *                     - 若为 nullptr，则忽略借位输出。
 */
void DEC(BlockArrayView AX, BlockType BX = 1, SBlockType* CF = nullptr);

/**
 * @brief 减法器
 * @ingroup Subtractors
 */
__interface Subtractor
{
    /**
     * @brief 减法器主函数
     * @param[out]    DST  目标结果数组视图。必须具有足够的空间容纳计算结果。
     * @param[in]     AX   被减数数组源视图。
     * @param[in]     BX   减数数组源视图。
     * @param[in]     CI   输入借位 (Carry In / Borrow In)。
     * @param[out]    CF   输出借位指针 (Carry Out / Borrow Out)。
     *                     - 若非 nullptr，则存储最终借位状态。（可能为0或-1）
     *                     - 若为 nullptr，则忽略。
     */
    virtual void Run(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, SBlockType CI = 0, SBlockType* CF = nullptr)const = 0;
};

_SUBTRACTOR_BEGIN

/**
 * @brief 改进的GMP串行减法器
 * @ingroup Subtractors
 * 
 * @details 执行减法运算：DST = AX - BX + CI。
 *          - 始终以较长操作数的长度为准进行计算。
 *          - 负数在内部以补码形式处理。
 */
class SerialSubtractor : public Subtractor
{
public:
    void Run(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, SBlockType CI = 0, SBlockType* CF = nullptr)const override;
};

_SUBTRACTOR_END

/**
 * @brief 减法器
 * @ingroup Subtractors
 *
 * @details 根据提供的SUBER函数指针执行减法操作：DST = AX - BX + CI。
 *          如果SUBER为nullptr，则根据输入值只能选择最优减法器实现。
 * 
 * @param[out]    DST   目标结果数组视图。
 * @param[in]     AX    被减数数组源视图。
 * @param[in]     BX    减数数组源视图。
 * @param[in]     CI    输入借位，默认为0。
 * @param[out]    CF    输出借位指针（值可能为0或-1），默认为nullptr（忽略借位）。
 * @param[in]     SUBER 自定义减法器函数指针。
 *                      - 默认为 nullptr，表示使用库默认实现。
 */
void SUB(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, SBlockType CI = 0, SBlockType* CF = nullptr, const Subtractor* SUBER = nullptr);

_ALU_END

#endif