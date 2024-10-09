#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "netutils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void on_pushButton_check_clicked();

    void on_pushButton_search_clicked();
private:
    Ui::Widget *ui;

    MicroKernel::NetUtils m_utils;
};
#endif // WIDGET_H
