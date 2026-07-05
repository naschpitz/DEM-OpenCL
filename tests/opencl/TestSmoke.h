#ifndef TESTSMOKE_H
#define TESTSMOKE_H

#include <QObject>

#include "KernelTestHarness.h"

class TestSmoke : public QObject
{
    Q_OBJECT

    private:
        KernelTestHarness* harness;

    public:
        TestSmoke();

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();
        void passthrough();
};

#endif // TESTSMOKE_H
