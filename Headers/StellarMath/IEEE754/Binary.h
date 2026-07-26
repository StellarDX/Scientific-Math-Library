/**
 * @file Binary.h
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 无限精度二进制浮点，基于GMP和GMPY2进行的C++改造和封装
 * @version 26H2
 * @date 2026-06-20
 * 
 * @copyright Copyright (c) 2026 StellarDX Astronomy
 */

#pragma once

#ifndef __Floating_point_arithmetic__
#define __Floating_point_arithmetic__

#include <StellarMath/Base/Standards.h>
#include <cstdint>
#include <cstddef>

_754_BEGIN

template<uint64_t ExponentSize, uint64_t MantissaSize>
struct StorageTypeGenerator
{
    struct Type
    {

    };
};

_754_END

#endif