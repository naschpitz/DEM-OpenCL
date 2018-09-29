#ifndef TESTMATERIALSMANAGER_H
#define TESTMATERIALSMANAGER_H

#include <QObject>
#include <QJsonValue>

#include "../Core/MaterialsManager.h"

class TestMaterialsManager : public QObject
{
    Q_OBJECT

    private:
        QJsonValue jsonValue;

    public:
        TestMaterialsManager();

    private Q_SLOTS:
        void constructor();
        void getCL();
};

#endif // TESTMATERIALSMANAGER_H
