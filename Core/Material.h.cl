#ifndef MATERIAL_H_CL
#define MATERIAL_H_CL

enum ForceType
{
    adiabatic_compression,
    hooks_law,
    inverse_linear,
    inverse_quadratic,
    inverse_cubic,
    morse,
    lennard_jones,
    realistic_material
};

enum DragForceType
{
    linear,
    quadratic,
    cubic
};

typedef struct
{
    int materialIndex1;
    int materialIndex2;

    int forceType;
    int dragForceType;

    float distanceThreshold;

    float coefficients[10];
    float dragCoefficients[10];
} Material;

#endif // MATERIAL_H_CL
