// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.h
//
#నిర్వచితం_కాకపోతే RBBI_CACHE_H
#నిర్వచించు RBBI_CACHE_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/rbbi.h"
#అన్తర్భావ "unicode/uobject.h"

#అన్తర్భావ "uvectr32.h"

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
class నియమం_Based_Break_Iterator::DictionaryCache: బహిరంగ UMemory {
  బహిరంగ:
     DictionaryCache(నియమం_Based_Break_Iterator *bi, UError_సంకేతం &స్థితి_2);
     ~DictionaryCache();

     శూన్య reset();

     UBool following(int32_t fromPos, int32_t *pos, int32_t *స్థితి_సూచీ);
     UBool preceding(int32_t fromPos, int32_t *pos, int32_t *స్థితి_సూచీ);

    /**
     * Populate the cache with the dictionary based boundaries within a region of text.
     * @param startPos  The start position of a range of text
     * @param endPos    The end position of a range of text
     * @param firstRuleStatus The rule status index that applies to the break at startPos
     * @param otherRuleStatus The rule status index that applies to boundaries other than startPos
     * @internal
     */
    శూన్య populateDictionary(int32_t ప్రారంభం_Pos, int32_t చివర_Pos,
                         int32_t మొదటి_నియమం_స్థితి, int32_t other_నియమం_స్థితి);



    నియమం_Based_Break_Iterator *fBI;
    
    UVector32           fBreaks;                // A vector containing the boundaries.
    int32_t             f_Position_లోపల_Cache;       // Index in fBreaks of last boundary returned by following()
                                                //    or preceding(). Optimizes sequential access.
    int32_t             f_ప్రారంభం;                 // Text position of first boundary in cache.
    int32_t             f_హద్దు;                 // Last boundary in cache. Which is the limit of the
                                                //    text segment being handled by the dictionary.
    int32_t             f_మొదటి_నియమం_స్థితి_సూచీ;  // Rule status info for first boundary.
    int32_t             f_Other_నియమం_స్థితి_సూచీ;  // Rule status info for 2nd through last boundaries.

    // If `this->isWordLike`, the status of dictionary breaks is equal to the status of the final
    // break of the rule-based segment they refine (fOtherRuleStatusIndex); otherwise, dictionary
    // breaks have status 0.
    // For compatibility, this property is determined by the largest status value used by `*fBI`:
    // rules that have a largest status greater than 100 are considered word-like.
    bool is_మాట_Like;
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

class నియమం_Based_Break_Iterator::BreakCache: బహిరంగ UMemory {
  బహిరంగ:
                BreakCache(నియమం_Based_Break_Iterator *bi, UError_సంకేతం &స్థితి_2);
    మాయా     ~BreakCache();
    శూన్య        reset(int32_t pos = 0, int32_t నియమం_స్థితి = 0);
    శూన్య        తదుపరి() {    ఒకవేళ (fBufIdx == f_చివర_Buf_Idx) {
                                తదుపరి_OL();
                            } లేకపోతే {
                                fBufIdx = mod_Chunk_కొలత(fBufIdx + 1);
                                f_పాఠ్యం_Idx = fBI->fPosition = fBoundaries[fBufIdx];
                                fBI->f_నియమం_స్థితి_సూచీ = fStatuses[fBufIdx];
                            }
                }


    శూన్య        తదుపరి_OL();
    శూన్య        మునుపటి(UError_సంకేతం &స్థితి_2);

    // Move the iteration state to the position following the startPosition.
    // Input position must be pinned to the input length.
    శూన్య        following(int32_t ప్రారంభం_Position, UError_సంకేతం &స్థితి_2);

    శూన్య        preceding(int32_t ప్రారంభం_Position, UError_సంకేతం &స్థితి_2);

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
    UBool populateNear(int32_t position, UError_సంకేతం &స్థితి_2);

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
    UBool populatePreceding(UError_సంకేతం &స్థితి_2);

    గణన Update_Position_విలువలు {
        RetainCachePosition = 0,
        UpdateCachePosition = 1
    };

    /*
     * Add the boundary following the current position.
     * The current position can be left as it was, or changed to the newly added boundary,
     * as specified by the update parameter.
     */
    శూన్య కూడు_Following(int32_t position, int32_t నియమం_స్థితి_Idx, Update_Position_విలువలు update);


    /*
     * Add the boundary preceding the current position.
     * The current position can be left as it was, or changed to the newly added boundary,
     * as specified by the update parameter.
     */
    bool కూడు_Preceding(int32_t position, int32_t నియమం_స్థితి_Idx, Update_Position_విలువలు update);

    /**
     *  Set the cache position to the specified position, or, if the position
     *  falls between to cached boundaries, to the preceding boundary.
     *  Fails if the requested position is outside of the range of boundaries currently held by the cache.
     *  The startPosition must be on a code point boundary.
     *
     *  Return true if successful, false if the specified position is after
     *  the last cached boundary or before the first.
     */
    UBool                   seek(int32_t ప్రారంభం_Position);

    శూన్య dumpCache();

  అంతరంగ:
    స్థిర అంతఃవరుస int32_t   mod_Chunk_కొలత(పూర్ణాంకం సూచీ) { తిరిగి సూచీ & (CACHE_కొలత - 1); }

    స్థిర constexpr int32_t CACHE_కొలత = 128;
    static_assert((CACHE_కొలత & (CACHE_కొలత-1)) == 0, "CACHE_SIZE must be power of two.");

    నియమం_Based_Break_Iterator *fBI;
    int32_t                 f_ప్రారంభం_Buf_Idx;
    int32_t                 f_చివర_Buf_Idx;    // inclusive

    int32_t                 f_పాఠ్యం_Idx;
    int32_t                 fBufIdx;

    int32_t                 fBoundaries[CACHE_కొలత];
    uint16_t                fStatuses[CACHE_కొలత];

    UVector32               f_Side_బఫరు;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు // #if !UCONFIG_NO_BREAK_ITERATION

#అయితే_ముగింపు // RBBI_CACHE_H
