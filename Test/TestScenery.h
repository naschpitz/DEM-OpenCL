#ifndef TESTSCENERY_H
#define TESTSCENERY_H

#include <QObject>
#include <QJsonValue>

#include "../Core/Scenery.h"

class TestScenery : public QObject
{
    Q_OBJECT

    private:
        QJsonValue sceneryJsonValue;

    public:
        TestScenery();

    private Q_SLOTS:
        void constructor();
};

#endif // TESTSCENERY_H
