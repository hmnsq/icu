// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/chariter.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "unicode/ucharstrie.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/rbbi.h"

#അന്തര്ഭാവ "brkeng.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "dictbe.h"
#അന്തര്ഭാവ "lstmbe.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "dictionarydata.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "ubrkimpl.h"

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
    (ശൂന്യം)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, സ്ഥിരം അക്ഷരം* locale) സ്ഥിരം {
    (ശൂന്യം)locale; // Unused
    തിരികെ fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    ആകുമ്പോൾ (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    തിരികെ 0;
}

ശൂന്യം
UnhandledEngine::handleCharacter(UChar32 c) {
    എങ്കിൽ (fHandled == nullptr) {
        fHandled = പുതിയ UnicodeSet();
        എങ്കിൽ (fHandled == nullptr) {
            തിരികെ;
        }
    }
    എങ്കിൽ (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_മൂല്യം(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_മൂല്യം(UCHAR_SCRIPT, script, status);
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

ശൂന്യം ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    സ്ഥിതം UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    എങ്കിൽ (fEngines == nullptr) {
        LocalPointer<UStack>  engines(പുതിയ UStack(uprv_deleteUObject, nullptr, status), status);
        എങ്കിൽ (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

സ്ഥിരം LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, സ്ഥിരം അക്ഷരം* locale) {
    സ്ഥിരം LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    എങ്കിൽ (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        തിരികെ nullptr;
    }

    സ്ഥിതം UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    ആകുമ്പോൾ (--i >= 0) {
        lbe = static_cast<സ്ഥിരം LanguageBreakEngine*>(fEngines->elementAt(i));
        എങ്കിൽ (lbe != nullptr && lbe->handles(c, locale)) {
            തിരികെ lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    എങ്കിൽ (lbe != nullptr) {
        fEngines->push((ശൂന്യം *)lbe, status);
    }
    തിരികെ U_SUCCESS(status) ? lbe : nullptr;
}

സ്ഥിരം LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, സ്ഥിരം അക്ഷരം*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    എങ്കിൽ (U_SUCCESS(status)) {
        സ്ഥിരം LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        സ്ഥിരം LSTMData *data = CreateLSTMDataForScript(code, status);
        എങ്കിൽ (U_SUCCESS(status)) {
            എങ്കിൽ (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                എങ്കിൽ (U_SUCCESS(status) && engine != nullptr) {
                    തിരികെ engine;
                }
                എങ്കിൽ (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } അല്ലെങ്കിൽ {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        എങ്കിൽ (m != nullptr) {
            തിരഞ്ഞെടുപ്പ്(code) {
            അവസ്ഥ USCRIPT_THAI:
                engine = പുതിയ ThaiBreakEngine(m, status);
                നിർത്തുക;
            അവസ്ഥ USCRIPT_LAO:
                engine = പുതിയ LaoBreakEngine(m, status);
                നിർത്തുക;
            അവസ്ഥ USCRIPT_MYANMAR:
                engine = പുതിയ BurmeseBreakEngine(m, status);
                നിർത്തുക;
            അവസ്ഥ USCRIPT_KHMER:
                engine = പുതിയ KhmerBreakEngine(m, status);
                നിർത്തുക;

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            അവസ്ഥ USCRIPT_HANGUL:
                engine = പുതിയ CjkBreakEngine(m, kKorean, status);
                നിർത്തുക;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            അവസ്ഥ USCRIPT_HIRAGANA:
            അവസ്ഥ USCRIPT_KATAKANA:
            അവസ്ഥ USCRIPT_HAN:
                engine = പുതിയ CjkBreakEngine(m, kChineseJapanese, status);
                നിർത്തുക;
#എങ്കിൽ 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            അവസ്ഥ USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                എങ്കിൽ (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = പുതിയ CjkBreakEngine(dict, kChineseJapanese, status);
                നിർത്തുക;
            }
#endif
#endif

            സ്ഥിരസ്ഥിതി:
                നിർത്തുക;
            }
            എങ്കിൽ (engine == nullptr) {
                delete m;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            തിരികെ engine;
        }
    }
    തിരികെ nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_താക്കോൽ_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    സ്ഥിരം char16_t *dictfname =
        ures_get_String_By_താക്കോൽ_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    എങ്കിൽ (U_FAILURE(status)) {
        ures_close(b);
        തിരികെ nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    സ്ഥിരം char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    എങ്കിൽ (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(അസത്യം, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(അസത്യം, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    എങ്കിൽ (U_SUCCESS(status)) {
        // build trie
        സ്ഥിരം uint8_t* data = static_cast<സ്ഥിരം uint8_t*>(udata_getMemory(file));
        സ്ഥിരം int32_t* indexes = reinterpret_cast<സ്ഥിരം int32_t*>(data);
        സ്ഥിരം int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        സ്ഥിരം int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        എങ്കിൽ (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            സ്ഥിരം int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            സ്ഥിരം അക്ഷരം* characters = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(data + offset);
            m = പുതിയ BytesDictionaryMatcher(characters, transform, file);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            സ്ഥിരം char16_t* characters = reinterpret_cast<സ്ഥിരം char16_t*>(data + offset);
            m = പുതിയ UCharsDictionaryMatcher(characters, file);
        }
        എങ്കിൽ (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        തിരികെ m;
    } അല്ലെങ്കിൽ എങ്കിൽ (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        തിരികെ nullptr;
    }
    തിരികെ nullptr;
}


ശൂന്യം ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        പുതിയ BreakEngineWrapper(engine.orphan(), status), status);
    സ്ഥിതം UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, സ്ഥിരം അക്ഷരം* locale) സ്ഥിരം {
    തിരികെ delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;
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
    ആകുമ്പോൾ ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    തിരികെ result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
