#ifndef TESTOBJECTSMANAGER_H
#define TESTOBJECTSMANAGER_H

#include <QObject>

#include "../ObjectsManager.h"

class TestObjectsManager : public QObject
{
    Q_OBJECT

  private:
    nlohmann::json objectsManagerJsonValue;

  public:
    TestObjectsManager();

  private Q_SLOTS:
    void constructor();
};

#endif // TESTOBJECTSMANAGER_H
