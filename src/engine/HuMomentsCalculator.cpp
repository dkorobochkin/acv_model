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

// This file is used to implement the methods of class to calculate of Hu's moments

#include "HuMomentsCalculator.h"
#include "Image.h"

#include <cmath>

namespace acv {

HuMomentsCalculator::HuMomentsCalculator(const Image& img, const int xStart, const int yStart, const int xEnd, const int yEnd)
    : mImg(img),
      mXStart(xStart),
      mYStart(yStart),
      mXEnd(xEnd),
      mYEnd(yEnd),
      mMoments()
{
    if (mXStart > mXEnd || mYStart > mYEnd)
        return;
    if (mImg.IsInvalidCoordinates(mYStart, mXStart) || mImg.IsInvalidCoordinates(mYEnd, mXEnd))
        return;

    GetConstants();
    CalcNormCentralMoments();
    CalcHuMoment();
}

int HuMomentsCalculator::CalcRegularMoment(const int p, const int q)
{
    int m = 0;

    for (int i = mXStart; i <= mXEnd; ++i)
    {
        for (int j = mYStart; j <= mYEnd; ++j)
        {
            if (mImg.GetPixel(j, i) > Image::MIN_PIXEL_VALUE)
            {
                int kx = i - mXStart;
                int ky = j - mYStart;

                if (kx > 0 && ky > 0)
                    m += pow(static_cast<double>(kx), p) * pow(static_cast<double>(ky), q);
            }
        }
    }

    return m;
}

int HuMomentsCalculator::CalcCentralMoment(const int p, const int q)
{
    int mu = 0;

    for (int i = mXStart; i <= mXEnd; ++i)
    {
        for (int j = mYStart; j <= mYEnd; ++j)
        {
            if (mImg.GetPixel(j, i) > Image::MIN_PIXEL_VALUE)
            {
                int kx = i - mXStart - mXz;
                int ky = j - mYStart - mYz;

                if (kx > 0 && ky > 0)
                    mu += pow(static_cast<double>(kx), p) * pow(static_cast<double>(ky), q);
            }
        }
    }

    return mu;
}

double HuMomentsCalculator::CalcNormCentralMoment(const int p, const int q)
{
    if (mMu00 && p >= 0 && q >= 0)
        return CalcCentralMoment(p, q) / pow(static_cast<double>(mMu00), static_cast<double>(p + q + 2.0) / 2.0);
    else
        return 0.0;
}

void HuMomentsCalculator::GetConstants()
{
    mM00 = CalcRegularMoment(0, 0);
    mM10 = CalcRegularMoment(1, 0);
    mM01 = CalcRegularMoment(0, 1);

    if (mM00 > 0)
    {
        mXz = mM10 / mM00;
        mYz = mM01 / mM00;
    }
    else
        mXz = mYz = 0.0;

    mMu00 = CalcCentralMoment(0, 0);
}

void HuMomentsCalculator::CalcNormCentralMoments()
{
    mNu20 = CalcNormCentralMoment(2, 0);
    mNu02 = CalcNormCentralMoment(0, 2);
    mNu11 = CalcNormCentralMoment(1, 1);
    mNu30 = CalcNormCentralMoment(3, 0);
    mNu03 = CalcNormCentralMoment(0, 3);
    mNu12 = CalcNormCentralMoment(1, 2);
    mNu21 = CalcNormCentralMoment(2, 1);
}

void HuMomentsCalculator::CalcHuMoment()
{
    mMoments[0] = 0;
    mMoments[1] = mNu20 + mNu02;
    mMoments[2] = pow(mNu20 - mNu02, 2) + 4 * pow(mNu11, 2);
    mMoments[3] = pow(mNu30 - 3 * mNu12, 2) + pow(3 * mNu21 - mNu03, 2);
    mMoments[4] = pow(mNu30 + mNu12, 2) + pow(mNu21 + mNu03, 2);
    mMoments[5] = (mNu30 - 3 * mNu12) * (mNu30 + mNu12) * (pow(mNu30 + mNu12, 2) - 3 * pow(mNu21 + mNu03, 2)) +
                  (3 * mNu21 - mNu03) * (mNu21 + mNu03) * (3 * pow(mNu30 + mNu12, 2) - pow(mNu21 + mNu03, 2));
    mMoments[6] = (mNu20 - mNu02) * (pow(mNu30 + mNu12, 2) - pow(mNu21 + mNu03, 2)) + 4 * mNu11 * (mNu30 + mNu12) * (mNu21 + mNu03);
    mMoments[7] = (3 * mNu21 - mNu03) * (mNu30 + mNu12) * (pow(mNu30 + mNu12, 2) - 3 * pow(mNu21 + mNu03, 2)) +
                  (3 * mNu21 - mNu30) * (mNu21 + mNu03) * (3 * pow(mNu30 + mNu12, 2) - pow(mNu21 + mNu03, 2));
}

}
