#include "TestObjectsManager.h"

#include <fstream>
#include <QTest>

TestObjectsManager::TestObjectsManager()
{
    std::ifstream file("../ObjectsManager.json");
    this->objectsManagerJsonValue = nlohmann::json::parse(file);
}

void TestObjectsManager::constructor()
{
    ObjectsManager objectsManager(this->objectsManagerJsonValue);

    QVERIFY(objectsManager.getNonSolidObjects().count() == 2);
}
