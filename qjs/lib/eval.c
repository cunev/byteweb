#include <string.h>
#include "../quickjs/quickjs.h"

char *emscripten_run_script_string(const char *script);

static JSValue js_host(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
    {
        const char *str = JS_ToCString(ctx, argv[0]);
        const char *res = emscripten_run_script_string(str);
        JS_FreeCString(ctx, str);
        return JS_NewString(ctx,res);
    }

void registerContext(JSContext *ctx){
  JSValue global = JS_GetGlobalObject(ctx);
  JSValue console = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, global, "host", console);
  JS_SetPropertyStr(ctx, console, "run", JS_NewCFunction(ctx, js_host, "run", 1));
}

const char *eval(uint8_t *bytes, int byteLength)
{
  JSRuntime *runtime = JS_NewRuntime();
  JSContext *ctx = JS_NewContext(runtime);

  registerContext(ctx);
  
  JSValue object;
  object = JS_ReadObject(ctx, bytes, byteLength, JS_READ_OBJ_BYTECODE);

  JSValue value = JS_EvalFunction(ctx, object);

  if (JS_IsException(value))
  {
    JSValue realException = JS_GetException(ctx);
    return JS_ToCString(ctx, realException);
  }

  JS_FreeValue(ctx, value);

  return JS_ToCString(ctx, value);
}

int bytecodelength(const char *str)
{
  JSRuntime *runtime = JS_NewRuntime();
  JSContext *ctx = JS_NewContext(runtime);
  JSValue object = JS_Eval(ctx, str, strlen(str), "<evalScript>", JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_COMPILE_ONLY);

  registerContext(ctx);


  if (JS_IsException(object))
  {
    JSValue realException = JS_GetException(ctx);
    return 0;
  }

  size_t byteCodeLength;
  uint8_t *bytes = JS_WriteObject(ctx, &byteCodeLength, object, JS_WRITE_OBJ_BYTECODE);

  JS_FreeValue(ctx, object);
  return byteCodeLength;
}

const char *bytecode(const char *str)
{
  JSRuntime *runtime = JS_NewRuntime();
  JSContext *ctx = JS_NewContext(runtime);
  JSValue object = JS_Eval(ctx, str, strlen(str), "<evalScript>", JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_COMPILE_ONLY);

  registerContext(ctx);

  if (JS_IsException(object))
  {
    JSValue realException = JS_GetException(ctx);
    return JS_ToCString(ctx, realException);
  }

  size_t byteCodeLength;
  uint8_t *bytes = JS_WriteObject(ctx, &byteCodeLength, object, JS_WRITE_OBJ_BYTECODE);

  JS_FreeValue(ctx, object);

  return bytes;
}
