#include <iostream>
#include <list>
#include <fstream>

#include "../src/hypervisor/kvm/kvm.hpp"
#include "../src/hypervisor/kvm/vm.hpp"
#include "../src/hypervisor/kvm/vcpu.hpp"
#include "../src/hypervisor/kvm/modes.h"

#include "../src/hypervisor/loader/mumu_elf.hpp"
#include "../src/xml/xml_parser_port.h"

#include <inttypes.h>

using namespace std;

struct SAMPLING_PORT{
    int port1;
    int port2;
    int name;
    int flip;
    long long cc;
};

struct CHANNELS{
    int names[10];
    int addresses[10];
    int size;
};


int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();
   
    auto xml_parser = new Parser("/home/musa/Desktop/Tübitak/mumu_visor/tests/test_sampling.xml");
    auto parsed_vms = xml_parser->parse_vm_structures();
    auto parsed_channels = xml_parser->parse_channels();

    vector<VM*> vm_list;
    for(auto l: parsed_vms) {
        VM *vm = new VM(_kvm);
        for(auto k = 0; k < l->cores.size(); k++){
            vm->create_vcpu(k,l->cores[k]->core_id,l->cores[k]->physical_core_map);
        }        
        vm->mem_init();
        load_elf(l->binary_path,vm);
        vm->entry_address = get_entry_address(l->binary_path);
        l->vm = vm;
        vm_list.push_back(vm);
    }

    vector<char*> shared_memories;
   
    
    int counter = 0;
    for(auto channel: parsed_channels) {
        kvm_userspace_memory_region mem;
        char* mem1 = (char*)mmap(NULL, RAM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        mem.flags = 0;
        mem.slot = counter+1;
        mem.guest_phys_addr = channel->source->vm->vm->top_address;
        mem.memory_size = 0x200000;
        mem.userspace_addr = (uint64_t)(mem1);

        auto source_p_address = channel->source->vm->vm->top_address;


        auto memory_struct = (SAMPLING_PORT*)mem1;
        memory_struct->port2 = 0;
        memory_struct->flip = 0;
        memory_struct->name = 0;
        memory_struct->port1 = 0;


        auto channel_struct = (CHANNELS*)(channel->source->vm->vm->mem+SHARED_BASE);
        // channel_struct->names[counter] = stoi(channel->source->name.c_str(), NULL,0);
        channel_struct->addresses[counter] = channel->source->vm->vm->top_address;
        channel_struct->names[counter] = stoi(channel->source->name.c_str());
        channel_struct->size = counter + 1;

        channel->source->vm->vm->top_address += 0x200000;

        int ret = ioctl(channel->source->vm->vm->fd, KVM_SET_USER_MEMORY_REGION, &(mem));
        
        if (ret < 0) {
            perror("ERROR KVM_SET_USER_MEMORY_REGION");
            return ret;
        }  
        
        for (auto destination: channel->destinations){
            mem.flags = KVM_MEM_READONLY;
            mem.guest_phys_addr = destination->vm->vm->top_address;

            auto channel_struct = (CHANNELS*)(destination->vm->vm->mem+SHARED_BASE);
            // channel_struct->names[counter] = stoi(channel->source->name.c_str(), NULL,0);
            channel_struct->addresses[counter] = destination->vm->vm->top_address;
            channel_struct->names[counter] = stoi(destination->name.c_str());
            channel_struct->size = counter + 1;

            destination->vm->vm->top_address += 0x200000;

            ret = ioctl(destination->vm->vm->fd, KVM_SET_USER_MEMORY_REGION, &(mem));
            if (ret < 0) {
                perror("ERROR KVM_SET_USER_MEMORY_REGION");
                return ret;
            }  
        }
        cout << "Channel created " << endl;
        counter ++;
        shared_memories.push_back(mem1);
    }

    for (auto vm : vm_list){
        run_long_mode(vm,vm->vcpus[0],vm->entry_address);
    }
    for (auto vm : vm_list){
        vm->run_vm();
    }
    for (auto vm : vm_list){
        vm->stop_vm();
    }
}