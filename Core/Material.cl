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

float4 material_calculateForce(const Material* material, float4 distance, float4 distanceUnitary, bool internal, float contactArea, float originalLength, float4 oldForce)
{
    float lengthDistance = length(distance);

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
            if (internal)
                distanceUnitary = -distanceUnitary;

            return (material->coefficients[0] / lengthDistance) * distanceUnitary;

        case inverse_quadratic:
            if (internal)
                distanceUnitary = -distanceUnitary;

            return (material->coefficients[0] / (lengthDistance * lengthDistance)) * distanceUnitary;

        case inverse_cubic:
            if (internal)
                distanceUnitary = -distanceUnitary;

            return (material->coefficients[0] / (lengthDistance * lengthDistance * lengthDistance)) * distanceUnitary;

        case morse:
        {
            if (internal)
                distanceUnitary = -distanceUnitary;

            float de = material->coefficients[0];
            float ke = material->coefficients[1];

            float a = sqrt(ke / (2 * de));
            float length = lengthDistance;

            // https://en.wikipedia.org/wiki/Morse_potential
            return -2 * de * a * exp(-a * length) * (1 - exp(-a * length)) * distanceUnitary;
        }

        case lennard_jones:
        {
            float e = material->coefficients[0];
            float n = material->coefficients[1];

            // https://en.wikipedia.org/wiki/Lennard-Jones_potential (n-exp form)
            // https://www.wolframalpha.com/input/?i=d%2Fdr+-E*%28%28s%2Fr%29%5E%282*n%29+-+2*%28s%2Fr%29%5E%28n%29%29
            return 2 * e * n * pown(originalLength, n) * pown(lengthDistance, (-2 * n) - 1) * (pown(originalLength, n) - pown(lengthDistance, n)) * distanceUnitary;
        }

        case realistic_material:
        {
            // Rupture regime:
            float ruptureDistance = originalLength * material->coefficients[3];

            if(lengthDistance > ruptureDistance && !internal) // First test, easyest calculation to return value, probably most of the cases.
                return (float4)0;

            // Elastic regime distance limit:
            float elasticLimitDistance = originalLength * material->coefficients[1];

            if(lengthDistance < elasticLimitDistance || internal) {
                // Elastic regime: F = -k*x
                float ea = material->coefficients[0] * contactArea;
                float k = ea / originalLength;

                return -k * distance;
            }

            else {
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

float4 material_calculateDragForce(const Material* material, float4 velocity, float4 rotationVelocity, float4 force)
{
    float forceModule;

    if(material->dragForceType >= 0)
        forceModule = length(force);

    switch(material->dragForceType)
    {
        case linear:
        {
            float4 linearDrag  = - material->dragCoefficients[0] * velocity;
            float4 angularDrag = - material->dragCoefficients[0] * rotationVelocity;

            return (linearDrag + angularDrag) * forceModule;
        }

        case quadratic:
        {
            float4 linearDrag  = - material->dragCoefficients[0] * pown(length(velocity), 2) * vector_getUnitary(velocity);
            float4 angularDrag = - material->dragCoefficients[0] * pown(length(rotationVelocity), 2) * vector_getUnitary(rotationVelocity);

            return (linearDrag + angularDrag) * forceModule;
        }

        case cubic:
        {
            float4 linearDrag  = - material->dragCoefficients[0] * pown(length(velocity), 3) * vector_getUnitary(velocity);
            float4 angularDrag = - material->dragCoefficients[0] * pown(length(rotationVelocity), 3) * vector_getUnitary(rotationVelocity);

            return (linearDrag + angularDrag) * forceModule;
        }
    }

    return (float4)0;
}

#endif // MATERIAL_CL
