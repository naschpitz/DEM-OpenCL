#ifndef TESTEDGE_H
#define TESTEDGE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestEdge : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestEdge();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void getClosestToInside();
    void getClosestToBeforeP1();
    void getClosestToAfterP2();
    void getClosestToCollinearBeyond();
    void getClosestToDegenerate();
};

#endif // TESTEDGE_H
