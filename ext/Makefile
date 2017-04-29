TDOMVERSION=0.8.3
TCLLIBVERSION=1.18
RLJSONVERSION=0.9.7

BUILDDIR=../jsbuild
LIBRARYDIR=../library

BCFLAGS?=-Oz -s WASM=1

TCLLIBS= {json,ncgi,fileutil,cmdline,uri,html,javascript}

.PHONY: tdom

tdom:
	cd tdom && emmake make -j

tdomconfig:
	cd tdom && emconfigure ./configure --disable-load --disable-threads --disable-shared \
		--with-tcl=$(CURDIR)/$(BUILDDIR)/lib/ --prefix=$(CURDIR)/$(BUILDDIR)/lib/
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
	if [ -e tdom/Makefile ] ; then cd tdom && make distclean ; fi
	
tcllib:
	mkdir -p $(LIBRARYDIR)
	cp -rf tcllib-$(TCLLIBVERSION)/modules/$(TCLLIBS) $(LIBRARYDIR)
	find $(LIBRARYDIR)/$(TCLLIBS) -type f | grep -v tcl$$ | xargs rm -f
	
tcllibprep:
	wget -nc http://prdownloads.sourceforge.net/tcllib/tcllib-$(TCLLIBVERSION).tar.gz
	tar -xzf tcllib-$(TCLLIBVERSION).tar.gz

rljsoninstall: rljson
	cd rl_json && make install

rljsonclean:
	cd rl_json && make clean

rljsondistclean:
	if [ -e rl_json/Makefile ] ; then cd rl_json && make distclean ; fi
	
rljson:
	cd rl_json && emmake make -j
	
rljsonconfig:
	cd rl_json && emconfigure ./configure --disable-threads --disable-shared \
		--with-tcl=$(CURDIR)/$(BUILDDIR)/lib/ --prefix=$(CURDIR)/$(BUILDDIR)/ \
		--exec-prefix=$(CURDIR)/$(BUILDDIR)/

rljsonprep:
	wget -nc https://github.com/RubyLane/rl_json/archive/$(RLJSONVERSION).zip
	mkdir rl_json && unzip -o -d /tmp/ $(RLJSONVERSION).zip \
		&& mv /tmp/rl_json-$(RLJSONVERSION)/* rl_json/ && rm -rf /tmp/rl_json-$(RLJSONVERSION)
	cd rl_json && patch --verbose -p1 < ../rl_json.patch
	cd rl_json && autoconf

rljsonpatch:
	wget -nc https://github.com/RubyLane/rl_json/archive/$(RLJSONVERSION).zip
	unzip -o $(RLJSONVERSION).zip
	if [ -e rl_json/Makefile ] ; then cd rl_json && make distclean ; fi
	rm -rf rl_json/{autom4te.cache,configure} rl_json-$(RLJSONVERSION)/{configure,.gitignore}
	echo `diff -ruN rl_json-$(RLJSONVERSION) rl_json > rl_json.patch`
	rm -rf rl_json-$(RLJSONVERSION)
	
reset:
	@read -p "This deletes ./rl_jsontcl, ./tdom, tcllib*, are you sure? Type 'YES I am sure' if so: " P && [ "$$P" = "YES I am sure" ]
	rm -rf rl_json tdom tcllib-1.18
	
