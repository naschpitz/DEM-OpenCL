#ifndef SOLIDOBJECT_H
#define SOLIDOBJECT_H

#include "Particle.h"
#include "Vector3D.h"

#include <QJsonValue>
#include <QString>
#include <QVector>

class SolidObject
{
    private:
        QString name;
        QString materialId;
        QString stl;
        double maximumTetrahedronVol;

        bool fixed;
        double mass;

        Vector3D position;
        Vector3D velocity;

        QVector<Particle> particles;

    public:
        SolidObject();
        SolidObject(const QJsonValue& jsonValue);

        const QString& getName()       const;
        const QString& getMaterialId() const;

        Vector3D getCurrentPosition() const;
        Vector3D getCurrentVelocity() const;
        double   getCurrentMass()     const;
        double   getCurrentVolume()   const;

        void getBox(Vector3D& min, Vector3D& max);

        Vector3D getCurrentMomentum()              const;
        Vector3D getCurrentAngularMomentum()       const;
        Vector3D getCurrentForce()                 const;
        double   getCurrentKineticEnergyTotal()    const;
        double   getCurrentKineticEnergyExternal() const;
        double   getCurrentKineticEnergyInternal() const;

        const QVector<Particle>& getParticles() const;
        void setParticlesCL(const QVector<ParticleCL>& particlesCL);
        QJsonObject getJson() const;

    private:
        void loadStl();

        void setFixed();
        void setMass();
        void setMaterialId();
        void setPosition();
        void setVelocity();
};

#endif // SOLIDOBJECT_H
