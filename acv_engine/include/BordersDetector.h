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

// This header file use to define a class to detect the borders of image

#ifndef BORDERS_DETECTOR_H
#define BORDERS_DETECTOR_H

#include "Image.h"
#include "Point.h"
#include <list>

namespace acv {

// This class is used to detect the borders of image by several methods
// The class contains only static methods
class BordersDetector
{

    struct Gradient;

public: // Public auxiliary types

    // Types of border detectors
    enum class DetectorType
    {
        SOBEL, // Sobel detector
        SCHARR, // Scharr detector
        CANNY // Canny detector
    };

    // Types of border detection operators
    enum class OperatorTypes
    {
        VERTICAL, // Vertical operator
        HORIZONTAL // Horizontal operator
    };

private: // Constants

    enum
    {
        DEFAULT_MIN_THRESHOLD = 20, // Default minimum threshold for Canny algorithm
        DEFAULT_MAX_THRESHOLD = 90 // Default maximum threshold for Canny algorithm
    };

public: // Public methods

    // Detect the borders of image
    static bool DetectBorders(Image& img, DetectorType detectorType,
                              const Image::Byte thresholdMin = DEFAULT_MIN_THRESHOLD, const Image::Byte thresholdMax = DEFAULT_MAX_THRESHOLD);
    static bool DetectBorders(const Image& srcImg, Image& dstImg, DetectorType detectorType,
                              const Image::Byte thresholdMin = DEFAULT_MIN_THRESHOLD, const Image::Byte thresholdMax = DEFAULT_MAX_THRESHOLD);

    // Convolution of image with specified operator
    static bool OperatorConvolution(Image& img, DetectorType operatorType, OperatorTypes type);
    static bool OperatorConvolution(const Image& srcImg, Image& dstImg, DetectorType operatorType, OperatorTypes type);

private: // Private methods

    // Detect borders by using the Canny algorithm
    static bool Canny(Image& img, const Image::Byte thresholdMin, const Image::Byte thresholdMax);
    static bool Canny(const Image& srcImg, Image& dstImg, const Image::Byte thresholdMin, const Image::Byte thresholdMax);

    // Detect the borders by using Sobel algorithm
    static bool Sobel(Image& img);
    static bool Sobel(const Image& srcImg, Image& dstImg);

    // Detect the borders by using Scharr algorithm
    static bool Scharr(Image& img);
    static bool Scharr(const Image& srcImg, Image& dstImg);

    // Non-convolutional of image with Sobel operator
    static bool NonConvSobel(Image& img, OperatorTypes type);
    static bool NonConvSobel(const Image& srcImg, Image& dstImg, OperatorTypes type);

    // Non-convolutional horizontal Sobel operator
    static bool NonConvSobelH(Image& img);
    static bool NonConvSobelH(const Image& srcImg, Image& dstImg);

    // Non-convolutional vertical Sobel operator
    static bool NonConvSobelV(Image& img);
    static bool NonConvSobelV(const Image& srcImg, Image& dstImg);

    // Convolution of image with Scharr operator
    static bool ConvScharr(Image& img, OperatorTypes type);
    static bool ConvScharr(const Image& srcImg, Image& dstImg, OperatorTypes type);

    // Calculate the modules of two image to third image
    static void FormGradientModules(const Image& horizImg, const Image& vertImg, Image& modImg);

private: // Private methods for Canny algorithm

    // An edge thinning technique by using maximum suppression
    static bool MaximumSuppression(std::vector<std::vector<Gradient>>& gradients);

    // An auxiliary method to tracing the ambiguity area
    static void AmbiguityTrace(const int row, const int col, const int height, const int width,
                               std::vector<std::vector<BordersDetector::Gradient>>& gradients,
                               std::list<Point>& pixelGroup, int& closer);

private: // Private types

    // Gradiend of image
    struct Gradient
    {

        Image::Byte abs; // Module
        int arg; // Argument

        // Public constructors
        Gradient();
        Gradient(int horiz, int vert);

    };

};

}

#endif // BORDERS_DETECTOR_H
