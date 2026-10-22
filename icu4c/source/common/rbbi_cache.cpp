// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/ubrk.h"
#অন্তর্ভাব "unicode/rbbi.h"

#অন্তর্ভাব "rbbi_cache.h"

#অন্তর্ভাব "brkeng.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "rbbidata.h"
#অন্তর্ভাব "rbbirb.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

RuleBasedBreakIterator::DictionaryCache::DictionaryCache(RuleBasedBreakIterator *bi, UErrorCode &status) :
        fBI(bi), fBreaks(status), fPositionInCache(-1),
        fStart(0), fLimit(0), f_First_Rule_Status_সূচক(0), f_Other_Rule_Status_সূচক(0) {
}

RuleBasedBreakIterator::DictionaryCache::~DictionaryCache() {
}

শূন্য RuleBasedBreakIterator::DictionaryCache::reset() {
    fPositionInCache = -1;
    fStart = 0;
    fLimit = 0;
    f_First_Rule_Status_সূচক = 0;
    f_Other_Rule_Status_সূচক = 0;
    fBreaks.removeAllElements();
    int32_t maxStatus = 0;
    জন্য (int32_t i = 0; i < fBI->f_তথ্য->fStatusMaxIdx; ++i) {
        maxStatus = std::max(maxStatus, fBI->f_তথ্য->fRuleStatusTable[i]);
    }
    isWordLike = maxStatus > 100;
}

