// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2016, International Business Machines Corporation
 * and others. All Rights Reserved.
 *******************************************************************************
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "brkeng.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dictbe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/chariter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/usetiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubrkimpl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normlzr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dictionarydata.h"

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

DictionaryBreakEngine::DictionaryBreakEngine() {
}

DictionaryBreakEngine::~DictionaryBreakEngine() {
}

UBool
DictionaryBreakEngine::handles(UChar32 c, ਅਚਲ ਅੱਖਰ*) ਅਚਲ {
    ਵਾਪਸ f_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c);
}

int32_t
DictionaryBreakEngine::ਲੱਭੋ_Breaks( UText *ਪਾਠ,
                                 int32_t ਸ਼ੁਰੂ_Pos,
                                 int32_t ਸਿਰਾ_Pos,
                                 UVector32 &ਮਿਲਿਆ_Breaks,
                                 UBool isPhraseBreaking,
                                 UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    int32_t ਨਤੀਜਾ = 0;

    // Find the span of characters included in the set.
    //   The span to break begins at the current position in the text, and
    //   extends towards the start or end of the text, depending on 'reverse'.

    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਸ਼ੁਰੂ_Pos);
    int32_t ਸ਼ੁਰੂ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
    int32_t current;
    int32_t range_ਸ਼ੁਰੂ;
    int32_t range_ਸਿਰਾ;
    UChar32 c = utext_current32(ਪਾਠ);
    ਜਦੋਂ ((current = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < ਸਿਰਾ_Pos && f_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c)) {
        utext_next32(ਪਾਠ);         // TODO:  recast loop for postincrement
        c = utext_current32(ਪਾਠ);
    }
    range_ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    range_ਸਿਰਾ = current;
    ਨਤੀਜਾ = divide_ਉੱਪਰ_Dictionary_Range(ਪਾਠ, range_ਸ਼ੁਰੂ, range_ਸਿਰਾ, ਮਿਲਿਆ_Breaks, isPhraseBreaking, ਸਥਿਤੀ);
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current);
    
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ
DictionaryBreakEngine::ਸੈੱਟ_Characters( ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ ) {
    f_ਸੈੱਟ = ਸੈੱਟ;
    // Compact for caching
    f_ਸੈੱਟ.compact();
}

/*
 ******************************************************************
 * PossibleWord
 */

// Helper class for improving readability of the Thai/Lao/Khmer word break
// algorithm. The implementation is completely inline.

// List size, limited by the maximum number of words in the dictionary
// that form a nested sequence.
ਸਥਿਰ ਅਚਲ int32_t POSSIBLE_ਸ਼ਬਦ_LIST_MAX = 20;

ਵਰਗ Possible_ਸ਼ਬਦ {
ਨਿਜੀ:
    // list of word candidate lengths, in increasing length order
    // TODO: bytes would be sufficient for word lengths.
    int32_t   count;      // Count of candidates
    int32_t   ਅਗੇਤਰ;     // The longest match with a dictionary word
    int32_t   ਵਿੱਥ;     // Offset in the text of these candidates
    int32_t   mark;       // The preferred candidate's offset
    int32_t   current;    // The candidate we're currently looking at
    int32_t   cuLengths[POSSIBLE_ਸ਼ਬਦ_LIST_MAX];   // Word Lengths, in code units.
    int32_t   cpLengths[POSSIBLE_ਸ਼ਬਦ_LIST_MAX];   // Word Lengths, in code points.

ਜਨਤਕ:
    Possible_ਸ਼ਬਦ() : count(0), ਅਗੇਤਰ(0), ਵਿੱਥ(-1), mark(0), current(0) {}
    ~Possible_ਸ਼ਬਦ() {}
  
    // Fill the list of candidates if needed, select the longest, and return the number found
    int32_t   candidates( UText *ਪਾਠ, DictionaryMatcher *dict, int32_t range_ਸਿਰਾ );
  
    // Select the currently marked candidate, point after it in the text, and invalidate self
    int32_t   acceptMarked( UText *ਪਾਠ );
  
    // Back up from the current candidate to the next shorter one; return true if that exists
    // and point the text after it
    UBool     ਪਿੱਠ_ਉੱਪਰ( UText *ਪਾਠ );
  
    // Return the longest prefix this candidate location shares with a dictionary word
    // Return value is in code points.
    int32_t   longest_ਅਗੇਤਰ() { ਵਾਪਸ ਅਗੇਤਰ; }
  
    // Mark the current candidate as the one we like
    ਖਾਲੀ      markCurrent() { mark = current; }
    
    // Get length in code points of the marked word.
    int32_t   markedCPLength() { ਵਾਪਸ cpLengths[mark]; }
};


int32_t Possible_ਸ਼ਬਦ::candidates( UText *ਪਾਠ, DictionaryMatcher *dict, int32_t range_ਸਿਰਾ ) {
    // TODO: If getIndex is too slow, use offset < 0 and add discardAll()
    int32_t ਸ਼ੁਰੂ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
    ਜੇ (ਸ਼ੁਰੂ != ਵਿੱਥ) {
        ਵਿੱਥ = ਸ਼ੁਰੂ;
        count = dict->matches(ਪਾਠ, range_ਸਿਰਾ-ਸ਼ੁਰੂ, UPRV_LENGTHOF(cuLengths), cuLengths, cpLengths, ਖਾਲੀ_ਸੂਚਕ, &ਅਗੇਤਰ);
        // Dictionary leaves text after longest prefix, not longest word. Back up.
        ਜੇ (count <= 0) {
            utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਸ਼ੁਰੂ);
        }
    }
    ਜੇ (count > 0) {
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਸ਼ੁਰੂ+cuLengths[count-1]);
    }
    current = count-1;
    mark = current;
    ਵਾਪਸ count;
}

int32_t
Possible_ਸ਼ਬਦ::acceptMarked( UText *ਪਾਠ ) {
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਵਿੱਥ + cuLengths[mark]);
    ਵਾਪਸ cuLengths[mark];
}


UBool
Possible_ਸ਼ਬਦ::ਪਿੱਠ_ਉੱਪਰ( UText *ਪਾਠ ) {
    ਜੇ (current > 0) {
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਵਿੱਥ + cuLengths[--current]);
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

/*
 ******************************************************************
 * ThaiBreakEngine
 */

// How many words in a row are "good enough"?
ਸਥਿਰ ਅਚਲ int32_t THAI_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ਸਥਿਰ ਅਚਲ int32_t THAI_ਜੜ੍ਹ_COMBINE_ਹੱਦ = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ਸਥਿਰ ਅਚਲ int32_t THAI_ਅਗੇਤਰ_COMBINE_ਹੱਦ = 3;

// Elision character
ਸਥਿਰ ਅਚਲ int32_t THAI_PAIYANNOI = 0x0E2F;

// Repeat character
ਸਥਿਰ ਅਚਲ int32_t THAI_MAIYAMOK = 0x0E46;

// Minimum word size
ਸਥਿਰ ਅਚਲ int32_t THAI_MIN_ਸ਼ਬਦ = 2;

// Minimum number of characters for two words
ਸਥਿਰ ਅਚਲ int32_t THAI_MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ = THAI_MIN_ਸ਼ਬਦ * 2;

ThaiBreakEngine::ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_ਕੋਡ &ਸਥਿਤੀ)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Thai");
    Unicode_ਸੈੱਟ thai_ਸ਼ਬਦ_ਸੈੱਟ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]"), ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਸੈੱਟ_Characters(thai_ਸ਼ਬਦ_ਸੈੱਟ);
    }
    f_Mark_ਸੈੱਟ.apply_ਨਮੂਨਾ(UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]&[:M:]]"), ਸਥਿਤੀ);
    f_Mark_ਸੈੱਟ.ਜੋੜੋ(0x0020);
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ = thai_ਸ਼ਬਦ_ਸੈੱਟ;
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.remove(0x0E31);             // MAI HAN-AKAT
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਜੋੜੋ(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਜੋੜੋ(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
    f_Suffix_ਸੈੱਟ.ਜੋੜੋ(THAI_PAIYANNOI);
    f_Suffix_ਸੈੱਟ.ਜੋੜੋ(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_ਸੈੱਟ.compact();
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    f_Suffix_ਸੈੱਟ.compact();
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

ThaiBreakEngine::~ThaiBreakEngine() {
    ਮਿਟਾਓ fDictionary;
}

int32_t
ThaiBreakEngine::divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                                int32_t range_ਸ਼ੁਰੂ,
                                                int32_t range_ਸਿਰਾ,
                                                UVector32 &ਮਿਲਿਆ_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, range_ਸ਼ੁਰੂ);
    utext_moveIndex32(ਪਾਠ, THAI_MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ);
    ਜੇ (utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ) >= range_ਸਿਰਾ) {
        ਵਾਪਸ 0;       // Not enough characters for two words
    }
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, range_ਸ਼ੁਰੂ);


    uint32_t ਸ਼ਬਦ_ਮਿਲਿਆ = 0;
    int32_t cp_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;    // Word Length in Code Points.
    int32_t cu_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;    // Word length in code units (UText native indexing)
    int32_t current;
    Possible_ਸ਼ਬਦ ਸ਼ਬਦ[THAI_LOOKAHEAD];
    
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, range_ਸ਼ੁਰੂ);
    
    ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ) && (current = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < range_ਸਿਰਾ) {
        cp_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
        cu_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;

        // Look for candidate words at the current position
        int32_t candidates = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%THAI_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ);
        
        // If we found exactly one, use that
        ਜੇ (candidates == 1) {
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % THAI_LOOKAHEAD].acceptMarked(ਪਾਠ);
            cp_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % THAI_LOOKAHEAD].markedCPLength();
            ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ਨਹੀਂਤਾਂ ਜੇ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) >= range_ਸਿਰਾ) {
                ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
            }
            ਕਰੋ {
                ਜੇ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % THAI_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%THAI_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) >= range_ਸਿਰਾ) {
                        ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    ਕਰੋ {
                        // If we find a third word, stop right away
                        ਜੇ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 2) % THAI_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ)) {
                            ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % THAI_LOOKAHEAD].markCurrent();
                            ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
                        }
                    }
                    ਜਦੋਂ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % THAI_LOOKAHEAD].ਪਿੱਠ_ਉੱਪਰ(ਪਾਠ));
                }
            }
            ਜਦੋਂ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % THAI_LOOKAHEAD].ਪਿੱਠ_ਉੱਪਰ(ਪਾਠ));
