#ifndef VECTOR_CL
#define VECTOR_CL

double4 vector_getUnitary(double4 vector)
{
    double vectorLength = length(vector);

    if (isequal(vectorLength, 0.0))
        return (double4)0;

    double4 unitary = vector / vectorLength;

    return unitary;
}

#endif // VECTOR_CL
