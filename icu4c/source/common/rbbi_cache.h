// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.h
//
#اگر_تعریف_نشده RBBI_CACHE_H
#تعریف_کن RBBI_CACHE_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/rbbi.h"
#ضمن "unicode/uobject.h"

#ضمن "uvectr32.h"

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
class قاعده_Based_Break_Iterator::DictionaryCache: عام UMemory {
  عام:
     DictionaryCache(قاعده_Based_Break_Iterator *bi, UError_کد &وضعیت);
     ~DictionaryCache();

     فراغ reset();

     UBool following(int32_t fromPos, int32_t *pos, int32_t *وضعیت_شاخص);
     UBool preceding(int32_t fromPos, int32_t *pos, int32_t *وضعیت_شاخص);

    /**
     * Populate the cache with the dictionary based boundaries within a region of text.
     * @param startPos  The start position of a range of text
     * @param endPos    The end position of a range of text
     * @param firstRuleStatus The rule status index that applies to the break at startPos
     * @param otherRuleStatus The rule status index that applies to boundaries other than startPos
     * @internal
     */
    فراغ populateDictionary(int32_t شروع_Pos, int32_t انتها_Pos,
                         int32_t اولی_قاعده_وضعیت, int32_t other_قاعده_وضعیت);



    قاعده_Based_Break_Iterator *fBI;
    
    UVector32           fBreaks;                // A vector containing the boundaries.
    int32_t             f_Position_درون_Cache;       // Index in fBreaks of last boundary returned by following()
                                                //    or preceding(). Optimizes sequential access.
    int32_t             f_شروع;                 // Text position of first boundary in cache.
    int32_t             f_حد;                 // Last boundary in cache. Which is the limit of the
                                                //    text segment being handled by the dictionary.
    int32_t             f_اولی_قاعده_وضعیت_شاخص;  // Rule status info for first boundary.
    int32_t             f_Other_قاعده_وضعیت_شاخص;  // Rule status info for 2nd through last boundaries.

    // If `this->isWordLike`, the status of dictionary breaks is equal to the status of the final
    // break of the rule-based segment they refine (fOtherRuleStatusIndex); otherwise, dictionary
    // breaks have status 0.
    // For compatibility, this property is determined by the largest status value used by `*fBI`:
    // rules that have a largest status greater than 100 are considered word-like.
    bool is_کلمه_Like;
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

class قاعده_Based_Break_Iterator::BreakCache: عام UMemory {
  عام:
                BreakCache(قاعده_Based_Break_Iterator *bi, UError_کد &وضعیت);
    مجازی     ~BreakCache();
    فراغ        reset(int32_t pos = 0, int32_t قاعده_وضعیت = 0);
    فراغ        بعدی() {    إذا (fBufIdx == f_انتها_Buf_Idx) {
                                بعدی_OL();
                            } وإلا {
                                fBufIdx = mod_Chunk_اندازه(fBufIdx + 1);
                                f_متن_Idx = fBI->fPosition = fBoundaries[fBufIdx];
                                fBI->f_قاعده_وضعیت_شاخص = fStatuses[fBufIdx];
                            }
                }


    فراغ        بعدی_OL();
    فراغ        قبلی(UError_کد &وضعیت);

    // Move the iteration state to the position following the startPosition.
    // Input position must be pinned to the input length.
    فراغ        following(int32_t شروع_Position, UError_کد &وضعیت);

    فراغ        preceding(int32_t شروع_Position, UError_کد &وضعیت);

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
    UBool populateNear(int32_t position, UError_کد &وضعیت);

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
    UBool populatePreceding(UError_کد &وضعیت);

    تعداد Update_Position_مقادیر {
        RetainCachePosition = 0,
        UpdateCachePosition = 1
    };

    /*
     * Add the boundary following the current position.
     * The current position can be left as it was, or changed to the newly added boundary,
     * as specified by the update parameter.
     */
    فراغ جمع_کن_Following(int32_t position, int32_t قاعده_وضعیت_Idx, Update_Position_مقادیر update);


    /*
     * Add the boundary preceding the current position.
     * The current position can be left as it was, or changed to the newly added boundary,
     * as specified by the update parameter.
     */
    bool جمع_کن_Preceding(int32_t position, int32_t قاعده_وضعیت_Idx, Update_Position_مقادیر update);

    /**
     *  Set the cache position to the specified position, or, if the position
     *  falls between to cached boundaries, to the preceding boundary.
     *  Fails if the requested position is outside of the range of boundaries currently held by the cache.
     *  The startPosition must be on a code point boundary.
     *
     *  Return true if successful, false if the specified position is after
     *  the last cached boundary or before the first.
     */
    UBool                   seek(int32_t شروع_Position);

    فراغ dumpCache();

  خاص:
    ساكن مضمن int32_t   mod_Chunk_اندازه(صحيح شاخص) { أرجع شاخص & (CACHE_اندازه - 1); }

    ساكن constexpr int32_t CACHE_اندازه = 128;
    static_assert((CACHE_اندازه & (CACHE_اندازه-1)) == 0, "CACHE_SIZE must be power of two.");

    قاعده_Based_Break_Iterator *fBI;
    int32_t                 f_شروع_Buf_Idx;
    int32_t                 f_انتها_Buf_Idx;    // inclusive

    int32_t                 f_متن_Idx;
    int32_t                 fBufIdx;

    int32_t                 fBoundaries[CACHE_اندازه];
    uint16_t                fStatuses[CACHE_اندازه];

    UVector32               f_Side_میانگیر;
};

U_NAMESPACE_انتها

#پایان_اگر // #if !UCONFIG_NO_BREAK_ITERATION

#پایان_اگر // RBBI_CACHE_H