ਮਿਲਿਆ_ਉੱਤਮ:
            // Set UText position to after the accepted word.
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % THAI_LOOKAHEAD].acceptMarked(ਪਾਠ);
            cp_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % THAI_LOOKAHEAD].markedCPLength();
            ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        
        UChar32 uc = 0;
        ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) < range_ਸਿਰਾ && cp_ਸ਼ਬਦ_ਲੰਮਾਈ < THAI_ਜੜ੍ਹ_COMBINE_ਹੱਦ) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ਜੇ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % THAI_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) <= 0
                  && (cu_ਸ਼ਬਦ_ਲੰਮਾਈ == 0
                      || ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%THAI_LOOKAHEAD].longest_ਅਗੇਤਰ() < THAI_ਅਗੇਤਰ_COMBINE_ਹੱਦ)) {
                // Look for a plausible word boundary
                int32_t ਬਚਿਆ = range_ਸਿਰਾ - (current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
                UChar32 pc;
                int32_t chars = 0;
                ਲਈ (;;) {
                    int32_t pc_ਸੂਚਕਾਂਕ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
                    pc = utext_next32(ਪਾਠ);
                    int32_t pc_ਮਾਪ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - pc_ਸੂਚਕਾਂਕ;
                    chars += pc_ਮਾਪ;
                    ਬਚਿਆ -= pc_ਮਾਪ;
                    ਜੇ (ਬਚਿਆ <= 0) {
                        ਰੋਕੋ;
                    }
                    uc = utext_current32(ਪਾਠ);
                    ਜੇ (f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(pc) && f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // NOTE: In the original Apple code, checked that the next
                        // two characters after uc were not 0x0E4C THANTHAKHAT before
                        // checking the dictionary. That is just a performance filter,
                        // but it's not clear it's faster than checking the trie.
                        int32_t ਅੰਕ_candidates = ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % THAI_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ);
                        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current + cu_ਸ਼ਬਦ_ਲੰਮਾਈ + chars);
                        ਜੇ (ਅੰਕ_candidates > 0) {
                            ਰੋਕੋ;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ਜੇ (cu_ਸ਼ਬਦ_ਲੰਮਾਈ <= 0) {
                    ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
                }
                
                // Update the length with the passed-over characters
                cu_ਸ਼ਬਦ_ਲੰਮਾਈ += chars;
            }
            ਨਹੀਂਤਾਂ {
                // Back up to where we were for next iteration
                utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        ਜਦੋਂ ((currPos = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < range_ਸਿਰਾ && f_Mark_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(utext_current32(ਪਾਠ))) {
            utext_next32(ਪਾਠ);
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ += static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - currPos;
        }

        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) < range_ਸਿਰਾ && cu_ਸ਼ਬਦ_ਲੰਮਾਈ > 0) {
            ਜੇ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%THAI_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) <= 0
                && f_Suffix_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(uc = utext_current32(ਪਾਠ))) {
                ਜੇ (uc == THAI_PAIYANNOI) {
                    ਜੇ (!f_Suffix_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(utext_previous32(ਪਾਠ))) {
                        // Skip over previous end and PAIYANNOI
                        utext_next32(ਪਾਠ);
                        int32_t paiyannoi_ਸੂਚਕਾਂਕ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
                        utext_next32(ਪਾਠ);
                        cu_ਸ਼ਬਦ_ਲੰਮਾਈ += static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - paiyannoi_ਸੂਚਕਾਂਕ; // Add PAIYANNOI to word
                        uc = utext_current32(ਪਾਠ);     // Fetch next character
                    }
                    ਨਹੀਂਤਾਂ {
                        // Restore prior position
                        utext_next32(ਪਾਠ);
                    }
                }
                ਜੇ (uc == THAI_MAIYAMOK) {
                    ਜੇ (utext_previous32(ਪਾਠ) != THAI_MAIYAMOK) {
                        // Skip over previous end and MAIYAMOK
                        utext_next32(ਪਾਠ);
                        int32_t maiyamok_ਸੂਚਕਾਂਕ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
                        utext_next32(ਪਾਠ);
                        cu_ਸ਼ਬਦ_ਲੰਮਾਈ += static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - maiyamok_ਸੂਚਕਾਂਕ; // Add MAIYAMOK to word
                    }
                    ਨਹੀਂਤਾਂ {
                        // Restore prior position
                        utext_next32(ਪਾਠ);
                    }
                }
            }
            ਨਹੀਂਤਾਂ {
                utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
            }
        }

        // Did we find a word on this iteration? If so, push it on the break stack
        ਜੇ (cu_ਸ਼ਬਦ_ਲੰਮਾਈ > 0) {
            ਮਿਲਿਆ_Breaks.push((current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ), ਸਥਿਤੀ);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ਜੇ (ਮਿਲਿਆ_Breaks.peeki() >= range_ਸਿਰਾ) {
        (ਖਾਲੀ) ਮਿਲਿਆ_Breaks.popi();
        ਸ਼ਬਦ_ਮਿਲਿਆ -= 1;
    }

    ਵਾਪਸ ਸ਼ਬਦ_ਮਿਲਿਆ;
}

/*
 ******************************************************************
 * LaoBreakEngine
 */

// How many words in a row are "good enough"?
ਸਥਿਰ ਅਚਲ int32_t LAO_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ਸਥਿਰ ਅਚਲ int32_t LAO_ਜੜ੍ਹ_COMBINE_ਹੱਦ = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ਸਥਿਰ ਅਚਲ int32_t LAO_ਅਗੇਤਰ_COMBINE_ਹੱਦ = 3;

// Minimum word size
ਸਥਿਰ ਅਚਲ int32_t LAO_MIN_ਸ਼ਬਦ = 2;

// Minimum number of characters for two words
ਸਥਿਰ ਅਚਲ int32_t LAO_MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ = LAO_MIN_ਸ਼ਬਦ * 2;

LaoBreakEngine::LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_ਕੋਡ &ਸਥਿਤੀ)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Laoo");
    Unicode_ਸੈੱਟ lao_ਸ਼ਬਦ_ਸੈੱਟ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]]"), ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਸੈੱਟ_Characters(lao_ਸ਼ਬਦ_ਸੈੱਟ);
    }
    f_Mark_ਸੈੱਟ.apply_ਨਮੂਨਾ(UnicodeString(u"[[:Laoo:]&[:LineBreak=SA:]&[:M:]]"), ਸਥਿਤੀ);
    f_Mark_ਸੈੱਟ.ਜੋੜੋ(0x0020);
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ = lao_ਸ਼ਬਦ_ਸੈੱਟ;
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.remove(0x0EC0, 0x0EC4);     // prefix vowels
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਜੋੜੋ(0x0E81, 0x0EAE);      // basic consonants (including holes for corresponding Thai characters)
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਜੋੜੋ(0x0EDC, 0x0EDD);      // digraph consonants (no Thai equivalent)
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਜੋੜੋ(0x0EC0, 0x0EC4);      // prefix vowels

    // Compact for caching.
    f_Mark_ਸੈੱਟ.compact();
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

