/*
 * The file was derived from src/spellcheckWrap.cc in https://github.com/atom/node-spellchecker
 */

#include "spellcheckWrap.h"
#include <assert.h>

napi_ref Spellchecker::constructor;

Spellchecker::Spellchecker(double value)
    : env_(nullptr), wrapper_(nullptr), spellcheckerImpl_(nullptr) {}

Spellchecker::~Spellchecker() { napi_delete_reference(env_, wrapper_); }

void Spellchecker::Destructor(napi_env env, void *nativeObject, void * /*finalize_hint*/)
{
    reinterpret_cast<Spellchecker *>(nativeObject)->~Spellchecker();
}

#define DECLARE_NAPI_METHOD(name, func)         \
    {                                           \
        name, 0, func, 0, 0, 0, napi_default, 0 \
    }

napi_value Spellchecker::Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_METHOD("setDictionary", SetDictionary),
        DECLARE_NAPI_METHOD("getAvailableDictionaries", GetAvailableDictionaries),
        DECLARE_NAPI_METHOD("getCorrectionsForMisspelling", GetCorrectionsForMisspelling),
        DECLARE_NAPI_METHOD("isMisspelled", IsMisspelled),
        DECLARE_NAPI_METHOD("checkSpelling", CheckSpelling),
        DECLARE_NAPI_METHOD("add", Add),
        DECLARE_NAPI_METHOD("remove", Remove)};

    napi_value cons;
    status =
        napi_define_class(env, "Spellchecker", -1, New, nullptr, 7, properties, &cons);
    assert(status == napi_ok);

    status = napi_create_reference(env, cons, 1, &constructor);
    assert(status == napi_ok);

    status = napi_set_named_property(env, exports, "Spellchecker", cons);
    assert(status == napi_ok);
	
	return exports;
}

std::string Spellchecker::ParseWord(napi_env env, napi_value str)
{
    napi_status status;

    size_t strLength = 1;
    status = napi_get_value_string_utf8(env, str, NULL, 0, &strLength);
    if (status != napi_ok) {
        napi_throw_error(env, nullptr, "Bad argument");
        return nullptr;
    }
    char buf[strLength];
    size_t result;
    status = napi_get_value_string_utf8(env, str, buf, strLength + 1, &result);
    if (status != napi_ok) {
        napi_throw_error(env, nullptr, "Bad argument");
        return nullptr;
    }

    std::string word(buf);
    return word;
}

Spellchecker *Spellchecker::Unwrap(napi_env env, napi_callback_info info)
{
    napi_status status;

    napi_value jsthis;
    status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
    assert(status == napi_ok);

    Spellchecker *obj;
    status = napi_unwrap(env, jsthis, reinterpret_cast<void **>(&obj));
    assert(status == napi_ok);

    return obj;
}

napi_value Spellchecker::New(napi_env env, napi_callback_info info)
{
    napi_status status;

    // napi_get_new_target replaced napi_is_construct_call https://github.com/nodejs/node/commit/973c12f631c53a9833e0bcd11f5457ebec5269c4
    napi_value new_target;
    status = napi_get_new_target(env, info, &new_target);
    bool is_constructor = (new_target != nullptr);

    if (is_constructor)
    {
        // Invoked as constructor: `new Spellchecker()`
        napi_value jsthis;
        status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
        assert(status == napi_ok);

        Spellchecker *obj = new Spellchecker();
        obj->spellcheckerImpl_ = SpellcheckerFactory::CreateSpellchecker();

        obj->env_ = env;
        status = napi_wrap(env,
                           jsthis,
                           reinterpret_cast<void *>(obj),
                           Spellchecker::Destructor,
                           nullptr, // finalize_hint
                           &obj->wrapper_);
        assert(status == napi_ok);

        return jsthis;
    }
    else
    {
        // Invoked as plain function `Spellchecker()`, turn into construct call.
        napi_value cons;
        status = napi_get_reference_value(env, constructor, &cons);
        assert(status == napi_ok);

        napi_value instance;
        status = napi_new_instance(env, cons, 0, nullptr, &instance);
        assert(status == napi_ok);

        return instance;
    }
}

