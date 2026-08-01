/**
    @file Divider.h
    @defgroup Dividers 除法器
    @ingroup IPN

    @details
    丹霞：除法运算一直以来几乎都是以竖式除法为主，虽然现在也有分治除法器和牛顿迭代除法器，GMP
    内置的也就只有这几种除法器了。但是这些优化方案实际上优化效果不是特别明显，因为除法本质上是
    一种“下一状态需要上一状态推导”的过程，无法真正实现完全的并行化，也就意味着它很难吃满分治带
    来的优势，甚至就连硬件除法器也难以通过堆料的方式获得更好的提升效果（除非增加U的面积）。
    例如GMP在它刚诞生的时期，爆出了如下的大料：
    >                    64-bit divisor       32-bit divisor\
    >                      cycles/limb          cycles/limb\
    >                       (approx)             (approx)\
    >                  integer  fraction    integer  fraction\
    >  Ultrasparc 2i:    160      160          122      96\
    >
    >  32-bit divisors are treated in special case code.  This requires 4 mulx
    >  per limb instead of 8 in the general case.
    >  For big endian systems we need HALF_ENDIAN_ADJ included in the src[i]
    >  addressing, to get the two halves of each limb read in the correct order.
    >  This is kept in an adj variable.  Doing that measures about 4 c/l faster
    >  than just writing HALF_ENDIAN_ADJ(i) in the integer loop.  The latter
    >  shouldn't be 6 cycles worth of work, but perhaps it doesn't schedule well
    >  (on gcc 3.2.1 at least).  The fraction loop doesn't seem affected, but we
    >  still use a variable since that ought to work out best.  
    文中的Ultrasparc 2i就是是当年Sun的那只“黑鸟”，定位是一款中低端服务器U。这个U在它主导的
    年代混的堪称风生水起——不像高端型号那样追求极限性能，在性能、成本和集成度之间找到了绝佳平衡，
    然后又在互联网泡沫的巅峰期乘上了东风——衍生产品Ultra 5和Ultra 10成了当时的“性价比之王”。
    然而阳极阴生，进入新世纪后IBM发布的Power系列以及英特尔发布的奔2和奔3开始从高端和低端两个
    方向夹击黑鸟的市场空间，又加上臭名昭著的“Sabre Bug”，搞得它包括整个系列从此一蹶不振，终
    成落日残烟。在GMP的爆料中也提到，“黑鸟”的除法器执行一次需要160个时钟周期，堪称“性能黑洞”。
    究其原因是当时的除法器还没实现完全的流水线化，计算方式也还是传统的竖式除法器，需要逐位（或
    每周期2-4位）迭代计算商，然后对齐，规格化，写回，一整套流程执行下来需要的时间非常长。如果
    后续有依赖本次除法结果的计算，这些计算也会被迫延迟执行，搞得整组的进度都被拖慢。而且不仅仅
    这一个U这样，当时几乎所有RISC的U都一个鸟样。Google的首席工程师Eric Dumazet在2006年在
    一封邮件里头提到，某款sparcv9的U执行一次除法需要消耗64个时钟周期。紧接着Linux开发者戴维·
    米勒在回信中爆出了一个更大的瓜：
    >For UltraSPARC I and II (which is what this 200mhz guy probably is),
    >it's 4 cycle latency for a multiply (32-bit or 64-bit) and 68 cycles
    >for a 64-bit divide (32-bit divide is 37 cycles).
    >
    >UltraSPARC-III and IV are worse, 6 cycles for multiply and 40/71
    >cycles (32/64-bit) for integer divides.
    >
    >Niagara is even worse :-)  11 cycle integer multiply and a 72 cycle
    >integer divide (regardless of 32-bit or 64-bit).
    >
    >(more details in gcc/config/sparc/sparc.c:{ultrasparc,ultrasparc3,niagara}_cost).
    >
    >So this change has tons of merit for sparc64 chips at least :-)
    >
    >Also, the multiply can parallelize with other operations but it
    >seems that integer divide stalls the pipe for most of the duration
    >of the calculation.  So this makes the divide even worse.
    这也就意味着，如果用当时的U，执行一次除法产生的开销至少能够计算8次乘法。而且这还没完，GMP
    文档中还有一个值得注意的地方，就是SPARC9的U寻址方式是大端序，而GMP的存储方式是小端序，这
    样一来又会增加一大笔寻址产生的开销而且可能还会干扰指令调度。当时GCC3的调度器有个非常恼人
    的特性是如果不把一些值缓存到变量会凭空生成一堆没什么卵用的指令。以上种种因素相互叠加耦合的
    结果就是本来就龟速的除法运算搞得更是“一拖四”。这也就是为什么GMP诞生时仅仅为了优化一个小小
    的除法器写了一吨代码。当然到了现在随着编译器不断升级以及硬件层SRT器和OoO（乱序执行）的引
    入，硬件除法器也得到了完整的流水线化并降低了延迟，实测std::div的速度已经能追平当年GMP的
    优化了。另外IEEE在一篇文章中提到，目前的先进除法器已经有能力在2个时钟周期内完成一次运算。
    因此本文的除法器直接用汇编写，因为std::div不支持无符号除法，而有符号除法和无符号除法语义
    是不一样的，不能直接套用，所以同时也会保留GMP的优化实现来弥补这一点。

    @par 参考文献
    [1] Moller N , Granlund T .Improved Division by Invariant Integers[J].IEEE 
        Transactions on Computers, 2010, 60(2):165-175.DOI:10.1109/TC.2010.143.
    [2]	Angioli M, Barbirotta M, Cheikh A, 等. Design, Implementation and 
        Evaluation of a New Variable Latency Integer Division Scheme[J/OL]. IEEE 
        Transactions on Computers, 2024, 73(7): 1767-1779. 
        DOI:10.1109/TC.2024.3386060.
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

/**
 * @brief 标准化除法器基类，适用于那些需要被除数和除数进行规格化的除法器
 */
