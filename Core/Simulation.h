#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

#ifndef SIMULATION_H
#define SIMULATION_H

#include "nlohmann/json.hpp"
#include "Scenery.h"
#include "Vector3D.h"

#include <CL/opencl.hpp>
#include <QObject>
#include <QThread>
#include <QHostAddress>

typedef struct
{
    cl_float currentTime;
    cl_ulong currentStep;

    cl_float timeStep;
    cl_float totalTime;

    cl_ulong  calcNeighStepsInt;
    cl_double neighDistThresMult;
} SimulationCL;

class Simulation : public QThread
{
    Q_OBJECT

    private:
        QHostAddress interfaceAddress;
        QString interfaceUrl;
        QString id;
        QString instance;

        double currentTime;
        ulong  currentStep;

        double timeStep;
        double totalTime;
        ulong  totalSteps;

        double frameTime;
        double logTime;

        double calcNeighTimeInt;
        ulong  calcNeighStepsInt;

        double neighDistThresMult;

        bool multiGPU;

        double stepsPerSecond;
        ulong et;

        Scenery scenery;

        bool initialized;
        bool paused;
        bool stoped;
        bool primary;

        void initialize();

    public:
        Simulation();
        Simulation(const nlohmann::json& jsonObject);
        ~Simulation();

        SimulationCL getCL() const;

        const QHostAddress& getInterfaceAddress() const;
        void setInterfaceAddress(const QHostAddress& interfaceAddress);

        const QString& getInterfaceUrl() const;
        void setInterfaceUrl(const QString& interfaceUrl);

        const QString& getId()          const;
        const QString& getInstance()    const;
        const double&  getCurrentTime() const;
        const ulong&   getCurrentStep() const;

        const double& getTimeStep()   const;
        const double& getTotalTime()  const;
        const ulong&  getTotalSteps() const;

        const double& getStepsPerSecond() const;
        ulong getEta() const;
        ulong getEt()  const;

        const Scenery& getScenery() const;

        bool isPaused() const;
        bool isStopped() const;
        bool isPrimary() const;

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
