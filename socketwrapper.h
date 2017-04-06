#ifndef SOCKETWRAPPER_H
#define SOCKETWRAPPER_H

#include <QObject>
#include <QWebSocket>

class Backend;

class SocketWrapper : public QObject
{
    Q_OBJECT
public:
    explicit SocketWrapper(QWebSocket * socket,
                           Backend    * backend,
                           QObject    * parent = 0);
    ~SocketWrapper();
signals:

public slots:
    void killConnection();
private:
    QWebSocket * socket;
};

#endif // SOCKETWRAPPER_H
