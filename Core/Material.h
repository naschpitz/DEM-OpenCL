#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef MATERIAL_H
#define MATERIAL_H

#include "nlohmann/json.hpp"

#include <CL/opencl.hpp>
#include <QString>
#include <QVector>
#include <QMap>

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
    cl_int materialIndex1;
    cl_int materialIndex2;

    cl_int forceType;
    cl_int dragForceType;

    cl_float distanceThreshold;

    cl_float coefficients[10];
    cl_float dragCoefficients[10];
} MaterialCL;

class Material
{
    private:
        QString id;

        QString material1;
        QString material2;

        double distanceThreshold;

        QString forceType;
        QString dragForceType;

        QVector<double> coefficients;
        QVector<double> dragCoefficients;

    public:
        Material();
        Material(const nlohmann::json& jsonObject);

        MaterialCL getCL(const QMap<QString, int>& idsMap) const;

        const QString& getId()        const;
        const QString& getMaterial1() const;
        const QString& getMaterial2() const;

        const double&  getDistanceThreshold() const;

        const QString& getForceType()     const;
        const QString& getDragForceType() const;

        const QVector<double>& getCoefficients()     const;
        const QVector<double>& getDragCoefficients() const;

        virtual bool operator==(const Material &other) const;

    private:
        void addCoefficient(const double& coefficient);
        void addDragCoefficient(const double& dragCoefficient);

        int fromQStringToForceType(const QString& forceTypeString) const;
        int fromQStringToDragForceType(const QString& dragForceTypeString) const;
};

#endif // MATERIAL_H
