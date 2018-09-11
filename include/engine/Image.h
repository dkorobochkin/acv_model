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

// В данном заголовочном файле содержится описание класса, представляющего изображение,
// методы для работы с ним, вспомогательные типы данных

#ifndef IMAGE_H
#define IMAGE_H

#include <vector>

namespace acv {

// Класс для работы с изображением (матрица пикселей)
class Image
{

public: // Константы

    enum
    {
        MIN_PIXEL_VALUE = 0, // Минимальное значение яркости пикселя
        MAX_PIXEL_VALUE = 255 // Максимальное значение яркости пикселя
    };

public: // Публичные вспогательные типы

    // Виды буфера данных, из которых возможно создать изображение
    enum class BufferType
    {
        RGB, // RGB представление
        DIRECT_SHOW // Представление DirectShow
    };

public: // Публичные вспогательные типы данных

    typedef unsigned char Byte; // Тип используется для представления пикселя (яркость пикселя, значение от 0 до 255)

    typedef std::vector<Byte> Row; // Тип используется для представления строки изображения (вектор пикселей)

    typedef std::vector<Row> Matrix; // Тип используется для представления матрицы пикселей (вектор строк)

public: // Публичные конструкторы

    // Конструктор по умолчанию
    Image();

    // Конструктор изображения с заданными размерами
    Image(const int height, const int width);

    // Конструктор изображения из буфера данных заданного типа
    Image(const int height, const int width, const void* buf, BufferType bufType);

public: // Публичные методы для работы с изображением

    // Получить ширину изображения
    int GetWidth() const;

    // Получить высоту изображения
    int GetHeight() const;

    // Получить значение пикселя по его координатам
    Byte GetPixel(const int rowNum, const int colNum) const;

    // Установить значение пикселя по его координатам
    void SetPixel(const int rowNum, const int colNum, const Byte val);

    // Получить ссылку на строку изображения
    Row& GetRow(const int rowNum);
    const Row& GetRow(const int rowNum) const;

    // Получить ссылку на строку изображения (в стиле массива)
    Row& operator[](const int rowNum);
    const Row& operator[](const int rowNum) const;

    // Получить ссылку на пиксель изображения
    Byte& operator()(const int rowNum, const int colNum);
    const Byte& operator()(const int rowNum, const int colNum) const;

    // Провеить, проинициализировано ли изображение
    // Изображение может быть не проинициализировано, если было создано с помощью конструктора по умолчанию
    // или при создании матрицы был какой-то сбой
    bool IsInitialized() const;

    // Метод проверяет некорректность координат пикселя (выходят ли за пределы изображения по ширине и высоте)
    bool IsInvalidCoordinates(const int rowNum, const int colNum);

    // Корректировка координат пикселя
    // Если координаты пикселя выходят за границы матрицы, то его координаты зеркально отражаются от ближайшей грани
    void CorrectCoordinates(int& rowNum, int& colNum) const;

    // Проверка нового значения пикселя на корретность (выход за границы возможных значений)
    static void CheckPixelValue(int& value);

private: // Закрытые методы

    // Расчет всопогательных параметров, которые используются при корректировке координат пикселя
    void CalcAuxParameters();

    // Заполнение пикселей из буфера заданного типа
    void FillPixels(const void* buf, BufferType bufType);

    // Заполнение пикселей из буфера RGB типа
    void FillPixelFromRGB(Byte* buf);

    // Заполнение пикселей из буфера типа DirectShow
    void FillPixelFromDirectShow(Byte* buf);

public: // Закрытые данные (внутреннее представление изображения)

    // Матрица пикселей
    Matrix mPixels;

    // Ширина изображения (-1, если изображение не проинициализировано)
    int mWidth;

    // Высота изображения (-1, если изображение не проинициализировано)
    int mHeight;

private: // Закрытые вспомогательные данные для быстрой корректировки пикселей, выходящих за пределы матрицы

    // Используется для корректировки координаты пикселя (x, colNum), если она больше ширины
    int mAuxWidth;

    // Используется для корректировки координаты пикселя (y, rowNum), если она больше высоты
    int mAuxHeight;

};

}

#endif // IMAGE_H
