#include "Error.h"

QString Error::getErrorMessage(cl_uint errorCode)
{
    switch(errorCode) {
        case ERROR_MAX_PARTICLES_TO_PARTICLES:
            return "MAX_PARTICLES_TO_PARTICLES limit reached! Simulation stopped.";
        case ERROR_MAX_FACES_TO_PARTICLES:
            return "MAX_FACES_TO_PARTICLES limit reached! Simulation stopped.";
        case ERROR_MAX_PARTICLES_TO_FACES:
            return "MAX_PARTICLES_TO_FACES limit reached! Simulation stopped.";
        default:
            return QString("Unknown error code %1! Simulation stopped.").arg(errorCode);
    }
}
