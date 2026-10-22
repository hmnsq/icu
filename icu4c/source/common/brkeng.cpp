// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/chariter.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/uscript.h"
#অন্তর্ভাব "unicode/ucharstrie.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/rbbi.h"

#অন্তর্ভাব "brkeng.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "dictbe.h"
#অন্তর্ভাব "lstmbe.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "dictionarydata.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "ubrkimpl.h"

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
UnhandledEngine::handles(UChar32 c, ধ্রুবক অক্ষর* locale) ধ্রুবক {
    (শূন্য)locale; // Unused
    ফেরত fHandled && fHandled->contains(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t endPos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UErrorCode &status) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত 0;
    utext_set_Native_সূচক(text, startPos);
    UChar32 c = utext_current32(text);
    যখন (static_cast<int32_t>(utext_get_Native_সূচক(text)) < endPos && fHandled->contains(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    ফেরত 0;
}

শূন্য
UnhandledEngine::handleCharacter(UChar32 c) {
    যদি (fHandled == nullptr) {
        fHandled = নতুন UnicodeSet();
        যদি (fHandled == nullptr) {
            ফেরত;
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
        LocalPointer<UStack>  engines(নতুন UStack(uprv_deleteUObject, nullptr, status), status);
        যদি (U_SUCCESS(status)) {
            fEngines = engines.orphan();
        }
    }
}

ধ্রুবক LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ধ্রুবক অক্ষর* locale) {
    ধ্রুবক LanguageBreakEngine *lbe = nullptr;
    UErrorCode  status = U_ZERO_ERROR;
    ensureEngines(status);
    যদি (U_FAILURE(status) ) {
        // Note: no way to return error code to caller.
        ফেরত nullptr;
    }

    স্থির UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    যখন (--i >= 0) {
        lbe = static_cast<ধ্রুবক LanguageBreakEngine*>(fEngines->elementAt(i));
        যদি (lbe != nullptr && lbe->handles(c, locale)) {
            ফেরত lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = loadEngineFor(c, locale);
    যদি (lbe != nullptr) {
        fEngines->push((শূন্য *)lbe, status);
    }
    ফেরত U_SUCCESS(status) ? lbe : nullptr;
}

ধ্রুবক LanguageBreakEngine *
ICULanguageBreakFactory::loadEngineFor(UChar32 c, ধ্রুবক অক্ষর*) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode code = uscript_getScript(c, &status);
    যদি (U_SUCCESS(status)) {
        ধ্রুবক LanguageBreakEngine *engine = nullptr;
        // Try to use LSTM first
        ধ্রুবক LSTMData *তথ্য = CreateLSTMDataForScript(code, status);
        যদি (U_SUCCESS(status)) {
            যদি (তথ্য != nullptr) {
                engine = CreateLSTMBreakEngine(code, তথ্য, status);
                যদি (U_SUCCESS(status) && engine != nullptr) {
                    ফেরত engine;
                }
                যদি (engine != nullptr) {
                    delete engine;
                    engine = nullptr;
                } অন্যথা {
                    DeleteLSTMData(তথ্য);
                }
            }
        }
        status = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = loadDictionaryMatcherFor(code);
        যদি (m != nullptr) {
            নির্বাচন(code) {
            ক্ষেত্র USCRIPT_THAI:
                engine = নতুন ThaiBreakEngine(m, status);
                বিরতি;
            ক্ষেত্র USCRIPT_LAO:
                engine = নতুন LaoBreakEngine(m, status);
                বিরতি;
            ক্ষেত্র USCRIPT_MYANMAR:
                engine = নতুন BurmeseBreakEngine(m, status);
                বিরতি;
            ক্ষেত্র USCRIPT_KHMER:
                engine = নতুন KhmerBreakEngine(m, status);
                বিরতি;

#যদি !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            ক্ষেত্র USCRIPT_HANGUL:
                engine = নতুন CjkBreakEngine(m, kKorean, status);
                বিরতি;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            ক্ষেত্র USCRIPT_HIRAGANA:
            ক্ষেত্র USCRIPT_KATAKANA:
            ক্ষেত্র USCRIPT_HAN:
                engine = নতুন CjkBreakEngine(m, kChineseJapanese, status);
                বিরতি;
#যদি 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            ক্ষেত্র USCRIPT_COMMON:
            {
                UBlockCode block = ublock_getCode(code);
                যদি (block == UBLOCK_HIRAGANA || block == UBLOCK_KATAKANA)
                   engine = নতুন CjkBreakEngine(dict, kChineseJapanese, status);
                বিরতি;
            }
#endif
#endif

            পূর্বনির্ধারিত:
                বিরতি;
            }
            যদি (engine == nullptr) {
                delete m;
            }
            অন্যথা যদি (U_FAILURE(status)) {
                delete engine;
                engine = nullptr;
            }
            ফেরত engine;
        }
    }
    ফেরত nullptr;
}

DictionaryMatcher *
ICULanguageBreakFactory::loadDictionaryMatcherFor(UScriptCode script) { 
    UErrorCode status = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_চাবি_With_Fallback(b, "dictionaries", b, &status);
    int32_t dictnlength = 0;
    ধ্রুবক char16_t *dictfname =
        ures_get_String_By_চাবি_With_Fallback(b, uscript_getShortName(script), &dictnlength, &status);
    যদি (U_FAILURE(status)) {
        ures_close(b);
        ফেরত nullptr;
    }
    CharString dictnbuf;
    CharString ext;
    ধ্রুবক char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    যদি (extStart != nullptr) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(মিথ্যা, extStart + 1, dictnlength - len - 1), status);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(মিথ্যা, dictfname, dictnlength), status);
    ures_close(b);

    UDataMemory *file = udata_open(U_ICUDATA_BRKITR, ext.তথ্য(), dictnbuf.তথ্য(), &status);
    যদি (U_SUCCESS(status)) {
        // build trie
        ধ্রুবক uint8_t* তথ্য = static_cast<ধ্রুবক uint8_t*>(udata_getMemory(file));
        ধ্রুবক int32_t* indexes = reinterpret_cast<ধ্রুবক int32_t*>(তথ্য);
        ধ্রুবক int32_t offset = indexes[Dictionary_তথ্য::IX_STRING_TRIE_OFFSET];
        ধ্রুবক int32_t trieType = indexes[Dictionary_তথ্য::IX_TRIE_TYPE] & Dictionary_তথ্য::TRIE_TYPE_MASK;
        DictionaryMatcher *m = nullptr;
        যদি (trieType == Dictionary_তথ্য::TRIE_TYPE_BYTES) {
            ধ্রুবক int32_t transform = indexes[Dictionary_তথ্য::IX_TRANSFORM];
            ধ্রুবক অক্ষর* characters = reinterpret_cast<ধ্রুবক অক্ষর*>(তথ্য + offset);
            m = নতুন BytesDictionaryMatcher(characters, transform, file);
        }
        অন্যথা যদি (trieType == Dictionary_তথ্য::TRIE_TYPE_UCHARS) {
            ধ্রুবক char16_t* characters = reinterpret_cast<ধ্রুবক char16_t*>(তথ্য + offset);
            m = নতুন UCharsDictionaryMatcher(characters, file);
        }
        যদি (m == nullptr) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        ফেরত m;
    } অন্যথা যদি (dictfname != nullptr) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        status = U_ZERO_ERROR;
        ফেরত nullptr;
    }
    ফেরত nullptr;
}


