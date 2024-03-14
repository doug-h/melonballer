
var Module = {
    print: (function() {})(),
    printErr: function(text) {
        if (arguments.length > 1)
            text = Array.prototype.slice.call(arguments).join(" ");
        if (0) {
            dump(text + "\n");
        }
    },
  onRuntimeInitialized: function() {
        var e = document.getElementById('loading-div');
        e.style.visibility = 'hidden';
  },
  canvas: (function() {
      var canvas = document.getElementById("canvas");
      return canvas;
  })(),
};

