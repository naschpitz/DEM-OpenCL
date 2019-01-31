#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#ifndef VERTEX_H
#define VERTEX_H

#include "nlohmann/json.hpp"
#include "Vector3D.h"

#include <CL/cl2.hpp>

typedef struct
{
    cl_bool fixed;

    cl_float4 originalPosition;
    cl_float4 originalVelocity;
    cl_float4 acceleration;

    cl_float4 currentPosition;
    cl_float4 oldPosition;

    cl_float4 currentVelocity;
    cl_float4 oldVelocity;
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
        Vector3D oldVelocity;

    public:
        Vertex();
        Vertex(const Vertex& other);
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

        nlohmann::json getJson() const;
};

#endif // VERTEX_H
