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

UnhandledEngine::UnhandledEngine(UError_کوڈ &حالت) : fHandled(خالی_سوچک) {
    (فراغ)حالت;
}

UnhandledEngine::~UnhandledEngine() {
    مٹاؤ fHandled;
    fHandled = خالی_سوچک;
}

UBool
UnhandledEngine::handles(UChar32 c, ثابت حرف* locale) ثابت {
    (فراغ)locale; //بن_ورتیا
    أرجع fHandled && fHandled->شامل_اے(c);
}

int32_t
UnhandledEngine::لبھو_Breaks( UText *متن,
                             int32_t شروع_Pos,
                             int32_t سرا_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_کوڈ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    utext_سیٹ_Native_اشاریہ(متن, شروع_Pos);
    UChar32 c = utext_current32(متن);
    طالما (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < سرا_Pos && fHandled->شامل_اے(c)) {
        utext_next32(متن);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(متن);
    }
    أرجع 0;
}

فراغ
UnhandledEngine::handleCharacter(UChar32 c) {
    إذا (fHandled == خالی_سوچک) {
        fHandled = جديد Unicode_سیٹ();
        إذا (fHandled == خالی_سوچک) {
            أرجع;
        }
    }
    إذا (!fHandled->شامل_اے(c)) {
        UError_کوڈ حالت = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_قدر(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_قدر(UCHAR_SCRIPT, script, حالت);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_کوڈ &/*حالت*/) {
    fEngines = خالی_سوچک;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    مٹاؤ fEngines;
}

فراغ ICULanguageBreakFactory::ensureEngines(UError_کوڈ& حالت) {
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    إذا (fEngines == خالی_سوچک) {
        Local_اشارہ_گر<UStack>  engines(جديد UStack(uprv_deleteUObject, خالی_سوچک, حالت), حالت);
        إذا (U_SUCCESS(حالت)) {
            fEngines = engines.orphan();
        }
    }
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ثابت حرف* locale) {
    ثابت LanguageBreakEngine *lbe = خالی_سوچک;
    UError_کوڈ  حالت = U_ZERO_ERROR;
    ensureEngines(حالت);
    إذا (U_FAILURE(حالت) ) {
        // Note: no way to return error code to caller.
        أرجع خالی_سوچک;
    }

    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    طالما (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fEngines->elementAt(i));
        إذا (lbe != خالی_سوچک && lbe->handles(c, locale)) {
            أرجع lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = لدو_Engine_For(c, locale);
    إذا (lbe != خالی_سوچک) {
        fEngines->push((فراغ *)lbe, حالت);
    }
    أرجع U_SUCCESS(حالت) ? lbe : خالی_سوچک;
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::لدو_Engine_For(UChar32 c, ثابت حرف*) {
    UError_کوڈ حالت = U_ZERO_ERROR;
    UScript_کوڈ کوڈ = uscript_getScript(c, &حالت);
    إذا (U_SUCCESS(حالت)) {
        ثابت LanguageBreakEngine *engine = خالی_سوچک;
        // Try to use LSTM first
        ثابت LSTMData *انکڑے = CreateLSTMDataForScript(کوڈ, حالت);
        إذا (U_SUCCESS(حالت)) {
            إذا (انکڑے != خالی_سوچک) {
                engine = CreateLSTMBreakEngine(کوڈ, انکڑے, حالت);
                إذا (U_SUCCESS(حالت) && engine != خالی_سوچک) {
                    أرجع engine;
                }
                إذا (engine != خالی_سوچک) {
                    مٹاؤ engine;
                    engine = خالی_سوچک;
                } وإلا {
                    DeleteLSTMData(انکڑے);
                }
            }
        }
        حالت = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = لدو_Dictionary_Matcher_For(کوڈ);
        إذا (m != خالی_سوچک) {
            بدل(کوڈ) {
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
                UBlock_کوڈ بلاک = ublock_get_کوڈ(کوڈ);
                إذا (بلاک == UBLOCK_HIRAGANA || بلاک == UBLOCK_KATAKANA)
                   engine = جديد CjkBreakEngine(dict, kChineseJapanese, حالت);
                اكسر;
            }
#جے_انت
#جے_انت

            افتراضي:
                اكسر;
            }
            إذا (engine == خالی_سوچک) {
                مٹاؤ m;
            }
            وإلا إذا (U_FAILURE(حالت)) {
                مٹاؤ engine;
                engine = خالی_سوچک;
            }
            أرجع engine;
        }
    }
    أرجع خالی_سوچک;
}

DictionaryMatcher *
ICULanguageBreakFactory::لدو_Dictionary_Matcher_For(UScript_کوڈ script) { 
    UError_کوڈ حالت = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &حالت);
    b = ures_get_By_کلید_With_Fallback(b, "dictionaries", b, &حالت);
    int32_t dictnlength = 0;
    ثابت char16_t *dictfname =
        ures_get_String_By_کلید_With_Fallback(b, uscript_getShortName(script), &dictnlength, &حالت);
    إذا (U_FAILURE(حالت)) {
        ures_close(b);
        أرجع خالی_سوچک;
    }
    CharString dictnbuf;
    CharString ext;
    ثابت char16_t *ext_شروع = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    إذا (ext_شروع != خالی_سوچک) {
        int32_t len = static_cast<int32_t>(ext_شروع - dictfname);
        ext.appendInvariantChars(UnicodeString(خطأ, ext_شروع + 1, dictnlength - len - 1), حالت);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(خطأ, dictfname, dictnlength), حالت);
    ures_close(b);

    UData_حافظہ *file = udata_open(U_ICUDATA_BRKITR, ext.انکڑے(), dictnbuf.انکڑے(), &حالت);
    إذا (U_SUCCESS(حالت)) {
        // build trie
        ثابت uint8_t* انکڑے = static_cast<ثابت uint8_t*>(udata_get_حافظہ(file));
        ثابت int32_t* indexes = reinterpret_cast<ثابت int32_t*>(انکڑے);
        ثابت int32_t وتھ = indexes[Dictionary_انکڑے::IX_STRING_TRIE_وتھ];
        ثابت int32_t trieType = indexes[Dictionary_انکڑے::IX_TRIE_TYPE] & Dictionary_انکڑے::TRIE_TYPE_نقاب;
        DictionaryMatcher *m = خالی_سوچک;
        إذا (trieType == Dictionary_انکڑے::TRIE_TYPE_بائٹاں) {
            ثابت int32_t transform = indexes[Dictionary_انکڑے::IX_TRANSFORM];
            ثابت حرف* characters = reinterpret_cast<ثابت حرف*>(انکڑے + وتھ);
            m = جديد بائٹاں_Dictionary_Matcher(characters, transform, file);
        }
        وإلا إذا (trieType == Dictionary_انکڑے::TRIE_TYPE_UCHARS) {
            ثابت char16_t* characters = reinterpret_cast<ثابت char16_t*>(انکڑے + وتھ);
            m = جديد UCharsDictionaryMatcher(characters, file);
        }
        إذا (m == خالی_سوچک) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        أرجع m;
    } وإلا إذا (dictfname != خالی_سوچک) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        حالت = U_ZERO_ERROR;
        أرجع خالی_سوچک;
    }
    أرجع خالی_سوچک;
}


فراغ ICULanguageBreakFactory::جوڑو_External_Engine(
        ExternalBreakEngine* external, UError_کوڈ& حالت) {
    Local_اشارہ_گر<ExternalBreakEngine> engine(external, حالت);
    ensureEngines(حالت);
    Local_اشارہ_گر<BreakEngineWrapper> wrapper(
        جديد BreakEngineWrapper(engine.orphan(), حالت), حالت);
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), حالت);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_کوڈ &حالت) : delegate(engine, حالت) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ثابت حرف* locale) ثابت {
    أرجع delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::لبھو_Breaks(
    UText *متن,
    int32_t شروع_Pos,
    int32_t سرا_Pos,
    UVector32 &لبھیا_Breaks,
    UBool /* isPhraseBreaking */,
    UError_کوڈ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t نتیجہ = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_سیٹ_Native_اشاریہ(متن, شروع_Pos);
    int32_t شروع = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    int32_t current;
    int32_t range_شروع;
    int32_t range_سرا;
    UChar32 c = utext_current32(متن);
    طالما ((current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < سرا_Pos && delegate->handles(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_شروع = شروع;
    range_سرا = current;
    int32_t before_ناپ = لبھیا_Breaks.size();
    int32_t additional_سمرتھا = range_سرا - range_شروع + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    لبھیا_Breaks.ensure_سمرتھا(before_ناپ+additional_سمرتھا, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    لبھیا_Breaks.سیٹ_ناپ(before_ناپ + before_ناپ+additional_سمرتھا);
    نتیجہ = delegate->fillBreaks(متن, range_شروع, range_سرا, لبھیا_Breaks.get_بفر()+before_ناپ,
                                  additional_سمرتھا, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    لبھیا_Breaks.سیٹ_ناپ(before_ناپ + نتیجہ);
    utext_سیٹ_Native_اشاریہ(متن, current);
    أرجع نتیجہ;
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
