#ifndef TESTMATEIRAL_H
#define TESTMATEIRAL_H

#include <QObject>
#include <QJsonObject>

#include "../Core/Material.h"

class TestMaterial : public QObject
{
    Q_OBJECT

    private:
        QJsonObject jsonValue;

    public:
        TestMaterial();

    private Q_SLOTS:
        void constructor();
        void getCL();
};

#endif // TESTMATEIRAL_H
