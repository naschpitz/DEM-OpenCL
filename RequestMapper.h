#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "httprequesthandler.h"
#include "Simulation.h"
#include "SimulationSink.h"

#include <QSet>
#include <QTimer>
#include <QThread>

using namespace stefanfrings;

/**
  The request mapper dispatches incoming HTTP requests to controller classes
  depending on the requested path.
*/

class RequestMapper : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(RequestMapper)

    private:
        QSet<Simulation*> simulations;
        SimulationSink* sink;

        Simulation* getSimulationById(const QString& _id);
        uint getTotalSimulations();
        uint getRunningSimulations();

        QTimer statusTimer;

    public:
        RequestMapper(SimulationSink* sink, QObject* parent=0);
        ~RequestMapper();

        void service(HttpRequest& request, HttpResponse& response);

    public slots:
        void simulationDestroyed(QObject*);
        void simulationFinished();

    private slots:
        void printStatus();
};

#endif // REQUESTMAPPER_H
