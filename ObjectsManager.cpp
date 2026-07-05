#include "ObjectsManager.h"

ObjectsManager::ObjectsManager()
{

}

ObjectsManager::ObjectsManager(const nlohmann::json& jsonValue)
{
    try {
        const nlohmann::json& nonSolidObjectsArray = jsonValue.at("nonSolidObjects");

        for(const auto& nonSolidObjectValue : nonSolidObjectsArray) {
            NonSolidObject nonSolidObject(nonSolidObjectValue);

            this->nonSolidObjects.push_back(nonSolidObject);
        }
    }

    catch (...) { }

    try {
        const nlohmann::json& solidObjectsArray = jsonValue.at("solidObjects");

        for(const auto& solidObjectValue : solidObjectsArray) {
            SolidObject solidObject(solidObjectValue);

            this->solidObjects.push_back(solidObject);
        }
    }

    catch (...) { }
}

void ObjectsManager::initialize()
{
    for(QVector<NonSolidObject>::iterator it = this->nonSolidObjects.begin(); it != this->nonSolidObjects.end(); it++) {
        it->initialize();
    }

    for(QVector<SolidObject>::iterator it = this->solidObjects.begin(); it != this->solidObjects.end(); it++) {
        it->initialize();
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

QVector<ParticleCL> ObjectsManager::getParticlesCL(const MaterialsManager& materialsManager) const
{
    uint particleIndex = 0;

    QVector<ParticleCL> particlesCL;

    for(const NonSolidObject& nonSolidObject : this->nonSolidObjects) {
        uint materialIndex = materialsManager.getMaterialIndex(nonSolidObject.getMaterial());

        const QVector<Particle>& particles = nonSolidObject.getParticles();

        for(const Particle& particle : particles) {
            ParticleCL particleCL = particle.getCL(particleIndex, materialIndex);
            particleIndex++;

            particlesCL.push_back(particleCL);
        }
    }

    return particlesCL;
}

QVector<FaceCL> ObjectsManager::getFacesCL(const MaterialsManager &materialsManager) const
{
    uint faceIndex = 0;

    QVector<FaceCL> facesCL;

    for(const SolidObject& solidObject : this->solidObjects) {
        uint materialIndex = materialsManager.getMaterialIndex(solidObject.getMaterial());

        const QVector<Face>& faces = solidObject.getFaces();

        for(const Face& face : faces) {
            FaceCL faceCL = face.getCL(faceIndex, materialIndex);
            faceIndex++;

            facesCL.push_back(faceCL);
        }
    }

    return facesCL;
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
    }
}

void ObjectsManager::setFacesCL(const QVector<FaceCL>& facesCL)
{
    int i = 0;

    while(i < facesCL.size())
    {
        for(QVector<SolidObject>::iterator it = this->solidObjects.begin(); it != this->solidObjects.end(); it++) {
            const QVector<Face>& faces = it->getFaces();

            QVector<FaceCL> facesClChunk = facesCL.mid(i, faces.size());
            i += faces.size();

            it->setFacesCL(facesClChunk);
        }
    }
}

nlohmann::json ObjectsManager::getJson(bool detailed = true) const
{
    nlohmann::json jsonObject;

    nlohmann::json nonSolidObjectsArray;
    nlohmann::json solidObjectsArray;

    if(!this->nonSolidObjects.isEmpty()) {
        for(const NonSolidObject& nonSolidObject : this->nonSolidObjects) {
            nonSolidObjectsArray.push_back(nonSolidObject.getJson(detailed));
        }

        jsonObject["nonSolidObjects"] = nonSolidObjectsArray;
    }

    if(!this->solidObjects.isEmpty()) {
        for(const SolidObject& solidObject : this->solidObjects) {
            solidObjectsArray.push_back(solidObject.getJson(detailed));
        }

        jsonObject["solidObjects"] = solidObjectsArray;
    }

    return jsonObject;
}
