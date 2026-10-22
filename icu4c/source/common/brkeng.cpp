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

UnhandledEngine::UnhandledEngine(UErrorCode &status) : fHandled(nullptr) {
    (تهی)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, ثابت نویسه* locale) ثابت {
    (تهی)locale; // Unused
    بازگشت fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) ثابت {
    اگر (U_FAILURE(status)) بازگشت 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    وقتی (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    بازگشت 0;
}

تهی
UnhandledEngine::handleCharacter(UChar32 c) {
    اگر (fHandled == nullptr) {
        fHandled = جدید UnicodeSet();
        اگر (fHandled == nullptr) {
            بازگشت;
        }
    }
    اگر (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_مقدار(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_مقدار(UCHAR_SCRIPT, script, status);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UErrorCode &/*status*/) {
    fEngines = nullptr;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    delete fEngines;
}

تهی ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    ایستا UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    اگر (fEngines == nullptr) {
        LocalPointer<UStack>  engines(جدید UStack(uprv_deleteUObject, nullptr, status), status);
        اگر (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ثابت نویسه* locale) {
    ثابت LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    اگر (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        بازگشت nullptr;
    }

    ایستا UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    وقتی (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fEngines->elementAt(i));
        اگر (lbe != nullptr && lbe->handles(c, locale)) {
            بازگشت lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    اگر (lbe != nullptr) {
        fEngines->push((تهی *)lbe, status);
    }
    بازگشت U_SUCCESS(status) ? lbe : nullptr;
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, ثابت نویسه*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    اگر (U_SUCCESS(status)) {
        ثابت LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        ثابت LSTMData *data = CreateLSTMDataForScript(code, status);
        اگر (U_SUCCESS(status)) {
            اگر (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                اگر (U_SUCCESS(status) && engine != nullptr) {
                    بازگشت engine;
                }
                اگر (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } وگرنه {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        اگر (m != nullptr) {
            انتخاب(code) {
            حالت USCRIPT_THAI:
                engine = جدید ThaiBreakEngine(m, status);
                بشکن;
            حالت USCRIPT_LAO:
                engine = جدید LaoBreakEngine(m, status);
                بشکن;
            حالت USCRIPT_MYANMAR:
                engine = جدید BurmeseBreakEngine(m, status);
                بشکن;
            حالت USCRIPT_KHMER:
                engine = جدید KhmerBreakEngine(m, status);
                بشکن;

#اگر !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            حالت USCRIPT_HANGUL:
                engine = جدید CjkBreakEngine(m, kKorean, status);
                بشکن;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            حالت USCRIPT_HIRAGANA:
            حالت USCRIPT_KATAKANA:
            حالت USCRIPT_HAN:
                engine = جدید CjkBreakEngine(m, kChineseJapanese, status);
                بشکن;
#اگر 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            حالت USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                اگر (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = جدید CjkBreakEngine(dict, kChineseJapanese, status);
                بشکن;
            }
#endif
#endif

            پیشفرض:
                بشکن;
            }
            اگر (engine == nullptr) {
                delete m;
            }
            وگرنه اگر (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            بازگشت engine;
        }
    }
    بازگشت nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_کلید_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    ثابت char16_t *dictfname =
        ures_get_String_By_کلید_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    اگر (U_FAILURE(status)) {
        ures_close(b);
        بازگشت nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    ثابت char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    اگر (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(نادرست, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(نادرست, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    اگر (U_SUCCESS(status)) {
        // build trie
        ثابت uint8_t* data = static_cast<ثابت uint8_t*>(udata_getMemory(file));
        ثابت int32_t* indexes = reinterpret_cast<ثابت int32_t*>(data);
        ثابت int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        ثابت int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        اگر (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            ثابت int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            ثابت نویسه* characters = reinterpret_cast<ثابت نویسه*>(data + offset);
            m = جدید BytesDictionaryMatcher(characters, transform, file);
        }
        وگرنه اگر (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ثابت char16_t* characters = reinterpret_cast<ثابت char16_t*>(data + offset);
            m = جدید UCharsDictionaryMatcher(characters, file);
        }
        اگر (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        بازگشت m;
    } وگرنه اگر (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        بازگشت nullptr;
    }
    بازگشت nullptr;
}


تهی ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        جدید BreakEngineWrapper(engine.orphan(), status), status);
    ایستا UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ثابت نویسه* locale) ثابت {
    بازگشت delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) ثابت {
    اگر (U_FAILURE(status)) بازگشت 0;
    int32_t result = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_setNativeIndex(text, startPos);
    int32_t start = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t current;
    int32_t rangeStart;
    int32_t rangeEnd;
    UChar32 c = utext_current32(text);
    وقتی ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    اگر (U_FAILURE(status)) بازگشت 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    اگر (U_FAILURE(status)) بازگشت 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    بازگشت result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
