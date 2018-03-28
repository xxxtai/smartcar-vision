#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
#include <QTcpSocket>

class socketClient : public QObject
{
    Q_OBJECT
public:
    socketClient();
    void write(char *data);

private:
    QTcpSocket tcpSocket;
};

#endif // SOCKETCLIENT_H
