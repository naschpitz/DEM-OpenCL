#include <Particle.h>

#include <QJsonArray>

Particle::Particle()
{

}

Particle::Particle(const double& x, const double& y, const double& z, const double& radius) : Vertex(x, y, z)
{
    this->radius = radius;

    this->calculateArea();
    this->calculateVolume();

    this->mass = 0;
}

Particle::Particle(const Vector3D& vector, const double& radius) : Vertex(vector)
{
    this->radius = radius;

    this->calculateArea();
    this->calculateVolume();

    this->mass = 0;
}

Particle::Particle(const QJsonValue& jsonValue) : Vertex(jsonValue["vertex"])
{
    this->_id        = jsonValue["_id"].toString();
    this->materialId = jsonValue["materialId"].toString();

    this->radius  = jsonValue["radius"].toDouble();
    this->density = jsonValue["density"].toDouble();
    this->mass    = jsonValue["mass"].toDouble();
    this->volume  = jsonValue["volume"].toDouble();

    QJsonArray currentForceArray = jsonValue["currentForce"].toArray();
    QJsonArray oldForceArray = jsonValue["oldForce"].toArray();

    this->currentForce = Vector3D(currentForceArray[0].toDouble(), currentForceArray[1].toDouble(), currentForceArray[2].toDouble());
    this->oldForce = Vector3D(oldForceArray[0].toDouble(), oldForceArray[1].toDouble(), oldForceArray[2].toDouble());

    QJsonArray currentTorqueArray = jsonValue["currentTorque"].toArray();
    QJsonArray oldTorqueArray = jsonValue["oldTorque"].toArray();

    this->currentTorque = Vector3D(currentTorqueArray[0].toDouble(), currentTorqueArray[1].toDouble(), currentTorqueArray[2].toDouble());
    this->oldTorque = Vector3D(oldTorqueArray[0].toDouble(), oldTorqueArray[1].toDouble(), oldTorqueArray[2].toDouble());
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

    particleCL.currentForce = {this->currentForce.getX(), this->currentForce.getY(), this->currentForce.getZ(), 0};
    particleCL.oldForce     = {this->oldForce.getX(), this->oldForce.getY(), this->oldForce.getZ(), 0};

    particleCL.currentTorque = {this->currentTorque.getX(), this->currentTorque.getY(), this->currentTorque.getZ(), 0};
    particleCL.oldTorque     = {this->oldTorque.getX(), this->oldTorque.getY(), this->oldTorque.getZ(), 0};

    Vertex vertex = (*this);
    particleCL.vertex = vertex.getCL();

    return particleCL;
}

void Particle::setCL(const ParticleCL& particleCL)
{
    this->currentForce = Vector3D(particleCL.currentForce.x, particleCL.currentForce.y, particleCL.currentForce.z);
    this->oldForce     = Vector3D(particleCL.oldForce.x, particleCL.oldForce.y, particleCL.oldForce.z);

    this->currentTorque = Vector3D(particleCL.currentTorque.x, particleCL.currentTorque.y, particleCL.currentTorque.z);
    this->oldTorque     = Vector3D(particleCL.oldTorque.x, particleCL.oldTorque.y, particleCL.oldTorque.z);

    ((Vertex *)(this))->setCL(particleCL.vertex);
}

void Particle::setMaterial(const QString& materialId)
{
    this->materialId = materialId;
}

void Particle::setDensity(const double& density)
{
    this->density = density;
    this->mass = density * this->volume;
}

const QString& Particle::getId() const
{
    return this->_id;
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

const Vector3D& Particle::getCurrentTorque() const
{
    return this->currentTorque;
}

Vector3D Particle::getCurrentMomentum() const
{
    return this->mass * this->currentVelocity;
}

// Still incomplete.
Vector3D Particle::getCurrentAngularMomentum() const
{
    return Vector3D();
}

double Particle::getCurrentKineticEnergyTotal() const
{
    return this->getCurrentKineticEnergyExternal() + this->getCurrentKineticEnergyInternal();
}

double Particle::getCurrentKineticEnergyExternal() const
{
    return this->mass * this->currentVelocity.getModuleSquared() / 2.0;
}

// Still incomplete.
double Particle::getCurrentKineticEnergyInternal() const
{
    return 0.0;
}

QJsonObject Particle::getJson() const
{
    QJsonObject jsonObject;

    // -- currentForce
    Vector3D currentForce = this->getCurrentForce();

    QJsonArray currentForceArray;
    currentForceArray.append(currentForce.getX());
    currentForceArray.append(currentForce.getY());
    currentForceArray.append(currentForce.getZ());

    jsonObject["currentForce"] = currentForceArray;
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
