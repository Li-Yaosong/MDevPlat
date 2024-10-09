#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
class ClientWorker;

class Server : public QObject
{
    Q_OBJECT
public:
    Server();

    void start();
private:
    QTcpServer s;
    QList<ClientWorker *> m_clients;
};

#endif // SERVER_H
