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

// This file contains implementations of methods of class to detect the borders of image

#include <algorithm>
#include <cstring>
#include <vector>
#include <cmath>
#include <list>

#include "BordersDetector.h"
#include "MatrixFilter.h"
#include "ImageFilter.h"

namespace acv {

bool BordersDetector::Canny(Image& img, const Image::Byte thresholdMin, const Image::Byte thresholdMax)
{
    // Gaussian blur
    MatrixFilter<int> filter(5, 159);
    filter[0][0] = 2; filter[0][1] =  4; filter[0][2] =  5; filter[0][3] =  4; filter[0][4] = 2;
    filter[1][0] = 4; filter[1][1] =  9; filter[1][2] = 12; filter[1][3] =  9; filter[1][4] = 4;
    filter[2][0] = 5; filter[2][1] = 12; filter[2][2] = 15; filter[2][3] = 12; filter[2][4] = 5;
    filter[3][0] = 4; filter[3][1] =  9; filter[3][2] = 12; filter[3][3] =  9; filter[3][4] = 4;
    filter[4][0] = 2; filter[4][1] =  4; filter[4][2] =  5; filter[4][3] =  4; filter[4][4] = 2;

    if (!MatrixFilterOperations::FastConvolutionImage<int>(img, filter))
        return false;

    // Calculation the gradients for each pixel
    Image tmpImg1(img.GetHeight(), img.GetWidth());
    Image tmpImg2(img.GetHeight(), img.GetWidth());

    bool ret = NonConvSobelH(img, tmpImg1);
    ret = ret && NonConvSobelV(img, tmpImg2);

    if (!ret)
        return false;

    std::vector<std::vector<Gradient>> gradients(img.GetHeight(), std::vector<Gradient>(img.GetWidth()));

    Image::Matrix::const_iterator it1 = tmpImg1.GetData().begin();
    Image::Matrix::const_iterator it2 = tmpImg2.GetData().begin();
    for (int row = 0; row < img.GetHeight(); ++row)
        for (int col = 0; col < img.GetWidth(); ++col)
            gradients[row][col] = Gradient(*it1++, *it2++);

    // Maximum suppression
    if (!BordersDetector::MaximumSuppression(gradients))
        return false;

    // Double threshold
    for (int row = 0; row < img.GetHeight(); ++row)
    {
        for (int col = 0; col < img.GetWidth(); ++col)
        {
            Gradient& gr = gradients[row][col];
            if (gr.abs > thresholdMax)
                gr.abs = Image::MAX_PIXEL_VALUE;
            else if (gr.abs < thresholdMin)
                gr.abs = Image::MIN_PIXEL_VALUE;
        }
    }

    // Tracing ambiguity area
    const int MAX_CLOSER_SIZE = 50;
    for (int row = 0; row < img.GetHeight(); ++row)
    {
        for (int col = 0; col < img.GetWidth(); ++col)
        {
            if (gradients[row][col].abs > Image::MIN_PIXEL_VALUE && gradients[row][col].abs < Image::MAX_PIXEL_VALUE)
            {
                std::list<Point> pixelGroup;
                int closer = 0; // Number of closing to found boundary

                pixelGroup.push_back(Point(col, row));
                gradients[row][col].abs = 0;

                for(std::list<Point>::iterator it = pixelGroup.begin(); it != pixelGroup.end(); ++it)
                    BordersDetector::AmbiguityTrace(it->GetY(), it->GetX(), img.GetHeight(), img.GetWidth(), gradients, pixelGroup, closer);

                if (closer > 0 && closer < MAX_CLOSER_SIZE)
                    for(std::list<Point>::iterator it = pixelGroup.begin(); it != pixelGroup.end(); ++it)
                        gradients[it->GetY()][it->GetX()].abs = Image::MAX_PIXEL_VALUE;
            }
        }
    }

    auto pixIt = img.GetData().begin();
    for (int row = 0; row < img.GetHeight(); ++row)
    {
        for (int col = 0; col < img.GetWidth(); ++col)
        {
            *pixIt = gradients[row][col].abs;
            ++pixIt;
        }
    }

    return true;
}

bool BordersDetector::Canny(const Image& srcImg, Image& dstImg, const Image::Byte thresholdMin, const Image::Byte thresholdMax)
{
    dstImg = srcImg;
    return Canny(dstImg, thresholdMin, thresholdMax);
}

bool BordersDetector::Sobel(Image& img)
{
    Image tmpImg(img.GetHeight(), img.GetWidth());

    bool ret = Sobel(img, tmpImg);
    if (ret)
        img = std::move(tmpImg);

    return ret;
}

void BordersDetector::FormGradientModules(const Image& horizImg, const Image& vertImg, Image& modImg)
{
    Image::Matrix::const_iterator it1 = horizImg.GetData().begin();
    Image::Matrix::const_iterator it2 = vertImg.GetData().begin();
    Image::Matrix::iterator itDst = modImg.GetData().begin();
    Image::Matrix::iterator itDstEnd = modImg.GetData().end();

    int prodBuf[Image::MAX_PIXEL_VALUE + 1][Image::MAX_PIXEL_VALUE + 1];
    for (size_t i = 0; i <= Image::MAX_PIXEL_VALUE; ++i)
        for (size_t j = 0; j <= Image::MAX_PIXEL_VALUE; ++j)
            prodBuf[i][j] = hypot(i, j);

    while (itDst != itDstEnd)
    {
        *itDst = prodBuf[*it1][*it2];

        ++itDst;
        ++it1;
        ++it2;
    }
}

bool BordersDetector::Sobel(const Image& srcImg, Image& dstImg)
{
    Image tmpImg1(srcImg.GetHeight(), srcImg.GetWidth());
    Image tmpImg2(srcImg.GetHeight(), srcImg.GetWidth());

    bool ret = NonConvSobelH(srcImg, tmpImg1);
    ret = ret && NonConvSobelV(srcImg, tmpImg2);

    if (ret)
        BordersDetector::FormGradientModules(tmpImg1, tmpImg2, dstImg);

    return ret;
}

bool BordersDetector::Scharr(Image& img)
{
    Image tmpImg(img.GetHeight(), img.GetWidth());

    bool ret = Sobel(img, tmpImg);
    if (ret)
        img = std::move(tmpImg);

    return ret;
}

bool BordersDetector::Scharr(const Image& srcImg, Image& dstImg)
{
    Image tmpImg1(srcImg.GetHeight(), srcImg.GetWidth());
    Image tmpImg2(srcImg.GetHeight(), srcImg.GetWidth());

    bool ret = ConvScharr(srcImg, tmpImg1, OperatorType::HORIZONTAL);
    ret = ret && ConvScharr(srcImg, tmpImg2, OperatorType::VERTICAL);

    if (ret)
        BordersDetector::FormGradientModules(tmpImg1, tmpImg2, dstImg);

    return ret;
}

bool BordersDetector::MaximumSuppression(std::vector<std::vector<BordersDetector::Gradient>>& gradients)
{
    int leftCol, leftRow, rightCol, rightRow;
    int leftLeftCol, leftLeftRow, rightRightCol, rightRightRow;

    int height = gradients.size();
    int width = gradients[0].size();

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            switch (gradients[row][col].arg)
            {
            case 0:
                leftCol = rightCol = col;
                leftRow = (row < (height - 1)) ? row + 1 : row - 1;
                rightRow = (row > 0) ? row - 1 : row + 1;
                leftLeftCol = rightRightCol= col;
                leftLeftRow = (row < (height - 2)) ? row + 2 : row - 2;
                rightRightRow = (row > 1) ? row - 2 : row + 2;
                break;
            case 90:
                leftRow = rightRow = row;
                leftCol = (col > 0) ? col - 1 : col + 1;
                rightCol = (col < (width - 1)) ? col + 1 : col - 1;
                leftLeftRow = rightRightRow = row;
                leftLeftCol = (col > 1) ? col - 2 : col + 2;
                rightRightCol = (col < (width - 2)) ? col + 2 : col - 2;
                break;
            case 45:
                if ((col <= 1) || (row >= (height - 2)))
                {
                    if ((col == 0) || (row == (height - 1)))
                    {
                        leftCol = rightCol = col + 1;
                        leftRow = rightRow = row - 1;
                    }
                    else
                    {
                        leftCol = col - 1; leftRow = row + 1;
                        rightCol = col + 1; rightRow = row - 1;
                    }
                    leftLeftCol = rightRightCol = col + 2;
                    leftLeftRow = rightRightRow = row - 2;
                }
                else
                {
                    if ((row <= 1) || (col >= (width - 2)))
                    {
                        if ((row == 0) || (col == (width - 1)))
                        {
                            leftCol = rightCol = col - 1;
                            leftRow = rightRow = row + 1;
                        }
                        else
                        {
                            leftCol = col - 1; leftRow = row + 1;
                            rightCol = col + 1; rightRow = row - 1;
                        }
                        leftLeftCol = rightRightCol = col - 2;
                        leftLeftRow = rightRightRow = row + 2;
                    }
                    else
                    {
                        leftCol = col - 1; leftRow = row + 1;
                        rightCol = col + 1; rightRow = row - 1;
                        leftLeftCol = col - 2; leftLeftRow = row + 2;
                        rightRightCol = col + 2; rightRightRow = row - 2;
                    }
                }
                break;
            case 135:
                if ((col <= 1) || (row <= 1))
                {
                    if ((col == 0) || (row == 0))
                    {
                        leftCol = rightCol = col + 1;
                        leftRow = rightRow = row + 1;
                    }
                    else
                    {
                        leftCol = col + 1; leftRow = row + 1;
                        rightCol = col - 1; rightRow = row - 1;
                    }
                    leftLeftCol = rightRightCol = col + 2;
                    leftLeftRow = rightRightRow = row + 2;
                }
                else
                {
                    if ((row >= height - 2) || (col >= width - 2))
                    {
                        if ((row == (height - 1)) || (col == (width - 1)))
                        {
                            leftCol = rightCol = col - 1;
                            leftRow = rightRow = row - 1;
                        }
                        else
                        {
                            leftCol = col + 1; leftRow = row + 1;
                            rightCol = col - 1; rightRow = row - 1;
                        }
                        leftLeftCol = rightRightCol = col - 2;
                        leftLeftRow = rightRightRow = row - 2;
                    }
                    else
                    {
                        leftCol = col + 1; leftRow = row + 1;
                        rightCol = col - 1; rightRow = row - 1;
                        leftLeftCol = col + 2; leftLeftRow = row + 2;
                        rightRightCol = col - 2; rightRightRow = row - 2;
                    }
                }
                break;
            default:
                return false;
            }

            if ((gradients[row][col].abs < gradients[leftRow][leftCol].abs) ||
                (gradients[row][col].abs < gradients[rightRow][rightCol].abs) ||
                (gradients[row][col].abs < gradients[leftLeftRow][leftLeftCol].abs) ||
                (gradients[row][col].abs < gradients[rightRightRow][rightRightCol].abs))
            {
                gradients[row][col].abs = 0;
            }
        }
    }

    return true;
}

