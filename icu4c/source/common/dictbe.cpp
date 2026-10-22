// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 *******************************************************************************
 */

#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "dictbe.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/usetiter.h"
#ಅನ್ತರ್ಭಾವ "ubrkimpl.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "unicode/normlzr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "dictionarydata.h"

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

DictionaryBreakEngine::DictionaryBreakEngine() {
}

DictionaryBreakEngine::~DictionaryBreakEngine() {
}

UBool
DictionaryBreakEngine::handles(UChar32 c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ f_ಗಣ.ಒಳಗೊಂಡಿದೆ(c);
}

int32_t
DictionaryBreakEngine::ಕಂಡುಹಿಡಿ_Breaks( UText *ಪಠ್ಯ,
                                 int32_t ಆರಂಭ_Pos,
                                 int32_t ಕೊನೆ_Pos,
                                 UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
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
    ಆಗ ((current = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < ಕೊನೆ_Pos && f_ಗಣ.ಒಳಗೊಂಡಿದೆ(c)) {
        utext_next32(ಪಠ್ಯ);         // TODO:  recast loop for postincrement
        c = utext_current32(ಪಠ್ಯ);
    }
    range_ಆರಂಭ = ಆರಂಭ;
    range_ಕೊನೆ = current;
    ಫಲಿತಾಂಶ = divide_ಮೇಲೆ_Dictionary_Range(ಪಠ್ಯ, range_ಆರಂಭ, range_ಕೊನೆ, ಸಿಕ್ಕಿತು_Breaks, isPhraseBreaking, ಸ್ಥಿತಿ_2);
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current);
    
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ
DictionaryBreakEngine::ಗಣ_Characters( ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ ) {
    f_ಗಣ = ಗಣ;
    // Compact for caching
    f_ಗಣ.compact();
}

/*
 ******************************************************************
 * PossibleWord
 */

// Helper class for improving readability of the Thai/Lao/Khmer word break
// algorithm. The implementation is completely inline.

// List size, limited by the maximum number of words in the dictionary
// that form a nested sequence.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t POSSIBLE_ಶಬ್ದ_LIST_MAX = 20;

ವರ್ಗ Possible_ಶಬ್ದ {
ಖಾಸಗಿ:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   ಪೂರ್ವಪ್ರತ್ಯಯ;     // The longest match with a dictionary word
    int32_t   ಸ್ಥಾನಾಂತರ;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_ಶಬ್ದ_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_ಶಬ್ದ_LIST_MAX];   // Word Lengths, in code points.

ಸಾರ್ವಜನಿಕ:
    Possible_ಶಬ್ದ() : count(0), ಪೂರ್ವಪ್ರತ್ಯಯ(0), ಸ್ಥಾನಾಂತರ(-1), mark(0), current(0) {}
    ~Possible_ಶಬ್ದ() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *ಪಠ್ಯ, DictionaryMatcher *dict, int32_t range_ಕೊನೆ );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *ಪಠ್ಯ );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     ಹಿಂಭಾಗ_ಮೇಲೆ( UText *ಪಠ್ಯ );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_ಪೂರ್ವಪ್ರತ್ಯಯ() { ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ; }
  
    // Mark the current candidate as the one we like
    ಶೂನ್ಯ      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { ಹಿಂತಿರುಗಿ cpLengths[mark]; }
};


int32_t Possible_ಶಬ್ದ::candidates( UText *ಪಠ್ಯ, DictionaryMatcher *dict, int32_t range_ಕೊನೆ ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t ಆರಂಭ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
    ಆದರೆ (ಆರಂಭ != ಸ್ಥಾನಾಂತರ) {
        ಸ್ಥಾನಾಂತರ = ಆರಂಭ;
        count = dict->matches(ಪಠ್ಯ, range_ಕೊನೆ-ಆರಂಭ, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, ಶೂನ್ಯಸೂಚಕ, &ಪೂರ್ವಪ್ರತ್ಯಯ);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        ಆದರೆ (count <= 0) {
            utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಆರಂಭ);
        }
    }
    ಆದರೆ (count > 0) {
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಆರಂಭ+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    ಹಿಂತಿರುಗಿ count;
}

int32_t
Possible_ಶಬ್ದ::acceptMarked( UText *ಪಠ್ಯ ) {
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಸ್ಥಾನಾಂತರ + cuLengths[mark]);
    ಹಿಂತಿರುಗಿ cuLengths[mark];
}


UBool
Possible_ಶಬ್ದ::ಹಿಂಭಾಗ_ಮೇಲೆ( UText *ಪಠ್ಯ ) {
    ಆದರೆ (current > 0) {
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಸ್ಥಾನಾಂತರ + cuLengths[--current]);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 ******************************************************************
 * ThaiBreakEngine
 */

// How many words in a row are "good enough"?
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t THAI_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t THAI_ಬೇರು_COMBINE_ಮಿತಿ = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t THAI_ಪೂರ್ವಪ್ರತ್ಯಯ_COMBINE_ಮಿತಿ = 3;

// Elision character
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t THAI_MIN_ಶಬ್ದ = 2;

// Minimum number of characters for two words
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t THAI_MIN_ಶಬ್ದ_ಹರವು = THAI_MIN_ಶಬ್ದ * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_ಗಣ thai_ಶಬ್ದ_ಗಣ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಗಣ_Characters(thai_ಶಬ್ದ_ಗಣ);
    }
    f_Mark_ಗಣ.apply_ನಮೂನೆ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), ಸ್ಥಿತಿ_2);
    f_Mark_ಗಣ.ಕೂಡಿಸು(0x0020);
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ = thai_ಶಬ್ದ_ಗಣ;
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.remove(0x0E31);             // MAI HAN-AKAT
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_ಶಬ್ದ_ಗಣ.ಕೂಡಿಸು(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_ಶಬ್ದ_ಗಣ.ಕೂಡಿಸು(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_ಗಣ.ಕೂಡಿಸು(THAI_PAIYANNOI);
    f_Suffix_ಗಣ.ಕೂಡಿಸು(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_ಗಣ.compact();
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.compact();
    f_Begin_ಶಬ್ದ_ಗಣ.compact();
    f_Suffix_ಗಣ.compact();
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    ಅಳಿಸು fDictionary;
}

int32_t
ThaiBreakEngine::divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                                int32_t range_ಆರಂಭ,
                                                int32_t range_ಕೊನೆ,
                                                UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, range_ಆರಂಭ);
    utext_moveIndex32(ಪಠ್ಯ, THAI_MIN_ಶಬ್ದ_ಹರವು);
    ಆದರೆ (utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ) >= range_ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ 0;       // Not enough characters for two words
    }
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, range_ಆರಂಭ);


    uint32_t ಪದಗಳು_ಸಿಕ್ಕಿತು = 0;
    int32_t cp_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;    // Word Length in Code Points.
    int32_t cu_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    Possible_ಶಬ್ದ ಪದಗಳು[THAI_LOOKAHEAD];
    
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, range_ಆರಂಭ);
    
    ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2) && (current = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < range_ಕೊನೆ) {
        cp_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
        cu_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;

        // Look for candidate words at the current position
        int32_t candidates = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%THAI_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ);
        
        // If we found exactly one, use that
        ಆದರೆ (candidates == 1) {
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % THAI_LOOKAHEAD].acceptMarked(ಪಠ್ಯ);
            cp_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % THAI_LOOKAHEAD].markedCPLength();
            ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) >= range_ಕೊನೆ) {
                ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
            }
            ಮಾಡು {
                ಆದರೆ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % THAI_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) >= range_ಕೊನೆ) {
                        ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    ಮಾಡು {
                        // If we find a third word, stop right away
                        ಆದರೆ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 2) % THAI_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ)) {
                            ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % THAI_LOOKAHEAD].markCurrent();
                            ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
                        }
                    }
                    ಆಗ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % THAI_LOOKAHEAD].ಹಿಂಭಾಗ_ಮೇಲೆ(ಪಠ್ಯ));
                }
            }
            ಆಗ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % THAI_LOOKAHEAD].ಹಿಂಭಾಗ_ಮೇಲೆ(ಪಠ್ಯ));
