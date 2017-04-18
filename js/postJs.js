      root['Module'] = Module;
    }
  };

  root.emtcl = root.TCL;
  delete root.TCL;

  root.emtcl();
  // Undo pollution of window
  delete window.Module;

  // Init emscripten stuff
  //root.Module.run();
  //root.Module.callMain();

  root.GetInterp = root.Module.cwrap('Emtcl_GetInterp', 'number', []);
  root.Eval = root.Module.cwrap('Tcl_Eval', 'number', [
      'number', // interp pointer
      'string'  // string to eval
  ]);
  root.GetStringResult = root.Module.cwrap('Tcl_GetStringResult', 'string', [
      'number' // interp pointer
  ]);
  
  root.onReady = function(code) {
    _Deffer.done(code);
  }
  
  self.emtcl = root;

})(this);
