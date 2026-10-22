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
    أرجع f_سیٹ.شامل_اے(c);
}

int32_t
DictionaryBreakEngine::لبھو_Breaks( UText *متن,
                                 int32_t شروع_Pos,
                                 int32_t سرا_Pos,
                                 UVector32 &لبھیا_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t نتیجہ = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_سیٹ_Native_اشاریہ(متن, شروع_Pos);
    int32_t شروع = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    int32_t current;
    int32_t range_شروع;
    int32_t range_سرا;
    UChar32 c = utext_current32(متن);
    طالما ((current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < سرا_Pos && f_سیٹ.شامل_اے(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_شروع = شروع;
    range_سرا = current;
    نتیجہ = divide_اتے_Dictionary_Range(متن, range_شروع, range_سرا, لبھیا_Breaks, isPhraseBreaking, حالت);
    utext_سیٹ_Native_اشاریہ(متن, current);
    
    أرجع نتیجہ;
}

فراغ
DictionaryBreakEngine::سیٹ_Characters( ثابت Unicode_سیٹ &سیٹ ) {
    f_سیٹ = سیٹ;
    // Compact for caching
    f_سیٹ.compact();
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
    int32_t   اگیتر;     // The longest match with a dictionary word
    int32_t   وتھ;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_لفظ_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_لفظ_LIST_MAX];   // Word Lengths, in code points.

عام:
    Possible_لفظ() : count(0), اگیتر(0), وتھ(-1), mark(0), current(0) {}
    ~Possible_لفظ() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *متن, DictionaryMatcher *dict, int32_t range_سرا );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *متن );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     پٹھ_اتے( UText *متن );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_اگیتر() { أرجع اگیتر; }
  
    // Mark the current candidate as the one we like
    فراغ      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { أرجع cpLengths[mark]; }
};