LaoBreakEngine::~LaoBreakEngine() {
    ਮਿਟਾਓ fDictionary;
}

int32_t
LaoBreakEngine::divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                                int32_t range_ਸ਼ੁਰੂ,
                                                int32_t range_ਸਿਰਾ,
                                                UVector32 &ਮਿਲਿਆ_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    ਜੇ ((range_ਸਿਰਾ - range_ਸ਼ੁਰੂ) < LAO_MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ) {
        ਵਾਪਸ 0;       // Not enough characters for two words
    }

    uint32_t ਸ਼ਬਦ_ਮਿਲਿਆ = 0;
    int32_t cp_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
    int32_t cu_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
    int32_t current;
    Possible_ਸ਼ਬਦ ਸ਼ਬਦ[LAO_LOOKAHEAD];

    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, range_ਸ਼ੁਰੂ);

    ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ) && (current = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < range_ਸਿਰਾ) {
        cu_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
        cp_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;

        // Look for candidate words at the current position
        int32_t candidates = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%LAO_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ);
        
        // If we found exactly one, use that
        ਜੇ (candidates == 1) {
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % LAO_LOOKAHEAD].acceptMarked(ਪਾਠ);
            cp_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % LAO_LOOKAHEAD].markedCPLength();
            ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ਨਹੀਂਤਾਂ ਜੇ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ਜੇ (utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ) >= range_ਸਿਰਾ) {
                ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
            }
            ਕਰੋ {
                ਜੇ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % LAO_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%LAO_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) >= range_ਸਿਰਾ) {
                        ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    ਕਰੋ {
                        // If we find a third word, stop right away
                        ਜੇ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 2) % LAO_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ)) {
                            ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % LAO_LOOKAHEAD].markCurrent();
                            ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
                        }
                    }
                    ਜਦੋਂ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % LAO_LOOKAHEAD].ਪਿੱਠ_ਉੱਪਰ(ਪਾਠ));
                }
            }
            ਜਦੋਂ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % LAO_LOOKAHEAD].ਪਿੱਠ_ਉੱਪਰ(ਪਾਠ));
ਮਿਲਿਆ_ਉੱਤਮ:
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % LAO_LOOKAHEAD].acceptMarked(ਪਾਠ);
            cp_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % LAO_LOOKAHEAD].markedCPLength();
            ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) < range_ਸਿਰਾ && cp_ਸ਼ਬਦ_ਲੰਮਾਈ < LAO_ਜੜ੍ਹ_COMBINE_ਹੱਦ) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ਜੇ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % LAO_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) <= 0
                  && (cu_ਸ਼ਬਦ_ਲੰਮਾਈ == 0
                      || ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%LAO_LOOKAHEAD].longest_ਅਗੇਤਰ() < LAO_ਅਗੇਤਰ_COMBINE_ਹੱਦ)) {
                // Look for a plausible word boundary
                int32_t ਬਚਿਆ = range_ਸਿਰਾ - (current + cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                ਲਈ (;;) {
                    int32_t pc_ਸੂਚਕਾਂਕ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
                    pc = utext_next32(ਪਾਠ);
                    int32_t pc_ਮਾਪ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - pc_ਸੂਚਕਾਂਕ;
                    chars += pc_ਮਾਪ;
                    ਬਚਿਆ -= pc_ਮਾਪ;
                    ਜੇ (ਬਚਿਆ <= 0) {
                        ਰੋਕੋ;
                    }
                    uc = utext_current32(ਪਾਠ);
                    ਜੇ (f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(pc) && f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t ਅੰਕ_candidates = ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % LAO_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ);
                        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current + cu_ਸ਼ਬਦ_ਲੰਮਾਈ + chars);
                        ਜੇ (ਅੰਕ_candidates > 0) {
                            ਰੋਕੋ;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ਜੇ (cu_ਸ਼ਬਦ_ਲੰਮਾਈ <= 0) {
                    ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
                }
                
                // Update the length with the passed-over characters
                cu_ਸ਼ਬਦ_ਲੰਮਾਈ += chars;
            }
            ਨਹੀਂਤਾਂ {
                // Back up to where we were for next iteration
                utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current + cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        ਜਦੋਂ ((currPos = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < range_ਸਿਰਾ && f_Mark_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(utext_current32(ਪਾਠ))) {
            utext_next32(ਪਾਠ);
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ += static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO LAO

        // Did we find a word on this iteration? If so, push it on the break stack
        ਜੇ (cu_ਸ਼ਬਦ_ਲੰਮਾਈ > 0) {
            ਮਿਲਿਆ_Breaks.push((current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ), ਸਥਿਤੀ);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ਜੇ (ਮਿਲਿਆ_Breaks.peeki() >= range_ਸਿਰਾ) {
        (ਖਾਲੀ) ਮਿਲਿਆ_Breaks.popi();
        ਸ਼ਬਦ_ਮਿਲਿਆ -= 1;
    }

    ਵਾਪਸ ਸ਼ਬਦ_ਮਿਲਿਆ;
}

/*
 ******************************************************************
 * BurmeseBreakEngine
 */

// How many words in a row are "good enough"?
ਸਥਿਰ ਅਚਲ int32_t BURMESE_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ਸਥਿਰ ਅਚਲ int32_t BURMESE_ਜੜ੍ਹ_COMBINE_ਹੱਦ = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ਸਥਿਰ ਅਚਲ int32_t BURMESE_ਅਗੇਤਰ_COMBINE_ਹੱਦ = 3;

// Minimum word size
ਸਥਿਰ ਅਚਲ int32_t BURMESE_MIN_ਸ਼ਬਦ = 2;

// Minimum number of characters for two words
ਸਥਿਰ ਅਚਲ int32_t BURMESE_MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ = BURMESE_MIN_ਸ਼ਬਦ * 2;

BurmeseBreakEngine::BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_ਕੋਡ &ਸਥਿਤੀ)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Mymr");
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਜੋੜੋ(0x1000, 0x102A);      // basic consonants and independent vowels
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.apply_ਨਮੂਨਾ(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]"), ਸਥਿਤੀ);
    f_Mark_ਸੈੱਟ.apply_ਨਮੂਨਾ(UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]&[:M:]]"), ਸਥਿਤੀ);
    f_Mark_ਸੈੱਟ.ਜੋੜੋ(0x0020);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਸੈੱਟ_Characters(f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ);
    }

    // Compact for caching.
    f_Mark_ਸੈੱਟ.compact();
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

BurmeseBreakEngine::~BurmeseBreakEngine() {
    ਮਿਟਾਓ fDictionary;
}

int32_t
BurmeseBreakEngine::divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                                int32_t range_ਸ਼ੁਰੂ,
                                                int32_t range_ਸਿਰਾ,
                                                UVector32 &ਮਿਲਿਆ_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ਕੋਡ& ਸਥਿਤੀ ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    ਜੇ ((range_ਸਿਰਾ - range_ਸ਼ੁਰੂ) < BURMESE_MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ) {
        ਵਾਪਸ 0;       // Not enough characters for two words
    }

    uint32_t ਸ਼ਬਦ_ਮਿਲਿਆ = 0;
    int32_t cp_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
    int32_t cu_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
    int32_t current;
    Possible_ਸ਼ਬਦ ਸ਼ਬਦ[BURMESE_LOOKAHEAD];

    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, range_ਸ਼ੁਰੂ);

    ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ) && (current = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < range_ਸਿਰਾ) {
        cu_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
        cp_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;

        // Look for candidate words at the current position
        int32_t candidates = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%BURMESE_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ);
        
        // If we found exactly one, use that
        ਜੇ (candidates == 1) {
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % BURMESE_LOOKAHEAD].acceptMarked(ਪਾਠ);
            cp_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % BURMESE_LOOKAHEAD].markedCPLength();
            ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
        }
        // If there was more than one, see which one can take us forward the most words
        ਨਹੀਂਤਾਂ ਜੇ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ਜੇ (utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ) >= range_ਸਿਰਾ) {
                ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
            }
            ਕਰੋ {
                ਜੇ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % BURMESE_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%BURMESE_LOOKAHEAD].markCurrent();
                    
                    // If we're already at the end of the range, we're done
                    ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) >= range_ਸਿਰਾ) {
                        ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
                    }
                    
                    // See if any of the possible second words is followed by a third word
                    ਕਰੋ {
                        // If we find a third word, stop right away
                        ਜੇ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 2) % BURMESE_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ)) {
                            ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % BURMESE_LOOKAHEAD].markCurrent();
                            ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
                        }
                    }
                    ਜਦੋਂ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % BURMESE_LOOKAHEAD].ਪਿੱਠ_ਉੱਪਰ(ਪਾਠ));
                }
            }
            ਜਦੋਂ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % BURMESE_LOOKAHEAD].ਪਿੱਠ_ਉੱਪਰ(ਪਾਠ));
