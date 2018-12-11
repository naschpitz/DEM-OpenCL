#include "Scenery.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Scenery::Scenery()
{

}

Scenery::Scenery(const QJsonObject& jsonObject)
{
    this->id = jsonObject["_id"].toString();

    QJsonArray gravityArray = jsonObject["gravity"].toArray();
    this->gravity = Vector3D(gravityArray[0].toDouble(), gravityArray[1].toDouble(), gravityArray[2].toDouble());

    QJsonArray materialsManager = jsonObject["materials"].toArray();
    this->materialsManager = MaterialsManager(materialsManager);

    QJsonObject objectsManager = jsonObject["objects"].toObject();
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

    jsonObject["owner"] = this->id;
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
