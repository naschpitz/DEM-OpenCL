#ifndef TESTNONCONVEXOBJECT_H
#define TESTNONCONVEXOBJECT_H

#include <QObject>

#include "KernelTestHarness.h"

class TestNonConvexObject : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestNonConvexObject();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void pocketInnerWallY();
    void outerHull();
    void equidistantInnerFaces();
    void pocketInnerWallX();
};

#endif // TESTNONCONVEXOBJECT_H
