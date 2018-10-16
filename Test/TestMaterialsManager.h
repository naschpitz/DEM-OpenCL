#ifndef TESTMATERIALSMANAGER_H
#define TESTMATERIALSMANAGER_H

#include <QObject>
#include <QJsonArray>

#include "../Core/MaterialsManager.h"

class TestMaterialsManager : public QObject
{
    Q_OBJECT

    private:
        QJsonArray jsonArray;

    public:
        TestMaterialsManager();

    private Q_SLOTS:
        void constructor();
        void getCL();
};

#endif // TESTMATERIALSMANAGER_H
