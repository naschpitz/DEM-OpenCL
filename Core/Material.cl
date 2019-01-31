#ifndef MATERIAL_CL
#define MATERIAL_CL

#include "../Vector.cl"

enum ForceType
{
    adiabatic_compression,
    hooks_law,
    inverse_linear,
    inverse_quadratic,
    inverse_cubic,
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

float4 material_calculateForce(const Material* material, float4 distance, float4 distanceUnitary, bool internal, float contactArea, float originalLength, float4 oldForce)
{
    float lengthDistance = length(distance);

    lengthDistance = (internal ? -lengthDistance : lengthDistance) + originalLength;

    switch(material->forceType)
    {
        case adiabatic_compression:
        {
            float newRadius = lengthDistance / 2.;
            float newRadius2 = newRadius * newRadius;

            float4 force = ((float)(material->coefficients[0] * 12.56637061 * newRadius2) * pow(pown((float)(originalLength / 2.), 3) / pown(newRadius, 3), material->coefficients[1])) * distanceUnitary;

            return force;
        }

        case hooks_law:
            return -(material->coefficients[0]) * distance;

        case inverse_linear:
            return (material->coefficients[0] / lengthDistance) * distanceUnitary;

        case inverse_quadratic:
            return (material->coefficients[0] / (lengthDistance * lengthDistance)) * distanceUnitary;

        case inverse_cubic:
            return (material->coefficients[0] / (lengthDistance * lengthDistance * lengthDistance)) * distanceUnitary;

        case lennard_jones:
        {
            float e = material->coefficients[0];
            float n = material->coefficients[1];

            return 2 * e * n * pown(originalLength, n) * pown(lengthDistance, (-2 * n) - 1) * (pown(originalLength, n) - pown(lengthDistance, n)) * distanceUnitary;
        }

        case realistic_material:
        {
            // Rupture regime:
            float ruptureDistance = originalLength * material->coefficients[3];

            if(length(distance) > ruptureDistance && !internal) // First test, easyest calculation to return value, probably most of the cases.
                return (float4)0;

            // Elastic regime distance limit:
            float elasticLimitDistance = originalLength * material->coefficients[1];

            if(length(distance) < elasticLimitDistance || internal) {
                // Elastic regime: F = -k*x
                float ea = material->coefficients[0] * contactArea;
                float k = ea / originalLength;

                return -k * distance;
            }

            else {
                float4 distanceUnitary = vector_getUnitary(distance);
                float4 oldForceProjection = dot(oldForce, distanceUnitary) * distanceUnitary;

                // Plastic regime: F = min(plasticMax, oldForce)
                float plasticMaximumForce = material->coefficients[2] * contactArea;

                if(length(oldForceProjection) > plasticMaximumForce)
                    return plasticMaximumForce * vector_getUnitary(oldForceProjection);

                else
                    return -oldForceProjection;
            }
        }
    }

    return (float4)0;
}

float4 material_calculateDragForce(const Material* material, float4 velocity, float4 distance)
{
    switch(material->dragForceType)
    {
        case linear:
            return -(material->dragCoefficients[0]) * velocity;

        case quadratic:
            return -(material->dragCoefficients[0]) * pown(length(velocity), 2) * vector_getUnitary(velocity);

        case cubic:
            return -(material->dragCoefficients[0]) * pown(length(velocity), 3) * vector_getUnitary(velocity);
    }

    return (float4)0;
}

#endif // MATERIAL_CL
