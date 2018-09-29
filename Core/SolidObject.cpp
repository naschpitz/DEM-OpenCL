#include "SolidObject.h"
#include "Vector3D.h"
#include "Vertex.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <tetgen.h>

SolidObject::SolidObject()
{

}

SolidObject::SolidObject(const QJsonValue& jsonValue)
{
    this->name = jsonValue["name"].toString();
    this->materialId = jsonValue["materialId"].toString();

    this->stl = jsonValue["stl"].toString();
    this->maximumTetrahedronVol = jsonValue["maximumTetrahedronVolume"].toDouble(-1);

    this->loadStl();

    this->fixed = jsonValue["fixed"].toBool();
    this->mass  = jsonValue["mass"].toDouble();

    QJsonArray positionArray = jsonValue["position"].toArray();
    QJsonArray velocityArray = jsonValue["velocity"].toArray();

    this->position = Vector3D(positionArray[0].toDouble(), positionArray[1].toDouble(), positionArray[2].toDouble());
    this->velocity = Vector3D(velocityArray[0].toDouble(), velocityArray[1].toDouble(), velocityArray[2].toDouble());

    this->setMaterialId();
    this->setFixed();
    this->setMass();
    this->setPosition();
    this->setVelocity();
}

void SolidObject::loadStl()
{
    tetgenio in;

    QFile file("temp.stl");
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    file.write(this->stl.toUtf8().data());
    file.flush();

    in.load_stl(QString("temp.stl").toLatin1().data());

    file.remove();

    QVector<Vertex> vertexes;

    for(int i = 0; i < in.numberofpoints; i++)
    {
        double x = in.pointlist[i * 3];
        double y = in.pointlist[i * 3 + 1];
        double z = in.pointlist[i * 3 + 2];

        vertexes.append(Vertex(x, y, z));
    }

    for (int i = 0; i < in.numberoffacets; i++)
    {
        tetgenio::facet facet = in.facetlist[i];

        for (int j = 0; j < facet.numberofpolygons; j++)
        {
            tetgenio::polygon polygon = facet.polygonlist[j];

            int v1Index = polygon.vertexlist[0];
            int v2Index = polygon.vertexlist[1];
            int v3Index = polygon.vertexlist[2];

            const Vector3D v1 = vertexes.at(v1Index - 1).getCurrentPosition();
            const Vector3D v2 = vertexes.at(v2Index - 1).getCurrentPosition();
            const Vector3D v3 = vertexes.at(v3Index - 1).getCurrentPosition();

            Vector3D e1 = v2 - v1;
            Vector3D e2 = v3 - v1;

            double u = -1.0;
            double v = e1.getModuleSquared() / Vector3D::dotProduct(e1, e2);

            Vector3D p = (u * e1) + (v * e2);
            Vector3D unitE1 = e1.getUnitary();
            Vector3D unitE2 = e2.getUnitary();
            Vector3D unitP = p.getUnitary();

            double modE1 = e1.getModule();
            double modE2 = e2.getModule();
            double modP  = p.getModule();

            double smallest = modE1 < modE2 ? modE1 : modE2;
            double radius = smallest / 10.0 / 2.0;

            QVector<Particle> particles;

            for (double lengthP = 0; lengthP < modP; lengthP += 2 * radius + 0.001 * radius)
            {
                for (double lengthE1 = 0; lengthE1 < modE1; lengthE1 += 2 * radius + 0.001 * radius)
                {
                    Vector3D vPos = lengthE1 * unitE1 + lengthP * unitP;

                    u = Vector3D::dotProduct(vPos, unitE1) / modE1;
                    v = Vector3D::dotProduct(vPos, unitE2) / modE2;

                    if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && (u + v) <= 1) {
                        Vector3D pos = v1 + vPos;

                        Particle particle(pos, radius);

                        bool coincident = false;
                        foreach(const Particle& thisParticle, this->particles)
                            coincident |= thisParticle.isInside(particle);

                        if (!coincident)
                            particles.append(particle);
                    }
                }
            }

            this->particles.append(particles);
        }
    }
}

void SolidObject::setFixed()
{
    for (QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->setFixed(this->fixed);
    }
}

void SolidObject::setMass()
{
    double volume = this->getCurrentVolume();
    double density = this->mass / volume;

    for (QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->setDensity(density);
    }
}

void SolidObject::setMaterialId()
{
    for (QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->setMaterial(this->materialId);
    }
}

void SolidObject::setPosition()
{
    for (QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->displaceBy(this->position);
    }
}

void SolidObject::setVelocity()
{
    for (QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->setVelocity(this->velocity);
    }
}

const QString& SolidObject::getName() const
{
    return this->name;
}

const QString& SolidObject::getMaterialId() const
{
    return this->materialId;
}

Vector3D SolidObject::getCurrentPosition() const
{
    Vector3D position;

    foreach (const Particle& particle, this->particles)
        position += particle.getCurrentPosition() * particle.getVolume();

    return position / this->getCurrentVolume();
}

