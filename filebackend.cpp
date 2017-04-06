#include "filebackend.h"

FileBackend::FileBackend(const QString &path,
                         bool zip,
                         QObject *parent)
    : Backend(parent)
{

}

FileBackend::~FileBackend()
{

}
