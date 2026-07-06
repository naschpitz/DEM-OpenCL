#ifndef TESTMATERIAL_H
#define TESTMATERIAL_H

#include <QObject>

#include "KernelTestHarness.h"

class TestMaterial : public QObject
{
    Q_OBJECT

  private:
    KernelTestHarness* harness;

  public:
    TestMaterial();

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void hooksLaw();
    void inverseLinearExternal();
    void inverseLinearInternal();
    void inverseQuadratic();
    void inverseCubic();
    void adiabaticCompression();
    void morseEquilibrium();
    void morseNontrivial();
    void lennardJones();
    void realisticElastic();
    void realisticInternal();
    void realisticPlasticBelowMax();
    void realisticPlasticAboveMax();
    void nanGuard();

    void dragLinear();
    void dragLinearWithRotation();
    void dragQuadratic();
    void dragCubic();
    void dragNone();
};

#endif // TESTMATERIAL_H
