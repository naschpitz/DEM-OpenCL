#ifndef VECTOR_CPP_CL
#define VECTOR_CPP_CL

float4 vector_getUnitary(float4 vector)
{
    float vectorLength = length(vector);

    if(isequal(vectorLength, (float)0.0))
        return (float4)0;

    float4 unitary = vector / vectorLength;

    return unitary;
}

#endif // VECTOR_CPP_CL
