#ifndef DEFLATER_H
#define DEFLATER_H

#include <QByteArray>
#include <QFile>
#include <QMutex>
#include <QPair>
#include <QSharedPointer>
#include <QThread>
#include <QVector>
#include <QVariant>

class Deflater : public QThread
{
    Q_OBJECT

    public:
        enum DataType {
            FileData,
            MemoryData
        };

        struct DeflationTask {
            DataType type;
            QVariant data;  // QSharedPointer<QFile> for files, QSharedPointer<QByteArray> for memory
        };

        Deflater();

        // File deflation (maintains backward compatibility)
        void deflate(QSharedPointer<QFile> file);

        // Memory deflation
        void deflate(QSharedPointer<QByteArray> data);

    private:
        QMutex mutex;
        QVector<DeflationTask> deflationQueue;

    protected:
        void run();

    signals:
        // Signal for file deflation completion
        void fileDeflated(QSharedPointer<QFile> inflatedFile, QSharedPointer<QFile> deflatedFile);

        // Signal for memory deflation completion
        void dataDeflated(QSharedPointer<QByteArray> inflatedData, QSharedPointer<QByteArray> deflatedData);
};

#endif // DEFLATER_H
