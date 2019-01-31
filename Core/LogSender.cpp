#include "EasyZLib.h"
#include "LogSender.h"
#include "Sender.h"

#include <iostream>
#include <cstring>

#include "restclient-cpp/restclient.h"

LogSender::LogSender()
{

}

void LogSender::send(const QString& url, const QByteArray& data)
{
    QPair<QString, QByteArray> pair = QPair<QString, QByteArray>(url, data);

    this->mutex.lock();
    this->buffer.push_back(pair);
    this->mutex.unlock();

    this->start();
}

void LogSender::run()
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

        std::string package = EasyZLib::deflate(data);

        std::cout << "Sending log, size: " << package.size() << "\n";
        RestClient::Response r = Sender::getInstance().send(url.toStdString(), "application/octet-stream", package);
        std::cout << "Log sent: " << r.code << " - " <<  r.body << "\n";
        std::cout.flush();

        // If the package was successfully sent, remove the pair that originated it from the buffer.
        if(r.code == 200) {
            this->mutex.lock();
            this->buffer.removeFirst();
            this->mutex.unlock();
        }

        else {
            this->sleep(5);
        }
    }
}
