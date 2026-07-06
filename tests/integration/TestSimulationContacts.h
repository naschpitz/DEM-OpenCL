#ifndef TESTSIMULATIONCONTACTS_H
#define TESTSIMULATIONCONTACTS_H

#include <QtTest>

#include "nlohmann/json.hpp"

class TestSimulationContacts : public QObject
{
    Q_OBJECT

  private:
    // Builds a Simulation JSON whose scenery contains exactly TWO
    // NonSolidObjects, each with a single particle (spacing=1,
    // length=[1.5,1.5,1.5]). The particles are placed at (-posX,0,0)
    // and (+posX,0,0). When posX < 1.0 (sum of radii) the particles
    // overlap and interact via hooks_law. Gravity is zero. Neighborhoods
    // are recalculated every step (calcNeighTimeInt == timeStep).
    static nlohmann::json contactSimulationJson(double posX, bool fixedA, bool fixedB);

    // Read particle[0] position/velocity from nonSolidObjects[obj].
    static double particlePosX(const nlohmann::json& scenery, int obj);
    static double particleVelX(const nlohmann::json& scenery, int obj);
    static double particleVelY(const nlohmann::json& scenery, int obj);
    static double particleVelZ(const nlohmann::json& scenery, int obj);

  private slots:
    void initTestCase();

    // Two identical overlapping free particles repel via hooks_law.
    // Newton's third law: total momentum must remain ~0 at every frame.
    void twoParticlesConserveMomentum();

    // Two identical free particles at mirrored positions (-d,0,0) and
    // (d,0,0). The dynamics are mirror-symmetric: posA.x = -posB.x and
    // velA.x = -velB.x at every frame. Y and Z stay at rest.
    void twoParticlesRemainSymmetric();

    // One free, one fixed, both overlapping. The fixed particle must stay
    // at its initial position with zero velocity. The free particle must
    // move away (repelled by the contact force).
    void fixedParticleStaysAtRest();
};

#endif // TESTSIMULATIONCONTACTS_H
