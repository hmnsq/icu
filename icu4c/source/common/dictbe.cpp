// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 *******************************************************************************
 */

#ضمن <utility>

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "brkeng.h"
#ضمن "dictbe.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/chariter.h"
#ضمن "unicode/resbund.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/usetiter.h"
#ضمن "ubrkimpl.h"
#ضمن "utracimp.h"
#ضمن "uvectr32.h"
#ضمن "uvector.h"
#ضمن "uassert.h"
#ضمن "unicode/normlzr.h"
#ضمن "cmemory.h"
#ضمن "dictionarydata.h"

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

DictionaryBreakEngine::DictionaryBreakEngine() {
}

DictionaryBreakEngine::~DictionaryBreakEngine() {
}

UBool
DictionaryBreakEngine::handles(UChar32 c, ثابت حرف*) ثابت {
    أرجع f_سيٽ.شامل_آهي(c);
}

int32_t
DictionaryBreakEngine::ڳوليو_Breaks( UText *متن,
                                 int32_t شروعات_Pos,
                                 int32_t پڇاڙي_Pos,
                                 UVector32 &مليو_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t نتيجو = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_سيٽ_Native_اشاريو(متن, شروعات_Pos);
    int32_t شروعات = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
    int32_t current;
    int32_t range_شروعات;
    int32_t range_پڇاڙي;
    UChar32 c = utext_current32(متن);
    طالما ((current = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < پڇاڙي_Pos && f_سيٽ.شامل_آهي(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_شروعات = شروعات;
    range_پڇاڙي = current;
    نتيجو = divide_مٿي_Dictionary_Range(متن, range_شروعات, range_پڇاڙي, مليو_Breaks, isPhraseBreaking, حالت);
    utext_سيٽ_Native_اشاريو(متن, current);
    
    أرجع نتيجو;
}

فراغ
DictionaryBreakEngine::سيٽ_Characters( ثابت Unicode_سيٽ &سيٽ ) {
    f_سيٽ = سيٽ;
    // Compact for caching
    f_سيٽ.compact();
}

/*
 ******************************************************************
 * PossibleWord
 */

// Helper class for improving readability of the Thai/Lao/Khmer word break
// algorithm. The implementation is completely inline.

// List size, limited by the maximum number of words in the dictionary
// that form a nested sequence.
ساكن ثابت int32_t POSSIBLE_لفظ_LIST_MAX = 20;

صنف Possible_لفظ {
خاص:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   اڳياڙي;     // The longest match with a dictionary word
    int32_t   وٿي;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_لفظ_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_لفظ_LIST_MAX];   // Word Lengths, in code points.

عام:
    Possible_لفظ() : count(0), اڳياڙي(0), وٿي(-1), mark(0), current(0) {}
    ~Possible_لفظ() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *متن, DictionaryMatcher *dict, int32_t range_پڇاڙي );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *متن );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     پٺ_مٿي( UText *متن );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_اڳياڙي() { أرجع اڳياڙي; }
  
    // Mark the current candidate as the one we like
    فراغ      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { أرجع cpLengths[mark]; }
};


