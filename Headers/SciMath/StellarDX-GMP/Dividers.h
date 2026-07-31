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

/**
 * @brief 除法器
 */
class Divider
{
protected:
    BlockArraySrcView NumeratorOriginalView;   ///<被除数（原始）
    BlockArraySrcView DenominatorOriginalView; ///<除数（原始）

public:
    /**
     * @brief 构造函数
     * @param AX 被除数
     * @param BX 除数
     */
    Divider(BlockArraySrcView AX, BlockArraySrcView BX);

    /**
     * @brief 除法器主函数
     * @param RAX 商
     * @param RBX 扩展商（小数部分，如果支持）
     * @param RDX 余数
     */
    virtual void Run(BlockArrayView RAX, BlockArrayView RBX, BlockArrayView RDX)const = 0;
};

class NormalizedDividerBase : public Divider
{
public:
    using Mybase = Divider;

protected:
    BlockArray Numerator;    ///<被除数（处理后）
    BlockArray Denominator;  ///<除数（处理后）
    size_t     Shift;        ///<移位数
    BlockType  DenomInverse; ///<除数的倒数

public:
    NormalizedDividerBase(BlockArraySrcView AX, BlockArraySrcView BX);
    virtual void Init() = 0; ///<初始化函数
};

/**
 * @brief 竖式/倒数除法器
 */
class LongReciprocalDivider : NormalizedDividerBase
{
public:
    using Mybase     = NormalizedDividerBase;
    using DblBlkType = std::pair<BlockType, BlockType>;
    using TplBlkType = std::tuple<BlockType, BlockType, BlockType>;

    /// @brief 单块除法器执行除数最高位为1的除法时，被除数块数高于此值切换为倒数除法器
    size_t SingleBlockDividerNormalizedThreshold = ~0;
    /// @brief 单块除法器执行除数最高位不为1的除法时，被除数块数高于此值切换为倒数除法器
    size_t SingleBlockDividerUnnormalizedThreshold = ~0;

public:
    /**
     * @brief 原子操作：把两个块按LE合并为一个大块
     * @param AX 输入块，低位放first，高位放second
     * @return constexpr ExtBlockType 拼好的大块
     */
    inline constexpr static ExtBlockType __Atomic_DblBlkToExtType(DblBlkType AX)
    {
        return AX.first | (ExtBlockType(AX.second) << BSIZE);
    }

    /**
     * @brief 原子操作：把一个大块拆分回两个块
     * @param AX 输入块
     * @return constexpr DblBlkType 拆分的两个块，低位放first，高位放second
     */
    inline constexpr static DblBlkType __Atomic_ExtTypeToDblBlk(ExtBlockType AX)
    {
        return std::pair{BlockType(AX), BlockType(AX >> BSIZE)};
    }

    /**
     * @brief 原子操作：双块除法器
     * @param AX 被除数
     * @param BX 除数
     * @return constexpr DblBlkType {商, 余数}
     */
    inline constexpr static DblBlkType __Atomic_DblDivSng(DblBlkType AX, BlockType BX)
    {
        #if 0 // 这样写固然是最快的，但是我更希望用更直观的写法然后剩下的交给编译器优化
        BlockType q, r;
        __asm__("divl %4": "=a" (q), "=d" (r) : "0" (AX.first), "1" (AX.second), "rm" (BX));
        return std::pair{q, r};
        #elif 0 // 但是这样最终编译器就可能会变出两次除法了
        ExtBlockType AXE = __Atomic_DblBlkToExtType(AX);
        return std::pair{AXE / BX, AXE % BX}; 
        #else // C++17及以上标准的正解，但是最高也只能覆盖到64位了
        ExtBlockType AXE = __Atomic_DblBlkToExtType(AX);
        auto Result = std::lldiv(AXE, BX);
        return std::pair{Result.quot, Result.rem};
        #endif
    }
    
