#include "Scenery.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Scenery::Scenery()
{

}

Scenery::Scenery(const QJsonValue& jsonValue)
{
    QJsonArray gravityArray = jsonValue["gravity"].toArray();
    this->gravity = Vector3D(gravityArray[0].toDouble(), gravityArray[1].toDouble(), gravityArray[2].toDouble());

    QJsonValue materialsManager = jsonValue["materials"];
    this->materialsManager = MaterialsManager(materialsManager);

    QJsonValue objectsManager = jsonValue["objects"];
    this->objectsManager = ObjectsManager(objectsManager);
}

SceneryCL Scenery::getCL() const
{
    SceneryCL sceneryCL;

    sceneryCL.numParticles = this->objectsManager.getParticlesCL(this->materialsManager).size();
    sceneryCL.numFaces     = this->objectsManager.getFacesCL(this->materialsManager).size();

    sceneryCL.gravity = {this->gravity.getX(), this->gravity.getY(), this->gravity.getZ(), 0};

    return sceneryCL;
}

const ObjectsManager& Scenery::getObjectsManager() const
{
    return this->objectsManager;
}

const MaterialsManager& Scenery::getMaterialsManager() const
{
    return this->materialsManager;
}

const QJsonObject Scenery::getJson() const
{
    QJsonObject jsonObject;

    jsonObject["objects"] = this->objectsManager.getJson();

    return jsonObject;
}

void Scenery::setParticlesCL(const QVector<ParticleCL>& particlesCL)
{
    this->objectsManager.setParticlesCL(particlesCL);
}

void Scenery::setFacesCL(const QVector<FaceCL>& facesCL)
{
    this->objectsManager.setFacesCL(facesCL);
}
