#pragma once

#include "./vm_structure.h"

struct window_structure
{
    int offset = 0;
    int duration = 0;
    bool is_dummy = true;
    vm_structure* vm_struct;
};
