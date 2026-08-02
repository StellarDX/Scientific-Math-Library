/**
 * @file Dividers.cc
 * @author StellarDX Astronomy (1829934307@qq.com)
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

#include "SciMath/StellarDX-GMP/Dividers.h"
#include "SciMath/StellarDX-GMP/IO.h"
#include "SciMath/StellarDX-GMP/整数乘法器的长征.h"
#include "SciMath/StellarDX-GMP/Logics.h"

#include <bit>
#include <iterator>
#include <ranges>

_ALU_BEGIN

Divider::Divider(BlockArraySrcView AX, BlockArraySrcView BX) : 
    NumeratorOriginalView(AX), DenominatorOriginalView(BX)
{
    if (TEST(DenominatorOriginalView))
    {
        Panic("Divider: 除数为0");
    }
}

_DIVIDER_BEGIN

NormalizedDividerBase::NormalizedDividerBase(BlockArraySrcView AX, BlockArraySrcView BX)
    : Mybase(AX, BX) {}


LongReciprocalDivider::LongReciprocalDivider(BlockArraySrcView AX, BlockArraySrcView BX) 
    : Mybase(AX, BX)
{
    Init();
}

void LongReciprocalDivider::__Single_Block_Long_Div_Normalized_Impl(
    BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
    BlockType D, 
    BlockArrayView::reverse_iterator QI, 
    BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
    BlockType* R, BlockType RI)
{
    for (; NI != NE; ++NI)
    {
        std::tie(*QI, RI) = __Atomic_DblDivSng({*NI, RI}, D);
        ++QI;
    }
    for (; QEI != QEE; ++QEI)
    {
        std::tie(*QEI, RI) = __Atomic_DblDivSng({0, RI}, D);
    }
    if (R) {*R = RI;}
}

void LongReciprocalDivider::__Single_Block_Recip_Div_Normalized_Impl(
    BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
    BlockType D,
    BlockArrayView::reverse_iterator QI, 
    BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
    BlockType* R, BlockType RI)
{
    BlockType DI = MakeReciprocal1(D);
    for (; NI != NE; ++NI)
    {
        std::tie(*QI, RI) = __Atomic_DblDivSngRecip({*NI, RI}, D, DI);
        ++QI;
    }
    for (; QEI != QEE; ++QEI)
    {
        std::tie(*QEI, RI) = __Atomic_DblDivSngRecip({0, RI}, D, DI);
    }
    if (R) {*R = RI;}
}

void LongReciprocalDivider::__Single_Block_Long_Div_Unnormalized_Impl(
    BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
    BlockType D, 
    BlockArrayView::reverse_iterator QI, 
    BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
    BlockType* R, BlockType RI)
{
    size_t SFT = std::countl_zero(D);
    D <<= SFT;
    RI <<= SFT;

    if (NI != NE)
    {
        BlockType NH = *NI;
        RI |= (NH >> (BSIZE - SFT));
        ++NI;
        for (; NI != NE; ++NI)
        {
            BlockType NL = *NI;
            BlockType NSFT = (NH << SFT) | (NL >> (BSIZE - SFT));
            std::tie(*QI, RI) = __Atomic_DblDivSng({NSFT, RI}, D);
            ++QI;
            NH = NL;
        }
        std::tie(*QI, RI) = __Atomic_DblDivSng({NH << SFT, RI}, D);
    }

    for (; QEI != QEE; ++QEI)
    {
        std::tie(*QEI, RI) = __Atomic_DblDivSng({0, RI}, D);
    }

    if (R) {*R = RI >> SFT;}
}

void LongReciprocalDivider::__Single_Block_Recip_Div_Unnormalized_Impl(
    BlockArraySrcView::reverse_iterator NI, BlockArraySrcView::reverse_iterator NE, 
    BlockType D, 
    BlockArrayView::reverse_iterator QI, 
    BlockArrayView::reverse_iterator QEI, BlockArrayView::reverse_iterator QEE,
    BlockType* R, BlockType RI)
{
    size_t SFT = std::countl_zero(D);
    D <<= SFT;
    RI <<= SFT;
    BlockType DI = MakeReciprocal1(D);

    if (NI != NE)
    {
        BlockType NH = *NI;
        RI |= (NH >> (BSIZE - SFT));
        ++NI;
        for (; NI != NE; ++NI)
        {
            BlockType NL = *NI;
            BlockType NSFT = (NH << SFT) | (NL >> (BSIZE - SFT));
            std::tie(*QI, RI) = __Atomic_DblDivSngRecip({NSFT, RI}, D, DI);
            ++QI;
            NH = NL;
        }
        std::tie(*QI, RI) = __Atomic_DblDivSngRecip({NH << SFT, RI}, D, DI);
    }

    for (; QEI != QEE; ++QEI)
    {
        std::tie(*QEI, RI) = __Atomic_DblDivSngRecip({0, RI}, D, DI);
    }

    if (R) {*R = RI >> SFT;}
}

void LongReciprocalDivider::SingleBlockDiv(
    BlockArrayView RAX, BlockArrayView RBX, BlockType* RDX,
    BlockArraySrcView AX, BlockType BX, size_t TN, size_t TU)
{
    size_t N = AX.size() + RBX.size();
    if (N == 0) {return;}

    auto QI = std::reverse_iterator{RAX.begin() + (AX.size())};
    decltype(BX) R = 0;

    if (BX & BHBIT)
    {
        if (AX.size())
        {
            if (AX.back() >= BX)
            {
                R = AX.back() - BX;
                *QI = 1;
            }
            else 
            {
                R = AX.back();
                *QI = 0;
            }
            ++QI;
        }

        if (N < TN)
        {
            __Single_Block_Long_Div_Normalized_Impl(
                AX.rbegin() + 1, AX.rend(), BX, QI, RBX.rbegin(), RBX.rend(), RDX, R);
        }
        else
        {
            __Single_Block_Recip_Div_Normalized_Impl(
                AX.rbegin() + 1, AX.rend(), BX, QI, RBX.rbegin(), RBX.rend(), RDX, R);
        }
    }
    else
    {
        auto NI = AX.rbegin();

        if (AX.size())
        {
            BlockType NH = *NI;
            if (NH < BX)
            {
                *QI = 0;
                R = NH;
                ++QI;
                --N;
                if (N == 0)
                {
                    if (RDX) {*RDX = R;}
                    return;
                }
                ++NI;
            }
        }

        if (N < TU)
        {
            __Single_Block_Long_Div_Unnormalized_Impl(
                NI, AX.rend(), BX, QI, RBX.rbegin(), RBX.rend(), RDX, R);
        }
        else
        {
            __Single_Block_Recip_Div_Unnormalized_Impl(
                NI, AX.rend(), BX, QI, RBX.rbegin(), RBX.rend(), RDX, R);
        }
    }
}

void LongReciprocalDivider::Run(BlockArrayView RAX, BlockArrayView RBX, BlockType* RDX,
    BlockArraySrcView AX, BlockType BX)const
{
    SingleBlockDiv(RAX, RBX, RDX, AX, BX, 
        SingleBlockDividerNormalizedThreshold, 
        SingleBlockDividerUnnormalizedThreshold);
}

void LongReciprocalDivider::DoubleBlockDiv(
    BlockArrayView RAX, BlockArrayView RBX, DblBlkType* RDX, 
    BlockArraySrcView AX, DblBlkType BX)
{
    auto NI = AX.rbegin();
    BlockType RL = *(NI + 1), RH = *(NI);
    BlockType R = __Atomic_DblBlkToExtType({RL, RH});
    ExtBlockType D = __Atomic_DblBlkToExtType(BX);
    auto QI = std::reverse_iterator{RAX.begin() + AX.size() - 1};
    
    if (R >= D) // 商的最高块的值只可能是0或1,具体取决于被除数最高2块和除数的大小
    {
        std::tie(RL, RH) = __Atomic_ExtTypeToDblBlk(R - D);
        *QI = 1;
    }
    else {*QI = 0;}
    ++QI;

    BlockType DI = MakeReciprocal2(BX);

    for (NI = AX.rbegin() + 2; NI != AX.rend(); ++NI)
    {
        std::tie(*QI, RL, RH) = __Atomic_TplDivDblRecip({*NI, RL, RH}, BX, DI);
        ++QI;
    }

    if (!RBX.empty())
    {
        for (auto QEI = RBX.rbegin(); QEI != RBX.rend(); ++QEI)
        {
            std::tie(*QEI, RL, RH) = __Atomic_TplDivDblRecip({0, RL, RH}, BX, DI);
        }
    }

    if (RDX) {*RDX = {RL, RH};}
}

void LongReciprocalDivider::Run(BlockArrayView RAX, BlockArrayView RBX, ExtBlockType* RDX,
    BlockArraySrcView AX, ExtBlockType BX)const
{
    DblBlkType D = __Atomic_ExtTypeToDblBlk(BX);
    DblBlkType R;
    DoubleBlockDiv(RAX, RBX, &R, AX, D);
    if (RDX) {*RDX = __Atomic_DblBlkToExtType(R);}
}

void LongReciprocalDivider::Init()
{

}

void LongReciprocalDivider::Run(BlockArrayView RAX, BlockArrayView RBX, BlockArrayView RDX)const
{

}

_DIVIDER_END

_ALU_END