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

// This header file is used to define a wrapper for class HuMomentsCalculator from engine level

#ifndef AHU_MOMENTS_CALCULATOR_H
#define AHU_MOMENTS_CALCULATOR_H

#include <array>
#include <memory>

typedef std::array<double, 8> AHuMoments; // Array of Hu's moments values

class AImage;
namespace acv {
    class HuMomentsCalculator;
}

// Wrapper for class HuMomentsCalculator from engine level
class AHuMomentsCalculator
{

public:

    // The constructor to calculate a Hu's moments of image part
    // The user should provide: xStart < xEnd and yStart < yEnd
    AHuMomentsCalculator(const AImage& img, int xStart, int yStart, int xEnd, int yEnd);

public:

    // Get Hu's moments array
    bool GetHuMoments(AHuMoments& moments) const;

private:

    // Low level representation of Hu's moments calculator
    std::shared_ptr<acv::HuMomentsCalculator> mHuMomentsCalculator;

};

#endif // AHU_MOMENTS_CALCULATOR_H
