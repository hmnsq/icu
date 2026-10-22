// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.h
//
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RBBI_CACHE_H
#ವ್ಯಾಖ್ಯಾನಿಸು RBBI_CACHE_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

/* DictionaryCache  stores the boundaries obtained from a run of dictionary characters.
 *                 Dictionary boundaries are moved first to this cache, then from here
 *                 to the main BreakCache, where they may inter-leave with non-dictionary
 *                 boundaries. The public BreakIterator API always fetches directly
 *                 from the main BreakCache, not from here.
 *
 *                 In common situations, the number of boundaries in a single dictionary run
 *                 should be quite small, it will be terminated by punctuation, spaces,
 *                 or any other non-dictionary characters. The main BreakCache may end
 *                 up with boundaries from multiple dictionary based runs.
 *
 *                 The boundaries are stored in a simple ArrayList (vector), with the
 *                 assumption that they will be accessed sequentially.
 */                 
class ನಿಯಮ_Based_Break_Iterator::DictionaryCache: ಸಾರ್ವಜನಿಕ UMemory {
  ಸಾರ್ವಜನಿಕ:
     DictionaryCache(ನಿಯಮ_Based_Break_Iterator *bi, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
     ~DictionaryCache();

     ಶೂನ್ಯ reset();

     UBool following(int32_t fromPos, int32_t *pos, int32_t *ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ);
     UBool preceding(int32_t fromPos, int32_t *pos, int32_t *ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ);

    /**
     * Populate the cache with the dictionary based boundaries within a region of text.
     * @param startPos  The start position of a range of text
     * @param endPos    The end position of a range of text
     * @param firstRuleStatus The rule status index that applies to the break at startPos
     * @param otherRuleStatus The rule status index that applies to boundaries other than startPos
     * @internal
     */
    ಶೂನ್ಯ populateDictionary(int32_t ಆರಂಭ_Pos, int32_t ಕೊನೆ_Pos,
                         int32_t ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ, int32_t other_ನಿಯಮ_ಸ್ಥಿತಿ);



    ನಿಯಮ_Based_Break_Iterator *fBI;
    
    UVector32           fBreaks;                // A vector containing the boundaries.
    int32_t             f_Position_ಒಳ_Cache;       // Index in fBreaks of last boundary returned by following()
                                                //    or preceding(). Optimizes sequential access.
    int32_t             f_ಆರಂಭ;                 // Text position of first boundary in cache.
    int32_t             f_ಎಲ್ಲೆ;                 // Last boundary in cache. Which is the limit of the
                                                //    text segment being handled by the dictionary.
    int32_t             f_ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ;  // Rule status info for first boundary.
    int32_t             f_Other_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ;  // Rule status info for 2nd through last boundaries.

    // If `this->isWordLike`, the status of dictionary breaks is equal to the status of the final
    // break of the rule-based segment they refine (fOtherRuleStatusIndex); otherwise, dictionary
    // breaks have status 0.
    // For compatibility, this property is determined by the largest status value used by `*fBI`:
    // rules that have a largest status greater than 100 are considered word-like.
    bool is_ಶಬ್ದ_Like;
};


/*
 * class BreakCache
 *
 * Cache of break boundary positions and rule status values.
 * Break iterator API functions, next(), previous(), etc., will use cached results
 * when possible, and otherwise cache new results as they are obtained.
 *
 * Uniformly caches both dictionary and rule based (non-dictionary) boundaries.
 *
 * The cache is implemented as a single circular buffer.
 */

/*
 * size of the circular cache buffer.
 */

class ನಿಯಮ_Based_Break_Iterator::BreakCache: ಸಾರ್ವಜನಿಕ UMemory {
  ಸಾರ್ವಜನಿಕ:
                BreakCache(ನಿಯಮ_Based_Break_Iterator *bi, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    ವಾಸ್ತವಿಕ     ~BreakCache();
    ಶೂನ್ಯ        reset(int32_t pos = 0, int32_t ನಿಯಮ_ಸ್ಥಿತಿ = 0);
    ಶೂನ್ಯ        ಮುಂದಿನ() {    ಆದರೆ (fBufIdx == f_ಕೊನೆ_Buf_Idx) {
                                ಮುಂದಿನ_OL();
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                fBufIdx = mod_Chunk_ಗಾತ್ರ(fBufIdx + 1);
                                f_ಪಠ್ಯ_Idx = fBI->fPosition = fBoundaries[fBufIdx];
                                fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = fStatuses[fBufIdx];
                            }
                }


    ಶೂನ್ಯ        ಮುಂದಿನ_OL();
    ಶೂನ್ಯ        ಹಿಂದಿನ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    // Move the iteration state to the position following the startPosition.
    // Input position must be pinned to the input length.
    ಶೂನ್ಯ        following(int32_t ಆರಂಭ_Position, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ಶೂನ್ಯ        preceding(int32_t ಆರಂಭ_Position, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    /*
     * Update the state of the public BreakIterator (fBI) to reflect the
     * current state of the break iterator cache (this).
     */
    int32_t     current();

    /**
     * Add boundaries to the cache near the specified position.
     * The given position need not be a boundary itself.
     * The input position must be within the range of the text, and
     * on a code point boundary.
     * If the requested position is a break boundary, leave the iteration
     * position on it.
     * If the requested position is not a boundary, leave the iteration
     * position on the preceding boundary and include both the
     * preceding and following boundaries in the cache.
     * Additional boundaries, either preceding or following, may be added
     * to the cache as a side effect.
     *
     * Return false if the operation failed.
     */
    UBool populateNear(int32_t position, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    /**
     *  Add boundary(s) to the cache following the current last boundary.
     *  Return false if at the end of the text, and no more boundaries can be added.
     *  Leave iteration position at the first newly added boundary, or unchanged if no boundary was added.
     */
    UBool populateFollowing();

    /**
     *  Add one or more boundaries to the cache preceding the first currently cached boundary.
     *  Leave the iteration position on the first added boundary.
     *  Return false if no boundaries could be added (if at the start of the text.)
     */
    UBool populatePreceding(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ಎಣಿಕೆ Update_Position_ಮೌಲ್ಯಗಳು {
        RetainCachePosition = 0,
        UpdateCachePosition = 1
    };

    /*
     * Add the boundary following the current position.
     * The current position can be left as it was, or changed to the newly added boundary,
     * as specified by the update parameter.
     */
    ಶೂನ್ಯ ಕೂಡಿಸು_Following(int32_t position, int32_t ನಿಯಮ_ಸ್ಥಿತಿ_Idx, Update_Position_ಮೌಲ್ಯಗಳು update);


    /*
     * Add the boundary preceding the current position.
     * The current position can be left as it was, or changed to the newly added boundary,
     * as specified by the update parameter.
     */
    bool ಕೂಡಿಸು_Preceding(int32_t position, int32_t ನಿಯಮ_ಸ್ಥಿತಿ_Idx, Update_Position_ಮೌಲ್ಯಗಳು update);

    /**
     *  Set the cache position to the specified position, or, if the position
     *  falls between to cached boundaries, to the preceding boundary.
     *  Fails if the requested position is outside of the range of boundaries currently held by the cache.
     *  The startPosition must be on a code point boundary.
     *
     *  Return true if successful, false if the specified position is after
     *  the last cached boundary or before the first.
     */
    UBool                   seek(int32_t ಆರಂಭ_Position);

    ಶೂನ್ಯ dumpCache();

  ಖಾಸಗಿ:
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t   mod_Chunk_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ & (CACHE_ಗಾತ್ರ - 1); }

    ಸ್ಥಿರ constexpr int32_t CACHE_ಗಾತ್ರ = 128;
    static_assert((CACHE_ಗಾತ್ರ & (CACHE_ಗಾತ್ರ-1)) == 0, "CACHE_SIZE must be power of two.");

    ನಿಯಮ_Based_Break_Iterator *fBI;
    int32_t                 f_ಆರಂಭ_Buf_Idx;
    int32_t                 f_ಕೊನೆ_Buf_Idx;    // inclusive

    int32_t                 f_ಪಠ್ಯ_Idx;
    int32_t                 fBufIdx;

    int32_t                 fBoundaries[CACHE_ಗಾತ್ರ];
    uint16_t                fStatuses[CACHE_ಗಾತ್ರ];

    UVector32               f_Side_ಬಫರ್;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // #if !UCONFIG_NO_BREAK_ITERATION

#ಆದರೆ_ಅಂತ್ಯ // RBBI_CACHE_H
