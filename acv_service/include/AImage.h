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

// This header file is used to define a wrapper for class Image from engine level

#ifndef AIMAGE_H
#define AIMAGE_H

#include "Image.h"

#include <memory>

typedef unsigned char AByte; // This type is used to representation of pixel brightness

// A wrapper of class Image from engine level
class AImage
{

public:

    // Constructor with dimensions
    AImage(int height, int width);

    // Get the width of image
    int GetWidth() const;

    // Get the height of image
    int GetHeight() const;

    // Get the pixel value by coordinates
    AByte GetPixel(int row, int col) const;

    // Set the pixel value by coordinates
    void SetPixel(int row, int col, AByte val);

private:

    // Constructor from low level representation of image
    AImage(const acv::Image& img);

private:

    // Low level representation of image
    const std::shared_ptr<acv::Image> mImage;

public:

    friend class AImageManager;

};

#endif // AIMAGE_H
