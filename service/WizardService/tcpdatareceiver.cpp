#include "tcpdatareceiver.h"

#include <QTcpSocket>
#include <QtEndian>
#include <QJsonDocument>
#include <QJsonObject>


/*!
 * \brief 构造一个Receiver, \a socket是被接收的套接字, Receiver不负责删除
 */
TcpDataReceiver::TcpDataReceiver(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , s(socket)
{
    connect(s, &QTcpSocket::disconnected, this, [this]{
        s = Q_NULLPTR;
    });
    connect(s, &QTcpSocket::readyRead, this, &TcpDataReceiver::read);
}

/*!
 * \overload
 * \brief 注册一个方法\a f。当有\a type类型的消息时会调用该方法。如果已经注册了会忽略
 * \param type
 * \param f
 */
void TcpDataReceiver::registerHandler(const QString &type, TcpDataReceiver::Func f)
{
    if (funcs.contains(type))
    {
        return;
    }
    funcs.insert(type, f);
}

/*!
 * \fn registerHandler
 * \brief 注册一个方法\a f。当有\a type类型的消息时会调用该方法。如果已经注册了会忽略
 */


/*!
 * \brief 取消\a type类型的注册
 * \param type
 */
void TcpDataReceiver::unRegisterHandler(const QString &type)
{
    if (!funcs.contains(type))
    {
        return;
    }
    funcs.remove(type);
}

/*!
 * \brief 获取接收到的数据
 * \return
 */
QVariantHash TcpDataReceiver::data() const
{
    return map;
}

/*!
 * \brief 接收数据
 */
void TcpDataReceiver::read()
{
    if (s == Q_NULLPTR)
    {
        return;
    }
    static const int minCommandLength = 4;
    while (s->bytesAvailable() > minCommandLength)
    {
        if (!streamData.isEmpty())
        {
            if (streamDataLength - streamData.length() > s->bytesAvailable())
            {
                return;
            }
            streamData.append(s->read(streamDataLength - streamData.length()));
            if (streamData.length() == streamDataLength)
            {
                handle(streamData);
                streamData.clear();
                streamDataLength = 0;
            }
            return;
        }

        QByteArray head = s->read(4);
        streamDataLength = qFromBigEndian<int>((uchar*)head.mid(0, 4).data());
        streamData = s->read(streamDataLength);
        if (streamData.length() == streamDataLength)
        {
            handle(streamData);
            streamData.clear();
            streamDataLength = 0;
        }
    }
}

/*!
 * \brief 解析接收到的数据\a data
 * \param data
 */
void TcpDataReceiver::handle(const QByteArray &data)
{
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(data, &err);
    if (!json.isNull())
    {
        map = json.toVariant().toHash();
        QString type = map.value(QLatin1String("command")).toString();
        if (funcs.contains(type))
        {
            funcs.value(type)(map);
        }
    }
    else
    {
        qDebug() << err.errorString();
    }
}
