﻿//
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

// The file contains implementation the methods of filter class

#include <vector>
#include <algorithm>

#include "MatrixFilter.h"
#include "ImageFilter.h"
#include "Image.h"

namespace acv {

FiltrationResult ImageFilter::Filter(Image& img, const int filterSize, ImageFilter::FilterType type)
{
    switch (type)
    {
    case FilterType::MEDIAN:
        return Median(img, filterSize);
    case FilterType::GAUSSIAN:
        return Gaussian(img, filterSize);
    case FilterType::IIR_GAUSSIAN:
        return GaussianIIR( img, static_cast<float>(filterSize/6.0) );
    default:
        return FiltrationResult::INCORRECT_FILTER_TYPE;
    }
}

FiltrationResult ImageFilter::Filter(const Image& srcImg, Image& dstImg, const int filterSize, ImageFilter::FilterType type)
{
    switch (type)
    {
    case FilterType::MEDIAN:
        return Median(srcImg, dstImg, filterSize);
    case FilterType::GAUSSIAN:
        return Gaussian(srcImg, dstImg, filterSize);
    case FilterType::IIR_GAUSSIAN:
        return GaussianIIR(srcImg, dstImg, static_cast<float>(filterSize / 6.0));
    case FilterType::SSRETINEX:
        return SingleScaleRetinex(srcImg, dstImg, static_cast<float>(filterSize / 6.0));
    default:
        return FiltrationResult::INCORRECT_FILTER_TYPE;
    }
}

FiltrationResult ImageFilter::Median(Image& img, const int filterSize)
{
    if (filterSize % 2 != 0) // The filter size should be odd
    {
        // We use temporary image because of the value of pixels are changed in process of filtration
        Image tmpImg = Image(img);

        Median(img, tmpImg, filterSize);

        img = std::move(tmpImg);
        return FiltrationResult::SUCCESS;
    }

    return FiltrationResult::INCORRECT_FILTER_SIZE;
}

FiltrationResult ImageFilter::Median(const Image& srcImg, Image& dstImg, const int filterSize)
{
    if (filterSize % 2 != 0) // The filter size should be odd
    {
        const int APERTURE = filterSize / 2; // Aparture size
        const int MEDIAN = filterSize * filterSize / 2; // Median index (the index of element that will be a new value)

        // We collect all pixels from the window of size filterSize*filterSize to vector for each pixel
        std::vector<Image::Byte> pixelsWindow(filterSize * filterSize);

        for (int row = 0; row < dstImg.GetHeight(); ++row)
        {
            for (int col = 0; col < dstImg.GetWidth(); ++col)
            {
                size_t vecIdx = 0;
                for (int relRow = row - APERTURE; relRow <= row + APERTURE; ++relRow)
                {
                    for (int relCol = col - APERTURE; relCol <= col + APERTURE; ++relCol)
                    {
                        int pixRow = relRow;
                        int pixCol = relCol;
                        srcImg.CorrectCoordinates(pixRow, pixCol);

                        pixelsWindow[vecIdx++] = srcImg(pixRow, pixCol);
                    }
                }

                // Sort the vector and setting of new pixel value
                std::nth_element(pixelsWindow.begin(), pixelsWindow.begin() + MEDIAN, pixelsWindow.end());
                dstImg(row, col) = pixelsWindow[MEDIAN];
            }
        }

        return FiltrationResult::SUCCESS;
    }

    return FiltrationResult::INCORRECT_FILTER_SIZE;
}

FiltrationResult ImageFilter::Gaussian(Image& img, const int filterSize)
{
    if (filterSize % 2 != 0) // The filter size should be odd
    {
        // Creation of the Gaussian filter
        MatrixFilter<int> filter(filterSize);

        const float SIGMA = (filterSize / 2.0 - 1.0) * 0.3 + 0.8, SIGMA2 = SIGMA * SIGMA;
        const int APERTURE = filterSize / 2, APERTURE2 = APERTURE * APERTURE;
        const float MIN = exp(-(2.0 * APERTURE2) / (2.0 * SIGMA2)) / (2.0 * M_PI * SIGMA2);

        int divider = 0.0;
        for (int filterRow = -APERTURE; filterRow <= APERTURE; ++filterRow)
        {
            for (int filterCol = -APERTURE; filterCol <= APERTURE; ++filterCol)
            {
                int filterVal = exp(-(filterRow * filterRow + filterCol * filterCol) / (2.0 * SIGMA2)) / (2.0 * M_PI * SIGMA2 * MIN);
                divider += filterVal;

                filter.SetElement(filterRow + APERTURE, filterCol + APERTURE, filterVal);
            }
        }
        filter.SetDivider(divider);

        bool ret = MatrixFilterOperations::FastConvolutionImage<int>(img, filter);
        return (ret) ? FiltrationResult::SUCCESS : FiltrationResult::INTERNAL_ERROR;
    }

    return FiltrationResult::INCORRECT_FILTER_SIZE;
}

FiltrationResult ImageFilter::GaussianIIR(Image& img, float sigma)
{
    if (sigma >= 1.)
    {
        IIRfilter<float> Filter(sigma);
        Image::Byte* ptr = img.GetRawPointer(0);

        auto width = img.GetWidth();
        auto height = img.GetHeight();

        for (int rowNum = 0; rowNum < height; ++rowNum, ptr += width+1)    // Горизонтальные проходы БИХ-фильтра
        {
            Filter.Reset();
            for (int colNum = 0; colNum < width; ++colNum, ++ptr)
            {
                int ycurr = static_cast<int>( Filter.Solve(*ptr) );
                Image::CheckPixelValue(ycurr);
                *ptr = static_cast<Image::Byte>(ycurr);
            }
            --ptr;

            for (int colNum = width - 1; colNum >= 0; --colNum, --ptr)
            {
                int ycurr = static_cast<int>( Filter.Solve(*ptr) );
                Image::CheckPixelValue(ycurr);
                *ptr = static_cast<Image::Byte>(ycurr);
            }

        }

        ptr = img.GetRawPointer(0);
        for (int colNum = 0; colNum < width; ++colNum , ptr += width+1)    // Вертикальные проходы БИХ-фильтра
        {
            Filter.Reset();
            for (int rowNum = 0; rowNum < height; ++rowNum, ptr += width)
            {
                int ycurr = static_cast<int>( Filter.Solve(*ptr) );
                Image::CheckPixelValue(ycurr);
                *ptr = static_cast<Image::Byte>(ycurr);
             }
            ptr -= width;

            for (int rowNum = height - 1; rowNum >= 0; --rowNum, ptr -= width)
            {
                int ycurr = static_cast<int>( Filter.Solve(*ptr) );
                Image::CheckPixelValue(ycurr);
                *ptr = static_cast<Image::Byte>(ycurr);
            }
        }

        return FiltrationResult::SUCCESS;
    }

    return FiltrationResult::SMALL_FILTER_SIZE;
}

FiltrationResult ImageFilter::GaussianIIR(const Image& srcImg, Image& dstImg, float sigma)
{
    if (sigma < 1.0)
        return FiltrationResult::SMALL_FILTER_SIZE;

    memcpy(dstImg.GetRawPointer(), srcImg.GetRawPointer(), srcImg.GetHeight() * srcImg.GetWidth());
    FiltrationResult ret = GaussianIIR(dstImg, sigma);

    return ret;
}

FiltrationResult ImageFilter::SingleScaleRetinex(const Image& srcImg, Image& dstImg, float sigma)
{
        size_t size = dstImg.GetWidth() * dstImg.GetHeight();

        memcpy(dstImg.GetRawPointer(), srcImg.GetRawPointer(), size);
        if ( GaussianIIR(dstImg,12.) == FiltrationResult::SMALL_FILTER_SIZE) 
        	return FiltrationResult::SMALL_FILTER_SIZE;

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

        return FiltrationResult::SUCCESS;
}

FiltrationResult ImageFilter::Gaussian(const Image& srcImg, Image& dstImg, const int filterSize)
{
    if (filterSize % 2 == 0)
        return FiltrationResult::INCORRECT_FILTER_SIZE;

    memcpy(dstImg.GetRawPointer(), srcImg.GetRawPointer(), srcImg.GetHeight() * srcImg.GetWidth());
    FiltrationResult ret = Gaussian(dstImg, filterSize);

    return ret;
}

}
