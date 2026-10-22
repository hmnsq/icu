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

UnhandledEngine::UnhandledEngine(UError_کوډ &حالت) : fHandled(تش_اشاره) {
    (فراغ)حالت;
}

UnhandledEngine::~UnhandledEngine() {
    ړنګول fHandled;
    fHandled = تش_اشاره;
}

UBool
UnhandledEngine::handles(UChar32 c, ثابت حرف* locale) ثابت {
    (فراغ)locale; //ناکارول_شوی
    أرجع fHandled && fHandled->لري(c);
}

int32_t
UnhandledEngine::پیدا_کړه_Breaks( UText *متن,
                             int32_t پیل_Pos,
                             int32_t پای_ټکی_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_کوډ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    utext_سېټ_Native_شاخص(متن, پیل_Pos);
    UChar32 c = utext_current32(متن);
    طالما (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < پای_ټکی_Pos && fHandled->لري(c)) {
        utext_next32(متن);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(متن);
    }
    أرجع 0;
}

فراغ
UnhandledEngine::handleCharacter(UChar32 c) {
    إذا (fHandled == تش_اشاره) {
        fHandled = جديد Unicode_سېټ();
        إذا (fHandled == تش_اشاره) {
            أرجع;
        }
    }
    إذا (!fHandled->لري(c)) {
        UError_کوډ حالت = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_ارزښت(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_ارزښت(UCHAR_SCRIPT, script, حالت);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_کوډ &/*حالت*/) {
    fEngines = تش_اشاره;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    ړنګول fEngines;
}

فراغ ICULanguageBreakFactory::ensureEngines(UError_کوډ& حالت) {
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    إذا (fEngines == تش_اشاره) {
        Local_اشاره_ګر<UStack>  engines(جديد UStack(uprv_deleteUObject, تش_اشاره, حالت), حالت);
        إذا (U_SUCCESS(حالت)) {
            fEngines = engines.orphan();
        }
    }
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ثابت حرف* locale) {
    ثابت LanguageBreakEngine *lbe = تش_اشاره;
    UError_کوډ  حالت = U_ZERO_ERROR;
    ensureEngines(حالت);
    إذا (U_FAILURE(حالت) ) {
        // Note: no way to return error code to caller.
        أرجع تش_اشاره;
    }

    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    طالما (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fEngines->elementAt(i));
        إذا (lbe != تش_اشاره && lbe->handles(c, locale)) {
            أرجع lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = بارول_Engine_For(c, locale);
    إذا (lbe != تش_اشاره) {
        fEngines->push((فراغ *)lbe, حالت);
    }
    أرجع U_SUCCESS(حالت) ? lbe : تش_اشاره;
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::بارول_Engine_For(UChar32 c, ثابت حرف*) {
    UError_کوډ حالت = U_ZERO_ERROR;
    UScript_کوډ کوډ = uscript_getScript(c, &حالت);
    إذا (U_SUCCESS(حالت)) {
        ثابت LanguageBreakEngine *engine = تش_اشاره;
        // Try to use LSTM first
        ثابت LSTMData *معلومات = CreateLSTMDataForScript(کوډ, حالت);
        إذا (U_SUCCESS(حالت)) {
            إذا (معلومات != تش_اشاره) {
                engine = CreateLSTMBreakEngine(کوډ, معلومات, حالت);
                إذا (U_SUCCESS(حالت) && engine != تش_اشاره) {
                    أرجع engine;
                }
                إذا (engine != تش_اشاره) {
                    ړنګول engine;
                    engine = تش_اشاره;
                } وإلا {
                    DeleteLSTMData(معلومات);
                }
            }
        }
        حالت = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = بارول_Dictionary_Matcher_For(کوډ);
        إذا (m != تش_اشاره) {
            بدل(کوډ) {
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
                UBlock_کوډ بلاک = ublock_get_کوډ(کوډ);
                إذا (بلاک == UBLOCK_HIRAGANA || بلاک == UBLOCK_KATAKANA)
                   engine = جديد CjkBreakEngine(dict, kChineseJapanese, حالت);
                اكسر;
            }
#د_که_پای
#د_که_پای

            افتراضي:
                اكسر;
            }
            إذا (engine == تش_اشاره) {
                ړنګول m;
            }
            وإلا إذا (U_FAILURE(حالت)) {
                ړنګول engine;
                engine = تش_اشاره;
            }
            أرجع engine;
        }
    }
    أرجع تش_اشاره;
}

DictionaryMatcher *
ICULanguageBreakFactory::بارول_Dictionary_Matcher_For(UScript_کوډ script) { 
    UError_کوډ حالت = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &حالت);
    b = ures_get_By_کلی_With_Fallback(b, "dictionaries", b, &حالت);
    int32_t dictnlength = 0;
    ثابت char16_t *dictfname =
        ures_get_String_By_کلی_With_Fallback(b, uscript_getShortName(script), &dictnlength, &حالت);
    إذا (U_FAILURE(حالت)) {
        ures_close(b);
        أرجع تش_اشاره;
    }
    CharString dictnbuf;
    CharString ext;
    ثابت char16_t *ext_پیل = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    إذا (ext_پیل != تش_اشاره) {
        int32_t len = static_cast<int32_t>(ext_پیل - dictfname);
        ext.appendInvariantChars(UnicodeString(خطأ, ext_پیل + 1, dictnlength - len - 1), حالت);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(خطأ, dictfname, dictnlength), حالت);
    ures_close(b);

    UData_حافظه *file = udata_open(U_ICUDATA_BRKITR, ext.معلومات(), dictnbuf.معلومات(), &حالت);
    إذا (U_SUCCESS(حالت)) {
        // build trie
        ثابت uint8_t* معلومات = static_cast<ثابت uint8_t*>(udata_get_حافظه(file));
        ثابت int32_t* indexes = reinterpret_cast<ثابت int32_t*>(معلومات);
        ثابت int32_t دننه_واټن = indexes[Dictionary_معلومات::IX_STRING_TRIE_دننه_واټن];
        ثابت int32_t trieType = indexes[Dictionary_معلومات::IX_TRIE_TYPE] & Dictionary_معلومات::TRIE_TYPE_ماسک;
        DictionaryMatcher *m = تش_اشاره;
        إذا (trieType == Dictionary_معلومات::TRIE_TYPE_بایټونه) {
            ثابت int32_t transform = indexes[Dictionary_معلومات::IX_TRANSFORM];
            ثابت حرف* characters = reinterpret_cast<ثابت حرف*>(معلومات + دننه_واټن);
            m = جديد بایټونه_Dictionary_Matcher(characters, transform, file);
        }
        وإلا إذا (trieType == Dictionary_معلومات::TRIE_TYPE_UCHARS) {
            ثابت char16_t* characters = reinterpret_cast<ثابت char16_t*>(معلومات + دننه_واټن);
            m = جديد UCharsDictionaryMatcher(characters, file);
        }
        إذا (m == تش_اشاره) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        أرجع m;
    } وإلا إذا (dictfname != تش_اشاره) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        حالت = U_ZERO_ERROR;
        أرجع تش_اشاره;
    }
    أرجع تش_اشاره;
}


فراغ ICULanguageBreakFactory::جمع_کړه_External_Engine(
        ExternalBreakEngine* external, UError_کوډ& حالت) {
    Local_اشاره_ګر<ExternalBreakEngine> engine(external, حالت);
    ensureEngines(حالت);
    Local_اشاره_ګر<BreakEngineWrapper> wrapper(
        جديد BreakEngineWrapper(engine.orphan(), حالت), حالت);
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), حالت);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_کوډ &حالت) : delegate(engine, حالت) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ثابت حرف* locale) ثابت {
    أرجع delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::پیدا_کړه_Breaks(
    UText *متن,
    int32_t پیل_Pos,
    int32_t پای_ټکی_Pos,
    UVector32 &وموندل_Breaks,
    UBool /* isPhraseBreaking */,
    UError_کوډ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t پایله = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_سېټ_Native_شاخص(متن, پیل_Pos);
    int32_t پیل = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    int32_t current;
    int32_t range_پیل;
    int32_t range_پای_ټکی;
    UChar32 c = utext_current32(متن);
    طالما ((current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < پای_ټکی_Pos && delegate->handles(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_پیل = پیل;
    range_پای_ټکی = current;
    int32_t before_اندازه = وموندل_Breaks.size();
    int32_t additional_ظرفیت = range_پای_ټکی - range_پیل + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    وموندل_Breaks.ensure_ظرفیت(before_اندازه+additional_ظرفیت, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    وموندل_Breaks.سېټ_اندازه(before_اندازه + before_اندازه+additional_ظرفیت);
    پایله = delegate->fillBreaks(متن, range_پیل, range_پای_ټکی, وموندل_Breaks.get_بفر()+before_اندازه,
                                  additional_ظرفیت, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    وموندل_Breaks.سېټ_اندازه(before_اندازه + پایله);
    utext_سېټ_Native_شاخص(متن, current);
    أرجع پایله;
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
