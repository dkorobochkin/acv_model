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

// This file contains definition and implementation of class "ImageTests" and his methods

#include <QString>
#include <QtTest>

#include <vector>

#include "Image.h"

// This class is used for testing of class Image
class ImageTests : public QObject
{
    Q_OBJECT

public:
    ImageTests();

private Q_SLOTS:

    // Test of default constructor
    void DefaultConstructor();

    // Test of constructor with dimensions parameters
    void ConstructorWithDimensions();

    // Test of constructor from buffer
    void ConstructorFromBuffer();

    // Test of methods for getting of dimensions
    void GetDimensions();

    // Test of methods for setting and getting of pixels
    void SetGetPixel();

    // Test of methods for checking of invalid coordinates
    void IsInvalidCoordinates();

    // Test of method for correcting of coordinates
    void CorrectCoordinates();

    // Test of static method for checking of pixel value
    void CheckPixelValue();

    // Test of method for getting the row pointer to the element of pixel vector
    void GetRawPointer();

    // Test of method for resizing of image
    void Resize();

    // Test of method for upscaling of image
    void Upscale();

};

ImageTests::ImageTests()
{
}

void ImageTests::DefaultConstructor()
{
    acv::Image img;

    QCOMPARE(img.IsInitialized(), false);

    QCOMPARE(img.GetHeight(), -1);
    QCOMPARE(img.GetWidth(), -1);

    QCOMPARE(img.GetData().empty(), true);
    QCOMPARE(img.GetRawPointer(), nullptr);
}

void ImageTests::ConstructorWithDimensions()
{
    acv::Image img(4, 3);

    QCOMPARE(img.IsInitialized(), true);

    QCOMPARE(img.GetHeight(), 4);
    QCOMPARE(img.GetWidth(), 3);

    for (acv::Image::Byte pixel : img.GetData())
        QCOMPARE(pixel, acv::Image::MIN_PIXEL_VALUE);
}

void ImageTests::ConstructorFromBuffer()
{
    const int NUM_ROWS = 4, NUM_COLS = 3;

    acv::Image::Byte val = 128;
    std::vector<acv::Image::Byte> buffer(NUM_ROWS * NUM_COLS, val);

    acv::Image img1(NUM_ROWS, NUM_COLS, &buffer[0], acv::Image::BufferType::DIRECT_SHOW);
    for (int row = 0; row < NUM_ROWS; ++row)
        for (int col = 0; col < NUM_COLS; ++col)
            QCOMPARE(img1.GetPixel(row, col), val);

    buffer.clear();
    val = 0;
    for (int row = 0; row < NUM_ROWS; ++row)
        for (int col = 0; col < NUM_COLS; ++col)
        {
            for (acv::Image::Byte i = 0; i < 3; ++i)
                buffer.push_back(val + i);
            ++val;
        }

    acv::Image img2(NUM_ROWS, NUM_COLS, &buffer[0], acv::Image::BufferType::RGB);
    val = 1;
    for (int row = 0; row < NUM_ROWS; ++row)
        for (int col = 0; col < NUM_COLS; ++col)
            QCOMPARE(img2.GetPixel(row, col), val++);
}

void ImageTests::GetDimensions()
{
    acv::Image img(480, 640);

    QCOMPARE(img.GetHeight(), 480);
    QCOMPARE(img.GetWidth(), 640);
}

void ImageTests::SetGetPixel()
{
    acv::Image img(10, 20);

    acv::Image::Byte pixel = 0;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 20; ++j)
            img.SetPixel(i, j, pixel++);

    pixel = 0;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 20; ++j)
            QCOMPARE(img.GetPixel(i, j), pixel++);
}

void ImageTests::IsInvalidCoordinates()
{
    const int NUM_ROWS = 10, NUM_COLS = 20;

    acv::Image img(NUM_ROWS, NUM_COLS);

    QCOMPARE(img.IsInvalidCoordinates(0, 0), false);
    QCOMPARE(img.IsInvalidCoordinates(NUM_ROWS - 1, 0), false);
    QCOMPARE(img.IsInvalidCoordinates(0, NUM_COLS - 1), false);
    QCOMPARE(img.IsInvalidCoordinates(NUM_ROWS - 1, NUM_COLS - 1), false);
    QCOMPARE(img.IsInvalidCoordinates(NUM_ROWS / 2, NUM_COLS / 2), false);

    QCOMPARE(img.IsInvalidCoordinates(-1, -1), true);
    QCOMPARE(img.IsInvalidCoordinates(NUM_ROWS, NUM_COLS), true);
    QCOMPARE(img.IsInvalidCoordinates(-1, 0), true);
    QCOMPARE(img.IsInvalidCoordinates(0, -1), true);
    QCOMPARE(img.IsInvalidCoordinates(0, NUM_COLS), true);
    QCOMPARE(img.IsInvalidCoordinates(NUM_ROWS, 0), true);
}

void ImageTests::CorrectCoordinates()
{
    const int NUM_ROWS = 10, NUM_COLS = 20;

    acv::Image img(NUM_ROWS, NUM_COLS);

    int row = 5, col = 5;
    img.CorrectCoordinates(row, col);
    QCOMPARE(row, 5);
    QCOMPARE(col, 5);

    row = 0;
    col = 0;
    img.CorrectCoordinates(row, col);
    QCOMPARE(row, 0);
    QCOMPARE(col, 0);

    row = -1;
    col = -5;
    img.CorrectCoordinates(row, col);
    QCOMPARE(row, 1);
    QCOMPARE(col, 5);

    row = NUM_ROWS;
    col = NUM_COLS;
    img.CorrectCoordinates(row, col);
    QCOMPARE(row, NUM_ROWS - 2);
    QCOMPARE(col, NUM_COLS - 2);

    row = 5;
    col = NUM_COLS;
    img.CorrectCoordinates(row, col);
    QCOMPARE(row, 5);
    QCOMPARE(col, NUM_COLS - 2);

    row = NUM_ROWS;
    col = 5;
    img.CorrectCoordinates(row, col);
    QCOMPARE(row, NUM_ROWS - 2);
    QCOMPARE(col, 5);
}

