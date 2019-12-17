#!/usr/bin/bash
find ./drivers -name "*.ko" -exec rm -rf {} \;
find ./drivers -name ".*.cmd" -exec rm -rf {} \;
find ./drivers -name "*.mod.c" -exec rm -rf {} \;
find ./drivers -name "*.mod.o" -exec rm -rf {} \;
find ./drivers -name "*.o" -exec rm -rf {} \;

pushd .
cd drivers
make
popd
umount /mnt > /dev/null 2>&1
mount /dev/vcablk0p1 /mnt
destdir=./initrd/quickbuild/workspace/19699/ValleyVistaBuildScripts/mounted_rootfs/lib/modules/`uname -r`/extra/
echo $destdir
cp ./drivers/plx87xx.ko $destdir/.
cp ./drivers/vca/blockio/*.ko $destdir/vca/blockio/.
cp ./drivers/vca/bus/*.ko $destdir/vca/bus/.
cp ./drivers/vca/plx87xx_dma/plx87xx_dma.ko $destdir/vca/plx87xx_dma/plx87xx_dma.ko
cp ./drivers/vca/vca_csa/vca_csa.ko $destdir/vca/vca_csa/vca_csa.ko 
cp ./drivers/vca/vca_virtio/*.ko $destdir/vca/vca_virtio/.
cp ./drivers/vca/vop/vop.ko $destdir/vca/vop/vop.ko 
pushd .
cd ./initrd
find . | cpio -H newc -o | gzip  > /mnt/vca_initramfs.img
umount /mnt
popd
sync

