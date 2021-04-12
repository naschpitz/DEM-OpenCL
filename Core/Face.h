#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef FACE_H
#define FACE_H

#include "Vertex.h"
#include "nlohmann/json.hpp"

#include <CL/cl2.hpp>
#include <QString>
#include <QVector>
#include <QMap>

typedef struct
{
    cl_uint index;
    cl_uint materialIndex;

    cl_float area;
    cl_float mass;

    cl_float4 currentPosition;
    cl_float4 currentVelocity;

    cl_float4 currentForce;
    cl_float4 oldForce;

    cl_float4 currentTorque;
    cl_float4 oldTorque;

    VertexCL vertexes[3];
} FaceCL;

class Face
{
    private:
        QString material;

        double area;
        double mass;

        Vector3D currentPosition;
        Vector3D currentVelocity;

        Vector3D currentForce;
        Vector3D oldForce;

        Vector3D currentTorque;
        Vector3D oldTorque;

        QVector<Vertex> vertexes;

    public:
        Face();
        Face(const Vertex& v1, const Vertex& v2, const Vertex& v3);

        const QVector<Vertex>& getVertexes() const;

        FaceCL getCL(uint index, uint materialIndex) const;
        void setCL(const FaceCL& particleCL);

        void displaceBy(const Vector3D& displacement);
        void setFixed(bool fixed);
        void setMass(const double& mass);
        void setMaterial(const QString& material);
        void setVelocity(const Vector3D& velocity);

        Vector3D      getNormal() const;
        const double& getArea()   const;

        Vector3D getCurrentMomentum() const;
        double   getCurrentKineticEnergy() const;

        const Vector3D& getCurrentPosition() const;
        const Vector3D& getCurrentVelocity() const;
        const Vector3D& getCurrentForce() const;
        const Vector3D& getCurrentTorque() const;

        nlohmann::json getJson() const;

    private:
        void calculateArea();
};

#endif // FACE_H
