#ifndef TESTOBJECTTOOBJECT_H
#define TESTOBJECTTOOBJECT_H

#include <QObject>

#include "KernelTestHarness.h"

class TestObjectToObject : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestObjectToObject();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void cornerToCorner();
    void faceToFace();
    void edgeToEdge();
    void overlapping();
    void touchingFaces();
    void lShapeToCube();
};

#endif // TESTOBJECTTOOBJECT_H
