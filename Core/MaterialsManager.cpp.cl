#ifndef MATERIALSMANAGER_CPP_CL
#define MATERIALSMANAGER_CPP_CL

#include "../Material.h.cl"
#include "../MaterialsManager.h.cl"

const Material* materialsManager_getMaterial(int materialIndex1, int materialIndex2, const MaterialsManager* materialsManager)
{
    if(materialIndex1 == materialIndex2)
        return &(materialsManager->materials[materialIndex1]);

    for(int i = 0; i < materialsManager->size; i++) {
        const Material* material = &(materialsManager->materials[i]);

        if((material->materialIndex1 == materialIndex1 && material->materialIndex2 == materialIndex2) ||
           (material->materialIndex1 == materialIndex2 && material->materialIndex2 == materialIndex1)) {
            return material;
        }
    }

    return 0;
}

#endif // MATERIALSMANAGER_CPP_CL

