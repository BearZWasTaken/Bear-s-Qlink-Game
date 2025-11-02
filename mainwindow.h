#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>

#include "textureloader.h"

#include "menupage.h"
#include "settingspage.h"
#include "oneplayergamepage.h"
#include "twoplayersgamepage.h"
#include "selectmappage.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QStackedWidget *stacked_widget;

    TextureLoader *textureLoader;

    MenuPage *menuPage;
    SettingsPage *settingsPage;
    OnePlayerGamePage *onePlayerGamePage;
    TwoPlayersGamePage *twoPlayersGamePage;
    SelectMapPage *selectMapPage;

    QString findImagesDir();
};
