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

// This header file is used to define a wrapper for class ImageFilter from engine level

#ifndef AIMAGE_FILTER_H
#define AIMAGE_FILTER_H

class AImage;

// This enum is used to represent the result of image filtering
enum class AFiltrationResult
{
    SUCCESS, // Success of filtration
    INTERNAL_ERROR, // Error during filtration
    INCORRECT_FILTER_TYPE, // Incorrect type of filter
    INCORRECT_FILTER_SIZE, // Incorrect filter size (he should be odd)
    SMALL_FILTER_SIZE // Filter size is small (this code is used for IIR Gaussian filtration)
};

// Used types of filtration
enum class AFilterType
{
    MEDIAN, // Median filtration
    GAUSSIAN, // Gaissian filtration
    SEP_GAUSSIAN, // Separated gaussian filtration
    IIR_GAUSSIAN, // IIR-imitated gaussian filtration
    SHARPEN // Increase the sharpness of the image
};

// Used types of threshold
enum class AThresholdType
{
    MAX_MORE_THRESHOLD, // All pixels more than threshold will be Image::MAX_VALUE
    MIN_MORE_THRESHOLD  // All pixels more than threshold will be Image::MIN_VALUE
};

// Wrapper for class ImageFilter from engine level
class AImageFilter
{

public:

    // Run a filtration by the specified method
    static AFiltrationResult Filter(const AImage& srcImg, AImage& dstImg, AFilterType type, int filterSize);

    // Run an adaptive threshold processing
    static bool AdaptiveThreshold(const AImage& srcImg, AImage& dstImg, int filterSize, int threshold, AThresholdType thresholdType);

};

#endif // AIMAGEFILTER_H
