// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/uchar.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/chariter.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/putil.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/rbbi.h"

#ضمن "brkeng.h"
#ضمن "cmemory.h"
#ضمن "dictbe.h"
#ضمن "lstmbe.h"
#ضمن "charstr.h"
#ضمن "dictionarydata.h"
#ضمن "mutex.h"
#ضمن "uvector.h"
#ضمن "umutex.h"
#ضمن "uresimp.h"
#ضمن "ubrkimpl.h"

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

UnhandledEngine::UnhandledEngine(UError_ڪوڊ &حالت) : fHandled(خالي_اشارو) {
    (فراغ)حالت;
}

UnhandledEngine::~UnhandledEngine() {
    ميٽيو fHandled;
    fHandled = خالي_اشارو;
}

UBool
UnhandledEngine::handles(UChar32 c, ثابت حرف* locale) ثابت {
    (فراغ)locale; //اڻ_استعمال_ٿيل
    أرجع fHandled && fHandled->شامل_آهي(c);
}

int32_t
UnhandledEngine::ڳوليو_Breaks( UText *متن,
                             int32_t شروعات_Pos,
                             int32_t پڇاڙي_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_ڪوڊ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    utext_سيٽ_Native_اشاريو(متن, شروعات_Pos);
    UChar32 c = utext_current32(متن);
    طالما (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) < پڇاڙي_Pos && fHandled->شامل_آهي(c)) {
        utext_next32(متن);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(متن);
    }
    أرجع 0;
}

