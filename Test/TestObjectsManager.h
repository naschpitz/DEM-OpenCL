#ifndef TESTOBJECTSMANAGER_H
#define TESTOBJECTSMANAGER_H

#include <QObject>
#include <QJsonValue>

#include "../Core/ObjectsManager.h"

class TestObjectsManager : public QObject
{
    Q_OBJECT

    private:
        QJsonValue objectsManagerJsonValue;

    public:
        TestObjectsManager();

    private Q_SLOTS:
        void constructor();
};

#endif // TESTOBJECTSMANAGER_H
