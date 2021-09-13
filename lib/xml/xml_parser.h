#ifndef XML_PARSER
#define XML_PARSER

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "rapid_xml/rapidxml.hpp"
#include <list>
#include <algorithm>
#include <map>

#include "../common/vm_structure.h"
#include "../common/window_structure.h"
#include "../common/cpu_core_structure.h"
#include "../scheduler/defines.h"

using namespace rapidxml;
using namespace std;

class xml_parser
{
public:
    vector<window_structure *> *windows;

    xml_parser(){};

    vector<vm_structure *> *parse(const string _file_path)
    {
        xml_document<> doc;
        xml_node<> *root_node;

        ifstream theFile(_file_path);
        vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
        buffer.push_back('\0');
        doc.parse<0>(&buffer[0]);
        root_node = doc.first_node("ARINC_653_Module");

        auto partition_node = root_node->first_node("Partitions");
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

        auto schedules_node = root_node->first_node("Schedules");
        windows = new vector<window_structure *>();

        for (auto k = schedules_node->first_node("PartitionTimeWindow"); k; k = k->next_sibling())
        {
            auto window = new window_structure();
            window->offset = stoi(k->first_attribute("Offset")->value());
            window->duration = stoi(k->first_attribute("Duration")->value());
            auto name = k->first_attribute("PartitionNameRef")->value();

            for (auto l : *vm_str_list)
            {
                if (l->name == name)
                {
                    window->is_dummy = false;
                    window->vm_struct = l;
                }
            }
            windows->push_back(window);
        }

        return vm_str_list;
    }

    vector<vector<window_structure *>> generate_core_scheduling_order(vector<window_structure *> *_partition_time_window)
    {
        // TODO should be changed size staticly initialized
        vector<vector<window_structure *>> cores(4, vector<window_structure *>(10));
        std::map<int, int> interval_map;

        for (auto window : *_partition_time_window)
        {
            for (auto counter = 0; counter < CORE_COUNT; counter++)
            {
                interval_map[window->offset]++;

                if (is_cpu_mapped(*window, counter))
                {
                    cores[counter].insert(cores[counter].begin() + (interval_map.size() - 1), window);
                }
                else
                {
                    // Creates empty slots as dummy points
                    if (cores[counter][interval_map.size() - 1] == nullptr)
                    {
                        window_structure *ws = new window_structure();
                        ws->offset = window->offset;
                        ws->duration = window->duration;
                        ws->is_dummy = true;
                        cores[counter].insert(cores[counter].begin() + ((interval_map.size() - 1)), ws);
                    }
                }
            }
        }

        return cores;
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
};

#endif