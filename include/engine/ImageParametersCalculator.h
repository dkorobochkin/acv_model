﻿//
// MIT License
//
// Copyright (c) 2018 Dmitriy Korobochkin, Vitaliy Garmash.
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

namespace acv {

class Image;

// Class is used to calculate parameters of image
// He contains only static methods
class ImageParametersCalculator
{

public: // Public methods

    // Calculate the entropy of image
    static double CalcEntropy(const Image& img);

    // Calculate the local entropy of image
    static double CalcLocalEntropy(const Image& img, const int row, const int col, const int aperture);

    // Calculate the average brightness of image
    static double CalcAverageBrightness(const Image& img);

};

}

#endif // IMAGE_PARAMETERS_CALCULATOR_H
