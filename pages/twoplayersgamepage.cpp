#include "twoplayersgamepage.h"

#include "gamesettings.h"
#include "gameboard.h"
#include "textureloader.h"
#include "player.h"
#include "filestreamer.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

TwoPlayersGamePage::TwoPlayersGamePage(QWidget *parent, TextureLoader *textureLoader)
    : QWidget(parent), textureLoader(textureLoader)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);


    //// Timer ////

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TwoPlayersGamePage::UpdateTimer);


    //// Top Bar ////

    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(80);
    topBar->setStyleSheet("background-color: transparent;");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);
    topLayout->setSpacing(20);

    // ---- Player 1 Section ----
    QWidget *player1Section = new QWidget(topBar);
    player1Section->setStyleSheet("background-color: transparent;");
    QHBoxLayout *player1Layout = new QHBoxLayout(player1Section);
    player1Layout->setContentsMargins(0, 0, 0, 0);
    player1Layout->setSpacing(15);

    p1BlocksLeftLabel = new QLabel("P1 Blocks: --", player1Section);
    p1BlocksLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    p1BlocksLeftLabel->setAlignment(Qt::AlignLeft);

    p1TimeLeftLabel = new QLabel("P1 Time: --:--.-", player1Section);
    p1TimeLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    p1TimeLeftLabel->setAlignment(Qt::AlignLeft);

    player1Layout->addWidget(p1BlocksLeftLabel);
    player1Layout->addWidget(p1TimeLeftLabel);

    // ---- Player 2 Section ----
    QWidget *player2Section = new QWidget(topBar);
    player2Section->setStyleSheet("background-color: transparent;");
    QHBoxLayout *player2Layout = new QHBoxLayout(player2Section);
    player2Layout->setContentsMargins(0, 0, 0, 0);
    player2Layout->setSpacing(15);

    p2BlocksLeftLabel = new QLabel("P2 Blocks: --", player2Section);
    p2BlocksLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    p2BlocksLeftLabel->setAlignment(Qt::AlignRight);

    p2TimeLeftLabel = new QLabel("P2 Time: --:--.-", player2Section);
    p2TimeLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    p2TimeLeftLabel->setAlignment(Qt::AlignRight);

    player2Layout->addWidget(p2TimeLeftLabel);
    player2Layout->addWidget(p2BlocksLeftLabel);

    // ---- Pause Button ----
    pauseBtn = new QPushButton("Pause⏸️", topBar);
    pauseBtn->setFixedSize(80, 30);
    pauseBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF6B9E;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8AB5;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #E55A8A;"
        "}"
        );

    topLayout->addWidget(player1Section, 1);
    topLayout->addStretch(1);
    topLayout->addWidget(pauseBtn, 0, Qt::AlignCenter);
    topLayout->addStretch(1);
    topLayout->addWidget(player2Section, 1);

    mainLayout->addWidget(topBar);


    //// Boards ////

    QWidget *boardsContainer = new QWidget(this);
    QHBoxLayout *boardsLayout = new QHBoxLayout(boardsContainer);
    boardsLayout->setSpacing(20);
    boardsLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 2; ++i)
    {
        QWidget *boardWrapper = new QWidget(boardsContainer);
        QVBoxLayout *boardLayout = new QVBoxLayout(boardWrapper);
        boardLayout->setContentsMargins(0, 0, 0, 0);
        boardLayout->setSpacing(0);

        QGraphicsView *view = new QGraphicsView(boardWrapper);
        view->setRenderHint(QPainter::Antialiasing);
        view->setRenderHint(QPainter::SmoothPixmapTransform);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setStyleSheet("background-color: transparent; border: none;");

        QGraphicsScene *scene = new QGraphicsScene(view);
        view->setScene(scene);

        boardLayout->addWidget(view, 1);
        boardsLayout->addWidget(boardWrapper, 1);

        views[i] = view;
        scenes[i] = scene;
        pathItems[i] = nullptr;
    }

    mainLayout->addWidget(boardsContainer, 1);


    //// Pause Overlay ////

    pauseOverlay = new QWidget(this);
    pauseOverlay->setStyleSheet("background-color: rgba(0, 0, 0, 0.7);");
    pauseOverlay->setVisible(false);

    QVBoxLayout *pauseOverlayLayout = new QVBoxLayout(pauseOverlay);
    pauseOverlayLayout->setAlignment(Qt::AlignCenter);
    pauseOverlayLayout->setSpacing(30);

    pauseLabel = new QLabel(pauseOverlay);
    pauseLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            font: bold 48pt "Comic Sans MS", Arial, sans-serif;
            text-align: center;
        }
    )");
    pauseLabel->setText("Pause... ⏸️");
    pauseLabel->setAlignment(Qt::AlignCenter);
    pauseOverlayLayout->addWidget(pauseLabel);

    connect(pauseBtn, &QPushButton::clicked, this, &TwoPlayersGamePage::Pause);

    resumeBtn = new QPushButton("Resume", pauseOverlay);
    resumeBtn->setFixedSize(200, 50);
    resumeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #FF6B9E;
            color: white;
            border: none;
            border-radius: 10px;
            font: bold 18pt "Comic Sans MS", Arial, sans-serif;
        }
        QPushButton:hover {
            background-color: #FF8AB5;
        }
        QPushButton:pressed {
            background-color: #E55A8A;
        }
    )");
    pauseOverlayLayout->addWidget(resumeBtn, 0, Qt::AlignCenter);
    connect(resumeBtn, &QPushButton::clicked, this, &TwoPlayersGamePage::Resume);

    saveAndExitBtn = new QPushButton("Save && Exit", pauseOverlay);
    saveAndExitBtn->setFixedSize(200, 50);
    saveAndExitBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #FF6B9E;
            color: white;
            border: none;
            border-radius: 10px;
            font: bold 18pt "Comic Sans MS", Arial, sans-serif;
        }
        QPushButton:hover {
            background-color: #FF8AB5;
        }
        QPushButton:pressed {
            background-color: #E55A8A;
        }
    )");
    pauseOverlayLayout->addWidget(saveAndExitBtn, 0, Qt::AlignCenter);
    connect(saveAndExitBtn, &QPushButton::clicked, this, &TwoPlayersGamePage::SaveAndExit);

    discardAndExitBtn = new QPushButton("Discard && Exit", pauseOverlay);
    discardAndExitBtn->setFixedSize(200, 50);
    discardAndExitBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #FF6B9E;
            color: white;
            border: none;
            border-radius: 10px;
            font: bold 18pt "Comic Sans MS", Arial, sans-serif;
        }
        QPushButton:hover {
            background-color: #FF8AB5;
        }
        QPushButton:pressed {
            background-color: #E55A8A;
        }
    )");
    pauseOverlayLayout->addWidget(discardAndExitBtn, 0, Qt::AlignCenter);
    connect(discardAndExitBtn, &QPushButton::clicked, this, &TwoPlayersGamePage::Exit);


    //// "Game Over" Overlay ////

    gameOverOverlay = new QWidget(this);
    gameOverOverlay->setStyleSheet("background-color: rgba(0, 0, 0, 0.7);");
    gameOverOverlay->setVisible(false);

    QVBoxLayout *gameOverOverlayLayout = new QVBoxLayout(gameOverOverlay);
    gameOverOverlayLayout->setAlignment(Qt::AlignCenter);
    gameOverOverlayLayout->setSpacing(30);

    gameOverLabel = new QLabel(gameOverOverlay);
    gameOverLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            font: bold 48pt "Comic Sans MS", Arial, sans-serif;
            text-align: center;
        }
    )");
    gameOverLabel->setAlignment(Qt::AlignCenter);
    gameOverOverlayLayout->addWidget(gameOverLabel);

    gameOverInfoLabel = new QLabel(gameOverOverlay);
    gameOverInfoLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            font: bold 24pt "Comic Sans MS", Arial, sans-serif;
            text-align: center;
        }
    )");
    gameOverInfoLabel->setAlignment(Qt::AlignCenter);
    gameOverOverlayLayout->addWidget(gameOverInfoLabel);

    returnToMenuBtn = new QPushButton("Return to Menu", gameOverOverlay);
    returnToMenuBtn->setFixedSize(200, 50);
    returnToMenuBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #FF6B9E;
            color: white;
            border: none;
            border-radius: 10px;
            font: bold 18pt "Comic Sans MS", Arial, sans-serif;
        }
        QPushButton:hover {
            background-color: #FF8AB5;
        }
        QPushButton:pressed {
            background-color: #E55A8A;
        }
    )");

    gameOverOverlayLayout->addWidget(returnToMenuBtn, 0, Qt::AlignCenter);

    connect(returnToMenuBtn, &QPushButton::clicked, this, &TwoPlayersGamePage::Exit);


    //// "Enter Map Name" Overlay ////

    enterMapNameOverlay = new QWidget(this);
    enterMapNameOverlay->setStyleSheet("background-color: rgba(0, 0, 0, 0.7);");
    enterMapNameOverlay->setVisible(false);

    QVBoxLayout *enterMapNameLayout = new QVBoxLayout(enterMapNameOverlay);
    enterMapNameLayout->setAlignment(Qt::AlignCenter);
    enterMapNameLayout->setSpacing(30);

    enterMapNameLabel = new QLabel(enterMapNameOverlay);
    enterMapNameLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            font: bold 36pt "Comic Sans MS", Arial, sans-serif;
            text-align: center;
        }
    )");
    enterMapNameLabel->setText("Enter Your Map Name!");
    enterMapNameLabel->setAlignment(Qt::AlignCenter);
    enterMapNameLayout->addWidget(enterMapNameLabel);

    enterMapNameLineEdit = new QLineEdit(enterMapNameOverlay);
    enterMapNameLineEdit->setFixedSize(400, 60);
    enterMapNameLineEdit->setStyleSheet(R"(
        QLineEdit {
            background-color: rgba(255, 255, 255, 0.9);
            color: black;
            border: 2px solid #FF6B9E;
            border-radius: 10px;
            font: bold 24pt "Comic Sans MS", Arial, sans-serif;
            padding: 5px;
        }
        QLineEdit:focus {
            border: 3px solid #FF8AB5;
        }
    )");
    enterMapNameLineEdit->setAlignment(Qt::AlignCenter);
    enterMapNameLineEdit->setMaxLength(20);
    enterMapNameLineEdit->setText("Untitled Map");
    enterMapNameLayout->addWidget(enterMapNameLineEdit, 0, Qt::AlignCenter);

    saveMapBtn = new QPushButton("Save Map", enterMapNameOverlay);
    saveMapBtn->setFixedSize(200, 50);
    saveMapBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #FF6B9E;
            color: white;
            border: none;
            border-radius: 10px;
            font: bold 18pt "Comic Sans MS", Arial, sans-serif;
        }
        QPushButton:hover {
            background-color: #FF8AB5;
        }
        QPushButton:pressed {
            background-color: #E55A8A;
        }
    )");
    enterMapNameLayout->addWidget(saveMapBtn, 0, Qt::AlignCenter);

    connect(saveMapBtn, &QPushButton::clicked, this, [this]() {
        QString mapName = enterMapNameLineEdit->text().trimmed();
        if (mapName.isEmpty()) mapName = "Untitled Map";
        SaveMap(mapName);
    });
}

