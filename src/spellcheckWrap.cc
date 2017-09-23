#include "spellcheckWrap.h"
#include <assert.h>

napi_ref Spellchecker::constructor;

Spellchecker::Spellchecker(double value)
    : env_(nullptr), wrapper_(nullptr), spellcheckerImpl_(nullptr) {}

Spellchecker::~Spellchecker() { napi_delete_reference(env_, wrapper_); }

void Spellchecker::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  reinterpret_cast<Spellchecker*>(nativeObject)->~Spellchecker();
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

void Spellchecker::Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor properties[] = {
      DECLARE_NAPI_METHOD("setDictionary", SetDictionary),
      DECLARE_NAPI_METHOD("getAvailableDictionaries", GetAvailableDictionaries),
      DECLARE_NAPI_METHOD("getCorrectionsForMisspelling", GetCorrectionsForMisspelling),
      DECLARE_NAPI_METHOD("isMisspelled", IsMisspelled),
      DECLARE_NAPI_METHOD("checkSpelling", CheckSpelling),
      DECLARE_NAPI_METHOD("add", Add),
      DECLARE_NAPI_METHOD("remove", Remove)
  };

  napi_value cons;
  status =
      napi_define_class(env, "Spellchecker", New, nullptr, 7, properties, &cons);
  assert(status == napi_ok);

  status = napi_create_reference(env, cons, 1, &constructor);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "Spellchecker", cons);
  assert(status == napi_ok);
}

napi_value Spellchecker::New(napi_env env, napi_callback_info info) {
  napi_status status;

  bool is_constructor;
  status = napi_is_construct_call(env, info, &is_constructor);
  assert(status == napi_ok);

  if (is_constructor) {
    // Invoked as constructor: `new Spellchecker(...)`
    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    assert(status == napi_ok);

    double value = 0;

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    assert(status == napi_ok);

    if (valuetype != napi_undefined) {
      status = napi_get_value_double(env, args[0], &value);
      assert(status == napi_ok);
    }

    Spellchecker* obj = new Spellchecker(value);
    obj->spellcheckerImpl_ = SpellcheckerFactory::CreateSpellchecker();

    obj->env_ = env;
    status = napi_wrap(env,
                       jsthis,
                       reinterpret_cast<void*>(obj),
                       Spellchecker::Destructor,
                       nullptr,  // finalize_hint
                       &obj->wrapper_);
    assert(status == napi_ok);

    return jsthis;
  } else {
    // Invoked as plain function `Spellchecker(...)`, turn into construct call.
    size_t argc_ = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr);
    assert(status == napi_ok);

    const size_t argc = 1;
    napi_value argv[argc] = {args[0]};

    napi_value cons;
    status = napi_get_reference_value(env, constructor, &cons);
    assert(status == napi_ok);

    napi_value instance;
    status = napi_new_instance(env, cons, argc, argv, &instance);
    assert(status == napi_ok);

    return instance;
  }
}

napi_value Spellchecker::SetDictionary(napi_env env, napi_callback_info info) {
}
napi_value Spellchecker::GetAvailableDictionaries(napi_env env, napi_callback_info info) {
}
napi_value Spellchecker::GetCorrectionsForMisspelling(napi_env env, napi_callback_info info) {
}

napi_value Spellchecker::IsMisspelled(napi_env env, napi_callback_info info) {
    napi_status status;

    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    assert(status == napi_ok);

    size_t strLength = 1;
    status = napi_get_value_string_utf8(env, args[0], NULL, 0, &strLength);
    assert(status == napi_ok);
    char buf[strLength];
    size_t result;
    status = napi_get_value_string_utf8(env, args[0], buf, strLength + 1, &result);
    assert(status == napi_ok);

    std::string word(buf);
    Spellchecker* obj;
    status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    assert(status == napi_ok);
    bool isMisspelledBool = obj->spellcheckerImpl_->IsMisspelled(word);

    napi_value isMisspelled;
    status = napi_get_boolean(env, isMisspelledBool, &isMisspelled);
    assert(status == napi_ok);

    return isMisspelled;
}

napi_value Spellchecker::CheckSpelling(napi_env env, napi_callback_info info) {
}
napi_value Spellchecker::Add(napi_env env, napi_callback_info info) {
}
napi_value Spellchecker::Remove(napi_env env, napi_callback_info info) {
}
