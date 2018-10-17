#ifndef REQUESTSENDER_H
#define REQUESTSENDER_H

#include <QJsonObject>

class RequestSender
{
    public:
        RequestSender();
        static void newFrame(QJsonObject frame);
};

#endif // REQUESTSENDER_H
