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

// File is used to implementation of methods of class to calculate parameters of image

#include <cmath>
#include <vector>
#include <map>
#include <set>

#include "ImageParametersCalculator.h"


namespace acv {

ImageParametersCalculator::ImageParametersCalculator()
    : mImage(nullptr)
{
}

ImageParametersCalculator::ImageParametersCalculator(const Image& img)
    : mImage(&img)
{
}

void ImageParametersCalculator::UpdateImage(const Image& img)
{
    mImage = &img;
}

double ImageParametersCalculator::CalcEntropy()
{
    if (!mImage || !mImage->IsInitialized())
        return 0.0;

    // Each element of vector contains the number of image pixels
    // with brightness value which is equal to index of vector (Lebesgue measure)
    std::vector<size_t> mz(Image::MAX_PIXEL_VALUE + 1, 0);

    // Calculate of the volume of brightness and Lebesgue measure
    int V = 0;
    for (auto imgPix : mImage->GetData())
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

double ImageParametersCalculator::CalcLocalEntropy(const int row, const int col, const int aperture)
{
    if (!mImage || !mImage->IsInitialized())
        return 0.0;

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
            mImage->CorrectCoordinates(innerRow, innerCol);

            Image::Byte z = mImage->GetPixel(innerRow, innerCol);

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

double ImageParametersCalculator::CalcAverageBrightness()
{
    long sum = 0;

    if (!mImage || !mImage->IsInitialized())
        return static_cast<double>(sum);

     for (auto imgPix : mImage->GetData())
         sum += imgPix;

     double ret = static_cast<double>(sum) / (mImage->GetHeight() * mImage->GetWidth());
     return ret;
}

Image::Byte ImageParametersCalculator::CalcMinBrightness()
{
    Image::Byte minBr = Image::MAX_PIXEL_VALUE;

    if (!mImage || !mImage->IsInitialized())
        return minBr;

    for (auto imgPix : mImage->GetData())
    {
        if (imgPix < minBr)
            minBr = imgPix;
    }

    return minBr;
}

Image::Byte ImageParametersCalculator::CalcMaxBrightness()
{
    Image::Byte maxBr = Image::MIN_PIXEL_VALUE;

    if (!mImage || !mImage->IsInitialized())
        return maxBr;

    for (auto imgPix : mImage->GetData())
    {
        if (imgPix > maxBr)
            maxBr = imgPix;
    }

    return maxBr;
}

void ImageParametersCalculator::CalcMinMaxBrightness(Image::Byte& minBrig, Image::Byte& maxBrig)
{
    minBrig = Image::MAX_PIXEL_VALUE;
    maxBrig = Image::MIN_PIXEL_VALUE;

    if (!mImage || !mImage->IsInitialized())
        return;

    for (auto imgPix : mImage->GetData())
    {
        if (imgPix < minBrig)
            minBrig = imgPix;
        else if (imgPix > maxBrig)
            maxBrig = imgPix;
    }
}

void ImageParametersCalculator::CreateBrightnessHistogram(std::vector<double>& brightnessHistogram)
{
    if (!mImage || !mImage->IsInitialized())
        return;

    for (auto imgPix : mImage->GetData()) // filling vector of brightness histogram
        ++brightnessHistogram[imgPix];
}

double ImageParametersCalculator::CalcStandardDeviation(const double aver)
{
    double sd = 0.0;

    if (!mImage || !mImage->IsInitialized())
        return sd;

    auto end = mImage->GetData().end();
    for (auto it = mImage->GetData().begin(); it != end; ++it)
    {
        double dev = static_cast<double>(*it - aver);
        sd += dev * dev;
    }

    sd /= mImage->GetHeight() * mImage->GetWidth() - 1;

    return sqrt(sd);
}

size_t ImageParametersCalculator::CalcNumberInformationLevels()
{
    if (!mImage || !mImage->IsInitialized())
        return 0;

    std::set<Image::Byte> levels;

    for (auto imgPix : mImage->GetData())
        levels.insert(imgPix);

    return levels.size();
}

double ImageParametersCalculator::CalcIntegralQualityIndicator()
{
    double iqi = 0.0;

    if (!mImage || !mImage->IsInitialized())
        return iqi;

    Image::Byte minBrig, maxBrig;
    CalcMinMaxBrightness(minBrig, maxBrig);
    double averBrig = CalcAverageBrightness();
    double stdDev = CalcStandardDeviation(averBrig);
    double entr = CalcEntropy();
    size_t numLevels = CalcNumberInformationLevels();

    double Ln;
    if (averBrig <= 107)
        Ln = averBrig / 128;
    else if (averBrig > 147)
        Ln = (255 - averBrig) / 128;
    else
        Ln = 1.0;
    double Sn;
    if (stdDev <= 50)
        Sn = stdDev / 50;
    else
        Sn = (100 - stdDev) / 50;
    double Kn = (maxBrig - minBrig) / 255;
    double Nn = numLevels / 256;
    double En = entr / 8;

    iqi = 0.33 * Ln + 0.27 * Sn + 0.20 * Kn + 0.13 * Nn + 0.07 * En;
    return iqi;
}

}
