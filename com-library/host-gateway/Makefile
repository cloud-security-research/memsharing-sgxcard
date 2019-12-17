include ../Makefile.in

CFLAGS+= -c -O2 -fPIC -g `pkg-config libzmq --cflags` -I../include -I../include/libcuckoo/libcuckoo-c -I../include/libcuckoo -I../../mem-sharing-library -I.
LDFLAGS+= -lpthread ../libvcacom/libvca_com.a ../../mem-sharing-library/libvca_mem.a `pkg-config libzmq --libs`
CTOOL=gcc
CCTOOL=g++
LTOOL=ar

default : host-gateway
host-gateway-connection-store.o : host-gateway-connection-store.cc host-host-gateway.h
	$(CCTOOL) -std=c++11 $(CFLAGS) $< -o $@

../shared/vca_com_ds.o: ../shared/vca_com_ds.c ../include/vca_com_ds.h ../include/vca_com.h 
	$(CTOOL) $(CFLAGS) $< -o $@

host-node-gateway.o: host-node-gateway.c host-node-gateway.h 
	$(CTOOL) $(CFLAGS) $< -o $@

host-host-gateway.o: host-host-gateway.c host-host-gateway.h
	$(CTOOL) $(CFLAGS) $< -o $@

host-gateway.o: host-gateway.c host-gateway.h
	$(CTOOL) $(CFLAGS) $< -o $@

host-gateway-msgs.o: host-gateway-msgs.c host-gateway-msgs.h
	$(CTOOL) $(CFLAGS) $< -o $@

host-gateway : host-gateway-connection-store.o ../shared/vca_com_ds.o host-node-gateway.o host-host-gateway.o host-gateway.o host-gateway-msgs.o 
	$(CCTOOL) $^ $(LDFLAGS) -o $@

clean :
	@echo "CLEANING UP "
	@rm -rf *.o *.a *~ \#* ../shared/*.o ../shared/*.a
	@rm -f host-gateway
