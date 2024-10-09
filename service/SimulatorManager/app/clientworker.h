#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QPointer>
class TcpDataReceiver;

class ClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit ClientWorker(QTcpSocket *socket, QObject *parent = nullptr);

private slots:
    void listSimulator(const QVariantMap &map);
    void addSimulator(const QVariantMap &map);
    void updateSimulator(const QVariantMap &map);
    void removeSimulator(const QVariantMap &map);
    void startSimulator(const QVariantMap &map);
    void stopSimulator(const QVariantMap &map);
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

#endif // SIMULATORWORKER_H
