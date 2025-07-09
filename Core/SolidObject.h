#ifndef SOLIDOBJECT_H
#define SOLIDOBJECT_H

#include "nlohmann/json.hpp"
#include "Face.h"
#include "Vector3D.h"

#include <QString>
#include <QVector>

class SolidObject
{
    private:
        QString id;
        QString material;

        QString stl;
        double maximumTetrahedronVol;

        bool fixed;
        double mass;

        Vector3D position;
        Vector3D velocity;

        QVector<Face> faces;

    public:
        SolidObject();
        SolidObject(const nlohmann::json& jsonObject);

        void initialize();

        const QString& getId()     const;
        const QString& getMaterial() const;

        void getBox(Vector3D& min, Vector3D& max);

        double        getArea() const;
        const double& getMass() const;

        Vector3D getCurrentPosition() const;

        Vector3D getCurrentVelocity() const;
        Vector3D getMeanVelocity()    const;

        Vector3D getCurrentMomentum() const;
        Vector3D getMeanMomentum()    const;

        Vector3D getCurrentForce() const;
        Vector3D getMeanForce()    const;

        double getCurrentKineticEnergyTotal() const;
        double getMeanKineticEnergyTotal()    const;

        const QVector<Face>& getFaces() const;
        void setFacesCL(const QVector<FaceCL>& facesCL);
        nlohmann::json getJson(bool detailed) const;

    private:
        void loadStl();

        void setFixed();
        void setMass();
        void setMaterial();
        void setPosition();
        void setVelocity();
};

#endif // SOLIDOBJECT_H
