#include "spellcheckWrap.h"
#include <napi.h>

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    return Spellchecker::Init(env, exports);
}

NODE_API_MODULE(spellchecker, Init)