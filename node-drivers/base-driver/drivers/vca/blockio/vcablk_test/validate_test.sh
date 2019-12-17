#!/bin/bash
#
# Intel VCA Software Stack (VCASS)
#
# Copyright(c) 2017 Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2, as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# The full GNU General Public License is included in this distribution in
# the file called "COPYING".
#
# Intel VCA Scripts.
#

DISK_FILE=./disk_file
MOUNT_PATH=mnt
DEV=/dev/vcablk1
DEV_PART=$DEV"p1"
err=0

echo "DEV:       $DEV"
echo "DEV_PART:      $PART"

./test_stop.sh ls

./build.sh

#Load modules
/sbin/insmod -f ./vcablk_bcknd_test.ko 
/sbin/insmod -f ./vcablk_test.ko

rm -fR $DISK_FILE

dd if=/dev/zero of=$DISK_FILE bs=1024 count=120000

./vcablkctrl /dev/vcablk_bcknd_local list
./vcablkctrl /dev/vcablk_bcknd_local open 1 rw $DISK_FILE || err=1
./vcablkctrl /dev/vcablk_bcknd_local list

mkdir $MOUNT_PATH

echo Create partition
echo "n
p
1





w
" | fdisk $DEV

fdisk -l $DEV
mkfs.ext3 $DEV_PART

ls /dev/vcabl*

mount $DEV_PART $MOUNT_PATH || err=2

dd if=/dev/zero of=$MOUNT_PATH/test_file bs=1024 count=30720

df -h | grep $DEV_PART  || err=3

umount $DEV_PART

echo RELOAD FRONTEND
/sbin/rmmod vcablk_test
sleep 1 
/sbin/insmod -f ./vcablk_test.ko
sleep 1

mount $DEV_PART $MOUNT_PATH || err=4
df -h | grep $DEV_PART  || err=5

echo CLEAN
umount $DEV_PART
./vcablkctrl /dev/vcablk_bcknd_local close 1 || err=6
./vcablkctrl /dev/vcablk_bcknd_local list | grep '\[  1\]\[ RW \]' && exit 1
/sbin/rmmod vcablk_test
/sbin/rmmod vcablk_bcknd_test
rm -fR $DISK_FILE

if [ $err != 0 ]; then
	echo TEST FAIL $err
else
	echo TEST PASS $err
fi

exit $err





