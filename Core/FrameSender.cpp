#include "EasyZLib.h"
#include "FrameSender.h"
#include "Sender.h"

#include <QDateTime>
#include <QFile>
#include <QSharedPointer>
#include <QtConcurrent/QtConcurrent>
#include <iostream>

#include "restclient-cpp/restclient.h"

FrameSender::FrameSender()
{
    connect(&this->deflater, SIGNAL(fileDeflated(QPair<QSharedPointer<QFile>, QSharedPointer<QFile>>)), this, SLOT(fileDeflated(QPair<QSharedPointer<QFile>, QSharedPointer<QFile>>)), Qt::BlockingQueuedConnection);
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

void FrameSender::send(const QString& url, QSharedPointer<QFile> file)
{
    QPair<QString, QSharedPointer<QFile>> inflatedFilePair = qMakePair(url, file);

    this->mutex.lock();
    this->inflatedFilePairs.push_back(inflatedFilePair);
    this->mutex.unlock();

    QSharedPointer<QFile> inflatedFile = inflatedFilePair.second;

    this->deflater.deflate(inflatedFile);
    this->deflater.start();

    this->start();
}

void FrameSender::fileDeflated(QPair<QSharedPointer<QFile>, QSharedPointer<QFile>> deflatedFilePair)
{
    QSharedPointer<QFile> inflatedFile = deflatedFilePair.first;
    // Remove the file immediately to free disk space, no need to keep it.
    inflatedFile->remove();

    this->mutex.lock();
    this->deflatedFilePairs.push_back(deflatedFilePair);
    this->mutex.unlock();
}

void FrameSender::run()
{
    while (true) {
        this->mutex.lock();
        bool isDeflatedFilePairsEmpty = this->deflatedFilePairs.isEmpty();
        this->mutex.unlock();

	if (isDeflatedFilePairsEmpty) {
	    this->sleep(1);
	    continue;
	}

	this->mutex.lock();

	QPair<QSharedPointer<QFile>, QSharedPointer<QFile>> deflatedFilePair = this->deflatedFilePairs.takeFirst();

	QString url;

	auto it = std::find_if(this->inflatedFilePairs.begin(), this->inflatedFilePairs.end(), [&](const QPair<QString, QSharedPointer<QFile>>& pair) {
	    return pair.second == deflatedFilePair.first;
	});

	if (it != this->inflatedFilePairs.end()) {
	    url = it->first;
	}

	this->mutex.unlock();

	// Dispatch to thread pool for concurrent sending
	QtConcurrent::run([this, url, deflatedFilePair]() {
	    sendFrame(url, deflatedFilePair);
	});
    }
}

void FrameSender::sendFrame(const QString& url, QPair<QSharedPointer<QFile>, QSharedPointer<QFile>> deflatedFilePair)
{
    QSharedPointer<QFile> inflatedFile = deflatedFilePair.first;
    QSharedPointer<QFile> deflatedFile = deflatedFilePair.second;

    while (true) {
        if (!deflatedFile->open(QIODevice::ReadOnly)) {
            std::cout << "Failed to open deflated file. Retrying in 1s..." << std::endl;
            QThread::sleep(1);
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

	    // Remove from internal queues
	    auto it = std::find_if(this->inflatedFilePairs.begin(), this->inflatedFilePairs.end(),
				   [&](const QPair<QString, QSharedPointer<QFile>>& pair) {
				       return pair.second == inflatedFile;
				   });

	    if (it != this->inflatedFilePairs.end())
		this->inflatedFilePairs.erase(it);

	    deflatedFile->remove();
	    this->mutex.unlock();

	    break; // Exit the retry loop
	} else {
	    std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString()
		      << "Failed to send frame. Retrying in 1s..." << std::endl;
	    QThread::sleep(1);
	}
    }
}
