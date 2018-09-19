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

// В данном заголовочном файле содержится описание класса, который занимается фильтрацией изображений

#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

namespace acv {

class Image;

// Класс используется для фильтрации изображения различными методами
// Содержит только статические методы
class ImageFilter
{

public: // Публичные вспогательные типы

    // Допступные типы фильтрации
    enum class FilterType
    {
        MEDIAN, // Медианная фильтрация
        GAUSSIAN // Гауссова фильтрация
    };

    enum class OperatorType
    {
        SOBEL, // Оператор Собеля
        SCHARR // Оператор Щарра
    };

    // Доступные типы оператора Собеля (Щарра)
    enum class SobelTypes
    {
        VERTICAL, // Вертикальный
        HORIZONTAL // Горизонтальный
    };

public: // Публичные методы

    // Запуск фильтрации заданным методом
    // В результате вызова метода изменяется ВХОДНОЕ ИЗОБРАЖЕНИЕ
    // Возвращаемое значение говорит об успешности фильтрации
    static bool Filter(Image& img, const int filterSize, FilterType type);

    // Свертка изображения с заданным оператором
    static bool OperatorConvolution(Image& img, OperatorType operatorType, SobelTypes type);

private: // Закрытые методы

    // Медианная фильтрация изображения (размер фильтр должен быть нечетным)
    // В результате вызова метода изменяется ВХОДНОЕ ИЗОБРАЖЕНИЕ
    // Возвращаемое значение говорит об успешности фильтрации
    static bool Median(Image& img, const int filterSize);

    // Гауссовая фильтрация
    // В результате вызова метода изменяется ВХОДНОЕ ИЗОБРАЖЕНИЕ
    // Возвращаемое значение говорит об успешности фильтрации
    static bool Gaussian(Image& img, const int filterSize);

    // Свертка изображения с оператором Собеля
    static bool Sobel(Image& img, SobelTypes type);

    // Non-convolutional horizontal Sobel operator
    static bool NonConvSobelH(Image& img);

    // Non-convolutional vertical Sobel operator
    static bool NonConvSobelV(Image& img);

    // Свертка изображения с оператором Щарра
    static bool Scharr(Image& img, SobelTypes type);

};

}

#endif // IMAGE_FILTER_H
