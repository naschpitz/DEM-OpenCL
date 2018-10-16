#include "MaterialsManager.h"

#include <QJsonArray>
#include <QJsonObject>

MaterialsManager::MaterialsManager()
{

}

MaterialsManager::MaterialsManager(const QJsonArray& materialsArray)
{
    foreach(QJsonValue materialValue, materialsArray) {
        Material material(materialValue.toObject());

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

uint MaterialsManager::getMaterialIndex(const QString& id) const
{
    return this->idsMap[id];
}
