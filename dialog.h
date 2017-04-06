#ifndef DIALOG_H
#define DIALOG_H

#include <QObject>

class Dialog : public QObject
{
    Q_OBJECT
public:
    explicit Dialog(QObject *parent = 0);

signals:

public slots:
};

#endif // DIALOG_H
