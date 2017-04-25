wacl
=====

This is a distribution of Tcl and some extensions to Webassembly. The code is 
available under the 3-clause BSD license (see "LICENSE").
It compiles the tcl library and exposes some C api calls. At runtime a Tcl 
interpreter is created and initialized, and the event loop is setup to run via 
emscripten\_set\_main\_loop (see opt/waclAppInit.c). Furthermore there is an
extension package "wacl", which provides commands:

- wacl::dom (with honors to Aidan Hobson's emtcl, where the code comes from)
  * manipulate the browser dom
- wacl::jscall
  * call prehooked javascript functions from Wacl

Currently supported is tcl - core\_8\_6\_6. It isTcl comes with a toy DOM library as a demo. See EXTRAS.

BUILD
-----

To build it, you need the emscripten sdk on your path:

    $ make tclprep # One off prep - tcl-core download, hacks.patch application and
                   # autoconf
    $ make config  # create build directory and run emconfigure tcl/unix/configure
    $ make [all]   # create the library and emtcl.js
    $ make install # copy emtcl.js to ../www/js/

If you want to totally reset all build files in ./tcl/ and start again:

    $ make reset

This removes all changes and untracked files in there, so be careful!

There is a target to recreate the patch, if you changed anything important in tcl/

    $ make patch

It downloads tcl-core (it not already present), extracts it and runs diff between 
it and tcl/. The result is the patch that is applied above via "make tclprep"

EXTRAS
------

Wacl comes with a toy DOM library compiled into it by default
as a demonstration of how one could do interesting things.

It currently is a command of the form `dom attr|css selector key val`. It sets
the appropriate key in either the attribute or style dictionary of each element
returned by the selector. It will return the number of elements modified. Note
that style keys are in their camelcase form (backgroundColor vs background-color)
as the styles are changed in the element style dictionary.

You can see the code in opt/wacl.c. It is compiled along with the other commands
in the "wacl" package and loaded into the default interpreter at startup.