void ImageTests::CheckPixelValue()
{
    for (int correctPixVal = acv::Image::MIN_PIXEL_VALUE; correctPixVal <= acv::Image::MAX_PIXEL_VALUE; ++correctPixVal)
    {
        int pixVal = correctPixVal;
        acv::Image::CheckPixelValue(pixVal);

        QCOMPARE(pixVal, correctPixVal);
    }

    int incorrectPixVal = acv::Image::MIN_PIXEL_VALUE - 1;
    acv::Image::CheckPixelValue(incorrectPixVal);
    QCOMPARE(incorrectPixVal, acv::Image::MIN_PIXEL_VALUE);

    incorrectPixVal = acv::Image::MAX_PIXEL_VALUE + 1;
    acv::Image::CheckPixelValue(incorrectPixVal);
    QCOMPARE(incorrectPixVal, acv::Image::MAX_PIXEL_VALUE);
}

void ImageTests::GetRawPointer()
{
    acv::Image img(10, 20);

    acv::Image::Byte pixelBrig = acv::Image::MIN_PIXEL_VALUE;
    for (int row = 0; row < 10; ++row)
        for (int col = 0; col < 20; ++col)
            img.SetPixel(row, col, pixelBrig++);

    pixelBrig = acv::Image::MIN_PIXEL_VALUE;
    int elemIdx = 0;
    for (int row = 0; row < 10; ++row)
        for (int col = 0; col < 20; ++col)
            QCOMPARE(*img.GetRawPointer(elemIdx++), pixelBrig++);
}

void ImageTests::Resize()
{
    const int NUM_ROWS = 6, NUM_COLS = 7;
    const int ROW_IDX_FROM = 2, ROW_IDX_TO = 3;
    const int COL_IDX_FROM = 2, COL_IDX_TO = 4;

    acv::Image imgSrc(NUM_ROWS, NUM_COLS);
    for (int row = 0; row < NUM_ROWS; ++row)
        for (int col = 0; col < NUM_COLS; ++col)
            if (row < ROW_IDX_FROM || row > ROW_IDX_TO || col < COL_IDX_FROM || col > COL_IDX_TO)
                imgSrc.SetPixel(row, col, acv::Image::MIN_PIXEL_VALUE);
            else
                imgSrc.SetPixel(row, col, acv::Image::MAX_PIXEL_VALUE);

    acv::Image resizeImg1 = imgSrc.Resize(COL_IDX_FROM, ROW_IDX_FROM, COL_IDX_TO, ROW_IDX_TO);
    for (int row = 0; row < resizeImg1.GetHeight(); ++row)
        for (int col = 0; col < resizeImg1.GetWidth(); ++col)
            QCOMPARE(resizeImg1.GetPixel(row, col), acv::Image::MAX_PIXEL_VALUE);

    acv::Image resizeImg2 = imgSrc.Resize(-1, -1, NUM_COLS, NUM_ROWS);
    const int ROW_IDX_FROM_2 = ROW_IDX_FROM + 1, ROW_IDX_TO_2 = ROW_IDX_TO + 1;
    const int COL_IDX_FROM_2 = COL_IDX_FROM + 1, COL_IDX_TO_2 = COL_IDX_TO + 1;
    for (int row = 0; row < resizeImg2.GetHeight(); ++row)
        for (int col = 0; col < resizeImg2.GetWidth(); ++col)
        {
            if (row < ROW_IDX_FROM_2 || row > ROW_IDX_TO_2 || col < COL_IDX_FROM_2 || col > COL_IDX_TO_2)
                QCOMPARE(resizeImg2.GetPixel(row, col), acv::Image::MIN_PIXEL_VALUE);
            else
                QCOMPARE(resizeImg2.GetPixel(row, col), acv::Image::MAX_PIXEL_VALUE);
        }
}

void ImageTests::Upscale()
{
    const int NUM_ROWS = 8, NUM_COLS = 9;

    for (int kScaleX = 2; kScaleX <= 4; kScaleX += 2)
        for (int kScaleY = 2; kScaleY <= 4; kScaleY += 2)
        {
            acv::Image imgSrc(NUM_ROWS, NUM_COLS);
            for (int row = 0; row < imgSrc.GetHeight(); ++row)
                for (int col = 0; col < imgSrc.GetWidth(); ++col)
                {
                    acv::Image::Byte brig = row * kScaleY + col * kScaleX;
                    imgSrc.SetPixel(row, col, brig);
                }

            acv::Image imgDst1 = imgSrc.Scale(kScaleX, kScaleY, acv::Image::ScaleType::UPSCALE);

            for (int row = 0; row < imgDst1.GetHeight(); ++row)
                for (int col = 0; col < imgDst1.GetWidth(); ++col)
                {
                    acv::Image::Byte brig = row + col;
                    QCOMPARE(imgDst1.GetPixel(row, col), brig);
                }
        }
}

QTEST_APPLESS_MAIN(ImageTests)

#include "ImageTests.moc"
