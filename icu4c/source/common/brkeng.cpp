// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/chariter.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "unicode/ucharstrie.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/rbbi.h"

#అన్తర్భావ "brkeng.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "dictbe.h"
#అన్తర్భావ "lstmbe.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "dictionarydata.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "ubrkimpl.h"

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

UnhandledEngine::UnhandledEngine(UError_సంకేతం &స్థితి_2) : fHandled(శూన్యసూచిక) {
    (శూన్య)స్థితి_2;
}

UnhandledEngine::~UnhandledEngine() {
    తొలగించు fHandled;
    fHandled = శూన్యసూచిక;
}

UBool
UnhandledEngine::handles(UChar32 c, స్థిరరాశి అక్షరం* locale) స్థిరరాశి {
    (శూన్య)locale; //వాడని
    తిరిగి fHandled && fHandled->కలిగియుంది(c);
}

int32_t
UnhandledEngine::కనుగొను_Breaks( UText *పాఠ్యం,
                             int32_t ప్రారంభం_Pos,
                             int32_t చివర_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    utext_సమితి_Native_సూచీ(పాఠ్యం, ప్రారంభం_Pos);
    UChar32 c = utext_current32(పాఠ్యం);
    అయితే (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) < చివర_Pos && fHandled->కలిగియుంది(c)) {
        utext_next32(పాఠ్యం);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(పాఠ్యం);
    }
    తిరిగి 0;
}

