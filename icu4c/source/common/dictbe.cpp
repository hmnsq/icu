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
    أرجع f_سېټ.لري(c);
}

int32_t
DictionaryBreakEngine::پیدا_کړه_Breaks( UText *متن,
                                 int32_t پیل_Pos,
                                 int32_t پای_ټکی_Pos,
                                 UVector32 &وموندل_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t پایله = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_سېټ_Native_شاخص(متن, پیل_Pos);
    int32_t پیل = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    int32_t current;
    int32_t range_پیل;
    int32_t range_پای_ټکی;
    UChar32 c = utext_current32(متن);
    طالما ((current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < پای_ټکی_Pos && f_سېټ.لري(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_پیل = پیل;
    range_پای_ټکی = current;
    پایله = divide_پورته_Dictionary_Range(متن, range_پیل, range_پای_ټکی, وموندل_Breaks, isPhraseBreaking, حالت);
    utext_سېټ_Native_شاخص(متن, current);
    
    أرجع پایله;
}

فراغ
DictionaryBreakEngine::سېټ_Characters( ثابت Unicode_سېټ &سېټ ) {
    f_سېټ = سېټ;
    // Compact for caching
    f_سېټ.compact();
}

/*
 ******************************************************************
 * PossibleWord
 */

// Helper class for improving readability of the Thai/Lao/Khmer word break
// algorithm. The implementation is completely inline.

// List size, limited by the maximum number of words in the dictionary
// that form a nested sequence.
ساكن ثابت int32_t POSSIBLE_کلمه_LIST_MAX = 20;

صنف Possible_کلمه {
خاص:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   مختاړی;     // The longest match with a dictionary word
    int32_t   دننه_واټن;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_کلمه_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_کلمه_LIST_MAX];   // Word Lengths, in code points.

عام:
    Possible_کلمه() : count(0), مختاړی(0), دننه_واټن(-1), mark(0), current(0) {}
    ~Possible_کلمه() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *متن, DictionaryMatcher *dict, int32_t range_پای_ټکی );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *متن );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     شا_پورته( UText *متن );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_مختاړی() { أرجع مختاړی; }
  
    // Mark the current candidate as the one we like
    فراغ      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { أرجع cpLengths[mark]; }
};


