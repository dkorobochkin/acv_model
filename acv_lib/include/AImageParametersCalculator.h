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

#include <vector>
#include <memory>

namespace acv {
class ImageParametersCalculator;
}

// Wrapper for class ImageParametersCalculator from engine level
class AImageParametersCalculator
{

public:

    // Default constructor
    AImageParametersCalculator();

    // Constructor with an image as a parameter
    AImageParametersCalculator(const AImage& img);

    // Copy-constructor
    AImageParametersCalculator(const AImageParametersCalculator&) = default;

    // Move-constructor
    AImageParametersCalculator(AImageParametersCalculator&&) = default;

    // Destructor
    virtual ~AImageParametersCalculator() = default;

public:

    // Assignment operator
    AImageParametersCalculator& operator = (const AImageParametersCalculator&) = default;

    // Move assignment operator
    AImageParametersCalculator& operator = (AImageParametersCalculator&&) = default;

public:

    // Update image to calculate the parameters
    bool UpdateImage(const AImage& img);

    // Calculate the entropy of image
    double CalcEntropy();

    // Calculate the average brightness of image
    double CalcAverageBrightness();

    // Calculate the minimum brightness of image
    AByte CalcMinBrightness();

    // Calculate the maximum brightness of image
    AByte CalcMaxBrightness();

    // Calculate the minimum and maximum brightness of image
    bool CalcMinMaxBrightness(AByte& minBrig, AByte& maxBrig);

    // Calculate the standard deviation of image brightness
    double CalcStandardDeviation(double aver);

    // Calculate the integral quality indicator of image
    double CalcIntegralQualityIndicator();

    // Create array for brightness histogram of image
    bool CreateBrightnessHistogram(std::vector<double>& brightnessHistogram);

private:

    // Low level representation of image parameters calculator
    std::shared_ptr<acv::ImageParametersCalculator> mCalculator;

};

#endif // AIMAGE_PARAMETERS_CALCULATOR_H