class NormalizedDividerBase : public Divider
{
public:
    using Mybase = Divider;

protected:
    BlockArray Numerator;       ///<被除数（处理后）
    BlockArray Denominator;     ///<除数（处理后）
    size_t     Shift;           ///<移位数
    BlockType  DenomReciprocal; ///<除数的倒数

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
    size_t SingleBlockDividerNormalizedThreshold = -1;
    /// @brief 单块除法器执行除数最高位不为1的除法时，被除数块数高于此值切换为倒数除法器
    size_t SingleBlockDividerUnnormalizedThreshold = -1;

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
     * @brief 原子操作：双块除法器（64位 / 32位）
     * @param AX 被除数
     * @param BX 除数
     * @return constexpr DblBlkType {商, 余数}
     * @note 丹霞：这里会出现一个两难的问题，就是执行一次/和一次%得到结果，编译器可能会变出
     * 两次除法，而C++17提供了std::div，但是这个函数直到C++23都没有无符号除法的支持（有符
     * 号除法和无符号除法语义不同，直接用会出问题）。因此最快的方法还是用汇编写，但是这样也会
     * 产生一个问题就是只能在一个平台用，不同平台div用法可能不一样甚至某些平台连个像样的div
     * 都没有。据我所知aa要udiv+msub两步才能得到商和余数，la的div.wu和mod.wu是两个独立指
     * 令，同样无法一步到位。GMP刚诞生那会就是为此写了一大堆宏来适配不同的平台。
     */
    __declspec(optimize("O2")) inline constexpr static DblBlkType __Atomic_DblDivSng(DblBlkType AX, BlockType BX)
    {
        #if (defined (__amd64__) || defined(__x86_64__) || defined(_M_AMD64) || defined(_M_X64)) || \
            (defined (__i386__) || defined (__i486__) || defined(_M_IX86)) // x86
        // 这样写固然是最快的，但是我更希望用更直观的写法然后剩下的交给编译器优化
        DblBlkType Result;
        __asm("divl %4": "=a" (Result.first), "=d" (Result.second) : "0" (AX.first), "1" (AX.second), "rm" (BX));
        return Result;
        #else // 但是这样最终编译器就可能会变出两次除法了，但据说新版编译器开O1能优化
        ExtBlockType AXE = __Atomic_DblBlkToExtType(AX);
        return std::pair{AXE / BX, AXE % BX}; 
        #endif
        #if 0 // C++17及以上标准的解法，但是最高也只能覆盖到64位
        // 而且std::div只能执行有符号除法，和无符号除法语义不同，实测这么写会出问题。
        ExtBlockType AXE = __Atomic_DblBlkToExtType(AX);
        auto Result = std::lldiv(AXE, BX);
        return std::pair{Result.quot, Result.rem};
        #endif // 属实没有别的正解了（
    }
    
