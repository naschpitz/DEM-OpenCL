#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>

#include "KernelTestHarness.h"

class TestObject : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestObject();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void outsideFace();
    void onFaceCenter();
    void sharedEdge();
    void sharedCorner();
    void insideCube();
    void farParticle();
};

#endif // TESTOBJECT_H
