#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Material.h"

#include <QVector>
#include <QMap>
#include <QString>
#include <QJsonValue>

typedef struct
{
    MaterialCL materials[20];
    cl_int size;
} MaterialsManagerCL;

class MaterialsManager
{
    private:
        QVector<Material> materials;
        QMap<QString, int> idsMap;

    public:
        MaterialsManager();
        MaterialsManager(const QJsonValue& jsonValue);

        MaterialsManagerCL getCL() const;

        const QVector<Material>& getMaterials() const;
        uint getMaterialIndex(const QString& materialId) const;
};

#endif // MATERIALMANAGER_H
