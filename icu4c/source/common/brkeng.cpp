// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/uchar.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/chariter.h"
#समावेश "unicode/ures.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/putil.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uscript.h"
#समावेश "unicode/ucharstrie.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/rbbi.h"

#समावेश "brkeng.h"
#समावेश "cmemory.h"
#समावेश "dictbe.h"
#समावेश "lstmbe.h"
#समावेश "charstr.h"
#समावेश "dictionarydata.h"
#समावेश "mutex.h"
#समावेश "uvector.h"
#समावेश "umutex.h"
#समावेश "uresimp.h"
#समावेश "ubrkimpl.h"

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
    (व्योम)status;
}

UnhandledEngine::~UnhandledEngine() {
    मिटाओ fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, स्थिर अक्षर* locale) स्थिर {
    (व्योम)locale; // Unused
    वापस fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) स्थिर {
    अगर (U_FAILURE(status)) वापस 0;
    utext_set_Native_सूचकांक(text, startPos);
    UChar32 c = utext_current32(text);
    जबतक (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    वापस 0;
}

व्योम
UnhandledEngine::handleCharacter(UChar32 c) {
    अगर (fHandled == nullptr) {
        fHandled = नया UnicodeSet();
        अगर (fHandled == nullptr) {
            वापस;
        }
    }
    अगर (!fHandled->contains(c)) {
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
    मिटाओ fEngines;
}

व्योम ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    अटल UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    अगर (fEngines == nullptr) {
        LocalPointer<UStack>  engines(नया UStack(uprv_deleteUObject, nullptr, status), status);
        अगर (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

स्थिर LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, स्थिर अक्षर* locale) {
    स्थिर LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    अगर (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        वापस nullptr;
    }

    अटल UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    जबतक (--i >= 0) {
        lbe = static_cast<स्थिर LanguageBreakEngine*>(fEngines->elementAt(i));
        अगर (lbe != nullptr && lbe->handles(c, locale)) {
            वापस lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    अगर (lbe != nullptr) {
        fEngines->push((व्योम *)lbe, status);
    }
    वापस U_SUCCESS(status) ? lbe : nullptr;
}

स्थिर LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, स्थिर अक्षर*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    अगर (U_SUCCESS(status)) {
        स्थिर LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        स्थिर LSTMData *तथ्य = CreateLSTMDataForScript(code, status);
        अगर (U_SUCCESS(status)) {
            अगर (तथ्य != nullptr) {
                engine = CreateLSTMBreakEngine(code, तथ्य, status);
                अगर (U_SUCCESS(status) && engine != nullptr) {
                    वापस engine;
                }
                अगर (engine != nullptr) {
                    मिटाओ engine;
                    engine = nullptr;
                } अन्यथा {
                    DeleteLSTMData(तथ्य);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        अगर (m != nullptr) {
            चयन(code) {
            हाल USCRIPT_THAI:
                engine = नया ThaiBreakEngine(m, status);
                अवरोध;
            हाल USCRIPT_LAO:
                engine = नया LaoBreakEngine(m, status);
                अवरोध;
            हाल USCRIPT_MYANMAR:
                engine = नया BurmeseBreakEngine(m, status);
                अवरोध;
            हाल USCRIPT_KHMER:
                engine = नया KhmerBreakEngine(m, status);
                अवरोध;

#अगर !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            हाल USCRIPT_HANGUL:
                engine = नया CjkBreakEngine(m, kKorean, status);
                अवरोध;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            हाल USCRIPT_HIRAGANA:
            हाल USCRIPT_KATAKANA:
            हाल USCRIPT_HAN:
                engine = नया CjkBreakEngine(m, kChineseJapanese, status);
                अवरोध;
#अगर 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            हाल USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                अगर (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = नया CjkBreakEngine(dict, kChineseJapanese, status);
                अवरोध;
            }
#पूर्ण_अगर
#पूर्ण_अगर

            शेष:
                अवरोध;
            }
            अगर (engine == nullptr) {
                मिटाओ m;
            }
            अन्यथा अगर (U_FAILURE(status)) {
                मिटाओ engine;
                engine = nullptr;
            }
            वापस engine;
        }
    }
    वापस nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_कुंजी_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    स्थिर char16_t *dictfname =
        ures_get_String_By_कुंजी_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    अगर (U_FAILURE(status)) {
        ures_close(b);
        वापस nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    स्थिर char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    अगर (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(false, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(false, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.तथ्य(), dictnbuf.तथ्य(), &status);
    अगर (U_SUCCESS(status)) {
        // build trie
        स्थिर uint8_t* तथ्य = static_cast<स्थिर uint8_t*>(udata_getMemory(file));
        स्थिर int32_t* indexes = reinterpret_cast<स्थिर int32_t*>(तथ्य);
        स्थिर int32_t offset = indexes[Dictionary_तथ्य::IX_STRING_TRIE_OFFSET];
        स्थिर int32_t trieType = indexes[Dictionary_तथ्य::IX_TRIE_TYPE] & Dictionary_तथ्य::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        अगर (trieType == Dictionary_तथ्य::TRIE_TYPE_BYTES) {
            स्थिर int32_t transform = indexes[Dictionary_तथ्य::IX_TRANSFORM];
            स्थिर अक्षर* characters = reinterpret_cast<स्थिर अक्षर*>(तथ्य + offset);
            m = नया BytesDictionaryMatcher(characters, transform, file);
        }
        अन्यथा अगर (trieType == Dictionary_तथ्य::TRIE_TYPE_UCHARS) {
            स्थिर char16_t* characters = reinterpret_cast<स्थिर char16_t*>(तथ्य + offset);
            m = नया UCharsDictionaryMatcher(characters, file);
        }
        अगर (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        वापस m;
    } अन्यथा अगर (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        वापस nullptr;
    }
    वापस nullptr;
}


व्योम ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        नया BreakEngineWrapper(engine.orphan(), status), status);
    अटल UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, स्थिर अक्षर* locale) स्थिर {
    वापस delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) स्थिर {
    अगर (U_FAILURE(status)) वापस 0;
    int32_t परिणाम = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_set_Native_सूचकांक(text, startPos);
    int32_t start = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
    int32_t current;
    int32_t rangeStart;
    int32_t rangeEnd;
    UChar32 c = utext_current32(text);
    जबतक ((current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t before_आकार = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(before_आकार+additionalCapacity, status);
    अगर (U_FAILURE(status)) वापस 0;
    foundBreaks.set_आकार(before_आकार + before_आकार+additionalCapacity);
    परिणाम = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.get_बफर()+before_आकार,
                                  additionalCapacity, status);
    अगर (U_FAILURE(status)) वापस 0;
    foundBreaks.set_आकार(before_आकार + परिणाम);
    utext_set_Native_सूचकांक(text, current);
    वापस परिणाम;
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
