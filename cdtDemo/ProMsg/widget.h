#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_sub_clicked();

    void on_pushButton_unsub_clicked();

    void on_pushButton_pub_clicked();

    void onGetData();
private:
    Ui::Widget *ui;
    QSet<QString> m_subTopic;

    QUdpSocket m_udpSocket;
    QThread m_thread;
};
#endif // WIDGET_H
