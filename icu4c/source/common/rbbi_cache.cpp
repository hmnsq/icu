// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/ubrk.h"
#समावेश "unicode/rbbi.h"

#समावेश "rbbi_cache.h"

#समावेश "brkeng.h"
#समावेश "cmemory.h"
#समावेश "rbbidata.h"
#समावेश "rbbirb.h"
#समावेश "uassert.h"
#समावेश "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

RuleBasedBreakIterator::DictionaryCache::DictionaryCache(RuleBasedBreakIterator *bi, UErrorCode &status) :
        fBI(bi), fBreaks(status), fPositionInCache(-1),
        fStart(0), fLimit(0), f_First_Rule_Status_सूचकांक(0), f_Other_Rule_Status_सूचकांक(0) {
}

RuleBasedBreakIterator::DictionaryCache::~DictionaryCache() {
}

व्योम RuleBasedBreakIterator::DictionaryCache::reset() {
    fPositionInCache = -1;
    fStart = 0;
    fLimit = 0;
    f_First_Rule_Status_सूचकांक = 0;
    f_Other_Rule_Status_सूचकांक = 0;
    fBreaks.removeAllElements();
    int32_t maxStatus = 0;
    क्रम (int32_t i = 0; i < fBI->f_तथ्य->fStatusMaxIdx; ++i) {
        maxStatus = std::max(maxStatus, fBI->f_तथ्य->fRuleStatusTable[i]);
    }
    isWordLike = maxStatus > 100;
}

