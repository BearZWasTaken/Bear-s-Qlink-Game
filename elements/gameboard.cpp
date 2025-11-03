#include "gameboard.h"

#include "player.h"
#include "filestreamer.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include <algorithm>
#include <random>
#include <ctime>
#include <cassert>
#include <iostream>
#include <queue>

GameBoard::GameBoard(QObject *parent)
    : QObject(parent)
{
    player = new Player(this);
}

GameBoard::~GameBoard()
{
    delete player;
}

void GameBoard::NewGame(const GameSettings &s)
{
    qDebug() << "New Game:" << s.puzzle_width << "*" << s.puzzle_height;

    settings = s;
    timeLeftMs = settings.time_limit * 1000;
    hintTimeLeftMs = 0;

    SpawnPlayer();
    SpawnBlocks();

    CalcLinkableTargets();

    if(settings.enable_props)
        SpawnProps();
}

QJsonObject GameBoard::SaveMap(QString mapName)
{
    QJsonObject jsonObject;
    QJsonArray boardArr;
    for (int j=1; j<=board_height; j++)
    {
        QJsonArray rowArr;
        for (int i=1; i<=board_width; i++)
        {
            QJsonArray cellArr;
            cellArr.append(cells[i][j].blockType);
            cellArr.append(cells[i][j].propType);
            rowArr.append(cellArr);
        }
        boardArr.append(rowArr);
    }
    jsonObject.insert("board", boardArr);

    jsonObject.insert("board_width", (int)board_width);
    jsonObject.insert("board_height", (int)board_height);
    jsonObject.insert("theme_name", settings.theme_name);
    jsonObject.insert("remaining_blocks", remaining_blocks);

    jsonObject.insert("player_x", player->x);
    jsonObject.insert("player_y", player->y);
    jsonObject.insert("select1_x", player->select1_x);
    jsonObject.insert("select1_y", player->select1_y);

    jsonObject.insert("timeLeftMs", timeLeftMs);
    jsonObject.insert("hintTimeLeftMs", hintTimeLeftMs);
    jsonObject.insert("freezeTimeLeftMs", freezeTimeLeftMs);
    jsonObject.insert("dizzyTimeLeftMs", dizzyTimeLeftMs);

    return jsonObject;
}

void GameBoard::LoadMap(const QJsonObject &jsonObject)
{
    if (jsonObject.empty()) return;

    board_width = jsonObject["board_width"].toInt();
    board_height = jsonObject["board_height"].toInt();
    remaining_blocks = jsonObject["remaining_blocks"].toInt();

    settings.theme_name = jsonObject["theme_name"].toString();

    player->x = jsonObject["player_x"].toInt();
    player->y = jsonObject["player_y"].toInt();
    player->select1_x = jsonObject["select1_x"].toInt();
    player->select1_y = jsonObject["select1_y"].toInt();

    timeLeftMs = jsonObject["timeLeftMs"].toInt();
    hintTimeLeftMs = jsonObject["hintTimeLeftMs"].toInt();
    freezeTimeLeftMs = jsonObject["freezeTimeLeftMs"].toInt();
    dizzyTimeLeftMs = jsonObject["dizzyTimeLeftMs"].toInt();

    QJsonArray boardArr = jsonObject["board"].toArray();
    for (int j=1; j<=board_height; j++)
    {
        QJsonArray rowArr = boardArr[j-1].toArray();
        for (int i=1; i<=board_width; i++)
        {
            QJsonArray cellArr = rowArr[i-1].toArray();
            cells[i][j].blockType = cellArr[0].toInt();
            cells[i][j].propType = cellArr[1].toInt();
        }
    }

    qDebug() << "Map loaded successfully";
}

void GameBoard::SpawnPlayer()
{
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist_x(3, settings.puzzle_width-2);
    std::uniform_int_distribution<int> dist_y(3, settings.puzzle_height-2);
    unsigned x = dist_x(rng);
    unsigned y = dist_y(rng);
    if(settings.enable_edge_alleyway) { x++; y++; }
    player->x = x;
    player->y = y;
}

void GameBoard::SpawnBlocks()
{
    unsigned int width = settings.puzzle_width;
    unsigned int height = settings.puzzle_height;
    unsigned int block_types = settings.block_types;


    //// random distribution ////

    unsigned int total = width * height - 9;
    unsigned int block_cnt = (unsigned int)(total * 0.8) /2 *2;
    unsigned int avg_cnt = block_cnt /block_types /2 *2;

    std::vector<unsigned int> counts(block_types+1, avg_cnt);
    counts[0] = total - block_cnt;

    unsigned int used = avg_cnt * block_types;
    unsigned int remaining = block_cnt - used;

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<unsigned int> dist(1, block_types);
    while (remaining > 0)
    {
        unsigned int idx = dist(rng);
        counts[idx] += 2;
        remaining -= 2;
    }

    std::vector<unsigned int> filled;
    filled.reserve(total);
    for (unsigned int i=0; i<=block_types; ++i)
    {
        for (unsigned int j=1; j<=counts[i]; ++j)
            filled.emplace_back(i);
    }
    std::shuffle(filled.begin(), filled.end(), rng);


    //// fill to board ////

    int i_l = 1, i_r = width, j_l = 1, j_r = height;
    if (settings.enable_edge_alleyway) { i_l++; i_r++; j_l++; j_r++; }
    int filled_idx = 0;
    for (int i=i_l; i<=i_r; ++i)
    {
        for (int j=j_l; j<=j_r; ++j)
        {
            if (std::abs(i - player->x) <= 1 && std::abs(j - player->y) <= 1)
                continue;
            cells[i][j].blockType = filled[filled_idx++];
        }
    }

    board_width = width; board_height = height;
    remaining_blocks = block_cnt;
    if (settings.enable_edge_alleyway) { board_width+=2; board_height+=2; }
}

