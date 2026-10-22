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
    أرجع f_مجموعه.شامل_است(c);
}

int32_t
DictionaryBreakEngine::پیدا_کن_Breaks( UText *متن,
                                 int32_t شروع_Pos,
                                 int32_t انتها_Pos,
                                 UVector32 &یافت_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    int32_t نتیجه = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_مجموعه_Native_شاخص(متن, شروع_Pos);
    int32_t شروع = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    int32_t current;
    int32_t range_شروع;
    int32_t range_انتها;
    UChar32 c = utext_current32(متن);
    طالما ((current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < انتها_Pos && f_مجموعه.شامل_است(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_شروع = شروع;
    range_انتها = current;
    نتیجه = divide_به_بالا_Dictionary_Range(متن, range_شروع, range_انتها, یافت_Breaks, isPhraseBreaking, وضعیت);
    utext_مجموعه_Native_شاخص(متن, current);
    
    أرجع نتیجه;
}

فراغ
DictionaryBreakEngine::مجموعه_Characters( ثابت Unicode_مجموعه &مجموعه ) {
    f_مجموعه = مجموعه;
    // Compact for caching
    f_مجموعه.compact();
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
    int32_t   پیشوند;     // The longest match with a dictionary word
    int32_t   جابه_جایی;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_کلمه_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_کلمه_LIST_MAX];   // Word Lengths, in code points.

عام:
    Possible_کلمه() : count(0), پیشوند(0), جابه_جایی(-1), mark(0), current(0) {}
    ~Possible_کلمه() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *متن, DictionaryMatcher *dict, int32_t range_انتها );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *متن );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     پشت_به_بالا( UText *متن );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_پیشوند() { أرجع پیشوند; }
  
    // Mark the current candidate as the one we like
    فراغ      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { أرجع cpLengths[mark]; }
};


