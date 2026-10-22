// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 *******************************************************************************
 */

#شامل <utility>

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "brkeng.h"
#شامل "dictbe.h"
#شامل "unicode/uniset.h"
#شامل "unicode/chariter.h"
#شامل "unicode/resbund.h"
#شامل "unicode/ubrk.h"
#شامل "unicode/usetiter.h"
#شامل "ubrkimpl.h"
#شامل "utracimp.h"
#شامل "uvectr32.h"
#شامل "uvector.h"
#شامل "uassert.h"
#شامل "unicode/normlzr.h"
#شامل "cmemory.h"
#شامل "dictionarydata.h"

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
    واپس f_سیٹ.شامل_ہے(c);
}

int32_t
DictionaryBreakEngine::ڈھونڈو_Breaks( UText *متن,
                                 int32_t آغاز_Pos,
                                 int32_t انتہا_Pos,
                                 UVector32 &ملا_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    int32_t نتیجہ = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_سیٹ_Native_اشاریہ(متن, آغاز_Pos);
    int32_t آغاز = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    int32_t current;
    int32_t range_آغاز;
    int32_t range_انتہا;
    UChar32 c = utext_current32(متن);
    جب ((current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < انتہا_Pos && f_سیٹ.شامل_ہے(c)) {
        utext_next32(متن);         // TODO:  recast loop for postincrement
        c = utext_current32(متن);
    }
    range_آغاز = آغاز;
    range_انتہا = current;
    نتیجہ = divide_اوپر_Dictionary_Range(متن, range_آغاز, range_انتہا, ملا_Breaks, isPhraseBreaking, حالت_2);
    utext_سیٹ_Native_اشاریہ(متن, current);
    
    واپس نتیجہ;
}

خالی
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
ساکن ثابت int32_t POSSIBLE_لفظ_LIST_MAX = 20;

جماعت Possible_لفظ {
نجی:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   سابقہ;     // The longest match with a dictionary word
    int32_t   انحراف;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_لفظ_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_لفظ_LIST_MAX];   // Word Lengths, in code points.

عوامی:
    Possible_لفظ() : count(0), سابقہ(0), انحراف(-1), mark(0), current(0) {}
    ~Possible_لفظ() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *متن, DictionaryMatcher *dict, int32_t range_انتہا );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *متن );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     عقب_اوپر( UText *متن );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_سابقہ() { واپس سابقہ; }
  
    // Mark the current candidate as the one we like
    خالی      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { واپس cpLengths[mark]; }
};


