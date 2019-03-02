#
# MIT License
#
# Copyright (c) 2018-2019 Dmitriy Korobochkin, Vitaliy Garmash.
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

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = gui
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
        include \
        ../engine/include \
        ../thirdparty/qcustomplot/include

SOURCES += \
        main.cpp \
        src/MainWindow.cpp \
        src/ImageViewer.cpp \
        src/ImageTransformer.cpp \
        src/HistogramWidget.cpp

debug {
SOURCES += \
        ../engine/src/BordersDetector.cpp \
        ../engine/src/HuMomentsCalculator.cpp \
        ../engine/src/Image.cpp \
        ../engine/src/ImageCombiner.cpp \
        ../engine/src/ImageCorrector.cpp \
        ../engine/src/ImageFilter.cpp \
        ../engine/src/ImageParametersCalculator.cpp \
        ../engine/src/MatrixFilter.cpp \
        ../engine/src/Point.cpp
}

HEADERS += \
        include/MainWindow.h \
        include/ImageViewer.h \
        include/ImageTransformer.h \
        include/HistogramWidget.h

debug {
HEADERS += \
        ../engine/include/BordersDetector.h \
        ../engine/include/HuMomentsCalculator.h \
        ../engine/include/Image.h \
        ../engine/include/ImageCombiner.h \
        ../engine/include/ImageCorrector.h \
        ../engine/include/ImageFilter.h \
        ../engine/include/ImageParametersCalculator.h \
        ../engine/include/MatrixFilter.h \
        ../engine/include/Point.h
}

FORMS += \
        ui/MainWindow.ui

include( ../common.pri )
include( ../app.pri )

LIBS += -lengine$${LIB_SUFFIX}
LIBS += -lqcustomplot$${LIB_SUFFIX}
