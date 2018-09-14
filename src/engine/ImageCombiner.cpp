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

// В данном файле содержится реализация методов класса, который занимается комплексированием изображений

#include <vector>
#include <algorithm>

#include "ImageParametersCalculator.h"
#include "ImageCombiner.h"
#include "Image.h"

namespace acv {

// Класс используется для представления координат пикселя, входящего в морфологическую форму
class PixelCoordinates
{

public: // Публичные конструкторы

    PixelCoordinates(const int x, const int y);

public: // Публичные методы

    // Методы для получения и установка X и Y
    void SetX(const int x);
    void SetY(const int y);
    void SetXY(const int x, const int y);
    int GetX() const;
    int GetY() const;

private: // Закрытые данные

    // Координаты X и Y
    int mX;
    int mY;

};

// Класс используется для представления морфологической формы - набора пикселей
// с одинаковой или находящейся в одном диапазоне яркостью
// Является вектором координат пикселей, входящих в форму
class AMorphologicalForm
{

public: // Публичные методы для работы с морфологической формой

    // Метод возвращает количество пикселей в форме
    size_t size() const;

    // Оператор для обращения к пикселю по индеку
    PixelCoordinates& operator[](const size_t idx);
    const PixelCoordinates& operator[](const size_t idx) const;

    // Добавление нового пикселя
    void AddNewPixel(const int x, const int y);

    // Очистка формы
    void Clear();

    // Объединение форм (в данную форму добавлябтся пиксели из формы otherForm)
    void Merge(const AMorphologicalForm& otherForm);

    // Проверка является ли форма пустой
    bool IsEmpty() const;

private: // Закрытые данные

