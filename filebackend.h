#ifndef FILEBACKEND_H
#define FILEBACKEND_H

#include "backend.h"

class FileBackend : public Backend
{
    Q_OBJECT
public:
    explicit FileBackend(const QString& path,
                         bool zip,
                         QObject *parent = 0);

    ~FileBackend();
signals:

public slots:

private:
};

#endif // FILEBACKEND_H
