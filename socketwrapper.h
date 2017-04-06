#ifndef SOCKETWRAPPER_H
#define SOCKETWRAPPER_H

#include <QObject>
#include <QWebSocket>

class SocketWrapper : public QObject
{
    Q_OBJECT
public:
    explicit SocketWrapper(QWebSocket * socket, QObject *parent = 0);
    ~SocketWrapper();
signals:

public slots:
private:
    QWebSocket * socket;
};

#endif // SOCKETWRAPPER_H