int32_t Possible_کلمه::candidates( UText *متن, DictionaryMatcher *dict, int32_t range_پای_ټکی ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t پیل = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    إذا (پیل != دننه_واټن) {
        دننه_واټن = پیل;
        count = dict->matches(متن, range_پای_ټکی-پیل, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, تش_اشاره, &مختاړی);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        إذا (count <= 0) {
            utext_سېټ_Native_شاخص(متن, پیل);
        }
    }
    إذا (count > 0) {
        utext_سېټ_Native_شاخص(متن, پیل+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    أرجع count;
}

int32_t
Possible_کلمه::acceptMarked( UText *متن ) {
    utext_سېټ_Native_شاخص(متن, دننه_واټن + cuLengths[mark]);
    أرجع cuLengths[mark];
}


UBool
Possible_کلمه::شا_پورته( UText *متن ) {
    إذا (current > 0) {
        utext_سېټ_Native_شاخص(متن, دننه_واټن + cuLengths[--current]);
        أرجع ریښتیا;
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
ساكن ثابت int32_t THAI_ریښه_COMBINE_پوله = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t THAI_مختاړی_COMBINE_پوله = 3;

// Elision character
ساكن ثابت int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
ساكن ثابت int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
ساكن ثابت int32_t THAI_MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t THAI_MIN_کلمه_پراختیا = THAI_MIN_کلمه * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوډ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ثبت(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_سېټ thai_کلمه_سېټ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سېټ_Characters(thai_کلمه_سېټ);
    }
    f_Mark_سېټ.apply_نمونه(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سېټ.جمع_کړه(0x0020);
    f_پای_ټکی_کلمه_سېټ = thai_کلمه_سېټ;
    f_پای_ټکی_کلمه_سېټ.remove(0x0E31);             // MAI HAN-AKAT
    f_پای_ټکی_کلمه_سېټ.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_کلمه_سېټ.جمع_کړه(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_کلمه_سېټ.جمع_کړه(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_سېټ.جمع_کړه(THAI_PAIYANNOI);
    f_Suffix_سېټ.جمع_کړه(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_سېټ.compact();
    f_پای_ټکی_کلمه_سېټ.compact();
    f_Begin_کلمه_سېټ.compact();
    f_Suffix_سېټ.compact();
    UTRACE_EXIT_حالت(حالت);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    ړنګول fDictionary;
}

int32_t
ThaiBreakEngine::divide_پورته_Dictionary_Range( UText *متن,
                                                int32_t range_پیل,
                                                int32_t range_پای_ټکی,
                                                UVector32 &وموندل_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    utext_سېټ_Native_شاخص(متن, range_پیل);
    utext_moveIndex32(متن, THAI_MIN_کلمه_پراختیا);
    إذا (utext_get_Native_شاخص(متن) >= range_پای_ټکی) {
        أرجع 0;       // Not enough characters for two words
    }
    utext_سېټ_Native_شاخص(متن, range_پیل);


    uint32_t ټکي_وموندل = 0;
    int32_t cp_کلمه_اوږدتیا = 0;    // Word Length in Code Points.
    int32_t cu_کلمه_اوږدتیا = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    Possible_کلمه ټکي[THAI_LOOKAHEAD];
    
    utext_سېټ_Native_شاخص(متن, range_پیل);
    
    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_پای_ټکی) {
        cp_کلمه_اوږدتیا = 0;
        cu_کلمه_اوږدتیا = 0;

        // Look for candidate words at the current position
        int32_t candidates = ټکي[ټکي_وموندل%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_کلمه_اوږدتیا = ټکي[ټکي_وموندل % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_اوږدتیا = ټکي[ټکي_وموندل % THAI_LOOKAHEAD].markedCPLength();
            ټکي_وموندل += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_پای_ټکی) {
                اذهب وموندل_غوره;
            }
            افعل {
                إذا (ټکي[(ټکي_وموندل + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ټکي[ټکي_وموندل%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_پای_ټکی) {
                        اذهب وموندل_غوره;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (ټکي[(ټکي_وموندل + 2) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی)) {
                            ټکي[ټکي_وموندل % THAI_LOOKAHEAD].markCurrent();
                            اذهب وموندل_غوره;
                        }
                    }
                    طالما (ټکي[(ټکي_وموندل + 1) % THAI_LOOKAHEAD].شا_پورته(متن));
                }
            }
            طالما (ټکي[ټکي_وموندل % THAI_LOOKAHEAD].شا_پورته(متن));
وموندل_غوره:
            // Set UText position to after the accepted word.
            cu_کلمه_اوږدتیا = ټکي[ټکي_وموندل % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_اوږدتیا = ټکي[ټکي_وموندل % THAI_LOOKAHEAD].markedCPLength();
            ټکي_وموندل += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_پای_ټکی && cp_کلمه_اوږدتیا < THAI_ریښه_COMBINE_پوله) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (ټکي[ټکي_وموندل % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) <= 0
                  && (cu_کلمه_اوږدتیا == 0
                      || ټکي[ټکي_وموندل%THAI_LOOKAHEAD].longest_مختاړی() < THAI_مختاړی_COMBINE_پوله)) {
                // Look for a plausible word boundary
                int32_t پاتې_شوی = range_پای_ټکی - (current+cu_کلمه_اوږدتیا);
                UChar32 pc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                    pc = utext_next32(متن);
                    int32_t pc_اندازه = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - pc_شاخص;
                    chars += pc_اندازه;
                    پاتې_شوی -= pc_اندازه;
                    إذا (پاتې_شوی <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_پای_ټکی_کلمه_سېټ.لري(pc) && f_Begin_کلمه_سېټ.لري(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t عدد_candidates = ټکي[(ټکي_وموندل + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی);
                        utext_سېټ_Native_شاخص(متن, current + cu_کلمه_اوږدتیا + chars);
                        إذا (عدد_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_کلمه_اوږدتیا <= 0) {
                    ټکي_وموندل += 1;
                }
                
                // Update the length with the passed-over characters
                cu_کلمه_اوږدتیا += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سېټ_Native_شاخص(متن, current+cu_کلمه_اوږدتیا);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_پای_ټکی && f_Mark_سېټ.لري(utext_current32(متن))) {
            utext_next32(متن);
            cu_کلمه_اوږدتیا += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_پای_ټکی && cu_کلمه_اوږدتیا > 0) {
            إذا (ټکي[ټکي_وموندل%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) <= 0
                && f_Suffix_سېټ.لري(uc = utext_current32(متن))) {
                إذا (uc == THAI_PAIYANNOI) {
                    إذا (!f_Suffix_سېټ.لري(utext_previous32(متن))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(متن);
                        int32_t paiyannoi_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                        utext_next32(متن);
                        cu_کلمه_اوږدتیا += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - paiyannoi_شاخص; // Add PAIYANNOI to word
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
                        int32_t maiyamok_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                        utext_next32(متن);
                        cu_کلمه_اوږدتیا += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - maiyamok_شاخص; // Add MAIYAMOK to word
                    }
                    وإلا {
                        // Restore prior position
                        utext_next32(متن);
                    }
                }
            }
            وإلا {
                utext_سېټ_Native_شاخص(متن, current+cu_کلمه_اوږدتیا);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_کلمه_اوږدتیا > 0) {
            وموندل_Breaks.push((current+cu_کلمه_اوږدتیا), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (وموندل_Breaks.peeki() >= range_پای_ټکی) {
        (فراغ) وموندل_Breaks.popi();
        ټکي_وموندل -= 1;
    }

    أرجع ټکي_وموندل;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t LAO_ریښه_COMBINE_پوله = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t LAO_مختاړی_COMBINE_پوله = 3;

// Minimum word size
ساكن ثابت int32_t LAO_MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t LAO_MIN_کلمه_پراختیا = LAO_MIN_کلمه * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوډ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ثبت(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_سېټ lao_کلمه_سېټ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سېټ_Characters(lao_کلمه_سېټ);
    }
    f_Mark_سېټ.apply_نمونه(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سېټ.جمع_کړه(0x0020);
    f_پای_ټکی_کلمه_سېټ = lao_کلمه_سېټ;
    f_پای_ټکی_کلمه_سېټ.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_کلمه_سېټ.جمع_کړه(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_کلمه_سېټ.جمع_کړه(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_کلمه_سېټ.جمع_کړه(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_سېټ.compact();
    f_پای_ټکی_کلمه_سېټ.compact();
    f_Begin_کلمه_سېټ.compact();
    UTRACE_EXIT_حالت(حالت);
}

LaoBreakEngine::~LaoBreakEngine() {
    ړنګول fDictionary;
}

int32_t
LaoBreakEngine::divide_پورته_Dictionary_Range( UText *متن,
                                                int32_t range_پیل,
                                                int32_t range_پای_ټکی,
                                                UVector32 &وموندل_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_پای_ټکی - range_پیل) < LAO_MIN_کلمه_پراختیا) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t ټکي_وموندل = 0;
    int32_t cp_کلمه_اوږدتیا = 0;
    int32_t cu_کلمه_اوږدتیا = 0;
    int32_t current;
    Possible_کلمه ټکي[LAO_LOOKAHEAD];

    utext_سېټ_Native_شاخص(متن, range_پیل);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_پای_ټکی) {
        cu_کلمه_اوږدتیا = 0;
        cp_کلمه_اوږدتیا = 0;

        // Look for candidate words at the current position
        int32_t candidates = ټکي[ټکي_وموندل%LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_کلمه_اوږدتیا = ټکي[ټکي_وموندل % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_اوږدتیا = ټکي[ټکي_وموندل % LAO_LOOKAHEAD].markedCPLength();
            ټکي_وموندل += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (utext_get_Native_شاخص(متن) >= range_پای_ټکی) {
                اذهب وموندل_غوره;
            }
            افعل {
                إذا (ټکي[(ټکي_وموندل + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ټکي[ټکي_وموندل%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_پای_ټکی) {
                        اذهب وموندل_غوره;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (ټکي[(ټکي_وموندل + 2) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی)) {
                            ټکي[ټکي_وموندل % LAO_LOOKAHEAD].markCurrent();
                            اذهب وموندل_غوره;
                        }
                    }
                    طالما (ټکي[(ټکي_وموندل + 1) % LAO_LOOKAHEAD].شا_پورته(متن));
                }
            }
            طالما (ټکي[ټکي_وموندل % LAO_LOOKAHEAD].شا_پورته(متن));
وموندل_غوره:
            cu_کلمه_اوږدتیا = ټکي[ټکي_وموندل % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_اوږدتیا = ټکي[ټکي_وموندل % LAO_LOOKAHEAD].markedCPLength();
            ټکي_وموندل += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_پای_ټکی && cp_کلمه_اوږدتیا < LAO_ریښه_COMBINE_پوله) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (ټکي[ټکي_وموندل % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) <= 0
                  && (cu_کلمه_اوږدتیا == 0
                      || ټکي[ټکي_وموندل%LAO_LOOKAHEAD].longest_مختاړی() < LAO_مختاړی_COMBINE_پوله)) {
                // Look for a plausible word boundary
                int32_t پاتې_شوی = range_پای_ټکی - (current + cu_کلمه_اوږدتیا);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                    pc = utext_next32(متن);
                    int32_t pc_اندازه = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - pc_شاخص;
                    chars += pc_اندازه;
                    پاتې_شوی -= pc_اندازه;
                    إذا (پاتې_شوی <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_پای_ټکی_کلمه_سېټ.لري(pc) && f_Begin_کلمه_سېټ.لري(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t عدد_candidates = ټکي[(ټکي_وموندل + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی);
                        utext_سېټ_Native_شاخص(متن, current + cu_کلمه_اوږدتیا + chars);
                        إذا (عدد_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_کلمه_اوږدتیا <= 0) {
                    ټکي_وموندل += 1;
                }
                
                // Update the length with the passed-over characters
                cu_کلمه_اوږدتیا += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سېټ_Native_شاخص(متن, current + cu_کلمه_اوږدتیا);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_پای_ټکی && f_Mark_سېټ.لري(utext_current32(متن))) {
            utext_next32(متن);
            cu_کلمه_اوږدتیا += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_کلمه_اوږدتیا > 0) {
            وموندل_Breaks.push((current+cu_کلمه_اوږدتیا), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (وموندل_Breaks.peeki() >= range_پای_ټکی) {
        (فراغ) وموندل_Breaks.popi();
        ټکي_وموندل -= 1;
    }

    أرجع ټکي_وموندل;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t BURMESE_ریښه_COMBINE_پوله = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t BURMESE_مختاړی_COMBINE_پوله = 3;

// Minimum word size
ساكن ثابت int32_t BURMESE_MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t BURMESE_MIN_کلمه_پراختیا = BURMESE_MIN_کلمه * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوډ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ثبت(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_کلمه_سېټ.جمع_کړه(0x1000, 0x102A);      // basic consonants and independent vowels
    f_پای_ټکی_کلمه_سېټ.apply_نمونه(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), حالت);
    f_Mark_سېټ.apply_نمونه(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سېټ.جمع_کړه(0x0020);
    إذا (U_SUCCESS(حالت)) {
        سېټ_Characters(f_پای_ټکی_کلمه_سېټ);
    }

    // Compact for caching.
    f_Mark_سېټ.compact();
    f_پای_ټکی_کلمه_سېټ.compact();
    f_Begin_کلمه_سېټ.compact();
    UTRACE_EXIT_حالت(حالت);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    ړنګول fDictionary;
}

int32_t
BurmeseBreakEngine::divide_پورته_Dictionary_Range( UText *متن,
                                                int32_t range_پیل,
                                                int32_t range_پای_ټکی,
                                                UVector32 &وموندل_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوډ& حالت ) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_پای_ټکی - range_پیل) < BURMESE_MIN_کلمه_پراختیا) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t ټکي_وموندل = 0;
    int32_t cp_کلمه_اوږدتیا = 0;
    int32_t cu_کلمه_اوږدتیا = 0;
    int32_t current;
    Possible_کلمه ټکي[BURMESE_LOOKAHEAD];

    utext_سېټ_Native_شاخص(متن, range_پیل);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_پای_ټکی) {
        cu_کلمه_اوږدتیا = 0;
        cp_کلمه_اوږدتیا = 0;

        // Look for candidate words at the current position
        int32_t candidates = ټکي[ټکي_وموندل%BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_کلمه_اوږدتیا = ټکي[ټکي_وموندل % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_اوږدتیا = ټکي[ټکي_وموندل % BURMESE_LOOKAHEAD].markedCPLength();
            ټکي_وموندل += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (utext_get_Native_شاخص(متن) >= range_پای_ټکی) {
                اذهب وموندل_غوره;
            }
            افعل {
                إذا (ټکي[(ټکي_وموندل + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ټکي[ټکي_وموندل%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_پای_ټکی) {
                        اذهب وموندل_غوره;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (ټکي[(ټکي_وموندل + 2) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی)) {
                            ټکي[ټکي_وموندل % BURMESE_LOOKAHEAD].markCurrent();
                            اذهب وموندل_غوره;
                        }
                    }
                    طالما (ټکي[(ټکي_وموندل + 1) % BURMESE_LOOKAHEAD].شا_پورته(متن));
                }
            }
            طالما (ټکي[ټکي_وموندل % BURMESE_LOOKAHEAD].شا_پورته(متن));
وموندل_غوره:
            cu_کلمه_اوږدتیا = ټکي[ټکي_وموندل % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_اوږدتیا = ټکي[ټکي_وموندل % BURMESE_LOOKAHEAD].markedCPLength();
            ټکي_وموندل += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_پای_ټکی && cp_کلمه_اوږدتیا < BURMESE_ریښه_COMBINE_پوله) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (ټکي[ټکي_وموندل % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) <= 0
                  && (cu_کلمه_اوږدتیا == 0
                      || ټکي[ټکي_وموندل%BURMESE_LOOKAHEAD].longest_مختاړی() < BURMESE_مختاړی_COMBINE_پوله)) {
                // Look for a plausible word boundary
                int32_t پاتې_شوی = range_پای_ټکی - (current + cu_کلمه_اوږدتیا);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                    pc = utext_next32(متن);
                    int32_t pc_اندازه = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - pc_شاخص;
                    chars += pc_اندازه;
                    پاتې_شوی -= pc_اندازه;
                    إذا (پاتې_شوی <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_پای_ټکی_کلمه_سېټ.لري(pc) && f_Begin_کلمه_سېټ.لري(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t عدد_candidates = ټکي[(ټکي_وموندل + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی);
                        utext_سېټ_Native_شاخص(متن, current + cu_کلمه_اوږدتیا + chars);
                        إذا (عدد_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_کلمه_اوږدتیا <= 0) {
                    ټکي_وموندل += 1;
                }
                
                // Update the length with the passed-over characters
                cu_کلمه_اوږدتیا += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سېټ_Native_شاخص(متن, current + cu_کلمه_اوږدتیا);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_پای_ټکی && f_Mark_سېټ.لري(utext_current32(متن))) {
            utext_next32(متن);
            cu_کلمه_اوږدتیا += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_کلمه_اوږدتیا > 0) {
            وموندل_Breaks.push((current+cu_کلمه_اوږدتیا), حالت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (وموندل_Breaks.peeki() >= range_پای_ټکی) {
        (فراغ) وموندل_Breaks.popi();
        ټکي_وموندل -= 1;
    }

    أرجع ټکي_وموندل;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t KHMER_ریښه_COMBINE_پوله = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t KHMER_مختاړی_COMBINE_پوله = 3;

// Minimum word size
ساكن ثابت int32_t KHMER_MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t KHMER_MIN_کلمه_پراختیا = KHMER_MIN_کلمه * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوډ &حالت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ثبت(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_سېټ khmer_کلمه_سېټ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), حالت);
    إذا (U_SUCCESS(حالت)) {
        سېټ_Characters(khmer_کلمه_سېټ);
    }
    f_Mark_سېټ.apply_نمونه(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), حالت);
    f_Mark_سېټ.جمع_کړه(0x0020);
    f_پای_ټکی_کلمه_سېټ = khmer_کلمه_سېټ;
    f_Begin_کلمه_سېټ.جمع_کړه(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_پای_ټکی_کلمه_سېټ.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_سېټ.compact();
    f_پای_ټکی_کلمه_سېټ.compact();
    f_Begin_کلمه_سېټ.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_حالت(حالت);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    ړنګول fDictionary;
}

int32_t
KhmerBreakEngine::divide_پورته_Dictionary_Range( UText *متن,
                                                int32_t range_پیل,
                                                int32_t range_پای_ټکی,
                                                UVector32 &وموندل_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوډ& حالت ) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا ((range_پای_ټکی - range_پیل) < KHMER_MIN_کلمه_پراختیا) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t ټکي_وموندل = 0;
    int32_t cp_کلمه_اوږدتیا = 0;
    int32_t cu_کلمه_اوږدتیا = 0;
    int32_t current;
    Possible_کلمه ټکي[KHMER_LOOKAHEAD];

    utext_سېټ_Native_شاخص(متن, range_پیل);

    طالما (U_SUCCESS(حالت) && (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_پای_ټکی) {
        cu_کلمه_اوږدتیا = 0;
        cp_کلمه_اوږدتیا = 0;

        // Look for candidate words at the current position
        int32_t candidates = ټکي[ټکي_وموندل%KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی);

        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_کلمه_اوږدتیا = ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_اوږدتیا = ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].markedCPLength();
            ټکي_وموندل += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_پای_ټکی) {
                اذهب وموندل_غوره;
            }
            افعل {
                إذا (ټکي[(ټکي_وموندل + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_پای_ټکی) {
                        اذهب وموندل_غوره;
                    }

                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (ټکي[(ټکي_وموندل + 2) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی)) {
                            ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].markCurrent();
                            اذهب وموندل_غوره;
                        }
                    }
                    طالما (ټکي[(ټکي_وموندل + 1) % KHMER_LOOKAHEAD].شا_پورته(متن));
                }
            }
            طالما (ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].شا_پورته(متن));
وموندل_غوره:
            cu_کلمه_اوږدتیا = ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_اوږدتیا = ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].markedCPLength();
            ټکي_وموندل += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_پای_ټکی && cp_کلمه_اوږدتیا < KHMER_ریښه_COMBINE_پوله) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی) <= 0
                  && (cu_کلمه_اوږدتیا == 0
                      || ټکي[ټکي_وموندل % KHMER_LOOKAHEAD].longest_مختاړی() < KHMER_مختاړی_COMBINE_پوله)) {
                // Look for a plausible word boundary
                int32_t پاتې_شوی = range_پای_ټکی - (current+cu_کلمه_اوږدتیا);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                    pc = utext_next32(متن);
                    int32_t pc_اندازه = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - pc_شاخص;
                    chars += pc_اندازه;
                    پاتې_شوی -= pc_اندازه;
                    إذا (پاتې_شوی <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_پای_ټکی_کلمه_سېټ.لري(pc) && f_Begin_کلمه_سېټ.لري(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t عدد_candidates = ټکي[(ټکي_وموندل + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_پای_ټکی);
                        utext_سېټ_Native_شاخص(متن, current+cu_کلمه_اوږدتیا+chars);
                        إذا (عدد_candidates > 0) {
                            اكسر;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                إذا (cu_کلمه_اوږدتیا <= 0) {
                    ټکي_وموندل += 1;
                }

                // Update the length with the passed-over characters
                cu_کلمه_اوږدتیا += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_سېټ_Native_شاخص(متن, current+cu_کلمه_اوږدتیا);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_پای_ټکی && f_Mark_سېټ.لري(utext_current32(متن))) {
            utext_next32(متن);
            cu_کلمه_اوږدتیا += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - currPos;
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
        إذا (cu_کلمه_اوږدتیا > 0) {
            وموندل_Breaks.push((current+cu_کلمه_اوږدتیا), حالت);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (وموندل_Breaks.peeki() >= range_پای_ټکی) {
        (فراغ) وموندل_Breaks.popi();
        ټکي_وموندل -= 1;
    }

    أرجع ټکي_وموندل;
}

#إذا !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
ساكن ثابت uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_کوډ &حالت)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(خطأ) {
    UTRACE_ثبت(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = تش_اشاره;
    nfkcNorm2 = Normalizer2::getNFKCInstance(حالت);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_کلمه_سېټ.apply_نمونه(UnicodeString(u"[\\uac00-\\ud7a3]"), حالت);
    f_Hangul_کلمه_سېټ.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_سېټ.apply_نمونه(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), حالت);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_سېټ.compact();
    f_Close_Punctuation_سېټ.apply_نمونه(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), حالت);
    f_Close_Punctuation_سېټ.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    إذا (type == kKorean) {
        إذا (U_SUCCESS(حالت)) {
            سېټ_Characters(f_Hangul_کلمه_سېټ);
        }
    } وإلا { // Chinese and Japanese
        Unicode_سېټ cj_سېټ(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), حالت);
        isCj = ریښتیا;
        إذا (U_SUCCESS(حالت)) {
            سېټ_Characters(cj_سېټ);
#إذا UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = جديد MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_سېټ,
                                               f_Close_Punctuation_سېټ, حالت);
            إذا (fMlBreakEngine == تش_اشاره) {
                حالت = U_حافظه_ویش_ERROR;
            }
#وإلا
            initJapanesePhraseParameter(حالت);
#د_که_پای
        }
    }
    UTRACE_EXIT_حالت(حالت);
}

CjkBreakEngine::~CjkBreakEngine(){
    ړنګول fDictionary;
    ړنګول fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
ساكن ثابت int32_t k_Max_Katakana_اوږدتیا = 8;
ساكن ثابت int32_t k_Max_Katakana_ډله_اوږدتیا = 20;
ساكن ثابت uint32_t maxSnlp = 255;

ساكن مضمن uint32_t get_Katakana_لګښت(int32_t کلمه_اوږدتیا){
    //TODO: fill array with actual values from dictionary!
    ساكن ثابت uint32_t katakana_لګښت[k_Max_Katakana_اوږدتیا + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    أرجع (کلمه_اوږدتیا > k_Max_Katakana_اوږدتیا) ? 8192 : katakana_لګښت[کلمه_اوږدتیا];
}

ساكن مضمن منطقي isKatakana(UChar32 value) {
    أرجع (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

ساكن مضمن int32_t utext_i32_flag(int32_t بټ_شاخص) {
    أرجع static_cast<int32_t>(1) << بټ_شاخص;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_پورته_Dictionary_Range( UText *دننه_متن,
        int32_t range_پیل,
        int32_t range_پای_ټکی,
        UVector32 &وموندل_Breaks,
        UBool isPhraseBreaking,
        UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    إذا (range_پیل >= range_پای_ټکی) {
        أرجع 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString دننه_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    Local_اشاره_ګر<UVector32>     ننوت_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    إذا ((دننه_متن->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         دننه_متن->chunk_Native_پیل <= range_پیل &&
         دننه_متن->chunk_Native_حد >= range_پای_ټکی   &&
         دننه_متن->native_Indexing_حد >= range_پای_ټکی - دننه_متن->chunk_Native_پیل) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        دننه_String.سېټ_منزل(خطأ,
                       دننه_متن->chunkContents + range_پیل - دننه_متن->chunk_Native_پیل,
                       range_پای_ټکی - range_پیل);
    } وإلا {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_سېټ_Native_شاخص(دننه_متن, range_پیل);
        int32_t حد = range_پای_ټکی;
        U_ASSERT(حد <= utext_native_اوږدتیا(دننه_متن));
        إذا (حد > utext_native_اوږدتیا(دننه_متن)) {
            حد = static_cast<int32_t>(utext_native_اوږدتیا(دننه_متن));
        }
        ننوت_Map.adopt_Instead_And_Check_Error_کوډ(جديد UVector32(حالت), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع 0;
        }
        طالما (utext_get_Native_شاخص(دننه_متن) < حد) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_شاخص(دننه_متن));
            UChar32 c = utext_next32(دننه_متن);
            U_ASSERT(c != U_SENTINEL);
            دننه_String.append(c);
            طالما (ننوت_Map->size() < دننه_String.length()) {
                ننوت_Map->جمع_کړه_Element(nativePosition, حالت);
            }
        }
        ننوت_Map->جمع_کړه_Element(حد, حالت);
    }


    إذا (!nfkcNorm2->isNormalized(دننه_String, حالت)) {
        UnicodeString normalized_ننوت;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        Local_اشاره_ګر<UVector32> normalizedMap(جديد UVector32(حالت), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        لكل (int32_t srcI = 0; srcI < دننه_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragment_پیل_I = srcI;
            UChar32 c = دننه_String.char32At(srcI);
            لكل (;;) {
                fragment.append(c);
                srcI = دننه_String.moveIndex32(srcI, 1);
                إذا (srcI == دننه_String.length()) {
                    اكسر;
                }
                c = دننه_String.char32At(srcI);
                إذا (nfkcNorm2->hasBoundaryBefore(c)) {
                    اكسر;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, حالت);
            normalized_ننوت.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragment_Original_پیل = ننوت_Map.isValid() ?
                    ننوت_Map->elementAti(fragment_پیل_I) : fragment_پیل_I+range_پیل;
            طالما (normalizedMap->size() < normalized_ننوت.length()) {
                normalizedMap->جمع_کړه_Element(fragment_Original_پیل, حالت);
                إذا (U_FAILURE(حالت)) {
                    اكسر;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_ننوت.length());
        int32_t native_پای_ټکی = ننوت_Map.isValid() ?
                ننوت_Map->elementAti(دننه_String.length()) : دننه_String.length()+range_پیل;
        normalizedMap->جمع_کړه_Element(native_پای_ټکی, حالت);

        ننوت_Map = std::move(normalizedMap);
        دننه_String = std::move(normalized_ننوت);
    }

    int32_t عدد_کوډ_Pts = دننه_String.شمېر_Char32();
    إذا (عدد_کوډ_Pts != دننه_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = ننوت_Map.isValid();
        إذا (!hadExistingMap) {
            ننوت_Map.adopt_Instead_And_Check_Error_کوډ(جديد UVector32(حالت), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع 0;
            }
        }
        int32_t cpIdx = 0;
        لكل (int32_t cuIdx = 0; ; cuIdx = دننه_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            إذا (hadExistingMap) {
                ننوت_Map->سېټ_Element_At(ننوت_Map->elementAti(cuIdx), cpIdx);
            } وإلا {
                ننوت_Map->جمع_کړه_Element(cuIdx+range_پیل, حالت);
            }
            cpIdx++;
            إذا (cuIdx == دننه_String.length()) {
               اكسر;
            }
        }
    }

#إذا UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    إذا (isPhraseBreaking && isCj) {
        أرجع fMlBreakEngine->divide_پورته_Range(دننه_متن, range_پیل, range_پای_ټکی, وموندل_Breaks, دننه_String,
                                             ننوت_Map, حالت);
    }
#د_که_پای

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 غوره_Snlp(عدد_کوډ_Pts + 1, حالت);
    غوره_Snlp.جمع_کړه_Element(0, حالت);
    لكل(int32_t i = 1; i <= عدد_کوډ_Pts; i++) {
        غوره_Snlp.جمع_کړه_Element(kuint32max, حالت);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 prev(عدد_کوډ_Pts + 1, حالت);
    لكل(int32_t i = 0; i <= عدد_کوډ_Pts; i++){
        prev.جمع_کړه_Element(-1, حالت);
    }

    ثابت int32_t max_کلمه_اندازه = 20;
    UVector32 ارزښتونه(عدد_کوډ_Pts, حالت);
    ارزښتونه.سېټ_اندازه(عدد_کوډ_Pts);
    UVector32 lengths(عدد_کوډ_Pts, حالت);
    lengths.سېټ_اندازه(عدد_کوډ_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &دننه_String, &حالت);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    منطقي is_prev_katakana = خطأ;
    لكل (int32_t i = 0;  i < عدد_کوډ_Pts;  ++i, ix = دننه_String.moveIndex32(ix, 1)) {
        إذا (static_cast<uint32_t>(غوره_Snlp.elementAti(i)) == kuint32max) {
            استمر;
        }

        int32_t count;
        utext_سېټ_Native_شاخص(&fu, ix);
        count = fDictionary->matches(&fu, max_کلمه_اندازه, عدد_کوډ_Pts,
                             تش_اشاره, lengths.get_بفر(), ارزښتونه.get_بفر(), تش_اشاره);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        إذا ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_کلمه_سېټ.لري(دننه_String.char32At(ix))) {
            ارزښتونه.سېټ_Element_At(maxSnlp, count);   // 255
            lengths.سېټ_Element_At(1, count++);
        }

        لكل (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(غوره_Snlp.elementAti(i)) + static_cast<uint32_t>(ارزښتونه.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            إذا (newSnlp < static_cast<uint32_t>(غوره_Snlp.elementAti(ln_j_i))) {
                غوره_Snlp.سېټ_Element_At(newSnlp, ln_j_i);
                prev.سېټ_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        منطقي is_katakana = isKatakana(دننه_String.char32At(ix));
        int32_t katakana_پرلپسې_اوږدتیا = 1;
        إذا (!is_prev_katakana && is_katakana) {
            int32_t j = دننه_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            طالما (j < دننه_String.length() && katakana_پرلپسې_اوږدتیا < k_Max_Katakana_ډله_اوږدتیا &&
                    isKatakana(دننه_String.char32At(j))) {
                j = دننه_String.moveIndex32(j, 1);
                katakana_پرلپسې_اوږدتیا++;
            }
            إذا (katakana_پرلپسې_اوږدتیا < k_Max_Katakana_ډله_اوږدتیا) {
                uint32_t newSnlp = غوره_Snlp.elementAti(i) + get_Katakana_لګښت(katakana_پرلپسې_اوږدتیا);
                إذا (newSnlp < static_cast<uint32_t>(غوره_Snlp.elementAti(i + katakana_پرلپسې_اوږدتیا))) {
                    غوره_Snlp.سېټ_Element_At(newSnlp, i+katakana_پرلپسې_اوږدتیا);
                    prev.سېټ_Element_At(i, i+katakana_پرلپسې_اوږدتیا);  // prev[j] = i;
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
    UVector32 t_boundary(عدد_کوډ_Pts+1, حالت);

    int32_t عدد_Breaks = 0;
    // No segmentation found, set boundary to end of range
    إذا (static_cast<uint32_t>(غوره_Snlp.elementAti(عدد_کوډ_Pts)) == kuint32max) {
        t_boundary.جمع_کړه_Element(عدد_کوډ_Pts, حالت);
        عدد_Breaks++;
    } وإلا إذا (isPhraseBreaking) {
        t_boundary.جمع_کړه_Element(عدد_کوډ_Pts, حالت);
        إذا(U_SUCCESS(حالت)) {
            عدد_Breaks++;
            int32_t prevIdx = عدد_کوډ_Pts;

            int32_t کوډ_Unit_Idx = -1;
            int32_t prev_کوډ_Unit_Idx = -1;
            int32_t length = -1;
            لكل (int32_t i = prev.elementAti(عدد_کوډ_Pts); i > 0; i = prev.elementAti(i)) {
                کوډ_Unit_Idx = دننه_String.moveIndex32(0, i);
                prev_کوډ_Unit_Idx = دننه_String.moveIndex32(0, prevIdx);
                // Calculate the length by using the code unit.
                length = prev_کوډ_Unit_Idx - کوډ_Unit_Idx;
                prevIdx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                إذا (!f_Skip_سېټ.لري_کلی(دننه_String.لنډمهاله_Sub_String(کوډ_Unit_Idx, length))
                    && (!isKatakana(دننه_String.char32At(دننه_String.moveIndex32(کوډ_Unit_Idx, -1)))
                           || !isKatakana(دننه_String.char32At(کوډ_Unit_Idx)))) {
                    t_boundary.جمع_کړه_Element(i, حالت);
                    عدد_Breaks++;
                }
            }
        }
    } وإلا {
        لكل (int32_t i = عدد_کوډ_Pts; i > 0; i = prev.elementAti(i)) {
            t_boundary.جمع_کړه_Element(i, حالت);
            عدد_Breaks++;
        }
        U_ASSERT(prev.elementAti(t_boundary.elementAti(عدد_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    إذا (وموندل_Breaks.size() == 0 || وموندل_Breaks.peeki() < range_پیل) {
        t_boundary.جمع_کړه_Element(0, حالت);
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
        int32_t utextPos =  ننوت_Map.isValid() ? ننوت_Map->elementAti(cpPos) : cpPos + range_پیل;
        U_ASSERT(utextPos >= prevUTextPos);
        إذا (utextPos > prevUTextPos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(وموندل_Breaks.size() == 0 || وموندل_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            إذا (utextPos != range_پیل
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_سېټ.لري(utext_char32At(دننه_متن, utextPos - 1)))) {
                وموندل_Breaks.push(utextPos, حالت);
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

    UChar32 بل_Char = utext_char32At(دننه_متن, range_پای_ټکی);
    إذا (!وموندل_Breaks.is_تشه() && وموندل_Breaks.peeki() == range_پای_ټکی) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (isPhraseBreaking) {
            إذا (!f_Digit_Or_Open_Punctuation_Or_Alphabet_سېټ.لري(بل_Char)) {
                وموندل_Breaks.popi();
                corrected_عدد_Breaks--;
            }
        } وإلا {
            وموندل_Breaks.popi();
            corrected_عدد_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    أرجع corrected_عدد_Breaks;
}

فراغ CjkBreakEngine::initJapanesePhraseParameter(UError_کوډ& error) {
    بارول_Japanese_Extensions(error);
    بارول_Hiragana(error);
}

فراغ CjkBreakEngine::بارول_Japanese_Extensions(UError_کوډ& error) {
    ثابت حرف* نښان = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    إذا (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(نښان, error);
        طالما (U_SUCCESS(error) && bundle.has_بل()) {
            f_Skip_سېټ.puti(bundle.get_بل_String(error), 1, error);
        }
    }
}

فراغ CjkBreakEngine::بارول_Hiragana(UError_کوډ& error) {
    Unicode_سېټ hiragana_کلمه_سېټ(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_کلمه_سېټ.compact();
    Unicode_سېټ_Iterator iterator(hiragana_کلمه_سېټ);
    طالما (iterator.next()) {
        f_Skip_سېټ.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#د_که_پای

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */

