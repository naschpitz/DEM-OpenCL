#ifndef TESTNONSOLIDOBJECT_H
#define TESTNONSOLIDOBJECT_H

#include <QObject>

#include "../NonSolidObject.h"

class TestNonSolidObject : public QObject
{
    Q_OBJECT

    private:
        nlohmann::json nonSolidObjectWireframeJsonValue;

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
        void particlePackingCount();
};

#endif // TESTNONSOLIDOBJECT_H
