/**
 * @file Dividers.cc
 * @author StellarDX Astronomy (1829934307@qq.com)
 * @brief 
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

#include "SciMath/StellarDX-GMP/Dividers.h"
#include "SciMath/StellarDX-GMP/Memory.h"
#include "SciMath/StellarDX-GMP/Multipliers.h"
#include "SciMath/StellarDX-GMP/Logics.h"
#include "SciMath/StellarDX-GMP/SMLDefs.h"
#include "SciMath/StellarDX-GMP/Subtractors.h"
#include "SciMath/StellarDX-GMP/Adders.h"

#include <algorithm>
#include <bit>
#include <iterator>
#include <ranges>

_80000_BEGIN
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

void NormalizedDividerBase::Normalize()
{
    Shift = std::countl_zero(DenominatorOriginalView.back());

    Numerator.reserve(NumeratorOriginalView.size() + 1);
    Numerator.resize(NumeratorOriginalView.size());
    BlockType CL = 0;
    SHL(Numerator, NumeratorOriginalView, Shift, &CL);
    if (CL) {Numerator.push_back(CL);}

    Denominator.resize(DenominatorOriginalView.size());
    SHL(Denominator, DenominatorOriginalView, Shift);
}


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
    size_t Shift = std::countl_zero(BX);
    BlockArray AXS;
    AXS.reserve(AX.size() + 1);
    AXS.resize(AX.size());
    BlockType CL = 0;
    SHL(AXS, AX, Shift, &CL);
    if (CL) {AXS.push_back(CL);}
    BX <<= Shift;

    DblBlkType D = __Atomic_ExtTypeToDblBlk(BX);
    DblBlkType R;
    DoubleBlockDiv(RAX, RBX, &R, AXS, D);
    if (RDX)
    {
        *RDX = __Atomic_DblBlkToExtType(R) >> Shift;
    }
}

void LongReciprocalDivider::Init()
{
    // 除数只有1块时不做预处理，因为规格化的过程已经在计算阶段自动进行了
    if (DenominatorOriginalView.size() == 1) {return;}

    if (DenominatorOriginalView.size() == 2 && !(DenominatorOriginalView.back() & BHBIT))
    {
        Normalize();
        // 此处不处理倒数，在计算阶段处理
        return;
    }

    Normalize();
    DenomReciprocal = MakeReciprocal2({*(Denominator.rbegin() + 1), Denominator.back()});
}

void LongReciprocalDivider::Run(BlockArrayView RAX, BlockArrayView RBX, BlockArrayView RDX)const
{
    if (DenominatorOriginalView.size() == 1)
    {
        SingleBlockDiv(RAX, RBX, &RDX.front(), 
            NumeratorOriginalView, DenominatorOriginalView.front(), 
            SingleBlockDividerNormalizedThreshold, 
            SingleBlockDividerUnnormalizedThreshold);
        return;
    }

    BlockArraySrcView N = Numerator.empty() ? NumeratorOriginalView : Numerator;
    BlockArraySrcView D = Denominator.empty() ? DenominatorOriginalView : Denominator;

    if (DenominatorOriginalView.size() == 2)
    {
        DblBlkType R;
        DoubleBlockDiv(RAX, RBX, &R, N, {D.at(0), D.at(1)});
        RDX.at(0) = (R.first >> Shift) | (R.second << (BSIZE - Shift));
        RDX.at(1) = (R.second >> Shift);
        return;
    }

    auto NN = N.size(), DN = D.size();
    BlockArray R(N.begin(), N.end());
    BlockArrayView RP = R;

    auto QH = CMP(RP.subspan(NN - DN, DN), D);
    if (QH == QH.greater)
    {
        SUB(RP.subspan(NN - DN, DN), RP.subspan(NN - DN, DN), D, 0, nullptr, InlineSubtractorPtr);
        *(RAX.begin() + (NN - DN)) = 1;
    }
    else {*(RAX.begin() + (NN - DN)) = 0;}

    auto QI = std::reverse_iterator{RAX.begin() + (NN - DN)};

    auto DR = D.subspan(0, DN - 2);

    DblBlkType DH{*(D.rbegin() + 1), D.back()};
    BlockType N1 = RP.back(), N0;

    auto NS = RP | std::ranges::views::slide(DN) | std::ranges::views::reverse;
    for (auto NI = NS.begin() + 1; NI != NS.end(); ++NI)
    {
        auto NP = *NI;
        if (N1 == DH.second && NP.back() == DH.first) [[unlikely]]
        {
            *QI = BMASK;
            MSUB(NP, D, *QI);
            N1 = NP.back();
        }
        else
        {
            std::tie(*QI, N0, N1) =  __Atomic_TplDivDblRecip(
                {*(NP.rbegin() + 1), NP.back(), N1}, DH, DenomReciprocal);
            BlockType CY, CY1;
            MSUB(NP, DR, *QI, &CY);

            CY1 = N0 < CY ? 1 : 0;
            N0 -= CY;
            CY = N1 < CY1 ? 1 : 0;
            N1 -= CY1;
            *(NP.rbegin() + 1) = N0;

            if (CY) [[unlikely]]
            {
                BlockType CYA;
                ADD(NP.subspan(0, DN - 1), NP.subspan(0, DN - 1), D.subspan(0, DN - 1), 0, &CYA, InlineAdderPtr);
                N1 += DH.second + CYA;
                --(*QI);
            }
        }
        ++QI;
    }
    NS.back().back() = N1;
    if (!RDX.empty())
    {
        SHR(RDX, RP.subspan(0, DN), Shift);
    }
}


WideDenominator::WideDenominator(std::shared_ptr<Divider> DIVER) :
    Mybase(DIVER->NumeratorOriginalView, DIVER->DenominatorOriginalView),
    Base(DIVER) {}

void WideDenominator::Init()
{
    // TODO
}

void WideDenominator::Run(BlockArrayView RAX, BlockArrayView RBX, BlockArrayView RDX)const
{
    // TODO
}

_DIVIDER_END

void DIV1(BlockArrayView RAX, BlockType* RDX, BlockArraySrcView AX, BlockType BX, const SingleBlockDivider* DIVER)
{
    if (DIVER)
    {
        DIVER->Run(RAX, BlockArrayView(), RDX, AX, BX);
        return;
    }

    std::array D{BX};
    _DIVIDER LongReciprocalDivider Div(AX, D);
    Div.Run(RAX, BlockArrayView(), RDX, AX, BX);
}

void DIV2(BlockArrayView RAX, ExtBlockType* RDX, BlockArraySrcView AX, ExtBlockType BX, const DoubleBlockDivider* DIVER)
{
    if (DIVER)
    {
        DIVER->Run(RAX, BlockArrayView(), RDX, AX, BX);
        return;
    }

    std::array D{BlockType(BX), BlockType(BX >> BSIZE)};
    _DIVIDER LongReciprocalDivider Div(AX, D);
    Div.Run(RAX, BlockArrayView(), RDX, AX, BX);
}

void DIV(BlockArrayView RAX, BlockArrayView RDX, BlockArraySrcView AX, BlockArrayView BX, const Divider* DIVER)
{
    if (DIVER)
    {
        DIVER->Run(RAX, BlockArrayView(), RDX);
        return;
    }

    _DIVIDER LongReciprocalDivider Div(AX, BX);
    Div.Run(RAX, BlockArrayView(), RDX);
}

_ALU_END
_80000_END