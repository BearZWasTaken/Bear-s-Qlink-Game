#include "mainwindow.h"

#include <QVBoxLayout>
#include <qdebug.h>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    textureLoader = new TextureLoader();
    textureLoader->LoadAll();

    setWindowTitle("Bear's QLink Game!");
    resize(800, 600);

    stacked_widget = new QStackedWidget(this);
    setCentralWidget(stacked_widget);

    menuPage = new MenuPage(this);
    stacked_widget->addWidget(menuPage);

    settingsPage = new SettingsPage(this, textureLoader);
    stacked_widget->addWidget(settingsPage);

    gamePage = new GamePage(this, textureLoader);
    stacked_widget->addWidget(gamePage);

    connect(menuPage, &MenuPage::NewGame, this, [this]() {
        stacked_widget->setCurrentWidget(settingsPage);
        qDebug() << "Switch to settings page.";
    });

    connect(menuPage, &MenuPage::ExitGame, this, [this]() {
        QApplication::quit();
    });

    connect(settingsPage, &SettingsPage::SettingsDone, this, [this](const GameSettings &settings) {
        stacked_widget->setCurrentWidget(gamePage);
        gamePage->NewGame(settings);
        qDebug() << "Switch to game page.";
    });

    connect(gamePage, &GamePage::ReturnToMenu, this, [this]() {
        menuPage->Reset();
        stacked_widget->setCurrentWidget(menuPage);
        qDebug() << "Switch to menu page.";
    });

    stacked_widget->setCurrentWidget(menuPage);
}

MainWindow::~MainWindow()
{
    delete textureLoader;
}
