#include <tcl.h>
#include <emscripten.h>
#include "wacl.h"

EXTERN int Tdom_Init     _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN int Tdom_SafeInit _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN int Rl_json_Init             (Tcl_Interp *interp);

/*
 * The main interpreter, 
 * initialized at startup and returned by Wacl_GetMainInterp
 */
static Tcl_Interp* mainInterp = NULL;

static int
Wacl_AppInit(Tcl_Interp* interp)
{
    if (Wacl_Init(interp) != TCL_OK)
        printf("Error while initialize Wacl! Package will not be present");
    if (Tdom_Init(interp) != TCL_OK)
        printf("Error while initialize tDOM! Package will not be present");
    if (Rl_json_Init(interp) != TCL_OK)
        printf("Error while initialize rl_json! Package will not be present");
    return 0;
}

static void
EmscriptenMainLoop()
{
    Tcl_DoOneEvent(TCL_DONT_WAIT|TCL_ALL_EVENTS);
}

Tcl_Interp*
Wacl_GetInterp()
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

    Wacl_AppInit(mainInterp);

    emscripten_set_main_loop(EmscriptenMainLoop,0,0);

    return 0;
}

