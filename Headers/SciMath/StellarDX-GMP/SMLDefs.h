/**
 * @file GMPDefs.h
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @defgroup IPN
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

#pragma once

#ifndef __GMP_DEFS__
#define __GMP_DEFS__

#include <cstdint>
#include <vector>
#include <span>
#include <stdexcept>
#include <concepts>

#if defined(__GNUG__) && !defined(__declspec)
#define __declspec(attrib) __attribute__((attrib))
#endif

#if !defined(_MSC_VER) && !defined(__interface)
#define __interface struct
#endif

#define _ALU_BEGIN namespace ALU {
#define _ALU_END }
#define _ALU ALU::

#define _754_BEGIN namespace IEEE754 {
#define _754_END }
#define _754 IEEE754::

#define _80000_BEGIN namespace ISOMath {
#define _80000_END }
#define _80000 ISOMath::

#if (!defined _EXTERN_C) && (!defined _END_EXTERN_C)
#define _EXTERN_C extern "C" {
#define _END_EXTERN_C }
#endif

using BlockType = uint32_t;
using SBlockType = int32_t;
using ExtBlockType = uint64_t;
inline static const auto BSIZE = sizeof(BlockType) * 8;
inline static const auto BMASK = ~BlockType(0);
inline static const auto BHBIT = BMASK ^ (BMASK >> 1);

using BlockArray = std::vector<BlockType>;
using BlockArrayView = std::span<BlockType>;
using BlockArrayConstView = std::span<const BlockType>;
using BlockArraySrcView = BlockArrayConstView;

template<typename ExceptType = std::invalid_argument> 
requires std::is_base_of_v<std::exception, ExceptType>
inline constexpr void Panic(char const* argv)
{
    throw ExceptType(argv);
}

#endif