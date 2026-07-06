#ifndef TESTMATERIALSMANAGER_H
#define TESTMATERIALSMANAGER_H

#include <QObject>

#include "../MaterialsManager.h"

class TestMaterialsManager : public QObject
{
    Q_OBJECT

  private:
    nlohmann::json jsonArray;

  public:
    TestMaterialsManager();

  private Q_SLOTS:
    void constructor();
    void getCL();
};

#endif // TESTMATERIALSMANAGER_H
