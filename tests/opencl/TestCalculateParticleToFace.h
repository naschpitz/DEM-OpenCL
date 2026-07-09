#ifndef TESTCALCULATEPARTICLETOFACE_H
#define TESTCALCULATEPARTICLETOFACE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestCalculateParticleToFace : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestCalculateParticleToFace();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void particleAboveFace();
    void beyondThreshold();
    void numFacesDivision();
};

#endif // TESTCALCULATEPARTICLETOFACE_H