ಸಿಕ್ಕಿತು_ಉತ್ತಮ:
            // Set UText position to after the accepted word.
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % THAI_LOOKAHEAD].acceptMarked(ಪಠ್ಯ);
            cp_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % THAI_LOOKAHEAD].markedCPLength();
            ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) < range_ಕೊನೆ && cp_ಶಬ್ದ_ಉದ್ದಳತೆ < THAI_ಬೇರು_COMBINE_ಮಿತಿ) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ಆದರೆ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % THAI_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) <= 0
                  && (cu_ಶಬ್ದ_ಉದ್ದಳತೆ == 0
                      || ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%THAI_LOOKAHEAD].longest_ಪೂರ್ವಪ್ರತ್ಯಯ() < THAI_ಪೂರ್ವಪ್ರತ್ಯಯ_COMBINE_ಮಿತಿ)) {
                // Look for a plausible word boundary
                int32_t ಉಳಿದ = range_ಕೊನೆ - (current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
                UChar32 pc;
                int32_t chars = 0;
                ಗಾಗಿ (;;) {
                    int32_t pc_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
                    pc = utext_next32(ಪಠ್ಯ);
                    int32_t pc_ಗಾತ್ರ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - pc_ಸೂಚ್ಯಂಕ;
                    chars += pc_ಗಾತ್ರ;
                    ಉಳಿದ -= pc_ಗಾತ್ರ;
                    ಆದರೆ (ಉಳಿದ <= 0) {
                        ನಿಲ್ಲಿಸು;
                    }
                    uc = utext_current32(ಪಠ್ಯ);
                    ಆದರೆ (f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(pc) && f_Begin_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t ಅಂಕೆ_candidates = ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % THAI_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ);
                        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current + cu_ಶಬ್ದ_ಉದ್ದಳತೆ + chars);
                        ಆದರೆ (ಅಂಕೆ_candidates > 0) {
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ಆದರೆ (cu_ಶಬ್ದ_ಉದ್ದಳತೆ <= 0) {
                    ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
                }
                
                // Update the length with the passed-over characters
                cu_ಶಬ್ದ_ಉದ್ದಳತೆ += chars;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                // Back up to where we were for next iteration
                utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        ಆಗ ((currPos = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < range_ಕೊನೆ && f_Mark_ಗಣ.ಒಳಗೊಂಡಿದೆ(utext_current32(ಪಠ್ಯ))) {
            utext_next32(ಪಠ್ಯ);
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ += static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) < range_ಕೊನೆ && cu_ಶಬ್ದ_ಉದ್ದಳತೆ > 0) {
            ಆದರೆ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%THAI_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) <= 0
                && f_Suffix_ಗಣ.ಒಳಗೊಂಡಿದೆ(uc = utext_current32(ಪಠ್ಯ))) {
                ಆದರೆ (uc == THAI_PAIYANNOI) {
                    ಆದರೆ (!f_Suffix_ಗಣ.ಒಳಗೊಂಡಿದೆ(utext_previous32(ಪಠ್ಯ))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(ಪಠ್ಯ);
                        int32_t paiyannoi_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
                        utext_next32(ಪಠ್ಯ);
                        cu_ಶಬ್ದ_ಉದ್ದಳತೆ += static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - paiyannoi_ಸೂಚ್ಯಂಕ; // Add PAIYANNOI to word
                        uc = utext_current32(ಪಠ್ಯ);     // Fetch next character
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        // Restore prior position
                        utext_next32(ಪಠ್ಯ);
                    }
                }
                ಆದರೆ (uc == THAI_MAIYAMOK) {
                    ಆದರೆ (utext_previous32(ಪಠ್ಯ) != THAI_MAIYAMOK) {
                        // Skip over previous end and MAIYAMOK
                        utext_next32(ಪಠ್ಯ);
                        int32_t maiyamok_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
                        utext_next32(ಪಠ್ಯ);
                        cu_ಶಬ್ದ_ಉದ್ದಳತೆ += static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - maiyamok_ಸೂಚ್ಯಂಕ; // Add MAIYAMOK to word
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        // Restore prior position
                        utext_next32(ಪಠ್ಯ);
                    }
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        ಆದರೆ (cu_ಶಬ್ದ_ಉದ್ದಳತೆ > 0) {
            ಸಿಕ್ಕಿತು_Breaks.push((current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ), ಸ್ಥಿತಿ_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ಆದರೆ (ಸಿಕ್ಕಿತು_Breaks.peeki() >= range_ಕೊನೆ) {
        (ಶೂನ್ಯ) ಸಿಕ್ಕಿತು_Breaks.popi();
        ಪದಗಳು_ಸಿಕ್ಕಿತು -= 1;
    }

    ಹಿಂತಿರುಗಿ ಪದಗಳು_ಸಿಕ್ಕಿತು;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t LAO_ಬೇರು_COMBINE_ಮಿತಿ = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t LAO_ಪೂರ್ವಪ್ರತ್ಯಯ_COMBINE_ಮಿತಿ = 3;

// Minimum word size
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t LAO_MIN_ಶಬ್ದ = 2;

// Minimum number of characters for two words
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t LAO_MIN_ಶಬ್ದ_ಹರವು = LAO_MIN_ಶಬ್ದ * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_ಗಣ lao_ಶಬ್ದ_ಗಣ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಗಣ_Characters(lao_ಶಬ್ದ_ಗಣ);
    }
    f_Mark_ಗಣ.apply_ನಮೂನೆ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), ಸ್ಥಿತಿ_2);
    f_Mark_ಗಣ.ಕೂಡಿಸು(0x0020);
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ = lao_ಶಬ್ದ_ಗಣ;
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_ಶಬ್ದ_ಗಣ.ಕೂಡಿಸು(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_ಶಬ್ದ_ಗಣ.ಕೂಡಿಸು(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_ಶಬ್ದ_ಗಣ.ಕೂಡಿಸು(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_ಗಣ.compact();
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.compact();
    f_Begin_ಶಬ್ದ_ಗಣ.compact();
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

LaoBreakEngine::~LaoBreakEngine() {
    ಅಳಿಸು fDictionary;
}

int32_t
LaoBreakEngine::divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                                int32_t range_ಆರಂಭ,
                                                int32_t range_ಕೊನೆ,
                                                UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ ((range_ಕೊನೆ - range_ಆರಂಭ) < LAO_MIN_ಶಬ್ದ_ಹರವು) {
        ಹಿಂತಿರುಗಿ 0;       // Not enough characters for two words
    }

    uint32_t ಪದಗಳು_ಸಿಕ್ಕಿತು = 0;
    int32_t cp_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
    int32_t cu_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
    int32_t current;
    Possible_ಶಬ್ದ ಪದಗಳು[LAO_LOOKAHEAD];

    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, range_ಆರಂಭ);

    ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2) && (current = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < range_ಕೊನೆ) {
        cu_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
        cp_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;

        // Look for candidate words at the current position
        int32_t candidates = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%LAO_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ);
        
        // If we found exactly one, use that
        ಆದರೆ (candidates == 1) {
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % LAO_LOOKAHEAD].acceptMarked(ಪಠ್ಯ);
            cp_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % LAO_LOOKAHEAD].markedCPLength();
            ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ಆದರೆ (utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ) >= range_ಕೊನೆ) {
                ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
            }
            ಮಾಡು {
                ಆದರೆ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % LAO_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) >= range_ಕೊನೆ) {
                        ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    ಮಾಡು {
                        // If we find a third word, stop right away
                        ಆದರೆ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 2) % LAO_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ)) {
                            ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % LAO_LOOKAHEAD].markCurrent();
                            ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
                        }
                    }
                    ಆಗ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % LAO_LOOKAHEAD].ಹಿಂಭಾಗ_ಮೇಲೆ(ಪಠ್ಯ));
                }
            }
            ಆಗ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % LAO_LOOKAHEAD].ಹಿಂಭಾಗ_ಮೇಲೆ(ಪಠ್ಯ));
