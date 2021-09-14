#pragma once

#include <stdint.h>
#include <string>
#include <iostream>

#include "./vm_structure.h"

struct ChannelNode
{
    std::string name;
    vm_structure* vm;
};

class Channel{
public:
    ChannelNode* source = nullptr;
    vector<ChannelNode*> destinations;
private:
    uint64_t address;
};