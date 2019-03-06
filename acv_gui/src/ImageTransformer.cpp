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

#include "ImageTransformer.h"
#include "Image.h"

#include <QImage>
#include <QRgb>

acv::Image ImageTransormer::QImage2AImage(const QImage& from)
{
    acv::Image to(from.height(), from.width());

    for (int y = 0; y < from.height(); ++y)
        for (int x = 0; x < from.width(); ++x)
        {
            QRgb pix = from.pixel(x, y);
            int brightness = qGray(pix);
            to(y, x) = static_cast<acv::Image::Byte>(brightness);
        }

    return to;
}

QImage ImageTransormer::AImage2QImage(const acv::Image& from)
{
    QImage to( from.GetWidth(), from.GetHeight(), QImage::Format_ARGB32);

    for (int y = 0; y < to.height(); ++y)
        for (int x = 0; x < to.width(); ++x)
        {
            int brig = static_cast<int>(from(y, x));
            QRgb pix = qRgb(brig, brig, brig);
            to.setPixel(x, y, pix);
        }

    return to;
}
