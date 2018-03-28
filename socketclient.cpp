#include "socketclient.h"
#include <QtNetwork>
socketClient::socketClient()
{
    tcpSocket.connectToHost(QHostAddress("192.168.1.103"), 8899);
}

void socketClient::write(char *data) {
    tcpSocket.write(data);
}

