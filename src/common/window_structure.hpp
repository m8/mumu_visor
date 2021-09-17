#pragma once

#include "./vm_structure.hpp"

struct window_structure
{   
    window_structure() = default;

    vm_structure* vm_struct = nullptr;
    
    int offset      = 0;
    int duration    = 0;
    bool is_dummy   = true;
};
