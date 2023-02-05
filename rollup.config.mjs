import typescript from "@rollup/plugin-typescript";

export default {
  input: "src/main.ts",
  output: {
    dir: "output",
    format: "esm",
  },
  plugins: [
    typescript({
      sourceMap: true,
      inlineSources: true,
      tsconfig: "./tsconfig.json",
    }),
  ],
};
