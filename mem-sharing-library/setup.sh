#!/bin/bash

#TODO: check for SGX mapping driver
#TODO: Check for support of transparent huge pages
#TODO: Check for SSH channel

if [ -f /usr/sbin/vcactl ]; then

NUM_VCA_SOCKETS=6 # Max 3x number of cards 
for card_socket in $(seq 0 $( expr $NUM_VCA_SOCKETS - 1 )) 
do
	echo "Setting up HOST Platform for Mem-Sharing with card socket ($card_socket)"
#	rm -rf /tmp/pipe_$card_socket ; mkfifo /tmp/pipe_$card_socket > /dev/null 2>&1
        echo "0x00 0x00" >| /sys/kernel/sgx5_mapper_$card_socket/map0
        echo "0x00 0x00" >| /sys/kernel/sgx5_mapper_$card_socket/map1
done

else

echo "Setting up Card Socket for Mem-Sharing with host"
#rm -rf /tmp/pipe; mkfifo /tmp/pipe > /dev/null 2>&1
#echo "disable=4" > /proc/mtrr 2>/dev/null
#echo "disable=5" > /proc/mtrr 2>/dev/null
#echo "disable=6" > /proc/mtrr 2>/dev/null
#echo "disable=7" > /proc/mtrr 2>/dev/null
#echo "disable=8" > /proc/mtrr 2>/dev/null
#echo "disable=9" > /proc/mtrr 2>/dev/null
echo "0x00 0x00" >| /sys/kernel/sgx5_mapper_0/map0 
echo "0x00 0x00" >| /sys/kernel/sgx5_mapper_0/map1

fi
