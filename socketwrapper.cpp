#include "socketwrapper.h"

SocketWrapper::SocketWrapper(QWebSocket * _socket,
                             Backend *backend,
                             QObject *parent)
    : QObject(parent)
    , socket(_socket)
{

}

SocketWrapper::~SocketWrapper() {
    killConnection();
}

void SocketWrapper::killConnection() {
    if (socket) {
        socket->close(QWebSocketProtocol::CloseCodeGoingAway);
        delete socket;
        socket = nullptr;
        deleteLater();
    }
}

