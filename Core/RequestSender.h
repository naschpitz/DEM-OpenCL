#ifndef REQUESTSENDER_H
#define REQUESTSENDER_H

#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QThread>
#include <QVector>

#include "FrameSender.h"
#include "LogSender.h"
#include "Simulation.h"

class RequestSender : public QObject
{        
    Q_OBJECT

    public:
        static RequestSender& getInstance();

    public slots:
        void newFrame(bool detailed = true);
        void newLog(QString message = QString());

    public:
        void waitForAllFramesSent(const Simulation* simulation);

    private:
        bool remoteInterface;

        FrameSender frameSender;
        LogSender logSender;

        RequestSender(); // Constructor? (the {} brackets) are needed here.
        QString getInterfaceAddress(const Simulation* simulation) const;

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
