#include "gamepage.h"

#include "gamesettings.h"
#include "gameboard.h"
#include "textureloader.h"
#include "player.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

GamePage::GamePage(QWidget *parent, TextureLoader *textureLoader)
    : QWidget(parent), textureLoader(textureLoader)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);


    //// Top Bar ////

    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(80);
    topBar->setStyleSheet("background-color: transparent;");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);
    topLayout->setSpacing(20);

    blocksLeftLabel = new QLabel("Blocks Left: --", topBar);
    blocksLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    topLayout->addWidget(blocksLeftLabel);

    timeLeftLabel = new QLabel("Time: --:--:-", topBar);
    timeLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    timeLeftLabel->setAlignment(Qt::AlignCenter);
    topLayout->addWidget(timeLeftLabel, 1);

    exitBtn = new QPushButton("Exit", topBar);
    exitBtn->setFixedSize(80, 30);
    exitBtn->setStyleSheet(
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
    topLayout->addWidget(exitBtn);

    connect(exitBtn, &QPushButton::clicked, this, &GamePage::Exit);

    mainLayout->addWidget(topBar);

    view = new QGraphicsView(this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setStyleSheet("background-color: transparent; border: none;");

    scene = new QGraphicsScene(view);
    view->setScene(scene);

    mainLayout->addWidget(view, 1);


    //// Timer ////

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GamePage::UpdateTimer);


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

    connect(returnToMenuBtn, &QPushButton::clicked, this, &GamePage::Exit);


    //// "Pause" Overlay ////

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

    connect(saveAndExitBtn, &QPushButton::clicked, this, &GamePage::SaveAndExit);


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

GamePage::~GamePage()
{
    if (board)
    {
        delete board;
        board = nullptr;
    }
}

void GamePage::NewGame(const GameSettings &settings)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    if (board)
    {
        delete board;
        board = nullptr;
    }
    board = new GameBoard(this);
    connect(board, &GameBoard::LinkSuccess, this, [this](const std::vector<Coord> &path) {
        this->DrawPath(path);
        this->UpdateBlocksLeft();
        qDebug() << "Link success.";
    });
    connect(board, &GameBoard::GameOver, this, &GamePage::ShowGameOverOverlay);

    board->NewGame(settings);
    timer->start(100);
    UpdateBlocksLeft();
    UpdateTimeLeft();
    LoadAll();
}

