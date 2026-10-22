// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

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

UnhandledEngine::UnhandledEngine(UError_संहिता &स्थितिः) : fHandled(शून्यसूचकः) {
    (शून्य)स्थितिः;
}

UnhandledEngine::~UnhandledEngine() {
    विलोपय fHandled;
    fHandled = शून्यसूचकः;
}

UBool
UnhandledEngine::handles(UChar32 c, ध्रुव वर्ण* locale) ध्रुव {
    (शून्य)locale; //अप्रयुक्तम्
    प्रत्यागम fHandled && fHandled->अन्तर्भवति(c);
}

int32_t
UnhandledEngine::findBreaks( UText *text,
                             int32_t startPos,
                             int32_t अन्तः_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_संहिता &स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    utext_समुच्चयः_Native_सूचकांक(text, startPos);
    UChar32 c = utext_current32(text);
    यावत् (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) < अन्तः_Pos && fHandled->अन्तर्भवति(c)) {
        utext_next32(text);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(text);
    }
    प्रत्यागम 0;
}

शून्य
UnhandledEngine::handleCharacter(UChar32 c) {
    यदि (fHandled == शून्यसूचकः) {
        fHandled = नूतन Unicode_समुच्चयः();
        यदि (fHandled == शून्यसूचकः) {
            प्रत्यागम;
        }
    }
    यदि (!fHandled->अन्तर्भवति(c)) {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_मानम्(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_मानम्(UCHAR_SCRIPT, script, स्थितिः);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_संहिता &/*स्थितिः*/) {
    fEngines = शून्यसूचकः;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    विलोपय fEngines;
}

शून्य ICULanguageBreakFactory::ensureEngines(UError_संहिता& स्थितिः) {
    स्थिर UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    यदि (fEngines == शून्यसूचकः) {
        LocalPointer<UStack>  engines(नूतन UStack(uprv_deleteUObject, शून्यसूचकः, स्थितिः), स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
            fEngines = engines.orphan();
        }
    }
}

ध्रुव LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ध्रुव वर्ण* locale) {
    ध्रुव LanguageBreakEngine *lbe = शून्यसूचकः;
    UError_संहिता  स्थितिः = U_ZERO_ERROR;
    ensureEngines(स्थितिः);
    यदि (U_FAILURE(स्थितिः) ) {
        // Note: no way to return error code to caller.
        प्रत्यागम शून्यसूचकः;
    }

    स्थिर UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    यावत् (--i >= 0) {
        lbe = static_cast<ध्रुव LanguageBreakEngine*>(fEngines->elementAt(i));
        यदि (lbe != शून्यसूचकः && lbe->handles(c, locale)) {
            प्रत्यागम lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = आरोपय_Engine_For(c, locale);
    यदि (lbe != शून्यसूचकः) {
        fEngines->push((शून्य *)lbe, स्थितिः);
    }
    प्रत्यागम U_SUCCESS(स्थितिः) ? lbe : शून्यसूचकः;
}

ध्रुव LanguageBreakEngine *
ICULanguageBreakFactory::आरोपय_Engine_For(UChar32 c, ध्रुव वर्ण*) {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    UScript_संहिता संहिता = uscript_getScript(c, &स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        ध्रुव LanguageBreakEngine *engine = शून्यसूचकः;
        // Try to use LSTM first
        ध्रुव LSTMData *दत्त = CreateLSTMDataForScript(संहिता, स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
            यदि (दत्त != शून्यसूचकः) {
                engine = CreateLSTMBreakEngine(संहिता, दत्त, स्थितिः);
                यदि (U_SUCCESS(स्थितिः) && engine != शून्यसूचकः) {
                    प्रत्यागम engine;
                }
                यदि (engine != शून्यसूचकः) {
                    विलोपय engine;
                    engine = शून्यसूचकः;
                } अन्यथा {
                    DeleteLSTMData(दत्त);
                }
            }
        }
        स्थितिः = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = आरोपय_Dictionary_Matcher_For(संहिता);
        यदि (m != शून्यसूचकः) {
            विकल्प(संहिता) {
            पक्ष USCRIPT_THAI:
                engine = नूतन ThaiBreakEngine(m, स्थितिः);
                भङ्ग;
            पक्ष USCRIPT_LAO:
                engine = नूतन LaoBreakEngine(m, स्थितिः);
                भङ्ग;
            पक्ष USCRIPT_MYANMAR:
                engine = नूतन BurmeseBreakEngine(m, स्थितिः);
                भङ्ग;
            पक्ष USCRIPT_KHMER:
                engine = नूतन KhmerBreakEngine(m, स्थितिः);
                भङ्ग;

#यदि !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            पक्ष USCRIPT_HANGUL:
                engine = नूतन CjkBreakEngine(m, kKorean, स्थितिः);
                भङ्ग;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            पक्ष USCRIPT_HIRAGANA:
            पक्ष USCRIPT_KATAKANA:
            पक्ष USCRIPT_HAN:
                engine = नूतन CjkBreakEngine(m, kChineseJapanese, स्थितिः);
                भङ्ग;
#यदि 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            पक्ष USCRIPT_COMMON:
            {
                UBlock_संहिता खण्डम् = ublock_get_संहिता(संहिता);
                यदि (खण्डम् == UBLOCK_HIRAGANA || खण्डम् == UBLOCK_KATAKANA)
                   engine = नूतन CjkBreakEngine(dict, kChineseJapanese, स्थितिः);
                भङ्ग;
            }
#यदि_अन्तः
#यदि_अन्तः

            स्वभाव:
                भङ्ग;
            }
            यदि (engine == शून्यसूचकः) {
                विलोपय m;
            }
            अन्यथा यदि (U_FAILURE(स्थितिः)) {
                विलोपय engine;
                engine = शून्यसूचकः;
            }
            प्रत्यागम engine;
        }
    }
    प्रत्यागम शून्यसूचकः;
}

DictionaryMatcher *
ICULanguageBreakFactory::आरोपय_Dictionary_Matcher_For(UScript_संहिता script) { 
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &स्थितिः);
    b = ures_get_By_कुञ्चिका_With_Fallback(b, "dictionaries", b, &स्थितिः);
    int32_t dictnlength = 0;
    ध्रुव char16_t *dictfname =
        ures_get_String_By_कुञ्चिका_With_Fallback(b, uscript_getShortName(script), &dictnlength, &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        ures_close(b);
        प्रत्यागम शून्यसूचकः;
    }
    CharString dictnbuf;
    CharString ext;
    ध्रुव char16_t *extStart = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    यदि (extStart != शून्यसूचकः) {
        int32_t len = static_cast<int32_t>(extStart - dictfname);
        ext.appendInvariantChars(UnicodeString(असत्य, extStart + 1, dictnlength - len - 1), स्थितिः);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(असत्य, dictfname, dictnlength), स्थितिः);
    ures_close(b);

    UData_स्मृतिः *file = udata_open(U_ICUDATA_BRKITR, ext.दत्त(), dictnbuf.दत्त(), &स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        // build trie
        ध्रुव uint8_t* दत्त = static_cast<ध्रुव uint8_t*>(udata_get_स्मृतिः(file));
        ध्रुव int32_t* indexes = reinterpret_cast<ध्रुव int32_t*>(दत्त);
        ध्रुव int32_t विस्थापनम् = indexes[Dictionary_दत्त::IX_STRING_TRIE_विस्थापनम्];
        ध्रुव int32_t trieType = indexes[Dictionary_दत्त::IX_TRIE_TYPE] & Dictionary_दत्त::TRIE_TYPE_आवरणम्;
        DictionaryMatcher *m = शून्यसूचकः;
        यदि (trieType == Dictionary_दत्त::TRIE_TYPE_अष्टकानि) {
            ध्रुव int32_t transform = indexes[Dictionary_दत्त::IX_TRANSFORM];
            ध्रुव वर्ण* characters = reinterpret_cast<ध्रुव वर्ण*>(दत्त + विस्थापनम्);
            m = नूतन अष्टकानि_Dictionary_Matcher(characters, transform, file);
        }
        अन्यथा यदि (trieType == Dictionary_दत्त::TRIE_TYPE_UCHARS) {
            ध्रुव char16_t* characters = reinterpret_cast<ध्रुव char16_t*>(दत्त + विस्थापनम्);
            m = नूतन UCharsDictionaryMatcher(characters, file);
        }
        यदि (m == शून्यसूचकः) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        प्रत्यागम m;
    } अन्यथा यदि (dictfname != शून्यसूचकः) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        स्थितिः = U_ZERO_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम शून्यसूचकः;
}


शून्य ICULanguageBreakFactory::सङ्कलय_External_Engine(
        ExternalBreakEngine* external, UError_संहिता& स्थितिः) {
    LocalPointer<ExternalBreakEngine> engine(external, स्थितिः);
    ensureEngines(स्थितिः);
    LocalPointer<BreakEngineWrapper> wrapper(
        नूतन BreakEngineWrapper(engine.orphan(), स्थितिः), स्थितिः);
    स्थिर UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), स्थितिः);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_संहिता &स्थितिः) : delegate(engine, स्थितिः) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ध्रुव वर्ण* locale) ध्रुव {
    प्रत्यागम delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::findBreaks(
    UText *text,
    int32_t startPos,
    int32_t अन्तः_Pos,
    UVector32 &प्राप्तम्_Breaks,
    UBool /* isPhraseBreaking */,
    UError_संहिता &स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    int32_t परिणाम = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_समुच्चयः_Native_सूचकांक(text, startPos);
    int32_t start = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
    int32_t current;
    int32_t rangeStart;
    int32_t range_अन्तः;
    UChar32 c = utext_current32(text);
    यावत् ((current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < अन्तः_Pos && delegate->handles(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    range_अन्तः = current;
    int32_t before_आकार = प्राप्तम्_Breaks.size();
    int32_t additional_क्षमता = range_अन्तः - rangeStart + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    प्राप्तम्_Breaks.ensure_क्षमता(before_आकार+additional_क्षमता, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    प्राप्तम्_Breaks.समुच्चयः_आकार(before_आकार + before_आकार+additional_क्षमता);
    परिणाम = delegate->fillBreaks(text, rangeStart, range_अन्तः, प्राप्तम्_Breaks.get_बफर()+before_आकार,
                                  additional_क्षमता, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    प्राप्तम्_Breaks.समुच्चयः_आकार(before_आकार + परिणाम);
    utext_समुच्चयः_Native_सूचकांक(text, current);
    प्रत्यागम परिणाम;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
