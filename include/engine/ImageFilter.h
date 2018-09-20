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

public: // Публичные методы

    // Запуск фильтрации заданным методом
    // В результате вызова метода изменяется ВХОДНОЕ ИЗОБРАЖЕНИЕ
    // Возвращаемое значение говорит об успешности фильтрации
    static bool Filter(Image& img, const int filterSize, FilterType type);

    // Run a filtration by the specified method
    // Return true if filtration was success
    static bool Filter(const Image& srcImg, Image& dstImg, const int filterSize, FilterType type);

private: // Закрытые методы

    // Медианная фильтрация изображения (размер фильтр должен быть нечетным)
    // В результате вызова метода изменяется ВХОДНОЕ ИЗОБРАЖЕНИЕ
    // Возвращаемое значение говорит об успешности фильтрации
    static bool Median(Image& img, const int filterSize);

    // Median filtration (filter size must be odd)
    // Return true if filtration was success
    static bool Median(const Image& srcImg, Image& dstImg, const int filterSize);

    // Гауссовая фильтрация
    // В результате вызова метода изменяется ВХОДНОЕ ИЗОБРАЖЕНИЕ
    // Возвращаемое значение говорит об успешности фильтрации
    static bool Gaussian(Image& img, const int filterSize);

    // Gaussian filtration (filter size must be odd)
    // Return true if filtration was success
    static bool Gaussian(const Image& srcImg, Image& dstImg, const int filterSize);

};

}

#endif // IMAGE_FILTER_H
