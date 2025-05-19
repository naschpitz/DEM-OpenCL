#include "EasyZLib.h"
#include "FrameSender.h"
#include "Sender.h"

#include <QDateTime>
#include <QFile>
#include <QSharedPointer>
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

        QPair<QSharedPointer<QFile>, QSharedPointer<QFile>> deflatedFilePair;

        if (!isDeflatedFilePairsEmpty) {
            this->mutex.lock();
            deflatedFilePair = this->deflatedFilePairs.first();
            this->mutex.unlock();
        }

        else {
            this->sleep(1);
            continue;
        }

        QSharedPointer<QFile> inflatedFile = deflatedFilePair.first;
        QSharedPointer<QFile> deflatedFile = deflatedFilePair.second;

	this->mutex.lock();
	auto inflatedFilePairIterator = std::find_if(this->inflatedFilePairs.begin(), this->inflatedFilePairs.end(), [&](const QPair<QString, QSharedPointer<QFile>>& inflatedFilePair) {
	    return inflatedFilePair.second == inflatedFile;
	});

	if (inflatedFilePairIterator == this->inflatedFilePairs.end()) {
	    this->mutex.unlock();
	    std::cout << "Something went wrong when searching for the inflatedFilePair" << std::endl;
	    continue;
	}

	QString url = inflatedFilePairIterator->first;
	this->mutex.unlock();

	bool result = deflatedFile->open(QIODevice::ReadOnly);

        if (result) {
            QByteArray data = deflatedFile->readAll();
            deflatedFile->close();

            std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString() << "Sending frame, size: " << data.size() << " to " << url.toStdString() << std::endl;
            RestClient::Response r = Sender::getInstance().send(url.toStdString(), "application/gzip", data.toStdString());
            std::cout << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz - ").toStdString() << "Frame sent: " << r.code << " - " <<  r.body << std::endl;
            std::cout.flush();

            // If the package was successfully sent, remove the pair that originated it from the buffer.
            if (r.code == 200 && r.body == "OK") {
                this->mutex.lock();

		auto inflatedFilePairIterator = std::find_if(this->inflatedFilePairs.begin(), this->inflatedFilePairs.end(), [&](const QPair<QString, QSharedPointer<QFile>>& inflatedFilePair) {
		    return inflatedFilePair.second == inflatedFile;
		});

		if (inflatedFilePairIterator == this->inflatedFilePairs.end()) {
		    this->mutex.unlock();
		    std::cout << "Something went wrong when searching for the inflatedFilePair (200)" << std::endl;
		    continue;
		}

                this->inflatedFilePairs.erase(inflatedFilePairIterator);
                this->deflatedFilePairs.removeFirst();
                deflatedFile->remove();

                this->mutex.unlock();
            }

            else {
                this->sleep(1);
            }
        }

	else {
	    std::cout << "Something went wrong when opening the deflated file" << std::endl;
	    continue;
	}
    }
}
