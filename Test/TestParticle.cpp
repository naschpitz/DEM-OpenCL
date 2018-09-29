#include "TestParticle.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTest>
#include <math.h>

TestParticle::TestParticle()
{
    QFile particleFile("../Particle.json");
    particleFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString particleFileString = particleFile.readAll();
    particleFile.close();

    QJsonDocument particleJsonDocument = QJsonDocument::fromJson(particleFileString.toUtf8());
    this->particleJsonValue = particleJsonDocument.object();
}

void TestParticle::constructor()
{
    Particle particle(this->particleJsonValue);

    Vector3D expectedCurrentPosition(0, 1, 2);
    double   expectedRadius = 0.5;

    QCOMPARE(particle.getCurrentPosition(), expectedCurrentPosition);
    QCOMPARE(particle.getRadius(), expectedRadius);
}

void TestParticle::getCL()
{
    Particle particle(1, 2, 3, 4);

    ParticleCL particleCL = particle.getCL(10, 20);

    QCOMPARE(particleCL.vertex.currentPosition.x, particle.getCurrentPosition().getX());
    QCOMPARE(particleCL.vertex.currentPosition.y, particle.getCurrentPosition().getY());
    QCOMPARE(particleCL.vertex.currentPosition.z, particle.getCurrentPosition().getZ());
}

void TestParticle::setCL()
{
    ParticleCL particleCL;
    particleCL.vertex.currentPosition.x = 1;
    particleCL.vertex.currentPosition.y = 2;
    particleCL.vertex.currentPosition.z = 3;

    particleCL.currentForce.x = 4;
    particleCL.currentForce.y = 5;
    particleCL.currentForce.z = 6;

    Particle particle;
    particle.setCL(particleCL);

    QCOMPARE(particle.getCurrentPosition().getX(), particleCL.vertex.currentPosition.x);
    QCOMPARE(particle.getCurrentPosition().getY(), particleCL.vertex.currentPosition.y);
    QCOMPARE(particle.getCurrentPosition().getZ(), particleCL.vertex.currentPosition.z);

    QCOMPARE(particle.getCurrentForce().getX(), particleCL.currentForce.x);
    QCOMPARE(particle.getCurrentForce().getY(), particleCL.currentForce.y);
    QCOMPARE(particle.getCurrentForce().getZ(), particleCL.currentForce.z);
}
