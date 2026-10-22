// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ************************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 ************************************************************************************
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "dictbe.h"
#ಅನ್ತರ್ಭಾವ "lstmbe.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "dictionarydata.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "ubrkimpl.h"

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

UnhandledEngine::UnhandledEngine(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) : fHandled(ಶೂನ್ಯಸೂಚಕ) {
    (ಶೂನ್ಯ)ಸ್ಥಿತಿ_2;
}

UnhandledEngine::~UnhandledEngine() {
    ಅಳಿಸು fHandled;
    fHandled = ಶೂನ್ಯಸೂಚಕ;
}

UBool
UnhandledEngine::handles(UChar32 c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale) ಸ್ಥಿರಾಂಕ {
    (ಶೂನ್ಯ)locale; //ಬಳಸದ
    ಹಿಂತಿರುಗಿ fHandled && fHandled->ಒಳಗೊಂಡಿದೆ(c);
}

int32_t
UnhandledEngine::ಕಂಡುಹಿಡಿ_Breaks( UText *ಪಠ್ಯ,
                             int32_t ಆರಂಭ_Pos,
                             int32_t ಕೊನೆ_Pos,
                             UVector32 &/*foundBreaks*/,
                             UBool /* isPhraseBreaking */,
                             UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಆರಂಭ_Pos);
    UChar32 c = utext_current32(ಪಠ್ಯ);
    ಆಗ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) < ಕೊನೆ_Pos && fHandled->ಒಳಗೊಂಡಿದೆ(c)) {
        utext_next32(ಪಠ್ಯ);            // TODO:  recast loop to work with post-increment operations.
        c = utext_current32(ಪಠ್ಯ);
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ
UnhandledEngine::handleCharacter(UChar32 c) {
    ಆದರೆ (fHandled == ಶೂನ್ಯಸೂಚಕ) {
        fHandled = ಹೊಸ Unicode_ಗಣ();
        ಆದರೆ (fHandled == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ;
        }
    }
    ಆದರೆ (!fHandled->ಒಳಗೊಂಡಿದೆ(c)) {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        // Apply the entire script of the character.
        int32_t script = u_get_Int_Property_ಮೌಲ್ಯ(c, UCHAR_SCRIPT);
        fHandled->apply_Int_Property_ಮೌಲ್ಯ(UCHAR_SCRIPT, script, ಸ್ಥಿತಿ_2);
    }
}

/*
 ******************************************************************
 */

ICULanguageBreakFactory::ICULanguageBreakFactory(UError_ಸಂಕೇತ &/*ಸ್ಥಿತಿ*/) {
    fEngines = ಶೂನ್ಯಸೂಚಕ;
}

ICULanguageBreakFactory::~ICULanguageBreakFactory() {
    ಅಳಿಸು fEngines;
}

ಶೂನ್ಯ ICULanguageBreakFactory::ensureEngines(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    ಆದರೆ (fEngines == ಶೂನ್ಯಸೂಚಕ) {
        Local_ಸೂಚಕ<UStack>  engines(ಹೊಸ UStack(uprv_deleteUObject, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            fEngines = engines.orphan();
        }
    }
}

ಸ್ಥಿರಾಂಕ LanguageBreakEngine *
ICULanguageBreakFactory::getEngineFor(UChar32 c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale) {
    ಸ್ಥಿರಾಂಕ LanguageBreakEngine *lbe = ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ  ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ensureEngines(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2) ) {
        // Note: no way to return error code to caller.
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಸ್ಥಿರ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    int32_t i = fEngines->size();
    ಆಗ (--i >= 0) {
        lbe = static_cast<ಸ್ಥಿರಾಂಕ LanguageBreakEngine*>(fEngines->elementAt(i));
        ಆದರೆ (lbe != ಶೂನ್ಯಸೂಚಕ && lbe->handles(c, locale)) {
            ಹಿಂತಿರುಗಿ lbe;
        }
    }

    // We didn't find an engine. Create one.
    lbe = ತುಂಬು_Engine_For(c, locale);
    ಆದರೆ (lbe != ಶೂನ್ಯಸೂಚಕ) {
        fEngines->push((ಶೂನ್ಯ *)lbe, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ U_SUCCESS(ಸ್ಥಿತಿ_2) ? lbe : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ LanguageBreakEngine *
ICULanguageBreakFactory::ತುಂಬು_Engine_For(UChar32 c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    UScript_ಸಂಕೇತ ಸಂಕೇತ_2 = uscript_getScript(c, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಸ್ಥಿರಾಂಕ LanguageBreakEngine *engine = ಶೂನ್ಯಸೂಚಕ;
        // Try to use LSTM first
        ಸ್ಥಿರಾಂಕ LSTMData *ದತ್ತಾಂಶ = CreateLSTMDataForScript(ಸಂಕೇತ_2, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಆದರೆ (ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
                engine = CreateLSTMBreakEngine(ಸಂಕೇತ_2, ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && engine != ಶೂನ್ಯಸೂಚಕ) {
                    ಹಿಂತಿರುಗಿ engine;
                }
                ಆದರೆ (engine != ಶೂನ್ಯಸೂಚಕ) {
                    ಅಳಿಸು engine;
                    engine = ಶೂನ್ಯಸೂಚಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    DeleteLSTMData(ದತ್ತಾಂಶ);
                }
            }
        }
        ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;  // fallback to dictionary based
        DictionaryMatcher *m = ತುಂಬು_Dictionary_Matcher_For(ಸಂಕೇತ_2);
        ಆದರೆ (m != ಶೂನ್ಯಸೂಚಕ) {
            ಆಯ್ಕೆ(ಸಂಕೇತ_2) {
            ಸ್ಥಿತಿ USCRIPT_THAI:
                engine = ಹೊಸ ThaiBreakEngine(m, ಸ್ಥಿತಿ_2);
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ USCRIPT_LAO:
                engine = ಹೊಸ LaoBreakEngine(m, ಸ್ಥಿತಿ_2);
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ USCRIPT_MYANMAR:
                engine = ಹೊಸ BurmeseBreakEngine(m, ಸ್ಥಿತಿ_2);
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ USCRIPT_KHMER:
                engine = ಹೊಸ KhmerBreakEngine(m, ಸ್ಥಿತಿ_2);
                ನಿಲ್ಲಿಸು;

#ಆದರೆ !UCONFIG_NO_NORMALIZATION
                // CJK not available w/o normalization
            ಸ್ಥಿತಿ USCRIPT_HANGUL:
                engine = ಹೊಸ CjkBreakEngine(m, kKorean, ಸ್ಥಿತಿ_2);
                ನಿಲ್ಲಿಸು;

            // use same BreakEngine and dictionary for both Chinese and Japanese
            ಸ್ಥಿತಿ USCRIPT_HIRAGANA:
            ಸ್ಥಿತಿ USCRIPT_KATAKANA:
            ಸ್ಥಿತಿ USCRIPT_HAN:
                engine = ಹೊಸ CjkBreakEngine(m, kChineseJapanese, ಸ್ಥಿತಿ_2);
                ನಿಲ್ಲಿಸು;
#ಆದರೆ 0
            // TODO: Have to get some characters with script=common handled
            // by CjkBreakEngine (e.g. U+309B). Simply subjecting
            // them to CjkBreakEngine does not work. The engine has to
            // special-case them.
            ಸ್ಥಿತಿ USCRIPT_COMMON:
            {
                UBlock_ಸಂಕೇತ ಖಂಡ = ublock_get_ಸಂಕೇತ(ಸಂಕೇತ_2);
                ಆದರೆ (ಖಂಡ == UBLOCK_HIRAGANA || ಖಂಡ == UBLOCK_KATAKANA)
                   engine = ಹೊಸ CjkBreakEngine(dict, kChineseJapanese, ಸ್ಥಿತಿ_2);
                ನಿಲ್ಲಿಸು;
            }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

            ಪೂರ್ವನಿಯೋಜಿತ:
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ (engine == ಶೂನ್ಯಸೂಚಕ) {
                ಅಳಿಸು m;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಅಳಿಸು engine;
                engine = ಶೂನ್ಯಸೂಚಕ;
            }
            ಹಿಂತಿರುಗಿ engine;
        }
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

DictionaryMatcher *
ICULanguageBreakFactory::ತುಂಬು_Dictionary_Matcher_For(UScript_ಸಂಕೇತ script) { 
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &ಸ್ಥಿತಿ_2);
    b = ures_get_By_ಕೀಲಿ_With_Fallback(b, "dictionaries", b, &ಸ್ಥಿತಿ_2);
    int32_t dictnlength = 0;
    ಸ್ಥಿರಾಂಕ char16_t *dictfname =
        ures_get_String_By_ಕೀಲಿ_With_Fallback(b, uscript_getShortName(script), &dictnlength, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ures_close(b);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    CharString dictnbuf;
    CharString ext;
    ಸ್ಥಿರಾಂಕ char16_t *ext_ಆರಂಭ = u_memrchr(dictfname, 0x002e, dictnlength);  // last dot
    ಆದರೆ (ext_ಆರಂಭ != ಶೂನ್ಯಸೂಚಕ) {
        int32_t len = static_cast<int32_t>(ext_ಆರಂಭ - dictfname);
        ext.appendInvariantChars(UnicodeString(ಸುಳ್ಳು, ext_ಆರಂಭ + 1, dictnlength - len - 1), ಸ್ಥಿತಿ_2);
        dictnlength = len;
    }
    dictnbuf.appendInvariantChars(UnicodeString(ಸುಳ್ಳು, dictfname, dictnlength), ಸ್ಥಿತಿ_2);
    ures_close(b);

    UData_ಸ್ಮರಣೆ *file = udata_open(U_ICUDATA_BRKITR, ext.ದತ್ತಾಂಶ(), dictnbuf.ದತ್ತಾಂಶ(), &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        // build trie
        ಸ್ಥಿರಾಂಕ uint8_t* ದತ್ತಾಂಶ = static_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(udata_get_ಸ್ಮರಣೆ(file));
        ಸ್ಥಿರಾಂಕ int32_t* indexes = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(ದತ್ತಾಂಶ);
        ಸ್ಥಿರಾಂಕ int32_t ಸ್ಥಾನಾಂತರ = indexes[Dictionary_ದತ್ತಾಂಶ::IX_STRING_TRIE_ಸ್ಥಾನಾಂತರ];
        ಸ್ಥಿರಾಂಕ int32_t trieType = indexes[Dictionary_ದತ್ತಾಂಶ::IX_TRIE_TYPE] & Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_ಮುಖವಾಡ;
        DictionaryMatcher *m = ಶೂನ್ಯಸೂಚಕ;
        ಆದರೆ (trieType == Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_ಬೈಟುಗಳು) {
            ಸ್ಥಿರಾಂಕ int32_t transform = indexes[Dictionary_ದತ್ತಾಂಶ::IX_TRANSFORM];
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* characters = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ದತ್ತಾಂಶ + ಸ್ಥಾನಾಂತರ);
            m = ಹೊಸ ಬೈಟುಗಳು_Dictionary_Matcher(characters, transform, file);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (trieType == Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_UCHARS) {
            ಸ್ಥಿರಾಂಕ char16_t* characters = reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(ದತ್ತಾಂಶ + ಸ್ಥಾನಾಂತರ);
            m = ಹೊಸ UCharsDictionaryMatcher(characters, file);
        }
        ಆದರೆ (m == ಶೂನ್ಯಸೂಚಕ) {
            // no matcher exists to take ownership - either we are an invalid 
            // type or memory allocation failed
            udata_close(file);
        }
        ಹಿಂತಿರುಗಿ m;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dictfname != ಶೂನ್ಯಸೂಚಕ) {
        // we don't have a dictionary matcher.
        // returning nullptr here will cause us to fail to find a dictionary break engine, as expected
        ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}


ಶೂನ್ಯ ICULanguageBreakFactory::ಕೂಡಿಸು_External_Engine(
        ExternalBreakEngine* external, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    Local_ಸೂಚಕ<ExternalBreakEngine> engine(external, ಸ್ಥಿತಿ_2);
    ensureEngines(ಸ್ಥಿತಿ_2);
    Local_ಸೂಚಕ<BreakEngineWrapper> wrapper(
        ಹೊಸ BreakEngineWrapper(engine.orphan(), ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
    ಸ್ಥಿರ UMutex gBreakEngineMutex;
    Mutex m(&gBreakEngineMutex);
    fEngines->push(wrapper.getAlias(), ಸ್ಥಿತಿ_2);
    wrapper.orphan();
}

BreakEngineWrapper::BreakEngineWrapper(
    ExternalBreakEngine* engine, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) : delegate(engine, ಸ್ಥಿತಿ_2) {
}

BreakEngineWrapper::~BreakEngineWrapper() {
}

UBool BreakEngineWrapper::handles(UChar32 c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ delegate->isFor(c, locale);
}

int32_t BreakEngineWrapper::ಕಂಡುಹಿಡಿ_Breaks(
    UText *ಪಠ್ಯ,
    int32_t ಆರಂಭ_Pos,
    int32_t ಕೊನೆ_Pos,
    UVector32 &ಸಿಕ್ಕಿತು_Breaks,
    UBool /* isPhraseBreaking */,
    UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    int32_t ಫಲಿತಾಂಶ = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಆರಂಭ_Pos);
    int32_t ಆರಂಭ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
    int32_t current;
    int32_t range_ಆರಂಭ;
    int32_t range_ಕೊನೆ;
    UChar32 c = utext_current32(ಪಠ್ಯ);
    ಆಗ ((current = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < ಕೊನೆ_Pos && delegate->handles(c)) {
        utext_next32(ಪಠ್ಯ);         // TODO:  recast loop for postincrement
        c = utext_current32(ಪಠ್ಯ);
    }
    range_ಆರಂಭ = ಆರಂಭ;
    range_ಕೊನೆ = current;
    int32_t before_ಗಾತ್ರ = ಸಿಕ್ಕಿತು_Breaks.size();
    int32_t additional_ಸಾಮರ್ಥ್ಯ = range_ಕೊನೆ - range_ಆರಂಭ + 1;
    // enlarge to contains (rangeEnd-rangeStart+1) more items
    ಸಿಕ್ಕಿತು_Breaks.ensure_ಸಾಮರ್ಥ್ಯ(before_ಗಾತ್ರ+additional_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    ಸಿಕ್ಕಿತು_Breaks.ಗಣ_ಗಾತ್ರ(before_ಗಾತ್ರ + before_ಗಾತ್ರ+additional_ಸಾಮರ್ಥ್ಯ);
    ಫಲಿತಾಂಶ = delegate->fillBreaks(ಪಠ್ಯ, range_ಆರಂಭ, range_ಕೊನೆ, ಸಿಕ್ಕಿತು_Breaks.get_ಬಫರ್()+before_ಗಾತ್ರ,
                                  additional_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    ಸಿಕ್ಕಿತು_Breaks.ಗಣ_ಗಾತ್ರ(before_ಗಾತ್ರ + ಫಲಿತಾಂಶ);
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
