#ifndef DIALOG_H
#define DIALOG_H

#include <QSettings>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>

class Engine;
class Backend;

class ConfDialog : public QDialog {
    Q_OBJECT
public:
    ConfDialog(QSettings& _settings, Engine * e);
    int exec();

    Backend * createBackend();
signals:
    void showDBOpts(bool);
    void showFileOpts(bool);
private slots:
    void chooseDatabase(int idx);
private:
    QSettings& settings;
    QStringList dbvariants;
    QStringList dbnames;
    QLineEdit* lj_user;
    QLineEdit* lj_passwd;
    bool       use_db;
    QString    db_driver;
    QLineEdit* file_path;
    QCheckBox* file_zip;
    QComboBox* db_type;
    QLineEdit* db_host;
    QSpinBox * db_port;
    QLineEdit* db_name;
    QLineEdit* db_user;
    QLineEdit* db_password;
    void readConf();
};

#endif // DIALOG_H
