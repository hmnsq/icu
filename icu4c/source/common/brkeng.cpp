// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#include "unicode/utypes.h"

#ᨀᨘ !UCONFIG_NO_BREAK_ITERATION

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
UnhandledEngine::handles(UChar32 c, const var_na* locale) const {
    (void)locale; // Unused
    ᨀᨘᨒ fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) const {
    ᨀᨘ (U_FAILURE(status)) ᨀᨘᨒ 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    ᨀᨛ (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    ᨀᨘᨒ 0;
}

void
UnhandledEngine::handleCharacter(UChar32 c) {
    ᨀᨘ (fHandled == nullptr) {
        fHandled = new UnicodeSet();
        ᨀᨘ (fHandled == nullptr) {
            ᨀᨘᨒ;
        }
    }
    ᨀᨘ (!fHandled->contains(c)) {
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
    ᨀᨘ (fEngines == nullptr) {
        LocalPointer<UStack>  engines(new UStack(uprv_deleteUObject, nullptr, status), status);
        ᨀᨘ (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

const LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, const var_na* locale) {
    const LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    ᨀᨘ (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        ᨀᨘᨒ nullptr;
    }

    static UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    ᨀᨛ (--i >= 0) {
        lbe = static_cast<const LanguageBreakEngine*>(fEngines->elementAt(i));
        ᨀᨘ (lbe != nullptr && lbe->handles(c, locale)) {
            ᨀᨘᨒ lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    ᨀᨘ (lbe != nullptr) {
        fEngines->push((void *)lbe, status);
    }
    ᨀᨘᨒ U_SUCCESS(status) ? lbe : nullptr;
}

const LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, const var_na*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    ᨀᨘ (U_SUCCESS(status)) {
        const LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        const LSTMData *data = CreateLSTMDataForScript(code, status);
        ᨀᨘ (U_SUCCESS(status)) {
            ᨀᨘ (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                ᨀᨘ (U_SUCCESS(status) && engine != nullptr) {
                    ᨀᨘᨒ engine;
                }
                ᨀᨘ (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } ᨀᨘᨁ {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        ᨀᨘ (m != nullptr) {
            switch(code) {
            pak_sa USCRIPT_THAI:
                engine = new ThaiBreakEngine(m, status);
                bha_nga;
            pak_sa USCRIPT_LAO:
                engine = new LaoBreakEngine(m, status);
                bha_nga;
            pak_sa USCRIPT_MYANMAR:
                engine = new BurmeseBreakEngine(m, status);
                bha_nga;
            pak_sa USCRIPT_KHMER:
                engine = new KhmerBreakEngine(m, status);
                bha_nga;

#ᨀᨘ !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            pak_sa USCRIPT_HANGUL:
                engine = new CjkBreakEngine(m, kKorean, status);
                bha_nga;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            pak_sa USCRIPT_HIRAGANA:
            pak_sa USCRIPT_KATAKANA:
            pak_sa USCRIPT_HAN:
                engine = new CjkBreakEngine(m, kChineseJapanese, status);
                bha_nga;
#ᨀᨘ 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            pak_sa USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                ᨀᨘ (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = new CjkBreakEngine(dict, kChineseJapanese, status);
                bha_nga;
            }
#endif
#endif

            default:
                bha_nga;
            }
            ᨀᨘ (engine == nullptr) {
                delete m;
            }
            ᨀᨘᨁ ᨀᨘ (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            ᨀᨘᨒ engine;
        }
    }
    ᨀᨘᨒ nullptr;
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
    ᨀᨘ (U_FAILURE(status)) {
        ures_close(b);
        ᨀᨘᨒ nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    const char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    ᨀᨘ (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(false, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(false, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    ᨀᨘ (U_SUCCESS(status)) {
        // build trie
        const uint8_t* data = static_cast<const uint8_t*>(udata_getMemory(file));
        const int32_t* indexes = reinterpret_cast<const int32_t*>(data);
        const int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        const int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        ᨀᨘ (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            const int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            const var_na* characters = reinterpret_cast<const var_na*>(data + offset);
            m = new BytesDictionaryMatcher(characters, transform, file);
        }
        ᨀᨘᨁ ᨀᨘ (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            const char16_t* characters = reinterpret_cast<const char16_t*>(data + offset);
            m = new UCharsDictionaryMatcher(characters, file);
        }
        ᨀᨘ (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        ᨀᨘᨒ m;
    } ᨀᨘᨁ ᨀᨘ (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        ᨀᨘᨒ nullptr;
    }
    ᨀᨘᨒ nullptr;
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

UBool BreakEngineWrapper::handles(UChar32 c, const var_na* locale) const {
    ᨀᨘᨒ delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) const {
    ᨀᨘ (U_FAILURE(status)) ᨀᨘᨒ 0;
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
    ᨀᨛ ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    ᨀᨘ (U_FAILURE(status)) ᨀᨘᨒ 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    ᨀᨘ (U_FAILURE(status)) ᨀᨘᨒ 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    ᨀᨘᨒ result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
