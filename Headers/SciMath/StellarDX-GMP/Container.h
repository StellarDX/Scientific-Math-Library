/**
 * @file Container.h
 * @defgroup Container 数字容器
 * @ingroup IPN
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 数字容器
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

#ifndef __CONTAINER__
#define __CONTAINER__

#include <SciMath/StellarDX-GMP/SMLDefs.h>

_80000_BEGIN

/**
 * @brief 分区元数据
 */
struct PartitionMetaData
{
    std::size_t StartIndex;  // 起始块位置
    std::size_t StartOffset; // 起始块块内偏移数
    std::size_t EndIndex;    // 结束块位置
    std::size_t EndOffset;   // 结束块块内偏移数
    std::string Type;        // 分区描述信息，几乎没什么卵用
};

/**
 * @brief 分区信息表（常量视图）
 */
using PartitionInfoConstView = std::span<const PartitionMetaData>;

/**
 * @brief 存放数字的统一容器，用于将块序列表示为实际的数字
 * @ingroup IPN
 */
__interface NumericContainer
{
    /**
     * @brief 获取原始数据块
     * @details 此处原本想着用mdspan的，但mdspan的寻址方式是按块寻址而有些类型会精确到位
     * @return BlockArrayView 原始数据块
     */
    virtual BlockArrayView GetRawData() = 0;

    /**
     * @brief 获取原始数据块（常量）
     * @return BlockArrayConstView 原始数据块（常量）
     */
    virtual BlockArrayConstView GetRawData()const = 0;

    /**
     * @brief 获取原始数据块（常量）
     * @return BlockArrayConstView 原始数据块（常量）
     */
    virtual BlockArrayConstView GetConstRawData()const = 0;

    /**
     * @brief 获取分区表
     * @return PartitionInfoType 分区表（常量视图）
     */
    virtual PartitionInfoConstView GetPartitionInfo()const = 0;

    /**
     * @brief 获取总块数（因为BlockArray强制32位对齐）
     * @return size_t 总块数
     */
    virtual size_t size()const = 0;

    /**
     * @brief 获取总字节数（依据分区表内容）
     * @return size_t 字节数
     */
    virtual size_t size_byte()const = 0;

    /**
     * @brief 获取位数（依据分区表内容）
     * @return size_t 位数
     */
    virtual size_t size_bit()const = 0;

    /**
     * @brief 转换为字符串
     * @return std::string 字符串
     */
    virtual std::string ToString()const = 0;
};

_80000_END

#endif