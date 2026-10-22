// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/chariter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rbbi.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "brkeng.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dictbe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lstmbe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dictionarydata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubrkimpl.h"

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

LanguageBreakEngine::LanguageBreakEngine() {
}

LanguageBreakEngine::~LanguageBreakEngine() {
}

/*
 ******************************************************************
 */

LanguageBreakFactory::LanguageBreakFactory() {
}

LanguageBreakFactory::~LanguageBreakFactory() {
}

/*
 ******************************************************************
 */

UnhandledEngine::UnhandledEngine(UError_ਕੋਡ &ਸਥਿਤੀ) : fHandled(ਖਾਲੀ_ਸੂਚਕ) {
    (ਖਾਲੀ)ਸਥਿਤੀ;
}

UnhandledEngine::~UnhandledEngine() {
    ਮਿਟਾਓ fHandled;
    fHandled = ਖਾਲੀ_ਸੂਚਕ;
}

UBool
UnhandledEngine::handles(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) ਅਚਲ {
    (ਖਾਲੀ)locale; //ਅਣਵਰਤਿਆ
    ਵਾਪਸ fHandled && fHandled->ਸ਼ਾਮਲ_ਹੈ(c);
}

int32_t
UnhandledEngine::ਲੱਭੋ_Breaks( UText *ਪਾਠ,
                             int32_t ਸ਼ੁਰੂ_Pos,
                             int32_t ਸਿਰਾ_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਸ਼ੁਰੂ_Pos);
    UChar32 c = utext_current32(ਪਾਠ);
    ਜਦੋਂ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) < ਸਿਰਾ_Pos && fHandled->ਸ਼ਾਮਲ_ਹੈ(c)) {
        utext_next32(ਪਾਠ);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(ਪਾਠ);
    }
    ਵਾਪਸ 0;
}

