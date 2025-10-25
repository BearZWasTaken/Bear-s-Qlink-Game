#ifndef PLAYER_H
#define PLAYER_H

#include "gamesettings.h"

class GameBoard;

class Player
{
public:
    Player(GameBoard *board);

    GameBoard *board;
    int x, y;
    Direction direction = DOWN;
    int select1_x = 0, select1_y = 0;
    int select2_x = 0, select2_y = 0;
    int appearance_index = 0;

    void Move(Direction direc);
    void NextAppearance();
    void ResetAppearance();

private:
    void SelectBlock(int x, int y);
};

#endif // PLAYER_H
