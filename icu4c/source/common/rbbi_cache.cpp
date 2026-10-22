// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#include "unicode/utypes.h"

#如果 !UCONFIG_NO_BREAK_ITERATION

#include "unicode/ubrk.h"
#include "unicode/rbbi.h"

#include "rbbi_cache.h"

#include "brkeng.h"
#include "cmemory.h"
#include "rbbidata.h"
#include "rbbirb.h"
#include "uassert.h"
#include "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

RuleBasedBreakIterator::DictionaryCache::DictionaryCache(RuleBasedBreakIterator *bi, UErrorCode &status) :
        fBI(bi), fBreaks(status), fPositionInCache(-1),
        fStart(0), fLimit(0), fFirstRuleStatusIndex(0), fOtherRuleStatusIndex(0) {
}

RuleBasedBreakIterator::DictionaryCache::~DictionaryCache() {
}

void RuleBasedBreakIterator::DictionaryCache::reset() {
    fPositionInCache = -1;
    fStart = 0;
    fLimit = 0;
    fFirstRuleStatusIndex = 0;
    fOtherRuleStatusIndex = 0;
    fBreaks.removeAllElements();
    int32_t maxStatus = 0;
    为 (int32_t i = 0; i < fBI->fData->fStatusMaxIdx; ++i) {
        maxStatus = std::max(maxStatus, fBI->fData->fRuleStatusTable[i]);
    }
    isWordLike = maxStatus > 100;
}

