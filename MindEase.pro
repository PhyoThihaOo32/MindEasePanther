QT       += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -Wno-implicit-function-declaration

TARGET = MindEase
TEMPLATE = app

INCLUDEPATH += \
    $$PWD \
    $$PWD/app \
    $$PWD/core \
    $$PWD/screens \
    $$PWD/models \
    $$PWD/storage \
    $$PWD/legacy

SOURCES += \
    app/main.cpp \
    app/mainwindow.cpp \
    core/screen.cpp \
    screens/recommendations.cpp \
    screens/toolkit.cpp \
    screens/assistantchat.cpp \
    screens/journal.cpp \
    models/journalentry.cpp \
    storage/journalstorage.cpp

HEADERS += \
    app/mainwindow.h \
    core/screen.h \
    screens/recommendations.h \
    screens/toolkit.h \
    screens/assistantchat.h \
    screens/journal.h \
    models/journalentry.h \
    storage/journalstorage.h

RESOURCES += \
    resources/resources.qrc