    // Вектор пикселей формы
    std::vector<PixelCoordinates> mPixels;

};

PixelCoordinates::PixelCoordinates(const int x, const int y)
    : mX(x), mY(y)
{ }

void PixelCoordinates::SetX(const int x)
{
    mX = x;
}

void PixelCoordinates::SetY(const int y)
{
    mY = y;
}

void PixelCoordinates::SetXY(const int x, const int y)
{
    SetX(x);
    SetY(y);
}

int PixelCoordinates::GetX() const
{
    return mX;
}

int PixelCoordinates::GetY() const
{
    return mY;
}

size_t AMorphologicalForm::size() const
{
    return mPixels.size();
}

PixelCoordinates& AMorphologicalForm::operator[](const size_t idx)
{
    return mPixels[idx];
}

const PixelCoordinates& AMorphologicalForm::operator[](const size_t idx) const
{
    return mPixels[idx];
}

void AMorphologicalForm::AddNewPixel(const int x, const int y)
{
    mPixels.push_back(PixelCoordinates(x, y));
}

void AMorphologicalForm::Clear()
{
    mPixels.clear();
}

void AMorphologicalForm::Merge(const AMorphologicalForm& otherForm)
{
    mPixels.insert(mPixels.end(), otherForm.mPixels.begin(), otherForm.mPixels.end());
}

bool AMorphologicalForm::IsEmpty() const
{
    return mPixels.empty();
}


void ImageCombiner::AddImage(const Image& img)
{
    mCombinedImages.push_back(&img);
}

void ImageCombiner::ClearImages()
{
    mCombinedImages.clear();
}

Image ImageCombiner::Combine(ImageCombiner::CombineType combineType, bool& combined, const bool needSort)
{
    combined = false;

    switch (combineType)
    {
    case ImageCombiner::CombineType::INFORM_PRIORITY:
        return InformativePriority(combined, needSort);
    case ImageCombiner::CombineType::MORPHOLOGICAL:
        return Morphological(DEFAULT_NUM_MODS, combined, needSort);
    case ImageCombiner::CombineType::LOCAL_ENTROPY:
        return LocalEntropy(combined);
    default:
        return Image();
    }
}

Image ImageCombiner::InformativePriority(bool& combined, const bool needSort)
{
    combined = CanCombine();

    if (combined)
    {
        std::vector<const Image*> sortedImages;
        if (needSort)
            FormSortedImagesArray(sortedImages);
        else
            sortedImages = mCombinedImages;

        // Базовое изображение на которое будут проецироваться остальные
        Image baseImg = *sortedImages[0];

        // Проецирование изображений на базовое
        for (size_t i = 1; i < sortedImages.size(); ++i)
        {
            const Image& projImg = *sortedImages[i];
            int Size= projImg.GetWidth() * projImg.GetHeight();

            // Расчитываем среднюю яркость проецируемого изображения
            int A = ImageParametersCalculator::CalcAverageBrightness(projImg);

            // Нахождение среднего отличия яркостей пикселей канала от средней яркости канала
            int dA = 0; // Срднее отличие

            for (auto ProjIt : projImg.mPixels)
            {
                int delta = static_cast<int>(ProjIt - A);
                //delta = (delta < 0) ? -delta : delta;
                dA += delta;
            }
            dA /= Size;

            // Установка новых значений яркостей канала базового изображения
            auto BaseIt = baseImg.mPixels.begin();

            for (auto ProjIt : projImg.mPixels)
            {
                    int delta = static_cast<int>(ProjIt - A);
                    //delta = (delta < 0) ? -delta : delta;

                    int px = static_cast<int>(*BaseIt + delta - dA);
                    px = (px < Image::MIN_PIXEL_VALUE) ? Image::MIN_PIXEL_VALUE : px;
                    px = (px > Image::MAX_PIXEL_VALUE) ? Image::MAX_PIXEL_VALUE : px;

                    *BaseIt++ = px;
            }
        }

        return baseImg;
    }

    return Image();
}

Image ImageCombiner::Morphological(const size_t numMods, bool& combined, const bool needSort)
{
    combined = CanCombine();

    if (combined)
    {
        std::vector<const Image*> sortedImages;
        if (needSort)
            FormSortedImagesArray(sortedImages);
        else
            sortedImages = mCombinedImages;

        Image baseImg = *sortedImages[0];
        std::vector<AMorphologicalForm> forms = CalcForms(baseImg, numMods);

        std::vector<Image> projections(sortedImages.size() - 1, Image(baseImg.GetHeight(), baseImg.GetWidth()));
        for (size_t imgIdx = 1; imgIdx < sortedImages.size(); ++imgIdx) // Пропускаем 1-ое изображение, т.к. оно базовое и мы проецируем на него
        {
            // В векторе хранятся средние значения яркости проецируемого значения на каждую из numMod форм базового изображения
            Image& projection = projections[imgIdx - 1];
            CalcProjectionToForms(forms, *sortedImages[imgIdx], projection);
        }

        MergeImages(baseImg, projections);

        return baseImg;
    }

    return Image();
}

Image ImageCombiner::LocalEntropy(bool& combined)
{
    const int APERTURE = 2; // Размер локальной окрестности

    combined = CanCombine();

    if (combined)
    {
        Image resImg(mCombinedImages[0]->GetHeight(), mCombinedImages[0]->GetWidth());

        for (int row = 0; row < resImg.GetHeight(); ++row)
        {
            for (int col = 0; col < resImg.GetWidth(); ++col)
            {
                // Выбор изображения с наибольшей локальной энтропией в данном пикселе
                double Emax = 0.0;
                size_t iMax = 0;
                for (size_t i = 0; i < mCombinedImages.size(); ++i)
                {
                    const Image& curImg = *mCombinedImages[i];
                    double E = ImageParametersCalculator::CalcLocalEntropy(curImg, row, col, APERTURE);
                    if (E > Emax)
                    {
                        Emax = E;
                        iMax = i;
                    }
                }

                resImg(row, col) = mCombinedImages[iMax]->GetPixel(row, col);
            }
        }

        return resImg;
    }

    return Image();
}

bool ImageCombiner::CanCombine() const
{
    bool ret = mCombinedImages.size() >= 2;

    for (size_t i = 1; ret && i < mCombinedImages.size(); ++i)
    {
        ret = ret && (mCombinedImages[i]->GetWidth() == mCombinedImages[0]->GetWidth());
        ret = ret && (mCombinedImages[i]->GetHeight() == mCombinedImages[0]->GetHeight());
    }

    return ret;
}

void ImageCombiner::FormSortedImagesArray(std::vector<const Image*>& sortedVec)
{
    // Структура, содержащая изображение и его энтропию
    struct SImageAndEntropy
    {
        const Image* img;
        double Entropy;
    };

    // Формируем вектор изображений с их энтропиями
    std::vector<SImageAndEntropy> imgEntrVec(mCombinedImages.size());
    size_t curStr = 0;
    for (const auto& img : mCombinedImages)
        imgEntrVec[curStr++] = { img, ImageParametersCalculator::CalcEntropy(*img) };

    // Сортируем вектор по возрастанию энтропии
    std::sort(imgEntrVec.begin(), imgEntrVec.end(),
              [](const SImageAndEntropy& left, const SImageAndEntropy& right)
              {
                return (left.Entropy > right.Entropy);
              });

    // Формируем результирующий вектор изображений, отсортированный по возрастанию энтропии
    sortedVec.clear();
    for (auto& str : imgEntrVec)
        sortedVec.push_back(str.img);
}

void ImageCombiner::MergeImages(Image& baseImg, const std::vector<Image>& projections)
{
    for (int row = 0; row < baseImg.GetHeight(); ++row)
    {
        for (int col = 0; col < baseImg.GetWidth(); ++col)
        {
            double baseVal = baseImg(row,col);
            double sum = baseVal;
            for (size_t i = 0; i < projections.size(); ++i)
            {
                const Image& proj = projections[i];
                sum += fabs(baseVal - proj(row,col));
            }
            baseVal = sum / (projections.size() + 1);

            baseImg(row,col) = baseVal;
        }
    }
}

std::vector<AMorphologicalForm> ImageCombiner::CalcForms(const Image& baseImg, const int numMods)
{
    Image histogramm = Segmentation(baseImg, numMods);
    std::vector<AMorphologicalForm> forms = FindForms(histogramm, numMods);
    return forms;
}

Image ImageCombiner::Segmentation(const Image& baseImg, const int numMods)
{
    Image histSeg = baseImg;

    // Шаг моды гистограммы по яркост
    Image::Byte dBrig = static_cast<Image::Byte>(Image::MAX_PIXEL_VALUE / numMods);
    while (static_cast<Image::Byte>(Image::MAX_PIXEL_VALUE / dBrig) >= numMods)
        ++dBrig;

    for (int row = 0; row < baseImg.GetHeight(); ++row)
    {
        for (int col = 0; col < baseImg.GetWidth(); ++col)
        {
            Image::Byte oldVal = baseImg(row,col);
            Image::Byte newVal = static_cast<Image::Byte>(oldVal / dBrig);
            histSeg(row,col) = newVal;
        }
    }

    return histSeg;
}

std::vector<AMorphologicalForm> ImageCombiner::FindForms(const Image& histogramm, const int numMods)
{
    // Морфологические формы
    std::vector<AMorphologicalForm> forms;

    // Элемент строки (содержит начальные и конечные координаты отрезка)
    struct ASStringElement
    {
        size_t startX;
        size_t finishX;
        int formIdx; // Индекс формы, которой принадлежит отрезок в векторе forms

        ASStringElement(size_t x1, size_t x2) : startX(x1), finishX(x2), formIdx(-1) { }

        bool IsIntersect(const ASStringElement& other) const
        {
            return ((other.startX >= startX  && other.startX <= finishX)  || // Старт предыдущего внутри текущего
                    (other.finishX >= startX && other.finishX <= finishX) || // Финиш предыдущего внутри текущего
                    (startX >= other.startX  && startX <= other.finishX)  || // Старт текущего внутри предыдущего
                    (finishX >= other.startX && finishX <= other.finishX));  // Финиш текущего внутри предыдущего
        }
    };

    for (int mod = 0; mod < numMods; ++mod)
    {
        std::vector<ASStringElement> prevStr, curStr;

        for (int y = 0; y < histogramm.GetHeight(); ++y)
        {
            curStr.clear();

            // Заполнение элементов текущей строки
            int prevVal = -1;
            for (int x = 0; x < histogramm.GetWidth(); ++x)
            {
                int curVal = static_cast<int>(histogramm(y,x));

                if ((x == 0 || prevVal != mod) && curVal == mod) // Начало нового отрезка
                    curStr.push_back(ASStringElement(x, x));
                else if (prevVal == mod && curVal == mod) // Внутри отрезка
                    curStr.back().finishX = x;

                prevVal = curVal;
            }

            // Рассматриваем возможные случаи для каждого отрезка текущей строки
            for (size_t i = 0; i < curStr.size(); ++i)
            {
                ASStringElement& curElem = curStr[i];

                // 1. Начало
                // На предыдущей строке на протяжении всего отрезка нет пикселей - значит это начало новой формы
                //
                // ----------
                // -++++++++-
                //
                bool prevEmpty = true;
                for (size_t j = 0; prevEmpty && j < prevStr.size(); ++j)
                {
                    const ASStringElement& prevElem = prevStr[j];
                    if (curElem.IsIntersect(prevElem))
                        prevEmpty = false;
                }
                if (prevEmpty) // Новая форма
                {
                    AMorphologicalForm newForm;
                    for (size_t x = curElem.startX; x <= curElem.finishX; ++x)
                        newForm.AddNewPixel(x, y);
                    curElem.formIdx = static_cast<int>(forms.size());

                    forms.push_back(newForm);
                    continue;
                }

                // 2. Продолжение, разветвление или объединение
                // Текущая строка полностью или частично покрывает одну или несколько предыдущих
                //
                //    (а)         (б)         (в)          (г)
                // --++++---   --++++++-   --++++++-   -+++--+++--
                // -+++++++-   ---++++--   -++++++--   --+++++++--
                //
                for (size_t j = 0; j < prevStr.size(); ++j)
                {
                    const ASStringElement& prevElem = prevStr[j];

                    if (curElem.IsIntersect(prevElem))
                    {
                        if (curElem.formIdx == -1) // Текущая строка никуда не добавлена - добавляем в форму от предыдущего отрезка (случаи А, Б или В)
                        {
                            AMorphologicalForm& form = forms[prevElem.formIdx];
                            for (size_t x = curElem.startX; x <= curElem.finishX; ++x)
                                form.AddNewPixel(x, y);
                            curElem.formIdx = prevElem.formIdx;
                        }
                        else // Случай Г
                        {
                            // Возможны 2 варианта:
                            // 1 - когда номера форм текущего отрезка и предыдущего совпадают,
                            // тогда текущий отрезок уже в форме, ничего делать не надо
                            // 2 - номер форм не совпадают, что обозначает что форма предыдущего отрезка
                            // является продолжением формы текущего отрезка, но ранее не была добавлена к этой форме
                            if (curElem.formIdx != prevElem.formIdx)
                            {
                                // В данном случае мы копируем все элементы формы предыдущего отрезка в форму текущего отрезка
                                AMorphologicalForm& curForm = forms[curElem.formIdx];
                                AMorphologicalForm& prevForm = forms[prevElem.formIdx];
                                curForm.Merge(prevForm);
                                prevForm.Clear(); // Не удаляем из вектора формы, чтобы не сбилась нумерация - это будет сделано позже
                            }
                        }
                    }
                }

                // 3. Конец
                // Отрезок текущей строки не покрывает предыдущий
                // Ничего не делаем
            }

            prevStr = curStr;
        }
    }

    std::vector<AMorphologicalForm> resForms;
    std::copy_if(forms.begin(), forms.end(),
                 std::back_inserter(resForms),
                 [](const AMorphologicalForm& form)
                 {
                    return !form.IsEmpty();
                 });

    return resForms;
}

void ImageCombiner::CalcProjectionToForms(const std::vector<AMorphologicalForm>& morphForm, const Image& projImg, Image& projection)
{
    for (size_t i = 0; i < morphForm.size(); ++i)
    {
        const auto& form = morphForm[i];

        // Находим среднее значение проецируемого изображения в форме
        double average = 0.0;
        for (size_t j = 0; j < form.size(); ++j)
        {
            int x = form[j].GetX();
            int y = form[j].GetY();

            Image::Byte val = projImg(y,x);
            average += static_cast<double>(val);
        }
        if (!form.IsEmpty())
            average /= form.size();

        // Устанавливаем новое значение в форме на проецируемом изображении
        Image::Byte newVal = static_cast<Image::Byte>(average);
        for (size_t j = 0; j < form.size(); ++j)
        {
            int x = form[j].GetX();
            int y = form[j].GetY();

            projection(y,x) = newVal;
        }
    }
}

}
