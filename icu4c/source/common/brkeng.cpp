// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/chariter.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "unicode/ucharstrie.h"
#અન્તર્ભાવ "unicode/bytestrie.h"
#અન્તર્ભાવ "unicode/rbbi.h"

#અન્તર્ભાવ "brkeng.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "dictbe.h"
#અન્તર્ભાવ "lstmbe.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "dictionarydata.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "uvector.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "ubrkimpl.h"

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
    (શૂન્ય)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, અચલ અક્ષર* locale) અચલ {
    (શૂન્ય)locale; // Unused
    પાછા fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) અચલ {
    જો (U_FAILURE(status)) પાછા 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    જ્યારે (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    પાછા 0;
}

શૂન્ય
UnhandledEngine::handleCharacter(UChar32 c) {
    જો (fHandled == nullptr) {
        fHandled = નવું UnicodeSet();
        જો (fHandled == nullptr) {
            પાછા;
        }
    }
    જો (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_કિંમત(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_કિંમત(UCHAR_SCRIPT, script, status);
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

શૂન્ય ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    સ્થિર UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    જો (fEngines == nullptr) {
        LocalPointer<UStack>  engines(નવું UStack(uprv_deleteUObject, nullptr, status), status);
        જો (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

અચલ LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, અચલ અક્ષર* locale) {
    અચલ LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    જો (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        પાછા nullptr;
    }

    સ્થિર UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    જ્યારે (--i >= 0) {
        lbe = static_cast<અચલ LanguageBreakEngine*>(fEngines->elementAt(i));
        જો (lbe != nullptr && lbe->handles(c, locale)) {
            પાછા lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    જો (lbe != nullptr) {
        fEngines->push((શૂન્ય *)lbe, status);
    }
    પાછા U_SUCCESS(status) ? lbe : nullptr;
}

અચલ LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, અચલ અક્ષર*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    જો (U_SUCCESS(status)) {
        અચલ LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        અચલ LSTMData *data = CreateLSTMDataForScript(code, status);
        જો (U_SUCCESS(status)) {
            જો (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                જો (U_SUCCESS(status) && engine != nullptr) {
                    પાછા engine;
                }
                જો (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } નહીંતર {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        જો (m != nullptr) {
            પસંદગી(code) {
            સ્થિતિ USCRIPT_THAI:
                engine = નવું ThaiBreakEngine(m, status);
                તોડો;
            સ્થિતિ USCRIPT_LAO:
                engine = નવું LaoBreakEngine(m, status);
                તોડો;
            સ્થિતિ USCRIPT_MYANMAR:
                engine = નવું BurmeseBreakEngine(m, status);
                તોડો;
            સ્થિતિ USCRIPT_KHMER:
                engine = નવું KhmerBreakEngine(m, status);
                તોડો;

#જો !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            સ્થિતિ USCRIPT_HANGUL:
                engine = નવું CjkBreakEngine(m, kKorean, status);
                તોડો;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            સ્થિતિ USCRIPT_HIRAGANA:
            સ્થિતિ USCRIPT_KATAKANA:
            સ્થિતિ USCRIPT_HAN:
                engine = નવું CjkBreakEngine(m, kChineseJapanese, status);
                તોડો;
#જો 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            સ્થિતિ USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                જો (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = નવું CjkBreakEngine(dict, kChineseJapanese, status);
                તોડો;
            }
#endif
#endif

            મૂળભૂત:
                તોડો;
            }
            જો (engine == nullptr) {
                delete m;
            }
            નહીંતર જો (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            પાછા engine;
        }
    }
    પાછા nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_ચાવી_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    અચલ char16_t *dictfname =
        ures_get_String_By_ચાવી_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    જો (U_FAILURE(status)) {
        ures_close(b);
        પાછા nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    અચલ char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    જો (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(ખોટું, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(ખોટું, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    જો (U_SUCCESS(status)) {
        // build trie
        અચલ uint8_t* data = static_cast<અચલ uint8_t*>(udata_getMemory(file));
        અચલ int32_t* indexes = reinterpret_cast<અચલ int32_t*>(data);
        અચલ int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        અચલ int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        જો (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            અચલ int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            અચલ અક્ષર* characters = reinterpret_cast<અચલ અક્ષર*>(data + offset);
            m = નવું BytesDictionaryMatcher(characters, transform, file);
        }
        નહીંતર જો (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            અચલ char16_t* characters = reinterpret_cast<અચલ char16_t*>(data + offset);
            m = નવું UCharsDictionaryMatcher(characters, file);
        }
        જો (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        પાછા m;
    } નહીંતર જો (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        પાછા nullptr;
    }
    પાછા nullptr;
}


શૂન્ય ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        નવું BreakEngineWrapper(engine.orphan(), status), status);
    સ્થિર UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, અચલ અક્ષર* locale) અચલ {
    પાછા delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) અચલ {
    જો (U_FAILURE(status)) પાછા 0;
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
    જ્યારે ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    જો (U_FAILURE(status)) પાછા 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    જો (U_FAILURE(status)) પાછા 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    પાછા result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
