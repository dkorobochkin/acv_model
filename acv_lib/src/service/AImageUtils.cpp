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

// This file contains implementations of auxiliary functions for images from service level

#include "AImageUtils.h"
#include "AImageManager.h"
#include "AImage.h"
#include "Image.h"

bool AImageUtils::ImagesHaveSameSizes(const AImage& lhs, const AImage& rhs)
{
    return ImagesHaveSameSizes(*AImageManager::GetEngineImage(lhs), *AImageManager::GetEngineImage(rhs));
}

bool AImageUtils::ImagesHaveSameSizes(const acv::Image& lhs, const acv::Image& rhs)
{
    bool same = lhs.IsInitialized() &&
                rhs.IsInitialized() &&
                lhs.GetWidth() == rhs.GetWidth() &&
                lhs.GetHeight() == rhs.GetHeight();
    return same;
}
