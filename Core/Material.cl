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

    double distanceThreshold;

    double coefficients[10];
    double dragCoefficients[10];
} Material;

double4 material_calculateForce(const Material* material, double4 distance, double4 distanceUnitary, bool internal, double contactArea, double originalLength, double4 oldForce)
{
    double lengthDistance = length(distance);

    lengthDistance = (internal ? -lengthDistance : lengthDistance) + originalLength;

    switch(material->forceType)
    {
        case adiabatic_compression:
        {
            double newRadius = lengthDistance / 2.;
            double newRadius2 = newRadius * newRadius;

            double4 force = (material->coefficients[0] * 12.56637061 * newRadius2 * pow(pown(originalLength / 2., 3) / pown(newRadius, 3), material->coefficients[1])) * distanceUnitary;

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
            double e = material->coefficients[0];
            double n = material->coefficients[1];

            return 2 * e * n * pown(originalLength, n) * pown(lengthDistance, (-2 * n) - 1) * (pown(originalLength, n) - pown(lengthDistance, n)) * distanceUnitary;
        }

        case realistic_material:
        {
            // Rupture regime:
            double ruptureDistance = originalLength * material->coefficients[3];

            if(length(distance) > ruptureDistance && !internal) // First test, easyest calculation to return value, probably most of the cases.
                return (double4)0;

            // Elastic regime distance limit:
            double elasticLimitDistance = originalLength * material->coefficients[1];

            if(length(distance) < elasticLimitDistance || internal) {
                // Elastic regime: F = -k*x
                double ea = material->coefficients[0] * contactArea;
                double k = ea / originalLength;

                return -k * distance;
            }

            else {
                double4 distanceUnitary = vector_getUnitary(distance);
                double4 oldForceProjection = dot(oldForce, distanceUnitary) * distanceUnitary;

                // Plastic regime: F = min(plasticMax, oldForce)
                double plasticMaximumForce = material->coefficients[2] * contactArea;

                if(length(oldForceProjection) > plasticMaximumForce)
                    return plasticMaximumForce * vector_getUnitary(oldForceProjection);

                else
                    return -oldForceProjection;
            }
        }
    }

    return (double4)0;
}

double4 material_calculateDragForce(const Material* material, double4 velocity, double4 distance)
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

    return (double4)0;
}

#endif // MATERIAL_CL
