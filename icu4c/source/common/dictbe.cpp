// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 *******************************************************************************
 */

#అన్తర్భావ <utility>

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "brkeng.h"
#అన్తర్భావ "dictbe.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/chariter.h"
#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "unicode/ubrk.h"
#అన్తర్భావ "unicode/usetiter.h"
#అన్తర్భావ "ubrkimpl.h"
#అన్తర్భావ "utracimp.h"
#అన్తర్భావ "uvectr32.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "unicode/normlzr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "dictionarydata.h"

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

DictionaryBreakEngine::DictionaryBreakEngine() {
}

DictionaryBreakEngine::~DictionaryBreakEngine() {
}

UBool
DictionaryBreakEngine::handles(UChar32 c, స్థిరరాశి అక్షరం*) స్థిరరాశి {
    తిరిగి f_సమితి.కలిగియుంది(c);
}

int32_t
DictionaryBreakEngine::కనుగొను_Breaks( UText *పాఠ్యం,
                                 int32_t ప్రారంభం_Pos,
                                 int32_t చివర_Pos,
                                 UVector32 &దొరికింది_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    int32_t ఫలితం = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_సమితి_Native_సూచీ(పాఠ్యం, ప్రారంభం_Pos);
    int32_t ప్రారంభం = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
    int32_t current;
    int32_t range_ప్రారంభం;
    int32_t range_చివర;
    UChar32 c = utext_current32(పాఠ్యం);
    అయితే ((current = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < చివర_Pos && f_సమితి.కలిగియుంది(c)) {
        utext_next32(పాఠ్యం);         // TODO:  recast loop for postincrement
        c = utext_current32(పాఠ్యం);
    }
    range_ప్రారంభం = ప్రారంభం;
    range_చివర = current;
    ఫలితం = divide_పైకి_Dictionary_Range(పాఠ్యం, range_ప్రారంభం, range_చివర, దొరికింది_Breaks, isPhraseBreaking, స్థితి_2);
    utext_సమితి_Native_సూచీ(పాఠ్యం, current);
    
    తిరిగి ఫలితం;
}

శూన్య
DictionaryBreakEngine::సమితి_Characters( స్థిరరాశి Unicode_సమితి &సమితి ) {
    f_సమితి = సమితి;
    // Compact for caching
    f_సమితి.compact();
}

/*
 ******************************************************************
 * PossibleWord
 */

// Helper class for improving readability of the Thai/Lao/Khmer word break
// algorithm. The implementation is completely inline.

// List size, limited by the maximum number of words in the dictionary
// that form a nested sequence.
స్థిర స్థిరరాశి int32_t POSSIBLE_మాట_LIST_MAX = 20;

తరగతి Possible_మాట {
అంతరంగ:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   ఉపసర్గ;     // The longest match with a dictionary word
    int32_t   స్థానభ్రంశం;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_మాట_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_మాట_LIST_MAX];   // Word Lengths, in code points.

బహిరంగ:
    Possible_మాట() : count(0), ఉపసర్గ(0), స్థానభ్రంశం(-1), mark(0), current(0) {}
    ~Possible_మాట() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *పాఠ్యం, DictionaryMatcher *dict, int32_t range_చివర );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *పాఠ్యం );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     వెనుక_పైకి( UText *పాఠ్యం );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_ఉపసర్గ() { తిరిగి ఉపసర్గ; }
  
    // Mark the current candidate as the one we like
    శూన్య      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { తిరిగి cpLengths[mark]; }
};


