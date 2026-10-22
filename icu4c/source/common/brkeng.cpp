// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "unicode/chariter.h"
#অন্তর্ভাৱ "unicode/ures.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/uscript.h"
#অন্তর্ভাৱ "unicode/ucharstrie.h"
#অন্তর্ভাৱ "unicode/bytestrie.h"
#অন্তর্ভাৱ "unicode/rbbi.h"

#অন্তর্ভাৱ "brkeng.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "dictbe.h"
#অন্তর্ভাৱ "lstmbe.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "dictionarydata.h"
#অন্তর্ভাৱ "mutex.h"
#অন্তর্ভাৱ "uvector.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "uresimp.h"
#অন্তর্ভাৱ "ubrkimpl.h"

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
    (শূন্য)status;
}

UnhandledEngine::~UnhandledEngine() {
    delete fHandled;
    fHandled = nullptr;
}

UBool
UnhandledEngine::handles(UChar32 c, ধ্রুৱ ৱর্ণ* locale) ধ্রুৱ {
    (শূন্য)locale; // Unused
    ঘূৰাই fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) ধ্রুৱ {
    যদি (U_FAILURE(status)) ঘূৰাই 0;
    utext_setNativeIndex(text, startPos);
    UChar32 c = utext_current32(text);
    যেতিয়ালৈ (static_cast<int32_t>(utext_getNativeIndex(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    ঘূৰাই 0;
}

শূন্য
UnhandledEngine::handleCharacter(UChar32 c) {
    যদি (fHandled == nullptr) {
        fHandled = নূতন UnicodeSet();
        যদি (fHandled == nullptr) {
            ঘূৰাই;
        }
    }
    যদি (!fHandled->contains(c)) {
        UErrorCode status = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_মান(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_মান(UCHAR_SCRIPT, script, status);
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

শূন্য ICULanguageBreakFactory::ensureEngines(UErrorCode& status) {
    স্থির UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    যদি (fEngines == nullptr) {
        LocalPointer<UStack>  engines(নূতন UStack(uprv_deleteUObject, nullptr, status), status);
        যদি (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

ধ্রুৱ LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ধ্রুৱ ৱর্ণ* locale) {
    ধ্রুৱ LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    যদি (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        ঘূৰাই nullptr;
    }

    স্থির UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    যেতিয়ালৈ (--i >= 0) {
        lbe = static_cast<ধ্রুৱ LanguageBreakEngine*>(fEngines->elementAt(i));
        যদি (lbe != nullptr && lbe->handles(c, locale)) {
            ঘূৰাই lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    যদি (lbe != nullptr) {
        fEngines->push((শূন্য *)lbe, status);
    }
    ঘূৰাই U_SUCCESS(status) ? lbe : nullptr;
}

ধ্রুৱ LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, ধ্রুৱ ৱর্ণ*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    যদি (U_SUCCESS(status)) {
        ধ্রুৱ LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        ধ্রুৱ LSTMData *data = CreateLSTMDataForScript(code, status);
        যদি (U_SUCCESS(status)) {
            যদি (data != nullptr) {
                engine = CreateLSTMBreakEngine(code, data, status);
                যদি (U_SUCCESS(status) && engine != nullptr) {
                    ঘূৰাই engine;
                }
                যদি (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } অন্যথা {
                    DeleteLSTMData(data);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        যদি (m != nullptr) {
            ৱিকল্প(code) {
            পক্ষ USCRIPT_THAI:
                engine = নূতন ThaiBreakEngine(m, status);
                ভঙ্গ;
            পক্ষ USCRIPT_LAO:
                engine = নূতন LaoBreakEngine(m, status);
                ভঙ্গ;
            পক্ষ USCRIPT_MYANMAR:
                engine = নূতন BurmeseBreakEngine(m, status);
                ভঙ্গ;
            পক্ষ USCRIPT_KHMER:
                engine = নূতন KhmerBreakEngine(m, status);
                ভঙ্গ;

#যদি !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            পক্ষ USCRIPT_HANGUL:
                engine = নূতন CjkBreakEngine(m, kKorean, status);
                ভঙ্গ;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            পক্ষ USCRIPT_HIRAGANA:
            পক্ষ USCRIPT_KATAKANA:
            পক্ষ USCRIPT_HAN:
                engine = নূতন CjkBreakEngine(m, kChineseJapanese, status);
                ভঙ্গ;
#যদি 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            পক্ষ USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                যদি (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = নূতন CjkBreakEngine(dict, kChineseJapanese, status);
                ভঙ্গ;
            }
#endif
#endif

            স্ৱভাৱ:
                ভঙ্গ;
            }
            যদি (engine == nullptr) {
                delete m;
            }
            অন্যথা যদি (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            ঘূৰাই engine;
        }
    }
    ঘূৰাই nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_চাবি_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    ধ্রুৱ char16_t *dictfname =
        ures_get_String_By_চাবি_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    যদি (U_FAILURE(status)) {
        ures_close(b);
        ঘূৰাই nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    ধ্রুৱ char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    যদি (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(মিছা, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(মিছা, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.data(), dictnbuf.data(), &status);
    যদি (U_SUCCESS(status)) {
        // build trie
        ধ্রুৱ uint8_t* data = static_cast<ধ্রুৱ uint8_t*>(udata_getMemory(file));
        ধ্রুৱ int32_t* indexes = reinterpret_cast<ধ্রুৱ int32_t*>(data);
        ধ্রুৱ int32_t offset = indexes[DictionaryData::IX_STRING_TRIE_OFFSET];
        ধ্রুৱ int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        যদি (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            ধ্রুৱ int32_t transform = indexes[DictionaryData::IX_TRANSFORM];
            ধ্রুৱ ৱর্ণ* characters = reinterpret_cast<ধ্রুৱ ৱর্ণ*>(data + offset);
            m = নূতন BytesDictionaryMatcher(characters, transform, file);
        }
        অন্যথা যদি (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ধ্রুৱ char16_t* characters = reinterpret_cast<ধ্রুৱ char16_t*>(data + offset);
            m = নূতন UCharsDictionaryMatcher(characters, file);
        }
        যদি (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        ঘূৰাই m;
    } অন্যথা যদি (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        ঘূৰাই nullptr;
    }
    ঘূৰাই nullptr;
}


শূন্য ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        নূতন BreakEngineWrapper(engine.orphan(), status), status);
    স্থির UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), status);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UErrorCode &status) : delegate(engine, status) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ধ্রুৱ ৱর্ণ* locale) ধ্রুৱ {
    ঘূৰাই delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) ধ্রুৱ {
    যদি (U_FAILURE(status)) ঘূৰাই 0;
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
    যেতিয়ালৈ ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t beforeSize = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(beforeSize+additionalCapacity, status);
    যদি (U_FAILURE(status)) ঘূৰাই 0;
    foundBreaks.setSize(beforeSize + beforeSize+additionalCapacity);
    result = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.getBuffer()+beforeSize,
                                  additionalCapacity, status);
    যদি (U_FAILURE(status)) ঘূৰাই 0;
    foundBreaks.setSize(beforeSize + result);
    utext_setNativeIndex(text, current);
    ঘূৰাই result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
