#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

#ifndef ERROR_H
#define ERROR_H

#include <CL/opencl.hpp>
#include <QString>

// Error codes for simulation errors
#define ERROR_NONE 0
#define ERROR_MAX_PARTICLES_TO_PARTICLES 1
#define ERROR_MAX_FACES_TO_PARTICLES 2
#define ERROR_MAX_PARTICLES_TO_FACES 3

typedef struct
{
    cl_uint errorCode;
} ErrorCL;

class Error
{
public:
    static QString getErrorMessage(cl_uint errorCode);
};

#endif // ERROR_H