ಸಿಕ್ಕಿತು_ಉತ್ತಮ:
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % LAO_LOOKAHEAD].acceptMarked(ಪಠ್ಯ);
            cp_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % LAO_LOOKAHEAD].markedCPLength();
            ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) < range_ಕೊನೆ && cp_ಶಬ್ದ_ಉದ್ದಳತೆ < LAO_ಬೇರು_COMBINE_ಮಿತಿ) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ಆದರೆ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % LAO_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) <= 0
                  && (cu_ಶಬ್ದ_ಉದ್ದಳತೆ == 0
                      || ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%LAO_LOOKAHEAD].longest_ಪೂರ್ವಪ್ರತ್ಯಯ() < LAO_ಪೂರ್ವಪ್ರತ್ಯಯ_COMBINE_ಮಿತಿ)) {
                // Look for a plausible word boundary
                int32_t ಉಳಿದ = range_ಕೊನೆ - (current + cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                ಗಾಗಿ (;;) {
                    int32_t pc_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
                    pc = utext_next32(ಪಠ್ಯ);
                    int32_t pc_ಗಾತ್ರ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - pc_ಸೂಚ್ಯಂಕ;
                    chars += pc_ಗಾತ್ರ;
                    ಉಳಿದ -= pc_ಗಾತ್ರ;
                    ಆದರೆ (ಉಳಿದ <= 0) {
                        ನಿಲ್ಲಿಸು;
                    }
                    uc = utext_current32(ಪಠ್ಯ);
                    ಆದರೆ (f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(pc) && f_Begin_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t ಅಂಕೆ_candidates = ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % LAO_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ);
                        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current + cu_ಶಬ್ದ_ಉದ್ದಳತೆ + chars);
                        ಆದರೆ (ಅಂಕೆ_candidates > 0) {
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ಆದರೆ (cu_ಶಬ್ದ_ಉದ್ದಳತೆ <= 0) {
                    ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
                }
                
                // Update the length with the passed-over characters
                cu_ಶಬ್ದ_ಉದ್ದಳತೆ += chars;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                // Back up to where we were for next iteration
                utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current + cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        ಆಗ ((currPos = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < range_ಕೊನೆ && f_Mark_ಗಣ.ಒಳಗೊಂಡಿದೆ(utext_current32(ಪಠ್ಯ))) {
            utext_next32(ಪಠ್ಯ);
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ += static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        ಆದರೆ (cu_ಶಬ್ದ_ಉದ್ದಳತೆ > 0) {
            ಸಿಕ್ಕಿತು_Breaks.push((current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ), ಸ್ಥಿತಿ_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ಆದರೆ (ಸಿಕ್ಕಿತು_Breaks.peeki() >= range_ಕೊನೆ) {
        (ಶೂನ್ಯ) ಸಿಕ್ಕಿತು_Breaks.popi();
        ಪದಗಳು_ಸಿಕ್ಕಿತು -= 1;
    }

    ಹಿಂತಿರುಗಿ ಪದಗಳು_ಸಿಕ್ಕಿತು;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t BURMESE_ಬೇರು_COMBINE_ಮಿತಿ = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t BURMESE_ಪೂರ್ವಪ್ರತ್ಯಯ_COMBINE_ಮಿತಿ = 3;

// Minimum word size
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t BURMESE_MIN_ಶಬ್ದ = 2;

// Minimum number of characters for two words
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t BURMESE_MIN_ಶಬ್ದ_ಹರವು = BURMESE_MIN_ಶಬ್ದ * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_ಶಬ್ದ_ಗಣ.ಕೂಡಿಸು(0x1000, 0x102A);      // basic consonants and independent vowels
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.apply_ನಮೂನೆ(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), ಸ್ಥಿತಿ_2);
    f_Mark_ಗಣ.apply_ನಮೂನೆ(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), ಸ್ಥಿತಿ_2);
    f_Mark_ಗಣ.ಕೂಡಿಸು(0x0020);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಗಣ_Characters(f_ಕೊನೆ_ಶಬ್ದ_ಗಣ);
    }

    // Compact for caching.
    f_Mark_ಗಣ.compact();
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.compact();
    f_Begin_ಶಬ್ದ_ಗಣ.compact();
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    ಅಳಿಸು fDictionary;
}

int32_t
BurmeseBreakEngine::divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                                int32_t range_ಆರಂಭ,
                                                int32_t range_ಕೊನೆ,
                                                UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2 ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ ((range_ಕೊನೆ - range_ಆರಂಭ) < BURMESE_MIN_ಶಬ್ದ_ಹರವು) {
        ಹಿಂತಿರುಗಿ 0;       // Not enough characters for two words
    }

    uint32_t ಪದಗಳು_ಸಿಕ್ಕಿತು = 0;
    int32_t cp_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
    int32_t cu_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
    int32_t current;
    Possible_ಶಬ್ದ ಪದಗಳು[BURMESE_LOOKAHEAD];

    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, range_ಆರಂಭ);

    ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2) && (current = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < range_ಕೊನೆ) {
        cu_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
        cp_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;

        // Look for candidate words at the current position
        int32_t candidates = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%BURMESE_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ);
        
        // If we found exactly one, use that
        ಆದರೆ (candidates == 1) {
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % BURMESE_LOOKAHEAD].acceptMarked(ಪಠ್ಯ);
            cp_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % BURMESE_LOOKAHEAD].markedCPLength();
            ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ಆದರೆ (utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ) >= range_ಕೊನೆ) {
                ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
            }
            ಮಾಡು {
                ಆದರೆ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % BURMESE_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) >= range_ಕೊನೆ) {
                        ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    ಮಾಡು {
                        // If we find a third word, stop right away
                        ಆದರೆ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 2) % BURMESE_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ)) {
                            ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % BURMESE_LOOKAHEAD].markCurrent();
                            ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
                        }
                    }
                    ಆಗ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % BURMESE_LOOKAHEAD].ಹಿಂಭಾಗ_ಮೇಲೆ(ಪಠ್ಯ));
                }
            }
            ಆಗ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % BURMESE_LOOKAHEAD].ಹಿಂಭಾಗ_ಮೇಲೆ(ಪಠ್ಯ));
