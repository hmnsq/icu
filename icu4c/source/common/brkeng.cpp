// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/chariter.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/rbbi.h"

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "dictbe.h"
#अन्तर्भाव "lstmbe.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "dictionarydata.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "ubrkimpl.h"

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
    (शून्य)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, स्थिरांक अक्षर* locale) स्थिरांक {
    (शून्य)locale; // Unused
    परत fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) स्थिरांक {
    जर (U_FAILURE(status)) परत 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    पर्यंत (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    परत 0;
}

शून्य
UnhandledEngine::handleCharacter(UChar32 c) {
    जर (fHandled == nullptr) {
        fHandled = नवीन UnicodeSet();
        जर (fHandled == nullptr) {
            परत;
        }
    }
    जर (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_मूल्य(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_मूल्य(UCHAR_SCRIPT, script, status);
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

शून्य ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    स्थिर UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    जर (fEngines == nullptr) {
        LocalPointer<UStack>  engines(नवीन UStack(uprv_deleteUObject, nullptr, status), status);
        जर (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

स्थिरांक LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, स्थिरांक अक्षर* locale) {
    स्थिरांक LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    जर (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        परत nullptr;
    }

    स्थिर UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    पर्यंत (--i >= 0) {
        lbe = static_cast<स्थिरांक LanguageBreakEngine*>(fEngines->elementAt(i));
        जर (lbe != nullptr && lbe->handles(c, locale)) {
            परत lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    जर (lbe != nullptr) {
        fEngines->push((शून्य *)lbe, status);
    }
    परत U_SUCCESS(status) ? lbe : nullptr;
}

स्थिरांक LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, स्थिरांक अक्षर*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    जर (U_SUCCESS(status)) {
        स्थिरांक LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        स्थिरांक LSTMData *data = CreateLSTMDataForScript(code, status);
        जर (U_SUCCESS(status)) {
            जर (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                जर (U_SUCCESS(status) && engine != nullptr) {
                    परत engine;
                }
                जर (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } अन्यथा {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        जर (m != nullptr) {
            निवड(code) {
            स्थिती USCRIPT_THAI:
                engine = नवीन ThaiBreakEngine(m, status);
                थांबा;
            स्थिती USCRIPT_LAO:
                engine = नवीन LaoBreakEngine(m, status);
                थांबा;
            स्थिती USCRIPT_MYANMAR:
                engine = नवीन BurmeseBreakEngine(m, status);
                थांबा;
            स्थिती USCRIPT_KHMER:
                engine = नवीन KhmerBreakEngine(m, status);
                थांबा;

#जर !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            स्थिती USCRIPT_HANGUL:
                engine = नवीन CjkBreakEngine(m, kKorean, status);
                थांबा;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            स्थिती USCRIPT_HIRAGANA:
            स्थिती USCRIPT_KATAKANA:
            स्थिती USCRIPT_HAN:
                engine = नवीन CjkBreakEngine(m, kChineseJapanese, status);
                थांबा;
#जर 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            स्थिती USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                जर (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = नवीन CjkBreakEngine(dict, kChineseJapanese, status);
                थांबा;
            }
#endif
#endif

            पूर्वनिर्धारित:
                थांबा;
            }
            जर (engine == nullptr) {
                delete m;
            }
            अन्यथा जर (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            परत engine;
        }
    }
    परत nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_किल्ली_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    स्थिरांक char16_t *dictfname =
        ures_get_String_By_किल्ली_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    जर (U_FAILURE(status)) {
        ures_close(b);
        परत nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    स्थिरांक char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    जर (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(असत्य, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(असत्य, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    जर (U_SUCCESS(status)) {
        // build trie
        स्थिरांक uint8_t* data = static_cast<स्थिरांक uint8_t*>(udata_getMemory(file));
        स्थिरांक int32_t* indexes = reinterpret_cast<स्थिरांक int32_t*>(data);
        स्थिरांक int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        स्थिरांक int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        जर (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            स्थिरांक int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            स्थिरांक अक्षर* characters = reinterpret_cast<स्थिरांक अक्षर*>(data + offset);
            m = नवीन BytesDictionaryMatcher(characters, transform, file);
        }
        अन्यथा जर (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            स्थिरांक char16_t* characters = reinterpret_cast<स्थिरांक char16_t*>(data + offset);
            m = नवीन UCharsDictionaryMatcher(characters, file);
        }
        जर (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        परत m;
    } अन्यथा जर (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        परत nullptr;
    }
    परत nullptr;
}


शून्य ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        नवीन BreakEngineWrapper(engine.orphan(), status), status);
    स्थिर UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, स्थिरांक अक्षर* locale) स्थिरांक {
    परत delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) स्थिरांक {
    जर (U_FAILURE(status)) परत 0;
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
    पर्यंत ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    जर (U_FAILURE(status)) परत 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    जर (U_FAILURE(status)) परत 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    परत result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
