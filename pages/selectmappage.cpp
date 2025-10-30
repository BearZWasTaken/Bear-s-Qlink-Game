#include "selectmappage.h"

#include "filestreamer.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QTimer>
#include <QPainter>
#include <QDir>

SelectMapPage::SelectMapPage(QWidget *parent, TextureLoader *textureLoader)
    : QWidget(parent), textureLoader(textureLoader)
{
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

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(50, 30, 50, 30);

    QLabel *title = new QLabel("🗺️ Select Map 🗺️", this);
    title->setStyleSheet(R"(
        QLabel {
            color: white;
            font: bold 32pt "Comic Sans MS";
            text-align: center;
            background-color: rgba(0,0,0,0.3);
            border-radius: 20px;
            padding: 15px;
        }
    )");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    mapTable = new QTableWidget(this);
    mapTable->setColumnCount(5);
    mapTable->setHorizontalHeaderLabels({"Map Name", "Theme", "Board", "Time Left", "Blocks"});

    mapTable->setStyleSheet(R"(
        QTableWidget {
            background-color: rgba(255,255,255,0.9);
            border-radius: 15px;
            gridline-color: rgba(0,0,0,0.1);
            font: 12pt "Comic Sans MS";
            color: #333;
        }
        QTableWidget::item {
            padding: 10px;
            border-bottom: 1px solid rgba(0,0,0,0.1);
        }
        QTableWidget::item:selected {
            background-color: rgba(255,182,193,0.5);
        }
        QHeaderView::section {
            background-color: rgba(255,182,193,0.8);
            color: white;
            font-weight: bold;
            font-size: 10pt;
            padding: 15px;
            border: none;
        }
    )");

    mapTable->setSelectionMode(QAbstractItemView::SingleSelection);
    mapTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mapTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mapTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mapTable->verticalHeader()->setVisible(false);
    mapTable->setAlternatingRowColors(true);

    mainLayout->addWidget(mapTable, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(30);

    QString btnStyle = R"(
        QPushButton {
            color: white;
            background-color: rgba(0,0,0,0.5);
            border-radius: 15px;
            font: 20pt "Comic Sans MS";
            padding: 12px 30px;
            min-width: 200px;
        }
        QPushButton:hover {
            background-color: rgba(0,0,0,0.3);
            transform: scale(1.05);
        }
        QPushButton:pressed {
            background-color: rgba(0,0,0,0.1);
        }
        QPushButton:disabled {
            background-color: rgba(0,0,0,0.2);
            color: rgba(255,255,255,0.5);
        }
    )";

    backBtn = new QPushButton("🔙 Back to Menu");
    backBtn->setStyleSheet(btnStyle);

    loadBtn = new QPushButton("🚀 Load Map");
    loadBtn->setEnabled(false);
    loadBtn->setStyleSheet(btnStyle);

    buttonLayout->addWidget(backBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(loadBtn);

    mainLayout->addLayout(buttonLayout);

    connect(backBtn, &QPushButton::clicked, this, &SelectMapPage::BackToMenu);

    connect(loadBtn, &QPushButton::clicked, this, [this]() {
        int currentRow = mapTable->currentRow();
        if (currentRow >= 0 && currentRow < mapOptions.size())
        {
            QString mapName = mapOptions[currentRow].name;
            this->LoadMap(mapName);
        }
    });

    connect(mapTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        loadBtn->setEnabled(mapTable->currentRow() >= 0);
    });

    PopulateMapTable();
}

void SelectMapPage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient grad(rect().topLeft(), rect().bottomRight());
    grad.setColorAt(0, color1);
    grad.setColorAt(1, color2);
    p.fillRect(rect(), grad);
}

void SelectMapPage::LoadMapList()
{
    QString mapsDir = FileStreamer::FindDir("maps");

    QDir dir(mapsDir);
    if (!dir.exists()) return;

    QStringList filters;
    filters << "*.json";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files, QDir::Name);

    mapOptions.clear();
    for (auto &fileInfo : fileList)
    {
        QJsonObject jsonObject = FileStreamer::ReadJsonObject(fileInfo.absoluteFilePath());
        MapOption new_option;
        new_option.name = fileInfo.baseName();
        new_option.theme = jsonObject["theme_name"].toString();
        new_option.board_width = jsonObject["board_width"].toInt();
        new_option.board_height = jsonObject["board_height"].toInt();
        new_option.timeLeft = jsonObject["timeLeftMs"].toInt() / 1000.0f;
        new_option.remaining_blocks = jsonObject["remaining_blocks"].toInt();
        mapOptions.emplace_back(new_option);
    }

    PopulateMapTable();
}

void SelectMapPage::PopulateMapTable()
{
    mapTable->setRowCount(mapOptions.size());

    for (int i=0; i<mapOptions.size(); i++)
    {
        const MapOption &map = mapOptions[i];

        QTableWidgetItem *nameItem = new QTableWidgetItem(map.name);
        nameItem->setTextAlignment(Qt::AlignCenter);
        mapTable->setItem(i, 0, nameItem);

        QTableWidgetItem *themeItem = new QTableWidgetItem(map.theme);
        themeItem->setTextAlignment(Qt::AlignCenter);
        mapTable->setItem(i, 1, themeItem);

        QTableWidgetItem *sizeItem = new QTableWidgetItem(QString::number(map.board_width) + "*" + QString::number(map.board_height));
        sizeItem->setTextAlignment(Qt::AlignCenter);
        mapTable->setItem(i, 2, sizeItem);

        QTableWidgetItem *timeItem = new QTableWidgetItem(QString::number(map.timeLeft) + "s");
        timeItem->setTextAlignment(Qt::AlignCenter);
        mapTable->setItem(i, 3, timeItem);

        QTableWidgetItem *blocksItem = new QTableWidgetItem(QString::number(map.remaining_blocks));
        blocksItem->setTextAlignment(Qt::AlignCenter);
        mapTable->setItem(i, 4, blocksItem);
    }

    if (mapOptions.size() > 0)
    {
        mapTable->selectRow(0);
    }
}

void SelectMapPage::BackToMenu()
{
    emit BackToMenuSignal();
}

void SelectMapPage::LoadMap(const QString mapName)
{
    emit LoadMapSignal(mapName);
}
