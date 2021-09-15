#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "rapid_xml/rapidxml.hpp"
#include <list>
#include <algorithm>
#include <map>

#include "../common/vm_structure.hpp"
#include "../common/channel_structure.hpp"
#include "../common/window_structure.h"
#include "../common/cpu_core_structure.hpp"
#include "../scheduler/defines.h"


using namespace rapidxml;
using namespace std;

class Parser
{
public:
    vector<window_structure *> *windows;
    vector<vm_structure *> vm_list;

    rapidxml::xml_node<char> *file_root;

    Parser (const string _file_path) {
        xml_document<> doc;
        ifstream theFile(_file_path);
        vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
        buffer.push_back('\0');
        doc.parse<0>(&buffer[0]);
        file_root = doc.first_node("ARINC_653_Module");
    };

    vector<vm_structure *> parse_vm_structures()
    {
        auto partition_node = file_root->first_node("Partitions");
        auto vm_str_list = new vector<vm_structure *>();

        for (auto k = partition_node->first_node("Partition"); k; k = k->next_sibling())
        {
            auto vms = new vm_structure();
            vms->id = stoi(k->first_node("PartitionDefinition")->first_attribute("Identifier")->value());
            vms->name = k->first_node("PartitionDefinition")->first_attribute("Name")->value();
            vms->binary_path = k->first_node("PartitionDefinition")->first_attribute("BinaryPath")->value();

            auto counter = 0;
            for (auto l = k->first_node("CoreAffinity")->first_node("CoreMapping"); l; l = l->next_sibling())
            {
                auto _core = new core();
                _core->core_id = stoi(l->first_attribute("LogicalCoreId")->value());
                _core->physical_core_map = stoi(l->first_attribute("PhysicalCoreId")->value());
                vms->cores.push_back(_core);
            }
            vm_str_list->push_back(vms);
        }
        vm_list = *vm_str_list;
        return *vm_str_list;
    }


    vector<Channel*> parse_channels(){
        
        vector<Channel*> channels;

        auto connection_node = file_root->first_node("Connection_Table");
    
        for (auto k = connection_node->first_node("SamplingChannel"); k; k = k->next_sibling())
        {
            auto channel = new Channel();
            auto source = new ChannelNode();

            source->vm =  get_vm_from_id(stoi(k->first_node("Source")->first_node("StandardPort")->first_attribute("PartitionIdentifier")->value()));
            source->name = k->first_node("Source")->first_node("StandardPort")->first_attribute("PortName")->value();

            channel->source = source;

            for (auto l = k->first_node("Destinations")->first_node("StandardPort"); l; l = l->next_sibling())
            {
                auto destination = new ChannelNode();
                destination->vm = get_vm_from_id(stoi(l->first_attribute("PartitionIdentifier")->value()));
                destination->name = l->first_attribute("PortName")->value();
                channel->destinations.push_back(destination);
            } 
            channels.push_back(channel);
        }
        return channels;
    }

private:

    bool is_cpu_mapped(window_structure &_window, int _core_id)
    {
        for (auto core : _window.vm_struct->cores)
        {
            if (core->physical_core_map == _core_id)
            {
                return true;
            }
        }
        return false;
    }

    vm_structure* get_vm_from_id(unsigned _id) {
        for(auto k: vm_list){
            if(k->id == _id){
                return k;
            }
        }
        return nullptr;
    }
};

