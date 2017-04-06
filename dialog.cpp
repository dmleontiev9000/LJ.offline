#include "dialog.h"
#include "filebackend.h"
#include "dbbackend.h"
#include <QSqlDatabase>
#include <QDebug>

ConfDialog::ConfDialog(QSettings& _settings, Engine * e)
    : settings(_settings)
{
    QLabel * label;
    QGridLayout * grid;

    QTabWidget  * tabs   = new QTabWidget;
    QWidget     * lj_box = new QWidget;
    tabs->addTab(lj_box, tr("Livejournal"));
    QWidget     * db_box = new QWidget;
    tabs->addTab(db_box, tr("Database"));
    QWidget     * cn_box = new QWidget;
    tabs->addTab(cn_box, tr("Connectivity"));
    QWidget     * ab_box = new QWidget;
    tabs->addTab(ab_box, tr("About"));

    QHBoxLayout * buttons= new QHBoxLayout;
    QPushButton * ok = new QPushButton("OK");
    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    buttons->addStretch(1);
    buttons->addWidget(ok);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(tabs, 1);
    layout->addLayout(buttons);
    QDialog::setLayout(layout);

    /****************************************************/
    // int LJ tab
    grid = new QGridLayout(lj_box);
    //layout = new QGridLayout(lj_box);
    label = new QLabel(tr("<h2>Warning!</h2>"
                                     "your password will be stored as<br>"
                                     "plain text. Do not enter password<br>"
                                     "if you are not confident in PC you<br>"
                                     "are working on"));
    grid->addWidget(label, 0, 0, 1, 2);
    label = new QLabel(tr("Livejournal login"));
    grid->addWidget(label, 1, 0, 1, 1, Qt::AlignRight);
    lj_user = new QLineEdit();
    lj_user->setMinimumWidth(200);
    grid->addWidget(lj_user, 1, 1, 1, 1, Qt::AlignLeft);
    label = new QLabel(tr("Password"));
    grid->addWidget(label, 2, 0, 1, 1, Qt::AlignRight);
    lj_passwd = new QLineEdit();
    lj_passwd->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    lj_passwd->setMinimumWidth(200);
    grid->addWidget(lj_passwd, 2, 1, 1, 1, Qt::AlignLeft);
    grid->setRowStretch(3, 1);
    grid->setColumnStretch(2, 1);
    /****************************************************/
    // int DB tab
    grid = new QGridLayout(db_box);

    bool has_sqlite   = QSqlDatabase::isDriverAvailable("QSQLITE");
    bool has_postgres = QSqlDatabase::isDriverAvailable("QPSQL");
    bool has_odbc     = QSqlDatabase::isDriverAvailable("QODBC");
    bool has_orcl     = QSqlDatabase::isDriverAvailable("QOCI");
    bool has_mysql    = QSqlDatabase::isDriverAvailable("QMYSQL");
    dbvariants<<"Disable";
    dbnames<<"";
    if (has_sqlite) {
        label = new QLabel(tr("Select a database to use to speed up<br>"
                              "access to your offline copy. You may<br>"
                              "use Disable option to store everything<br>"
                              "in files, or SQLite option without filling<br>"
                              "any fields as safe fallbacks."));
        grid->addWidget(label, 0, 0, 1, 2);

        dbvariants<<"SQLite";
        dbnames<<"QSQLITE";
    } else {
        label = new QLabel(tr("Select a database to use to speed up<br>"
                              "access to your offline copy. You may<br>"
                              "use Disable option to store everything<br>"
                              "in files as safe fallback."));
        grid->addWidget(label, 0, 0, 1, 2);
    }
    if (has_postgres) {
        dbvariants<<"PostgreSQL";
        dbnames<<"QPSQL";
    }
    if (has_odbc) {
        dbvariants<<"ODBC";
        dbnames<<"QODBC";
    }
    if (has_orcl) {
        dbvariants<<"Oracle CLI";
        dbnames<<"QOCI";
    }
    if (has_mysql) {
        dbvariants<<"MySQL";
        dbnames<<"QMYSQL";
    }
    label = new QLabel(tr("Database to use"));
    grid->addWidget(label, 1, 0, 1, 1, Qt::AlignRight);
    db_type = new QComboBox;
    db_type->setEditable(false);
    db_type->addItems(dbvariants);
    db_type->setMinimumWidth(200);
    connect(db_type, SIGNAL(currentIndexChanged(int)),
            this, SLOT(chooseDatabase(int)));
    grid->addWidget(db_type, 1, 1, 1, 1, Qt::AlignLeft);

    label = new QLabel(tr("Path"));
    grid->addWidget(label, 2, 0, 1, 1, Qt::AlignRight);
    connect(this, SIGNAL(showFileOpts(bool)), label, SLOT(setVisible(bool)));
    file_path = new QLineEdit();
    file_path->setMinimumWidth(200);
    grid->addWidget(file_path, 2, 1, 1, 1, Qt::AlignLeft);
    connect(this, SIGNAL(showFileOpts(bool)), file_path, SLOT(setVisible(bool)));

    label = new QLabel(tr("Compress files"));
    grid->addWidget(label, 3, 0, 1, 1, Qt::AlignRight);
    connect(this, SIGNAL(showFileOpts(bool)), label, SLOT(setVisible(bool)));
    file_zip = new QCheckBox();
    file_zip->setMinimumWidth(200);
    grid->addWidget(file_zip, 3, 1, 1, 1, Qt::AlignLeft);
    connect(this, SIGNAL(showFileOpts(bool)), file_zip, SLOT(setVisible(bool)));

    label = new QLabel(tr("Host name"));
    grid->addWidget(label, 2, 0, 1, 1, Qt::AlignRight);
    connect(this, SIGNAL(showDBOpts(bool)), label, SLOT(setVisible(bool)));
    db_host = new QLineEdit();
    db_host->setMinimumWidth(200);
    grid->addWidget(db_host, 2, 1, 1, 1, Qt::AlignLeft);
    connect(this, SIGNAL(showDBOpts(bool)), db_host, SLOT(setVisible(bool)));

    label = new QLabel(tr("Port(0 for default)"));
    grid->addWidget(label, 3, 0, 1, 1, Qt::AlignRight);
    connect(this, SIGNAL(showDBOpts(bool)), label, SLOT(setVisible(bool)));
    db_port = new QSpinBox();
    db_port->setMinimum(0);
    db_port->setValue(0);
    db_port->setMaximum(65535);
    db_port->setMinimumWidth(200);
    grid->addWidget(db_port, 3, 1, 1, 1, Qt::AlignLeft);
    connect(this, SIGNAL(showDBOpts(bool)), db_port, SLOT(setVisible(bool)));

    label = new QLabel(tr("Database name"));
    grid->addWidget(label, 4, 0, 1, 1, Qt::AlignRight);
    connect(this, SIGNAL(showDBOpts(bool)), label, SLOT(setVisible(bool)));
    db_name = new QLineEdit();
    db_name->setMinimumWidth(200);
    grid->addWidget(db_name, 4, 1, 1, 1, Qt::AlignLeft);
    connect(this, SIGNAL(showDBOpts(bool)), db_name, SLOT(setVisible(bool)));

    label = new QLabel(tr("User name"));
    grid->addWidget(label, 5, 0, 1, 1, Qt::AlignRight);
    connect(this, SIGNAL(showDBOpts(bool)), label, SLOT(setVisible(bool)));
    db_user = new QLineEdit();
    db_user->setMinimumWidth(200);
    grid->addWidget(db_user, 5, 1, 1, 1, Qt::AlignLeft);
    connect(this, SIGNAL(showDBOpts(bool)), db_user, SLOT(setVisible(bool)));

    label = new QLabel(tr("Password"));
    grid->addWidget(label, 6, 0, 1, 1, Qt::AlignRight);
    connect(this, SIGNAL(showDBOpts(bool)), label, SLOT(setVisible(bool)));
    db_password = new QLineEdit();
    db_password->setMinimumWidth(200);
    db_password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    grid->addWidget(db_password, 6, 1, 1, 1, Qt::AlignLeft);
    connect(this, SIGNAL(showDBOpts(bool)), db_password, SLOT(setVisible(bool)));
    grid->setRowStretch(7, 1);
    grid->setColumnStretch(2, 1);

    readConf();
}

