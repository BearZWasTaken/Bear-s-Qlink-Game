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

    onePlayerGamePage = new OnePlayerGamePage(this, textureLoader);
    stacked_widget->addWidget(onePlayerGamePage);

    twoPlayersGamePage = new TwoPlayersGamePage(this, textureLoader);
    stacked_widget->addWidget(twoPlayersGamePage);

    selectMapPage = new SelectMapPage(this, textureLoader);
    stacked_widget->addWidget(selectMapPage);

    connect(menuPage, &MenuPage::NewGameSignal, this, [this]() {
        stacked_widget->setCurrentWidget(settingsPage);
    });

    connect(menuPage, &MenuPage::LoadGameSignal, this, [this]() {
        selectMapPage->LoadMapList();
        stacked_widget->setCurrentWidget(selectMapPage);
    });

    connect(menuPage, &MenuPage::ExitGameSignal, this, []() {
        QApplication::quit();
    });

    connect(settingsPage, &SettingsPage::SettingsDoneSignal, this, [this](const GameSettings &settings) {
        if (settings.player_cnt == 1)
        {
            stacked_widget->setCurrentWidget(onePlayerGamePage);
            onePlayerGamePage->NewGame(settings);
        }
        else
        {
            stacked_widget->setCurrentWidget(twoPlayersGamePage);
            twoPlayersGamePage->NewGame(settings);
        }
    });

    connect(onePlayerGamePage, &OnePlayerGamePage::ReturnToMenuSignal, this, [this]() {
        menuPage->Reset();
        stacked_widget->setCurrentWidget(menuPage);
    });

    connect(selectMapPage, &SelectMapPage::BackToMenuSignal, this, [this]() {
        stacked_widget->setCurrentWidget(menuPage);
    });

    connect(selectMapPage, &SelectMapPage::LoadMapSignal, this, [this](const QString &mapName) {
        stacked_widget->setCurrentWidget(onePlayerGamePage);
        onePlayerGamePage->LoadGame(mapName);
    });

    stacked_widget->setCurrentWidget(menuPage);
}

MainWindow::~MainWindow()
{
    delete textureLoader;
}
