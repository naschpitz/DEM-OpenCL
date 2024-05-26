#ifndef UTILS_CPP_CL
#define UTILS_CPP_CL

inline void atomic_add_float(volatile global float *addr, float value) {
    volatile global int *i_addr = (volatile global int *)addr;  // Reinterpret float address as integer address
    int old_val, new_val;

    do {
        old_val = *i_addr;  // Load the old bitwise representation
        new_val = as_int(as_float(old_val) + value);  // Add the value and reinterpret as integer
    } while (atomic_cmpxchg(i_addr, old_val, new_val) != old_val);  // CAS loop
}

#endif // UTILS_CPP_CL
