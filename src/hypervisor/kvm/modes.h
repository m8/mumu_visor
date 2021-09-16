#pragma once

#include <iostream>
#include <string.h>
#include <stdint.h>

#include "kvm.hpp"
#include "vcpu.hpp"
#include "vm.hpp"
#include "defines.h"

void setup_protected_mode(VCPU *vcpu)
{
    if (ioctl(vcpu->vcpu_fd, KVM_GET_SREGS, &(vcpu->sregs)) < 0)
    {
        perror("KVM_GET_SREGS");
        exit(1);
    }

    vcpu->set_segment_registers();

    if (ioctl(vcpu->vcpu_fd, KVM_SET_SREGS, &(vcpu->sregs)) < 0)
    {
        perror("KVM_SET_SREGS");
        exit(1);
    }

    /* Clear all FLAGS bits, except bit 1 which is always set. */
    vcpu->regs.rflags = 2;
    vcpu->regs.rip = 0;

    if (ioctl(vcpu->vcpu_fd, KVM_SET_REGS, &vcpu->regs) < 0)
    {
        perror("KVM_SET_REGS");
        exit(1);
    }
}

void setup_paged_32bit_mode(VM *vm, VCPU *vcpu)
{
    uint32_t pd_addr = 0x2000;
    uint32_t *pd = (uint32_t *)(vm->mem + pd_addr);

    pd[0] = PDE32_PRESENT | PDE32_RW | PDE32_USER | PDE32_PS;

    vcpu->sregs.cr3 = pd_addr;
    vcpu->sregs.cr4 = 1U << 4;
    vcpu->sregs.cr0 = CR0_PE | CR0_MP | CR0_ET | CR0_NE | CR0_WP | CR0_AM | CR0_PG;
    vcpu->sregs.efer = 0;
}

int run_paged_32bit_mode(VM *vm, VCPU *vcpu)
{

    printf("Testing 32-bit paging\n");

    if (ioctl(vcpu->vcpu_fd, KVM_GET_SREGS, &vcpu->sregs) < 0)
    {
        perror("KVM_GET_SREGS");
        exit(1);
    }

    setup_protected_mode(vcpu);
    setup_paged_32bit_mode(vm, vcpu);

    if (ioctl(vcpu->vcpu_fd, KVM_SET_SREGS, &vcpu->sregs) < 0)
    {
        perror("KVM_SET_SREGS");
        exit(1);
    }

    /* Clear all FLAGS bits, except bit 1 which is always set. */
    vcpu->regs.rflags = 2;
    vcpu->regs.rip = 0;

    if (ioctl(vcpu->vcpu_fd, KVM_SET_REGS, &vcpu->regs) < 0)
    {
        perror("KVM_SET_REGS");
        exit(1);
    }

    return 1;
}

static void setup_64bit_code_segment(VCPU *vcpu)
{
    struct kvm_segment seg = {
        .base = 0,
        .limit = 0xffffffff,
        .selector = 1 << 3,
        .type = 11,
        .present = 1,
        .dpl = 0,
        .db = 0,
        .s = 1,
        .l = 1,
        .g = 1,     };

    vcpu->sregs.cs = seg;

    seg.type = 3;
    seg.selector = 2 << 3;
    vcpu->sregs.ds = vcpu->sregs.es = vcpu->sregs.fs = vcpu->sregs.gs = vcpu->sregs.ss = seg;
}

void setup_long_mode(VM *vm, VCPU *vcpu, uint64_t top_memory)
{
    uint64_t pml4_addr = 0x10000;
    uint64_t *pml4 = (uint64_t *)(vm->mem + pml4_addr);

    uint64_t pdpt_addr = 0x11000;
    uint64_t *pdpt = (uint64_t *)(vm->mem + pdpt_addr);

    uint64_t pd_addr = 0x12000;
    uint64_t *pd = (uint64_t *)(vm->mem + pd_addr);


    pml4[0] = PDE64_PRESENT | PDE32_RW | pdpt_addr; 
    pdpt[0] = PDE64_PRESENT | PDE32_RW | pd_addr;   
    
    pd[0] =   PDE64_PRESENT | PDE32_RW | PDE64_PS;
    pd[1] =   PDE64_PRESENT | PDE32_RW | PDE64_PS | 0x200000;
    pd[2] =   PDE64_PRESENT | PDE32_RW | PDE64_PS | 0x400000;


    // Create shared memory region top of the memory
    for (auto k = 0; k < (top_memory-RAM_SIZE) / 0x200000 ; k++) {
        pd[3+k] = PDE64_PRESENT | PDE32_RW | PDE64_PS | 0x200000*(3+k);
    }

    vcpu->sregs.cr3 = pml4_addr;
    vcpu->sregs.cr4 = 1 << 5;    
    vcpu->sregs.cr4 |= 0x600;    
    vcpu->sregs.cr0 = 0x80050033;
    vcpu->sregs.efer = 0x500;    
}


int run_long_mode(VM *vm, VCPU *vcpu, unsigned starting_point = 0x0)
{

    printf("Testing 64-bit mode\n");

    if (ioctl(vcpu->vcpu_fd, KVM_GET_SREGS, &vcpu->sregs) < 0)
    {
        perror("KVM_GET_SREGS");
        exit(1);
    }

    setup_long_mode(vm, vcpu,vm->top_address);
    setup_64bit_code_segment(vcpu);


    if (ioctl(vcpu->vcpu_fd, KVM_SET_SREGS, &vcpu->sregs) < 0)
    {
        perror("KVM_SET_SREGS");
        exit(1);
    }

    vcpu->regs.rflags = 2;
    vcpu->regs.rip = starting_point;

    vcpu->regs.rsp = 0x600000;

    if (ioctl(vcpu->vcpu_fd, KVM_SET_REGS, &vcpu->regs) < 0)
    {
        perror("KVM_SET_SREGS");
        exit(1);
    }

    return 1;
}
