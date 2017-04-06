#include "engine.h"
#include "dialog.h"
#include <stdio.h>
#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QDebug>


Engine::Engine(QSettings &_settings, QObject *parent)
    : QObject(parent)
    , settings(_settings)
    , dialog(new ConfDialog(_settings, this))
    , ws_server(QString("lj.offline"), QWebSocketServer::NonSecureMode)
{
    messages_enabled = settings.value("popup").toBool();

    if (!server.listen(QHostAddress::LocalHost)) {
        qDebug()<<"Failed to start HTTP server: "<<server.errorString();
        last_error = server.errorString();
        return;
    }
    connect(&server, SIGNAL(newConnection()), this, SLOT(newHttpReq()));
    server_path = qApp->applicationDirPath()+"/web/";

    if (!ws_server.listen(QHostAddress::LocalHost)) {
        qDebug()<<"Failed to start websocket server: "<<server.errorString();
        last_error = ws_server.errorString();
        return;
    }
    connect(&ws_server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    index_html = QUrl(
        "http://localhost:"+QString::number(server.serverPort())+"/index.html");
    index_html.setQuery(QString::number(ws_server.serverPort()));
}

Engine::~Engine() {
    emit killConnections();
}
void Engine::showConfigDialog() {
    if (dialog->exec() == QDialog::Accepted) {
        emit killConnections();

        delete backend;
        backend = dialog->createBackend();
    }
}

void Engine::enableMessages(bool on) {
    messages_enabled = on;
}

void Engine::showInBrowser()
{
    if (!QDesktopServices::openUrl(index_html)) {
        emit message(tr("Error"),
                     tr("Failed to open browser"),
                     QSystemTrayIcon::Warning,
                     2000);
    }
}

void Engine::syncronize() {

}


void Engine::newHttpReq() {
    if (!server.hasPendingConnections())
        return;

    QTcpSocket * socket = server.nextPendingConnection();
    if (!socket)
        return;

    connect(socket, SIGNAL(disconnected()), this, SLOT(httpConnClosed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(httpDataReceived()));

    if (socket->bytesAvailable()) {
        while (socket->canReadLine())
            httpDataReceived(socket);
    }
}

void Engine::httpDataReceived() {
    QTcpSocket * socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    while (socket->canReadLine())
        httpDataReceived(socket);
}

void Engine::httpDataReceived(QTcpSocket *socket) {
    const int L = 1024;
    char buffer[L];

    do {//reading first line of request
        qint64 len = socket->readLine(buffer, L);
        if (len < 1 || len >= L)
            break;
        buffer[L-1] = 0;

        qDebug("<< %s", qPrintable(buffer));
        //empty line received, find and send file
        if (buffer[0]=='\n' || (buffer[0]=='\r' && buffer[1]=='\n')) {
            qDebug()<<"empty line received";
            QVariant v = socket->property("url");
            if (v.isNull())
                break;

            QString newpath = server_path + v.toString();
            qDebug()<<"returning "<<newpath;
            if (!QFile::exists(newpath)) {
                socket->write("HTTP/1.0 404 not found\r\n\r\n");
                return;
            }

            QFile file(newpath);
            if (!file.open(QIODevice::ReadOnly)) {
                socket->write("HTTP/1.0 403 forbidden\r\n\r\n");
                return;
            }
            if (file.size() > 256*1024) {
                socket->write("HTTP/1.0 600 file too large\r\n\r\n");
                return;
            }
            QByteArray contents = file.readAll();

            char buffer[10];
            snprintf(buffer, 10, "%d", contents.size());

            socket->write("HTTP/1.0 200 OK\r\n"
                          "Content-Length: ");
            socket->write(buffer);
            if (newpath.endsWith(".html"))
                socket->write("\r\nContent-Type: text/html\r\n\r\n");
            else
            if (newpath.endsWith(".js"))
                socket->write("\r\nContent-Type: application/javascript\r\n\r\n");
            else
            if (newpath.endsWith(".css"))
                socket->write("\r\nContent-Type: text/css\r\n\r\n");
            else
            if (newpath.endsWith(".png"))
                socket->write("\r\nContent-Type: image/png\r\n\r\n");
            else
                socket->write("\r\nContent-Type: text/plain\r\n\r\n");
            socket->write(contents);
            socket->setProperty("url", QVariant());
            return;
        }

        //check if it is a header
        QVariant v = socket->property("url");
        if (!v.isNull())
            return;


        char * next;
        if ((0x20|buffer[0]) == 'g' &&
            (0x20|buffer[1]) == 'e' &&
            (0x20|buffer[2]) == 't' &&
            buffer[3] == ' ')
        {
            next   = buffer+4;
        }
        else break;

        while((next < buffer+L) && (*next == ' '))
            ++next;

        if (next >= buffer+L)
            break;

        if ((0x20|next[0]) == 'h' &&
            (0x20|next[1]) == 't' &&
            (0x20|next[2]) == 't' &&
            (0x20|next[3]) == 'p' &&
            next[4] == ':' &&
            next[5] == '/' &&
            next[6] == '/')
        {
            next += 7;
            //skip hostname and port, if any
            for(;;) {
                char cc = *next;
                if (cc == '?' || cc == '#')
                    cc = ' ';
                if (!cc || cc == '/' || isspace(cc))
                    break;
                ++next;
            }

            if (isspace(*next)) {
                //assume index.html
                socket->setProperty("url", QString("index.html"));
                return;
            }

            if (*next != '/')
                break;
        }

        //if we start from '/', cut it away
        if (*next == '/')
            ++next;

        //safety checks: we have only one '.' in string(prevents ..)
        //we start with have_1_dot = false;
        //first time we see '.' we set it to true.
        //if we see another '.', and have_1_dot == true, we set it to false & break;
        bool have_1_dot = false;
        for(char * c = next; *c; ++c) {
            //normal browser won't send us spaces before HTTP/1.x
            //we also stop on non-ascii characters, any whitespace character
            //or control symbols(00-1f)
            char cc = *c;
            if (cc == '?' || cc == '#')
                cc = ' ';
            if (cc <= 0x20 || cc & 0x80 || isspace(cc)) {
                *c = 0;
                break;
            }
            bool dot = (*c) == '.';
            if (have_1_dot & dot) {
                have_1_dot = false;
                break;
            }
            have_1_dot |= dot;
        }
        if (!have_1_dot)
            break;
        if (*next == 0)
            break;

        socket->setProperty("url", QString::fromLatin1(next));
        return;
    } while(0);
    socket->close();
    socket->deleteLater();
}

void Engine::httpConnClosed() {
    QTcpSocket * socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    socket->deleteLater();
}

void Engine::newConnection() {
    if (!ws_server.hasPendingConnections())
        return;

    QWebSocket * socket = ws_server.nextPendingConnection();
    if (!socket)
        return;

    SocketWrapper * wrapper = new SocketWrapper(socket, backend, this);
    connect(this, SIGNAL(killConnections()), wrapper, SLOT(killConnection()));
}
