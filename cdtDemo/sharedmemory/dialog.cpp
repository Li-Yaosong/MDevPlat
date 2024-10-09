#include <QBuffer>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include "dialog.h"

using namespace MicroKernel;
Dialog::Dialog(QWidget *parent)
  : QDialog(parent), sharedMemory("SharedMemoryExample")
{
    ui.setupUi(this);
    connect(ui.loadFromFileButton, &QPushButton::clicked,
            this, &Dialog::loadFromFile);
    connect(ui.loadFromSharedMemoryButton, &QPushButton::clicked,
            this, &Dialog::loadFromMemory);
    setWindowTitle(tr("SharedMemory Example"));
}

void Dialog::loadFromFile()
{
    if (sharedMemory.isAttached())
        detach();

    ui.label->setText(tr("Select an image file"));
    QString fileName = QFileDialog::getOpenFileName(0, QString(), QString(),
                                        tr("Images (*.png *.xpm *.jpg)"));

    QFileInfo info(fileName);
    int fileSize = info.size();
    int mb = fileSize / 1024 / 1024;
    if(mb >= 2048) //MB
    {
        QMessageBox::information(nullptr, "tips" ,"file is too big!");
        return;
    }

    QImage image;
    if (!image.load(fileName)) {
        ui.label->setText(tr("Selected file is not an image, please select another."));
        return;
    }
    ui.label->setPixmap(QPixmap::fromImage(image));

    // load into shared memory
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << image;
    int size = buffer.size();

    if (!sharedMemory.create(size)) {
        ui.label->setText(tr("Unable to create shared memory segment."));
        return;
    }
    sharedMemory.lock();
    char *to = (char*)sharedMemory.data();
    const char *from = buffer.data().data();
    memcpy(to, from, qMin(sharedMemory.size(), size));
    sharedMemory.unlock();
}
void Dialog::loadFromMemory()
{
    if (!sharedMemory.attach()) {
        ui.label->setText(tr("Unable to attach to shared memory segment.\n" \
                             "Load an image first."));
        return;
    }

    QBuffer buffer;
    QDataStream in(&buffer);
    QImage image;

    sharedMemory.lock();
    buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
    buffer.open(QBuffer::ReadOnly);
    in >> image;
    sharedMemory.unlock();

    sharedMemory.detach();
    ui.label->setPixmap(QPixmap::fromImage(image));
}

void Dialog::detach()
{
    if (!sharedMemory.detach())
        ui.label->setText(tr("Unable to detach from shared memory."));
}

