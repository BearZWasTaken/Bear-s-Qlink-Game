#ifndef TWOPLAYERSGAMEPAGE_H
#define TWOPLAYERSGAMEPAGE_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

struct GameSettings;
class GameBoard;
class TextureLoader;
struct Coord;

class TwoPlayersGamePage : public QWidget
{
    Q_OBJECT

private:
    GameBoard *boards[2] = {nullptr, nullptr};
    TextureLoader *textureLoader;

    QGraphicsScene *scenes[2];
    QGraphicsView *views[2];
    QGraphicsPathItem *pathItems[2];

    QTimer *timer = nullptr;

    QLabel *p1BlocksLeftLabel = nullptr;
    QLabel *p1TimeLeftLabel = nullptr;
    QLabel *p2BlocksLeftLabel = nullptr;
    QLabel *p2TimeLeftLabel = nullptr;
    QPushButton *pauseBtn = nullptr;

    const int cellSize = 100;
    const int spacing = 2;

    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void LoadAll();
    void LoadBoards();
    void LoadPlayers();
    void LoadMisc();

    void UpdateTimer();
    void UpdateTimeLeft();

public:
    explicit TwoPlayersGamePage(QWidget *parent, TextureLoader *textureLoader);
    ~TwoPlayersGamePage();

    void NewGame(const GameSettings &settings);
    void LoadGame(const QString &mapName);
};

#endif // TWOPLAYERSGAMEPAGE_H
