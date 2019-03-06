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

// This header is used to define a wrapper for class ImageParametersCalculator from engine level

#ifndef AIMAGE_PARAMETERS_CALCULATOR_H
#define AIMAGE_PARAMETERS_CALCULATOR_H

#include "AImage.h"

// Wrapper for class ImageParametersCalculator from engine level
class AImageParametersCalculator
{

public:

    // Calculate the entropy of image
    static double CalcEntropy(const AImage& img);

    // Calculate the average brightness of image
    static double CalcAverageBrightness(const AImage& img);

    // Calculate the minimum brightness of image
    static AByte CalcMinBrightness(const AImage& img);

    // Calculate the maximum brightness of image
    static AByte CalcMaxBrightness(const AImage& img);

    // Calculate the minimum and maximum brightness of image
    static void CalcMinMaxBrightness(const AImage& img, AByte& minBrig, AByte& maxBrig);

    // Calculate the standard deviation of image brightness
    static double CalcStandardDeviation(const AImage& img, double aver);

    // Calculate the integral quality indicator of image
    static double CalcIntegralQualityIndicator(const AImage& img);

};

#endif // AIMAGE_PARAMETERS_CALCULATOR_H
