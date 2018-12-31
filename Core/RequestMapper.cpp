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
    QByteArray path = request.getPath();

    if (path.startsWith("/simulations/start")) {
        response.setHeader("Content-Type", "application/json; charset=UTF-8");

        QJsonDocument jsonDocument;
        jsonDocument = QJsonDocument::fromJson(request.getBody());

        QJsonObject jsonObject = jsonDocument.object();

        QString _id = jsonObject["_id"].toString();

        Simulation* simulation = this->getSimulationById(_id);
        simulation = simulation ? simulation : new Simulation(jsonObject);

        this->simulations.insert(simulation);

        simulation->setServerAddress(request.getPeerAddress());
        simulation->start();
    }

    if (path.startsWith("/simulations/pause")) {
        response.setHeader("Content-Type", "application/json; charset=UTF-8");

        QJsonDocument jsonDocument;
        jsonDocument = QJsonDocument::fromJson(request.getBody());

        QJsonObject jsonObject = jsonDocument.object();

        QString _id = jsonObject["_id"].toString();

        Simulation* simulation = this->getSimulationById(_id);

        if (!simulation->isRunning())
            response.setStatus(412, "Simulation not running");

        else {
            simulation->setServerAddress(request.getPeerAddress());
            simulation->pause();
        }
    }

    if (path.startsWith("/simulations/stop")) {
        response.setHeader("Content-Type", "application/json; charset=UTF-8");

        QJsonDocument jsonDocument;
        jsonDocument = QJsonDocument::fromJson(request.getBody());

        QJsonObject jsonObject = jsonDocument.object();

        QString _id = jsonObject["_id"].toString();

        Simulation* simulation = this->getSimulationById(_id);
        this->simulations.remove(simulation);

        if (!simulation)
            response.setStatus(412, "Simulation not paused or running");

        else {
            simulation->setServerAddress(request.getPeerAddress());
            simulation->stop();
        }
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
