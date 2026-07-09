#ifndef TESTCALCULATEPARTICLETOPARTICLE_H
#define TESTCALCULATEPARTICLETOPARTICLE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestCalculateParticleToParticle : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestCalculateParticleToParticle();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void overlappingRepel();
    void separatedAttract();
    void beyondThreshold();
};

#endif // TESTCALCULATEPARTICLETOPARTICLE_H
