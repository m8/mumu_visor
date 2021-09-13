#include <iostream>
#include <list>
#include <fstream>
#include <sys/ioctl.h>

#include "../src/hypervisor/kvm/kvm.h"
#include "../src/hypervisor/kvm/vm.h"
#include "../src/hypervisor/kvm/vcpu.h"
#include "../src/hypervisor/kvm/modes.h"
#include "../src/xml/xml_parser.h"

using namespace std;

void *mumu_mem_thread(void *_mem)
{
    char *mem = (char *)_mem;
    while (1)
    {
        uint64_t memval = 0;
        memcpy(&memval, &mem[1024], 8);
        cout << memval << endl;
        sleep(1);
    }
}

int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    VM *vm = new VM(_kvm);
    VM *vm1 = new VM(_kvm);

    vm->create_vcpu(0, 0, 0);
    vm1->create_vcpu(0, 0, 0);

    vm->mem_init();
    vm1->mem_init();

    kvm_userspace_memory_region mem;

    char *mem1 = (char *)mmap(NULL, RAM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if ((void *)mem1 == MAP_FAILED)
    {
        perror("ERROR MAP_FAILED");
        return -1;
    }

    mem.flags = 0;
    mem.slot = 1;
    mem.guest_phys_addr = RAM_SIZE;
    mem.memory_size = RAM_SIZE;
    mem.userspace_addr = (uint64_t)(mem1);

    for (int k = 0; k < 100; k++)
    {
        mem1[k] = '\0';
    }

    int ret = ioctl(vm->fd, KVM_SET_USER_MEMORY_REGION, &(mem));
    int ret2 = ioctl(vm1->fd, KVM_SET_USER_MEMORY_REGION, &(mem));

    if (ret < 0)
    {
        perror("ERROR KVM_SET_USER_MEMORY_REGION");
        return ret;
    }

    if (ret2 < 0)
    {
        perror("ERROR KVM_SET_USER_MEMORY_REGION");
        return ret;
    }

    vm->load_binary("/home/musa/Desktop/Tübitak/mumu_visor/Hello World/readwrite/write.bin");
    vm1->load_binary("/home/musa/Desktop/Tübitak/mumu_visor/Hello World/readwrite/read.bin");

    vm->top_address += 0x200000;
    vm1->top_address += 0x200000;
    run_long_mode(vm, vm->vcpus[0]);
    run_long_mode(vm1, vm1->vcpus[0]);

    vm->run_vm();
    vm1->run_vm();
    pthread_t th1;
    pthread_create(&th1, NULL, mumu_mem_thread, mem1);

    vm->stop_vm();
    vm1->stop_vm();
}