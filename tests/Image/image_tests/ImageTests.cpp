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

QTEST_APPLESS_MAIN(ImageTests)

#include "ImageTests.moc"
