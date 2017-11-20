#include "spellcheckWrap.h"

Napi::FunctionReference Spellchecker::constructor;

// Spellchecker::~Spellchecker() { napi_delete_reference(env_, wrapper_); }

// void Spellchecker::Destructor(napi_env env, void *nativeObject, void * /*finalize_hint*/)
// {
//     reinterpret_cast<Spellchecker *>(nativeObject)->~Spellchecker();
// }


Napi::Object Spellchecker::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Spellchecker", {
        InstanceMethod("setDictionary", &Spellchecker::SetDictionary),
        InstanceMethod("getAvailableDictionaries", &Spellchecker::GetAvailableDictionaries),
        InstanceMethod("getCorrectionsForMisspelling", &Spellchecker::GetCorrectionsForMisspelling),
        InstanceMethod("isMisspelled", &Spellchecker::IsMisspelled),
        InstanceMethod("checkSpelling", &Spellchecker::CheckSpelling),
        InstanceMethod("add", &Spellchecker::Add),
        InstanceMethod("remove", &Spellchecker::Remove)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Spellchecker", func);
    return exports;
}

Spellchecker::Spellchecker(const Napi::CallbackInfo& info)
: Napi::ObjectWrap<Spellchecker>(info),  spellcheckerImpl_(nullptr)
{
    this->spellcheckerImpl_ = SpellcheckerFactory::CreateSpellchecker();
}

Napi::Value Spellchecker::SetDictionary(const Napi::CallbackInfo& info)
{
    std::string language = "en";
    if (info.Length() > 0) {
        language = info[0].As<Napi::String>();
    }
    std::string directory = ".";
    if (info.Length() > 1) {
        directory = info[1].As<Napi::String>();
    }

    bool result = this->spellcheckerImpl_->SetDictionary(language, directory);
    return Napi::Boolean::New(info.Env(), result);
}

Napi::Value Spellchecker::GetAvailableDictionaries(const Napi::CallbackInfo& info)
{
    std::string path = ".";
    if (info.Length() > 0) {
        path = info[0].As<Napi::String>();
    }

    std::vector<std::string> dictionaries =
            this->spellcheckerImpl_->GetAvailableDictionaries(path);

    Napi::Array ret =  Napi::Array::New(info.Env(), dictionaries.size());

    for (std::vector<std::string>::size_type i = 0; i != dictionaries.size(); i++)
    {
        std::string val = dictionaries[i];
        Napi::String dic = Napi::String::New(info.Env(), val);
        ret[i] = dic;
    }

    return ret;
}
Napi::Value Spellchecker::GetCorrectionsForMisspelling(const Napi::CallbackInfo& info)
{
    Napi::String word;
    word = info[0].As<Napi::String>();
    std::vector<std::string> corrections =
            this->spellcheckerImpl_->GetCorrectionsForMisspelling(word);

    Napi::Array ret =  Napi::Array::New(info.Env(), corrections.size());

    for (std::vector<std::string>::size_type i = 0; i != corrections.size(); i++)
    {
        std::string val = corrections[i];
        Napi::String correction = Napi::String::New(info.Env(), val);
        ret[i] = correction;
    }

    return ret;
}

Napi::Value Spellchecker::IsMisspelled(const Napi::CallbackInfo& info)
{
    Napi::String word;
    word = info[0].As<Napi::String>();
    bool isMisspelledBool = this->spellcheckerImpl_->IsMisspelled(word);

    return Napi::Boolean::New(info.Env(), isMisspelledBool);
}

Napi::Value Spellchecker::CheckSpelling(const Napi::CallbackInfo& info)
{
    // if (info.Length() > 0) {
    //     throw Napi::Error::New(info.Env(), "Bad argument");
    //     return;
    // }

    // if (!info[0].IsString()) {
    //     throw Napi::Error::New(info.Env(), "Bad argument");
    //     return;
    // }

    Napi::String str = info[0].As<Napi::String>();
    std::u16string u16string = str.Utf16Value();

    const uint16_t * text = reinterpret_cast<const uint16_t *>(u16string.c_str());

    std::vector<MisspelledRange> misspelled_ranges = this->spellcheckerImpl_->CheckSpelling(text, u16string.length());

    Napi::Array result =  Napi::Array::New(info.Env(), misspelled_ranges.size());
    std::vector<MisspelledRange>::const_iterator iter = misspelled_ranges.begin();

    for (; iter != misspelled_ranges.end(); ++iter) {
        size_t index = iter - misspelled_ranges.begin();
        uint32_t start = iter->start, end = iter->end;

        Napi::Object misspelled_range = Napi::Object::New(info.Env());
        misspelled_range["start"] = Napi::Number::New(info.Env(), start);
        misspelled_range["end"] = Napi::Number::New(info.Env(), end);
        result[index] = misspelled_range;
    }

    return result;
}

void Spellchecker::Add(const Napi::CallbackInfo& info)
{
    Napi::String word;
    word = info[0].As<Napi::String>();
    this->spellcheckerImpl_->Add(word);
}

void Spellchecker::Remove(const Napi::CallbackInfo& info)
{
    Napi::String word;
    word = info[0].As<Napi::String>();
    this->spellcheckerImpl_->Remove(word);
}
