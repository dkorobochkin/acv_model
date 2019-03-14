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

// This file contains implementations of methods for class AImageParametersCalculator

#include "AImageParametersCalculator.h"
#include "ImageParametersCalculator.h"
#include "AImageManager.h"
#include "Image.h"

#include <memory>

AImageParametersCalculator::AImageParametersCalculator()
    : mCalculator(std::make_shared<acv::ImageParametersCalculator>())
{
}

AImageParametersCalculator::AImageParametersCalculator(const AImage& img)
    : mCalculator(nullptr)
{
    const auto& image = AImageManager::GetEngineImage(img);

    if (image)
        mCalculator = std::make_shared<acv::ImageParametersCalculator>(*image);
    else
        mCalculator = std::make_shared<acv::ImageParametersCalculator>();
}

bool AImageParametersCalculator::UpdateImage(const AImage& img)
{
    bool ret = mCalculator != nullptr;

    if (ret)
    {
        const auto& newImg = AImageManager::GetEngineImage(img);
        ret = ret && newImg != nullptr;

        if (ret)
            mCalculator->UpdateImage(*newImg);
    }

    return ret;
}

double AImageParametersCalculator::CalcEntropy()
{
    double ret = 0.0;

    if (mCalculator)
        ret = mCalculator->CalcEntropy();

    return ret;
}

double AImageParametersCalculator::CalcAverageBrightness()
{
    double ret = 0.0;

    if (mCalculator)
        ret = mCalculator->CalcAverageBrightness();

    return ret;
}

AByte AImageParametersCalculator::CalcMinBrightness()
{
    AByte ret = 0;

    if (mCalculator)
        ret = mCalculator->CalcMinBrightness();

    return ret;
}

AByte AImageParametersCalculator::CalcMaxBrightness()
{
    AByte ret = 0;

    if (mCalculator)
        ret = mCalculator->CalcMaxBrightness();

    return ret;
}

bool AImageParametersCalculator::CalcMinMaxBrightness(AByte& minBrig, AByte& maxBrig)
{
    bool ret = mCalculator != nullptr;

    if (ret)
        mCalculator->CalcMinMaxBrightness(minBrig, maxBrig);

    return ret;
}

double AImageParametersCalculator::CalcStandardDeviation(double aver)
{
    AByte ret = 0;

    if (mCalculator)
        ret = mCalculator->CalcStandardDeviation(aver);

    return ret;
}

double AImageParametersCalculator::CalcIntegralQualityIndicator()
{
    double ret = 0;

    if (mCalculator)
        ret = mCalculator->CalcIntegralQualityIndicator();

    return ret;
}

bool AImageParametersCalculator::CreateBrightnessHistogram(std::vector<double>& brightnessHistogram)
{
    bool ret = mCalculator != nullptr;

    if (ret)
        mCalculator->CreateBrightnessHistogram(brightnessHistogram);

    return ret;
}
