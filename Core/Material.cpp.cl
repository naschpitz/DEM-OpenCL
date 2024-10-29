#ifndef MATERIAL_CPP_CL
#define MATERIAL_CPP_CL

#include "../Material.h.cl"

#include "../Vector.cpp.cl"

float4 material_calculateForce(const Material* material, float4 distance, float4 distanceUnitary, bool internal, float contactArea, float originalLength, float4 oldForce)
{
    float lengthDistance = length(distance);
    float4 force = (float4)0;

    switch(material->forceType)
    {     
        case adiabatic_compression:
        {
            float newRadius = (lengthDistance + originalLength) / 2.;
            float newRadius2 = newRadius * newRadius;

            force = ((float)(material->coefficients[0] * 12.56637061 * newRadius2) * pow(pown((float)(originalLength / 2.), 3) / pown(newRadius, 3), material->coefficients[1])) * distanceUnitary;
            break;
        }

        case hooks_law:
            force = -(material->coefficients[0]) * distance;
            break;

        case inverse_linear:
            if (internal)
                distanceUnitary = -distanceUnitary;

            force = (material->coefficients[0] / lengthDistance) * distanceUnitary;
            break;

        case inverse_quadratic:
            if (internal)
                distanceUnitary = -distanceUnitary;

            force = (material->coefficients[0] / (lengthDistance * lengthDistance)) * distanceUnitary;
            break;

        case inverse_cubic:
            if (internal)
                distanceUnitary = -distanceUnitary;

            force = (material->coefficients[0] / (lengthDistance * lengthDistance * lengthDistance)) * distanceUnitary;
            break;

        case morse:
        {
            if (internal)
                distanceUnitary = -distanceUnitary;

            float de = material->coefficients[0];
            float ke = material->coefficients[1];

            float a = sqrt(ke / (2 * de));
            float length = lengthDistance;

            // https://en.wikipedia.org/wiki/Morse_potential
            force = -2 * de * a * exp(-a * length) * (1 - exp(-a * length)) * distanceUnitary;
            break;
        }

        case lennard_jones:
        {
            float e = material->coefficients[0];
            float n = material->coefficients[1];

            // https://en.wikipedia.org/wiki/Lennard-Jones_potential (n-exp form)
            // https://www.wolframalpha.com/input/?i=d%2Fdr+-E*%28%28s%2Fr%29%5E%282*n%29+-+2*%28s%2Fr%29%5E%28n%29%29
            force = 2 * e * n * pown(originalLength, n) * pown(lengthDistance, (-2 * n) - 1) * (pown(originalLength, n) - pown(lengthDistance, n)) * distanceUnitary;
            break;
        }

        case realistic_material:
        {
            // https://en.wikipedia.org/wiki/Young%27s_modulus

            // Rupture regime:
            float ruptureDistance = originalLength * material->coefficients[3];

            if(lengthDistance > ruptureDistance && !internal) // First test, easyest calculation to return value, probably most of the cases.
                force = (float4)0;

            // Elastic regime distance limit:
            float elasticLimitDistance = originalLength * material->coefficients[1];

            if(lengthDistance < elasticLimitDistance || internal) {
                // Elastic regime: F = -k*x
                float ea = material->coefficients[0] * contactArea;
                float k = ea / originalLength;

                force = -k * distance;
            }

            else {
                float4 oldForceProjection = dot(oldForce, distanceUnitary) * distanceUnitary;

                // Plastic regime: F = min(plasticMax, oldForce)
                float plasticMaximumForce = material->coefficients[2] * contactArea;

                if(length(oldForceProjection) > plasticMaximumForce)
                    force = plasticMaximumForce * vector_getUnitary(oldForceProjection);

                else
                    force = -oldForceProjection;
            }

            break;
        }
    }

    int4 nan_mask = isnan(force);
    bool any_nan = nan_mask.x || nan_mask.y || nan_mask.z || nan_mask.w;

    if (any_nan)
        return (float4)0;

    return force;
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

#endif // MATERIAL_CPP_CL
