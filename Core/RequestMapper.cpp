#include "RequestMapper.h"
#include "RequestSender.h"

#include "nlohmann/json.hpp"

#include <QCoreApplication>
#include <iostream>

RequestMapper::RequestMapper(QObject* parent) : HttpRequestHandler(parent)
{
}

RequestMapper::~RequestMapper()
{

}

void RequestMapper::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path = request.getPath();

    if(path.startsWith("/simulations/start")) {
        response.setHeader("Content-Type", "application/json; charset=UTF-8");

        nlohmann::json jsonObject = nlohmann::json::parse(request.getBody().toStdString());

        try {
            QString _id = QString::fromStdString(jsonObject.at("_id").get<std::string>());

            Simulation* simulation = this->getSimulationById(_id);

            try {
                simulation = simulation ? simulation : new Simulation(jsonObject);

                connect(simulation, SIGNAL(destroyed(QObject*)), this, SLOT(simulationDestroyed(QObject*)));

                this->simulations.insert(simulation);

                simulation->setServerAddress(request.getPeerAddress());
                simulation->start();
            }

            catch (const std::runtime_error& e) {
                std::cout << e.what() << "\n";
                std::cout.flush();
                response.setStatus(402);
                response.write(e.what());
            }
        }

        catch (const nlohmann::detail::exception& e) {
            response.setStatus(402);
            response.write("Simulation '_id' field missing");
        }
    }

    if(path.startsWith("/simulations/pause")) {
        response.setHeader("Content-Type", "application/json; charset=UTF-8");

        nlohmann::json jsonObject = nlohmann::json::parse(request.getBody().toStdString());

        try {
            QString _id = QString::fromStdString(jsonObject.at("_id").get<std::string>());

            Simulation* simulation = this->getSimulationById(_id);

            if(!simulation)
                return;

            if(!simulation->isRunning()) {
                response.setStatus(412);
                response.write("Simulation not running");
            }

            else {
                simulation->setServerAddress(request.getPeerAddress());
                simulation->pause();
            }
        }

        catch (const nlohmann::detail::exception& e) {
            response.setStatus(402);
            response.write("Simulation '_id' field missing");
        }
    }

    if(path.startsWith("/simulations/stop")) {
        response.setHeader("Content-Type", "application/json; charset=UTF-8");

        nlohmann::json jsonObject = nlohmann::json::parse(request.getBody().toStdString());

        try {
            QString _id = QString::fromStdString(jsonObject.at("_id").get<std::string>());

            Simulation* simulation = this->getSimulationById(_id);

            if(!simulation) {
                response.setStatus(412);
                response.write("Simulation not paused or running");
            }

            else {
                simulation->setServerAddress(request.getPeerAddress());
                simulation->stop();
            }
        }

        catch (const nlohmann::detail::exception& e) {
            response.setStatus(402);
            response.write("Simulation '_id' field missing");
        }
    }
}

Simulation* RequestMapper::getSimulationById(const QString& _id)
{
    foreach(Simulation* simulation, this->simulations) {
        if(simulation->getId() == _id) {
            return simulation;
        }
    }

    return NULL;
}

void RequestMapper::simulationDestroyed(QObject* obj)
{
    Simulation *simulation = (Simulation*)obj;

    this->simulations.remove(simulation);
}
