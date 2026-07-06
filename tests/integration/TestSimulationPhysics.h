#ifndef TESTSIMULATIONPHYSICS_H
#define TESTSIMULATIONPHYSICS_H

#include <QtTest>

#include "nlohmann/json.hpp"

class TestSimulationPhysics : public QObject
{
    Q_OBJECT

  private:
    // Builds a Simulation JSON whose scenery contains exactly ONE particle
    // (a NonSolidObject with spacing=1, length=[1.5,1.5,1.5] yields a single
    // particle under the close-packing builder, centered at the object's
    // position). gravity and particleVelocity parameterize the scene. The
    // time window is short (totalTime=0.1, timeStep=0.001 -> 100 steps,
    // 11 frames at steps 0,10,...,100) so the test runs in well under a
    // second on any GPU.
    static nlohmann::json physicsSimulationJson(double gx, double gy, double gz, double vx, double vy, double vz);

    // Reads particle[0] position out of a captured scenery snapshot.
    static double particleX(const nlohmann::json& scenery, int particle = 0);
    static double particleY(const nlohmann::json& scenery, int particle = 0);
    static double particleZ(const nlohmann::json& scenery, int particle = 0);

  private slots:
    // Executed once before the first test slot: verifies a GPU/OpenCL device
    // is reachable so the physics slots can assume the kernel build will work.
    void initTestCase();

    // g=0, v=0: a single free particle with no forces must not move. Verlet
    // is exact for zero acceleration, so position and velocity stay at their
    // initial values to float32 precision. This is the strongest smoke test
    // of the whole loop (kernel load, marshal, neighborhood, forces, gravity,
    // integrate, readback, frame emission) on a real GPU.
    void stationaryParticleDrifts();

    // g=0, v=[1,0,0]: no forces, so the particle moves at constant velocity.
    // Verlet gives x_n = x_0 + v_0 * (n*dt) exactly. Locks the velocity term.
    void particleMovesAtConstantVelocity();

    // g=[0,0,-9.81], v=0: constant acceleration. The initialize_particles
    // kernel seeds oldPosition = originalPosition - originalVelocity*dt
    // (without the +0.5*a*dt^2 term), so the discrete Verlet closed form is
    //   z_n = 0.5 * a * dt^2 * n * (n+1)
    // rather than the textbook 0.5*a*(n*dt)^2. The n(n+1) factor vs n^2 is a
    // ~1% effect at n=100; this test locks the ACTUAL integration behavior.
    void particleFallsUnderGravity();
};

#endif // TESTSIMULATIONPHYSICS_H
