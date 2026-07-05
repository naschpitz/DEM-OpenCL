#ifndef TESTFACE_H
#define TESTFACE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestFace : public QObject
{
    Q_OBJECT

    private:
        KernelTestHarness* harness;

    public:
        TestFace();

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();
        void getClosestToInside();
        void getClosestToEdgeVNegative();
        void getClosestToEdgeUNegative();
        void getClosestToEdgeUVOverflow();
        void getClosestToSeamVZero();
        void getClosestToSeamUZero();
        void getClosestToSeamUVSumOne();
        void getClosestToNearVertexV0();
        void getClosestToNearVertexV1();
        void getClosestToNearVertexV2();
        void getClosestToCoplanar();
        void getClosestToDegenerate();
        void getClosestToRadiusOffset();
};

#endif // TESTFACE_H
