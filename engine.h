#ifndef ENGINE_H
#define ENGINE_H

#include <QSettings>
#include <QSystemTrayIcon>
#include <QObject>
#include <QTcpServer>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QUrl>
#include "socketwrapper.h"
#include "backend.h"

class ConfDialog;

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QSettings& settings, QObject *parent = 0);
    ~Engine();
    bool ok() const;
signals:
    void message(const QString& title, const QString& message,QSystemTrayIcon::MessageIcon,int);
    void killConnections();
public slots:
    void enableMessages(bool on);
    void syncronize();
    void showInBrowser();
    void showConfigDialog();
private slots:
    void newHttpReq();
    void newConnection();
    void httpConnClosed();
    void httpDataReceived();
private:
    QSettings&       settings;
    QString          last_error;
    bool             messages_enabled;

    ConfDialog     * dialog;

    QUrl             index_html;
    QTcpServer       server;
    QString          server_path;

    QWebSocketServer ws_server;

    Backend        * backend = nullptr;
    void httpDataReceived(QTcpSocket * socket);
};

#endif // ENGINE_H
