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

include( ../common.pri )
include( ../lib.pri )

TARGET = engine$${LIB_SUFFIX}
TEMPLATE = lib

CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += ENGINE_LIBRARY

INCLUDEPATH += \
        include

SOURCES += \
        src/BordersDetector.cpp \
        src/HuMomentsCalculator.cpp \
        src/Image.cpp \
        src/ImageCombiner.cpp \
        src/ImageCorrector.cpp \
        src/ImageFilter.cpp \
        src/ImageParametersCalculator.cpp \
        src/MatrixFilter.cpp \
        src/Point.cpp

HEADERS += \
        include/Image.h \
        include/ImageFilter.h \
        include/ImageCombiner.h \
        include/ImageParametersCalculator.h \
        include/MatrixFilter.h \
        include/BordersDetector.h \
        include/Point.h \
        include/ImageCorrector.h \
        include/HuMomentsCalculator.h
