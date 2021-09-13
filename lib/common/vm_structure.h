#ifndef VM_STRUCT
#define VM_STRUCT

#include <string>
#include <vector>

#include "../kvm/vcpu.h"
#include "cpu_core_structure.h"

// Defines simple VM structure

struct vm_structure
{
    int id = 0;
    std::string name = "";
    std::string binary_path = "";
    vector<core*> cores;
    VM* vm;

    vm_structure(){};

    vm_structure(int _id, std::string _name, std::string _binary_path){
        this->id = _id;
        this->name = _name;
        this->binary_path = _binary_path;
    };

};

#endif