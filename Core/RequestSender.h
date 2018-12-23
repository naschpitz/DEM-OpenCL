#ifndef REQUESTSENDER_H
#define REQUESTSENDER_H

#include <QJsonObject>
#include <QObject>

#include "Simulation.h"

class RequestSender : public QObject
{        
    Q_OBJECT

    public:
        static RequestSender& getInstance();

    public slots:
        void newFrame();
        void newLog(QString message = QString());

    private:
        QString serverAddress;

        RequestSender(); // Constructor? (the {} brackets) are needed here.
        QString getServerAddress(const Simulation* simulation) const;

        // C++ 11
        // =======
        // We can use the better technique of deleting the methods
        // we don't want.
    public:
        RequestSender(RequestSender const&)  = delete;
        void operator=(RequestSender const&) = delete;

        // Note: Scott Meyers mentions in his Effective Modern
        //       C++ book, that deleted functions should generally
        //       be public as it results in better error messages
        //       due to the compilers behavior to check accessibility
        //       before deleted status
};

#endif // REQUESTSENDER_H
