
define('emtcl', function() {
  var _Interp = null;
  var _getInterp = null;
  var _eval = null;
  var _getStringResult = null;
  var _Result = null;

  var _OnReadyCb = function (obj) {};
  
  var _TclException = function(errCode, errInfo) {
    this.errorCode = errCode;
    this.errorInfo = errInfo;
    this.toString = function() {
      return "TclException: " + this.errorCode + " => " + this.errorInfo;
    }
  }
  
  
  var Module = {
    noInitialRun: false,
    noExitRuntime: true,
    filePackagePrefixURL: 'emtcl/',
    print: function(txt) { console.log('emtcl stdout: ' + txt); },
    printErr: function(txt) { console.error('emtcl stderr: ' + txt); },
    preRun: [],
    postRun: function () {
      _getInterp = Module.cwrap('Emtcl_GetInterp', 'number', []);
      _eval = Module.cwrap('Tcl_Eval', 'number', ['number', 'string']);
      _getStringResult = Module.cwrap('Tcl_GetStringResult', 'string', ['number']);
      _Interp = _getInterp();
      
      _Result = {
        Module: Module,
       
        set stdout(fn) {
          Module.print = fn;
        },
        set stderr(fn) {
          Module.printErr = fn;
        },
        get interp() {
          return _Interp;
        },
        
        str2ptr: function (strObj) {
          return Module.allocate(
                      Module.intArrayFromString(strObj), 
                      'i8', 
                      Module.ALLOC_NORMAL);
        },
        
        ptr2str: function (strPtr) {
          return Module.UTF8ToString(strPtr);
        },
       
        jswrap: function(fcn, returnType, argType) {
          var fnPtr = Runtime.addFunction(fcn);
          return "::emtcl::jscall " + fnPtr + " " + returnType + " " + argType;
        },
       
        Eval: function(str) {
          _eval(this.interp, 'catch {' + str + '} ::jsResult');
          var errCode = _getStringResult(this.interp);
          if (errCode != 0) {
            _eval(this.interp, 'set ::errorInfo');
            var errInfo = _getStringResult(this.interp);
            throw new _TclException(errCode, errInfo);
          } else {
            _eval(this.interp, 'set ::jsResult');
            return _getStringResult(this.interp); 
          }
        }
      };

      _OnReadyCb(_Result);
    }
  };
