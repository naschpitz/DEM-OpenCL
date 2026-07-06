#ifndef TESTMATEIRAL_H
#define TESTMATEIRAL_H

#include <QObject>

#include "../Material.h"

class TestMaterial : public QObject
{
    Q_OBJECT

  private:
    nlohmann::json jsonValue;

  public:
    TestMaterial();

  private Q_SLOTS:
    void constructor();
    void getCL();
    void defaultsWhenOptionalFieldsMissing();
    void invalidForceTypeString();
};

#endif // TESTMATEIRAL_H
