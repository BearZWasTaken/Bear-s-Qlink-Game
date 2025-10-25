#include "settingspage.h"
#include "textureloader.h"

#include <QPainter>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>

SettingsPage::SettingsPage(QWidget *parent, TextureLoader *textureLoader)
    : QWidget(parent), textureLoader(textureLoader)
{
    //// bkgrd color ////
    color1 = QColor("#fbc2eb");
    color2 = QColor("#a6c1ee");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        phase += 1;
        if (phase >= 360.0)
            phase -= 360.0;

        QColor c1 = QColor::fromHsvF(fmod(phase / 360.0, 1.0), 0.4, 1.0);
        QColor c2 = QColor::fromHsvF(fmod((phase + 60.0) / 360.0, 1.0), 0.4, 1.0);

        color1 = c1.lighter(120);
        color2 = c2.lighter(120);
        update();
    });
    timer->start(50);

    //// main layout ////
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("⚙️ Game Settings", this);
    title->setStyleSheet(R"(
        color: black;
        font: bold 28pt "Comic Sans MS";
    )");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QString labelStyle = R"(color: black; font: 16pt "Comic Sans MS";)";
    QString spinStyle = R"(
        QSpinBox {
            background-color: rgba(255,255,255,0.8);
            border-radius: 8px;
            padding: 3px 10px;
            font: 14pt "Comic Sans MS";
        }
    )";

    QString comboStyle = R"(
        QComboBox {
            background-color: rgba(255,255,255,0.8);
            border-radius: 8px;
            padding: 5px 10px;
            font: 14pt "Comic Sans MS";
        }
    )";

    QString switchStyle = R"(
        QCheckBox::indicator {
            width: 50px;
            height: 25px;
        }
        QCheckBox::indicator:unchecked {
            border-radius: 12px;
            background-color: #ee0000;
        }
        QCheckBox::indicator:checked {
            border-radius: 12px;
            background-color: #00c853;
        }
    )";

    QHBoxLayout *playerLayout = new QHBoxLayout();
    QPushButton *p1Btn = new QPushButton("1 Player");
    QPushButton *p2Btn = new QPushButton("2 Players");
    QString btnStyle = R"(
        QPushButton {
            color: white;
            background-color: rgba(0,0,0,0.5);
            border-radius: 10px;
            font: 14pt "Comic Sans MS";
            padding: 5px 15px;
        }
        QPushButton:hover {
            background-color: rgba(0,0,0,0.3);
        }
        QPushButton:checked {
            background-color: rgba(255,255,255,0.4);
            color: black;
        }
    )";
    p1Btn->setCheckable(true);
    p2Btn->setCheckable(true);
    p1Btn->setStyleSheet(btnStyle);
    p2Btn->setStyleSheet(btnStyle);
    p1Btn->setChecked(true);
    connect(p1Btn, &QPushButton::clicked, this, [p1Btn, p2Btn]() {
        p1Btn->setChecked(true);
        p2Btn->setChecked(false);
    });
    connect(p2Btn, &QPushButton::clicked, this, [p1Btn, p2Btn]() {
        p2Btn->setChecked(true);
        p1Btn->setChecked(false);
    });
    playerLayout->addWidget(p1Btn);
    playerLayout->addWidget(p2Btn);
    layout->addLayout(playerLayout);

    QHBoxLayout *sizeLayout = new QHBoxLayout();
    QLabel *sizeLabel = new QLabel("Puzzle Size:");
    sizeLabel->setStyleSheet(labelStyle);
    QSpinBox *widthBox = new QSpinBox();
    QSpinBox *heightBox = new QSpinBox();
    widthBox->setRange(5, 25);
    heightBox->setRange(5, 25);
    widthBox->setValue(settings.puzzle_width);
    heightBox->setValue(settings.puzzle_height);
    widthBox->setStyleSheet(spinStyle);
    heightBox->setStyleSheet(spinStyle);
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(widthBox);
    sizeLayout->addWidget(new QLabel("×"));
    sizeLayout->addWidget(heightBox);
    layout->addLayout(sizeLayout);

    QHBoxLayout *blockLayout = new QHBoxLayout();
    QLabel *blockLabel = new QLabel("Block Types:");
    blockLabel->setStyleSheet(labelStyle);
    QSpinBox *blockBox = new QSpinBox();
    blockBox->setRange(3, 10);
    blockBox->setValue(settings.block_types);
    blockBox->setStyleSheet(spinStyle);
    blockLayout->addWidget(blockLabel);
    blockLayout->addWidget(blockBox);
    layout->addLayout(blockLayout);

    QHBoxLayout *timeLayout = new QHBoxLayout();
    QLabel *timeLabel = new QLabel("Time Limit:");
    timeLabel->setStyleSheet(labelStyle);
    QSpinBox *timeBox = new QSpinBox();
    timeBox->setRange(3, 1000);
    timeBox->setValue(settings.time_limit);
    timeBox->setStyleSheet(spinStyle);
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeBox);
    layout->addLayout(timeLayout);

    QHBoxLayout *edgeLayout = new QHBoxLayout();
    QLabel *edgeLabel = new QLabel("Enable Edge Alleyway:");
    edgeLabel->setStyleSheet(labelStyle);
    QCheckBox *edgeSwitch = new QCheckBox();
    edgeSwitch->setChecked(settings.enable_edge_alleyway);
    edgeSwitch->setStyleSheet(switchStyle);
    edgeLayout->addWidget(edgeLabel);
    edgeLayout->addWidget(edgeSwitch);
    layout->addLayout(edgeLayout);

    QHBoxLayout *propLayout = new QHBoxLayout();
    QLabel *propLabel = new QLabel("Enable Props:");
    propLabel->setStyleSheet(labelStyle);
    QCheckBox *propSwitch = new QCheckBox();
    propSwitch->setChecked(settings.enable_props);
    propSwitch->setStyleSheet(switchStyle);
    propLayout->addWidget(propLabel);
    propLayout->addWidget(propSwitch);
    layout->addLayout(propLayout);

    QHBoxLayout *themeLayout = new QHBoxLayout();
    QLabel *themeLabel = new QLabel("Theme:");
    themeLabel->setStyleSheet(labelStyle);
    QComboBox *themeBox = new QComboBox();
    QStringList themeVec;
    for (auto &theme : textureLoader->themes)
        themeVec.append(theme.name);
    themeBox->addItems(themeVec);
    if (themeVec.empty())
        qWarning() << "No theme available!";
    else
        settings.theme_name = themeVec[0];
    themeBox->setStyleSheet(comboStyle);
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeBox);
    layout->addLayout(themeLayout);

    QHBoxLayout *doneLayout = new QHBoxLayout();
    QPushButton *doneBtn = new QPushButton("Done!");
    doneBtn->setStyleSheet(btnStyle);

    doneLayout->addWidget(doneBtn);
    layout->addLayout(doneLayout);

    connect(widthBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](unsigned int v) {
        settings.puzzle_width = v;
    });
    connect(heightBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](unsigned int v) {
        settings.puzzle_height = v;
    });
    connect(blockBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](unsigned int v) {
        settings.block_types = v;
    });
    connect(timeBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](unsigned int v) {
        settings.time_limit = v;
    });
    connect(edgeSwitch, &QCheckBox::toggled, this, [this](bool checked) {
        settings.enable_edge_alleyway = checked;
    });
    connect(propSwitch, &QCheckBox::toggled, this, [this](bool checked) {
        settings.enable_props = checked;
    });
    connect(themeBox, &QComboBox::currentTextChanged, this, [this](const QString &themeName) {
        settings.theme_name = themeName;
    });
    connect(doneBtn, &QPushButton::clicked, this, [this]() {
        emit SettingsDone(settings);
    });
}

void SettingsPage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient grad(rect().topLeft(), rect().bottomRight());
    grad.setColorAt(0, color1);
    grad.setColorAt(1, color2);
    p.fillRect(rect(), grad);
}