napi_value Spellchecker::SetDictionary(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 2;
    napi_value args[2];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    assert(status == napi_ok);

    std::string language = Spellchecker::ParseWord(env, args[0]);
    std::string directory = ".";

    if (argc > 1) {
        directory = Spellchecker::ParseWord(env, args[1]);
    }

    Spellchecker *obj = Spellchecker::Unwrap(env, info);
    bool result = obj->spellcheckerImpl_->SetDictionary(language, directory);

    napi_value setDictionaryRet;
    status = napi_get_boolean(env, result, &setDictionaryRet);
    assert(status == napi_ok);

    return setDictionaryRet;
}
napi_value Spellchecker::GetAvailableDictionaries(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    assert(status == napi_ok);

    std::string path = ".";
    if (argc > 0) {
        path = Spellchecker::ParseWord(env, args[0]);
    }

    Spellchecker *obj = Spellchecker::Unwrap(env, info);
    std::vector<std::string> dictionaries = obj->spellcheckerImpl_->GetAvailableDictionaries(path);

    napi_value arr, value;
    status = napi_create_array(env, &arr);

    for (size_t i = 0; i < dictionaries.size(); ++i) {
        std::string dict = dictionaries[i];
        const char *dictStr = dict.c_str();
        status = napi_create_string_utf8(env, dictStr, -1, &value);
        status = napi_set_element(env, arr, i, value);
    }

    return arr;
}

napi_value Spellchecker::GetCorrectionsForMisspelling(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    assert(status == napi_ok);

    std::string word = Spellchecker::ParseWord(env, args[0]);
    Spellchecker *obj = Spellchecker::Unwrap(env, info);

    std::vector<std::string> corrections =
        obj->spellcheckerImpl_->GetCorrectionsForMisspelling(word);

    napi_value arr, value;
    status = napi_create_array(env, &arr);

    for (std::vector<std::string>::size_type i = 0; i != corrections.size(); i++)
    {
        std::string val = corrections[i];
        const char *cstr = val.c_str();
        status = napi_create_string_utf8(env, cstr, -1, &value);
        status = napi_set_element(env, arr, i, value);
    }

    return arr;
}

napi_value Spellchecker::IsMisspelled(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    assert(status == napi_ok);

    std::string word = Spellchecker::ParseWord(env, args[0]);
    Spellchecker *obj = Spellchecker::Unwrap(env, info);
    bool isMisspelledBool = obj->spellcheckerImpl_->IsMisspelled(word);

    napi_value isMisspelled;
    status = napi_get_boolean(env, isMisspelledBool, &isMisspelled);
    assert(status == napi_ok);

    return isMisspelled;
}

napi_value Spellchecker::CheckSpelling(napi_env env, napi_callback_info info)
{
    napi_status status;

    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    if (status != napi_ok || argc < 1) {
        napi_throw_error(env, nullptr, "Bad argument");
        return nullptr;
    }

    size_t strLength = 1;
    status = napi_get_value_string_utf8(env, args[0], NULL, 0, &strLength);
    if (status != napi_ok || argc < 1) {
        napi_throw_error(env, nullptr, "Bad argument");
        return nullptr;
    }

    char16_t buf[strLength];
    size_t result;
    status = napi_get_value_string_utf16(env, args[0], buf, strLength + 1, &result);
    if (status != napi_ok) {
        napi_throw_error(env, nullptr, "Bad argument");
        return nullptr;
    }

    Spellchecker *obj = Spellchecker::Unwrap(env, info);

    std::vector<MisspelledRange> misspelled_ranges = obj->spellcheckerImpl_->CheckSpelling(reinterpret_cast<const uint16_t*>(buf), strLength);
    napi_value arr;
    status = napi_create_array(env, &arr);

    std::vector<MisspelledRange>::const_iterator iter = misspelled_ranges.begin();
    for (; iter != misspelled_ranges.end(); ++iter)
    {
        size_t index = iter - misspelled_ranges.begin();
        uint32_t start = iter->start, end = iter->end;

        napi_value misspelled_range;
        status = napi_create_object(env, &misspelled_range);

        napi_value startValue, endValue;
        status = napi_create_int32(env, start, &startValue);
        assert(status == napi_ok);
        status = napi_create_int32(env, end, &endValue);
        assert(status == napi_ok);

        napi_property_descriptor descriptors[] = {
            {"start", nullptr, 0, 0, 0, startValue, napi_default, 0},
            {"end", nullptr, 0, 0, 0, endValue, napi_default, 0}
        };

        status = napi_define_properties(env, misspelled_range, sizeof(descriptors) / sizeof(descriptors[0]), descriptors);
        assert(status == napi_ok);
        status = napi_set_element(env, arr, index, misspelled_range);
        assert(status == napi_ok);
    }

    return arr;
}

napi_value Spellchecker::Add(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 1) {
        napi_throw_error(env, nullptr, "Bad argument");
        return nullptr;
    }

    std::string word = Spellchecker::ParseWord(env, args[0]);
    Spellchecker *obj = Spellchecker::Unwrap(env, info);

    obj->spellcheckerImpl_->Add(word);
    return nullptr;
}

napi_value Spellchecker::Remove(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok || argc < 1) {
        napi_throw_error(env, nullptr, "Bad argument");
        return nullptr;
    }

    std::string word = Spellchecker::ParseWord(env, args[0]);
    Spellchecker *obj = Spellchecker::Unwrap(env, info);

    obj->spellcheckerImpl_->Remove(word);
    return nullptr;
}
