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

double AImageParametersCalculator::CalcEntropy(const AImage& img)
{
    double ret = 0.0;

    const auto& image = AImageManager::GetEngineImage(img);
    if (image)
        ret = acv::ImageParametersCalculator::CalcEntropy(*image);

    return ret;
}

double AImageParametersCalculator::CalcAverageBrightness(const AImage& img)
{
    double ret = 0.0;

    const auto& image = AImageManager::GetEngineImage(img);
    if (image)
        ret = acv::ImageParametersCalculator::CalcAverageBrightness(*image);

    return ret;
}

AByte AImageParametersCalculator::CalcMinBrightness(const AImage& img)
{
    AByte ret = 0;

    const auto& image = AImageManager::GetEngineImage(img);
    if (image)
        ret = acv::ImageParametersCalculator::CalcMinBrightness(*image);

    return ret;
}

AByte AImageParametersCalculator::CalcMaxBrightness(const AImage& img)
{
    AByte ret = 0;

    const auto& image = AImageManager::GetEngineImage(img);
    if (image)
        ret = acv::ImageParametersCalculator::CalcMaxBrightness(*image);

    return ret;
}

void AImageParametersCalculator::CalcMinMaxBrightness(const AImage& img, AByte& minBrig, AByte& maxBrig)
{
    const auto& image = AImageManager::GetEngineImage(img);
    if (image)
        acv::ImageParametersCalculator::CalcMinMaxBrightness(*image, minBrig, maxBrig);
}

double AImageParametersCalculator::CalcStandardDeviation(const AImage& img, double aver)
{
    AByte ret = 0;

    const auto& image = AImageManager::GetEngineImage(img);
    if (image)
        ret = acv::ImageParametersCalculator::CalcStandardDeviation(*image, aver);

    return ret;
}

double AImageParametersCalculator::CalcIntegralQualityIndicator(const AImage& img)
{
    double ret = 0;

    const auto& image = AImageManager::GetEngineImage(img);
    if (image)
        ret = acv::ImageParametersCalculator::CalcIntegralQualityIndicator(*image);

    return ret;
}

bool AImageParametersCalculator::CreateBrightnessHistogram(const AImage& img, std::vector<double>& brightnessHistogram)
{
    const auto& image = AImageManager::GetEngineImage(img);

    bool ret = image != nullptr;
    if (ret)
        acv::ImageParametersCalculator::CreateBrightnessHistogram(*image, brightnessHistogram);

    return ret;
}