TwoPlayersGamePage::~TwoPlayersGamePage()
{
    if (boards[0])
    {
        delete boards[0];
        delete boards[1];
        boards[0] = boards[1] = nullptr;
    }
}

void TwoPlayersGamePage::NewGame(const GameSettings &settings)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    if (boards[0])
    {
        delete boards[0];
        delete boards[1];
        boards[0] = boards[1] = nullptr;
    }
    boards[0] = new GameBoard(this);
    boards[1] = new GameBoard(this);
    boards[0]->opponent_board = boards[1];
    boards[1]->opponent_board = boards[0];

    connect(boards[0], &GameBoard::LinkSuccessSignal, this, [this](const std::vector<Coord> &path) {
        this->DrawPath(0, path);
        this->UpdateBlocksLeft();
        qDebug() << "Player 1 Link success. Path size:" << path.size();
    });

    connect(boards[1], &GameBoard::LinkSuccessSignal, this, [this](const std::vector<Coord> &path) {
        this->DrawPath(1, path);
        this->UpdateBlocksLeft();
        qDebug() << "Player 2 Link success. Path size:" << path.size();
    });

    connect(boards[0], &GameBoard::GameOverSignal, this, [this](bool win, QString info) {
        this->ShowGameOverOverlay(win, 1, info);
    });

    connect(boards[1], &GameBoard::GameOverSignal, this, [this](bool win, QString info) {
        this->ShowGameOverOverlay(win, 2, info);
    });

    boards[0]->NewGame(settings);
    boards[1]->NewGame(settings);

    LoadAll();
    timer->start(100);
}

