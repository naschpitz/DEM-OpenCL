#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

#ifndef VERTEX_H
#define VERTEX_H

#include "nlohmann/json.hpp"
#include "Vector3D.h"

#include <CL/opencl.hpp>

typedef struct
{
    cl_bool fixed;

    cl_float4 originalPosition;
    cl_float4 originalVelocity;
    cl_float4 acceleration;

    cl_float4 currentPosition;
    cl_float4 oldPosition;

    cl_double4 preciseCurrentPosition;
    cl_double4 preciseOldPosition;

    cl_float4 currentVelocity;
    cl_float4 meanVelocity;
    cl_float4 oldVelocity;

    cl_float4 angularAcceleration;

    cl_float4 currentAngularVelocity;
    cl_float4 meanAngularVelocity;
    cl_float4 oldAngularVelocity;
} VertexCL;

class Vertex
{
    protected:
        bool fixed;

        Vector3D originalPosition;
        Vector3D originalVelocity;
        Vector3D acceleration;

        Vector3D currentPosition;
        Vector3D oldPosition;

        Vector3D currentVelocity;
        Vector3D meanVelocity;
        Vector3D oldVelocity;

        Vector3D angularAcceleration;

        Vector3D currentAngularVelocity;
        Vector3D meanAngularVelocity;
        Vector3D oldAngularVelocity;

    public:
        Vertex();
        Vertex(const Vertex& other);
        Vertex& operator=(const Vertex& other);  // Copy assignment operator
        Vertex(const Vector3D& vector);
        Vertex(const double& x, const double& y, const double& z);

        VertexCL getCL() const;
        void setCL(const VertexCL& vertexCL);

        virtual void setFixed(bool fixed);
        virtual void setPosition(const Vector3D& position);
        virtual void setVelocity(const Vector3D& velocity);

        virtual void displaceBy(const Vector3D& displacement);

        virtual const Vector3D& getCurrentPosition() const;

        virtual const Vector3D& getCurrentVelocity() const;
        virtual const Vector3D& getMeanVelocity()    const;

        nlohmann::json getJson() const;
};

#endif // VERTEX_H
