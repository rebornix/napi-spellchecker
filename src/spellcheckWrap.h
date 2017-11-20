#ifndef TEST_ADDONS_NAPI_6_OBJECT_WRAP_Spellchecker_H_
#define TEST_ADDONS_NAPI_6_OBJECT_WRAP_Spellchecker_H_

#include <napi.h>
#include "spellchecker.h"

using namespace spellchecker;
class Spellchecker : public Napi::ObjectWrap<Spellchecker>  {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Spellchecker(const Napi::CallbackInfo& info);
//   static void Destructor(Napi::Env env, void* nativeObject, void* finalize_hint);

 private:
//   ~Spellchecker();

  Napi::Value SetDictionary(const Napi::CallbackInfo& info);
  Napi::Value GetAvailableDictionaries(const Napi::CallbackInfo& info);
  Napi::Value GetCorrectionsForMisspelling(const Napi::CallbackInfo& info);
  Napi::Value IsMisspelled(const Napi::CallbackInfo& info);
  Napi::Value CheckSpelling(const Napi::CallbackInfo& info);
  void Add(const Napi::CallbackInfo& info);
  void Remove(const Napi::CallbackInfo& info);
  static Napi::FunctionReference constructor;
//   Napi::Env env_;
//   napi_ref wrapper_;
  SpellcheckerImplementation *spellcheckerImpl_;
};

#endif  // TEST_ADDONS_NAPI_6_OBJECT_WRAP_MYOBJECT_H_
