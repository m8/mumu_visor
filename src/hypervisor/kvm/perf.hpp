#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <sys/mman.h>
#include <vector>
#include <cstdio>

__u32 buf_size_shift = 8;
__u32 PERF_FD = 0;

static long perf_event_open(perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags)
{
    int ret;
    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
    return ret;
}

static unsigned perf_mmap_size(int buf_size_shift)
{
    return ((1U << buf_size_shift) + 1) * sysconf(_SC_PAGESIZE);
}


int mumu_perf_init()
{
    struct perf_event_attr pe;
    long long count;
    int fd;

    memset(&pe, 0, sizeof(pe));
    pe.type = PERF_TYPE_HW_CACHE;
    pe.size = sizeof(pe);
    pe.config = (PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);

    pe.exclude_host = 1;   
    pe.exclude_hv = 0; 
    
    fd = perf_event_open(&pe, 0, -1, -1,0);
    if (fd == -1)
    {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        exit(EXIT_FAILURE);
    }

    struct perf_event_mmap_page *mpage;
    mpage = (perf_event_mmap_page *) mmap(NULL,  perf_mmap_size(buf_size_shift), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (mpage == (struct perf_event_mmap_page *)-1L) {
        close(fd);
        exit(EXIT_FAILURE);
    }

    PERF_FD = fd;
    return PERF_FD;
}


void mumu_perf_reset_counters()
{
    ioctl(PERF_FD, PERF_EVENT_IOC_RESET, 0);
    return;
}

void mumu_perf_start_counters()
{
    ioctl(PERF_FD, PERF_EVENT_IOC_ENABLE, 0);
    return;
}

void mumu_perf_stop_counters()
{
    ioctl(PERF_FD, PERF_EVENT_IOC_DISABLE, 0);
    return;
}

long long mumu_perf_read_counters()
{
    long long count;
    read(PERF_FD, &count, sizeof(count));
    return count;
}