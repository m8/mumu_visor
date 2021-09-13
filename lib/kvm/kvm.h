#ifndef KVM_H
#define KVM_H 

class KVM{
    public:
        int init();

        unsigned int fd;
        unsigned int version;

    private:
     
};

#include "kvm.cpp"

#endif