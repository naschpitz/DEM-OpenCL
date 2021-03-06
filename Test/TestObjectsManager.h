#ifndef TESTOBJECTSMANAGER_H
#define TESTOBJECTSMANAGER_H

#include <QObject>
#include <QJsonObject>

#include "../Core/ObjectsManager.h"

class TestObjectsManager : public QObject
{
    Q_OBJECT

    private:
        QJsonObject objectsManagerJsonValue;

    public:
        TestObjectsManager();

    private Q_SLOTS:
        void constructor();
};

#endif // TESTOBJECTSMANAGER_H
