#include "SolidObject.h"
#include "Vector3D.h"
#include "Vertex.h"

#include <QFile>
#include <tetgen1.5.1/tetgen.h>

SolidObject::SolidObject()
{

}

SolidObject::SolidObject(const nlohmann::json& jsonObject)
{
    try {
        this->id = QString::fromStdString(jsonObject.at("_id").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing '_id' field in Solid Object");
    }

    try {
        this->material = QString::fromStdString(jsonObject.at("material").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'material' field in Solid Object");
    }

    try {
        this->stl = QString::fromStdString(jsonObject.at("stl").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'stl' field in Solid Object");
    }

    try {
        this->maximumTetrahedronVol = jsonObject.at("maximumTetrahedronVolume").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        this->maximumTetrahedronVol = -1;
    }

    this->loadStl();

    try {
        this->fixed = jsonObject.at("fixed").get<bool>();
    }

    catch (const nlohmann::detail::exception& e) {
        this->fixed = false;
    }

    try {
        this->mass = jsonObject.at("mass").get<double>();
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'mass' field in Solid Object");
    }

    try {
        const nlohmann::json& positionArray = jsonObject.at("position");
        this->position = Vector3D(positionArray.at(0).get<double>(), positionArray.at(1).get<double>(), positionArray.at(2).get<double>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'position' field in Solid Object");
    }

    try {
        const nlohmann::json& velocityArray = jsonObject.at("velocity");
        this->velocity = Vector3D(velocityArray.at(0).get<double>(), velocityArray.at(1).get<double>(), velocityArray.at(2).get<double>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'velocity' field in Solid Object");
    }

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

    for(int i = 0; i < in.numberofpoints; i++) {
        double x = in.pointlist[i * 3];
        double y = in.pointlist[i * 3 + 1];
        double z = in.pointlist[i * 3 + 2];

        vertexes.append(Vertex(x, y, z));
    }

    for(int i = 0; i < in.numberoffacets; i++) {
        tetgenio::facet facet = in.facetlist[i];

        for(int j = 0; j < facet.numberofpolygons; j++) {
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
    for(QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        it->setFixed(this->fixed);
    }
}

void SolidObject::setMass()
{
    double totalArea = this->getArea();

    for(QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        double mass = (this->mass * it->getArea()) / totalArea;

        it->setMass(mass);
    }
}

void SolidObject::setMaterial()
{
    for(QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        it->setMaterial(this->material);
    }
}

void SolidObject::setPosition()
{
    for(QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
        it->displaceBy(this->position);
    }
}

void SolidObject::setVelocity()
{
    for(QVector<Face>::iterator it = this->faces.begin(); it != this->faces.end(); it++) {
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

    foreach(const Face& face, this->faces) {
        const QVector<Vertex>& vertexes = face.getVertexes();

        foreach(const Vertex& vertex, vertexes) {
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

nlohmann::json SolidObject::getJson(bool detailed = true) const
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

    // -- faces
    if(detailed) {
        nlohmann::json facesArray;

        foreach(const Face& face, this->faces) {
            facesArray.push_back(face.getJson());
        }

        jsonObject["faces"] = facesArray;
    }
    //

    return jsonObject;
}
