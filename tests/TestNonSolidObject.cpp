#include "TestNonSolidObject.h"

#include <QTest>
#include <QtGlobal>
#include <array>
#include <math.h>

TestNonSolidObject::TestNonSolidObject()
{
    this->nonSolidObjectWireframeJsonValue = nlohmann::json::parse(R"({
        "_id": "TestNonSolidObjectWireframe",
        "material": "6pwfsYxGumpgZzAfP",
        "type": "wireframe",
        "density": 7.07,
        "dimensions": {
            "length": [10, 10, 10],
            "spacing": 1
        },
        "fixed": false,
        "position": [0, 0, 0],
        "velocity": [4.04, 5.05, 6.06]
    })");
}

// All NonSolidObject queries iterate this->particles, which are only built by
// initialize(). Without it the particle vector is empty and the getters divide
// by zero (getCurrentVolume == 0) or index particles[0] out of range (getBox).
// Every test below therefore constructs and initializes the object first.

void TestNonSolidObject::constructorWireframe()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);
    nonSolidObject.initialize();

    // mass == volume * density (each particle's mass is set as density * volume)
    double density     = this->nonSolidObjectWireframeJsonValue["density"].get<double>();
    double expectedMass = nonSolidObject.getCurrentVolume() * density;

    // Relative tolerance: particle packing sums many cl_float-rounded terms.
    QVERIFY(qAbs(nonSolidObject.getCurrentMass() - expectedMass) <= 1e-4 * qAbs(expectedMass));

    // The packing must have produced particles.
    QVERIFY(nonSolidObject.getCurrentVolume() > 0.0);
}

void TestNonSolidObject::getMaterialId()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);

    const QString expectedMaterial = "6pwfsYxGumpgZzAfP";

    QVERIFY(nonSolidObject.getMaterial() == expectedMaterial);
}

void TestNonSolidObject::getBox()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);
    nonSolidObject.initialize();

    Vector3D min, max;
    nonSolidObject.getBox(min, max);

    // setPosition() centers the bounding box on this->position ([0,0,0] here),
    // so the box midpoint must sit at the origin (within one packing spacing).
    Vector3D center((min.getX() + max.getX()) / 2.0,
                    (min.getY() + max.getY()) / 2.0,
                    (min.getZ() + max.getZ()) / 2.0);

    QVERIFY(qAbs(center.getX()) < 1.0);
    QVERIFY(qAbs(center.getY()) < 1.0);
    QVERIFY(qAbs(center.getZ()) < 1.0);

    // The box spans roughly the requested length ([10,10,10]).
    QVERIFY((max.getX() - min.getX()) > 8.0);
    QVERIFY((max.getY() - min.getY()) > 8.0);
    QVERIFY((max.getZ() - min.getZ()) > 8.0);
}

void TestNonSolidObject::getCurrentMomentum()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);
    nonSolidObject.initialize();

    double density      = this->nonSolidObjectWireframeJsonValue["density"].get<double>();
    double mass         = nonSolidObject.getCurrentVolume() * density;
    Vector3D velocity   = nonSolidObject.getCurrentVelocity();

    Vector3D currentMomentum    = nonSolidObject.getCurrentMomentum();
    Vector3D expectedMomentum   = mass * velocity;

    // Relative tolerance: momentum sums mass*velocity over hundreds of particles.
    QVERIFY(qAbs(currentMomentum.getX() - expectedMomentum.getX()) <= 1e-4 * qAbs(expectedMomentum.getX()));
    QVERIFY(qAbs(currentMomentum.getY() - expectedMomentum.getY()) <= 1e-4 * qAbs(expectedMomentum.getY()));
    QVERIFY(qAbs(currentMomentum.getZ() - expectedMomentum.getZ()) <= 1e-4 * qAbs(expectedMomentum.getZ()));
}

void TestNonSolidObject::getCurrentKineticEnergyTotal()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);
    nonSolidObject.initialize();

    double density    = this->nonSolidObjectWireframeJsonValue["density"].get<double>();
    double mass       = nonSolidObject.getCurrentVolume() * density;
    Vector3D velocity = nonSolidObject.getCurrentVelocity();

    double currentKineticEnergyTotal   = nonSolidObject.getCurrentKineticEnergyTotal();
    double expectedKineticEnergyTotal  = mass * velocity.getModuleSquared() / 2.0;

    QVERIFY(qAbs(currentKineticEnergyTotal - expectedKineticEnergyTotal) <= 1e-4 * qAbs(expectedKineticEnergyTotal));
}

void TestNonSolidObject::getCurrentKineticEnergyExternal()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);
    nonSolidObject.initialize();

    double density    = this->nonSolidObjectWireframeJsonValue["density"].get<double>();
    double mass       = nonSolidObject.getCurrentVolume() * density;
    Vector3D velocity = nonSolidObject.getCurrentVelocity();

    // Uniform velocity => external KE == total KE.
    double currentKineticEnergyExternal  = nonSolidObject.getCurrentKineticEnergyExternal();
    double expectedKineticEnergyExternal = mass * velocity.getModuleSquared() / 2.0;

    QVERIFY(qAbs(currentKineticEnergyExternal - expectedKineticEnergyExternal) <= 1e-4 * qAbs(expectedKineticEnergyExternal));
}

void TestNonSolidObject::getCurrentKineticEnergyInternal()
{
    NonSolidObject nonSolidObject(this->nonSolidObjectWireframeJsonValue);
    nonSolidObject.initialize();

    // Uniform velocity and zero angular velocity => no internal kinetic energy.
    // internal = total - external; both large and nearly equal => cancellation
    // residual is non-zero, so bound the residual relative to total, not to zero.
    double currentKineticEnergyInternal = nonSolidObject.getCurrentKineticEnergyInternal();
    double currentKineticEnergyTotal    = nonSolidObject.getCurrentKineticEnergyTotal();

    QVERIFY(qAbs(currentKineticEnergyInternal) <= 1e-4 * qAbs(currentKineticEnergyTotal));
}

void TestNonSolidObject::particlePackingCount()
{
    // buildObject uses hexagonal close packing (HCP), not a simple cubic grid.
    // Each z-layer alternates the x-offset pattern ((j+k)%2), so odd rows
    // have one fewer particle in x than even rows when length is barely large
    // enough. Verified by running the actual HCP loop.

    struct Case { std::array<double, 3> length; int expected; };
    const QVector<Case> cases = {
        {{1.5, 1.5, 1.5},   1},
        {{3.5, 1.5, 1.5},   3},
        {{3.5, 3.5, 1.5},   8},
        {{3.5, 3.5, 3.5},  30},
    };

    for (const Case& c : cases) {
        nlohmann::json json;
        json["_id"]      = "pack";
        json["material"] = "mat";
        json["density"]  = 1.0;
        json["dimensions"]["spacing"] = 1;
        json["dimensions"]["length"]  = nlohmann::json::array(
            {c.length[0], c.length[1], c.length[2]});
        json["position"]  = nlohmann::json::array({0.0, 0.0, 0.0});
        json["velocity"]  = nlohmann::json::array({0.0, 0.0, 0.0});

        NonSolidObject obj(json);
        obj.initialize();

        QCOMPARE(static_cast<int>(obj.getParticles().size()), c.expected);
    }
}
