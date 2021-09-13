#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <semaphore.h>

#include "kvm.h"

int KVM::init(){
    
    this->fd = open("/dev/kvm", O_RDWR | O_CLOEXEC);

    if (this->fd < 0)
    {
        perror("CAN NOT OPEN KVM\n");
        return -1;
    }

    this->version = ioctl(this->fd, KVM_GET_API_VERSION, 0);

    if (this->version != 12)
    {
        fprintf(stderr, "KVM_GET_API_VERSION %d, expected 12", this->version);
        return -1;
    }

    int asd = ioctl(this->fd, KVM_CAP_NR_VCPUS, 0);
    printf("%d\n",asd);
    fprintf(stdout, "KVM version %d\n", this->version);
    return 0;
}


