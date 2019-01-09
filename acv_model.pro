#
# MIT License
#
# Copyright (c) 2018 Dmitriy Korobochkin, Vitaliy Garmash.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

#-------------------------------------------------
#
# Project created by QtCreator 2018-08-26T10:30:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = acv_model
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
        include/engine \
        include/gui

SOURCES += \
        src/main.cpp \
        src/gui/mainwindow.cpp \
        src/gui/imageviewer.cpp \
        src/gui/imagetransformer.cpp \
        src/engine/Image.cpp \
        src/engine/ImageFilter.cpp \
        src/engine/ImageCombiner.cpp \
        src/engine/ImageParametersCalculator.cpp \
        src/engine/MatrixFilter.cpp \
        src/engine/BordersDetector.cpp \
        src/engine/Point.cpp \
        src/engine/ImageCorrector.cpp \
        src/engine/HuMomentsCalculator.cpp \
    src/engine/qcustomplot.cpp \
    include/gui/hist1.cpp

HEADERS += \
        include/gui/mainwindow.h \
        include/gui/imageviewer.h \
        include/gui/imagetransformer.h \
        include/engine/Image.h \
        include/engine/ImageFilter.h \
        include/engine/ImageCombiner.h \
        include/engine/ImageParametersCalculator.h \
        include/engine/MatrixFilter.h \
        include/engine/BordersDetector.h \
        include/engine/Point.h \
        include/engine/ImageCorrector.h \
        include/engine/HuMomentsCalculator.h \
    include/engine/qcustomplot.h \
    include/gui/hist1.h

FORMS += \
        ui/mainwindow.ui
