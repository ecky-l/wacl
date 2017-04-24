TDOMVERSION=0.8.3
TCLLIBVERSION=1.18

BUILDDIR=../jsbuild
LIBRARYDIR=../library

BCFLAGS?=-Oz -s WASM=1

TCLLIBS= {json,ncgi,fileutil,cmdline,uri,html,javascript}

.PHONY: tdom

tdom:
	cd tdom && emmake make -j

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
	
tcllib:
	mkdir -p $(LIBRARYDIR)
	cp -rf tcllib-$(TCLLIBVERSION)/modules/$(TCLLIBS) $(LIBRARYDIR)
	find $(LIBRARYDIR)/$(TCLLIBS) -type f | grep -v tcl$ | xargs rm -f
	
tcllibprep:
	wget -nc http://prdownloads.sourceforge.net/tcllib/tcllib-$(TCLLIBVERSION).tar.gz
	tar -xzf tcllib-$(TCLLIBVERSION).tar.gz
	
