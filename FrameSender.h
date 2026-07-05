#ifndef FRAMESENDER_H
#define FRAMESENDER_H

#include <QFile>
#include <QMutex>
#include <QPair>
#include <QSharedPointer>
#include <QThread>
#include <QVector>

#include "Deflater.h"

// Forward declaration to avoid circular dependency
class Simulation;

class FrameSender : public QThread
{
    Q_OBJECT

    public:
        FrameSender();

        void send(const QString& url, const Simulation* simulation, QSharedPointer<QFile> file);
        void setFramesDir(const QString& framesDir);


        const QString& getFramesDir() const;

        // Methods to check frame sending status per simulation
        bool hasFramesToSend(const Simulation* simulation) const;
        void waitForAllFramesSent(const Simulation* simulation);

    private:
        mutable QMutex mutex;
        QString framesDir;

	// Structure to hold frame data with simulation reference and URL
	struct FrameData {
	    const Simulation* simulation;
	    QString url;
	    QSharedPointer<QFile> file;
	    bool scheduled = false;
	};

	QVector<FrameData> inflatedFrames;
	QVector<FrameData> deflatedFrames;

	Deflater deflater;

	void sendFrame(const FrameData& frameData);

    protected:
        void run();

    public slots:
        void fileDeflated(QSharedPointer<QFile> inflatedFile, QSharedPointer<QFile> deflatedFile);
};

#endif // FRAMESENDER_H
