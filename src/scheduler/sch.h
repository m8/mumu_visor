#ifndef SCH_H
#define SCH_H

#include <list>
#include "../kvm/vm.hpp"
#include "../kvm/defines.h"
#include "signal.h"
#include "time.h"


using namespace std;

void signal_handler2(int sig);

class Scheduler;
list<Scheduler*> schedulers;


class Scheduler
{
public:
    list<VM *> vm_tasks;
    list<VCPU *> vcpus;
    int core_id;

    sem_t consuming_sem;
    VCPU *current_vcpu;

    Scheduler(list<VM *> &vms, int _core_id) : vm_tasks(vms), core_id(_core_id)
    {
        sem_init(&consuming_sem, 0, 0);
        schedulers.push_back(this);
    };

    void start()
    {
        pthread_t scheduler_thread;
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
        sched_setaffinity(scheduler_thread, sizeof(cpuset), &cpuset);
        pthread_create(&(scheduler_thread), NULL, (ps)&Scheduler::start_multiple_vcpus, this);
    }

    void finish()
    {
        for (auto k : vm_tasks)
        {
            pthread_detach(k->vcpu->vcpu_thread);
        }
    }

    void finish_multiple_vcpus()
    {
        for (auto k : vcpus)
        {
            pthread_detach(k->vcpu_thread);
        }
    }

    void start_multiple_vcpus(void *ctx)
    {
        cout << "Scheudler: " << this->core_id << endl;
        // Scheduler* c = (Scheduler*)(ctx);
        for (auto vm : this->vm_tasks)
        {
            for (auto vcpu : vm->vcpus)
            {
                if (vcpu->physical_core == core_id)
                {
                    vcpus.push_back(vcpu);
                }
            }
        }

        for (int l = 0; l < 3; l++)
        {
            for (auto k : this->vcpus)
            {
                current_vcpu = k;
                k->lock = false;
                
                if(this->core_id == 0){
                    signal(SIGALRM, &signal_handler2);
                    alarm(k->ms / 1000);
                }
                
                sem_post(&k->mtx);
                sem_wait(&consuming_sem);
            }
        }
        this->finish_multiple_vcpus();
    }

private:
};

// list<Scheduler*> schedulers;


void signal_handler2(int sig)
{
    for(auto k : schedulers){
        if(k->current_vcpu != nullptr){
            k->current_vcpu->lock = true;
            sem_post(&k->consuming_sem);
        }
        cout << "hebeleel" << endl;
    }
}


#endif 