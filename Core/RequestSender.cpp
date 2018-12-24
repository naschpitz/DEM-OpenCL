#include "RequestSender.h"

#include <QJsonDocument>
#include <QFile>
#include <iostream>

#include "restclient-cpp/restclient.h"

RequestSender::RequestSender()
{

}

RequestSender& RequestSender::getInstance()
{
    static RequestSender instance; // Guaranteed to be destroyed.
                                   // Instantiated on first use.
    return instance;
}

void RequestSender::newFrame()
{    
    Simulation *simulation = (Simulation*)QObject::sender();

    QJsonObject jsonObject;

    jsonObject["owner"]   = simulation->getId();
    jsonObject["step"]    = (int)simulation->getCurrentStep();
    jsonObject["time"]    = simulation->getCurrentTime();
    jsonObject["scenery"] = simulation->getScenery().getJson();

    QJsonDocument document(jsonObject);
    QByteArray data = document.toJson();

    QString url = this->getServerAddress(simulation) + "/api/frames";
    RestClient::Response r = RestClient::post(url.toStdString(), "application/json", data.toStdString());
}

void RequestSender::newLog(QString message)
{
    Simulation *simulation = (Simulation*)QObject::sender();

    QJsonObject jsonObject;

    jsonObject["owner"] = simulation->getId();

    QString state;
    if (simulation->isRunning() && simulation->getCurrentStep() == 0) {
        state = "new";
    }

    else if (simulation->isRunning()) {
        state = "running";
    }

    else if (simulation->isStopped()) {
        state = "stopped";
    }

    else if (simulation->isPaused()) {
        state = "paused";
    }

    else {
        state = "done";
    }

    jsonObject["state"] = state;
    jsonObject["message"] = message;

    QJsonObject progressJsonObject;
    progressJsonObject["step"] = (int)simulation->getCurrentStep();
    progressJsonObject["time"] = simulation->getCurrentTime();
    progressJsonObject["stepsPerSecond"] = simulation->getStepsPerSecond();
    progressJsonObject["et"] = (int)simulation->getEt();
    progressJsonObject["eta"] = (int)simulation->getEta();

    jsonObject["progress"] = progressJsonObject;

    QJsonDocument document(jsonObject);
    QByteArray data = document.toJson();

    QString url = this->getServerAddress(simulation) + "/api/simulationsLogs";
    RestClient::Response r = RestClient::post(url.toStdString(), "application/json", data.toStdString());
}

QString RequestSender::getServerAddress(const Simulation *simulation) const
{
    QHostAddress serverAddress = simulation->getServerAddress();
    QString url;

    std::cout << serverAddress.toString().toStdString() << "\n";

    if(serverAddress.isEqual(QHostAddress("127.0.0.1"))) {
        url = "127.0.0.1:3000";
    }

    if(QHostAddress(serverAddress.toIPv4Address()).isInSubnet(QHostAddress("192.168.25.0"), 24)) {
        url = QHostAddress(serverAddress.toIPv4Address()).toString() + ":3000";
    }

    if(serverAddress.isEqual(QHostAddress("163.172.186.232"))) {
        url = "https://icnsim.cienciasnauticas.org.br";
    }

    return url;
}
