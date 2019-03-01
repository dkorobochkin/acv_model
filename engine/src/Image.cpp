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

Image Image::Scale(const short kScaleX, const short kScaleY, ScaleType scaleType) const
{
    switch (scaleType)
    {
    case ScaleType::DOWNSCALE:
        return AverageDownscale(kScaleX, kScaleY);
    case ScaleType::UPSCALE:
        return BilinearUpscale(kScaleX, kScaleY);
    default:
        return Image();
    }
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

Image Image::BilinearUpscale(const short kScaleX, const short kScaleY) const
{
    int newHeight = (GetHeight() - 1) * kScaleY + 1;
    int newWidth = (GetWidth() - 1) * kScaleX + 1;
    Image img(newHeight, newWidth);

    double dx = 1.0 / kScaleX;
    double dy = 1.0 / kScaleY;

    for (int row = 0, newRow = 0; row < GetHeight(); ++row, newRow += kScaleY)
    {
        for (int col = 0, newCol = 0; col < GetWidth(); ++col, newCol += kScaleX)
        {
            Byte I[4] = { GetPixel(row, col),           // I(r,c)
                          GetPixel(row + 1, col),       // I(r+1,c)
                          GetPixel(row, col + 1),       // I(r,c+1)
                          GetPixel(row + 1, col + 1) }; // I(r+1,c+1)

            for (int shiftRow = 0; shiftRow < kScaleY; ++shiftRow)
                 for (int shiftCol = 0; shiftCol < kScaleX; ++shiftCol)
                 {
                     int newShiftRow = newRow + shiftRow;
                     int newShiftCol = newCol + shiftCol;

                     if (img.IsInvalidCoordinates(newShiftRow, newShiftCol))
                         continue;

                     double curDx = shiftCol * dx;
                     double curDx2 = 1.0 - curDx;
                     double curDy = shiftRow * dy;
                     double curDy2 = 1.0 - curDy;

                     Byte newVal = I[0] * curDy2 * curDx2 +
                                   I[1] * curDy * curDx2 +
                                   I[2] * curDy2 * curDx  +
                                   I[3] * curDy * curDx;

                     img.SetPixel(newShiftRow, newShiftCol, newVal);
                 }
        }
    }

    return img;
}

Image Image::AverageDownscale(const short kScaleX, const short kScaleY) const
{
    Image img(GetHeight() / kScaleY, GetWidth() / kScaleX);

    int kXY = kScaleX * kScaleY;

    for (int newRow = 0; newRow < img.GetHeight(); ++newRow)
        for (int newCol = 0; newCol < img.GetWidth(); ++newCol)
        {
            int newVal = 0;

            for (int shiftRow = 0, row = newRow * kScaleY; shiftRow < kScaleY; ++shiftRow, ++row)
                 for (int shiftCol = 0, col = newCol * kScaleX; shiftCol < kScaleX; ++shiftCol, ++col)
                     newVal += GetPixel(row, col);

            img.SetPixel(newRow, newCol, static_cast<Byte>(newVal / kXY));
        }

    return img;
}

}
