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

// This header is used to define a wrapper for class BordersDetector from engine level

#ifndef ABORDERS_DETECTOR_H
#define ABORDERS_DETECTOR_H

class AImage;

// Types of border detectors
enum class ADetectorType
{
    SOBEL, // Sobel detector
    SCHARR, // Scharr detector
    CANNY // Canny detector
};

// Types of border detection operators
enum class AOperatorType
{
    VERTICAL, // Vertical operator
    HORIZONTAL // Horizontal operator
};

// Wrapper for class BordersDetector from engine level
class ABordersDetector
{

public:

    // Detect the borders of image
    static bool DetectBorders(const AImage& srcImg, AImage& dstImg, ADetectorType detectorType);

    // Convolution of image with specified operator
    static bool OperatorConvolution(const AImage& srcImg, AImage& dstImg, ADetectorType detectorType, AOperatorType operatorType);

};

#endif // ABORDERS_DETECTOR_H
