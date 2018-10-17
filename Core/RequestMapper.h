#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "httprequesthandler.h"
#include "Simulation.h"

#include <QSet>

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

        Simulation* getSimulationById(const QString& _id);

    public:
        RequestMapper(QObject* parent=0);
        ~RequestMapper();

        void service(HttpRequest& request, HttpResponse& response);
};

#endif // REQUESTMAPPER_H
