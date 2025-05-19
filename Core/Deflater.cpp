#include "Deflater.h"
#include "Pigz.h"

#include <iostream>

Deflater::Deflater() {

}

void Deflater::deflate(QSharedPointer<QFile> inflatedFile) {
    this->mutex.lock();
    this->inflatedFiles.push_back(inflatedFile);
    this->mutex.unlock();
}

void Deflater::run()
{
    while (true) {
        this->mutex.lock();
        bool isInflatedFilesEmpty = this->inflatedFiles.empty();
        this->mutex.unlock();

        QSharedPointer<QFile> inflatedFile;

	if (!isInflatedFilesEmpty) {
	    this->mutex.lock();
	    inflatedFile = this->inflatedFiles.first();
	    this->mutex.unlock();
	}

	else {
	    this->sleep(1);
	    continue;
	}

	bool result = inflatedFile->open(QIODevice::ReadOnly);

	if (result) {
	    QByteArray inflatedData = inflatedFile->readAll();
	    inflatedFile->close();

	    QByteArray deflatedData = Pigz::deflate(inflatedData);

	    QString inflatedFilename = inflatedFile->fileName();
	    QString defatedFilename = inflatedFilename;
	    defatedFilename.chop(5); // Remove ".json"
	    defatedFilename += ".gz";

	    QSharedPointer<QFile> deflatedFile = QSharedPointer<QFile>::create(defatedFilename);
	    deflatedFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
	    deflatedFile->write(deflatedData);
	    deflatedFile->flush();
	    deflatedFile->close();

	    QPair<QSharedPointer<QFile>, QSharedPointer<QFile>> deflatedFilePair = qMakePair(inflatedFile, deflatedFile);

	    emit this->fileDeflated(deflatedFilePair);

	    // Remove from the list of files to be deflated
	    this->inflatedFiles.removeFirst();
	}

	else {
	    std::cout << "Something went wrong when opening the inflated file" << std::endl;
	    continue;
	}
    }
}
