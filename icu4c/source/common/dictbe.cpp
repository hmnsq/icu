// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 *******************************************************************************
 */

#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "dictbe.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/chariter.h"
#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/usetiter.h"
#अन्तर्भाव "ubrkimpl.h"
#अन्तर्भाव "utracimp.h"
#अन्तर्भाव "uvectr32.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "unicode/normlzr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "dictionarydata.h"

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

DictionaryBreakEngine::DictionaryBreakEngine() {
}

DictionaryBreakEngine::~DictionaryBreakEngine() {
}

UBool
DictionaryBreakEngine::handles(UChar32 c, ध्रुव वर्ण*) ध्रुव {
    प्रत्यागम f_समुच्चयः.अन्तर्भवति(c);
}

int32_t
DictionaryBreakEngine::findBreaks( UText *text,
                                 int32_t startPos,
                                 int32_t अन्तः_Pos,
                                 UVector32 &प्राप्तम्_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    int32_t परिणाम = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_समुच्चयः_Native_सूचकांक(text, startPos);
    int32_t start = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
    int32_t current;
    int32_t rangeStart;
    int32_t range_अन्तः;
    UChar32 c = utext_current32(text);
    यावत् ((current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < अन्तः_Pos && f_समुच्चयः.अन्तर्भवति(c)) {
        utext_next32(text);         // TODO:  recast loop for postincrement
        c = utext_current32(text);
    }
    rangeStart = start;
    range_अन्तः = current;
    परिणाम = divide_ऊर्ध्वम्_Dictionary_Range(text, rangeStart, range_अन्तः, प्राप्तम्_Breaks, isPhraseBreaking, स्थितिः);
    utext_समुच्चयः_Native_सूचकांक(text, current);
    
    प्रत्यागम परिणाम;
}

शून्य
DictionaryBreakEngine::समुच्चयः_Characters( ध्रुव Unicode_समुच्चयः &समुच्चयः ) {
    f_समुच्चयः = समुच्चयः;
    // Compact for caching
    f_समुच्चयः.compact();
}

/*
 ******************************************************************
 * PossibleWord
 */

// Helper class for improving readability of the Thai/Lao/Khmer word break
// algorithm. The implementation is completely inline.

// List size, limited by the maximum number of words in the dictionary
// that form a nested sequence.
स्थिर ध्रुव int32_t POSSIBLE_WORD_LIST_MAX = 20;

वर्ग PossibleWord {
गुप्त:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   उपसर्गः;     // The longest match with a dictionary word
    int32_t   विस्थापनम्;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_WORD_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_WORD_LIST_MAX];   // Word Lengths, in code points.

सार्वजनिक:
    PossibleWord() : count(0), उपसर्गः(0), विस्थापनम्(-1), mark(0), current(0) {}
    ~PossibleWord() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *text, DictionaryMatcher *dict, int32_t range_अन्तः );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *text );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     पृष्ठभागः_ऊर्ध्वम्( UText *text );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_उपसर्गः() { प्रत्यागम उपसर्गः; }
  
    // Mark the current candidate as the one we like
    शून्य      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { प्रत्यागम cpLengths[mark]; }
};


