include Makefile.in

all: include/libcuckoo/README.md
	CFLAGS="$(CFLAGS)" $(MAKE) -C ../mem-sharing-library
	CFLAGS="$(CFLAGS)" $(MAKE) -C libvcacom
	CFLAGS="$(CFLAGS)" $(MAKE) -C host-gateway
	CFLAGS="$(CFLAGS)" $(MAKE) -C examples

include/libcuckoo/README.md:
	git submodule init
	git submodule update

clean:
	$(MAKE) -C ../mem-sharing-library clean
	$(MAKE) -C libvcacom clean
	$(MAKE) -C host-gateway clean
	$(MAKE) -C examples clean
