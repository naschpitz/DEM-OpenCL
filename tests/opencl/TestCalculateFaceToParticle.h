#ifndef TESTCALCULATEFACETOPARTICLE_H
#define TESTCALCULATEFACETOPARTICLE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestCalculateFaceToParticle : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestCalculateFaceToParticle();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void particleAboveFace();
    void beyondThreshold();
    void numFacesDivision();
};

#endif // TESTCALCULATEFACETOPARTICLE_H
