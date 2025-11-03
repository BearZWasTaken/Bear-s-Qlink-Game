#pragma once

#include <QtTest/QtTest>
#include <QObject>

class GameBoard;

class SimpleTest : public QObject
{
    Q_OBJECT

public:
    SimpleTest();
    ~SimpleTest();

private slots:
    void testDirectHorizontalConnection();
    void testDirectVerticalConnection();
    void testOneCornerConnection();
    void testTwoCornerConnection();
    void testBlockedPath();
    void testDifferentTypes();
    void testSameTypeButNoPath();
    void testEdgeCases();
    void testEmptyBoard();
    void testComplexPath();

private:
    GameBoard* createTestBoard(int width, int height);
    void setCell(GameBoard* board, int x, int y, int blockType);
    bool canLink(GameBoard* board, int x1, int y1, int x2, int y2);
};
