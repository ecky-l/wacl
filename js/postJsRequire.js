  delete window.Module;
  
  return {
    onReady: function(callback) {
      if (Module["calledRun"] != undefined && Module["calledRun"]) {
        callback(_Result);
      } else {
        _OnReadyCb = callback;
      }
    }
  };
});