శూన్య
UnhandledEngine::handleCharacter(UChar32 c) {
    ఒకవేళ (fHandled == శూన్యసూచిక) {
        fHandled = కొత్త Unicode_సమితి();
        ఒకవేళ (fHandled == శూన్యసూచిక) {
            తిరిగి;
        }
    }
    ఒకవేళ (!fHandled->కలిగియుంది(c)) {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_విలువ(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_విలువ(UCHAR_SCRIPT, script, స్థితి_2);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_సంకేతం &/*స్థితి*/) {
    fEngines = శూన్యసూచిక;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    తొలగించు fEngines;
}

శూన్య ICULanguageBreakFactory::ensureEngines(UError_సంకేతం& స్థితి_2) {
    స్థిర UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    ఒకవేళ (fEngines == శూన్యసూచిక) {
        Local_సూచిక<UStack>  engines(కొత్త UStack(uprv_deleteUObject, శూన్యసూచిక, స్థితి_2), స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            fEngines = engines.orphan();
        }
    }
}

స్థిరరాశి LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, స్థిరరాశి అక్షరం* locale) {
    స్థిరరాశి LanguageBreakEngine *lbe = శూన్యసూచిక;
    UError_సంకేతం  స్థితి_2 = U_ZERO_ERROR;
    ensureEngines(స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2) ) {
        // Note: no way to return error code to caller.
        తిరిగి శూన్యసూచిక;
    }

    స్థిర UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    అయితే (--i >= 0) {
        lbe = static_cast<స్థిరరాశి LanguageBreakEngine*>(fEngines->elementAt(i));
        ఒకవేళ (lbe != శూన్యసూచిక && lbe->handles(c, locale)) {
            తిరిగి lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = ఎక్కించు_Engine_For(c, locale);
    ఒకవేళ (lbe != శూన్యసూచిక) {
        fEngines->push((శూన్య *)lbe, స్థితి_2);
    }
    తిరిగి U_SUCCESS(స్థితి_2) ? lbe : శూన్యసూచిక;
}

స్థిరరాశి LanguageBreakEngine *
ICULanguageBreakFactory::ఎక్కించు_Engine_For(UChar32 c, స్థిరరాశి అక్షరం*) {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    UScript_సంకేతం సంకేతం_2 = uscript_getScript(c, &స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        స్థిరరాశి LanguageBreakEngine *engine = శూన్యసూచిక;
        // Try to use LSTM first
        స్థిరరాశి LSTMData *దత్తాంశం = CreateLSTMDataForScript(సంకేతం_2, స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            ఒకవేళ (దత్తాంశం != శూన్యసూచిక) {
                engine = CreateLSTMBreakEngine(సంకేతం_2, దత్తాంశం, స్థితి_2);
                ఒకవేళ (U_SUCCESS(స్థితి_2) && engine != శూన్యసూచిక) {
                    తిరిగి engine;
                }
                ఒకవేళ (engine != శూన్యసూచిక) {
                    తొలగించు engine;
                    engine = శూన్యసూచిక;
                } లేకపోతే {
                    DeleteLSTMData(దత్తాంశం);
                }
            }
        }
        స్థితి_2 = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = ఎక్కించు_Dictionary_Matcher_For(సంకేతం_2);
        ఒకవేళ (m != శూన్యసూచిక) {
            ఎంపిక(సంకేతం_2) {
            స్థితి USCRIPT_THAI:
                engine = కొత్త ThaiBreakEngine(m, స్థితి_2);
                ఆపు;
            స్థితి USCRIPT_LAO:
                engine = కొత్త LaoBreakEngine(m, స్థితి_2);
                ఆపు;
            స్థితి USCRIPT_MYANMAR:
                engine = కొత్త BurmeseBreakEngine(m, స్థితి_2);
                ఆపు;
            స్థితి USCRIPT_KHMER:
                engine = కొత్త KhmerBreakEngine(m, స్థితి_2);
                ఆపు;

#ఒకవేళ !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            స్థితి USCRIPT_HANGUL:
                engine = కొత్త CjkBreakEngine(m, kKorean, స్థితి_2);
                ఆపు;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            స్థితి USCRIPT_HIRAGANA:
            స్థితి USCRIPT_KATAKANA:
            స్థితి USCRIPT_HAN:
                engine = కొత్త CjkBreakEngine(m, kChineseJapanese, స్థితి_2);
                ఆపు;
#ఒకవేళ 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            స్థితి USCRIPT_COMMON:
            {
                UBlock_సంకేతం ఖండం = ublock_get_సంకేతం(సంకేతం_2);
                ఒకవేళ (ఖండం == UBLOCK_HIRAGANA || ఖండం == UBLOCK_KATAKANA)
                   engine = కొత్త CjkBreakEngine(dict, kChineseJapanese, స్థితి_2);
                ఆపు;
            }
#అయితే_ముగింపు
#అయితే_ముగింపు

            అప్రమేయ:
                ఆపు;
            }
            ఒకవేళ (engine == శూన్యసూచిక) {
                తొలగించు m;
            }
            లేకపోతే ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తొలగించు engine;
                engine = శూన్యసూచిక;
            }
            తిరిగి engine;
        }
    }
    తిరిగి శూన్యసూచిక;
}

DictionaryMatcher *
ICULanguageBreakFactory::ఎక్కించు_Dictionary_Matcher_For(UScript_సంకేతం script) { 
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &స్థితి_2);
    b = ures_get_By_కీలకం_With_Fallback(b, "dictionaries", b, &స్థితి_2);
    int32_t dictnlength = 0;
    స్థిరరాశి char16_t *dictfname =
        ures_get_String_By_కీలకం_With_Fallback(b, uscript_getShortName(script), &dictnlength, &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        ures_close(b);
        తిరిగి శూన్యసూచిక;
    }
    CharString dictnbuf;
    CharString ext;
    స్థిరరాశి char16_t *ext_ప్రారంభం = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    ఒకవేళ (ext_ప్రారంభం != శూన్యసూచిక) {
        int32_t len = static_cast<int32_t>(ext_ప్రారంభం - dictfname);
        ext.appendInvariantChars(UnicodeString(అబద్ధం, ext_ప్రారంభం + 1, dictnlength - len - 1), స్థితి_2);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(అబద్ధం, dictfname, dictnlength), స్థితి_2);
    ures_close(b);

    UData_జ్ఞాపకం *file = udata_open(U_ICUDATA_BRKITR, ext.దత్తాంశం(), dictnbuf.దత్తాంశం(), &స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        // build trie
        స్థిరరాశి uint8_t* దత్తాంశం = static_cast<స్థిరరాశి uint8_t*>(udata_get_జ్ఞాపకం(file));
        స్థిరరాశి int32_t* indexes = reinterpret_cast<స్థిరరాశి int32_t*>(దత్తాంశం);
        స్థిరరాశి int32_t స్థానభ్రంశం = indexes[Dictionary_దత్తాంశం::IX_STRING_TRIE_స్థానభ్రంశం];
        స్థిరరాశి int32_t trieType = indexes[Dictionary_దత్తాంశం::IX_TRIE_TYPE] & Dictionary_దత్తాంశం::TRIE_TYPE_ముసుగు;
        DictionaryMatcher *m = శూన్యసూచిక;
        ఒకవేళ (trieType == Dictionary_దత్తాంశం::TRIE_TYPE_బైట్లు) {
            స్థిరరాశి int32_t transform = indexes[Dictionary_దత్తాంశం::IX_TRANSFORM];
            స్థిరరాశి అక్షరం* characters = reinterpret_cast<స్థిరరాశి అక్షరం*>(దత్తాంశం + స్థానభ్రంశం);
            m = కొత్త బైట్లు_Dictionary_Matcher(characters, transform, file);
        }
        లేకపోతే ఒకవేళ (trieType == Dictionary_దత్తాంశం::TRIE_TYPE_UCHARS) {
            స్థిరరాశి char16_t* characters = reinterpret_cast<స్థిరరాశి char16_t*>(దత్తాంశం + స్థానభ్రంశం);
            m = కొత్త UCharsDictionaryMatcher(characters, file);
        }
        ఒకవేళ (m == శూన్యసూచిక) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        తిరిగి m;
    } లేకపోతే ఒకవేళ (dictfname != శూన్యసూచిక) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        స్థితి_2 = U_ZERO_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి శూన్యసూచిక;
}


