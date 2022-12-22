#include "Simulation.h"
#include "SimulationExtra.h"

#include <iostream>
#include <cstring>

SimulationExtra::SimulationExtra()
{

}

SimulationExtra::SimulationExtra(const Simulation* simulation)
{
    this->simulation = simulation;
}

SimulationExtra::~SimulationExtra()
{

}

SimulationExtraCL SimulationExtra::getCL() const
{
    SimulationExtraCL simulationExtraCL;

    const long& currentStep = this->simulation->getCurrentStep();

    simulationExtraCL.recalculateNeighborhood = (currentStep % 200 == 0);

    return simulationExtraCL;
}
