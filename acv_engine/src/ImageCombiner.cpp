//
// MIT License
//
// Copyright (c) 2018-2019 Dmitriy Korobochkin, Vitaliy Garmash.
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
#include <cstring>
#include <algorithm>

#include "ImageParametersCalculator.h"
#include "ImageCombiner.h"
#include "Image.h"
#include "Point.h"

namespace acv {

// This class represent the morphological form
class AMorphologicalForm
{

public: // Public methods

    // Return the number of pixels in the morphological form
    size_t size() const { return mPixels.size(); }

    // Operator [] (array style)
    Point& operator[](const size_t idx) { return mPixels[idx]; }
    const Point& operator[](const size_t idx) const { return mPixels[idx]; }

    // Add new pixel
    void AddNewPixel(const int x, const int y) { mPixels.push_back(Point(x, y)); }

    // Clear the vector of form pixels
    void Clear() { mPixels.clear(); }

    // Merge the two forms. To this form will be added the pixels from other form
    void Merge(const AMorphologicalForm& otherForm) { mPixels.insert(mPixels.end(), otherForm.mPixels.begin(), otherForm.mPixels.end()); }

    // Return "true" if the form is empty
    bool IsEmpty() const { return mPixels.empty(); }

private: // Private members

    // Vector of the pixels in the form
    std::vector<Point> mPixels;

};


void ImageCombiner::AddImage(const Image& img)
{
    mCombinedImages.push_back(&img);
}

void ImageCombiner::ClearImages()
{
    mCombinedImages.clear();
}

Image ImageCombiner::Combine(CombineType combineType, CombinationResult& combRes, const bool needSort/* = true*/)
{
    combRes = CombinationResult::INCORRECT_COMBINER_TYPE;

    switch (combineType)
    {
    case ImageCombiner::CombineType::INFORM_PRIORITY:
        return InformativePriority(combRes, needSort);
    case ImageCombiner::CombineType::MORPHOLOGICAL:
        return Morphological(DEFAULT_NUM_MODS, combRes, needSort);
    case ImageCombiner::CombineType::LOCAL_ENTROPY:
        return LocalEntropy(combRes);
    case ImageCombiner::CombineType::DIFFERENCES_ADDING:
        return DifferencesAdding(combRes, needSort);
    case ImageCombiner::CombineType::CALC_DIFF:
        return CalcDiff(combRes);
    default:
        return Image();
    }
}

CombinationResult ImageCombiner::Combine(CombineType combineType, Image& combImg, const bool needSort/* = true*/)
{
    CombinationResult combRes = CombinationResult::INCORRECT_COMBINER_TYPE;

    switch (combineType)
    {
    case ImageCombiner::CombineType::INFORM_PRIORITY:
        return InformativePriority(combImg, needSort);
    case ImageCombiner::CombineType::MORPHOLOGICAL:
        return Morphological(DEFAULT_NUM_MODS, combImg, needSort);
    case ImageCombiner::CombineType::LOCAL_ENTROPY:
        return LocalEntropy(combImg);
    case ImageCombiner::CombineType::DIFFERENCES_ADDING:
        return DifferencesAdding(combImg, needSort);
    case ImageCombiner::CombineType::CALC_DIFF:
        return CalcDiff(combImg);

    }

    return combRes;
}

Image ImageCombiner::InformativePriority(CombinationResult& combRes, const bool needSort/* = true*/)
{
    Image combImg(mCombinedImages[0]->GetHeight(), mCombinedImages[0]->GetWidth());
    combRes = InformativePriority(combImg, needSort);

    return (combRes == CombinationResult::SUCCESS) ? combImg : Image();
}

CombinationResult ImageCombiner::InformativePriority(Image& combImg, const bool needSort/* = true*/)
{
    CombinationResult combRes;

    if (CanCombine(combRes))
    {
        std::vector<const Image*> sortedImages;
        if (needSort)
            FormSortedImagesArray(sortedImages);
        else
            sortedImages = mCombinedImages;

        // Basic image. To this image will be projected other images
        memcpy(combImg.GetRawPointer(), sortedImages[0]->GetRawPointer(), sortedImages[0]->GetHeight() * sortedImages[0]->GetWidth());

        // Projection images to basic image
        for (size_t i = 1; i < sortedImages.size(); ++i)
        {
            const Image& projImg = *sortedImages[i];

            // Calculation the average difference from average brightness
            int A = ImageParametersCalculator::CalcAverageBrightness(projImg);

            // Calculation the average difference from average brightness
            int dA = 0;
            for (auto projPix : projImg.GetData())
            {
                int delta = static_cast<int>(projPix - A);
                dA += delta;
            }
            dA /= projImg.GetWidth() * projImg.GetHeight();

            auto baseIt = combImg.GetData().begin();
            for (auto projPix : projImg.GetData())
            {
                int delta = static_cast<int>(projPix - A);

                int px = static_cast<int>(*baseIt + delta - dA);
                Image::CheckPixelValue(px);

                *baseIt++ = px;
            }
        }

        combRes = CombinationResult::SUCCESS;
    }

    return combRes;
}

Image ImageCombiner::Morphological(const size_t numMods, CombinationResult& combRes, const bool needSort/* = true*/)
{
    Image combImg(mCombinedImages[0]->GetHeight(), mCombinedImages[0]->GetWidth());
    combRes = Morphological(numMods, combImg, needSort);

    return (combRes == CombinationResult::SUCCESS) ? combImg : Image();
}

CombinationResult ImageCombiner::Morphological(const size_t numMods, Image& combImg, const bool needSort/* = true*/)
{
    CombinationResult combRes;

    if (CanCombine(combRes))
    {
        std::vector<const Image*> sortedImages;
        if (needSort)
            FormSortedImagesArray(sortedImages);
        else
            sortedImages = mCombinedImages;

        memcpy(combImg.GetRawPointer(), sortedImages[0]->GetRawPointer(), sortedImages[0]->GetHeight() * sortedImages[0]->GetWidth());

        std::vector<AMorphologicalForm> forms = CalcForms(combImg, numMods);

        std::vector<Image> projections(sortedImages.size() - 1, Image(combImg.GetHeight(), combImg.GetWidth()));
        for (size_t imgIdx = 1; imgIdx < sortedImages.size(); ++imgIdx) // First image is basic
        {
            Image& projection = projections[imgIdx - 1];
            CalcProjectionToForms(forms, *sortedImages[imgIdx], projection);
        }

        MergeImages(combImg, projections);

        combRes = CombinationResult::SUCCESS;
    }

    return combRes;
}

Image ImageCombiner::LocalEntropy(CombinationResult& combRes)
{
    Image combImg(mCombinedImages[0]->GetHeight(), mCombinedImages[0]->GetWidth());
    combRes = LocalEntropy(combImg);

    return (combRes == CombinationResult::SUCCESS) ? combImg : Image();
}

CombinationResult ImageCombiner::LocalEntropy(Image& combImg)
{
    const int APERTURE = 2;

    CombinationResult combRes;

    if (CanCombine(combRes))
    {
        for (int row = 0; row < combImg.GetHeight(); ++row)
        {
            for (int col = 0; col < combImg.GetWidth(); ++col)
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

                combImg(row, col) = mCombinedImages[iMax]->GetPixel(row, col);
            }
        }

        combRes = CombinationResult::SUCCESS;
    }

