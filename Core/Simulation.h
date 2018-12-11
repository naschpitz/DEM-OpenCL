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
        QString id;

        double currentTime;
        long   currentStep;

        double timeStep;
        double totalTime;
        long   totalSteps;

        double frameTime;
        double logTime;

        double stepsPerSecond;
        long et;

        Scenery scenery;

        bool paused;
        bool stoped;

    public:
        Simulation();
        Simulation(const QJsonObject& jsonObject);

        SimulationCL getCL() const;

        const QString& getId() const;
        const double& getCurrentTime() const;
        const long& getCurrentStep()   const;

        const double& getTimeStep()  const;
        const double& getTotalTime() const;
        const long& getTotalSteps()  const;

        const double& getStepsPerSecond() const;
        long getEta() const;
        long getEt()  const;

        const Scenery& getScenery() const;

        bool isPaused() const;
        bool isStopped() const;

        void pause();
        void stop();

    protected:
        void run();

    public slots:
        void selfDelete();

    signals:
        void newFrame();
        void newLog(QString message = QString());
};

#endif // SIMULATION_H
