#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#ifndef MATERIAL_H
#define MATERIAL_H

#include <CL/cl2.hpp>
#include <QString>
#include <QVector>
#include <QMap>
#include <QJsonValue>

enum ForceType
{
    hooks_law,
    inverse_linear,
    inverse_quadratic,
    inverse_cubic,
    adiabatic_compression,
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
    cl_int index;

    cl_int materialIndex1;
    cl_int materialIndex2;

    cl_double distanceThreshold;

    cl_int forceType;
    cl_int dragForceType;

    cl_double coefficients[10];
    cl_double dragCoefficients[10];
} MaterialCL;

class Material
{
    private:
        QString _id;
        QString name;

        QString materialId1;
        QString materialId2;

        double distanceThreshold;

        QString forceType;
        QString dragForceType;

        QVector<double> coefficients;
        QVector<double> dragCoefficients;

    public:
        Material();
        Material(const QJsonValue& jsonValue);

        MaterialCL getCL(const QMap<QString, int>& idsMap) const;

        const QString& getId()          const;
        const QString& getName()        const;
        const QString& getMaterialId1() const;
        const QString& getMaterialId2() const;

        const double&  getDistanceThreshold() const;

        const QString& getForceType()     const;
        const QString& getDragForceType() const;

        const QVector<double>& getCoefficients()     const;
        const QVector<double>& getDragCoefficients() const;

        virtual bool operator==(const Material &other) const;

    private:
        void setId(const QString& _id);
        void setName(const QString& name);
        void setParentsIds(const QString& materialId1, const QString& materialId2);

        void setDensity(const double& density);
        void setDistanceThreshold(const double& distanceThreshold);

        void setForceType(const QString& forceType);
        void setDragForceType(const QString& dragForceType);

        void addCoefficient(const double& coefficient);
        void addDragCoefficient(const double& dragCoefficient);

        int fromQStringToForceType(const QString& forceTypeString) const;
        int fromQStringToDragForceType(const QString& dragForceTypeString) const;
};

#endif // MATERIAL_H
