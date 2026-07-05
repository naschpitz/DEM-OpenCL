#ifndef TESTPARTICLE_H
#define TESTPARTICLE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestParticle : public QObject
{
    Q_OBJECT

    private:
        KernelTestHarness* harness;

    public:
        TestParticle();

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();
        void getClosestToAligned();
        void getClosestTo3D();
        void getClosestToOverlapping();
        void isInternalOutside();
        void isInternalInside();
        void isInternalBoundary();
};

#endif // TESTPARTICLE_H
