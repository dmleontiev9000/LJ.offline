#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDate>


class Backend : public QObject
{
    Q_OBJECT
public:

signals:

public slots:

protected:
    explicit Backend(QObject *parent = 0);
};

#endif // BACKEND_H
