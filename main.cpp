#include <iostream>
#include <list>

#include "lib/kvm/kvm.h"
#include "lib/kvm/vm.h"
#include "lib/kvm/vcpu.h"
#include "lib/scheduler/sch2.h"

#include "lib/xml/xml_parser.h"
#include <fstream>

using namespace std;

int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    xml_parser* parser = new xml_parser();
    auto parsed_vms =  parser->parse("/home/musa/Desktop/Tübitak M/Source/test.xml");
    
    auto vm_list = new list<VM*>();
    for(auto l: *parsed_vms){
        VM *vm = new VM(_kvm);
        for(auto k = 0; k < l->cores.size(); k++){
            vm->create_vcpu(k,l->cores[k]->core_id,l->cores[k]->physical_core_map);
        }        
        vm->mem_init();
        vm->load_binary(l->binary_path);
        l->vm = vm;
        vm_list->push_back(vm);
    }

    auto sched_info = parser->generate_core_scheduling_order(parser->windows);
    cout << "asd" << endl;

    vector<Scheduler*> scheduler_list;
    for(auto k = 0; k<4; k++){
        Scheduler* sch = new Scheduler(&sched_info[k],k);
        scheduler_list.push_back(sch);
    }

    for(auto vm: *vm_list){
        vm->run_vm();
    }

    for(auto k = 0; k<4; k++){
        scheduler_list[k]->start();
    }
    

    for(auto vm: *vm_list){
        vm->stop_vm();
    }

    for(auto sch: scheduler_list){
        sch->finish();
    }
    
    // // pthread_t scheduler_thread;
    // // pthread_create(&(scheduler_thread), NULL, (ps) &Scheduler::start, sch);

    // for(auto l: *vm_list){
    //    l->stop_vm();
    // }
    // VM *vm1 = new VM(_kvm);
    // VCPU* vm1_vcpu1 = vm1->create_vcpu(0,0,0);
    // VCPU* vm1_vcpu2 = vm1->create_vcpu(1,1,0);
    // vm1->mem_init();

    // vm1_vcpu1->set_ms(4000);
    // vm1_vcpu2->set_ms(3000);
    // vm1->load_binary("/home/musa/Desktop/Tübitak M/Source/test.bin");
    // vm1->run_vm(0);

    // // VM *vm2 = new VM(_kvm);
    // // VCPU* vm2_vcpu1 = vm2->create_vcpu(1);
    // // vm2_vcpu1->set_ms(1000);
    // // vm2->load_binary("/home/musa/Desktop/Tübitak M/Source/test2.bin");
    // // vm2->run_vm(0);

    // auto vm_list = new list<VM*>();
    // vm_list->push_back(vm1);
    // // vm_list->push_back(vm2);

    // Scheduler* sch = new Scheduler(*vm_list,0);
    // // Scheduler* sch2 = new Scheduler(*vm_list,1);
    
    // sch->start();
    // // sch2->start();


    // vm1->stop_vm();
    // vm2->stop_vm();
}