# wacl

### A Tcl distribution for WebAssembly or Javascript

This is a Tcl distribution for WebAssembly (webassembly.org). It enables Web developers to embed a Tcl interpreter in the browser and integrate Tcl with JavaScript. It enables Tcl developers to use their tools and language of choice to create client side web applications. It enables all developers to reuse a great and (over decades) grown code base of useful packages and scripts, such as Tcllib, to be used in web browsers.

It is an extension of the Emtcl project from Aidan Hobsen, which can be found [here](https://aidanhs.github.io/emtcl/). But Wacl takes things a few steps further: it integrates a fully featured Tcl interpreter into the webpage and adds the following features:

* A main tclsh interpreter and capability to get it via JavaScript
* An event loop to process all Tcl events (timer events, fileevents, custom events)
* Client sockets. The socket -async ... command connects to websocket servers with the binary protocol. The resulting handle can be used to transmit binary data as with normal TCP sockets.
* The Tcl library: modules and packages in the Emscripten virtual filesystem. You can add your own packages!
* Proper initialization via Tcl_Init()
* An extension to call javascript functions from Tcl
* various useful extensions (see below for a list and comments)

The original illustrative dom command has been moved to the "wacl" namespace in the package of same name, which is available right at startup. This package contains also a command ::wacl::jscall to call javascript functions from Tcl which have been registered before via the jswrap() module function.

The code compiles fine with Emscripten 1.37.9 to JavaScript and WebAssembly. The latter is the preferred format: WebAssembly is only half the size of the JavaScript "asm.js" output (~1.4MB vs. 2.9MB) and at least twice as fast! However, that could induce incompatibilities with older browsers, which don't (yet) support WebAssembly.

### Extensions
The following extensions are included in Wacl

* wacl native extension with commands wacl::dom and wacl::jscall
* [tDOM](https://tdom.github.io/) for parsing and creating XML and HTML content
* [json](http://core.tcl.tk/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/json/json.html) and [json::write](http://core.tcl.tk/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/json/json_write.html) from tcllib
* [html](http://core.tcl.tk/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/html/html.html) from tcllib
* [javascript](http://core.tcl.tk/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/javascript/javascript.html) from tcllib
* [ncgi](http://core.tcl.tk/tcllib/doc/tcllib-1-18/embedded/www/tcllib/files/modules/ncgi/ncgi.html) as dependency for html
* [rl_json](https://github.com/RubyLane/rl_json/) A Tcl_Obj type for efficient JSON parsing and generation

More extensions can easily be included and used. C extensions can be compiled with Emscripten (with USE_TCL_STUBS disabled and statically initialized via waclAppInit()) and Tcl extensions can be included in the library virtual filesystem.

But be aware that including extensions is a tradeoff: for the additional functionality you pay with a larger download size. The really useful tDOM extension for instance increases the Wacl distribution by not less than 400kB, which must be downloaded  to the users client when (s)he wants to run a wacl based application, and this can be painful with lower bandwidth. Thus it is better to limit the number of packages to what is necessary rather than to build a batteries included distribution which contains everything.

### Getting excited
You can try it out [here](https://ecky-l.github.io/wacl/). You can download the precompiled version with the index page to play on your own webpage by downloading the precompiled binary from [here](https://ecky-l.github.io/wacl/releases/wacl.zip). Both of these pages require a recent browser with webassembly support:

* Mozilla Firefox >= 52.0
* Google Chrome >= 57.0
* Microsoft Edge (Windows 10 "Creators" update)
* Opera

### Getting started
Wacl will compile on a Unix/Linux environment with the following tools installed:

* the Emscripten SDK. Installation is documented on its [web page](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html)
* make, autoconf
* diff, patch (some patches to the original sources must be applied, this is done mostly automatically)

Windows is not supported, but macOS with the appropriate tools from MacPorts will probably work (not tested by myself).

First step is to checkout this repository.This will checkout the files in the current directory. There is a Makefile with the build steps and a README with instructione. The make procedure does merely download the tcl core sources, apply a small patch and configure & build the interpreter to a webassembly plus accompanying .data + .js files. These files can be deployed to the corresponding web source directories. The Emscripten SDK must be on the PATH (i.e. via source $EMSCRIPTEN/emsdk_set_env.sh). Once wacl is built, it can be used in any browser which supports webassembly, also on Windows.  
The build system can be changed to produce javascript instead of webassembly, by simply removing the -s WASM=1 flag from the BCFLAGS variable in the Makefile. This will generate a larger (~2.8MB), yet minified .js output, which is slower at runtime, but compatible with browsers that don't support webassembly.

To build it, you need the emscripten sdk on your path. Then:

    $ make waclprep  # One off prep - tcl-core download, hacks.patch application and autoconf
    $ make config    # create build directory and run emconfigure tcl/unix/configure
    $ make [all]     # create the library and emtcl.js
    $ make install   # copy emtcl.js to ../www/js/

If you want to totally reset all build files in ./tcl/ and start again:

    $ make reset

This removes all changes and untracked files in there, so be careful!

There is a target to recreate the patch, if you changed anything important in tcl/

    $ make patch

It downloads tcl-core (it not already present), extracts it and runs diff between 
it and tcl/. The result is the patch that is applied above via "make tclprep"


