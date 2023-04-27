#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef NONSOLIDOBJECT_H
#define NONSOLIDOBJECT_H

#include "nlohmann/json.hpp"
#include "Particle.h"
#include "Vector3D.h"

#include <QVector>
#include <QMap>
#include <CL/opencl.hpp>

class NonSolidObject
{
    private:
        QString id;
        QString material;

        bool fixed;
        double density;

        Vector3D length;
        double spacing;

        Vector3D position;
        Vector3D velocity;

        QVector<Particle> particles;

    public:
        NonSolidObject();
        NonSolidObject(const nlohmann::json& jsonObject);

        void initialize();

        const QString& getId()       const;
        const QString& getMaterial() const;

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
        nlohmann::json getJson(bool detailed) const;

        void reset();

    private:
        void setMaterial();
        void setFixed();
        void setDensity();
        void setPosition();
        void setVelocity();

        void buildObject();
};

#endif // NONSOLIDOBJECT_H