UBool RuleBasedBreakIterator::DictionaryCache::following(int32_t fromPos, int32_t *result, int32_t *statusIndex) {
    如果 (fromPos >= fLimit || fromPos < fStart) {
        fPositionInCache = -1;
        返回 false;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    如果 (fPositionInCache >= 0 && fPositionInCache < fBreaks.size() && fBreaks.elementAti(fPositionInCache) == fromPos) {
        ++fPositionInCache;
        如果 (fPositionInCache >= fBreaks.size()) {
            fPositionInCache = -1;
            返回 false;
        }
        r = fBreaks.elementAti(fPositionInCache);
        U_ASSERT(r > fromPos);
        *result = r;
        *statusIndex = isWordLike || r == fLimit ? fOtherRuleStatusIndex : 0;
        返回 true;
    }

    // Random indexing. Linear search for the boundary following the given position.

    为 (fPositionInCache = 0; fPositionInCache < fBreaks.size(); ++fPositionInCache) {
        r= fBreaks.elementAti(fPositionInCache);
        如果 (r > fromPos) {
            *result = r;
            *statusIndex = isWordLike || r == fLimit ? fOtherRuleStatusIndex : 0;
            返回 true;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool RuleBasedBreakIterator::DictionaryCache::preceding(int32_t fromPos, int32_t *result, int32_t *statusIndex) {
    如果 (fromPos <= fStart || fromPos > fLimit) {
        fPositionInCache = -1;
        返回 false;
    }

    如果 (fromPos == fLimit) {
        fPositionInCache = fBreaks.size() - 1;
        如果 (fPositionInCache >= 0) {
            U_ASSERT(fBreaks.elementAti(fPositionInCache) == fromPos);
        }
    }

    int32_t r;
    如果 (fPositionInCache > 0 && fPositionInCache < fBreaks.size() && fBreaks.elementAti(fPositionInCache) == fromPos) {
        --fPositionInCache;
        r = fBreaks.elementAti(fPositionInCache);
        U_ASSERT(r < fromPos);
        *result = r;
        *statusIndex = (r == fStart) ? fFirstRuleStatusIndex
                       : isWordLike  ? fOtherRuleStatusIndex
                                     : 0;
        返回 true;
    }

    如果 (fPositionInCache == 0) {
        fPositionInCache = -1;
        返回 false;
    }

    为 (fPositionInCache = fBreaks.size()-1; fPositionInCache >= 0; --fPositionInCache) {
        r = fBreaks.elementAti(fPositionInCache);
        如果 (r < fromPos) {
            *result = r;
            *statusIndex = (r == fStart) ? fFirstRuleStatusIndex
                           : isWordLike  ? fOtherRuleStatusIndex
                                         : 0;
            返回 true;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

void RuleBasedBreakIterator::DictionaryCache::populateDictionary(int32_t startPos, int32_t endPos,
                                       int32_t firstRuleStatus, int32_t otherRuleStatus) {
    如果 ((endPos - startPos) <= 1) {
        返回;
    }

    reset();
    fFirstRuleStatusIndex = firstRuleStatus;
    fOtherRuleStatusIndex = otherRuleStatus;

    int32_t rangeStart = startPos;
    int32_t rangeEnd = endPos;

    uint16_t    category;
    int32_t     current;
    UErrorCode  status = U_ZERO_ERROR;
    int32_t     foundBreakCount = 0;
    UText      *text = &fBI->fText;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_setNativeIndex(text, rangeStart);
    UChar32     c = utext_current32(text);
    category = ucptrie_get(fBI->fData->fTrie, c);
    uint32_t dictStart = fBI->fData->fForwardTable->fDictCategoriesStart;

    当(U_SUCCESS(status)) {
        当 ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(text))) < rangeEnd
                && (category < dictStart)) {
            utext_next32(text);           // TODO: cleaner loop structure.
            c = utext_current32(text);
            category = ucptrie_get(fBI->fData->fTrie, c);
        }
        如果 (current >= rangeEnd) {
            break;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        const LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, status));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        如果 (lbe != nullptr) {
            foundBreakCount += lbe->findBreaks(text, current, rangeEnd, fBreaks, fBI->fIsPhraseBreaking, status);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(text);
        category = ucptrie_get(fBI->fData->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    如果 (foundBreakCount > 0) {
        U_ASSERT(foundBreakCount == fBreaks.size());
        如果 (startPos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.insertElementAt(startPos, 0, status);
        }
        如果 (endPos > fBreaks.peeki()) {
            fBreaks.push(endPos, status);
        }
        fPositionInCache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        fStart = fBreaks.elementAti(0);
        fLimit = fBreaks.peeki();
    } 否则 {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

RuleBasedBreakIterator::BreakCache::BreakCache(RuleBasedBreakIterator *bi, UErrorCode &status) :
        fBI(bi), fSideBuffer(status) {
    reset();
}


RuleBasedBreakIterator::BreakCache::~BreakCache() {
}


void RuleBasedBreakIterator::BreakCache::reset(int32_t pos, int32_t ruleStatus) {
    fStartBufIdx = 0;
    fEndBufIdx = 0;
    fTextIdx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(ruleStatus);
}


int32_t  RuleBasedBreakIterator::BreakCache::current() {
    fBI->fPosition = fTextIdx;
    fBI->fRuleStatusIndex = fStatuses[fBufIdx];
    fBI->fDone = false;
    返回 fTextIdx;
}


void RuleBasedBreakIterator::BreakCache::following(int32_t startPos, UErrorCode &status) {
    如果 (U_FAILURE(status)) {
        返回;
    }
    如果 (startPos == fTextIdx || seek(startPos) || populateNear(startPos, status)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->fDone = false;
        next();
    }
}


void RuleBasedBreakIterator::BreakCache::preceding(int32_t startPos, UErrorCode &status) {
    如果 (U_FAILURE(status)) {
        返回;
    }
    如果 (startPos == fTextIdx || seek(startPos) || populateNear(startPos, status)) {
        如果 (startPos == fTextIdx) {
            previous(status);
        } 否则 {
            // seek() leaves the BreakCache positioned at the preceding boundary
            //        if the requested position is between two boundaries.
            // current() pushes the BreakCache position out to the BreakIterator itself.
            U_ASSERT(startPos > fTextIdx);
            current();
        }
    }
}


/*
 * Out-of-line code for BreakCache::next().
 * Cache does not already contain the boundary
 */
void RuleBasedBreakIterator::BreakCache::nextOL() {
    fBI->fDone = !populateFollowing();
    fBI->fPosition = fTextIdx;
    fBI->fRuleStatusIndex = fStatuses[fBufIdx];
}


void RuleBasedBreakIterator::BreakCache::previous(UErrorCode &status) {
    如果 (U_FAILURE(status)) {
        返回;
    }
    int32_t initialBufIdx = fBufIdx;
    如果 (fBufIdx == fStartBufIdx) {
        // At start of cache. Prepend to it.
        populatePreceding(status);
    } 否则 {
        // Cache already holds the next boundary
        fBufIdx = modChunkSize(fBufIdx - 1);
        fTextIdx = fBoundaries[fBufIdx];
    }
    fBI->fDone = (fBufIdx == initialBufIdx);
    fBI->fPosition = fTextIdx;
    fBI->fRuleStatusIndex = fStatuses[fBufIdx];
}


UBool RuleBasedBreakIterator::BreakCache::seek(int32_t pos) {
    如果 (pos < fBoundaries[fStartBufIdx] || pos > fBoundaries[fEndBufIdx]) {
        返回 false;
    }
    如果 (pos == fBoundaries[fStartBufIdx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = fStartBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        返回 true;
    }
    如果 (pos == fBoundaries[fEndBufIdx]) {
        fBufIdx = fEndBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        返回 true;
    }

    int32_t min = fStartBufIdx;
    int32_t max = fEndBufIdx;
    当 (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_SIZE : 0)) / 2;
        probe = modChunkSize(probe);
        如果 (fBoundaries[probe] > pos) {
            max = probe;
        } 否则 {
            min = modChunkSize(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = modChunkSize(max - 1);
    fTextIdx = fBoundaries[fBufIdx];
    U_ASSERT(fTextIdx <= pos);
    返回 true;
}


UBool RuleBasedBreakIterator::BreakCache::populateNear(int32_t position, UErrorCode &status) {
    如果 (U_FAILURE(status)) {
        返回 false;
    }
    U_ASSERT(position < fBoundaries[fStartBufIdx] || position > fBoundaries[fEndBufIdx]);

    // Add boundaries to the cache near the specified position.
    // The given position need not be a boundary itself.
    // The input position must be within the range of the text, and
    // on a code point boundary.
    // If the requested position is a break boundary, leave the iteration
    // position on it.
    // If the requested position is not a boundary, leave the iteration
    // position on the preceding boundary and include both the
    // preceding and following boundaries in the cache.
    // Additional boundaries, either preceding or following, may be added
    // to the cache as a side effect.

    // If the requested position is not near already cached positions, clear the existing cache,
    // find a near-by boundary and begin new cache contents there.

    // Threshold for a text position to be considered near to existing cache contents.
    // TODO: See issue ICU-22024 "perf tuning of Cache needed."
    //       This value is subject to change. See the ticket for more details.
    static constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t ruleStatusIndex = 0;
    bool retainCache = false;
    如果 ((position > fBoundaries[fStartBufIdx] - CACHE_NEAR) && position < (fBoundaries[fEndBufIdx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = true;
    } 否则 如果 (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = false;
        aBoundary = 0;
    } 否则 {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handleSafePrevious(position);

        如果 (fBoundaries[fEndBufIdx] < position && fBoundaries[fEndBufIdx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = true;
        } 否则 如果 (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[fStartBufIdx] <= (position + CACHE_NEAR));
        } 否则 {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = false;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handleNext();
            如果 (aBoundary != UBRK_DONE && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_setNativeIndex(&fBI->fText, aBoundary);
                如果 (backupPos == utext_getPreviousNativeIndex(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handleNext();   // Safe rules identify safe pairs.
                }
            }
            如果 (aBoundary == UBRK_DONE) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_nativeLength(&fBI->fText);
            }
            ruleStatusIndex = fBI->fRuleStatusIndex;
        }
    }

    如果 (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, ruleStatusIndex);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    如果 (fBoundaries[fEndBufIdx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        当 (fBoundaries[fEndBufIdx] < position) {
            如果 (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = fEndBufIdx;                      // Set iterator position to the end of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        当 (fTextIdx > position) {              // Move backwards to a position at or preceding the requested pos.
            previous(status);
        }
        返回 true;
    }

    如果 (fBoundaries[fStartBufIdx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        当 (fBoundaries[fStartBufIdx] > position) {
            populatePreceding(status);
        }
        fBufIdx = fStartBufIdx;                    // Set iterator position to the start of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        当 (fTextIdx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        如果 (fTextIdx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            previous(status);
        }
        返回 true;
    }

    U_ASSERT(fTextIdx == position);
    返回 true;
}



UBool RuleBasedBreakIterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[fEndBufIdx];
    int32_t fromRuleStatusIdx = fStatuses[fEndBufIdx];
    int32_t pos = 0;
    int32_t ruleStatusIdx = 0;

    如果 (fBI->fDictionaryCache->following(fromPosition, &pos, &ruleStatusIdx)) {
        addFollowing(pos, ruleStatusIdx, UpdateCachePosition);
        返回 true;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handleNext();
    如果 (pos == UBRK_DONE) {
        返回 false;
    }

    ruleStatusIdx = fBI->fRuleStatusIndex;
    如果 (fBI->fDictionaryCharCount > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, fromRuleStatusIdx, ruleStatusIdx);
        如果 (fBI->fDictionaryCache->following(fromPosition, &pos, &ruleStatusIdx)) {
            addFollowing(pos, ruleStatusIdx, UpdateCachePosition);
            返回 true;
            // TODO: may want to move a sizable chunk of dictionary cache to break cache at this point.
            //       But be careful with interactions with populateNear().
        }
    }

    // Rule based segment did not include dictionary characters.
    // Or, it did contain dictionary chars, but the dictionary segmenter didn't handle them,
    //    meaning that we didn't take the return, above.
    // Add its end point to the cache.
    addFollowing(pos, ruleStatusIdx, UpdateCachePosition);

    // Add several non-dictionary boundaries at this point, to optimize straight forward iteration.
    //    (subsequent calls to BreakIterator::next() will take the fast path, getting cached results.
    //
    为 (整数 count=0; count<6; ++count) {
        pos = fBI->handleNext();
        如果 (pos == UBRK_DONE || fBI->fDictionaryCharCount > 0) {
            break;
        }
        addFollowing(pos, fBI->fRuleStatusIndex, RetainCachePosition);
    }

    返回 true;
}


UBool RuleBasedBreakIterator::BreakCache::populatePreceding(UErrorCode &status) {
    如果 (U_FAILURE(status)) {
        返回 false;
    }

    int32_t fromPosition = fBoundaries[fStartBufIdx];
    如果 (fromPosition == 0) {
        返回 false;
    }

    int32_t position = 0;
    int32_t positionStatusIdx = 0;

    如果 (fBI->fDictionaryCache->preceding(fromPosition, &position, &positionStatusIdx)) {
        addPreceding(position, positionStatusIdx, UpdateCachePosition);
        返回 true;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    do {
        backupPosition = backupPosition - 30;
        如果 (backupPosition <= 0) {
            backupPosition = 0;
        } 否则 {
            backupPosition = fBI->handleSafePrevious(backupPosition);
        }
        如果 (backupPosition == UBRK_DONE || backupPosition == 0) {
            position = 0;
            positionStatusIdx = 0;
        } 否则 {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handleNext();
            如果 (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_setNativeIndex(&fBI->fText, position);
                如果 (backupPosition == utext_getPreviousNativeIndex(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handleNext();   // Safe rules identify safe pairs.
                }
            }
            positionStatusIdx = fBI->fRuleStatusIndex;
        }
    } 当 (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    fSideBuffer.removeAllElements();
    fSideBuffer.addElement(position, status);
    fSideBuffer.addElement(positionStatusIdx, status);

    do {
        int32_t prevPosition = fBI->fPosition = position;
        int32_t prevStatusIdx = positionStatusIdx;
        position = fBI->handleNext();
        positionStatusIdx = fBI->fRuleStatusIndex;
        如果 (position == UBRK_DONE) {
            break;
        }

        UBool segmentHandledByDictionary = false;
        如果 (fBI->fDictionaryCharCount != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dictSegEndPosition = position;
            fBI->fDictionaryCache->populateDictionary(prevPosition, dictSegEndPosition, prevStatusIdx, positionStatusIdx);
            当 (fBI->fDictionaryCache->following(prevPosition, &position, &positionStatusIdx)) {
                segmentHandledByDictionary = true;
                U_ASSERT(position > prevPosition);
                如果 (position >= fromPosition) {
                    break;
                }
                U_ASSERT(position <= dictSegEndPosition);
                fSideBuffer.addElement(position, status);
                fSideBuffer.addElement(positionStatusIdx, status);
                prevPosition = position;
            }
            U_ASSERT(position==dictSegEndPosition || position>=fromPosition);
        }

        如果 (!segmentHandledByDictionary && position < fromPosition) {
            fSideBuffer.addElement(position, status);
            fSideBuffer.addElement(positionStatusIdx, status);
        }
    } 当 (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = false;
    如果 (!fSideBuffer.isEmpty()) {
        positionStatusIdx = fSideBuffer.popi();
        position = fSideBuffer.popi();
        addPreceding(position, positionStatusIdx, UpdateCachePosition);
        success = true;
    }

    当 (!fSideBuffer.isEmpty()) {
        positionStatusIdx = fSideBuffer.popi();
        position = fSideBuffer.popi();
        如果 (!addPreceding(position, positionStatusIdx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            break;
        }
    }

    返回 success;
}


void RuleBasedBreakIterator::BreakCache::addFollowing(int32_t position, int32_t ruleStatusIdx, UpdatePositionValues update) {
    U_ASSERT(position > fBoundaries[fEndBufIdx]);
    U_ASSERT(ruleStatusIdx <= UINT16_MAX);
    int32_t nextIdx = modChunkSize(fEndBufIdx + 1);
    如果 (nextIdx == fStartBufIdx) {
        fStartBufIdx = modChunkSize(fStartBufIdx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[nextIdx] = position;
    fStatuses[nextIdx] = static_cast<uint16_t>(ruleStatusIdx);
    fEndBufIdx = nextIdx;
    如果 (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = nextIdx;
        fTextIdx = position;
    } 否则 {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(nextIdx != fBufIdx);
    }
}

bool RuleBasedBreakIterator::BreakCache::addPreceding(int32_t position, int32_t ruleStatusIdx, UpdatePositionValues update) {
    U_ASSERT(position < fBoundaries[fStartBufIdx]);
    U_ASSERT(ruleStatusIdx <= UINT16_MAX);
    int32_t nextIdx = modChunkSize(fStartBufIdx - 1);
    如果 (nextIdx == fEndBufIdx) {
        如果 (fBufIdx == fEndBufIdx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            返回 false;
        }
        fEndBufIdx = modChunkSize(fEndBufIdx - 1);
    }
    fBoundaries[nextIdx] = position;
    fStatuses[nextIdx] = static_cast<uint16_t>(ruleStatusIdx);
    fStartBufIdx = nextIdx;
    如果 (update == UpdateCachePosition) {
        fBufIdx = nextIdx;
        fTextIdx = position;
    }
    返回 true;
}


void RuleBasedBreakIterator::BreakCache::dumpCache() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", fTextIdx, fBufIdx);
    为 (int32_t i=fStartBufIdx; ; i=modChunkSize(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        如果 (i == fEndBufIdx) {
            break;
        }
    }
#endif
}

U_NAMESPACE_END

#endif // #if !UCONFIG_NO_BREAK_ITERATION
