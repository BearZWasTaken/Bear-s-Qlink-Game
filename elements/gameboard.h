#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "gamesettings.h"

#include <QObject>
#include <vector>
#include <list>

class Player;

class GameBoard : public QObject
{
    Q_OBJECT

public:
    struct Cell
    {
        int blockType = 0;
        int propType = NO_PROP;
    };

    struct State
    {
        Coord now;
        int turns;
        Direction direc;
        std::vector<Coord> path;
    };

public:
    unsigned int board_width, board_height;
    GameSettings settings;
    Cell cells[50][50];
    Player *player;
    int remaining_blocks;
    int timeLeftMs;

    int hintTimeLeftMs = 0;
    int dizzyTimeLeftMs = 0;
    int freezeTimeLeftMs = 0;
    Coord hintBlock1, hintBlock2;

    GameBoard *opponent_board = nullptr;

private:
    struct LinkHint
    {
        Coord this_block;
        std::list<Coord> linkable_targets;
    };
    std::list<LinkHint> linkHints;

public:
    GameBoard(QObject *parent = nullptr);
    ~GameBoard();

    void NewGame(const GameSettings &settings);
    QJsonObject SaveMap(QString mapName);
    void LoadMap(const QJsonObject &jsonObject);

    bool hasBlock(const int x, const int y);
    bool hasBlock(const Coord coord);
    bool hasProp(const int x, const int y);
    bool outOfBoard(const int x, const int y);
    void TryLink(const int x1, const int y1, const int x2, const int y2);

    void TriggerProp(const int x, const int y);

    bool isPlaying();
    void Pause();
    void Resume();
    void GameWin();
    void GameLose(QString info);

private:    
    bool playing = true;

    void SpawnPlayer();
    void SpawnBlocks();
    void SpawnProps();

    bool canLink(const int x, const int y, const int dest_x, const int dest_y, std::vector<Coord> &shortest_path);
    bool canLinkFromBlank(const int x, const int y, const int dest_x, const int dest_y, Direction direc, std::vector<Coord> &path);

    void CalcLinkableTargets();
    void dfsGetReachableBlocks(int x, int y, std::vector<std::vector<bool>> &book);
    void RefreshHint();
    bool isHintInvalid(LinkHint &hint);

signals:
    void LinkSuccessSignal(const std::vector<Coord> &path);
    void GameOverSignal(bool win, QString info);
};

#endif // GAMEBOARD_H
