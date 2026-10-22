// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_BREAK_ITERATION

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

UnhandledEngine::UnhandledEngine(UErrorCode &status) : fHandled(nullptr) {
    (فراغ)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, ثابت حرف* locale) ثابت {
    (فراغ)locale; // Unused
    ܦܢܐ fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) ثابت {
    ܐܢ (U_FAILURE(status)) ܦܢܐ 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    ܟܡܐ (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    ܦܢܐ 0;
}

فراغ
UnhandledEngine::handleCharacter(UChar32 c) {
    ܐܢ (fHandled == nullptr) {
        fHandled = جديد UnicodeSet();
        ܐܢ (fHandled == nullptr) {
            ܦܢܐ;
        }
    }
    ܐܢ (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_getIntPropertyValue(c, UCHAR_SCRIPT);
        fHandled->applyIntPropertyValue(UCHAR_SCRIPT, script, status);
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

فراغ ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    ܐܢ (fEngines == nullptr) {
        LocalPointer<UStack>  engines(جديد UStack(uprv_deleteUObject, nullptr, status), status);
        ܐܢ (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ثابت حرف* locale) {
    ثابت LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    ܐܢ (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        ܦܢܐ nullptr;
    }

    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    ܟܡܐ (--i >= 0) {
        lbe = static_cast<ثابت LanguageBreakEngine*>(fEngines->elementAt(i));
        ܐܢ (lbe != nullptr && lbe->handles(c, locale)) {
            ܦܢܐ lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    ܐܢ (lbe != nullptr) {
        fEngines->push((فراغ *)lbe, status);
    }
    ܦܢܐ U_SUCCESS(status) ? lbe : nullptr;
}

ثابت LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, ثابت حرف*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    ܐܢ (U_SUCCESS(status)) {
        ثابت LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        ثابت LSTMData *data = CreateLSTMDataForScript(code, status);
        ܐܢ (U_SUCCESS(status)) {
            ܐܢ (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                ܐܢ (U_SUCCESS(status) && engine != nullptr) {
                    ܦܢܐ engine;
                }
                ܐܢ (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } ܐܚܪܢܐ {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        ܐܢ (m != nullptr) {
            بدل(code) {
            حالة USCRIPT_THAI:
                engine = جديد ThaiBreakEngine(m, status);
                اكسر;
            حالة USCRIPT_LAO:
                engine = جديد LaoBreakEngine(m, status);
                اكسر;
            حالة USCRIPT_MYANMAR:
                engine = جديد BurmeseBreakEngine(m, status);
                اكسر;
            حالة USCRIPT_KHMER:
                engine = جديد KhmerBreakEngine(m, status);
                اكسر;

#ܐܢ !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            حالة USCRIPT_HANGUL:
                engine = جديد CjkBreakEngine(m, kKorean, status);
                اكسر;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            حالة USCRIPT_HIRAGANA:
            حالة USCRIPT_KATAKANA:
            حالة USCRIPT_HAN:
                engine = جديد CjkBreakEngine(m, kChineseJapanese, status);
                اكسر;
#ܐܢ 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            حالة USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                ܐܢ (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = جديد CjkBreakEngine(dict, kChineseJapanese, status);
                اكسر;
            }
#endif
#endif

            افتراضي:
                اكسر;
            }
            ܐܢ (engine == nullptr) {
                delete m;
            }
            ܐܚܪܢܐ ܐܢ (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            ܦܢܐ engine;
        }
    }
    ܦܢܐ nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_getByKeyWithFallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    ثابت char16_t *dictfname =
        ures_getStringByKeyWithFallback(b, uscript_getShortName(script), &dictnlength, &status);
    ܐܢ (U_FAILURE(status)) {
        ures_close(b);
        ܦܢܐ nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    ثابت char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    ܐܢ (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(ܕܓܠܐ, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(ܕܓܠܐ, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    ܐܢ (U_SUCCESS(status)) {
        // build trie
        ثابت uint8_t* data = static_cast<ثابت uint8_t*>(udata_getMemory(file));
        ثابت int32_t* indexes = reinterpret_cast<ثابت int32_t*>(data);
        ثابت int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        ثابت int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        ܐܢ (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            ثابت int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            ثابت حرف* characters = reinterpret_cast<ثابت حرف*>(data + offset);
            m = جديد BytesDictionaryMatcher(characters, transform, file);
        }
        ܐܚܪܢܐ ܐܢ (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ثابت char16_t* characters = reinterpret_cast<ثابت char16_t*>(data + offset);
            m = جديد UCharsDictionaryMatcher(characters, file);
        }
        ܐܢ (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        ܦܢܐ m;
    } ܐܚܪܢܐ ܐܢ (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        ܦܢܐ nullptr;
    }
    ܦܢܐ nullptr;
}


فراغ ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        جديد BreakEngineWrapper(engine.orphan(), status), status);
    ساكن UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ثابت حرف* locale) ثابت {
    ܦܢܐ delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) ثابت {
    ܐܢ (U_FAILURE(status)) ܦܢܐ 0;
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
    ܟܡܐ ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    ܐܢ (U_FAILURE(status)) ܦܢܐ 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    ܐܢ (U_FAILURE(status)) ܦܢܐ 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    ܦܢܐ result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
