var defer = require("./eval.js")();

function getInstance() {
  return defer.then(function (Module) {
    var evalBytecode = Module.cwrap("eval", "string", ["array", "number"]);
    var rawBytecode = Module.cwrap("bytecode", "number", ["string"]);
    var rawBytecodeLength = Module.cwrap("bytecodelength", "number", [
      "string",
    ]);

    return {
      bytecode: (str) => {
        let res = rawBytecode(str);
        return Module.HEAPU8.subarray(res, res + rawBytecodeLength(str));
      },
      eval: (arr)=>evalBytecode(arr,arr.length),
    };
  });
}

function bytecode(code) {
  return getInstance().then(function (ins) {
    return ins.bytecode(code);
  });
}

function fn(fn) {
  var str = fn.trim();
  var begin = str.indexOf("{");
  var end = str.lastIndexOf("}");
  var head = str.substring(0, begin);
  var body = str.substring(begin + 1, end);
  var matched = head.match(/\((.*?)\)/);
  var argsStr = matched[1];
  var args = argsStr.split(",").map(function (item) {
    return item.trim();
  });

  return function () {
    var params = Array.prototype.slice.call(arguments, 1, arguments.length);
    return getInstance().then(function (ins) {
      var func = ins.newFunction(args, body);
      return func.apply(null, params);
    });
  };
}

module.exports = {
  getInstance: getInstance,
  bytez: bytecode,
};