    return combRes;
}

Image ImageCombiner::DifferencesAdding(CombinationResult& combRes, const bool needSort/* = true*/)
{
    Image combImg(mCombinedImages[0]->GetHeight(), mCombinedImages[0]->GetWidth());
    combRes = DifferencesAdding(combImg, needSort);

    return (combRes == CombinationResult::SUCCESS) ? combImg : Image();
}

CombinationResult ImageCombiner::DifferencesAdding(Image& combImg, const bool needSort/* = true*/)
{
    CombinationResult combRes;

    if (mCombinedImages.size() != 2)
      return CombinationResult::MANY_IMAGES;

    if (CanCombine(combRes))
    {
        std::vector<const Image*> sortedImages;
        if (needSort)
            FormSortedImagesArray(sortedImages);
        else
            sortedImages = mCombinedImages;

        Image::Byte Dmin = Image::MAX_PIXEL_VALUE, Dmax = Image::MIN_PIXEL_VALUE;

        // Search of minimum and maximum of brightness differences
        Image::Matrix::const_iterator it1, it2;
        Image::Matrix::iterator itDst;
        for (it1 = sortedImages[0]->GetData().cbegin(), it2 = sortedImages[1]->GetData().cbegin(), itDst = combImg.GetData().begin();
             it1 != sortedImages[0]->GetData().cend();
             ++it1, ++it2, ++itDst)
        {
            Image::Byte D = (*it1 >= *it2) ? (*it1 - *it2) : (*it2 - *it1);

            if (D < Dmin)
                Dmin = D;
            if (D > Dmax)
                Dmax = D;

            *itDst = D;
        }

        double k1 = (Dmax + 3.0 * Dmin) / 1020.0;
        double k2 = (3.0 * Dmax + Dmin) / 1020.0;

        Image::Byte b1 = Dmin + k1 * (Dmax - Dmin);
        Image::Byte b2 = Dmin + k2 * (Dmax - Dmin);
        Image::Byte db = b2 - b1;

        for (it1 = sortedImages[0]->GetData().cbegin(), it2 = sortedImages[1]->GetData().cbegin(), itDst = combImg.GetData().begin();
             it1 != sortedImages[0]->GetData().cend();
             ++it1, ++it2, ++itDst)
        {
            if (*itDst <= b1)
                *itDst = *it1;
            else if (*itDst >= b2)
                *itDst = *it2;
            else
                *itDst = *it1 + (b1 - *itDst) * (*it1 - *it2) / db;
        }

        combRes = CombinationResult::SUCCESS;
    }

    return combRes;
}