ਮਿਲਿਆ_ਉੱਤਮ:
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % BURMESE_LOOKAHEAD].acceptMarked(ਪਾਠ);
            cp_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % BURMESE_LOOKAHEAD].markedCPLength();
            ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
        }
        
        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) < range_ਸਿਰਾ && cp_ਸ਼ਬਦ_ਲੰਮਾਈ < BURMESE_ਜੜ੍ਹ_COMBINE_ਹੱਦ) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ਜੇ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % BURMESE_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) <= 0
                  && (cu_ਸ਼ਬਦ_ਲੰਮਾਈ == 0
                      || ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%BURMESE_LOOKAHEAD].longest_ਅਗੇਤਰ() < BURMESE_ਅਗੇਤਰ_COMBINE_ਹੱਦ)) {
                // Look for a plausible word boundary
                int32_t ਬਚਿਆ = range_ਸਿਰਾ - (current + cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                ਲਈ (;;) {
                    int32_t pc_ਸੂਚਕਾਂਕ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
                    pc = utext_next32(ਪਾਠ);
                    int32_t pc_ਮਾਪ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - pc_ਸੂਚਕਾਂਕ;
                    chars += pc_ਮਾਪ;
                    ਬਚਿਆ -= pc_ਮਾਪ;
                    ਜੇ (ਬਚਿਆ <= 0) {
                        ਰੋਕੋ;
                    }
                    uc = utext_current32(ਪਾਠ);
                    ਜੇ (f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(pc) && f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(uc)) {
                        // Maybe. See if it's in the dictionary.
                        // TODO: this looks iffy; compare with old code.
                        int32_t ਅੰਕ_candidates = ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % BURMESE_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ);
                        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current + cu_ਸ਼ਬਦ_ਲੰਮਾਈ + chars);
                        ਜੇ (ਅੰਕ_candidates > 0) {
                            ਰੋਕੋ;
                        }
                    }
                }
                
                // Bump the word count if there wasn't already one
                ਜੇ (cu_ਸ਼ਬਦ_ਲੰਮਾਈ <= 0) {
                    ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
                }
                
                // Update the length with the passed-over characters
                cu_ਸ਼ਬਦ_ਲੰਮਾਈ += chars;
            }
            ਨਹੀਂਤਾਂ {
                // Back up to where we were for next iteration
                utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current + cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
            }
        }
        
        // Never stop before a combining mark.
        int32_t currPos;
        ਜਦੋਂ ((currPos = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < range_ਸਿਰਾ && f_Mark_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(utext_current32(ਪਾਠ))) {
            utext_next32(ਪਾਠ);
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ += static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - currPos;
        }
        
        // Look ahead for possible suffixes if a dictionary word does not follow.
        // We do this in code rather than using a rule so that the heuristic
        // resynch continues to function. For example, one of the suffix characters
        // could be a typo in the middle of a word.
        // NOT CURRENTLY APPLICABLE TO BURMESE

        // Did we find a word on this iteration? If so, push it on the break stack
        ਜੇ (cu_ਸ਼ਬਦ_ਲੰਮਾਈ > 0) {
            ਮਿਲਿਆ_Breaks.push((current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ), ਸਥਿਤੀ);
        }
    }

    // Don't return a break for the end of the dictionary range if there is one there.
    ਜੇ (ਮਿਲਿਆ_Breaks.peeki() >= range_ਸਿਰਾ) {
        (ਖਾਲੀ) ਮਿਲਿਆ_Breaks.popi();
        ਸ਼ਬਦ_ਮਿਲਿਆ -= 1;
    }

    ਵਾਪਸ ਸ਼ਬਦ_ਮਿਲਿਆ;
}

/*
 ******************************************************************
 * KhmerBreakEngine
 */

// How many words in a row are "good enough"?
ਸਥਿਰ ਅਚਲ int32_t KHMER_LOOKAHEAD = 3;

// Will not combine a non-word with a preceding dictionary word longer than this
ਸਥਿਰ ਅਚਲ int32_t KHMER_ਜੜ੍ਹ_COMBINE_ਹੱਦ = 3;

// Will not combine a non-word that shares at least this much prefix with a
// dictionary word, with a preceding word
ਸਥਿਰ ਅਚਲ int32_t KHMER_ਅਗੇਤਰ_COMBINE_ਹੱਦ = 3;

// Minimum word size
ਸਥਿਰ ਅਚਲ int32_t KHMER_MIN_ਸ਼ਬਦ = 2;

// Minimum number of characters for two words
ਸਥਿਰ ਅਚਲ int32_t KHMER_MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ = KHMER_MIN_ਸ਼ਬਦ * 2;

KhmerBreakEngine::KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_ਕੋਡ &ਸਥਿਤੀ)
    : DictionaryBreakEngine(),
      fDictionary(adoptDictionary)
{
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Khmr");
    Unicode_ਸੈੱਟ khmer_ਸ਼ਬਦ_ਸੈੱਟ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]]"), ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਸੈੱਟ_Characters(khmer_ਸ਼ਬਦ_ਸੈੱਟ);
    }
    f_Mark_ਸੈੱਟ.apply_ਨਮੂਨਾ(UnicodeString(u"[[:Khmr:]&[:LineBreak=SA:]&[:M:]]"), ਸਥਿਤੀ);
    f_Mark_ਸੈੱਟ.ਜੋੜੋ(0x0020);
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ = khmer_ਸ਼ਬਦ_ਸੈੱਟ;
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਜੋੜੋ(0x1780, 0x17B3);
    //fBeginWordSet.add(0x17A3, 0x17A4);      // deprecated vowels
    //fEndWordSet.remove(0x17A5, 0x17A9);     // Khmer independent vowels that can't end a word
    //fEndWordSet.remove(0x17B2);             // Khmer independent vowel that can't end a word
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.remove(0x17D2);             // KHMER SIGN COENG that combines some following characters
    //fEndWordSet.remove(0x17B6, 0x17C5);     // Remove dependent vowels
//    fEndWordSet.remove(0x0E31);             // MAI HAN-AKAT
//    fEndWordSet.remove(0x0E40, 0x0E44);     // SARA E through SARA AI MAIMALAI
//    fBeginWordSet.add(0x0E01, 0x0E2E);      // KO KAI through HO NOKHUK
//    fBeginWordSet.add(0x0E40, 0x0E44);      // SARA E through SARA AI MAIMALAI
//    fSuffixSet.add(THAI_PAIYANNOI);
//    fSuffixSet.add(THAI_MAIYAMOK);

    // Compact for caching.
    f_Mark_ਸੈੱਟ.compact();
    f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.compact();
//    fSuffixSet.compact();
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

KhmerBreakEngine::~KhmerBreakEngine() {
    ਮਿਟਾਓ fDictionary;
}

