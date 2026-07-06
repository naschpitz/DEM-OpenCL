#ifndef TESTPARTICLEWORKER_H
#define TESTPARTICLEWORKER_H

#include <QObject>

#include "KernelTestHarness.h"

class TestParticleWorker : public QObject
{
    Q_OBJECT

    private:
        KernelTestHarness* harness;

    public:
        TestParticleWorker();

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();

        void p2pOverlappingRepel();
        void p2pSeparatedAttract();
        void p2pBeyondThreshold();

        void p2fParticleAboveFace();
        void p2fBeyondThreshold();
        void p2fNumFacesDivision();
};

#endif // TESTPARTICLEWORKER_H