int32_t Possible_మాట::candidates( UText *పాఠ్యం, DictionaryMatcher *dict, int32_t range_చివర ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t ప్రారంభం = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
    ఒకవేళ (ప్రారంభం != స్థానభ్రంశం) {
        స్థానభ్రంశం = ప్రారంభం;
        count = dict->matches(పాఠ్యం, range_చివర-ప్రారంభం, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, శూన్యసూచిక, &ఉపసర్గ);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        ఒకవేళ (count <= 0) {
            utext_సమితి_Native_సూచీ(పాఠ్యం, ప్రారంభం);
        }
    }
    ఒకవేళ (count > 0) {
        utext_సమితి_Native_సూచీ(పాఠ్యం, ప్రారంభం+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    తిరిగి count;
}

int32_t
Possible_మాట::acceptMarked( UText *పాఠ్యం ) {
    utext_సమితి_Native_సూచీ(పాఠ్యం, స్థానభ్రంశం + cuLengths[mark]);
    తిరిగి cuLengths[mark];
}


UBool
Possible_మాట::వెనుక_పైకి( UText *పాఠ్యం ) {
    ఒకవేళ (current > 0) {
        utext_సమితి_Native_సూచీ(పాఠ్యం, స్థానభ్రంశం + cuLengths[--current]);
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

/*
 ******************************************************************
 * ThaiBreakEngine
 */

// How many words in a row are "good enough"?
స్థిర స్థిరరాశి int32_t THAI_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
స్థిర స్థిరరాశి int32_t THAI_మూలం_COMBINE_పరిమితి = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
స్థిర స్థిరరాశి int32_t THAI_ఉపసర్గ_COMBINE_పరిమితి = 3;

// Elision character
స్థిర స్థిరరాశి int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
స్థిర స్థిరరాశి int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
స్థిర స్థిరరాశి int32_t THAI_MIN_మాట = 2;

// Minimum number of characters for two words
స్థిర స్థిరరాశి int32_t THAI_MIN_మాట_పరిధి = THAI_MIN_మాట * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_సంకేతం &స్థితి_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_నమోదు(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_సమితి thai_మాట_సమితి(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        సమితి_Characters(thai_మాట_సమితి);
    }
    f_Mark_సమితి.apply_నమూనా(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), స్థితి_2);
    f_Mark_సమితి.కూడు(0x0020);
    f_చివర_మాట_సమితి = thai_మాట_సమితి;
    f_చివర_మాట_సమితి.remove(0x0E31);             // MAI HAN-AKAT
    f_చివర_మాట_సమితి.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_మాట_సమితి.కూడు(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_మాట_సమితి.కూడు(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_సమితి.కూడు(THAI_PAIYANNOI);
    f_Suffix_సమితి.కూడు(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_సమితి.compact();
    f_చివర_మాట_సమితి.compact();
    f_Begin_మాట_సమితి.compact();
    f_Suffix_సమితి.compact();
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    తొలగించు fDictionary;
}

int32_t
ThaiBreakEngine::divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                                int32_t range_ప్రారంభం,
                                                int32_t range_చివర,
                                                UVector32 &దొరికింది_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    utext_సమితి_Native_సూచీ(పాఠ్యం, range_ప్రారంభం);
    utext_moveIndex32(పాఠ్యం, THAI_MIN_మాట_పరిధి);
    ఒకవేళ (utext_get_Native_సూచీ(పాఠ్యం) >= range_చివర) {
        తిరిగి 0;       // Not enough characters for two words
    }
    utext_సమితి_Native_సూచీ(పాఠ్యం, range_ప్రారంభం);


    uint32_t పదాలు_దొరికింది = 0;
    int32_t cp_మాట_నిడివి = 0;    // Word Length in Code Points.
    int32_t cu_మాట_నిడివి = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    Possible_మాట పదాలు[THAI_LOOKAHEAD];
    
    utext_సమితి_Native_సూచీ(పాఠ్యం, range_ప్రారంభం);
    
    అయితే (U_SUCCESS(స్థితి_2) && (current = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < range_చివర) {
        cp_మాట_నిడివి = 0;
        cu_మాట_నిడివి = 0;

        // Look for candidate words at the current position
        int32_t candidates = పదాలు[పదాలు_దొరికింది%THAI_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర);
        
        // If we found exactly one, use that
        ఒకవేళ (candidates == 1) {
            cu_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % THAI_LOOKAHEAD].acceptMarked(పాఠ్యం);
            cp_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % THAI_LOOKAHEAD].markedCPLength();
            పదాలు_దొరికింది += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        లేకపోతే ఒకవేళ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) >= range_చివర) {
                వెళ్ళు దొరికింది_ఉత్తమ;
            }
            చేయి {
                ఒకవేళ (పదాలు[(పదాలు_దొరికింది + 1) % THAI_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    పదాలు[పదాలు_దొరికింది%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) >= range_చివర) {
                        వెళ్ళు దొరికింది_ఉత్తమ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    చేయి {
                        // If we find a third word, stop right away
                        ఒకవేళ (పదాలు[(పదాలు_దొరికింది + 2) % THAI_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర)) {
                            పదాలు[పదాలు_దొరికింది % THAI_LOOKAHEAD].markCurrent();
                            వెళ్ళు దొరికింది_ఉత్తమ;
                        }
                    }
                    అయితే (పదాలు[(పదాలు_దొరికింది + 1) % THAI_LOOKAHEAD].వెనుక_పైకి(పాఠ్యం));
                }
            }
            అయితే (పదాలు[పదాలు_దొరికింది % THAI_LOOKAHEAD].వెనుక_పైకి(పాఠ్యం));
దొరికింది_ఉత్తమ:
            // Set UText position to after the accepted word.
            cu_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % THAI_LOOKAHEAD].acceptMarked(పాఠ్యం);
            cp_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % THAI_LOOKAHEAD].markedCPLength();
            పదాలు_దొరికింది += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) < range_చివర && cp_మాట_నిడివి < THAI_మూలం_COMBINE_పరిమితి) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ఒకవేళ (పదాలు[పదాలు_దొరికింది % THAI_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) <= 0
                  && (cu_మాట_నిడివి == 0
                      || పదాలు[పదాలు_దొరికింది%THAI_LOOKAHEAD].longest_ఉపసర్గ() < THAI_ఉపసర్గ_COMBINE_పరిమితి)) {
                // Look for a plausible word boundary
                int32_t మిగిలిన = range_చివర - (current+cu_మాట_నిడివి);
                UChar32 pc;
                int32_t chars = 0;
                కొరకు (;;) {
                    int32_t pc_సూచీ = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
                    pc = utext_next32(పాఠ్యం);
                    int32_t pc_కొలత = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - pc_సూచీ;
                    chars += pc_కొలత;
                    మిగిలిన -= pc_కొలత;
                    ఒకవేళ (మిగిలిన <= 0) {
                        ఆపు;
                    }
                    uc = utext_current32(పాఠ్యం);
                    ఒకవేళ (f_చివర_మాట_సమితి.కలిగియుంది(pc) && f_Begin_మాట_సమితి.కలిగియుంది(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t అంకె_candidates = పదాలు[(పదాలు_దొరికింది + 1) % THAI_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర);
                        utext_సమితి_Native_సూచీ(పాఠ్యం, current + cu_మాట_నిడివి + chars);
                        ఒకవేళ (అంకె_candidates > 0) {
                            ఆపు;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ఒకవేళ (cu_మాట_నిడివి <= 0) {
                    పదాలు_దొరికింది += 1;
                }
                
                // Update the length with the passed-over characters
                cu_మాట_నిడివి += chars;
            }
            లేకపోతే {
                // Back up to where we were for next iteration
                utext_సమితి_Native_సూచీ(పాఠ్యం, current+cu_మాట_నిడివి);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        అయితే ((currPos = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < range_చివర && f_Mark_సమితి.కలిగియుంది(utext_current32(పాఠ్యం))) {
            utext_next32(పాఠ్యం);
            cu_మాట_నిడివి += static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) < range_చివర && cu_మాట_నిడివి > 0) {
            ఒకవేళ (పదాలు[పదాలు_దొరికింది%THAI_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) <= 0
                && f_Suffix_సమితి.కలిగియుంది(uc = utext_current32(పాఠ్యం))) {
                ఒకవేళ (uc == THAI_PAIYANNOI) {
                    ఒకవేళ (!f_Suffix_సమితి.కలిగియుంది(utext_previous32(పాఠ్యం))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(పాఠ్యం);
                        int32_t paiyannoi_సూచీ = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
                        utext_next32(పాఠ్యం);
                        cu_మాట_నిడివి += static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - paiyannoi_సూచీ; // Add PAIYANNOI to word
                        uc = utext_current32(పాఠ్యం);     // Fetch next character
                    }
                    లేకపోతే {
                        // Restore prior position
                        utext_next32(పాఠ్యం);
                    }
                }
                ఒకవేళ (uc == THAI_MAIYAMOK) {
                    ఒకవేళ (utext_previous32(పాఠ్యం) != THAI_MAIYAMOK) {
                        // Skip over previous end and MAIYAMOK
                        utext_next32(పాఠ్యం);
                        int32_t maiyamok_సూచీ = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
                        utext_next32(పాఠ్యం);
                        cu_మాట_నిడివి += static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - maiyamok_సూచీ; // Add MAIYAMOK to word
                    }
                    లేకపోతే {
                        // Restore prior position
                        utext_next32(పాఠ్యం);
                    }
                }
            }
            లేకపోతే {
                utext_సమితి_Native_సూచీ(పాఠ్యం, current+cu_మాట_నిడివి);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        ఒకవేళ (cu_మాట_నిడివి > 0) {
            దొరికింది_Breaks.push((current+cu_మాట_నిడివి), స్థితి_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ఒకవేళ (దొరికింది_Breaks.peeki() >= range_చివర) {
        (శూన్య) దొరికింది_Breaks.popi();
        పదాలు_దొరికింది -= 1;
    }

    తిరిగి పదాలు_దొరికింది;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
స్థిర స్థిరరాశి int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
స్థిర స్థిరరాశి int32_t LAO_మూలం_COMBINE_పరిమితి = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
స్థిర స్థిరరాశి int32_t LAO_ఉపసర్గ_COMBINE_పరిమితి = 3;

// Minimum word size
స్థిర స్థిరరాశి int32_t LAO_MIN_మాట = 2;

// Minimum number of characters for two words
స్థిర స్థిరరాశి int32_t LAO_MIN_మాట_పరిధి = LAO_MIN_మాట * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_సంకేతం &స్థితి_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_నమోదు(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_సమితి lao_మాట_సమితి(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        సమితి_Characters(lao_మాట_సమితి);
    }
    f_Mark_సమితి.apply_నమూనా(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), స్థితి_2);
    f_Mark_సమితి.కూడు(0x0020);
    f_చివర_మాట_సమితి = lao_మాట_సమితి;
    f_చివర_మాట_సమితి.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_మాట_సమితి.కూడు(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_మాట_సమితి.కూడు(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_మాట_సమితి.కూడు(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_సమితి.compact();
    f_చివర_మాట_సమితి.compact();
    f_Begin_మాట_సమితి.compact();
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

LaoBreakEngine::~LaoBreakEngine() {
    తొలగించు fDictionary;
}

int32_t
LaoBreakEngine::divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                                int32_t range_ప్రారంభం,
                                                int32_t range_చివర,
                                                UVector32 &దొరికింది_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    ఒకవేళ ((range_చివర - range_ప్రారంభం) < LAO_MIN_మాట_పరిధి) {
        తిరిగి 0;       // Not enough characters for two words
    }

    uint32_t పదాలు_దొరికింది = 0;
    int32_t cp_మాట_నిడివి = 0;
    int32_t cu_మాట_నిడివి = 0;
    int32_t current;
    Possible_మాట పదాలు[LAO_LOOKAHEAD];

    utext_సమితి_Native_సూచీ(పాఠ్యం, range_ప్రారంభం);

    అయితే (U_SUCCESS(స్థితి_2) && (current = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < range_చివర) {
        cu_మాట_నిడివి = 0;
        cp_మాట_నిడివి = 0;

        // Look for candidate words at the current position
        int32_t candidates = పదాలు[పదాలు_దొరికింది%LAO_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర);
        
        // If we found exactly one, use that
        ఒకవేళ (candidates == 1) {
            cu_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % LAO_LOOKAHEAD].acceptMarked(పాఠ్యం);
            cp_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % LAO_LOOKAHEAD].markedCPLength();
            పదాలు_దొరికింది += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        లేకపోతే ఒకవేళ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ఒకవేళ (utext_get_Native_సూచీ(పాఠ్యం) >= range_చివర) {
                వెళ్ళు దొరికింది_ఉత్తమ;
            }
            చేయి {
                ఒకవేళ (పదాలు[(పదాలు_దొరికింది + 1) % LAO_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    పదాలు[పదాలు_దొరికింది%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) >= range_చివర) {
                        వెళ్ళు దొరికింది_ఉత్తమ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    చేయి {
                        // If we find a third word, stop right away
                        ఒకవేళ (పదాలు[(పదాలు_దొరికింది + 2) % LAO_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర)) {
                            పదాలు[పదాలు_దొరికింది % LAO_LOOKAHEAD].markCurrent();
                            వెళ్ళు దొరికింది_ఉత్తమ;
                        }
                    }
                    అయితే (పదాలు[(పదాలు_దొరికింది + 1) % LAO_LOOKAHEAD].వెనుక_పైకి(పాఠ్యం));
                }
            }
            అయితే (పదాలు[పదాలు_దొరికింది % LAO_LOOKAHEAD].వెనుక_పైకి(పాఠ్యం));
దొరికింది_ఉత్తమ:
            cu_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % LAO_LOOKAHEAD].acceptMarked(పాఠ్యం);
            cp_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % LAO_LOOKAHEAD].markedCPLength();
            పదాలు_దొరికింది += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) < range_చివర && cp_మాట_నిడివి < LAO_మూలం_COMBINE_పరిమితి) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ఒకవేళ (పదాలు[పదాలు_దొరికింది % LAO_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) <= 0
                  && (cu_మాట_నిడివి == 0
                      || పదాలు[పదాలు_దొరికింది%LAO_LOOKAHEAD].longest_ఉపసర్గ() < LAO_ఉపసర్గ_COMBINE_పరిమితి)) {
                // Look for a plausible word boundary
                int32_t మిగిలిన = range_చివర - (current + cu_మాట_నిడివి);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                కొరకు (;;) {
                    int32_t pc_సూచీ = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
                    pc = utext_next32(పాఠ్యం);
                    int32_t pc_కొలత = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - pc_సూచీ;
                    chars += pc_కొలత;
                    మిగిలిన -= pc_కొలత;
                    ఒకవేళ (మిగిలిన <= 0) {
                        ఆపు;
                    }
                    uc = utext_current32(పాఠ్యం);
                    ఒకవేళ (f_చివర_మాట_సమితి.కలిగియుంది(pc) && f_Begin_మాట_సమితి.కలిగియుంది(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t అంకె_candidates = పదాలు[(పదాలు_దొరికింది + 1) % LAO_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర);
                        utext_సమితి_Native_సూచీ(పాఠ్యం, current + cu_మాట_నిడివి + chars);
                        ఒకవేళ (అంకె_candidates > 0) {
                            ఆపు;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ఒకవేళ (cu_మాట_నిడివి <= 0) {
                    పదాలు_దొరికింది += 1;
                }
                
                // Update the length with the passed-over characters
                cu_మాట_నిడివి += chars;
            }
            లేకపోతే {
                // Back up to where we were for next iteration
                utext_సమితి_Native_సూచీ(పాఠ్యం, current + cu_మాట_నిడివి);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        అయితే ((currPos = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < range_చివర && f_Mark_సమితి.కలిగియుంది(utext_current32(పాఠ్యం))) {
            utext_next32(పాఠ్యం);
            cu_మాట_నిడివి += static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        ఒకవేళ (cu_మాట_నిడివి > 0) {
            దొరికింది_Breaks.push((current+cu_మాట_నిడివి), స్థితి_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ఒకవేళ (దొరికింది_Breaks.peeki() >= range_చివర) {
        (శూన్య) దొరికింది_Breaks.popi();
        పదాలు_దొరికింది -= 1;
    }

    తిరిగి పదాలు_దొరికింది;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
స్థిర స్థిరరాశి int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
స్థిర స్థిరరాశి int32_t BURMESE_మూలం_COMBINE_పరిమితి = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
స్థిర స్థిరరాశి int32_t BURMESE_ఉపసర్గ_COMBINE_పరిమితి = 3;

// Minimum word size
స్థిర స్థిరరాశి int32_t BURMESE_MIN_మాట = 2;

// Minimum number of characters for two words
స్థిర స్థిరరాశి int32_t BURMESE_MIN_మాట_పరిధి = BURMESE_MIN_మాట * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_సంకేతం &స్థితి_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_నమోదు(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_మాట_సమితి.కూడు(0x1000, 0x102A);      // basic consonants and independent vowels
    f_చివర_మాట_సమితి.apply_నమూనా(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), స్థితి_2);
    f_Mark_సమితి.apply_నమూనా(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), స్థితి_2);
    f_Mark_సమితి.కూడు(0x0020);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        సమితి_Characters(f_చివర_మాట_సమితి);
    }

    // Compact for caching.
    f_Mark_సమితి.compact();
    f_చివర_మాట_సమితి.compact();
    f_Begin_మాట_సమితి.compact();
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    తొలగించు fDictionary;
}

int32_t
BurmeseBreakEngine::divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                                int32_t range_ప్రారంభం,
                                                int32_t range_చివర,
                                                UVector32 &దొరికింది_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_సంకేతం& స్థితి_2 ) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    ఒకవేళ ((range_చివర - range_ప్రారంభం) < BURMESE_MIN_మాట_పరిధి) {
        తిరిగి 0;       // Not enough characters for two words
    }

    uint32_t పదాలు_దొరికింది = 0;
    int32_t cp_మాట_నిడివి = 0;
    int32_t cu_మాట_నిడివి = 0;
    int32_t current;
    Possible_మాట పదాలు[BURMESE_LOOKAHEAD];

    utext_సమితి_Native_సూచీ(పాఠ్యం, range_ప్రారంభం);

    అయితే (U_SUCCESS(స్థితి_2) && (current = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < range_చివర) {
        cu_మాట_నిడివి = 0;
        cp_మాట_నిడివి = 0;

        // Look for candidate words at the current position
        int32_t candidates = పదాలు[పదాలు_దొరికింది%BURMESE_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర);
        
        // If we found exactly one, use that
        ఒకవేళ (candidates == 1) {
            cu_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % BURMESE_LOOKAHEAD].acceptMarked(పాఠ్యం);
            cp_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % BURMESE_LOOKAHEAD].markedCPLength();
            పదాలు_దొరికింది += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        లేకపోతే ఒకవేళ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ఒకవేళ (utext_get_Native_సూచీ(పాఠ్యం) >= range_చివర) {
                వెళ్ళు దొరికింది_ఉత్తమ;
            }
            చేయి {
                ఒకవేళ (పదాలు[(పదాలు_దొరికింది + 1) % BURMESE_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    పదాలు[పదాలు_దొరికింది%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) >= range_చివర) {
                        వెళ్ళు దొరికింది_ఉత్తమ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    చేయి {
                        // If we find a third word, stop right away
                        ఒకవేళ (పదాలు[(పదాలు_దొరికింది + 2) % BURMESE_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర)) {
                            పదాలు[పదాలు_దొరికింది % BURMESE_LOOKAHEAD].markCurrent();
                            వెళ్ళు దొరికింది_ఉత్తమ;
                        }
                    }
                    అయితే (పదాలు[(పదాలు_దొరికింది + 1) % BURMESE_LOOKAHEAD].వెనుక_పైకి(పాఠ్యం));
                }
            }
            అయితే (పదాలు[పదాలు_దొరికింది % BURMESE_LOOKAHEAD].వెనుక_పైకి(పాఠ్యం));
దొరికింది_ఉత్తమ:
            cu_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % BURMESE_LOOKAHEAD].acceptMarked(పాఠ్యం);
            cp_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % BURMESE_LOOKAHEAD].markedCPLength();
            పదాలు_దొరికింది += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) < range_చివర && cp_మాట_నిడివి < BURMESE_మూలం_COMBINE_పరిమితి) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ఒకవేళ (పదాలు[పదాలు_దొరికింది % BURMESE_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) <= 0
                  && (cu_మాట_నిడివి == 0
                      || పదాలు[పదాలు_దొరికింది%BURMESE_LOOKAHEAD].longest_ఉపసర్గ() < BURMESE_ఉపసర్గ_COMBINE_పరిమితి)) {
                // Look for a plausible word boundary
                int32_t మిగిలిన = range_చివర - (current + cu_మాట_నిడివి);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                కొరకు (;;) {
                    int32_t pc_సూచీ = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
                    pc = utext_next32(పాఠ్యం);
                    int32_t pc_కొలత = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - pc_సూచీ;
                    chars += pc_కొలత;
                    మిగిలిన -= pc_కొలత;
                    ఒకవేళ (మిగిలిన <= 0) {
                        ఆపు;
                    }
                    uc = utext_current32(పాఠ్యం);
                    ఒకవేళ (f_చివర_మాట_సమితి.కలిగియుంది(pc) && f_Begin_మాట_సమితి.కలిగియుంది(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t అంకె_candidates = పదాలు[(పదాలు_దొరికింది + 1) % BURMESE_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర);
                        utext_సమితి_Native_సూచీ(పాఠ్యం, current + cu_మాట_నిడివి + chars);
                        ఒకవేళ (అంకె_candidates > 0) {
                            ఆపు;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ఒకవేళ (cu_మాట_నిడివి <= 0) {
                    పదాలు_దొరికింది += 1;
                }
                
                // Update the length with the passed-over characters
                cu_మాట_నిడివి += chars;
            }
            లేకపోతే {
                // Back up to where we were for next iteration
                utext_సమితి_Native_సూచీ(పాఠ్యం, current + cu_మాట_నిడివి);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        అయితే ((currPos = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < range_చివర && f_Mark_సమితి.కలిగియుంది(utext_current32(పాఠ్యం))) {
            utext_next32(పాఠ్యం);
            cu_మాట_నిడివి += static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        ఒకవేళ (cu_మాట_నిడివి > 0) {
            దొరికింది_Breaks.push((current+cu_మాట_నిడివి), స్థితి_2);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ఒకవేళ (దొరికింది_Breaks.peeki() >= range_చివర) {
        (శూన్య) దొరికింది_Breaks.popi();
        పదాలు_దొరికింది -= 1;
    }

    తిరిగి పదాలు_దొరికింది;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
స్థిర స్థిరరాశి int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
స్థిర స్థిరరాశి int32_t KHMER_మూలం_COMBINE_పరిమితి = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
స్థిర స్థిరరాశి int32_t KHMER_ఉపసర్గ_COMBINE_పరిమితి = 3;

// Minimum word size
స్థిర స్థిరరాశి int32_t KHMER_MIN_మాట = 2;

// Minimum number of characters for two words
స్థిర స్థిరరాశి int32_t KHMER_MIN_మాట_పరిధి = KHMER_MIN_మాట * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_సంకేతం &స్థితి_2)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_నమోదు(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_సమితి khmer_మాట_సమితి(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        సమితి_Characters(khmer_మాట_సమితి);
    }
    f_Mark_సమితి.apply_నమూనా(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), స్థితి_2);
    f_Mark_సమితి.కూడు(0x0020);
    f_చివర_మాట_సమితి = khmer_మాట_సమితి;
    f_Begin_మాట_సమితి.కూడు(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_చివర_మాట_సమితి.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_సమితి.compact();
    f_చివర_మాట_సమితి.compact();
    f_Begin_మాట_సమితి.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    తొలగించు fDictionary;
}

int32_t
KhmerBreakEngine::divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                                int32_t range_ప్రారంభం,
                                                int32_t range_చివర,
                                                UVector32 &దొరికింది_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_సంకేతం& స్థితి_2 ) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    ఒకవేళ ((range_చివర - range_ప్రారంభం) < KHMER_MIN_మాట_పరిధి) {
        తిరిగి 0;       // Not enough characters for two words
    }

    uint32_t పదాలు_దొరికింది = 0;
    int32_t cp_మాట_నిడివి = 0;
    int32_t cu_మాట_నిడివి = 0;
    int32_t current;
    Possible_మాట పదాలు[KHMER_LOOKAHEAD];

    utext_సమితి_Native_సూచీ(పాఠ్యం, range_ప్రారంభం);

    అయితే (U_SUCCESS(స్థితి_2) && (current = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < range_చివర) {
        cu_మాట_నిడివి = 0;
        cp_మాట_నిడివి = 0;

        // Look for candidate words at the current position
        int32_t candidates = పదాలు[పదాలు_దొరికింది%KHMER_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర);

        // If we found exactly one, use that
        ఒకవేళ (candidates == 1) {
            cu_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].acceptMarked(పాఠ్యం);
            cp_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].markedCPLength();
            పదాలు_దొరికింది += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        లేకపోతే ఒకవేళ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) >= range_చివర) {
                వెళ్ళు దొరికింది_ఉత్తమ;
            }
            చేయి {
                ఒకవేళ (పదాలు[(పదాలు_దొరికింది + 1) % KHMER_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) >= range_చివర) {
                        వెళ్ళు దొరికింది_ఉత్తమ;
                    }

                    // See if any of the possible second words is followed by a third word
                    చేయి {
                        // If we find a third word, stop right away
                        ఒకవేళ (పదాలు[(పదాలు_దొరికింది + 2) % KHMER_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర)) {
                            పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].markCurrent();
                            వెళ్ళు దొరికింది_ఉత్తమ;
                        }
                    }
                    అయితే (పదాలు[(పదాలు_దొరికింది + 1) % KHMER_LOOKAHEAD].వెనుక_పైకి(పాఠ్యం));
                }
            }
            అయితే (పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].వెనుక_పైకి(పాఠ్యం));
దొరికింది_ఉత్తమ:
            cu_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].acceptMarked(పాఠ్యం);
            cp_మాట_నిడివి = పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].markedCPLength();
            పదాలు_దొరికింది += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ఒకవేళ (static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) < range_చివర && cp_మాట_నిడివి < KHMER_మూలం_COMBINE_పరిమితి) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ఒకవేళ (పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర) <= 0
                  && (cu_మాట_నిడివి == 0
                      || పదాలు[పదాలు_దొరికింది % KHMER_LOOKAHEAD].longest_ఉపసర్గ() < KHMER_ఉపసర్గ_COMBINE_పరిమితి)) {
                // Look for a plausible word boundary
                int32_t మిగిలిన = range_చివర - (current+cu_మాట_నిడివి);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                కొరకు (;;) {
                    int32_t pc_సూచీ = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
                    pc = utext_next32(పాఠ్యం);
                    int32_t pc_కొలత = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - pc_సూచీ;
                    chars += pc_కొలత;
                    మిగిలిన -= pc_కొలత;
                    ఒకవేళ (మిగిలిన <= 0) {
                        ఆపు;
                    }
                    uc = utext_current32(పాఠ్యం);
                    ఒకవేళ (f_చివర_మాట_సమితి.కలిగియుంది(pc) && f_Begin_మాట_సమితి.కలిగియుంది(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t అంకె_candidates = పదాలు[(పదాలు_దొరికింది + 1) % KHMER_LOOKAHEAD].candidates(పాఠ్యం, fDictionary, range_చివర);
                        utext_సమితి_Native_సూచీ(పాఠ్యం, current+cu_మాట_నిడివి+chars);
                        ఒకవేళ (అంకె_candidates > 0) {
                            ఆపు;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                ఒకవేళ (cu_మాట_నిడివి <= 0) {
                    పదాలు_దొరికింది += 1;
                }

                // Update the length with the passed-over characters
                cu_మాట_నిడివి += chars;
            }
            లేకపోతే {
                // Back up to where we were for next iteration
                utext_సమితి_Native_సూచీ(పాఠ్యం, current+cu_మాట_నిడివి);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        అయితే ((currPos = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < range_చివర && f_Mark_సమితి.కలిగియుంది(utext_current32(పాఠ్యం))) {
            utext_next32(పాఠ్యం);
            cu_మాట_నిడివి += static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - currPos;
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
        ఒకవేళ (cu_మాట_నిడివి > 0) {
            దొరికింది_Breaks.push((current+cu_మాట_నిడివి), స్థితి_2);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    ఒకవేళ (దొరికింది_Breaks.peeki() >= range_చివర) {
        (శూన్య) దొరికింది_Breaks.popi();
        పదాలు_దొరికింది -= 1;
    }

    తిరిగి పదాలు_దొరికింది;
}

#ఒకవేళ !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
స్థిర స్థిరరాశి uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_సంకేతం &స్థితి_2)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(అబద్ధం) {
    UTRACE_నమోదు(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = శూన్యసూచిక;
    nfkcNorm2 = Normalizer2::getNFKCInstance(స్థితి_2);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_మాట_సమితి.apply_నమూనా(UnicodeString(u"[\\uac00-\\ud7a3]"), స్థితి_2);
    f_Hangul_మాట_సమితి.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_సమితి.apply_నమూనా(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), స్థితి_2);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_సమితి.compact();
    f_Close_Punctuation_సమితి.apply_నమూనా(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), స్థితి_2);
    f_Close_Punctuation_సమితి.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    ఒకవేళ (type == kKorean) {
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            సమితి_Characters(f_Hangul_మాట_సమితి);
        }
    } లేకపోతే { // Chinese and Japanese
        Unicode_సమితి cj_సమితి(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), స్థితి_2);
        isCj = నిజం;
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            సమితి_Characters(cj_సమితి);
#ఒకవేళ UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = కొత్త MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_సమితి,
                                               f_Close_Punctuation_సమితి, స్థితి_2);
            ఒకవేళ (fMlBreakEngine == శూన్యసూచిక) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
#లేకపోతే
            initJapanesePhraseParameter(స్థితి_2);
#అయితే_ముగింపు
        }
    }
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

CjkBreakEngine::~CjkBreakEngine(){
    తొలగించు fDictionary;
    తొలగించు fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
స్థిర స్థిరరాశి int32_t k_Max_Katakana_నిడివి = 8;
స్థిర స్థిరరాశి int32_t k_Max_Katakana_సమూహం_నిడివి = 20;
స్థిర స్థిరరాశి uint32_t maxSnlp = 255;

స్థిర అంతఃవరుస uint32_t get_Katakana_ఖర్చు(int32_t మాట_నిడివి){
    //TODO: fill array with actual values from dictionary!
    స్థిర స్థిరరాశి uint32_t katakana_ఖర్చు[k_Max_Katakana_నిడివి + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    తిరిగి (మాట_నిడివి > k_Max_Katakana_నిడివి) ? 8192 : katakana_ఖర్చు[మాట_నిడివి];
}

స్థిర అంతఃవరుస తార్కిక isKatakana(UChar32 value) {
    తిరిగి (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

స్థిర అంతఃవరుస int32_t utext_i32_flag(int32_t బిట్టు_సూచీ) {
    తిరిగి static_cast<int32_t>(1) << బిట్టు_సూచీ;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_పైకి_Dictionary_Range( UText *లోపల_పాఠ్యం,
        int32_t range_ప్రారంభం,
        int32_t range_చివర,
        UVector32 &దొరికింది_Breaks,
        UBool isPhraseBreaking,
        UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    ఒకవేళ (range_ప్రారంభం >= range_చివర) {
        తిరిగి 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString లోపల_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    Local_సూచిక<UVector32>     ఆదానం_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    ఒకవేళ ((లోపల_పాఠ్యం->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         లోపల_పాఠ్యం->chunk_Native_ప్రారంభం <= range_ప్రారంభం &&
         లోపల_పాఠ్యం->chunk_Native_హద్దు >= range_చివర   &&
         లోపల_పాఠ్యం->native_Indexing_హద్దు >= range_చివర - లోపల_పాఠ్యం->chunk_Native_ప్రారంభం) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        లోపల_String.సమితి_గమ్యం(అబద్ధం,
                       లోపల_పాఠ్యం->chunkContents + range_ప్రారంభం - లోపల_పాఠ్యం->chunk_Native_ప్రారంభం,
                       range_చివర - range_ప్రారంభం);
    } లేకపోతే {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_సమితి_Native_సూచీ(లోపల_పాఠ్యం, range_ప్రారంభం);
        int32_t హద్దు = range_చివర;
        U_ASSERT(హద్దు <= utext_native_నిడివి(లోపల_పాఠ్యం));
        ఒకవేళ (హద్దు > utext_native_నిడివి(లోపల_పాఠ్యం)) {
            హద్దు = static_cast<int32_t>(utext_native_నిడివి(లోపల_పాఠ్యం));
        }
        ఆదానం_Map.adopt_Instead_And_Check_Error_సంకేతం(కొత్త UVector32(స్థితి_2), స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి 0;
        }
        అయితే (utext_get_Native_సూచీ(లోపల_పాఠ్యం) < హద్దు) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_సూచీ(లోపల_పాఠ్యం));
            UChar32 c = utext_next32(లోపల_పాఠ్యం);
            U_ASSERT(c != U_SENTINEL);
            లోపల_String.append(c);
            అయితే (ఆదానం_Map->size() < లోపల_String.length()) {
                ఆదానం_Map->కూడు_Element(nativePosition, స్థితి_2);
            }
        }
        ఆదానం_Map->కూడు_Element(హద్దు, స్థితి_2);
    }


    ఒకవేళ (!nfkcNorm2->isNormalized(లోపల_String, స్థితి_2)) {
        UnicodeString normalized_ఆదానం;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        Local_సూచిక<UVector32> normalizedMap(కొత్త UVector32(స్థితి_2), స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        కొరకు (int32_t srcI = 0; srcI < లోపల_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragment_ప్రారంభం_I = srcI;
            UChar32 c = లోపల_String.char32At(srcI);
            కొరకు (;;) {
                fragment.append(c);
                srcI = లోపల_String.moveIndex32(srcI, 1);
                ఒకవేళ (srcI == లోపల_String.length()) {
                    ఆపు;
                }
                c = లోపల_String.char32At(srcI);
                ఒకవేళ (nfkcNorm2->hasBoundaryBefore(c)) {
                    ఆపు;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, స్థితి_2);
            normalized_ఆదానం.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragment_Original_ప్రారంభం = ఆదానం_Map.isValid() ?
                    ఆదానం_Map->elementAti(fragment_ప్రారంభం_I) : fragment_ప్రారంభం_I+range_ప్రారంభం;
            అయితే (normalizedMap->size() < normalized_ఆదానం.length()) {
                normalizedMap->కూడు_Element(fragment_Original_ప్రారంభం, స్థితి_2);
                ఒకవేళ (U_FAILURE(స్థితి_2)) {
                    ఆపు;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_ఆదానం.length());
        int32_t native_చివర = ఆదానం_Map.isValid() ?
                ఆదానం_Map->elementAti(లోపల_String.length()) : లోపల_String.length()+range_ప్రారంభం;
        normalizedMap->కూడు_Element(native_చివర, స్థితి_2);

        ఆదానం_Map = std::move(normalizedMap);
        లోపల_String = std::move(normalized_ఆదానం);
    }

    int32_t అంకె_సంకేతం_Pts = లోపల_String.లెక్క_Char32();
    ఒకవేళ (అంకె_సంకేతం_Pts != లోపల_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = ఆదానం_Map.isValid();
        ఒకవేళ (!hadExistingMap) {
            ఆదానం_Map.adopt_Instead_And_Check_Error_సంకేతం(కొత్త UVector32(స్థితి_2), స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తిరిగి 0;
            }
        }
        int32_t cpIdx = 0;
        కొరకు (int32_t cuIdx = 0; ; cuIdx = లోపల_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            ఒకవేళ (hadExistingMap) {
                ఆదానం_Map->సమితి_Element_At(ఆదానం_Map->elementAti(cuIdx), cpIdx);
            } లేకపోతే {
                ఆదానం_Map->కూడు_Element(cuIdx+range_ప్రారంభం, స్థితి_2);
            }
            cpIdx++;
            ఒకవేళ (cuIdx == లోపల_String.length()) {
               ఆపు;
            }
        }
    }

#ఒకవేళ UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    ఒకవేళ (isPhraseBreaking && isCj) {
        తిరిగి fMlBreakEngine->divide_పైకి_Range(లోపల_పాఠ్యం, range_ప్రారంభం, range_చివర, దొరికింది_Breaks, లోపల_String,
                                             ఆదానం_Map, స్థితి_2);
    }
#అయితే_ముగింపు

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 ఉత్తమ_Snlp(అంకె_సంకేతం_Pts + 1, స్థితి_2);
    ఉత్తమ_Snlp.కూడు_Element(0, స్థితి_2);
    కొరకు(int32_t i = 1; i <= అంకె_సంకేతం_Pts; i++) {
        ఉత్తమ_Snlp.కూడు_Element(kuint32max, స్థితి_2);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 prev(అంకె_సంకేతం_Pts + 1, స్థితి_2);
    కొరకు(int32_t i = 0; i <= అంకె_సంకేతం_Pts; i++){
        prev.కూడు_Element(-1, స్థితి_2);
    }

    స్థిరరాశి int32_t max_మాట_కొలత = 20;
    UVector32 విలువలు(అంకె_సంకేతం_Pts, స్థితి_2);
    విలువలు.సమితి_కొలత(అంకె_సంకేతం_Pts);
    UVector32 lengths(అంకె_సంకేతం_Pts, స్థితి_2);
    lengths.సమితి_కొలత(అంకె_సంకేతం_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &లోపల_String, &స్థితి_2);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    తార్కిక is_prev_katakana = అబద్ధం;
    కొరకు (int32_t i = 0;  i < అంకె_సంకేతం_Pts;  ++i, ix = లోపల_String.moveIndex32(ix, 1)) {
        ఒకవేళ (static_cast<uint32_t>(ఉత్తమ_Snlp.elementAti(i)) == kuint32max) {
            కొనసాగు;
        }

        int32_t count;
        utext_సమితి_Native_సూచీ(&fu, ix);
        count = fDictionary->matches(&fu, max_మాట_కొలత, అంకె_సంకేతం_Pts,
                             శూన్యసూచిక, lengths.get_బఫరు(), విలువలు.get_బఫరు(), శూన్యసూచిక);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        ఒకవేళ ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_మాట_సమితి.కలిగియుంది(లోపల_String.char32At(ix))) {
            విలువలు.సమితి_Element_At(maxSnlp, count);   // 255
            lengths.సమితి_Element_At(1, count++);
        }

        కొరకు (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(ఉత్తమ_Snlp.elementAti(i)) + static_cast<uint32_t>(విలువలు.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            ఒకవేళ (newSnlp < static_cast<uint32_t>(ఉత్తమ_Snlp.elementAti(ln_j_i))) {
                ఉత్తమ_Snlp.సమితి_Element_At(newSnlp, ln_j_i);
                prev.సమితి_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        తార్కిక is_katakana = isKatakana(లోపల_String.char32At(ix));
        int32_t katakana_నిరంతరం_నిడివి = 1;
        ఒకవేళ (!is_prev_katakana && is_katakana) {
            int32_t j = లోపల_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            అయితే (j < లోపల_String.length() && katakana_నిరంతరం_నిడివి < k_Max_Katakana_సమూహం_నిడివి &&
                    isKatakana(లోపల_String.char32At(j))) {
                j = లోపల_String.moveIndex32(j, 1);
                katakana_నిరంతరం_నిడివి++;
            }
            ఒకవేళ (katakana_నిరంతరం_నిడివి < k_Max_Katakana_సమూహం_నిడివి) {
                uint32_t newSnlp = ఉత్తమ_Snlp.elementAti(i) + get_Katakana_ఖర్చు(katakana_నిరంతరం_నిడివి);
                ఒకవేళ (newSnlp < static_cast<uint32_t>(ఉత్తమ_Snlp.elementAti(i + katakana_నిరంతరం_నిడివి))) {
                    ఉత్తమ_Snlp.సమితి_Element_At(newSnlp, i+katakana_నిరంతరం_నిడివి);
                    prev.సమితి_Element_At(i, i+katakana_నిరంతరం_నిడివి);  // prev[j] = i;
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
    UVector32 t_boundary(అంకె_సంకేతం_Pts+1, స్థితి_2);

    int32_t అంకె_Breaks = 0;
    // No segmentation found, set boundary to end of range
    ఒకవేళ (static_cast<uint32_t>(ఉత్తమ_Snlp.elementAti(అంకె_సంకేతం_Pts)) == kuint32max) {
        t_boundary.కూడు_Element(అంకె_సంకేతం_Pts, స్థితి_2);
        అంకె_Breaks++;
    } లేకపోతే ఒకవేళ (isPhraseBreaking) {
        t_boundary.కూడు_Element(అంకె_సంకేతం_Pts, స్థితి_2);
        ఒకవేళ(U_SUCCESS(స్థితి_2)) {
            అంకె_Breaks++;
            int32_t prevIdx = అంకె_సంకేతం_Pts;

            int32_t సంకేతం_Unit_Idx = -1;
            int32_t prev_సంకేతం_Unit_Idx = -1;
            int32_t length = -1;
            కొరకు (int32_t i = prev.elementAti(అంకె_సంకేతం_Pts); i > 0; i = prev.elementAti(i)) {
                సంకేతం_Unit_Idx = లోపల_String.moveIndex32(0, i);
                prev_సంకేతం_Unit_Idx = లోపల_String.moveIndex32(0, prevIdx);
                // Calculate the length by using the code unit.
                length = prev_సంకేతం_Unit_Idx - సంకేతం_Unit_Idx;
                prevIdx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                ఒకవేళ (!f_Skip_సమితి.కలిగియుంది_కీలకం(లోపల_String.తాత్కాలిక_Sub_String(సంకేతం_Unit_Idx, length))
                    && (!isKatakana(లోపల_String.char32At(లోపల_String.moveIndex32(సంకేతం_Unit_Idx, -1)))
                           || !isKatakana(లోపల_String.char32At(సంకేతం_Unit_Idx)))) {
                    t_boundary.కూడు_Element(i, స్థితి_2);
                    అంకె_Breaks++;
                }
            }
        }
    } లేకపోతే {
        కొరకు (int32_t i = అంకె_సంకేతం_Pts; i > 0; i = prev.elementAti(i)) {
            t_boundary.కూడు_Element(i, స్థితి_2);
            అంకె_Breaks++;
        }
        U_ASSERT(prev.elementAti(t_boundary.elementAti(అంకె_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    ఒకవేళ (దొరికింది_Breaks.size() == 0 || దొరికింది_Breaks.peeki() < range_ప్రారంభం) {
        t_boundary.కూడు_Element(0, స్థితి_2);
        అంకె_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_అంకె_Breaks = 0;
    కొరకు (int32_t i = అంకె_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > prevCPPos);
        int32_t utextPos =  ఆదానం_Map.isValid() ? ఆదానం_Map->elementAti(cpPos) : cpPos + range_ప్రారంభం;
        U_ASSERT(utextPos >= prevUTextPos);
        ఒకవేళ (utextPos > prevUTextPos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(దొరికింది_Breaks.size() == 0 || దొరికింది_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            ఒకవేళ (utextPos != range_ప్రారంభం
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_సమితి.కలిగియుంది(utext_char32At(లోపల_పాఠ్యం, utextPos - 1)))) {
                దొరికింది_Breaks.push(utextPos, స్థితి_2);
                corrected_అంకె_Breaks++;
            }
        } లేకపోతే {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --అంకె_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (శూన్య)prevCPPos; // suppress compiler warnings about unused variable

    UChar32 తదుపరి_Char = utext_char32At(లోపల_పాఠ్యం, range_చివర);
    ఒకవేళ (!దొరికింది_Breaks.is_ఖాళీలు() && దొరికింది_Breaks.peeki() == range_చివర) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        ఒకవేళ (isPhraseBreaking) {
            ఒకవేళ (!f_Digit_Or_Open_Punctuation_Or_Alphabet_సమితి.కలిగియుంది(తదుపరి_Char)) {
                దొరికింది_Breaks.popi();
                corrected_అంకె_Breaks--;
            }
        } లేకపోతే {
            దొరికింది_Breaks.popi();
            corrected_అంకె_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    తిరిగి corrected_అంకె_Breaks;
}

శూన్య CjkBreakEngine::initJapanesePhraseParameter(UError_సంకేతం& error) {
    ఎక్కించు_Japanese_Extensions(error);
    ఎక్కించు_Hiragana(error);
}

శూన్య CjkBreakEngine::ఎక్కించు_Japanese_Extensions(UError_సంకేతం& error) {
    స్థిరరాశి అక్షరం* ట్యాగ్ = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    ఒకవేళ (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(ట్యాగ్, error);
        అయితే (U_SUCCESS(error) && bundle.has_తదుపరి()) {
            f_Skip_సమితి.puti(bundle.get_తదుపరి_String(error), 1, error);
        }
    }
}

శూన్య CjkBreakEngine::ఎక్కించు_Hiragana(UError_సంకేతం& error) {
    Unicode_సమితి hiragana_మాట_సమితి(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_మాట_సమితి.compact();
    Unicode_సమితి_Iterator iterator(hiragana_మాట_సమితి);
    అయితే (iterator.next()) {
        f_Skip_సమితి.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#అయితే_ముగింపు

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */

