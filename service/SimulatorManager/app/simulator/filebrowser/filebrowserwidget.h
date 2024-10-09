#ifndef FILEBROWSERWIDGET_H
#define FILEBROWSERWIDGET_H

#include <QWidget>
#include <QHash>

namespace Ui {
class FileBrowserWidget;
}

namespace HandleldTerminal {
struct FileInfo;
namespace Internal {
class AbstractFileBrowser;
}
}

class QStandardItemModel;

class FileBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowserWidget(HandleldTerminal::Internal::AbstractFileBrowser *fileBrowser, QWidget *parent = nullptr);
    ~FileBrowserWidget();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_tableView_customContextMenuRequested(const QPoint &pos);
    void onUploadFile();
    void onUploadDir();
    void onDownloadFile();
    void onFileInfoAvailable(const QList<HandleldTerminal::FileInfo> &fileInfoList);
private:
    void initFileBrowser();
    void listDirectory(const QString &path);
    void addFileToView(const QString &name, quint64 size, bool isFolder);
private:
    Ui::FileBrowserWidget *ui;
    HandleldTerminal::Internal::AbstractFileBrowser *m_fileBrowser;
    QString m_path = "/";
    QString m_dir;
    QStandardItemModel *m_fileModel;
    QAction *m_uploadFile = nullptr;
    QAction *m_uploadDir = nullptr;
    QAction *m_download = nullptr;
};

#endif // FILEBROWSERWIDGET_H
