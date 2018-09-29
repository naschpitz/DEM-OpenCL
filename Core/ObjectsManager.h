#ifndef OBJECTSMANAGER_H
#define OBJECTSMANAGER_H

#include "MaterialsManager.h"
#include "NonSolidObject.h"
#include "SolidObject.h"

#include <QVector>

class ObjectsManager
{
    private:
        QVector<NonSolidObject> nonSolidObjects;
        QVector<SolidObject> solidObjects;
        QMap<QString, uint> idsMap;

    public:
        ObjectsManager();
        ObjectsManager(const QJsonValue& jsonValue);

        const QVector<NonSolidObject>& getNonSolidObjects() const;
        const QVector<SolidObject>& getSolidObjects() const;

        const QVector<ParticleCL> getParticlesCL(const MaterialsManager& materialsManager) const;
        void setParticlesCL(const QVector<ParticleCL>& particlesCL);

        QJsonObject getJson() const;
};

#endif // OBJECTSMANAGER_H
