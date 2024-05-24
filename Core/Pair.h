#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef PAIR_H
#define PAIR_H

#include <QVector>
#include <QPair>

#include <CL/opencl.hpp>

typedef struct
{
    cl_uint first;
    cl_uint second;
} PairCL;

class Pair
{
    public:
        Pair();

        static QVector<PairCL> getPairsCL(uint nElements);
};

#endif // PAIR_H