void TwoPlayersGamePage::LoadGame(const QString &mapName)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    if (boards[0])
    {
        delete boards[0]; delete boards[1];
        boards[0] = boards[1] = nullptr;
    }
    boards[0] = new GameBoard(this);
    boards[1] = new GameBoard(this);
    boards[0]->opponent_board = boards[1];
    boards[1]->opponent_board = boards[0];

    connect(boards[0], &GameBoard::LinkSuccessSignal, this, [this](const std::vector<Coord> &path) {
        this->DrawPath(0, path);
        this->UpdateBlocksLeft();
        qDebug() << "Player 1 Link success. Path size:" << path.size();
    });

    connect(boards[1], &GameBoard::LinkSuccessSignal, this, [this](const std::vector<Coord> &path) {
        this->DrawPath(1, path);
        this->UpdateBlocksLeft();
        qDebug() << "Player 2 Link success. Path size:" << path.size();
    });

    connect(boards[0], &GameBoard::GameOverSignal, this, [this](bool win, QString info) {
        this->ShowGameOverOverlay(win, 1, info);
    });

    connect(boards[1], &GameBoard::GameOverSignal, this, [this](bool win, QString info) {
        this->ShowGameOverOverlay(win, 2, info);
    });

    QString mapPath = FileStreamer::FindDir("maps") + "/" + mapName + ".json";
    QJsonObject jsonObject = FileStreamer::ReadJsonObject(mapPath);
    boards[0]->LoadMap(jsonObject["p1"].toObject());
    boards[1]->LoadMap(jsonObject["p2"].toObject());
    LoadAll();
    timer->start(100);
}

