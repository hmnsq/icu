// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/chariter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucharstrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/rbbi.h"

#ଅନ୍ତର୍ଭାଵ "brkeng.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "dictbe.h"
#ଅନ୍ତର୍ଭାଵ "lstmbe.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "dictionarydata.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "ubrkimpl.h"

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
    (ଶୂନ୍ୟ)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, ଅଚଳ ଅକ୍ଷର* locale) ଅଚଳ {
    (ଶୂନ୍ୟ)locale; // Unused
    ଫେରନ୍ତୁ fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    ଯେତେବେଳେ (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ
UnhandledEngine::handleCharacter(UChar32 c) {
    ଯଦି (fHandled == nullptr) {
        fHandled = ନୂତନ UnicodeSet();
        ଯଦି (fHandled == nullptr) {
            ଫେରନ୍ତୁ;
        }
    }
    ଯଦି (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_ମୂଲ୍ୟ(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_ମୂଲ୍ୟ(UCHAR_SCRIPT, script, status);
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

ଶୂନ୍ୟ ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    ସ୍ଥିର UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    ଯଦି (fEngines == nullptr) {
        LocalPointer<UStack>  engines(ନୂତନ UStack(uprv_deleteUObject, nullptr, status), status);
        ଯଦି (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

ଅଚଳ LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ଅଚଳ ଅକ୍ଷର* locale) {
    ଅଚଳ LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    ଯଦି (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        ଫେରନ୍ତୁ nullptr;
    }

    ସ୍ଥିର UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    ଯେତେବେଳେ (--i >= 0) {
        lbe = static_cast<ଅଚଳ LanguageBreakEngine*>(fEngines->elementAt(i));
        ଯଦି (lbe != nullptr && lbe->handles(c, locale)) {
            ଫେରନ୍ତୁ lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    ଯଦି (lbe != nullptr) {
        fEngines->push((ଶୂନ୍ୟ *)lbe, status);
    }
    ଫେରନ୍ତୁ U_SUCCESS(status) ? lbe : nullptr;
}

ଅଚଳ LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, ଅଚଳ ଅକ୍ଷର*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    ଯଦି (U_SUCCESS(status)) {
        ଅଚଳ LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        ଅଚଳ LSTMData *data = CreateLSTMDataForScript(code, status);
        ଯଦି (U_SUCCESS(status)) {
            ଯଦି (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                ଯଦି (U_SUCCESS(status) && engine != nullptr) {
                    ଫେରନ୍ତୁ engine;
                }
                ଯଦି (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } ଅନ୍ୟଥା {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        ଯଦି (m != nullptr) {
            ବାଛ(code) {
            ସ୍ଥିତି USCRIPT_THAI:
                engine = ନୂତନ ThaiBreakEngine(m, status);
                ଭାଙ୍ଗ;
            ସ୍ଥିତି USCRIPT_LAO:
                engine = ନୂତନ LaoBreakEngine(m, status);
                ଭାଙ୍ଗ;
            ସ୍ଥିତି USCRIPT_MYANMAR:
                engine = ନୂତନ BurmeseBreakEngine(m, status);
                ଭାଙ୍ଗ;
            ସ୍ଥିତି USCRIPT_KHMER:
                engine = ନୂତନ KhmerBreakEngine(m, status);
                ଭାଙ୍ଗ;

#ଯଦି !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            ସ୍ଥିତି USCRIPT_HANGUL:
                engine = ନୂତନ CjkBreakEngine(m, kKorean, status);
                ଭାଙ୍ଗ;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            ସ୍ଥିତି USCRIPT_HIRAGANA:
            ସ୍ଥିତି USCRIPT_KATAKANA:
            ସ୍ଥିତି USCRIPT_HAN:
                engine = ନୂତନ CjkBreakEngine(m, kChineseJapanese, status);
                ଭାଙ୍ଗ;
#ଯଦି 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            ସ୍ଥିତି USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                ଯଦି (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = ନୂତନ CjkBreakEngine(dict, kChineseJapanese, status);
                ଭାଙ୍ଗ;
            }
#endif
#endif

            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଭାଙ୍ଗ;
            }
            ଯଦି (engine == nullptr) {
                delete m;
            }
            ଅନ୍ୟଥା ଯଦି (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            ଫେରନ୍ତୁ engine;
        }
    }
    ଫେରନ୍ତୁ nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_ଚାବି_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    ଅଚଳ char16_t *dictfname =
        ures_get_String_By_ଚାବି_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    ଯଦି (U_FAILURE(status)) {
        ures_close(b);
        ଫେରନ୍ତୁ nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    ଅଚଳ char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    ଯଦି (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(ମିଥ୍ୟା, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(ମିଥ୍ୟା, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    ଯଦି (U_SUCCESS(status)) {
        // build trie
        ଅଚଳ uint8_t* data = static_cast<ଅଚଳ uint8_t*>(udata_getMemory(file));
        ଅଚଳ int32_t* indexes = reinterpret_cast<ଅଚଳ int32_t*>(data);
        ଅଚଳ int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        ଅଚଳ int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        ଯଦି (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            ଅଚଳ int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            ଅଚଳ ଅକ୍ଷର* characters = reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(data + offset);
            m = ନୂତନ BytesDictionaryMatcher(characters, transform, file);
        }
        ଅନ୍ୟଥା ଯଦି (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ଅଚଳ char16_t* characters = reinterpret_cast<ଅଚଳ char16_t*>(data + offset);
            m = ନୂତନ UCharsDictionaryMatcher(characters, file);
        }
        ଯଦି (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        ଫେରନ୍ତୁ m;
    } ଅନ୍ୟଥା ଯଦି (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ nullptr;
}


ଶୂନ୍ୟ ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        ନୂତନ BreakEngineWrapper(engine.orphan(), status), status);
    ସ୍ଥିର UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ଅଚଳ ଅକ୍ଷର* locale) ଅଚଳ {
    ଫେରନ୍ତୁ delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;
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
    ଯେତେବେଳେ ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    ଫେରନ୍ତୁ result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
