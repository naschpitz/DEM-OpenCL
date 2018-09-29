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
    this->oldVelocity      = other.oldVelocity;
    this->originalVelocity = other.originalVelocity;

    this->fixed = other.fixed;
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

Vertex::Vertex(const QJsonValue &jsonValue)
{
    QJsonArray currentPositionArray = jsonValue["currentPosition"].toArray();
    QJsonArray oldPositionArray = jsonValue["oldPosition"].toArray();
    QJsonArray originalPositionArray = jsonValue["originalPosition"].toArray();

    QJsonArray currentVelocityArray = jsonValue["currentVelocity"].toArray();
    QJsonArray oldVelocityArray = jsonValue["oldVelocity"].toArray();
    QJsonArray originalVelocityArray = jsonValue["originalVelocity"].toArray();

    this->currentPosition = Vector3D(currentPositionArray[0].toDouble(), currentPositionArray[1].toDouble(), currentPositionArray[2].toDouble());
    this->oldPosition = Vector3D(oldPositionArray[0].toDouble(), oldPositionArray[1].toDouble(), oldPositionArray[2].toDouble());
    this->originalPosition = Vector3D(originalPositionArray[0].toDouble(), originalPositionArray[1].toDouble(), originalPositionArray[2].toDouble());

    this->currentVelocity = Vector3D(currentVelocityArray[0].toDouble(), currentVelocityArray[1].toDouble(), currentVelocityArray[2].toDouble());
    this->oldVelocity = Vector3D(oldVelocityArray[0].toDouble(), oldVelocityArray[1].toDouble(), oldVelocityArray[2].toDouble());
    this->originalVelocity = Vector3D(originalVelocityArray[0].toDouble(), originalVelocityArray[1].toDouble(), originalVelocityArray[2].toDouble());

    this->fixed = jsonValue["fixed"].toBool();
}

VertexCL Vertex::getCL() const
{
    VertexCL vertexCL;

    vertexCL.currentPosition = {this->currentPosition.getX(), this->currentPosition.getY(), this->currentPosition.getZ(), 0};
    vertexCL.oldPosition = {this->oldPosition.getX(), this->oldPosition.getY(), this->oldPosition.getZ(), 0};
    vertexCL.originalPosition = {this->originalPosition.getX(), this->originalPosition.getY(), this->originalPosition.getZ(), 0};

    vertexCL.currentVelocity = {this->currentVelocity.getX(), this->currentVelocity.getY(), this->currentVelocity.getZ(), 0};
    vertexCL.oldVelocity = {this->oldVelocity.getX(), this->oldVelocity.getY(), this->oldVelocity.getZ(), 0};
    vertexCL.originalVelocity = {this->originalVelocity.getX(), this->originalVelocity.getY(), this->originalVelocity.getZ(), 0};

    vertexCL.fixed = this->fixed;

    return vertexCL;
}

void Vertex::setCL(const VertexCL& vertexCL)
{
    this->currentPosition  = Vector3D(vertexCL.currentPosition.x, vertexCL.currentPosition.y, vertexCL.currentPosition.z);
    this->oldPosition      = Vector3D(vertexCL.oldPosition.x, vertexCL.oldPosition.y, vertexCL.oldPosition.z);
    this->originalPosition = Vector3D(vertexCL.originalPosition.x, vertexCL.originalPosition.y, vertexCL.originalPosition.z);

    this->currentVelocity  = Vector3D(vertexCL.currentVelocity.x, vertexCL.currentVelocity.y, vertexCL.currentVelocity.z);
    this->oldVelocity      = Vector3D(vertexCL.oldVelocity.x, vertexCL.oldVelocity.y, vertexCL.oldVelocity.z);
    this->originalVelocity = Vector3D(vertexCL.originalVelocity.x, vertexCL.originalVelocity.y, vertexCL.originalVelocity.z);
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
