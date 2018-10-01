#include "Scenery.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Scenery::Scenery()
{

}

Scenery::Scenery(const QJsonValue& jsonValue)
{
    QJsonValue materialsManager = jsonValue["materialsManager"];
    this->materialsManager = MaterialsManager(materialsManager);

    QJsonValue objectsManager = jsonValue["objectsManager"];
    this->objectsManager = ObjectsManager(objectsManager);
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

    jsonObject["objectsManager"] = this->objectsManager.getJson();

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
