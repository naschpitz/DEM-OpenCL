#include "Face.h"

Face::Face()
{

}

Face::Face(const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
    this->vertexes.append(v1);
    this->vertexes.append(v2);
    this->vertexes.append(v3);

    this->calculateArea();
}

const QVector<Vertex>& Face::getVertexes() const
{
    return this->vertexes;
}

FaceCL Face::getCL(uint index, uint materialIndex) const
{
    FaceCL faceCL;

    faceCL.index = index;
    faceCL.materialIndex = materialIndex;

    faceCL.area = this->area;
    faceCL.mass = this->mass;

    faceCL.currentPosition = {{this->currentPosition.getX(), this->currentPosition.getY(), this->currentPosition.getZ(), 0}};
    faceCL.currentVelocity = {{this->currentVelocity.getX(), this->currentVelocity.getY(), this->currentVelocity.getZ(), 0}};

    faceCL.currentForceX = this->currentForce.getX();
    faceCL.currentForceY = this->currentForce.getY();
    faceCL.currentForceZ = this->currentForce.getZ();

    faceCL.currentForce = {{this->currentForce.getX(), this->currentForce.getY(), this->currentForce.getZ(), 0}};
    faceCL.oldForce     = {{this->oldForce.getX(), this->oldForce.getY(), this->oldForce.getZ(), 0}};

    faceCL.currentTorqueX = this->currentTorque.getX();
    faceCL.currentTorqueY = this->currentTorque.getY();
    faceCL.currentTorqueZ = this->currentTorque.getZ();

    faceCL.currentTorque = {{this->currentTorque.getX(), this->currentTorque.getY(), this->currentTorque.getZ(), 0}};
    faceCL.oldTorque     = {{this->oldTorque.getX(), this->oldTorque.getY(), this->oldTorque.getZ(), 0}};

    for(int i = 0; i < 3; i++) {
        faceCL.vertexes[i] = this->vertexes[i].getCL();
    }

    return faceCL;
}

void Face::setCL(const FaceCL& faceCL)
{
    this->currentPosition = Vector3D(faceCL.currentPosition.x, faceCL.currentPosition.y, faceCL.currentPosition.z);
    this->currentVelocity = Vector3D(faceCL.currentVelocity.x, faceCL.currentVelocity.y, faceCL.currentVelocity.z);

    this->currentForce = Vector3D(faceCL.currentForce.x, faceCL.currentForce.y, faceCL.currentForce.z);
    this->oldForce     = Vector3D(faceCL.oldForce.x, faceCL.oldForce.y, faceCL.oldForce.z);

    this->currentTorque = Vector3D(faceCL.currentTorque.x, faceCL.currentTorque.y, faceCL.currentTorque.z);
    this->oldTorque     = Vector3D(faceCL.oldTorque.x, faceCL.oldTorque.y, faceCL.oldTorque.z);

    for(int i = 0; i < 3; i++) {
        this->vertexes[i].setCL(faceCL.vertexes[i]);
    }
}

void Face::displaceBy(const Vector3D& displacement)
{
    for(QVector<Vertex>::iterator it = this->vertexes.begin(); it != this->vertexes.end(); it++) {
        it->displaceBy(displacement);
    }
}

void Face::setFixed(bool fixed)
{
    for(QVector<Vertex>::iterator it = this->vertexes.begin(); it != this->vertexes.end(); it++) {
        it->setFixed(fixed);
    }
}

void Face::setMass(const double& mass)
{
    this->mass = mass;
}

void Face::setMaterial(const QString& material)
{
    this->material = material;
}

void Face::setVelocity(const Vector3D& velocity)
{
    this->currentVelocity = velocity;

    for(QVector<Vertex>::iterator it = this->vertexes.begin(); it != this->vertexes.end(); it++) {
        it->setVelocity(velocity);
    }
}

Vector3D Face::getNormal() const
{
    Vector3D e1 = this->vertexes[1].getCurrentPosition() - this->vertexes[0].getCurrentPosition();
    Vector3D e2 = this->vertexes[2].getCurrentPosition() - this->vertexes[0].getCurrentPosition();

    return Vector3D::crossProduct(e1, e2);
}

const double& Face::getArea() const
{
    return this->area;
}

Vector3D Face::getCurrentMomentum() const
{
    return this->mass * this->currentVelocity;
}

double Face::getCurrentKineticEnergy() const
{
    return this->mass * this->currentVelocity.getModuleSquared() / 2;
}

const Vector3D& Face::getCurrentPosition() const
{
    return this->currentPosition;
}

const Vector3D& Face::getCurrentVelocity() const
{
    return this->currentVelocity;
}

const Vector3D& Face::getCurrentForce() const
{
    return this->currentForce;
}

const Vector3D& Face::getCurrentTorque() const
{
    return this->currentTorque;
}

nlohmann::json Face::getJson() const
{
    nlohmann::json jsonObject;

    // -- currentPosition
    Vector3D currentPosition = this->getCurrentPosition();

    nlohmann::json currentPositionArray;
    currentPositionArray.push_back(currentPosition.getX());
    currentPositionArray.push_back(currentPosition.getY());
    currentPositionArray.push_back(currentPosition.getZ());

    jsonObject["currentPosition"] = currentPositionArray;
    //

    // -- currentVelocity
    Vector3D currentVelocity = this->getCurrentVelocity();

    nlohmann::json currentVelocityArray;
    currentVelocityArray.push_back(currentVelocity.getX());
    currentVelocityArray.push_back(currentVelocity.getY());
    currentVelocityArray.push_back(currentVelocity.getZ());

    jsonObject["currentVelocity"] = currentVelocityArray;
    //

    // -- currentForce
    Vector3D currentForce = this->getCurrentForce();

    nlohmann::json currentForceArray;
    currentForceArray.push_back(currentForce.getX());
    currentForceArray.push_back(currentForce.getY());
    currentForceArray.push_back(currentForce.getZ());

    jsonObject["currentForce"] = currentForceArray;
    //

    // -- vertexes
    nlohmann::json vertexesArray;

    for(int i = 0; i < 3; i++) {
        vertexesArray.push_back(this->vertexes[i].getJson());
    }

    jsonObject["vertexes"] = vertexesArray;
    //

    return jsonObject;
}

void Face::calculateArea()
{
    this->area = this->getNormal().getModule() / 2.0;
}