    /**
     * @brief 原子操作：双块倒数除法器
     * @note 此除法器仅在除数最高位为1且被除数最高块的值小于除数时生效
     * @bug 已知在被除数最高块的最高位也是1时大概率得到错误的结果
     * @deprecated 随着现在的U和标准库的优化，百万次计时实测std::div的延迟已经追平这个函数了，后续使用std::div即可
     * @param AX 被除数
     * @param BX 除数
     * @param BR 除数的倒数
     * @return constexpr DblBlkType {商, 余数}
     */
    [[deprecated("随着现在的U和标准库的优化，百万次计时实测std::div的延迟已经追平这个函数了，后续使用std::div即可")]]
    inline constexpr static DblBlkType __Atomic_DblDivSngRecip
        (DblBlkType AX, BlockType BX, BlockType BR)
    {
        ExtBlockType AXE = __Atomic_DblBlkToExtType(AX);
        ExtBlockType Q = ExtBlockType(AX.second) * ExtBlockType(BR);
        Q += AXE + (ExtBlockType(1) << BSIZE);

        auto [ql, qh] = __Atomic_ExtTypeToDblBlk(Q);
        BlockType R = AX.first - qh * BX;

        if (R > ql) // unpredictable
        {
            --qh;
            R += BX;
        }

        if (R >= BX) [[unlikely]]
        {
            return std::pair{++qh, R - BX};
        }

        return std::pair{qh, R};
    }

    /**
     * @brief 单块竖式除法器第二阶段函数，要求除数最高块为1
     * @param NI 被除数起始
     * @param NE 被除数终止
     * @param D 除数
     * @param QI 商起始
     * @param QEI 扩展商起始
     * @param QEE 扩展商终止
     * @param R 余数
     * @param RI 初始余数
     */
    static void __Single_Block_Long_Div_Normalized_Stage2(
        BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
        BlockType D, 
        BlockArrayView::reverse_iterator QI, 
        BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
        BlockType* R, BlockType RI);

    /**
     * @brief 单块倒数除法器第二阶段函数，要求除数最高块为1
     * @param NI 被除数起始
     * @param NE 被除数终止
     * @param D 除数
     * @param QI 商起始
     * @param QEI 扩展商起始
     * @param QEE 扩展商终止
     * @param R 余数
     * @param RI 初始余数
     */
    static void __Single_Block_Recip_Div_Normalized_Stage2(
        BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
        BlockType D,
        BlockArrayView::reverse_iterator QI, 
        BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
        BlockType* R, BlockType RI);

    /**
     * @brief 单块竖式除法器第二阶段函数，不要求除数最高块为1
     * @param NI 被除数起始
     * @param NE 被除数终止
     * @param D 除数
     * @param QI 商起始
     * @param QEI 扩展商起始
     * @param QEE 扩展商终止
     * @param R 余数
     * @param RI 初始余数
     */
    static void __Single_Block_Long_Div_Unnormalized_Stage2(
        BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
        BlockType D, 
        BlockArrayView::reverse_iterator QI, 
        BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
        BlockType* R, BlockType RI);

    /**
     * @brief 单块倒数除法器第二阶段函数，不要求除数最高块为1
     * @param NI 被除数起始
     * @param NE 被除数终止
     * @param D 除数
     * @param QI 商起始
     * @param QEI 扩展商起始
     * @param QEE 扩展商终止
     * @param R 余数
     * @param RI 初始余数
     */
    static void __Single_Block_Recip_Div_Unnormalized_Stage2(
        BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
        BlockType D, 
        BlockArrayView::reverse_iterator QI, 
        BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
        BlockType* R, BlockType RI);

    void Init()override;

public:
    LongReciprocalDivider(BlockArraySrcView AX, BlockArraySrcView BX);

    /**
     * @brief 创建除数的倒数
     * @param AX 除数
     * @return constexpr BlockType 除数的倒数
     */
    inline constexpr static BlockType MakeReciprocal1(BlockType AX)
    {
        return (~(ExtBlockType(AX) << BSIZE)) / AX;
    }

    static void SingleBlockDiv(
        BlockArrayView RAX, BlockArrayView RBX, BlockType* RDX,
        BlockArraySrcView AX, BlockType BX, 
        size_t TN = ~0, size_t TU = ~0);

    void Run(BlockArrayView RAX, BlockArrayView RBX, BlockArrayView RDX)const override;
};

/**
 * @brief 递归分治除法器
 */
class RecursiveDivideAndConquerDivider : LongReciprocalDivider
{
    // TODO
};

/**
 * @brief 牛顿迭代除法器
 */
class NewtonIterationDivider : NormalizedDividerBase
{
    // TODO
};

_DIVIDER_END

_ALU_END

#endif