int32_t PossibleWord::candidates( UText *text, DictionaryMatcher *dict, int32_t range_अन्तः ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t start = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
    यदि (start != विस्थापनम्) {
        विस्थापनम् = start;
        count = dict->matches(text, range_अन्तः-start, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, शून्यसूचकः, &उपसर्गः);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        यदि (count <= 0) {
            utext_समुच्चयः_Native_सूचकांक(text, start);
        }
    }
    यदि (count > 0) {
        utext_समुच्चयः_Native_सूचकांक(text, start+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    प्रत्यागम count;
}

int32_t
PossibleWord::acceptMarked( UText *text ) {
    utext_समुच्चयः_Native_सूचकांक(text, विस्थापनम् + cuLengths[mark]);
    प्रत्यागम cuLengths[mark];
}


UBool
PossibleWord::पृष्ठभागः_ऊर्ध्वम्( UText *text ) {
    यदि (current > 0) {
        utext_समुच्चयः_Native_सूचकांक(text, विस्थापनम् + cuLengths[--current]);
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

/*
 ******************************************************************
 * ThaiBreakEngine
 */

// How many words in a row are "good enough"?
स्थिर ध्रुव int32_t THAI_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
स्थिर ध्रुव int32_t THAI_मूलम्_COMBINE_देहली = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
स्थिर ध्रुव int32_t THAI_उपसर्गः_COMBINE_देहली = 3;

// Elision character
स्थिर ध्रुव int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
स्थिर ध्रुव int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
स्थिर ध्रुव int32_t THAI_MIN_WORD = 2;

// Minimum number of characters for two words
स्थिर ध्रुव int32_t THAI_MIN_WORD_विस्तृतिः = THAI_MIN_WORD * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_संहिता &स्थितिः)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_समुच्चयः thai_Word_समुच्चयः(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        समुच्चयः_Characters(thai_Word_समुच्चयः);
    }
    f_Mark_समुच्चयः.apply_आदर्शः(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), स्थितिः);
    f_Mark_समुच्चयः.सङ्कलय(0x0020);
    f_अन्तः_Word_समुच्चयः = thai_Word_समुच्चयः;
    f_अन्तः_Word_समुच्चयः.remove(0x0E31);             // MAI HAN-AKAT
    f_अन्तः_Word_समुच्चयः.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_Word_समुच्चयः.सङ्कलय(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_Word_समुच्चयः.सङ्कलय(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_समुच्चयः.सङ्कलय(THAI_PAIYANNOI);
    f_Suffix_समुच्चयः.सङ्कलय(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_समुच्चयः.compact();
    f_अन्तः_Word_समुच्चयः.compact();
    f_Begin_Word_समुच्चयः.compact();
    f_Suffix_समुच्चयः.compact();
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    विलोपय fDictionary;
}

int32_t
ThaiBreakEngine::divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                                int32_t rangeStart,
                                                int32_t range_अन्तः,
                                                UVector32 &प्राप्तम्_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    utext_समुच्चयः_Native_सूचकांक(text, rangeStart);
    utext_moveIndex32(text, THAI_MIN_WORD_विस्तृतिः);
    यदि (utext_get_Native_सूचकांक(text) >= range_अन्तः) {
        प्रत्यागम 0;       // Not enough characters for two words
    }
    utext_समुच्चयः_Native_सूचकांक(text, rangeStart);


    uint32_t शब्दाः_प्राप्तम् = 0;
    int32_t cp_Word_दीर्घता = 0;    // Word Length in Code Points.
    int32_t cu_Word_दीर्घता = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    PossibleWord शब्दाः[THAI_LOOKAHEAD];
    
    utext_समुच्चयः_Native_सूचकांक(text, rangeStart);
    
    यावत् (U_SUCCESS(स्थितिः) && (current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < range_अन्तः) {
        cp_Word_दीर्घता = 0;
        cu_Word_दीर्घता = 0;

        // Look for candidate words at the current position
        int32_t candidates = शब्दाः[शब्दाः_प्राप्तम्%THAI_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः);
        
        // If we found exactly one, use that
        यदि (candidates == 1) {
            cu_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % THAI_LOOKAHEAD].acceptMarked(text);
            cp_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % THAI_LOOKAHEAD].markedCPLength();
            शब्दाः_प्राप्तम् += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        अन्यथा यदि (candidates > 1) {
            // If we're already at the end of the range, we're done
            यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) >= range_अन्तः) {
                गच्छ प्राप्तम्_Best;
            }
            कुरु {
                यदि (शब्दाः[(शब्दाः_प्राप्तम् + 1) % THAI_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    शब्दाः[शब्दाः_प्राप्तम्%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) >= range_अन्तः) {
                        गच्छ प्राप्तम्_Best;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    कुरु {
                        // If we find a third word, stop right away
                        यदि (शब्दाः[(शब्दाः_प्राप्तम् + 2) % THAI_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः)) {
                            शब्दाः[शब्दाः_प्राप्तम् % THAI_LOOKAHEAD].markCurrent();
                            गच्छ प्राप्तम्_Best;
                        }
                    }
                    यावत् (शब्दाः[(शब्दाः_प्राप्तम् + 1) % THAI_LOOKAHEAD].पृष्ठभागः_ऊर्ध्वम्(text));
                }
            }
            यावत् (शब्दाः[शब्दाः_प्राप्तम् % THAI_LOOKAHEAD].पृष्ठभागः_ऊर्ध्वम्(text));
प्राप्तम्_Best:
            // Set UText position to after the accepted word.
            cu_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % THAI_LOOKAHEAD].acceptMarked(text);
            cp_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % THAI_LOOKAHEAD].markedCPLength();
            शब्दाः_प्राप्तम् += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) < range_अन्तः && cp_Word_दीर्घता < THAI_मूलम्_COMBINE_देहली) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            यदि (शब्दाः[शब्दाः_प्राप्तम् % THAI_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) <= 0
                  && (cu_Word_दीर्घता == 0
                      || शब्दाः[शब्दाः_प्राप्तम्%THAI_LOOKAHEAD].longest_उपसर्गः() < THAI_उपसर्गः_COMBINE_देहली)) {
                // Look for a plausible word boundary
                int32_t remaining = range_अन्तः - (current+cu_Word_दीर्घता);
                UChar32 pc;
                int32_t chars = 0;
                प्रति (;;) {
                    int32_t pc_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
                    pc = utext_next32(text);
                    int32_t pc_आकार = static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - pc_सूचकांक;
                    chars += pc_आकार;
                    remaining -= pc_आकार;
                    यदि (remaining <= 0) {
                        भङ्ग;
                    }
                    uc = utext_current32(text);
                    यदि (f_अन्तः_Word_समुच्चयः.अन्तर्भवति(pc) && f_Begin_Word_समुच्चयः.अन्तर्भवति(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t अङ्कः_candidates = शब्दाः[(शब्दाः_प्राप्तम् + 1) % THAI_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः);
                        utext_समुच्चयः_Native_सूचकांक(text, current + cu_Word_दीर्घता + chars);
                        यदि (अङ्कः_candidates > 0) {
                            भङ्ग;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                यदि (cu_Word_दीर्घता <= 0) {
                    शब्दाः_प्राप्तम् += 1;
                }
                
                // Update the length with the passed-over characters
                cu_Word_दीर्घता += chars;
            }
            अन्यथा {
                // Back up to where we were for next iteration
                utext_समुच्चयः_Native_सूचकांक(text, current+cu_Word_दीर्घता);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        यावत् ((currPos = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < range_अन्तः && f_Mark_समुच्चयः.अन्तर्भवति(utext_current32(text))) {
            utext_next32(text);
            cu_Word_दीर्घता += static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) < range_अन्तः && cu_Word_दीर्घता > 0) {
            यदि (शब्दाः[शब्दाः_प्राप्तम्%THAI_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) <= 0
                && f_Suffix_समुच्चयः.अन्तर्भवति(uc = utext_current32(text))) {
                यदि (uc == THAI_PAIYANNOI) {
                    यदि (!f_Suffix_समुच्चयः.अन्तर्भवति(utext_previous32(text))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(text);
                        int32_t paiyannoi_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
                        utext_next32(text);
                        cu_Word_दीर्घता += static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - paiyannoi_सूचकांक; // Add PAIYANNOI to word
                        uc = utext_current32(text);     // Fetch next character
                    }
                    अन्यथा {
                        // Restore prior position
                        utext_next32(text);
                    }
                }
                यदि (uc == THAI_MAIYAMOK) {
                    यदि (utext_previous32(text) != THAI_MAIYAMOK) {
                        // Skip over previous end and MAIYAMOK
                        utext_next32(text);
                        int32_t maiyamok_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
                        utext_next32(text);
                        cu_Word_दीर्घता += static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - maiyamok_सूचकांक; // Add MAIYAMOK to word
                    }
                    अन्यथा {
                        // Restore prior position
                        utext_next32(text);
                    }
                }
            }
            अन्यथा {
                utext_समुच्चयः_Native_सूचकांक(text, current+cu_Word_दीर्घता);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        यदि (cu_Word_दीर्घता > 0) {
            प्राप्तम्_Breaks.push((current+cu_Word_दीर्घता), स्थितिः);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    यदि (प्राप्तम्_Breaks.peeki() >= range_अन्तः) {
        (शून्य) प्राप्तम्_Breaks.popi();
        शब्दाः_प्राप्तम् -= 1;
    }

    प्रत्यागम शब्दाः_प्राप्तम्;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
स्थिर ध्रुव int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
स्थिर ध्रुव int32_t LAO_मूलम्_COMBINE_देहली = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
स्थिर ध्रुव int32_t LAO_उपसर्गः_COMBINE_देहली = 3;

// Minimum word size
स्थिर ध्रुव int32_t LAO_MIN_WORD = 2;

// Minimum number of characters for two words
स्थिर ध्रुव int32_t LAO_MIN_WORD_विस्तृतिः = LAO_MIN_WORD * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_संहिता &स्थितिः)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_समुच्चयः lao_Word_समुच्चयः(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        समुच्चयः_Characters(lao_Word_समुच्चयः);
    }
    f_Mark_समुच्चयः.apply_आदर्शः(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), स्थितिः);
    f_Mark_समुच्चयः.सङ्कलय(0x0020);
    f_अन्तः_Word_समुच्चयः = lao_Word_समुच्चयः;
    f_अन्तः_Word_समुच्चयः.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_Word_समुच्चयः.सङ्कलय(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_Word_समुच्चयः.सङ्कलय(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_Word_समुच्चयः.सङ्कलय(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_समुच्चयः.compact();
    f_अन्तः_Word_समुच्चयः.compact();
    f_Begin_Word_समुच्चयः.compact();
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

LaoBreakEngine::~LaoBreakEngine() {
    विलोपय fDictionary;
}

int32_t
LaoBreakEngine::divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                                int32_t rangeStart,
                                                int32_t range_अन्तः,
                                                UVector32 &प्राप्तम्_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    यदि ((range_अन्तः - rangeStart) < LAO_MIN_WORD_विस्तृतिः) {
        प्रत्यागम 0;       // Not enough characters for two words
    }

    uint32_t शब्दाः_प्राप्तम् = 0;
    int32_t cp_Word_दीर्घता = 0;
    int32_t cu_Word_दीर्घता = 0;
    int32_t current;
    PossibleWord शब्दाः[LAO_LOOKAHEAD];

    utext_समुच्चयः_Native_सूचकांक(text, rangeStart);

    यावत् (U_SUCCESS(स्थितिः) && (current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < range_अन्तः) {
        cu_Word_दीर्घता = 0;
        cp_Word_दीर्घता = 0;

        // Look for candidate words at the current position
        int32_t candidates = शब्दाः[शब्दाः_प्राप्तम्%LAO_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः);
        
        // If we found exactly one, use that
        यदि (candidates == 1) {
            cu_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % LAO_LOOKAHEAD].acceptMarked(text);
            cp_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % LAO_LOOKAHEAD].markedCPLength();
            शब्दाः_प्राप्तम् += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        अन्यथा यदि (candidates > 1) {
            // If we're already at the end of the range, we're done
            यदि (utext_get_Native_सूचकांक(text) >= range_अन्तः) {
                गच्छ प्राप्तम्_Best;
            }
            कुरु {
                यदि (शब्दाः[(शब्दाः_प्राप्तम् + 1) % LAO_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    शब्दाः[शब्दाः_प्राप्तम्%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) >= range_अन्तः) {
                        गच्छ प्राप्तम्_Best;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    कुरु {
                        // If we find a third word, stop right away
                        यदि (शब्दाः[(शब्दाः_प्राप्तम् + 2) % LAO_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः)) {
                            शब्दाः[शब्दाः_प्राप्तम् % LAO_LOOKAHEAD].markCurrent();
                            गच्छ प्राप्तम्_Best;
                        }
                    }
                    यावत् (शब्दाः[(शब्दाः_प्राप्तम् + 1) % LAO_LOOKAHEAD].पृष्ठभागः_ऊर्ध्वम्(text));
                }
            }
            यावत् (शब्दाः[शब्दाः_प्राप्तम् % LAO_LOOKAHEAD].पृष्ठभागः_ऊर्ध्वम्(text));
प्राप्तम्_Best:
            cu_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % LAO_LOOKAHEAD].acceptMarked(text);
            cp_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % LAO_LOOKAHEAD].markedCPLength();
            शब्दाः_प्राप्तम् += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) < range_अन्तः && cp_Word_दीर्घता < LAO_मूलम्_COMBINE_देहली) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            यदि (शब्दाः[शब्दाः_प्राप्तम् % LAO_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) <= 0
                  && (cu_Word_दीर्घता == 0
                      || शब्दाः[शब्दाः_प्राप्तम्%LAO_LOOKAHEAD].longest_उपसर्गः() < LAO_उपसर्गः_COMBINE_देहली)) {
                // Look for a plausible word boundary
                int32_t remaining = range_अन्तः - (current + cu_Word_दीर्घता);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                प्रति (;;) {
                    int32_t pc_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
                    pc = utext_next32(text);
                    int32_t pc_आकार = static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - pc_सूचकांक;
                    chars += pc_आकार;
                    remaining -= pc_आकार;
                    यदि (remaining <= 0) {
                        भङ्ग;
                    }
                    uc = utext_current32(text);
                    यदि (f_अन्तः_Word_समुच्चयः.अन्तर्भवति(pc) && f_Begin_Word_समुच्चयः.अन्तर्भवति(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t अङ्कः_candidates = शब्दाः[(शब्दाः_प्राप्तम् + 1) % LAO_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः);
                        utext_समुच्चयः_Native_सूचकांक(text, current + cu_Word_दीर्घता + chars);
                        यदि (अङ्कः_candidates > 0) {
                            भङ्ग;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                यदि (cu_Word_दीर्घता <= 0) {
                    शब्दाः_प्राप्तम् += 1;
                }
                
                // Update the length with the passed-over characters
                cu_Word_दीर्घता += chars;
            }
            अन्यथा {
                // Back up to where we were for next iteration
                utext_समुच्चयः_Native_सूचकांक(text, current + cu_Word_दीर्घता);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        यावत् ((currPos = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < range_अन्तः && f_Mark_समुच्चयः.अन्तर्भवति(utext_current32(text))) {
            utext_next32(text);
            cu_Word_दीर्घता += static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        यदि (cu_Word_दीर्घता > 0) {
            प्राप्तम्_Breaks.push((current+cu_Word_दीर्घता), स्थितिः);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    यदि (प्राप्तम्_Breaks.peeki() >= range_अन्तः) {
        (शून्य) प्राप्तम्_Breaks.popi();
        शब्दाः_प्राप्तम् -= 1;
    }

    प्रत्यागम शब्दाः_प्राप्तम्;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
स्थिर ध्रुव int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
स्थिर ध्रुव int32_t BURMESE_मूलम्_COMBINE_देहली = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
स्थिर ध्रुव int32_t BURMESE_उपसर्गः_COMBINE_देहली = 3;

// Minimum word size
स्थिर ध्रुव int32_t BURMESE_MIN_WORD = 2;

// Minimum number of characters for two words
स्थिर ध्रुव int32_t BURMESE_MIN_WORD_विस्तृतिः = BURMESE_MIN_WORD * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_संहिता &स्थितिः)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_Word_समुच्चयः.सङ्कलय(0x1000, 0x102A);      // basic consonants and independent vowels
    f_अन्तः_Word_समुच्चयः.apply_आदर्शः(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), स्थितिः);
    f_Mark_समुच्चयः.apply_आदर्शः(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), स्थितिः);
    f_Mark_समुच्चयः.सङ्कलय(0x0020);
    यदि (U_SUCCESS(स्थितिः)) {
        समुच्चयः_Characters(f_अन्तः_Word_समुच्चयः);
    }

    // Compact for caching.
    f_Mark_समुच्चयः.compact();
    f_अन्तः_Word_समुच्चयः.compact();
    f_Begin_Word_समुच्चयः.compact();
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    विलोपय fDictionary;
}

int32_t
BurmeseBreakEngine::divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                                int32_t rangeStart,
                                                int32_t range_अन्तः,
                                                UVector32 &प्राप्तम्_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_संहिता& स्थितिः ) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    यदि ((range_अन्तः - rangeStart) < BURMESE_MIN_WORD_विस्तृतिः) {
        प्रत्यागम 0;       // Not enough characters for two words
    }

    uint32_t शब्दाः_प्राप्तम् = 0;
    int32_t cp_Word_दीर्घता = 0;
    int32_t cu_Word_दीर्घता = 0;
    int32_t current;
    PossibleWord शब्दाः[BURMESE_LOOKAHEAD];

    utext_समुच्चयः_Native_सूचकांक(text, rangeStart);

    यावत् (U_SUCCESS(स्थितिः) && (current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < range_अन्तः) {
        cu_Word_दीर्घता = 0;
        cp_Word_दीर्घता = 0;

        // Look for candidate words at the current position
        int32_t candidates = शब्दाः[शब्दाः_प्राप्तम्%BURMESE_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः);
        
        // If we found exactly one, use that
        यदि (candidates == 1) {
            cu_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % BURMESE_LOOKAHEAD].acceptMarked(text);
            cp_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % BURMESE_LOOKAHEAD].markedCPLength();
            शब्दाः_प्राप्तम् += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        अन्यथा यदि (candidates > 1) {
            // If we're already at the end of the range, we're done
            यदि (utext_get_Native_सूचकांक(text) >= range_अन्तः) {
                गच्छ प्राप्तम्_Best;
            }
            कुरु {
                यदि (शब्दाः[(शब्दाः_प्राप्तम् + 1) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    शब्दाः[शब्दाः_प्राप्तम्%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) >= range_अन्तः) {
                        गच्छ प्राप्तम्_Best;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    कुरु {
                        // If we find a third word, stop right away
                        यदि (शब्दाः[(शब्दाः_प्राप्तम् + 2) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः)) {
                            शब्दाः[शब्दाः_प्राप्तम् % BURMESE_LOOKAHEAD].markCurrent();
                            गच्छ प्राप्तम्_Best;
                        }
                    }
                    यावत् (शब्दाः[(शब्दाः_प्राप्तम् + 1) % BURMESE_LOOKAHEAD].पृष्ठभागः_ऊर्ध्वम्(text));
                }
            }
            यावत् (शब्दाः[शब्दाः_प्राप्तम् % BURMESE_LOOKAHEAD].पृष्ठभागः_ऊर्ध्वम्(text));
प्राप्तम्_Best:
            cu_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % BURMESE_LOOKAHEAD].acceptMarked(text);
            cp_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % BURMESE_LOOKAHEAD].markedCPLength();
            शब्दाः_प्राप्तम् += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) < range_अन्तः && cp_Word_दीर्घता < BURMESE_मूलम्_COMBINE_देहली) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            यदि (शब्दाः[शब्दाः_प्राप्तम् % BURMESE_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) <= 0
                  && (cu_Word_दीर्घता == 0
                      || शब्दाः[शब्दाः_प्राप्तम्%BURMESE_LOOKAHEAD].longest_उपसर्गः() < BURMESE_उपसर्गः_COMBINE_देहली)) {
                // Look for a plausible word boundary
                int32_t remaining = range_अन्तः - (current + cu_Word_दीर्घता);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                प्रति (;;) {
                    int32_t pc_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
                    pc = utext_next32(text);
                    int32_t pc_आकार = static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - pc_सूचकांक;
                    chars += pc_आकार;
                    remaining -= pc_आकार;
                    यदि (remaining <= 0) {
                        भङ्ग;
                    }
                    uc = utext_current32(text);
                    यदि (f_अन्तः_Word_समुच्चयः.अन्तर्भवति(pc) && f_Begin_Word_समुच्चयः.अन्तर्भवति(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t अङ्कः_candidates = शब्दाः[(शब्दाः_प्राप्तम् + 1) % BURMESE_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः);
                        utext_समुच्चयः_Native_सूचकांक(text, current + cu_Word_दीर्घता + chars);
                        यदि (अङ्कः_candidates > 0) {
                            भङ्ग;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                यदि (cu_Word_दीर्घता <= 0) {
                    शब्दाः_प्राप्तम् += 1;
                }
                
                // Update the length with the passed-over characters
                cu_Word_दीर्घता += chars;
            }
            अन्यथा {
                // Back up to where we were for next iteration
                utext_समुच्चयः_Native_सूचकांक(text, current + cu_Word_दीर्घता);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        यावत् ((currPos = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < range_अन्तः && f_Mark_समुच्चयः.अन्तर्भवति(utext_current32(text))) {
            utext_next32(text);
            cu_Word_दीर्घता += static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        यदि (cu_Word_दीर्घता > 0) {
            प्राप्तम्_Breaks.push((current+cu_Word_दीर्घता), स्थितिः);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    यदि (प्राप्तम्_Breaks.peeki() >= range_अन्तः) {
        (शून्य) प्राप्तम्_Breaks.popi();
        शब्दाः_प्राप्तम् -= 1;
    }

    प्रत्यागम शब्दाः_प्राप्तम्;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
स्थिर ध्रुव int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
स्थिर ध्रुव int32_t KHMER_मूलम्_COMBINE_देहली = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
स्थिर ध्रुव int32_t KHMER_उपसर्गः_COMBINE_देहली = 3;

// Minimum word size
स्थिर ध्रुव int32_t KHMER_MIN_WORD = 2;

// Minimum number of characters for two words
स्थिर ध्रुव int32_t KHMER_MIN_WORD_विस्तृतिः = KHMER_MIN_WORD * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_संहिता &स्थितिः)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_समुच्चयः khmer_Word_समुच्चयः(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        समुच्चयः_Characters(khmer_Word_समुच्चयः);
    }
    f_Mark_समुच्चयः.apply_आदर्शः(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), स्थितिः);
    f_Mark_समुच्चयः.सङ्कलय(0x0020);
    f_अन्तः_Word_समुच्चयः = khmer_Word_समुच्चयः;
    f_Begin_Word_समुच्चयः.सङ्कलय(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_अन्तः_Word_समुच्चयः.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_समुच्चयः.compact();
    f_अन्तः_Word_समुच्चयः.compact();
    f_Begin_Word_समुच्चयः.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    विलोपय fDictionary;
}

int32_t
KhmerBreakEngine::divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                                int32_t rangeStart,
                                                int32_t range_अन्तः,
                                                UVector32 &प्राप्तम्_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_संहिता& स्थितिः ) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    यदि ((range_अन्तः - rangeStart) < KHMER_MIN_WORD_विस्तृतिः) {
        प्रत्यागम 0;       // Not enough characters for two words
    }

    uint32_t शब्दाः_प्राप्तम् = 0;
    int32_t cp_Word_दीर्घता = 0;
    int32_t cu_Word_दीर्घता = 0;
    int32_t current;
    PossibleWord शब्दाः[KHMER_LOOKAHEAD];

    utext_समुच्चयः_Native_सूचकांक(text, rangeStart);

    यावत् (U_SUCCESS(स्थितिः) && (current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < range_अन्तः) {
        cu_Word_दीर्घता = 0;
        cp_Word_दीर्घता = 0;

        // Look for candidate words at the current position
        int32_t candidates = शब्दाः[शब्दाः_प्राप्तम्%KHMER_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः);

        // If we found exactly one, use that
        यदि (candidates == 1) {
            cu_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].acceptMarked(text);
            cp_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].markedCPLength();
            शब्दाः_प्राप्तम् += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        अन्यथा यदि (candidates > 1) {
            // If we're already at the end of the range, we're done
            यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) >= range_अन्तः) {
                गच्छ प्राप्तम्_Best;
            }
            कुरु {
                यदि (शब्दाः[(शब्दाः_प्राप्तम् + 1) % KHMER_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) >= range_अन्तः) {
                        गच्छ प्राप्तम्_Best;
                    }

                    // See if any of the possible second words is followed by a third word
                    कुरु {
                        // If we find a third word, stop right away
                        यदि (शब्दाः[(शब्दाः_प्राप्तम् + 2) % KHMER_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः)) {
                            शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].markCurrent();
                            गच्छ प्राप्तम्_Best;
                        }
                    }
                    यावत् (शब्दाः[(शब्दाः_प्राप्तम् + 1) % KHMER_LOOKAHEAD].पृष्ठभागः_ऊर्ध्वम्(text));
                }
            }
            यावत् (शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].पृष्ठभागः_ऊर्ध्वम्(text));
प्राप्तम्_Best:
            cu_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].acceptMarked(text);
            cp_Word_दीर्घता = शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].markedCPLength();
            शब्दाः_प्राप्तम् += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        यदि (static_cast<int32_t>(utext_get_Native_सूचकांक(text)) < range_अन्तः && cp_Word_दीर्घता < KHMER_मूलम्_COMBINE_देहली) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            यदि (शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः) <= 0
                  && (cu_Word_दीर्घता == 0
                      || शब्दाः[शब्दाः_प्राप्तम् % KHMER_LOOKAHEAD].longest_उपसर्गः() < KHMER_उपसर्गः_COMBINE_देहली)) {
                // Look for a plausible word boundary
                int32_t remaining = range_अन्तः - (current+cu_Word_दीर्घता);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                प्रति (;;) {
                    int32_t pc_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
                    pc = utext_next32(text);
                    int32_t pc_आकार = static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - pc_सूचकांक;
                    chars += pc_आकार;
                    remaining -= pc_आकार;
                    यदि (remaining <= 0) {
                        भङ्ग;
                    }
                    uc = utext_current32(text);
                    यदि (f_अन्तः_Word_समुच्चयः.अन्तर्भवति(pc) && f_Begin_Word_समुच्चयः.अन्तर्भवति(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t अङ्कः_candidates = शब्दाः[(शब्दाः_प्राप्तम् + 1) % KHMER_LOOKAHEAD].candidates(text, fDictionary, range_अन्तः);
                        utext_समुच्चयः_Native_सूचकांक(text, current+cu_Word_दीर्घता+chars);
                        यदि (अङ्कः_candidates > 0) {
                            भङ्ग;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                यदि (cu_Word_दीर्घता <= 0) {
                    शब्दाः_प्राप्तम् += 1;
                }

                // Update the length with the passed-over characters
                cu_Word_दीर्घता += chars;
            }
            अन्यथा {
                // Back up to where we were for next iteration
                utext_समुच्चयः_Native_सूचकांक(text, current+cu_Word_दीर्घता);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        यावत् ((currPos = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < range_अन्तः && f_Mark_समुच्चयः.अन्तर्भवति(utext_current32(text))) {
            utext_next32(text);
            cu_Word_दीर्घता += static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - currPos;
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
        यदि (cu_Word_दीर्घता > 0) {
            प्राप्तम्_Breaks.push((current+cu_Word_दीर्घता), स्थितिः);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    यदि (प्राप्तम्_Breaks.peeki() >= range_अन्तः) {
        (शून्य) प्राप्तम्_Breaks.popi();
        शब्दाः_प्राप्तम् -= 1;
    }

    प्रत्यागम शब्दाः_प्राप्तम्;
}

#यदि !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
स्थिर ध्रुव uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_संहिता &स्थितिः)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(असत्य) {
    UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = शून्यसूचकः;
    nfkcNorm2 = Normalizer2::getNFKCInstance(स्थितिः);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_Word_समुच्चयः.apply_आदर्शः(UnicodeString(u"[\\uac00-\\ud7a3]"), स्थितिः);
    f_Hangul_Word_समुच्चयः.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः.apply_आदर्शः(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), स्थितिः);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः.compact();
    f_Close_Punctuation_समुच्चयः.apply_आदर्शः(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), स्थितिः);
    f_Close_Punctuation_समुच्चयः.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    यदि (type == kKorean) {
        यदि (U_SUCCESS(स्थितिः)) {
            समुच्चयः_Characters(f_Hangul_Word_समुच्चयः);
        }
    } अन्यथा { // Chinese and Japanese
        Unicode_समुच्चयः cj_समुच्चयः(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), स्थितिः);
        isCj = सत्य;
        यदि (U_SUCCESS(स्थितिः)) {
            समुच्चयः_Characters(cj_समुच्चयः);
#यदि UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = नूतन MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः,
                                               f_Close_Punctuation_समुच्चयः, स्थितिः);
            यदि (fMlBreakEngine == शून्यसूचकः) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            }
#अन्यथा
            initJapanesePhraseParameter(स्थितिः);
#यदि_अन्तः
        }
    }
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

CjkBreakEngine::~CjkBreakEngine(){
    विलोपय fDictionary;
    विलोपय fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
स्थिर ध्रुव int32_t k_Max_Katakana_दीर्घता = 8;
स्थिर ध्रुव int32_t k_Max_Katakana_गणः_दीर्घता = 20;
स्थिर ध्रुव uint32_t maxSnlp = 255;

स्थिर अन्तःस्थ uint32_t getKatakanaCost(int32_t word_दीर्घता){
    //TODO: fill array with actual values from dictionary!
    स्थिर ध्रुव uint32_t katakanaCost[k_Max_Katakana_दीर्घता + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    प्रत्यागम (word_दीर्घता > k_Max_Katakana_दीर्घता) ? 8192 : katakanaCost[word_दीर्घता];
}

स्थिर अन्तःस्थ तार्किक isKatakana(UChar32 value) {
    प्रत्यागम (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

स्थिर अन्तःस्थ int32_t utext_i32_flag(int32_t bit_सूचकांक) {
    प्रत्यागम static_cast<int32_t>(1) << bit_सूचकांक;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_ऊर्ध्वम्_Dictionary_Range( UText *अन्तर्_Text,
        int32_t rangeStart,
        int32_t range_अन्तः,
        UVector32 &प्राप्तम्_Breaks,
        UBool isPhraseBreaking,
        UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    यदि (rangeStart >= range_अन्तः) {
        प्रत्यागम 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString अन्तर्_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    LocalPointer<UVector32>     निवेशः_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    यदि ((अन्तर्_Text->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         अन्तर्_Text->chunkNativeStart <= rangeStart &&
         अन्तर्_Text->chunk_Native_सीमा >= range_अन्तः   &&
         अन्तर्_Text->native_Indexing_सीमा >= range_अन्तः - अन्तर्_Text->chunkNativeStart) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        अन्तर्_String.समुच्चयः_गन्तव्यम्(असत्य,
                       अन्तर्_Text->chunkContents + rangeStart - अन्तर्_Text->chunkNativeStart,
                       range_अन्तः - rangeStart);
    } अन्यथा {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_समुच्चयः_Native_सूचकांक(अन्तर्_Text, rangeStart);
        int32_t सीमा = range_अन्तः;
        U_ASSERT(सीमा <= utext_native_दीर्घता(अन्तर्_Text));
        यदि (सीमा > utext_native_दीर्घता(अन्तर्_Text)) {
            सीमा = static_cast<int32_t>(utext_native_दीर्घता(अन्तर्_Text));
        }
        निवेशः_Map.adopt_Instead_And_Check_Error_संहिता(नूतन UVector32(स्थितिः), स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम 0;
        }
        यावत् (utext_get_Native_सूचकांक(अन्तर्_Text) < सीमा) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_सूचकांक(अन्तर्_Text));
            UChar32 c = utext_next32(अन्तर्_Text);
            U_ASSERT(c != U_SENTINEL);
            अन्तर्_String.append(c);
            यावत् (निवेशः_Map->size() < अन्तर्_String.length()) {
                निवेशः_Map->सङ्कलय_Element(nativePosition, स्थितिः);
            }
        }
        निवेशः_Map->सङ्कलय_Element(सीमा, स्थितिः);
    }


    यदि (!nfkcNorm2->isNormalized(अन्तर्_String, स्थितिः)) {
        UnicodeString normalized_निवेशः;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        LocalPointer<UVector32> normalizedMap(नूतन UVector32(स्थितिः), स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        प्रति (int32_t srcI = 0; srcI < अन्तर्_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragmentStartI = srcI;
            UChar32 c = अन्तर्_String.char32At(srcI);
            प्रति (;;) {
                fragment.append(c);
                srcI = अन्तर्_String.moveIndex32(srcI, 1);
                यदि (srcI == अन्तर्_String.length()) {
                    भङ्ग;
                }
                c = अन्तर्_String.char32At(srcI);
                यदि (nfkcNorm2->hasBoundaryBefore(c)) {
                    भङ्ग;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, स्थितिः);
            normalized_निवेशः.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragmentOriginalStart = निवेशः_Map.isValid() ?
                    निवेशः_Map->elementAti(fragmentStartI) : fragmentStartI+rangeStart;
            यावत् (normalizedMap->size() < normalized_निवेशः.length()) {
                normalizedMap->सङ्कलय_Element(fragmentOriginalStart, स्थितिः);
                यदि (U_FAILURE(स्थितिः)) {
                    भङ्ग;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_निवेशः.length());
        int32_t native_अन्तः = निवेशः_Map.isValid() ?
                निवेशः_Map->elementAti(अन्तर्_String.length()) : अन्तर्_String.length()+rangeStart;
        normalizedMap->सङ्कलय_Element(native_अन्तः, स्थितिः);

        निवेशः_Map = std::move(normalizedMap);
        अन्तर्_String = std::move(normalized_निवेशः);
    }

    int32_t अङ्कः_संहिता_Pts = अन्तर्_String.गणनम्_Char32();
    यदि (अङ्कः_संहिता_Pts != अन्तर्_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = निवेशः_Map.isValid();
        यदि (!hadExistingMap) {
            निवेशः_Map.adopt_Instead_And_Check_Error_संहिता(नूतन UVector32(स्थितिः), स्थितिः);
            यदि (U_FAILURE(स्थितिः)) {
                प्रत्यागम 0;
            }
        }
        int32_t cpIdx = 0;
        प्रति (int32_t cuIdx = 0; ; cuIdx = अन्तर्_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            यदि (hadExistingMap) {
                निवेशः_Map->समुच्चयः_Element_At(निवेशः_Map->elementAti(cuIdx), cpIdx);
            } अन्यथा {
                निवेशः_Map->सङ्कलय_Element(cuIdx+rangeStart, स्थितिः);
            }
            cpIdx++;
            यदि (cuIdx == अन्तर्_String.length()) {
               भङ्ग;
            }
        }
    }

#यदि UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    यदि (isPhraseBreaking && isCj) {
        प्रत्यागम fMlBreakEngine->divide_ऊर्ध्वम्_Range(अन्तर्_Text, rangeStart, range_अन्तः, प्राप्तम्_Breaks, अन्तर्_String,
                                             निवेशः_Map, स्थितिः);
    }
#यदि_अन्तः

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 bestSnlp(अङ्कः_संहिता_Pts + 1, स्थितिः);
    bestSnlp.सङ्कलय_Element(0, स्थितिः);
    प्रति(int32_t i = 1; i <= अङ्कः_संहिता_Pts; i++) {
        bestSnlp.सङ्कलय_Element(kuint32max, स्थितिः);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 पूर्व(अङ्कः_संहिता_Pts + 1, स्थितिः);
    प्रति(int32_t i = 0; i <= अङ्कः_संहिता_Pts; i++){
        पूर्व.सङ्कलय_Element(-1, स्थितिः);
    }

    ध्रुव int32_t max_Word_आकार = 20;
    UVector32 मानानि(अङ्कः_संहिता_Pts, स्थितिः);
    मानानि.समुच्चयः_आकार(अङ्कः_संहिता_Pts);
    UVector32 lengths(अङ्कः_संहिता_Pts, स्थितिः);
    lengths.समुच्चयः_आकार(अङ्कः_संहिता_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &अन्तर्_String, &स्थितिः);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    तार्किक is_पूर्व_katakana = असत्य;
    प्रति (int32_t i = 0;  i < अङ्कः_संहिता_Pts;  ++i, ix = अन्तर्_String.moveIndex32(ix, 1)) {
        यदि (static_cast<uint32_t>(bestSnlp.elementAti(i)) == kuint32max) {
            अनुवर्त;
        }

        int32_t count;
        utext_समुच्चयः_Native_सूचकांक(&fu, ix);
        count = fDictionary->matches(&fu, max_Word_आकार, अङ्कः_संहिता_Pts,
                             शून्यसूचकः, lengths.get_बफर(), मानानि.get_बफर(), शून्यसूचकः);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        यदि ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_Word_समुच्चयः.अन्तर्भवति(अन्तर्_String.char32At(ix))) {
            मानानि.समुच्चयः_Element_At(maxSnlp, count);   // 255
            lengths.समुच्चयः_Element_At(1, count++);
        }

        प्रति (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(bestSnlp.elementAti(i)) + static_cast<uint32_t>(मानानि.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            यदि (newSnlp < static_cast<uint32_t>(bestSnlp.elementAti(ln_j_i))) {
                bestSnlp.समुच्चयः_Element_At(newSnlp, ln_j_i);
                पूर्व.समुच्चयः_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        तार्किक is_katakana = isKatakana(अन्तर्_String.char32At(ix));
        int32_t katakana_सततम्_दीर्घता = 1;
        यदि (!is_पूर्व_katakana && is_katakana) {
            int32_t j = अन्तर्_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            यावत् (j < अन्तर्_String.length() && katakana_सततम्_दीर्घता < k_Max_Katakana_गणः_दीर्घता &&
                    isKatakana(अन्तर्_String.char32At(j))) {
                j = अन्तर्_String.moveIndex32(j, 1);
                katakana_सततम्_दीर्घता++;
            }
            यदि (katakana_सततम्_दीर्घता < k_Max_Katakana_गणः_दीर्घता) {
                uint32_t newSnlp = bestSnlp.elementAti(i) + getKatakanaCost(katakana_सततम्_दीर्घता);
                यदि (newSnlp < static_cast<uint32_t>(bestSnlp.elementAti(i + katakana_सततम्_दीर्घता))) {
                    bestSnlp.समुच्चयः_Element_At(newSnlp, i+katakana_सततम्_दीर्घता);
                    पूर्व.समुच्चयः_Element_At(i, i+katakana_सततम्_दीर्घता);  // prev[j] = i;
                }
            }
        }
        is_पूर्व_katakana = is_katakana;
    }
    utext_close(&fu);

    // Start pushing the optimal offset index into t_boundary (t for tentative).
    // prev[numCodePts] is guaranteed to be meaningful.
    // We'll first push in the reverse order, i.e.,
    // t_boundary[0] = numCodePts, and afterwards do a swap.
    UVector32 t_boundary(अङ्कः_संहिता_Pts+1, स्थितिः);

    int32_t अङ्कः_Breaks = 0;
    // No segmentation found, set boundary to end of range
    यदि (static_cast<uint32_t>(bestSnlp.elementAti(अङ्कः_संहिता_Pts)) == kuint32max) {
        t_boundary.सङ्कलय_Element(अङ्कः_संहिता_Pts, स्थितिः);
        अङ्कः_Breaks++;
    } अन्यथा यदि (isPhraseBreaking) {
        t_boundary.सङ्कलय_Element(अङ्कः_संहिता_Pts, स्थितिः);
        यदि(U_SUCCESS(स्थितिः)) {
            अङ्कः_Breaks++;
            int32_t पूर्व_Idx = अङ्कः_संहिता_Pts;

            int32_t संहिता_Unit_Idx = -1;
            int32_t पूर्व_संहिता_Unit_Idx = -1;
            int32_t length = -1;
            प्रति (int32_t i = पूर्व.elementAti(अङ्कः_संहिता_Pts); i > 0; i = पूर्व.elementAti(i)) {
                संहिता_Unit_Idx = अन्तर्_String.moveIndex32(0, i);
                पूर्व_संहिता_Unit_Idx = अन्तर्_String.moveIndex32(0, पूर्व_Idx);
                // Calculate the length by using the code unit.
                length = पूर्व_संहिता_Unit_Idx - संहिता_Unit_Idx;
                पूर्व_Idx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                यदि (!f_Skip_समुच्चयः.अन्तर्भवति_कुञ्चिका(अन्तर्_String.अस्थायी_Sub_String(संहिता_Unit_Idx, length))
                    && (!isKatakana(अन्तर्_String.char32At(अन्तर्_String.moveIndex32(संहिता_Unit_Idx, -1)))
                           || !isKatakana(अन्तर्_String.char32At(संहिता_Unit_Idx)))) {
                    t_boundary.सङ्कलय_Element(i, स्थितिः);
                    अङ्कः_Breaks++;
                }
            }
        }
    } अन्यथा {
        प्रति (int32_t i = अङ्कः_संहिता_Pts; i > 0; i = पूर्व.elementAti(i)) {
            t_boundary.सङ्कलय_Element(i, स्थितिः);
            अङ्कः_Breaks++;
        }
        U_ASSERT(पूर्व.elementAti(t_boundary.elementAti(अङ्कः_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    यदि (प्राप्तम्_Breaks.size() == 0 || प्राप्तम्_Breaks.peeki() < rangeStart) {
        t_boundary.सङ्कलय_Element(0, स्थितिः);
        अङ्कः_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t पूर्व_CPPos = -1;
    int32_t पूर्व_UText_Pos = -1;
    int32_t corrected_अङ्कः_Breaks = 0;
    प्रति (int32_t i = अङ्कः_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > पूर्व_CPPos);
        int32_t utextPos =  निवेशः_Map.isValid() ? निवेशः_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(utextPos >= पूर्व_UText_Pos);
        यदि (utextPos > पूर्व_UText_Pos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(प्राप्तम्_Breaks.size() == 0 || प्राप्तम्_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            यदि (utextPos != rangeStart
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_समुच्चयः.अन्तर्भवति(utext_char32At(अन्तर्_Text, utextPos - 1)))) {
                प्राप्तम्_Breaks.push(utextPos, स्थितिः);
                corrected_अङ्कः_Breaks++;
            }
        } अन्यथा {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --अङ्कः_Breaks;
        }
        पूर्व_CPPos = cpPos;
        पूर्व_UText_Pos = utextPos;
    }
    (शून्य)पूर्व_CPPos; // suppress compiler warnings about unused variable

    UChar32 अग्र_Char = utext_char32At(अन्तर्_Text, range_अन्तः);
    यदि (!प्राप्तम्_Breaks.is_रिक्तानि() && प्राप्तम्_Breaks.peeki() == range_अन्तः) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        यदि (isPhraseBreaking) {
            यदि (!f_Digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः.अन्तर्भवति(अग्र_Char)) {
                प्राप्तम्_Breaks.popi();
                corrected_अङ्कः_Breaks--;
            }
        } अन्यथा {
            प्राप्तम्_Breaks.popi();
            corrected_अङ्कः_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    प्रत्यागम corrected_अङ्कः_Breaks;
}

शून्य CjkBreakEngine::initJapanesePhraseParameter(UError_संहिता& error) {
    आरोपय_Japanese_Extensions(error);
    आरोपय_Hiragana(error);
}

शून्य CjkBreakEngine::आरोपय_Japanese_Extensions(UError_संहिता& error) {
    ध्रुव वर्ण* चिह्नकम् = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    यदि (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(चिह्नकम्, error);
        यावत् (U_SUCCESS(error) && bundle.has_अग्र()) {
            f_Skip_समुच्चयः.puti(bundle.get_अग्र_String(error), 1, error);
        }
    }
}

शून्य CjkBreakEngine::आरोपय_Hiragana(UError_संहिता& error) {
    Unicode_समुच्चयः hiragana_Word_समुच्चयः(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_Word_समुच्चयः.compact();
    Unicode_समुच्चयः_Iterator iterator(hiragana_Word_समुच्चयः);
    यावत् (iterator.next()) {
        f_Skip_समुच्चयः.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#यदि_अन्तः

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */

