#ifndef SIMULATIONEXTRA_H
#define SIMULATIONEXTRA_H

// Forward declaration
class Simulation;

typedef struct
{
    bool recalculateNeighborhood;
} SimulationExtraCL;

class SimulationExtra
{
    private:
        const Simulation* simulation;

    public:
        SimulationExtra();
        SimulationExtra(const Simulation* simulation);
        ~SimulationExtra();

        SimulationExtraCL getCL() const;
};

#endif // SIMULATIONEXTRA_H
