/*
 * The file was derived from src/spellcheckWrap.cc in https://github.com/atom/node-spellchecker
 */

#ifndef TEST_ADDONS_NAPI_6_OBJECT_WRAP_Spellchecker_H_
#define TEST_ADDONS_NAPI_6_OBJECT_WRAP_Spellchecker_H_

#include <node_api.h>
#include "spellchecker.h"

using namespace spellchecker;
class Spellchecker {
 public:
  static napi_value Init(napi_env env, napi_value exports);
  static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

 private:
  explicit Spellchecker(double value_ = 0);
  ~Spellchecker();

  static napi_value New(napi_env env, napi_callback_info info);
  static std::string ParseWord(napi_env env, napi_value str);
  static Spellchecker *Unwrap(napi_env env, napi_callback_info info);
  static napi_value SetDictionary(napi_env env, napi_callback_info info);
  static napi_value GetAvailableDictionaries(napi_env env, napi_callback_info info);
  static napi_value GetCorrectionsForMisspelling(napi_env env, napi_callback_info info);
  static napi_value IsMisspelled(napi_env env, napi_callback_info info);
  static napi_value CheckSpelling(napi_env env, napi_callback_info info);
  static napi_value Add(napi_env env, napi_callback_info info);
  static napi_value Remove(napi_env env, napi_callback_info info);
  static napi_ref constructor;
  napi_env env_;
  napi_ref wrapper_;
  SpellcheckerImplementation *spellcheckerImpl_;
};

#endif  // TEST_ADDONS_NAPI_6_OBJECT_WRAP_MYOBJECT_H_