శూన్య ICULanguageBreakFactory::కూడు_External_Engine(
        ExternalBreakEngine* external, UError_సంకేతం& స్థితి_2) {
    Local_సూచిక<ExternalBreakEngine> engine(external, స్థితి_2);
    ensureEngines(స్థితి_2);
    Local_సూచిక<BreakEngineWrapper> wrapper(
        కొత్త BreakEngineWrapper(engine.orphan(), స్థితి_2), స్థితి_2);
    స్థిర UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), స్థితి_2);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_సంకేతం &స్థితి_2) : delegate(engine, స్థితి_2) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, స్థిరరాశి అక్షరం* locale) స్థిరరాశి {
    తిరిగి delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::కనుగొను_Breaks(
    UText *పాఠ్యం,
    int32_t ప్రారంభం_Pos,
    int32_t చివర_Pos,
    UVector32 &దొరికింది_Breaks,
    UBool /* isPhraseBreaking */,
    UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    int32_t ఫలితం = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_సమితి_Native_సూచీ(పాఠ్యం, ప్రారంభం_Pos);
    int32_t ప్రారంభం = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
    int32_t current;
    int32_t range_ప్రారంభం;
    int32_t range_చివర;
    UChar32 c = utext_current32(పాఠ్యం);
    అయితే ((current = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < చివర_Pos && delegate->handles(c)) {
        utext_next32(పాఠ్యం);         // TODO:  recast loop for postincrement
        c = utext_current32(పాఠ్యం);
    }
    range_ప్రారంభం = ప్రారంభం;
    range_చివర = current;
    int32_t before_కొలత = దొరికింది_Breaks.size();
    int32_t additional_సామర్థ్యం = range_చివర - range_ప్రారంభం + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    దొరికింది_Breaks.ensure_సామర్థ్యం(before_కొలత+additional_సామర్థ్యం, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    దొరికింది_Breaks.సమితి_కొలత(before_కొలత + before_కొలత+additional_సామర్థ్యం);
    ఫలితం = delegate->fillBreaks(పాఠ్యం, range_ప్రారంభం, range_చివర, దొరికింది_Breaks.get_బఫరు()+before_కొలత,
                                  additional_సామర్థ్యం, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    దొరికింది_Breaks.సమితి_కొలత(before_కొలత + ఫలితం);
    utext_సమితి_Native_సూచీ(పాఠ్యం, current);
    తిరిగి ఫలితం;
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
