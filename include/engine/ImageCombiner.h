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

// В данном заголовочном файле содержится описание класса, который занимается комплексированием изображений

#ifndef IMAGE_COMBINER_H
#define IMAGE_COMBINER_H

#include <vector>

namespace acv {

class Image;
class AMorphologicalForm;

// Класс используется для комплексирования изображения различными методами
class ImageCombiner
{

public: // Публичные вспогательные типы

    // Допступные типы комплексирования
    enum class CombineType
    {
        INFORM_PRIORITY, // Комплексирование с приоритетом наиболее информативного изображения
        MORPHOLOGICAL, // Морфологическое комплексирование
        LOCAL_ENTROPY // Локально-энтропийное комплексирование
    };

public: // Публичные методы

    // Метод добавляет изображение для комбинирования
    void AddImage(const Image& img);

    // Метод очищает контейнер с изображениями, использующимися для комплексирования
    void ClearImages();

    // Метод производит комплексирование изображений заданным методом
    // Флаг needSort=true говорит о том, что изображения необходимо ранжировать по информативности для выбора в качестве базового
    Image Combine(CombineType combineType, bool& combined, const bool needSort = true);

private: // Закрытые методы

    // Метод совмещения изображений с приоритетом наиболее информативного изображения
    // Флаг needSort=true говорит о том, что необходимо ранжировать изображения по информативности, иначе первое переданное - базовое
    Image InformativePriority(bool& combined, const bool needSort = true);

    // Морфологический метод совмещения изображений
    // Флаг needSort=true говорит о том, что необходимо ранжировать изображения по информативности, иначе первое переданное - базовое
    Image Morphological(const size_t numMods, bool& combined, const bool needSort = true);

    // Метод производит локально-энтропийное комплексирование
    // Его суть заключается в том, что пиксель комплексированного изображения берется из того изображения,
    // локальная энтропия в пикселе которого самая большая
    Image LocalEntropy(bool& combined);

    // Метод для проверки изображений на возможность комплексирования
    // Для комбинирования должны быть одинаковыми ширина и высота изображения
    bool CanCombine() const;

    // Метод формирует вектор изображений, которые будут комплексироваться,
    // причем вектор упорядочен по возрастанию информативности изображения (оценка по энтропии)
    void FormSortedImagesArray(std::vector<const Image*>& sortedVec);

private: // Закрытые методы для морфологического комплексирования

    // Метод расчета морфологических форм
    // На первом этапе проводится критериальная гистограммная сегментация с заданным количеством мод гистограммы
    // На втором этапе производится поиск форм на полученной гистограмме
    std::vector<AMorphologicalForm> CalcForms(const Image& baseImg, const int numMods);

    // Критериальная гистограмная сегментация
    // В результате получаем матрицу, значения пикселей которой равны номеру моды гистограммы
    Image Segmentation(const Image& baseImg, const int numMods);

    // Поиск замкнутых форм и их внутренностей проводится с помощью сканирующего алгоритма
    std::vector<AMorphologicalForm> FindForms(const Image& histogramm, const int numMods);

    // Метод получения средних значений яркостей проецируемого изображения на формах базового
    void CalcProjectionToForms(const std::vector<AMorphologicalForm>& morphForm, const Image& projImg, Image& projection);

    // Метод объединяет изображения усреднением суммы яркостей от каждого из изображения
    // Для каждой из проекций используется модуль разности базовой яркости и усредненной в форме яркости
    void MergeImages(Image& baseImg, const std::vector<Image>& projections);

private: // Закрытые данные

    // Комбинируемые изображения
    std::vector<const Image*> mCombinedImages;

private: // Константы

    enum
    {
        DEFAULT_NUM_MODS = 16 // Количество мод для морфологического комплексирования по умолчанию
    };

};

}

#endif // IMAGE_COMBINER_H