UBool RuleBasedBreakIterator::DictionaryCache::following(int32_t fromPos, int32_t *परिणाम, int32_t *status_सूचकांक) {
    अगर (fromPos >= fLimit || fromPos < fStart) {
        fPositionInCache = -1;
        वापस false;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    अगर (fPositionInCache >= 0 && fPositionInCache < fBreaks.size() && fBreaks.elementAti(fPositionInCache) == fromPos) {
        ++fPositionInCache;
        अगर (fPositionInCache >= fBreaks.size()) {
            fPositionInCache = -1;
            वापस false;
        }
        r = fBreaks.elementAti(fPositionInCache);
        U_ASSERT(r > fromPos);
        *परिणाम = r;
        *status_सूचकांक = isWordLike || r == fLimit ? f_Other_Rule_Status_सूचकांक : 0;
        वापस true;
    }

    // Random indexing. Linear search for the boundary following the given position.

    क्रम (fPositionInCache = 0; fPositionInCache < fBreaks.size(); ++fPositionInCache) {
        r= fBreaks.elementAti(fPositionInCache);
        अगर (r > fromPos) {
            *परिणाम = r;
            *status_सूचकांक = isWordLike || r == fLimit ? f_Other_Rule_Status_सूचकांक : 0;
            वापस true;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool RuleBasedBreakIterator::DictionaryCache::preceding(int32_t fromPos, int32_t *परिणाम, int32_t *status_सूचकांक) {
    अगर (fromPos <= fStart || fromPos > fLimit) {
        fPositionInCache = -1;
        वापस false;
    }

    अगर (fromPos == fLimit) {
        fPositionInCache = fBreaks.size() - 1;
        अगर (fPositionInCache >= 0) {
            U_ASSERT(fBreaks.elementAti(fPositionInCache) == fromPos);
        }
    }

    int32_t r;
    अगर (fPositionInCache > 0 && fPositionInCache < fBreaks.size() && fBreaks.elementAti(fPositionInCache) == fromPos) {
        --fPositionInCache;
        r = fBreaks.elementAti(fPositionInCache);
        U_ASSERT(r < fromPos);
        *परिणाम = r;
        *status_सूचकांक = (r == fStart) ? f_First_Rule_Status_सूचकांक
                       : isWordLike  ? f_Other_Rule_Status_सूचकांक
                                     : 0;
        वापस true;
    }

    अगर (fPositionInCache == 0) {
        fPositionInCache = -1;
        वापस false;
    }

    क्रम (fPositionInCache = fBreaks.size()-1; fPositionInCache >= 0; --fPositionInCache) {
        r = fBreaks.elementAti(fPositionInCache);
        अगर (r < fromPos) {
            *परिणाम = r;
            *status_सूचकांक = (r == fStart) ? f_First_Rule_Status_सूचकांक
                           : isWordLike  ? f_Other_Rule_Status_सूचकांक
                                         : 0;
            वापस true;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

व्योम RuleBasedBreakIterator::DictionaryCache::populateDictionary(int32_t startPos, int32_t endPos,
                                       int32_t firstRuleStatus, int32_t otherRuleStatus) {
    अगर ((endPos - startPos) <= 1) {
        वापस;
    }

    reset();
    f_First_Rule_Status_सूचकांक = firstRuleStatus;
    f_Other_Rule_Status_सूचकांक = otherRuleStatus;

    int32_t rangeStart = startPos;
    int32_t rangeEnd = endPos;

    uint16_t    category;
    int32_t     current;
    UErrorCode  status = U_ZERO_ERROR;
    int32_t     found_Break_गिनती = 0;
    UText      *text = &fBI->fText;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_set_Native_सूचकांक(text, rangeStart);
    UChar32     c = utext_current32(text);
    category = ucptrie_get(fBI->f_तथ्य->fTrie, c);
    uint32_t dictStart = fBI->f_तथ्य->fForwardTable->fDictCategoriesStart;

    जबतक(U_SUCCESS(status)) {
        जबतक ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(text))) < rangeEnd
                && (category < dictStart)) {
            utext_next32(text);           // TODO: cleaner loop structure.
            c = utext_current32(text);
            category = ucptrie_get(fBI->f_तथ्य->fTrie, c);
        }
        अगर (current >= rangeEnd) {
            अवरोध;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        स्थिर LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, status));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        अगर (lbe != nullptr) {
            found_Break_गिनती += lbe->findBreaks(text, current, rangeEnd, fBreaks, fBI->fIsPhraseBreaking, status);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(text);
        category = ucptrie_get(fBI->f_तथ्य->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    अगर (found_Break_गिनती > 0) {
        U_ASSERT(found_Break_गिनती == fBreaks.size());
        अगर (startPos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.जोड़ो_Element_At(startPos, 0, status);
        }
        अगर (endPos > fBreaks.peeki()) {
            fBreaks.push(endPos, status);
        }
        fPositionInCache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        fStart = fBreaks.elementAti(0);
        fLimit = fBreaks.peeki();
    } अन्यथा {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

RuleBasedBreakIterator::BreakCache::BreakCache(RuleBasedBreakIterator *bi, UErrorCode &status) :
        fBI(bi), f_Side_बफर(status) {
    reset();
}


RuleBasedBreakIterator::BreakCache::~BreakCache() {
}


व्योम RuleBasedBreakIterator::BreakCache::reset(int32_t pos, int32_t ruleStatus) {
    fStartBufIdx = 0;
    fEndBufIdx = 0;
    fTextIdx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(ruleStatus);
}


int32_t  RuleBasedBreakIterator::BreakCache::current() {
    fBI->fPosition = fTextIdx;
    fBI->f_Rule_Status_सूचकांक = fStatuses[fBufIdx];
    fBI->fDone = false;
    वापस fTextIdx;
}


व्योम RuleBasedBreakIterator::BreakCache::following(int32_t startPos, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (startPos == fTextIdx || seek(startPos) || populateNear(startPos, status)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->fDone = false;
        next();
    }
}


व्योम RuleBasedBreakIterator::BreakCache::preceding(int32_t startPos, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (startPos == fTextIdx || seek(startPos) || populateNear(startPos, status)) {
        अगर (startPos == fTextIdx) {
            previous(status);
        } अन्यथा {
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
व्योम RuleBasedBreakIterator::BreakCache::अग्र_OL() {
    fBI->fDone = !populateFollowing();
    fBI->fPosition = fTextIdx;
    fBI->f_Rule_Status_सूचकांक = fStatuses[fBufIdx];
}


व्योम RuleBasedBreakIterator::BreakCache::previous(UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    int32_t initialBufIdx = fBufIdx;
    अगर (fBufIdx == fStartBufIdx) {
        // At start of cache. Prepend to it.
        populatePreceding(status);
    } अन्यथा {
        // Cache already holds the next boundary
        fBufIdx = mod_Chunk_आकार(fBufIdx - 1);
        fTextIdx = fBoundaries[fBufIdx];
    }
    fBI->fDone = (fBufIdx == initialBufIdx);
    fBI->fPosition = fTextIdx;
    fBI->f_Rule_Status_सूचकांक = fStatuses[fBufIdx];
}


UBool RuleBasedBreakIterator::BreakCache::seek(int32_t pos) {
    अगर (pos < fBoundaries[fStartBufIdx] || pos > fBoundaries[fEndBufIdx]) {
        वापस false;
    }
    अगर (pos == fBoundaries[fStartBufIdx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = fStartBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        वापस true;
    }
    अगर (pos == fBoundaries[fEndBufIdx]) {
        fBufIdx = fEndBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        वापस true;
    }

    int32_t min = fStartBufIdx;
    int32_t max = fEndBufIdx;
    जबतक (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_आकार : 0)) / 2;
        probe = mod_Chunk_आकार(probe);
        अगर (fBoundaries[probe] > pos) {
            max = probe;
        } अन्यथा {
            min = mod_Chunk_आकार(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = mod_Chunk_आकार(max - 1);
    fTextIdx = fBoundaries[fBufIdx];
    U_ASSERT(fTextIdx <= pos);
    वापस true;
}


UBool RuleBasedBreakIterator::BreakCache::populateNear(int32_t position, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस false;
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
    अटल constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t rule_Status_सूचकांक = 0;
    bool retainCache = false;
    अगर ((position > fBoundaries[fStartBufIdx] - CACHE_NEAR) && position < (fBoundaries[fEndBufIdx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = true;
    } अन्यथा अगर (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = false;
        aBoundary = 0;
    } अन्यथा {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handleSafePrevious(position);

        अगर (fBoundaries[fEndBufIdx] < position && fBoundaries[fEndBufIdx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = true;
        } अन्यथा अगर (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[fStartBufIdx] <= (position + CACHE_NEAR));
        } अन्यथा {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = false;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handle_अग्र();
            अगर (aBoundary != UBRK_DONE && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_set_Native_सूचकांक(&fBI->fText, aBoundary);
                अगर (backupPos == utext_get_Previous_Native_सूचकांक(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handle_अग्र();   // Safe rules identify safe pairs.
                }
            }
            अगर (aBoundary == UBRK_DONE) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_native_लंबाई(&fBI->fText);
            }
            rule_Status_सूचकांक = fBI->f_Rule_Status_सूचकांक;
        }
    }

    अगर (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, rule_Status_सूचकांक);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    अगर (fBoundaries[fEndBufIdx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        जबतक (fBoundaries[fEndBufIdx] < position) {
            अगर (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = fEndBufIdx;                      // Set iterator position to the end of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        जबतक (fTextIdx > position) {              // Move backwards to a position at or preceding the requested pos.
            previous(status);
        }
        वापस true;
    }

    अगर (fBoundaries[fStartBufIdx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        जबतक (fBoundaries[fStartBufIdx] > position) {
            populatePreceding(status);
        }
        fBufIdx = fStartBufIdx;                    // Set iterator position to the start of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        जबतक (fTextIdx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        अगर (fTextIdx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            previous(status);
        }
        वापस true;
    }

    U_ASSERT(fTextIdx == position);
    वापस true;
}



UBool RuleBasedBreakIterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[fEndBufIdx];
    int32_t fromRuleStatusIdx = fStatuses[fEndBufIdx];
    int32_t pos = 0;
    int32_t ruleStatusIdx = 0;

    अगर (fBI->fDictionaryCache->following(fromPosition, &pos, &ruleStatusIdx)) {
        addFollowing(pos, ruleStatusIdx, UpdateCachePosition);
        वापस true;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handle_अग्र();
    अगर (pos == UBRK_DONE) {
        वापस false;
    }

    ruleStatusIdx = fBI->f_Rule_Status_सूचकांक;
    अगर (fBI->f_Dictionary_Char_गिनती > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, fromRuleStatusIdx, ruleStatusIdx);
        अगर (fBI->fDictionaryCache->following(fromPosition, &pos, &ruleStatusIdx)) {
            addFollowing(pos, ruleStatusIdx, UpdateCachePosition);
            वापस true;
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
    क्रम (पूर्णांक count=0; count<6; ++count) {
        pos = fBI->handle_अग्र();
        अगर (pos == UBRK_DONE || fBI->f_Dictionary_Char_गिनती > 0) {
            अवरोध;
        }
        addFollowing(pos, fBI->f_Rule_Status_सूचकांक, RetainCachePosition);
    }

    वापस true;
}


UBool RuleBasedBreakIterator::BreakCache::populatePreceding(UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस false;
    }

    int32_t fromPosition = fBoundaries[fStartBufIdx];
    अगर (fromPosition == 0) {
        वापस false;
    }

    int32_t position = 0;
    int32_t positionStatusIdx = 0;

    अगर (fBI->fDictionaryCache->preceding(fromPosition, &position, &positionStatusIdx)) {
        addPreceding(position, positionStatusIdx, UpdateCachePosition);
        वापस true;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    करो {
        backupPosition = backupPosition - 30;
        अगर (backupPosition <= 0) {
            backupPosition = 0;
        } अन्यथा {
            backupPosition = fBI->handleSafePrevious(backupPosition);
        }
        अगर (backupPosition == UBRK_DONE || backupPosition == 0) {
            position = 0;
            positionStatusIdx = 0;
        } अन्यथा {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handle_अग्र();
            अगर (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_set_Native_सूचकांक(&fBI->fText, position);
                अगर (backupPosition == utext_get_Previous_Native_सूचकांक(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handle_अग्र();   // Safe rules identify safe pairs.
                }
            }
            positionStatusIdx = fBI->f_Rule_Status_सूचकांक;
        }
    } जबतक (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    f_Side_बफर.removeAllElements();
    f_Side_बफर.addElement(position, status);
    f_Side_बफर.addElement(positionStatusIdx, status);

    करो {
        int32_t पूर्व_Position = fBI->fPosition = position;
        int32_t पूर्व_Status_Idx = positionStatusIdx;
        position = fBI->handle_अग्र();
        positionStatusIdx = fBI->f_Rule_Status_सूचकांक;
        अगर (position == UBRK_DONE) {
            अवरोध;
        }

        UBool segmentHandledByDictionary = false;
        अगर (fBI->f_Dictionary_Char_गिनती != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dictSegEndPosition = position;
            fBI->fDictionaryCache->populateDictionary(पूर्व_Position, dictSegEndPosition, पूर्व_Status_Idx, positionStatusIdx);
            जबतक (fBI->fDictionaryCache->following(पूर्व_Position, &position, &positionStatusIdx)) {
                segmentHandledByDictionary = true;
                U_ASSERT(position > पूर्व_Position);
                अगर (position >= fromPosition) {
                    अवरोध;
                }
                U_ASSERT(position <= dictSegEndPosition);
                f_Side_बफर.addElement(position, status);
                f_Side_बफर.addElement(positionStatusIdx, status);
                पूर्व_Position = position;
            }
            U_ASSERT(position==dictSegEndPosition || position>=fromPosition);
        }

        अगर (!segmentHandledByDictionary && position < fromPosition) {
            f_Side_बफर.addElement(position, status);
            f_Side_बफर.addElement(positionStatusIdx, status);
        }
    } जबतक (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = false;
    अगर (!f_Side_बफर.isEmpty()) {
        positionStatusIdx = f_Side_बफर.popi();
        position = f_Side_बफर.popi();
        addPreceding(position, positionStatusIdx, UpdateCachePosition);
        success = true;
    }

    जबतक (!f_Side_बफर.isEmpty()) {
        positionStatusIdx = f_Side_बफर.popi();
        position = f_Side_बफर.popi();
        अगर (!addPreceding(position, positionStatusIdx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            अवरोध;
        }
    }

    वापस success;
}


व्योम RuleBasedBreakIterator::BreakCache::addFollowing(int32_t position, int32_t ruleStatusIdx, Update_Position_मूल्यावली update) {
    U_ASSERT(position > fBoundaries[fEndBufIdx]);
    U_ASSERT(ruleStatusIdx <= UINT16_MAX);
    int32_t अग्र_Idx = mod_Chunk_आकार(fEndBufIdx + 1);
    अगर (अग्र_Idx == fStartBufIdx) {
        fStartBufIdx = mod_Chunk_आकार(fStartBufIdx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[अग्र_Idx] = position;
    fStatuses[अग्र_Idx] = static_cast<uint16_t>(ruleStatusIdx);
    fEndBufIdx = अग्र_Idx;
    अगर (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = अग्र_Idx;
        fTextIdx = position;
    } अन्यथा {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(अग्र_Idx != fBufIdx);
    }
}

bool RuleBasedBreakIterator::BreakCache::addPreceding(int32_t position, int32_t ruleStatusIdx, Update_Position_मूल्यावली update) {
    U_ASSERT(position < fBoundaries[fStartBufIdx]);
    U_ASSERT(ruleStatusIdx <= UINT16_MAX);
    int32_t अग्र_Idx = mod_Chunk_आकार(fStartBufIdx - 1);
    अगर (अग्र_Idx == fEndBufIdx) {
        अगर (fBufIdx == fEndBufIdx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            वापस false;
        }
        fEndBufIdx = mod_Chunk_आकार(fEndBufIdx - 1);
    }
    fBoundaries[अग्र_Idx] = position;
    fStatuses[अग्र_Idx] = static_cast<uint16_t>(ruleStatusIdx);
    fStartBufIdx = अग्र_Idx;
    अगर (update == UpdateCachePosition) {
        fBufIdx = अग्र_Idx;
        fTextIdx = position;
    }
    वापस true;
}


व्योम RuleBasedBreakIterator::BreakCache::dumpCache() {
#अगर_घोषित RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", fTextIdx, fBufIdx);
    क्रम (int32_t i=fStartBufIdx; ; i=mod_Chunk_आकार(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        अगर (i == fEndBufIdx) {
            अवरोध;
        }
    }
#पूर्ण_अगर
}

U_NAMESPACE_END

#पूर्ण_अगर // #if !UCONFIG_NO_BREAK_ITERATION
