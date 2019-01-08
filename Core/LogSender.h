#ifndef LOGSENDER_H
#define LOGSENDER_H

#include <QMutex>
#include <QPair>
#include <QThread>
#include <QVector>

class LogSender : public QThread
{
    public:
        LogSender();

        void send(const QString& url, const QByteArray& data);

    private:
        QMutex mutex;
        QVector<QPair<QString, QByteArray> > buffer;

    protected:
        void run();
};

#endif // LOGSENDER_H