int32_t
KhmerBreakEngine::divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                                int32_t range_ਸ਼ੁਰੂ,
                                                int32_t range_ਸਿਰਾ,
                                                UVector32 &ਮਿਲਿਆ_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ਕੋਡ& ਸਥਿਤੀ ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    ਜੇ ((range_ਸਿਰਾ - range_ਸ਼ੁਰੂ) < KHMER_MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ) {
        ਵਾਪਸ 0;       // Not enough characters for two words
    }

    uint32_t ਸ਼ਬਦ_ਮਿਲਿਆ = 0;
    int32_t cp_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
    int32_t cu_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
    int32_t current;
    Possible_ਸ਼ਬਦ ਸ਼ਬਦ[KHMER_LOOKAHEAD];

    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, range_ਸ਼ੁਰੂ);

    ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ) && (current = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < range_ਸਿਰਾ) {
        cu_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;
        cp_ਸ਼ਬਦ_ਲੰਮਾਈ = 0;

        // Look for candidate words at the current position
        int32_t candidates = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ%KHMER_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ);

        // If we found exactly one, use that
        ਜੇ (candidates == 1) {
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].acceptMarked(ਪਾਠ);
            cp_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].markedCPLength();
            ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
        }

        // If there was more than one, see which one can take us forward the most words
        ਨਹੀਂਤਾਂ ਜੇ (candidates > 1) {
            // If we're already at the end of the range, we're done
            ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) >= range_ਸਿਰਾ) {
                ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
            }
            ਕਰੋ {
                ਜੇ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % KHMER_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) > 0) {
                    // Followed by another dictionary word; mark first word as a good candidate
                    ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].markCurrent();

                    // If we're already at the end of the range, we're done
                    ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) >= range_ਸਿਰਾ) {
                        ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
                    }

                    // See if any of the possible second words is followed by a third word
                    ਕਰੋ {
                        // If we find a third word, stop right away
                        ਜੇ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 2) % KHMER_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ)) {
                            ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].markCurrent();
                            ਜਾਓ ਮਿਲਿਆ_ਉੱਤਮ;
                        }
                    }
                    ਜਦੋਂ (ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % KHMER_LOOKAHEAD].ਪਿੱਠ_ਉੱਪਰ(ਪਾਠ));
                }
            }
            ਜਦੋਂ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].ਪਿੱਠ_ਉੱਪਰ(ਪਾਠ));
ਮਿਲਿਆ_ਉੱਤਮ:
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].acceptMarked(ਪਾਠ);
            cp_ਸ਼ਬਦ_ਲੰਮਾਈ = ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].markedCPLength();
            ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
        }

        // We come here after having either found a word or not. We look ahead to the
        // next word. If it's not a dictionary word, we will combine it with the word we
        // just found (if there is one), but only if the preceding word does not exceed
        // the threshold.
        // The text iterator should now be positioned at the end of the word we found.
        ਜੇ (static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) < range_ਸਿਰਾ && cp_ਸ਼ਬਦ_ਲੰਮਾਈ < KHMER_ਜੜ੍ਹ_COMBINE_ਹੱਦ) {
            // if it is a dictionary word, do nothing. If it isn't, then if there is
            // no preceding word, or the non-word shares less than the minimum threshold
            // of characters with a dictionary word, then scan to resynchronize
            ਜੇ (ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ) <= 0
                  && (cu_ਸ਼ਬਦ_ਲੰਮਾਈ == 0
                      || ਸ਼ਬਦ[ਸ਼ਬਦ_ਮਿਲਿਆ % KHMER_LOOKAHEAD].longest_ਅਗੇਤਰ() < KHMER_ਅਗੇਤਰ_COMBINE_ਹੱਦ)) {
                // Look for a plausible word boundary
                int32_t ਬਚਿਆ = range_ਸਿਰਾ - (current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
                UChar32 pc;
                UChar32 uc;
                int32_t chars = 0;
                ਲਈ (;;) {
                    int32_t pc_ਸੂਚਕਾਂਕ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
                    pc = utext_next32(ਪਾਠ);
                    int32_t pc_ਮਾਪ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - pc_ਸੂਚਕਾਂਕ;
                    chars += pc_ਮਾਪ;
                    ਬਚਿਆ -= pc_ਮਾਪ;
                    ਜੇ (ਬਚਿਆ <= 0) {
                        ਰੋਕੋ;
                    }
                    uc = utext_current32(ਪਾਠ);
                    ਜੇ (f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(pc) && f_Begin_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(uc)) {
                        // Maybe. See if it's in the dictionary.
                        int32_t ਅੰਕ_candidates = ਸ਼ਬਦ[(ਸ਼ਬਦ_ਮਿਲਿਆ + 1) % KHMER_LOOKAHEAD].candidates(ਪਾਠ, fDictionary, range_ਸਿਰਾ);
                        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ+chars);
                        ਜੇ (ਅੰਕ_candidates > 0) {
                            ਰੋਕੋ;
                        }
                    }
                }

                // Bump the word count if there wasn't already one
                ਜੇ (cu_ਸ਼ਬਦ_ਲੰਮਾਈ <= 0) {
                    ਸ਼ਬਦ_ਮਿਲਿਆ += 1;
                }

                // Update the length with the passed-over characters
                cu_ਸ਼ਬਦ_ਲੰਮਾਈ += chars;
            }
            ਨਹੀਂਤਾਂ {
                // Back up to where we were for next iteration
                utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ);
            }
        }

        // Never stop before a combining mark.
        int32_t currPos;
        ਜਦੋਂ ((currPos = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < range_ਸਿਰਾ && f_Mark_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(utext_current32(ਪਾਠ))) {
            utext_next32(ਪਾਠ);
            cu_ਸ਼ਬਦ_ਲੰਮਾਈ += static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - currPos;
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
        ਜੇ (cu_ਸ਼ਬਦ_ਲੰਮਾਈ > 0) {
            ਮਿਲਿਆ_Breaks.push((current+cu_ਸ਼ਬਦ_ਲੰਮਾਈ), ਸਥਿਤੀ);
        }
    }
    
    // Don't return a break for the end of the dictionary range if there is one there.
    ਜੇ (ਮਿਲਿਆ_Breaks.peeki() >= range_ਸਿਰਾ) {
        (ਖਾਲੀ) ਮਿਲਿਆ_Breaks.popi();
        ਸ਼ਬਦ_ਮਿਲਿਆ -= 1;
    }

    ਵਾਪਸ ਸ਼ਬਦ_ਮਿਲਿਆ;
}

#ਜੇ !UCONFIG_NO_NORMALIZATION
/*
 ******************************************************************
 * CjkBreakEngine
 */
ਸਥਿਰ ਅਚਲ uint32_t kuint32max = 0xFFFFFFFF;
CjkBreakEngine::CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_ਕੋਡ &ਸਥਿਤੀ)
: DictionaryBreakEngine(), fDictionary(adoptDictionary), isCj(ਝੂਠ) {
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_BREAK_ENGINE);
    UTRACE_DATA1(UTRACE_INFO, "dictbe=%s", "Hani");
    fMlBreakEngine = ਖਾਲੀ_ਸੂਚਕ;
    nfkcNorm2 = Normalizer2::getNFKCInstance(ਸਥਿਤੀ);
    // Korean dictionary only includes Hangul syllables
    f_Hangul_ਸ਼ਬਦ_ਸੈੱਟ.apply_ਨਮੂਨਾ(UnicodeString(u"[\\uac00-\\ud7a3]"), ਸਥਿਤੀ);
    f_Hangul_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    // Digits, open puncutation and Alphabetic characters.
    f_Digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ.apply_ਨਮੂਨਾ(
        UnicodeString(u"[[:Nd:][:Pi:][:Ps:][:Alphabetic:]]"), ਸਥਿਤੀ);
    f_Digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ.compact();
    f_Close_Punctuation_ਸੈੱਟ.apply_ਨਮੂਨਾ(UnicodeString(u"[[:Pc:][:Pd:][:Pe:][:Pf:][:Po:]]"), ਸਥਿਤੀ);
    f_Close_Punctuation_ਸੈੱਟ.compact();

    // handle Korean and Japanese/Chinese using different dictionaries
    ਜੇ (type == kKorean) {
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਸੈੱਟ_Characters(f_Hangul_ਸ਼ਬਦ_ਸੈੱਟ);
        }
    } ਨਹੀਂਤਾਂ { // Chinese and Japanese
        Unicode_ਸੈੱਟ cj_ਸੈੱਟ(UnicodeString(u"[[:Han:][:Hiragana:][:Katakana:]\\u30fc\\uff70\\uff9e\\uff9f]"), ਸਥਿਤੀ);
        isCj = ਸੱਚ;
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਸੈੱਟ_Characters(cj_ਸੈੱਟ);
#ਜੇ UCONFIG_USE_ML_PHRASE_BREAKING
            fMlBreakEngine = ਨਵਾਂ MlBreakEngine(f_Digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ,
                                               f_Close_Punctuation_ਸੈੱਟ, ਸਥਿਤੀ);
            ਜੇ (fMlBreakEngine == ਖਾਲੀ_ਸੂਚਕ) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            }
#ਨਹੀਂਤਾਂ
            initJapanesePhraseParameter(ਸਥਿਤੀ);
#ਜੇ_ਅੰਤ
        }
    }
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

