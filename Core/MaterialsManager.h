#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Material.h"

#include <QVector>
#include <QMap>
#include <QString>
#include <QJsonArray>

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
        MaterialsManager(const QJsonArray& materialsArray);

        MaterialsManagerCL getCL() const;

        const QVector<Material>& getMaterials() const;
        uint getMaterialIndex(const QString& id) const;
};

#endif // MATERIALMANAGER_H
