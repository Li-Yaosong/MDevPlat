#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>

#include "clientworker.h"
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void start(const QHostAddress &address = QHostAddress::Any, quint16 port = 9999);

private:
    QTcpServer s;
    QList<ClientWorker *> m_clients;signals:
};

#endif // SERVER_H