فراغ
UnhandledEngine::handleCharacter(UChar32 c) {
    إذا (fHandled == خالي_اشارو) {
        fHandled = جديد Unicode_سيٽ();
        إذا (fHandled == خالي_اشارو) {
            أرجع;
        }
    }
    إذا (!fHandled->شامل_آهي(c)) {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_قدر(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_قدر(UCHAR_SCRIPT, script, حالت);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_ڪوڊ &/*حالت*/) {
    fEngines = خالي_اشارو;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    ميٽيو fEngines;
}

فراغ ICULanguageBreakFactory::ensureEngines(UError_ڪوڊ& حالت) {
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    إذا (fEngines == خالي_اشارو) {
        Local_اشارو<UStack>  engines(جديد UStack(uprv_deleteUObject, خالي_اشارو, حالت), حالت);
        إذا (U_SUCCESS(حالت)) {
            fEngines = engines.orphan();
        }
    }
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ثابت حرف* locale) {
    ثابت LanguageBreakEngine *lbe = خالي_اشارو;
    UError_ڪوڊ  حالت = U_ZERO_ERROR;
    ensureEngines(حالت);
    إذا (U_FAILURE(حالت) ) {
        // Note: no way to return error code to caller.
        أرجع خالي_اشارو;
    }

    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    طالما (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fEngines->elementAt(i));
        إذا (lbe != خالي_اشارو && lbe->handles(c, locale)) {
            أرجع lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = ڀريو_Engine_For(c, locale);
    إذا (lbe != خالي_اشارو) {
        fEngines->push((فراغ *)lbe, حالت);
    }
    أرجع U_SUCCESS(حالت) ? lbe : خالي_اشارو;
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::ڀريو_Engine_For(UChar32 c, ثابت حرف*) {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    UScript_ڪوڊ ڪوڊ = uscript_getScript(c, &حالت);
    إذا (U_SUCCESS(حالت)) {
        ثابت LanguageBreakEngine *engine = خالي_اشارو;
        // Try to use LSTM first
        ثابت LSTMData *معلومات = CreateLSTMDataForScript(ڪوڊ, حالت);
        إذا (U_SUCCESS(حالت)) {
            إذا (معلومات != خالي_اشارو) {
                engine = CreateLSTMBreakEngine(ڪوڊ, معلومات, حالت);
                إذا (U_SUCCESS(حالت) && engine != خالي_اشارو) {
                    أرجع engine;
                }
                إذا (engine != خالي_اشارو) {
                    ميٽيو engine;
                    engine = خالي_اشارو;
                } وإلا {
                    DeleteLSTMData(معلومات);
                }
            }
        }
        حالت = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = ڀريو_Dictionary_Matcher_For(ڪوڊ);
        إذا (m != خالي_اشارو) {
            بدل(ڪوڊ) {
            حالة USCRIPT_THAI:
                engine = جديد ThaiBreakEngine(m, حالت);
                اكسر;
            حالة USCRIPT_LAO:
                engine = جديد LaoBreakEngine(m, حالت);
                اكسر;
            حالة USCRIPT_MYANMAR:
                engine = جديد BurmeseBreakEngine(m, حالت);
                اكسر;
            حالة USCRIPT_KHMER:
                engine = جديد KhmerBreakEngine(m, حالت);
                اكسر;

#إذا !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            حالة USCRIPT_HANGUL:
                engine = جديد CjkBreakEngine(m, kKorean, حالت);
                اكسر;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            حالة USCRIPT_HIRAGANA:
            حالة USCRIPT_KATAKANA:
            حالة USCRIPT_HAN:
                engine = جديد CjkBreakEngine(m, kChineseJapanese, حالت);
                اكسر;
#إذا 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            حالة USCRIPT_COMMON:
            {
                UBlock_ڪوڊ بلاڪ = ublock_get_ڪوڊ(ڪوڊ);
                إذا (بلاڪ == UBLOCK_HIRAGANA || بلاڪ == UBLOCK_KATAKANA)
                   engine = جديد CjkBreakEngine(dict, kChineseJapanese, حالت);
                اكسر;
            }
#جيڪڏهن_پورو
#جيڪڏهن_پورو

            افتراضي:
                اكسر;
            }
            إذا (engine == خالي_اشارو) {
                ميٽيو m;
            }
            وإلا إذا (U_FAILURE(حالت)) {
                ميٽيو engine;
                engine = خالي_اشارو;
            }
            أرجع engine;
        }
    }
    أرجع خالي_اشارو;
}

DictionaryMatcher *
ICULanguageBreakFactory::ڀريو_Dictionary_Matcher_For(UScript_ڪوڊ script) { 
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &حالت);
    b = ures_get_By_ڪنجي_With_Fallback(b, "dictionaries", b, &حالت);
    int32_t dictnlength = 0;
    ثابت char16_t *dictfname =
        ures_get_String_By_ڪنجي_With_Fallback(b, uscript_getShortName(script), &dictnlength, &حالت);
    إذا (U_FAILURE(حالت)) {
        ures_close(b);
        أرجع خالي_اشارو;
    }
    CharString dictnbuf;
    CharString ext;
    ثابت char16_t *ext_شروعات = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    إذا (ext_شروعات != خالي_اشارو) {
        int32_t len = static_cast<int32_t>(ext_شروعات - dictfname);
        ext.appendInvariantChars(UnicodeString(خطأ, ext_شروعات + 1, dictnlength - len - 1), حالت);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(خطأ, dictfname, dictnlength), حالت);
    ures_close(b);

    UData_يادگيري *file = udata_open(U_ICUDATA_BRKITR, ext.معلومات(), dictnbuf.معلومات(), &حالت);
    إذا (U_SUCCESS(حالت)) {
        // build trie
        ثابت uint8_t* معلومات = static_cast<ثابت uint8_t*>(udata_get_يادگيري(file));
        ثابت int32_t* indexes = reinterpret_cast<ثابت int32_t*>(معلومات);
        ثابت int32_t وٿي = indexes[Dictionary_معلومات::IX_STRING_TRIE_وٿي];
        ثابت int32_t trieType = indexes[Dictionary_معلومات::IX_TRIE_TYPE] & Dictionary_معلومات::TRIE_TYPE_نقاب;
        DictionaryMatcher *m = خالي_اشارو;
        إذا (trieType == Dictionary_معلومات::TRIE_TYPE_بائيٽون) {
            ثابت int32_t transform = indexes[Dictionary_معلومات::IX_TRANSFORM];
            ثابت حرف* characters = reinterpret_cast<ثابت حرف*>(معلومات + وٿي);
            m = جديد بائيٽون_Dictionary_Matcher(characters, transform, file);
        }
        وإلا إذا (trieType == Dictionary_معلومات::TRIE_TYPE_UCHARS) {
            ثابت char16_t* characters = reinterpret_cast<ثابت char16_t*>(معلومات + وٿي);
            m = جديد UCharsDictionaryMatcher(characters, file);
        }
        إذا (m == خالي_اشارو) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        أرجع m;
    } وإلا إذا (dictfname != خالي_اشارو) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        حالت = U_ZERO_ERROR;
        أرجع خالي_اشارو;
    }
    أرجع خالي_اشارو;
}


فراغ ICULanguageBreakFactory::جوڙيو_External_Engine(
        ExternalBreakEngine* external, UError_ڪوڊ& حالت) {
    Local_اشارو<ExternalBreakEngine> engine(external, حالت);
    ensureEngines(حالت);
    Local_اشارو<BreakEngineWrapper> wrapper(
        جديد BreakEngineWrapper(engine.orphan(), حالت), حالت);
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), حالت);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_ڪوڊ &حالت) : delegate(engine, حالت) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ثابت حرف* locale) ثابت {
    أرجع delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::ڳوليو_Breaks(
    UText *متن,
    int32_t شروعات_Pos,
    int32_t پڇاڙي_Pos,
    UVector32 &مليو_Breaks,
    UBool /* isPhraseBreaking */,
    UError_ڪوڊ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t نتيجو = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_سيٽ_Native_اشاريو(متن, شروعات_Pos);
    int32_t شروعات = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
    int32_t current;
    int32_t range_شروعات;
    int32_t range_پڇاڙي;
    UChar32 c = utext_current32(متن);
    طالما ((current = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < پڇاڙي_Pos && delegate->handles(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_شروعات = شروعات;
    range_پڇاڙي = current;
    int32_t before_سائيز = مليو_Breaks.size();
    int32_t additional_صلاحيت = range_پڇاڙي - range_شروعات + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    مليو_Breaks.ensure_صلاحيت(before_سائيز+additional_صلاحيت, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    مليو_Breaks.سيٽ_سائيز(before_سائيز + before_سائيز+additional_صلاحيت);
    نتيجو = delegate->fillBreaks(متن, range_شروعات, range_پڇاڙي, مليو_Breaks.get_بفر()+before_سائيز,
                                  additional_صلاحيت, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    مليو_Breaks.سيٽ_سائيز(before_سائيز + نتيجو);
    utext_سيٽ_Native_اشاريو(متن, current);
    أرجع نتيجو;
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
