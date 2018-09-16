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

// This file contains implementations of methods of class to detect the borders of image

#include <algorithm>
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

    if (!MatrixFilterOperations::ConvolutionImage<int>(img, filter))
        return false;

    // Calculation the gradients for each pixel
    MatrixFilter<int> hMask(3, 1);
    hMask[0][0] =  1; hMask[0][1] =  2; hMask[0][2] =  1;
    hMask[1][0] =  0; hMask[1][1] =  0; hMask[1][2] =  0;
    hMask[2][0] = -1; hMask[2][1] = -2; hMask[2][2] = -1;

    MatrixFilter<int> vMask(3, 1);
    vMask[0][0] = 1;  vMask[0][1] = 0; vMask[0][2] = -1;
    vMask[1][0] = 2;  vMask[1][1] = 0; vMask[1][2] = -2;
    vMask[2][0] = 1;  vMask[2][1] = 0; vMask[2][2] = -1;

    std::vector<std::vector<Gradient>> gradients(img.GetHeight(), std::vector<Gradient>(img.GetWidth()));

    for (int row = 0; row < img.GetHeight(); ++row)
    {
        for (int col = 0; col < img.GetWidth(); ++col)
        {
            int vConv = MatrixFilterOperations::ConvolutionPixel<int>(img, row, col, vMask, 1);
            int hConv = MatrixFilterOperations::ConvolutionPixel<int>(img, row, col, hMask, 1);            
            gradients[row][col] = Gradient(hConv, vConv);
        }
    }

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

}
