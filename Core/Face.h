#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#ifndef FACE_H
#define FACE_H

#include "Vertex.h"

#include <CL/cl2.hpp>
#include <QString>
#include <QVector>
#include <QMap>
#include <QJsonValue>

typedef struct
{
    cl_uint index;
    cl_uint materialIndex;

    cl_double area;

    cl_double4 currentForce;
    cl_double4 oldForce;

    cl_double4 currentTorque;
    cl_double4 oldTorque;

    VertexCL v1, v2, v3;
} FaceCL;

class Face
{
    private:
        QString _id;
        QString materialId;

        double area;

        Vector3D currentForce;
        Vector3D oldForce;

        Vector3D currentTorque;
        Vector3D oldTorque;

        Vertex v1, v2, v3;

    public:
        Face();

        Vector3D      getNormal() const;
        const double& getArea()   const;

    private:
        void calculateArea();
};

#endif // FACE_H
