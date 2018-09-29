#ifndef TESTVECTOR3D_H
#define TESTVECTOR3D_H

#include <QObject>

#include "../Core/Vector3D.h"

class TestVector3D : public QObject
{
    Q_OBJECT

    public:
        TestVector3D();

    private Q_SLOTS:
        void emptyConstructor();
        void xyzConstructor();
        void assignmentOperatorToVector3d();
        void assignmentOperatorToDouble();
        void sumAssignmentOperatorToVector3d();
        void sumAssignmentOperatorToDouble();
        void subtractionAssignmentOperatorToVector3d();
        void subtractionAssignmentOperatorToDouble();
        void dotProductAssingmentToDouble();
        void dotProductToDouble();
        void crossProductToVector3d();
        void divisionByDouble();
        void sumToVector3d();
        void sumToDouble();
        void subtractionToVector3d();
        void subtractionToDouble();
        void equaltyOperatorEqual();
        void equaltyOperatorDifferent();
        void getModuleSquared();
        void getModule();
        void getUnitary();
        void dotDoubleToVector3d();
        void sumDoubleToVector3d();
        void subtractionDoubleToVector3d();
        void signalInversion();
        void implicitAssignmentOperator();
};

#endif // TESTVECTOR3D_H
