# Tcl version. Relies on tcl-core$(TCLVERSION) being vailable at sourceforge
TCLVERSION?=8.6.6
TDOMVERSION?=0.8.3
RLJSONVERSION?=0.9.7

INSTALLDIR=jsbuild

WASMLIBS=$(INSTALLDIR)/lib/libtcl8.6.a \
	   $(INSTALLDIR)/lib/tdom$(TDOMVERSION)/libtdom$(TDOMVERSION).a \
	   $(INSTALLDIR)/lib/rl_json$(RLJSONVERSION)/librl_json$(RLJSONVERSION).a

# Optimisation to use for generating bc
BCFLAGS?=-Oz -s WASM=1
#BCFLAGS?=-O0 -g4 -s WASM=1


# post-js happens later to write cwrap code conditional on tcl distro
WASMFLAGS=\
	--pre-js preGeneratedJs.js --post-js js/postJsRequire.js $(BCFLAGS) \
		-s FORCE_FILESYSTEM=1 -s RESERVED_FUNCTION_POINTERS=100 \
			--memory-init-file 0  --llvm-lto 3 --closure 0
	#-s FORCE_ALIGNED_MEMORY=1 -s CLOSURE_COMPILER=1 -s CLOSURE_ANNOTATIONS=1\
	#-s NODE_STDOUT_FLUSH_WORKAROUND=0 -s RUNNING_JS_OPTS=1

WACLEXPORTS=\
	-s EXPORTED_FUNCTIONS="[\
		'_main',\
		'_Wacl_GetInterp',\
		'_Tcl_Eval',\
		'_Tcl_GetStringResult',\
	]"

.PHONY: all wacl.bc extensions waclinstall preGeneratedJs clean distclean tclprep reset install uninstall


all: wacl.js

wacl.js: wacl.bc extensions preGeneratedJs
	emcc $(WASMFLAGS) $(WACLEXPORTS) $(WASMLIBS) -o $@

wacl.bc:
	cd tcl/unix && emmake make -j

waclinstall: wacl.bc
	cd tcl/unix && make install
	
extensions: waclinstall
	cd ext && if [ ! -e tdom/Makefile ] ; then make tdomconfig ; fi && make tdominstall
	cd ext && if [ ! -e rl_json/Makefile ] ; then make rljsonconfig ; fi && make rljsoninstall

library:
	mkdir -p library
	cp -r $(INSTALLDIR)/lib/tcl8* library/
	cd ext && if [ ! -e tcllib* ] ; then make tcllibprep ; fi && make tcllib
	
preGeneratedJs: library
	python $(EMSCRIPTEN)/tools/file_packager.py wacl-library.data --preload library@/usr/lib/ | tail -n +5 > library.js
	python $(EMSCRIPTEN)/tools/file_packager.py wacl-custom.data --preload custom@/usr/lib/ | tail -n +5 > custom.js
	cat js/preJsRequire.js library.js custom.js > preGeneratedJs.js
	rm -f {library,custom}.js

waclprep:
	wget -nc http://prdownloads.sourceforge.net/tcl/tcl-core$(TCLVERSION)-src.tar.gz
	mkdir -p tcl
	tar -C tcl --strip-components=1 -xf tcl-core$(TCLVERSION)-src.tar.gz
	cd tcl && patch --verbose -p1 < ../wacl.patch
	cd tcl/unix && autoconf
	cd ext && make tdomprep
	cd ext && make rljsonprep
	cd ext && make tcllibprep

config:
	mkdir -p $(INSTALLDIR)
	cd tcl/unix && emconfigure ./configure --prefix=$(CURDIR)/$(INSTALLDIR) \
		--disable-threads --disable-load --disable-shared
	cd tcl/unix && sed -i 's/-O2//g' Makefile
	cd tcl/unix && sed -i 's/^\(CFLAGS\t.*\)/\1 $(BCFLAGS)/g' Makefile

install:
	mkdir -p www/js/tcl/
	cp wacl.{js,wasm} ecky-l.github.io/wacl/js/tcl/
	cp wacl-{library,custom}.data ecky-l.github.io/wacl/js/tcl/

package: install
	cd www && zip -r ../wacl.zip *
	
clean:
	rm -rf library wacl.js* *.data *.wasm *.js wacl.zip $(INSTALLDIR) 
	cd tcl/unix && make clean
	cd ext && make tdomclean

distclean:
	rm -rf library wacl.js* *.data *wasm *.js wacl.zip $(INSTALLDIR)
	if [ -e tcl/unix/Makefile ] ; then cd tcl/unix && make distclean ; fi
	cd ext && make tdomdistclean
	cd ext && make rljsondistclean

patch:
	wget -nc http://prdownloads.sourceforge.net/tcl/tcl-core$(TCLVERSION)-src.tar.gz
	tar -xzf tcl-core$(TCLVERSION)-src.tar.gz
	rm -rf tcl/unix/{autom4te.cache,configure} tcl$(TCLVERSION)/unix/configure
	echo `diff -ruN tcl$(TCLVERSION) tcl > wacl.patch`
	rm -rf tcl$(TCLVERSION)

reset:
	@read -p "This nukes anything in ./tcl/, are you sure? Type 'YES I am sure' if so: " P && [ "$$P" = "YES I am sure" ]
	rm -rf tcl $(INSTALLDIR) wacl.js* preGeneratedJs.js

