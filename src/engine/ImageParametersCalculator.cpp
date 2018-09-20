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

// File is used to implementation of methods of class to calculate parameters of image

#include <cmath>
#include <vector>
#include <map>

#include "ImageParametersCalculator.h"
#include "Image.h"

namespace acv {

double ImageParametersCalculator::CalcEntropy(const Image& img)
{
    if (!img.IsInitialized())
        return 0.0;

    // Each element of vector contains the number of image pixels
    // with brightness value which is equal to index of vector (Lebesgue measure)
    std::vector<size_t> mz(Image::MAX_PIXEL_VALUE + 1, 0);

    // Calculate of the volume of brightness and Lebesgue measure
    int V = 0;
    for (auto imgPix : img.GetData())
    {
        Image::Byte z = imgPix;

        V += z;
        ++mz[z];
    }

    // Calculate of entropy
    const double LOG2 = log(2.0);
    double EX = 0.0;
    for (size_t z = 0; z < mz.size(); ++z)
    {
        double px = static_cast<double>(z * mz[z])/ V;
        if (px > 0.0)
            EX += px * log(px) / LOG2;
    }

    EX = fabs(EX);
    return EX;
}

double ImageParametersCalculator::CalcLocalEntropy(const Image& img, const int row, const int col, const int aperture)
{
    // Lebesgue measure
    // Key - brightness, value - number of pixels with brightness which is equal to key
    std::map<size_t, size_t> mz;

    // Calculate of the volume of brightness and Lebesgue measure
    double V = 0.0;
    for (int y = row - aperture; y < row + aperture; ++y)
    {
        for (int x = col - aperture; x < col + aperture; ++x)
        {
            // Inner coordinates which are can going abroad of image
            int innerRow = y;
            int innerCol = x;
            img.CorrectCoordinates(innerRow, innerCol);

            Image::Byte z = img(innerRow, innerCol);

            V += z;

            auto it = mz.find(z);
            if (it != mz.end())
                ++it->second;
            else
                mz[z] = 1;
        }
    }

    // Calculate of entropy
    const double LOG2 = log(2.0);
    double EX = 0.0;
    for (auto it = mz.begin(); it != mz.end(); ++it)
    {
        double px = it->first * it->second / V;
        if (px > 0.0)
            EX += px * log(px) / LOG2;
    }

    EX = fabs(EX);
    return EX;
}

double ImageParametersCalculator::CalcAverageBrightness(const Image& img)
{
    int aver = 0.0;

    if (!img.IsInitialized())
        return aver;

     for (auto imgPix : img.GetData())
     {
         aver += imgPix;
     }

    aver /= img.GetHeight() * img.GetWidth();

    return aver;
}

}
