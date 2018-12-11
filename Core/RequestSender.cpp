#include "RequestSender.h"

#include <QJsonDocument>
#include <QFile>
#include "restclient-cpp/restclient.h"

RequestSender::RequestSender()
{
    this->serverAddress = "127.0.0.1:3000/api";
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

    QString url = this->serverAddress + "/frames";
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
        message = "Simulation stopped";
    }

    else if (simulation->isPaused()) {
        state = "paused";
        message = "Simulation paused";
    }

    else {
        state = "done";
        message = "Simulation done";
    }

    jsonObject["state"] = state;

    QJsonObject progressJsonObject;
    progressJsonObject["step"] = (int)simulation->getCurrentStep();
    progressJsonObject["time"] = simulation->getCurrentTime();
    progressJsonObject["stepsPerSecond"] = simulation->getStepsPerSecond();
    progressJsonObject["et"] = (int)simulation->getEt();
    progressJsonObject["eta"] = (int)simulation->getEta();

    jsonObject["progress"] = progressJsonObject;

    QJsonDocument document(jsonObject);
    QByteArray data = document.toJson();

    QString url = this->serverAddress + "/simulationsLogs";
    RestClient::Response r = RestClient::post(url.toStdString(), "application/json", data.toStdString());
}
