#!/usr/bin/bash
find ./ -name "*.ko" -exec rm -rf {} \;
find ./ -name ".*.cmd" -exec rm -rf {} \;
find ./ -name "*.mod.c" -exec rm -rf {} \;
find ./ -name "*.mod.o" -exec rm -rf {} \;
find ./ -name "*.o" -exec rm -rf {} \;

make
dest_dir=./mods/
rm -rf $dest_dir/*
find ./ -name "*.ko" -exec cp {} $dest_dir/. \;
sync


dest_dir=/usr/lib/modules/`uname -r`/extra/
find $dest_dir/vca/ -name "*.ko" -exec rm -rf {} \;
rm -rf $dest_dir/plx87xx.ko

cp mods/plx87xx.ko $dest_dir/.
cp mods/*.ko $dest_dir/vca/blockio/.
cp mods/*.ko $dest_dir/vca/bus/.
cp mods/plx87xx_dma.ko $dest_dir/vca/plx87xx_dma/plx87xx_dma.ko
cp mods/vca_csa.ko $dest_dir/vca/vca_csa/vca_csa.ko 
cp mods/vca_csm.ko $dest_dir/vca/vca_csm/vca_csm.ko 
cp mods/*.ko $dest_dir/vca/vca_virtio/.
cp mods/vop.ko $dest_dir/vca/vop/vop.ko 
cp mods/vca_mgr_extd.ko $dest_dir/vca/vca_mgr_extd/vca_mgr_extd.ko
cp mods/vca_mgr.ko $dest_dir/vca/vca_mgr/vca_mgr.ko
sync



