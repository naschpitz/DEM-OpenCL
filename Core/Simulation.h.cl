#ifndef SIMULATION_H_CL
#define SIMULATION_H_CL

typedef struct
{
    float currentTime;
    ulong currentStep;

    float timeStep;
    float totalTime;

    float frameTime;

    ulong  calcNeighStepsInt;
    double neighDistThresMult;
} Simulation;

#endif // SIMULATION_H_CL