void GameBoard::SpawnProps()
{
    unsigned int blank_cnt = board_width*board_height - remaining_blocks - 1;
    unsigned int prop_cnt = (unsigned int)(blank_cnt * 0.1);
    unsigned int prop_type_cnt;
    if (settings.player_cnt == 1) prop_type_cnt = one_player_props.size();
    else prop_type_cnt = two_players_props.size();
    std::vector<unsigned int> filled;
    filled.reserve(blank_cnt);

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<unsigned int> dist(0, prop_type_cnt-1);

    if (settings.player_cnt == 1)
    {
        for (unsigned int i=1; i<=prop_cnt; i++)
            filled.emplace_back(one_player_props[dist(rng)]);
    }
    else
    {
        for (unsigned int i=1; i<=prop_cnt; i++)
            filled.emplace_back(two_players_props[dist(rng)]);
    }
    for (unsigned int i=1; i<=blank_cnt - prop_cnt; i++)
        filled.emplace_back(NO_PROP);

    std::shuffle(filled.begin(), filled.end(), rng);

    int filled_idx = 0;
    for (int i=1; i<=board_width; i++)
    {
        for (int j=1; j<=board_height; j++)
        {
            if (hasBlock(i, j)) continue;
            if (i == player->x && j == player->y) continue;
            cells[i][j].propType = filled[filled_idx++];
        }
    }
}

bool GameBoard::hasBlock(const int x, const int y)
{
    return cells[x][y].blockType != 0;
}

bool GameBoard::hasBlock(const Coord coord)
{
    return cells[coord.x][coord.y].blockType != 0;
}

bool GameBoard::hasProp(const int x, const int y)
{
    return cells[x][y].propType != 0;
}

bool GameBoard::outOfBoard(const int x, const int y)
{
    if (x <= 0 || x > board_width) return true;
    if (y <= 0 || y > board_height) return true;
    return false;
}

void GameBoard::TryLink(const int x1, const int y1, const int x2, const int y2)
{
    std::vector<Coord> shortest_path;

    if (canLink(x1, y1, x2, y2, shortest_path))
    {
        cells[x1][y1].blockType = 0;
        cells[x2][y2].blockType = 0;
        remaining_blocks -= 2;
        emit LinkSuccessSignal(shortest_path);
        qDebug() << "Linked!";

        if (remaining_blocks == 0)
        {
            GameWin();
        }

        RefreshHint();
    }
}

bool GameBoard::canLink(const int x1, const int y1, const int x2, const int y2, std::vector<Coord> &shortest_path)
{
    bool ok = false;
    if (cells[x1][y1].blockType == cells[x2][y2].blockType && !(x1 == x2 && y1 == y2))
    {
        for (Direction direc : four_directions)
        {
            std::vector<Coord> path;
            path.push_back({x1, y1});
            if (canLinkFromBlank(x1 + direction_x[direc], y1 + direction_y[direc], x2, y2, direc, path))
            {
                ok = true;
                if (shortest_path.empty() || path.size() < shortest_path.size())
                    shortest_path = path;
            }
        }
    }
    return ok;
}

bool GameBoard::canLinkFromBlank(const int x, const int y, const int dest_x, const int dest_y, Direction direc, std::vector<Coord> &path)
{
    if (outOfBoard(x, y)) return false;
    path.push_back({x, y});
    if (x == dest_x && y == dest_y) return true;
    if (hasBlock(x, y)) return false;

    std::queue<State> q;

    State start;
    start.now = {x, y};
    start.turns = 0;
    start.direc = direc;
    start.path = path;
    q.push(start);

    bool book[50][50][4][3] = {{{{false}}}};
    book[x][y][direc][0] = true;

    while (!q.empty())
    {
        State current = q.front();
        q.pop();

        for (Direction next_direc : four_directions)
        {
            if (is_opposite(next_direc, current.direc)) continue;

            int next_x = current.now.x + direction_x[next_direc];
            int next_y = current.now.y + direction_y[next_direc];

            if (outOfBoard(next_x, next_y)) continue;

            int next_turns = current.turns;
            if (next_direc != current.direc)
                next_turns++;

            if (next_turns > 2) continue;

            if (next_x == dest_x && next_y == dest_y)
            {
                current.path.push_back({dest_x, dest_y});
                path = current.path;
                return true;
            }

            if (hasBlock(next_x, next_y)) continue;

            if (book[next_x][next_y][next_direc][next_turns]) continue;
            book[next_x][next_y][next_direc][next_turns] = true;

            State next_state;
            next_state.now = {next_x, next_y};
            next_state.turns = next_turns;
            next_state.direc = next_direc;
            next_state.path = current.path;
            next_state.path.push_back({next_x, next_y});

            q.push(next_state);
        }
    }

    return false;
}

void GameBoard::CalcLinkableTargets()
{
    // Get reachable blocks
    std::vector<std::vector<bool>> book1(board_width+1, std::vector<bool>(board_height+1, false));
    linkHints.clear();
    dfsGetReachableBlocks(player->x, player->y, book1);

    // Get linkable blocks for each reachable block
    auto it1 = linkHints.begin();
    while (it1 != linkHints.end())
    {
        auto it2 = std::next(it1);
        while (it2 != linkHints.end())
        {
            std::vector<Coord> path;
            path.push_back({it1->this_block.x, it1->this_block.y});
            if (canLink(it1->this_block.x, it1->this_block.y, it2->this_block.x, it2->this_block.y, path))
            {
                it1->linkable_targets.push_back(it2->this_block);
                it2->linkable_targets.push_back(it1->this_block);
            }
            ++it2;
        }
        ++it1;
    }

    linkHints.remove_if( [](LinkHint hint){ return hint.linkable_targets.empty(); } );

    if (linkHints.empty() && remaining_blocks > 0)
        GameLose("No links left!");
}

void GameBoard::dfsGetReachableBlocks(int x, int y, std::vector<std::vector<bool>> &book)
{
    if (outOfBoard(x, y)) return;
    if (book[x][y]) return;
    book[x][y] = 1;
    if (hasBlock(x, y))
    {
        LinkHint new_hint;
        new_hint.this_block = {x, y};
        linkHints.push_back(new_hint);
        return;
    }
    for (auto &dire : four_directions)
        dfsGetReachableBlocks(x + direction_x[dire], y + direction_y[dire], book);
}

void GameBoard::TriggerProp(const int x, const int y)
{
    int propType = cells[x][y].propType;
    switch (propType)
    {
        case PROP_ADDTIME:
        {
            timeLeftMs += 30000;
            break;
        }

        case PROP_SHUFFLE:
        {
            player->select1_x = player->select1_y = player->select2_x = player->select2_y = 0;
            std::vector<Coord> existedBlocksPos;
            std::vector<int> existedBlocksType;
            for (int i=1; i<=board_width; i++)
            {
                for (int j=1; j<=board_height; j++)
                {
                    if (hasBlock(i, j))
                    {
                        existedBlocksPos.push_back({i, j});
                        existedBlocksType.emplace_back(cells[i][j].blockType);
                    }
                }
            }
            std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
            std::uniform_int_distribution<unsigned int> dist(1, settings.block_types);
            std::shuffle(existedBlocksType.begin(), existedBlocksType.end(), rng);
            int replaced_cnt = 0;
            for (Coord pos : existedBlocksPos)
                cells[pos.x][pos.y].blockType = existedBlocksType[replaced_cnt++];

            CalcLinkableTargets();
            RefreshHint();

            break;
        }

        case PROP_HINT:
        {
            hintTimeLeftMs = 10000;
            RefreshHint();
            break;
        }

        case PROP_FREEZE:
        {
            opponent_board->freezeTimeLeftMs = 3000;
            break;
        }

        case PROP_DIZZY:
        {
            opponent_board->dizzyTimeLeftMs = 10000;
            break;
        }
    }
    cells[x][y].propType = NO_PROP;
}

void GameBoard::RefreshHint()
{
    linkHints.remove_if( [this](LinkHint &hint){ return isHintInvalid(hint); } );
    if (linkHints.empty())
        CalcLinkableTargets();

    if (!linkHints.empty())
    {
        LinkHint &hint = linkHints.front();
        hintBlock1 = hint.this_block;
        hintBlock2 = hint.linkable_targets.front();
    }
}

bool GameBoard::isHintInvalid(LinkHint &hint)
{
    if (!hasBlock(hint.this_block)) return true;
    hint.linkable_targets.remove_if( [this](Coord &coord){ return !hasBlock(coord); } );
    if (hint.linkable_targets.empty()) return true;
    return false;
}

bool GameBoard::isPlaying()
{
    return playing;
}

void GameBoard::Pause()
{
    playing = false;
}

void GameBoard::Resume()
{
    playing = true;
}

void GameBoard::GameWin()
{
    playing = false;
    emit GameOverSignal(true, "Congrats! 👏");
    qDebug() << "WinWinWin";
}

void GameBoard::GameLose(QString info)
{
    playing = false;
    emit GameOverSignal(false, info);
    qDebug() << "Lose. fvv! Info:" << info;
}
