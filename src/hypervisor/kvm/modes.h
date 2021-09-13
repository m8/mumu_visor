#pragma once

#include "kvm.h"
#include "vcpu.h"
#include "vm.h"
#include <iostream>
#include <string.h>
#include <stdint.h>

/* CR0 bits */
#define CR0_PE 1u
#define CR0_MP (1U << 1)
#define CR0_EM (1U << 2)
#define CR0_TS (1U << 3)
#define CR0_ET (1U << 4)
#define CR0_NE (1U << 5)
#define CR0_WP (1U << 16)
#define CR0_AM (1U << 18)
#define CR0_NW (1U << 29)
#define CR0_CD (1U << 30)
#define CR0_PG (1U << 31)

/* CR4 bits */
#define CR4_VME 1
#define CR4_PVI (1U << 1)
#define CR4_TSD (1U << 2)
#define CR4_DE (1U << 3)
#define CR4_PSE (1U << 4)
#define CR4_PAE (1U << 5)
#define CR4_MCE (1U << 6)
#define CR4_PGE (1U << 7)
#define CR4_PCE (1U << 8)
#define CR4_OSFXSR (1U << 8)
#define CR4_OSXMMEXCPT (1U << 10)
#define CR4_UMIP (1U << 11)
#define CR4_VMXE (1U << 13)
#define CR4_SMXE (1U << 14)
#define CR4_FSGSBASE (1U << 16)
#define CR4_PCIDE (1U << 17)
#define CR4_OSXSAVE (1U << 18)
#define CR4_SMEP (1U << 20)
#define CR4_SMAP (1U << 21)

#define EFER_SCE 1
#define EFER_LME (1U << 8)
#define EFER_LMA (1U << 10)
#define EFER_NXE (1U << 11)

/* 32-bit page directory entry bits */
#define PDE32_PRESENT 1
#define PDE32_RW (1U << 1)
#define PDE32_USER (1U << 2)
#define PDE32_PS (1U << 7)

/* 64-bit page * entry bits */
#define PDE64_PRESENT 1
#define PDE64_RW (1U << 1)
#define PDE64_USER (1U << 2)
#define PDE64_ACCESSED (1U << 5)
#define PDE64_DIRTY (1U << 6)
#define PDE64_PS (1U << 7)
#define PDE64_G (1U << 8)

void setup_protected_mode(VCPU *vcpu)
{
    if (ioctl(vcpu->vcpu_fd, KVM_GET_SREGS, &(vcpu->sregs)) < 0)
    {
        perror("KVM_GET_SREGS");
        exit(1);
    }

    vcpu->setup_segment_registers();

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
        .type = 11, /* Code: execute, read, accessed */
        .present = 1,
        .dpl = 0,
        .db = 0,
        .s = 1, /* Code/data */
        .l = 1,
        .g = 1, /* 4KB granularity */
    };

    vcpu->sregs.cs = seg;

    seg.type = 3; /* Data: read/write, accessed */
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
    vcpu->sregs.cr4 = 1 << 5;     // CR4_PAE;
    vcpu->sregs.cr4 |= 0x600;     // CR4_OSFXSR | CR4_OSXMMEXCPT; /* enable SSE instructions */
    vcpu->sregs.cr0 = 0x80050033; // CR0_PE | CR0_MP | CR0_ET | CR0_NE | CR0_WP | CR0_AM | CR0_PG
    vcpu->sregs.efer = 0x500;     // EFER_LME | EFER_LMA
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
