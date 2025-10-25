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
    main.cpp \
    mainwindow.cpp \
    pages/menupage.cpp \
    pages/settingspage.cpp \
    pages/gamepage.cpp \
    textureloader.cpp

HEADERS += \
    elements/gameboard.h \
    elements/player.h \
    gamesettings.h \
    mainwindow.h \
    pages/menupage.h \
    pages/settingspage.h \
    pages/gamepage.h \
    textureloader.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
