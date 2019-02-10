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

// The file contains implementation the methods of filter class

#include <vector>
#include <algorithm>

#include "MatrixFilter.h"
#include "ImageFilter.h"
#include "Image.h"

namespace acv {

FiltrationResult ImageFilter::Filter(Image& img, ImageFilter::FilterType type, const int filterSize/* = -1*/)
{
    switch (type)
    {
    case FilterType::MEDIAN:
        return Median(img, filterSize);
    case FilterType::GAUSSIAN:
        return Gaussian(img, filterSize);
    case FilterType::IIR_GAUSSIAN:
        return GaussianIIR(img, static_cast<float>(filterSize/6.0));
    case FilterType::SHARPEN:
        return Sharpen(img);
    default:
        return FiltrationResult::INCORRECT_FILTER_TYPE;
    }
}

FiltrationResult ImageFilter::Filter(const Image& srcImg, Image& dstImg, ImageFilter::FilterType type, const int filterSize/* = -1*/)
{
    switch (type)
    {
    case FilterType::MEDIAN:
        return Median(srcImg, dstImg, filterSize);
    case FilterType::GAUSSIAN:
        return Gaussian(srcImg, dstImg, filterSize);
    case FilterType::SEP_GAUSSIAN:
        return SeparateGaussian(srcImg, dstImg, filterSize);
    case FilterType::IIR_GAUSSIAN:
        return GaussianIIR(srcImg, dstImg, static_cast<float>(filterSize / 6.0));
   case FilterType::SHARPEN:
        return Sharpen(srcImg, dstImg);
    default:
        return FiltrationResult::INCORRECT_FILTER_TYPE;
    }
}

bool ImageFilter::AdaptiveThreshold(Image& img, const int filterSize, const int threshold, ImageFilter::ThresholdType thresholdType)
{
    Image tmpImg = Image(img.GetHeight(), img.GetWidth());
    bool ret = AdaptiveThreshold(img, tmpImg, filterSize, threshold, thresholdType);
    if (ret)
        img = std::move(tmpImg);
    return ret;
}

bool ImageFilter::AdaptiveThreshold(const Image& srcImg, Image& dstImg, const int filterSize, const int threshold, ThresholdType thresholdType)
{
    FiltrationResult filterRes;
    if (filterSize >= 6)
        filterRes = GaussianIIR(srcImg, dstImg, static_cast<float>(filterSize / 6.0));
    else
        filterRes = SeparateGaussian(srcImg, dstImg, filterSize);

    if (filterRes != FiltrationResult::SUCCESS)
        return false;

    Image::Byte moreTh, lessTh;
    if (thresholdType == ThresholdType::MAX_MORE_THRESHOLD)
    {
        moreTh = Image::MAX_PIXEL_VALUE;
        lessTh = Image::MIN_PIXEL_VALUE;
    }
    else
    {
        moreTh = Image::MIN_PIXEL_VALUE;
        lessTh = Image::MAX_PIXEL_VALUE;
    }


    Image::Matrix::iterator itDst = dstImg.GetData().begin();
    for (auto it = srcImg.GetData().begin(); it != srcImg.GetData().end(); ++it)
    {
        int p = *itDst - threshold;
        *itDst++ = (*it > p) ? moreTh : lessTh;
    }

    return true;
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

FiltrationResult ImageFilter::SeparateGaussian(const Image& srcImg, Image& dstImg, const int filterSize)
{
    if (filterSize % 2 != 0) // The filter size should be odd
    {
        auto width = srcImg.GetWidth();
        auto height = srcImg.GetHeight();
        auto size = width * height;
        Image tmpImg(height, width);

        // Creation of the Gaussian 1D filter
        std::vector<int> filter(filterSize);

        const float SIGMA = (filterSize / 2.0 - 1.0) * 0.3 + 0.8, SIGMA2 = SIGMA * SIGMA;
        const int APERTURE = filterSize / 2, APERTURE2 = APERTURE * APERTURE;
        const float MIN = exp(-(2.0 * APERTURE2) / (2.0 * SIGMA2)) / (2.0 * M_PI * SIGMA2);

        int divider = 0;
        for (int i = -APERTURE; i <= APERTURE; ++i)
        {
                int filterVal = exp(-(i * i) / (2.0 * SIGMA2)) / (2.0 * M_PI * SIGMA2 * MIN);
                divider += filterVal;
                filter[i + APERTURE] = filterVal;
        }

        const Image::Byte* ptrSrc = srcImg.GetRawPointer(0);
        Image::Byte* ptrDst = tmpImg.GetRawPointer(0);

        for (int rowNum = 0; rowNum < height; ++rowNum)    // Horizontal filter movement
        {
            for (int colNum = 0; colNum < APERTURE; ++colNum, ++ptrSrc, ++ptrDst)
            {
                int acc = 0;
                for (int i = -APERTURE; i <= APERTURE; ++i)
                       acc += (colNum + i < 0)
                               ? *(ptrSrc - i) * filter[i + APERTURE]
                               : *(ptrSrc + i) * filter[i + APERTURE];
                int ycurr = acc / divider;
                *ptrDst = static_cast<Image::Byte>(ycurr);
            }

            for (int colNum = APERTURE; colNum < width - APERTURE; ++colNum, ++ptrSrc, ++ptrDst)
            {
                int acc = 0;
                for (int i = -APERTURE; i <= APERTURE; ++i)
                       acc += *(ptrSrc + i) * filter[i + APERTURE];
                int ycurr = acc / divider;
                *ptrDst = static_cast<Image::Byte>(ycurr);
            }

            for (int colNum = width - APERTURE; colNum < width; ++colNum, ++ptrSrc, ++ptrDst)
            {
                int acc = 0;
                for (int i = -APERTURE; i <= APERTURE; ++i)
                       acc += (colNum + i >= width)
                               ? *(ptrSrc - i) * filter[i + APERTURE]
                               : *(ptrSrc + i) * filter[i + APERTURE];
                int ycurr = acc / divider;
                *ptrDst = static_cast<Image::Byte>(ycurr);
            }
        }

        ptrSrc = tmpImg.GetRawPointer(0);
        ptrDst = dstImg.GetRawPointer(0);

        for (int colNum = 0; colNum < width; ++colNum, ptrSrc -= size - 1, ptrDst -= size - 1)    // Vertical filter movement
        {
            for (int rowNum = 0; rowNum < APERTURE; ++rowNum, ptrSrc += width, ptrDst += width)
            {
                int acc = 0;
                for (int i = -APERTURE; i <= APERTURE; ++i)
                       acc += (rowNum + i < 0)
                               ? *(ptrSrc - i * width) * filter[i + APERTURE]
                               : *(ptrSrc + i * width) * filter[i + APERTURE];
                int ycurr = acc / divider;
                *ptrDst = static_cast<Image::Byte>(ycurr);
            }

            for (int rowNum = APERTURE; rowNum < height - APERTURE; ++rowNum, ptrSrc += width, ptrDst += width)
            {
                int acc = 0;
                for (int i = -APERTURE; i <= APERTURE; ++i)
                       acc += *(ptrSrc + i * width) * filter[i + APERTURE];
                int ycurr = acc / divider;
                *ptrDst = static_cast<Image::Byte>(ycurr);
            }

            for (int rowNum = height - APERTURE; rowNum < height; ++rowNum, ptrSrc += width, ptrDst += width)
            {
                int acc = 0;
                for (int i = -APERTURE; i <= APERTURE; ++i)
                       acc += (rowNum + i >= height)
                               ? *(ptrSrc - i * width) * filter[i + APERTURE]
                               : *(ptrSrc + i * width) * filter[i + APERTURE];
                int ycurr = acc / divider;
                *ptrDst = static_cast<Image::Byte>(ycurr);
            }
        }

        return FiltrationResult::SUCCESS;
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

        for (int rowNum = 0; rowNum < height; ++rowNum, ptr += width+1)    // Horizontal IIR-filter movement
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
        for (int colNum = 0; colNum < width; ++colNum , ptr += width+1)    // Vertical IIR-filter movement
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

FiltrationResult ImageFilter::Gaussian(const Image& srcImg, Image& dstImg, const int filterSize)
{
    if (filterSize % 2 == 0)
        return FiltrationResult::INCORRECT_FILTER_SIZE;

    memcpy(dstImg.GetRawPointer(), srcImg.GetRawPointer(), srcImg.GetHeight() * srcImg.GetWidth());
    FiltrationResult ret = Gaussian(dstImg, filterSize);

    return ret;
}

FiltrationResult ImageFilter::Sharpen(Image& img)
{
    // Creation of the sharpen filter
    MatrixFilter<int> filter(3, 1);

    filter[0][0] = -1; filter[0][1] = -1; filter[0][2] = -1;
    filter[1][0] = -1; filter[1][1] =  9; filter[1][2] = -1;
    filter[2][0] = -1; filter[2][1] = -1; filter[2][2] = -1;

    bool ret = MatrixFilterOperations::FastConvolutionImage<int>(img, filter);
    return (ret) ? FiltrationResult::SUCCESS : FiltrationResult::INTERNAL_ERROR;
}

FiltrationResult ImageFilter::Sharpen(const Image& srcImg, Image& dstImg)
{
    memcpy(dstImg.GetRawPointer(), srcImg.GetRawPointer(), srcImg.GetHeight() * srcImg.GetWidth());
    FiltrationResult ret = Sharpen(dstImg);

    return ret;
}

}
