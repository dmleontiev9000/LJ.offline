#include "engine.h"
#include <QApplication>
#include <QAction>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("LJ.offline");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("ckotinko");
    a.setOrganizationDomain("ckotinko.ru");

    QSettings settings;
    settings.beginGroup("LJ_offline");

    Engine          * engine = new Engine(settings);
    QSystemTrayIcon * icon   = new QSystemTrayIcon(QIcon(":/icons/lj.png"));
    QMenu           * menu   = new QMenu;

    menu->addAction(QIcon(":/icons/lj.png"), a.translate("menu", "open browser"),
                    engine, SLOT(showInBrowser()));
    menu->addSeparator();
    menu->addAction(QIcon(":/icons/conf.png"), a.translate("menu", "configuration"),
                    engine, SLOT(showConfigDialog()));
    menu->addAction(QIcon(":/icons/sync.png"), a.translate("menu", "syncronize"),
                    engine, SLOT(syncronize()));
    QAction * msg_action  = nullptr;
    if (icon->supportsMessages()) {
        msg_action = menu->addAction(a.translate("menu", "enable popups"),
                                     engine, SLOT(enableMessages(bool)));
        msg_action->setCheckable(true);
        msg_action->setChecked(settings.value("popup").toBool());
        QObject::connect(engine, SIGNAL(message(QString,QString,QSystemTrayIcon::MessageIcon,int)), icon, SLOT(showMessage(QString,QString,QSystemTrayIcon::MessageIcon,int)));
    }
    menu->addSeparator();
    menu->addAction(QIcon(":/icons/exit.png"), a.translate("menu", "exit"),
                    &a, SLOT(quit()));
    icon->setContextMenu(menu);
    icon->show();
    int retc = a.exec();

    delete icon;
    delete menu;
    delete engine;
    return retc;
}
