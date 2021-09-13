#pragma once

class KVM {
public:
    int init();
    uint32_t get_fd() const;
    uint32_t get_version() const;

private:
    uint32_t version;
    uint32_t fd;
};

#include "kvm.cpp"
