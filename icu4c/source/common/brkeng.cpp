// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "dictbe.h"
#ಅನ್ತರ್ಭಾವ "lstmbe.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "dictionarydata.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "ubrkimpl.h"

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
    (ಶೂನ್ಯ)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, ಧ್ರುವ ವರ್ಣ* locale) ಧ್ರುವ {
    (ಶೂನ್ಯ)locale; // Unused
    ಪ್ರತ್ಯಾಗಮ fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    ಯಾವತ್ (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ
UnhandledEngine::handleCharacter(UChar32 c) {
    ಯದಿ (fHandled == nullptr) {
        fHandled = ನೂತನ UnicodeSet();
        ಯದಿ (fHandled == nullptr) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    ಯದಿ (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_ಮೌಲ್ಯ(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_ಮೌಲ್ಯ(UCHAR_SCRIPT, script, status);
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

ಶೂನ್ಯ ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    ಸ್ಥಿರ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    ಯದಿ (fEngines == nullptr) {
        LocalPointer<UStack>  engines(ನೂತನ UStack(uprv_deleteUObject, nullptr, status), status);
        ಯದಿ (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

ಧ್ರುವ LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ಧ್ರುವ ವರ್ಣ* locale) {
    ಧ್ರುವ LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    ಯದಿ (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಸ್ಥಿರ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    ಯಾವತ್ (--i >= 0) {
        lbe = static_cast<ಧ್ರುವ LanguageBreakEngine*>(fEngines->elementAt(i));
        ಯದಿ (lbe != nullptr && lbe->handles(c, locale)) {
            ಪ್ರತ್ಯಾಗಮ lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    ಯದಿ (lbe != nullptr) {
        fEngines->push((ಶೂನ್ಯ *)lbe, status);
    }
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(status) ? lbe : nullptr;
}

ಧ್ರುವ LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, ಧ್ರುವ ವರ್ಣ*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    ಯದಿ (U_SUCCESS(status)) {
        ಧ್ರುವ LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        ಧ್ರುವ LSTMData *data = CreateLSTMDataForScript(code, status);
        ಯದಿ (U_SUCCESS(status)) {
            ಯದಿ (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                ಯದಿ (U_SUCCESS(status) && engine != nullptr) {
                    ಪ್ರತ್ಯಾಗಮ engine;
                }
                ಯದಿ (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } ಅನ್ಯಥಾ {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        ಯದಿ (m != nullptr) {
            ವಿಕಲ್ಪ(code) {
            ಪಕ್ಷ USCRIPT_THAI:
                engine = ನೂತನ ThaiBreakEngine(m, status);
                ಭಙ್ಗ;
            ಪಕ್ಷ USCRIPT_LAO:
                engine = ನೂತನ LaoBreakEngine(m, status);
                ಭಙ್ಗ;
            ಪಕ್ಷ USCRIPT_MYANMAR:
                engine = ನೂತನ BurmeseBreakEngine(m, status);
                ಭಙ್ಗ;
            ಪಕ್ಷ USCRIPT_KHMER:
                engine = ನೂತನ KhmerBreakEngine(m, status);
                ಭಙ್ಗ;

#ಯದಿ !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            ಪಕ್ಷ USCRIPT_HANGUL:
                engine = ನೂತನ CjkBreakEngine(m, kKorean, status);
                ಭಙ್ಗ;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            ಪಕ್ಷ USCRIPT_HIRAGANA:
            ಪಕ್ಷ USCRIPT_KATAKANA:
            ಪಕ್ಷ USCRIPT_HAN:
                engine = ನೂತನ CjkBreakEngine(m, kChineseJapanese, status);
                ಭಙ್ಗ;
#ಯದಿ 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            ಪಕ್ಷ USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                ಯದಿ (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = ನೂತನ CjkBreakEngine(dict, kChineseJapanese, status);
                ಭಙ್ಗ;
            }
#endif
#endif

            ಸ್ವಭಾವ:
                ಭಙ್ಗ;
            }
            ಯದಿ (engine == nullptr) {
                delete m;
            }
            ಅನ್ಯಥಾ ಯದಿ (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            ಪ್ರತ್ಯಾಗಮ engine;
        }
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_ಕೀಲಿ_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    ಧ್ರುವ char16_t *dictfname =
        ures_get_String_By_ಕೀಲಿ_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    ಯದಿ (U_FAILURE(status)) {
        ures_close(b);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    ಧ್ರುವ char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    ಯದಿ (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(ಅಸತ್ಯ, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(ಅಸತ್ಯ, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    ಯದಿ (U_SUCCESS(status)) {
        // build trie
        ಧ್ರುವ uint8_t* data = static_cast<ಧ್ರುವ uint8_t*>(udata_getMemory(file));
        ಧ್ರುವ int32_t* indexes = reinterpret_cast<ಧ್ರುವ int32_t*>(data);
        ಧ್ರುವ int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        ಧ್ರುವ int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        ಯದಿ (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            ಧ್ರುವ int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            ಧ್ರುವ ವರ್ಣ* characters = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(data + offset);
            m = ನೂತನ BytesDictionaryMatcher(characters, transform, file);
        }
        ಅನ್ಯಥಾ ಯದಿ (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ಧ್ರುವ char16_t* characters = reinterpret_cast<ಧ್ರುವ char16_t*>(data + offset);
            m = ನೂತನ UCharsDictionaryMatcher(characters, file);
        }
        ಯದಿ (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        ಪ್ರತ್ಯಾಗಮ m;
    } ಅನ್ಯಥಾ ಯದಿ (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}


ಶೂನ್ಯ ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        ನೂತನ BreakEngineWrapper(engine.orphan(), status), status);
    ಸ್ಥಿರ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ಧ್ರುವ ವರ್ಣ* locale) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;
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
    ಯಾವತ್ ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    ಪ್ರತ್ಯಾಗಮ result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
