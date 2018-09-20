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

// В данном файле содержится реализация методов класса, который занимается фильтрацией изображений

#include <vector>
#include <algorithm>

#include "MatrixFilter.h"
#include "ImageFilter.h"
#include "Image.h"

namespace acv {

bool ImageFilter::Filter(Image& img, const int filterSize, ImageFilter::FilterType type)
{
    switch (type)
    {
    case FilterType::MEDIAN:
        return Median(img, filterSize);
    case FilterType::GAUSSIAN:
        return Gaussian(img, filterSize);
    default:
        return false;
    }
}

bool ImageFilter::Filter(const Image& srcImg, Image& dstImg, const int filterSize, ImageFilter::FilterType type)
{
    switch (type)
    {
    case FilterType::MEDIAN:
        return Median(srcImg, dstImg, filterSize);
    case FilterType::GAUSSIAN:
        return Gaussian(srcImg, dstImg, filterSize);
    default:
        return false;
    }
}


bool ImageFilter::Median(Image& img, const int filterSize)
{
    if (filterSize % 2 != 0) // Размер фильтра должен быть нечетным
    {
        // Используем временной изображение, т.к. при фильтрации изменются значения пикселей
        Image tmpImg = Image(img);

        Median(img, tmpImg, filterSize);

        img = std::move(tmpImg);
        return true;
    }

    return false;
}

bool ImageFilter::Median(const Image& srcImg, Image& dstImg, const int filterSize)
{
    if (filterSize % 2 != 0) // Размер фильтра должен быть нечетным
    {
        const int APERTURE = filterSize / 2; // Размер апертуры фильтра
        const int MEDIAN = filterSize * filterSize / 2; // Индекс медианы (индекс элемента, который будет новым значением)

        // Для каждого пикселя изображения задаем окно размером filterSize*filterSize
        // и собираем все пиксели, входящие в окно в вектор
        std::vector<Image::Byte> pixelsWindow(filterSize * filterSize);

        for (int row = 0; row < dstImg.GetHeight(); ++row)
        {
            for (int col = 0; col < dstImg.GetWidth(); ++col)
            {
                // Проходим по окну и сохраняем значения канала пикселей, входящих в окно
                size_t vecIdx = 0;
                for (int relRow = row - APERTURE; relRow <= row + APERTURE; ++relRow)
                {
                    for (int relCol = col - APERTURE; relCol <= col + APERTURE; ++relCol)
                    {
                        // Проверяем выход пикселей за границы изображения
                        int pixRow = relRow;
                        int pixCol = relCol;
                        srcImg.CorrectCoordinates(pixRow, pixCol);

                        pixelsWindow[vecIdx++] = srcImg(pixRow, pixCol);
                    }
                }

                // Упорядочение массива полученных пикселей и установка нового значения канала
                std::nth_element(pixelsWindow.begin(), pixelsWindow.begin() + MEDIAN, pixelsWindow.end());
                dstImg(row, col) = pixelsWindow[MEDIAN];
            }
        }

        return true;
    }

    return false;
}

bool ImageFilter::Gaussian(Image& img, const int filterSize)
{
    if (filterSize % 2 != 0) // Размер фильтра должен быть нечетным
    {
        // Создание фильтра Гаусса
        MatrixFilter<int> filter(filterSize);

        const float SIGMA = (filterSize / 2.0 - 1.0) * 0.3 + 0.8, SIGMA2 = SIGMA * SIGMA;
        const int APERTURE = filterSize / 2, APERTURE2 = APERTURE * APERTURE;
        const float MIN = exp(-(2.0 * APERTURE2) / (2.0 * SIGMA2)) / (2.0 * M_PI * SIGMA2);

        int divider = 0.0;
        for (int filterRow = -APERTURE; filterRow <= APERTURE; ++filterRow)
        {
            for (int filterCol = -APERTURE; filterCol <= APERTURE; ++filterCol)
            {
                int filterVal = exp(-(filterRow * filterRow + filterCol * filterCol) / (2.0 * SIGMA2)) / (2.0 * M_PI * SIGMA2 * MIN);
                divider += filterVal;

                filter.SetElement(filterRow + APERTURE, filterCol + APERTURE, filterVal);
            }
        }
        filter.SetDivider(divider);

        return MatrixFilterOperations::FastConvolutionImage<int>(img, filter);
    }

    return false;
}

bool ImageFilter::Gaussian(const Image& srcImg, Image& dstImg, const int filterSize)
{
    bool ret = filterSize % 2 != 0;

    if (ret)
    {
        memcpy(dstImg.GetRawPointer(), srcImg.GetRawPointer(), srcImg.GetHeight() * srcImg.GetWidth());
        ret = ret && Gaussian(dstImg, filterSize);
    }

    return ret;
}

}
