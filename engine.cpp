#include "engine.h"
#include <QApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

#include <QComboBox>


class ConfDialog : public QDialog {
public:
    ConfDialog(QSettings& _settings, Engine * e)
        : settings(_settings)
    {
        QGridLayout * layout = new QGridLayout();
        QGroupBox   * lj_box = new QGroupBox(tr("Livejournal"));
        QGroupBox   * db_box = new QGroupBox(tr("Database"));
        QGroupBox   * cn_box = new QGroupBox(tr("Connectivity"));
        QGroupBox   * ab_box = new QGroupBox(tr("About"));
        layout->addWidget(lj_box, 0, 0);
        layout->addWidget(db_box, 0, 1);
        layout->addWidget(cn_box, 1, 0);
        layout->addWidget(ab_box, 1, 1);
        QDialog::setLayout(layout);

        layout = new QGridLayout(lj_box);
        QLabel      * l0 = new QLabel(tr("<h2>Warning!</h2><br>"
                                         "your password will be stored as<br>"
                                         "plain text. Do not enter password<br>"
                                         "if you are not confident in PC you<br>"
                                         "are working on"));
        layout->addWidget(l0, 0, 0, 1, 2);
        QLabel      * l1 = new QLabel(tr("Livejournal login"));
        layout->addWidget(l1, 1, 0, 1, 1, Qt::AlignRight);
        QLineEdit   * e1 = new QLineEdit();
        e1->setText(settings.value("lj_user").toString());
        e1->setMinimumWidth(200);
        layout->addWidget(e1, 1, 1, 1, 1, Qt::AlignLeft);
        QLabel      * l2 = new QLabel(tr("Password"));
        layout->addWidget(l2, 2, 0, 1, 1, Qt::AlignRight);
        QLineEdit   * e2 = new QLineEdit();
        e2->setText(settings.value("lj_passwd").toString());
        e2->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        e2->setMinimumWidth(200);
        layout->addWidget(e2, 2, 1, 1, 1, Qt::AlignLeft);
        lj_user = e1;
        lj_passwd = e2;


    }
    int exec() {
        int result = QDialog::exec();
        if (result == QDialog::Accepted) {
            settings.setValue("lj_user", lj_user->text());
            settings.setValue("lj_passwd", lj_passwd->text());
            settings.sync();
        } else {
            lj_user->setText(settings.value("lj_user").toString());
            lj_passwd->setText(settings.value("lj_passwd").toString());
        }
        return result;
    }
private:
    QSettings& settings;
    QLineEdit* lj_user;
    QLineEdit* lj_passwd;
};

Engine::Engine(QSettings &_settings, QObject *parent)
    : QObject(parent)
    , settings(_settings)
    , dialog(new ConfDialog(_settings, this))
    , server(QString("lj.offline"), QWebSocketServer::NonSecureMode)
{
    messages_enabled = settings.value("popup").toBool();

    if (!server.listen(QHostAddress::LocalHost)) {
        last_error = server.errorString();
        return;
    }
    connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    index_html = QUrl(
        "file://"+
        qApp->applicationDirPath()+
        "/web/index.html");
    index_html.setQuery("server=localhost:"+QString::number(server.serverPort()));

    reopenDatabase();
}

void Engine::reopenDatabase() {
}

void Engine::showConfigDialog() {
    dialog->exec();
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

void Engine::newConnection() {
    if (!server.hasPendingConnections())
        return;

    QWebSocket * socket = server.nextPendingConnection();
    if (!socket)
        return;

    SocketWrapper * wrapper = new SocketWrapper(socket, this);
}