ಸಿಕ್ಕಿತು_ಉತ್ತಮ:
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % BURMESE_LOOKAHEAD].acceptMarked(ಪಠ್ಯ);
            cp_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % BURMESE_LOOKAHEAD].markedCPLength();
            ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) < range_ಕೊನೆ && cp_ಶಬ್ದ_ಉದ್ದಳತೆ < BURMESE_ಬೇರು_COMBINE_ಮಿತಿ) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ಆದರೆ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % BURMESE_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) <= 0
                  && (cu_ಶಬ್ದ_ಉದ್ದಳತೆ == 0
                      || ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%BURMESE_LOOKAHEAD].longest_ಪೂರ್ವಪ್ರತ್ಯಯ() < BURMESE_ಪೂರ್ವಪ್ರತ್ಯಯ_COMBINE_ಮಿತಿ)) {
                // Look for a plausible word boundary
                int32_t ಉಳಿದ = range_ಕೊನೆ - (current + cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                ಗಾಗಿ (;;) {
                    int32_t pc_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
                    pc = utext_next32(ಪಠ್ಯ);
                    int32_t pc_ಗಾತ್ರ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - pc_ಸೂಚ್ಯಂಕ;
                    chars += pc_ಗಾತ್ರ;
                    ಉಳಿದ -= pc_ಗಾತ್ರ;
                    ಆದರೆ (ಉಳಿದ <= 0) {
                        ನಿಲ್ಲಿಸು;
                    }
                    uc = utext_current32(ಪಠ್ಯ);
                    ಆದರೆ (f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(pc) && f_Begin_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t ಅಂಕೆ_candidates = ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % BURMESE_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ);
                        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current + cu_ಶಬ್ದ_ಉದ್ದಳತೆ + chars);
                        ಆದರೆ (ಅಂಕೆ_candidates > 0) {
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ಆದರೆ (cu_ಶಬ್ದ_ಉದ್ದಳತೆ <= 0) {
                    ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
                }
                
                // Update the length with the passed-over characters
                cu_ಶಬ್ದ_ಉದ್ದಳತೆ += chars;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                // Back up to where we were for next iteration
                utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current + cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        ಆಗ ((currPos = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < range_ಕೊನೆ && f_Mark_ಗಣ.ಒಳಗೊಂಡಿದೆ(utext_current32(ಪಠ್ಯ))) {
            utext_next32(ಪಠ್ಯ);
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ += static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        ಆದರೆ (cu_ಶಬ್ದ_ಉದ್ದಳತೆ > 0) {
            ಸಿಕ್ಕಿತು_Breaks.push((current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ), ಸ್ಥಿತಿ_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ಆದರೆ (ಸಿಕ್ಕಿತು_Breaks.peeki() >= range_ಕೊನೆ) {
        (ಶೂನ್ಯ) ಸಿಕ್ಕಿತು_Breaks.popi();
        ಪದಗಳು_ಸಿಕ್ಕಿತು -= 1;
    }

    ಹಿಂತಿರುಗಿ ಪದಗಳು_ಸಿಕ್ಕಿತು;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t KHMER_ಬೇರು_COMBINE_ಮಿತಿ = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t KHMER_ಪೂರ್ವಪ್ರತ್ಯಯ_COMBINE_ಮಿತಿ = 3;

// Minimum word size
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t KHMER_MIN_ಶಬ್ದ = 2;

// Minimum number of characters for two words
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t KHMER_MIN_ಶಬ್ದ_ಹರವು = KHMER_MIN_ಶಬ್ದ * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_ಗಣ khmer_ಶಬ್ದ_ಗಣ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಗಣ_Characters(khmer_ಶಬ್ದ_ಗಣ);
    }
    f_Mark_ಗಣ.apply_ನಮೂನೆ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), ಸ್ಥಿತಿ_2);
    f_Mark_ಗಣ.ಕೂಡಿಸು(0x0020);
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ = khmer_ಶಬ್ದ_ಗಣ;
    f_Begin_ಶಬ್ದ_ಗಣ.ಕೂಡಿಸು(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_ಗಣ.compact();
    f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.compact();
    f_Begin_ಶಬ್ದ_ಗಣ.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    ಅಳಿಸು fDictionary;
}

int32_t
KhmerBreakEngine::divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                                int32_t range_ಆರಂಭ,
                                                int32_t range_ಕೊನೆ,
                                                UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2 ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ ((range_ಕೊನೆ - range_ಆರಂಭ) < KHMER_MIN_ಶಬ್ದ_ಹರವು) {
        ಹಿಂತಿರುಗಿ 0;       // Not enough characters for two words
    }

    uint32_t ಪದಗಳು_ಸಿಕ್ಕಿತು = 0;
    int32_t cp_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
    int32_t cu_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
    int32_t current;
    Possible_ಶಬ್ದ ಪದಗಳು[KHMER_LOOKAHEAD];

    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, range_ಆರಂಭ);

    ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2) && (current = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < range_ಕೊನೆ) {
        cu_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;
        cp_ಶಬ್ದ_ಉದ್ದಳತೆ = 0;

        // Look for candidate words at the current position
        int32_t candidates = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು%KHMER_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ);

        // If we found exactly one, use that
        ಆದರೆ (candidates == 1) {
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].acceptMarked(ಪಠ್ಯ);
            cp_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].markedCPLength();
            ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) >= range_ಕೊನೆ) {
                ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
            }
            ಮಾಡು {
                ಆದರೆ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % KHMER_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) >= range_ಕೊನೆ) {
                        ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
                    }

                    // See if any of the possible second words is followed by a third word
                    ಮಾಡು {
                        // If we find a third word, stop right away
                        ಆದರೆ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 2) % KHMER_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ)) {
                            ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].markCurrent();
                            ಹೋಗು ಸಿಕ್ಕಿತು_ಉತ್ತಮ;
                        }
                    }
                    ಆಗ (ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % KHMER_LOOKAHEAD].ಹಿಂಭಾಗ_ಮೇಲೆ(ಪಠ್ಯ));
                }
            }
            ಆಗ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].ಹಿಂಭಾಗ_ಮೇಲೆ(ಪಠ್ಯ));
