#pragma once

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include "gamesettings.h"

class TextureLoader;

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr, TextureLoader *textureLoader = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GameSettings settings;
    TextureLoader *textureLoader;
    QColor color1, color2;
    double phase = 0.0;

signals:
    void SettingsDone(GameSettings settings);
};
