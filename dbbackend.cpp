#include "dbbackend.h"

DBBackend::DBBackend(const QString &driver,
                     const QString &host,
                     int port,
                     const QString &name,
                     const QString &user,
                     const QString &password,
                     QObject *parent)
    : Backend(parent)
{

}

DBBackend::~DBBackend()
{

}
