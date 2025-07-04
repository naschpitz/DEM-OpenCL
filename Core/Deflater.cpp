#include "Deflater.h"
#include "Pigz.h"

#include <iostream>

Deflater::Deflater() {

}

void Deflater::deflate(QSharedPointer<QFile> file) {
    DeflationTask task;
    task.type = FileData;
    task.data = QVariant::fromValue(file);

    this->mutex.lock();
    this->deflationQueue.push_back(task);
    this->mutex.unlock();
}

void Deflater::deflate(QSharedPointer<QByteArray> data) {
    DeflationTask task;
    task.type = MemoryData;
    task.data = QVariant::fromValue(data);

    this->mutex.lock();
    this->deflationQueue.push_back(task);
    this->mutex.unlock();
}

void Deflater::run()
{
    while (true) {
        this->mutex.lock();
        bool isQueueEmpty = this->deflationQueue.isEmpty();
        this->mutex.unlock();

        if (isQueueEmpty) {
            this->msleep(100);
            continue;
        }

        this->mutex.lock();
        DeflationTask task = this->deflationQueue.first();
        this->deflationQueue.removeFirst();
        this->mutex.unlock();

        if (task.type == FileData) {
            // Handle file deflation
            QSharedPointer<QFile> inflatedFile = task.data.value<QSharedPointer<QFile>>();

            if (inflatedFile->open(QIODevice::ReadOnly)) {
                QByteArray inflatedData = inflatedFile->readAll();
                inflatedFile->close();

                QByteArray deflatedData = Pigz::deflate(inflatedData);

                QString inflatedFilename = inflatedFile->fileName();
                QString deflatedFilename = inflatedFilename;
                deflatedFilename.chop(5); // Remove ".json"
                deflatedFilename += ".gz";

                QSharedPointer<QFile> deflatedFile = QSharedPointer<QFile>::create(deflatedFilename);
                deflatedFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
                deflatedFile->write(deflatedData);
                deflatedFile->flush();
                deflatedFile->close();

                emit this->fileDeflated(inflatedFile, deflatedFile);
            } else {
                std::cout << "Something went wrong when opening the inflated file" << std::endl;
            }
        } else if (task.type == MemoryData) {
            // Handle memory deflation
            QSharedPointer<QByteArray> inflatedData = task.data.value<QSharedPointer<QByteArray>>();
            QByteArray deflatedDataRaw = Pigz::deflate(*inflatedData);
            QSharedPointer<QByteArray> deflatedData = QSharedPointer<QByteArray>::create(deflatedDataRaw);

            emit this->dataDeflated(inflatedData, deflatedData);
        }
    }
}
