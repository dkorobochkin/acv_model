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

// This header use to define a class that are calculate parameters of image

#ifndef IMAGE_PARAMETERS_CALCULATOR_H
#define IMAGE_PARAMETERS_CALCULATOR_H

#include "Image.h"

namespace acv {

// Class is used to calculate parameters of image
class ImageParametersCalculator
{

public: // Constructors

    // Default constructor
    ImageParametersCalculator();

    // Constructor with an image as a parameter
    ImageParametersCalculator(const Image& img);

    // Copy-constructor
    ImageParametersCalculator(const ImageParametersCalculator&) = default;

    // Move-constructor
    ImageParametersCalculator(ImageParametersCalculator&&) = default;

    // Destructor
    virtual ~ImageParametersCalculator() = default;

public: // Operators

    // Assignment operator
    ImageParametersCalculator& operator = (const ImageParametersCalculator&) = default;

    // Move assignment operator
    ImageParametersCalculator& operator = (ImageParametersCalculator&&) = default;

public: // Public methods

    // Update image to calculate the parameters
    void UpdateImage(const Image& img);

    // Calculate the entropy of image
    double CalcEntropy();

    // Calculate the local entropy of image
    double CalcLocalEntropy(const int row, const int col, const int aperture);

    // Calculate the average brightness of image
    double CalcAverageBrightness();

    // Calculate the minimum brightness of image
    Image::Byte CalcMinBrightness();

    // Calculate the maximum brightness of image
    Image::Byte CalcMaxBrightness();

    // Calculate the minimum and maximum brightness of image
    void CalcMinMaxBrightness(Image::Byte& minBrig, Image::Byte& maxBrig);

    // Calculate the standard deviation of image brightness
    double CalcStandardDeviation(const double aver);

    // Calculate the integral quality indicator of image
    double CalcIntegralQualityIndicator();

    // Create array for brightness histogram of image
    void CreateBrightnessHistogram(std::vector<double>& brightnessHistogram);

private: // Private methods

    // Calculate the numer of information levels of image
    size_t CalcNumberInformationLevels();

private: // Private members

    // Image to calculate the parameters
    const Image* mImage;

};

}

#endif // IMAGE_PARAMETERS_CALCULATOR_H
