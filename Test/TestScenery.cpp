#include "TestScenery.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTest>
#include <math.h>

TestScenery::TestScenery()
{
    QFile sceneryFile("../Scenery.json");
    sceneryFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString sceneryFileString = sceneryFile.readAll();
    sceneryFile.close();

    QJsonDocument sceneryJsonDocument = QJsonDocument::fromJson(sceneryFileString.toUtf8());
    this->sceneryJsonValue = sceneryJsonDocument.object();
}

void TestScenery::constructor()
{
    Scenery scenery(this->sceneryJsonValue);

    QVERIFY(scenery.getMaterialsManager().getMaterials().count() == 3);
    QVERIFY(scenery.getObjectsManager().getNonSolidObjects().count() == 2);
}
