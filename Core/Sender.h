#ifndef SENDER_H
#define SENDER_H

#include <cstring>
#include <QMutex>

#include "restclient-cpp/restclient.h"

class Sender
{
    private:
        Sender(); // Constructor? (the {} brackets) are needed here.

    public:
        Sender(Sender const&)         = delete;
        void operator=(Sender const&) = delete;

        static Sender& getInstance();
        RestClient::Response send(const std::string& url, const std::string& contentType, const std::string& data);
};

#endif // SENDER_H
