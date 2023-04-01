#include "OpenCL.h"
#include "RequestMapper.h"
#include "RequestSender.h"

#include "nlohmann/json.hpp"

#include <QCoreApplication>
#include <iostream>
#include <cstring>

RequestMapper::RequestMapper(QObject* parent) : HttpRequestHandler(parent)
{
    connect(&this->statusTimer, SIGNAL(timeout()), this, SLOT(printStatus()));

    this->statusTimer.start(10000);
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

                simulation->setInterfaceAddress(request.getPeerAddress());
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
                simulation->setInterfaceAddress(request.getPeerAddress());
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
                simulation->setInterfaceAddress(request.getPeerAddress());
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

uint RequestMapper::getTotalSimulations()
{
    return this->simulations.count();
}

uint RequestMapper::getRunningSimulations()
{
    uint simulationsRunning = 0;

    foreach(const Simulation* simulation, this->simulations) {
        if(simulation->isRunning())
            simulationsRunning++;
    }

    return simulationsRunning;
}

void RequestMapper::simulationDestroyed(QObject* obj)
{
    Simulation *simulation = (Simulation*)obj;

    this->simulations.remove(simulation);
}

void RequestMapper::printStatus()
{
    std::map<const cl::Device*, uint> &devicesUsage = OpenCL::Core::getDevicesUsage();

    std::cout << "\n-----------------------STATUS-----------------------\n";
    std::cout << "               " << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz").toStdString() << "\n\n";

    std::cout << "There are " << this->getTotalSimulations() << " Simulations instantiated\n";
    std::cout << "There are " << this->getRunningSimulations() << " Simulations running\n";
    std::cout << "\n";

    for(auto it = devicesUsage.begin(); it != devicesUsage.end(); it++) {
        uint deviceIndex = std::distance(devicesUsage.begin(), it);
        uint jobs = it->second;

        std::cout << "GPU #" << deviceIndex << " has " << jobs << " job(s) running\n";
    }

    std::cout << "----------------------------------------------------\n\n";
}
