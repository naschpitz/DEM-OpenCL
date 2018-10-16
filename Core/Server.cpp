#include "Server.h"

#include <QJsonValue>
#include <QDebug>

Server::Server()
{
    this->listen(QHostAddress::Any, 8141);
    bool result = this->isListening();

    this->clientSocket = NULL;

    connect(this, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void Server::acceptConnection()
{
    this->clientSocket = this->nextPendingConnection();

    connect(this->clientSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(this->clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientError(QAbstractSocket::SocketError)));

    this->pauseAccepting();
}

void Server::clientError(QAbstractSocket::SocketError error)
{
    this->clientSocket = NULL;
    this->resumeAccepting();
}

void Server::readyRead()
{
    QByteArray data;

    while(this->clientSocket->bytesAvailable()) {
        data += this->clientSocket->readAll();
    }

    QJsonDocument jsonDocument;
    jsonDocument = QJsonDocument::fromJson(data);

    this->parse(jsonDocument);
}

void Server::parse(const QJsonDocument &jsonDocument)
{
    QJsonObject jsonObject = jsonDocument.object();

    QString command = jsonObject["command"].toString();
    QJsonValue payloadJsonValue = jsonObject["payload"];

    if (command == "start_simulation") {
        QString _id = payloadJsonValue["_id"].toString();

        Simulation* simulation = this->getSimulationById(_id);
        simulation = simulation ? simulation : new Simulation(payloadJsonValue);

        connect(simulation, SIGNAL(newFrame(QJsonObject)), this, SLOT(newFrame(QJsonObject)), Qt::BlockingQueuedConnection);
        connect(simulation, SIGNAL(finished()), this, SLOT(simulationFinished()), Qt::BlockingQueuedConnection);

        this->simulations.insert(simulation);

        simulation->start();
    }

    if (command == "pause_simulation") {
        QString _id = payloadJsonValue["_id"].toString();

        Simulation* simulation = this->getSimulationById(_id);

        simulation->pause();
    }

    if (command == "stop_simulation") {
        QString _id = payloadJsonValue["_id"].toString();

        Simulation* simulation = this->getSimulationById(_id);

        simulation->stop();

        disconnect(simulation);
        simulation->deleteLater();

        this->simulations.remove(simulation);
    }
}

void Server::newFrame(QJsonObject frame)
{
    QJsonObject package;

    package["command"] = "new_frame";
    package["payload"] = frame;

    QJsonDocument document(package);

    QByteArray data = document.toJson();
    data += "### END ###";

    this->clientSocket->write(data);
    this->clientSocket->waitForBytesWritten(600000);
}

void Server::simulationFinished()
{
    Simulation* simulation = (Simulation*)QObject::sender();

    disconnect(simulation, SIGNAL(newFrame(QJsonObject)), this, SLOT(newFrame(QJsonObject)));
    disconnect(simulation, SIGNAL(finished()), this, SLOT(simulationFinished()));

    if (simulation->isStoped())
        simulation->deleteLater();

    this->simulations.remove(simulation);
}

Simulation* Server::getSimulationById(const QString& _id)
{
    foreach(Simulation* simulation, this->simulations) {
        if (simulation->getId() == _id) {
            return simulation;
        }
    }

    return NULL;
}
