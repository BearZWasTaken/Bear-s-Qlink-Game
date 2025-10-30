#ifndef FILESTREAMER_H
#define FILESTREAMER_H

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class FileStreamer
{
public:
    FileStreamer();

    static QString FindDir(QString dirName);
    static void WriteJsonObject(QString filePath, QJsonObject &jsonObject);
    static QJsonObject ReadJsonObject(QString filePath);
};

#endif // FILESTREAMER_H
