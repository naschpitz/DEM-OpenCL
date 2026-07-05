#include "Common.h"
#include "RequestSender.h"

#include <QDir>
#include <QFile>
#include <QHostAddress>
#include <QSettings>
#include <QUuid>
#include <iostream>
#include <cstring>

#include "nlohmann/json.hpp"

RequestSender::RequestSender()
{
    QString configFileName = Common::searchConfigFile();

    QSettings* settings = new QSettings(configFileName, QSettings::IniFormat);

    settings->beginGroup("interface");
    this->remoteInterface = settings->value("remote").toBool();
    settings->endGroup();

    QString dirPath = "./framesData/";

    QDir framesDataDir(dirPath);

    if(!framesDataDir.exists())
       QDir().mkdir(dirPath);

    this->frameSender.setFramesDir(dirPath);
}

RequestSender& RequestSender::getInstance()
{
    static RequestSender instance; // Guaranteed to be destroyed.
                                   // Instantiated on first use.
    return instance;
}

void RequestSender::newFrame(bool detailed)
{    
    Simulation *simulation = (Simulation*)QObject::sender();

    nlohmann::json jsonObject;

    jsonObject["owner"]    = simulation->getScenery().getId().toStdString();
    jsonObject["instance"] = simulation->getInstance().toStdString();

    jsonObject["detailed"] = detailed;
    jsonObject["step"]     = simulation->getCurrentStep();
    jsonObject["time"]     = simulation->getCurrentTime();
    jsonObject["scenery"]  = simulation->getScenery().getJson(detailed);

    QByteArray data = QByteArray::fromStdString(jsonObject.dump());

    QString filename = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString& framesDir = this->frameSender.getFramesDir();

    QSharedPointer<QFile> file = QSharedPointer<QFile>::create(framesDir + filename + ".json");
    file->open(QIODevice::WriteOnly | QIODevice::Truncate);
    file->write(data);
    file->flush();
    file->close();

    QString url = this->getInterfaceAddress(simulation) + "/api/frames";
    this->frameSender.send(url, simulation, file);
}

void RequestSender::newLog(QString message)
{
    Simulation *simulation = (Simulation*)QObject::sender();

    nlohmann::json jsonObject;

    jsonObject["owner"] = simulation->getId().toStdString();
    jsonObject["instance"] = simulation->getInstance().toStdString();

    QString state;
    if(simulation->isRunning()) {
        state = "running";
    }

    else if(simulation->isStopped()) {
        state = "stopped";
    }

    else if(simulation->isPaused()) {
        state = "paused";
    }

    else if(simulation->isDone()) {
        state = "done";
    }

    jsonObject["state"]   = state.toStdString();
    jsonObject["message"] = message.toStdString();

    nlohmann::json progressJsonObject;
    progressJsonObject["step"]       = simulation->getCurrentStep();
    progressJsonObject["totalSteps"] = simulation->getTotalSteps();
    progressJsonObject["time"]       = simulation->getCurrentTime();
    progressJsonObject["et"]         = simulation->getEt();
    progressJsonObject["eta"]        = simulation->getEta();

    jsonObject["progress"] = progressJsonObject;

    QString url = this->getInterfaceAddress(simulation) + "/api/logs";
    QByteArray data = QByteArray::fromStdString(jsonObject.dump());

    this->logSender.send(url, data);
}

void RequestSender::waitForAllFramesSent(const Simulation* simulation)
{
    this->frameSender.waitForAllFramesSent(simulation);
}

QString RequestSender::getInterfaceAddress(const Simulation *simulation) const
{
    QHostAddress interfaceAddress = simulation->getInterfaceAddress();
    QString url;

    if(this->remoteInterface)
    {
        url = simulation->getInterfaceUrl();
    }

    else
    {
        if(interfaceAddress.isEqual(QHostAddress("127.0.0.1"))) {
            url = "localhost:8000";
        }

        else if(QHostAddress(interfaceAddress.toIPv4Address()).isInSubnet(QHostAddress("192.168.0.0"), 16)) {
            url = QHostAddress(interfaceAddress.toIPv4Address()).toString() + ":8000";
        }
    }

    return url;
}
