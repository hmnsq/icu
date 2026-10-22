// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 *******************************************************************************
 */

#অন্তর্ভাব <utility>

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "brkeng.h"
#অন্তর্ভাব "dictbe.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/chariter.h"
#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "unicode/ubrk.h"
#অন্তর্ভাব "unicode/usetiter.h"
#অন্তর্ভাব "ubrkimpl.h"
#অন্তর্ভাব "utracimp.h"
#অন্তর্ভাব "uvectr32.h"
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "unicode/normlzr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "dictionarydata.h"

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

DictionaryBreakEngine::DictionaryBreakEngine() {
}

DictionaryBreakEngine::~DictionaryBreakEngine() {
}

UBool
DictionaryBreakEngine::handles(UChar32 c, ধ্রুবক অক্ষর*) ধ্রুবক {
    ফেরত fSet.contains(c);
}

int32_t
DictionaryBreakEngine::findBreaks( UText *text,
                                 int32_t startPos,
                                 int32_t endPos,
                                 UVector32 &foundBreaks,
                                 UBool isPhraseBreaking,
                                 UErrorCode& status) ধ্রুবক {
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
    যখন ((current = static_cast<int32_t>(utext_get_Native_সূচক(text))) < endPos && fSet.contains(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    ফলাফল = divideUpDictionaryRange(text, rangeStart, rangeEnd, foundBreaks, isPhraseBreaking, status);
    utext_set_Native_সূচক(text, current);
    
    ফেরত ফলাফল;
}

শূন্য
DictionaryBreakEngine::setCharacters( ধ্রুবক UnicodeSet &set ) {
    fSet = set;
    // Compact for caching
    fSet.compact();
}

/*
 ******************************************************************
 * PossibleWord
 */

// Helper class for improving readability of the Thai/Lao/Khmer word break
// algorithm. The implementation is completely inline.

// List size, limited by the maximum number of words in the dictionary
// that form a nested sequence.
স্থির ধ্রুবক int32_t POSSIBLE_WORD_LIST_MAX = 20;

শ্রেণী PossibleWord {
ব্যক্তিগত:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   prefix;     // The longest match with a dictionary word
    int32_t   offset;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_WORD_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_WORD_LIST_MAX];   // Word Lengths, in code points.

সর্বজনীন:
    PossibleWord() : count(0), prefix(0), offset(-1), mark(0), current(0) {}
    ~PossibleWord() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *text, DictionaryMatcher *dict, int32_t rangeEnd );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *text );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     backUp( UText *text );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longestPrefix() { ফেরত prefix; }
  
    // Mark the current candidate as the one we like
    শূন্য      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { ফেরত cpLengths[mark]; }
};


int32_t PossibleWord::candidates( UText *text, DictionaryMatcher *dict, int32_t rangeEnd ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t start = static_cast<int32_t>(utext_get_Native_সূচক(text));
    যদি (start != offset) {
        offset = start;
        count = dict->matches(text, rangeEnd-start, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, nullptr, &prefix);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        যদি (count <= 0) {
            utext_set_Native_সূচক(text, start);
        }
    }
    যদি (count > 0) {
        utext_set_Native_সূচক(text, start+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    ফেরত count;
}

int32_t
PossibleWord::acceptMarked( UText *text ) {
    utext_set_Native_সূচক(text, offset + cuLengths[mark]);
    ফেরত cuLengths[mark];
}


UBool
PossibleWord::backUp( UText *text ) {
    যদি (current > 0) {
        utext_set_Native_সূচক(text, offset + cuLengths[--current]);
        ফেরত সত্য;
    }
    ফেরত মিথ্যা;
}

/*
 ******************************************************************
 * ThaiBreakEngine
 */

// How many words in a row are "good enough"?
স্থির ধ্রুবক int32_t THAI_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
স্থির ধ্রুবক int32_t THAI_মূল_COMBINE_THRESHOLD = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
স্থির ধ্রুবক int32_t THAI_PREFIX_COMBINE_THRESHOLD = 3;

// Elision character
স্থির ধ্রুবক int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
স্থির ধ্রুবক int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
স্থির ধ্রুবক int32_t THAI_MIN_WORD = 2;

// Minimum number of characters for two words
স্থির ধ্রুবক int32_t THAI_MIN_WORD_SPAN = THAI_MIN_WORD * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ENTRY(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    UnicodeSet thaiWordSet(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), status);
    যদি (U_SUCCESS(status)) {
        setCharacters(thaiWordSet);
    }
    fMarkSet.applyPattern(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), status);
    fMarkSet.add(0x0020);
    fEndWordSet = thaiWordSet;
    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    fSuffixSet.add(THAI_PAIYANNOI);
    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    fMarkSet.compact();
    fEndWordSet.compact();
    fBeginWordSet.compact();
    fSuffixSet.compact();
    UTRACE_EXIT_STATUS(status);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    delete fDictionary;
}

