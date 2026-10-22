// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#include "unicode/utypes.h"

#якщо !UCONFIG_NO_BREAK_ITERATION

#include "unicode/uchar.h"
#include "unicode/uniset.h"
#include "unicode/chariter.h"
#include "unicode/ures.h"
#include "unicode/udata.h"
#include "unicode/putil.h"
#include "unicode/ustring.h"
#include "unicode/uscript.h"
#include "unicode/ucharstrie.h"
#include "unicode/bytestrie.h"
#include "unicode/rbbi.h"

#include "brkeng.h"
#include "cmemory.h"
#include "dictbe.h"
#include "lstmbe.h"
#include "charstr.h"
#include "dictionarydata.h"
#include "mutex.h"
#include "uvector.h"
#include "umutex.h"
#include "uresimp.h"
#include "ubrkimpl.h"

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
    (void)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, const char* locale) const {
    (void)locale; // Unused
    повернути fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) const {
    якщо (U_FAILURE(status)) повернути 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    поки (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    повернути 0;
}

void
UnhandledEngine::handleCharacter(UChar32 c) {
    якщо (fHandled == nullptr) {
        fHandled = new UnicodeSet();
        якщо (fHandled == nullptr) {
            повернути;
        }
    }
    якщо (!fHandled->contains(c)) {
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

void ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    static UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    якщо (fEngines == nullptr) {
        LocalPointer<UStack>  engines(new UStack(uprv_deleteUObject, nullptr, status), status);
        якщо (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

const LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, const char* locale) {
    const LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    якщо (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        повернути nullptr;
    }

    static UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    поки (--i >= 0) {
        lbe = static_cast<const LanguageBreakEngine*>(fEngines->elementAt(i));
        якщо (lbe != nullptr && lbe->handles(c, locale)) {
            повернути lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    якщо (lbe != nullptr) {
        fEngines->push((void *)lbe, status);
    }
    повернути U_SUCCESS(status) ? lbe : nullptr;
}

const LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, const char*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    якщо (U_SUCCESS(status)) {
        const LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        const LSTMData *data = CreateLSTMDataForScript(code, status);
        якщо (U_SUCCESS(status)) {
            якщо (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                якщо (U_SUCCESS(status) && engine != nullptr) {
                    повернути engine;
                }
                якщо (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } інакше {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        якщо (m != nullptr) {
            switch(code) {
            case USCRIPT_THAI:
                engine = new ThaiBreakEngine(m, status);
                break;
            case USCRIPT_LAO:
                engine = new LaoBreakEngine(m, status);
                break;
            case USCRIPT_MYANMAR:
                engine = new BurmeseBreakEngine(m, status);
                break;
            case USCRIPT_KHMER:
                engine = new KhmerBreakEngine(m, status);
                break;

#якщо !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            case USCRIPT_HANGUL:
                engine = new CjkBreakEngine(m, kKorean, status);
                break;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            case USCRIPT_HIRAGANA:
            case USCRIPT_KATAKANA:
            case USCRIPT_HAN:
                engine = new CjkBreakEngine(m, kChineseJapanese, status);
                break;
#якщо 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            case USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                якщо (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = new CjkBreakEngine(dict, kChineseJapanese, status);
                break;
            }
#endif
#endif

            default:
                break;
            }
            якщо (engine == nullptr) {
                delete m;
            }
            інакше якщо (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            повернути engine;
        }
    }
    повернути nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_getByKeyWithFallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    const char16_t *dictfname =
        ures_getStringByKeyWithFallback(b, uscript_getShortName(script), &dictnlength, &status);
    якщо (U_FAILURE(status)) {
        ures_close(b);
        повернути nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    const char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    якщо (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(false, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(false, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    якщо (U_SUCCESS(status)) {
        // build trie
        const uint8_t* data = static_cast<const uint8_t*>(udata_getMemory(file));
        const int32_t* indexes = reinterpret_cast<const int32_t*>(data);
        const int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        const int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        якщо (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            const int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            const char* characters = reinterpret_cast<const char*>(data + offset);
            m = new BytesDictionaryMatcher(characters, transform, file);
        }
        інакше якщо (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            const char16_t* characters = reinterpret_cast<const char16_t*>(data + offset);
            m = new UCharsDictionaryMatcher(characters, file);
        }
        якщо (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        повернути m;
    } інакше якщо (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        повернути nullptr;
    }
    повернути nullptr;
}


void ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        new BreakEngineWrapper(engine.orphan(), status), status);
    static UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, const char* locale) const {
    повернути delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) const {
    якщо (U_FAILURE(status)) повернути 0;
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
    поки ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    якщо (U_FAILURE(status)) повернути 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    якщо (U_FAILURE(status)) повернути 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    повернути result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
