#include "TestScenery.h"

#include <fstream>
#include <QTest>

TestScenery::TestScenery()
{
    std::ifstream file("../Scenery.json");
    this->sceneryJsonValue = nlohmann::json::parse(file);
}

void TestScenery::constructor()
{
    Scenery scenery(this->sceneryJsonValue);

    QVERIFY(scenery.getMaterialsManager().getMaterials().count() == 3);
    QVERIFY(scenery.getObjectsManager().getNonSolidObjects().count() == 2);
}
