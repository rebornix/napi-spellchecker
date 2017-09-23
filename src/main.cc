#include "spellcheckWrap.h"

#define DECLARE_NAPI_METHOD(name, func)                          \
{ name, 0, func, 0, 0, 0, napi_default, 0 }

void Init(napi_env env, napi_value exports, napi_value module, void* priv) {
    Spellchecker::Init(env, exports);
}

NAPI_MODULE(spellchecker, Init)