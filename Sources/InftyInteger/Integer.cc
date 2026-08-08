/**
 * @file Integer.h
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 无限长度整数类模板
 * @version 26H2
 * @date 2026-08-04
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

#include "SciMath/InftyInteger/Integer.h"

_80000_BEGIN

std::shared_ptr<ALU::Adder>      DefaultAdder      = nullptr;
std::shared_ptr<ALU::Subtractor> DefaultSubtractor = nullptr;
std::shared_ptr<ALU::Multiplier> DefaultMultiplier = nullptr;
std::shared_ptr<ALU::Divider>    DefaultDivider    = nullptr;

std::shared_ptr<ALU::SingleBlockMultiplier> DefaultSingleBlockMultiplier = nullptr;

_80000_END