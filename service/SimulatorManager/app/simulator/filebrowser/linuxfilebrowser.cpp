#include "linuxfilebrowser.h"
#include "linuxsimulator.h"

#include <ssh/sshconnection.h>
#include <ssh/sftpchannel.h>
#include <ssh/sftpdefs.h>

#include <QFileInfo>

namespace HandleldTerminal {
namespace Internal {

LinuxFileBrowser::LinuxFileBrowser(LinuxSimulator *sim, QObject *parent) : AbstractFileBrowser(parent)
{
    Q_ASSERT(sim != nullptr);
    m_connect = sim->sshConnection();
    if (m_connect->state() == QSsh::SshConnection::Connected) {
        onCreateSftpChannel();
    } else {
        connect(m_connect, &QSsh::SshConnection::connected, this, &LinuxFileBrowser::onCreateSftpChannel);
    }
}

LinuxFileBrowser::~LinuxFileBrowser()
{
    m_connect->closeAllChannels();
}

bool LinuxFileBrowser::isReady() const
{
    return m_ready;
}

void LinuxFileBrowser::uploadFile(const QString &localFilePath, const QString &remoteFilePath)
{
    QFileInfo info(localFilePath);
    QString name = info.fileName();
    auto jobId = m_sftp->uploadFile(localFilePath, remoteFilePath + "/" + name, QSsh::SftpOverwriteExisting);
    if (jobId != 0) {
        m_uploadFileInfos.insert(jobId, {name, static_cast<quint64>(info.size()), false});
    }
}

void LinuxFileBrowser::downloadFile(const QString &remoteFilePath, const QString &localFilePath)
{
    auto jobId = m_sftp->downloadFile(remoteFilePath, localFilePath, QSsh::SftpOverwriteExisting);
    if (jobId != 0) {
        m_downloadFiles.insert(jobId, localFilePath);
    }
}

void LinuxFileBrowser::uploadDir(const QString &localDirPath, const QString &remoteParentDirPath)
{
    QFileInfo info(localDirPath);
    QString name = info.fileName();
    auto jobId = m_sftp->uploadDir(localDirPath, remoteParentDirPath + "/" + name);
    if (jobId != 0) {
        m_uploadFileInfos.insert(jobId, {name, static_cast<quint64>(info.size()), true});
    }
}

void LinuxFileBrowser::onDataAvailable(QSsh::SftpJobId job, const QString &data)
{
    qDebug() << "dddddd" << data;
}

void LinuxFileBrowser::onFileInfoAvailable(QSsh::SftpJobId job, const QList<QSsh::SftpFileInfo> &fileInfoList)
{
    if (job != m_listJobId)
        return;

    static QStringList hideDirs = {".", "..", "root", "swapfile", "lost+found"};
    QList<FileInfo> results;
    foreach(const QSsh::SftpFileInfo &info, fileInfoList) {
        if (hideDirs.contains(info.name))
            continue;
        results << FileInfo{info.name, info.size, info.type == QSsh::FileTypeDirectory};
    }
    emit listDirectoryResult(results);
}

void LinuxFileBrowser::onCreateSftpChannel()
{
    m_sftp = m_connect->createSftpChannel();
    if (m_sftp.isNull())
        return;
    m_sftp->initialize();
    connect(m_sftp.data(), &QSsh::SftpChannel::dataAvailable, this, &LinuxFileBrowser::onDataAvailable);
    connect(m_sftp.data(), &QSsh::SftpChannel::fileInfoAvailable, this, &LinuxFileBrowser::onFileInfoAvailable);
    connect(m_sftp.data(), &QSsh::SftpChannel::finished, this, &LinuxFileBrowser::onJobFinished);
    connect(m_sftp.data(), &QSsh::SftpChannel::channelError, this, [this](const QString &error){
        qWarning() << "SftpChannel: " << error;
        emit errorOccured(error);
    });

    auto handleReady = [this](){
        m_ready = true;
        emit ready();
    };

    if (m_sftp->state() == QSsh::SftpChannel::Initialized) {
        handleReady();
    } else {
        connect(m_sftp.data(), &QSsh::SftpChannel::initialized, this, handleReady);
    }
}

void LinuxFileBrowser::onJobFinished(QSsh::SftpJobId job, const QString &error)
{
    if (!error.isEmpty()) {
        emit errorOccured(error);
        m_uploadFileInfos.take(job);
        m_downloadFiles.take(job);
        return;
    }
    if (m_uploadFileInfos.contains(job)) {
        emit fileUploaded(m_uploadFileInfos.take(job));
    }
    if (m_downloadFiles.contains(job)) {
        QString file = m_downloadFiles.take(job);
        qDebug() << QString("Download file: %1 completed.").arg(file);
        emit fileDownloaded(file);
    }
}

void LinuxFileBrowser::listDirectory(const QString &path)
{
    m_listJobId = m_sftp->listDirectory(path);
}

}
}
