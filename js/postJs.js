  delete window.Module;
  
  self.wasmTcl = {
    onReady: function(callback) {
      if (Module["calledRun"] != undefined && Module["calledRun"]) {
        callback(_Result);
      } else {
        _OnReadyCb = callback;
      }
    }
  };
  
})(this);
