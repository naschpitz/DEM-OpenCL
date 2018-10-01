#ifndef SCENERY_H
#define SCENERY_H

#include "NonSolidObject.h"
#include "MaterialsManager.h"
#include "ObjectsManager.h"
#include "Vector3D.h"

#include <QJsonValue>

class Scenery
{
    private:
        ObjectsManager objectsManager;
        MaterialsManager materialsManager;

    public:
        Scenery();
        Scenery(const QJsonValue& jsonValue);
        const ObjectsManager& getObjectsManager() const;
        const MaterialsManager& getMaterialsManager() const;
        const QJsonObject getJson() const;

        void setParticlesCL(const QVector<ParticleCL>& particlesCL);
        void setFacesCL(const QVector<FaceCL>& facesCL);
};

#endif // SCENERY_H
