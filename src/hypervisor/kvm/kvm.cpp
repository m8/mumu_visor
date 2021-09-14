#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <semaphore.h>

#include "kvm.hpp"

int KVM::init() {
    
    this->fd = open("/dev/kvm", O_RDWR | O_CLOEXEC);

    if (this->fd < 0)
    {
        perror("CAN NOT OPEN KVM\n");
        return -1;
    }

    this->version = ioctl(this->fd, KVM_GET_API_VERSION, 0);

    if (this->version != 12)
    {
        printf("KVM_GET_API_VERSION %d, expected 12", this->version);
        return -1;
    }

    printf("KVM version %d\n", this->version);
    return 0;
}

uint32_t KVM::get_fd() const {
    return this->fd;
}

uint32_t KVM::get_version() const {
    return this->version;
}