int32_t
ThaiBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t rangeStart,
                                                int32_t rangeEnd,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত 0;
    utext_set_Native_সূচক(text, rangeStart);
    utext_moveIndex32(text, THAI_MIN_WORD_SPAN);
    যদি (utext_get_Native_সূচক(text) >= rangeEnd) {
        ফেরত 0;       // Not enough characters for two words
    }
    utext_set_Native_সূচক(text, rangeStart);


    uint32_t শব্দগুলি_Found = 0;
    int32_t cp_Word_দৈর্ঘ্য = 0;    // Word Length in Code Points.
    int32_t cu_Word_দৈর্ঘ্য = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    PossibleWord শব্দগুলি[THAI_LOOKAHEAD];
    
    utext_set_Native_সূচক(text, rangeStart);
    
    যখন (U_SUCCESS(status) && (current = static_cast<int32_t>(utext_get_Native_সূচক(text))) < rangeEnd) {
        cp_Word_দৈর্ঘ্য = 0;
        cu_Word_দৈর্ঘ্য = 0;

        // Look for candidate words at the current position
        int32_t candidates = শব্দগুলি[শব্দগুলি_Found%THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
        
        // If we found exactly one, use that
        যদি (candidates == 1) {
            cu_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % THAI_LOOKAHEAD].acceptMarked(text);
            cp_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % THAI_LOOKAHEAD].markedCPLength();
            শব্দগুলি_Found += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        অন্যথা যদি (candidates > 1) {
            // If we're already at the end of the range, we're done
            যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) >= rangeEnd) {
                যাও foundBest;
            }
            করো {
                যদি (শব্দগুলি[(শব্দগুলি_Found + 1) % THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    শব্দগুলি[শব্দগুলি_Found%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) >= rangeEnd) {
                        যাও foundBest;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    করো {
                        // If we find a third word, stop right away
                        যদি (শব্দগুলি[(শব্দগুলি_Found + 2) % THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd)) {
                            শব্দগুলি[শব্দগুলি_Found % THAI_LOOKAHEAD].markCurrent();
                            যাও foundBest;
                        }
                    }
                    যখন (শব্দগুলি[(শব্দগুলি_Found + 1) % THAI_LOOKAHEAD].backUp(text));
                }
            }
            যখন (শব্দগুলি[শব্দগুলি_Found % THAI_LOOKAHEAD].backUp(text));
foundBest:
            // Set UText position to after the accepted word.
            cu_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % THAI_LOOKAHEAD].acceptMarked(text);
            cp_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % THAI_LOOKAHEAD].markedCPLength();
            শব্দগুলি_Found += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) < rangeEnd && cp_Word_দৈর্ঘ্য < THAI_মূল_COMBINE_THRESHOLD) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            যদি (শব্দগুলি[শব্দগুলি_Found % THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                  && (cu_Word_দৈর্ঘ্য == 0
                      || শব্দগুলি[শব্দগুলি_Found%THAI_LOOKAHEAD].longestPrefix() < THAI_PREFIX_COMBINE_THRESHOLD)) {
                // Look for a plausible word boundary
                int32_t remaining = rangeEnd - (current+cu_Word_দৈর্ঘ্য);
                UChar32 pc;
                int32_t chars = 0;
                জন্য (;;) {
                    int32_t pc_সূচক = static_cast<int32_t>(utext_get_Native_সূচক(text));
                    pc = utext_next32(text);
                    int32_t pc_আকার = static_cast<int32_t>(utext_get_Native_সূচক(text)) - pc_সূচক;
                    chars += pc_আকার;
                    remaining -= pc_আকার;
                    যদি (remaining <= 0) {
                        বিরতি;
                    }
                    uc = utext_current32(text);
                    যদি (fEndWordSet.contains(pc) && fBeginWordSet.contains(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t অঙ্ক_candidates = শব্দগুলি[(শব্দগুলি_Found + 1) % THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
                        utext_set_Native_সূচক(text, current + cu_Word_দৈর্ঘ্য + chars);
                        যদি (অঙ্ক_candidates > 0) {
                            বিরতি;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                যদি (cu_Word_দৈর্ঘ্য <= 0) {
                    শব্দগুলি_Found += 1;
                }
                
                // Update the length with the passed-over characters
                cu_Word_দৈর্ঘ্য += chars;
            }
            অন্যথা {
                // Back up to where we were for next iteration
                utext_set_Native_সূচক(text, current+cu_Word_দৈর্ঘ্য);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        যখন ((currPos = static_cast<int32_t>(utext_get_Native_সূচক(text))) < rangeEnd && fMarkSet.contains(utext_current32(text))) {
            utext_next32(text);
            cu_Word_দৈর্ঘ্য += static_cast<int32_t>(utext_get_Native_সূচক(text)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) < rangeEnd && cu_Word_দৈর্ঘ্য > 0) {
            যদি (শব্দগুলি[শব্দগুলি_Found%THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                && fSuffixSet.contains(uc = utext_current32(text))) {
                যদি (uc == THAI_PAIYANNOI) {
                    যদি (!fSuffixSet.contains(utext_previous32(text))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(text);
                        int32_t paiyannoi_সূচক = static_cast<int32_t>(utext_get_Native_সূচক(text));
                        utext_next32(text);
                        cu_Word_দৈর্ঘ্য += static_cast<int32_t>(utext_get_Native_সূচক(text)) - paiyannoi_সূচক; // Add PAIYANNOI to word
                        uc = utext_current32(text);     // Fetch next character
                    }
                    অন্যথা {
                        // Restore prior position
                        utext_next32(text);
                    }
                }
                যদি (uc == THAI_MAIYAMOK) {
                    যদি (utext_previous32(text) != THAI_MAIYAMOK) {
                        // Skip over previous end and MAIYAMOK
                        utext_next32(text);
                        int32_t maiyamok_সূচক = static_cast<int32_t>(utext_get_Native_সূচক(text));
                        utext_next32(text);
                        cu_Word_দৈর্ঘ্য += static_cast<int32_t>(utext_get_Native_সূচক(text)) - maiyamok_সূচক; // Add MAIYAMOK to word
                    }
                    অন্যথা {
                        // Restore prior position
                        utext_next32(text);
                    }
                }
            }
            অন্যথা {
                utext_set_Native_সূচক(text, current+cu_Word_দৈর্ঘ্য);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        যদি (cu_Word_দৈর্ঘ্য > 0) {
            foundBreaks.push((current+cu_Word_দৈর্ঘ্য), status);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    যদি (foundBreaks.peeki() >= rangeEnd) {
        (শূন্য) foundBreaks.popi();
        শব্দগুলি_Found -= 1;
    }

    ফেরত শব্দগুলি_Found;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
স্থির ধ্রুবক int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
স্থির ধ্রুবক int32_t LAO_মূল_COMBINE_THRESHOLD = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
স্থির ধ্রুবক int32_t LAO_PREFIX_COMBINE_THRESHOLD = 3;

// Minimum word size
স্থির ধ্রুবক int32_t LAO_MIN_WORD = 2;

// Minimum number of characters for two words
স্থির ধ্রুবক int32_t LAO_MIN_WORD_SPAN = LAO_MIN_WORD * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ENTRY(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    UnicodeSet laoWordSet(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), status);
    যদি (U_SUCCESS(status)) {
        setCharacters(laoWordSet);
    }
    fMarkSet.applyPattern(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), status);
    fMarkSet.add(0x0020);
    fEndWordSet = laoWordSet;
    fEndWordSet.remove(0x0EC0, 0x0EC4);     // prefix vowels
    fBeginWordSet.add(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    fBeginWordSet.add(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    fBeginWordSet.add(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    fMarkSet.compact();
    fEndWordSet.compact();
    fBeginWordSet.compact();
    UTRACE_EXIT_STATUS(status);
}

LaoBreakEngine::~LaoBreakEngine() {
    delete fDictionary;
}

int32_t
LaoBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t rangeStart,
                                                int32_t rangeEnd,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত 0;
    যদি ((rangeEnd - rangeStart) < LAO_MIN_WORD_SPAN) {
        ফেরত 0;       // Not enough characters for two words
    }

    uint32_t শব্দগুলি_Found = 0;
    int32_t cp_Word_দৈর্ঘ্য = 0;
    int32_t cu_Word_দৈর্ঘ্য = 0;
    int32_t current;
    PossibleWord শব্দগুলি[LAO_LOOKAHEAD];

    utext_set_Native_সূচক(text, rangeStart);

    যখন (U_SUCCESS(status) && (current = static_cast<int32_t>(utext_get_Native_সূচক(text))) < rangeEnd) {
        cu_Word_দৈর্ঘ্য = 0;
        cp_Word_দৈর্ঘ্য = 0;

        // Look for candidate words at the current position
        int32_t candidates = শব্দগুলি[শব্দগুলি_Found%LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
        
        // If we found exactly one, use that
        যদি (candidates == 1) {
            cu_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % LAO_LOOKAHEAD].acceptMarked(text);
            cp_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % LAO_LOOKAHEAD].markedCPLength();
            শব্দগুলি_Found += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        অন্যথা যদি (candidates > 1) {
            // If we're already at the end of the range, we're done
            যদি (utext_get_Native_সূচক(text) >= rangeEnd) {
                যাও foundBest;
            }
            করো {
                যদি (শব্দগুলি[(শব্দগুলি_Found + 1) % LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    শব্দগুলি[শব্দগুলি_Found%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) >= rangeEnd) {
                        যাও foundBest;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    করো {
                        // If we find a third word, stop right away
                        যদি (শব্দগুলি[(শব্দগুলি_Found + 2) % LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd)) {
                            শব্দগুলি[শব্দগুলি_Found % LAO_LOOKAHEAD].markCurrent();
                            যাও foundBest;
                        }
                    }
                    যখন (শব্দগুলি[(শব্দগুলি_Found + 1) % LAO_LOOKAHEAD].backUp(text));
                }
            }
            যখন (শব্দগুলি[শব্দগুলি_Found % LAO_LOOKAHEAD].backUp(text));
foundBest:
            cu_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % LAO_LOOKAHEAD].acceptMarked(text);
            cp_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % LAO_LOOKAHEAD].markedCPLength();
            শব্দগুলি_Found += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) < rangeEnd && cp_Word_দৈর্ঘ্য < LAO_মূল_COMBINE_THRESHOLD) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            যদি (শব্দগুলি[শব্দগুলি_Found % LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                  && (cu_Word_দৈর্ঘ্য == 0
                      || শব্দগুলি[শব্দগুলি_Found%LAO_LOOKAHEAD].longestPrefix() < LAO_PREFIX_COMBINE_THRESHOLD)) {
                // Look for a plausible word boundary
                int32_t remaining = rangeEnd - (current + cu_Word_দৈর্ঘ্য);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                জন্য (;;) {
                    int32_t pc_সূচক = static_cast<int32_t>(utext_get_Native_সূচক(text));
                    pc = utext_next32(text);
                    int32_t pc_আকার = static_cast<int32_t>(utext_get_Native_সূচক(text)) - pc_সূচক;
                    chars += pc_আকার;
                    remaining -= pc_আকার;
                    যদি (remaining <= 0) {
                        বিরতি;
                    }
                    uc = utext_current32(text);
                    যদি (fEndWordSet.contains(pc) && fBeginWordSet.contains(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t অঙ্ক_candidates = শব্দগুলি[(শব্দগুলি_Found + 1) % LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
                        utext_set_Native_সূচক(text, current + cu_Word_দৈর্ঘ্য + chars);
                        যদি (অঙ্ক_candidates > 0) {
                            বিরতি;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                যদি (cu_Word_দৈর্ঘ্য <= 0) {
                    শব্দগুলি_Found += 1;
                }
                
                // Update the length with the passed-over characters
                cu_Word_দৈর্ঘ্য += chars;
            }
            অন্যথা {
                // Back up to where we were for next iteration
                utext_set_Native_সূচক(text, current + cu_Word_দৈর্ঘ্য);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        যখন ((currPos = static_cast<int32_t>(utext_get_Native_সূচক(text))) < rangeEnd && fMarkSet.contains(utext_current32(text))) {
            utext_next32(text);
            cu_Word_দৈর্ঘ্য += static_cast<int32_t>(utext_get_Native_সূচক(text)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        যদি (cu_Word_দৈর্ঘ্য > 0) {
            foundBreaks.push((current+cu_Word_দৈর্ঘ্য), status);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    যদি (foundBreaks.peeki() >= rangeEnd) {
        (শূন্য) foundBreaks.popi();
        শব্দগুলি_Found -= 1;
    }

    ফেরত শব্দগুলি_Found;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
স্থির ধ্রুবক int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
স্থির ধ্রুবক int32_t BURMESE_মূল_COMBINE_THRESHOLD = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
স্থির ধ্রুবক int32_t BURMESE_PREFIX_COMBINE_THRESHOLD = 3;

// Minimum word size
স্থির ধ্রুবক int32_t BURMESE_MIN_WORD = 2;

// Minimum number of characters for two words
স্থির ধ্রুবক int32_t BURMESE_MIN_WORD_SPAN = BURMESE_MIN_WORD * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ENTRY(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    fBeginWordSet.add(0x1000, 0x102A);      // basic consonants and independent vowels
    fEndWordSet.applyPattern(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), status);
    fMarkSet.applyPattern(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), status);
    fMarkSet.add(0x0020);
    যদি (U_SUCCESS(status)) {
        setCharacters(fEndWordSet);
    }

    // Compact for caching.
    fMarkSet.compact();
    fEndWordSet.compact();
    fBeginWordSet.compact();
    UTRACE_EXIT_STATUS(status);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    delete fDictionary;
}

int32_t
BurmeseBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t rangeStart,
                                                int32_t rangeEnd,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status ) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত 0;
    যদি ((rangeEnd - rangeStart) < BURMESE_MIN_WORD_SPAN) {
        ফেরত 0;       // Not enough characters for two words
    }

    uint32_t শব্দগুলি_Found = 0;
    int32_t cp_Word_দৈর্ঘ্য = 0;
    int32_t cu_Word_দৈর্ঘ্য = 0;
    int32_t current;
    PossibleWord শব্দগুলি[BURMESE_LOOKAHEAD];

    utext_set_Native_সূচক(text, rangeStart);

    যখন (U_SUCCESS(status) && (current = static_cast<int32_t>(utext_get_Native_সূচক(text))) < rangeEnd) {
        cu_Word_দৈর্ঘ্য = 0;
        cp_Word_দৈর্ঘ্য = 0;

        // Look for candidate words at the current position
        int32_t candidates = শব্দগুলি[শব্দগুলি_Found%BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
        
        // If we found exactly one, use that
        যদি (candidates == 1) {
            cu_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % BURMESE_LOOKAHEAD].acceptMarked(text);
            cp_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % BURMESE_LOOKAHEAD].markedCPLength();
            শব্দগুলি_Found += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        অন্যথা যদি (candidates > 1) {
            // If we're already at the end of the range, we're done
            যদি (utext_get_Native_সূচক(text) >= rangeEnd) {
                যাও foundBest;
            }
            করো {
                যদি (শব্দগুলি[(শব্দগুলি_Found + 1) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    শব্দগুলি[শব্দগুলি_Found%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) >= rangeEnd) {
                        যাও foundBest;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    করো {
                        // If we find a third word, stop right away
                        যদি (শব্দগুলি[(শব্দগুলি_Found + 2) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd)) {
                            শব্দগুলি[শব্দগুলি_Found % BURMESE_LOOKAHEAD].markCurrent();
                            যাও foundBest;
                        }
                    }
                    যখন (শব্দগুলি[(শব্দগুলি_Found + 1) % BURMESE_LOOKAHEAD].backUp(text));
                }
            }
            যখন (শব্দগুলি[শব্দগুলি_Found % BURMESE_LOOKAHEAD].backUp(text));
foundBest:
            cu_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % BURMESE_LOOKAHEAD].acceptMarked(text);
            cp_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % BURMESE_LOOKAHEAD].markedCPLength();
            শব্দগুলি_Found += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) < rangeEnd && cp_Word_দৈর্ঘ্য < BURMESE_মূল_COMBINE_THRESHOLD) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            যদি (শব্দগুলি[শব্দগুলি_Found % BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                  && (cu_Word_দৈর্ঘ্য == 0
                      || শব্দগুলি[শব্দগুলি_Found%BURMESE_LOOKAHEAD].longestPrefix() < BURMESE_PREFIX_COMBINE_THRESHOLD)) {
                // Look for a plausible word boundary
                int32_t remaining = rangeEnd - (current + cu_Word_দৈর্ঘ্য);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                জন্য (;;) {
                    int32_t pc_সূচক = static_cast<int32_t>(utext_get_Native_সূচক(text));
                    pc = utext_next32(text);
                    int32_t pc_আকার = static_cast<int32_t>(utext_get_Native_সূচক(text)) - pc_সূচক;
                    chars += pc_আকার;
                    remaining -= pc_আকার;
                    যদি (remaining <= 0) {
                        বিরতি;
                    }
                    uc = utext_current32(text);
                    যদি (fEndWordSet.contains(pc) && fBeginWordSet.contains(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t অঙ্ক_candidates = শব্দগুলি[(শব্দগুলি_Found + 1) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
                        utext_set_Native_সূচক(text, current + cu_Word_দৈর্ঘ্য + chars);
                        যদি (অঙ্ক_candidates > 0) {
                            বিরতি;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                যদি (cu_Word_দৈর্ঘ্য <= 0) {
                    শব্দগুলি_Found += 1;
                }
                
                // Update the length with the passed-over characters
                cu_Word_দৈর্ঘ্য += chars;
            }
            অন্যথা {
                // Back up to where we were for next iteration
                utext_set_Native_সূচক(text, current + cu_Word_দৈর্ঘ্য);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        যখন ((currPos = static_cast<int32_t>(utext_get_Native_সূচক(text))) < rangeEnd && fMarkSet.contains(utext_current32(text))) {
            utext_next32(text);
            cu_Word_দৈর্ঘ্য += static_cast<int32_t>(utext_get_Native_সূচক(text)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        যদি (cu_Word_দৈর্ঘ্য > 0) {
            foundBreaks.push((current+cu_Word_দৈর্ঘ্য), status);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    যদি (foundBreaks.peeki() >= rangeEnd) {
        (শূন্য) foundBreaks.popi();
        শব্দগুলি_Found -= 1;
    }

    ফেরত শব্দগুলি_Found;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
স্থির ধ্রুবক int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
স্থির ধ্রুবক int32_t KHMER_মূল_COMBINE_THRESHOLD = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
স্থির ধ্রুবক int32_t KHMER_PREFIX_COMBINE_THRESHOLD = 3;

// Minimum word size
স্থির ধ্রুবক int32_t KHMER_MIN_WORD = 2;

// Minimum number of characters for two words
স্থির ধ্রুবক int32_t KHMER_MIN_WORD_SPAN = KHMER_MIN_WORD * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ENTRY(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    UnicodeSet khmerWordSet(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), status);
    যদি (U_SUCCESS(status)) {
        setCharacters(khmerWordSet);
    }
    fMarkSet.applyPattern(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), status);
    fMarkSet.add(0x0020);
    fEndWordSet = khmerWordSet;
    fBeginWordSet.add(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    fEndWordSet.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    fMarkSet.compact();
    fEndWordSet.compact();
    fBeginWordSet.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_STATUS(status);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    delete fDictionary;
}

int32_t
KhmerBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t rangeStart,
                                                int32_t rangeEnd,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status ) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত 0;
    যদি ((rangeEnd - rangeStart) < KHMER_MIN_WORD_SPAN) {
        ফেরত 0;       // Not enough characters for two words
    }

    uint32_t শব্দগুলি_Found = 0;
    int32_t cp_Word_দৈর্ঘ্য = 0;
    int32_t cu_Word_দৈর্ঘ্য = 0;
    int32_t current;
    PossibleWord শব্দগুলি[KHMER_LOOKAHEAD];

    utext_set_Native_সূচক(text, rangeStart);

    যখন (U_SUCCESS(status) && (current = static_cast<int32_t>(utext_get_Native_সূচক(text))) < rangeEnd) {
        cu_Word_দৈর্ঘ্য = 0;
        cp_Word_দৈর্ঘ্য = 0;

        // Look for candidate words at the current position
        int32_t candidates = শব্দগুলি[শব্দগুলি_Found%KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);

        // If we found exactly one, use that
        যদি (candidates == 1) {
            cu_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].acceptMarked(text);
            cp_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].markedCPLength();
            শব্দগুলি_Found += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        অন্যথা যদি (candidates > 1) {
            // If we're already at the end of the range, we're done
            যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) >= rangeEnd) {
                যাও foundBest;
            }
            করো {
                যদি (শব্দগুলি[(শব্দগুলি_Found + 1) % KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) >= rangeEnd) {
                        যাও foundBest;
                    }

                    // See if any of the possible second words is followed by a third word
                    করো {
                        // If we find a third word, stop right away
                        যদি (শব্দগুলি[(শব্দগুলি_Found + 2) % KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd)) {
                            শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].markCurrent();
                            যাও foundBest;
                        }
                    }
                    যখন (শব্দগুলি[(শব্দগুলি_Found + 1) % KHMER_LOOKAHEAD].backUp(text));
                }
            }
            যখন (শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].backUp(text));
foundBest:
            cu_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].acceptMarked(text);
            cp_Word_দৈর্ঘ্য = শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].markedCPLength();
            শব্দগুলি_Found += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        যদি (static_cast<int32_t>(utext_get_Native_সূচক(text)) < rangeEnd && cp_Word_দৈর্ঘ্য < KHMER_মূল_COMBINE_THRESHOLD) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            যদি (শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                  && (cu_Word_দৈর্ঘ্য == 0
                      || শব্দগুলি[শব্দগুলি_Found % KHMER_LOOKAHEAD].longestPrefix() < KHMER_PREFIX_COMBINE_THRESHOLD)) {
                // Look for a plausible word boundary
                int32_t remaining = rangeEnd - (current+cu_Word_দৈর্ঘ্য);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                জন্য (;;) {
                    int32_t pc_সূচক = static_cast<int32_t>(utext_get_Native_সূচক(text));
                    pc = utext_next32(text);
                    int32_t pc_আকার = static_cast<int32_t>(utext_get_Native_সূচক(text)) - pc_সূচক;
                    chars += pc_আকার;
                    remaining -= pc_আকার;
                    যদি (remaining <= 0) {
                        বিরতি;
                    }
                    uc = utext_current32(text);
                    যদি (fEndWordSet.contains(pc) && fBeginWordSet.contains(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t অঙ্ক_candidates = শব্দগুলি[(শব্দগুলি_Found + 1) % KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
                        utext_set_Native_সূচক(text, current+cu_Word_দৈর্ঘ্য+chars);
                        যদি (অঙ্ক_candidates > 0) {
                            বিরতি;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                যদি (cu_Word_দৈর্ঘ্য <= 0) {
                    শব্দগুলি_Found += 1;
                }

                // Update the length with the passed-over characters
                cu_Word_দৈর্ঘ্য += chars;
            }
            অন্যথা {
                // Back up to where we were for next iteration
                utext_set_Native_সূচক(text, current+cu_Word_দৈর্ঘ্য);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        যখন ((currPos = static_cast<int32_t>(utext_get_Native_সূচক(text))) < rangeEnd && fMarkSet.contains(utext_current32(text))) {
            utext_next32(text);
            cu_Word_দৈর্ঘ্য += static_cast<int32_t>(utext_get_Native_সূচক(text)) - currPos;
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
        যদি (cu_Word_দৈর্ঘ্য > 0) {
            foundBreaks.push((current+cu_Word_দৈর্ঘ্য), status);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    যদি (foundBreaks.peeki() >= rangeEnd) {
        (শূন্য) foundBreaks.popi();
        শব্দগুলি_Found -= 1;
    }

    ফেরত শব্দগুলি_Found;
}

#যদি !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
স্থির ধ্রুবক uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UErrorCode &status)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(মিথ্যা) {
    UTRACE_ENTRY(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = nullptr;
    nfkcNorm2 = Normalizer2::getNFKCInstance(status);
    // Korean dictionary only includes Hangul syllables
    fHangulWordSet.applyPattern(UnicodeString(u"[\\uac00-\\ud7a3]"), status);
    fHangulWordSet.compact();
    // Digits, open puncutation and Alphabetic characters.
    fDigitOrOpenPunctuationOrAlphabetSet.applyPattern(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), status);
    fDigitOrOpenPunctuationOrAlphabetSet.compact();
    fClosePunctuationSet.applyPattern(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), status);
    fClosePunctuationSet.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    যদি (type == kKorean) {
        যদি (U_SUCCESS(status)) {
            setCharacters(fHangulWordSet);
        }
    } অন্যথা { // Chinese and Japanese
        UnicodeSet cjSet(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), status);
        isCj = সত্য;
        যদি (U_SUCCESS(status)) {
            setCharacters(cjSet);
#যদি UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = নতুন MlBreakEngine(fDigitOrOpenPunctuationOrAlphabetSet,
                                               fClosePunctuationSet, status);
            যদি (fMlBreakEngine == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
#অন্যথা
            initJapanesePhraseParameter(status);
#endif
        }
    }
    UTRACE_EXIT_STATUS(status);
}

CjkBreakEngine::~CjkBreakEngine(){
    delete fDictionary;
    delete fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
স্থির ধ্রুবক int32_t k_Max_Katakana_দৈর্ঘ্য = 8;
স্থির ধ্রুবক int32_t k_Max_Katakana_Group_দৈর্ঘ্য = 20;
স্থির ধ্রুবক uint32_t maxSnlp = 255;

স্থির অন্তঃরেখ uint32_t getKatakanaCost(int32_t word_দৈর্ঘ্য){
    //TODO: fill array with actual values from dictionary!
    স্থির ধ্রুবক uint32_t katakanaCost[k_Max_Katakana_দৈর্ঘ্য + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    ফেরত (word_দৈর্ঘ্য > k_Max_Katakana_দৈর্ঘ্য) ? 8192 : katakanaCost[word_দৈর্ঘ্য];
}

স্থির অন্তঃরেখ যৌক্তিক isKatakana(UChar32 value) {
    ফেরত (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

স্থির অন্তঃরেখ int32_t utext_i32_flag(int32_t bit_সূচক) {
    ফেরত static_cast<int32_t>(1) << bit_সূচক;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divideUpDictionaryRange( UText *inText,
        int32_t rangeStart,
        int32_t rangeEnd,
        UVector32 &foundBreaks,
        UBool isPhraseBreaking,
        UErrorCode& status) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত 0;
    যদি (rangeStart >= rangeEnd) {
        ফেরত 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString inString;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    LocalPointer<UVector32>     প্রবেশ_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    যদি ((inText->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         inText->chunkNativeStart <= rangeStart &&
         inText->chunkNativeLimit >= rangeEnd   &&
         inText->nativeIndexingLimit >= rangeEnd - inText->chunkNativeStart) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        inString.setTo(মিথ্যা,
                       inText->chunkContents + rangeStart - inText->chunkNativeStart,
                       rangeEnd - rangeStart);
    } অন্যথা {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_set_Native_সূচক(inText, rangeStart);
        int32_t limit = rangeEnd;
        U_ASSERT(limit <= utext_native_দৈর্ঘ্য(inText));
        যদি (limit > utext_native_দৈর্ঘ্য(inText)) {
            limit = static_cast<int32_t>(utext_native_দৈর্ঘ্য(inText));
        }
        প্রবেশ_Map.adoptInsteadAndCheckErrorCode(নতুন UVector32(status), status);
        যদি (U_FAILURE(status)) {
            ফেরত 0;
        }
        যখন (utext_get_Native_সূচক(inText) < limit) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_সূচক(inText));
            UChar32 c = utext_next32(inText);
            U_ASSERT(c != U_SENTINEL);
            inString.append(c);
            যখন (প্রবেশ_Map->size() < inString.length()) {
                প্রবেশ_Map->addElement(nativePosition, status);
            }
        }
        প্রবেশ_Map->addElement(limit, status);
    }


    যদি (!nfkcNorm2->isNormalized(inString, status)) {
        UnicodeString normalized_প্রবেশ;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        LocalPointer<UVector32> normalizedMap(নতুন UVector32(status), status);
        যদি (U_FAILURE(status)) {
            ফেরত 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        জন্য (int32_t srcI = 0; srcI < inString.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragmentStartI = srcI;
            UChar32 c = inString.char32At(srcI);
            জন্য (;;) {
                fragment.append(c);
                srcI = inString.moveIndex32(srcI, 1);
                যদি (srcI == inString.length()) {
                    বিরতি;
                }
                c = inString.char32At(srcI);
                যদি (nfkcNorm2->hasBoundaryBefore(c)) {
                    বিরতি;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, status);
            normalized_প্রবেশ.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragmentOriginalStart = প্রবেশ_Map.isValid() ?
                    প্রবেশ_Map->elementAti(fragmentStartI) : fragmentStartI+rangeStart;
            যখন (normalizedMap->size() < normalized_প্রবেশ.length()) {
                normalizedMap->addElement(fragmentOriginalStart, status);
                যদি (U_FAILURE(status)) {
                    বিরতি;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_প্রবেশ.length());
        int32_t nativeEnd = প্রবেশ_Map.isValid() ?
                প্রবেশ_Map->elementAti(inString.length()) : inString.length()+rangeStart;
        normalizedMap->addElement(nativeEnd, status);

        প্রবেশ_Map = std::move(normalizedMap);
        inString = std::move(normalized_প্রবেশ);
    }

    int32_t অঙ্ক_Code_Pts = inString.গুনতি_Char32();
    যদি (অঙ্ক_Code_Pts != inString.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = প্রবেশ_Map.isValid();
        যদি (!hadExistingMap) {
            প্রবেশ_Map.adoptInsteadAndCheckErrorCode(নতুন UVector32(status), status);
            যদি (U_FAILURE(status)) {
                ফেরত 0;
            }
        }
        int32_t cpIdx = 0;
        জন্য (int32_t cuIdx = 0; ; cuIdx = inString.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            যদি (hadExistingMap) {
                প্রবেশ_Map->setElementAt(প্রবেশ_Map->elementAti(cuIdx), cpIdx);
            } অন্যথা {
                প্রবেশ_Map->addElement(cuIdx+rangeStart, status);
            }
            cpIdx++;
            যদি (cuIdx == inString.length()) {
               বিরতি;
            }
        }
    }

#যদি UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    যদি (isPhraseBreaking && isCj) {
        ফেরত fMlBreakEngine->divideUpRange(inText, rangeStart, rangeEnd, foundBreaks, inString,
                                             প্রবেশ_Map, status);
    }
#endif

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 bestSnlp(অঙ্ক_Code_Pts + 1, status);
    bestSnlp.addElement(0, status);
    জন্য(int32_t i = 1; i <= অঙ্ক_Code_Pts; i++) {
        bestSnlp.addElement(kuint32max, status);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 পূর্ব(অঙ্ক_Code_Pts + 1, status);
    জন্য(int32_t i = 0; i <= অঙ্ক_Code_Pts; i++){
        পূর্ব.addElement(-1, status);
    }

    ধ্রুবক int32_t max_Word_আকার = 20;
    UVector32 মানসমূহ(অঙ্ক_Code_Pts, status);
    মানসমূহ.set_আকার(অঙ্ক_Code_Pts);
    UVector32 lengths(অঙ্ক_Code_Pts, status);
    lengths.set_আকার(অঙ্ক_Code_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &inString, &status);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    যৌক্তিক is_পূর্ব_katakana = মিথ্যা;
    জন্য (int32_t i = 0;  i < অঙ্ক_Code_Pts;  ++i, ix = inString.moveIndex32(ix, 1)) {
        যদি (static_cast<uint32_t>(bestSnlp.elementAti(i)) == kuint32max) {
            চালু;
        }

        int32_t count;
        utext_set_Native_সূচক(&fu, ix);
        count = fDictionary->matches(&fu, max_Word_আকার, অঙ্ক_Code_Pts,
                             nullptr, lengths.get_বাফার(), মানসমূহ.get_বাফার(), nullptr);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        যদি ((count == 0 || lengths.elementAti(0) != 1) &&
                !fHangulWordSet.contains(inString.char32At(ix))) {
            মানসমূহ.setElementAt(maxSnlp, count);   // 255
            lengths.setElementAt(1, count++);
        }

        জন্য (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(bestSnlp.elementAti(i)) + static_cast<uint32_t>(মানসমূহ.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            যদি (newSnlp < static_cast<uint32_t>(bestSnlp.elementAti(ln_j_i))) {
                bestSnlp.setElementAt(newSnlp, ln_j_i);
                পূর্ব.setElementAt(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        যৌক্তিক is_katakana = isKatakana(inString.char32At(ix));
        int32_t katakana_Run_দৈর্ঘ্য = 1;
        যদি (!is_পূর্ব_katakana && is_katakana) {
            int32_t j = inString.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            যখন (j < inString.length() && katakana_Run_দৈর্ঘ্য < k_Max_Katakana_Group_দৈর্ঘ্য &&
                    isKatakana(inString.char32At(j))) {
                j = inString.moveIndex32(j, 1);
                katakana_Run_দৈর্ঘ্য++;
            }
            যদি (katakana_Run_দৈর্ঘ্য < k_Max_Katakana_Group_দৈর্ঘ্য) {
                uint32_t newSnlp = bestSnlp.elementAti(i) + getKatakanaCost(katakana_Run_দৈর্ঘ্য);
                যদি (newSnlp < static_cast<uint32_t>(bestSnlp.elementAti(i + katakana_Run_দৈর্ঘ্য))) {
                    bestSnlp.setElementAt(newSnlp, i+katakana_Run_দৈর্ঘ্য);
                    পূর্ব.setElementAt(i, i+katakana_Run_দৈর্ঘ্য);  // prev[j] = i;
                }
            }
        }
        is_পূর্ব_katakana = is_katakana;
    }
    utext_close(&fu);

    // Start pushing the optimal offset index into t_boundary (t for tentative).
    // prev[numCodePts] is guaranteed to be meaningful.
    // We'll first push in the reverse order, i.e.,
    // t_boundary[0] = numCodePts, and afterwards do a swap.
    UVector32 t_boundary(অঙ্ক_Code_Pts+1, status);

    int32_t অঙ্ক_Breaks = 0;
    // No segmentation found, set boundary to end of range
    যদি (static_cast<uint32_t>(bestSnlp.elementAti(অঙ্ক_Code_Pts)) == kuint32max) {
        t_boundary.addElement(অঙ্ক_Code_Pts, status);
        অঙ্ক_Breaks++;
    } অন্যথা যদি (isPhraseBreaking) {
        t_boundary.addElement(অঙ্ক_Code_Pts, status);
        যদি(U_SUCCESS(status)) {
            অঙ্ক_Breaks++;
            int32_t পূর্ব_Idx = অঙ্ক_Code_Pts;

            int32_t codeUnitIdx = -1;
            int32_t পূর্ব_Code_Unit_Idx = -1;
            int32_t length = -1;
            জন্য (int32_t i = পূর্ব.elementAti(অঙ্ক_Code_Pts); i > 0; i = পূর্ব.elementAti(i)) {
                codeUnitIdx = inString.moveIndex32(0, i);
                পূর্ব_Code_Unit_Idx = inString.moveIndex32(0, পূর্ব_Idx);
                // Calculate the length by using the code unit.
                length = পূর্ব_Code_Unit_Idx - codeUnitIdx;
                পূর্ব_Idx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                যদি (!fSkipSet.contains_চাবি(inString.অস্থায়ী_Sub_String(codeUnitIdx, length))
                    && (!isKatakana(inString.char32At(inString.moveIndex32(codeUnitIdx, -1)))
                           || !isKatakana(inString.char32At(codeUnitIdx)))) {
                    t_boundary.addElement(i, status);
                    অঙ্ক_Breaks++;
                }
            }
        }
    } অন্যথা {
        জন্য (int32_t i = অঙ্ক_Code_Pts; i > 0; i = পূর্ব.elementAti(i)) {
            t_boundary.addElement(i, status);
            অঙ্ক_Breaks++;
        }
        U_ASSERT(পূর্ব.elementAti(t_boundary.elementAti(অঙ্ক_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    যদি (foundBreaks.size() == 0 || foundBreaks.peeki() < rangeStart) {
        t_boundary.addElement(0, status);
        অঙ্ক_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t পূর্ব_CPPos = -1;
    int32_t পূর্ব_UText_Pos = -1;
    int32_t corrected_অঙ্ক_Breaks = 0;
    জন্য (int32_t i = অঙ্ক_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > পূর্ব_CPPos);
        int32_t utextPos =  প্রবেশ_Map.isValid() ? প্রবেশ_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(utextPos >= পূর্ব_UText_Pos);
        যদি (utextPos > পূর্ব_UText_Pos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(foundBreaks.size() == 0 || foundBreaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            যদি (utextPos != rangeStart
                || (isPhraseBreaking && utextPos > 0
                       && fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_অঙ্ক_Breaks++;
            }
        } অন্যথা {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --অঙ্ক_Breaks;
        }
        পূর্ব_CPPos = cpPos;
        পূর্ব_UText_Pos = utextPos;
    }
    (শূন্য)পূর্ব_CPPos; // suppress compiler warnings about unused variable

    UChar32 পরবর্তী_Char = utext_char32At(inText, rangeEnd);
    যদি (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        যদি (isPhraseBreaking) {
            যদি (!fDigitOrOpenPunctuationOrAlphabetSet.contains(পরবর্তী_Char)) {
                foundBreaks.popi();
                corrected_অঙ্ক_Breaks--;
            }
        } অন্যথা {
            foundBreaks.popi();
            corrected_অঙ্ক_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    ফেরত corrected_অঙ্ক_Breaks;
}

শূন্য CjkBreakEngine::initJapanesePhraseParameter(UErrorCode& error) {
    loadJapaneseExtensions(error);
    loadHiragana(error);
}

শূন্য CjkBreakEngine::loadJapaneseExtensions(UErrorCode& error) {
    ধ্রুবক অক্ষর* tag = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    যদি (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(tag, error);
        যখন (U_SUCCESS(error) && bundle.has_পরবর্তী()) {
            fSkipSet.puti(bundle.get_পরবর্তী_String(error), 1, error);
        }
    }
}

শূন্য CjkBreakEngine::loadHiragana(UErrorCode& error) {
    UnicodeSet hiraganaWordSet(UnicodeString(u"[:Hiragana:]"), error);
    hiraganaWordSet.compact();
    UnicodeSetIterator iterator(hiraganaWordSet);
    যখন (iterator.next()) {
        fSkipSet.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

