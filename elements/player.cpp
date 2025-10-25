#include "player.h"

#include "gameboard.h"
#include <QDebug>

Player::Player(GameBoard *board)
    : board(board)
{}

void Player::Move(Direction direc)
{
    if (!board->isPlaying()) return;

    direction = direc;
    NextAppearance();
    int target_x = x + direction_x[direc];
    int target_y = y + direction_y[direc];

    if (board->outOfBoard(target_x, target_y)) return;

    if (board->hasBlock(target_x, target_y))
    {
        SelectBlock(target_x, target_y);
    }
    else
    {
        x = target_x;
        y = target_y;

        if (board->hasProp(x, y))
        {
            board->TriggerProp(x, y);
        }
    }
}

void Player::NextAppearance()
{
    if (appearance_index == 3)
        appearance_index = 0;
    else
        appearance_index++;
}

void Player::ResetAppearance()
{
    appearance_index = 0;
}

void Player::SelectBlock(int x, int y)
{
    if (!select1_x)
    {
        qDebug() << "Select:" << x << y << "as 1st selection";
        select1_x = x;
        select1_y = y;
    }
    else
    {
        qDebug() << "Select:" << x << y << "as 2nd selection";
        select2_x = x;
        select2_y = y;
        board->TryLink(select1_x, select1_y, select2_x, select2_y);
        select1_x = select1_y = select2_x = select2_y = 0;
    }
}
