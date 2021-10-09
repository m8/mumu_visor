# mumu

![](https://img.shields.io/github/commit-activity/m/m8/mumu_visor) ![](https://img.shields.io/tokei/lines/github/m8/mumu_visor) ![](https://img.shields.io/github/license/m8/mumu_visor) ![](https://img.shields.io/github/contributors/m8/mumu_visor)


*mumu* is a hobby hypervisor-ish. mumu can be a good resource to learn virtualization, hypervisors and KVM right now. In the future, who knows, it may become a real hypervisor.

```
          o                 o   
                     o
            o   ______      o   
              _/  (   \_
    _       _/  (       \_  O   
   | \_   _/  (   (    0  \
   |== \_/  (   (          |
   |=== _ (   (   (        |
   |==_/ \_ (   (          |
   |_/     \_ (   (    \__/
             \_ (      _/
               |  |___/
              /__/           
  _ __ ___  _   _ _ __ ___  _   _ 
 | '_ ` _ \| | | | '_ ` _ \| | | |
 | | | | | | |_| | | | | | | |_| |
 |_| |_| |_|\__,_|_| |_| |_|\__,_|                   
```

## features
- can run multiple vms
- static scheduler
- multicore support
- can run 64bit x86 assembly and C programs
- serial console implementation
- static configuration through xml file
- shared memory with sampling port implementation

## hello world

it's very easy to run virtual machines under mumu.

1. Create partition vm.
```
cd examples/vm/hello_world
make
```

2. Run mumu
```
cd examples/mumu
g++ helloworld.cpp -lpthread
./a.out
```

3. Output:
```
KVM version 12
vcpu init success
vm created 
Testing 64-bit mode
core: 0 140737341830912
CPU init with id: 462474 core: 0
id: 462474 core: 0 id: 0 data: H
id: 462474 core: 0 id: 0 data: e
id: 462474 core: 0 id: 0 data: l
id: 462474 core: 0 id: 0 data: l
id: 462474 core: 0 id: 0 data: o
id: 462474 core: 0 id: 0 data:  
id: 462474 core: 0 id: 0 data: W
id: 462474 core: 0 id: 0 data: o
id: 462474 core: 0 id: 0 data: r
id: 462474 core: 0 id: 0 data: l
id: 462474 core: 0 id: 0 data: d
id: 462474 core: 0 id: 0 data: 
halted
```

### resources
- https://github.com/dpw/kvm-hello-world