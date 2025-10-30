#ifndef SELECTMAPPAGE_H
#define SELECTMAPPAGE_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

#include <vector>

struct GameSettings;
class GameBoard;
class TextureLoader;
struct Coord;

class SelectMapPage : public QWidget
{
    Q_OBJECT

private:
    TextureLoader *textureLoader;
    QColor color1, color2;
    double phase = 0.0;

    struct MapOption
    {
        QString name;
        QString theme;
        int board_width, board_height;
        float timeLeft;
        int remaining_blocks;
    };
    std::vector<MapOption> mapOptions;

    QTableWidget *mapTable;
    QPushButton *backBtn;
    QPushButton *loadBtn;

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    explicit SelectMapPage(QWidget *parent, TextureLoader *textureLoader);

    void LoadMapList();
    void BackToMenu();
    void LoadMap(QString mapName);

private:
    void PopulateMapTable();

signals:
    void BackToMenuSignal();
    void LoadMapSignal(const QString &mapName);
};

#endif // SELECTMAPPAGE_H
