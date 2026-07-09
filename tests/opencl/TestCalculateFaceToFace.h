#ifndef TESTCALCULATEFACETOFACE_H
#define TESTCALCULATEFACETOFACE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestCalculateFaceToFace : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestCalculateFaceToFace();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void hookeForce();
    void beyondThreshold();
    void overlapping();
    void swapped();
};

#endif // TESTCALCULATEFACETOFACE_H
