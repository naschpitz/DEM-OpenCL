#include "Deflater.h"
#include "LogSender.h"
#include "Sender.h"

#include <QDateTime>
#include <QtConcurrent/QtConcurrent>
#include <algorithm>
#include <iostream>
#include <cstring>

#include "restclient-cpp/restclient.h"

LogSender::LogSender()
{
    connect(&this->deflater, SIGNAL(dataDeflated(QSharedPointer<QByteArray>, QSharedPointer<QByteArray>)), this, SLOT(dataDeflated(QSharedPointer<QByteArray>, QSharedPointer<QByteArray>)), Qt::BlockingQueuedConnection);
}

void LogSender::send(const QString& url, const QByteArray& data)
{
    LogData logData;
    logData.url = url;
    logData.data = QSharedPointer<QByteArray>::create(data);

    this->mutex.lock();
    this->inflatedLogs.push_back(logData);
    this->mutex.unlock();

    // Send to deflater for processing
    this->deflater.deflate(logData.data);
    this->deflater.start();

    this->start();
}

void LogSender::dataDeflated(QSharedPointer<QByteArray> inflatedData, QSharedPointer<QByteArray> deflatedData)
{
    this->mutex.lock();

    // Find the corresponding log data and remove it from inflated logs
    auto it = std::find_if(this->inflatedLogs.begin(), this->inflatedLogs.end(),
                          [&](const LogData& logData) {
                              return logData.data == inflatedData;
                          });

    if (it != this->inflatedLogs.end()) {
        LogData logData = *it;
        // Update the data to point to the deflated data
        logData.data = deflatedData;
        this->deflatedLogs.push_back(logData);

        // Remove from inflated logs since it's now deflated
        this->inflatedLogs.erase(it);
    }

    this->mutex.unlock();
}

void LogSender::run()
{
    while (true) {
        this->mutex.lock();

        // Look for logs that are not yet scheduled to be sent
        auto it = std::find_if(this->deflatedLogs.begin(), this->deflatedLogs.end(),
                              [](const LogData& logData) {
                                  return !logData.scheduled;
                              });

        if (it == this->deflatedLogs.end()) {
            // No unscheduled logs found
            this->mutex.unlock();
            this->msleep(100);
            continue;
        }

        // Mark this log as scheduled before sending
        it->scheduled = true;
        LogData logData = *it;
        this->mutex.unlock();

        // Dispatch to thread pool for concurrent sending
        auto future = QtConcurrent::run([this, logData]() {
            sendLog(logData);
        });
        Q_UNUSED(future);
    }
}

void LogSender::sendLog(const LogData& logData)
{
    const QString& url = logData.url;
    const QByteArray& deflatedData = *logData.data;  // Dereference shared pointer

    while (true) {
        std::string package = deflatedData.toStdString();

        std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString()
                  << "Sending log, size: " << package.size() << " to " << url.toStdString() << std::endl;

        RestClient::Response r = Sender::getInstance().send(url.toStdString(), "application/octet-stream", package);

        std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString()
                  << "Log sent: " << r.code << " - " << r.body << std::endl;
        std::cout.flush();

        // If the package was successfully sent, remove the pair that originated it from the buffer.
        if (r.code == 200 && r.body == "OK") {
            this->mutex.lock();

            // Find and remove the specific log data from deflated logs
            auto it = std::find_if(this->deflatedLogs.begin(), this->deflatedLogs.end(),
                                  [&](const LogData& bufferData) {
                                      return bufferData.data == logData.data;
                                  });

            if (it != this->deflatedLogs.end()) {
                this->deflatedLogs.erase(it);
            }

            this->mutex.unlock();
            break; // Exit the retry loop
        } else {
            std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString()
                      << "Failed to send log. Retrying in 1s..." << std::endl;
            QThread::msleep(1000);
        }
    }
}