    /**
     * @brief 原子操作：双块倒数除法器（64位 / 32位）
     * @note 此除法器仅在除数最高位为1且被除数最高块的值小于除数时生效
     * @param AX 被除数
     * @param BX 除数
     * @param BR 除数的倒数
     * @return constexpr DblBlkType {商, 余数}
     */
    __declspec(optimize("O2")) inline constexpr static DblBlkType __Atomic_DblDivSngRecip(
        DblBlkType AX, BlockType BX, BlockType BR)
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
     * @brief 原子操作：三块除法器（96位 / 64位）
     * @note 此除法器仅在除数最高位为1且被除数最高2块的值小于除数时生效，
     * 另外C++没有自带的96位数据类型，因此这种规模只有倒数除法器
     * @param AX 被除数
     * @param BX 除数
     * @param BR 除数的倒数
     * @return constexpr TplBlkType {商, 余数低位, 余数高位}
     */
    __declspec(optimize("O2")) inline constexpr static TplBlkType __Atomic_TplDivDblRecip(
        TplBlkType AX, DblBlkType BX, BlockType BR)
    {
        auto [U0, U1, U2] = AX;
        auto [D0, D1] = BX;
        ExtBlockType Q = ExtBlockType(BR) * ExtBlockType(U2)
            + __Atomic_DblBlkToExtType({U1, U2});
        auto [Q0, Q1] = __Atomic_ExtTypeToDblBlk(Q);
        // Compute the two most significant blocks of n - q * d
        BlockType RT = U1 - Q1 * D1;
        ExtBlockType D = __Atomic_DblBlkToExtType(BX);
        ExtBlockType R = __Atomic_DblBlkToExtType({U0, RT})
            - (ExtBlockType(D0) * ExtBlockType(Q1)) - D;
        ++Q1;
        if ((R >> BSIZE) > Q0)
        {
            --Q1;
            R += D;
        }
        if (R >= D) [[unlikely]]
        {
            ++Q1;
            R -= D;
        }
        auto [R0, R1] = __Atomic_ExtTypeToDblBlk(R);
        return std::tuple{Q1, R0, R1};
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
    static void __Single_Block_Long_Div_Normalized_Impl(
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
    static void __Single_Block_Recip_Div_Normalized_Impl(
        BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
        BlockType D,
        BlockArrayView::reverse_iterator QI, 
        BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
        BlockType* R, BlockType RI);

    /**
     * @brief 单块竖式除法器第二阶段函数，不要求除数最高块为1（利用商不变的规律移位求解）
     * @param NI 被除数起始
     * @param NE 被除数终止
     * @param D 除数
     * @param QI 商起始
     * @param QEI 扩展商起始
     * @param QEE 扩展商终止
     * @param R 余数
     * @param RI 初始余数
     */
    static void __Single_Block_Long_Div_Unnormalized_Impl(
        BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
        BlockType D, 
        BlockArrayView::reverse_iterator QI, 
        BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
        BlockType* R, BlockType RI);

    /**
     * @brief 单块倒数除法器第二阶段函数，不要求除数最高块为1（利用商不变的规律移位求解）
     * @param NI 被除数起始
     * @param NE 被除数终止
     * @param D 除数
     * @param QI 商起始
     * @param QEI 扩展商起始
     * @param QEE 扩展商终止
     * @param R 余数
     * @param RI 初始余数
     */
    static void __Single_Block_Recip_Div_Unnormalized_Impl(
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
        return BlockType(~(ExtBlockType(AX) << BSIZE) / AX);
    }

    /**
     * @brief 创建除数的倒数
     * @param AX 除数
     * @return constexpr BlockType 除数的倒数
     */
    inline constexpr static BlockType MakeReciprocal2(DblBlkType AX)
    {
        BlockType V = MakeReciprocal1(AX.second);
        // We have β^2 − d_1 <= (β + v) * d_1 < β^2.
        BlockType P = AX.second * V + AX.first;
        if (P < AX.first) // Equivalent to carry out
        {
            --V;
            if (P >= AX.second)
            {
                --V;
                P -= AX.second;
            }
            P -= AX.second;
        }
        // We have β^2 − d_1 <= (β + v) * d_1 + d_0 < β^2.
        ExtBlockType T = ExtBlockType(V) * ExtBlockType(AX.first);
        auto [T0, T1] = __Atomic_ExtTypeToDblBlk(T);
        P += T1;
        if (P < T1) // Equivalent to carry out
        {
            --V;
            if (P >= AX.second || T0 >= AX.first) [[unlikely]]
            {
                --V;
            }
        }
        return V;
    }

    /**
     * @brief 单块除法
     * @param RAX 商
     * @param RBX 扩展商
     * @param RDX 余数
     * @param AX 被除数
     * @param BX 除数
     * @param TN 执行除数最高位为1的除法时，被除数块数高于此值切换为倒数除法器
     * @param TU 执行除数最高位不为1的除法时，被除数块数高于此值切换为倒数除法器
     */
    static void SingleBlockDiv(
        BlockArrayView RAX, BlockArrayView RBX, BlockType* RDX,
        BlockArraySrcView AX, BlockType BX, 
        size_t TN = -1, size_t TU = -1);

    /**
     * @brief 双块除法
     * @param RAX 商
     * @param RBX 扩展商
     * @param RDX 余数
     * @param AX 被除数
     * @param BX 除数
     */
    static void DoubleBlockDiv(
        BlockArrayView RAX, BlockArrayView RBX, DblBlkType* RDX, 
        BlockArraySrcView AX, DblBlkType BX);

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