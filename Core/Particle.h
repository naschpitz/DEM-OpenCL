#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vertex.h"

#include <QJsonValue>
#include <QJsonObject>

typedef struct
{
    cl_uint index;
    cl_uint materialIndex;

    cl_double radius;
    cl_double density;
    cl_double mass;
    cl_double area;
    cl_double volume;

    cl_double4 currentForce;
    cl_double4 oldForce;

    cl_double4 currentTorque;
    cl_double4 oldTorque;

    VertexCL vertex;
} ParticleCL;

class Particle : public Vertex
{
    private:
        QString _id;
        QString materialId;

        double radius;
        double density;
        double mass;
        double area;
        double volume;

        Vector3D currentForce;
        Vector3D oldForce;

        Vector3D currentTorque;
        Vector3D oldTorque;

    public:
        Particle();
        Particle(const double& x, const double& y, const double& z, const double& radius);
        Particle(const Vector3D& vector, const double& radius);
        Particle(const QJsonValue& jsonValue);

        ParticleCL getCL(uint index, uint materialIndex) const;
        void setCL(const ParticleCL& particleCL);

        void setMaterial(const QString& materialId);
        void setDensity(const double& density);

        const QString& getId() const;

        const double& getRadius() const;
        const double& getMass()   const;
        const double& getArea()   const;
        const double& getVolume() const;

        const Vector3D& getCurrentForce() const;

        const Vector3D& getCurrentTorque()   const;
        Vector3D getCurrentMomentum()        const;
        Vector3D getCurrentAngularMomentum() const;

        double getCurrentKineticEnergyTotal()    const;
        double getCurrentKineticEnergyExternal() const;
        double getCurrentKineticEnergyInternal() const;

        QJsonObject getJson() const;

        bool isInside(const Particle& otherParticle) const;

        void calculateArea();
        void calculateVolume();

        virtual bool operator==(const Particle &other) const;
};

#endif // PARTICLE_H
