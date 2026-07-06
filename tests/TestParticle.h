#ifndef TESTPARTICLE_H
#define TESTPARTICLE_H

#include <QObject>

#include "../Particle.h"

class TestParticle : public QObject
{
    Q_OBJECT

  public:
    TestParticle();

  private Q_SLOTS:
    void getCL();
    void setCL();
};

#endif // TESTPARTICLE_H
