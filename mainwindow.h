#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>

#include "textureloader.h"

#include "menupage.h"
#include "settingspage.h"
#include "gamepage.h"

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
    GamePage *gamePage;

    QString findImagesDir();
};
