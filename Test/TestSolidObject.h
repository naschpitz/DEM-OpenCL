#ifndef TESTSOLIDOBJECT_H
#define TESTSOLIDOBJECT_H

#include <QObject>
#include <QJsonObject>

#include "../Core/SolidObject.h"

class TestSolidObject : public QObject
{
    Q_OBJECT

    private:
        QJsonObject solidObjectJsonValue;

    public:
        TestSolidObject();

    private Q_SLOTS:
        void constructor();
        void getMaterialId();
        void getBox();
        void getCurrentMomentum();
        void getCurrentKineticEnergyTotal();
};

#endif // TESTSOLIDOBJECT_H