Image ImageCombiner::CalcDiff(CombinationResult &combRes)
{
    Image combImg(mCombinedImages[0]->GetHeight(), mCombinedImages[0]->GetWidth());
    combRes = CalcDiff(combImg);

    return (combRes == CombinationResult::SUCCESS) ? combImg : Image();
}

CombinationResult ImageCombiner::CalcDiff(Image& combImg)
{
    CombinationResult combRes;

    if (mCombinedImages.size() != 2)
      return CombinationResult::MANY_IMAGES;

    if (CanCombine(combRes))
    {
        combImg = (*mCombinedImages[0] - *mCombinedImages[1]);
        combRes = CombinationResult::SUCCESS;
    }

    return combRes;
}

bool ImageCombiner::CanCombine(CombinationResult& combRes) const
{
    bool ret = mCombinedImages.size() >= 2;

    if (!ret)
    {
        combRes = CombinationResult::FEW_IMAGES;
        return ret;
    }

    for (size_t i = 1; ret && i < mCombinedImages.size(); ++i)
    {
        ret = ret && (mCombinedImages[i]->GetWidth() == mCombinedImages[0]->GetWidth());
        ret = ret && (mCombinedImages[i]->GetHeight() == mCombinedImages[0]->GetHeight());
    }

    if (!ret)
    {
        combRes = CombinationResult::NOT_SAME_IMAGES;
        return ret;
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
    std::vector<Image::Matrix::const_iterator> projectionsIts(projections.size());
    for (size_t i = 0; i < projections.size(); ++i)
        projectionsIts[i] = projections[i].GetData().begin();

    for (auto& basePix : baseImg.GetData())
    {
        double baseVal = basePix;
        double sum = baseVal;
        for (size_t i = 0; i < projectionsIts.size(); ++i)
        {
            sum += fabs(baseVal - *projectionsIts[i]);
            ++projectionsIts[i];
        }
        baseVal = sum / (projectionsIts.size() + 1);

        basePix = baseVal;
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
    Image histSeg(baseImg.GetHeight(), baseImg.GetWidth());

    // Mod step by brightness
    Image::Byte dBrig = static_cast<Image::Byte>(Image::MAX_PIXEL_VALUE / numMods);
    while (static_cast<Image::Byte>(Image::MAX_PIXEL_VALUE / dBrig) >= numMods)
        ++dBrig;

    Image::Byte* pDstPix = histSeg.GetRawPointer();
    for (auto srcPix : baseImg.GetData())
        *pDstPix++ = static_cast<Image::Byte>(srcPix / dBrig);

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

        auto histPixIt = histogramm.GetData().begin();
        for (int y = 0; y < histogramm.GetHeight(); ++y)
        {
            curStr.clear();

            // Fill the current row
            int prevVal = -1;
            for (int x = 0; x < histogramm.GetWidth(); ++x, ++histPixIt)
            {
                int curVal = static_cast<int>(*histPixIt);

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
    resForms.reserve(forms.size());
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
