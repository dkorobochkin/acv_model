#-------------------------------------------------
#
# Project created by QtCreator 2019-01-20T23:50:42
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = ImageTests
CONFIG   += console
CONFIG   -= app_bundle

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
        ..\..\engine\include

SOURCES += \
        ImageTests.cpp

debug {
SOURCES += \
        ../../engine/src/BordersDetector.cpp \
        ../../engine/src/HuMomentsCalculator.cpp \
        ../../engine/src/Image.cpp \
        ../../engine/src/ImageCombiner.cpp \
        ../../engine/src/ImageCorrector.cpp \
        ../../engine/src/ImageFilter.cpp \
        ../../engine/src/ImageParametersCalculator.cpp \
        ../../engine/src/MatrixFilter.cpp \
        ../../engine/src/Point.cpp
}

debug {
HEADERS += \
        ../../engine/include/BordersDetector.h \
        ../../engine/include/HuMomentsCalculator.h \
        ../../engine/include/Image.h \
        ../../engine/include/ImageCombiner.h \
        ../../engine/include/ImageCorrector.h \
        ../../engine/include/ImageFilter.h \
        ../../engine/include/ImageParametersCalculator.h \
        ../../engine/include/MatrixFilter.h \
        ../../engine/include/Point.h
}

include( ../../common.pri )

LIBS += -lengine$${LIB_SUFFIX}
