#ifndef ERROR_H_CL
#define ERROR_H_CL

// Error codes for simulation errors
#define ERROR_NONE 0
#define ERROR_MAX_PARTICLES_TO_PARTICLES 1
#define ERROR_MAX_FACES_TO_PARTICLES 2
#define ERROR_MAX_PARTICLES_TO_FACES 3

typedef struct
{
    uint errorCode;
} Error;

#endif // ERROR_H_CL
