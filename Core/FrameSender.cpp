#include "EasyZLib.h"
#include "FrameSender.h"
#include "Sender.h"
#include "Simulation.h"

#include <QDateTime>
#include <QFile>
#include <QSharedPointer>
#include <QtConcurrent/QtConcurrent>
#include <algorithm>
#include <iostream>

#include "restclient-cpp/restclient.h"

FrameSender::FrameSender()
{
    connect(&this->deflater, SIGNAL(fileDeflated(QSharedPointer<QFile>, QSharedPointer<QFile>)), this, SLOT(fileDeflated(QSharedPointer<QFile>, QSharedPointer<QFile>)), Qt::BlockingQueuedConnection);
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

bool FrameSender::hasFramesToSend(const Simulation* simulation) const
{
    QMutexLocker locker(&this->mutex);

    // Check inflated frames
    for (const FrameData& frameData : this->inflatedFrames) {
        if (frameData.simulation == simulation) {
            return true;
        }
    }

    // Check deflated frames
    for (const FrameData& frameData : this->deflatedFrames) {
        if (frameData.simulation == simulation) {
            return true;
        }
    }

    return false;
}

void FrameSender::waitForAllFramesSent(const Simulation* simulation)
{
    while (this->hasFramesToSend(simulation)) {
        // Sleep for a short time to avoid busy waiting
        QThread::msleep(100);
    }
}

void FrameSender::send(const QString& url, const Simulation* simulation, QSharedPointer<QFile> file)
{
    FrameData frameData;
    frameData.simulation = simulation;
    frameData.url = url;
    frameData.file = file;

    this->mutex.lock();
    this->inflatedFrames.push_back(frameData);
    this->mutex.unlock();

    this->deflater.deflate(file);
    this->deflater.start();

    this->start();
}

void FrameSender::fileDeflated(QSharedPointer<QFile> inflatedFile, QSharedPointer<QFile> deflatedFile)
{

    // Remove the file immediately to free disk space, no need to keep it.
    inflatedFile->remove();

    this->mutex.lock();

    // Find the corresponding frame data
    auto it = std::find_if(this->inflatedFrames.begin(), this->inflatedFrames.end(),
                          [&](const FrameData& frameData) {
                              return frameData.file == inflatedFile;
                          });

    if (it != this->inflatedFrames.end()) {
        FrameData frameData = *it;
        // Update the file reference to point to the deflated file
        frameData.file = deflatedFile;
        this->deflatedFrames.push_back(frameData);

        // Remove from inflated frames since it's now deflated
        this->inflatedFrames.erase(it);
    }

    this->mutex.unlock();
}

void FrameSender::run()
{
    while (true) {
        this->mutex.lock();

        // Look for frames that are not yet scheduled to be sent
        auto it = std::find_if(this->deflatedFrames.begin(), this->deflatedFrames.end(),
                              [](const FrameData& frameData) {
                                  return !frameData.scheduled;
                              });

        if (it == this->deflatedFrames.end()) {
            // No unscheduled frames found
            this->mutex.unlock();
            this->msleep(100);
            continue;
        }

        // Mark this frame as scheduled before sending
        it->scheduled = true;
        FrameData frameData = *it;
        this->mutex.unlock();

        // Dispatch to thread pool for concurrent sending
        QtConcurrent::run([this, frameData]() {
            sendFrame(frameData);
        });
    }
}

void FrameSender::sendFrame(const FrameData& frameData)
{
    QSharedPointer<QFile> deflatedFile = frameData.file;
    const QString& url = frameData.url;

    while (true) {
        if (!deflatedFile->open(QIODevice::ReadOnly)) {
            std::cout << "Failed to open deflated file. Retrying in 1s..." << std::endl;
            QThread::msleep(100);
            continue;
        }

        QByteArray data = deflatedFile->readAll();
        deflatedFile->close();

        std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString()
            << "Sending frame (" << data.size() << " bytes) to " << url.toStdString() << std::endl;

        RestClient::Response r = Sender::getInstance().send(url.toStdString(), "application/gzip", data.toStdString());

        if (r.code == 200 && r.body == "OK") {
            std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString()
                << "Frame sent successfully." << std::endl;

            this->mutex.lock();

            // Remove the frame from deflatedFrames now that it's actually sent
            auto it = std::find_if(this->deflatedFrames.begin(), this->deflatedFrames.end(),
                    [&](const FrameData& bufferData) {
                        return bufferData.simulation == frameData.simulation && bufferData.file == frameData.file;
                    });

            if (it != this->deflatedFrames.end())
                this->deflatedFrames.erase(it);

            // Clean up the deflated file
            deflatedFile->remove();

            this->mutex.unlock();
            break; // Exit the retry loop
        } else {
            std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString()
                << "Failed to send frame. Retrying in 1s..." << std::endl;
            QThread::msleep(100);
        }
    }
}
