#ifndef LOGSENDER_H
#define LOGSENDER_H

#include <QMutex>
#include <QPair>
#include <QThread>
#include <QVector>

#include "Deflater.h"

class LogSender : public QThread
{
    Q_OBJECT

    public:
        LogSender();

        void send(const QString& url, const QByteArray& data);

    private:
        // Structure to hold log data
        struct LogData {
            QString url;
            QSharedPointer<QByteArray> data;
            bool scheduled = false;
        };

        QMutex mutex;
        QVector<LogData> inflatedLogs;
        QVector<LogData> deflatedLogs;

        Deflater deflater;

        void sendLog(const LogData& logData);

    protected:
        void run();

    public slots:
        void dataDeflated(QSharedPointer<QByteArray> inflatedData, QSharedPointer<QByteArray> deflatedData);
};

#endif // LOGSENDER_H
