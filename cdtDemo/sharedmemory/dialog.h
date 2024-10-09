#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include "ui_dialog.h"
#include "sharedmemory.h"

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);

public slots:
    void loadFromFile();
    void loadFromMemory();

private:
    void detach();

private:
    Ui::Dialog ui;
    MicroKernel::SharedMemory sharedMemory;
};

#endif

