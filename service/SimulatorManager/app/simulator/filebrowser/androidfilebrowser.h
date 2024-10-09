#ifndef ANDROIDFILEBROWSER_H
#define ANDROIDFILEBROWSER_H

#include "abstractfilebrowser.h"
#include <QPointer>
namespace HandleldTerminal {
namespace Internal {

class AndroidSimulator;

class AndroidFileBrowser : public AbstractFileBrowser
{
    Q_OBJECT
public:
    AndroidFileBrowser(AndroidSimulator *sim, QObject *parent = nullptr);

    // AbstractFileBrowser interface
public:
    virtual bool isReady() const override;
    virtual void listDirectory(const QString &path) override;
    virtual void uploadFile(const QString &localFilePath, const QString &remoteFilePath) override;
    virtual void downloadFile(const QString &remoteFilePath, const QString &localFilePath) override;
    virtual void uploadDir(const QString &localDirPath, const QString &remoteParentDirPath) override;

private:
    bool parseFileInfo(const QString &line, FileInfo &info);
    bool adbCommand(const QStringList &args, QString *output);
private:
    QPointer<AndroidSimulator> m_simulator;
};
}
}
#endif // ANDROIDFILEBROWSER_H
