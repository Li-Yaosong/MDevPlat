#ifndef TCPDATARECEIVER_H
#define TCPDATARECEIVER_H

#include <QObject>
#include <QHash>
#include <functional>
#include <QVariantMap>

class QTcpSocket;

class TcpDataReceiver : public QObject
{
    Q_OBJECT
public:
    using Func = std::function<void(const QVariantHash &)>;
    explicit TcpDataReceiver(QTcpSocket *socket, QObject *parent = nullptr);

    template<typename R, typename F>
    void registerHandler(const QString &type, R *r, F f)
    {
        registerHandler(type, std::bind(f, r, std::placeholders::_1));
    }

    void registerHandler(const QString &type, Func f);

    void unRegisterHandler(const QString &type);

    QVariantHash data() const;
public slots:
    void read();

private:
    void handle(const QByteArray &data);
private:
    QTcpSocket *s;
    QByteArray streamData;
    int streamDataLength = 0;
    QHash<QString, Func> funcs;
    QVariantHash map;

};
#endif // TCPDATARECEIVER_H
