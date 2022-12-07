#include "Scenery.h"

Scenery::Scenery()
{

}

Scenery::Scenery(const nlohmann::json& jsonObject)
{
    try {
        this->id = QString::fromStdString(jsonObject.at("_id").get<std::string>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing '_id' field in Scenery");
    }

    try {
        const nlohmann::json& gravityArray = jsonObject.at("gravity");
        this->gravity = Vector3D(gravityArray.at(0).get<double>(), gravityArray.at(1).get<double>(), gravityArray.at(2).get<double>());
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'gravity' field in Scenery");
    }

    try {
        const nlohmann::json& materialsManager = jsonObject.at("materials");
        this->materialsManager = MaterialsManager(materialsManager);
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'materials' field in Scenery");
    }

    try {
        const nlohmann::json& objectsManager = jsonObject.at("objects");
        this->objectsManager = ObjectsManager(objectsManager);
    }

    catch (const nlohmann::detail::exception& e) {
        throw std::runtime_error("Missing 'objects' field in Scenery");
    }
}

SceneryCL Scenery::getCL() const
{
    SceneryCL sceneryCL;

    sceneryCL.numParticles = this->objectsManager.getParticlesCL(this->materialsManager).size();
    sceneryCL.numFaces     = this->objectsManager.getFacesCL(this->materialsManager).size();

    sceneryCL.gravity = {this->gravity.getX(), this->gravity.getY(), this->gravity.getZ(), 0};

    return sceneryCL;
}

const QString& Scenery::getId() const
{
    return this->id;
}

const ObjectsManager& Scenery::getObjectsManager() const
{
    return this->objectsManager;
}

const MaterialsManager& Scenery::getMaterialsManager() const
{
    return this->materialsManager;
}

const nlohmann::json Scenery::getJson(bool detailed = true) const
{
    nlohmann::json jsonObject;

    jsonObject["_id"] = this->id.toStdString();
    jsonObject["objects"] = this->objectsManager.getJson(detailed);

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
