#ifndef EASYZLIB_H
#define EASYZLIB_H

#include <QByteArray>
#include <cstring>

class EasyZLib
{
    public:
        EasyZLib();
        static std::string deflate(const QByteArray& data);
};

#endif // EASYZLIB_H
