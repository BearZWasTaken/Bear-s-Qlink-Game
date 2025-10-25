#include "textureloader.h"

#include "gamesettings.h"

TextureLoader::TextureLoader() {}

void TextureLoader::LoadAll()
{
    QString imagesDir = findImagesDir();
    LoadThemes(imagesDir + "/blocks");
    LoadPlayers(imagesDir + "/players");
    LoadProps(imagesDir + "/props");
    LoadMisc(imagesDir + "/misc");
}

QString TextureLoader::findImagesDir()
{
    QStringList candidates;
    candidates << "./images"
               << "../images"
               << "../../images";

    for (const QString &path : candidates)
    {
        QDir dir(path);
        if (dir.exists())
        {
            qDebug() << "Found images directory:" << dir.absolutePath();
            return dir.absolutePath();
        }
    }

    qWarning() << "Cannot find images directory in any candidate path!";
    return QString();
}

void TextureLoader::LoadBlocks(const QString &dirPath, std::vector<QPixmap> &pixmap)
{
    QDir dir(dirPath);
    if (!dir.exists()) return;

    QStringList filters;
    filters << "*.png" << "*.jpg";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files, QDir::Name);

    for (auto &fileInfo : fileList)
    {
        QPixmap pix(fileInfo.absoluteFilePath());
        if (!pix.isNull())
            pixmap.emplace_back(pix);
    }
}

void TextureLoader::LoadThemes(const QString &rootPath)
{
    QDir rootDir(rootPath);
    if (!rootDir.exists())
    {
        qWarning() << "Root directory does not exist:" << rootPath;
        return;
    }

    QFileInfoList dirList = rootDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    themes.clear();

    for (const QFileInfo &dirInfo : dirList)
    {
        TextureTheme newTheme;
        newTheme.name = dirInfo.fileName();

        QDir themeDir(dirInfo.absoluteFilePath());
        LoadBlocks(themeDir.absolutePath(), newTheme.blocks);

        if (!newTheme.blocks.empty())
        {
            themes.push_back(newTheme);
            qDebug() << "Loaded theme:" << newTheme.name << "with" << newTheme.blocks.size() << "blocks.";
        }
        else
        {
            qDebug() << "Theme" << newTheme.name << "has no blocks.";
        }
    }
}

void TextureLoader::LoadPlayers(const QString &dirPath)
{
    QDir dir(dirPath);
    QStringList filter;
    filter << "*.png";
    QFileInfoList fileList = dir.entryInfoList(filter, QDir::Files, QDir::Name);

    for (auto &fileInfo : fileList)
    {
        QPixmap spriteSheet(fileInfo.absoluteFilePath());
        int frameWidth = spriteSheet.width() / 4;
        int frameHeight = spriteSheet.height() / 4;

        PlayerTexture newPlayer;
        newPlayer.name = fileInfo.baseName();
        for (int direc=0; direc<4; direc++)
            for (int i=0; i<4; i++)
                newPlayer.texture[direc][i] = ExtractSprite(spriteSheet, direc, i, frameWidth, frameHeight);

        players.emplace_back(newPlayer);

        qDebug() << "Loaded player texture:" << newPlayer.name;
    }
}

void TextureLoader::LoadProps(const QString &dirPath)
{
    props[PROP_ADDTIME] = QPixmap(dirPath + "/addTime.png");
    props[PROP_SHUFFLE] = QPixmap(dirPath + "/shuffle.png");
    props[PROP_HINT] = QPixmap(dirPath + "/hint.png");
    props[PROP_FREEZE] = QPixmap(dirPath + "/freeze.png");
    props[PROP_DIZZY] = QPixmap(dirPath + "/dizzy.png");
    qDebug() << "Loaded props texture";
}

void TextureLoader::LoadMisc(const QString &dirPath)
{
    selectTexture = QPixmap(dirPath + "/select.png");
    hintSignTexture = QPixmap(dirPath + "/hint.png");
    qDebug() << "Loaded misc texture";
}

QPixmap TextureLoader::ExtractSprite(const QPixmap &spriteSheet, int row, int col, int frameWidth, int frameHeight)
{
    return spriteSheet.copy(col * frameWidth, row * frameHeight, frameWidth, frameHeight);
}