UBool RuleBasedBreakIterator::DictionaryCache::following(int32_t fromPos, int32_t *ফলাফল, int32_t *status_সূচক) {
    যদি (fromPos >= fLimit || fromPos < fStart) {
        fPositionInCache = -1;
        ফেরত মিথ্যা;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    যদি (fPositionInCache >= 0 && fPositionInCache < fBreaks.size() && fBreaks.elementAti(fPositionInCache) == fromPos) {
        ++fPositionInCache;
        যদি (fPositionInCache >= fBreaks.size()) {
            fPositionInCache = -1;
            ফেরত মিথ্যা;
        }
        r = fBreaks.elementAti(fPositionInCache);
        U_ASSERT(r > fromPos);
        *ফলাফল = r;
        *status_সূচক = isWordLike || r == fLimit ? f_Other_Rule_Status_সূচক : 0;
        ফেরত সত্য;
    }

    // Random indexing. Linear search for the boundary following the given position.

    জন্য (fPositionInCache = 0; fPositionInCache < fBreaks.size(); ++fPositionInCache) {
        r= fBreaks.elementAti(fPositionInCache);
        যদি (r > fromPos) {
            *ফলাফল = r;
            *status_সূচক = isWordLike || r == fLimit ? f_Other_Rule_Status_সূচক : 0;
            ফেরত সত্য;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool RuleBasedBreakIterator::DictionaryCache::preceding(int32_t fromPos, int32_t *ফলাফল, int32_t *status_সূচক) {
    যদি (fromPos <= fStart || fromPos > fLimit) {
        fPositionInCache = -1;
        ফেরত মিথ্যা;
    }

    যদি (fromPos == fLimit) {
        fPositionInCache = fBreaks.size() - 1;
        যদি (fPositionInCache >= 0) {
            U_ASSERT(fBreaks.elementAti(fPositionInCache) == fromPos);
        }
    }

    int32_t r;
    যদি (fPositionInCache > 0 && fPositionInCache < fBreaks.size() && fBreaks.elementAti(fPositionInCache) == fromPos) {
        --fPositionInCache;
        r = fBreaks.elementAti(fPositionInCache);
        U_ASSERT(r < fromPos);
        *ফলাফল = r;
        *status_সূচক = (r == fStart) ? f_First_Rule_Status_সূচক
                       : isWordLike  ? f_Other_Rule_Status_সূচক
                                     : 0;
        ফেরত সত্য;
    }

    যদি (fPositionInCache == 0) {
        fPositionInCache = -1;
        ফেরত মিথ্যা;
    }

    জন্য (fPositionInCache = fBreaks.size()-1; fPositionInCache >= 0; --fPositionInCache) {
        r = fBreaks.elementAti(fPositionInCache);
        যদি (r < fromPos) {
            *ফলাফল = r;
            *status_সূচক = (r == fStart) ? f_First_Rule_Status_সূচক
                           : isWordLike  ? f_Other_Rule_Status_সূচক
                                         : 0;
            ফেরত সত্য;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

শূন্য RuleBasedBreakIterator::DictionaryCache::populateDictionary(int32_t startPos, int32_t endPos,
                                       int32_t firstRuleStatus, int32_t otherRuleStatus) {
    যদি ((endPos - startPos) <= 1) {
        ফেরত;
    }

    reset();
    f_First_Rule_Status_সূচক = firstRuleStatus;
    f_Other_Rule_Status_সূচক = otherRuleStatus;

    int32_t rangeStart = startPos;
    int32_t rangeEnd = endPos;

    uint16_t    category;
    int32_t     current;
    UErrorCode  status = U_ZERO_ERROR;
    int32_t     found_Break_গুনতি = 0;
    UText      *text = &fBI->fText;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_set_Native_সূচক(text, rangeStart);
    UChar32     c = utext_current32(text);
    category = ucptrie_get(fBI->f_তথ্য->fTrie, c);
    uint32_t dictStart = fBI->f_তথ্য->fForwardTable->fDictCategoriesStart;

    যখন(U_SUCCESS(status)) {
        যখন ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(text))) < rangeEnd
                && (category < dictStart)) {
            utext_next32(text);           // TODO: cleaner loop structure.
            c = utext_current32(text);
            category = ucptrie_get(fBI->f_তথ্য->fTrie, c);
        }
        যদি (current >= rangeEnd) {
            বিরতি;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        ধ্রুবক LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, status));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        যদি (lbe != nullptr) {
            found_Break_গুনতি += lbe->findBreaks(text, current, rangeEnd, fBreaks, fBI->fIsPhraseBreaking, status);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(text);
        category = ucptrie_get(fBI->f_তথ্য->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    যদি (found_Break_গুনতি > 0) {
        U_ASSERT(found_Break_গুনতি == fBreaks.size());
        যদি (startPos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.ঢোকাও_Element_At(startPos, 0, status);
        }
        যদি (endPos > fBreaks.peeki()) {
            fBreaks.push(endPos, status);
        }
        fPositionInCache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        fStart = fBreaks.elementAti(0);
        fLimit = fBreaks.peeki();
    } অন্যথা {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

RuleBasedBreakIterator::BreakCache::BreakCache(RuleBasedBreakIterator *bi, UErrorCode &status) :
        fBI(bi), f_Side_বাফার(status) {
    reset();
}


RuleBasedBreakIterator::BreakCache::~BreakCache() {
}


শূন্য RuleBasedBreakIterator::BreakCache::reset(int32_t pos, int32_t ruleStatus) {
    fStartBufIdx = 0;
    fEndBufIdx = 0;
    fTextIdx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(ruleStatus);
}


int32_t  RuleBasedBreakIterator::BreakCache::current() {
    fBI->fPosition = fTextIdx;
    fBI->f_Rule_Status_সূচক = fStatuses[fBufIdx];
    fBI->fDone = মিথ্যা;
    ফেরত fTextIdx;
}


শূন্য RuleBasedBreakIterator::BreakCache::following(int32_t startPos, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (startPos == fTextIdx || seek(startPos) || populateNear(startPos, status)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->fDone = মিথ্যা;
        next();
    }
}


শূন্য RuleBasedBreakIterator::BreakCache::preceding(int32_t startPos, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (startPos == fTextIdx || seek(startPos) || populateNear(startPos, status)) {
        যদি (startPos == fTextIdx) {
            previous(status);
        } অন্যথা {
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
শূন্য RuleBasedBreakIterator::BreakCache::পরবর্তী_OL() {
    fBI->fDone = !populateFollowing();
    fBI->fPosition = fTextIdx;
    fBI->f_Rule_Status_সূচক = fStatuses[fBufIdx];
}


শূন্য RuleBasedBreakIterator::BreakCache::previous(UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    int32_t initialBufIdx = fBufIdx;
    যদি (fBufIdx == fStartBufIdx) {
        // At start of cache. Prepend to it.
        populatePreceding(status);
    } অন্যথা {
        // Cache already holds the next boundary
        fBufIdx = mod_Chunk_আকার(fBufIdx - 1);
        fTextIdx = fBoundaries[fBufIdx];
    }
    fBI->fDone = (fBufIdx == initialBufIdx);
    fBI->fPosition = fTextIdx;
    fBI->f_Rule_Status_সূচক = fStatuses[fBufIdx];
}


UBool RuleBasedBreakIterator::BreakCache::seek(int32_t pos) {
    যদি (pos < fBoundaries[fStartBufIdx] || pos > fBoundaries[fEndBufIdx]) {
        ফেরত মিথ্যা;
    }
    যদি (pos == fBoundaries[fStartBufIdx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = fStartBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        ফেরত সত্য;
    }
    যদি (pos == fBoundaries[fEndBufIdx]) {
        fBufIdx = fEndBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        ফেরত সত্য;
    }

    int32_t min = fStartBufIdx;
    int32_t max = fEndBufIdx;
    যখন (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_আকার : 0)) / 2;
        probe = mod_Chunk_আকার(probe);
        যদি (fBoundaries[probe] > pos) {
            max = probe;
        } অন্যথা {
            min = mod_Chunk_আকার(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = mod_Chunk_আকার(max - 1);
    fTextIdx = fBoundaries[fBufIdx];
    U_ASSERT(fTextIdx <= pos);
    ফেরত সত্য;
}


UBool RuleBasedBreakIterator::BreakCache::populateNear(int32_t position, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
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
    স্থির constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t rule_Status_সূচক = 0;
    যৌক্তিক retainCache = মিথ্যা;
    যদি ((position > fBoundaries[fStartBufIdx] - CACHE_NEAR) && position < (fBoundaries[fEndBufIdx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = সত্য;
    } অন্যথা যদি (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = মিথ্যা;
        aBoundary = 0;
    } অন্যথা {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handleSafePrevious(position);

        যদি (fBoundaries[fEndBufIdx] < position && fBoundaries[fEndBufIdx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = সত্য;
        } অন্যথা যদি (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[fStartBufIdx] <= (position + CACHE_NEAR));
        } অন্যথা {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = মিথ্যা;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handle_পরবর্তী();
            যদি (aBoundary != UBRK_DONE && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_set_Native_সূচক(&fBI->fText, aBoundary);
                যদি (backupPos == utext_get_Previous_Native_সূচক(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handle_পরবর্তী();   // Safe rules identify safe pairs.
                }
            }
            যদি (aBoundary == UBRK_DONE) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_native_দৈর্ঘ্য(&fBI->fText);
            }
            rule_Status_সূচক = fBI->f_Rule_Status_সূচক;
        }
    }

    যদি (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, rule_Status_সূচক);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    যদি (fBoundaries[fEndBufIdx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        যখন (fBoundaries[fEndBufIdx] < position) {
            যদি (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = fEndBufIdx;                      // Set iterator position to the end of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        যখন (fTextIdx > position) {              // Move backwards to a position at or preceding the requested pos.
            previous(status);
        }
        ফেরত সত্য;
    }

    যদি (fBoundaries[fStartBufIdx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        যখন (fBoundaries[fStartBufIdx] > position) {
            populatePreceding(status);
        }
        fBufIdx = fStartBufIdx;                    // Set iterator position to the start of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        যখন (fTextIdx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        যদি (fTextIdx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            previous(status);
        }
        ফেরত সত্য;
    }

    U_ASSERT(fTextIdx == position);
    ফেরত সত্য;
}



UBool RuleBasedBreakIterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[fEndBufIdx];
    int32_t fromRuleStatusIdx = fStatuses[fEndBufIdx];
    int32_t pos = 0;
    int32_t ruleStatusIdx = 0;

    যদি (fBI->fDictionaryCache->following(fromPosition, &pos, &ruleStatusIdx)) {
        addFollowing(pos, ruleStatusIdx, UpdateCachePosition);
        ফেরত সত্য;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handle_পরবর্তী();
    যদি (pos == UBRK_DONE) {
        ফেরত মিথ্যা;
    }

    ruleStatusIdx = fBI->f_Rule_Status_সূচক;
    যদি (fBI->f_Dictionary_Char_গুনতি > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, fromRuleStatusIdx, ruleStatusIdx);
        যদি (fBI->fDictionaryCache->following(fromPosition, &pos, &ruleStatusIdx)) {
            addFollowing(pos, ruleStatusIdx, UpdateCachePosition);
            ফেরত সত্য;
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
    জন্য (পূর্ণসংখ্যা count=0; count<6; ++count) {
        pos = fBI->handle_পরবর্তী();
        যদি (pos == UBRK_DONE || fBI->f_Dictionary_Char_গুনতি > 0) {
            বিরতি;
        }
        addFollowing(pos, fBI->f_Rule_Status_সূচক, RetainCachePosition);
    }

    ফেরত সত্য;
}


UBool RuleBasedBreakIterator::BreakCache::populatePreceding(UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }

    int32_t fromPosition = fBoundaries[fStartBufIdx];
    যদি (fromPosition == 0) {
        ফেরত মিথ্যা;
    }

    int32_t position = 0;
    int32_t positionStatusIdx = 0;

    যদি (fBI->fDictionaryCache->preceding(fromPosition, &position, &positionStatusIdx)) {
        addPreceding(position, positionStatusIdx, UpdateCachePosition);
        ফেরত সত্য;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    করো {
        backupPosition = backupPosition - 30;
        যদি (backupPosition <= 0) {
            backupPosition = 0;
        } অন্যথা {
            backupPosition = fBI->handleSafePrevious(backupPosition);
        }
        যদি (backupPosition == UBRK_DONE || backupPosition == 0) {
            position = 0;
            positionStatusIdx = 0;
        } অন্যথা {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handle_পরবর্তী();
            যদি (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_set_Native_সূচক(&fBI->fText, position);
                যদি (backupPosition == utext_get_Previous_Native_সূচক(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handle_পরবর্তী();   // Safe rules identify safe pairs.
                }
            }
            positionStatusIdx = fBI->f_Rule_Status_সূচক;
        }
    } যখন (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    f_Side_বাফার.removeAllElements();
    f_Side_বাফার.addElement(position, status);
    f_Side_বাফার.addElement(positionStatusIdx, status);

    করো {
        int32_t পূর্ব_Position = fBI->fPosition = position;
        int32_t পূর্ব_Status_Idx = positionStatusIdx;
        position = fBI->handle_পরবর্তী();
        positionStatusIdx = fBI->f_Rule_Status_সূচক;
        যদি (position == UBRK_DONE) {
            বিরতি;
        }

        UBool segmentHandledByDictionary = মিথ্যা;
        যদি (fBI->f_Dictionary_Char_গুনতি != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dictSegEndPosition = position;
            fBI->fDictionaryCache->populateDictionary(পূর্ব_Position, dictSegEndPosition, পূর্ব_Status_Idx, positionStatusIdx);
            যখন (fBI->fDictionaryCache->following(পূর্ব_Position, &position, &positionStatusIdx)) {
                segmentHandledByDictionary = সত্য;
                U_ASSERT(position > পূর্ব_Position);
                যদি (position >= fromPosition) {
                    বিরতি;
                }
                U_ASSERT(position <= dictSegEndPosition);
                f_Side_বাফার.addElement(position, status);
                f_Side_বাফার.addElement(positionStatusIdx, status);
                পূর্ব_Position = position;
            }
            U_ASSERT(position==dictSegEndPosition || position>=fromPosition);
        }

        যদি (!segmentHandledByDictionary && position < fromPosition) {
            f_Side_বাফার.addElement(position, status);
            f_Side_বাফার.addElement(positionStatusIdx, status);
        }
    } যখন (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = মিথ্যা;
    যদি (!f_Side_বাফার.isEmpty()) {
        positionStatusIdx = f_Side_বাফার.popi();
        position = f_Side_বাফার.popi();
        addPreceding(position, positionStatusIdx, UpdateCachePosition);
        success = সত্য;
    }

    যখন (!f_Side_বাফার.isEmpty()) {
        positionStatusIdx = f_Side_বাফার.popi();
        position = f_Side_বাফার.popi();
        যদি (!addPreceding(position, positionStatusIdx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            বিরতি;
        }
    }

    ফেরত success;
}


শূন্য RuleBasedBreakIterator::BreakCache::addFollowing(int32_t position, int32_t ruleStatusIdx, Update_Position_মানসমূহ update) {
    U_ASSERT(position > fBoundaries[fEndBufIdx]);
    U_ASSERT(ruleStatusIdx <= UINT16_MAX);
    int32_t পরবর্তী_Idx = mod_Chunk_আকার(fEndBufIdx + 1);
    যদি (পরবর্তী_Idx == fStartBufIdx) {
        fStartBufIdx = mod_Chunk_আকার(fStartBufIdx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[পরবর্তী_Idx] = position;
    fStatuses[পরবর্তী_Idx] = static_cast<uint16_t>(ruleStatusIdx);
    fEndBufIdx = পরবর্তী_Idx;
    যদি (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = পরবর্তী_Idx;
        fTextIdx = position;
    } অন্যথা {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(পরবর্তী_Idx != fBufIdx);
    }
}

যৌক্তিক RuleBasedBreakIterator::BreakCache::addPreceding(int32_t position, int32_t ruleStatusIdx, Update_Position_মানসমূহ update) {
    U_ASSERT(position < fBoundaries[fStartBufIdx]);
    U_ASSERT(ruleStatusIdx <= UINT16_MAX);
    int32_t পরবর্তী_Idx = mod_Chunk_আকার(fStartBufIdx - 1);
    যদি (পরবর্তী_Idx == fEndBufIdx) {
        যদি (fBufIdx == fEndBufIdx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            ফেরত মিথ্যা;
        }
        fEndBufIdx = mod_Chunk_আকার(fEndBufIdx - 1);
    }
    fBoundaries[পরবর্তী_Idx] = position;
    fStatuses[পরবর্তী_Idx] = static_cast<uint16_t>(ruleStatusIdx);
    fStartBufIdx = পরবর্তী_Idx;
    যদি (update == UpdateCachePosition) {
        fBufIdx = পরবর্তী_Idx;
        fTextIdx = position;
    }
    ফেরত সত্য;
}


শূন্য RuleBasedBreakIterator::BreakCache::dumpCache() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", fTextIdx, fBufIdx);
    জন্য (int32_t i=fStartBufIdx; ; i=mod_Chunk_আকার(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        যদি (i == fEndBufIdx) {
            বিরতি;
        }
    }
#endif
}

U_NAMESPACE_END

#endif // #if !UCONFIG_NO_BREAK_ITERATION
