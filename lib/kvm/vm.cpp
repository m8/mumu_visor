#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <linux/kvm.h>
#include <fcntl.h>

#include "vm.h"
#include "kvm.h"

using namespace std;

VM::VM(KVM *_ref) : kvm_ref(_ref)
{

    this->fd = ioctl(_ref->fd, KVM_CREATE_VM, 0);
    if (this->fd < 0)
    {
        perror("ERROR KVM_CREATE_VM");
    }
};

void VM::run_vm()
{
    for (auto k : this->vcpus)
    {
        if (k != nullptr)
        {
            k->run(k->physical_core);
        }
    }
}

void VM::stop_vm()
{
    for (auto k : this->vcpus)
    {
        if (k != nullptr)
        {
            pthread_join(k->vcpu_thread, NULL);
        }
    }
}

// Load binary file to memory
// Should be
void VM::load_binary(const string _file)
{
    int fd = open(_file.c_str(), O_RDONLY);
    int ret = 0;
    char *p = (char *)this->mem;

    while (1)
    {
        ret = read(fd, p, 4096);
        if (ret <= 0)
        {
            break;
        }
        p += ret;
    }
}

int VM::mem_init()
{
    struct kvm_userspace_memory_region mem;

    this->mem = (uint64_t)mmap(NULL, RAM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if ((void *)this->mem == MAP_FAILED)
    {
        perror("ERROR MAP_FAILED");
        return -1;
    }

    mem.flags = 0;
    mem.slot = 0;
    mem.guest_phys_addr = 0;
    mem.memory_size = RAM_SIZE;
    mem.userspace_addr = (uint64_t)this->mem;

    int ret = ioctl(this->fd, KVM_SET_USER_MEMORY_REGION, &(mem));
    if (ret < 0)
    {
        perror("ERROR KVM_SET_USER_MEMORY_REGION");
        return ret;
    }

    printf("vm created \n");
    return ret;
}

VCPU *VM::create_vcpu(int _id, uint8_t _virtual_core, uint8_t _physical_core)
{
    int vcpu_fd = ioctl(this->fd, KVM_CREATE_VCPU, _virtual_core);

    if (vcpu_fd < 0)
    {
        perror("ERROR: KVM_CREATE_VCPU");
        return nullptr;
    }

    // size of vcpu mmap area, in bytes
    int kvm_run_mmap_size = ioctl(this->kvm_ref->fd, KVM_GET_VCPU_MMAP_SIZE, 0);

    if (kvm_run_mmap_size < 0)
    {
        perror("ERROR: KVM_GET_VCPU_MMAP_SIZE");
        return nullptr;
    }

    auto kvm_run = (struct kvm_run *)mmap(NULL, kvm_run_mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0);

    if (kvm_run == MAP_FAILED)
    {
        perror("ERROR: MAP_FAILED");
        return nullptr;
    }

    this->vcpu = new VCPU(vcpu_fd, kvm_run_mmap_size, kvm_run, _id);
    this->vcpu->vcpu_thread_func = (ps)&VCPU::kvm_cpu_thread;
    this->vcpu->virtual_core = _virtual_core;
    this->vcpu->physical_core = _physical_core;

    // this->vcpu_thread_func = kvm_cpu_thread;
    this->vcpus[_physical_core] = (this->vcpu);

    printf("vcpu init success\n");
    return this->vcpu;
}
