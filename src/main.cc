/*
 * The file was derived from src/main.cc in https://github.com/atom/node-spellchecker
 */

#include "spellcheckWrap.h"

#define DECLARE_NAPI_METHOD(name, func)                          \
{ name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
    return Spellchecker::Init(env, exports);
}

NAPI_MODULE(spellchecker, Init)