void TwoPlayersGamePage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    for (int i = 0; i < 2; i++)
    {
        if (scenes[i]->sceneRect().isValid())
        {
            views[i]->fitInView(scenes[i]->sceneRect(), Qt::KeepAspectRatio);
        }
    }
}

void TwoPlayersGamePage::keyPressEvent(QKeyEvent *event)
{
    Player *player1 = boards[0]->player;
    Player *player2 = boards[1]->player;

    switch (event->key())
    {
    case Qt::Key_S:
        if (boards[0]->freezeTimeLeftMs > 0) break;
        if (boards[0]->dizzyTimeLeftMs > 0) player1->Move(UP);
        else player1->Move(DOWN);
        break;

    case Qt::Key_A:
        if (boards[0]->freezeTimeLeftMs > 0) break;
        if (boards[0]->dizzyTimeLeftMs > 0) player1->Move(RIGHT);
        else player1->Move(LEFT);
        break;

    case Qt::Key_D:
        if (boards[0]->freezeTimeLeftMs > 0) break;
        if (boards[0]->dizzyTimeLeftMs > 0) player1->Move(LEFT);
        else player1->Move(RIGHT);
        break;

    case Qt::Key_W:
        if (boards[0]->freezeTimeLeftMs > 0) break;
        if (boards[0]->dizzyTimeLeftMs > 0) player1->Move(DOWN);
        else player1->Move(UP);
        break;

    case Qt::Key_Down:
        if (boards[1]->freezeTimeLeftMs > 0) break;
        if (boards[1]->dizzyTimeLeftMs > 0) player2->Move(UP);
        else player2->Move(DOWN);
        break;

    case Qt::Key_Left:
        if (boards[1]->freezeTimeLeftMs > 0) break;
        if (boards[1]->dizzyTimeLeftMs > 0) player2->Move(RIGHT);
        else player2->Move(LEFT);
        break;

    case Qt::Key_Right:
        if (boards[1]->freezeTimeLeftMs > 0) break;
        if (boards[1]->dizzyTimeLeftMs > 0) player2->Move(LEFT);
        else player2->Move(RIGHT);
        break;

    case Qt::Key_Up:
        if (boards[1]->freezeTimeLeftMs > 0) break;
        if (boards[1]->dizzyTimeLeftMs > 0) player2->Move(DOWN);
        else player2->Move(UP);
        break;

    case Qt::Key_Escape: case Qt::Key_Delete:
        Pause();
        break;

    default:
        QWidget::keyPressEvent(event);
        return;
    }

    LoadAll();
}

