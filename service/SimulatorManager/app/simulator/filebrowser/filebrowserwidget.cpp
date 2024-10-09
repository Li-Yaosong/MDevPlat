#include "filebrowserwidget.h"
#include "ui_filebrowserwidget.h"
#include "linuxsimulator.h"
#include "handleldterminalutils.h"
#include "abstractfilebrowser.h"

#include <QMenu>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>

FileBrowserWidget::FileBrowserWidget(HandleldTerminal::Internal::AbstractFileBrowser *fileBrowser, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::FileBrowserWidget),
      m_fileBrowser(fileBrowser)
{
    ui->setupUi(this);

    Q_ASSERT(fileBrowser != nullptr);

    m_fileModel = new QStandardItemModel(this);
    ui->tableView->setModel(m_fileModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    initFileBrowser();
    connect(ui->pathLabel, &PathLabel::pathChanged, this, [this](const QString &path){
        if (m_path == path)
            return ;
        m_path = path;
        listDirectory(m_path);
    });
    ui->pathLabel->setPath(m_path);
}

FileBrowserWidget::~FileBrowserWidget()
{
    delete ui;
}

void FileBrowserWidget::onFileInfoAvailable(const QList<HandleldTerminal::FileInfo> &fileInfoList)
{
    m_fileModel->clear();
    ui->tableView->setUpdatesEnabled(false);
    m_fileModel->setHorizontalHeaderLabels({"Name", "Size", "Type"});
    static QStringList hideDirs = {".", "..", "root", "swapfile", "lost+found"};
    foreach(const HandleldTerminal::FileInfo &info, fileInfoList) {
        addFileToView(info.name, info.size, info.isFolder);
    }
    ui->tableView->setUpdatesEnabled(true);
    ui->tableView->setColumnWidth(0, 200);
    ui->tableView->setColumnWidth(1, 200);
    if (!m_dir.isEmpty()) {
        if (m_path != "/")
            m_path.append("/");
        m_path.append(m_dir);
        ui->pathLabel->append(m_dir);
        m_dir.clear();
    }
}

void FileBrowserWidget::initFileBrowser()
{
    m_fileBrowser->setParent(this);
    connect(m_fileBrowser, &HandleldTerminal::Internal::AbstractFileBrowser::listDirectoryResult,
            this, &FileBrowserWidget::onFileInfoAvailable);
    if (m_fileBrowser->isReady())
        listDirectory(m_path);
    else {
        connect(m_fileBrowser, &HandleldTerminal::Internal::AbstractFileBrowser::ready,
                this, std::bind(&FileBrowserWidget::listDirectory, this, m_path));
    }
    connect(m_fileBrowser, &HandleldTerminal::Internal::AbstractFileBrowser::fileUploaded,
            this, [this](const HandleldTerminal::FileInfo &file){
        addFileToView(file.name, file.size, file.isFolder);
        QMessageBox::information(this, tr("Tips"), tr("upload file: %1 succeeded").arg(file.name));
    });
    connect(m_fileBrowser, &HandleldTerminal::Internal::AbstractFileBrowser::fileDownloaded,
            this, [this](const QString &file){
        QMessageBox::information(this, tr("Tips"), tr("Download file: %1 succeeded").arg(file));
    });
    connect(m_fileBrowser, &HandleldTerminal::Internal::AbstractFileBrowser::errorOccured,
            this, [this](const QString &error){
        QMessageBox::information(this, tr("Tips"), error);
    });
}

void FileBrowserWidget::listDirectory(const QString &path)
{
    m_fileBrowser->listDirectory(path);
}

void FileBrowserWidget::addFileToView(const QString &name, quint64 size, bool isFolder)
{
    QList<QStandardItem *> row;
    row.append(new QStandardItem(name));
    row.append(new QStandardItem(HandleldTerminal::Utils::memorySize(size)));
    auto item = new QStandardItem(isFolder ? QString("Folder") : QString("File"));
    item->setData(isFolder);
    row.append(item);
    m_fileModel->appendRow(row);
}

void FileBrowserWidget::on_tableView_doubleClicked(const QModelIndex &index)
{
    QModelIndex firstColumn = m_fileModel->index(index.row(), 0);
    if (!firstColumn.isValid())
        return;

    QModelIndex thirdColumn = m_fileModel->index(index.row(), 2);
    if (!thirdColumn.isValid())
        return;

    QString dir = firstColumn.data().toString();
    bool isFolder = thirdColumn.data(Qt::UserRole + 1).toBool();
    if (isFolder) {
        QString path = m_path;
        if (path != "/")
            path.append("/");
        m_dir = dir;
        path.append(dir);
        listDirectory(path);
    }
}

void FileBrowserWidget::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (m_uploadFile == nullptr) {
        m_uploadFile = new QAction(tr("Upload file"), this);
        connect(m_uploadFile, &QAction::triggered, this, &FileBrowserWidget::onUploadFile);
    }

    if (m_uploadDir == nullptr) {
        m_uploadDir = new QAction(tr("Upload dir"), this);
        connect(m_uploadDir, &QAction::triggered, this, &FileBrowserWidget::onUploadDir);
    }

    if (m_download == nullptr) {
        m_download = new QAction(tr("Download file"), this);
        connect(m_download, &QAction::triggered, this, &FileBrowserWidget::onDownloadFile);
    }

    QMenu menu;
    menu.addAction(m_uploadFile);
    menu.addAction(m_uploadDir);
    menu.addAction(m_download);
    menu.exec(QCursor::pos());
}

void FileBrowserWidget::onUploadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"));
    if (fileName.isEmpty())
        return;
    m_fileBrowser->uploadFile(fileName, m_path);
}

void FileBrowserWidget::onUploadDir()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Select Dir"));
    if (dirName.isEmpty())
        return;

    m_fileBrowser->uploadDir(dirName, m_path);
}

void FileBrowserWidget::onDownloadFile()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid())
        return;

    QModelIndex nameIndex = m_fileModel->index(index.row(), 0);
    QModelIndex flagIndex = m_fileModel->index(index.row(), 2);
    if (!nameIndex.isValid() || !flagIndex.isValid())
        return;

    QString downloadFileName = nameIndex.data().toString();
    bool isFile = !flagIndex.data(Qt::UserRole + 1).toBool();
    if (!isFile) {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
    if (fileName.isEmpty())
        return;

    m_fileBrowser->downloadFile(m_path + "/" + downloadFileName, fileName);
}
