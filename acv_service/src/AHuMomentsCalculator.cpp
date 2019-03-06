//
// MIT License
//
// Copyright (c) 2018-2019 Dmitriy Korobochkin, Vitaliy Garmash.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

// This file contains implementations of methods for class HuMomentsCalculator

#include "AHuMomentsCalculator.h"
#include "AImageManager.h"
#include "AImage.h"

AHuMomentsCalculator::AHuMomentsCalculator(const AImage& img, int xStart, int yStart, int xEnd, int yEnd)
    : mHuMomentsCalculator(nullptr)
{
    const auto& srcImg = AImageManager::GetEngineImage(img);
    if (srcImg)
        mHuMomentsCalculator = std::make_shared<acv::HuMomentsCalculator>(*srcImg, xStart, yStart, xEnd, yEnd);
}

bool AHuMomentsCalculator::GetHuMoments(AHuMoments& moments) const
{
    bool ret = mHuMomentsCalculator != nullptr;

    if (ret)
        moments = mHuMomentsCalculator->GetHuMoments();

    return ret;
}