CjkBreakEngine::~CjkBreakEngine(){
    ਮਿਟਾਓ fDictionary;
    ਮਿਟਾਓ fMlBreakEngine;
}

// The katakanaCost values below are based on the length frequencies of all
// katakana phrases in the dictionary
ਸਥਿਰ ਅਚਲ int32_t k_Max_Katakana_ਲੰਮਾਈ = 8;
ਸਥਿਰ ਅਚਲ int32_t k_Max_Katakana_ਸਮੂਹ_ਲੰਮਾਈ = 20;
ਸਥਿਰ ਅਚਲ uint32_t maxSnlp = 255;

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t get_Katakana_ਲਾਗਤ(int32_t ਸ਼ਬਦ_ਲੰਮਾਈ){
    //TODO: fill array with actual values from dictionary!
    ਸਥਿਰ ਅਚਲ uint32_t katakana_ਲਾਗਤ[k_Max_Katakana_ਲੰਮਾਈ + 1]
                                       = {8192, 984, 408, 240, 204, 252, 300, 372, 480};
    ਵਾਪਸ (ਸ਼ਬਦ_ਲੰਮਾਈ > k_Max_Katakana_ਲੰਮਾਈ) ? 8192 : katakana_ਲਾਗਤ[ਸ਼ਬਦ_ਲੰਮਾਈ];
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ isKatakana(UChar32 value) {
    ਵਾਪਸ (value >= 0x30A1 && value <= 0x30FE && value != 0x30FB) ||
            (value >= 0xFF66 && value <= 0xFF9f);
}

// Function for accessing internal utext flags.
//   Replicates an internal UText function.

ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t utext_i32_flag(int32_t ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ) {
    ਵਾਪਸ static_cast<int32_t>(1) << ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ;
}
       
/*
 * @param text A UText representing the text
 * @param rangeStart The start of the range of dictionary characters
 * @param rangeEnd The end of the range of dictionary characters
 * @param foundBreaks vector<int32> to receive the break positions
 * @return The number of breaks found
 */
int32_t 
CjkBreakEngine::divide_ਉੱਪਰ_Dictionary_Range( UText *ਵਿੱਚ_ਪਾਠ,
        int32_t range_ਸ਼ੁਰੂ,
        int32_t range_ਸਿਰਾ,
        UVector32 &ਮਿਲਿਆ_Breaks,
        UBool isPhraseBreaking,
        UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    ਜੇ (range_ਸ਼ੁਰੂ >= range_ਸਿਰਾ) {
        ਵਾਪਸ 0;
    }

    // UnicodeString version of input UText, NFKC normalized if necessary.
    UnicodeString ਵਿੱਚ_String;

    // inputMap[inStringIndex] = corresponding native index from UText inText.
    // If nullptr then mapping is 1:1
    Local_ਸੂਚਕ<UVector32>     ਨਿਵੇਸ਼_Map;

    // if UText has the input string as one contiguous UTF-16 chunk
    ਜੇ ((ਵਿੱਚ_ਪਾਠ->providerProperties & utext_i32_flag(UTEXT_PROVIDER_STABLE_CHUNKS)) &&
         ਵਿੱਚ_ਪਾਠ->chunk_Native_ਸ਼ੁਰੂ <= range_ਸ਼ੁਰੂ &&
         ਵਿੱਚ_ਪਾਠ->chunk_Native_ਹੱਦਬੰਦੀ >= range_ਸਿਰਾ   &&
         ਵਿੱਚ_ਪਾਠ->native_Indexing_ਹੱਦਬੰਦੀ >= range_ਸਿਰਾ - ਵਿੱਚ_ਪਾਠ->chunk_Native_ਸ਼ੁਰੂ) {

        // Input UText is in one contiguous UTF-16 chunk.
        // Use Read-only aliasing UnicodeString.
        ਵਿੱਚ_String.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਝੂਠ,
                       ਵਿੱਚ_ਪਾਠ->chunkContents + range_ਸ਼ੁਰੂ - ਵਿੱਚ_ਪਾਠ->chunk_Native_ਸ਼ੁਰੂ,
                       range_ਸਿਰਾ - range_ਸ਼ੁਰੂ);
    } ਨਹੀਂਤਾਂ {
        // Copy the text from the original inText (UText) to inString (UnicodeString).
        // Create a map from UnicodeString indices -> UText offsets.
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਵਿੱਚ_ਪਾਠ, range_ਸ਼ੁਰੂ);
        int32_t ਹੱਦਬੰਦੀ = range_ਸਿਰਾ;
        U_ASSERT(ਹੱਦਬੰਦੀ <= utext_native_ਲੰਮਾਈ(ਵਿੱਚ_ਪਾਠ));
        ਜੇ (ਹੱਦਬੰਦੀ > utext_native_ਲੰਮਾਈ(ਵਿੱਚ_ਪਾਠ)) {
            ਹੱਦਬੰਦੀ = static_cast<int32_t>(utext_native_ਲੰਮਾਈ(ਵਿੱਚ_ਪਾਠ));
        }
        ਨਿਵੇਸ਼_Map.adopt_Instead_And_Check_Error_ਕੋਡ(ਨਵਾਂ UVector32(ਸਥਿਤੀ), ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ 0;
        }
        ਜਦੋਂ (utext_get_Native_ਸੂਚਕਾਂਕ(ਵਿੱਚ_ਪਾਠ) < ਹੱਦਬੰਦੀ) {
            int32_t nativePosition = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਵਿੱਚ_ਪਾਠ));
            UChar32 c = utext_next32(ਵਿੱਚ_ਪਾਠ);
            U_ASSERT(c != U_SENTINEL);
            ਵਿੱਚ_String.append(c);
            ਜਦੋਂ (ਨਿਵੇਸ਼_Map->size() < ਵਿੱਚ_String.length()) {
                ਨਿਵੇਸ਼_Map->ਜੋੜੋ_Element(nativePosition, ਸਥਿਤੀ);
            }
        }
        ਨਿਵੇਸ਼_Map->ਜੋੜੋ_Element(ਹੱਦਬੰਦੀ, ਸਥਿਤੀ);
    }


    ਜੇ (!nfkcNorm2->isNormalized(ਵਿੱਚ_String, ਸਥਿਤੀ)) {
        UnicodeString normalized_ਨਿਵੇਸ਼;
        //  normalizedMap[normalizedInput position] ==  original UText position.
        Local_ਸੂਚਕ<UVector32> normalizedMap(ਨਵਾਂ UVector32(ਸਥਿਤੀ), ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ 0;
        }
        
        UnicodeString fragment;
        UnicodeString normalizedFragment;
        ਲਈ (int32_t srcI = 0; srcI < ਵਿੱਚ_String.length();) {  // Once per normalization chunk
            fragment.remove();
            int32_t fragment_ਸ਼ੁਰੂ_I = srcI;
            UChar32 c = ਵਿੱਚ_String.char32At(srcI);
            ਲਈ (;;) {
                fragment.append(c);
                srcI = ਵਿੱਚ_String.moveIndex32(srcI, 1);
                ਜੇ (srcI == ਵਿੱਚ_String.length()) {
                    ਰੋਕੋ;
                }
                c = ਵਿੱਚ_String.char32At(srcI);
                ਜੇ (nfkcNorm2->hasBoundaryBefore(c)) {
                    ਰੋਕੋ;
                }
            }
            nfkcNorm2->normalize(fragment, normalizedFragment, ਸਥਿਤੀ);
            normalized_ਨਿਵੇਸ਼.append(normalizedFragment);

            // Map every position in the normalized chunk to the start of the chunk
            //   in the original input.
            int32_t fragment_Original_ਸ਼ੁਰੂ = ਨਿਵੇਸ਼_Map.isValid() ?
                    ਨਿਵੇਸ਼_Map->elementAti(fragment_ਸ਼ੁਰੂ_I) : fragment_ਸ਼ੁਰੂ_I+range_ਸ਼ੁਰੂ;
            ਜਦੋਂ (normalizedMap->size() < normalized_ਨਿਵੇਸ਼.length()) {
                normalizedMap->ਜੋੜੋ_Element(fragment_Original_ਸ਼ੁਰੂ, ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                    ਰੋਕੋ;
                }
            }
        }
        U_ASSERT(normalizedMap->size() == normalized_ਨਿਵੇਸ਼.length());
        int32_t native_ਸਿਰਾ = ਨਿਵੇਸ਼_Map.isValid() ?
                ਨਿਵੇਸ਼_Map->elementAti(ਵਿੱਚ_String.length()) : ਵਿੱਚ_String.length()+range_ਸ਼ੁਰੂ;
        normalizedMap->ਜੋੜੋ_Element(native_ਸਿਰਾ, ਸਥਿਤੀ);

        ਨਿਵੇਸ਼_Map = std::move(normalizedMap);
        ਵਿੱਚ_String = std::move(normalized_ਨਿਵੇਸ਼);
    }

    int32_t ਅੰਕ_ਕੋਡ_Pts = ਵਿੱਚ_String.ਗਿਣਤੀ_Char32();
    ਜੇ (ਅੰਕ_ਕੋਡ_Pts != ਵਿੱਚ_String.length()) {
        // There are supplementary characters in the input.
        // The dictionary will produce boundary positions in terms of code point indexes,
        //   not in terms of code unit string indexes.
        // Use the inputMap mechanism to take care of this in addition to indexing differences
        //    from normalization and/or UTF-8 input.
        UBool hadExistingMap = ਨਿਵੇਸ਼_Map.isValid();
        ਜੇ (!hadExistingMap) {
            ਨਿਵੇਸ਼_Map.adopt_Instead_And_Check_Error_ਕੋਡ(ਨਵਾਂ UVector32(ਸਥਿਤੀ), ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਵਾਪਸ 0;
            }
        }
        int32_t cpIdx = 0;
        ਲਈ (int32_t cuIdx = 0; ; cuIdx = ਵਿੱਚ_String.moveIndex32(cuIdx, 1)) {
            U_ASSERT(cuIdx >= cpIdx);
            ਜੇ (hadExistingMap) {
                ਨਿਵੇਸ਼_Map->ਸੈੱਟ_Element_At(ਨਿਵੇਸ਼_Map->elementAti(cuIdx), cpIdx);
            } ਨਹੀਂਤਾਂ {
                ਨਿਵੇਸ਼_Map->ਜੋੜੋ_Element(cuIdx+range_ਸ਼ੁਰੂ, ਸਥਿਤੀ);
            }
            cpIdx++;
            ਜੇ (cuIdx == ਵਿੱਚ_String.length()) {
               ਰੋਕੋ;
            }
        }
    }

