/**
    @file Divider.h
    @defgroup Dividers 除法器
    @ingroup IPN

    @details
    丹霞：除法运算一直以来几乎都是以竖式除法为主，虽然现在也有分治除法器和牛顿迭代除法器，GMP
    内置的也就只有这几种除法器了。但是这些优化方案实际上优化效果不是特别明显，因为除法本质上是
    一种“下一状态需要上一状态推导”的过程，无法真正实现完全的并行化，也就意味着它很难吃满分治带
    来的优势，甚至就连硬件除法器也难以通过堆料的方式获得更好的提升效果（除非增加U的面积）。就
    现在的那些U而言，执行一次除法器需要十几二十个时钟周期，相比乘法器只需要不到5个时钟周期就能
    执行一次，妥妥的“性能黑洞”。因此，在设计大数除法算法时，一般会尽量避免使用硬件除法器。
 */

#pragma once

#ifndef __Dividers__
#define __Dividers__

#include <SciMath/StellarDX-GMP/SMLDefs.h>

#define _DIVIDER_BEGIN namespace Dividers {
#define _DIVIDER_END }
#define _DIVIDER Dividers::

_ALU_BEGIN

_DIVIDER_BEGIN

__interface Divider
{
    /**
     * @brief 
     * 
     * @param RAX 商
     * @param RDX 余数
     * @param AX  被除数
     * @param BX  除数
     */
    virtual void Run(BlockArrayView RAX, BlockArrayView RDX, BlockArraySrcView AX, BlockArraySrcView BX)const = 0;

    /**
     * @brief 
     * 
     * @param RAX 商
     * @param AX  被除数
     * @param BX  除数
     */
    virtual void RunWithoutRem(BlockArrayView RAX, BlockArraySrcView AX, BlockArraySrcView BX)const = 0;
};

class GMP_Divider : public Divider
{

};

_DIVIDER_END

_ALU_END

#endif