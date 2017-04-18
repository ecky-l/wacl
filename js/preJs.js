(function (self) {
  var _Deffer = $.Deferred();
  var root = {
    TCL: function () {
      var Module = {
        noInitialRun: false,
        noExitRuntime: true,
        print: function(txt) { console.log('emtcl stdout: ' + txt); },
        printErr: function(txt) { console.log('emtcl stderr: ' + txt); },
        preRun: [],
        postRun: function () {
            _Deffer.resolve();
        }
      };
