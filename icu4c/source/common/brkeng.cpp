// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/uchar.h"
#شامل "unicode/uniset.h"
#شامل "unicode/chariter.h"
#شامل "unicode/ures.h"
#شامل "unicode/udata.h"
#شامل "unicode/putil.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uscript.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/rbbi.h"

#شامل "brkeng.h"
#شامل "cmemory.h"
#شامل "dictbe.h"
#شامل "lstmbe.h"
#شامل "charstr.h"
#شامل "dictionarydata.h"
#شامل "mutex.h"
#شامل "uvector.h"
#شامل "umutex.h"
#شامل "uresimp.h"
#شامل "ubrkimpl.h"

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

UnhandledEngine::UnhandledEngine(UError_رمز &حالت_2) : fHandled(خالی_اشاریہ) {
    (خالی)حالت_2;
}

UnhandledEngine::~UnhandledEngine() {
    حذف fHandled;
    fHandled = خالی_اشاریہ;
}

UBool
UnhandledEngine::handles(UChar32 c, ثابت حرف* locale) ثابت {
    (خالی)locale; //غیر_مستعمل
    واپس fHandled && fHandled->شامل_ہے(c);
}

int32_t
UnhandledEngine::ڈھونڈو_Breaks( UText *متن,
                             int32_t آغاز_Pos,
                             int32_t انتہا_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_رمز &حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    utext_سیٹ_Native_اشاریہ(متن, آغاز_Pos);
    UChar32 c = utext_current32(متن);
    جب (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < انتہا_Pos && fHandled->شامل_ہے(c)) {
        utext_next32(متن);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(متن);
    }
    واپس 0;
}

