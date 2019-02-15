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

// This file contains implementations of image class methods

#include <cstring>

#include "Image.h"

namespace acv {

Image::Image()
    : mPixels(),
      mWidth(-1),
      mHeight(-1),
      mAuxWidth(0),
      mAuxHeight(0)
{ }

Image::Image(const int height, const int width)
    : mPixels(height * width * sizeof(Byte)),
      mWidth(width),
      mHeight(height)
{
    CalcAuxParameters();
}

Image::Image(const int height, const int width, const void* buf, BufferType bufType)
    : Image(height, width)
{
    FillPixels(buf, bufType);
}

bool Image::IsInitialized() const
{
    return (mWidth != -1 || mHeight != -1);
}

bool Image::IsInvalidCoordinates(const int rowNum, const int colNum) const
{
    return (rowNum < 0 || rowNum >= mHeight || colNum < 0 || colNum >= mWidth);
}

void Image::CorrectCoordinates(int& rowNum, int& colNum) const
{
    if (rowNum < 0)
        rowNum = -rowNum;
    else if (rowNum >= mHeight)
        rowNum = mAuxHeight - rowNum;
    if (colNum < 0)
        colNum = -colNum;
    else if (colNum >= mWidth)
        colNum = mAuxWidth - colNum;
}

Image Image::Resize(const int xMin, const int yMin, const int xMax, const int yMax) const
{
    if (xMin >= xMax || yMin >= yMax)
        return Image();

    int newWidth = xMax - xMin + 1;
    int newHeight = yMax - yMin + 1;
    Image newImg(newHeight, newWidth);
    Byte* pDst = &newImg.mPixels[0];

    int row, col;
    int bufMaxY = (yMax >= mHeight) ? (mHeight - 1) : yMax;
    int bufMaxX = (xMax >= mWidth) ? (mWidth - 1) : xMax;

    // Expand upper rows
    for (row = yMin; row < 0; ++row )
    {
        for (col = xMin; col <= xMax; ++col)
        {
            int bufRow = row;
            int bufCol = col;
            CorrectCoordinates(bufRow, bufCol);

            *pDst++ = GetPixel(bufRow, bufCol);
        }
    }

    // Creation center rows
    for ( ; row <= bufMaxY; ++row)
    {
        // Expand left columns
        for (col = xMin; col < 0; ++col)
        {
            int bufRow = row;
            int bufCol = col;
            CorrectCoordinates(bufRow, bufCol);

            *pDst++ = GetPixel(bufRow, bufCol);
        }

        int centerWidth = bufMaxX - col + 1;
        const Byte* pSrc = &mPixels[0] + row * mWidth + col;
        memcpy(pDst, pSrc, centerWidth);
        col += centerWidth;
        pDst += centerWidth;

        // Expand right columns
        for ( ; col <= xMax; ++col)
        {
            int bufRow = row;
            int bufCol = col;
            CorrectCoordinates(bufRow, bufCol);

            *pDst++ = GetPixel(bufRow, bufCol);
        }
    }

    // Expand lower rows
    for ( ; row <= yMax; ++row )
    {
        for (col = xMin; col <= xMax; ++col)
        {
            int bufRow = row;
            int bufCol = col;
            CorrectCoordinates(bufRow, bufCol);

            *pDst++ = GetPixel(bufRow, bufCol);
        }
    }

    return newImg;
}

Image Image::operator - (const Image &rhs) const
{
    Matrix::const_iterator it1, it2;
    Matrix::iterator itDst;
    Image resImg(rhs.GetHeight(), rhs.GetWidth());
    for (it1 = this->GetData().cbegin(), it2 = rhs.GetData().cbegin(), itDst = resImg.GetData().begin();
         it1 != this->GetData().cend();
         ++it1, ++it2, ++itDst)
    {
        Byte brightnessOfPixel = (*it1 >= *it2) ? (*it1 - *it2) : (*it2 - *it1);
        *itDst = brightnessOfPixel;
    }
    return resImg;
}

void Image::CalcAuxParameters()
{
    mAuxHeight = 2 * mHeight - 2;
    mAuxWidth = 2 * mWidth - 2;
}

void Image::FillPixels(const void* buf, BufferType bufType)
{
    Byte* pData = const_cast<Byte*>(reinterpret_cast<const Byte*>(buf));

    switch (bufType)
    {
    case BufferType::RGB:
        FillPixelFromRGB(pData);
        break;
    case BufferType::DIRECT_SHOW:
        FillPixelFromDirectShow(pData);
        break;
    }
}

void Image::FillPixelFromRGB(Byte* buf)
{
    for (int row = 0; row < mHeight; ++row)
        for (int col = 0; col < mWidth; ++col)
        {
            Byte r, g, b;
            r = *buf++;
            g = *buf++;
            b = *buf++;

            mPixels[mWidth * row + col] = (r + g + b) / 3;
        }
}

void Image::FillPixelFromDirectShow(Byte* buf)
{
    memcpy(&mPixels[0], buf, mWidth * mHeight * sizeof(Byte));
}

}
