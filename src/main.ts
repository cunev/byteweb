import * as byteweb from "./bin"

interface VM {
  bytecode: Bytecode
  eval: Evaluate
}
type Bytecode = (code: string) => Uint8Array
type Evaluate = (bytecode: Uint8Array) => string

export function getInstance(): Promise<VM> {
  return byteweb.getInstance();
}