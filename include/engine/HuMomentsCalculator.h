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

// This header file is used to define a class to calculate a Hu's moments of the image

#ifndef HU_MOMENTS_CALCULATOR_H
#define HU_MOMENTS_CALCULATOR_H

#include <array>

namespace acv {

class Image;

// This array represent the Hu's moments
typedef std::array<double, 8> HuMoments;

// The class is used to calculate of Hu's moments
class HuMomentsCalculator
{

public: // Public constructors

    // The constructor to calculate a Hu's moments of image part
    // The user should provide: xStart < xEnd, yStart < yEnd
    HuMomentsCalculator(const Image& img, const int xStart, const int yStart, const int xEnd, const int yEnd);

public: // Public methods

    const HuMoments& GetHuMoments() const { return mMoments; }

private: // Private members

    // Calculate the regular moment
    int CalcRegularMoment(const int p, const int q);

    // Calculate the central moment
    int CalcCentralMoment(const int p, const int q);

    // Calculate the normalized central moment
    double CalcNormCentralMoment(const int p, const int q);

    // Calculate the constants
    void GetConstants();

    // Calculate the normalized central moments
    void CalcNormCentralMoments();

    void CalcHuMoment();

private: // Private members

    // The image to calculate his moments
    const Image& mImg;

    // The boundary coordinates of image part
    int mXStart;
    int mYStart;
    int mXEnd;
    int mYEnd;

    // Hu's moments
    HuMoments mMoments;

    // Regular moments
    int mM00, mM01, mM10, mMu00;

    // The center of gravity coordinates
    double mXz, mYz;

    // The normalized central moments
    double mNu20, mNu02, mNu11, mNu30, mNu03, mNu12, mNu21;

};

}

#endif // HU_MOMENTS_CALCULATOR_H
