#ifndef SIMPLETEST_H
#define SIMPLETEST_H

#include <QObject>
#include <QtTest/QtTest>

class GameBoard;

class SimpleTest : public QObject
{
    Q_OBJECT

public:
    SimpleTest();
    ~SimpleTest();

private:
    GameBoard* createTestBoard(int width, int height);
    void setCell(GameBoard* board, int x, int y, int blockType);
    bool canLink(GameBoard* board, int x1, int y1, int x2, int y2);

private slots:
    void testDirectHorizontalConnection();
    void testDirectVerticalConnection();
    void testOneCornerConnection();
    void testTwoCornerConnection();
    void testBlockedPath();
    void testDifferentTypes();
    void testSameTypeButNoPath();
    void testSameCellRejected();
    void testOutOfBoundsCoordinates();
    void testEmptyBoard();
    void testMinimalBoard();
    void testMaxTurnsExceeded();
    void testComplexPath();
};

#endif // SIMPLETEST_H
