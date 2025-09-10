#include "../quickjs/quickjs.h"
#include <stdbool.h>
#include <string.h>

char *emscripten_run_script_string(const char *script);

static JSRuntime *global_runtime = NULL;
static JSContext *global_ctx = NULL;
static bool context_initialized = false;

static JSValue js_host(JSContext *ctx, JSValueConst jsThis, int argc,
                       JSValueConst *argv) {
  const char *str = JS_ToCString(ctx, argv[0]);
  const char *res = emscripten_run_script_string(str);
  JS_FreeCString(ctx, str);
  return JS_NewString(ctx, res);
}

void registerContext(JSContext *ctx) {
  JSValue global = JS_GetGlobalObject(ctx);
  JSValue console = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, global, "host", console);
  JS_SetPropertyStr(ctx, console, "run",
                    JS_NewCFunction(ctx, js_host, "run", 1));
}

static void initialize_context() {
  if (!global_runtime) {
    global_runtime = JS_NewRuntime();
  }
  if (!global_ctx) {
    global_ctx = JS_NewContext(global_runtime);
  }
  if (!context_initialized) {
    registerContext(global_ctx);
    context_initialized = 1;
  }
}

const char *eval(uint8_t *bytes, int byteLength) {
  initialize_context();
  JSContext *ctx = global_ctx;

  JSValue object = JS_ReadObject(ctx, bytes, byteLength, JS_READ_OBJ_BYTECODE);

  JSValue value = JS_EvalFunction(ctx, object);

  if (JS_IsException(value)) {
    JSValue realException = JS_GetException(ctx);
    return JS_ToCString(ctx, realException);
  }

  JS_FreeValue(ctx, value);

  return JS_ToCString(ctx, value);
}

int bytecodelength(const char *str) {
  initialize_context();
  JSContext *ctx = global_ctx;
  JSValue object = JS_Eval(ctx, str, strlen(str), "<evalScript>",
                           JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_COMPILE_ONLY);

  if (JS_IsException(object)) {
    JSValue realException = JS_GetException(ctx);
    return 0;
  }

  size_t byteCodeLength;
  uint8_t *bytes =
      JS_WriteObject(ctx, &byteCodeLength, object, JS_WRITE_OBJ_BYTECODE);

  JS_FreeValue(ctx, object);
  return byteCodeLength;
}

const char *bytecode(const char *str) {
  initialize_context();
  JSContext *ctx = global_ctx;
  JSValue object = JS_Eval(ctx, str, strlen(str), "<evalScript>",
                           JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_COMPILE_ONLY);

  if (JS_IsException(object)) {
    JSValue realException = JS_GetException(ctx);
    return JS_ToCString(ctx, realException);
  }

  size_t byteCodeLength;
  uint8_t *bytes =
      JS_WriteObject(ctx, &byteCodeLength, object, JS_WRITE_OBJ_BYTECODE);

  JS_FreeValue(ctx, object);

  return (const char *)bytes;
}
