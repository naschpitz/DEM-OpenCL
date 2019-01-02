#include "RequestSender.h"

#include <QBuffer>
#include <QtEndian>
#include <QJsonDocument>
#include <QFile>
#include <iostream>
#include <cstring>

#include "restclient-cpp/restclient.h"
#include "../libs/easyzlib/easyzlib.h"

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

    QPair<QString, QByteArray> pair = QPair<QString, QByteArray>(url, data);

    // As frames data is pretty large usually, a buffer is used to send it as soon as possible.
    this->mutex.lock();
    this->buffer.append(pair);
    this->mutex.unlock();

    this->start();
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

    QPair<QString, QByteArray> pair = QPair<QString, QByteArray>(url, data);

    // As frames data is pretty large usually, a buffer is used to send it as soon as possible.
    this->mutex.lock();
    this->buffer.append(pair);
    this->mutex.unlock();

    this->start();
}

void RequestSender::run()
{
    while (true) {
        QPair<QString, QByteArray> pair;

        this->mutex.lock();
        bool isEmpty = this->buffer.isEmpty();
        this->mutex.unlock();

        if (!isEmpty) {
            this->mutex.lock();
            pair = this->buffer.first();
            this->mutex.unlock();
        }

        else {
            this->sleep(1);
            continue;
        }

        QString& url = pair.first;
        QByteArray& data = pair.second;

        ezbuffer bufferData(data.size());
        std::memcpy(bufferData.pBuf, data.data(), data.size());
        ezbuffer bufferCompressed;
        ezcompress(bufferCompressed, bufferData);

        std::string package(reinterpret_cast<const char*>(bufferCompressed.pBuf), bufferCompressed.nLen);

        std::cout << "Sending data, size: " << package.size() << "\n";
        RestClient::Response r = RestClient::post(url.toStdString(), "application/octet-stream", package);
        std::cout << "Data sent: " << r.code << " - " <<  r.body << "\n";

        // If the package was successfully sent, remove the pair that originated it from the buffer.
        if(r.code == 200) {
            this->mutex.lock();
            this->buffer.removeFirst();
            this->mutex.unlock();
        }

        std::cout.flush();
    }
}

QString RequestSender::getServerAddress(const Simulation *simulation) const
{
    QHostAddress serverAddress = simulation->getServerAddress();
    QString url;

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
