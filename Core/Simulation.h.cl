#ifndef SIMULATION_H_CL
#define SIMULATION_H_CL

typedef struct
{
    float currentTime;
    ulong currentStep;

    float timeStep;
    float totalTime;
} Simulation;

#endif // SIMULATION_H_CL