ಸಿಕ್ಕಿತು_ಉತ್ತಮ:
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].acceptMarked(ಪಠ್ಯ);
            cp_ಶಬ್ದ_ಉದ್ದಳತೆ = ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].markedCPLength();
            ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ಆದರೆ (static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) < range_ಕೊನೆ && cp_ಶಬ್ದ_ಉದ್ದಳತೆ < KHMER_ಬೇರು_COMBINE_ಮಿತಿ) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ಆದರೆ (ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ) <= 0
                  && (cu_ಶಬ್ದ_ಉದ್ದಳತೆ == 0
                      || ಪದಗಳು[ಪದಗಳು_ಸಿಕ್ಕಿತು % KHMER_LOOKAHEAD].longest_ಪೂರ್ವಪ್ರತ್ಯಯ() < KHMER_ಪೂರ್ವಪ್ರತ್ಯಯ_COMBINE_ಮಿತಿ)) {
                // Look for a plausible word boundary
                int32_t ಉಳಿದ = range_ಕೊನೆ - (current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                ಗಾಗಿ (;;) {
                    int32_t pc_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
                    pc = utext_next32(ಪಠ್ಯ);
                    int32_t pc_ಗಾತ್ರ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - pc_ಸೂಚ್ಯಂಕ;
                    chars += pc_ಗಾತ್ರ;
                    ಉಳಿದ -= pc_ಗಾತ್ರ;
                    ಆದರೆ (ಉಳಿದ <= 0) {
                        ನಿಲ್ಲಿಸು;
                    }
                    uc = utext_current32(ಪಠ್ಯ);
                    ಆದರೆ (f_ಕೊನೆ_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(pc) && f_Begin_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t ಅಂಕೆ_candidates = ಪದಗಳು[(ಪದಗಳು_ಸಿಕ್ಕಿತು + 1) % KHMER_LOOKAHEAD].candidates(ಪಠ್ಯ, fDictionary, range_ಕೊನೆ);
                        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ+chars);
                        ಆದರೆ (ಅಂಕೆ_candidates > 0) {
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                ಆದರೆ (cu_ಶಬ್ದ_ಉದ್ದಳತೆ <= 0) {
                    ಪದಗಳು_ಸಿಕ್ಕಿತು += 1;
                }

                // Update the length with the passed-over characters
                cu_ಶಬ್ದ_ಉದ್ದಳತೆ += chars;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                // Back up to where we were for next iteration
                utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        ಆಗ ((currPos = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < range_ಕೊನೆ && f_Mark_ಗಣ.ಒಳಗೊಂಡಿದೆ(utext_current32(ಪಠ್ಯ))) {
            utext_next32(ಪಠ್ಯ);
            cu_ಶಬ್ದ_ಉದ್ದಳತೆ += static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
//        if ((int32_t)utext_getNativeIndex(text) < rangeEnd && wordLength > 0) {
//            if (words[wordsFound%KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
//                && fSuffixSet.contains(uc = utext_current32(text))) {
//                if (uc == KHMER_PAIYANNOI) {
//                    if (!fSuffixSet.contains(utext_previous32(text))) {
//                        // Skip over previous end and PAIYANNOI
//                        utext_next32(text);
//                        utext_next32(text);
//                        wordLength += 1;            // Add PAIYANNOI to word
//                        uc = utext_current32(text);     // Fetch next character
//                    }
//                    else {
//                        // Restore prior position
//                        utext_next32(text);
//                    }
//                }
//                if (uc == KHMER_MAIYAMOK) {
//                    if (utext_previous32(text) != KHMER_MAIYAMOK) {
//                        // Skip over previous end and MAIYAMOK
//                        utext_next32(text);
//                        utext_next32(text);
//                        wordLength += 1;            // Add MAIYAMOK to word
//                    }
//                    else {
//                        // Restore prior position
//                        utext_next32(text);
//                    }
//                }
//            }
//            else {
//                utext_setNativeIndex(text, current+wordLength);
//            }
//        }

        // Did we find a word on this iteration? If so, push it on the break stack
        ಆದರೆ (cu_ಶಬ್ದ_ಉದ್ದಳತೆ > 0) {
            ಸಿಕ್ಕಿತು_Breaks.push((current+cu_ಶಬ್ದ_ಉದ್ದಳತೆ), ಸ್ಥಿತಿ_2);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    ಆದರೆ (ಸಿಕ್ಕಿತು_Breaks.peeki() >= range_ಕೊನೆ) {
        (ಶೂನ್ಯ) ಸಿಕ್ಕಿತು_Breaks.popi();
        ಪದಗಳು_ಸಿಕ್ಕಿತು -= 1;
    }

    ಹಿಂತಿರುಗಿ ಪದಗಳು_ಸಿಕ್ಕಿತು;
}

#ಆದರೆ !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(ಸುಳ್ಳು) {
    UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = ಶೂನ್ಯಸೂಚಕ;
    nfkcNorm2 = Normalizer2::getNFKCInstance(ಸ್ಥಿತಿ_2);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_ಶಬ್ದ_ಗಣ.apply_ನಮೂನೆ(UnicodeString(u"[\\uac00-\\ud7a3]"), ಸ್ಥಿತಿ_2);
    f_Hangul_ಶಬ್ದ_ಗಣ.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ.apply_ನಮೂನೆ(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), ಸ್ಥಿತಿ_2);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ.compact();
    f_Close_Punctuation_ಗಣ.apply_ನಮೂನೆ(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), ಸ್ಥಿತಿ_2);
    f_Close_Punctuation_ಗಣ.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    ಆದರೆ (type == kKorean) {
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಗಣ_Characters(f_Hangul_ಶಬ್ದ_ಗಣ);
        }
    } ಇಲ್ಲದಿದ್ದರೆ { // Chinese and Japanese
        Unicode_ಗಣ cj_ಗಣ(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), ಸ್ಥಿತಿ_2);
        isCj = ನಿಜ;
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಗಣ_Characters(cj_ಗಣ);
#ಆದರೆ UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = ಹೊಸ MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ,
                                               f_Close_Punctuation_ಗಣ, ಸ್ಥಿತಿ_2);
            ಆದರೆ (fMlBreakEngine == ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
#ಇಲ್ಲದಿದ್ದರೆ
            initJapanesePhraseParameter(ಸ್ಥಿತಿ_2);
#ಆದರೆ_ಅಂತ್ಯ
        }
    }
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

CjkBreakEngine::~CjkBreakEngine(){
    ಅಳಿಸು fDictionary;
    ಅಳಿಸು fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_Katakana_ಉದ್ದಳತೆ = 8;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_Katakana_ಗುಂಪು_ಉದ್ದಳತೆ = 20;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t maxSnlp = 255;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t get_Katakana_ವೆಚ್ಚ(int32_t ಶಬ್ದ_ಉದ್ದಳತೆ){
    //TODO: fill array with actual values from dictionary!
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t katakana_ವೆಚ್ಚ[k_Max_Katakana_ಉದ್ದಳತೆ + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    ಹಿಂತಿರುಗಿ (ಶಬ್ದ_ಉದ್ದಳತೆ > k_Max_Katakana_ಉದ್ದಳತೆ) ? 8192 : katakana_ವೆಚ್ಚ[ಶಬ್ದ_ಉದ್ದಳತೆ];
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ isKatakana(UChar32 value) {
    ಹಿಂತಿರುಗಿ (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t utext_i32_flag(int32_t ಬಿಟ್_ಸೂಚ್ಯಂಕ) {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(1) << ಬಿಟ್_ಸೂಚ್ಯಂಕ;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_ಮೇಲೆ_Dictionary_Range( UText *ಒಳ_ಪಠ್ಯ,
        int32_t range_ಆರಂಭ,
        int32_t range_ಕೊನೆ,
        UVector32 &ಸಿಕ್ಕಿತು_Breaks,
        UBool isPhraseBreaking,
        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (range_ಆರಂಭ >= range_ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString ಒಳ_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    Local_ಸೂಚಕ<UVector32>     ಒಳಹರಿವು_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    ಆದರೆ ((ಒಳ_ಪಠ್ಯ->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         ಒಳ_ಪಠ್ಯ->chunk_Native_ಆರಂಭ <= range_ಆರಂಭ &&
         ಒಳ_ಪಠ್ಯ->chunk_Native_ಎಲ್ಲೆ >= range_ಕೊನೆ   &&
         ಒಳ_ಪಠ್ಯ->native_Indexing_ಎಲ್ಲೆ >= range_ಕೊನೆ - ಒಳ_ಪಠ್ಯ->chunk_Native_ಆರಂಭ) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        ಒಳ_String.ಗಣ_ಗಮ್ಯ(ಸುಳ್ಳು,
                       ಒಳ_ಪಠ್ಯ->chunkContents + range_ಆರಂಭ - ಒಳ_ಪಠ್ಯ->chunk_Native_ಆರಂಭ,
                       range_ಕೊನೆ - range_ಆರಂಭ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಒಳ_ಪಠ್ಯ, range_ಆರಂಭ);
        int32_t ಎಲ್ಲೆ = range_ಕೊನೆ;
        U_ASSERT(ಎಲ್ಲೆ <= utext_native_ಉದ್ದಳತೆ(ಒಳ_ಪಠ್ಯ));
        ಆದರೆ (ಎಲ್ಲೆ > utext_native_ಉದ್ದಳತೆ(ಒಳ_ಪಠ್ಯ)) {
            ಎಲ್ಲೆ = static_cast<int32_t>(utext_native_ಉದ್ದಳತೆ(ಒಳ_ಪಠ್ಯ));
        }
        ಒಳಹರಿವು_Map.adopt_Instead_And_Check_Error_ಸಂಕೇತ(ಹೊಸ UVector32(ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆಗ (utext_get_Native_ಸೂಚ್ಯಂಕ(ಒಳ_ಪಠ್ಯ) < ಎಲ್ಲೆ) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಒಳ_ಪಠ್ಯ));
            UChar32 c = utext_next32(ಒಳ_ಪಠ್ಯ);
            U_ASSERT(c != U_SENTINEL);
            ಒಳ_String.append(c);
            ಆಗ (ಒಳಹರಿವು_Map->size() < ಒಳ_String.length()) {
                ಒಳಹರಿವು_Map->ಕೂಡಿಸು_Element(nativePosition, ಸ್ಥಿತಿ_2);
            }
        }
        ಒಳಹರಿವು_Map->ಕೂಡಿಸು_Element(ಎಲ್ಲೆ, ಸ್ಥಿತಿ_2);
    }


    ಆದರೆ (!nfkcNorm2->isNormalized(ಒಳ_String, ಸ್ಥಿತಿ_2)) {
        UnicodeString normalized_ಒಳಹರಿವು;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        Local_ಸೂಚಕ<UVector32> normalizedMap(ಹೊಸ UVector32(ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        ಗಾಗಿ (int32_t srcI = 0; srcI < ಒಳ_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragment_ಆರಂಭ_I = srcI;
            UChar32 c = ಒಳ_String.char32At(srcI);
            ಗಾಗಿ (;;) {
                fragment.append(c);
                srcI = ಒಳ_String.moveIndex32(srcI, 1);
                ಆದರೆ (srcI == ಒಳ_String.length()) {
                    ನಿಲ್ಲಿಸು;
                }
                c = ಒಳ_String.char32At(srcI);
                ಆದರೆ (nfkcNorm2->hasBoundaryBefore(c)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, ಸ್ಥಿತಿ_2);
            normalized_ಒಳಹರಿವು.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragment_Original_ಆರಂಭ = ಒಳಹರಿವು_Map.isValid() ?
                    ಒಳಹರಿವು_Map->elementAti(fragment_ಆರಂಭ_I) : fragment_ಆರಂಭ_I+range_ಆರಂಭ;
            ಆಗ (normalizedMap->size() < normalized_ಒಳಹರಿವು.length()) {
                normalizedMap->ಕೂಡಿಸು_Element(fragment_Original_ಆರಂಭ, ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_ಒಳಹರಿವು.length());
        int32_t native_ಕೊನೆ = ಒಳಹರಿವು_Map.isValid() ?
                ಒಳಹರಿವು_Map->elementAti(ಒಳ_String.length()) : ಒಳ_String.length()+range_ಆರಂಭ;
        normalizedMap->ಕೂಡಿಸು_Element(native_ಕೊನೆ, ಸ್ಥಿತಿ_2);

        ಒಳಹರಿವು_Map = std::move(normalizedMap);
        ಒಳ_String = std::move(normalized_ಒಳಹರಿವು);
    }

    int32_t ಅಂಕೆ_ಸಂಕೇತ_Pts = ಒಳ_String.ಎಣಿಕೆ_Char32();
    ಆದರೆ (ಅಂಕೆ_ಸಂಕೇತ_Pts != ಒಳ_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = ಒಳಹರಿವು_Map.isValid();
        ಆದರೆ (!hadExistingMap) {
            ಒಳಹರಿವು_Map.adopt_Instead_And_Check_Error_ಸಂಕೇತ(ಹೊಸ UVector32(ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ 0;
            }
        }
        int32_t cpIdx = 0;
        ಗಾಗಿ (int32_t cuIdx = 0; ; cuIdx = ಒಳ_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            ಆದರೆ (hadExistingMap) {
                ಒಳಹರಿವು_Map->ಗಣ_Element_At(ಒಳಹರಿವು_Map->elementAti(cuIdx), cpIdx);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಒಳಹರಿವು_Map->ಕೂಡಿಸು_Element(cuIdx+range_ಆರಂಭ, ಸ್ಥಿತಿ_2);
            }
            cpIdx++;
            ಆದರೆ (cuIdx == ಒಳ_String.length()) {
               ನಿಲ್ಲಿಸು;
            }
        }
    }

#ಆದರೆ UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    ಆದರೆ (isPhraseBreaking && isCj) {
        ಹಿಂತಿರುಗಿ fMlBreakEngine->divide_ಮೇಲೆ_Range(ಒಳ_ಪಠ್ಯ, range_ಆರಂಭ, range_ಕೊನೆ, ಸಿಕ್ಕಿತು_Breaks, ಒಳ_String,
                                             ಒಳಹರಿವು_Map, ಸ್ಥಿತಿ_2);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 ಉತ್ತಮ_Snlp(ಅಂಕೆ_ಸಂಕೇತ_Pts + 1, ಸ್ಥಿತಿ_2);
    ಉತ್ತಮ_Snlp.ಕೂಡಿಸು_Element(0, ಸ್ಥಿತಿ_2);
    ಗಾಗಿ(int32_t i = 1; i <= ಅಂಕೆ_ಸಂಕೇತ_Pts; i++) {
        ಉತ್ತಮ_Snlp.ಕೂಡಿಸು_Element(kuint32max, ಸ್ಥಿತಿ_2);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 prev(ಅಂಕೆ_ಸಂಕೇತ_Pts + 1, ಸ್ಥಿತಿ_2);
    ಗಾಗಿ(int32_t i = 0; i <= ಅಂಕೆ_ಸಂಕೇತ_Pts; i++){
        prev.ಕೂಡಿಸು_Element(-1, ಸ್ಥಿತಿ_2);
    }

    ಸ್ಥಿರಾಂಕ int32_t max_ಶಬ್ದ_ಗಾತ್ರ = 20;
    UVector32 ಮೌಲ್ಯಗಳು(ಅಂಕೆ_ಸಂಕೇತ_Pts, ಸ್ಥಿತಿ_2);
    ಮೌಲ್ಯಗಳು.ಗಣ_ಗಾತ್ರ(ಅಂಕೆ_ಸಂಕೇತ_Pts);
    UVector32 lengths(ಅಂಕೆ_ಸಂಕೇತ_Pts, ಸ್ಥಿತಿ_2);
    lengths.ಗಣ_ಗಾತ್ರ(ಅಂಕೆ_ಸಂಕೇತ_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &ಒಳ_String, &ಸ್ಥಿತಿ_2);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    ತಾರ್ಕಿಕ is_prev_katakana = ಸುಳ್ಳು;
    ಗಾಗಿ (int32_t i = 0;  i < ಅಂಕೆ_ಸಂಕೇತ_Pts;  ++i, ix = ಒಳ_String.moveIndex32(ix, 1)) {
        ಆದರೆ (static_cast<uint32_t>(ಉತ್ತಮ_Snlp.elementAti(i)) == kuint32max) {
            ಮುಂದುವರಿ;
        }

        int32_t count;
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(&fu, ix);
        count = fDictionary->matches(&fu, max_ಶಬ್ದ_ಗಾತ್ರ, ಅಂಕೆ_ಸಂಕೇತ_Pts,
                             ಶೂನ್ಯಸೂಚಕ, lengths.get_ಬಫರ್(), ಮೌಲ್ಯಗಳು.get_ಬಫರ್(), ಶೂನ್ಯಸೂಚಕ);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        ಆದರೆ ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_ಶಬ್ದ_ಗಣ.ಒಳಗೊಂಡಿದೆ(ಒಳ_String.char32At(ix))) {
            ಮೌಲ್ಯಗಳು.ಗಣ_Element_At(maxSnlp, count);   // 255
            lengths.ಗಣ_Element_At(1, count++);
        }

        ಗಾಗಿ (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(ಉತ್ತಮ_Snlp.elementAti(i)) + static_cast<uint32_t>(ಮೌಲ್ಯಗಳು.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            ಆದರೆ (newSnlp < static_cast<uint32_t>(ಉತ್ತಮ_Snlp.elementAti(ln_j_i))) {
                ಉತ್ತಮ_Snlp.ಗಣ_Element_At(newSnlp, ln_j_i);
                prev.ಗಣ_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        ತಾರ್ಕಿಕ is_katakana = isKatakana(ಒಳ_String.char32At(ix));
        int32_t katakana_ಸತತ_ಉದ್ದಳತೆ = 1;
        ಆದರೆ (!is_prev_katakana && is_katakana) {
            int32_t j = ಒಳ_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            ಆಗ (j < ಒಳ_String.length() && katakana_ಸತತ_ಉದ್ದಳತೆ < k_Max_Katakana_ಗುಂಪು_ಉದ್ದಳತೆ &&
                    isKatakana(ಒಳ_String.char32At(j))) {
                j = ಒಳ_String.moveIndex32(j, 1);
                katakana_ಸತತ_ಉದ್ದಳತೆ++;
            }
            ಆದರೆ (katakana_ಸತತ_ಉದ್ದಳತೆ < k_Max_Katakana_ಗುಂಪು_ಉದ್ದಳತೆ) {
                uint32_t newSnlp = ಉತ್ತಮ_Snlp.elementAti(i) + get_Katakana_ವೆಚ್ಚ(katakana_ಸತತ_ಉದ್ದಳತೆ);
                ಆದರೆ (newSnlp < static_cast<uint32_t>(ಉತ್ತಮ_Snlp.elementAti(i + katakana_ಸತತ_ಉದ್ದಳತೆ))) {
                    ಉತ್ತಮ_Snlp.ಗಣ_Element_At(newSnlp, i+katakana_ಸತತ_ಉದ್ದಳತೆ);
                    prev.ಗಣ_Element_At(i, i+katakana_ಸತತ_ಉದ್ದಳತೆ);  // prev[j] = i;
                }
            }
        }
        is_prev_katakana = is_katakana;
    }
    utext_close(&fu);

    // Start pushing the optimal offset index into t_boundary (t for tentative).
    // prev[numCodePts] is guaranteed to be meaningful.
    // We'll first push in the reverse order, i.e.,
    // t_boundary[0] = numCodePts, and afterwards do a swap.
    UVector32 t_boundary(ಅಂಕೆ_ಸಂಕೇತ_Pts+1, ಸ್ಥಿತಿ_2);

    int32_t ಅಂಕೆ_Breaks = 0;
    // No segmentation found, set boundary to end of range
    ಆದರೆ (static_cast<uint32_t>(ಉತ್ತಮ_Snlp.elementAti(ಅಂಕೆ_ಸಂಕೇತ_Pts)) == kuint32max) {
        t_boundary.ಕೂಡಿಸು_Element(ಅಂಕೆ_ಸಂಕೇತ_Pts, ಸ್ಥಿತಿ_2);
        ಅಂಕೆ_Breaks++;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isPhraseBreaking) {
        t_boundary.ಕೂಡಿಸು_Element(ಅಂಕೆ_ಸಂಕೇತ_Pts, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಅಂಕೆ_Breaks++;
            int32_t prevIdx = ಅಂಕೆ_ಸಂಕೇತ_Pts;

            int32_t ಸಂಕೇತ_Unit_Idx = -1;
            int32_t prev_ಸಂಕೇತ_Unit_Idx = -1;
            int32_t length = -1;
            ಗಾಗಿ (int32_t i = prev.elementAti(ಅಂಕೆ_ಸಂಕೇತ_Pts); i > 0; i = prev.elementAti(i)) {
                ಸಂಕೇತ_Unit_Idx = ಒಳ_String.moveIndex32(0, i);
                prev_ಸಂಕೇತ_Unit_Idx = ಒಳ_String.moveIndex32(0, prevIdx);
                // Calculate the length by using the code unit.
                length = prev_ಸಂಕೇತ_Unit_Idx - ಸಂಕೇತ_Unit_Idx;
                prevIdx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                ಆದರೆ (!f_Skip_ಗಣ.ಒಳಗೊಂಡಿದೆ_ಕೀಲಿ(ಒಳ_String.ತಾತ್ಕಾಲಿಕ_Sub_String(ಸಂಕೇತ_Unit_Idx, length))
                    && (!isKatakana(ಒಳ_String.char32At(ಒಳ_String.moveIndex32(ಸಂಕೇತ_Unit_Idx, -1)))
                           || !isKatakana(ಒಳ_String.char32At(ಸಂಕೇತ_Unit_Idx)))) {
                    t_boundary.ಕೂಡಿಸು_Element(i, ಸ್ಥಿತಿ_2);
                    ಅಂಕೆ_Breaks++;
                }
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಾಗಿ (int32_t i = ಅಂಕೆ_ಸಂಕೇತ_Pts; i > 0; i = prev.elementAti(i)) {
            t_boundary.ಕೂಡಿಸು_Element(i, ಸ್ಥಿತಿ_2);
            ಅಂಕೆ_Breaks++;
        }
        U_ASSERT(prev.elementAti(t_boundary.elementAti(ಅಂಕೆ_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    ಆದರೆ (ಸಿಕ್ಕಿತು_Breaks.size() == 0 || ಸಿಕ್ಕಿತು_Breaks.peeki() < range_ಆರಂಭ) {
        t_boundary.ಕೂಡಿಸು_Element(0, ಸ್ಥಿತಿ_2);
        ಅಂಕೆ_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_ಅಂಕೆ_Breaks = 0;
    ಗಾಗಿ (int32_t i = ಅಂಕೆ_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > prevCPPos);
        int32_t utextPos =  ಒಳಹರಿವು_Map.isValid() ? ಒಳಹರಿವು_Map->elementAti(cpPos) : cpPos + range_ಆರಂಭ;
        U_ASSERT(utextPos >= prevUTextPos);
        ಆದರೆ (utextPos > prevUTextPos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(ಸಿಕ್ಕಿತು_Breaks.size() == 0 || ಸಿಕ್ಕಿತು_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            ಆದರೆ (utextPos != range_ಆರಂಭ
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_ಗಣ.ಒಳಗೊಂಡಿದೆ(utext_char32At(ಒಳ_ಪಠ್ಯ, utextPos - 1)))) {
                ಸಿಕ್ಕಿತು_Breaks.push(utextPos, ಸ್ಥಿತಿ_2);
                corrected_ಅಂಕೆ_Breaks++;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --ಅಂಕೆ_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (ಶೂನ್ಯ)prevCPPos; // suppress compiler warnings about unused variable

    UChar32 ಮುಂದಿನ_Char = utext_char32At(ಒಳ_ಪಠ್ಯ, range_ಕೊನೆ);
    ಆದರೆ (!ಸಿಕ್ಕಿತು_Breaks.is_ಖಾಲಿಗಳು() && ಸಿಕ್ಕಿತು_Breaks.peeki() == range_ಕೊನೆ) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        ಆದರೆ (isPhraseBreaking) {
            ಆದರೆ (!f_Digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ.ಒಳಗೊಂಡಿದೆ(ಮುಂದಿನ_Char)) {
                ಸಿಕ್ಕಿತು_Breaks.popi();
                corrected_ಅಂಕೆ_Breaks--;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸಿಕ್ಕಿತು_Breaks.popi();
            corrected_ಅಂಕೆ_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    ಹಿಂತಿರುಗಿ corrected_ಅಂಕೆ_Breaks;
}

ಶೂನ್ಯ CjkBreakEngine::initJapanesePhraseParameter(UError_ಸಂಕೇತ& error) {
    ತುಂಬು_Japanese_Extensions(error);
    ತುಂಬು_Hiragana(error);
}

ಶೂನ್ಯ CjkBreakEngine::ತುಂಬು_Japanese_Extensions(UError_ಸಂಕೇತ& error) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಟ್ಯಾಗ್ = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    ಆದರೆ (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(ಟ್ಯಾಗ್, error);
        ಆಗ (U_SUCCESS(error) && bundle.has_ಮುಂದಿನ()) {
            f_Skip_ಗಣ.puti(bundle.get_ಮುಂದಿನ_String(error), 1, error);
        }
    }
}

ಶೂನ್ಯ CjkBreakEngine::ತುಂಬು_Hiragana(UError_ಸಂಕೇತ& error) {
    Unicode_ಗಣ hiragana_ಶಬ್ದ_ಗಣ(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_ಶಬ್ದ_ಗಣ.compact();
    Unicode_ಗಣ_Iterator iterator(hiragana_ಶಬ್ದ_ಗಣ);
    ಆಗ (iterator.next()) {
        f_Skip_ಗಣ.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */

