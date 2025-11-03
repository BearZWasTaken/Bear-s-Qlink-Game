#include "simpletest.h"

#include "gameboard.h"
#include "player.h"

SimpleTest::SimpleTest() {}
SimpleTest::~SimpleTest() {}

GameBoard* SimpleTest::createTestBoard(int width, int height)
{
    GameBoard* board = new GameBoard();
    board->board_width = width;
    board->board_height = height;
    return board;
}

void SimpleTest::setCell(GameBoard* board, int x, int y, int blockType)
{
    if (x >= 1 && x <= board->board_width && y >= 1 && y <= board->board_height)
        board->cells[x][y].blockType = blockType;
}

bool SimpleTest::canLink(GameBoard* board, int x1, int y1, int x2, int y2)
{
    std::vector<Coord> path;
    return board->canLink(x1, y1, x2, y2, path);
}

void SimpleTest::testDirectHorizontalConnection()
{
    GameBoard* board = createTestBoard(3, 3);
    setCell(board, 1, 1, 1);
    setCell(board, 1, 3, 1);
    QVERIFY(canLink(board, 1, 1, 1, 3));
    delete board;
}

void SimpleTest::testDirectVerticalConnection()
{
    GameBoard* board = createTestBoard(3, 3);
    setCell(board, 1, 1, 1);
    setCell(board, 3, 1, 1);
    QVERIFY(canLink(board, 1, 1, 3, 1));
    delete board;
}

void SimpleTest::testOneCornerConnection()
{
    GameBoard* board = createTestBoard(3, 3);
    setCell(board, 1, 1, 1);
    setCell(board, 2, 3, 1);
    QVERIFY(canLink(board, 1, 1, 2, 3));
    delete board;
}

void SimpleTest::testTwoCornerConnection()
{
    GameBoard* board = createTestBoard(4, 4);
    setCell(board, 1, 1, 1);
    setCell(board, 1, 2, 2);
    setCell(board, 4, 1, 2);
    setCell(board, 4, 4, 1);
    QVERIFY(canLink(board, 1, 1, 4, 4));
    delete board;
}

void SimpleTest::testBlockedPath()
{
    GameBoard* board = createTestBoard(3, 3);
    setCell(board, 1, 1, 1);
    setCell(board, 1, 2, 0);
    setCell(board, 1, 3, 2);
    setCell(board, 2, 1, 0);
    setCell(board, 2, 2, 2);
    setCell(board, 2, 3, 0);
    setCell(board, 3, 1, 2);
    setCell(board, 3, 2, 0);
    setCell(board, 3, 3, 1);
    QVERIFY(!canLink(board, 1, 1, 3, 3));
    delete board;
}

void SimpleTest::testDifferentTypes()
{
    GameBoard* board = createTestBoard(2, 2);
    setCell(board, 1, 1, 1);
    setCell(board, 1, 2, 2);
    QVERIFY(!canLink(board, 1, 1, 1, 2));
    delete board;
}

void SimpleTest::testSameTypeButNoPath()
{
    GameBoard* board = createTestBoard(4, 4);
    setCell(board, 1, 1, 1);
    setCell(board, 1, 2, 2);
    setCell(board, 3, 4, 2);
    setCell(board, 4, 1, 2);
    setCell(board, 4, 4, 1);
    QVERIFY(!canLink(board, 1, 1, 4, 4));
    delete board;
}

void SimpleTest::testEdgeCases()
{
    GameBoard* board = createTestBoard(1, 1);
    setCell(board, 1, 1, 1);
    QVERIFY(!canLink(board, 1, 1, 1, 1));
    GameBoard* board2 = createTestBoard(2, 2);
    setCell(board2, 1, 1, 1);
    setCell(board2, 2, 2, 1);
    QVERIFY(!canLink(board2, 1, 1, 3, 3));
    delete board;
    delete board2;
}

void SimpleTest::testEmptyBoard()
{
    GameBoard* board = createTestBoard(0, 0);
    QVERIFY(!canLink(board, 0, 0, 0, 0));
    delete board;
}

void SimpleTest::testComplexPath()
{
    GameBoard* board = createTestBoard(5, 5);

    setCell(board, 1, 1, 1);
    setCell(board, 1, 2, 0);
    setCell(board, 1, 3, 2);
    setCell(board, 1, 4, 0);
    setCell(board, 1, 5, 0);

    setCell(board, 2, 1, 0);
    setCell(board, 2, 2, 0);
    setCell(board, 2, 3, 0);
    setCell(board, 2, 4, 2);
    setCell(board, 2, 5, 0);

    setCell(board, 3, 1, 0);
    setCell(board, 3, 2, 0);
    setCell(board, 3, 3, 0);
    setCell(board, 3, 4, 0);
    setCell(board, 3, 5, 0);

    setCell(board, 4, 1, 0);
    setCell(board, 4, 2, 2);
    setCell(board, 4, 3, 0);
    setCell(board, 4, 4, 0);
    setCell(board, 4, 5, 0);

    setCell(board, 5, 1, 0);
    setCell(board, 5, 2, 0);
    setCell(board, 5, 3, 0);
    setCell(board, 5, 4, 2);
    setCell(board, 5, 5, 1);

    QVERIFY(canLink(board, 1, 1, 5, 5));
    delete board;
}

QTEST_MAIN(SimpleTest)
