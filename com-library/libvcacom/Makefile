include ../Makefile.in

CFLAGS+= -c -fPIC -g -I../include -I../../mem-sharing-library
CTOOL=gcc
LTOOL=ar
LFLAGS=rcs

default : libvca_com.a

../shared/vca_com_ds.o : ../shared/vca_com_ds.c  ../include/vca_com_ds.h ../include/vca_com.h
	$(CTOOL) $(CFLAGS) $< -o $@ 

vca_com.o : vca_com.c ../include/vca_com.h
	$(CTOOL) $(CFLAGS) $< -o $@ 

libvca_com.a : ../shared/vca_com_ds.o vca_com.o
	$(LTOOL) $(LFLAGS) $@ $^


clean :
	@echo "CLEANING UP "
	@rm -rf *.o *.a ../shared/*.o ../shared/*.a
