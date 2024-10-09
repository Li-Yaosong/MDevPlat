#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_check_clicked()
{
    int value = ui->spinBox_port->value();
    quint16 port = value;
    if(m_utils.portUsed(port) && value <= 65535)
    {
        QMessageBox::information(nullptr, u8"提示", u8"端口可以使用!");
    }
    else
    {
        QMessageBox::information(nullptr, u8"提示", u8"端口无法使用!");
    }
}

void Widget::on_pushButton_search_clicked()
{
    QStringList list = m_utils.addresses();
    ui->textEdit_ip->setText(list.join('\n'));
}
