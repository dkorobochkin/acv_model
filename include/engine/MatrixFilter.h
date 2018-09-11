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

// Заголовочный файл используется для описания классов, работающих с матричными фильтрами и операциями над ними

#ifndef MATRIX_FILTER_H
#define MATRIX_FILTER_H

#include <vector>

#include "Image.h"

namespace acv {

// Шаблонный класс для представления матричного фильтра с произвольным типом элемента
template<typename T>
class MatrixFilter
{

public: // Публичные конструкторы

    // Конструктор принимает размер фильтра и делитель
    MatrixFilter(const int filterSize, const T divider)
        : mFilter(filterSize, std::vector<T>(filterSize)),
          mDivider(divider)
    { }

    // Конструктор принимает только размер фильтра
    MatrixFilter(const int filterSize)
        :
        MatrixFilter(filterSize, 0)
    { }

public: // Публичные методы

    // Установка значения элемента фильтра
    void SetElement(const int rowNum, const int colNum, const T value)
    {
        mFilter[rowNum][colNum] = value;
    }

    // Получение значения элемента фильтра
    T GetElement(const int rowNum, const int colNum) const
    {
        return mFilter[rowNum][colNum];
    }

    // Возвращает размер фильтра
    int GetSize() const
    {
        return mFilter.size();
    }

    // Проверка корректности фильтра (фильтр должен иметь нечетный размер)
    bool IsCorrectFilter() const
    {
        return (GetSize() % 2 != 0);
    }

    // Возвращает апертуру фильтра
    int GetAperture() const
    {
        return (GetSize() / 2);
    }

    // Возвращает делитель фильтра
    T GetDivider() const
    {
        return mDivider;
    }

    // Устанавливает делитель фильтра
    void SetDivider(const T divider)
    {
        mDivider = divider;
    }

    // Получение ссылки на строку фильтра
    std::vector<T>& operator[](const int rowNum)
    {
        return mFilter[rowNum];
    }
    const std::vector<T>& operator[](const int rowNum) const
    {
        return mFilter[rowNum];
    }


private: // Закрытые данные

    // Фильтр - квадратная матрица
    std::vector<std::vector<T>> mFilter;

    // Делитель фильтра
    T mDivider;

};


// Класс для выполнения операций c матричным фильтром
// Содержит только статические методы
class MatrixFilterOperations
{

public: // Публичные методы

    // Свертка изображения (выполняется для определенного канала изображения) с фильтром
    template<typename FilterElementT>
    static bool ConvolutionImage(Image& img, const MatrixFilter<FilterElementT>& filter);

private: // Закрытые методы

    // Свертка пикселя изображения (выполняется для определенного канала изображения) с фильтром
    template<typename FilterElementT>
    static FilterElementT ConvolutionPixel(const Image& img, const int rowNum, const int colNum,
                                           const MatrixFilter<FilterElementT>& filter, const int aperture);

};

template<typename FilterElementT>
bool MatrixFilterOperations::ConvolutionImage(Image& img, const MatrixFilter<FilterElementT>& filter)
{
    if (!filter.IsCorrectFilter())
        return false;

    Image tmpImg(img.GetHeight(), img.GetWidth());

    int aperture = filter.GetAperture();
    for (int rowNum = 0; rowNum < img.GetHeight(); ++rowNum)
    {
        for (int colNum = 0; colNum < img.GetWidth(); ++colNum)
        {
            FilterElementT conv = MatrixFilterOperations::ConvolutionPixel<FilterElementT>(img, rowNum, colNum, filter, aperture);

            if (conv < Image::MIN_PIXEL_VALUE)
                conv = Image::MIN_PIXEL_VALUE;
            else if (conv > Image::MAX_PIXEL_VALUE)
                conv = Image::MAX_PIXEL_VALUE;

            tmpImg(rowNum, colNum) = static_cast<Image::Byte>(conv);
        }
    }

    img = std::move(tmpImg);
    return true;
}

template<typename FilterElementT>
FilterElementT MatrixFilterOperations::ConvolutionPixel(const Image& img, const int rowNum, const int colNum,
                                                        const MatrixFilter<FilterElementT>& filter, const int aperture)
{
    FilterElementT res = 0;
    for (int pixRow = rowNum - aperture, filterRow = 0; pixRow <= rowNum + aperture; ++pixRow, ++filterRow)
    {
        for (int pixCol = colNum - aperture, filterCol = 0; pixCol <=  colNum + aperture; ++pixCol, ++filterCol)
        {
            // Внутренние индексы пикселя, которые могут изменится,
            // если пиксель выходит за границу матрицы
            int innerRow = pixRow;
            int innerCol = pixCol;
            img.CorrectCoordinates(innerRow, innerCol);

            res += filter.GetElement(filterRow, filterCol) * img.GetPixel(innerRow, innerCol);
        }
    }

    FilterElementT div = filter.GetDivider();
    if (div != 0)
        res /= div;

    return res;
}

}

#endif // MATRIX_FILTER_H
