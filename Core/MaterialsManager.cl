#ifndef MATERIALSMANAGER_CL
#define MATERIALSMANAGER_CL

#include "../Material.cl"

typedef struct
{
    int size;
    Material materials[32];
} MaterialsManager;

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

#endif // MATERIALSMANAGER_CL

