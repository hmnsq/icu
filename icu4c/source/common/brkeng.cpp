// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/chariter.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "unicode/ucharstrie.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/rbbi.h"

#அந்தர்பாவ "brkeng.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "dictbe.h"
#அந்தர்பாவ "lstmbe.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "dictionarydata.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "ubrkimpl.h"

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
    (வெறுமை)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, மாறிலி எழுத்து* locale) மாறிலி {
    (வெறுமை)locale; // Unused
    திரும்பு fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) மாறிலி {
    எனில் (U_FAILURE(status)) திரும்பு 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    போது (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    திரும்பு 0;
}

வெறுமை
UnhandledEngine::handleCharacter(UChar32 c) {
    எனில் (fHandled == nullptr) {
        fHandled = புதிய UnicodeSet();
        எனில் (fHandled == nullptr) {
            திரும்பு;
        }
    }
    எனில் (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_மதிப்பு(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_மதிப்பு(UCHAR_SCRIPT, script, status);
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

வெறுமை ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    நிலையான UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    எனில் (fEngines == nullptr) {
        LocalPointer<UStack>  engines(புதிய UStack(uprv_deleteUObject, nullptr, status), status);
        எனில் (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

மாறிலி LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, மாறிலி எழுத்து* locale) {
    மாறிலி LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    எனில் (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        திரும்பு nullptr;
    }

    நிலையான UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    போது (--i >= 0) {
        lbe = static_cast<மாறிலி LanguageBreakEngine*>(fEngines->elementAt(i));
        எனில் (lbe != nullptr && lbe->handles(c, locale)) {
            திரும்பு lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    எனில் (lbe != nullptr) {
        fEngines->push((வெறுமை *)lbe, status);
    }
    திரும்பு U_SUCCESS(status) ? lbe : nullptr;
}

மாறிலி LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, மாறிலி எழுத்து*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    எனில் (U_SUCCESS(status)) {
        மாறிலி LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        மாறிலி LSTMData *data = CreateLSTMDataForScript(code, status);
        எனில் (U_SUCCESS(status)) {
            எனில் (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                எனில் (U_SUCCESS(status) && engine != nullptr) {
                    திரும்பு engine;
                }
                எனில் (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } இல்லையெனில் {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        எனில் (m != nullptr) {
            தேர்வு(code) {
            நிலை USCRIPT_THAI:
                engine = புதிய ThaiBreakEngine(m, status);
                நிறுத்து;
            நிலை USCRIPT_LAO:
                engine = புதிய LaoBreakEngine(m, status);
                நிறுத்து;
            நிலை USCRIPT_MYANMAR:
                engine = புதிய BurmeseBreakEngine(m, status);
                நிறுத்து;
            நிலை USCRIPT_KHMER:
                engine = புதிய KhmerBreakEngine(m, status);
                நிறுத்து;

#எனில் !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            நிலை USCRIPT_HANGUL:
                engine = புதிய CjkBreakEngine(m, kKorean, status);
                நிறுத்து;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            நிலை USCRIPT_HIRAGANA:
            நிலை USCRIPT_KATAKANA:
            நிலை USCRIPT_HAN:
                engine = புதிய CjkBreakEngine(m, kChineseJapanese, status);
                நிறுத்து;
#எனில் 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            நிலை USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                எனில் (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = புதிய CjkBreakEngine(dict, kChineseJapanese, status);
                நிறுத்து;
            }
#endif
#endif

            இயல்பு:
                நிறுத்து;
            }
            எனில் (engine == nullptr) {
                delete m;
            }
            இல்லையெனில் எனில் (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            திரும்பு engine;
        }
    }
    திரும்பு nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_சாவி_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    மாறிலி char16_t *dictfname =
        ures_get_String_By_சாவி_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    எனில் (U_FAILURE(status)) {
        ures_close(b);
        திரும்பு nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    மாறிலி char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    எனில் (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(பொய், extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(பொய், dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    எனில் (U_SUCCESS(status)) {
        // build trie
        மாறிலி uint8_t* data = static_cast<மாறிலி uint8_t*>(udata_getMemory(file));
        மாறிலி int32_t* indexes = reinterpret_cast<மாறிலி int32_t*>(data);
        மாறிலி int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        மாறிலி int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        எனில் (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            மாறிலி int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            மாறிலி எழுத்து* characters = reinterpret_cast<மாறிலி எழுத்து*>(data + offset);
            m = புதிய BytesDictionaryMatcher(characters, transform, file);
        }
        இல்லையெனில் எனில் (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            மாறிலி char16_t* characters = reinterpret_cast<மாறிலி char16_t*>(data + offset);
            m = புதிய UCharsDictionaryMatcher(characters, file);
        }
        எனில் (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        திரும்பு m;
    } இல்லையெனில் எனில் (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு nullptr;
}


வெறுமை ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        புதிய BreakEngineWrapper(engine.orphan(), status), status);
    நிலையான UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, மாறிலி எழுத்து* locale) மாறிலி {
    திரும்பு delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) மாறிலி {
    எனில் (U_FAILURE(status)) திரும்பு 0;
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
    போது ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    எனில் (U_FAILURE(status)) திரும்பு 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    எனில் (U_FAILURE(status)) திரும்பு 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    திரும்பு result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