void GamePage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (scene->sceneRect().isValid())
    {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void GamePage::keyPressEvent(QKeyEvent *event)
{
    Player *player = board->player;

    switch (event->key())
    {
    case Qt::Key_S: case Qt::Key_Down:
        player->Move(DOWN);
        break;

    case Qt::Key_A: case Qt::Key_Left:
        player->Move(LEFT);
        break;

    case Qt::Key_D: case Qt::Key_Right:
        player->Move(RIGHT);
        break;

    case Qt::Key_W: case Qt::Key_Up:
        player->Move(UP);
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

void GamePage::LoadAll()
{
    LoadBoard();
    LoadPlayer();
    LoadMisc();
}

void GamePage::LoadBoard()
{
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

    pathItem = nullptr;

    GameSettings &settings = board->settings;
    int theme_idx = -1;
    for (unsigned int i=0; i<textureLoader->themes.size(); ++i)
    {
        if (textureLoader->themes[i].name == settings.theme_name)
        {
            theme_idx = i;
            break;
        }
    }

    if (theme_idx == -1) return;
    TextureTheme theme = textureLoader->themes[theme_idx];

    for (unsigned int y=1; y<=board->board_height; ++y)
    {
        for (unsigned int x=1; x<=board->board_width; ++x)
        {
            QPixmap scaledBlockPixmap = theme.blocks[board->cells[x][y].blockType].scaled(
                cellSize, cellSize,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );

            QGraphicsPixmapItem *blockItem = scene->addPixmap(scaledBlockPixmap);
            blockItem->setPos((x-1) * (cellSize + spacing), (y-1) * (cellSize + spacing));
            blockItem->setTransformationMode(Qt::SmoothTransformation);

            int propType = board->cells[x][y].propType;
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
                        board->board_width * cellSize + (board->board_width-1) * spacing,
                        board->board_height * cellSize + (board->board_height-1) * spacing);
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void GamePage::LoadPlayer()
{
    Player *player = board->player;
    QPixmap scaledPixmap = textureLoader->players[2].texture[player->direction][player->appearance_index].scaled(
        cellSize, cellSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    );
    QGraphicsPixmapItem *item = scene->addPixmap(scaledPixmap);
    item->setZValue(1);
    item->setPos((player->x-1) * (cellSize + spacing), (player->y-1) * (cellSize + spacing));
}

void GamePage::LoadMisc()
{
    Player *player = board->player;
    if (player->select1_x)
    {
        QPixmap scaledPixmap = textureLoader->selectTexture.scaled(
            cellSize, cellSize,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
        QGraphicsPixmapItem *item = scene->addPixmap(scaledPixmap);
        item->setZValue(2);
        item->setPos((player->select1_x-1) * (cellSize + spacing), (player->select1_y-1) * (cellSize + spacing));
    }

    if (board->hintTimeLeftMs > 0)
    {
        QPixmap scaledPixmap1 = textureLoader->hintSignTexture.scaled(
            cellSize, cellSize,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
        QGraphicsPixmapItem *item1 = scene->addPixmap(scaledPixmap1);
        item1->setZValue(1);
        item1->setPos((board->hintBlock1.x-1) * (cellSize + spacing), (player->board->hintBlock1.y-1) * (cellSize + spacing));

        QPixmap scaledPixmap2 = textureLoader->hintSignTexture.scaled(
            cellSize, cellSize,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
        QGraphicsPixmapItem *item2 = scene->addPixmap(scaledPixmap2);
        item2->setZValue(1);
        item2->setPos((board->hintBlock2.x-1) * (cellSize + spacing), (player->board->hintBlock2.y-1) * (cellSize + spacing));
    }
}

void GamePage::DrawPath(const std::vector<Coord> &path)
{
    if (path.empty()) return;

    ClearPath();

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

    pathItem = scene->addPath(painterPath, pathPen);
    pathItem->setZValue(10);
    pathItem->setOpacity(1.0);

    StartFadeAnimation();
}

void GamePage::ClearPath()
{
    if (!pathItem) return;

    scene->removeItem(pathItem);
    delete pathItem;
    pathItem = nullptr;
}

void GamePage::StartFadeAnimation()
{
    if (!pathItem) return;

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    pathItem->setGraphicsEffect(effect);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(effect, "opacity", this);
    fadeAnimation->setDuration(1000);
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);
    fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(fadeAnimation, &QPropertyAnimation::finished, this, [this]() {
        this->ClearPath();
    });

    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GamePage::UpdateBlocksLeft()
{
    blocksLeftLabel->setText(QString("Blocks Left: %1").arg(board->remaining_blocks));
}

void GamePage::UpdateTimer()
{
    if (board->timeLeftMs > 0)
    {
        board->timeLeftMs -= 100;
        UpdateTimeLeft();

        if (board->hintTimeLeftMs > 0)
            board->hintTimeLeftMs -= 100;
    }
    else
    {
        timer->stop();
        board->GameLose("Time Out!");
    }
}

void GamePage::UpdateTimeLeft()
{
    int minute = board->timeLeftMs / 60000;
    int second = board->timeLeftMs % 60000 / 1000;
    int one_tenth_second = board->timeLeftMs % 60000 / 100 % 10;
    timeLeftLabel->setText(QString("Time: %1:%2.%3")
                               .arg(minute, 2, 10, QLatin1Char('0'))
                               .arg(second, 2, 10, QLatin1Char('0'))
                               .arg(one_tenth_second, 1, 10, QLatin1Char('0')));
}

void GamePage::ShowGameOverOverlay(bool win, QString info)
{
    timer->stop();

    if (win)
        gameOverLabel->setText("You win! :)");
    else
        gameOverLabel->setText("You lose! :(");

    gameOverInfoLabel->setText(info);

    gameOverOverlay->setGeometry(0, 0, width(), height());
    gameOverOverlay->raise();
    gameOverOverlay->setVisible(true);
}

void GamePage::Pause()
{
    timer->stop();
    pauseOverlay->setVisible(true);
    pauseOverlay->setGeometry(0, 0, width(), height());
}

void GamePage::SaveAndExit()
{
    pauseOverlay->setVisible(false);
    enterMapNameOverlay->setVisible(true);
    enterMapNameOverlay->setGeometry(0, 0, width(), height());
}

void GamePage::SaveMap(QString mapName)
{
    pauseOverlay->setVisible(false);
    board->SaveMap(mapName);
    emit ReturnToMenu();
}

void GamePage::Exit()
{
    gameOverOverlay->setVisible(false);
    emit ReturnToMenu();
}
