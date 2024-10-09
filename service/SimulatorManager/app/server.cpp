#include "server.h"
#include "clientworker.h"

#include <QTcpSocket>

Server::Server()
{

}

void Server::start()
{
    if (s.isListening()) {
        return;
    }
    s.listen(QHostAddress::Any, 8888);
    connect(&s, &QTcpServer::newConnection, this, [this]{
        if (s.hasPendingConnections()) {
            QTcpSocket *client = s.nextPendingConnection();
            new ClientWorker(client, this);
        }
    });
}
