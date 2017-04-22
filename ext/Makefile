TDOMVERSION=0.8.3
BUILDDIR=../jsbuild

BCFLAGS?=-Oz -s WASM=1

.PHONY: tdom

tdom:
	cd tdom && emmake make

tdomconfig:
	cd tdom && emconfigure ./configure --disable-load --disable-threads --disable-shared \
		--with-tcl=$(CURDIR)/../jsbuild/lib/ --prefix=$(CURDIR)/$(BUILDDIR)/lib/
	cd tdom && sed -i 's/-O2/$(BCFLAGS)/g' Makefile

tdomprep:
	wget -nc https://github.com/downloads/tDOM/tdom/tDOM-$(TDOMVERSION).tgz
	mkdir -p tdom
	tar -C tdom --strip-components=1 -xf tDOM-$(TDOMVERSION).tgz
	cd tdom && patch --verbose -p1 < ../tdom.patch
	cd tdom && autoconf

tdompatch:
	wget -nc https://github.com/downloads/tDOM/tdom/tDOM-$(TDOMVERSION).tgz
	tar -xzf tDOM-$(TDOMVERSION).tgz
	rm -rf tdom/{autom4te.cache,configure} tDOM-$(TDOMVERSION)/configure 
	echo `diff -ruN tDOM-$(TDOMVERSION) tdom > tdom.patch`
	rm -rf tDOM-$(TDOMVERSION)

tdominstall: tdom
	cd tdom && make install

tdomclean:
	cd tdom && make clean

tdomdistclean:
	cd tdom && make distclean
