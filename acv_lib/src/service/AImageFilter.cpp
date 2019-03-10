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

// This file contains implementations of methods for class AImageFilter

#include "AImageFilter.h"
#include "ImageFilter.h"
#include "AImageManager.h"
#include "AImageUtils.h"
#include "AImage.h"

#include <cassert>

acv::ImageFilter::FilterType ConvertToEngineFilterType(AFilterType type)
{
    switch (type)
    {
    case AFilterType::MEDIAN:
        return acv::ImageFilter::FilterType::MEDIAN;
    case AFilterType::GAUSSIAN:
        return acv::ImageFilter::FilterType::GAUSSIAN;
    case AFilterType::SEP_GAUSSIAN:
        return acv::ImageFilter::FilterType::SEP_GAUSSIAN;
    case AFilterType::IIR_GAUSSIAN:
        return acv::ImageFilter::FilterType::IIR_GAUSSIAN;
    case AFilterType::SHARPEN:
        return acv::ImageFilter::FilterType::SHARPEN;
    }

    assert(false);
    return acv::ImageFilter::FilterType::MEDIAN;
}

AFiltrationResult ConvertToAFiltrationResult(acv::FiltrationResult res)
{
    switch (res)
    {
    case acv::FiltrationResult::SUCCESS:
        return AFiltrationResult::SUCCESS;
    case acv::FiltrationResult::INTERNAL_ERROR:
        return AFiltrationResult::INTERNAL_ERROR;
    case acv::FiltrationResult::INCORRECT_FILTER_TYPE:
        return AFiltrationResult::INCORRECT_FILTER_TYPE;
    case acv::FiltrationResult::INCORRECT_FILTER_SIZE:
        return AFiltrationResult::INCORRECT_FILTER_SIZE;
    case acv::FiltrationResult::SMALL_FILTER_SIZE:
        return AFiltrationResult::SMALL_FILTER_SIZE;
    }

    assert(false);
    return AFiltrationResult::INTERNAL_ERROR;
}

AFiltrationResult AImageFilter::Filter(const AImage& srcImg, AImage& dstImg, AFilterType type, int filterSize)
{
    AFiltrationResult ret = AFiltrationResult::INTERNAL_ERROR;

    if (AImageUtils::ImagesHaveSameSizes(srcImg, dstImg))
    {
        const auto& srcImgPtr = AImageManager::GetEngineImage(srcImg);
        auto& dstImgPtr = AImageManager::GetEngineImage(dstImg);

        if (srcImgPtr && dstImgPtr)
        {
            acv::FiltrationResult engRes = acv::ImageFilter::Filter(*srcImgPtr, *dstImgPtr,
                                                                    ConvertToEngineFilterType(type), filterSize);
            ret = ConvertToAFiltrationResult(engRes);
        }
    }

    return ret;
}

acv::ImageFilter::ThresholdType ConvertToEngineThresholdType(AThresholdType thresholdType)
{
    switch (thresholdType)
    {
    case AThresholdType::MAX_MORE_THRESHOLD:
        return acv::ImageFilter::ThresholdType::MAX_MORE_THRESHOLD;
    case AThresholdType::MIN_MORE_THRESHOLD:
        return acv::ImageFilter::ThresholdType::MIN_MORE_THRESHOLD;
    }

    assert(false);
    return acv::ImageFilter::ThresholdType::MAX_MORE_THRESHOLD;
}

bool AImageFilter::AdaptiveThreshold(const AImage& srcImg, AImage& dstImg,
                                     int filterSize, int threshold, AThresholdType thresholdType)
{
    bool ret = AImageUtils::ImagesHaveSameSizes(srcImg, dstImg);

    if (ret)
    {
        const auto& srcImgPtr = AImageManager::GetEngineImage(srcImg);
        auto& dstImgPtr = AImageManager::GetEngineImage(dstImg);

        ret = ret && srcImgPtr != nullptr && dstImgPtr != nullptr;
        ret = ret && acv::ImageFilter::AdaptiveThreshold(*srcImgPtr, *dstImgPtr,
                                                         filterSize, threshold, ConvertToEngineThresholdType(thresholdType));
    }

    return ret;
}
