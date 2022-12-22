#include "Neighborhood.h"
#include "Simulation.h"

QVector<ParticleNeighborhoodCL> Neighborhood::getParticlesCL(uint particlesCount)
{
    return QVector<ParticleNeighborhoodCL>(particlesCount);
}

QVector<FaceNeighborhoodCL> Neighborhood::getFacesCL(uint facesCount)
{
    return QVector<FaceNeighborhoodCL>(facesCount);
}
