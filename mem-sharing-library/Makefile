CFLAGS= -c -O2 -fPIC -g `pkg-config libzmq --cflags` 
CTOOL=gcc
LTOOL=ar
LFLAGS=rcs

ifneq (,$(wildcard /usr/sbin/vcactl))
$(info Building Mem Sharing Library for Intel Xeon E5 Host)
MODE=HOST_MODE
default : libvca_mem.a 
vca_mem.o : vca_mem.c
	@echo "BUILDING in" $(MODE) $@
	@$(CTOOL) -D$(MODE) $(CFLAGS) $< -o $@
libvca_mem.a : vca_mem.o
	@echo "CREATING LIB in" $(MODE) $@
	@$(LTOOL) $(LFLAGS) $@ $<
else
$(info Building Mem Sharing Library for Intel SGX Card Nodes)
MODE=NODE_MODE
default : libvca_mem.a libtstd_vca_mem.a
vca_mem.o : vca_mem.c
	@echo "BUILDING in" $(MODE) $@  
	@$(CTOOL) -D$(MODE) $(CFLAGS) $< -o $@ 
libvca_mem.a : vca_mem.o
	@echo "CREATING LIB in" $(MODE) $@  
	@$(LTOOL) $(LFLAGS) $@ $<       
tstd_vca_mem.o : vca_mem.c
	@echo "BUILDING in NODE_MODE" $@  
	@$(CTOOL) -DNODE_MODE -DENCLAVE $(CFLAGS) $< -o $@ 
libtstd_vca_mem.a : tstd_vca_mem.o
	@echo "CREATING LIB in NODE_MODE" $@  
	@$(LTOOL) $(LFLAGS) $@ $<       
endif


clean :
	@echo "CLEANING UP "
	@rm -rf *.o *.a
