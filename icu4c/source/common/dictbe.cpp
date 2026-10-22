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
DictionaryBreakEngine::handles(UChar32 c, ثابت نویسه*) ثابت {
    بازگشت fSet.contains(c);
}

int32_t
DictionaryBreakEngine::findBreaks( UText *text,
                                 int32_t startPos,
                                 int32_t endPos,
                                 UVector32 &foundBreaks,
                                 UBool isPhraseBreaking,
                                 UErrorCode& status) ثابت {
    اگر (U_FAILURE(status)) بازگشت 0;
    int32_t result = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_setNativeIndex(text, startPos);
    int32_t start = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t current;
    int32_t rangeStart;
    int32_t rangeEnd;
    UChar32 c = utext_current32(text);
    وقتی ((current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos && fSet.contains(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    rangeEnd = current;
    result = divideUpDictionaryRange(text, rangeStart, rangeEnd, foundBreaks, isPhraseBreaking, status);
    utext_setNativeIndex(text, current);
    
    بازگشت result;
}

تهی
DictionaryBreakEngine::setCharacters( ثابت UnicodeSet &set ) {
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
ایستا ثابت int32_t POSSIBLE_WORD_LIST_MAX = 20;

رده PossibleWord {
خصوصی:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   prefix;     // The longest match with a dictionary word
    int32_t   offset;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_WORD_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_WORD_LIST_MAX];   // Word Lengths, in code points.

عمومی:
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
    int32_t   longestPrefix() { بازگشت prefix; }
  
    // Mark the current candidate as the one we like
    تهی      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { بازگشت cpLengths[mark]; }
};


int32_t PossibleWord::candidates( UText *text, DictionaryMatcher *dict, int32_t rangeEnd ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t start = static_cast<int32_t>(utext_getNativeIndex(text));
    اگر (start != offset) {
        offset = start;
        count = dict->matches(text, rangeEnd-start, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, nullptr, &prefix);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        اگر (count <= 0) {
            utext_setNativeIndex(text, start);
        }
    }
    اگر (count > 0) {
        utext_setNativeIndex(text, start+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    بازگشت count;
}

int32_t
PossibleWord::acceptMarked( UText *text ) {
    utext_setNativeIndex(text, offset + cuLengths[mark]);
    بازگشت cuLengths[mark];
}


UBool
PossibleWord::backUp( UText *text ) {
    اگر (current > 0) {
        utext_setNativeIndex(text, offset + cuLengths[--current]);
        بازگشت درست;
    }
    بازگشت نادرست;
}

/*
 ******************************************************************
 * ThaiBreakEngine
 */

// How many words in a row are "good enough"?
ایستا ثابت int32_t THAI_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ایستا ثابت int32_t THAI_ریشه_COMBINE_THRESHOLD = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ایستا ثابت int32_t THAI_PREFIX_COMBINE_THRESHOLD = 3;

// Elision character
ایستا ثابت int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
ایستا ثابت int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
ایستا ثابت int32_t THAI_MIN_WORD = 2;

// Minimum number of characters for two words
ایستا ثابت int32_t THAI_MIN_WORD_SPAN = THAI_MIN_WORD * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ENTRY(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    UnicodeSet thaiWordSet(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), status);
    اگر (U_SUCCESS(status)) {
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
                                                UErrorCode& status) ثابت {
    اگر (U_FAILURE(status)) بازگشت 0;
    utext_setNativeIndex(text, rangeStart);
    utext_moveIndex32(text, THAI_MIN_WORD_SPAN);
    اگر (utext_getNativeIndex(text) >= rangeEnd) {
        بازگشت 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, rangeStart);


    uint32_t واژگان_Found = 0;
    int32_t cpWordLength = 0;    // Word Length in Code Points.
    int32_t cuWordLength = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    PossibleWord واژگان[THAI_LOOKAHEAD];
    
    utext_setNativeIndex(text, rangeStart);
    
    وقتی (U_SUCCESS(status) && (current = static_cast<int32_t>(utext_getNativeIndex(text))) < rangeEnd) {
        cpWordLength = 0;
        cuWordLength = 0;

        // Look for candidate words at the current position
        int32_t candidates = واژگان[واژگان_Found%THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
        
        // If we found exactly one, use that
        اگر (candidates == 1) {
            cuWordLength = واژگان[واژگان_Found % THAI_LOOKAHEAD].acceptMarked(text);
            cpWordLength = واژگان[واژگان_Found % THAI_LOOKAHEAD].markedCPLength();
            واژگان_Found += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وگرنه اگر (candidates > 1) {
            // If we're already at the end of the range, we're done
            اگر (static_cast<int32_t>(utext_getNativeIndex(text)) >= rangeEnd) {
                برو foundBest;
            }
            انجامبده {
                اگر (واژگان[(واژگان_Found + 1) % THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    واژگان[واژگان_Found%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    اگر (static_cast<int32_t>(utext_getNativeIndex(text)) >= rangeEnd) {
                        برو foundBest;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    انجامبده {
                        // If we find a third word, stop right away
                        اگر (واژگان[(واژگان_Found + 2) % THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd)) {
                            واژگان[واژگان_Found % THAI_LOOKAHEAD].markCurrent();
                            برو foundBest;
                        }
                    }
                    وقتی (واژگان[(واژگان_Found + 1) % THAI_LOOKAHEAD].backUp(text));
                }
            }
            وقتی (واژگان[واژگان_Found % THAI_LOOKAHEAD].backUp(text));
foundBest:
            // Set UText position to after the accepted word.
            cuWordLength = واژگان[واژگان_Found % THAI_LOOKAHEAD].acceptMarked(text);
            cpWordLength = واژگان[واژگان_Found % THAI_LOOKAHEAD].markedCPLength();
            واژگان_Found += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        اگر (static_cast<int32_t>(utext_getNativeIndex(text)) < rangeEnd && cpWordLength < THAI_ریشه_COMBINE_THRESHOLD) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            اگر (واژگان[واژگان_Found % THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                  && (cuWordLength == 0
                      || واژگان[واژگان_Found%THAI_LOOKAHEAD].longestPrefix() < THAI_PREFIX_COMBINE_THRESHOLD)) {
                // Look for a plausible word boundary
                int32_t remaining = rangeEnd - (current+cuWordLength);
                UChar32 pc;
                int32_t chars = 0;
                برای (;;) {
                    int32_t pcIndex = static_cast<int32_t>(utext_getNativeIndex(text));
                    pc = utext_next32(text);
                    int32_t pcSize = static_cast<int32_t>(utext_getNativeIndex(text)) - pcIndex;
                    chars += pcSize;
                    remaining -= pcSize;
                    اگر (remaining <= 0) {
                        بشکن;
                    }
                    uc = utext_current32(text);
                    اگر (fEndWordSet.contains(pc) && fBeginWordSet.contains(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t رقم_candidates = واژگان[(واژگان_Found + 1) % THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
                        utext_setNativeIndex(text, current + cuWordLength + chars);
                        اگر (رقم_candidates > 0) {
                            بشکن;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                اگر (cuWordLength <= 0) {
                    واژگان_Found += 1;
                }
                
                // Update the length with the passed-over characters
                cuWordLength += chars;
            }
            وگرنه {
                // Back up to where we were for next iteration
                utext_setNativeIndex(text, current+cuWordLength);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        وقتی ((currPos = static_cast<int32_t>(utext_getNativeIndex(text))) < rangeEnd && fMarkSet.contains(utext_current32(text))) {
            utext_next32(text);
            cuWordLength += static_cast<int32_t>(utext_getNativeIndex(text)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        اگر (static_cast<int32_t>(utext_getNativeIndex(text)) < rangeEnd && cuWordLength > 0) {
            اگر (واژگان[واژگان_Found%THAI_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                && fSuffixSet.contains(uc = utext_current32(text))) {
                اگر (uc == THAI_PAIYANNOI) {
                    اگر (!fSuffixSet.contains(utext_previous32(text))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(text);
                        int32_t paiyannoiIndex = static_cast<int32_t>(utext_getNativeIndex(text));
                        utext_next32(text);
                        cuWordLength += static_cast<int32_t>(utext_getNativeIndex(text)) - paiyannoiIndex; // Add PAIYANNOI to word
                        uc = utext_current32(text);     // Fetch next character
                    }
                    وگرنه {
                        // Restore prior position
                        utext_next32(text);
                    }
                }
                اگر (uc == THAI_MAIYAMOK) {
                    اگر (utext_previous32(text) != THAI_MAIYAMOK) {
                        // Skip over previous end and MAIYAMOK
                        utext_next32(text);
                        int32_t maiyamokIndex = static_cast<int32_t>(utext_getNativeIndex(text));
                        utext_next32(text);
                        cuWordLength += static_cast<int32_t>(utext_getNativeIndex(text)) - maiyamokIndex; // Add MAIYAMOK to word
                    }
                    وگرنه {
                        // Restore prior position
                        utext_next32(text);
                    }
                }
            }
            وگرنه {
                utext_setNativeIndex(text, current+cuWordLength);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        اگر (cuWordLength > 0) {
            foundBreaks.push((current+cuWordLength), status);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    اگر (foundBreaks.peeki() >= rangeEnd) {
        (تهی) foundBreaks.popi();
        واژگان_Found -= 1;
    }

    بازگشت واژگان_Found;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
ایستا ثابت int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ایستا ثابت int32_t LAO_ریشه_COMBINE_THRESHOLD = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ایستا ثابت int32_t LAO_PREFIX_COMBINE_THRESHOLD = 3;

// Minimum word size
ایستا ثابت int32_t LAO_MIN_WORD = 2;

// Minimum number of characters for two words
ایستا ثابت int32_t LAO_MIN_WORD_SPAN = LAO_MIN_WORD * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ENTRY(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    UnicodeSet laoWordSet(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), status);
    اگر (U_SUCCESS(status)) {
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
                                                UErrorCode& status) ثابت {
    اگر (U_FAILURE(status)) بازگشت 0;
    اگر ((rangeEnd - rangeStart) < LAO_MIN_WORD_SPAN) {
        بازگشت 0;       // Not enough characters for two words
    }

    uint32_t واژگان_Found = 0;
    int32_t cpWordLength = 0;
    int32_t cuWordLength = 0;
    int32_t current;
    PossibleWord واژگان[LAO_LOOKAHEAD];

    utext_setNativeIndex(text, rangeStart);

    وقتی (U_SUCCESS(status) && (current = static_cast<int32_t>(utext_getNativeIndex(text))) < rangeEnd) {
        cuWordLength = 0;
        cpWordLength = 0;

        // Look for candidate words at the current position
        int32_t candidates = واژگان[واژگان_Found%LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
        
        // If we found exactly one, use that
        اگر (candidates == 1) {
            cuWordLength = واژگان[واژگان_Found % LAO_LOOKAHEAD].acceptMarked(text);
            cpWordLength = واژگان[واژگان_Found % LAO_LOOKAHEAD].markedCPLength();
            واژگان_Found += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وگرنه اگر (candidates > 1) {
            // If we're already at the end of the range, we're done
            اگر (utext_getNativeIndex(text) >= rangeEnd) {
                برو foundBest;
            }
            انجامبده {
                اگر (واژگان[(واژگان_Found + 1) % LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    واژگان[واژگان_Found%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    اگر (static_cast<int32_t>(utext_getNativeIndex(text)) >= rangeEnd) {
                        برو foundBest;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    انجامبده {
                        // If we find a third word, stop right away
                        اگر (واژگان[(واژگان_Found + 2) % LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd)) {
                            واژگان[واژگان_Found % LAO_LOOKAHEAD].markCurrent();
                            برو foundBest;
                        }
                    }
                    وقتی (واژگان[(واژگان_Found + 1) % LAO_LOOKAHEAD].backUp(text));
                }
            }
            وقتی (واژگان[واژگان_Found % LAO_LOOKAHEAD].backUp(text));
foundBest:
            cuWordLength = واژگان[واژگان_Found % LAO_LOOKAHEAD].acceptMarked(text);
            cpWordLength = واژگان[واژگان_Found % LAO_LOOKAHEAD].markedCPLength();
            واژگان_Found += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        اگر (static_cast<int32_t>(utext_getNativeIndex(text)) < rangeEnd && cpWordLength < LAO_ریشه_COMBINE_THRESHOLD) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            اگر (واژگان[واژگان_Found % LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                  && (cuWordLength == 0
                      || واژگان[واژگان_Found%LAO_LOOKAHEAD].longestPrefix() < LAO_PREFIX_COMBINE_THRESHOLD)) {
                // Look for a plausible word boundary
                int32_t remaining = rangeEnd - (current + cuWordLength);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                برای (;;) {
                    int32_t pcIndex = static_cast<int32_t>(utext_getNativeIndex(text));
                    pc = utext_next32(text);
                    int32_t pcSize = static_cast<int32_t>(utext_getNativeIndex(text)) - pcIndex;
                    chars += pcSize;
                    remaining -= pcSize;
                    اگر (remaining <= 0) {
                        بشکن;
                    }
                    uc = utext_current32(text);
                    اگر (fEndWordSet.contains(pc) && fBeginWordSet.contains(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t رقم_candidates = واژگان[(واژگان_Found + 1) % LAO_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
                        utext_setNativeIndex(text, current + cuWordLength + chars);
                        اگر (رقم_candidates > 0) {
                            بشکن;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                اگر (cuWordLength <= 0) {
                    واژگان_Found += 1;
                }
                
                // Update the length with the passed-over characters
                cuWordLength += chars;
            }
            وگرنه {
                // Back up to where we were for next iteration
                utext_setNativeIndex(text, current + cuWordLength);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        وقتی ((currPos = static_cast<int32_t>(utext_getNativeIndex(text))) < rangeEnd && fMarkSet.contains(utext_current32(text))) {
            utext_next32(text);
            cuWordLength += static_cast<int32_t>(utext_getNativeIndex(text)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        اگر (cuWordLength > 0) {
            foundBreaks.push((current+cuWordLength), status);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    اگر (foundBreaks.peeki() >= rangeEnd) {
        (تهی) foundBreaks.popi();
        واژگان_Found -= 1;
    }

    بازگشت واژگان_Found;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
ایستا ثابت int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ایستا ثابت int32_t BURMESE_ریشه_COMBINE_THRESHOLD = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ایستا ثابت int32_t BURMESE_PREFIX_COMBINE_THRESHOLD = 3;

// Minimum word size
ایستا ثابت int32_t BURMESE_MIN_WORD = 2;

// Minimum number of characters for two words
ایستا ثابت int32_t BURMESE_MIN_WORD_SPAN = BURMESE_MIN_WORD * 2;

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
    اگر (U_SUCCESS(status)) {
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
                                                UErrorCode& status ) ثابت {
    اگر (U_FAILURE(status)) بازگشت 0;
    اگر ((rangeEnd - rangeStart) < BURMESE_MIN_WORD_SPAN) {
        بازگشت 0;       // Not enough characters for two words
    }

    uint32_t واژگان_Found = 0;
    int32_t cpWordLength = 0;
    int32_t cuWordLength = 0;
    int32_t current;
    PossibleWord واژگان[BURMESE_LOOKAHEAD];

    utext_setNativeIndex(text, rangeStart);

    وقتی (U_SUCCESS(status) && (current = static_cast<int32_t>(utext_getNativeIndex(text))) < rangeEnd) {
        cuWordLength = 0;
        cpWordLength = 0;

        // Look for candidate words at the current position
        int32_t candidates = واژگان[واژگان_Found%BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
        
        // If we found exactly one, use that
        اگر (candidates == 1) {
            cuWordLength = واژگان[واژگان_Found % BURMESE_LOOKAHEAD].acceptMarked(text);
            cpWordLength = واژگان[واژگان_Found % BURMESE_LOOKAHEAD].markedCPLength();
            واژگان_Found += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        وگرنه اگر (candidates > 1) {
            // If we're already at the end of the range, we're done
            اگر (utext_getNativeIndex(text) >= rangeEnd) {
                برو foundBest;
            }
            انجامبده {
                اگر (واژگان[(واژگان_Found + 1) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    واژگان[واژگان_Found%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    اگر (static_cast<int32_t>(utext_getNativeIndex(text)) >= rangeEnd) {
                        برو foundBest;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    انجامبده {
                        // If we find a third word, stop right away
                        اگر (واژگان[(واژگان_Found + 2) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd)) {
                            واژگان[واژگان_Found % BURMESE_LOOKAHEAD].markCurrent();
                            برو foundBest;
                        }
                    }
                    وقتی (واژگان[(واژگان_Found + 1) % BURMESE_LOOKAHEAD].backUp(text));
                }
            }
            وقتی (واژگان[واژگان_Found % BURMESE_LOOKAHEAD].backUp(text));
foundBest:
            cuWordLength = واژگان[واژگان_Found % BURMESE_LOOKAHEAD].acceptMarked(text);
            cpWordLength = واژگان[واژگان_Found % BURMESE_LOOKAHEAD].markedCPLength();
            واژگان_Found += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        اگر (static_cast<int32_t>(utext_getNativeIndex(text)) < rangeEnd && cpWordLength < BURMESE_ریشه_COMBINE_THRESHOLD) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            اگر (واژگان[واژگان_Found % BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                  && (cuWordLength == 0
                      || واژگان[واژگان_Found%BURMESE_LOOKAHEAD].longestPrefix() < BURMESE_PREFIX_COMBINE_THRESHOLD)) {
                // Look for a plausible word boundary
                int32_t remaining = rangeEnd - (current + cuWordLength);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                برای (;;) {
                    int32_t pcIndex = static_cast<int32_t>(utext_getNativeIndex(text));
                    pc = utext_next32(text);
                    int32_t pcSize = static_cast<int32_t>(utext_getNativeIndex(text)) - pcIndex;
                    chars += pcSize;
                    remaining -= pcSize;
                    اگر (remaining <= 0) {
                        بشکن;
                    }
                    uc = utext_current32(text);
                    اگر (fEndWordSet.contains(pc) && fBeginWordSet.contains(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t رقم_candidates = واژگان[(واژگان_Found + 1) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
                        utext_setNativeIndex(text, current + cuWordLength + chars);
                        اگر (رقم_candidates > 0) {
                            بشکن;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                اگر (cuWordLength <= 0) {
                    واژگان_Found += 1;
                }
                
                // Update the length with the passed-over characters
                cuWordLength += chars;
            }
            وگرنه {
                // Back up to where we were for next iteration
                utext_setNativeIndex(text, current + cuWordLength);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        وقتی ((currPos = static_cast<int32_t>(utext_getNativeIndex(text))) < rangeEnd && fMarkSet.contains(utext_current32(text))) {
            utext_next32(text);
            cuWordLength += static_cast<int32_t>(utext_getNativeIndex(text)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        اگر (cuWordLength > 0) {
            foundBreaks.push((current+cuWordLength), status);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    اگر (foundBreaks.peeki() >= rangeEnd) {
        (تهی) foundBreaks.popi();
        واژگان_Found -= 1;
    }

    بازگشت واژگان_Found;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
ایستا ثابت int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ایستا ثابت int32_t KHMER_ریشه_COMBINE_THRESHOLD = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ایستا ثابت int32_t KHMER_PREFIX_COMBINE_THRESHOLD = 3;

// Minimum word size
ایستا ثابت int32_t KHMER_MIN_WORD = 2;

// Minimum number of characters for two words
ایستا ثابت int32_t KHMER_MIN_WORD_SPAN = KHMER_MIN_WORD * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ENTRY(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    UnicodeSet khmerWordSet(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), status);
    اگر (U_SUCCESS(status)) {
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
                                                UErrorCode& status ) ثابت {
    اگر (U_FAILURE(status)) بازگشت 0;
    اگر ((rangeEnd - rangeStart) < KHMER_MIN_WORD_SPAN) {
        بازگشت 0;       // Not enough characters for two words
    }

    uint32_t واژگان_Found = 0;
    int32_t cpWordLength = 0;
    int32_t cuWordLength = 0;
    int32_t current;
    PossibleWord واژگان[KHMER_LOOKAHEAD];

    utext_setNativeIndex(text, rangeStart);

    وقتی (U_SUCCESS(status) && (current = static_cast<int32_t>(utext_getNativeIndex(text))) < rangeEnd) {
        cuWordLength = 0;
        cpWordLength = 0;

        // Look for candidate words at the current position
        int32_t candidates = واژگان[واژگان_Found%KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);

        // If we found exactly one, use that
        اگر (candidates == 1) {
            cuWordLength = واژگان[واژگان_Found % KHMER_LOOKAHEAD].acceptMarked(text);
            cpWordLength = واژگان[واژگان_Found % KHMER_LOOKAHEAD].markedCPLength();
            واژگان_Found += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        وگرنه اگر (candidates > 1) {
            // If we're already at the end of the range, we're done
            اگر (static_cast<int32_t>(utext_getNativeIndex(text)) >= rangeEnd) {
                برو foundBest;
            }
            انجامبده {
                اگر (واژگان[(واژگان_Found + 1) % KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    واژگان[واژگان_Found % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    اگر (static_cast<int32_t>(utext_getNativeIndex(text)) >= rangeEnd) {
                        برو foundBest;
                    }

                    // See if any of the possible second words is followed by a third word
                    انجامبده {
                        // If we find a third word, stop right away
                        اگر (واژگان[(واژگان_Found + 2) % KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd)) {
                            واژگان[واژگان_Found % KHMER_LOOKAHEAD].markCurrent();
                            برو foundBest;
                        }
                    }
                    وقتی (واژگان[(واژگان_Found + 1) % KHMER_LOOKAHEAD].backUp(text));
                }
            }
            وقتی (واژگان[واژگان_Found % KHMER_LOOKAHEAD].backUp(text));
foundBest:
            cuWordLength = واژگان[واژگان_Found % KHMER_LOOKAHEAD].acceptMarked(text);
            cpWordLength = واژگان[واژگان_Found % KHMER_LOOKAHEAD].markedCPLength();
            واژگان_Found += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        اگر (static_cast<int32_t>(utext_getNativeIndex(text)) < rangeEnd && cpWordLength < KHMER_ریشه_COMBINE_THRESHOLD) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            اگر (واژگان[واژگان_Found % KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd) <= 0
                  && (cuWordLength == 0
                      || واژگان[واژگان_Found % KHMER_LOOKAHEAD].longestPrefix() < KHMER_PREFIX_COMBINE_THRESHOLD)) {
                // Look for a plausible word boundary
                int32_t remaining = rangeEnd - (current+cuWordLength);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                برای (;;) {
                    int32_t pcIndex = static_cast<int32_t>(utext_getNativeIndex(text));
                    pc = utext_next32(text);
                    int32_t pcSize = static_cast<int32_t>(utext_getNativeIndex(text)) - pcIndex;
                    chars += pcSize;
                    remaining -= pcSize;
                    اگر (remaining <= 0) {
                        بشکن;
                    }
                    uc = utext_current32(text);
                    اگر (fEndWordSet.contains(pc) && fBeginWordSet.contains(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t رقم_candidates = واژگان[(واژگان_Found + 1) % KHMER_LOOKAHEAD].candidates(text, fDictionary, rangeEnd);
                        utext_setNativeIndex(text, current+cuWordLength+chars);
                        اگر (رقم_candidates > 0) {
                            بشکن;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                اگر (cuWordLength <= 0) {
                    واژگان_Found += 1;
                }

                // Update the length with the passed-over characters
                cuWordLength += chars;
            }
            وگرنه {
                // Back up to where we were for next iteration
                utext_setNativeIndex(text, current+cuWordLength);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        وقتی ((currPos = static_cast<int32_t>(utext_getNativeIndex(text))) < rangeEnd && fMarkSet.contains(utext_current32(text))) {
            utext_next32(text);
            cuWordLength += static_cast<int32_t>(utext_getNativeIndex(text)) - currPos;
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
        اگر (cuWordLength > 0) {
            foundBreaks.push((current+cuWordLength), status);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    اگر (foundBreaks.peeki() >= rangeEnd) {
        (تهی) foundBreaks.popi();
        واژگان_Found -= 1;
    }

    بازگشت واژگان_Found;
}

#اگر !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
ایستا ثابت uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UErrorCode &status)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(نادرست) {
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
    اگر (type == kKorean) {
        اگر (U_SUCCESS(status)) {
            setCharacters(fHangulWordSet);
        }
    } وگرنه { // Chinese and Japanese
        UnicodeSet cjSet(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), status);
        isCj = درست;
        اگر (U_SUCCESS(status)) {
            setCharacters(cjSet);
#اگر UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = جدید MlBreakEngine(fDigitOrOpenPunctuationOrAlphabetSet,
                                               fClosePunctuationSet, status);
            اگر (fMlBreakEngine == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
#وگرنه
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
ایستا ثابت int32_t kMaxKatakanaLength = 8;
ایستا ثابت int32_t kMaxKatakanaGroupLength = 20;
ایستا ثابت uint32_t maxSnlp = 255;

ایستا درونخطی uint32_t getKatakanaCost(int32_t wordLength){
    //TODO: fill array with actual values from dictionary!
    ایستا ثابت uint32_t katakanaCost[kMaxKatakanaLength + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    بازگشت (wordLength > kMaxKatakanaLength) ? 8192 : katakanaCost[wordLength];
}

ایستا درونخطی منطقی isKatakana(UChar32 value) {
    بازگشت (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

ایستا درونخطی int32_t utext_i32_flag(int32_t bitIndex) {
    بازگشت static_cast<int32_t>(1) << bitIndex;
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
        UErrorCode& status) ثابت {
    اگر (U_FAILURE(status)) بازگشت 0;
    اگر (rangeStart >= rangeEnd) {
        بازگشت 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString inString;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    LocalPointer<UVector32>     درون_داد_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    اگر ((inText->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         inText->chunkNativeStart <= rangeStart &&
         inText->chunkNativeLimit >= rangeEnd   &&
         inText->nativeIndexingLimit >= rangeEnd - inText->chunkNativeStart) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        inString.setTo(نادرست,
                       inText->chunkContents + rangeStart - inText->chunkNativeStart,
                       rangeEnd - rangeStart);
    } وگرنه {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_setNativeIndex(inText, rangeStart);
        int32_t limit = rangeEnd;
        U_ASSERT(limit <= utext_nativeLength(inText));
        اگر (limit > utext_nativeLength(inText)) {
            limit = static_cast<int32_t>(utext_nativeLength(inText));
        }
        درون_داد_Map.adoptInsteadAndCheckErrorCode(جدید UVector32(status), status);
        اگر (U_FAILURE(status)) {
            بازگشت 0;
        }
        وقتی (utext_getNativeIndex(inText) < limit) {
            int32_t nativePosition = static_cast<int32_t>(utext_getNativeIndex(inText));
            UChar32 c = utext_next32(inText);
            U_ASSERT(c != U_SENTINEL);
            inString.append(c);
            وقتی (درون_داد_Map->size() < inString.length()) {
                درون_داد_Map->addElement(nativePosition, status);
            }
        }
        درون_داد_Map->addElement(limit, status);
    }


    اگر (!nfkcNorm2->isNormalized(inString, status)) {
        UnicodeString normalized_درون_داد;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        LocalPointer<UVector32> normalizedMap(جدید UVector32(status), status);
        اگر (U_FAILURE(status)) {
            بازگشت 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        برای (int32_t srcI = 0; srcI < inString.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragmentStartI = srcI;
            UChar32 c = inString.char32At(srcI);
            برای (;;) {
                fragment.append(c);
                srcI = inString.moveIndex32(srcI, 1);
                اگر (srcI == inString.length()) {
                    بشکن;
                }
                c = inString.char32At(srcI);
                اگر (nfkcNorm2->hasBoundaryBefore(c)) {
                    بشکن;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, status);
            normalized_درون_داد.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragmentOriginalStart = درون_داد_Map.isValid() ?
                    درون_داد_Map->elementAti(fragmentStartI) : fragmentStartI+rangeStart;
            وقتی (normalizedMap->size() < normalized_درون_داد.length()) {
                normalizedMap->addElement(fragmentOriginalStart, status);
                اگر (U_FAILURE(status)) {
                    بشکن;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_درون_داد.length());
        int32_t nativeEnd = درون_داد_Map.isValid() ?
                درون_داد_Map->elementAti(inString.length()) : inString.length()+rangeStart;
        normalizedMap->addElement(nativeEnd, status);

        درون_داد_Map = std::move(normalizedMap);
        inString = std::move(normalized_درون_داد);
    }

    int32_t رقم_Code_Pts = inString.شمار_Char32();
    اگر (رقم_Code_Pts != inString.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = درون_داد_Map.isValid();
        اگر (!hadExistingMap) {
            درون_داد_Map.adoptInsteadAndCheckErrorCode(جدید UVector32(status), status);
            اگر (U_FAILURE(status)) {
                بازگشت 0;
            }
        }
        int32_t cpIdx = 0;
        برای (int32_t cuIdx = 0; ; cuIdx = inString.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            اگر (hadExistingMap) {
                درون_داد_Map->setElementAt(درون_داد_Map->elementAti(cuIdx), cpIdx);
            } وگرنه {
                درون_داد_Map->addElement(cuIdx+rangeStart, status);
            }
            cpIdx++;
            اگر (cuIdx == inString.length()) {
               بشکن;
            }
        }
    }

#اگر UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    اگر (isPhraseBreaking && isCj) {
        بازگشت fMlBreakEngine->divideUpRange(inText, rangeStart, rangeEnd, foundBreaks, inString,
                                             درون_داد_Map, status);
    }
#endif

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 bestSnlp(رقم_Code_Pts + 1, status);
    bestSnlp.addElement(0, status);
    برای(int32_t i = 1; i <= رقم_Code_Pts; i++) {
        bestSnlp.addElement(kuint32max, status);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 prev(رقم_Code_Pts + 1, status);
    برای(int32_t i = 0; i <= رقم_Code_Pts; i++){
        prev.addElement(-1, status);
    }

    ثابت int32_t maxWordSize = 20;
    UVector32 مقادیر(رقم_Code_Pts, status);
    مقادیر.setSize(رقم_Code_Pts);
    UVector32 lengths(رقم_Code_Pts, status);
    lengths.setSize(رقم_Code_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &inString, &status);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    منطقی is_prev_katakana = نادرست;
    برای (int32_t i = 0;  i < رقم_Code_Pts;  ++i, ix = inString.moveIndex32(ix, 1)) {
        اگر (static_cast<uint32_t>(bestSnlp.elementAti(i)) == kuint32max) {
            ادامه;
        }

        int32_t count;
        utext_setNativeIndex(&fu, ix);
        count = fDictionary->matches(&fu, maxWordSize, رقم_Code_Pts,
                             nullptr, lengths.getBuffer(), مقادیر.getBuffer(), nullptr);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        اگر ((count == 0 || lengths.elementAti(0) != 1) &&
                !fHangulWordSet.contains(inString.char32At(ix))) {
            مقادیر.setElementAt(maxSnlp, count);   // 255
            lengths.setElementAt(1, count++);
        }

        برای (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(bestSnlp.elementAti(i)) + static_cast<uint32_t>(مقادیر.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            اگر (newSnlp < static_cast<uint32_t>(bestSnlp.elementAti(ln_j_i))) {
                bestSnlp.setElementAt(newSnlp, ln_j_i);
                prev.setElementAt(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        منطقی is_katakana = isKatakana(inString.char32At(ix));
        int32_t katakanaRunLength = 1;
        اگر (!is_prev_katakana && is_katakana) {
            int32_t j = inString.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            وقتی (j < inString.length() && katakanaRunLength < kMaxKatakanaGroupLength &&
                    isKatakana(inString.char32At(j))) {
                j = inString.moveIndex32(j, 1);
                katakanaRunLength++;
            }
            اگر (katakanaRunLength < kMaxKatakanaGroupLength) {
                uint32_t newSnlp = bestSnlp.elementAti(i) + getKatakanaCost(katakanaRunLength);
                اگر (newSnlp < static_cast<uint32_t>(bestSnlp.elementAti(i + katakanaRunLength))) {
                    bestSnlp.setElementAt(newSnlp, i+katakanaRunLength);
                    prev.setElementAt(i, i+katakanaRunLength);  // prev[j] = i;
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
    UVector32 t_boundary(رقم_Code_Pts+1, status);

    int32_t رقم_Breaks = 0;
    // No segmentation found, set boundary to end of range
    اگر (static_cast<uint32_t>(bestSnlp.elementAti(رقم_Code_Pts)) == kuint32max) {
        t_boundary.addElement(رقم_Code_Pts, status);
        رقم_Breaks++;
    } وگرنه اگر (isPhraseBreaking) {
        t_boundary.addElement(رقم_Code_Pts, status);
        اگر(U_SUCCESS(status)) {
            رقم_Breaks++;
            int32_t prevIdx = رقم_Code_Pts;

            int32_t codeUnitIdx = -1;
            int32_t prevCodeUnitIdx = -1;
            int32_t length = -1;
            برای (int32_t i = prev.elementAti(رقم_Code_Pts); i > 0; i = prev.elementAti(i)) {
                codeUnitIdx = inString.moveIndex32(0, i);
                prevCodeUnitIdx = inString.moveIndex32(0, prevIdx);
                // Calculate the length by using the code unit.
                length = prevCodeUnitIdx - codeUnitIdx;
                prevIdx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                اگر (!fSkipSet.contains_کلید(inString.tempSubString(codeUnitIdx, length))
                    && (!isKatakana(inString.char32At(inString.moveIndex32(codeUnitIdx, -1)))
                           || !isKatakana(inString.char32At(codeUnitIdx)))) {
                    t_boundary.addElement(i, status);
                    رقم_Breaks++;
                }
            }
        }
    } وگرنه {
        برای (int32_t i = رقم_Code_Pts; i > 0; i = prev.elementAti(i)) {
            t_boundary.addElement(i, status);
            رقم_Breaks++;
        }
        U_ASSERT(prev.elementAti(t_boundary.elementAti(رقم_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    اگر (foundBreaks.size() == 0 || foundBreaks.peeki() < rangeStart) {
        t_boundary.addElement(0, status);
        رقم_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_رقم_Breaks = 0;
    برای (int32_t i = رقم_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > prevCPPos);
        int32_t utextPos =  درون_داد_Map.isValid() ? درون_داد_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(utextPos >= prevUTextPos);
        اگر (utextPos > prevUTextPos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(foundBreaks.size() == 0 || foundBreaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            اگر (utextPos != rangeStart
                || (isPhraseBreaking && utextPos > 0
                       && fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_رقم_Breaks++;
            }
        } وگرنه {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --رقم_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (تهی)prevCPPos; // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    اگر (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        اگر (isPhraseBreaking) {
            اگر (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
                foundBreaks.popi();
                corrected_رقم_Breaks--;
            }
        } وگرنه {
            foundBreaks.popi();
            corrected_رقم_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    بازگشت corrected_رقم_Breaks;
}

تهی CjkBreakEngine::initJapanesePhraseParameter(UErrorCode& error) {
    loadJapaneseExtensions(error);
    loadHiragana(error);
}

تهی CjkBreakEngine::loadJapaneseExtensions(UErrorCode& error) {
    ثابت نویسه* tag = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    اگر (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(tag, error);
        وقتی (U_SUCCESS(error) && bundle.hasNext()) {
            fSkipSet.puti(bundle.getNextString(error), 1, error);
        }
    }
}

تهی CjkBreakEngine::loadHiragana(UErrorCode& error) {
    UnicodeSet hiraganaWordSet(UnicodeString(u"[:Hiragana:]"), error);
    hiraganaWordSet.compact();
    UnicodeSetIterator iterator(hiraganaWordSet);
    وقتی (iterator.next()) {
        fSkipSet.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

