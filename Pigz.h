#ifndef PIGZ_H
#define PIGZ_H

#include <QByteArray>
#include <cstring>

class Pigz
{
  public:
    Pigz();
    static QByteArray deflate(const QByteArray& data);
};

#endif // PIGZ_H
