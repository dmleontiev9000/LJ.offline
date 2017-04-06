#ifndef ENGINE_H
#define ENGINE_H

#include <QSettings>
#include <QSystemTrayIcon>
#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QUrl>
#include <QSqlDatabase>
#include "socketwrapper.h"

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QSettings& settings, QObject *parent = 0);
    bool ok() const;
signals:
    void message(const QString& title, const QString& message,QSystemTrayIcon::MessageIcon,int);
public slots:
    void enableMessages(bool on);
    void syncronize();
    void showInBrowser();
    void showConfigDialog();
private slots:
    void newConnection();
private:
    QSettings&       settings;
    QString          last_error;
    bool             messages_enabled;

    QDialog        * dialog;

    QUrl             index_html;
    QWebSocketServer server;

    QSqlDatabase     database;
    void reopenDatabase();
};

#endif // ENGINE_H
