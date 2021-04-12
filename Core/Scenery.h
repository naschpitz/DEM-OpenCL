#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef SCENERY_H
#define SCENERY_H

#include "nlohmann/json.hpp"
#include "NonSolidObject.h"
#include "MaterialsManager.h"
#include "ObjectsManager.h"
#include "Vector3D.h"

#include <CL/cl2.hpp>

typedef struct
{
    cl_ulong numParticles;
    cl_ulong numFaces;

    cl_float4 gravity;
} SceneryCL;

class Scenery
{
    private:
        QString id;
        QString owner;
        Vector3D gravity;

        ObjectsManager objectsManager;
        MaterialsManager materialsManager;

    public:
        Scenery();
        Scenery(const nlohmann::json& jsonObject);

        SceneryCL getCL() const;

        const QString& getId() const;

        const ObjectsManager& getObjectsManager() const;
        const MaterialsManager& getMaterialsManager() const;
        const nlohmann::json getJson() const;

        void setParticlesCL(const QVector<ParticleCL>& particlesCL);
        void setFacesCL(const QVector<FaceCL>& facesCL);
};

#endif // SCENERY_H
