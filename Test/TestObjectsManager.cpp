#include "TestObjectsManager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTest>
#include <math.h>

TestObjectsManager::TestObjectsManager()
{
    QFile objectsManagerFile("../ObjectsManager.json");
    objectsManagerFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString objectsManagerFileString = objectsManagerFile.readAll();
    objectsManagerFile.close();

    QJsonDocument objectsManagerJsonDocument = QJsonDocument::fromJson(objectsManagerFileString.toUtf8());
    this->objectsManagerJsonValue = objectsManagerJsonDocument.object();
}

void TestObjectsManager::constructor()
{
    ObjectsManager objectsManager(this->objectsManagerJsonValue);

    QVERIFY(objectsManager.getNonSolidObjects().count() == 2);
}
