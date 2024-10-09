#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QSqlTableModel;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_add_clicked();

    void on_pushButton_del_clicked();

private:
    bool initDataBase();

private:
    Ui::Widget *ui;
    QSqlTableModel *m_model;
    QSqlDatabase m_db;
};
#endif // WIDGET_H
