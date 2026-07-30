/**
 * @file Adders.h
 * @defgroup Adders 加法器
 * @ingroup IPN
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 一些无限精度加法器
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

#ifndef __STEL_GMP_ADDERS__
#define __STEL_GMP_ADDERS__

#include <SciMath/StellarDX-GMP/SMLDefs.h>

#define _ADDER_BEGIN namespace Adders {
#define _ADDER_END }
#define _ADDER Adders::

_ALU_BEGIN

/**
 * @brief 增量器
 * @ingroup Adders
 *
 * 将 BlockArrayView 视为一个大整数，并加上一个小的增量值 BX。
 *
 * @param AX   [in/out] 目标数组视图，将被修改为增加后的值
 * @param BX   [in]     增量值，默认为 1
 * @param CF   [out]    可选的最终进位输出指针，若不为 nullptr 则写入进位状态
 */
void INC(BlockArrayView AX, BlockType BX = 1, BlockType* CF = nullptr);

_ADDER_BEGIN

/**
 * @brief GMP 串行加法器 (Serial Adder)
 * @ingroup Adders
 *
 * 执行 DST = AX + BX + Cin，并将最终进位写入 CF。
 * 当输入输出 span 长度不一致时，以最短者为准进行安全计算，剩余部分继续处理进位。
 *
 * @param DST  [out] 目标缓冲区，存储加法结果
 * @param AX   [in]  第一个加数
 * @param BX   [in]  第二个加数
 * @param CI   [in]  初始进位输入（0 或 1），默认为 0
 * @param CF   [out] 最终进位输出指针，若无需获取进位可传 nullptr
 */
void GMP_SerialAdder(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI = 0, BlockType* CF = nullptr);

/**
 * @brief 超前进位并行加法器 (Carry Lookahead Parallel Adder)
 * @ingroup Adders
 * 
 * @details 使用超前进位原理加速大数加法运算。
 *          理论上相比串行加法器的 O(n) 时间复杂度，超前进位并行加法器可将关键路径缩短至 O(log(n))。
 *          
 * @deprecated 后续研究发现，CLA 的软件实现极其复杂，且由于分支预测和缓存命中率问题，
 *             在通用 CPU 上的实际性能往往不如基于 SIMD（AVX2/AVX-512）的并行加法器。
 *             建议使用 SIMD_ParallelAdder 或 AVX512_ParallelAdder 替代。
 * @see SIMD_ParallelAdder
 * @see AVX512_ParallelAdder
 *
 * @param DST  [out] 目标缓冲区，存储加法结果
 * @param AX   [in]  第一个加数
 * @param BX   [in]  第二个加数
 * @param CI   [in]  初始进位输入（0 或 1），默认为 0
 * @param CF   [out] 最终进位输出指针，若无需获取进位可传 nullptr
 */
[[deprecated("此特性已废弃，请使用SIMD_ParallelAdder或AVX512_ParallelAdder")]]
void CarryLookaheadParallelAdder(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI = 0, BlockType* CF = nullptr);

/**
 * @brief 基于 AVX2 的 SIMD 并行加法器
 * @ingroup Adders
 * 
 * @details 利用 AVX2 指令集进行单指令多数据流并行加法计算，显著提升大数加法吞吐量。
 * 
 * @param[out]    DST  目标缓冲区，存储加法结果
 * @param[in]     AX   第一个加数
 * @param[in]     BX   第二个加数
 * @param[in]     CI   初始进位输入（0 或 1），默认为 0
 * @param[out]    CF   最终进位输出指针。若无需获取进位可传 nullptr
 *
 * @todo 待实现
 */
void SIMD_ParallelAdder(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI = 0, BlockType* CF = nullptr);

/**
 * @brief AVX-512 超高速并行加法器
 * @ingroup Adders
 * 
 * @details 利用 AVX-512 指令集进行更高宽度的并行加法计算。
 *
 * @note AVX512为英特尔特有指令集，因此本函数在其他平台实际可用情况及调用的指令集（预计）如下：
 *  - AMD：支持AVX512
 *  - 鲲鹏：SVE或SVE2
 *  - 海光：不可用
 *  - 龙芯：LASX
 * 
 * @param[out]    DST  目标缓冲区，存储加法结果
 * @param[in]     AX   第一个加数
 * @param[in]     BX   第二个加数
 * @param[in]     CI   初始进位输入（0 或 1），默认为 0
 * @param[out]    CF   最终进位输出指针。若无需获取进位可传 nullptr
 *
 * @todo 待实现
 */
void AVX512_ParallelAdder(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI = 0, BlockType* CF = nullptr);

_ADDER_END

/**
 * @brief 加法器函数指针类型定义
 * @ingroup Adders
 */
using AdderFuncType = void(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI, BlockType* CF);

/**
 * @brief 加法器函数指针
 * @ingroup Adders
 */
using AdderFunc = AdderFuncType*;

/**
 * @brief 加法器
 * @ingroup Adders
 *
 * 执行加法运算，如果 ADDER 为 nullptr，则根据传入值智能选择加法器（通常为 GMP_SerialAdder）。
 *
 * @param DST    [out] 目标缓冲区，存储加法结果
 * @param AX     [in]  第一个加数
 * @param BX     [in]  第二个加数
 * @param CI     [in]  初始进位输入，默认为 0
 * @param CF     [out] 最终进位输出指针，默认为 nullptr
 * @param ADDER  [in]  指定的加法器函数指针，默认为 nullptr
 */
void ADD(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX, BlockType CI = 0, BlockType* CF = nullptr, AdderFunc ADDER = nullptr);

_ALU_END

#endif