خالی
UnhandledEngine::handleCharacter(UChar32 c) {
    اگر (fHandled == خالی_اشاریہ) {
        fHandled = نیا Unicode_سیٹ();
        اگر (fHandled == خالی_اشاریہ) {
            واپس;
        }
    }
    اگر (!fHandled->شامل_ہے(c)) {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_قدر(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_قدر(UCHAR_SCRIPT, script, حالت_2);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_رمز &/*حالت*/) {
    fEngines = خالی_اشاریہ;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    حذف fEngines;
}

خالی ICULanguageBreakFactory::ensureEngines(UError_رمز& حالت_2) {
    ساکن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    اگر (fEngines == خالی_اشاریہ) {
        Local_اشارہ_گر<UStack>  engines(نیا UStack(uprv_deleteUObject, خالی_اشاریہ, حالت_2), حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
            fEngines = engines.orphan();
        }
    }
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ثابت حرف* locale) {
    ثابت LanguageBreakEngine *lbe = خالی_اشاریہ;
    UError_رمز  حالت_2 = U_ZERO_ERROR;
    ensureEngines(حالت_2);
    اگر (U_FAILURE(حالت_2) ) {
        // Note: no way to return error code to caller.
        واپس خالی_اشاریہ;
    }

    ساکن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    جب (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fEngines->elementAt(i));
        اگر (lbe != خالی_اشاریہ && lbe->handles(c, locale)) {
            واپس lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = بھرو_Engine_For(c, locale);
    اگر (lbe != خالی_اشاریہ) {
        fEngines->push((خالی *)lbe, حالت_2);
    }
    واپس U_SUCCESS(حالت_2) ? lbe : خالی_اشاریہ;
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::بھرو_Engine_For(UChar32 c, ثابت حرف*) {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    UScript_رمز رمز = uscript_getScript(c, &حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        ثابت LanguageBreakEngine *engine = خالی_اشاریہ;
        // Try to use LSTM first
        ثابت LSTMData *ڈیٹا = CreateLSTMDataForScript(رمز, حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
            اگر (ڈیٹا != خالی_اشاریہ) {
                engine = CreateLSTMBreakEngine(رمز, ڈیٹا, حالت_2);
                اگر (U_SUCCESS(حالت_2) && engine != خالی_اشاریہ) {
                    واپس engine;
                }
                اگر (engine != خالی_اشاریہ) {
                    حذف engine;
                    engine = خالی_اشاریہ;
                } ورنہ {
                    DeleteLSTMData(ڈیٹا);
                }
            }
        }
        حالت_2 = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = بھرو_Dictionary_Matcher_For(رمز);
        اگر (m != خالی_اشاریہ) {
            انتخاب(رمز) {
            حالت USCRIPT_THAI:
                engine = نیا ThaiBreakEngine(m, حالت_2);
                توڑو;
            حالت USCRIPT_LAO:
                engine = نیا LaoBreakEngine(m, حالت_2);
                توڑو;
            حالت USCRIPT_MYANMAR:
                engine = نیا BurmeseBreakEngine(m, حالت_2);
                توڑو;
            حالت USCRIPT_KHMER:
                engine = نیا KhmerBreakEngine(m, حالت_2);
                توڑو;

#اگر !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            حالت USCRIPT_HANGUL:
                engine = نیا CjkBreakEngine(m, kKorean, حالت_2);
                توڑو;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            حالت USCRIPT_HIRAGANA:
            حالت USCRIPT_KATAKANA:
            حالت USCRIPT_HAN:
                engine = نیا CjkBreakEngine(m, kChineseJapanese, حالت_2);
                توڑو;
#اگر 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            حالت USCRIPT_COMMON:
            {
                UBlock_رمز بلاک = ublock_get_رمز(رمز);
                اگر (بلاک == UBLOCK_HIRAGANA || بلاک == UBLOCK_KATAKANA)
                   engine = نیا CjkBreakEngine(dict, kChineseJapanese, حالت_2);
                توڑو;
            }
#اگر_ختم
#اگر_ختم

            طےشدہ:
                توڑو;
            }
            اگر (engine == خالی_اشاریہ) {
                حذف m;
            }
            ورنہ اگر (U_FAILURE(حالت_2)) {
                حذف engine;
                engine = خالی_اشاریہ;
            }
            واپس engine;
        }
    }
    واپس خالی_اشاریہ;
}

DictionaryMatcher *
ICULanguageBreakFactory::بھرو_Dictionary_Matcher_For(UScript_رمز script) { 
    UError_رمز حالت_2 = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &حالت_2);
    b = ures_get_By_کلید_With_Fallback(b, "dictionaries", b, &حالت_2);
    int32_t dictnlength = 0;
    ثابت char16_t *dictfname =
        ures_get_String_By_کلید_With_Fallback(b, uscript_getShortName(script), &dictnlength, &حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        ures_close(b);
        واپس خالی_اشاریہ;
    }
    CharString dictnbuf;
    CharString ext;
    ثابت char16_t *ext_آغاز = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    اگر (ext_آغاز != خالی_اشاریہ) {
        int32_t len = static_cast<int32_t>(ext_آغاز - dictfname);
        ext.appendInvariantChars(UnicodeString(جھوٹ, ext_آغاز + 1, dictnlength - len - 1), حالت_2);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(جھوٹ, dictfname, dictnlength), حالت_2);
    ures_close(b);

    UData_حافظہ *file = udata_open(U_ICUDATA_BRKITR, ext.ڈیٹا(), dictnbuf.ڈیٹا(), &حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        // build trie
        ثابت uint8_t* ڈیٹا = static_cast<ثابت uint8_t*>(udata_get_حافظہ(file));
        ثابت int32_t* indexes = reinterpret_cast<ثابت int32_t*>(ڈیٹا);
        ثابت int32_t انحراف = indexes[Dictionary_ڈیٹا::IX_STRING_TRIE_انحراف];
        ثابت int32_t trieType = indexes[Dictionary_ڈیٹا::IX_TRIE_TYPE] & Dictionary_ڈیٹا::TRIE_TYPE_نقاب;
        DictionaryMatcher *m = خالی_اشاریہ;
        اگر (trieType == Dictionary_ڈیٹا::TRIE_TYPE_بائٹیں) {
            ثابت int32_t transform = indexes[Dictionary_ڈیٹا::IX_TRANSFORM];
            ثابت حرف* characters = reinterpret_cast<ثابت حرف*>(ڈیٹا + انحراف);
            m = نیا بائٹیں_Dictionary_Matcher(characters, transform, file);
        }
        ورنہ اگر (trieType == Dictionary_ڈیٹا::TRIE_TYPE_UCHARS) {
            ثابت char16_t* characters = reinterpret_cast<ثابت char16_t*>(ڈیٹا + انحراف);
            m = نیا UCharsDictionaryMatcher(characters, file);
        }
        اگر (m == خالی_اشاریہ) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        واپس m;
    } ورنہ اگر (dictfname != خالی_اشاریہ) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        حالت_2 = U_ZERO_ERROR;
        واپس خالی_اشاریہ;
    }
    واپس خالی_اشاریہ;
}


خالی ICULanguageBreakFactory::جوڑو_External_Engine(
        ExternalBreakEngine* external, UError_رمز& حالت_2) {
    Local_اشارہ_گر<ExternalBreakEngine> engine(external, حالت_2);
    ensureEngines(حالت_2);
    Local_اشارہ_گر<BreakEngineWrapper> wrapper(
        نیا BreakEngineWrapper(engine.orphan(), حالت_2), حالت_2);
    ساکن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), حالت_2);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_رمز &حالت_2) : delegate(engine, حالت_2) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ثابت حرف* locale) ثابت {
    واپس delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::ڈھونڈو_Breaks(
    UText *متن,
    int32_t آغاز_Pos,
    int32_t انتہا_Pos,
    UVector32 &ملا_Breaks,
    UBool /* isPhraseBreaking */,
    UError_رمز &حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    int32_t نتیجہ = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_سیٹ_Native_اشاریہ(متن, آغاز_Pos);
    int32_t آغاز = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    int32_t current;
    int32_t range_آغاز;
    int32_t range_انتہا;
    UChar32 c = utext_current32(متن);
    جب ((current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < انتہا_Pos && delegate->handles(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_آغاز = آغاز;
    range_انتہا = current;
    int32_t before_سائز = ملا_Breaks.size();
    int32_t additional_استعداد = range_انتہا - range_آغاز + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    ملا_Breaks.ensure_استعداد(before_سائز+additional_استعداد, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس 0;
    ملا_Breaks.سیٹ_سائز(before_سائز + before_سائز+additional_استعداد);
    نتیجہ = delegate->fillBreaks(متن, range_آغاز, range_انتہا, ملا_Breaks.get_بفر()+before_سائز,
                                  additional_استعداد, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس 0;
    ملا_Breaks.سیٹ_سائز(before_سائز + نتیجہ);
    utext_سیٹ_Native_اشاریہ(متن, current);
    واپس نتیجہ;
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */
