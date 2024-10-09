#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QObject>
#include <QPointer>
#include <QVariant>
class QTcpSocket;
class TcpDataReceiver;
class ClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit ClientWorker(QTcpSocket *socket, QObject *parent = nullptr);
private slots:
    void devices(const QVariantMap &map);
    void deviceBaseInfo(const QVariantMap &map);
    void deviceBatteryInfo(const QVariantMap &map);
    void deviceStorageInfo(const QVariantMap &map);
    void deviceMemoryInfo(const QVariantMap &map);
    void deviceEthInfo(const QVariantMap &map);
    void deviceWlanInfo(const QVariantMap &map);

    void startWebsocket(const QVariantMap &map);

private:
    void reply(const QVariant &result);
    template<typename T>
    T value(const QVariantMap &map, const QString &key)
    {
        return map.value(key).value<T>();
    }
private:
    TcpDataReceiver *m_receiver;
    QPointer<QTcpSocket> m_socket;
};

#endif // CLIENTWORKER_H
