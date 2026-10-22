// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/ubrk.h"
#അന്തര്ഭാവ "unicode/rbbi.h"

#അന്തര്ഭാവ "rbbi_cache.h"

#അന്തര്ഭാവ "brkeng.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "rbbidata.h"
#അന്തര്ഭാവ "rbbirb.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "uvectr32.h"

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

ശൂന്യം RuleBasedBreakIterator::DictionaryCache::reset() {
    fPositionInCache = -1;
    fStart = 0;
    fLimit = 0;
    fFirstRuleStatusIndex = 0;
    fOtherRuleStatusIndex = 0;
    fBreaks.removeAllElements();
    int32_t maxStatus = 0;
    വേണ്ടി (int32_t i = 0; i < fBI->fData->fStatusMaxIdx; ++i) {
        maxStatus = std::max(maxStatus, fBI->fData->fRuleStatusTable[i]);
    }
    isWordLike = maxStatus > 100;
}

UBool RuleBasedBreakIterator::DictionaryCache::following(int32_t fromPos, int32_t *result, int32_t *statusIndex) {
    എങ്കിൽ (fromPos >= fLimit || fromPos < fStart) {
        fPositionInCache = -1;
        തിരികെ അസത്യം;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    എങ്കിൽ (fPositionInCache >= 0 && fPositionInCache < fBreaks.size() && fBreaks.elementAti(fPositionInCache) == fromPos) {
        ++fPositionInCache;
        എങ്കിൽ (fPositionInCache >= fBreaks.size()) {
            fPositionInCache = -1;
            തിരികെ അസത്യം;
        }
        r = fBreaks.elementAti(fPositionInCache);
        U_ASSERT(r > fromPos);
        *result = r;
        *statusIndex = isWordLike || r == fLimit ? fOtherRuleStatusIndex : 0;
        തിരികെ സത്യം;
    }

    // Random indexing. Linear search for the boundary following the given position.

    വേണ്ടി (fPositionInCache = 0; fPositionInCache < fBreaks.size(); ++fPositionInCache) {
        r= fBreaks.elementAti(fPositionInCache);
        എങ്കിൽ (r > fromPos) {
            *result = r;
            *statusIndex = isWordLike || r == fLimit ? fOtherRuleStatusIndex : 0;
            തിരികെ സത്യം;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool RuleBasedBreakIterator::DictionaryCache::preceding(int32_t fromPos, int32_t *result, int32_t *statusIndex) {
    എങ്കിൽ (fromPos <= fStart || fromPos > fLimit) {
        fPositionInCache = -1;
        തിരികെ അസത്യം;
    }

    എങ്കിൽ (fromPos == fLimit) {
        fPositionInCache = fBreaks.size() - 1;
        എങ്കിൽ (fPositionInCache >= 0) {
            U_ASSERT(fBreaks.elementAti(fPositionInCache) == fromPos);
        }
    }

    int32_t r;
    എങ്കിൽ (fPositionInCache > 0 && fPositionInCache < fBreaks.size() && fBreaks.elementAti(fPositionInCache) == fromPos) {
        --fPositionInCache;
        r = fBreaks.elementAti(fPositionInCache);
        U_ASSERT(r < fromPos);
        *result = r;
        *statusIndex = (r == fStart) ? fFirstRuleStatusIndex
                       : isWordLike  ? fOtherRuleStatusIndex
                                     : 0;
        തിരികെ സത്യം;
    }

    എങ്കിൽ (fPositionInCache == 0) {
        fPositionInCache = -1;
        തിരികെ അസത്യം;
    }

    വേണ്ടി (fPositionInCache = fBreaks.size()-1; fPositionInCache >= 0; --fPositionInCache) {
        r = fBreaks.elementAti(fPositionInCache);
        എങ്കിൽ (r < fromPos) {
            *result = r;
            *statusIndex = (r == fStart) ? fFirstRuleStatusIndex
                           : isWordLike  ? fOtherRuleStatusIndex
                                         : 0;
            തിരികെ സത്യം;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

ശൂന്യം RuleBasedBreakIterator::DictionaryCache::populateDictionary(int32_t startPos, int32_t endPos,
                                       int32_t firstRuleStatus, int32_t otherRuleStatus) {
    എങ്കിൽ ((endPos - startPos) <= 1) {
        തിരികെ;
    }

    reset();
    fFirstRuleStatusIndex = firstRuleStatus;
    fOtherRuleStatusIndex = otherRuleStatus;

    int32_t rangeStart = startPos;
    int32_t rangeEnd = endPos;

    uint16_t    category;
    int32_t     current;
    UErrorCode  status = U_ZERO_ERROR;
    int32_t     found_Break_എണ്ണം = 0;
    UText      *text = &fBI->fText;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_setNativeIndex(text, rangeStart);
    UChar32     c = utext_current32(text);
    category = ucptrie_get(fBI->fData->fTrie, c);
    uint32_t dictStart = fBI->fData->fForwardTable->fDictCategoriesStart;

    ആകുമ്പോൾ(U_SUCCESS(status)) {
        ആകുമ്പോൾ ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(text))) < rangeEnd
                && (category < dictStart)) {
            utext_next32(text);           // TODO: cleaner loop structure.
            c = utext_current32(text);
            category = ucptrie_get(fBI->fData->fTrie, c);
        }
        എങ്കിൽ (current >= rangeEnd) {
            നിർത്തുക;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        സ്ഥിരം LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, status));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        എങ്കിൽ (lbe != nullptr) {
            found_Break_എണ്ണം += lbe->findBreaks(text, current, rangeEnd, fBreaks, fBI->fIsPhraseBreaking, status);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(text);
        category = ucptrie_get(fBI->fData->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    എങ്കിൽ (found_Break_എണ്ണം > 0) {
        U_ASSERT(found_Break_എണ്ണം == fBreaks.size());
        എങ്കിൽ (startPos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.തിരുകുക_Element_At(startPos, 0, status);
        }
        എങ്കിൽ (endPos > fBreaks.peeki()) {
            fBreaks.push(endPos, status);
        }
        fPositionInCache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        fStart = fBreaks.elementAti(0);
        fLimit = fBreaks.peeki();
    } അല്ലെങ്കിൽ {
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


ശൂന്യം RuleBasedBreakIterator::BreakCache::reset(int32_t pos, int32_t ruleStatus) {
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
    fBI->fDone = അസത്യം;
    തിരികെ fTextIdx;
}


ശൂന്യം RuleBasedBreakIterator::BreakCache::following(int32_t startPos, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (startPos == fTextIdx || seek(startPos) || populateNear(startPos, status)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->fDone = അസത്യം;
        next();
    }
}


ശൂന്യം RuleBasedBreakIterator::BreakCache::preceding(int32_t startPos, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (startPos == fTextIdx || seek(startPos) || populateNear(startPos, status)) {
        എങ്കിൽ (startPos == fTextIdx) {
            previous(status);
        } അല്ലെങ്കിൽ {
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
ശൂന്യം RuleBasedBreakIterator::BreakCache::nextOL() {
    fBI->fDone = !populateFollowing();
    fBI->fPosition = fTextIdx;
    fBI->fRuleStatusIndex = fStatuses[fBufIdx];
}


ശൂന്യം RuleBasedBreakIterator::BreakCache::previous(UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    int32_t initialBufIdx = fBufIdx;
    എങ്കിൽ (fBufIdx == fStartBufIdx) {
        // At start of cache. Prepend to it.
        populatePreceding(status);
    } അല്ലെങ്കിൽ {
        // Cache already holds the next boundary
        fBufIdx = modChunkSize(fBufIdx - 1);
        fTextIdx = fBoundaries[fBufIdx];
    }
    fBI->fDone = (fBufIdx == initialBufIdx);
    fBI->fPosition = fTextIdx;
    fBI->fRuleStatusIndex = fStatuses[fBufIdx];
}


UBool RuleBasedBreakIterator::BreakCache::seek(int32_t pos) {
    എങ്കിൽ (pos < fBoundaries[fStartBufIdx] || pos > fBoundaries[fEndBufIdx]) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (pos == fBoundaries[fStartBufIdx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = fStartBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        തിരികെ സത്യം;
    }
    എങ്കിൽ (pos == fBoundaries[fEndBufIdx]) {
        fBufIdx = fEndBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        തിരികെ സത്യം;
    }

    int32_t min = fStartBufIdx;
    int32_t max = fEndBufIdx;
    ആകുമ്പോൾ (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_SIZE : 0)) / 2;
        probe = modChunkSize(probe);
        എങ്കിൽ (fBoundaries[probe] > pos) {
            max = probe;
        } അല്ലെങ്കിൽ {
            min = modChunkSize(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = modChunkSize(max - 1);
    fTextIdx = fBoundaries[fBufIdx];
    U_ASSERT(fTextIdx <= pos);
    തിരികെ സത്യം;
}


UBool RuleBasedBreakIterator::BreakCache::populateNear(int32_t position, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
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
    സ്ഥിതം constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t ruleStatusIndex = 0;
    തർക്കം retainCache = അസത്യം;
    എങ്കിൽ ((position > fBoundaries[fStartBufIdx] - CACHE_NEAR) && position < (fBoundaries[fEndBufIdx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = സത്യം;
    } അല്ലെങ്കിൽ എങ്കിൽ (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = അസത്യം;
        aBoundary = 0;
    } അല്ലെങ്കിൽ {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handleSafePrevious(position);

        എങ്കിൽ (fBoundaries[fEndBufIdx] < position && fBoundaries[fEndBufIdx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = സത്യം;
        } അല്ലെങ്കിൽ എങ്കിൽ (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[fStartBufIdx] <= (position + CACHE_NEAR));
        } അല്ലെങ്കിൽ {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = അസത്യം;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handleNext();
            എങ്കിൽ (aBoundary != UBRK_DONE && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_setNativeIndex(&fBI->fText, aBoundary);
                എങ്കിൽ (backupPos == utext_getPreviousNativeIndex(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handleNext();   // Safe rules identify safe pairs.
                }
            }
            എങ്കിൽ (aBoundary == UBRK_DONE) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_nativeLength(&fBI->fText);
            }
            ruleStatusIndex = fBI->fRuleStatusIndex;
        }
    }

    എങ്കിൽ (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, ruleStatusIndex);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    എങ്കിൽ (fBoundaries[fEndBufIdx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        ആകുമ്പോൾ (fBoundaries[fEndBufIdx] < position) {
            എങ്കിൽ (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = fEndBufIdx;                      // Set iterator position to the end of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        ആകുമ്പോൾ (fTextIdx > position) {              // Move backwards to a position at or preceding the requested pos.
            previous(status);
        }
        തിരികെ സത്യം;
    }

    എങ്കിൽ (fBoundaries[fStartBufIdx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        ആകുമ്പോൾ (fBoundaries[fStartBufIdx] > position) {
            populatePreceding(status);
        }
        fBufIdx = fStartBufIdx;                    // Set iterator position to the start of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        ആകുമ്പോൾ (fTextIdx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        എങ്കിൽ (fTextIdx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            previous(status);
        }
        തിരികെ സത്യം;
    }

    U_ASSERT(fTextIdx == position);
    തിരികെ സത്യം;
}



UBool RuleBasedBreakIterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[fEndBufIdx];
    int32_t fromRuleStatusIdx = fStatuses[fEndBufIdx];
    int32_t pos = 0;
    int32_t ruleStatusIdx = 0;

    എങ്കിൽ (fBI->fDictionaryCache->following(fromPosition, &pos, &ruleStatusIdx)) {
        addFollowing(pos, ruleStatusIdx, UpdateCachePosition);
        തിരികെ സത്യം;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handleNext();
    എങ്കിൽ (pos == UBRK_DONE) {
        തിരികെ അസത്യം;
    }

    ruleStatusIdx = fBI->fRuleStatusIndex;
    എങ്കിൽ (fBI->f_Dictionary_Char_എണ്ണം > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, fromRuleStatusIdx, ruleStatusIdx);
        എങ്കിൽ (fBI->fDictionaryCache->following(fromPosition, &pos, &ruleStatusIdx)) {
            addFollowing(pos, ruleStatusIdx, UpdateCachePosition);
            തിരികെ സത്യം;
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
    വേണ്ടി (പൂർണ്ണസംഖ്യ count=0; count<6; ++count) {
        pos = fBI->handleNext();
        എങ്കിൽ (pos == UBRK_DONE || fBI->f_Dictionary_Char_എണ്ണം > 0) {
            നിർത്തുക;
        }
        addFollowing(pos, fBI->fRuleStatusIndex, RetainCachePosition);
    }

    തിരികെ സത്യം;
}


UBool RuleBasedBreakIterator::BreakCache::populatePreceding(UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }

    int32_t fromPosition = fBoundaries[fStartBufIdx];
    എങ്കിൽ (fromPosition == 0) {
        തിരികെ അസത്യം;
    }

    int32_t position = 0;
    int32_t positionStatusIdx = 0;

    എങ്കിൽ (fBI->fDictionaryCache->preceding(fromPosition, &position, &positionStatusIdx)) {
        addPreceding(position, positionStatusIdx, UpdateCachePosition);
        തിരികെ സത്യം;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    ചെയ്യുക {
        backupPosition = backupPosition - 30;
        എങ്കിൽ (backupPosition <= 0) {
            backupPosition = 0;
        } അല്ലെങ്കിൽ {
            backupPosition = fBI->handleSafePrevious(backupPosition);
        }
        എങ്കിൽ (backupPosition == UBRK_DONE || backupPosition == 0) {
            position = 0;
            positionStatusIdx = 0;
        } അല്ലെങ്കിൽ {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handleNext();
            എങ്കിൽ (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_setNativeIndex(&fBI->fText, position);
                എങ്കിൽ (backupPosition == utext_getPreviousNativeIndex(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handleNext();   // Safe rules identify safe pairs.
                }
            }
            positionStatusIdx = fBI->fRuleStatusIndex;
        }
    } ആകുമ്പോൾ (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    fSideBuffer.removeAllElements();
    fSideBuffer.addElement(position, status);
    fSideBuffer.addElement(positionStatusIdx, status);

    ചെയ്യുക {
        int32_t prevPosition = fBI->fPosition = position;
        int32_t prevStatusIdx = positionStatusIdx;
        position = fBI->handleNext();
        positionStatusIdx = fBI->fRuleStatusIndex;
        എങ്കിൽ (position == UBRK_DONE) {
            നിർത്തുക;
        }

        UBool segmentHandledByDictionary = അസത്യം;
        എങ്കിൽ (fBI->f_Dictionary_Char_എണ്ണം != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dictSegEndPosition = position;
            fBI->fDictionaryCache->populateDictionary(prevPosition, dictSegEndPosition, prevStatusIdx, positionStatusIdx);
            ആകുമ്പോൾ (fBI->fDictionaryCache->following(prevPosition, &position, &positionStatusIdx)) {
                segmentHandledByDictionary = സത്യം;
                U_ASSERT(position > prevPosition);
                എങ്കിൽ (position >= fromPosition) {
                    നിർത്തുക;
                }
                U_ASSERT(position <= dictSegEndPosition);
                fSideBuffer.addElement(position, status);
                fSideBuffer.addElement(positionStatusIdx, status);
                prevPosition = position;
            }
            U_ASSERT(position==dictSegEndPosition || position>=fromPosition);
        }

        എങ്കിൽ (!segmentHandledByDictionary && position < fromPosition) {
            fSideBuffer.addElement(position, status);
            fSideBuffer.addElement(positionStatusIdx, status);
        }
    } ആകുമ്പോൾ (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = അസത്യം;
    എങ്കിൽ (!fSideBuffer.isEmpty()) {
        positionStatusIdx = fSideBuffer.popi();
        position = fSideBuffer.popi();
        addPreceding(position, positionStatusIdx, UpdateCachePosition);
        success = സത്യം;
    }

    ആകുമ്പോൾ (!fSideBuffer.isEmpty()) {
        positionStatusIdx = fSideBuffer.popi();
        position = fSideBuffer.popi();
        എങ്കിൽ (!addPreceding(position, positionStatusIdx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            നിർത്തുക;
        }
    }

    തിരികെ success;
}


ശൂന്യം RuleBasedBreakIterator::BreakCache::addFollowing(int32_t position, int32_t ruleStatusIdx, Update_Position_മൂല്യങ്ങൾ update) {
    U_ASSERT(position > fBoundaries[fEndBufIdx]);
    U_ASSERT(ruleStatusIdx <= UINT16_MAX);
    int32_t nextIdx = modChunkSize(fEndBufIdx + 1);
    എങ്കിൽ (nextIdx == fStartBufIdx) {
        fStartBufIdx = modChunkSize(fStartBufIdx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[nextIdx] = position;
    fStatuses[nextIdx] = static_cast<uint16_t>(ruleStatusIdx);
    fEndBufIdx = nextIdx;
    എങ്കിൽ (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = nextIdx;
        fTextIdx = position;
    } അല്ലെങ്കിൽ {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(nextIdx != fBufIdx);
    }
}

തർക്കം RuleBasedBreakIterator::BreakCache::addPreceding(int32_t position, int32_t ruleStatusIdx, Update_Position_മൂല്യങ്ങൾ update) {
    U_ASSERT(position < fBoundaries[fStartBufIdx]);
    U_ASSERT(ruleStatusIdx <= UINT16_MAX);
    int32_t nextIdx = modChunkSize(fStartBufIdx - 1);
    എങ്കിൽ (nextIdx == fEndBufIdx) {
        എങ്കിൽ (fBufIdx == fEndBufIdx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            തിരികെ അസത്യം;
        }
        fEndBufIdx = modChunkSize(fEndBufIdx - 1);
    }
    fBoundaries[nextIdx] = position;
    fStatuses[nextIdx] = static_cast<uint16_t>(ruleStatusIdx);
    fStartBufIdx = nextIdx;
    എങ്കിൽ (update == UpdateCachePosition) {
        fBufIdx = nextIdx;
        fTextIdx = position;
    }
    തിരികെ സത്യം;
}


ശൂന്യം RuleBasedBreakIterator::BreakCache::dumpCache() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", fTextIdx, fBufIdx);
    വേണ്ടി (int32_t i=fStartBufIdx; ; i=modChunkSize(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        എങ്കിൽ (i == fEndBufIdx) {
            നിർത്തുക;
        }
    }
#endif
}

U_NAMESPACE_END

#endif // #if !UCONFIG_NO_BREAK_ITERATION