void ConfDialog::readConf() {
    lj_user->setText(settings.value("lj_user").toString());
    lj_passwd->setText(settings.value("lj_passwd").toString());

    file_path->setText(settings.value("file_path").toString());
    file_zip->setChecked(settings.value("file_zip").toBool());

    int i = qMax(0, dbnames.indexOf(settings.value("db_type").toString()));
    db_type->setCurrentIndex(i);
    db_host->setText(settings.value("db_host").toString());
    int port = settings.value("db_port").toInt();
    if (port < 0 || port > 65535) port = 0;
    db_port->setValue(port);
    db_name->setText(settings.value("db_name").toString());
    db_user->setText(settings.value("db_user").toString());
    db_password->setText(settings.value("db_password").toString());

    chooseDatabase(db_type->currentIndex());
}
int ConfDialog::exec() {
    int result = QDialog::exec();
    if (result == QDialog::Accepted) {
        settings.setValue("lj_user", lj_user->text());
        settings.setValue("lj_passwd", lj_passwd->text());

        settings.setValue("file_path", file_path->text());
        settings.setValue("file_zip", file_zip->isChecked());

        settings.setValue("db_type", dbnames.at(db_type->currentIndex()));
        settings.setValue("db_host", db_host->text());
        settings.setValue("db_port", db_port->value());
        settings.setValue("db_name", db_name->text());
        settings.setValue("db_user", db_user->text());
        settings.setValue("db_password", db_password->text());

        settings.sync();
    } else {
        readConf();
    }
    return result;
}
void ConfDialog::chooseDatabase(int idx) {
    if (idx >= dbnames.size())
        idx = -1;

    use_db = true;
    if (idx < 0 || dbnames.at(idx).isEmpty())
        use_db = false;
    else
        db_driver = dbnames.at(idx);
    emit showFileOpts(!use_db);
    emit showDBOpts(use_db);
}
Backend * ConfDialog::createBackend() {
    if (use_db)
        return new DBBackend(db_driver,
                             db_host->text(),
                             db_port->value(),
                             db_name->text(),
                             db_user->text(),
                             db_password->text());
    else
        return new FileBackend(file_path->text(),
                               file_zip->isChecked());
}
