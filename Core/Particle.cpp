#include <Particle.h>

#include <QJsonArray>
#include <QStringList>

Particle::Particle()
{

}

Particle::Particle(const double& x, const double& y, const double& z, const double& radius) : Vertex(x, y, z)
{
    this->radius = radius;

    this->calculateArea();
    this->calculateVolume();

    this->mass = 0;
    this->inertiaMomentum = 0;
}

Particle::Particle(const Vector3D& vector, const double& radius) : Vertex(vector)
{
    this->radius = radius;

    this->calculateArea();
    this->calculateVolume();

    this->mass = 0;
    this->inertiaMomentum = 0;
}

ParticleCL Particle::getCL(uint index, uint materialIndex) const
{
    ParticleCL particleCL;

    particleCL.index = index;
    particleCL.materialIndex = materialIndex;

    particleCL.radius = this->radius;
    particleCL.density = this->density;
    particleCL.mass = this->mass;
    particleCL.area = this->area;
    particleCL.volume = this->volume;
    particleCL.inertiaMomentum = this->inertiaMomentum;

    particleCL.currentForce = {this->currentForce.getX(), this->currentForce.getY(), this->currentForce.getZ(), 0};
    particleCL.meanForce    = {this->meanForce.getX(), this->meanForce.getY(), this->meanForce.getZ(), 0};
    particleCL.oldForce     = {this->oldForce.getX(), this->oldForce.getY(), this->oldForce.getZ(), 0};

    particleCL.currentTorque = {this->currentTorque.getX(), this->currentTorque.getY(), this->currentTorque.getZ(), 0};
    particleCL.meanTorque    = {this->meanTorque.getX(), this->meanTorque.getY(), this->meanTorque.getZ(), 0};
    particleCL.oldTorque     = {this->oldTorque.getX(), this->oldTorque.getY(), this->oldTorque.getZ(), 0};

    Vertex vertex = (*this);
    particleCL.vertex = vertex.getCL();

    return particleCL;
}

void Particle::setCL(const ParticleCL& particleCL)
{
    this->currentForce = Vector3D(particleCL.currentForce.x, particleCL.currentForce.y, particleCL.currentForce.z);
    this->meanForce    = Vector3D(particleCL.meanForce.x, particleCL.meanForce.y, particleCL.meanForce.z);
    this->oldForce     = Vector3D(particleCL.oldForce.x, particleCL.oldForce.y, particleCL.oldForce.z);

    this->currentTorque = Vector3D(particleCL.currentTorque.x, particleCL.currentTorque.y, particleCL.currentTorque.z);
    this->meanTorque    = Vector3D(particleCL.meanTorque.x, particleCL.meanTorque.y, particleCL.meanTorque.z);
    this->oldTorque     = Vector3D(particleCL.oldTorque.x, particleCL.oldTorque.y, particleCL.oldTorque.z);

    ((Vertex *)(this))->setCL(particleCL.vertex);
}

void Particle::setMaterial(const QString& materialName)
{
    this->materialName = materialName;
}

void Particle::setDensity(const double& density)
{
    this->density = density;
    this->mass = density * this->volume;
    this->inertiaMomentum = 2.0/5.0 * this->mass * this->radius * this->radius;
}

const double& Particle::getRadius() const
{
    return this->radius;
}

const double& Particle::getMass() const
{
    return this->mass;
}

const double& Particle::getArea() const
{
    return this->area;
}

const double& Particle::getVolume() const
{
    return this->volume;
}

const Vector3D& Particle::getCurrentForce() const
{
    return this->currentForce;
}

const Vector3D& Particle::getMeanForce() const
{
    return this->meanForce;
}

const Vector3D& Particle::getCurrentTorque() const
{
    return this->currentTorque;
}

const Vector3D& Particle::getMeanTorque() const
{
    return this->meanTorque;
}

Vector3D Particle::getCurrentMomentum() const
{
    return this->mass * this->currentVelocity;
}

Vector3D Particle::getMeanMomentum() const
{
    return this->mass * this->meanVelocity;
}

Vector3D Particle::getCurrentAngularMomentum() const
{
    return this->inertiaMomentum * this->currentAngularVelocity;
}

Vector3D Particle::getMeanAngularMomentum() const
{
    return this->inertiaMomentum * this->meanAngularVelocity;
}

double Particle::getCurrentKineticEnergyTotal() const
{
    return this->getCurrentKineticEnergyExternal() + this->getCurrentKineticEnergyInternal();
}

double Particle::getMeanKineticEnergyTotal() const
{
    return this->getMeanKineticEnergyExternal() + this->getMeanKineticEnergyInternal();
}

double Particle::getCurrentKineticEnergyExternal() const
{
    return this->mass * this->currentVelocity.getModuleSquared() / 2.0;
}

double Particle::getMeanKineticEnergyExternal() const
{
    return this->mass * this->meanVelocity.getModuleSquared() / 2.0;
}

double Particle::getCurrentKineticEnergyInternal() const
{
    return this->inertiaMomentum * this->currentAngularVelocity.getModuleSquared() / 2.0;
}

double Particle::getMeanKineticEnergyInternal() const
{
    return this->inertiaMomentum * this->meanAngularVelocity.getModuleSquared() / 2.0;
}


nlohmann::json Particle::getJson() const
{
    nlohmann::json jsonObject;

    jsonObject["radius"] = this->getRadius();

    // -- force
    Vector3D meanForce = this->getMeanForce();

    nlohmann::json meanForceArray;
    meanForceArray.push_back(currentForce.getX());
    meanForceArray.push_back(currentForce.getY());
    meanForceArray.push_back(currentForce.getZ());

    jsonObject["force"] = meanForceArray;
    //

    // -- vertex
    nlohmann::json vertexJsonObjects = ((Vertex*)this)->getJson();

    for(nlohmann::json::iterator it = vertexJsonObjects.begin(); it != vertexJsonObjects.end(); it++) {
        jsonObject[it.key()] = vertexJsonObjects[it.key()];
    }
    //

    return jsonObject;
}

bool Particle::isInside(const Particle& otherParticle) const
{
    double distanceModule = (otherParticle.currentPosition - this->currentPosition).getModule();

    return distanceModule < (this->radius + otherParticle.radius);
}

void Particle::calculateArea()
{
    const double& radius = this->radius;

    // Surface area
    this->area = 4 * 3.14159265359 * radius * radius;
}

void Particle::calculateVolume()
{
    const double& radius = this->radius;

    this->volume = (4.0 / 3.0) *  3.14159265359 * radius * radius * radius;
}

bool Particle::operator==(const Particle &other) const
{
    return this == &other;
}
