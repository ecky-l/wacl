#include <tcl.h>
#include <emscripten.h>
#include "wasmtcl.h"

/*
 * The main interpreter, 
 * initialized at startup and returned by Wasmtcl_GetMainInterp
 */
static Tcl_Interp* mainInterp = NULL;

static int
Wasmtcl_AppInit(Tcl_Interp* interp)
{
    if (Wasmtcl_Init(interp) != TCL_OK)
    {
        printf("Error while initialize Wasmtcl! Package will not be present");
    }
    return 0;
}

static void
EmscriptenMainLoop()
{
    Tcl_DoOneEvent(TCL_DONT_WAIT|TCL_ALL_EVENTS);
}

Tcl_Interp*
Wasmtcl_GetInterp()
{

    return mainInterp;
}

int
main(int argc, char** argv)
{
    mainInterp = Tcl_CreateInterp();
    Tcl_SetVar(mainInterp, "::tcl_library", "/usr/lib/tcl8.6", TCL_GLOBAL_ONLY);
    if (Tcl_Init(mainInterp) != TCL_OK)
    {
        const char* errInfo = Tcl_GetVar(mainInterp, "::errorInfo", TCL_GLOBAL_ONLY);
        printf("Error while calling Tcl_Init: %s", errInfo);
    }

    Wasmtcl_AppInit(mainInterp);

    emscripten_set_main_loop(EmscriptenMainLoop,0,0);

    return 0;
}

