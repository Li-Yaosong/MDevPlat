#ifndef ABSTRACTFILEBROWSER_H
#define ABSTRACTFILEBROWSER_H

#include <QObject>
namespace HandleldTerminal {
struct FileInfo;
namespace Internal {

class AbstractFileBrowser : public QObject
{
    Q_OBJECT
public:
    explicit AbstractFileBrowser(QObject *parent = nullptr);

    virtual bool isReady() const = 0;

    virtual void listDirectory(const QString &path) = 0;

    virtual void uploadFile(const QString &localFilePath,
        const QString &remoteFilePath) = 0;

    virtual void downloadFile(const QString &remoteFilePath,
        const QString &localFilePath) = 0;

    virtual void uploadDir(const QString &localDirPath,
        const QString &remoteParentDirPath) = 0;

signals:
    void ready();
    void listDirectoryResult(const QList<FileInfo> &files);
    void fileUploaded(const FileInfo &file);
    void fileDownloaded(const QString &file);
    void errorOccured(const QString &errorString);
};
}
}
#endif // ABSTRACTFILEBROWSER_H
