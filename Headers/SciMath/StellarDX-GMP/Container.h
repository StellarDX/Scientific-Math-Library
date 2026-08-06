/**
 * @file Container.h
 * @defgroup Container 容器化存储
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
#include <meta>

#define _CONTAINER_BEGIN namespace Containers {
#define _CONTAINER_END }
#define _CONTAINER Containers::

_80000_BEGIN
_CONTAINER_BEGIN

/**
 * @brief 地址描述符
 * @details 用于精确定位数据块中的具体位置，支持块索引和块内偏移。
 * @ingroup Container
 */
typedef struct Address
{
    std::size_t BlockSize; ///< 块索引/整块大小
    std::size_t Offset;    ///< 块内偏移/不足整块的部分
}PartitionSizeType;

/**
 * @brief 分区信息表
 * @details 描述一个分区在数据块中的起止位置及类型信息。
 * @ingroup Container
 */
struct PartitionInfo
{
    Address     Begin;  ///< 起始位置
    Address     End;    ///< 结束位置
    std::string Type;   ///< 分区描述信息，几乎没什么卵用
};

/**
 * @brief 分区表
 * @details 存储所有分区信息的向量容器。
 * @ingroup Container
 */
using PartitionTable = std::vector<PartitionInfo>;

/**
 * @brief 分区表常量视图
 * @details 用于只读访问分区信息，避免拷贝开销。
 * @ingroup Container
 */
using PartitionInfoConstView = std::span<const PartitionInfo>;

/**
 * @brief 元数据
 * @details 描述容器的整体属性，包括类型、格式、动态扩缩容能力及分区布局。
 * @ingroup Container
 */
struct MetadataType
{
    std::string    TypeName;       ///< 类型名
    std::string    Format;         ///< 存储格式名
    bool           IsDynamicSized; ///< 是否可扩缩容
    bool           IsSigned;       ///< 是否为有符号类型
    PartitionTable Partitions;     ///< 分区表
};

/**
 * @brief 存放数字的统一容器，用于将块序列表示为实际的数字
 * @ingroup Container
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
     * @details 提供对底层原始数据块的只读访问。
     * @return BlockArrayConstView 原始数据块（常量）
     */
    virtual BlockArrayConstView GetRawData()const = 0;

    /**
     * @brief 获取原始数据块（常量）
     * @details 与GetRawData()const功能相同，提供更明确的语义。
     * @return BlockArrayConstView 原始数据块（常量）
     */
    virtual BlockArrayConstView GetConstRawData()const = 0;

    /**
     * @brief 获取元数据
     * @details 返回描述当前容器结构的元数据副本。
     * @return Metadata 元数据
     */
    virtual MetadataType GetMetaData()const = 0;

    /**
     * @brief 获取总块数（因为BlockArray强制32位对齐）
     * @details 返回容器占用的总块数量。
     * @return size_t 总块数
     */
    virtual size_t size()const = 0;

    /**
     * @brief 获取总字节数（依据分区表内容）
     * @details 依据分区表内容计算的有效数据字节总数。
     * @return size_t 字节数
     */
    virtual size_t size_byte()const = 0;

    /**
     * @brief 获取位数（依据分区表内容）
     * @details 依据分区表内容计算的有效数据位总数。
     * @return size_t 位数
     */
    virtual size_t size_bit()const = 0;

    /**
     * @brief 扩/缩容（需要看具体类型只不支持）
     * @details 调整指定分区的大小。具体行为取决于实现类是否支持动态扩缩容。
     * @param[in] Partition 分区编号
     * @param[in] Size 目标大小
     * @note 如果容器不支持动态大小调整，调用此方法可能会抛出异常或未定义行为。
     */
    virtual void Adjust(std::size_t Partition, PartitionSizeType Size) = 0;

    /**
     * @brief 转换为字符串
     * @details 将容器内的数字数据转换为字符串。
     * @return std::string 字符串
     */
    virtual std::string ToString()const = 0;
};

/**
 * @brief 计算类型所需的块数量
 * @ingroup Container
 * @details 在编译期计算给定类型Tp占用多少个块（Block）。
 * @tparam Tp 数据类型
 * @return constexpr std::size_t所需块数
 */
template<typename Tp>
inline consteval size_t BlocksAsneeded()
{
    return sizeof(Tp) / BBYTE + (sizeof(Tp) % BBYTE ? 1 : 0);
}

_CONTAINER_END
_80000_END

#endif