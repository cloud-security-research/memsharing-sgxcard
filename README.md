# Memory Sharing Library for Intel® SGX Card
contact: somnath.chakrabarti@intel.com  or  anjo.lucas.vahldiek-oberwagner@intel.com

# Installation Guide

## Prerequisite
In this version of the library, The memory map caching types are controlled using MTRR and not PAT.
But since PAT takes precedence over MTRR, we have to disable using PAT for the card the CARD OS. 

Here are the steps
1. losetup loop0 <VCA card OS image file path>
2. partprobe /dev/loop0
3. mount /dev/loop0p1 /mnt
4. open /mnt/grub.cfg
5. add the kernel command line parameter "nopat" at the end of kernel command line options
6. save and umount /mnt
7. partx -d /dev/loop0
8. losetup -d loop0
9. vcactl boot 0 0 vcablk0

Ensure can build kernel modules on your host OS

if you are using 3.10.0-514.el7.centos.2.1.13.VCA.x86_64 kernel then you will need the kernel headers to build kernel modules. The correct host kernel source package is 3.10.0-514.el7.centos.2.1.13.VCA.x86_64.tgz. It is now available in syncplicity share

Please extract it under /usr/src/kernels/ and create a softlink from /lib/modules/`uname –r`/build to this newly extracted kernel source folder.

## INSTALLATION

1. download the stack into a base folder on both host and vca node
2. On host go to folder <base_folder>/host-drivers/base-drivers then execute: ./build_and_install.sh 
2. On node go to folder <base_folder>/node-drivers/base-drivers then execute: tar zxf initrd.tgz and then ./build_and_install.sh 
3. reboot system
4. Check dmesg and look for message "g_vca_xdev" 
5. DO NOT PROCEED if step above is not successful
6. if on host then go inside <base_folder>/host-drivers/dma-mapping-driver
6. if on node then go inside <base_folder>/node-drivers/dma-mapping-driver
7. execute:  make clean ; make 
8. execute: find ./ -name "*.ko" -exec insmod {} \;
9. go inside <base_folder>/mem-sharing-library
10. if on host then execute: make clean ; make MODE=HOST_MODE 
10. if on node then execute: make clean ; make MODE=NODE_MODE 
11. Installation of Drivers and Library is Done at this point

## BUILD EXAMPLES

1. if on host then go inside <base_folder>/host-examples
2. if on node then go inside <base_folder>/node-examples
3. execute: make clean ; make


## TESTING

1. on node execute : ./write
2. on host execute : ./read 

1. on node execute : ./read
2. on host execute : ./write 

1. on node execute : ./enqueue
2. on host execute : ./dequeue

1. on node execute : ./dequeue
2. on host execute : ./enqueue

1. on node execute : ./thread_enqueue
2. on host execute : ./thread_dequeue

1. on node execute : ./thread_dequeue
2. on host execute : ./thread_enqueue

NFV POC 

1. NFV POC Host side code base is located inside nfv/host folder. Follow the README to setup host packet capture application
2. NFV POC VCA side code base is located inside nfv/node folder. Follow the README to setup enclave based packet processing on the VCA node


# COMMUNICATION LIBRARY FOR TRANSPARENT MEMORY SHARING AND SOCKET COMMUNICATION

To allow Intel® VCA/SGX card nodes and external entities to communicate transparently the com-library implements a routing host gateway and a library for
node applications and clients. This provides node applications the ability to to use memory sharing for communication when possible and 
regular network sockets when necessary. For more information and examples we refer you to the README in com-library.
