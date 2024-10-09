#include "widget.h"
#include "ui_widget.h"

#include <QSet>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    if(!m_udpSocket.bind(QHostAddress::AnyIPv4, 8900, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    {
        qDebug() << "bind failed!";
    }

    connect(&m_udpSocket, &QUdpSocket::readyRead, this, &Widget::onGetData);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_sub_clicked()
{
    m_subTopic.insert(ui->lineEdit_subTopic->text());
}


void Widget::on_pushButton_unsub_clicked()
{
    m_subTopic.remove(ui->lineEdit_subTopic->text());
}


void Widget::on_pushButton_pub_clicked()
{
    QByteArray byte;
    quint16 tmp = 0xA5;
    byte.append((char *)(&tmp), 1);

    QByteArray topicByte = ui->lineEdit_pub->text().toLocal8Bit();
    quint16 len = topicByte.length();
    byte.append((char *)(&len), 2);
    byte.append(topicByte);

    byte.append(ui->textEdit_pub->toPlainText().toLocal8Bit());

    m_udpSocket.writeDatagram(byte, QHostAddress::Broadcast, 8900);
}

void Widget::onGetData()
{
    while(m_udpSocket.hasPendingDatagrams())
    {
        QByteArray byte;
        quint16 senderPort;
        QHostAddress senderHost;
        byte.resize(m_udpSocket.pendingDatagramSize());
        m_udpSocket.readDatagram(byte.data(),byte.size(),&senderHost,&senderPort);

        if((quint8)byte[0] != 0xA5)
        {
            continue;
        }

        quint16 tLen = *(quint16 *)(byte.data() + 1);
        QString topic = QString::fromLocal8Bit( QByteArray(byte.data() + 3, tLen) );
        if(!m_subTopic.contains(topic))
        {
            continue;
        }

        QString content = QString::fromLocal8Bit( QByteArray(byte.data() + 3 + tLen, byte.size() - 3 - tLen) );


        ui->textEdit_sub->append(content);
    }
}

