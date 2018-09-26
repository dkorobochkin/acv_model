//
// MIT License
//
// Copyright (c) 2018 Dmitriy Korobochkin, Vitaliy Garmash.
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

// This file is used to implement class methods that correct images

#include <vector>
#include <cmath>

#include "ImageCorrector.h"
#include "Image.h"
#include "ImageFilter.h"

namespace acv {

bool ImageCorrector::Correct(const Image& srcImg, Image& dstImg, CorrectorType corType)
{
    switch (corType)
    {
    case CorrectorType::SSRETINEX:
        return SingleScaleRetinex(srcImg, dstImg);
    default:
        return false;
    }
}

bool ImageCorrector::SingleScaleRetinex(const Image& srcImg, Image& dstImg)
{
    if (ImageFilter::Filter(srcImg, dstImg, ImageFilter::FilterType::IIR_GAUSSIAN, 72.0) != FiltrationResult::SUCCESS)
        return false;

    size_t size = dstImg.GetWidth() * dstImg.GetHeight();
    std::vector<float> Ret(size);

    auto srcIt= const_cast<Image&>(srcImg).GetData().cbegin();
    auto dstIt= dstImg.GetData().begin();
    auto retIt= Ret.begin();
    float retAvg=0.;

    for (size_t i = 0; i < size; ++i, ++srcIt, ++dstIt, ++retIt)
    {
         *retIt = (!*srcIt || !*dstIt) ? 0. : (static_cast<float>(*srcIt) / *dstIt) * log(*srcIt);
         retAvg += *retIt;
    }
    retAvg /= size;

    float Pmin = 0., Pmax = 2.5 * retAvg, DP = Pmax - Pmin;
    dstIt = dstImg.GetData().begin();
    retIt = Ret.begin();

    for (size_t i = 0; i < size; ++i, ++dstIt, ++retIt)
    {
       int px = Image::MAX_PIXEL_VALUE * (*retIt - Pmin) / DP;
       Image::CheckPixelValue(px);
       *dstIt = px;
    }

    return true;
}

}