ਖਾਲੀ
UnhandledEngine::handleCharacter(UChar32 c) {
    ਜੇ (fHandled == ਖਾਲੀ_ਸੂਚਕ) {
        fHandled = ਨਵਾਂ Unicode_ਸੈੱਟ();
        ਜੇ (fHandled == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ;
        }
    }
    ਜੇ (!fHandled->ਸ਼ਾਮਲ_ਹੈ(c)) {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_ਮੁੱਲ(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_ਮੁੱਲ(UCHAR_SCRIPT, script, ਸਥਿਤੀ);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_ਕੋਡ &/*ਸਥਿਤੀ*/) {
    fEngines = ਖਾਲੀ_ਸੂਚਕ;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    ਮਿਟਾਓ fEngines;
}

ਖਾਲੀ ICULanguageBreakFactory::ensureEngines(UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਸਥਿਰ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    ਜੇ (fEngines == ਖਾਲੀ_ਸੂਚਕ) {
        Local_ਸੂਚਕ<UStack>  engines(ਨਵਾਂ UStack(uprv_deleteUObject, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ), ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            fEngines = engines.orphan();
        }
    }
}

ਅਚਲ LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) {
    ਅਚਲ LanguageBreakEngine *lbe = ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ  ਸਥਿਤੀ = U_ZERO_ERROR;
    ensureEngines(ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ) ) {
        // Note: no way to return error code to caller.
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਸਥਿਰ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    ਜਦੋਂ (--i >= 0) {
        lbe = static_cast<ਅਚਲ LanguageBreakEngine*>(fEngines->elementAt(i));
        ਜੇ (lbe != ਖਾਲੀ_ਸੂਚਕ && lbe->handles(c, locale)) {
            ਵਾਪਸ lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = ਲੱਦੋ_Engine_For(c, locale);
    ਜੇ (lbe != ਖਾਲੀ_ਸੂਚਕ) {
        fEngines->push((ਖਾਲੀ *)lbe, ਸਥਿਤੀ);
    }
    ਵਾਪਸ U_SUCCESS(ਸਥਿਤੀ) ? lbe : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ LanguageBreakEngine *
ICULanguageBreakFactory::ਲੱਦੋ_Engine_For(UChar32 c, ਅਚਲ ਅੱਖਰ*) {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    UScript_ਕੋਡ ਕੋਡ = uscript_getScript(c, &ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਅਚਲ LanguageBreakEngine *engine = ਖਾਲੀ_ਸੂਚਕ;
        // Try to use LSTM first
        ਅਚਲ LSTMData *ਅੰਕੜੇ = CreateLSTMDataForScript(ਕੋਡ, ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਜੇ (ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
                engine = CreateLSTMBreakEngine(ਕੋਡ, ਅੰਕੜੇ, ਸਥਿਤੀ);
                ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && engine != ਖਾਲੀ_ਸੂਚਕ) {
                    ਵਾਪਸ engine;
                }
                ਜੇ (engine != ਖਾਲੀ_ਸੂਚਕ) {
                    ਮਿਟਾਓ engine;
                    engine = ਖਾਲੀ_ਸੂਚਕ;
                } ਨਹੀਂਤਾਂ {
                    DeleteLSTMData(ਅੰਕੜੇ);
                }
            }
        }
        ਸਥਿਤੀ = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = ਲੱਦੋ_Dictionary_Matcher_For(ਕੋਡ);
        ਜੇ (m != ਖਾਲੀ_ਸੂਚਕ) {
            ਚੋਣ(ਕੋਡ) {
            ਹਾਲਤ USCRIPT_THAI:
                engine = ਨਵਾਂ ThaiBreakEngine(m, ਸਥਿਤੀ);
                ਰੋਕੋ;
            ਹਾਲਤ USCRIPT_LAO:
                engine = ਨਵਾਂ LaoBreakEngine(m, ਸਥਿਤੀ);
                ਰੋਕੋ;
            ਹਾਲਤ USCRIPT_MYANMAR:
                engine = ਨਵਾਂ BurmeseBreakEngine(m, ਸਥਿਤੀ);
                ਰੋਕੋ;
            ਹਾਲਤ USCRIPT_KHMER:
                engine = ਨਵਾਂ KhmerBreakEngine(m, ਸਥਿਤੀ);
                ਰੋਕੋ;

#ਜੇ !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            ਹਾਲਤ USCRIPT_HANGUL:
                engine = ਨਵਾਂ CjkBreakEngine(m, kKorean, ਸਥਿਤੀ);
                ਰੋਕੋ;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            ਹਾਲਤ USCRIPT_HIRAGANA:
            ਹਾਲਤ USCRIPT_KATAKANA:
            ਹਾਲਤ USCRIPT_HAN:
                engine = ਨਵਾਂ CjkBreakEngine(m, kChineseJapanese, ਸਥਿਤੀ);
                ਰੋਕੋ;
#ਜੇ 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            ਹਾਲਤ USCRIPT_COMMON:
            {
                UBlock_ਕੋਡ ਖੰਡ = ublock_get_ਕੋਡ(ਕੋਡ);
                ਜੇ (ਖੰਡ == UBLOCK_HIRAGANA || ਖੰਡ == UBLOCK_KATAKANA)
                   engine = ਨਵਾਂ CjkBreakEngine(dict, kChineseJapanese, ਸਥਿਤੀ);
                ਰੋਕੋ;
            }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

            ਮੂਲ:
                ਰੋਕੋ;
            }
            ਜੇ (engine == ਖਾਲੀ_ਸੂਚਕ) {
                ਮਿਟਾਓ m;
            }
            ਨਹੀਂਤਾਂ ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਮਿਟਾਓ engine;
                engine = ਖਾਲੀ_ਸੂਚਕ;
            }
            ਵਾਪਸ engine;
        }
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

DictionaryMatcher *
ICULanguageBreakFactory::ਲੱਦੋ_Dictionary_Matcher_For(UScript_ਕੋਡ script) { 
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &ਸਥਿਤੀ);
    b = ures_get_By_ਕੁੰਜੀ_With_Fallback(b, "dictionaries", b, &ਸਥਿਤੀ);
    int32_t dictnlength = 0;
    ਅਚਲ char16_t *dictfname =
        ures_get_String_By_ਕੁੰਜੀ_With_Fallback(b, uscript_getShortName(script), &dictnlength, &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ures_close(b);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    CharString dictnbuf;
    CharString ext;
    ਅਚਲ char16_t *ext_ਸ਼ੁਰੂ = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    ਜੇ (ext_ਸ਼ੁਰੂ != ਖਾਲੀ_ਸੂਚਕ) {
        int32_t len = static_cast<int32_t>(ext_ਸ਼ੁਰੂ - dictfname);
        ext.appendInvariantChars(UnicodeString(ਝੂਠ, ext_ਸ਼ੁਰੂ + 1, dictnlength - len - 1), ਸਥਿਤੀ);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(ਝੂਠ, dictfname, dictnlength), ਸਥਿਤੀ);
    ures_close(b);

    UData_ਸਮ੍ਰਿਤੀ *file = udata_open(U_ICUDATA_BRKITR, ext.ਅੰਕੜੇ(), dictnbuf.ਅੰਕੜੇ(), &ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        // build trie
        ਅਚਲ uint8_t* ਅੰਕੜੇ = static_cast<ਅਚਲ uint8_t*>(udata_get_ਸਮ੍ਰਿਤੀ(file));
        ਅਚਲ int32_t* indexes = reinterpret_cast<ਅਚਲ int32_t*>(ਅੰਕੜੇ);
        ਅਚਲ int32_t ਵਿੱਥ = indexes[Dictionary_ਅੰਕੜੇ::IX_STRING_TRIE_ਵਿੱਥ];
        ਅਚਲ int32_t trieType = indexes[Dictionary_ਅੰਕੜੇ::IX_TRIE_TYPE] & Dictionary_ਅੰਕੜੇ::TRIE_TYPE_ਨਕਾਬ;
        DictionaryMatcher *m = ਖਾਲੀ_ਸੂਚਕ;
        ਜੇ (trieType == Dictionary_ਅੰਕੜੇ::TRIE_TYPE_ਬਾਈਟਾਂ) {
            ਅਚਲ int32_t transform = indexes[Dictionary_ਅੰਕੜੇ::IX_TRANSFORM];
            ਅਚਲ ਅੱਖਰ* characters = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਅੰਕੜੇ + ਵਿੱਥ);
            m = ਨਵਾਂ ਬਾਈਟਾਂ_Dictionary_Matcher(characters, transform, file);
        }
        ਨਹੀਂਤਾਂ ਜੇ (trieType == Dictionary_ਅੰਕੜੇ::TRIE_TYPE_UCHARS) {
            ਅਚਲ char16_t* characters = reinterpret_cast<ਅਚਲ char16_t*>(ਅੰਕੜੇ + ਵਿੱਥ);
            m = ਨਵਾਂ UCharsDictionaryMatcher(characters, file);
        }
        ਜੇ (m == ਖਾਲੀ_ਸੂਚਕ) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        ਵਾਪਸ m;
    } ਨਹੀਂਤਾਂ ਜੇ (dictfname != ਖਾਲੀ_ਸੂਚਕ) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        ਸਥਿਤੀ = U_ZERO_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}


ਖਾਲੀ ICULanguageBreakFactory::ਜੋੜੋ_External_Engine(
        ExternalBreakEngine* external, UError_ਕੋਡ& ਸਥਿਤੀ) {
    Local_ਸੂਚਕ<ExternalBreakEngine> engine(external, ਸਥਿਤੀ);
    ensureEngines(ਸਥਿਤੀ);
    Local_ਸੂਚਕ<BreakEngineWrapper> wrapper(
        ਨਵਾਂ BreakEngineWrapper(engine.orphan(), ਸਥਿਤੀ), ਸਥਿਤੀ);
    ਸਥਿਰ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), ਸਥਿਤੀ);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_ਕੋਡ &ਸਥਿਤੀ) : delegate(engine, ਸਥਿਤੀ) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) ਅਚਲ {
    ਵਾਪਸ delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::ਲੱਭੋ_Breaks(
    UText *ਪਾਠ,
    int32_t ਸ਼ੁਰੂ_Pos,
    int32_t ਸਿਰਾ_Pos,
    UVector32 &ਮਿਲਿਆ_Breaks,
    UBool /* isPhraseBreaking */,
    UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    int32_t ਨਤੀਜਾ = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਸ਼ੁਰੂ_Pos);
    int32_t ਸ਼ੁਰੂ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
    int32_t current;
    int32_t range_ਸ਼ੁਰੂ;
    int32_t range_ਸਿਰਾ;
    UChar32 c = utext_current32(ਪਾਠ);
    ਜਦੋਂ ((current = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < ਸਿਰਾ_Pos && delegate->handles(c)) {
        utext_next32(ਪਾਠ);         // TODO:  recast loop for postincrement
        c = utext_current32(ਪਾਠ);
    }
    range_ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    range_ਸਿਰਾ = current;
    int32_t before_ਮਾਪ = ਮਿਲਿਆ_Breaks.size();
    int32_t additional_ਸਮਰੱਥਾ = range_ਸਿਰਾ - range_ਸ਼ੁਰੂ + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    ਮਿਲਿਆ_Breaks.ensure_ਸਮਰੱਥਾ(before_ਮਾਪ+additional_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    ਮਿਲਿਆ_Breaks.ਸੈੱਟ_ਮਾਪ(before_ਮਾਪ + before_ਮਾਪ+additional_ਸਮਰੱਥਾ);
    ਨਤੀਜਾ = delegate->fillBreaks(ਪਾਠ, range_ਸ਼ੁਰੂ, range_ਸਿਰਾ, ਮਿਲਿਆ_Breaks.get_ਬਫਰ()+before_ਮਾਪ,
                                  additional_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    ਮਿਲਿਆ_Breaks.ਸੈੱਟ_ਮਾਪ(before_ਮਾਪ + ਨਤੀਜਾ);
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current);
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
