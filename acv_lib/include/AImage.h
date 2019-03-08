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

#include <memory>

namespace acv
{
    class Image;
}

typedef unsigned char AByte; // This type is used to representation of pixel brightness

enum class AScaleType
{
    UPSCALE, // Upscaling
    DOWNSCALE // downscaling
};

// A wrapper of class Image from engine level
class AImage
{

public:

    enum
    {
        MIN_PIXEL_VALUE = 0, // Minimum value of pixel brightness
        MAX_PIXEL_VALUE = 255 // Maximum value of pixel brightness
    };

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

    // Check the initialization of image
    bool IsInitialized() const;

    // Image scaling (upscaling and downscaling)
    AImage Scale(short kScaleX, short kScaleY, AScaleType scaleType) const;

    // Check pixel coordinates for image boundaries
    bool IsValidCoordinates(int row, int col) const;

private:

    // Default constructor
    AImage();

    // Constructor from low level representation of image
    AImage(const acv::Image& img);

    // Constructor with moving from low level representation of image
    AImage(acv::Image&& img);

private:

    // Low level representation of image
    std::shared_ptr<acv::Image> mImage;

public:

    friend class AImageManager;

};

#endif // AIMAGE_H
