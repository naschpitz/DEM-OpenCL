#ifndef MATERIALSMANAGER_H_CL
#define MATERIALSMANAGER_H_CL

#include "../Material.h.cl"

typedef struct
{
    int size;
    Material materials[20];
} MaterialsManager;

#endif // MATERIALSMANAGER_H_CL

