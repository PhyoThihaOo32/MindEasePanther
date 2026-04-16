QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -Wno-implicit-function-declaration

TARGET = MindEase
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    recommendations.cpp \
    toolkit.cpp \
    journal.cpp

HEADERS += \
    mainwindow.h \
    recommendations.h \
    toolkit.h \
    journal.h
