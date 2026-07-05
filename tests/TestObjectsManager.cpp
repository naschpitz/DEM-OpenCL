#include "TestObjectsManager.h"

#include <QtTest>

TestObjectsManager::TestObjectsManager()
{
    this->objectsManagerJsonValue = nlohmann::json::parse(R"({
        "nonSolidObjects": [
            {
                "_id": "TestNonSolidObject0",
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
            },
            {
                "_id": "TestNonSolidObject1",
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
            }
        ]
    })");
}

void TestObjectsManager::constructor()
{
    ObjectsManager objectsManager(this->objectsManagerJsonValue);

    QVERIFY(objectsManager.getNonSolidObjects().count() == 2);
}
