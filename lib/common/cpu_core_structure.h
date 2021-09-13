#ifndef CORE_STRUCT
#define CORE_STRUCT

#include "../kvm/vcpu.h"

// Core mapping structure
struct core
{
    int core_id = 0;
    int physical_core_map = 0;
    VCPU* vcpu = nullptr;
};

#endif