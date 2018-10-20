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

SolidObject::SolidObject(const QJsonObject& jsonObject)
{
    this->id = jsonObject["_id"].toString();
    this->material = jsonObject["material"].toString();

    this->stl = jsonObject["stl"].toString();
    this->maximumTetrahedronVol = jsonObject["maximumTetrahedronVolume"].toDouble(-1);

    this->loadStl();

    this->fixed = jsonObject["fixed"].toBool();
    this->mass  = jsonObject["mass"].toDouble();

    QJsonArray positionArray = jsonObject["position"].toArray();
    QJsonArray velocityArray = jsonObject["velocity"].toArray();

    this->position = Vector3D(positionArray[0].toDouble(), positionArray[1].toDouble(), positionArray[2].toDouble());
    this->velocity = Vector3D(velocityArray[0].toDouble(), velocityArray[1].toDouble(), velocityArray[2].toDouble());

    this->setMaterial();
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

    in.load_stl(QString("temp.stl").toUtf8().data());

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

            Vertex v1 = vertexes.at(v1Index - 1);
            Vertex v2 = vertexes.at(v2Index - 1);
            Vertex v3 = vertexes.at(v3Index - 1);

            Face face(v1, v2, v3);
            face.setVelocity(this->velocity);

            this->faces.append(face);
        }
    }
}

void SolidObject::setFixed()
{
    for (QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        it->setFixed(this->fixed);
    }
}

void SolidObject::setMass()
{
    double totalArea = this->getArea();

    for (QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        double mass = (this->mass * it->getArea()) / totalArea;

        it->setMass(mass);
    }
}

void SolidObject::setMaterial()
{
    for (QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        it->setMaterial(this->material);
    }
}

void SolidObject::setPosition()
{
    for (QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        it->displaceBy(this->position);
    }
}

void SolidObject::setVelocity()
{
    for (QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        it->setVelocity(this->velocity);
    }
}

const QString& SolidObject::getId() const
{
    return this->id;
}

const QString& SolidObject::getMaterial() const
{
    return this->material;
}

Vector3D SolidObject::getCurrentPosition() const
{
    Vector3D position;

    foreach (const Face& face, this->faces)
        position += face.getCurrentPosition() * face.getArea();

    return position / this->getArea();
}

Vector3D SolidObject::getCurrentVelocity() const
{
    Vector3D velocity;

    foreach (const Face& face, this->faces)
        velocity += face.getCurrentVelocity() * face.getArea();

    return velocity / this->getArea();
}

const double& SolidObject::getMass() const
{
    return this->mass;
}

void SolidObject::getBox(Vector3D &min, Vector3D &max)
{
    double minX, maxX;
    double minY, maxY;
    double minZ, maxZ;

    minX = maxX = this->faces[0].getVertexes()[0].getCurrentPosition().getX();
    minY = maxY = this->faces[0].getVertexes()[0].getCurrentPosition().getY();
    minZ = maxZ = this->faces[0].getVertexes()[0].getCurrentPosition().getZ();

    foreach(const Face& face, this->faces){
        const QVector<Vertex>& vertexes = face.getVertexes();

        foreach(const Vertex& vertex, vertexes){
            const Vector3D &position = vertex.getCurrentPosition();

            if(position.getX() < minX) minX = position.getX();
            if(position.getX() > maxX) maxX = position.getX();

            if(position.getY() < minY) minY = position.getY();
            if(position.getY() > maxY) maxY = position.getY();

            if(position.getZ() < minZ) minZ = position.getZ();
            if(position.getZ() > maxZ) maxZ = position.getZ();
        }
    }

    min.setX(minX);
    min.setY(minY);
    min.setZ(minZ);

    max.setX(maxX);
    max.setY(maxY);
    max.setZ(maxZ);
}

double SolidObject::getArea() const
{
    double area = 0;

    foreach (const Face& face, this->faces)
        area += face.getArea();

    return area;
}

Vector3D SolidObject::getCurrentMomentum() const
{
    Vector3D momentum;

    foreach (const Face& face, this->faces)
        momentum += face.getCurrentMomentum();

    return momentum;
}

Vector3D SolidObject::getCurrentForce() const
{
    Vector3D force;

    foreach (const Face& face, this->faces)
        force += face.getCurrentForce();

    return force;
}

double SolidObject::getCurrentKineticEnergyTotal() const
{
    double kineticEnergy = 0;

    foreach (const Face& face, this->faces)
        kineticEnergy += face.getCurrentKineticEnergy();

    return kineticEnergy;
}

const QVector<Face>& SolidObject::getFaces() const
{
    return this->faces;
}

void SolidObject::setFacesCL(const QVector<FaceCL>& facesCL)
{
    for(int i = 0; i < facesCL.count(); i++) {
        this->faces[i].setCL(facesCL[i]);
    }
}

QJsonObject SolidObject::getJson() const
{
    QJsonObject jsonObject;

    jsonObject["_id"] = this->id;

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

    // -- faces
    QJsonArray facesArray;

    foreach(const Face& face, this->faces) {
        facesArray.append(face.getJson());
    }

    jsonObject["faces"] = facesArray;
    //

    return jsonObject;
}
