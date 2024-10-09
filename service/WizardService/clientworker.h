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
    void createKernel(const QVariantHash &map);
    void createPlugin(const QVariantHash &map);

private:
    void createPluginUi(const QVariantHash &plugin, const QString pluginPath);

    void reply(const QVariant &result);
    template<typename T>
    T value(const QVariantHash &map, const QString &key)
    {
        return map.value(key).value<T>();
    }
private:
    TcpDataReceiver *m_receiver;
    QPointer<QTcpSocket> m_socket;

};

#endif // SIMULATORWORKER_H
