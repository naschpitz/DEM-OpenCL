#include "Vertex.h"

#include <QJsonArray>

Vertex::Vertex()
{
    this->currentPosition  = 0;
    this->oldPosition      = 0;
    this->originalPosition = 0;

    this->fixed = false;
}

Vertex::Vertex(const Vertex& other)
{
    this->currentPosition  = other.currentPosition;
    this->oldPosition      = other.oldPosition;
    this->originalPosition = other.originalPosition;

    this->currentVelocity  = other.currentVelocity;
    this->meanVelocity     = other.meanVelocity;
    this->oldVelocity      = other.oldVelocity;
    this->originalVelocity = other.originalVelocity;

    this->acceleration = other.acceleration;
    this->angularAcceleration = other.angularAcceleration;

    this->currentAngularVelocity = other.currentAngularVelocity;
    this->meanAngularVelocity    = other.meanAngularVelocity;
    this->oldAngularVelocity     = other.oldAngularVelocity;

    this->fixed = other.fixed;
}

Vertex& Vertex::operator=(const Vertex& other)
{
    if (this != &other)
    {
        this->currentPosition  = other.currentPosition;
        this->oldPosition      = other.oldPosition;
        this->originalPosition = other.originalPosition;

        this->currentVelocity  = other.currentVelocity;
        this->meanVelocity     = other.meanVelocity;
        this->oldVelocity      = other.oldVelocity;
        this->originalVelocity = other.originalVelocity;

        this->acceleration = other.acceleration;
        this->angularAcceleration = other.angularAcceleration;

        this->currentAngularVelocity = other.currentAngularVelocity;
        this->meanAngularVelocity    = other.meanAngularVelocity;
        this->oldAngularVelocity     = other.oldAngularVelocity;

        this->fixed = other.fixed;
    }
    return *this;
}

Vertex::Vertex(const Vector3D& vector)
{
    this->currentPosition  = vector;
    this->oldPosition      = vector;
    this->originalPosition = vector;

    this->fixed = false;
}

Vertex::Vertex(const double& x, const double& y, const double& z)
{
    Vector3D position(x, y, z);

    this->currentPosition  = position;
    this->oldPosition      = position;
    this->originalPosition = position;

    this->fixed = false;
}

VertexCL Vertex::getCL() const
{
    VertexCL vertexCL;

    vertexCL.currentPosition = {this->currentPosition.getX(), this->currentPosition.getY(), this->currentPosition.getZ(), 0};
    vertexCL.oldPosition = {this->oldPosition.getX(), this->oldPosition.getY(), this->oldPosition.getZ(), 0};
    vertexCL.originalPosition = {this->originalPosition.getX(), this->originalPosition.getY(), this->originalPosition.getZ(), 0};

    vertexCL.currentVelocity = {this->currentVelocity.getX(), this->currentVelocity.getY(), this->currentVelocity.getZ(), 0};
    vertexCL.meanVelocity = {this->meanVelocity.getX(), this->meanVelocity.getY(), this->meanVelocity.getZ(), 0};
    vertexCL.oldVelocity = {this->oldVelocity.getX(), this->oldVelocity.getY(), this->oldVelocity.getZ(), 0};
    vertexCL.originalVelocity = {this->originalVelocity.getX(), this->originalVelocity.getY(), this->originalVelocity.getZ(), 0};

    vertexCL.currentAngularVelocity = {this->currentAngularVelocity.getX(), this->currentAngularVelocity.getY(), this->currentAngularVelocity.getZ(), 0};
    vertexCL.meanAngularVelocity = {this->meanAngularVelocity.getX(), this->meanAngularVelocity.getY(), this->meanAngularVelocity.getZ(), 0};
    vertexCL.oldAngularVelocity = {this->oldAngularVelocity.getX(), this->oldAngularVelocity.getY(), this->oldAngularVelocity.getZ(), 0};

    vertexCL.fixed = this->fixed;

    return vertexCL;
}

void Vertex::setCL(const VertexCL& vertexCL)
{
    this->currentPosition  = Vector3D(vertexCL.currentPosition.x, vertexCL.currentPosition.y, vertexCL.currentPosition.z);
    this->oldPosition      = Vector3D(vertexCL.oldPosition.x, vertexCL.oldPosition.y, vertexCL.oldPosition.z);
    this->originalPosition = Vector3D(vertexCL.originalPosition.x, vertexCL.originalPosition.y, vertexCL.originalPosition.z);

    this->currentVelocity  = Vector3D(vertexCL.currentVelocity.x, vertexCL.currentVelocity.y, vertexCL.currentVelocity.z);
    this->meanVelocity     = Vector3D(vertexCL.meanVelocity.x, vertexCL.meanVelocity.y, vertexCL.meanVelocity.z);
    this->oldVelocity      = Vector3D(vertexCL.oldVelocity.x, vertexCL.oldVelocity.y, vertexCL.oldVelocity.z);
    this->originalVelocity = Vector3D(vertexCL.originalVelocity.x, vertexCL.originalVelocity.y, vertexCL.originalVelocity.z);

    this->currentAngularVelocity = Vector3D(vertexCL.currentAngularVelocity.x, vertexCL.currentAngularVelocity.y, vertexCL.currentAngularVelocity.z);
    this->meanAngularVelocity    = Vector3D(vertexCL.meanAngularVelocity.x, vertexCL.meanAngularVelocity.y, vertexCL.meanAngularVelocity.z);
    this->oldAngularVelocity     = Vector3D(vertexCL.oldAngularVelocity.x, vertexCL.oldAngularVelocity.y, vertexCL.oldAngularVelocity.z);
}

void Vertex::setFixed(bool fixed)
{
    this->fixed = fixed;
}

void Vertex::setPosition(const Vector3D& position)
{
    this->currentPosition  = position;
    this->oldPosition      = position;
    this->originalPosition = position;
}

void Vertex::setVelocity(const Vector3D& velocity)
{
    this->currentVelocity  = velocity;
    this->oldVelocity      = velocity;
    this->originalVelocity = velocity;
}

void Vertex::displaceBy(const Vector3D& displacement)
{
    this->currentPosition  += displacement;
    this->oldPosition      += displacement;
    this->originalPosition += displacement;
}

const Vector3D& Vertex::getCurrentPosition() const
{
    return this->currentPosition;
}

const Vector3D& Vertex::getCurrentVelocity() const
{
    return this->currentVelocity;
}

const Vector3D& Vertex::getMeanVelocity() const
{
    return this->meanVelocity;
}

nlohmann::json Vertex::getJson() const
{
    nlohmann::json jsonObject;

    // -- currentPosition
    Vector3D currentPosition = this->getCurrentPosition();

    nlohmann::json currentPositionArray;
    currentPositionArray.push_back(currentPosition.getX());
    currentPositionArray.push_back(currentPosition.getY());
    currentPositionArray.push_back(currentPosition.getZ());

    jsonObject["position"] = currentPositionArray;
    //

    // -- meanVelocity
    Vector3D meanVelocity = this->getCurrentVelocity();

    nlohmann::json meanVelocityArray;
    meanVelocityArray.push_back(currentVelocity.getX());
    meanVelocityArray.push_back(currentVelocity.getY());
    meanVelocityArray.push_back(currentVelocity.getZ());

    jsonObject["velocity"] = meanVelocityArray;
    //

    return jsonObject;
}
