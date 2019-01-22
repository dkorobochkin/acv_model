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

QTEST_APPLESS_MAIN(ImageTests)

#include "ImageTests.moc"
