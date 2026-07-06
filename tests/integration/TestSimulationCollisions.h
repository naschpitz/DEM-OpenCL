#ifndef TESTSIMULATIONCOLLISIONS_H
#define TESTSIMULATIONCOLLISIONS_H

#include <QObject>
#include <nlohmann/json.hpp>

class TestSimulationCollisions : public QObject
{
    Q_OBJECT

  public:
    TestSimulationCollisions();

  private Q_SLOTS:
    void initTestCase();
    void particleDoesNotFallThroughFloor();
    void particleInitiallyFallsTowardsFloor();

  private:
    static nlohmann::json floorSimulationJson(double particleZ);
    static double particleZ(const nlohmann::json& scenery);
    static std::string makeFloorSTL();
};

#endif // TESTSIMULATIONCOLLISIONS_H