Vector3D SolidObject::getCurrentVelocity() const
{
    Vector3D velocity;

    foreach (const Particle& particle, this->particles)
        velocity += particle.getCurrentVelocity() * particle.getVolume();

    return velocity / this->getCurrentVolume();
}

double SolidObject::getCurrentMass() const
{
    double mass = 0;

    foreach (const Particle& particle, this->particles)
        mass += particle.getMass();

    return mass;
}

double SolidObject::getCurrentVolume() const
{
    double volume = 0;

    foreach (const Particle& particle, this->particles)
        volume += particle.getVolume();

    return volume;
}

void SolidObject::getBox(Vector3D &min, Vector3D &max)
{
    double minX, maxX;
    double minY, maxY;
    double minZ, maxZ;

    minX = maxX = this->particles[0].getCurrentPosition().getX();
    minY = maxY = this->particles[0].getCurrentPosition().getY();
    minZ = maxZ = this->particles[0].getCurrentPosition().getZ();

    foreach (const Particle& particle, this->particles)
    {
        const Vector3D &position = particle.getCurrentPosition();

        double newMinX = position.getX() - particle.getRadius();
        double newMaxX = position.getX() + particle.getRadius();

        if (newMinX < minX) minX = newMinX;
        if (newMaxX > maxX) maxX = newMaxX;

        double newMinY = position.getY() - particle.getRadius();
        double newMaxY = position.getY() + particle.getRadius();

        if (newMinY < minY) minY = newMinY;
        if (newMaxY > maxY) maxY = newMaxY;

        double newMinZ = position.getZ() - particle.getRadius();
        double newMaxZ = position.getZ() + particle.getRadius();

        if (newMinZ < minZ) minZ = newMinZ;
        if (newMaxZ > maxZ) maxZ = newMaxZ;
    }

    min.setX(minX);
    min.setY(minY);
    min.setZ(minZ);

    max.setX(maxX);
    max.setY(maxY);
    max.setZ(maxZ);
}

Vector3D SolidObject::getCurrentMomentum() const
{
    Vector3D momentum;

    foreach (const Particle& particle, this->particles)
        momentum += particle.getCurrentMomentum();

    return momentum;
}

Vector3D SolidObject::getCurrentAngularMomentum() const
{
    return Vector3D();
}

Vector3D SolidObject::getCurrentForce() const
{
    Vector3D force;

    foreach (const Particle& particle, this->particles)
        force += particle.getCurrentForce();

    return force;
}

double SolidObject::getCurrentKineticEnergyTotal() const
{
    double kineticEnergyTotal = 0;

    foreach (const Particle& particle, this->particles)
        kineticEnergyTotal += particle.getCurrentKineticEnergyTotal();

    return kineticEnergyTotal;
}

double SolidObject::getCurrentKineticEnergyExternal() const
{
    Vector3D momentum = this->getCurrentMomentum();
    double   mass     = this->getCurrentMass();

    return momentum.getModuleSquared() / (2.0 * mass);
}

double SolidObject::getCurrentKineticEnergyInternal() const
{
    return this->getCurrentKineticEnergyTotal() - this->getCurrentKineticEnergyExternal();
}

const QVector<Particle>& SolidObject::getParticles() const
{
    return this->particles;
}

void SolidObject::setParticlesCL(const QVector<ParticleCL>& particlesCL)
{
    for(int i = 0; i < particlesCL.count(); i++) {
        this->particles[i].setCL(particlesCL[i]);
    }
}

QJsonObject SolidObject::getJson() const
{
    QJsonObject jsonObject;

    jsonObject["name"] = this->name;

    // -- currentPosition
    Vector3D currentPosition = this->getCurrentPosition();

    QJsonArray currentPositionArray;
    currentPositionArray.append(currentPosition.getX());
    currentPositionArray.append(currentPosition.getY());
    currentPositionArray.append(currentPosition.getZ());

    jsonObject["currentPosition"] = currentPositionArray;
    //

    // -- currentVelocity
    Vector3D currentVelocity = this->getCurrentVelocity();

    QJsonArray currentVelocityArray;
    currentVelocityArray.append(currentVelocity.getX());
    currentVelocityArray.append(currentVelocity.getY());
    currentVelocityArray.append(currentVelocity.getZ());

    jsonObject["currentVelocity"] = currentVelocityArray;
    //

    // -- currentForce
    Vector3D currentForce = this->getCurrentForce();

    QJsonArray currentForceArray;
    currentForceArray.append(currentForce.getX());
    currentForceArray.append(currentForce.getY());
    currentForceArray.append(currentForce.getZ());

    jsonObject["currentForce"] = currentForceArray;
    //

    // -- particles
    QJsonArray particlesArray;

    foreach(const Particle& particle, this->particles) {
        particlesArray.append(particle.getJson());
    }

    jsonObject["particles"] = particlesArray;
    //

    return jsonObject;
}
