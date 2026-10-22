// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.h
//
#اگر_غیر_تعریف_شدہ RBBI_CACHE_H
#تعریف_کرو RBBI_CACHE_H

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/rbbi.h"
#شامل "unicode/uobject.h"

#شامل "uvectr32.h"

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
class قاعدہ_Based_Break_Iterator::DictionaryCache: عوامی UMemory {
  عوامی:
     DictionaryCache(قاعدہ_Based_Break_Iterator *bi, UError_رمز &حالت_2);
     ~DictionaryCache();

     خالی reset();

     UBool following(int32_t fromPos, int32_t *pos, int32_t *حالت_اشاریہ);
     UBool preceding(int32_t fromPos, int32_t *pos, int32_t *حالت_اشاریہ);

    /**
     * Populate the cache with the dictionary based boundaries within a region of text.
     * @param startPos  The start position of a range of text
     * @param endPos    The end position of a range of text
     * @param firstRuleStatus The rule status index that applies to the break at startPos
     * @param otherRuleStatus The rule status index that applies to boundaries other than startPos
     * @internal
     */
    خالی populateDictionary(int32_t آغاز_Pos, int32_t انتہا_Pos,
                         int32_t پہلا_قاعدہ_حالت, int32_t other_قاعدہ_حالت);



    قاعدہ_Based_Break_Iterator *fBI;
    
    UVector32           fBreaks;                // A vector containing the boundaries.
    int32_t             f_Position_اندر_Cache;       // Index in fBreaks of last boundary returned by following()
                                                //    or preceding(). Optimizes sequential access.
    int32_t             f_آغاز;                 // Text position of first boundary in cache.
    int32_t             f_حد_بندی;                 // Last boundary in cache. Which is the limit of the
                                                //    text segment being handled by the dictionary.
    int32_t             f_پہلا_قاعدہ_حالت_اشاریہ;  // Rule status info for first boundary.
    int32_t             f_Other_قاعدہ_حالت_اشاریہ;  // Rule status info for 2nd through last boundaries.

    // If `this->isWordLike`, the status of dictionary breaks is equal to the status of the final
    // break of the rule-based segment they refine (fOtherRuleStatusIndex); otherwise, dictionary
    // breaks have status 0.
    // For compatibility, this property is determined by the largest status value used by `*fBI`:
    // rules that have a largest status greater than 100 are considered word-like.
    bool is_لفظ_Like;
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

class قاعدہ_Based_Break_Iterator::BreakCache: عوامی UMemory {
  عوامی:
                BreakCache(قاعدہ_Based_Break_Iterator *bi, UError_رمز &حالت_2);
    مجازی     ~BreakCache();
    خالی        reset(int32_t pos = 0, int32_t قاعدہ_حالت = 0);
    خالی        اگلا() {    اگر (fBufIdx == f_انتہا_Buf_Idx) {
                                اگلا_OL();
                            } ورنہ {
                                fBufIdx = mod_Chunk_سائز(fBufIdx + 1);
                                f_متن_Idx = fBI->fPosition = fBoundaries[fBufIdx];
                                fBI->f_قاعدہ_حالت_اشاریہ = fStatuses[fBufIdx];
                            }
                }


    خالی        اگلا_OL();
    خالی        پچھلا_2(UError_رمز &حالت_2);

    // Move the iteration state to the position following the startPosition.
    // Input position must be pinned to the input length.
    خالی        following(int32_t آغاز_Position, UError_رمز &حالت_2);

    خالی        preceding(int32_t آغاز_Position, UError_رمز &حالت_2);

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
    UBool populateNear(int32_t position, UError_رمز &حالت_2);

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
    UBool populatePreceding(UError_رمز &حالت_2);

    شمار Update_Position_اقدار {
        RetainCachePosition = 0,
        UpdateCachePosition = 1
    };

    /*
     * Add the boundary following the current position.
     * The current position can be left as it was, or changed to the newly added boundary,
     * as specified by the update parameter.
     */
    خالی جوڑو_Following(int32_t position, int32_t قاعدہ_حالت_Idx, Update_Position_اقدار update);


    /*
     * Add the boundary preceding the current position.
     * The current position can be left as it was, or changed to the newly added boundary,
     * as specified by the update parameter.
     */
    bool جوڑو_Preceding(int32_t position, int32_t قاعدہ_حالت_Idx, Update_Position_اقدار update);

    /**
     *  Set the cache position to the specified position, or, if the position
     *  falls between to cached boundaries, to the preceding boundary.
     *  Fails if the requested position is outside of the range of boundaries currently held by the cache.
     *  The startPosition must be on a code point boundary.
     *
     *  Return true if successful, false if the specified position is after
     *  the last cached boundary or before the first.
     */
    UBool                   seek(int32_t آغاز_Position);

    خالی dumpCache();

  نجی:
    ساکن اندرونی int32_t   mod_Chunk_سائز(صحیح اشاریہ) { واپس اشاریہ & (CACHE_سائز - 1); }

    ساکن constexpr int32_t CACHE_سائز = 128;
    static_assert((CACHE_سائز & (CACHE_سائز-1)) == 0, "CACHE_SIZE must be power of two.");

    قاعدہ_Based_Break_Iterator *fBI;
    int32_t                 f_آغاز_Buf_Idx;
    int32_t                 f_انتہا_Buf_Idx;    // inclusive

    int32_t                 f_متن_Idx;
    int32_t                 fBufIdx;

    int32_t                 fBoundaries[CACHE_سائز];
    uint16_t                fStatuses[CACHE_سائز];

    UVector32               f_Side_بفر;
};

U_NAMESPACE_انتہا

#اگر_ختم // #if !UCONFIG_NO_BREAK_ITERATION

#اگر_ختم // RBBI_CACHE_H
