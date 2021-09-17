#pragma once

#include <string>
#include <vector>

#include "../hypervisor/kvm/vcpu.hpp"
#include "cpu_core_structure.hpp"

// Defines simple VM structure

struct vm_structure
{
    vm_structure() = default;

    vm_structure(int _id, std::string _name, std::string _binary_path){
        this->id = _id;
        this->name = _name;
        this->binary_path = _binary_path;
    };

    VM* vm;
    vector<core*> cores;

    std::string name        = "";
    std::string binary_path = "";
    
    int id = 0;
};
