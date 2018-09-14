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

// В данном файле содержится реализация методов класса, представляющего изображение

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
    : mPixels(height*width),
      mWidth(width),
      mHeight(height)
{
    CalcAuxParameters();
}

Image::Image(const int height, const int width, const void* buf, Image::BufferType bufType)
    : Image(height, width)
{
    FillPixels(buf, bufType);
}

int Image::GetWidth() const
{
    return mWidth;
}

int Image::GetHeight() const
{
    return mHeight;
}

Image::Byte Image::GetPixel(const int rowNum, const int colNum) const
{
    return mPixels[mWidth*rowNum+colNum];
}

void Image::SetPixel(const int rowNum, const int colNum, const Image::Byte val)
{
    mPixels[mWidth*rowNum+colNum] = val;
}

Image::Byte& Image::operator()(const int rowNum, const int colNum)
{
    return mPixels[mWidth*rowNum+colNum];
}

const Image::Byte& Image::operator()(const int rowNum, const int colNum) const
{
    return mPixels[mWidth*rowNum+colNum];
}

bool Image::IsInitialized() const
{
    return (mWidth != -1 || mHeight != -1);
}

bool Image::IsInvalidCoordinates(const int rowNum, const int colNum)
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

void Image::CheckPixelValue(int& value)
{
    if (value < MIN_PIXEL_VALUE)
        value = MIN_PIXEL_VALUE;
    else if (value > MAX_PIXEL_VALUE)
        value = MAX_PIXEL_VALUE;
}

void Image::CalcAuxParameters()
{
    mAuxHeight = 2 * mHeight - 2;
    mAuxWidth = 2 * mWidth - 2;
}

void Image::FillPixels(const void* buf, Image::BufferType bufType)
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
            r = (*buf)++;
            g = (*buf)++;
            b = (*buf)++;

            mPixels[mWidth*row+col] = (r + g + b) / 3;
        }
}

void Image::FillPixelFromDirectShow(Byte* buf)
{
    const size_t ROW_SIZE = mWidth * sizeof(Byte);
    for (int row = 0; row < mHeight; ++row)
    {
        memcpy(buf, &mPixels[mWidth*row], ROW_SIZE);
        buf += mWidth;
    }
}

}
