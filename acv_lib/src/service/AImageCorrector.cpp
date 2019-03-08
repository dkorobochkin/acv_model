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

// This file contains implementations of methods for class AImageCorrector

#include "AImageCorrector.h"
#include "ImageCorrector.h"
#include "AImageManager.h"
#include "Image.h"

#include <memory>
#include <cassert>

acv::ImageCorrector::CorrectorType ConvertToEngineCorrectorType(ACorrectorType corType)
{
    switch (corType)
    {
    case ACorrectorType::SSRETINEX:
        return acv::ImageCorrector::CorrectorType::SSRETINEX;
    case ACorrectorType::AUTO_LEVELS:
        return acv::ImageCorrector::CorrectorType::AUTO_LEVELS;
    case ACorrectorType::NORM_AUTO_LEVELS:
        return acv::ImageCorrector::CorrectorType::NORM_AUTO_LEVELS;
    case ACorrectorType::GAMMA:
        return acv::ImageCorrector::CorrectorType::GAMMA;
    }

    assert(false);
    return acv::ImageCorrector::CorrectorType::AUTO_LEVELS;
}

bool AImageCorrector::Correct(const AImage& srcImg, AImage& dstImg, ACorrectorType corType)
{
    bool ret = false;

    const auto& sourceImage = AImageManager::GetEngineImage(srcImg);
    auto& destinationImage = AImageManager::GetEngineImage(dstImg);

    if (sourceImage && destinationImage)
        ret = acv::ImageCorrector::Correct(*sourceImage, *destinationImage, ConvertToEngineCorrectorType(corType));

    return ret;
}
