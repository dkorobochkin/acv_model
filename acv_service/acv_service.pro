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

TARGET = acv_service$${LIB_SUFFIX}

TEMPLATE = lib

CONFIG += c++11
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += ACV_SERVICE_LIBRARY

INCLUDEPATH += \
        include \
        src/include \
        ../acv_engine/include

SOURCES += \
        src/AImage.cpp \
        src/AImageManager.cpp \
        src/AImageParametersCalculator.cpp \
        src/AImageCorrector.cpp \
        src/AHuMomentsCalculator.cpp \
        src/AImageCombiner.cpp \
        src/ABordersDetector.cpp \
        src/AImageFilter.cpp

HEADERS += \
        src/include/AImageManager.h \
        include/AImage.h \
        include/AImageParametersCalculator.h \
        include/AImageCorrector.h \        
        include/AHuMomentsCalculator.h \
        include/AImageCombiner.h \
        include/ABordersDetector.h \
        include/AImageFilter.h
