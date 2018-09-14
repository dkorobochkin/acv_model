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

// This file contains implementations of class to combine of images

#include <vector>
#include <algorithm>

#include "ImageParametersCalculator.h"
#include "ImageCombiner.h"
#include "Image.h"

namespace acv {

// This class represents pixel in morphological form
class PixelCoordinates
{

public: // Constructors

    PixelCoordinates(const int x, const int y);

public: // Public methods

    // Methods to get and set of pixel coordinates
    void SetX(const int x);
    void SetY(const int y);
    void SetXY(const int x, const int y);
    int GetX() const;
    int GetY() const;

private: // Private members

    // Coordinates
    int mX;
    int mY;

};

// This class represent the morphological form
class AMorphologicalForm
{

public: // Public methods

    // Return the number of pixels in the morphological form
    size_t size() const;

    // Operator [] (array style)
    PixelCoordinates& operator[](const size_t idx);
    const PixelCoordinates& operator[](const size_t idx) const;

    // Add new pixel
    void AddNewPixel(const int x, const int y);

    // Clear the vector of form pixels
    void Clear();

    // Merge the two forms. To this form will be added the pixels from other form
    void Merge(const AMorphologicalForm& otherForm);

    // Return "true" if the form is empty
    bool IsEmpty() const;

private: // Private members

    // Vector of the pixels in the form
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

        // Basic image. To this image will be projected other images
        Image baseImg = *sortedImages[0];

