#ifndef DBBACKEND_H
#define DBBACKEND_H

#include "backend.h"

class DBBackend : public Backend
{
    Q_OBJECT
public:
    explicit DBBackend(const QString& driver,
                       const QString& host,
                       int            port,
                       const QString& name,
                       const QString& user,
                       const QString& password,
                       QObject *parent = 0);
    ~DBBackend();
signals:

public slots:

};

#endif // DBBACKEND_H
