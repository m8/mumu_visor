#pragma once

#include <string>
#include <vector>

#include "../kvm/vm.h"
#include "./elf.hpp"
#include "../../tools/exec.h"


using namespace std;
vector<elf_parser::section_t> secs;


elf_parser::section_t get_section_from_name (string name,vector<elf_parser::section_t> & secs = secs) {
    for(auto k: secs){
        if(k.section_name == name){
            return k;
        }
    }
}

// Get entry point of elf folder each load.
long get_entry_address(string path){
    string command = "readelf -h \""+path+"\" | grep Entry";
    auto asd = exec(command.c_str());
    auto q = asd.substr(asd.size()-7).substr(0, 7);
    char *p;
    long n = strtol( q.c_str(), & p, 16 );
    return n;
}


void load_elf(string path, VM* vm){
    elf_parser::Elf_parser parser(path);
    vector<elf_parser::segment_t> segs = parser.get_segments();
    secs = parser.get_sections();
    auto map = parser.get_memory_map();


    char *text = (char *)vm->mem + 0x0000;
    for(auto sec: secs){
        memcpy(text + sec.section_offset,map + sec.section_offset, sec.section_size );
    }
}

