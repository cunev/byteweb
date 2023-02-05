import commonjs from "rollup-plugin-commonjs";

export default {
  input: "lib/index.js",
  output: {
    file: "../src/bin.ts",
    format: "es",
    name: "quickjsEval",
  },
  plugins: [commonjs()],
};
