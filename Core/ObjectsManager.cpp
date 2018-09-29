#include "ObjectsManager.h"

#include <QJsonArray>
#include <QJsonObject>

ObjectsManager::ObjectsManager()
{

}

ObjectsManager::ObjectsManager(const QJsonValue& jsonValue)
{
    QJsonArray nonSolidObjectsArray = jsonValue["nonSolidObjects"].toArray();

    foreach(QJsonValue nonSolidObjectValue, nonSolidObjectsArray) {
        NonSolidObject nonSolidObject(nonSolidObjectValue);

        this->nonSolidObjects.push_back(nonSolidObject);
    }

    QJsonArray solidObjectsArray = jsonValue["solidObjects"].toArray();

    foreach(QJsonValue solidObjectValye, solidObjectsArray) {
        SolidObject solidObject(solidObjectValye);

        this->solidObjects.push_back(solidObject);
    }
}

const QVector<NonSolidObject>& ObjectsManager::getNonSolidObjects() const
{
    return this->nonSolidObjects;
}

const QVector<SolidObject>& ObjectsManager::getSolidObjects() const
{
    return this->solidObjects;
}

const QVector<ParticleCL> ObjectsManager::getParticlesCL(const MaterialsManager& materialsManager) const
{
    uint particleIndex = 0;
    QVector<ParticleCL> particlesCL;

    foreach(const NonSolidObject& nonSolidObject, this->nonSolidObjects) {
        uint materialIndex = materialsManager.getMaterialIndex(nonSolidObject.getMaterialId());

        const QVector<Particle>& particles = nonSolidObject.getParticles();

        foreach(const Particle& particle, particles) {
            ParticleCL particleCL = particle.getCL(particleIndex, materialIndex);
            particleIndex++;

            particlesCL.push_back(particleCL);
        }
    }

    foreach(const SolidObject& solidObject, this->solidObjects) {
        uint materialIndex = materialsManager.getMaterialIndex(solidObject.getMaterialId());

        const QVector<Particle>& particles = solidObject.getParticles();

        foreach(const Particle& particle, particles) {
            ParticleCL particleCL = particle.getCL(particleIndex, materialIndex);
            particleIndex++;

            particlesCL.push_back(particleCL);
        }
    }

    return particlesCL;
}

void ObjectsManager::setParticlesCL(const QVector<ParticleCL>& particlesCL)
{
    int i = 0;

    while(i < particlesCL.size())
    {
        for(QVector<NonSolidObject>::iterator it = this->nonSolidObjects.begin(); it != this->nonSolidObjects.end(); it++) {
            const QVector<Particle>& particles = it->getParticles();

            QVector<ParticleCL> particlesClChunk = particlesCL.mid(i, particles.size());
            i += particles.size();

            it->setParticlesCL(particlesClChunk);
        }

        for(QVector<SolidObject>::iterator it = this->solidObjects.begin(); it != this->solidObjects.end(); it++) {
            const QVector<Particle>& particles = it->getParticles();

            QVector<ParticleCL> particlesClChunk = particlesCL.mid(i, particles.size());
            i += particles.size();

            it->setParticlesCL(particlesClChunk);
        }
    }
}

QJsonObject ObjectsManager::getJson() const
{
    QJsonObject jsonObject;

    QJsonArray nonSolidObjectsArray;
    QJsonArray solidObjectsArray;

    foreach(const NonSolidObject& nonSolidObject, this->nonSolidObjects) {
        nonSolidObjectsArray.append(nonSolidObject.getJson());
    }

    foreach(const SolidObject& solidObject, this->solidObjects) {
        solidObjectsArray.append(solidObject.getJson());
    }

    jsonObject["nonSolidObjects"] = nonSolidObjectsArray;
    jsonObject["solidObjects"] = solidObjectsArray;

    return jsonObject;
}