শূন্য ICULanguageBreakFactory::addExternalEngine(
        ExternalBreakEngine* external, UErrorCode& status) {
    LocalPointer<ExternalBreakEngine> engine(external, status);
    ensureEngines(status);
    LocalPointer<BreakEngineWrapper> wrapper(
        নতুন BreakEngineWrapper(engine.orphan(), status), status);
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

UBool BreakEngineWrapper::handles(UChar32 c, ধ্রুবক অক্ষর* locale) ধ্রুবক {
    ফেরত delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t endPos,
    UVector32 &foundBreaks,
    UBool /* isPhraseBreaking */,
    UErrorCode &status) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত 0;
    int32_t ফলাফল = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_set_Native_সূচক(text, startPos);
    int32_t start = static_cast<int32_t>(utext_get_Native_সূচক(text));
    int32_t current;
    int32_t rangeStart;
    int32_t rangeEnd;
    UChar32 c = utext_current32(text);
    যখন ((current = static_cast<int32_t>(utext_get_Native_সূচক(text))) < endPos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    int32_t before_আকার = foundBreaks.size();
    int32_t additionalCapacity = rangeEnd - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    foundBreaks.ensureCapacity(before_আকার+additionalCapacity, status);
    যদি (U_FAILURE(status)) ফেরত 0;
    foundBreaks.set_আকার(before_আকার + before_আকার+additionalCapacity);
    ফলাফল = delegate->fillBreaks(text, rangeStart, rangeEnd, foundBreaks.get_বাফার()+before_আকার,
                                  additionalCapacity, status);
    যদি (U_FAILURE(status)) ফেরত 0;
    foundBreaks.set_আকার(before_আকার + ফলাফল);
    utext_set_Native_সূচক(text, current);
    ফেরত ফলাফল;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
