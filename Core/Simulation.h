#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 210
#endif

#ifndef SIMULATION_H
#define SIMULATION_H

#include "nlohmann/json.hpp"
#include "Scenery.h"
#include "Vector3D.h"

#include <CL/cl2.hpp>
#include <QObject>
#include <QThread>
#include <QHostAddress>

typedef struct
{
    cl_float currentTime;
    cl_float timeStep;
    cl_float totalTime;
} SimulationCL;

class Simulation : public QThread
{
    Q_OBJECT

    private:
        QHostAddress serverAddress;
        QString serverUrl;
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
        Simulation(const nlohmann::json& jsonObject);
        ~Simulation();

        SimulationCL getCL() const;

        const QHostAddress& getServerAddress() const;
        void setServerAddress(const QHostAddress& serverAddress);

        const QString& getServerUrl() const;
        void setServerUrl(const QString& serverUrl);

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
