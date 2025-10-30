#include "twoplayersgamepage.h"

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

    // ---- Player 1 ----
    p1BlocksLeftLabel = new QLabel("P1 Blocks: --", topBar);
    p1BlocksLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    p1BlocksLeftLabel->setAlignment(Qt::AlignLeft);
    topLayout->addWidget(p1BlocksLeftLabel);

    p1TimeLeftLabel = new QLabel("P1 Time: --:--.-", topBar);
    p1TimeLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    p1TimeLeftLabel->setAlignment(Qt::AlignLeft);
    topLayout->addWidget(p1TimeLeftLabel, 1);

    // ---- Player 2 ----
    p2BlocksLeftLabel = new QLabel("P2 Blocks: --", topBar);
    p2BlocksLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    p2BlocksLeftLabel->setAlignment(Qt::AlignRight);
    topLayout->addWidget(p2BlocksLeftLabel);

    p2TimeLeftLabel = new QLabel("P2 Time: --:--.-", topBar);
    p2TimeLeftLabel->setStyleSheet("QLabel { color: black; font-size: 16px; font-weight: bold; }");
    p2TimeLeftLabel->setAlignment(Qt::AlignRight);
    topLayout->addWidget(p2TimeLeftLabel, 1);

    // ---- Pause ----
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
    topLayout->addWidget(pauseBtn);

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

    boards[0]->NewGame(settings);
    boards[1]->NewGame(settings);

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
        player1->Move(DOWN);
        break;

    case Qt::Key_A:
        player1->Move(LEFT);
        break;

    case Qt::Key_D:
        player1->Move(RIGHT);
        break;

    case Qt::Key_W:
        player1->Move(UP);
        break;

    case Qt::Key_Down:
        player1->Move(DOWN);
        break;

    case Qt::Key_Left:
        player1->Move(LEFT);
        break;

    case Qt::Key_Right:
        player1->Move(RIGHT);
        break;

    case Qt::Key_Up:
        player1->Move(UP);
        break;

    case Qt::Key_Escape: case Qt::Key_Delete:
        // Pause();
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
    // LoadPlayers();
    LoadMisc();
    // UpdateBlocksLeft();
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
    }

    // UpdateBlocksLeft();
    UpdateTimeLeft();
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
    }
    else
    {
        timer->stop();

        if (boards[0]->timeLeftMs <= 0 && boards[1]->timeLeftMs <= 0)
        {

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

    if (boards[0] && p1TimeLeftLabel)
        p1TimeLeftLabel->setText(QString("P1 Time: %1").arg(formatTime(std::max(0, boards[0]->timeLeftMs))));

    if (boards[1] && p2TimeLeftLabel)
        p2TimeLeftLabel->setText(QString("P2 Time: %1").arg(formatTime(std::max(0, boards[1]->timeLeftMs))));
}