#ਜੇ UCONFIG_USE_ML_PHRASE_BREAKING
    // PhraseBreaking is supported in ja and ko; MlBreakEngine only supports ja.
    ਜੇ (isPhraseBreaking && isCj) {
        ਵਾਪਸ fMlBreakEngine->divide_ਉੱਪਰ_Range(ਵਿੱਚ_ਪਾਠ, range_ਸ਼ੁਰੂ, range_ਸਿਰਾ, ਮਿਲਿਆ_Breaks, ਵਿੱਚ_String,
                                             ਨਿਵੇਸ਼_Map, ਸਥਿਤੀ);
    }
#ਜੇ_ਅੰਤ

    // bestSnlp[i] is the snlp of the best segmentation of the first i
    // code points in the range to be matched.
    UVector32 ਉੱਤਮ_Snlp(ਅੰਕ_ਕੋਡ_Pts + 1, ਸਥਿਤੀ);
    ਉੱਤਮ_Snlp.ਜੋੜੋ_Element(0, ਸਥਿਤੀ);
    ਲਈ(int32_t i = 1; i <= ਅੰਕ_ਕੋਡ_Pts; i++) {
        ਉੱਤਮ_Snlp.ਜੋੜੋ_Element(kuint32max, ਸਥਿਤੀ);
    }


    // prev[i] is the index of the last CJK code point in the previous word in 
    // the best segmentation of the first i characters.
    UVector32 prev(ਅੰਕ_ਕੋਡ_Pts + 1, ਸਥਿਤੀ);
    ਲਈ(int32_t i = 0; i <= ਅੰਕ_ਕੋਡ_Pts; i++){
        prev.ਜੋੜੋ_Element(-1, ਸਥਿਤੀ);
    }

    ਅਚਲ int32_t max_ਸ਼ਬਦ_ਮਾਪ = 20;
    UVector32 ਮੁੱਲਾਂ(ਅੰਕ_ਕੋਡ_Pts, ਸਥਿਤੀ);
    ਮੁੱਲਾਂ.ਸੈੱਟ_ਮਾਪ(ਅੰਕ_ਕੋਡ_Pts);
    UVector32 lengths(ਅੰਕ_ਕੋਡ_Pts, ਸਥਿਤੀ);
    lengths.ਸੈੱਟ_ਮਾਪ(ਅੰਕ_ਕੋਡ_Pts);

    UText fu = UTEXT_INITIALIZER;
    utext_openUnicodeString(&fu, &ਵਿੱਚ_String, &ਸਥਿਤੀ);

    // Dynamic programming to find the best segmentation.

    // In outer loop, i  is the code point index,
    //                ix is the corresponding string (code unit) index.
    //    They differ when the string contains supplementary characters.
    int32_t ix = 0;
    ਤਾਰਕਿਕ is_prev_katakana = ਝੂਠ;
    ਲਈ (int32_t i = 0;  i < ਅੰਕ_ਕੋਡ_Pts;  ++i, ix = ਵਿੱਚ_String.moveIndex32(ix, 1)) {
        ਜੇ (static_cast<uint32_t>(ਉੱਤਮ_Snlp.elementAti(i)) == kuint32max) {
            ਜਾਰੀ;
        }

        int32_t count;
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(&fu, ix);
        count = fDictionary->matches(&fu, max_ਸ਼ਬਦ_ਮਾਪ, ਅੰਕ_ਕੋਡ_Pts,
                             ਖਾਲੀ_ਸੂਚਕ, lengths.get_ਬਫਰ(), ਮੁੱਲਾਂ.get_ਬਫਰ(), ਖਾਲੀ_ਸੂਚਕ);
                             // Note: lengths is filled with code point lengths
                             //       The nullptr parameter is the ignored code unit lengths.

        // if there are no single character matches found in the dictionary 
        // starting with this character, treat character as a 1-character word 
        // with the highest value possible, i.e. the least likely to occur.
        // Exclude Korean characters from this treatment, as they should be left
        // together by default.
        ਜੇ ((count == 0 || lengths.elementAti(0) != 1) &&
                !f_Hangul_ਸ਼ਬਦ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(ਵਿੱਚ_String.char32At(ix))) {
            ਮੁੱਲਾਂ.ਸੈੱਟ_Element_At(maxSnlp, count);   // 255
            lengths.ਸੈੱਟ_Element_At(1, count++);
        }

        ਲਈ (int32_t j = 0; j < count; j++) {
            uint32_t newSnlp = static_cast<uint32_t>(ਉੱਤਮ_Snlp.elementAti(i)) + static_cast<uint32_t>(ਮੁੱਲਾਂ.elementAti(j));
            int32_t ln_j_i = lengths.elementAti(j) + i;
            ਜੇ (newSnlp < static_cast<uint32_t>(ਉੱਤਮ_Snlp.elementAti(ln_j_i))) {
                ਉੱਤਮ_Snlp.ਸੈੱਟ_Element_At(newSnlp, ln_j_i);
                prev.ਸੈੱਟ_Element_At(i, ln_j_i);
            }
        }

        // In Japanese,
        // Katakana word in single character is pretty rare. So we apply
        // the following heuristic to Katakana: any continuous run of Katakana
        // characters is considered a candidate word with a default cost
        // specified in the katakanaCost table according to its length.

        ਤਾਰਕਿਕ is_katakana = isKatakana(ਵਿੱਚ_String.char32At(ix));
        int32_t katakana_ਲੜੀਵਾਰ_ਲੰਮਾਈ = 1;
        ਜੇ (!is_prev_katakana && is_katakana) {
            int32_t j = ਵਿੱਚ_String.moveIndex32(ix, 1);
            // Find the end of the continuous run of Katakana characters
            ਜਦੋਂ (j < ਵਿੱਚ_String.length() && katakana_ਲੜੀਵਾਰ_ਲੰਮਾਈ < k_Max_Katakana_ਸਮੂਹ_ਲੰਮਾਈ &&
                    isKatakana(ਵਿੱਚ_String.char32At(j))) {
                j = ਵਿੱਚ_String.moveIndex32(j, 1);
                katakana_ਲੜੀਵਾਰ_ਲੰਮਾਈ++;
            }
            ਜੇ (katakana_ਲੜੀਵਾਰ_ਲੰਮਾਈ < k_Max_Katakana_ਸਮੂਹ_ਲੰਮਾਈ) {
                uint32_t newSnlp = ਉੱਤਮ_Snlp.elementAti(i) + get_Katakana_ਲਾਗਤ(katakana_ਲੜੀਵਾਰ_ਲੰਮਾਈ);
                ਜੇ (newSnlp < static_cast<uint32_t>(ਉੱਤਮ_Snlp.elementAti(i + katakana_ਲੜੀਵਾਰ_ਲੰਮਾਈ))) {
                    ਉੱਤਮ_Snlp.ਸੈੱਟ_Element_At(newSnlp, i+katakana_ਲੜੀਵਾਰ_ਲੰਮਾਈ);
                    prev.ਸੈੱਟ_Element_At(i, i+katakana_ਲੜੀਵਾਰ_ਲੰਮਾਈ);  // prev[j] = i;
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
    UVector32 t_boundary(ਅੰਕ_ਕੋਡ_Pts+1, ਸਥਿਤੀ);

    int32_t ਅੰਕ_Breaks = 0;
    // No segmentation found, set boundary to end of range
    ਜੇ (static_cast<uint32_t>(ਉੱਤਮ_Snlp.elementAti(ਅੰਕ_ਕੋਡ_Pts)) == kuint32max) {
        t_boundary.ਜੋੜੋ_Element(ਅੰਕ_ਕੋਡ_Pts, ਸਥਿਤੀ);
        ਅੰਕ_Breaks++;
    } ਨਹੀਂਤਾਂ ਜੇ (isPhraseBreaking) {
        t_boundary.ਜੋੜੋ_Element(ਅੰਕ_ਕੋਡ_Pts, ਸਥਿਤੀ);
        ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
            ਅੰਕ_Breaks++;
            int32_t prevIdx = ਅੰਕ_ਕੋਡ_Pts;

            int32_t ਕੋਡ_Unit_Idx = -1;
            int32_t prev_ਕੋਡ_Unit_Idx = -1;
            int32_t length = -1;
            ਲਈ (int32_t i = prev.elementAti(ਅੰਕ_ਕੋਡ_Pts); i > 0; i = prev.elementAti(i)) {
                ਕੋਡ_Unit_Idx = ਵਿੱਚ_String.moveIndex32(0, i);
                prev_ਕੋਡ_Unit_Idx = ਵਿੱਚ_String.moveIndex32(0, prevIdx);
                // Calculate the length by using the code unit.
                length = prev_ਕੋਡ_Unit_Idx - ਕੋਡ_Unit_Idx;
                prevIdx = i;
                // Keep the breakpoint if the pattern is not in the fSkipSet and continuous Katakana
                // characters don't occur.
                ਜੇ (!f_Skip_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ_ਕੁੰਜੀ(ਵਿੱਚ_String.ਆਰਜ਼ੀ_Sub_String(ਕੋਡ_Unit_Idx, length))
                    && (!isKatakana(ਵਿੱਚ_String.char32At(ਵਿੱਚ_String.moveIndex32(ਕੋਡ_Unit_Idx, -1)))
                           || !isKatakana(ਵਿੱਚ_String.char32At(ਕੋਡ_Unit_Idx)))) {
                    t_boundary.ਜੋੜੋ_Element(i, ਸਥਿਤੀ);
                    ਅੰਕ_Breaks++;
                }
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਲਈ (int32_t i = ਅੰਕ_ਕੋਡ_Pts; i > 0; i = prev.elementAti(i)) {
            t_boundary.ਜੋੜੋ_Element(i, ਸਥਿਤੀ);
            ਅੰਕ_Breaks++;
        }
        U_ASSERT(prev.elementAti(t_boundary.elementAti(ਅੰਕ_Breaks - 1)) == 0);
    }

    // Add a break for the start of the dictionary range if there is not one
    // there already.
    ਜੇ (ਮਿਲਿਆ_Breaks.size() == 0 || ਮਿਲਿਆ_Breaks.peeki() < range_ਸ਼ੁਰੂ) {
        t_boundary.ਜੋੜੋ_Element(0, ਸਥਿਤੀ);
        ਅੰਕ_Breaks++;
    }

    // Now that we're done, convert positions in t_boundary[] (indices in 
    // the normalized input string) back to indices in the original input UText
    // while reversing t_boundary and pushing values to foundBreaks.
    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_ਅੰਕ_Breaks = 0;
    ਲਈ (int32_t i = ਅੰਕ_Breaks - 1; i >= 0; i--) {
        int32_t cpPos = t_boundary.elementAti(i);
        U_ASSERT(cpPos > prevCPPos);
        int32_t utextPos =  ਨਿਵੇਸ਼_Map.isValid() ? ਨਿਵੇਸ਼_Map->elementAti(cpPos) : cpPos + range_ਸ਼ੁਰੂ;
        U_ASSERT(utextPos >= prevUTextPos);
        ਜੇ (utextPos > prevUTextPos) {
            // Boundaries are added to foundBreaks output in ascending order.
            U_ASSERT(ਮਿਲਿਆ_Breaks.size() == 0 || ਮਿਲਿਆ_Breaks.peeki() < utextPos);
            // In phrase breaking, there has to be a breakpoint between Cj character and close
            // punctuation.
            // E.g.［携帯電話］正しい選択 -> ［携帯▁電話］▁正しい▁選択 -> breakpoint between ］ and 正
            ਜੇ (utextPos != range_ਸ਼ੁਰੂ
                || (isPhraseBreaking && utextPos > 0
                       && f_Close_Punctuation_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(utext_char32At(ਵਿੱਚ_ਪਾਠ, utextPos - 1)))) {
                ਮਿਲਿਆ_Breaks.push(utextPos, ਸਥਿਤੀ);
                corrected_ਅੰਕ_Breaks++;
            }
        } ਨਹੀਂਤਾਂ {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --ਅੰਕ_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (ਖਾਲੀ)prevCPPos; // suppress compiler warnings about unused variable

    UChar32 ਅਗਲਾ_Char = utext_char32At(ਵਿੱਚ_ਪਾਠ, range_ਸਿਰਾ);
    ਜੇ (!ਮਿਲਿਆ_Breaks.is_ਖਾਲੀ_ਥਾਵਾਂ() && ਮਿਲਿਆ_Breaks.peeki() == range_ਸਿਰਾ) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        ਜੇ (isPhraseBreaking) {
            ਜੇ (!f_Digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(ਅਗਲਾ_Char)) {
                ਮਿਲਿਆ_Breaks.popi();
                corrected_ਅੰਕ_Breaks--;
            }
        } ਨਹੀਂਤਾਂ {
            ਮਿਲਿਆ_Breaks.popi();
            corrected_ਅੰਕ_Breaks--;
        }
    }

    // inString goes out of scope
    // inputMap goes out of scope
    ਵਾਪਸ corrected_ਅੰਕ_Breaks;
}

ਖਾਲੀ CjkBreakEngine::initJapanesePhraseParameter(UError_ਕੋਡ& error) {
    ਲੱਦੋ_Japanese_Extensions(error);
    ਲੱਦੋ_Hiragana(error);
}

ਖਾਲੀ CjkBreakEngine::ਲੱਦੋ_Japanese_Extensions(UError_ਕੋਡ& error) {
    ਅਚਲ ਅੱਖਰ* ਟੈਗ = "extensions";
    ResourceBundle ja(U_ICUDATA_BRKITR, "ja", error);
    ਜੇ (U_SUCCESS(error)) {
        ResourceBundle bundle = ja.get(ਟੈਗ, error);
        ਜਦੋਂ (U_SUCCESS(error) && bundle.has_ਅਗਲਾ()) {
            f_Skip_ਸੈੱਟ.puti(bundle.get_ਅਗਲਾ_String(error), 1, error);
        }
    }
}

ਖਾਲੀ CjkBreakEngine::ਲੱਦੋ_Hiragana(UError_ਕੋਡ& error) {
    Unicode_ਸੈੱਟ hiragana_ਸ਼ਬਦ_ਸੈੱਟ(UnicodeString(u"[:Hiragana:]"), error);
    hiragana_ਸ਼ਬਦ_ਸੈੱਟ.compact();
    Unicode_ਸੈੱਟ_Iterator iterator(hiragana_ਸ਼ਬਦ_ਸੈੱਟ);
    ਜਦੋਂ (iterator.next()) {
        f_Skip_ਸੈੱਟ.puti(UnicodeString(iterator.getCodepoint()), 1, error);
    }
}
#ਜੇ_ਅੰਤ

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */

