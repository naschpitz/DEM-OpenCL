#ifndef DEFLATER_H
#define DEFLATER_H

#include <QFile>
#include <QMutex>
#include <QPair>
#include <QSharedPointer>
#include <QThread>
#include <QVector>

class Deflater : public QThread
{
    Q_OBJECT

    public:
        Deflater();
        void deflate(QSharedPointer<QFile>);

    private:
        QMutex mutex;
        QVector<QSharedPointer<QFile>> inflatedFiles;

    protected:
        void run();

    signals:
        void fileDeflated(QPair<QSharedPointer<QFile>, QSharedPointer<QFile>> deflatedFilePair);
};

#endif // DEFLATER_H
