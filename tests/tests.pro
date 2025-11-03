TEMPLATE = app

QT       += core gui
QT       += core testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += simpletest.cpp

HEADERS += simpletest.h

INCLUDEPATH += \
    .. \
    ../pages \
    ../elements

SOURCES += \
    ../elements/gameboard.cpp \
    ../elements/player.cpp \
    ../filestreamer.cpp \
    ../mainwindow.cpp \
    ../pages/menupage.cpp \
    ../pages/oneplayergamepage.cpp \
    ../pages/selectmappage.cpp \
    ../pages/settingspage.cpp \
    ../pages/twoplayersgamepage.cpp \
    ../textureloader.cpp

HEADERS += \
    ../elements/gameboard.h \
    ../elements/player.h \
    ../filestreamer.h \
    ../gamesettings.h \
    ../mainwindow.h \
    ../pages/menupage.h \
    ../pages/oneplayergamepage.h \
    ../pages/selectmappage.h \
    ../pages/settingspage.h \
    ../pages/twoplayersgamepage.h \
    ../textureloader.h
