#include "androidfilebrowser.h"
#include "androidsdksettings.h"
#include "androidsimulator.h"

#include <utils/synchronousprocess.h>
#include <utils/runextensions.h>

#include <QRegularExpression>
#include <QFileInfo>

namespace HandleldTerminal {
namespace Internal {

AndroidFileBrowser::AndroidFileBrowser(AndroidSimulator *sim, QObject *parent)
    : AbstractFileBrowser (parent)
    , m_simulator(sim)
{
    Q_ASSERT(m_simulator != nullptr);
    adbCommand({"root"}, nullptr);
}

bool AndroidFileBrowser::isReady() const
{
    return true;
}

bool AndroidFileBrowser::adbCommand(const QStringList &args, QString *output)
{
    if (m_simulator.isNull())
        return false;
    QStringList _args = {"-s", m_simulator->emulatorId()};
    _args << args;
    QString adb = qAndroid->adb();
    Utils::SynchronousProcess proc;
    auto env = qAndroid->toolsEnvironment().toStringList();
    proc.setEnvironment(env);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adb, _args);
//    if (response.result == Utils::SynchronousProcessResponse::Finished) {
//        if (output)
//            *output = response.allOutput();
//        return true;
//    }
    if (output)
        *output = response.stdOut();
    return true;
}

void AndroidFileBrowser::listDirectory(const QString &path)
{
    QString output;
    QStringList args = {"shell", "ls", "-l", path };
    if (!adbCommand(args, &output))
        return;
    QRegularExpression delimiters("[\\n\\r]");
    QStringList lines = output.split(delimiters);
    QList<FileInfo> files;
    foreach(QString line, lines) {
        if (line.contains("Permission denied"))
            continue;
        FileInfo f;
        if (parseFileInfo(line, f))
            files << f;
    }
    if (files.isEmpty())
        return;
    emit listDirectoryResult(files);
}

void AndroidFileBrowser::uploadFile(const QString &localFilePath, const QString &remoteFilePath)
{
    Utils::runAsync([this, localFilePath, remoteFilePath]{
        QFileInfo info(localFilePath);
        QString name = info.fileName();
        QStringList args = { "push", "-z", "lz4", localFilePath, remoteFilePath + "/" + name };
        QString output;
        adbCommand(args, &output);
    });
}

void AndroidFileBrowser::downloadFile(const QString &remoteFilePath, const QString &localFilePath)
{
    Utils::runAsync([this, remoteFilePath, localFilePath]{
        QStringList args = { "pull", "-z", "lz4", remoteFilePath, localFilePath};
        QString output;
        adbCommand(args, &output);
    });
}

void AndroidFileBrowser::uploadDir(const QString &localDirPath, const QString &remoteParentDirPath)
{
    Utils::runAsync([this, localDirPath, remoteParentDirPath]{
        QFileInfo info(localDirPath);
        QString name = info.fileName();
        QStringList args = { "push", "-z", "lz4", localDirPath, remoteParentDirPath + "/" + name };
        QString output;
        adbCommand(args, &output);
    });
}

bool AndroidFileBrowser::parseFileInfo(const QString &line, FileInfo &info)
{
    static QRegularExpression re(R"(^([drwx-]{10})\s+\d+\s+\w+\s+\w+\s+(\d+)\s+[\d-]+\s+[\d:]+\s+(.+)$)");
    QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch()) {
        QString permissions = match.captured(1);
        info.size = match.captured(2).toULongLong();
        info.name = match.captured(3);
        info.isFolder = (permissions[0] == 'd');
        return true;
    }
    return false;
}
}
}