        // Projection images to basic image
        for (size_t i = 1; i < sortedImages.size(); ++i)
        {
            const Image& projImg = *sortedImages[i];
            int size = projImg.GetWidth() * projImg.GetHeight();

            // Calculation the average difference from average brightness
            int A = ImageParametersCalculator::CalcAverageBrightness(projImg);

            // Calculation the average difference from average brightness
            int dA = 0;
            for (auto projPix : projImg.mPixels)
            {
                int delta = static_cast<int>(projPix - A);
                dA += delta;
            }
            dA /= size;

            auto baseIt = baseImg.mPixels.begin();
            for (auto projPix : projImg.mPixels)
            {
                    int delta = static_cast<int>(projPix - A);

                    int px = static_cast<int>(*baseIt + delta - dA);
                    Image::CheckPixelValue(px);

                    *baseIt++ = px;
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
        for (size_t imgIdx = 1; imgIdx < sortedImages.size(); ++imgIdx) // First image is basic
        {
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
    const int APERTURE = 2;

    combined = CanCombine();

    if (combined)
    {
        Image resImg(mCombinedImages[0]->GetHeight(), mCombinedImages[0]->GetWidth());

        for (int row = 0; row < resImg.GetHeight(); ++row)
        {
            for (int col = 0; col < resImg.GetWidth(); ++col)
            {
                // Select the pixel with maximum entropy
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
    // Image and his entropy
    struct SImageAndEntropy
    {
        const Image* img;
        double entropy;
    };

    // Form the vector of images with their entropies
    std::vector<SImageAndEntropy> imgEntrVec(mCombinedImages.size());
    size_t curStr = 0;
    for (const auto& img : mCombinedImages)
        imgEntrVec[curStr++] = { img, ImageParametersCalculator::CalcEntropy(*img) };

    // Sort ascending the vector by their entropy
    std::sort(imgEntrVec.begin(), imgEntrVec.end(),
              [](const SImageAndEntropy& left, const SImageAndEntropy& right)
              {
                return (left.entropy > right.entropy);
              });

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
            double baseVal = baseImg(row, col);
            double sum = baseVal;
            for (size_t i = 0; i < projections.size(); ++i)
            {
                const Image& proj = projections[i];
                sum += fabs(baseVal - proj(row, col));
            }
            baseVal = sum / (projections.size() + 1);

            baseImg(row, col) = baseVal;
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
            Image::Byte oldVal = baseImg(row, col);
            Image::Byte newVal = static_cast<Image::Byte>(oldVal / dBrig);
            histSeg(row, col) = newVal;
        }
    }

    return histSeg;
}

std::vector<AMorphologicalForm> ImageCombiner::FindForms(const Image& histogramm, const int numMods)
{
    std::vector<AMorphologicalForm> forms;

    // Element of the row (contains the start and finish coordinates (X) of element on the row)
    struct RowElement
    {
        int startX;
        int finishX;
        int formIdx; // Index of the form to which the row element belong

        RowElement(int x1, int x2) : startX(x1), finishX(x2), formIdx(-1) { }

        // Check the intersection of row element with row element on next or previous row
        bool IsIntersect(const RowElement& other) const
        {
            return ((other.startX >= startX  && other.startX <= finishX)  ||
                    (other.finishX >= startX && other.finishX <= finishX) ||
                    (startX >= other.startX  && startX <= other.finishX)  ||
                    (finishX >= other.startX && finishX <= other.finishX));
        }
    };

    for (int mod = 0; mod < numMods; ++mod)
    {
        std::vector<RowElement> prevStr, curStr;

        for (int y = 0; y < histogramm.GetHeight(); ++y)
        {
            curStr.clear();

            // Fill the current row
            int prevVal = -1;
            for (int x = 0; x < histogramm.GetWidth(); ++x)
            {
                int curVal = static_cast<int>(histogramm(y, x));

                if ((x == 0 || prevVal != mod) && curVal == mod) // Start of new row element
                    curStr.push_back(RowElement(x, x));
                else if (prevVal == mod && curVal == mod) // Inside the segment
                    curStr.back().finishX = x;

                prevVal = curVal;
            }

            // Consider the all cases for each element on current row
            for (size_t i = 0; i < curStr.size(); ++i)
            {
                RowElement& curElem = curStr[i];

                // 1. Begin
                // Previous row have not pixels on the considered range of X coordinates
                //
                // ----------
                // -++++++++-
                //
                bool prevEmpty = true;
                for (size_t j = 0; prevEmpty && j < prevStr.size(); ++j)
                {
                    const RowElement& prevElem = prevStr[j];
                    if (curElem.IsIntersect(prevElem))
                        prevEmpty = false;
                }
                if (prevEmpty) // New form
                {
                    AMorphologicalForm newForm;
                    for (int x = curElem.startX; x <= curElem.finishX; ++x)
                        newForm.AddNewPixel(x, y);
                    curElem.formIdx = static_cast<int>(forms.size());

                    forms.push_back(newForm);
                    continue;
                }

                // 2. Continuation, branching or merger
                // Current row element covers one or several row element on previous row full or partuially
                //
                //    (a)         (b)         (c)          (d)
                // --++++---   --++++++-   --++++++-   -+++--+++--
                // -+++++++-   ---++++--   -++++++--   --+++++++--
                //
                for (size_t j = 0; j < prevStr.size(); ++j)
                {
                    const RowElement& prevElem = prevStr[j];

                    if (curElem.IsIntersect(prevElem))
                    {
                        // Current row element doesn't belong to form - he will be added to form of element from previous row (A, B, C cases)
                        if (curElem.formIdx == -1)
                        {
                            AMorphologicalForm& form = forms[prevElem.formIdx];
                            for (int x = curElem.startX; x <= curElem.finishX; ++x)
                                form.AddNewPixel(x, y);
                            curElem.formIdx = prevElem.formIdx;
                        }
                        else // case D
                        {
                            // There are 2 cases:
                            // indexes of forms of elements from current and previous rows is same,
                            // then current element already in form, nothing to do
                            // 2nd - indexes of forms of elements from current and previous rows is not same.
                            // It's mean that current element is continuation of the form of previous element but was not added before
                            if (curElem.formIdx != prevElem.formIdx)
                            {
                                AMorphologicalForm& curForm = forms[curElem.formIdx];
                                AMorphologicalForm& prevForm = forms[prevElem.formIdx];
                                curForm.Merge(prevForm);
                                prevForm.Clear(); // Don't erase the empty form!!! Otherwise the indexes will be incorrect
                            }
                        }
                    }
                }

                // 3. End
                // Current row have not pixels on the considered range of X coordinates from previous row
                // It's the end of the form
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

        // Calculation the average brightness of projected image in the form
        double average = 0.0;
        for (size_t j = 0; j < form.size(); ++j)
        {
            int x = form[j].GetX();
            int y = form[j].GetY();

            Image::Byte val = projImg(y, x);
            average += static_cast<double>(val);
        }
        if (!form.IsEmpty())
            average /= form.size();

        Image::Byte newVal = static_cast<Image::Byte>(average);
        for (size_t j = 0; j < form.size(); ++j)
        {
            int x = form[j].GetX();
            int y = form[j].GetY();

            projection(y, x) = newVal;
        }
    }
}

}
