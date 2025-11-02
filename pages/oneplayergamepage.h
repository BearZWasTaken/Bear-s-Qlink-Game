#pragma once

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

class OnePlayerGamePage : public QWidget
{
    Q_OBJECT

public:
    explicit OnePlayerGamePage(QWidget *parent, TextureLoader *textureLoader);
    ~OnePlayerGamePage();

    void NewGame(const GameSettings &settings);
    void LoadGame(const QString &mapName);

private:
    GameBoard *board = nullptr;
    TextureLoader *textureLoader;

    QWidget *boardWidget;
    QGridLayout *boardLayout;

    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsPathItem *pathItem;

    QLabel *blocksLeftLabel;
    QLabel *timeLeftLabel;
    QPushButton *pauseBtn;

    QTimer *timer;

    QWidget *gameOverOverlay;
    QLabel *gameOverLabel;
    QLabel *gameOverInfoLabel;
    QPushButton *returnToMenuBtn;

    QWidget *pauseOverlay;
    QLabel *pauseLabel;
    QPushButton *resumeBtn;
    QPushButton *saveAndExitBtn;
    QPushButton *discardAndExitBtn;

    QWidget *enterMapNameOverlay;
    QLabel *enterMapNameLabel;
    QLineEdit *enterMapNameLineEdit;
    QPushButton *saveMapBtn;

    const int cellSize = 100;
    const int spacing = 2;

    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void LoadAll();
    void LoadBoard();
    void LoadPlayer();
    void LoadMisc();
    void DrawPath(const std::vector<Coord> &path);
    void ClearPath();
    void StartFadeAnimation();

    void DrawHint(const Coord &block1, const Coord &block2);

    void UpdateBlocksLeft();
    void UpdateTimer();
    void UpdateTimeLeft();
    void ShowGameOverOverlay(bool win, QString info);

    void Pause();
    void Resume();
    void SaveAndExit();
    void SaveMap(QString mapName);
    void Exit();

signals:
    void GameOverSignal(bool win, QString info);
    void ReturnToMenuSignal();
};
