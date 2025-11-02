#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <QString>
#include <vector>

struct GameSettings
{
    unsigned int player_cnt = 1;
    unsigned int puzzle_width = 10, puzzle_height = 10;
    unsigned int block_types = 5;
    unsigned int time_limit = 100;
    bool enable_edge_alleyway = true;
    bool enable_props = true;
    QString theme_name = "Unknown";
};

struct Coord
{
    int x, y;
};

typedef enum
{
    DOWN = 0,
    LEFT,
    RIGHT,
    UP
} Direction;
static constexpr Direction four_directions[4] = {DOWN, LEFT, RIGHT, UP};
static constexpr int direction_x[4] = { 0,-1, 1, 0};
static constexpr int direction_y[4] = { 1, 0, 0,-1};
static bool is_opposite(Direction x, Direction y) { return x + y == 3; }

typedef enum
{
    NO_PROP = 0,
    PROP_ADDTIME = 1,
    PROP_SHUFFLE,
    PROP_HINT,
    PROP_FREEZE,
    PROP_DIZZY
} PropType;
static const std::vector<PropType> one_player_props = {PROP_ADDTIME, PROP_SHUFFLE, PROP_HINT};
static const std::vector<PropType> two_players_props = {PROP_ADDTIME, PROP_SHUFFLE, PROP_HINT, PROP_FREEZE, PROP_DIZZY};

#endif // GAMESETTINGS_H
