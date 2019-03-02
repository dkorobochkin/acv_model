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

// This header file is used to define a classes that are work with matrix filter and do operations with him

#ifndef MATRIX_FILTER_H
#define MATRIX_FILTER_H

#include <vector>

#include "Image.h"

namespace acv {

// Generic class that used to representations of matrix filter with arbitrary type of elements
template<typename T>
class MatrixFilter
{

public: // Public constructors

    // Constructor from filter size and divider
    MatrixFilter(const int filterSize, const T divider)
        : mFilter(filterSize, std::vector<T>(filterSize)),
          mDivider(divider)
    { }

    // Constructor from filter size only
    MatrixFilter(const int filterSize)
        :
        MatrixFilter(filterSize, 0)
    { }

public: // Public methods

    // Set the value of the filter element
    void SetElement(const int rowNum, const int colNum, const T value)
    {
        mFilter[rowNum][colNum] = value;
    }

    // Get the value of the filter element
    T GetElement(const int rowNum, const int colNum) const
    {
        return mFilter[rowNum][colNum];
    }

    // Get the filter size
    int GetSize() const
    {
        return mFilter.size();
    }

    // Check correct of filter (filter size should be odd)
    bool IsCorrectFilter() const
    {
        return (GetSize() % 2 != 0);
    }

    // Get the filter aperture
    int GetAperture() const
    {
        return (GetSize() / 2);
    }

    // Get the filter divider
    T GetDivider() const
    {
        return mDivider;
    }

    // Set the filter divider
    void SetDivider(const T divider)
    {
        mDivider = divider;
    }

    // Get the reference to the filter row
    std::vector<T>& operator[](const int rowNum)
    {
        return mFilter[rowNum];
    }
    const std::vector<T>& operator[](const int rowNum) const
    {
        return mFilter[rowNum];
    }


private: // Private members

    // Filter - square matrix
    std::vector<std::vector<T>> mFilter;

    // Filter divider
    T mDivider;

};


// Class is used to do operations with matrix filter
// Contains only static methods
class MatrixFilterOperations
{

public: // Public methods

    // Convolution of image with filter
    template<typename FilterElementT>
    static bool ConvolutionImage(Image& img, const MatrixFilter<FilterElementT>& filter);

    // Fast convolution of image with filter
    template<typename FilterElementT>
    static bool FastConvolutionImage(Image& img, const MatrixFilter<FilterElementT>& filter);

    // Convolution of pixel with filter
    template<typename FilterElementT>
    static FilterElementT ConvolutionPixel(const Image& img, const int rowNum, const int colNum,
                                           const MatrixFilter<FilterElementT>& filter, const int aperture);

private: // Private methods

    // Convolution of pixel with filter (used in fast convolution)
    template<typename FilterElementT>
    inline static FilterElementT ConvolutionPixel(std::vector<Image::Byte*>& rowsStarts, const MatrixFilter<FilterElementT>& filter);

};

template<typename FilterElementT>
bool MatrixFilterOperations::ConvolutionImage(Image& img, const MatrixFilter<FilterElementT>& filter)
{
    if (!filter.IsCorrectFilter())
        return false;

    Image tmpImg(img.GetHeight(), img.GetWidth());

    int aperture = filter.GetAperture();
    for (int rowNum = 0; rowNum < img.GetHeight(); ++rowNum)
    {
        for (int colNum = 0; colNum < img.GetWidth(); ++colNum)
        {
            FilterElementT conv = MatrixFilterOperations::ConvolutionPixel<FilterElementT>(img, rowNum, colNum, filter, aperture);

            if (conv < Image::MIN_PIXEL_VALUE)
                conv = Image::MIN_PIXEL_VALUE;
            else if (conv > Image::MAX_PIXEL_VALUE)
                conv = Image::MAX_PIXEL_VALUE;

            tmpImg(rowNum, colNum) = static_cast<Image::Byte>(conv);
        }
    }

    img = std::move(tmpImg);
    return true;
}

template<typename FilterElementT>
bool MatrixFilterOperations::FastConvolutionImage(Image& img, const MatrixFilter<FilterElementT>& filter)
{
    if (!filter.IsCorrectFilter())
        return false;

    int aperture = filter.GetAperture();
    Image tmpImg = img.Resize(-aperture, -aperture, img.GetWidth() + aperture - 1, img.GetHeight() + aperture - 1);
    Image::Byte* pTmpPix = tmpImg.GetRawPointer();

    std::vector<Image::Byte*> rowsStarts(filter.GetSize());
    for (int i = 0; i < filter.GetSize(); ++i)
    {
        rowsStarts[i] = pTmpPix;
        pTmpPix += tmpImg.GetWidth();
    }

    Image::Byte* pDst = img.GetRawPointer();
    for (int rowNum = 0; rowNum < img.GetHeight(); ++rowNum)
    {
        for (int colNum = 0; colNum < img.GetWidth(); ++colNum)
        {
            FilterElementT conv = MatrixFilterOperations::ConvolutionPixel<FilterElementT>(rowsStarts, filter);
            Image::CheckPixelValue(conv);
            *pDst++ = static_cast<Image::Byte>(conv);
        }

        for (int i = 0; i < filter.GetSize(); ++i)
            rowsStarts[i] += filter.GetSize() - 1;
    }

    return true;
}

template<typename FilterElementT>
FilterElementT MatrixFilterOperations::ConvolutionPixel(const Image& img, const int rowNum, const int colNum,
                                                        const MatrixFilter<FilterElementT>& filter, const int aperture)
{
    FilterElementT res = 0;
    for (int pixRow = rowNum - aperture, filterRow = 0; pixRow <= rowNum + aperture; ++pixRow, ++filterRow)
    {
        for (int pixCol = colNum - aperture, filterCol = 0; pixCol <=  colNum + aperture; ++pixCol, ++filterCol)
        {
            int innerRow = pixRow;
            int innerCol = pixCol;
            img.CorrectCoordinates(innerRow, innerCol);

            res += filter.GetElement(filterRow, filterCol) * img.GetPixel(innerRow, innerCol);
        }
    }

    FilterElementT div = filter.GetDivider();
    if (div != 0)
        res /= div;

    return res;
}

template<typename FilterElementT>
inline FilterElementT MatrixFilterOperations::ConvolutionPixel(std::vector<Image::Byte*>& rowsStarts, const MatrixFilter<FilterElementT>& filter)
{
    FilterElementT res = 0;

    for (int row = 0; row < filter.GetSize(); ++row)
    {
        for (int col = 0; col < filter.GetSize(); ++col)
        {
            Image::Byte rowPix = *rowsStarts[row];
            res += filter.GetElement(row, col) * rowPix;
            ++rowsStarts[row];
        }
        rowsStarts[row] -= filter.GetSize() - 1;
    }

    FilterElementT div = filter.GetDivider();
    if (div != 0)
        res /= div;

    return res;
}

}

#endif // MATRIX_FILTER_H
