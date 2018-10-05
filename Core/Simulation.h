#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Scenery.h"
#include "Vector3D.h"

#include <CL/cl2.hpp>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QObject>
#include <QThread>

typedef struct
{
    cl_double currentTime;
    cl_double timeStep;
    cl_double totalTime;
} SimulationCL;

class Simulation : public QThread
{
    Q_OBJECT

    private:
        QString _id;

        double currentTime;
        long   currentStep;
        double timeStep;
        double totalTime;
        long   totalSteps;
        double logTime;

        Scenery scenery;

        bool paused;
        bool stoped;

    public:
        Simulation();
        Simulation(const QJsonValue& jsonValue);

        const QString& getId() const;

        void addFrame();
        QJsonObject getJson() const;
        void writeLog() const;
        SimulationCL getCL() const;

        const double& getCurrentTime() const;
        const Scenery& getScenery() const;
        const double& getTimeStep() const;
        const double& getTotalTime() const;

        bool isPaused() const;
        bool isStoped() const;

        void pause();
        void stop();

    protected:
        void run();

    signals:
        void newFrame(QJsonDocument frame);
};

#endif // SIMULATION_H