int32_t Possible_لفظ::candidates( UText *متن, DictionaryMatcher *dict, int32_t range_پڇاڙي ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t شروعات = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
    إذا (شروعات != وٿي) {
        وٿي = شروعات;
        count = dict->matches(متن, range_پڇاڙي-شروعات, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, خالي_اشارو, &اڳياڙي);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        إذا (count <= 0) {
            utext_سيٽ_Native_اشاريو(متن, شروعات);
        }
    }
    إذا (count > 0) {
        utext_سيٽ_Native_اشاريو(متن, شروعات+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    أرجع count;
}

int32_t
Possible_لفظ::acceptMarked( UText *متن ) {
    utext_سيٽ_Native_اشاريو(متن, وٿي + cuLengths[mark]);
    أرجع cuLengths[mark];
}


UBool
Possible_لفظ::پٺ_مٿي( UText *متن ) {
    إذا (current > 0) {
        utext_سيٽ_Native_اشاريو(متن, وٿي + cuLengths[--current]);
        أرجع سچ;
    }
    أرجع خطأ;
}

/*
 ******************************************************************
 * ThaiBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t THAI_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t THAI_پاڙ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t THAI_اڳياڙي_COMBINE_حد = 3;

// Elision character
ساكن ثابت int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
ساكن ثابت int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
ساكن ثابت int32_t THAI_MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t THAI_MIN_لفظ_وسعت = THAI_MIN_لفظ * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_ڪوڊ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_داخلا(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_سيٽ thai_لفظ_سيٽ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سيٽ_Characters(thai_لفظ_سيٽ);
    }
    f_Mark_سيٽ.apply_نمونو(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سيٽ.جوڙيو(0x0020);
    f_پڇاڙي_لفظ_سيٽ = thai_لفظ_سيٽ;
    f_پڇاڙي_لفظ_سيٽ.remove(0x0E31);             // MAI HAN-AKAT
    f_پڇاڙي_لفظ_سيٽ.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_لفظ_سيٽ.جوڙيو(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_لفظ_سيٽ.جوڙيو(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_سيٽ.جوڙيو(THAI_PAIYANNOI);
    f_Suffix_سيٽ.جوڙيو(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_سيٽ.compact();
    f_پڇاڙي_لفظ_سيٽ.compact();
    f_Begin_لفظ_سيٽ.compact();
    f_Suffix_سيٽ.compact();
    UTRACE_EXIT_حالت(حالت);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    ميٽيو fDictionary;
}

int32_t
ThaiBreakEngine::divide_مٿي_Dictionary_Range( UText *متن,
                                                int32_t range_شروعات,
                                                int32_t range_پڇاڙي,
                                                UVector32 &مليو_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    utext_سيٽ_Native_اشاريو(متن, range_شروعات);
    utext_moveIndex32(متن, THAI_MIN_لفظ_وسعت);
    إذا (utext_get_Native_اشاريو(متن) >= range_پڇاڙي) {
        أرجع 0;       // Not enough characters for two words
    }
    utext_سيٽ_Native_اشاريو(متن, range_شروعات);


    uint32_t لفظ_مليو = 0;
    int32_t cp_لفظ_ڊيگهائي = 0;    // Word Length in Code Points.
    int32_t cu_لفظ_ڊيگهائي = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    Possible_لفظ لفظ[THAI_LOOKAHEAD];
    
    utext_سيٽ_Native_اشاريو(متن, range_شروعات);
    
    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < range_پڇاڙي) {
        cp_لفظ_ڊيگهائي = 0;
        cu_لفظ_ڊيگهائي = 0;

        // Look for candidate words at the current position
        int32_t candidates = لفظ[لفظ_مليو%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_لفظ_ڊيگهائي = لفظ[لفظ_مليو % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_ڊيگهائي = لفظ[لفظ_مليو % THAI_LOOKAHEAD].markedCPLength();
            لفظ_مليو += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) >= range_پڇاڙي) {
                اذهب مليو_بهترين;
            }
            افعل {
                إذا (لفظ[(لفظ_مليو + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    لفظ[لفظ_مليو%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) >= range_پڇاڙي) {
                        اذهب مليو_بهترين;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (لفظ[(لفظ_مليو + 2) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي)) {
                            لفظ[لفظ_مليو % THAI_LOOKAHEAD].markCurrent();
                            اذهب مليو_بهترين;
                        }
                    }
                    طالما (لفظ[(لفظ_مليو + 1) % THAI_LOOKAHEAD].پٺ_مٿي(متن));
                }
            }
            طالما (لفظ[لفظ_مليو % THAI_LOOKAHEAD].پٺ_مٿي(متن));
مليو_بهترين:
            // Set UText position to after the accepted word.
            cu_لفظ_ڊيگهائي = لفظ[لفظ_مليو % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_ڊيگهائي = لفظ[لفظ_مليو % THAI_LOOKAHEAD].markedCPLength();
            لفظ_مليو += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) < range_پڇاڙي && cp_لفظ_ڊيگهائي < THAI_پاڙ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (لفظ[لفظ_مليو % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) <= 0
                  && (cu_لفظ_ڊيگهائي == 0
                      || لفظ[لفظ_مليو%THAI_LOOKAHEAD].longest_اڳياڙي() < THAI_اڳياڙي_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بچيل = range_پڇاڙي - (current+cu_لفظ_ڊيگهائي);
                UChar32 pc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_اشاريو = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
                    pc = utext_next32(متن);
                    int32_t pc_سائيز = static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - pc_اشاريو;
                    chars += pc_سائيز;
                    بچيل -= pc_سائيز;
                    إذا (بچيل <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_پڇاڙي_لفظ_سيٽ.شامل_آهي(pc) && f_Begin_لفظ_سيٽ.شامل_آهي(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t انگ_candidates = لفظ[(لفظ_مليو + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي);
                        utext_سيٽ_Native_اشاريو(متن, current + cu_لفظ_ڊيگهائي + chars);
                        إذا (انگ_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_لفظ_ڊيگهائي <= 0) {
                    لفظ_مليو += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_ڊيگهائي += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سيٽ_Native_اشاريو(متن, current+cu_لفظ_ڊيگهائي);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < range_پڇاڙي && f_Mark_سيٽ.شامل_آهي(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_ڊيگهائي += static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) < range_پڇاڙي && cu_لفظ_ڊيگهائي > 0) {
            إذا (لفظ[لفظ_مليو%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) <= 0
                && f_Suffix_سيٽ.شامل_آهي(uc = utext_current32(متن))) {
                إذا (uc == THAI_PAIYANNOI) {
                    إذا (!f_Suffix_سيٽ.شامل_آهي(utext_previous32(متن))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(متن);
                        int32_t paiyannoi_اشاريو = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
                        utext_next32(متن);
                        cu_لفظ_ڊيگهائي += static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - paiyannoi_اشاريو; // Add PAIYANNOI to word
                        uc = utext_current32(متن);     // Fetch next character
                    }
                    وإلا {
                        // Restore prior position
                        utext_next32(متن);
                    }
                }
                إذا (uc == THAI_MAIYAMOK) {
                    إذا (utext_previous32(متن) != THAI_MAIYAMOK) {
                        // Skip over previous end and MAIYAMOK
                        utext_next32(متن);
                        int32_t maiyamok_اشاريو = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
                        utext_next32(متن);
                        cu_لفظ_ڊيگهائي += static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - maiyamok_اشاريو; // Add MAIYAMOK to word
                    }
                    وإلا {
                        // Restore prior position
                        utext_next32(متن);
                    }
                }
            }
            وإلا {
                utext_سيٽ_Native_اشاريو(متن, current+cu_لفظ_ڊيگهائي);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_لفظ_ڊيگهائي > 0) {
            مليو_Breaks.push((current+cu_لفظ_ڊيگهائي), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (مليو_Breaks.peeki() >= range_پڇاڙي) {
        (فراغ) مليو_Breaks.popi();
        لفظ_مليو -= 1;
    }

    أرجع لفظ_مليو;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t LAO_پاڙ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t LAO_اڳياڙي_COMBINE_حد = 3;

// Minimum word size
ساكن ثابت int32_t LAO_MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t LAO_MIN_لفظ_وسعت = LAO_MIN_لفظ * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_ڪوڊ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_داخلا(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_سيٽ lao_لفظ_سيٽ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سيٽ_Characters(lao_لفظ_سيٽ);
    }
    f_Mark_سيٽ.apply_نمونو(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سيٽ.جوڙيو(0x0020);
    f_پڇاڙي_لفظ_سيٽ = lao_لفظ_سيٽ;
    f_پڇاڙي_لفظ_سيٽ.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_لفظ_سيٽ.جوڙيو(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_لفظ_سيٽ.جوڙيو(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_لفظ_سيٽ.جوڙيو(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_سيٽ.compact();
    f_پڇاڙي_لفظ_سيٽ.compact();
    f_Begin_لفظ_سيٽ.compact();
    UTRACE_EXIT_حالت(حالت);
}

LaoBreakEngine::~LaoBreakEngine() {
    ميٽيو fDictionary;
}

int32_t
LaoBreakEngine::divide_مٿي_Dictionary_Range( UText *متن,
                                                int32_t range_شروعات,
                                                int32_t range_پڇاڙي,
                                                UVector32 &مليو_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_پڇاڙي - range_شروعات) < LAO_MIN_لفظ_وسعت) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t لفظ_مليو = 0;
    int32_t cp_لفظ_ڊيگهائي = 0;
    int32_t cu_لفظ_ڊيگهائي = 0;
    int32_t current;
    Possible_لفظ لفظ[LAO_LOOKAHEAD];

    utext_سيٽ_Native_اشاريو(متن, range_شروعات);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < range_پڇاڙي) {
        cu_لفظ_ڊيگهائي = 0;
        cp_لفظ_ڊيگهائي = 0;

        // Look for candidate words at the current position
        int32_t candidates = لفظ[لفظ_مليو%LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_لفظ_ڊيگهائي = لفظ[لفظ_مليو % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_ڊيگهائي = لفظ[لفظ_مليو % LAO_LOOKAHEAD].markedCPLength();
            لفظ_مليو += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (utext_get_Native_اشاريو(متن) >= range_پڇاڙي) {
                اذهب مليو_بهترين;
            }
            افعل {
                إذا (لفظ[(لفظ_مليو + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    لفظ[لفظ_مليو%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) >= range_پڇاڙي) {
                        اذهب مليو_بهترين;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (لفظ[(لفظ_مليو + 2) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي)) {
                            لفظ[لفظ_مليو % LAO_LOOKAHEAD].markCurrent();
                            اذهب مليو_بهترين;
                        }
                    }
                    طالما (لفظ[(لفظ_مليو + 1) % LAO_LOOKAHEAD].پٺ_مٿي(متن));
                }
            }
            طالما (لفظ[لفظ_مليو % LAO_LOOKAHEAD].پٺ_مٿي(متن));
مليو_بهترين:
            cu_لفظ_ڊيگهائي = لفظ[لفظ_مليو % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_ڊيگهائي = لفظ[لفظ_مليو % LAO_LOOKAHEAD].markedCPLength();
            لفظ_مليو += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) < range_پڇاڙي && cp_لفظ_ڊيگهائي < LAO_پاڙ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (لفظ[لفظ_مليو % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) <= 0
                  && (cu_لفظ_ڊيگهائي == 0
                      || لفظ[لفظ_مليو%LAO_LOOKAHEAD].longest_اڳياڙي() < LAO_اڳياڙي_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بچيل = range_پڇاڙي - (current + cu_لفظ_ڊيگهائي);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_اشاريو = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
                    pc = utext_next32(متن);
                    int32_t pc_سائيز = static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - pc_اشاريو;
                    chars += pc_سائيز;
                    بچيل -= pc_سائيز;
                    إذا (بچيل <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_پڇاڙي_لفظ_سيٽ.شامل_آهي(pc) && f_Begin_لفظ_سيٽ.شامل_آهي(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t انگ_candidates = لفظ[(لفظ_مليو + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي);
                        utext_سيٽ_Native_اشاريو(متن, current + cu_لفظ_ڊيگهائي + chars);
                        إذا (انگ_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_لفظ_ڊيگهائي <= 0) {
                    لفظ_مليو += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_ڊيگهائي += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سيٽ_Native_اشاريو(متن, current + cu_لفظ_ڊيگهائي);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < range_پڇاڙي && f_Mark_سيٽ.شامل_آهي(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_ڊيگهائي += static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_لفظ_ڊيگهائي > 0) {
            مليو_Breaks.push((current+cu_لفظ_ڊيگهائي), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (مليو_Breaks.peeki() >= range_پڇاڙي) {
        (فراغ) مليو_Breaks.popi();
        لفظ_مليو -= 1;
    }

    أرجع لفظ_مليو;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t BURMESE_پاڙ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t BURMESE_اڳياڙي_COMBINE_حد = 3;

// Minimum word size
ساكن ثابت int32_t BURMESE_MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t BURMESE_MIN_لفظ_وسعت = BURMESE_MIN_لفظ * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_ڪوڊ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_داخلا(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_لفظ_سيٽ.جوڙيو(0x1000, 0x102A);      // basic consonants and independent vowels
    f_پڇاڙي_لفظ_سيٽ.apply_نمونو(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), حالت);
    f_Mark_سيٽ.apply_نمونو(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سيٽ.جوڙيو(0x0020);
    إذا (U_SUCCESS(حالت)) {
        سيٽ_Characters(f_پڇاڙي_لفظ_سيٽ);
    }

    // Compact for caching.
    f_Mark_سيٽ.compact();
    f_پڇاڙي_لفظ_سيٽ.compact();
    f_Begin_لفظ_سيٽ.compact();
    UTRACE_EXIT_حالت(حالت);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    ميٽيو fDictionary;
}

int32_t
BurmeseBreakEngine::divide_مٿي_Dictionary_Range( UText *متن,
                                                int32_t range_شروعات,
                                                int32_t range_پڇاڙي,
                                                UVector32 &مليو_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ڪوڊ& حالت ) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_پڇاڙي - range_شروعات) < BURMESE_MIN_لفظ_وسعت) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t لفظ_مليو = 0;
    int32_t cp_لفظ_ڊيگهائي = 0;
    int32_t cu_لفظ_ڊيگهائي = 0;
    int32_t current;
    Possible_لفظ لفظ[BURMESE_LOOKAHEAD];

    utext_سيٽ_Native_اشاريو(متن, range_شروعات);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < range_پڇاڙي) {
        cu_لفظ_ڊيگهائي = 0;
        cp_لفظ_ڊيگهائي = 0;

        // Look for candidate words at the current position
        int32_t candidates = لفظ[لفظ_مليو%BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_لفظ_ڊيگهائي = لفظ[لفظ_مليو % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_ڊيگهائي = لفظ[لفظ_مليو % BURMESE_LOOKAHEAD].markedCPLength();
            لفظ_مليو += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (utext_get_Native_اشاريو(متن) >= range_پڇاڙي) {
                اذهب مليو_بهترين;
            }
            افعل {
                إذا (لفظ[(لفظ_مليو + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    لفظ[لفظ_مليو%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) >= range_پڇاڙي) {
                        اذهب مليو_بهترين;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (لفظ[(لفظ_مليو + 2) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي)) {
                            لفظ[لفظ_مليو % BURMESE_LOOKAHEAD].markCurrent();
                            اذهب مليو_بهترين;
                        }
                    }
                    طالما (لفظ[(لفظ_مليو + 1) % BURMESE_LOOKAHEAD].پٺ_مٿي(متن));
                }
            }
            طالما (لفظ[لفظ_مليو % BURMESE_LOOKAHEAD].پٺ_مٿي(متن));
مليو_بهترين:
            cu_لفظ_ڊيگهائي = لفظ[لفظ_مليو % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_ڊيگهائي = لفظ[لفظ_مليو % BURMESE_LOOKAHEAD].markedCPLength();
            لفظ_مليو += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) < range_پڇاڙي && cp_لفظ_ڊيگهائي < BURMESE_پاڙ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (لفظ[لفظ_مليو % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) <= 0
                  && (cu_لفظ_ڊيگهائي == 0
                      || لفظ[لفظ_مليو%BURMESE_LOOKAHEAD].longest_اڳياڙي() < BURMESE_اڳياڙي_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بچيل = range_پڇاڙي - (current + cu_لفظ_ڊيگهائي);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_اشاريو = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
                    pc = utext_next32(متن);
                    int32_t pc_سائيز = static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - pc_اشاريو;
                    chars += pc_سائيز;
                    بچيل -= pc_سائيز;
                    إذا (بچيل <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_پڇاڙي_لفظ_سيٽ.شامل_آهي(pc) && f_Begin_لفظ_سيٽ.شامل_آهي(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t انگ_candidates = لفظ[(لفظ_مليو + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي);
                        utext_سيٽ_Native_اشاريو(متن, current + cu_لفظ_ڊيگهائي + chars);
                        إذا (انگ_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_لفظ_ڊيگهائي <= 0) {
                    لفظ_مليو += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_ڊيگهائي += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سيٽ_Native_اشاريو(متن, current + cu_لفظ_ڊيگهائي);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < range_پڇاڙي && f_Mark_سيٽ.شامل_آهي(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_ڊيگهائي += static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_لفظ_ڊيگهائي > 0) {
            مليو_Breaks.push((current+cu_لفظ_ڊيگهائي), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (مليو_Breaks.peeki() >= range_پڇاڙي) {
        (فراغ) مليو_Breaks.popi();
        لفظ_مليو -= 1;
    }

    أرجع لفظ_مليو;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t KHMER_پاڙ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t KHMER_اڳياڙي_COMBINE_حد = 3;

// Minimum word size
ساكن ثابت int32_t KHMER_MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t KHMER_MIN_لفظ_وسعت = KHMER_MIN_لفظ * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_ڪوڊ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_داخلا(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_سيٽ khmer_لفظ_سيٽ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سيٽ_Characters(khmer_لفظ_سيٽ);
    }
    f_Mark_سيٽ.apply_نمونو(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سيٽ.جوڙيو(0x0020);
    f_پڇاڙي_لفظ_سيٽ = khmer_لفظ_سيٽ;
    f_Begin_لفظ_سيٽ.جوڙيو(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_پڇاڙي_لفظ_سيٽ.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_سيٽ.compact();
    f_پڇاڙي_لفظ_سيٽ.compact();
    f_Begin_لفظ_سيٽ.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_حالت(حالت);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    ميٽيو fDictionary;
}

int32_t
KhmerBreakEngine::divide_مٿي_Dictionary_Range( UText *متن,
                                                int32_t range_شروعات,
                                                int32_t range_پڇاڙي,
                                                UVector32 &مليو_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ڪوڊ& حالت ) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_پڇاڙي - range_شروعات) < KHMER_MIN_لفظ_وسعت) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t لفظ_مليو = 0;
    int32_t cp_لفظ_ڊيگهائي = 0;
    int32_t cu_لفظ_ڊيگهائي = 0;
    int32_t current;
    Possible_لفظ لفظ[KHMER_LOOKAHEAD];

    utext_سيٽ_Native_اشاريو(متن, range_شروعات);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < range_پڇاڙي) {
        cu_لفظ_ڊيگهائي = 0;
        cp_لفظ_ڊيگهائي = 0;

        // Look for candidate words at the current position
        int32_t candidates = لفظ[لفظ_مليو%KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي);

        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_لفظ_ڊيگهائي = لفظ[لفظ_مليو % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_ڊيگهائي = لفظ[لفظ_مليو % KHMER_LOOKAHEAD].markedCPLength();
            لفظ_مليو += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) >= range_پڇاڙي) {
                اذهب مليو_بهترين;
            }
            افعل {
                إذا (لفظ[(لفظ_مليو + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    لفظ[لفظ_مليو % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) >= range_پڇاڙي) {
                        اذهب مليو_بهترين;
                    }

                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (لفظ[(لفظ_مليو + 2) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي)) {
                            لفظ[لفظ_مليو % KHMER_LOOKAHEAD].markCurrent();
                            اذهب مليو_بهترين;
                        }
                    }
                    طالما (لفظ[(لفظ_مليو + 1) % KHMER_LOOKAHEAD].پٺ_مٿي(متن));
                }
            }
            طالما (لفظ[لفظ_مليو % KHMER_LOOKAHEAD].پٺ_مٿي(متن));
مليو_بهترين:
            cu_لفظ_ڊيگهائي = لفظ[لفظ_مليو % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_ڊيگهائي = لفظ[لفظ_مليو % KHMER_LOOKAHEAD].markedCPLength();
            لفظ_مليو += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_اشاريو(متن)) < range_پڇاڙي && cp_لفظ_ڊيگهائي < KHMER_پاڙ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (لفظ[لفظ_مليو % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي) <= 0
                  && (cu_لفظ_ڊيگهائي == 0
                      || لفظ[لفظ_مليو % KHMER_LOOKAHEAD].longest_اڳياڙي() < KHMER_اڳياڙي_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بچيل = range_پڇاڙي - (current+cu_لفظ_ڊيگهائي);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_اشاريو = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
                    pc = utext_next32(متن);
                    int32_t pc_سائيز = static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - pc_اشاريو;
                    chars += pc_سائيز;
                    بچيل -= pc_سائيز;
                    إذا (بچيل <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_پڇاڙي_لفظ_سيٽ.شامل_آهي(pc) && f_Begin_لفظ_سيٽ.شامل_آهي(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t انگ_candidates = لفظ[(لفظ_مليو + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پڇاڙي);
                        utext_سيٽ_Native_اشاريو(متن, current+cu_لفظ_ڊيگهائي+chars);
                        إذا (انگ_candidates > 0) {
                            اكسر;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                إذا (cu_لفظ_ڊيگهائي <= 0) {
                    لفظ_مليو += 1;
                }

                // Update the length with the passed-over characters
                cu_لفظ_ڊيگهائي += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سيٽ_Native_اشاريو(متن, current+cu_لفظ_ڊيگهائي);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < range_پڇاڙي && f_Mark_سيٽ.شامل_آهي(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_ڊيگهائي += static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - currPos;
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
        إذا (cu_لفظ_ڊيگهائي > 0) {
            مليو_Breaks.push((current+cu_لفظ_ڊيگهائي), حالت);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (مليو_Breaks.peeki() >= range_پڇاڙي) {
        (فراغ) مليو_Breaks.popi();
        لفظ_مليو -= 1;
    }

    أرجع لفظ_مليو;
}

#إذا !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
ساكن ثابت uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_ڪوڊ &حالت)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(خطأ) {
    UTRACE_داخلا(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = خالي_اشارو;
    nfkcNorm2 = Normalizer2::getNFKCInstance(حالت);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_لفظ_سيٽ.apply_نمونو(UnicodeString(u"[\\uac00-\\ud7a3]"), حالت);
    f_Hangul_لفظ_سيٽ.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_سيٽ.apply_نمونو(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), حالت);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_سيٽ.compact();
    f_Close_Punctuation_سيٽ.apply_نمونو(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), حالت);
    f_Close_Punctuation_سيٽ.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    إذا (type == kKorean) {
        إذا (U_SUCCESS(حالت)) {
            سيٽ_Characters(f_Hangul_لفظ_سيٽ);
        }
    } وإلا { // Chinese and Japanese
        Unicode_سيٽ cj_سيٽ(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), حالت);
        isCj = سچ;
        إذا (U_SUCCESS(حالت)) {
            سيٽ_Characters(cj_سيٽ);
#إذا UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = جديد MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_سيٽ,
                                               f_Close_Punctuation_سيٽ, حالت);
            إذا (fMlBreakEngine == خالي_اشارو) {
                حالت = U_يادگيري_ورهاست_ERROR;
            }
#وإلا
            initJapanesePhraseParameter(حالت);
#جيڪڏهن_پورو
        }
    }
    UTRACE_EXIT_حالت(حالت);
}

CjkBreakEngine::~CjkBreakEngine(){
    ميٽيو fDictionary;
    ميٽيو fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
ساكن ثابت int32_t k_Max_Katakana_ڊيگهائي = 8;
ساكن ثابت int32_t k_Max_Katakana_گروهه_ڊيگهائي = 20;
ساكن ثابت uint32_t maxSnlp = 255;

ساكن مضمن uint32_t get_Katakana_لاڳت(int32_t لفظ_ڊيگهائي){
    //TODO: fill array with actual values from dictionary!
    ساكن ثابت uint32_t katakana_لاڳت[k_Max_Katakana_ڊيگهائي + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    أرجع (لفظ_ڊيگهائي > k_Max_Katakana_ڊيگهائي) ? 8192 : katakana_لاڳت[لفظ_ڊيگهائي];
}

ساكن مضمن منطقي isKatakana(UChar32 value) {
    أرجع (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

ساكن مضمن int32_t utext_i32_flag(int32_t ٻٽو_انگ_اشاريو) {
    أرجع static_cast<int32_t>(1) << ٻٽو_انگ_اشاريو;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_مٿي_Dictionary_Range( UText *اندريون_متن,
        int32_t range_شروعات,
        int32_t range_پڇاڙي,
        UVector32 &مليو_Breaks,
        UBool isPhraseBreaking,
        UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا (range_شروعات >= range_پڇاڙي) {
        أرجع 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString اندريون_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    Local_اشارو<UVector32>     داخلا_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    إذا ((اندريون_متن->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         اندريون_متن->chunk_Native_شروعات <= range_شروعات &&
         اندريون_متن->chunk_Native_حد_بندي >= range_پڇاڙي   &&
         اندريون_متن->native_Indexing_حد_بندي >= range_پڇاڙي - اندريون_متن->chunk_Native_شروعات) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        اندريون_String.سيٽ_منزل(خطأ,
                       اندريون_متن->chunkContents + range_شروعات - اندريون_متن->chunk_Native_شروعات,
                       range_پڇاڙي - range_شروعات);
    } وإلا {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_سيٽ_Native_اشاريو(اندريون_متن, range_شروعات);
        int32_t حد_بندي = range_پڇاڙي;
        U_ASSERT(حد_بندي <= utext_native_ڊيگهائي(اندريون_متن));
        إذا (حد_بندي > utext_native_ڊيگهائي(اندريون_متن)) {
            حد_بندي = static_cast<int32_t>(utext_native_ڊيگهائي(اندريون_متن));
        }
        داخلا_Map.adopt_Instead_And_Check_Error_ڪوڊ(جديد UVector32(حالت), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع 0;
        }
        طالما (utext_get_Native_اشاريو(اندريون_متن) < حد_بندي) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_اشاريو(اندريون_متن));
            UChar32 c = utext_next32(اندريون_متن);
            U_ASSERT(c != U_SENTINEL);
            اندريون_String.append(c);
            طالما (داخلا_Map->size() < اندريون_String.length()) {
                داخلا_Map->جوڙيو_Element(nativePosition, حالت);
            }
        }
        داخلا_Map->جوڙيو_Element(حد_بندي, حالت);
    }


    إذا (!nfkcNorm2->isNormalized(اندريون_String, حالت)) {
        UnicodeString normalized_داخلا;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        Local_اشارو<UVector32> normalizedMap(جديد UVector32(حالت), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        لكل (int32_t srcI = 0; srcI < اندريون_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragment_شروعات_I = srcI;
            UChar32 c = اندريون_String.char32At(srcI);
            لكل (;;) {
                fragment.append(c);
                srcI = اندريون_String.moveIndex32(srcI, 1);
                إذا (srcI == اندريون_String.length()) {
                    اكسر;
                }
                c = اندريون_String.char32At(srcI);
                إذا (nfkcNorm2->hasBoundaryBefore(c)) {
                    اكسر;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, حالت);
            normalized_داخلا.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragment_Original_شروعات = داخلا_Map.isValid() ?
                    داخلا_Map->elementAti(fragment_شروعات_I) : fragment_شروعات_I+range_شروعات;
            طالما (normalizedMap->size() < normalized_داخلا.length()) {
                normalizedMap->جوڙيو_Element(fragment_Original_شروعات, حالت);
                إذا (U_FAILURE(حالت)) {
                    اكسر;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_داخلا.length());
        int32_t native_پڇاڙي = داخلا_Map.isValid() ?
                داخلا_Map->elementAti(اندريون_String.length()) : اندريون_String.length()+range_شروعات;
        normalizedMap->جوڙيو_Element(native_پڇاڙي, حالت);

        داخلا_Map = std::move(normalizedMap);
        اندريون_String = std::move(normalized_داخلا);
    }

    int32_t انگ_ڪوڊ_Pts = اندريون_String.ڳڻپ_Char32();
    إذا (انگ_ڪوڊ_Pts != اندريون_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = داخلا_Map.isValid();
        إذا (!hadExistingMap) {
            داخلا_Map.adopt_Instead_And_Check_Error_ڪوڊ(جديد UVector32(حالت), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع 0;
            }
        }
        int32_t cpIdx = 0;
        لكل (int32_t cuIdx = 0; ; cuIdx = اندريون_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            إذا (hadExistingMap) {
                داخلا_Map->سيٽ_Element_At(داخلا_Map->elementAti(cuIdx), cpIdx);
            } وإلا {
                داخلا_Map->جوڙيو_Element(cuIdx+range_شروعات, حالت);
            }
            cpIdx++;
            إذا (cuIdx == اندريون_String.length()) {
               اكسر;
            }
        }
    }

#إذا UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    إذا (isPhraseBreaking && isCj) {
        أرجع fMlBreakEngine->divide_مٿي_Range(اندريون_متن, range_شروعات, range_پڇاڙي, مليو_Breaks, اندريون_String,
                                             داخلا_Map, حالت);
    }
#جيڪڏهن_پورو

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 بهترين_Snlp(انگ_ڪوڊ_Pts + 1, حالت);
    بهترين_Snlp.جوڙيو_Element(0, حالت);
    لكل(int32_t i = 1; i <= انگ_ڪوڊ_Pts; i++) {
        بهترين_Snlp.جوڙيو_Element(kuint32max, حالت);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 prev(انگ_ڪوڊ_Pts + 1, حالت);
    لكل(int32_t i = 0; i <= انگ_ڪوڊ_Pts; i++){
        prev.جوڙيو_Element(-1, حالت);
    }

    ثابت int32_t max_لفظ_سائيز = 20;
    UVector32 قدرون(انگ_ڪوڊ_Pts, حالت);
    قدرون.سيٽ_سائيز(انگ_ڪوڊ_Pts);
    UVector32 lengths(انگ_ڪوڊ_Pts, حالت);
    lengths.سيٽ_سائيز(انگ_ڪوڊ_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &اندريون_String, &حالت);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    منطقي is_prev_katakana = خطأ;
    لكل (int32_t i = 0;  i < انگ_ڪوڊ_Pts;  ++i, ix = اندريون_String.moveIndex32(ix, 1)) {
        إذا (static_cast<uint32_t>(بهترين_Snlp.elementAti(i)) == kuint32max) {
            استمر;
        }

        int32_t count;
        utext_سيٽ_Native_اشاريو(&fu, ix);
        count = fDictionary->matches(&fu, max_لفظ_سائيز, انگ_ڪوڊ_Pts,
                             خالي_اشارو, lengths.get_بفر(), قدرون.get_بفر(), خالي_اشارو);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        إذا ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_لفظ_سيٽ.شامل_آهي(اندريون_String.char32At(ix))) {
            قدرون.سيٽ_Element_At(maxSnlp, count);   // 255
            lengths.سيٽ_Element_At(1, count++);
        }

        لكل (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(بهترين_Snlp.elementAti(i)) + static_cast<uint32_t>(قدرون.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            إذا (newSnlp < static_cast<uint32_t>(بهترين_Snlp.elementAti(ln_j_i))) {
                بهترين_Snlp.سيٽ_Element_At(newSnlp, ln_j_i);
                prev.سيٽ_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        منطقي is_katakana = isKatakana(اندريون_String.char32At(ix));
        int32_t katakana_لاڳيتو_ڊيگهائي = 1;
        إذا (!is_prev_katakana && is_katakana) {
            int32_t j = اندريون_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            طالما (j < اندريون_String.length() && katakana_لاڳيتو_ڊيگهائي < k_Max_Katakana_گروهه_ڊيگهائي &&
                    isKatakana(اندريون_String.char32At(j))) {
                j = اندريون_String.moveIndex32(j, 1);
                katakana_لاڳيتو_ڊيگهائي++;
            }
            إذا (katakana_لاڳيتو_ڊيگهائي < k_Max_Katakana_گروهه_ڊيگهائي) {
                uint32_t newSnlp = بهترين_Snlp.elementAti(i) + get_Katakana_لاڳت(katakana_لاڳيتو_ڊيگهائي);
                إذا (newSnlp < static_cast<uint32_t>(بهترين_Snlp.elementAti(i + katakana_لاڳيتو_ڊيگهائي))) {
                    بهترين_Snlp.سيٽ_Element_At(newSnlp, i+katakana_لاڳيتو_ڊيگهائي);
                    prev.سيٽ_Element_At(i, i+katakana_لاڳيتو_ڊيگهائي);  // prev[j] = i;
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
    UVector32 t_boundary(انگ_ڪوڊ_Pts+1, حالت);

    int32_t انگ_Breaks = 0;
    // No segmentation found, set boundary to end of range
    إذا (static_cast<uint32_t>(بهترين_Snlp.elementAti(انگ_ڪوڊ_Pts)) == kuint32max) {
        t_boundary.جوڙيو_Element(انگ_ڪوڊ_Pts, حالت);
        انگ_Breaks++;
    } وإلا إذا (isPhraseBreaking) {
        t_boundary.جوڙيو_Element(انگ_ڪوڊ_Pts, حالت);
        إذا(U_SUCCESS(حالت)) {
            انگ_Breaks++;
            int32_t prevIdx = انگ_ڪوڊ_Pts;

            int32_t ڪوڊ_Unit_Idx = -1;
            int32_t prev_ڪوڊ_Unit_Idx = -1;
            int32_t length = -1;
            لكل (int32_t i = prev.elementAti(انگ_ڪوڊ_Pts); i > 0; i = prev.elementAti(i)) {
                ڪوڊ_Unit_Idx = اندريون_String.moveIndex32(0, i);
                prev_ڪوڊ_Unit_Idx = اندريون_String.moveIndex32(0, prevIdx);
                // Calculate the length by using the code unit.
                length = prev_ڪوڊ_Unit_Idx - ڪوڊ_Unit_Idx;
                prevIdx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                إذا (!f_Skip_سيٽ.شامل_آهي_ڪنجي(اندريون_String.عارضي_Sub_String(ڪوڊ_Unit_Idx, length))
                    && (!isKatakana(اندريون_String.char32At(اندريون_String.moveIndex32(ڪوڊ_Unit_Idx, -1)))
                           || !isKatakana(اندريون_String.char32At(ڪوڊ_Unit_Idx)))) {
                    t_boundary.جوڙيو_Element(i, حالت);
                    انگ_Breaks++;
                }
            }
        }
    } وإلا {
        لكل (int32_t i = انگ_ڪوڊ_Pts; i > 0; i = prev.elementAti(i)) {
            t_boundary.جوڙيو_Element(i, حالت);
            انگ_Breaks++;
        }
        U_ASSERT(prev.elementAti(t_boundary.elementAti(انگ_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    إذا (مليو_Breaks.size() == 0 || مليو_Breaks.peeki() < range_شروعات) {
        t_boundary.جوڙيو_Element(0, حالت);
        انگ_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_انگ_Breaks = 0;
    لكل (int32_t i = انگ_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > prevCPPos);
        int32_t utextPos =  داخلا_Map.isValid() ? داخلا_Map->elementAti(cpPos) : cpPos + range_شروعات;
        U_ASSERT(utextPos >= prevUTextPos);
        إذا (utextPos > prevUTextPos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(مليو_Breaks.size() == 0 || مليو_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            إذا (utextPos != range_شروعات
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_سيٽ.شامل_آهي(utext_char32At(اندريون_متن, utextPos - 1)))) {
                مليو_Breaks.push(utextPos, حالت);
                corrected_انگ_Breaks++;
            }
        } وإلا {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --انگ_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (فراغ)prevCPPos; // suppress compiler warnings about unused variable

    UChar32 ايندڙ_Char = utext_char32At(اندريون_متن, range_پڇاڙي);
    إذا (!مليو_Breaks.is_خالي_جايون() && مليو_Breaks.peeki() == range_پڇاڙي) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (isPhraseBreaking) {
            إذا (!f_Digit_Or_Open_Punctuation_Or_Alphabet_سيٽ.شامل_آهي(ايندڙ_Char)) {
                مليو_Breaks.popi();
                corrected_انگ_Breaks--;
            }
        } وإلا {
            مليو_Breaks.popi();
            corrected_انگ_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    أرجع corrected_انگ_Breaks;
}

فراغ CjkBreakEngine::initJapanesePhraseParameter(UError_ڪوڊ& error) {
    ڀريو_Japanese_Extensions(error);
    ڀريو_Hiragana(error);
}

فراغ CjkBreakEngine::ڀريو_Japanese_Extensions(UError_ڪوڊ& error) {
    ثابت حرف* ٽيگ = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    إذا (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(ٽيگ, error);
        طالما (U_SUCCESS(error) && bundle.has_ايندڙ()) {
            f_Skip_سيٽ.puti(bundle.get_ايندڙ_String(error), 1, error);
        }
    }
}

فراغ CjkBreakEngine::ڀريو_Hiragana(UError_ڪوڊ& error) {
    Unicode_سيٽ hiragana_لفظ_سيٽ(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_لفظ_سيٽ.compact();
    Unicode_سيٽ_Iterator iterator(hiragana_لفظ_سيٽ);
    طالما (iterator.next()) {
        f_Skip_سيٽ.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#جيڪڏهن_پورو

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */

