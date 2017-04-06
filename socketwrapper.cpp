#include "socketwrapper.h"

SocketWrapper::SocketWrapper(QWebSocket * _socket, QObject *parent)
    : QObject(parent)
    , socket(_socket)
{

}

SocketWrapper::~SocketWrapper() {
    socket->close(QWebSocketProtocol::CloseCodeGoingAway);
    delete socket;
}

