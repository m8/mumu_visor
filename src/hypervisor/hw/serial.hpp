#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#define PORT 0x2f8
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02

std::string port = "/dev/ttyS10";
std::string buffer = "";

unsigned device_fd;

void open_linux_serial() {
    device_fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    std::cout << "Linux Serial Device Forwarded" << std::endl; 
}

std::string* port_read() {
    char* ch;
    std::string response;
    do
    {
        int n = read(device_fd, ch,1);
        if (n > 0) {
            response.push_back(*ch);
        }
    } while (*ch != '\n');
    return &response;
}

void port_write(char k) {
    string buf;
    buf.push_back(k);
    write(device_fd,buf.c_str(),1);
}   

void port_write_str(std::string k) {
    write(device_fd,k.c_str(),k.size());
}   
