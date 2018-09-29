#include "MaterialsManager.h"

#include <QJsonArray>
#include <QJsonObject>

MaterialsManager::MaterialsManager()
{

}

MaterialsManager::MaterialsManager(const QJsonValue& jsonValue)
{
    QJsonArray materialsArray = jsonValue.toArray();

    foreach(QJsonValue materialValue, materialsArray) {
        Material material(materialValue);

        this->idsMap[material.getId()] = this->materials.count();

        this->materials.push_back(material);
    }
}

MaterialsManagerCL MaterialsManager::getCL() const
{
    MaterialsManagerCL materialsManagerCL;

    materialsManagerCL.size = this->materials.count();

    foreach(Material material, this->materials) {
        int index = this->materials.indexOf(material);

        materialsManagerCL.materials[index] = material.getCL(this->idsMap);
    }

    return materialsManagerCL;
}

const QVector<Material>& MaterialsManager::getMaterials() const
{
    return this->materials;
}

uint MaterialsManager::getMaterialIndex(const QString& materialId) const
{
    return this->idsMap[materialId];
}
