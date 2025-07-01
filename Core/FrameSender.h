#ifndef FRAMESENDER_H
#define FRAMESENDER_H

#include <QFile>
#include <QMutex>
#include <QPair>
#include <QSharedPointer>
#include <QThread>
#include <QVector>

#include "Deflater.h"

class FrameSender : public QThread
{
    Q_OBJECT

    public:
        FrameSender();

        void send(const QString& url, QSharedPointer<QFile> file);
        void setFramesDir(const QString& framesDir);

        const QString& getFramesDir() const;

    private:
        QMutex mutex;
        QString framesDir;

	QVector<QPair<QString, QSharedPointer<QFile>>> inflatedFilePairs;
	QVector<QPair<QSharedPointer<QFile>, QSharedPointer<QFile>>> deflatedFilePairs;

	Deflater deflater;

	void sendFrame(const QString& url, QPair<QSharedPointer<QFile>, QSharedPointer<QFile>> deflatedFilePair);

    protected:
        void run();

    public slots:
        void fileDeflated(QPair<QSharedPointer<QFile>, QSharedPointer<QFile>>);
};

#endif // FRAMESENDER_H
