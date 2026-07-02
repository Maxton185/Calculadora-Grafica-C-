QT += widgets charts

CONFIG += c++17
CONFIG += console

HEADERS += \
    CambiarTema.h \
    exprtk.hpp \
    CustomChartView.h \
    WindowGraficadora.h

SOURCES += \
    CustomChartView.cpp \
    WindowGraficadora.cpp \
    main.cpp

INCLUDEPATH += .

QMAKE_CXX = clang++
QMAKE_CC = clang
