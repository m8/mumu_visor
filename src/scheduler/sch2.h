#pragma once

#include <list>
#include "signal.h"
#include "time.h"

#include "../kvm/vm.hpp"
#include "../kvm/defines.h"
#include "../common/window_structure.h"
#include "./defines.h"

using namespace std;

void signal_handler2(int sig);

class Scheduler;
list<Scheduler *> schedulers;

static pthread_barrier_t barrier;

class Scheduler
{
public:

    Scheduler(vector<window_structure *> *windows, int _core_id) : vm_tasks(windows), core_id(_core_id)
    {
        sem_init(&consuming_sem, 0, 0);
        schedulers.push_back(this);
        pthread_barrier_init(&barrier, NULL, 4);

    };

    void start()
    {
        // Set affinity of the scheduler
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
        sched_setaffinity(scheduler_thread, sizeof(cpuset), &cpuset);

        pthread_create(&(scheduler_thread), NULL, (ps)&Scheduler::start_vms, this);
    }

    void finish()
    {
        pthread_join(this->scheduler_thread,NULL);   
    }

    void start_vms(void *ctx)
    {

        cout << "Scheduler: " << this->core_id << " started\n process-id: " << getpid()<< endl;
        
        pthread_barrier_wait(&barrier);

        for (auto k : *this->vm_tasks)
        {
            if (k != nullptr)
            {
                if (this->core_id == 0)
                {
                    signal(SIGALRM, &signal_handler2);
                    alarm(k->duration / 1000);
                }

                if (!k->is_dummy)
                {
                    current_vcpu = k->vm_struct->vm->vcpus[this->core_id];
                    current_vcpu->lock = false;
                    sem_post(&current_vcpu->mtx);
                }

                sem_wait(&consuming_sem);
            }
        }
    }

    VCPU* get_current_vcpu(){
        return this->current_vcpu;
    }

    sem_t & get_scheduler_semaphore(){
        return this->consuming_sem;
    }


private:
    sem_t consuming_sem;
    VCPU *current_vcpu = nullptr;
    pthread_t scheduler_thread;
    int core_id = 0;
    vector<window_structure *> *vm_tasks = nullptr;
    list<VCPU *> vcpus;
};

void signal_handler2(int sig)
{
    for (auto k : schedulers)
    {
        if (k->get_current_vcpu() != nullptr)
        {
            k->get_current_vcpu()->lock = true;
            
        }
        sem_post(&k->get_scheduler_semaphore());
    }
    cout << "hebeleel" << endl;
}