int32_t Possible_کلمه::candidates( UText *متن, DictionaryMatcher *dict, int32_t range_انتها ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t شروع = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    إذا (شروع != جابه_جایی) {
        جابه_جایی = شروع;
        count = dict->matches(متن, range_انتها-شروع, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, اشاره_تهی, &پیشوند);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        إذا (count <= 0) {
            utext_مجموعه_Native_شاخص(متن, شروع);
        }
    }
    إذا (count > 0) {
        utext_مجموعه_Native_شاخص(متن, شروع+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    أرجع count;
}

int32_t
Possible_کلمه::acceptMarked( UText *متن ) {
    utext_مجموعه_Native_شاخص(متن, جابه_جایی + cuLengths[mark]);
    أرجع cuLengths[mark];
}


UBool
Possible_کلمه::پشت_به_بالا( UText *متن ) {
    إذا (current > 0) {
        utext_مجموعه_Native_شاخص(متن, جابه_جایی + cuLengths[--current]);
        أرجع درست;
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
ساكن ثابت int32_t THAI_ریشه_COMBINE_آستانه = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t THAI_پیشوند_COMBINE_آستانه = 3;

// Elision character
ساكن ثابت int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
ساكن ثابت int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
ساكن ثابت int32_t THAI_MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t THAI_MIN_کلمه_بازه = THAI_MIN_کلمه * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_کد &وضعیت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_مدخل(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_مجموعه thai_کلمه_مجموعه(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        مجموعه_Characters(thai_کلمه_مجموعه);
    }
    f_Mark_مجموعه.apply_الگو(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), وضعیت);
    f_Mark_مجموعه.جمع_کن(0x0020);
    f_انتها_کلمه_مجموعه = thai_کلمه_مجموعه;
    f_انتها_کلمه_مجموعه.remove(0x0E31);             // MAI HAN-AKAT
    f_انتها_کلمه_مجموعه.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_کلمه_مجموعه.جمع_کن(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_کلمه_مجموعه.جمع_کن(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_مجموعه.جمع_کن(THAI_PAIYANNOI);
    f_Suffix_مجموعه.جمع_کن(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_مجموعه.compact();
    f_انتها_کلمه_مجموعه.compact();
    f_Begin_کلمه_مجموعه.compact();
    f_Suffix_مجموعه.compact();
    UTRACE_EXIT_وضعیت(وضعیت);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    حذف fDictionary;
}

int32_t
ThaiBreakEngine::divide_به_بالا_Dictionary_Range( UText *متن,
                                                int32_t range_شروع,
                                                int32_t range_انتها,
                                                UVector32 &یافت_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    utext_مجموعه_Native_شاخص(متن, range_شروع);
    utext_moveIndex32(متن, THAI_MIN_کلمه_بازه);
    إذا (utext_get_Native_شاخص(متن) >= range_انتها) {
        أرجع 0;       // Not enough characters for two words
    }
    utext_مجموعه_Native_شاخص(متن, range_شروع);


    uint32_t واژگان_یافت = 0;
    int32_t cp_کلمه_درازی = 0;    // Word Length in Code Points.
    int32_t cu_کلمه_درازی = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    Possible_کلمه واژگان[THAI_LOOKAHEAD];
    
    utext_مجموعه_Native_شاخص(متن, range_شروع);
    
    طالما (U_SUCCESS(وضعیت) && (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_انتها) {
        cp_کلمه_درازی = 0;
        cu_کلمه_درازی = 0;

        // Look for candidate words at the current position
        int32_t candidates = واژگان[واژگان_یافت%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتها);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_کلمه_درازی = واژگان[واژگان_یافت % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_درازی = واژگان[واژگان_یافت % THAI_LOOKAHEAD].markedCPLength();
            واژگان_یافت += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_انتها) {
                اذهب یافت_بهترین;
            }
            افعل {
                إذا (واژگان[(واژگان_یافت + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    واژگان[واژگان_یافت%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_انتها) {
                        اذهب یافت_بهترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (واژگان[(واژگان_یافت + 2) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتها)) {
                            واژگان[واژگان_یافت % THAI_LOOKAHEAD].markCurrent();
                            اذهب یافت_بهترین;
                        }
                    }
                    طالما (واژگان[(واژگان_یافت + 1) % THAI_LOOKAHEAD].پشت_به_بالا(متن));
                }
            }
            طالما (واژگان[واژگان_یافت % THAI_LOOKAHEAD].پشت_به_بالا(متن));
یافت_بهترین:
            // Set UText position to after the accepted word.
            cu_کلمه_درازی = واژگان[واژگان_یافت % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_درازی = واژگان[واژگان_یافت % THAI_LOOKAHEAD].markedCPLength();
            واژگان_یافت += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_انتها && cp_کلمه_درازی < THAI_ریشه_COMBINE_آستانه) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (واژگان[واژگان_یافت % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) <= 0
                  && (cu_کلمه_درازی == 0
                      || واژگان[واژگان_یافت%THAI_LOOKAHEAD].longest_پیشوند() < THAI_پیشوند_COMBINE_آستانه)) {
                // Look for a plausible word boundary
                int32_t مانده = range_انتها - (current+cu_کلمه_درازی);
                UChar32 pc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                    pc = utext_next32(متن);
                    int32_t pc_اندازه = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - pc_شاخص;
                    chars += pc_اندازه;
                    مانده -= pc_اندازه;
                    إذا (مانده <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_انتها_کلمه_مجموعه.شامل_است(pc) && f_Begin_کلمه_مجموعه.شامل_است(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t رقم_candidates = واژگان[(واژگان_یافت + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتها);
                        utext_مجموعه_Native_شاخص(متن, current + cu_کلمه_درازی + chars);
                        إذا (رقم_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_کلمه_درازی <= 0) {
                    واژگان_یافت += 1;
                }
                
                // Update the length with the passed-over characters
                cu_کلمه_درازی += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_مجموعه_Native_شاخص(متن, current+cu_کلمه_درازی);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_انتها && f_Mark_مجموعه.شامل_است(utext_current32(متن))) {
            utext_next32(متن);
            cu_کلمه_درازی += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_انتها && cu_کلمه_درازی > 0) {
            إذا (واژگان[واژگان_یافت%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) <= 0
                && f_Suffix_مجموعه.شامل_است(uc = utext_current32(متن))) {
                إذا (uc == THAI_PAIYANNOI) {
                    إذا (!f_Suffix_مجموعه.شامل_است(utext_previous32(متن))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(متن);
                        int32_t paiyannoi_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                        utext_next32(متن);
                        cu_کلمه_درازی += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - paiyannoi_شاخص; // Add PAIYANNOI to word
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
                        cu_کلمه_درازی += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - maiyamok_شاخص; // Add MAIYAMOK to word
                    }
                    وإلا {
                        // Restore prior position
                        utext_next32(متن);
                    }
                }
            }
            وإلا {
                utext_مجموعه_Native_شاخص(متن, current+cu_کلمه_درازی);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_کلمه_درازی > 0) {
            یافت_Breaks.push((current+cu_کلمه_درازی), وضعیت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (یافت_Breaks.peeki() >= range_انتها) {
        (فراغ) یافت_Breaks.popi();
        واژگان_یافت -= 1;
    }

    أرجع واژگان_یافت;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t LAO_ریشه_COMBINE_آستانه = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t LAO_پیشوند_COMBINE_آستانه = 3;

// Minimum word size
ساكن ثابت int32_t LAO_MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t LAO_MIN_کلمه_بازه = LAO_MIN_کلمه * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_کد &وضعیت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_مدخل(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_مجموعه lao_کلمه_مجموعه(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        مجموعه_Characters(lao_کلمه_مجموعه);
    }
    f_Mark_مجموعه.apply_الگو(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), وضعیت);
    f_Mark_مجموعه.جمع_کن(0x0020);
    f_انتها_کلمه_مجموعه = lao_کلمه_مجموعه;
    f_انتها_کلمه_مجموعه.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_کلمه_مجموعه.جمع_کن(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_کلمه_مجموعه.جمع_کن(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_کلمه_مجموعه.جمع_کن(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_مجموعه.compact();
    f_انتها_کلمه_مجموعه.compact();
    f_Begin_کلمه_مجموعه.compact();
    UTRACE_EXIT_وضعیت(وضعیت);
}

LaoBreakEngine::~LaoBreakEngine() {
    حذف fDictionary;
}

int32_t
LaoBreakEngine::divide_به_بالا_Dictionary_Range( UText *متن,
                                                int32_t range_شروع,
                                                int32_t range_انتها,
                                                UVector32 &یافت_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    إذا ((range_انتها - range_شروع) < LAO_MIN_کلمه_بازه) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t واژگان_یافت = 0;
    int32_t cp_کلمه_درازی = 0;
    int32_t cu_کلمه_درازی = 0;
    int32_t current;
    Possible_کلمه واژگان[LAO_LOOKAHEAD];

    utext_مجموعه_Native_شاخص(متن, range_شروع);

    طالما (U_SUCCESS(وضعیت) && (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_انتها) {
        cu_کلمه_درازی = 0;
        cp_کلمه_درازی = 0;

        // Look for candidate words at the current position
        int32_t candidates = واژگان[واژگان_یافت%LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتها);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_کلمه_درازی = واژگان[واژگان_یافت % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_درازی = واژگان[واژگان_یافت % LAO_LOOKAHEAD].markedCPLength();
            واژگان_یافت += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (utext_get_Native_شاخص(متن) >= range_انتها) {
                اذهب یافت_بهترین;
            }
            افعل {
                إذا (واژگان[(واژگان_یافت + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    واژگان[واژگان_یافت%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_انتها) {
                        اذهب یافت_بهترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (واژگان[(واژگان_یافت + 2) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتها)) {
                            واژگان[واژگان_یافت % LAO_LOOKAHEAD].markCurrent();
                            اذهب یافت_بهترین;
                        }
                    }
                    طالما (واژگان[(واژگان_یافت + 1) % LAO_LOOKAHEAD].پشت_به_بالا(متن));
                }
            }
            طالما (واژگان[واژگان_یافت % LAO_LOOKAHEAD].پشت_به_بالا(متن));
یافت_بهترین:
            cu_کلمه_درازی = واژگان[واژگان_یافت % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_درازی = واژگان[واژگان_یافت % LAO_LOOKAHEAD].markedCPLength();
            واژگان_یافت += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_انتها && cp_کلمه_درازی < LAO_ریشه_COMBINE_آستانه) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (واژگان[واژگان_یافت % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) <= 0
                  && (cu_کلمه_درازی == 0
                      || واژگان[واژگان_یافت%LAO_LOOKAHEAD].longest_پیشوند() < LAO_پیشوند_COMBINE_آستانه)) {
                // Look for a plausible word boundary
                int32_t مانده = range_انتها - (current + cu_کلمه_درازی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                    pc = utext_next32(متن);
                    int32_t pc_اندازه = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - pc_شاخص;
                    chars += pc_اندازه;
                    مانده -= pc_اندازه;
                    إذا (مانده <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_انتها_کلمه_مجموعه.شامل_است(pc) && f_Begin_کلمه_مجموعه.شامل_است(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t رقم_candidates = واژگان[(واژگان_یافت + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتها);
                        utext_مجموعه_Native_شاخص(متن, current + cu_کلمه_درازی + chars);
                        إذا (رقم_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_کلمه_درازی <= 0) {
                    واژگان_یافت += 1;
                }
                
                // Update the length with the passed-over characters
                cu_کلمه_درازی += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_مجموعه_Native_شاخص(متن, current + cu_کلمه_درازی);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_انتها && f_Mark_مجموعه.شامل_است(utext_current32(متن))) {
            utext_next32(متن);
            cu_کلمه_درازی += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_کلمه_درازی > 0) {
            یافت_Breaks.push((current+cu_کلمه_درازی), وضعیت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (یافت_Breaks.peeki() >= range_انتها) {
        (فراغ) یافت_Breaks.popi();
        واژگان_یافت -= 1;
    }

    أرجع واژگان_یافت;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t BURMESE_ریشه_COMBINE_آستانه = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t BURMESE_پیشوند_COMBINE_آستانه = 3;

// Minimum word size
ساكن ثابت int32_t BURMESE_MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t BURMESE_MIN_کلمه_بازه = BURMESE_MIN_کلمه * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_کد &وضعیت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_مدخل(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_کلمه_مجموعه.جمع_کن(0x1000, 0x102A);      // basic consonants and independent vowels
    f_انتها_کلمه_مجموعه.apply_الگو(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), وضعیت);
    f_Mark_مجموعه.apply_الگو(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), وضعیت);
    f_Mark_مجموعه.جمع_کن(0x0020);
    إذا (U_SUCCESS(وضعیت)) {
        مجموعه_Characters(f_انتها_کلمه_مجموعه);
    }

    // Compact for caching.
    f_Mark_مجموعه.compact();
    f_انتها_کلمه_مجموعه.compact();
    f_Begin_کلمه_مجموعه.compact();
    UTRACE_EXIT_وضعیت(وضعیت);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    حذف fDictionary;
}

int32_t
BurmeseBreakEngine::divide_به_بالا_Dictionary_Range( UText *متن,
                                                int32_t range_شروع,
                                                int32_t range_انتها,
                                                UVector32 &یافت_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کد& وضعیت ) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    إذا ((range_انتها - range_شروع) < BURMESE_MIN_کلمه_بازه) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t واژگان_یافت = 0;
    int32_t cp_کلمه_درازی = 0;
    int32_t cu_کلمه_درازی = 0;
    int32_t current;
    Possible_کلمه واژگان[BURMESE_LOOKAHEAD];

    utext_مجموعه_Native_شاخص(متن, range_شروع);

    طالما (U_SUCCESS(وضعیت) && (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_انتها) {
        cu_کلمه_درازی = 0;
        cp_کلمه_درازی = 0;

        // Look for candidate words at the current position
        int32_t candidates = واژگان[واژگان_یافت%BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتها);
        
        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_کلمه_درازی = واژگان[واژگان_یافت % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_درازی = واژگان[واژگان_یافت % BURMESE_LOOKAHEAD].markedCPLength();
            واژگان_یافت += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (utext_get_Native_شاخص(متن) >= range_انتها) {
                اذهب یافت_بهترین;
            }
            افعل {
                إذا (واژگان[(واژگان_یافت + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    واژگان[واژگان_یافت%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_انتها) {
                        اذهب یافت_بهترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (واژگان[(واژگان_یافت + 2) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتها)) {
                            واژگان[واژگان_یافت % BURMESE_LOOKAHEAD].markCurrent();
                            اذهب یافت_بهترین;
                        }
                    }
                    طالما (واژگان[(واژگان_یافت + 1) % BURMESE_LOOKAHEAD].پشت_به_بالا(متن));
                }
            }
            طالما (واژگان[واژگان_یافت % BURMESE_LOOKAHEAD].پشت_به_بالا(متن));
یافت_بهترین:
            cu_کلمه_درازی = واژگان[واژگان_یافت % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_درازی = واژگان[واژگان_یافت % BURMESE_LOOKAHEAD].markedCPLength();
            واژگان_یافت += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_انتها && cp_کلمه_درازی < BURMESE_ریشه_COMBINE_آستانه) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (واژگان[واژگان_یافت % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) <= 0
                  && (cu_کلمه_درازی == 0
                      || واژگان[واژگان_یافت%BURMESE_LOOKAHEAD].longest_پیشوند() < BURMESE_پیشوند_COMBINE_آستانه)) {
                // Look for a plausible word boundary
                int32_t مانده = range_انتها - (current + cu_کلمه_درازی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                    pc = utext_next32(متن);
                    int32_t pc_اندازه = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - pc_شاخص;
                    chars += pc_اندازه;
                    مانده -= pc_اندازه;
                    إذا (مانده <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_انتها_کلمه_مجموعه.شامل_است(pc) && f_Begin_کلمه_مجموعه.شامل_است(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t رقم_candidates = واژگان[(واژگان_یافت + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتها);
                        utext_مجموعه_Native_شاخص(متن, current + cu_کلمه_درازی + chars);
                        إذا (رقم_candidates > 0) {
                            اكسر;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                إذا (cu_کلمه_درازی <= 0) {
                    واژگان_یافت += 1;
                }
                
                // Update the length with the passed-over characters
                cu_کلمه_درازی += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_مجموعه_Native_شاخص(متن, current + cu_کلمه_درازی);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_انتها && f_Mark_مجموعه.شامل_است(utext_current32(متن))) {
            utext_next32(متن);
            cu_کلمه_درازی += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        إذا (cu_کلمه_درازی > 0) {
            یافت_Breaks.push((current+cu_کلمه_درازی), وضعیت);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (یافت_Breaks.peeki() >= range_انتها) {
        (فراغ) یافت_Breaks.popi();
        واژگان_یافت -= 1;
    }

    أرجع واژگان_یافت;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
ساكن ثابت int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساكن ثابت int32_t KHMER_ریشه_COMBINE_آستانه = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساكن ثابت int32_t KHMER_پیشوند_COMBINE_آستانه = 3;

// Minimum word size
ساكن ثابت int32_t KHMER_MIN_کلمه = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t KHMER_MIN_کلمه_بازه = KHMER_MIN_کلمه * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_کد &وضعیت)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_مدخل(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_مجموعه khmer_کلمه_مجموعه(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        مجموعه_Characters(khmer_کلمه_مجموعه);
    }
    f_Mark_مجموعه.apply_الگو(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), وضعیت);
    f_Mark_مجموعه.جمع_کن(0x0020);
    f_انتها_کلمه_مجموعه = khmer_کلمه_مجموعه;
    f_Begin_کلمه_مجموعه.جمع_کن(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_انتها_کلمه_مجموعه.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_مجموعه.compact();
    f_انتها_کلمه_مجموعه.compact();
    f_Begin_کلمه_مجموعه.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_وضعیت(وضعیت);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    حذف fDictionary;
}

int32_t
KhmerBreakEngine::divide_به_بالا_Dictionary_Range( UText *متن,
                                                int32_t range_شروع,
                                                int32_t range_انتها,
                                                UVector32 &یافت_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کد& وضعیت ) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    إذا ((range_انتها - range_شروع) < KHMER_MIN_کلمه_بازه) {
        أرجع 0;       // Not enough characters for two words
    }

    uint32_t واژگان_یافت = 0;
    int32_t cp_کلمه_درازی = 0;
    int32_t cu_کلمه_درازی = 0;
    int32_t current;
    Possible_کلمه واژگان[KHMER_LOOKAHEAD];

    utext_مجموعه_Native_شاخص(متن, range_شروع);

    طالما (U_SUCCESS(وضعیت) && (current = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_انتها) {
        cu_کلمه_درازی = 0;
        cp_کلمه_درازی = 0;

        // Look for candidate words at the current position
        int32_t candidates = واژگان[واژگان_یافت%KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتها);

        // If we found exactly one, use that
        إذا (candidates == 1) {
            cu_کلمه_درازی = واژگان[واژگان_یافت % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_درازی = واژگان[واژگان_یافت % KHMER_LOOKAHEAD].markedCPLength();
            واژگان_یافت += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        وإلا إذا (candidates > 1) {
            // If we're already at the end of the range, we're done
            إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_انتها) {
                اذهب یافت_بهترین;
            }
            افعل {
                إذا (واژگان[(واژگان_یافت + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    واژگان[واژگان_یافت % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) >= range_انتها) {
                        اذهب یافت_بهترین;
                    }

                    // See if any of the possible second words is followed by a third word
                    افعل {
                        // If we find a third word, stop right away
                        إذا (واژگان[(واژگان_یافت + 2) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتها)) {
                            واژگان[واژگان_یافت % KHMER_LOOKAHEAD].markCurrent();
                            اذهب یافت_بهترین;
                        }
                    }
                    طالما (واژگان[(واژگان_یافت + 1) % KHMER_LOOKAHEAD].پشت_به_بالا(متن));
                }
            }
            طالما (واژگان[واژگان_یافت % KHMER_LOOKAHEAD].پشت_به_بالا(متن));
یافت_بهترین:
            cu_کلمه_درازی = واژگان[واژگان_یافت % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_کلمه_درازی = واژگان[واژگان_یافت % KHMER_LOOKAHEAD].markedCPLength();
            واژگان_یافت += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        إذا (static_cast<int32_t>(utext_get_Native_شاخص(متن)) < range_انتها && cp_کلمه_درازی < KHMER_ریشه_COMBINE_آستانه) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            إذا (واژگان[واژگان_یافت % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتها) <= 0
                  && (cu_کلمه_درازی == 0
                      || واژگان[واژگان_یافت % KHMER_LOOKAHEAD].longest_پیشوند() < KHMER_پیشوند_COMBINE_آستانه)) {
                // Look for a plausible word boundary
                int32_t مانده = range_انتها - (current+cu_کلمه_درازی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                لكل (;;) {
                    int32_t pc_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
                    pc = utext_next32(متن);
                    int32_t pc_اندازه = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - pc_شاخص;
                    chars += pc_اندازه;
                    مانده -= pc_اندازه;
                    إذا (مانده <= 0) {
                        اكسر;
                    }
                    uc = utext_current32(متن);
                    إذا (f_انتها_کلمه_مجموعه.شامل_است(pc) && f_Begin_کلمه_مجموعه.شامل_است(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t رقم_candidates = واژگان[(واژگان_یافت + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتها);
                        utext_مجموعه_Native_شاخص(متن, current+cu_کلمه_درازی+chars);
                        إذا (رقم_candidates > 0) {
                            اكسر;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                إذا (cu_کلمه_درازی <= 0) {
                    واژگان_یافت += 1;
                }

                // Update the length with the passed-over characters
                cu_کلمه_درازی += chars;
            }
            وإلا {
                // Back up to where we were for next iteration
                utext_مجموعه_Native_شاخص(متن, current+cu_کلمه_درازی);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        طالما ((currPos = static_cast<int32_t>(utext_get_Native_شاخص(متن))) < range_انتها && f_Mark_مجموعه.شامل_است(utext_current32(متن))) {
            utext_next32(متن);
            cu_کلمه_درازی += static_cast<int32_t>(utext_get_Native_شاخص(متن)) - currPos;
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
        إذا (cu_کلمه_درازی > 0) {
            یافت_Breaks.push((current+cu_کلمه_درازی), وضعیت);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    إذا (یافت_Breaks.peeki() >= range_انتها) {
        (فراغ) یافت_Breaks.popi();
        واژگان_یافت -= 1;
    }

    أرجع واژگان_یافت;
}

#إذا !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
ساكن ثابت uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_کد &وضعیت)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(خطأ) {
    UTRACE_مدخل(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = اشاره_تهی;
    nfkcNorm2 = Normalizer2::getNFKCInstance(وضعیت);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_کلمه_مجموعه.apply_الگو(UnicodeString(u"[\\uac00-\\ud7a3]"), وضعیت);
    f_Hangul_کلمه_مجموعه.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_مجموعه.apply_الگو(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), وضعیت);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_مجموعه.compact();
    f_Close_Punctuation_مجموعه.apply_الگو(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), وضعیت);
    f_Close_Punctuation_مجموعه.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    إذا (type == kKorean) {
        إذا (U_SUCCESS(وضعیت)) {
            مجموعه_Characters(f_Hangul_کلمه_مجموعه);
        }
    } وإلا { // Chinese and Japanese
        Unicode_مجموعه cj_مجموعه(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), وضعیت);
        isCj = درست;
        إذا (U_SUCCESS(وضعیت)) {
            مجموعه_Characters(cj_مجموعه);
#إذا UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = جديد MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_مجموعه,
                                               f_Close_Punctuation_مجموعه, وضعیت);
            إذا (fMlBreakEngine == اشاره_تهی) {
                وضعیت = U_حافظه_تخصیص_ERROR;
            }
#وإلا
            initJapanesePhraseParameter(وضعیت);
#پایان_اگر
        }
    }
    UTRACE_EXIT_وضعیت(وضعیت);
}

CjkBreakEngine::~CjkBreakEngine(){
    حذف fDictionary;
    حذف fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
ساكن ثابت int32_t k_Max_Katakana_درازی = 8;
ساكن ثابت int32_t k_Max_Katakana_گروه_درازی = 20;
ساكن ثابت uint32_t maxSnlp = 255;

ساكن مضمن uint32_t get_Katakana_هزینه(int32_t کلمه_درازی){
    //TODO: fill array with actual values from dictionary!
    ساكن ثابت uint32_t katakana_هزینه[k_Max_Katakana_درازی + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    أرجع (کلمه_درازی > k_Max_Katakana_درازی) ? 8192 : katakana_هزینه[کلمه_درازی];
}

ساكن مضمن منطقي isKatakana(UChar32 value) {
    أرجع (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

ساكن مضمن int32_t utext_i32_flag(int32_t بیت_شاخص) {
    أرجع static_cast<int32_t>(1) << بیت_شاخص;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_به_بالا_Dictionary_Range( UText *درون_متن,
        int32_t range_شروع,
        int32_t range_انتها,
        UVector32 &یافت_Breaks,
        UBool isPhraseBreaking,
        UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) أرجع 0;
    إذا (range_شروع >= range_انتها) {
        أرجع 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString درون_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    Local_اشاره_گر<UVector32>     درون_داد_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    إذا ((درون_متن->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         درون_متن->chunk_Native_شروع <= range_شروع &&
         درون_متن->chunk_Native_حد >= range_انتها   &&
         درون_متن->native_Indexing_حد >= range_انتها - درون_متن->chunk_Native_شروع) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        درون_String.مجموعه_مقصد(خطأ,
                       درون_متن->chunkContents + range_شروع - درون_متن->chunk_Native_شروع,
                       range_انتها - range_شروع);
    } وإلا {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_مجموعه_Native_شاخص(درون_متن, range_شروع);
        int32_t حد = range_انتها;
        U_ASSERT(حد <= utext_native_درازی(درون_متن));
        إذا (حد > utext_native_درازی(درون_متن)) {
            حد = static_cast<int32_t>(utext_native_درازی(درون_متن));
        }
        درون_داد_Map.adopt_Instead_And_Check_Error_کد(جديد UVector32(وضعیت), وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع 0;
        }
        طالما (utext_get_Native_شاخص(درون_متن) < حد) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_شاخص(درون_متن));
            UChar32 c = utext_next32(درون_متن);
            U_ASSERT(c != U_SENTINEL);
            درون_String.append(c);
            طالما (درون_داد_Map->size() < درون_String.length()) {
                درون_داد_Map->جمع_کن_Element(nativePosition, وضعیت);
            }
        }
        درون_داد_Map->جمع_کن_Element(حد, وضعیت);
    }


    إذا (!nfkcNorm2->isNormalized(درون_String, وضعیت)) {
        UnicodeString normalized_درون_داد;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        Local_اشاره_گر<UVector32> normalizedMap(جديد UVector32(وضعیت), وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        لكل (int32_t srcI = 0; srcI < درون_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragment_شروع_I = srcI;
            UChar32 c = درون_String.char32At(srcI);
            لكل (;;) {
                fragment.append(c);
                srcI = درون_String.moveIndex32(srcI, 1);
                إذا (srcI == درون_String.length()) {
                    اكسر;
                }
                c = درون_String.char32At(srcI);
                إذا (nfkcNorm2->hasBoundaryBefore(c)) {
                    اكسر;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, وضعیت);
            normalized_درون_داد.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragment_Original_شروع = درون_داد_Map.isValid() ?
                    درون_داد_Map->elementAti(fragment_شروع_I) : fragment_شروع_I+range_شروع;
            طالما (normalizedMap->size() < normalized_درون_داد.length()) {
                normalizedMap->جمع_کن_Element(fragment_Original_شروع, وضعیت);
                إذا (U_FAILURE(وضعیت)) {
                    اكسر;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_درون_داد.length());
        int32_t native_انتها = درون_داد_Map.isValid() ?
                درون_داد_Map->elementAti(درون_String.length()) : درون_String.length()+range_شروع;
        normalizedMap->جمع_کن_Element(native_انتها, وضعیت);

        درون_داد_Map = std::move(normalizedMap);
        درون_String = std::move(normalized_درون_داد);
    }

    int32_t رقم_کد_Pts = درون_String.شمار_Char32();
    إذا (رقم_کد_Pts != درون_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = درون_داد_Map.isValid();
        إذا (!hadExistingMap) {
            درون_داد_Map.adopt_Instead_And_Check_Error_کد(جديد UVector32(وضعیت), وضعیت);
            إذا (U_FAILURE(وضعیت)) {
                أرجع 0;
            }
        }
        int32_t cpIdx = 0;
        لكل (int32_t cuIdx = 0; ; cuIdx = درون_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            إذا (hadExistingMap) {
                درون_داد_Map->مجموعه_Element_At(درون_داد_Map->elementAti(cuIdx), cpIdx);
            } وإلا {
                درون_داد_Map->جمع_کن_Element(cuIdx+range_شروع, وضعیت);
            }
            cpIdx++;
            إذا (cuIdx == درون_String.length()) {
               اكسر;
            }
        }
    }

#إذا UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    إذا (isPhraseBreaking && isCj) {
        أرجع fMlBreakEngine->divide_به_بالا_Range(درون_متن, range_شروع, range_انتها, یافت_Breaks, درون_String,
                                             درون_داد_Map, وضعیت);
    }
#پایان_اگر

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 بهترین_Snlp(رقم_کد_Pts + 1, وضعیت);
    بهترین_Snlp.جمع_کن_Element(0, وضعیت);
    لكل(int32_t i = 1; i <= رقم_کد_Pts; i++) {
        بهترین_Snlp.جمع_کن_Element(kuint32max, وضعیت);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 prev(رقم_کد_Pts + 1, وضعیت);
    لكل(int32_t i = 0; i <= رقم_کد_Pts; i++){
        prev.جمع_کن_Element(-1, وضعیت);
    }

    ثابت int32_t max_کلمه_اندازه = 20;
    UVector32 مقادیر(رقم_کد_Pts, وضعیت);
    مقادیر.مجموعه_اندازه(رقم_کد_Pts);
    UVector32 lengths(رقم_کد_Pts, وضعیت);
    lengths.مجموعه_اندازه(رقم_کد_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &درون_String, &وضعیت);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    منطقي is_prev_katakana = خطأ;
    لكل (int32_t i = 0;  i < رقم_کد_Pts;  ++i, ix = درون_String.moveIndex32(ix, 1)) {
        إذا (static_cast<uint32_t>(بهترین_Snlp.elementAti(i)) == kuint32max) {
            استمر;
        }

        int32_t count;
        utext_مجموعه_Native_شاخص(&fu, ix);
        count = fDictionary->matches(&fu, max_کلمه_اندازه, رقم_کد_Pts,
                             اشاره_تهی, lengths.get_میانگیر(), مقادیر.get_میانگیر(), اشاره_تهی);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        إذا ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_کلمه_مجموعه.شامل_است(درون_String.char32At(ix))) {
            مقادیر.مجموعه_Element_At(maxSnlp, count);   // 255
            lengths.مجموعه_Element_At(1, count++);
        }

        لكل (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(بهترین_Snlp.elementAti(i)) + static_cast<uint32_t>(مقادیر.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            إذا (newSnlp < static_cast<uint32_t>(بهترین_Snlp.elementAti(ln_j_i))) {
                بهترین_Snlp.مجموعه_Element_At(newSnlp, ln_j_i);
                prev.مجموعه_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        منطقي is_katakana = isKatakana(درون_String.char32At(ix));
        int32_t katakana_پیاپی_درازی = 1;
        إذا (!is_prev_katakana && is_katakana) {
            int32_t j = درون_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            طالما (j < درون_String.length() && katakana_پیاپی_درازی < k_Max_Katakana_گروه_درازی &&
                    isKatakana(درون_String.char32At(j))) {
                j = درون_String.moveIndex32(j, 1);
                katakana_پیاپی_درازی++;
            }
            إذا (katakana_پیاپی_درازی < k_Max_Katakana_گروه_درازی) {
                uint32_t newSnlp = بهترین_Snlp.elementAti(i) + get_Katakana_هزینه(katakana_پیاپی_درازی);
                إذا (newSnlp < static_cast<uint32_t>(بهترین_Snlp.elementAti(i + katakana_پیاپی_درازی))) {
                    بهترین_Snlp.مجموعه_Element_At(newSnlp, i+katakana_پیاپی_درازی);
                    prev.مجموعه_Element_At(i, i+katakana_پیاپی_درازی);  // prev[j] = i;
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
    UVector32 t_boundary(رقم_کد_Pts+1, وضعیت);

    int32_t رقم_Breaks = 0;
    // No segmentation found, set boundary to end of range
    إذا (static_cast<uint32_t>(بهترین_Snlp.elementAti(رقم_کد_Pts)) == kuint32max) {
        t_boundary.جمع_کن_Element(رقم_کد_Pts, وضعیت);
        رقم_Breaks++;
    } وإلا إذا (isPhraseBreaking) {
        t_boundary.جمع_کن_Element(رقم_کد_Pts, وضعیت);
        إذا(U_SUCCESS(وضعیت)) {
            رقم_Breaks++;
            int32_t prevIdx = رقم_کد_Pts;

            int32_t کد_Unit_Idx = -1;
            int32_t prev_کد_Unit_Idx = -1;
            int32_t length = -1;
            لكل (int32_t i = prev.elementAti(رقم_کد_Pts); i > 0; i = prev.elementAti(i)) {
                کد_Unit_Idx = درون_String.moveIndex32(0, i);
                prev_کد_Unit_Idx = درون_String.moveIndex32(0, prevIdx);
                // Calculate the length by using the code unit.
                length = prev_کد_Unit_Idx - کد_Unit_Idx;
                prevIdx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                إذا (!f_Skip_مجموعه.شامل_است_کلید(درون_String.موقت_Sub_String(کد_Unit_Idx, length))
                    && (!isKatakana(درون_String.char32At(درون_String.moveIndex32(کد_Unit_Idx, -1)))
                           || !isKatakana(درون_String.char32At(کد_Unit_Idx)))) {
                    t_boundary.جمع_کن_Element(i, وضعیت);
                    رقم_Breaks++;
                }
            }
        }
    } وإلا {
        لكل (int32_t i = رقم_کد_Pts; i > 0; i = prev.elementAti(i)) {
            t_boundary.جمع_کن_Element(i, وضعیت);
            رقم_Breaks++;
        }
        U_ASSERT(prev.elementAti(t_boundary.elementAti(رقم_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    إذا (یافت_Breaks.size() == 0 || یافت_Breaks.peeki() < range_شروع) {
        t_boundary.جمع_کن_Element(0, وضعیت);
        رقم_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_رقم_Breaks = 0;
    لكل (int32_t i = رقم_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > prevCPPos);
        int32_t utextPos =  درون_داد_Map.isValid() ? درون_داد_Map->elementAti(cpPos) : cpPos + range_شروع;
        U_ASSERT(utextPos >= prevUTextPos);
        إذا (utextPos > prevUTextPos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(یافت_Breaks.size() == 0 || یافت_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            إذا (utextPos != range_شروع
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_مجموعه.شامل_است(utext_char32At(درون_متن, utextPos - 1)))) {
                یافت_Breaks.push(utextPos, وضعیت);
                corrected_رقم_Breaks++;
            }
        } وإلا {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --رقم_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (فراغ)prevCPPos; // suppress compiler warnings about unused variable

    UChar32 بعدی_Char = utext_char32At(درون_متن, range_انتها);
    إذا (!یافت_Breaks.is_خالی() && یافت_Breaks.peeki() == range_انتها) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (isPhraseBreaking) {
            إذا (!f_Digit_Or_Open_Punctuation_Or_Alphabet_مجموعه.شامل_است(بعدی_Char)) {
                یافت_Breaks.popi();
                corrected_رقم_Breaks--;
            }
        } وإلا {
            یافت_Breaks.popi();
            corrected_رقم_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    أرجع corrected_رقم_Breaks;
}

فراغ CjkBreakEngine::initJapanesePhraseParameter(UError_کد& error) {
    بارگیری_Japanese_Extensions(error);
    بارگیری_Hiragana(error);
}

فراغ CjkBreakEngine::بارگیری_Japanese_Extensions(UError_کد& error) {
    ثابت حرف* برچسب = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    إذا (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(برچسب, error);
        طالما (U_SUCCESS(error) && bundle.has_بعدی()) {
            f_Skip_مجموعه.puti(bundle.get_بعدی_String(error), 1, error);
        }
    }
}

فراغ CjkBreakEngine::بارگیری_Hiragana(UError_کد& error) {
    Unicode_مجموعه hiragana_کلمه_مجموعه(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_کلمه_مجموعه.compact();
    Unicode_مجموعه_Iterator iterator(hiragana_کلمه_مجموعه);
    طالما (iterator.next()) {
        f_Skip_مجموعه.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#پایان_اگر

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */

