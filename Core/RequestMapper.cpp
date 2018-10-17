#include "RequestMapper.h"
#include "RequestSender.h"

#include <QCoreApplication>
#include <QJsonDocument>

RequestMapper::RequestMapper(QObject* parent) : HttpRequestHandler(parent)
{
}

RequestMapper::~RequestMapper()
{

}

void RequestMapper::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();

    // For the following pathes, each request gets its own new instance of the related controller.

    if (path.startsWith("/simulations/start")) {
        response.setHeader("Content-Type", "application/json; charset=UTF-8");

        QJsonDocument jsonDocument;
        jsonDocument = QJsonDocument::fromJson(request.getBody());

        QJsonObject jsonObject = jsonDocument.object();

        QString _id = jsonObject["_id"].toString();

        Simulation* simulation = this->getSimulationById(_id);
        simulation = simulation ? simulation : new Simulation(jsonObject);

        this->simulations.insert(simulation);

        simulation->start();

        response.write("OK");
    }
}

Simulation* RequestMapper::getSimulationById(const QString& _id)
{
    foreach(Simulation* simulation, this->simulations) {
        if (simulation->getId() == _id) {
            return simulation;
        }
    }

    return NULL;
}
