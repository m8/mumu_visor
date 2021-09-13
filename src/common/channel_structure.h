#pragma once

#include <stdint.h>
#include <string>
#include <iostream>
#include "./vm_structure.h"

using namespace std;

struct ChannelNode
{
    string name;
    vm_structure* vm;
};

class Channel{
public:
    ChannelNode* source = nullptr;
    vector<ChannelNode*> destinations;
private:
    uint64_t address;
};