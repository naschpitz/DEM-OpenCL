#ifndef TESTFACE_H
#define TESTFACE_H

#include <QObject>

#include "../Face.h"

class TestFace : public QObject
{
    Q_OBJECT

  public:
    TestFace();

  private slots:
    void constructor();
    void getArea();
    void getNormal();
    void getCL();
    void setCLgetCLRoundtrip();
    void getJson();
    void displaceBy();
    void setFixed();
    void setVelocity();
    void getCurrentMomentum();
    void getCurrentKineticEnergy();
};

#endif // TESTFACE_H
