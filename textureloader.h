#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QWidget>
#include <QDir>
#include <vector>

struct TextureTheme
{
    QString name;
    std::vector<QPixmap> blocks;
};

struct PlayerTexture
{
    QString name;
    QPixmap texture[4][4];
};

class TextureLoader
{
public:
    TextureLoader();
    void LoadAll();
    void LoadBlocks(const QString &dirPath, std::vector<QPixmap> &pixmap);
    void LoadThemes(const QString &rootPath);
    void LoadPlayers(const QString &dirPath);
    void LoadProps(const QString &dirPath);
    void LoadMisc(const QString &dirPath);
    QPixmap ExtractSprite(const QPixmap &spriteSheet, int row, int col, int frameWidth, int frameHeight);

    std::vector<TextureTheme> themes;
    std::vector<PlayerTexture> players;
    QPixmap props[6];
    QPixmap selectTexture;
    QPixmap hintSignTexture;
};

#endif // TEXTURELOADER_H
