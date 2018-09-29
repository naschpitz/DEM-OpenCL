#ifndef SIMULATION_H
#define SIMULATION_H

#include "Scenery.h"
#include "Vector3D.h"

#include <QJsonValue>
#include <QJsonObject>

typedef struct
{
    double currentTime;
    double timeStep;
    double totalTime;

    cl_double4 gravity;
} SimulationCL;

class Simulation
{
    private:
        double currentTime;
        long   currentStep;
        double timeStep;
        double totalTime;
        long   totalSteps;
        double logTime;

        QJsonObject log;
        Vector3D gravity;
        Scenery scenery;

    public:
        Simulation(const QJsonValue& jsonValue);

        void addFrame();
        QJsonObject getJson() const;
        void writeLog() const;
        SimulationCL getCL() const;

        const double& getCurrentTime() const;
        const Vector3D& getGravity() const;
        const Scenery& getScenery() const;
        const double& getTimeStep() const;
        const double& getTotalTime() const;

        void run();
};

#endif // SIMULATION_H
