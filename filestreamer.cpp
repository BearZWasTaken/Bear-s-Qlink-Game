#include "filestreamer.h"
\
#include <QStringList>
#include <QDir>

FileStreamer::FileStreamer() {}

QString FileStreamer::FindDir(QString dirName)
{
    QStringList candidates;
    candidates << "./" + dirName
               << "../" + dirName
               << "../../" + dirName
               << "../../../" + dirName;

    for (const QString &path : candidates)
    {
        QDir dir(path);
        if (dir.exists())
        {
            qDebug() << "Found images directory:" << dir.absolutePath();
            return dir.absolutePath();
        }
    }

    qWarning() << "Cannot find" << dirName << "directory in any candidate path!";
    return QString();
}

void FileStreamer::WriteJsonObject(QString filePath, QJsonObject &jsonObject)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "Cannot open json file:" << filePath;
        return;
    }
    QTextStream stream(&file);

    QJsonDocument jsonDocument;
    jsonDocument.setObject(jsonObject);
    stream << jsonDocument.toJson();
    file.close();
}

QJsonObject FileStreamer::ReadJsonObject(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open json file:" << filePath;
        return QJsonObject();
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return QJsonObject();
    }

    if (!jsonDocument.isObject())
    {
        qDebug() << "Invalid JSON format: root is not an object";
        return QJsonObject();
    }

    return jsonDocument.object();
}