int32_t Possible_لفظ::candidates( UText *متن, DictionaryMatcher *dict, int32_t range_انتہا ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t آغاز = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    اگر (آغاز != انحراف) {
        انحراف = آغاز;
        count = dict->matches(متن, range_انتہا-آغاز, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, خالی_اشاریہ, &سابقہ);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        اگر (count <= 0) {
            utext_سیٹ_Native_اشاریہ(متن, آغاز);
        }
    }
    اگر (count > 0) {
        utext_سیٹ_Native_اشاریہ(متن, آغاز+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    واپس count;
}

int32_t
Possible_لفظ::acceptMarked( UText *متن ) {
    utext_سیٹ_Native_اشاریہ(متن, انحراف + cuLengths[mark]);
    واپس cuLengths[mark];
}


UBool
Possible_لفظ::عقب_اوپر( UText *متن ) {
    اگر (current > 0) {
        utext_سیٹ_Native_اشاریہ(متن, انحراف + cuLengths[--current]);
        واپس سچ;
    }
    واپس جھوٹ;
}

/*
 ******************************************************************
 * ThaiBreakEngine
 */

// How many words in a row are "good enough"?
ساکن ثابت int32_t THAI_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساکن ثابت int32_t THAI_جڑ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساکن ثابت int32_t THAI_سابقہ_COMBINE_حد = 3;

// Elision character
ساکن ثابت int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
ساکن ثابت int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
ساکن ثابت int32_t THAI_MIN_لفظ = 2;

// Minimum number of characters for two words
ساکن ثابت int32_t THAI_MIN_لفظ_وسعت = THAI_MIN_لفظ * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_رمز &حالت_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_سیٹ thai_لفظ_سیٹ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        سیٹ_Characters(thai_لفظ_سیٹ);
    }
    f_Mark_سیٹ.apply_نمونہ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), حالت_2);
    f_Mark_سیٹ.جوڑو(0x0020);
    f_انتہا_لفظ_سیٹ = thai_لفظ_سیٹ;
    f_انتہا_لفظ_سیٹ.remove(0x0E31);             // MAI HAN-AKAT
    f_انتہا_لفظ_سیٹ.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_لفظ_سیٹ.جوڑو(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_لفظ_سیٹ.جوڑو(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_سیٹ.جوڑو(THAI_PAIYANNOI);
    f_Suffix_سیٹ.جوڑو(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_سیٹ.compact();
    f_انتہا_لفظ_سیٹ.compact();
    f_Begin_لفظ_سیٹ.compact();
    f_Suffix_سیٹ.compact();
    UTRACE_EXIT_حالت(حالت_2);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    حذف fDictionary;
}

int32_t
ThaiBreakEngine::divide_اوپر_Dictionary_Range( UText *متن,
                                                int32_t range_آغاز,
                                                int32_t range_انتہا,
                                                UVector32 &ملا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    utext_سیٹ_Native_اشاریہ(متن, range_آغاز);
    utext_moveIndex32(متن, THAI_MIN_لفظ_وسعت);
    اگر (utext_get_Native_اشاریہ(متن) >= range_انتہا) {
        واپس 0;       // Not enough characters for two words
    }
    utext_سیٹ_Native_اشاریہ(متن, range_آغاز);


    uint32_t الفاظ_ملا = 0;
    int32_t cp_لفظ_لمبائی = 0;    // Word Length in Code Points.
    int32_t cu_لفظ_لمبائی = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    Possible_لفظ الفاظ[THAI_LOOKAHEAD];
    
    utext_سیٹ_Native_اشاریہ(متن, range_آغاز);
    
    جب (U_SUCCESS(حالت_2) && (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_انتہا) {
        cp_لفظ_لمبائی = 0;
        cu_لفظ_لمبائی = 0;

        // Look for candidate words at the current position
        int32_t candidates = الفاظ[الفاظ_ملا%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا);
        
        // If we found exactly one, use that
        اگر (candidates == 1) {
            cu_لفظ_لمبائی = الفاظ[الفاظ_ملا % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمبائی = الفاظ[الفاظ_ملا % THAI_LOOKAHEAD].markedCPLength();
            الفاظ_ملا += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ورنہ اگر (candidates > 1) {
            // If we're already at the end of the range, we're done
            اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_انتہا) {
                جاؤ ملا_بہترین;
            }
            کرو {
                اگر (الفاظ[(الفاظ_ملا + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    الفاظ[الفاظ_ملا%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_انتہا) {
                        جاؤ ملا_بہترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    کرو {
                        // If we find a third word, stop right away
                        اگر (الفاظ[(الفاظ_ملا + 2) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا)) {
                            الفاظ[الفاظ_ملا % THAI_LOOKAHEAD].markCurrent();
                            جاؤ ملا_بہترین;
                        }
                    }
                    جب (الفاظ[(الفاظ_ملا + 1) % THAI_LOOKAHEAD].عقب_اوپر(متن));
                }
            }
            جب (الفاظ[الفاظ_ملا % THAI_LOOKAHEAD].عقب_اوپر(متن));
ملا_بہترین:
            // Set UText position to after the accepted word.
            cu_لفظ_لمبائی = الفاظ[الفاظ_ملا % THAI_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمبائی = الفاظ[الفاظ_ملا % THAI_LOOKAHEAD].markedCPLength();
            الفاظ_ملا += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_انتہا && cp_لفظ_لمبائی < THAI_جڑ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            اگر (الفاظ[الفاظ_ملا % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) <= 0
                  && (cu_لفظ_لمبائی == 0
                      || الفاظ[الفاظ_ملا%THAI_LOOKAHEAD].longest_سابقہ() < THAI_سابقہ_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بقایا = range_انتہا - (current+cu_لفظ_لمبائی);
                UChar32 pc;
                int32_t chars = 0;
                برائے (;;) {
                    int32_t pc_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                    pc = utext_next32(متن);
                    int32_t pc_سائز = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - pc_اشاریہ;
                    chars += pc_سائز;
                    بقایا -= pc_سائز;
                    اگر (بقایا <= 0) {
                        توڑو;
                    }
                    uc = utext_current32(متن);
                    اگر (f_انتہا_لفظ_سیٹ.شامل_ہے(pc) && f_Begin_لفظ_سیٹ.شامل_ہے(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t عدد_candidates = الفاظ[(الفاظ_ملا + 1) % THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا);
                        utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمبائی + chars);
                        اگر (عدد_candidates > 0) {
                            توڑو;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                اگر (cu_لفظ_لمبائی <= 0) {
                    الفاظ_ملا += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_لمبائی += chars;
            }
            ورنہ {
                // Back up to where we were for next iteration
                utext_سیٹ_Native_اشاریہ(متن, current+cu_لفظ_لمبائی);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        جب ((currPos = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_انتہا && f_Mark_سیٹ.شامل_ہے(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_لمبائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_انتہا && cu_لفظ_لمبائی > 0) {
            اگر (الفاظ[الفاظ_ملا%THAI_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) <= 0
                && f_Suffix_سیٹ.شامل_ہے(uc = utext_current32(متن))) {
                اگر (uc == THAI_PAIYANNOI) {
                    اگر (!f_Suffix_سیٹ.شامل_ہے(utext_previous32(متن))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(متن);
                        int32_t paiyannoi_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                        utext_next32(متن);
                        cu_لفظ_لمبائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - paiyannoi_اشاریہ; // Add PAIYANNOI to word
                        uc = utext_current32(متن);     // Fetch next character
                    }
                    ورنہ {
                        // Restore prior position
                        utext_next32(متن);
                    }
                }
                اگر (uc == THAI_MAIYAMOK) {
                    اگر (utext_previous32(متن) != THAI_MAIYAMOK) {
                        // Skip over previous end and MAIYAMOK
                        utext_next32(متن);
                        int32_t maiyamok_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                        utext_next32(متن);
                        cu_لفظ_لمبائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - maiyamok_اشاریہ; // Add MAIYAMOK to word
                    }
                    ورنہ {
                        // Restore prior position
                        utext_next32(متن);
                    }
                }
            }
            ورنہ {
                utext_سیٹ_Native_اشاریہ(متن, current+cu_لفظ_لمبائی);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        اگر (cu_لفظ_لمبائی > 0) {
            ملا_Breaks.push((current+cu_لفظ_لمبائی), حالت_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    اگر (ملا_Breaks.peeki() >= range_انتہا) {
        (خالی) ملا_Breaks.popi();
        الفاظ_ملا -= 1;
    }

    واپس الفاظ_ملا;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
ساکن ثابت int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساکن ثابت int32_t LAO_جڑ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساکن ثابت int32_t LAO_سابقہ_COMBINE_حد = 3;

// Minimum word size
ساکن ثابت int32_t LAO_MIN_لفظ = 2;

// Minimum number of characters for two words
ساکن ثابت int32_t LAO_MIN_لفظ_وسعت = LAO_MIN_لفظ * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_رمز &حالت_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_سیٹ lao_لفظ_سیٹ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        سیٹ_Characters(lao_لفظ_سیٹ);
    }
    f_Mark_سیٹ.apply_نمونہ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), حالت_2);
    f_Mark_سیٹ.جوڑو(0x0020);
    f_انتہا_لفظ_سیٹ = lao_لفظ_سیٹ;
    f_انتہا_لفظ_سیٹ.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_لفظ_سیٹ.جوڑو(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_لفظ_سیٹ.جوڑو(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_لفظ_سیٹ.جوڑو(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_سیٹ.compact();
    f_انتہا_لفظ_سیٹ.compact();
    f_Begin_لفظ_سیٹ.compact();
    UTRACE_EXIT_حالت(حالت_2);
}

LaoBreakEngine::~LaoBreakEngine() {
    حذف fDictionary;
}

int32_t
LaoBreakEngine::divide_اوپر_Dictionary_Range( UText *متن,
                                                int32_t range_آغاز,
                                                int32_t range_انتہا,
                                                UVector32 &ملا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    اگر ((range_انتہا - range_آغاز) < LAO_MIN_لفظ_وسعت) {
        واپس 0;       // Not enough characters for two words
    }

    uint32_t الفاظ_ملا = 0;
    int32_t cp_لفظ_لمبائی = 0;
    int32_t cu_لفظ_لمبائی = 0;
    int32_t current;
    Possible_لفظ الفاظ[LAO_LOOKAHEAD];

    utext_سیٹ_Native_اشاریہ(متن, range_آغاز);

    جب (U_SUCCESS(حالت_2) && (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_انتہا) {
        cu_لفظ_لمبائی = 0;
        cp_لفظ_لمبائی = 0;

        // Look for candidate words at the current position
        int32_t candidates = الفاظ[الفاظ_ملا%LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا);
        
        // If we found exactly one, use that
        اگر (candidates == 1) {
            cu_لفظ_لمبائی = الفاظ[الفاظ_ملا % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمبائی = الفاظ[الفاظ_ملا % LAO_LOOKAHEAD].markedCPLength();
            الفاظ_ملا += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ورنہ اگر (candidates > 1) {
            // If we're already at the end of the range, we're done
            اگر (utext_get_Native_اشاریہ(متن) >= range_انتہا) {
                جاؤ ملا_بہترین;
            }
            کرو {
                اگر (الفاظ[(الفاظ_ملا + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    الفاظ[الفاظ_ملا%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_انتہا) {
                        جاؤ ملا_بہترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    کرو {
                        // If we find a third word, stop right away
                        اگر (الفاظ[(الفاظ_ملا + 2) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا)) {
                            الفاظ[الفاظ_ملا % LAO_LOOKAHEAD].markCurrent();
                            جاؤ ملا_بہترین;
                        }
                    }
                    جب (الفاظ[(الفاظ_ملا + 1) % LAO_LOOKAHEAD].عقب_اوپر(متن));
                }
            }
            جب (الفاظ[الفاظ_ملا % LAO_LOOKAHEAD].عقب_اوپر(متن));
ملا_بہترین:
            cu_لفظ_لمبائی = الفاظ[الفاظ_ملا % LAO_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمبائی = الفاظ[الفاظ_ملا % LAO_LOOKAHEAD].markedCPLength();
            الفاظ_ملا += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_انتہا && cp_لفظ_لمبائی < LAO_جڑ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            اگر (الفاظ[الفاظ_ملا % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) <= 0
                  && (cu_لفظ_لمبائی == 0
                      || الفاظ[الفاظ_ملا%LAO_LOOKAHEAD].longest_سابقہ() < LAO_سابقہ_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بقایا = range_انتہا - (current + cu_لفظ_لمبائی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                برائے (;;) {
                    int32_t pc_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                    pc = utext_next32(متن);
                    int32_t pc_سائز = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - pc_اشاریہ;
                    chars += pc_سائز;
                    بقایا -= pc_سائز;
                    اگر (بقایا <= 0) {
                        توڑو;
                    }
                    uc = utext_current32(متن);
                    اگر (f_انتہا_لفظ_سیٹ.شامل_ہے(pc) && f_Begin_لفظ_سیٹ.شامل_ہے(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t عدد_candidates = الفاظ[(الفاظ_ملا + 1) % LAO_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا);
                        utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمبائی + chars);
                        اگر (عدد_candidates > 0) {
                            توڑو;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                اگر (cu_لفظ_لمبائی <= 0) {
                    الفاظ_ملا += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_لمبائی += chars;
            }
            ورنہ {
                // Back up to where we were for next iteration
                utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمبائی);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        جب ((currPos = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_انتہا && f_Mark_سیٹ.شامل_ہے(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_لمبائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        اگر (cu_لفظ_لمبائی > 0) {
            ملا_Breaks.push((current+cu_لفظ_لمبائی), حالت_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    اگر (ملا_Breaks.peeki() >= range_انتہا) {
        (خالی) ملا_Breaks.popi();
        الفاظ_ملا -= 1;
    }

    واپس الفاظ_ملا;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
ساکن ثابت int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساکن ثابت int32_t BURMESE_جڑ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساکن ثابت int32_t BURMESE_سابقہ_COMBINE_حد = 3;

// Minimum word size
ساکن ثابت int32_t BURMESE_MIN_لفظ = 2;

// Minimum number of characters for two words
ساکن ثابت int32_t BURMESE_MIN_لفظ_وسعت = BURMESE_MIN_لفظ * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_رمز &حالت_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_لفظ_سیٹ.جوڑو(0x1000, 0x102A);      // basic consonants and independent vowels
    f_انتہا_لفظ_سیٹ.apply_نمونہ(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), حالت_2);
    f_Mark_سیٹ.apply_نمونہ(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), حالت_2);
    f_Mark_سیٹ.جوڑو(0x0020);
    اگر (U_SUCCESS(حالت_2)) {
        سیٹ_Characters(f_انتہا_لفظ_سیٹ);
    }

    // Compact for caching.
    f_Mark_سیٹ.compact();
    f_انتہا_لفظ_سیٹ.compact();
    f_Begin_لفظ_سیٹ.compact();
    UTRACE_EXIT_حالت(حالت_2);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    حذف fDictionary;
}

int32_t
BurmeseBreakEngine::divide_اوپر_Dictionary_Range( UText *متن,
                                                int32_t range_آغاز,
                                                int32_t range_انتہا,
                                                UVector32 &ملا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_رمز& حالت_2 ) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    اگر ((range_انتہا - range_آغاز) < BURMESE_MIN_لفظ_وسعت) {
        واپس 0;       // Not enough characters for two words
    }

    uint32_t الفاظ_ملا = 0;
    int32_t cp_لفظ_لمبائی = 0;
    int32_t cu_لفظ_لمبائی = 0;
    int32_t current;
    Possible_لفظ الفاظ[BURMESE_LOOKAHEAD];

    utext_سیٹ_Native_اشاریہ(متن, range_آغاز);

    جب (U_SUCCESS(حالت_2) && (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_انتہا) {
        cu_لفظ_لمبائی = 0;
        cp_لفظ_لمبائی = 0;

        // Look for candidate words at the current position
        int32_t candidates = الفاظ[الفاظ_ملا%BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا);
        
        // If we found exactly one, use that
        اگر (candidates == 1) {
            cu_لفظ_لمبائی = الفاظ[الفاظ_ملا % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمبائی = الفاظ[الفاظ_ملا % BURMESE_LOOKAHEAD].markedCPLength();
            الفاظ_ملا += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ورنہ اگر (candidates > 1) {
            // If we're already at the end of the range, we're done
            اگر (utext_get_Native_اشاریہ(متن) >= range_انتہا) {
                جاؤ ملا_بہترین;
            }
            کرو {
                اگر (الفاظ[(الفاظ_ملا + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    الفاظ[الفاظ_ملا%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_انتہا) {
                        جاؤ ملا_بہترین;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    کرو {
                        // If we find a third word, stop right away
                        اگر (الفاظ[(الفاظ_ملا + 2) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا)) {
                            الفاظ[الفاظ_ملا % BURMESE_LOOKAHEAD].markCurrent();
                            جاؤ ملا_بہترین;
                        }
                    }
                    جب (الفاظ[(الفاظ_ملا + 1) % BURMESE_LOOKAHEAD].عقب_اوپر(متن));
                }
            }
            جب (الفاظ[الفاظ_ملا % BURMESE_LOOKAHEAD].عقب_اوپر(متن));
ملا_بہترین:
            cu_لفظ_لمبائی = الفاظ[الفاظ_ملا % BURMESE_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمبائی = الفاظ[الفاظ_ملا % BURMESE_LOOKAHEAD].markedCPLength();
            الفاظ_ملا += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_انتہا && cp_لفظ_لمبائی < BURMESE_جڑ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            اگر (الفاظ[الفاظ_ملا % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) <= 0
                  && (cu_لفظ_لمبائی == 0
                      || الفاظ[الفاظ_ملا%BURMESE_LOOKAHEAD].longest_سابقہ() < BURMESE_سابقہ_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بقایا = range_انتہا - (current + cu_لفظ_لمبائی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                برائے (;;) {
                    int32_t pc_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                    pc = utext_next32(متن);
                    int32_t pc_سائز = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - pc_اشاریہ;
                    chars += pc_سائز;
                    بقایا -= pc_سائز;
                    اگر (بقایا <= 0) {
                        توڑو;
                    }
                    uc = utext_current32(متن);
                    اگر (f_انتہا_لفظ_سیٹ.شامل_ہے(pc) && f_Begin_لفظ_سیٹ.شامل_ہے(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t عدد_candidates = الفاظ[(الفاظ_ملا + 1) % BURMESE_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا);
                        utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمبائی + chars);
                        اگر (عدد_candidates > 0) {
                            توڑو;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                اگر (cu_لفظ_لمبائی <= 0) {
                    الفاظ_ملا += 1;
                }
                
                // Update the length with the passed-over characters
                cu_لفظ_لمبائی += chars;
            }
            ورنہ {
                // Back up to where we were for next iteration
                utext_سیٹ_Native_اشاریہ(متن, current + cu_لفظ_لمبائی);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        جب ((currPos = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_انتہا && f_Mark_سیٹ.شامل_ہے(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_لمبائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        اگر (cu_لفظ_لمبائی > 0) {
            ملا_Breaks.push((current+cu_لفظ_لمبائی), حالت_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    اگر (ملا_Breaks.peeki() >= range_انتہا) {
        (خالی) ملا_Breaks.popi();
        الفاظ_ملا -= 1;
    }

    واپس الفاظ_ملا;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
ساکن ثابت int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ساکن ثابت int32_t KHMER_جڑ_COMBINE_حد = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ساکن ثابت int32_t KHMER_سابقہ_COMBINE_حد = 3;

// Minimum word size
ساکن ثابت int32_t KHMER_MIN_لفظ = 2;

// Minimum number of characters for two words
ساکن ثابت int32_t KHMER_MIN_لفظ_وسعت = KHMER_MIN_لفظ * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_رمز &حالت_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_سیٹ khmer_لفظ_سیٹ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        سیٹ_Characters(khmer_لفظ_سیٹ);
    }
    f_Mark_سیٹ.apply_نمونہ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), حالت_2);
    f_Mark_سیٹ.جوڑو(0x0020);
    f_انتہا_لفظ_سیٹ = khmer_لفظ_سیٹ;
    f_Begin_لفظ_سیٹ.جوڑو(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_انتہا_لفظ_سیٹ.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_سیٹ.compact();
    f_انتہا_لفظ_سیٹ.compact();
    f_Begin_لفظ_سیٹ.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_حالت(حالت_2);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    حذف fDictionary;
}

int32_t
KhmerBreakEngine::divide_اوپر_Dictionary_Range( UText *متن,
                                                int32_t range_آغاز,
                                                int32_t range_انتہا,
                                                UVector32 &ملا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_رمز& حالت_2 ) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    اگر ((range_انتہا - range_آغاز) < KHMER_MIN_لفظ_وسعت) {
        واپس 0;       // Not enough characters for two words
    }

    uint32_t الفاظ_ملا = 0;
    int32_t cp_لفظ_لمبائی = 0;
    int32_t cu_لفظ_لمبائی = 0;
    int32_t current;
    Possible_لفظ الفاظ[KHMER_LOOKAHEAD];

    utext_سیٹ_Native_اشاریہ(متن, range_آغاز);

    جب (U_SUCCESS(حالت_2) && (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_انتہا) {
        cu_لفظ_لمبائی = 0;
        cp_لفظ_لمبائی = 0;

        // Look for candidate words at the current position
        int32_t candidates = الفاظ[الفاظ_ملا%KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا);

        // If we found exactly one, use that
        اگر (candidates == 1) {
            cu_لفظ_لمبائی = الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمبائی = الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].markedCPLength();
            الفاظ_ملا += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        ورنہ اگر (candidates > 1) {
            // If we're already at the end of the range, we're done
            اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_انتہا) {
                جاؤ ملا_بہترین;
            }
            کرو {
                اگر (الفاظ[(الفاظ_ملا + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) >= range_انتہا) {
                        جاؤ ملا_بہترین;
                    }

                    // See if any of the possible second words is followed by a third word
                    کرو {
                        // If we find a third word, stop right away
                        اگر (الفاظ[(الفاظ_ملا + 2) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا)) {
                            الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].markCurrent();
                            جاؤ ملا_بہترین;
                        }
                    }
                    جب (الفاظ[(الفاظ_ملا + 1) % KHMER_LOOKAHEAD].عقب_اوپر(متن));
                }
            }
            جب (الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].عقب_اوپر(متن));
ملا_بہترین:
            cu_لفظ_لمبائی = الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].acceptMarked(متن);
            cp_لفظ_لمبائی = الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].markedCPLength();
            الفاظ_ملا += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        اگر (static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) < range_انتہا && cp_لفظ_لمبائی < KHMER_جڑ_COMBINE_حد) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            اگر (الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا) <= 0
                  && (cu_لفظ_لمبائی == 0
                      || الفاظ[الفاظ_ملا % KHMER_LOOKAHEAD].longest_سابقہ() < KHMER_سابقہ_COMBINE_حد)) {
                // Look for a plausible word boundary
                int32_t بقایا = range_انتہا - (current+cu_لفظ_لمبائی);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                برائے (;;) {
                    int32_t pc_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
                    pc = utext_next32(متن);
                    int32_t pc_سائز = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - pc_اشاریہ;
                    chars += pc_سائز;
                    بقایا -= pc_سائز;
                    اگر (بقایا <= 0) {
                        توڑو;
                    }
                    uc = utext_current32(متن);
                    اگر (f_انتہا_لفظ_سیٹ.شامل_ہے(pc) && f_Begin_لفظ_سیٹ.شامل_ہے(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t عدد_candidates = الفاظ[(الفاظ_ملا + 1) % KHMER_LOOKAHEAD].candidates(متن, fDictionary, range_انتہا);
                        utext_سیٹ_Native_اشاریہ(متن, current+cu_لفظ_لمبائی+chars);
                        اگر (عدد_candidates > 0) {
                            توڑو;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                اگر (cu_لفظ_لمبائی <= 0) {
                    الفاظ_ملا += 1;
                }

                // Update the length with the passed-over characters
                cu_لفظ_لمبائی += chars;
            }
            ورنہ {
                // Back up to where we were for next iteration
                utext_سیٹ_Native_اشاریہ(متن, current+cu_لفظ_لمبائی);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        جب ((currPos = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < range_انتہا && f_Mark_سیٹ.شامل_ہے(utext_current32(متن))) {
            utext_next32(متن);
            cu_لفظ_لمبائی += static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - currPos;
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
        اگر (cu_لفظ_لمبائی > 0) {
            ملا_Breaks.push((current+cu_لفظ_لمبائی), حالت_2);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    اگر (ملا_Breaks.peeki() >= range_انتہا) {
        (خالی) ملا_Breaks.popi();
        الفاظ_ملا -= 1;
    }

    واپس الفاظ_ملا;
}

#اگر !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
ساکن ثابت uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_رمز &حالت_2)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(جھوٹ) {
    UTRACE_اندراج(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = خالی_اشاریہ;
    nfkcNorm2 = Normalizer2::getNFKCInstance(حالت_2);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_لفظ_سیٹ.apply_نمونہ(UnicodeString(u"[\\uac00-\\ud7a3]"), حالت_2);
    f_Hangul_لفظ_سیٹ.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ.apply_نمونہ(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), حالت_2);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ.compact();
    f_Close_Punctuation_سیٹ.apply_نمونہ(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), حالت_2);
    f_Close_Punctuation_سیٹ.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    اگر (type == kKorean) {
        اگر (U_SUCCESS(حالت_2)) {
            سیٹ_Characters(f_Hangul_لفظ_سیٹ);
        }
    } ورنہ { // Chinese and Japanese
        Unicode_سیٹ cj_سیٹ(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), حالت_2);
        isCj = سچ;
        اگر (U_SUCCESS(حالت_2)) {
            سیٹ_Characters(cj_سیٹ);
#اگر UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = نیا MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ,
                                               f_Close_Punctuation_سیٹ, حالت_2);
            اگر (fMlBreakEngine == خالی_اشاریہ) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
            }
#ورنہ
            initJapanesePhraseParameter(حالت_2);
#اگر_ختم
        }
    }
    UTRACE_EXIT_حالت(حالت_2);
}

CjkBreakEngine::~CjkBreakEngine(){
    حذف fDictionary;
    حذف fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
ساکن ثابت int32_t k_Max_Katakana_لمبائی = 8;
ساکن ثابت int32_t k_Max_Katakana_گروہ_لمبائی = 20;
ساکن ثابت uint32_t maxSnlp = 255;

ساکن اندرونی uint32_t get_Katakana_لاگت(int32_t لفظ_لمبائی){
    //TODO: fill array with actual values from dictionary!
    ساکن ثابت uint32_t katakana_لاگت[k_Max_Katakana_لمبائی + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    واپس (لفظ_لمبائی > k_Max_Katakana_لمبائی) ? 8192 : katakana_لاگت[لفظ_لمبائی];
}

ساکن اندرونی منطقی isKatakana(UChar32 value) {
    واپس (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

ساکن اندرونی int32_t utext_i32_flag(int32_t ثنائی_ہندسہ_اشاریہ) {
    واپس static_cast<int32_t>(1) << ثنائی_ہندسہ_اشاریہ;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_اوپر_Dictionary_Range( UText *اندر_متن,
        int32_t range_آغاز,
        int32_t range_انتہا,
        UVector32 &ملا_Breaks,
        UBool isPhraseBreaking,
        UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) واپس 0;
    اگر (range_آغاز >= range_انتہا) {
        واپس 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString اندر_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    Local_اشارہ_گر<UVector32>     داخلہ_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    اگر ((اندر_متن->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         اندر_متن->chunk_Native_آغاز <= range_آغاز &&
         اندر_متن->chunk_Native_حد_بندی >= range_انتہا   &&
         اندر_متن->native_Indexing_حد_بندی >= range_انتہا - اندر_متن->chunk_Native_آغاز) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        اندر_String.سیٹ_مقصد(جھوٹ,
                       اندر_متن->chunkContents + range_آغاز - اندر_متن->chunk_Native_آغاز,
                       range_انتہا - range_آغاز);
    } ورنہ {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_سیٹ_Native_اشاریہ(اندر_متن, range_آغاز);
        int32_t حد_بندی = range_انتہا;
        U_ASSERT(حد_بندی <= utext_native_لمبائی(اندر_متن));
        اگر (حد_بندی > utext_native_لمبائی(اندر_متن)) {
            حد_بندی = static_cast<int32_t>(utext_native_لمبائی(اندر_متن));
        }
        داخلہ_Map.adopt_Instead_And_Check_Error_رمز(نیا UVector32(حالت_2), حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس 0;
        }
        جب (utext_get_Native_اشاریہ(اندر_متن) < حد_بندی) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_اشاریہ(اندر_متن));
            UChar32 c = utext_next32(اندر_متن);
            U_ASSERT(c != U_SENTINEL);
            اندر_String.append(c);
            جب (داخلہ_Map->size() < اندر_String.length()) {
                داخلہ_Map->جوڑو_Element(nativePosition, حالت_2);
            }
        }
        داخلہ_Map->جوڑو_Element(حد_بندی, حالت_2);
    }


    اگر (!nfkcNorm2->isNormalized(اندر_String, حالت_2)) {
        UnicodeString normalized_داخلہ;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        Local_اشارہ_گر<UVector32> normalizedMap(نیا UVector32(حالت_2), حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        برائے (int32_t srcI = 0; srcI < اندر_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragment_آغاز_I = srcI;
            UChar32 c = اندر_String.char32At(srcI);
            برائے (;;) {
                fragment.append(c);
                srcI = اندر_String.moveIndex32(srcI, 1);
                اگر (srcI == اندر_String.length()) {
                    توڑو;
                }
                c = اندر_String.char32At(srcI);
                اگر (nfkcNorm2->hasBoundaryBefore(c)) {
                    توڑو;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, حالت_2);
            normalized_داخلہ.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragment_Original_آغاز = داخلہ_Map.isValid() ?
                    داخلہ_Map->elementAti(fragment_آغاز_I) : fragment_آغاز_I+range_آغاز;
            جب (normalizedMap->size() < normalized_داخلہ.length()) {
                normalizedMap->جوڑو_Element(fragment_Original_آغاز, حالت_2);
                اگر (U_FAILURE(حالت_2)) {
                    توڑو;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_داخلہ.length());
        int32_t native_انتہا = داخلہ_Map.isValid() ?
                داخلہ_Map->elementAti(اندر_String.length()) : اندر_String.length()+range_آغاز;
        normalizedMap->جوڑو_Element(native_انتہا, حالت_2);

        داخلہ_Map = std::move(normalizedMap);
        اندر_String = std::move(normalized_داخلہ);
    }

    int32_t عدد_رمز_Pts = اندر_String.گنتی_Char32();
    اگر (عدد_رمز_Pts != اندر_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = داخلہ_Map.isValid();
        اگر (!hadExistingMap) {
            داخلہ_Map.adopt_Instead_And_Check_Error_رمز(نیا UVector32(حالت_2), حالت_2);
            اگر (U_FAILURE(حالت_2)) {
                واپس 0;
            }
        }
        int32_t cpIdx = 0;
        برائے (int32_t cuIdx = 0; ; cuIdx = اندر_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            اگر (hadExistingMap) {
                داخلہ_Map->سیٹ_Element_At(داخلہ_Map->elementAti(cuIdx), cpIdx);
            } ورنہ {
                داخلہ_Map->جوڑو_Element(cuIdx+range_آغاز, حالت_2);
            }
            cpIdx++;
            اگر (cuIdx == اندر_String.length()) {
               توڑو;
            }
        }
    }

#اگر UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    اگر (isPhraseBreaking && isCj) {
        واپس fMlBreakEngine->divide_اوپر_Range(اندر_متن, range_آغاز, range_انتہا, ملا_Breaks, اندر_String,
                                             داخلہ_Map, حالت_2);
    }
#اگر_ختم

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 بہترین_Snlp(عدد_رمز_Pts + 1, حالت_2);
    بہترین_Snlp.جوڑو_Element(0, حالت_2);
    برائے(int32_t i = 1; i <= عدد_رمز_Pts; i++) {
        بہترین_Snlp.جوڑو_Element(kuint32max, حالت_2);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 پچھلا(عدد_رمز_Pts + 1, حالت_2);
    برائے(int32_t i = 0; i <= عدد_رمز_Pts; i++){
        پچھلا.جوڑو_Element(-1, حالت_2);
    }

    ثابت int32_t max_لفظ_سائز = 20;
    UVector32 اقدار(عدد_رمز_Pts, حالت_2);
    اقدار.سیٹ_سائز(عدد_رمز_Pts);
    UVector32 lengths(عدد_رمز_Pts, حالت_2);
    lengths.سیٹ_سائز(عدد_رمز_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &اندر_String, &حالت_2);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    منطقی is_پچھلا_katakana = جھوٹ;
    برائے (int32_t i = 0;  i < عدد_رمز_Pts;  ++i, ix = اندر_String.moveIndex32(ix, 1)) {
        اگر (static_cast<uint32_t>(بہترین_Snlp.elementAti(i)) == kuint32max) {
            جاری;
        }

        int32_t count;
        utext_سیٹ_Native_اشاریہ(&fu, ix);
        count = fDictionary->matches(&fu, max_لفظ_سائز, عدد_رمز_Pts,
                             خالی_اشاریہ, lengths.get_بفر(), اقدار.get_بفر(), خالی_اشاریہ);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        اگر ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_لفظ_سیٹ.شامل_ہے(اندر_String.char32At(ix))) {
            اقدار.سیٹ_Element_At(maxSnlp, count);   // 255
            lengths.سیٹ_Element_At(1, count++);
        }

        برائے (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(بہترین_Snlp.elementAti(i)) + static_cast<uint32_t>(اقدار.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            اگر (newSnlp < static_cast<uint32_t>(بہترین_Snlp.elementAti(ln_j_i))) {
                بہترین_Snlp.سیٹ_Element_At(newSnlp, ln_j_i);
                پچھلا.سیٹ_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        منطقی is_katakana = isKatakana(اندر_String.char32At(ix));
        int32_t katakana_تسلسل_لمبائی = 1;
        اگر (!is_پچھلا_katakana && is_katakana) {
            int32_t j = اندر_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            جب (j < اندر_String.length() && katakana_تسلسل_لمبائی < k_Max_Katakana_گروہ_لمبائی &&
                    isKatakana(اندر_String.char32At(j))) {
                j = اندر_String.moveIndex32(j, 1);
                katakana_تسلسل_لمبائی++;
            }
            اگر (katakana_تسلسل_لمبائی < k_Max_Katakana_گروہ_لمبائی) {
                uint32_t newSnlp = بہترین_Snlp.elementAti(i) + get_Katakana_لاگت(katakana_تسلسل_لمبائی);
                اگر (newSnlp < static_cast<uint32_t>(بہترین_Snlp.elementAti(i + katakana_تسلسل_لمبائی))) {
                    بہترین_Snlp.سیٹ_Element_At(newSnlp, i+katakana_تسلسل_لمبائی);
                    پچھلا.سیٹ_Element_At(i, i+katakana_تسلسل_لمبائی);  // prev[j] = i;
                }
            }
        }
        is_پچھلا_katakana = is_katakana;
    }
    utext_close(&fu);

    // Start pushing the optimal offset index into t_boundary (t for tentative).
    // prev[numCodePts] is guaranteed to be meaningful.
    // We'll first push in the reverse order, i.e.,
    // t_boundary[0] = numCodePts, and afterwards do a swap.
    UVector32 t_boundary(عدد_رمز_Pts+1, حالت_2);

    int32_t عدد_Breaks = 0;
    // No segmentation found, set boundary to end of range
    اگر (static_cast<uint32_t>(بہترین_Snlp.elementAti(عدد_رمز_Pts)) == kuint32max) {
        t_boundary.جوڑو_Element(عدد_رمز_Pts, حالت_2);
        عدد_Breaks++;
    } ورنہ اگر (isPhraseBreaking) {
        t_boundary.جوڑو_Element(عدد_رمز_Pts, حالت_2);
        اگر(U_SUCCESS(حالت_2)) {
            عدد_Breaks++;
            int32_t پچھلا_Idx = عدد_رمز_Pts;

            int32_t رمز_Unit_Idx = -1;
            int32_t پچھلا_رمز_Unit_Idx = -1;
            int32_t length = -1;
            برائے (int32_t i = پچھلا.elementAti(عدد_رمز_Pts); i > 0; i = پچھلا.elementAti(i)) {
                رمز_Unit_Idx = اندر_String.moveIndex32(0, i);
                پچھلا_رمز_Unit_Idx = اندر_String.moveIndex32(0, پچھلا_Idx);
                // Calculate the length by using the code unit.
                length = پچھلا_رمز_Unit_Idx - رمز_Unit_Idx;
                پچھلا_Idx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                اگر (!f_Skip_سیٹ.شامل_ہے_کلید(اندر_String.عارضی_Sub_String(رمز_Unit_Idx, length))
                    && (!isKatakana(اندر_String.char32At(اندر_String.moveIndex32(رمز_Unit_Idx, -1)))
                           || !isKatakana(اندر_String.char32At(رمز_Unit_Idx)))) {
                    t_boundary.جوڑو_Element(i, حالت_2);
                    عدد_Breaks++;
                }
            }
        }
    } ورنہ {
        برائے (int32_t i = عدد_رمز_Pts; i > 0; i = پچھلا.elementAti(i)) {
            t_boundary.جوڑو_Element(i, حالت_2);
            عدد_Breaks++;
        }
        U_ASSERT(پچھلا.elementAti(t_boundary.elementAti(عدد_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    اگر (ملا_Breaks.size() == 0 || ملا_Breaks.peeki() < range_آغاز) {
        t_boundary.جوڑو_Element(0, حالت_2);
        عدد_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t پچھلا_CPPos = -1;
    int32_t پچھلا_UText_Pos = -1;
    int32_t corrected_عدد_Breaks = 0;
    برائے (int32_t i = عدد_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > پچھلا_CPPos);
        int32_t utextPos =  داخلہ_Map.isValid() ? داخلہ_Map->elementAti(cpPos) : cpPos + range_آغاز;
        U_ASSERT(utextPos >= پچھلا_UText_Pos);
        اگر (utextPos > پچھلا_UText_Pos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(ملا_Breaks.size() == 0 || ملا_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            اگر (utextPos != range_آغاز
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_سیٹ.شامل_ہے(utext_char32At(اندر_متن, utextPos - 1)))) {
                ملا_Breaks.push(utextPos, حالت_2);
                corrected_عدد_Breaks++;
            }
        } ورنہ {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --عدد_Breaks;
        }
        پچھلا_CPPos = cpPos;
        پچھلا_UText_Pos = utextPos;
    }
    (خالی)پچھلا_CPPos; // suppress compiler warnings about unused variable

    UChar32 اگلا_Char = utext_char32At(اندر_متن, range_انتہا);
    اگر (!ملا_Breaks.is_خالی_خانے() && ملا_Breaks.peeki() == range_انتہا) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        اگر (isPhraseBreaking) {
            اگر (!f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ.شامل_ہے(اگلا_Char)) {
                ملا_Breaks.popi();
                corrected_عدد_Breaks--;
            }
        } ورنہ {
            ملا_Breaks.popi();
            corrected_عدد_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    واپس corrected_عدد_Breaks;
}

خالی CjkBreakEngine::initJapanesePhraseParameter(UError_رمز& error) {
    بھرو_Japanese_Extensions(error);
    بھرو_Hiragana(error);
}

خالی CjkBreakEngine::بھرو_Japanese_Extensions(UError_رمز& error) {
    ثابت حرف* لیبل = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    اگر (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(لیبل, error);
        جب (U_SUCCESS(error) && bundle.has_اگلا()) {
            f_Skip_سیٹ.puti(bundle.get_اگلا_String(error), 1, error);
        }
    }
}

خالی CjkBreakEngine::بھرو_Hiragana(UError_رمز& error) {
    Unicode_سیٹ hiragana_لفظ_سیٹ(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_لفظ_سیٹ.compact();
    Unicode_سیٹ_Iterator iterator(hiragana_لفظ_سیٹ);
    جب (iterator.next()) {
        f_Skip_سیٹ.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#اگر_ختم

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */

