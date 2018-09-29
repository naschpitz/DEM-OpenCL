#ifndef TESTNONSOLIDOBJECT_H
#define TESTNONSOLIDOBJECT_H

#include <QObject>
#include <QJsonValue>

#include "../Core/NonSolidObject.h"

class TestNonSolidObject : public QObject
{
    Q_OBJECT

    private:
        QJsonValue nonSolidObjectWireframeJsonValue;
        QJsonValue nonSolidObjectInstanceJsonValue;

    public:
        TestNonSolidObject();

    private Q_SLOTS:
        void constructorWireframe();
        void constructorInstance();
        void getMaterialId();
        void getBox();
        void getCurrentMomentum();
        void getCurrentKineticEnergyTotal();
        void getCurrentKineticEnergyExternal();
        void getCurrentKineticEnergyInternal();
};

#endif // TESTNONSOLIDOBJECT_H
