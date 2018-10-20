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

void RequestSender::newFrame(const Simulation* simulation)
{    
    QJsonObject jsonObject;

    jsonObject["owner"]       = simulation->getId();
    jsonObject["currentStep"] = (int)simulation->getCurrentStep();
    jsonObject["currentTime"] = simulation->getCurrentTime();
    jsonObject["scenery"]     = simulation->getScenery().getJson();

    QJsonDocument document(jsonObject);
    QByteArray data = document.toJson();

    QString url = this->serverAddress + "/frames";
    RestClient::Response r = RestClient::post(url.toStdString(), "application/json", data.toStdString());
}

void RequestSender::newInfo(const Simulation* simulation)
{
    QJsonObject jsonObject;

    jsonObject["owner"] = simulation->getId();

    jsonObject["currentStep"] = (int)simulation->getCurrentStep();
    jsonObject["currentTime"] = simulation->getCurrentTime();
    jsonObject["stepsPerSecond"] = simulation->getStepsPerSecond();
    jsonObject["et"] = (int)simulation->getEt();
    jsonObject["eta"] = (int)simulation->getEta();

    QJsonDocument document(jsonObject);
    QByteArray data = document.toJson();

    QString url = this->serverAddress + "/simulationsInfos";
    RestClient::Response r = RestClient::post(url.toStdString(), "application/json", data.toStdString());
}
