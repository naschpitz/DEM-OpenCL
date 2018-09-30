#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#ifndef VERTEX_H
#define VERTEX_H

#include "Vector3D.h"

#include <CL/cl2.hpp>
#include <QJsonValue>
#include <QJsonObject>

typedef struct
{
    cl_bool fixed;

    cl_double4 originalPosition;
    cl_double4 originalVelocity;
    cl_double4 acceleration;

    cl_double4 currentPosition;
    cl_double4 oldPosition;

    cl_double4 currentVelocity;
    cl_double4 oldVelocity;
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
        Vertex(const QJsonValue& jsonValue);

        VertexCL getCL() const;
        void setCL(const VertexCL& vertexCL);

        virtual void setFixed(bool fixed);
        virtual void setPosition(const Vector3D& position);
        virtual void setVelocity(const Vector3D& velocity);

        virtual void displaceBy(const Vector3D& displacement);

        virtual const Vector3D& getCurrentPosition() const;
        virtual const Vector3D& getCurrentVelocity() const;

        QJsonObject getJson() const;
};

#endif // VERTEX_H
