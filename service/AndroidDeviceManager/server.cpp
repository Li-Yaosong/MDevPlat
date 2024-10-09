#include "server.h"

Server::Server(QObject *parent)
    : QObject{parent}
{}

void Server::start(const QHostAddress &address, quint16 port)
{
    if (s.isListening()) {
        return;
    }
    s.listen(address, port);
    connect(&s, &QTcpServer::newConnection, this, [this]{
        if (s.hasPendingConnections()) {
            QTcpSocket *client = s.nextPendingConnection();
            new ClientWorker(client, this);
            qDebug("cccccc");
        }
    });
}
