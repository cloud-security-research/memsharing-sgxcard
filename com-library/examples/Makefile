include ../Makefile.in

CFLAGS+=-DCOML_DBG -c -O2 -fPIC -g `pkg-config libzmq --cflags` -I../include -I../../mem-sharing-library
LDFLAGS+=`pkg-config libzmq --libs`
CTOOL=gcc
CCTOOL=g++
LTOOL=ar

default : recvHello sendHello

../libvcacom/libvca_com.a:
	$(MAKE) -C ../libvcacom

recvHello.o: recvHello.c
	$(CTOOL) $(CFLAGS) $< -o $@

recvHello: recvHello.o ../libvcacom/libvca_com.a ../../mem-sharing-library/libvca_mem.a
	$(CCTOOL) $^ $(LDFLAGS) -o $@

sendHello.o: sendHello.c
	$(CTOOL) $(CFLAGS) $< -o $@

sendHello: sendHello.o ../libvcacom/libvca_com.a ../../mem-sharing-library/libvca_mem.a
	$(CCTOOL) $^ $(LDFLAGS) -o $@

clean :
	@echo "CLEANING UP "
	@rm -rf *.o *.a *~ \#* 
	@rm -rf recvHello sendHello
