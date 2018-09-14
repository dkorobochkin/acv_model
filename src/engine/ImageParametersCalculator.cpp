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

// Файл используется для имплементации методов класса расчета параметров изображения

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

    // В каждом элементе вектора хранится количество точек изображения
    // со значение канала равному индексу вектора (мера Лебега)
    std::vector<size_t> mz(Image::MAX_PIXEL_VALUE + 1, 0);

    // Расчет яркостного объема изображения и меры Лебега
    int V = 0; // Яркостной объем изображения
    for (auto it : img.mPixels)
    {
        Image::Byte z = it;

        V += z;
        ++mz[z];
    }

    // Расчет энтропии
    const double LOG2 = log(2.0);
    double EX = 0.0; // Энтропия
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
    // Ключ - яркость, значение - количество пикселей с данной яркостью
    std::map<size_t, size_t> mz;

    // Расчет яркостного объема изображения и меры Лебега
    double V = 0.0; // Яркостной объем изображения
    for (int y = row - aperture; y < row + aperture; ++y)
    {
        for (int x = col - aperture; x < col + aperture; ++x)
        {
            // Внутренние индексы пикселя, которые могут изменится,
            // если пиксель выходит за границу матрицы
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

    // Расчет энтропии
    const double LOG2 = log(2.0);
    double EX = 0.0; // Энтропия
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

     for (auto it : img.mPixels)
     {
         aver += it;
     }

    aver /= img.GetHeight() * img.GetWidth();

    return aver;
}

}
