//
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

// This header file use to define a class to detect the borders of image

#ifndef BORDERS_DETECTOR_H
#define BORDERS_DETECTOR_H

#include "Image.h"
#include "Point.h"

namespace acv {

// This class is used to detect the borders of image by several methods
// The class contains only static methods
class BordersDetector
{

    struct Gradient;

public: // Public methods

    // Detect borders by using the Canny algorithm
    static bool Canny(Image& img, const Image::Byte thresholdMin, const Image::Byte thresholdMax);

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
