#include "EasyZLib.h"
#include "FrameSender.h"

#include <QFile>
#include <iostream>
#include <cstring>

#include "restclient-cpp/restclient.h"

FrameSender::FrameSender()
{

}

void FrameSender::setFramesDir(const QString &framesDir)
{
    this->mutex.lock();
    this->framesDir = framesDir;
    this->mutex.unlock();
}

const QString& FrameSender::getFramesDir() const
{
    return this->framesDir;
}

void FrameSender::send(const QString& url, const QString& filename)
{
    QPair<QString, QString> pair = QPair<QString, QString>(url, filename);

    this->mutex.lock();
    this->buffer.push_back(pair);
    this->mutex.unlock();

    this->start();
}

void FrameSender::run()
{
    while (true) {
        QPair<QString, QString> pair;

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
        QString& filename = pair.second;

        this->mutex.lock();
        QFile file(this->framesDir + filename);
        this->mutex.unlock();

        file.open(QIODevice::ReadOnly);
        QByteArray data = file.readAll();

        std::string package = EasyZLib::deflate(data);

        std::cout << "Sending frame, size: " << package.size() << "\n";
        RestClient::Response r = RestClient::post(url.toStdString(), "application/octet-stream", package);
        std::cout << "Frame sent: " << r.code << " - " <<  r.body << "\n";

        // If the package was successfully sent, remove the pair that originated it from the buffer.
        if(r.code == 200) {
            this->mutex.lock();
            this->buffer.removeFirst();
            this->mutex.unlock();

            file.remove();
        }

        std::cout.flush();
    }
}
