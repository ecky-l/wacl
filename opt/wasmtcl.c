#include <tcl.h>
#include <string.h>
#include <stdarg.h>
#include <emscripten.h>

static const char* _valTypes[] = {
    "void",
    "array",
    "string", 
    "int", 
    "double", 
    "bool",
    (const char*)NULL
};

enum _valTypesEnum
{
    EMTCL_VOID,
    EMTCL_ARRAY,
    EMTCL_STRING,
    EMTCL_INT,
    EMTCL_DOUBLE,
    EMTCL_BOOL
};


static int 
DomCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	char *argsHelp = "attr|css selector key val";
	if (objc != 5) {
		Tcl_WrongNumArgs(interp, 1, objv, argsHelp);
		return TCL_ERROR;
	}

	const char *action   = Tcl_GetString(objv[1]);
	const char *selector = Tcl_GetString(objv[2]);
	const char *key      = Tcl_GetString(objv[3]);
	const char *val      = Tcl_GetString(objv[4]);

	Tcl_Obj *res;

	if (strcmp(action, "attr") != 0 && strcmp(action, "css") != 0) {
		res = Tcl_NewStringObj("Action must be attr or css", -1);
		Tcl_SetObjResult(interp, res);
		return TCL_ERROR;
	}

	// TODO: always catch errors
	int numChanged = EM_ASM_INT({
		var action   = Pointer_stringify($0);
		    selector = Pointer_stringify($1);
		    key      = Pointer_stringify($2);
		    val      = Pointer_stringify($3);
		var elts = document.querySelectorAll(selector);
		for (var i = 0; i < elts.length; i++) {
			if (action === "attr") {
				elts[i][key] = val;
			} else {
				elts[i].style[key] = val;
			}
		}
		return elts.length;
	}, action, selector, key, val);

	res = Tcl_NewIntObj(numChanged);
	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}

#define EXPAND_FCN_CAST_CALL(R, X, ...) \
R (*fcn)(__VA_ARGS__) = ( R (*)(__VA_ARGS__))fcnPtr;\
R result;\
X

#define EXPAND_FCN_RET_TYPE(X, ...) \
switch (retTypeN)\
  {\
  case EMTCL_VOID:\
  {\
      EXPAND_FCN_CAST_CALL(int,X,__VA_ARGS__)\
      break;\
  }\
  case EMTCL_INT: case EMTCL_BOOL:\
  {\
      EXPAND_FCN_CAST_CALL(int,X,__VA_ARGS__)\
      Tcl_SetObjResult(interp, Tcl_NewIntObj(result));\
      break;\
  }\
  case EMTCL_ARRAY:\
  case EMTCL_STRING:\
    {\
      EXPAND_FCN_CAST_CALL(char*,X,__VA_ARGS__)\
      Tcl_SetObjResult(interp, Tcl_NewStringObj(result, -1));\
      break;\
    }\
  case EMTCL_DOUBLE:\
    {\
      EXPAND_FCN_CAST_CALL(double,X,__VA_ARGS__)\
      Tcl_SetObjResult(interp, Tcl_NewDoubleObj(result));\
      break;\
    }\
  default:\
    break;\
  }

#define EXPAND_FCN_ARG_TYPE(X) \
switch (argTypeN)\
{\
    case EMTCL_INT : case EMTCL_BOOL :\
    {\
      EXPAND_FCN_RET_TYPE(\
        int val;\
        Tcl_GetIntFromObj(interp, objv[4], &val);\
        X\
        , int\
      )\
      break;\
    }\
    case EMTCL_DOUBLE:\
    {\
      EXPAND_FCN_RET_TYPE(\
        double val;\
        Tcl_GetDoubleFromObj(interp, objv[4], &val);\
        X\
        , double\
      )\
      break;\
    }\
    case EMTCL_ARRAY: case EMTCL_STRING: default:\
    {\
      EXPAND_FCN_RET_TYPE(\
        const char* val = Tcl_GetString(objv[4]);\
        X\
        ,const char*\
      )\
      break;\
    }\
  }


static int 
JsCallCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) 
{
    int fcnPtr, retTypeN, argTypeN;

    if (objc < 4 || objc > 5)
    {
    Tcl_WrongNumArgs(
                    interp, 
                    1,
                    objv, 
                    "fcnPtr returnType argsTypes ?arg1 arg2 ...?");
    return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[1], &fcnPtr) != TCL_OK)
    {
      Tcl_SetObjResult(interp, Tcl_NewStringObj("first argument must be a function pointer", -1));
      return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objv[2], _valTypes, "return type", TCL_EXACT, &retTypeN) != TCL_OK)
    return TCL_ERROR;

    if (Tcl_GetIndexFromObj(interp, objv[3], _valTypes, "argument type", TCL_EXACT, &argTypeN) != TCL_OK)
    return TCL_ERROR;

    if (argTypeN != EMTCL_VOID && objc != 5)
    {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("for void argument type there must be no argument", -1));
        return TCL_ERROR;
    }
    
    if (argTypeN == EMTCL_VOID)
    {
        EXPAND_FCN_RET_TYPE( result = fcn(); )
    }
    else
    {
        EXPAND_FCN_ARG_TYPE( result = fcn(val); )
    }
    
    return TCL_OK;
}


static void
WasmtclDeleteNamespace(ClientData clientData)
{
}

int
Wasmtcl_Init(Tcl_Interp* interp)
{
    Tcl_Namespace* wasmtclNs = NULL;
    
    /* commands initialization */
    wasmtclNs = Tcl_CreateNamespace(interp, "::wasmtcl", NULL, WasmtclDeleteNamespace);
    Tcl_CreateObjCommand(interp, 
                         "::wasmtcl::dom", 
                         DomCmd, 
                         (ClientData) NULL, 
                         (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, 
                         "::wasmtcl::jscall", 
                         JsCallCmd, 
                         (ClientData) NULL, 
                         (Tcl_CmdDeleteProc *) NULL);

    Tcl_Export(interp, wasmtclNs, "dom", 0);
    Tcl_Export(interp, wasmtclNs, "jscall", 0);
    Tcl_PkgProvide(interp, "wasmtcl", "1.0.0");
    return TCL_OK;
}

