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

// This file contains implementations of methods for class AImage

#include "AImage.h"
#include "Image.h"

#include <cassert>

AImage::AImage(int height, int width)
    : mImage(nullptr)
{
    if (height > 0 && width > 0)
        mImage = std::make_shared<acv::Image>(height, width);
}

int AImage::GetWidth() const
{
    return (mImage) ? mImage->GetWidth() : -1;
}

int AImage::GetHeight() const
{
    return (mImage) ? mImage->GetHeight() : -1;
}

AByte AImage::GetPixel(int row, int col) const
{
    return mImage->GetPixel(row, col);
}

void AImage::SetPixel(int row, int col, AByte val)
{
    mImage->SetPixel(row, col, val);
}

bool AImage::IsInitialized() const
{
    return (mImage && mImage->IsInitialized());
}

acv::Image::ScaleType ConvertToEngineScaleType(AScaleType scaleType)
{
    switch (scaleType)
    {
    case AScaleType::DOWNSCALE:
        return acv::Image::ScaleType::DOWNSCALE;
    case AScaleType::UPSCALE:
        return acv::Image::ScaleType::UPSCALE;
    }

    assert(false);
    return acv::Image::ScaleType::DOWNSCALE;
}

AImage AImage::Scale(short kScaleX, short kScaleY, AScaleType scaleType) const
{
    AImage retImg(-1, -1);

    if (mImage && kScaleX > 1 && kScaleY > 1)
        retImg.mImage = std::make_shared<acv::Image>(std::move(mImage->Scale(kScaleX, kScaleY, ConvertToEngineScaleType(scaleType))));

    return std::move(retImg);
}

bool AImage::IsValidCoordinates(int row, int col) const
{
    return ((mImage) ? !mImage->IsInvalidCoordinates(row, col) : false);
}
