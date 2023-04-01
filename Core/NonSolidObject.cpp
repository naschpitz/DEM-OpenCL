#include <NonSolidObject.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

#include <iostream>

NonSolidObject::NonSolidObject()
{

}

NonSolidObject::NonSolidObject(const nlohmann::json& jsonObject)
{    
    try {
        this->id = QString::fromStdString(jsonObject.at("_id").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing '_id' field in Non-Solid Object");
    }

    try {
        this->material = QString::fromStdString(jsonObject.at("material").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'material' field in Non-Solid Object");
    }

    try {
        this->fixed = jsonObject.at("fixed").get<bool>();
    }

    catch (const nlohmann::detail::exception& e) {
        this->fixed = false;
    }

    try {
        this->density = jsonObject.at("density").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'density' field in Non-Solid Object");
    }

    try {
        const nlohmann::json& dimensions = jsonObject.at("dimensions");

        try {
            this->spacing = dimensions.at("spacing").get<double>();
        }

        catch (const nlohmann::detail::exception& e) {
            throw std::runtime_error("Missing 'spacing' field in Non-Solid Object");
        }

        try {
            const nlohmann::json& lengthArray = dimensions.at("length");
            this->length = Vector3D(lengthArray.at(0).get<double>(), lengthArray.at(1).get<double>(), lengthArray.at(2).get<double>());
        }

        catch (const nlohmann::detail::exception& e) {
            throw std::runtime_error("Missing 'length' field in Non-Solid Object");
        }
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'dimensions' field in Non-Solid Object");
    }

    try {
        const nlohmann::json& positionArray = jsonObject.at("position");
        this->position = Vector3D(positionArray.at(0).get<double>(), positionArray.at(1).get<double>(), positionArray.at(2).get<double>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'position' field in Non-Solid Object");
    }

    try {
        const nlohmann::json& velocityArray = jsonObject.at("velocity");
        this->velocity = Vector3D(velocityArray.at(0).get<double>(), velocityArray.at(1).get<double>(), velocityArray.at(2).get<double>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'velocity' field in Non-Solid Object");
    }
}

void NonSolidObject::initialize()
{
    this->buildObject();
    this->setMaterial();
    this->setDensity();
    this->setFixed();
    this->setPosition();
    this->setVelocity();
}

void NonSolidObject::setMaterial()
{
    for(QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->setMaterial(this->material);
    }
}

void NonSolidObject::setFixed()
{
    for(QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->setFixed(this->fixed);
    }
}

void NonSolidObject::setDensity()
{
    for(QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->setDensity(this->density);
    }
}

void NonSolidObject::setPosition()
{
    Vector3D min, max;
    this->getBox(min, max);

    double xLength = max.getX() - min.getX();
    double yLength = max.getY() - min.getY();
    double zLength = max.getZ() - min.getZ();

    double xMaxCentered = xLength / 2.;
    double xCorrection = -(max.getX() - xMaxCentered);

    double yMaxCentered = yLength / 2.;
    double yCorrection = -(max.getY() - yMaxCentered);

    double zMaxCentered = zLength / 2.;
    double zCorrection = -(max.getZ() - zMaxCentered);

    Vector3D correction = Vector3D (xCorrection, yCorrection, zCorrection);

    for(QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->displaceBy(this->position + correction);
    }
}

void NonSolidObject::setVelocity()
{
    for(QVector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
        it->setVelocity(this->velocity);
    }
}

const QString& NonSolidObject::getId() const
{
    return this->id;
}

const QString& NonSolidObject::getMaterial() const
{
    return this->material;
}

Vector3D NonSolidObject::getCurrentPosition() const
{
    Vector3D position;

    foreach(const Particle& particle, this->particles)
        position += particle.getCurrentPosition() * particle.getVolume();

    return position / this->getCurrentVolume();
}

Vector3D NonSolidObject::getCurrentVelocity() const
{
    Vector3D velocity;

    foreach(const Particle& particle, this->particles)
        velocity += particle.getCurrentVelocity() * particle.getVolume();

    return velocity / this->getCurrentVolume();
}

double NonSolidObject::getCurrentMass() const
{
    double mass = 0;

    foreach(const Particle& particle, this->particles)
        mass += particle.getMass();

    return mass;
}

double NonSolidObject::getCurrentVolume() const
{
    double volume = 0;

    foreach(const Particle& particle, this->particles)
        volume += particle.getVolume();

    return volume;
}

void NonSolidObject::getBox(Vector3D &min, Vector3D &max)
{
    double minX, maxX;
    double minY, maxY;
    double minZ, maxZ;

    minX = maxX = this->particles[0].getCurrentPosition().getX();
    minY = maxY = this->particles[0].getCurrentPosition().getY();
    minZ = maxZ = this->particles[0].getCurrentPosition().getZ();

    foreach(const Particle& particle, this->particles)
    {
        const Vector3D &position = particle.getCurrentPosition();

        double newMinX = position.getX() - particle.getRadius();
        double newMaxX = position.getX() + particle.getRadius();

        if(newMinX < minX) minX = newMinX;
        if(newMaxX > maxX) maxX = newMaxX;

        double newMinY = position.getY() - particle.getRadius();
        double newMaxY = position.getY() + particle.getRadius();

        if(newMinY < minY) minY = newMinY;
        if(newMaxY > maxY) maxY = newMaxY;

        double newMinZ = position.getZ() - particle.getRadius();
        double newMaxZ = position.getZ() + particle.getRadius();

        if(newMinZ < minZ) minZ = newMinZ;
        if(newMaxZ > maxZ) maxZ = newMaxZ;
    }

    min.setX(minX);
    min.setY(minY);
    min.setZ(minZ);

    max.setX(maxX);
    max.setY(maxY);
    max.setZ(maxZ);
}

Vector3D NonSolidObject::getCurrentMomentum() const
{
    Vector3D momentum;

    foreach(const Particle& particle, this->particles)
        momentum += particle.getCurrentMomentum();

    return momentum;
}

Vector3D NonSolidObject::getCurrentAngularMomentum() const
{
    return Vector3D();
}

Vector3D NonSolidObject::getCurrentForce() const
{
    Vector3D force;

    foreach(const Particle& particle, this->particles)
        force += particle.getCurrentForce();

    return force;
}

double NonSolidObject::getCurrentKineticEnergyTotal() const
{
    double kineticEnergyTotal = 0;

    foreach(const Particle& particle, this->particles)
        kineticEnergyTotal += particle.getCurrentKineticEnergyTotal();

    return kineticEnergyTotal;
}

double NonSolidObject::getCurrentKineticEnergyExternal() const
{
    Vector3D momentum = this->getCurrentMomentum();
    double   mass     = this->getCurrentMass();

    return momentum.getModuleSquared() / (2.0 * mass);
}

double NonSolidObject::getCurrentKineticEnergyInternal() const
{
    return this->getCurrentKineticEnergyTotal() - this->getCurrentKineticEnergyExternal();
}

const QVector<Particle>& NonSolidObject::getParticles() const
{
    return this->particles;
}

void NonSolidObject::setParticlesCL(const QVector<ParticleCL>& particlesCL)
{
    for(int i = 0; i < particlesCL.count(); i++) {
        this->particles[i].setCL(particlesCL[i]);
    }
}

nlohmann::json NonSolidObject::getJson(bool detailed = true) const
{
    nlohmann::json jsonObject;

    jsonObject["_id"] = this->id.toStdString();

    // -- currentPosition
    Vector3D currentPosition = this->getCurrentPosition();

    nlohmann::json currentPositionArray;
    currentPositionArray.push_back(currentPosition.getX());
    currentPositionArray.push_back(currentPosition.getY());
    currentPositionArray.push_back(currentPosition.getZ());

    jsonObject["position"] = currentPositionArray;
    //

    // -- currentVelocity
    Vector3D currentVelocity = this->getCurrentVelocity();

    nlohmann::json currentVelocityArray;
    currentVelocityArray.push_back(currentVelocity.getX());
    currentVelocityArray.push_back(currentVelocity.getY());
    currentVelocityArray.push_back(currentVelocity.getZ());

    jsonObject["velocity"] = currentVelocityArray;
    //

    // -- currentForce
    Vector3D currentForce = this->getCurrentForce();

    nlohmann::json currentForceArray;
    currentForceArray.push_back(currentForce.getX());
    currentForceArray.push_back(currentForce.getY());
    currentForceArray.push_back(currentForce.getZ());

    jsonObject["force"] = currentForceArray;
    //

    // -- kineticEnergyTotal
    double currentKineticEnergyTotal = this->getCurrentKineticEnergyTotal();

    jsonObject["kineticEnergyTotal"] = currentKineticEnergyTotal;
    //

    // -- kineticEnergyInternal
    double currentKineticEnergyInternal = this->getCurrentKineticEnergyInternal();

    jsonObject["kineticEnergyInternal"] = currentKineticEnergyInternal;
    //

    // -- kineticEnergyExternal
    double currentKineticEnergyExternal = this->getCurrentKineticEnergyExternal();

    jsonObject["kineticEnergyExternal"] = currentKineticEnergyExternal;
    //

    // -- particles
    if(detailed) {
        nlohmann::json particlesArray;

        foreach(const Particle& particle, this->particles) {
            particlesArray.push_back(particle.getJson());
        }

        jsonObject["particles"] = particlesArray;
    }
    //

    return jsonObject;
}

// https://www.wikiwand.com/en/Close-packing_of_equal_spheres
void NonSolidObject::buildObject()
{
    this->particles.clear();

    double radius = this->spacing / 2.0;

    int k = 0;
    double z = 0;

    while(((z = ((2 * sqrt(6) * k) / 3.) * radius + radius) + radius) < this->length.getZ()) {
        int j = 0;
        double y = 0;

        while(((y = ((sqrt(3) * (j + (k % 2) / 3.)) * radius + radius)) + radius) < this->length.getY()) {
            int i = 0;
            double x = 0;

            while(((x = ((2 * i + ((j + k) % 2)) * radius + radius)) + radius) < this->length.getX()) {
                Particle particle = Particle(x, y, z, radius);
                this->particles.append(particle);

                i++;
            }

            j++;
        }

        k++;
    }
}

// Random particle position packing.
//void NonSolidObject::buildObject()
//{
//    this->particles.clear();

//    double radius = this->spacing / 2.0;
//    double maxVolume = this->length.getX() * this->length.getY() * this->length.getZ();

//    double density = 0;
//    double particlesVolume = 0;
//    QRandomGenerator randomGenerator;

//    while(density < 0.5) {
//        double x = randomGenerator.generateDouble() * this->length.getX();
//        double y = randomGenerator.generateDouble() * this->length.getY();
//        double z = randomGenerator.generateDouble() * this->length.getZ();

//        if ((x - radius < 0) || (x + radius > this->length.getX())) continue;
//        if ((y - radius < 0) || (y + radius > this->length.getY())) continue;
//        if ((z - radius < 0) || (z + radius > this->length.getZ())) continue;

//        bool isHit = false;

//        foreach(const Particle& particle, this->particles) {
//            double intersectionDistance = particle.getRadius() + radius;

//            double xDiff = particle.getCurrentPosition().getX() - x;
//            if(xDiff > intersectionDistance) continue;

//            double yDiff = particle.getCurrentPosition().getY() - y;
//            if(yDiff > intersectionDistance) continue;

//            double zDiff = particle.getCurrentPosition().getZ() - z;
//            if(zDiff > intersectionDistance) continue;

//            double dist = sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

//            if(dist < intersectionDistance) {
//                isHit = true;
//                break;
//            }
//        }

//        if(isHit) continue;

//        Particle particle = Particle(x, y, z, radius);

//        this->particles.append(particle);

//        particlesVolume += particle.getVolume();
//        density = particlesVolume / maxVolume;
//    }
//}
