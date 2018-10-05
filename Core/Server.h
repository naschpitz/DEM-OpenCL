#ifndef SERVER_H
#define SERVER_H

#include "Simulation.h"

#include <QJsonDocument>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class Server : public QTcpServer
{
    Q_OBJECT

    private:
        QTcpSocket* clientSocket;
        QSet<Simulation*> simulations;

        void parse(const QJsonDocument& jsonDocument);

    private slots:
        void acceptConnection();
        void readyRead();
        void clientError(QAbstractSocket::SocketError);
        void newFrame(QJsonDocument frame);
        void simulationFinished();

        Simulation* getSimulationById(const QString& _id);

    public:
        Server();
};

#endif // SERVER_H
