QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    ./pages \
    ./elements

SOURCES += \
    elements/gameboard.cpp \
    elements/player.cpp \
    filestreamer.cpp \
    main.cpp \
    mainwindow.cpp \
    pages/menupage.cpp \
    pages/oneplayergamepage.cpp \
    pages/selectmappage.cpp \
    pages/settingspage.cpp \
    pages/twoplayersgamepage.cpp \
    textureloader.cpp

HEADERS += \
    elements/gameboard.h \
    elements/player.h \
    filestreamer.h \
    gamesettings.h \
    mainwindow.h \
    pages/menupage.h \
    pages/oneplayergamepage.h \
    pages/selectmappage.h \
    pages/settingspage.h \
    pages/twoplayersgamepage.h \
    textureloader.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
