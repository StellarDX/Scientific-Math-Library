/**
 * @file IO.h
 * @defgroup IO 字节数组输入输出
 * @ingroup IPN
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief ByteArray和一些基本指令，是无限精度整数/无限精度浮点/无限精度十进制浮点的基础
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

#pragma once

#ifndef __STEL_GMP_IO__
#define __STEL_GMP_IO__

#include <compare>
#include <StellarMath/StellarDX-GMP/SMLDefs.h>

_ALU_BEGIN

/**
 * @brief 将源块数组数据复制到目标块数组
 * @ingroup IO
 *
 * @param DST 目标块数组视图
 * @param SRC 源块数组只读视图
 * @param STD 是否使用 std::copy_backward 进行反向复制。当 src 与 dst 存在重叠且 dst 起始地址 > src 起始地址时应设为 true
 *
 * @note 调用方需保证 dst.size() >= src.size()，否则行为未定义
 */
void MOV(BlockArrayView DST, BlockArraySrcView SRC, bool STD = 0);

/**
 * @brief 按大端语义比较两个块数组的数值大小
 * @ingroup IO
 *
 * 先比较有效长度（BSR），长度不同则较长者更大；
 * 长度相同时从最高有效块向最低有效块逐块比较。
 *
 * @param AX 左操作数块数组只读视图
 * @param BX 右操作数块数组只读视图
 * @return std::strong_ordering 三路比较结果
 */
std::strong_ordering CMP(BlockArraySrcView AX, BlockArraySrcView BX);

/**
 * @brief 获取块数组有效长度（Bit Scan Reverse 语义）
 * @ingroup IO
 *
 * @param AX 块数组只读视图
 * @return size_t 有效长度；若所有块均为零则返回 0
 */
size_t BSR(BlockArraySrcView AX);

/**
 * @brief 检测块数组是否全部为零
 * @ingroup IO
 *
 * @param ax 块数组只读视图
 * @return true  所有块均为零
 * @return false 至少有一个块非零
 */
bool TEST(BlockArraySrcView AX);

/**
 * @brief 将块数组所有元素置零
 * @ingroup IO
 *
 * @param ax 目标块数组视图
 */
void STOS(BlockArrayView AX);

_ALU_END

#endif