int32_t Possible_لفظ::candidates( UText *متن, DictionaryMatcher *dict, int32_t range_سرا ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t شروع = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    إذا (شروع != وتھ) {
        وتھ = شروع;
        count = dict->matches(متن, range_سرا-شروع, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, خالی_سوچک, &اگیتر);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        إذا (count <= 0) {
            utext_سیٹ_Native_اشاریہ(متن, شروع);
        }
    }
    إذا (count > 0) {
        utext_سیٹ_Native_اشاریہ(متن, شروع+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    أرجع count;
}

int32_t
Possible_لفظ::acceptMarked( UText *متن ) {
    utext_سیٹ_Native_اشاریہ(متن, وتھ + cuLengths[mark]);
    أرجع cuLengths[mark];
}


UBool
Possible_لفظ::پٹھ_اتے( UText *متن ) {
    إذا (current > 0) {
        utext_سیٹ_Native_اشاریہ(متن, وتھ + cuLengths[--current]);
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
ساكن ثابت int32_t THAI_جڑ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t THAI_اگیتر_COMBINE_حد = 3;

// Elision character
ساكن ثابت int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
ساكن ثابت int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
ساكن ثابت int32_t THAI_MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t THAI_MIN_لفظ_وسعت = THAI_MIN_لفظ * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوڈ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_سیٹ thai_لفظ_سیٹ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سیٹ_Characters(thai_لفظ_سیٹ);
    }
    f_Mark_سیٹ.apply_نمونہ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سیٹ.جوڑو(0x0020);
    f_سرا_لفظ_سیٹ = thai_لفظ_سیٹ;
    f_سرا_لفظ_سیٹ.remove(0x0E31);             // MAI HAN-AKAT
    f_سرا_لفظ_سیٹ.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_لفظ_سیٹ.جوڑو(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_لفظ_سیٹ.جوڑو(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_سیٹ.جوڑو(THAI_PAIYANNOI);
    f_Suffix_سیٹ.جوڑو(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_سیٹ.compact();
    f_سرا_لفظ_سیٹ.compact();
    f_Begin_لفظ_سیٹ.compact();
    f_Suffix_سیٹ.compact();
    UTRACE_EXIT_حالت(حالت);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    مٹاؤ fDictionary;
}

int32_t
ThaiBreakEngine::divide_اتے_Dictionary_Range( UText *متن,
                                                int32_t range_شروع,
                                                int32_t range_سرا,
                                                UVector32 &لبھیا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    utext_سیٹ_Native_اشاریہ(متن, range_شروع);
    utext_moveIndex32(متن, THAI_MIN_لفظ_وسعت);
    إذا (utext_get_Native_اشاریہ(متن) >= range_سرا) {
        أرجع 0;       // Not enough characters for two words
    }
    utext_سیٹ_Native_اشاریہ(متن, range_شروع);


    uint32_t الفاظ_لبھیا = 0;
    int32_t cp_لفظ_لمائی = 0;    // Word Length in Code Points.
    int32_t cu_لفظ_لمائی = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    Possible_لفظ الفاظ[THAI_LOOKAHEAD];
    
    utext_سیٹ_Native_اشاریہ(متن, range_شروع);
    
    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_سرا) {
        cp_لفظ_لمائی = 0;
        cu_لفظ_لمائی = 0;

        // Look for candidate words at the current position
        int32_t candidates = الفاظ[الفاظ_لبھیا%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_سرا);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_لفظ_لمائی = الفاظ[الفاظ_لبھیا % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمائی = الفاظ[الفاظ_لبھیا % THAI_LOOKAHEAD].markedCPLength();
            الفاظ_لبھیا += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_سرا) {
                اذهب لبھیا_بہترین;
            }
            افعل {
                إذا (الفاظ[(الفاظ_لبھیا + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    الفاظ[الفاظ_لبھیا%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_سرا) {
                        اذهب لبھیا_بہترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (الفاظ[(الفاظ_لبھیا + 2) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_سرا)) {
                            الفاظ[الفاظ_لبھیا % THAI_LOOKAHEAD].markCurrent();
                            اذهب لبھیا_بہترین;
                        }
                    }
                    طالما (الفاظ[(الفاظ_لبھیا + 1) % THAI_LOOKAHEAD].پٹھ_اتے(متن));
                }
            }
            طالما (الفاظ[الفاظ_لبھیا % THAI_LOOKAHEAD].پٹھ_اتے(متن));
لبھیا_بہترین:
            // Set UText position to after the accepted word.
            cu_لفظ_لمائی = الفاظ[الفاظ_لبھیا % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمائی = الفاظ[الفاظ_لبھیا % THAI_LOOKAHEAD].markedCPLength();
            الفاظ_لبھیا += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_سرا && cp_لفظ_لمائی < THAI_جڑ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (الفاظ[الفاظ_لبھیا % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) <= 0
                  && (cu_لفظ_لمائی == 0
                      || الفاظ[الفاظ_لبھیا%THAI_LOOKAHEAD].longest_اگیتر() < THAI_اگیتر_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بچیا = range_سرا - (current+cu_لفظ_لمائی);
                UChar32 pc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                    pc = utext_next32(متن);
                    int32_t pc_ناپ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - pc_اشاریہ;
                    chars += pc_ناپ;
                    بچیا -= pc_ناپ;
                    إذا (بچیا <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_سرا_لفظ_سیٹ.شامل_اے(pc) && f_Begin_لفظ_سیٹ.شامل_اے(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t عدد_candidates = الفاظ[(الفاظ_لبھیا + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_سرا);
                        utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمائی + chars);
                        إذا (عدد_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_لفظ_لمائی <= 0) {
                    الفاظ_لبھیا += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_لمائی += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سیٹ_Native_اشاریہ(متن, current+cu_لفظ_لمائی);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_سرا && f_Mark_سیٹ.شامل_اے(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_لمائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_سرا && cu_لفظ_لمائی > 0) {
            إذا (الفاظ[الفاظ_لبھیا%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) <= 0
                && f_Suffix_سیٹ.شامل_اے(uc = utext_current32(متن))) {
                إذا (uc == THAI_PAIYANNOI) {
                    إذا (!f_Suffix_سیٹ.شامل_اے(utext_previous32(متن))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(متن);
                        int32_t paiyannoi_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                        utext_next32(متن);
                        cu_لفظ_لمائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - paiyannoi_اشاریہ; // Add PAIYANNOI to word
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
                        int32_t maiyamok_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                        utext_next32(متن);
                        cu_لفظ_لمائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - maiyamok_اشاریہ; // Add MAIYAMOK to word
                    }
                    وإلا {
                        // Restore prior position
                        utext_next32(متن);
                    }
                }
            }
            وإلا {
                utext_سیٹ_Native_اشاریہ(متن, current+cu_لفظ_لمائی);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_لفظ_لمائی > 0) {
            لبھیا_Breaks.push((current+cu_لفظ_لمائی), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (لبھیا_Breaks.peeki() >= range_سرا) {
        (فراغ) لبھیا_Breaks.popi();
        الفاظ_لبھیا -= 1;
    }

    أرجع الفاظ_لبھیا;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t LAO_جڑ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t LAO_اگیتر_COMBINE_حد = 3;

// Minimum word size
ساكن ثابت int32_t LAO_MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t LAO_MIN_لفظ_وسعت = LAO_MIN_لفظ * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوڈ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_سیٹ lao_لفظ_سیٹ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سیٹ_Characters(lao_لفظ_سیٹ);
    }
    f_Mark_سیٹ.apply_نمونہ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سیٹ.جوڑو(0x0020);
    f_سرا_لفظ_سیٹ = lao_لفظ_سیٹ;
    f_سرا_لفظ_سیٹ.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_لفظ_سیٹ.جوڑو(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_لفظ_سیٹ.جوڑو(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_لفظ_سیٹ.جوڑو(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_سیٹ.compact();
    f_سرا_لفظ_سیٹ.compact();
    f_Begin_لفظ_سیٹ.compact();
    UTRACE_EXIT_حالت(حالت);
}

LaoBreakEngine::~LaoBreakEngine() {
    مٹاؤ fDictionary;
}

int32_t
LaoBreakEngine::divide_اتے_Dictionary_Range( UText *متن,
                                                int32_t range_شروع,
                                                int32_t range_سرا,
                                                UVector32 &لبھیا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_سرا - range_شروع) < LAO_MIN_لفظ_وسعت) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t الفاظ_لبھیا = 0;
    int32_t cp_لفظ_لمائی = 0;
    int32_t cu_لفظ_لمائی = 0;
    int32_t current;
    Possible_لفظ الفاظ[LAO_LOOKAHEAD];

    utext_سیٹ_Native_اشاریہ(متن, range_شروع);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_سرا) {
        cu_لفظ_لمائی = 0;
        cp_لفظ_لمائی = 0;

        // Look for candidate words at the current position
        int32_t candidates = الفاظ[الفاظ_لبھیا%LAO_LOOKAHEAD].candidates(متن, fDictionary, range_سرا);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_لفظ_لمائی = الفاظ[الفاظ_لبھیا % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمائی = الفاظ[الفاظ_لبھیا % LAO_LOOKAHEAD].markedCPLength();
            الفاظ_لبھیا += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (utext_get_Native_اشاریہ(متن) >= range_سرا) {
                اذهب لبھیا_بہترین;
            }
            افعل {
                إذا (الفاظ[(الفاظ_لبھیا + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    الفاظ[الفاظ_لبھیا%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_سرا) {
                        اذهب لبھیا_بہترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (الفاظ[(الفاظ_لبھیا + 2) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_سرا)) {
                            الفاظ[الفاظ_لبھیا % LAO_LOOKAHEAD].markCurrent();
                            اذهب لبھیا_بہترین;
                        }
                    }
                    طالما (الفاظ[(الفاظ_لبھیا + 1) % LAO_LOOKAHEAD].پٹھ_اتے(متن));
                }
            }
            طالما (الفاظ[الفاظ_لبھیا % LAO_LOOKAHEAD].پٹھ_اتے(متن));
لبھیا_بہترین:
            cu_لفظ_لمائی = الفاظ[الفاظ_لبھیا % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمائی = الفاظ[الفاظ_لبھیا % LAO_LOOKAHEAD].markedCPLength();
            الفاظ_لبھیا += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_سرا && cp_لفظ_لمائی < LAO_جڑ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (الفاظ[الفاظ_لبھیا % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) <= 0
                  && (cu_لفظ_لمائی == 0
                      || الفاظ[الفاظ_لبھیا%LAO_LOOKAHEAD].longest_اگیتر() < LAO_اگیتر_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بچیا = range_سرا - (current + cu_لفظ_لمائی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                    pc = utext_next32(متن);
                    int32_t pc_ناپ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - pc_اشاریہ;
                    chars += pc_ناپ;
                    بچیا -= pc_ناپ;
                    إذا (بچیا <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_سرا_لفظ_سیٹ.شامل_اے(pc) && f_Begin_لفظ_سیٹ.شامل_اے(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t عدد_candidates = الفاظ[(الفاظ_لبھیا + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_سرا);
                        utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمائی + chars);
                        إذا (عدد_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_لفظ_لمائی <= 0) {
                    الفاظ_لبھیا += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_لمائی += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمائی);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_سرا && f_Mark_سیٹ.شامل_اے(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_لمائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_لفظ_لمائی > 0) {
            لبھیا_Breaks.push((current+cu_لفظ_لمائی), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (لبھیا_Breaks.peeki() >= range_سرا) {
        (فراغ) لبھیا_Breaks.popi();
        الفاظ_لبھیا -= 1;
    }

    أرجع الفاظ_لبھیا;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t BURMESE_جڑ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t BURMESE_اگیتر_COMBINE_حد = 3;

// Minimum word size
ساكن ثابت int32_t BURMESE_MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t BURMESE_MIN_لفظ_وسعت = BURMESE_MIN_لفظ * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوڈ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_لفظ_سیٹ.جوڑو(0x1000, 0x102A);      // basic consonants and independent vowels
    f_سرا_لفظ_سیٹ.apply_نمونہ(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), حالت);
    f_Mark_سیٹ.apply_نمونہ(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سیٹ.جوڑو(0x0020);
    إذا (U_SUCCESS(حالت)) {
        سیٹ_Characters(f_سرا_لفظ_سیٹ);
    }

    // Compact for caching.
    f_Mark_سیٹ.compact();
    f_سرا_لفظ_سیٹ.compact();
    f_Begin_لفظ_سیٹ.compact();
    UTRACE_EXIT_حالت(حالت);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    مٹاؤ fDictionary;
}

int32_t
BurmeseBreakEngine::divide_اتے_Dictionary_Range( UText *متن,
                                                int32_t range_شروع,
                                                int32_t range_سرا,
                                                UVector32 &لبھیا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوڈ& حالت ) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_سرا - range_شروع) < BURMESE_MIN_لفظ_وسعت) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t الفاظ_لبھیا = 0;
    int32_t cp_لفظ_لمائی = 0;
    int32_t cu_لفظ_لمائی = 0;
    int32_t current;
    Possible_لفظ الفاظ[BURMESE_LOOKAHEAD];

    utext_سیٹ_Native_اشاریہ(متن, range_شروع);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_سرا) {
        cu_لفظ_لمائی = 0;
        cp_لفظ_لمائی = 0;

        // Look for candidate words at the current position
        int32_t candidates = الفاظ[الفاظ_لبھیا%BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_سرا);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_لفظ_لمائی = الفاظ[الفاظ_لبھیا % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمائی = الفاظ[الفاظ_لبھیا % BURMESE_LOOKAHEAD].markedCPLength();
            الفاظ_لبھیا += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (utext_get_Native_اشاریہ(متن) >= range_سرا) {
                اذهب لبھیا_بہترین;
            }
            افعل {
                إذا (الفاظ[(الفاظ_لبھیا + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    الفاظ[الفاظ_لبھیا%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_سرا) {
                        اذهب لبھیا_بہترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (الفاظ[(الفاظ_لبھیا + 2) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_سرا)) {
                            الفاظ[الفاظ_لبھیا % BURMESE_LOOKAHEAD].markCurrent();
                            اذهب لبھیا_بہترین;
                        }
                    }
                    طالما (الفاظ[(الفاظ_لبھیا + 1) % BURMESE_LOOKAHEAD].پٹھ_اتے(متن));
                }
            }
            طالما (الفاظ[الفاظ_لبھیا % BURMESE_LOOKAHEAD].پٹھ_اتے(متن));
لبھیا_بہترین:
            cu_لفظ_لمائی = الفاظ[الفاظ_لبھیا % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمائی = الفاظ[الفاظ_لبھیا % BURMESE_LOOKAHEAD].markedCPLength();
            الفاظ_لبھیا += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_سرا && cp_لفظ_لمائی < BURMESE_جڑ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (الفاظ[الفاظ_لبھیا % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) <= 0
                  && (cu_لفظ_لمائی == 0
                      || الفاظ[الفاظ_لبھیا%BURMESE_LOOKAHEAD].longest_اگیتر() < BURMESE_اگیتر_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بچیا = range_سرا - (current + cu_لفظ_لمائی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                    pc = utext_next32(متن);
                    int32_t pc_ناپ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - pc_اشاریہ;
                    chars += pc_ناپ;
                    بچیا -= pc_ناپ;
                    إذا (بچیا <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_سرا_لفظ_سیٹ.شامل_اے(pc) && f_Begin_لفظ_سیٹ.شامل_اے(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t عدد_candidates = الفاظ[(الفاظ_لبھیا + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_سرا);
                        utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمائی + chars);
                        إذا (عدد_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_لفظ_لمائی <= 0) {
                    الفاظ_لبھیا += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_لمائی += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمائی);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_سرا && f_Mark_سیٹ.شامل_اے(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_لمائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_لفظ_لمائی > 0) {
            لبھیا_Breaks.push((current+cu_لفظ_لمائی), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (لبھیا_Breaks.peeki() >= range_سرا) {
        (فراغ) لبھیا_Breaks.popi();
        الفاظ_لبھیا -= 1;
    }

    أرجع الفاظ_لبھیا;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t KHMER_جڑ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t KHMER_اگیتر_COMBINE_حد = 3;

// Minimum word size
ساكن ثابت int32_t KHMER_MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t KHMER_MIN_لفظ_وسعت = KHMER_MIN_لفظ * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوڈ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_سیٹ khmer_لفظ_سیٹ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سیٹ_Characters(khmer_لفظ_سیٹ);
    }
    f_Mark_سیٹ.apply_نمونہ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سیٹ.جوڑو(0x0020);
    f_سرا_لفظ_سیٹ = khmer_لفظ_سیٹ;
    f_Begin_لفظ_سیٹ.جوڑو(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_سرا_لفظ_سیٹ.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_سیٹ.compact();
    f_سرا_لفظ_سیٹ.compact();
    f_Begin_لفظ_سیٹ.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_حالت(حالت);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    مٹاؤ fDictionary;
}

int32_t
KhmerBreakEngine::divide_اتے_Dictionary_Range( UText *متن,
                                                int32_t range_شروع,
                                                int32_t range_سرا,
                                                UVector32 &لبھیا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوڈ& حالت ) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_سرا - range_شروع) < KHMER_MIN_لفظ_وسعت) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t الفاظ_لبھیا = 0;
    int32_t cp_لفظ_لمائی = 0;
    int32_t cu_لفظ_لمائی = 0;
    int32_t current;
    Possible_لفظ الفاظ[KHMER_LOOKAHEAD];

    utext_سیٹ_Native_اشاریہ(متن, range_شروع);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_سرا) {
        cu_لفظ_لمائی = 0;
        cp_لفظ_لمائی = 0;

        // Look for candidate words at the current position
        int32_t candidates = الفاظ[الفاظ_لبھیا%KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_سرا);

        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_لفظ_لمائی = الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمائی = الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].markedCPLength();
            الفاظ_لبھیا += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_سرا) {
                اذهب لبھیا_بہترین;
            }
            افعل {
                إذا (الفاظ[(الفاظ_لبھیا + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_سرا) {
                        اذهب لبھیا_بہترین;
                    }

                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (الفاظ[(الفاظ_لبھیا + 2) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_سرا)) {
                            الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].markCurrent();
                            اذهب لبھیا_بہترین;
                        }
                    }
                    طالما (الفاظ[(الفاظ_لبھیا + 1) % KHMER_LOOKAHEAD].پٹھ_اتے(متن));
                }
            }
            طالما (الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].پٹھ_اتے(متن));
لبھیا_بہترین:
            cu_لفظ_لمائی = الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمائی = الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].markedCPLength();
            الفاظ_لبھیا += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_سرا && cp_لفظ_لمائی < KHMER_جڑ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_سرا) <= 0
                  && (cu_لفظ_لمائی == 0
                      || الفاظ[الفاظ_لبھیا % KHMER_LOOKAHEAD].longest_اگیتر() < KHMER_اگیتر_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بچیا = range_سرا - (current+cu_لفظ_لمائی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                    pc = utext_next32(متن);
                    int32_t pc_ناپ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - pc_اشاریہ;
                    chars += pc_ناپ;
                    بچیا -= pc_ناپ;
                    إذا (بچیا <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_سرا_لفظ_سیٹ.شامل_اے(pc) && f_Begin_لفظ_سیٹ.شامل_اے(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t عدد_candidates = الفاظ[(الفاظ_لبھیا + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_سرا);
                        utext_سیٹ_Native_اشاریہ(متن, current+cu_لفظ_لمائی+chars);
                        إذا (عدد_candidates > 0) {
                            اكسر;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                إذا (cu_لفظ_لمائی <= 0) {
                    الفاظ_لبھیا += 1;
                }

                // Update the length with the passed-over characters
                cu_لفظ_لمائی += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سیٹ_Native_اشاریہ(متن, current+cu_لفظ_لمائی);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_سرا && f_Mark_سیٹ.شامل_اے(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_لمائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - currPos;
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
        إذا (cu_لفظ_لمائی > 0) {
            لبھیا_Breaks.push((current+cu_لفظ_لمائی), حالت);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (لبھیا_Breaks.peeki() >= range_سرا) {
        (فراغ) لبھیا_Breaks.popi();
        الفاظ_لبھیا -= 1;
    }

    أرجع الفاظ_لبھیا;
}

#إذا !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
ساكن ثابت uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_کوڈ &حالت)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(خطأ) {
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = خالی_سوچک;
    nfkcNorm2 = Normalizer2::getNFKCInstance(حالت);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_لفظ_سیٹ.apply_نمونہ(UnicodeString(u"[\\uac00-\\ud7a3]"), حالت);
    f_Hangul_لفظ_سیٹ.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ.apply_نمونہ(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), حالت);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ.compact();
    f_Close_Punctuation_سیٹ.apply_نمونہ(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), حالت);
    f_Close_Punctuation_سیٹ.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    إذا (type == kKorean) {
        إذا (U_SUCCESS(حالت)) {
            سیٹ_Characters(f_Hangul_لفظ_سیٹ);
        }
    } وإلا { // Chinese and Japanese
        Unicode_سیٹ cj_سیٹ(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), حالت);
        isCj = سچ;
        إذا (U_SUCCESS(حالت)) {
            سیٹ_Characters(cj_سیٹ);
#إذا UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = جديد MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ,
                                               f_Close_Punctuation_سیٹ, حالت);
            إذا (fMlBreakEngine == خالی_سوچک) {
                حالت = U_حافظہ_ونڈ_ERROR;
            }
#وإلا
            initJapanesePhraseParameter(حالت);
#جے_انت
        }
    }
    UTRACE_EXIT_حالت(حالت);
}

CjkBreakEngine::~CjkBreakEngine(){
    مٹاؤ fDictionary;
    مٹاؤ fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
ساكن ثابت int32_t k_Max_Katakana_لمائی = 8;
ساكن ثابت int32_t k_Max_Katakana_گروہ_لمائی = 20;
ساكن ثابت uint32_t maxSnlp = 255;

ساكن مضمن uint32_t get_Katakana_لاگت(int32_t لفظ_لمائی){
    //TODO: fill array with actual values from dictionary!
    ساكن ثابت uint32_t katakana_لاگت[k_Max_Katakana_لمائی + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    أرجع (لفظ_لمائی > k_Max_Katakana_لمائی) ? 8192 : katakana_لاگت[لفظ_لمائی];
}

ساكن مضمن منطقي isKatakana(UChar32 value) {
    أرجع (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

ساكن مضمن int32_t utext_i32_flag(int32_t ثنائی_ہندسہ_اشاریہ) {
    أرجع static_cast<int32_t>(1) << ثنائی_ہندسہ_اشاریہ;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_اتے_Dictionary_Range( UText *وچ_متن,
        int32_t range_شروع,
        int32_t range_سرا,
        UVector32 &لبھیا_Breaks,
        UBool isPhraseBreaking,
        UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا (range_شروع >= range_سرا) {
        أرجع 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString وچ_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    Local_اشارہ_گر<UVector32>     داخلہ_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    إذا ((وچ_متن->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         وچ_متن->chunk_Native_شروع <= range_شروع &&
         وچ_متن->chunk_Native_حد_بندی >= range_سرا   &&
         وچ_متن->native_Indexing_حد_بندی >= range_سرا - وچ_متن->chunk_Native_شروع) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        وچ_String.سیٹ_منزل(خطأ,
                       وچ_متن->chunkContents + range_شروع - وچ_متن->chunk_Native_شروع,
                       range_سرا - range_شروع);
    } وإلا {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_سیٹ_Native_اشاریہ(وچ_متن, range_شروع);
        int32_t حد_بندی = range_سرا;
        U_ASSERT(حد_بندی <= utext_native_لمائی(وچ_متن));
        إذا (حد_بندی > utext_native_لمائی(وچ_متن)) {
            حد_بندی = static_cast<int32_t>(utext_native_لمائی(وچ_متن));
        }
        داخلہ_Map.adopt_Instead_And_Check_Error_کوڈ(جديد UVector32(حالت), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع 0;
        }
        طالما (utext_get_Native_اشاریہ(وچ_متن) < حد_بندی) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_اشاریہ(وچ_متن));
            UChar32 c = utext_next32(وچ_متن);
            U_ASSERT(c != U_SENTINEL);
            وچ_String.append(c);
            طالما (داخلہ_Map->size() < وچ_String.length()) {
                داخلہ_Map->جوڑو_Element(nativePosition, حالت);
            }
        }
        داخلہ_Map->جوڑو_Element(حد_بندی, حالت);
    }


    إذا (!nfkcNorm2->isNormalized(وچ_String, حالت)) {
        UnicodeString normalized_داخلہ;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        Local_اشارہ_گر<UVector32> normalizedMap(جديد UVector32(حالت), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        لكل (int32_t srcI = 0; srcI < وچ_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragment_شروع_I = srcI;
            UChar32 c = وچ_String.char32At(srcI);
            لكل (;;) {
                fragment.append(c);
                srcI = وچ_String.moveIndex32(srcI, 1);
                إذا (srcI == وچ_String.length()) {
                    اكسر;
                }
                c = وچ_String.char32At(srcI);
                إذا (nfkcNorm2->hasBoundaryBefore(c)) {
                    اكسر;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, حالت);
            normalized_داخلہ.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragment_Original_شروع = داخلہ_Map.isValid() ?
                    داخلہ_Map->elementAti(fragment_شروع_I) : fragment_شروع_I+range_شروع;
            طالما (normalizedMap->size() < normalized_داخلہ.length()) {
                normalizedMap->جوڑو_Element(fragment_Original_شروع, حالت);
                إذا (U_FAILURE(حالت)) {
                    اكسر;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_داخلہ.length());
        int32_t native_سرا = داخلہ_Map.isValid() ?
                داخلہ_Map->elementAti(وچ_String.length()) : وچ_String.length()+range_شروع;
        normalizedMap->جوڑو_Element(native_سرا, حالت);

        داخلہ_Map = std::move(normalizedMap);
        وچ_String = std::move(normalized_داخلہ);
    }

    int32_t عدد_کوڈ_Pts = وچ_String.گنتی_Char32();
    إذا (عدد_کوڈ_Pts != وچ_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = داخلہ_Map.isValid();
        إذا (!hadExistingMap) {
            داخلہ_Map.adopt_Instead_And_Check_Error_کوڈ(جديد UVector32(حالت), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع 0;
            }
        }
        int32_t cpIdx = 0;
        لكل (int32_t cuIdx = 0; ; cuIdx = وچ_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            إذا (hadExistingMap) {
                داخلہ_Map->سیٹ_Element_At(داخلہ_Map->elementAti(cuIdx), cpIdx);
            } وإلا {
                داخلہ_Map->جوڑو_Element(cuIdx+range_شروع, حالت);
            }
            cpIdx++;
            إذا (cuIdx == وچ_String.length()) {
               اكسر;
            }
        }
    }

#إذا UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    إذا (isPhraseBreaking && isCj) {
        أرجع fMlBreakEngine->divide_اتے_Range(وچ_متن, range_شروع, range_سرا, لبھیا_Breaks, وچ_String,
                                             داخلہ_Map, حالت);
    }
#جے_انت

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 بہترین_Snlp(عدد_کوڈ_Pts + 1, حالت);
    بہترین_Snlp.جوڑو_Element(0, حالت);
    لكل(int32_t i = 1; i <= عدد_کوڈ_Pts; i++) {
        بہترین_Snlp.جوڑو_Element(kuint32max, حالت);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 prev(عدد_کوڈ_Pts + 1, حالت);
    لكل(int32_t i = 0; i <= عدد_کوڈ_Pts; i++){
        prev.جوڑو_Element(-1, حالت);
    }

    ثابت int32_t max_لفظ_ناپ = 20;
    UVector32 اقدار(عدد_کوڈ_Pts, حالت);
    اقدار.سیٹ_ناپ(عدد_کوڈ_Pts);
    UVector32 lengths(عدد_کوڈ_Pts, حالت);
    lengths.سیٹ_ناپ(عدد_کوڈ_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &وچ_String, &حالت);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    منطقي is_prev_katakana = خطأ;
    لكل (int32_t i = 0;  i < عدد_کوڈ_Pts;  ++i, ix = وچ_String.moveIndex32(ix, 1)) {
        إذا (static_cast<uint32_t>(بہترین_Snlp.elementAti(i)) == kuint32max) {
            استمر;
        }

        int32_t count;
        utext_سیٹ_Native_اشاریہ(&fu, ix);
        count = fDictionary->matches(&fu, max_لفظ_ناپ, عدد_کوڈ_Pts,
                             خالی_سوچک, lengths.get_بفر(), اقدار.get_بفر(), خالی_سوچک);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        إذا ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_لفظ_سیٹ.شامل_اے(وچ_String.char32At(ix))) {
            اقدار.سیٹ_Element_At(maxSnlp, count);   // 255
            lengths.سیٹ_Element_At(1, count++);
        }

        لكل (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(بہترین_Snlp.elementAti(i)) + static_cast<uint32_t>(اقدار.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            إذا (newSnlp < static_cast<uint32_t>(بہترین_Snlp.elementAti(ln_j_i))) {
                بہترین_Snlp.سیٹ_Element_At(newSnlp, ln_j_i);
                prev.سیٹ_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        منطقي is_katakana = isKatakana(وچ_String.char32At(ix));
        int32_t katakana_لگاتار_لمائی = 1;
        إذا (!is_prev_katakana && is_katakana) {
            int32_t j = وچ_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            طالما (j < وچ_String.length() && katakana_لگاتار_لمائی < k_Max_Katakana_گروہ_لمائی &&
                    isKatakana(وچ_String.char32At(j))) {
                j = وچ_String.moveIndex32(j, 1);
                katakana_لگاتار_لمائی++;
            }
            إذا (katakana_لگاتار_لمائی < k_Max_Katakana_گروہ_لمائی) {
                uint32_t newSnlp = بہترین_Snlp.elementAti(i) + get_Katakana_لاگت(katakana_لگاتار_لمائی);
                إذا (newSnlp < static_cast<uint32_t>(بہترین_Snlp.elementAti(i + katakana_لگاتار_لمائی))) {
                    بہترین_Snlp.سیٹ_Element_At(newSnlp, i+katakana_لگاتار_لمائی);
                    prev.سیٹ_Element_At(i, i+katakana_لگاتار_لمائی);  // prev[j] = i;
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
    UVector32 t_boundary(عدد_کوڈ_Pts+1, حالت);

    int32_t عدد_Breaks = 0;
    // No segmentation found, set boundary to end of range
    إذا (static_cast<uint32_t>(بہترین_Snlp.elementAti(عدد_کوڈ_Pts)) == kuint32max) {
        t_boundary.جوڑو_Element(عدد_کوڈ_Pts, حالت);
        عدد_Breaks++;
    } وإلا إذا (isPhraseBreaking) {
        t_boundary.جوڑو_Element(عدد_کوڈ_Pts, حالت);
        إذا(U_SUCCESS(حالت)) {
            عدد_Breaks++;
            int32_t prevIdx = عدد_کوڈ_Pts;

            int32_t کوڈ_Unit_Idx = -1;
            int32_t prev_کوڈ_Unit_Idx = -1;
            int32_t length = -1;
            لكل (int32_t i = prev.elementAti(عدد_کوڈ_Pts); i > 0; i = prev.elementAti(i)) {
                کوڈ_Unit_Idx = وچ_String.moveIndex32(0, i);
                prev_کوڈ_Unit_Idx = وچ_String.moveIndex32(0, prevIdx);
                // Calculate the length by using the code unit.
                length = prev_کوڈ_Unit_Idx - کوڈ_Unit_Idx;
                prevIdx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                إذا (!f_Skip_سیٹ.شامل_اے_کلید(وچ_String.عارضی_Sub_String(کوڈ_Unit_Idx, length))
                    && (!isKatakana(وچ_String.char32At(وچ_String.moveIndex32(کوڈ_Unit_Idx, -1)))
                           || !isKatakana(وچ_String.char32At(کوڈ_Unit_Idx)))) {
                    t_boundary.جوڑو_Element(i, حالت);
                    عدد_Breaks++;
                }
            }
        }
    } وإلا {
        لكل (int32_t i = عدد_کوڈ_Pts; i > 0; i = prev.elementAti(i)) {
            t_boundary.جوڑو_Element(i, حالت);
            عدد_Breaks++;
        }
        U_ASSERT(prev.elementAti(t_boundary.elementAti(عدد_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    إذا (لبھیا_Breaks.size() == 0 || لبھیا_Breaks.peeki() < range_شروع) {
        t_boundary.جوڑو_Element(0, حالت);
        عدد_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_عدد_Breaks = 0;
    لكل (int32_t i = عدد_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > prevCPPos);
        int32_t utextPos =  داخلہ_Map.isValid() ? داخلہ_Map->elementAti(cpPos) : cpPos + range_شروع;
        U_ASSERT(utextPos >= prevUTextPos);
        إذا (utextPos > prevUTextPos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(لبھیا_Breaks.size() == 0 || لبھیا_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            إذا (utextPos != range_شروع
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_سیٹ.شامل_اے(utext_char32At(وچ_متن, utextPos - 1)))) {
                لبھیا_Breaks.push(utextPos, حالت);
                corrected_عدد_Breaks++;
            }
        } وإلا {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --عدد_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (فراغ)prevCPPos; // suppress compiler warnings about unused variable

    UChar32 اگلا_Char = utext_char32At(وچ_متن, range_سرا);
    إذا (!لبھیا_Breaks.is_خالی_تھاواں() && لبھیا_Breaks.peeki() == range_سرا) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (isPhraseBreaking) {
            إذا (!f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ.شامل_اے(اگلا_Char)) {
                لبھیا_Breaks.popi();
                corrected_عدد_Breaks--;
            }
        } وإلا {
            لبھیا_Breaks.popi();
            corrected_عدد_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    أرجع corrected_عدد_Breaks;
}

فراغ CjkBreakEngine::initJapanesePhraseParameter(UError_کوڈ& error) {
    لدو_Japanese_Extensions(error);
    لدو_Hiragana(error);
}

فراغ CjkBreakEngine::لدو_Japanese_Extensions(UError_کوڈ& error) {
    ثابت حرف* ٹیگ = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    إذا (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(ٹیگ, error);
        طالما (U_SUCCESS(error) && bundle.has_اگلا()) {
            f_Skip_سیٹ.puti(bundle.get_اگلا_String(error), 1, error);
        }
    }
}

فراغ CjkBreakEngine::لدو_Hiragana(UError_کوڈ& error) {
    Unicode_سیٹ hiragana_لفظ_سیٹ(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_لفظ_سیٹ.compact();
    Unicode_سیٹ_Iterator iterator(hiragana_لفظ_سیٹ);
    طالما (iterator.next()) {
        f_Skip_سیٹ.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#جے_انت

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */

