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

UnhandledEngine::UnhandledEngine(UError_کد &وضعیت) : fHandled(اشاره_تهی) {
    (فراغ)وضعیت;
}

UnhandledEngine::~UnhandledEngine() {
    حذف fHandled;
    fHandled = اشاره_تهی;
}

UBool
UnhandledEngine::handles(UChar32 c, ثابت حرف* locale) ثابت {
    (فراغ)locale; //استفادهنشده
    أرجع fHandled && fHandled->شامل_است(c);
}

int32_t
UnhandledEngine::پیدا_کن_Breaks( UText *متن,
                             int32_t شروع_Pos,
                             int32_t انتها_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_کد &وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    utext_مجموعه_Native_شاخص(متن, شروع_Pos);
    UChar32 c = utext_current32(متن);
    طالما (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < انتها_Pos && fHandled->شامل_است(c)) {
        utext_next32(متن);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(متن);
    }
    أرجع 0;
}

فراغ
UnhandledEngine::handleCharacter(UChar32 c) {
    إذا (fHandled == اشاره_تهی) {
        fHandled = جديد Unicode_مجموعه();
        إذا (fHandled == اشاره_تهی) {
            أرجع;
        }
    }
    إذا (!fHandled->شامل_است(c)) {
        UError_کد وضعیت = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_مقدار(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_مقدار(UCHAR_SCRIPT, script, وضعیت);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_کد &/*وضعیت*/) {
    fEngines = اشاره_تهی;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    حذف fEngines;
}

فراغ ICULanguageBreakFactory::ensureEngines(UError_کد& وضعیت) {
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    إذا (fEngines == اشاره_تهی) {
        Local_اشاره_گر<UStack>  engines(جديد UStack(uprv_deleteUObject, اشاره_تهی, وضعیت), وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
            fEngines = engines.orphan();
        }
    }
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ثابت حرف* locale) {
    ثابت LanguageBreakEngine *lbe = اشاره_تهی;
    UError_کد  وضعیت = U_ZERO_ERROR;
    ensureEngines(وضعیت);
    إذا (U_FAILURE(وضعیت) ) {
        // Note: no way to return error code to caller.
        أرجع اشاره_تهی;
    }

    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    طالما (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fEngines->elementAt(i));
        إذا (lbe != اشاره_تهی && lbe->handles(c, locale)) {
            أرجع lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = بارگیری_Engine_For(c, locale);
    إذا (lbe != اشاره_تهی) {
        fEngines->push((فراغ *)lbe, وضعیت);
    }
    أرجع U_SUCCESS(وضعیت) ? lbe : اشاره_تهی;
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::بارگیری_Engine_For(UChar32 c, ثابت حرف*) {
    UError_کد وضعیت = U_ZERO_ERROR;
    UScript_کد کد = uscript_getScript(c, &وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        ثابت LanguageBreakEngine *engine = اشاره_تهی;
        // Try to use LSTM first
        ثابت LSTMData *داده = CreateLSTMDataForScript(کد, وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
            إذا (داده != اشاره_تهی) {
                engine = CreateLSTMBreakEngine(کد, داده, وضعیت);
                إذا (U_SUCCESS(وضعیت) && engine != اشاره_تهی) {
                    أرجع engine;
                }
                إذا (engine != اشاره_تهی) {
                    حذف engine;
                    engine = اشاره_تهی;
                } وإلا {
                    DeleteLSTMData(داده);
                }
            }
        }
        وضعیت = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = بارگیری_Dictionary_Matcher_For(کد);
        إذا (m != اشاره_تهی) {
            بدل(کد) {
            حالة USCRIPT_THAI:
                engine = جديد ThaiBreakEngine(m, وضعیت);
                اكسر;
            حالة USCRIPT_LAO:
                engine = جديد LaoBreakEngine(m, وضعیت);
                اكسر;
            حالة USCRIPT_MYANMAR:
                engine = جديد BurmeseBreakEngine(m, وضعیت);
                اكسر;
            حالة USCRIPT_KHMER:
                engine = جديد KhmerBreakEngine(m, وضعیت);
                اكسر;

#إذا !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            حالة USCRIPT_HANGUL:
                engine = جديد CjkBreakEngine(m, kKorean, وضعیت);
                اكسر;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            حالة USCRIPT_HIRAGANA:
            حالة USCRIPT_KATAKANA:
            حالة USCRIPT_HAN:
                engine = جديد CjkBreakEngine(m, kChineseJapanese, وضعیت);
                اكسر;
#إذا 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            حالة USCRIPT_COMMON:
            {
                UBlock_کد بلوک = ublock_get_کد(کد);
                إذا (بلوک == UBLOCK_HIRAGANA || بلوک == UBLOCK_KATAKANA)
                   engine = جديد CjkBreakEngine(dict, kChineseJapanese, وضعیت);
                اكسر;
            }
#پایان_اگر
#پایان_اگر

            افتراضي:
                اكسر;
            }
            إذا (engine == اشاره_تهی) {
                حذف m;
            }
            وإلا إذا (U_FAILURE(وضعیت)) {
                حذف engine;
                engine = اشاره_تهی;
            }
            أرجع engine;
        }
    }
    أرجع اشاره_تهی;
}

DictionaryMatcher *
ICULanguageBreakFactory::بارگیری_Dictionary_Matcher_For(UScript_کد script) { 
    UError_کد وضعیت = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &وضعیت);
    b = ures_get_By_کلید_With_Fallback(b, "dictionaries", b, &وضعیت);
    int32_t dictnlength = 0;
    ثابت char16_t *dictfname =
        ures_get_String_By_کلید_With_Fallback(b, uscript_getShortName(script), &dictnlength, &وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        ures_close(b);
        أرجع اشاره_تهی;
    }
    CharString dictnbuf;
    CharString ext;
    ثابت char16_t *ext_شروع = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    إذا (ext_شروع != اشاره_تهی) {
        int32_t len = static_cast<int32_t>(ext_شروع - dictfname);
        ext.appendInvariantChars(UnicodeString(خطأ, ext_شروع + 1, dictnlength - len - 1), وضعیت);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(خطأ, dictfname, dictnlength), وضعیت);
    ures_close(b);

    UData_حافظه *file = udata_open(U_ICUDATA_BRKITR, ext.داده(), dictnbuf.داده(), &وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        // build trie
        ثابت uint8_t* داده = static_cast<ثابت uint8_t*>(udata_get_حافظه(file));
        ثابت int32_t* indexes = reinterpret_cast<ثابت int32_t*>(داده);
        ثابت int32_t جابه_جایی = indexes[Dictionary_داده::IX_STRING_TRIE_جابه_جایی];
        ثابت int32_t trieType = indexes[Dictionary_داده::IX_TRIE_TYPE] & Dictionary_داده::TRIE_TYPE_ماسک;
        DictionaryMatcher *m = اشاره_تهی;
        إذا (trieType == Dictionary_داده::TRIE_TYPE_بایتها) {
            ثابت int32_t transform = indexes[Dictionary_داده::IX_TRANSFORM];
            ثابت حرف* characters = reinterpret_cast<ثابت حرف*>(داده + جابه_جایی);
            m = جديد بایتها_Dictionary_Matcher(characters, transform, file);
        }
        وإلا إذا (trieType == Dictionary_داده::TRIE_TYPE_UCHARS) {
            ثابت char16_t* characters = reinterpret_cast<ثابت char16_t*>(داده + جابه_جایی);
            m = جديد UCharsDictionaryMatcher(characters, file);
        }
        إذا (m == اشاره_تهی) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        أرجع m;
    } وإلا إذا (dictfname != اشاره_تهی) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        وضعیت = U_ZERO_ERROR;
        أرجع اشاره_تهی;
    }
    أرجع اشاره_تهی;
}


فراغ ICULanguageBreakFactory::جمع_کن_External_Engine(
        ExternalBreakEngine* external, UError_کد& وضعیت) {
    Local_اشاره_گر<ExternalBreakEngine> engine(external, وضعیت);
    ensureEngines(وضعیت);
    Local_اشاره_گر<BreakEngineWrapper> wrapper(
        جديد BreakEngineWrapper(engine.orphan(), وضعیت), وضعیت);
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), وضعیت);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_کد &وضعیت) : delegate(engine, وضعیت) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ثابت حرف* locale) ثابت {
    أرجع delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::پیدا_کن_Breaks(
    UText *متن,
    int32_t شروع_Pos,
    int32_t انتها_Pos,
    UVector32 &یافت_Breaks,
    UBool /* isPhraseBreaking */,
    UError_کد &وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    int32_t نتیجه = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_مجموعه_Native_شاخص(متن, شروع_Pos);
    int32_t شروع = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    int32_t current;
    int32_t range_شروع;
    int32_t range_انتها;
    UChar32 c = utext_current32(متن);
    طالما ((current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < انتها_Pos && delegate->handles(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_شروع = شروع;
    range_انتها = current;
    int32_t before_اندازه = یافت_Breaks.size();
    int32_t additional_ظرفیت = range_انتها - range_شروع + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    یافت_Breaks.ensure_ظرفیت(before_اندازه+additional_ظرفیت, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    یافت_Breaks.مجموعه_اندازه(before_اندازه + before_اندازه+additional_ظرفیت);
    نتیجه = delegate->fillBreaks(متن, range_شروع, range_انتها, یافت_Breaks.get_میانگیر()+before_اندازه,
                                  additional_ظرفیت, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    یافت_Breaks.مجموعه_اندازه(before_اندازه + نتیجه);
    utext_مجموعه_Native_شاخص(متن, current);
    أرجع نتیجه;
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
