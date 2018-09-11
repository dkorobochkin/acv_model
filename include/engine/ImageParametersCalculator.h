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

// В данном заголовочном файле содержится описание класса, который расчитывает различные параметры изображения

#ifndef IMAGE_PARAMETERS_CALCULATOR_H
#define IMAGE_PARAMETERS_CALCULATOR_H

namespace acv {

class Image;

// Класс используется для расчета параметров изображения
// Содержит только статические методы
class ImageParametersCalculator
{

public: // Публичные методы

    // Расчет энтропии изображения
    static double CalcEntropy(const Image& img);

    // Расчет локальной энтропии изображенния
    static double CalcLocalEntropy(const Image& img, const int row, const int col, const int aperture);

    // Расчет средней яркости изображения
    static double CalcAverageBrightness(const Image& img);

};

}

#endif // IMAGE_PARAMETERS_CALCULATOR_H
