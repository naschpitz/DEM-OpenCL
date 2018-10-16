#ifndef TESTNONSOLIDOBJECT_H
#define TESTNONSOLIDOBJECT_H

#include <QObject>
#include <QJsonObject>

#include "../Core/NonSolidObject.h"

class TestNonSolidObject : public QObject
{
    Q_OBJECT

    private:
        QJsonObject nonSolidObjectWireframeJsonValue;

    public:
        TestNonSolidObject();

    private Q_SLOTS:
        void constructorWireframe();
        void getMaterialId();
        void getBox();
        void getCurrentMomentum();
        void getCurrentKineticEnergyTotal();
        void getCurrentKineticEnergyExternal();
        void getCurrentKineticEnergyInternal();
};

#endif // TESTNONSOLIDOBJECT_H
