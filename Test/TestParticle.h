#ifndef TESTPARTICLE_H
#define TESTPARTICLE_H

#include <QObject>
#include <QJsonObject>

#include "../Core/Particle.h"

class TestParticle : public QObject
{
    Q_OBJECT

    private:
        QJsonObject particleJsonValue;

    public:
        TestParticle();

    private Q_SLOTS:
        void constructor();
        void getCL();
        void setCL();
};

#endif // TESTPARTICLE_H
