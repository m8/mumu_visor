#pragma once

#include <string>
#include <vector>

#include "./elf.hpp"
#include "../kvm/vm.hpp"
#include "../../tools/exec.h"

vector<elf_parser::section_t> secs;

// Finds and returns section from name
elf_parser::section_t get_section_from_name (std::string name,vector<elf_parser::section_t> & secs = secs) {
    for(auto k: secs){
        if(k.section_name == name){
            return k;
        }
    }
    return {};
}

// Get entry point of elf folder each load.
long get_entry_address(std::string path){
    std::string command = "readelf -h \""+path+"\" | grep Entry";
    auto cmd = exec(command.c_str());
    auto parsed_address = cmd.substr(cmd.size()-7).substr(0, 7);
    return (strtol(parsed_address.c_str(), NULL, 16 ));
}

// Load elf file to vm memory region
void load_elf(std::string path, VM* vm){
    elf_parser::Elf_parser parser {path};
    auto map = parser.get_memory_map();
    auto segs = parser.get_segments();

    secs = parser.get_sections();

    char *text = (char *)vm->mem + 0x0000;

    for(auto sec: secs){
        memcpy(text + sec.section_offset,map + sec.section_offset, sec.section_size );
    }
}

