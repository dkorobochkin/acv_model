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

// This file contains implementations of methods for class ABordersDetector

#include "ABordersDetector.h"
#include "BordersDetector.h"
#include "AImageManager.h"
#include "AImage.h"

#include <cassert>

acv::BordersDetector::DetectorType ConvertToEngineDetectorType(ADetectorType detectorType)
{
    switch (detectorType)
    {
    case ADetectorType::CANNY:
        return acv::BordersDetector::DetectorType::CANNY;
    case ADetectorType::SCHARR:
        return acv::BordersDetector::DetectorType::SCHARR;
    case ADetectorType::SOBEL:
        return acv::BordersDetector::DetectorType::SOBEL;
    }

    assert(false);
    return acv::BordersDetector::DetectorType::CANNY;
}

bool ABordersDetector::DetectBorders(const AImage& srcImg, AImage& dstImg, ADetectorType detectorType)
{
    bool ret = false;

    const auto& srcImgPtr = AImageManager::GetEngineImage(srcImg);
    auto& dstImgPtr = AImageManager::GetEngineImage(dstImg);

    if (srcImgPtr && dstImgPtr)
        ret = acv::BordersDetector::DetectBorders(*srcImgPtr, *dstImgPtr, ConvertToEngineDetectorType(detectorType));

    return ret;
}

acv::BordersDetector::OperatorType ConvertToEngineOperatorType(AOperatorType operatorType)
{
    switch (operatorType)
    {
    case AOperatorType::HORIZONTAL:
        return acv::BordersDetector::OperatorType::HORIZONTAL;
    case AOperatorType::VERTICAL:
        return acv::BordersDetector::OperatorType::VERTICAL;
    }

    assert(false);
    return acv::BordersDetector::OperatorType::HORIZONTAL;
}

bool ABordersDetector::OperatorConvolution(const AImage& srcImg, AImage& dstImg, ADetectorType detectorType, AOperatorType operatorType)
{
    bool ret = false;

    const auto& srcImgPtr = AImageManager::GetEngineImage(srcImg);
    auto& dstImgPtr = AImageManager::GetEngineImage(dstImg);

    if (srcImgPtr && dstImgPtr)
        ret = acv::BordersDetector::OperatorConvolution(*srcImgPtr, *dstImgPtr,
                                                        ConvertToEngineDetectorType(detectorType),
                                                        ConvertToEngineOperatorType(operatorType));

    return ret;
}
