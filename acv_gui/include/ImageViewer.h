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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "AImage.h"

#include <QWidget>
#include <QPixmap>

class QImage;

// Class is used to draw the images on the screen
class ImageViewer : public QWidget
{
    Q_OBJECT

public: // Public constructors

    explicit ImageViewer(QWidget *parent = nullptr);

public: // Public draw methods

    // Method is used to draw QImage class
    void DrawImage(const QImage& img);

    // Method is used to draw Image class
    void DrawImage(const AImage& img);

    // Clear the screen
    void Clear();

protected:
    void paintEvent(QPaintEvent*);

signals:

public slots:

private: // Private members

    // This pixmap is used to storage the drawn data
    QPixmap mPixmap;

};

#endif // IMAGEVIEWER_H
