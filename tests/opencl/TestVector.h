#ifndef TESTVECTOR_H
#define TESTVECTOR_H

#include <QObject>

#include "KernelTestHarness.h"

class TestVector : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestVector();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void getUnitaryGeneric();
    void getUnitaryAlreadyUnit();
    void getUnitaryZero();
};

#endif // TESTVECTOR_H
