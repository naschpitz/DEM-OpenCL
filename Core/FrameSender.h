#ifndef FRAMESENDER_H
#define FRAMESENDER_H

#include <QMutex>
#include <QPair>
#include <QThread>
#include <QVector>

class FrameSender : public QThread
{
    public:
        FrameSender();

        void send(const QString& url, const QString& filename);
        void setFramesDir(const QString& framesDir);

        const QString& getFramesDir() const;

    private:
        QMutex mutex;
        QString framesDir;
        QVector<QPair<QString, QString> > buffer;

    protected:
        void run();
};

#endif // FRAMESENDER_H