void TwoPlayersGamePage::LoadAll()
{
    LoadBoards();
    LoadPlayers();
    LoadMisc();
    UpdateBlocksLeft();
    UpdateTimeLeft();
}

void TwoPlayersGamePage::LoadBoards()
{
    for (int i = 0; i < 2; ++i)
    {
        GameBoard *b = boards[i];
        QGraphicsScene *scene = scenes[i];
        QGraphicsView *view = views[i];

        if (!b || !scene || !view) continue;

        QList<QGraphicsItem*> itemsToRemove;
        for (QGraphicsItem* item : scene->items())
        {
            if (qgraphicsitem_cast<QGraphicsPixmapItem*>(item) && item->zValue() < 10)
                itemsToRemove.append(item);
        }
        for (QGraphicsItem* item : itemsToRemove)
        {
            scene->removeItem(item);
            delete item;
        }

        pathItems[i] = nullptr;

        GameSettings &settings = b->settings;
        int theme_idx = -1;
        for (unsigned int t = 0; t < textureLoader->themes.size(); ++t)
        {
            if (textureLoader->themes[t].name == settings.theme_name)
            {
                theme_idx = (int)t;
                break;
            }
        }
        if (theme_idx == -1) continue;
        TextureTheme theme = textureLoader->themes[theme_idx];

        for (unsigned int y = 1; y <= b->board_height; ++y)
        {
            for (unsigned int x = 1; x <= b->board_width; ++x)
            {
                QPixmap scaledBlockPixmap = theme.blocks[b->cells[x][y].blockType].scaled(
                    cellSize, cellSize,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    );

                QGraphicsPixmapItem *blockItem = scene->addPixmap(scaledBlockPixmap);
                blockItem->setPos((x-1) * (cellSize + spacing), (y-1) * (cellSize + spacing));
                blockItem->setTransformationMode(Qt::SmoothTransformation);

                int propType = b->cells[x][y].propType;
                if (propType)
                {
                    QPixmap scaledPropPixmap = textureLoader->props[propType].scaled(
                        cellSize, cellSize,
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation
                        );
                    QGraphicsPixmapItem *propItem = scene->addPixmap(scaledPropPixmap);
                    propItem->setPos((x-1) * (cellSize + spacing), (y-1) * (cellSize + spacing));
                    propItem->setTransformationMode(Qt::SmoothTransformation);
                }
            }
        }

        scene->setSceneRect(0, 0,
                            b->board_width * cellSize + (b->board_width - 1) * spacing,
                            b->board_height * cellSize + (b->board_height - 1) * spacing);
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void TwoPlayersGamePage::LoadPlayers()
{
    for (int i = 0; i < 2; ++i)
    {
        GameBoard *b = boards[i];
        QGraphicsScene *scene = scenes[i];
        if (!b || !scene) continue;

        Player *player = b->player;
        if (!player) continue;

        QPixmap scaledPixmap = textureLoader->players[2].texture[player->direction][player->appearance_index].scaled(
            cellSize, cellSize,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );
        QGraphicsPixmapItem *item = scene->addPixmap(scaledPixmap);
        item->setZValue(1);
        item->setPos((player->x - 1) * (cellSize + spacing), (player->y - 1) * (cellSize + spacing));
    }
}

void TwoPlayersGamePage::LoadMisc()
{
    for (int i = 0; i < 2; ++i)
    {
        GameBoard *b = boards[i];
        QGraphicsScene *scene = scenes[i];
        if (!b || !scene) continue;

        Player *player = b->player;
        if (!player) continue;

        // selection
        if (player->select1_x)
        {
            QPixmap scaledPixmap = textureLoader->selectTexture.scaled(
                cellSize, cellSize,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
            QGraphicsPixmapItem *item = scene->addPixmap(scaledPixmap);
            item->setZValue(2);
            item->setPos((player->select1_x - 1) * (cellSize + spacing), (player->select1_y - 1) * (cellSize + spacing));
        }

        // hints
        if (b->hintTimeLeftMs > 0)
        {
            QPixmap scaledPixmap1 = textureLoader->hintSignTexture.scaled(
                cellSize, cellSize,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
            QGraphicsPixmapItem *item1 = scene->addPixmap(scaledPixmap1);
            item1->setZValue(1);
            item1->setPos((b->hintBlock1.x - 1) * (cellSize + spacing), (b->hintBlock1.y - 1) * (cellSize + spacing));

            QPixmap scaledPixmap2 = textureLoader->hintSignTexture.scaled(
                cellSize, cellSize,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
            QGraphicsPixmapItem *item2 = scene->addPixmap(scaledPixmap2);
            item2->setZValue(1);
            item2->setPos((b->hintBlock2.x - 1) * (cellSize + spacing), (b->hintBlock2.y - 1) * (cellSize + spacing));
        }

        // freeze
        if (b->freezeTimeLeftMs > 0)
        {
            QPixmap scaledPixmap1 = textureLoader->freezeEffectTexture.scaled(
                cellSize, cellSize,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
            QGraphicsPixmapItem *item1 = scene->addPixmap(scaledPixmap1);
            item1->setZValue(1);
            item1->setPos((player->x - 1) * (cellSize + spacing), (player->y - 1) * (cellSize + spacing));
        }

        // dizzy
        if (b->dizzyTimeLeftMs > 0)
        {
            QPixmap scaledPixmap1 = textureLoader->dizzyEffectTexture.scaled(
                cellSize, cellSize,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
            QGraphicsPixmapItem *item1 = scene->addPixmap(scaledPixmap1);
            item1->setZValue(1);
            item1->setPos((player->x - 1) * (cellSize + spacing), (player->y - 1) * (cellSize + spacing));
        }
    }

    UpdateBlocksLeft();
    UpdateTimeLeft();
}

void TwoPlayersGamePage::DrawPath(int playerIndex, const std::vector<Coord> &path)
{
    if (path.empty()) return;

    ClearPath(playerIndex);

    QPen pathPen(QColor("#FFAEC9"));
    pathPen.setWidth(6);
    pathPen.setCapStyle(Qt::RoundCap);
    pathPen.setJoinStyle(Qt::RoundJoin);

    QPainterPath painterPath;

    for (size_t i = 0; i < path.size(); ++i)
    {
        const Coord &coord = path[i];
        qreal x = (coord.x - 1) * (cellSize + spacing) + cellSize / 2;
        qreal y = (coord.y - 1) * (cellSize + spacing) + cellSize / 2;

        if (i == 0)
            painterPath.moveTo(x, y);
        else
            painterPath.lineTo(x, y);
    }

    pathItems[playerIndex] = scenes[playerIndex]->addPath(painterPath, pathPen);
    pathItems[playerIndex]->setZValue(10);
    pathItems[playerIndex]->setOpacity(1.0);

    StartFadeAnimation(playerIndex);
}

void TwoPlayersGamePage::ClearPath(int playerIndex)
{
    if (!pathItems[playerIndex]) return;

    scenes[playerIndex]->removeItem(pathItems[playerIndex]);
    delete pathItems[playerIndex];
    pathItems[playerIndex] = nullptr;
}

void TwoPlayersGamePage::StartFadeAnimation(int playerIndex)
{
    if (!pathItems[playerIndex]) return;

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    pathItems[playerIndex]->setGraphicsEffect(effect);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(effect, "opacity");
    fadeAnimation->setDuration(1000);
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);
    fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(fadeAnimation, &QPropertyAnimation::finished, this, [this, playerIndex]() {
        this->ClearPath(playerIndex);
    });

    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void TwoPlayersGamePage::UpdateBlocksLeft()
{
    p1BlocksLeftLabel->setText(QString("Blocks Left: %1").arg(boards[0]->remaining_blocks));
    p2BlocksLeftLabel->setText(QString("Blocks Left: %1").arg(boards[1]->remaining_blocks));
}

void TwoPlayersGamePage::UpdateTimer()
{
    if (boards[0]->timeLeftMs > 0 && boards[1]->timeLeftMs > 0)
    {
        boards[0]->timeLeftMs -= 100;
        boards[1]->timeLeftMs -= 100;
        UpdateTimeLeft();

        if (boards[0]->hintTimeLeftMs > 0)
            boards[0]->hintTimeLeftMs -= 100;
        if (boards[1]->hintTimeLeftMs > 0)
            boards[1]->hintTimeLeftMs -= 100;

        if (boards[0]->freezeTimeLeftMs > 0)
            boards[0]->freezeTimeLeftMs -= 100;
        if (boards[1]->freezeTimeLeftMs > 0)
            boards[1]->freezeTimeLeftMs -= 100;

        if (boards[0]->dizzyTimeLeftMs > 0)
            boards[0]->dizzyTimeLeftMs -= 100;
        if (boards[1]->dizzyTimeLeftMs > 0)
            boards[1]->dizzyTimeLeftMs -= 100;
    }
    else
    {
        timer->stop();

        if (boards[0]->timeLeftMs <= 0 && boards[1]->timeLeftMs <= 0)
        {
            ShowGameOverOverlay(true, 3, "Tie");
        }
        else
        {
            if (boards[0]->timeLeftMs <= 0)
                boards[0]->GameLose("Time Out!");
            if (boards[1]->timeLeftMs <= 0)
                boards[1]->GameLose("Time Out!");
        }
    }
}

void TwoPlayersGamePage::UpdateTimeLeft()
{
    auto formatTime = [](int ms) {
        int totalSeconds = ms / 1000;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        int deciseconds = (ms % 1000) / 100;
        return QString("%1:%2.%3")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
            .arg(deciseconds);
    };

    p1TimeLeftLabel->setText(QString("P1 Time: %1").arg(formatTime(std::max(0, boards[0]->timeLeftMs))));
    p2TimeLeftLabel->setText(QString("P2 Time: %1").arg(formatTime(std::max(0, boards[1]->timeLeftMs))));
}

void TwoPlayersGamePage::ShowGameOverOverlay(bool win, int playerIndex, QString info)
{
    timer->stop();

    QString resultText;
    QString infoText = info;

    int winner;

    if (playerIndex == 3)
    {
        resultText = "Tie!";
        infoText = "No one loses~";
    }
    else
    {
        if (win)
        {
            if (playerIndex == 1) winner = 1;
            else winner = 2;
        }
        else
        {
            if (playerIndex == 1) winner = 2;
            else winner = 1;
            infoText = QString("Player %1 loses: ").arg(3 - winner) + infoText;
        }
        resultText = QString("Player %1 wins!").arg(winner);
    }

    gameOverLabel->setText(resultText);
    gameOverInfoLabel->setText(infoText);

    gameOverOverlay->setGeometry(0, 0, width(), height());
    gameOverOverlay->raise();
    gameOverOverlay->setVisible(true);

    setFocusPolicy(Qt::NoFocus);
}

void TwoPlayersGamePage::Pause()
{
    timer->stop();
    boards[0]->Pause();
    boards[1]->Pause();
    pauseOverlay->setVisible(true);
    pauseOverlay->setGeometry(0, 0, width(), height());
}

void TwoPlayersGamePage::Resume()
{
    timer->start(100);
    boards[0]->Resume();
    boards[1]->Resume();
    pauseOverlay->setVisible(false);
}

void TwoPlayersGamePage::SaveAndExit()
{
    pauseOverlay->setVisible(false);
    enterMapNameOverlay->setVisible(true);
    enterMapNameOverlay->setGeometry(0, 0, width(), height());
}

void TwoPlayersGamePage::SaveMap(QString mapName)
{
    QString mapsDir = FileStreamer::FindDir("maps");
    if (mapsDir == QString()) return;

    pauseOverlay->setVisible(false);
    enterMapNameOverlay->setVisible(false);
    gameOverOverlay->setVisible(false);

    QJsonObject jsonObject;
    QJsonObject jsonObject1 = boards[0]->SaveMap(mapName);
    QJsonObject jsonObject2 = boards[1]->SaveMap(mapName);
    jsonObject["player_cnt"] = 2;
    jsonObject["p1"] = jsonObject1;
    jsonObject["p2"] = jsonObject2;

    QString mapPath = mapsDir + "/" + mapName + ".json";
    QFileInfo fileInfo(mapPath);
    int num = 1;
    while (fileInfo.exists())
    {
        mapPath = mapsDir + "/" + mapName + " (" + QString::number(num) + ").json";
        fileInfo = QFileInfo(mapPath);
        num++;
    }

    FileStreamer::WriteJsonObject(mapPath, jsonObject);

    emit ReturnToMenuSignal();
}

void TwoPlayersGamePage::Exit()
{
    pauseOverlay->setVisible(false);
    enterMapNameOverlay->setVisible(false);
    gameOverOverlay->setVisible(false);
    emit ReturnToMenuSignal();
}
