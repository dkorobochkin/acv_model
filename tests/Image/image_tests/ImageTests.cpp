#include <QString>
#include <QtTest>

#include "Image.h"

class ImageTests : public QObject
{
    Q_OBJECT

public:
    ImageTests();

private Q_SLOTS:
    void DefaultConstructor();
    void ConstructorWithDimensions();
    void GetDimensions();
    void SetGetPixel();
    void IsInvalidCoordinates();
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

QTEST_APPLESS_MAIN(ImageTests)

#include "ImageTests.moc"
