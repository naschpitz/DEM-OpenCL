#ifndef SIMULATIONLOGGER_H
#define SIMULATIONLOGGER_H

#include <QDateTime>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QTimer>

class Simulation;

class SimulationLogger : public QObject
{
    Q_OBJECT

    private:
        Simulation* simulation;
        QTimer* logTimer;
        
        // Logging state variables
        QDateTime lastLogTime;
        ulong previousStep;
        double previousStepsPerSecond;
        ulong previousEt;
        
        // Thread safety
        mutable QMutex mutex;
        bool running;
        
        void updateLogData();
        
    public:
        explicit SimulationLogger(Simulation* simulation, QObject* parent = nullptr);
        ~SimulationLogger();
        
        void start();
        void stop();
        bool isRunning() const;
        
    private slots:
        void onLogTimerTimeout();
        void onSimulationFinished();
        

};

#endif // SIMULATIONLOGGER_H