// Вспомагательная функция для трассировки областей неоднозначности в алгоритме Канни
void BordersDetector::AmbiguityTrace(const int row, const int col, const int height, const int width,
                                     std::vector<std::vector<BordersDetector::Gradient>>& gradients,
                                     std::list<Point>& pixelGroup, int& closer)
{
    const int NEIGHBOR_SHIFT_X[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    const int NEIGHBOR_SHIFT_Y[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

    for (int i = 0; i < 8; ++i)
    {
        int newCol = col + NEIGHBOR_SHIFT_X[i];
        int newRow = row + NEIGHBOR_SHIFT_Y[i];

        if (newCol >= 0 && newCol < width && newRow >= 0 && newRow < height)
        {
            if (gradients[newRow][newCol].abs > Image::MIN_PIXEL_VALUE)
            {
                if (gradients[newRow][newCol].abs == Image::MAX_PIXEL_VALUE)
                    ++closer;
                else
                {
                    gradients[newRow][newCol].abs = 0;
                    pixelGroup.push_back(Point(newCol, newRow));
                }
            }
        }
    }
}

BordersDetector::Gradient::Gradient()
    : abs(0),
      arg(0)
{ }

BordersDetector::Gradient::Gradient(int horiz, int vert)
    : abs(static_cast<Image::Byte>(hypot(horiz, vert)))
{
    const double TRESHOLD_0 = 0.414, TRESHOLD_90 = 2.414;

    double b = (horiz != 0) ? fabs(static_cast<double>(vert / horiz)) : 10.0;
    if (b < TRESHOLD_0)
        arg = 0;
    else if (b > TRESHOLD_90)
        arg = 90;
    else
        arg = ((vert > 0 && horiz > 0) || (vert < 0 && horiz < 0)) ? 45 : 135;
}

bool BordersDetector::DetectBorders(Image& img, DetectorType detectorType,
                                    const Image::Byte thresholdMin /*= DEFAULT_MIN_THRESHOLD*/, const Image::Byte thresholdMax /*= DEFAULT_MAX_THRESHOLD*/)
{
    switch (detectorType)
    {
    case DetectorType::CANNY:
        return Canny(img, thresholdMin, thresholdMax);
    case DetectorType::SOBEL:
        return Sobel(img);
    case DetectorType::SCHARR:
        return Scharr(img);
    default:
        return false;
    }
}

bool BordersDetector::DetectBorders(const Image& srcImg, Image& dstImg, DetectorType detectorType,
                                    const Image::Byte thresholdMin /*= DEFAULT_MIN_THRESHOLD*/, const Image::Byte thresholdMax /*= DEFAULT_MAX_THRESHOLD*/)
{
    switch (detectorType)
    {
    case DetectorType::CANNY:
        return Canny(srcImg, dstImg, thresholdMin, thresholdMax);
    case DetectorType::SOBEL:
        return Sobel(srcImg, dstImg);
    case DetectorType::SCHARR:
        return Scharr(srcImg, dstImg);
    default:
        return false;
    }
}

bool BordersDetector::OperatorConvolution(Image& img, DetectorType detectorType, OperatorType operatorType)
{
    switch (detectorType)
    {
    case DetectorType::SOBEL:
        return NonConvSobel(img, operatorType);
    case DetectorType::SCHARR:
        return ConvScharr(img, operatorType);
    default:
        return false;
    }
}

bool BordersDetector::OperatorConvolution(const Image& srcImg, Image& dstImg, DetectorType detectorType, OperatorType operatorType)
{
    switch (detectorType)
    {
    case DetectorType::SOBEL:
        return NonConvSobel(srcImg, dstImg, operatorType);
    case DetectorType::SCHARR:
        return ConvScharr(srcImg, dstImg, operatorType);
    default:
        return false;
    }
}

bool BordersDetector::NonConvSobel(Image& img, OperatorType type)
{
    bool res = (type == OperatorType::HORIZONTAL) ? NonConvSobelH(img) : NonConvSobelV(img);
    return res;
}

bool BordersDetector::NonConvSobel(const Image& srcImg, Image& dstImg, BordersDetector::OperatorType type)
{
    bool res = (type == OperatorType::HORIZONTAL) ? NonConvSobelH(srcImg, dstImg) : NonConvSobelV(srcImg, dstImg);
    return res;
}

// Non-convolutional horizontal Sobel operator
bool BordersDetector::NonConvSobelH(Image& img)
{
    Image tmpImg(img.GetHeight(), img.GetWidth());

    NonConvSobelH(img, tmpImg);

    img = std::move(tmpImg);
    return true;
}

// Non-convolutional horizontal Sobel operator
bool BordersDetector::NonConvSobelH(const Image& srcImg, Image& dstImg)
{
    auto width = srcImg.GetWidth();
    auto height = srcImg.GetHeight();

    Image::Byte* ptrInput = const_cast<Image::Byte*>(srcImg.GetRawPointer());
    Image::Byte* ptrOutput = dstImg.GetRawPointer();

    // 1st row loop
    for (int colNum = 0; colNum < width; ++colNum, ++ptrInput, ++ptrOutput)
        *ptrOutput = 0;

    // Main loop
    for (int rowNum = 1; rowNum < height - 1; ++rowNum)
    {
         // 1-st element in row
        int res = (*(ptrInput - width) << 1) + (*(ptrInput - width + 1) << 1) -
                  (*(ptrInput + width) << 1) - (*(ptrInput + width + 1) << 1);

        Image::CheckPixelValue(res);
        *ptrOutput++ = static_cast<Image::Byte>(res);
        ++ptrInput;

        for (int colNum = 1; colNum < width - 1; ++colNum, ++ptrInput, ++ptrOutput)
        {
            res = *(ptrInput - width - 1) + (*(ptrInput - width) << 1) + *(ptrInput - width + 1) -
                  *(ptrInput + width - 1) - (*(ptrInput + width) << 1) - *(ptrInput + width + 1);

            Image::CheckPixelValue(res);
            *ptrOutput = static_cast<Image::Byte>(res);
        }

        // last element in row
        res = (*(ptrInput - width - 1) << 1) + (*(ptrInput - width) << 1) -
              (*(ptrInput + width - 1) << 1) - (*(ptrInput + width) << 1);

        Image::CheckPixelValue(res);
        *ptrOutput++ = static_cast<Image::Byte>(res);
         ++ptrInput;
    }

    // last row loop
    for (int colNum = 0; colNum < width; ++colNum, ++ptrInput, ++ptrOutput)
        *ptrOutput = 0;

    return true;
}

// Non-convolutional vertical Sobel operator
bool BordersDetector::NonConvSobelV(Image& img)
{
    Image tmpImg(img.GetHeight(), img.GetWidth());

    NonConvSobelV(img, tmpImg);

    img = std::move(tmpImg);
    return true;
}

// Non-convolutional vertical Sobel operator
bool BordersDetector::NonConvSobelV(const Image& srcImg, Image& dstImg)
{
    auto width = srcImg.GetWidth();
    auto height = srcImg.GetHeight();

    Image::Byte* ptrInput = const_cast<Image::Byte*>(srcImg.GetRawPointer());
    Image::Byte* ptrOutput = dstImg.GetRawPointer();

    // 1st row loop
    *ptrOutput++ = 0;
    ++ptrInput;
    for (int colNum = 1; colNum < width - 1; ++colNum, ++ptrInput, ++ptrOutput)
    {
        int res = (*(ptrInput - 1) << 1) - (*(ptrInput + 1) << 1) +
                  (*(ptrInput + width - 1) << 1) - (*(ptrInput + width + 1) << 1);

        Image::CheckPixelValue(res);
        *ptrOutput = static_cast<Image::Byte>(res);
    }
    *ptrOutput++ = 0;
     ++ptrInput;

    // Main loop
    for (int rowNum = 1; rowNum < height - 1; ++rowNum)
    {
         // 1-st element in row
        *ptrOutput++= 0;
        ++ptrInput;

        for (int colNum = 1; colNum < width - 1; ++colNum, ++ptrInput, ++ptrOutput)
        {
            int res = *(ptrInput - width - 1)   - *(ptrInput - width + 1) +
                      (*(ptrInput - 1) << 1) - (*(ptrInput + 1) << 1) +
                      *(ptrInput + width - 1)  - *(ptrInput + width + 1);

            Image::CheckPixelValue(res);
            *ptrOutput = static_cast<Image::Byte>(res);
        }

        // last element in row
        *ptrOutput++ = 0;
         ++ptrInput;
    }

    // last row loop
    *ptrOutput++ = 0;
    ++ptrInput;
    for (int colNum = 1; colNum < width - 1; ++colNum, ++ptrInput, ++ptrOutput)
    {
        int res = (*(ptrInput - width - 1) << 1) - (*(ptrInput - width + 1) << 1) +
                  (*(ptrInput - 1) << 1) - (*(ptrInput + 1) << 1);

        Image::CheckPixelValue(res);
        *ptrOutput = static_cast<Image::Byte>(res);
    }
    *ptrOutput++ = 0;
     ++ptrInput;

    return true;
}

bool BordersDetector::ConvScharr(Image& img, OperatorType type)
{
    MatrixFilter<int> filter(3, 1);

    if (type == OperatorType::HORIZONTAL)
    {
        filter[0][0] =  3;  filter[0][1] =  10;  filter[0][2] =  3;
        filter[1][0] =  0;  filter[1][1] =   0;  filter[1][2] =  0;
        filter[2][0] = -3;  filter[2][1] = -10;  filter[2][2] = -3;
    }
    else if (type == OperatorType::VERTICAL)
    {
        filter[0][0] =  3;  filter[0][1] =  0;  filter[0][2] =  -3;
        filter[1][0] = 10;  filter[1][1] =  0;  filter[1][2] = -10;
        filter[2][0] =  3;  filter[2][1] =  0;  filter[2][2] =  -3;
    }
    else
        return false;

    return MatrixFilterOperations::FastConvolutionImage<int>(img, filter);
}

bool BordersDetector::ConvScharr(const Image& srcImg, Image& dstImg, BordersDetector::OperatorType type)
{
    memcpy(dstImg.GetRawPointer(), srcImg.GetRawPointer(), srcImg.GetHeight() * srcImg.GetWidth());
    bool ret = ConvScharr(dstImg, type);
    return ret;
}

}
