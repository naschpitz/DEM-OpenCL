#ifndef TESTOPENCL_H
#define TESTOPENCL_H

#include <QObject>

#include "../Core/OpenCL.h"

class TestOpenCL : public QObject
{
    Q_OBJECT

    public:
        TestOpenCL();

    private Q_SLOTS:
        void test();
        void simulation();
};

#endif // TESTOPENCL_H
