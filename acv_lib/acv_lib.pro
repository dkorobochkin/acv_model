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

include( ../common.pri )
include( ../lib.pri )

TARGET = acv_lib$${LIB_SUFFIX}

TEMPLATE = lib

CONFIG += c++11
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += ACV_LIBRARY

INCLUDEPATH += \
        include \
        src\include\engine \
        src\include\service

SOURCES += \
        # Engine level cpp-files
        src/engine/BordersDetector.cpp \
        src/engine/HuMomentsCalculator.cpp \
        src/engine/Image.cpp \
        src/engine/ImageCombiner.cpp \
        src/engine/ImageCorrector.cpp \
        src/engine/ImageFilter.cpp \
        src/engine/ImageParametersCalculator.cpp \
        src/engine/MatrixFilter.cpp \
        src/engine/Point.cpp \
        # Service level cpp-files
        src/service/AImage.cpp \
        src/service/AImageManager.cpp \
        src/service/AImageParametersCalculator.cpp \
        src/service/AImageCorrector.cpp \
        src/service/AHuMomentsCalculator.cpp \
        src/service/AImageCombiner.cpp \
        src/service/ABordersDetector.cpp \
        src/service/AImageFilter.cpp \
        src/service/AImageUtils.cpp

HEADERS += \
        # Engine level h-files (private for external applications)
        src/include/engine/Image.h \
        src/include/engine/ImageFilter.h \
        src/include/engine/ImageCombiner.h \
        src/include/engine/ImageParametersCalculator.h \
        src/include/engine/MatrixFilter.h \
        src/include/engine/BordersDetector.h \
        src/include/engine/Point.h \
        src/include/engine/ImageCorrector.h \
        src/include/engine/HuMomentsCalculator.h \
        # Service level h-files (private for external applications)
        src/include/service/AImageManager.h \
        src/include/service/AImageUtils.h \
        # Service level h-files (public for external applications)
        include/AImage.h \
        include/AImageParametersCalculator.h \
        include/AImageCorrector.h \        
        include/AHuMomentsCalculator.h \
        include/AImageCombiner.h \
        include/ABordersDetector.h \
        include/AImageFilter.h
