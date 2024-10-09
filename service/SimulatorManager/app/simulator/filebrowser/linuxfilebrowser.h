#ifndef LINUXFILEBROWSER_H
#define LINUXFILEBROWSER_H

#include "abstractfilebrowser.h"
#include "handleldterminaldefines.h"

#include <QHash>
#include <QSharedPointer>

namespace QSsh {
class SshConnection;
class SftpChannel;
typedef quint32 SftpJobId;
class SftpFileInfo;
}

namespace HandleldTerminal {
namespace Internal {
class LinuxSimulator;

class LinuxFileBrowser : public AbstractFileBrowser
{
    Q_OBJECT
public:
    explicit LinuxFileBrowser(LinuxSimulator *sim, QObject *parent = nullptr);
    ~LinuxFileBrowser();

    bool isReady() const;

    virtual void listDirectory(const QString &path);

    virtual void uploadFile(const QString &localFilePath,
        const QString &remoteFilePath);

    virtual void downloadFile(const QString &remoteFilePath,
        const QString &localFilePath);

    virtual void uploadDir(const QString &localDirPath,
        const QString &remoteParentDirPath);
signals:

private slots:
    void onDataAvailable(QSsh::SftpJobId job, const QString &data);
    void onFileInfoAvailable(QSsh::SftpJobId job, const QList<QSsh::SftpFileInfo> &fileInfoList);
    void onCreateSftpChannel();
    void onJobFinished(QSsh::SftpJobId job, const QString &error = QString());
private:
    bool m_ready = false;
    QSsh::SshConnection *m_connect;
    QSharedPointer<QSsh::SftpChannel> m_sftp;
    QSsh::SftpJobId m_listJobId;
    QHash<QSsh::SftpJobId, HandleldTerminal::FileInfo> m_uploadFileInfos;
    QHash<QSsh::SftpJobId, QString> m_downloadFiles;
};
}
}
#endif // LINUXFILEBROWSER_H
