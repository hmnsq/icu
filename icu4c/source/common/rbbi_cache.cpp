// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/rbbi.h"

#अन्तर्भाव "rbbi_cache.h"

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "rbbidata.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

नियमः_Based_Break_Iterator::DictionaryCache::DictionaryCache(नियमः_Based_Break_Iterator *bi, UError_संहिता &स्थितिः) :
        fBI(bi), fBreaks(स्थितिः), f_Position_अन्तर्_Cache(-1),
        fStart(0), f_सीमा(0), f_First_नियमः_स्थितिः_सूचकांक(0), f_Other_नियमः_स्थितिः_सूचकांक(0) {
}

नियमः_Based_Break_Iterator::DictionaryCache::~DictionaryCache() {
}

शून्य नियमः_Based_Break_Iterator::DictionaryCache::reset() {
    f_Position_अन्तर्_Cache = -1;
    fStart = 0;
    f_सीमा = 0;
    f_First_नियमः_स्थितिः_सूचकांक = 0;
    f_Other_नियमः_स्थितिः_सूचकांक = 0;
    fBreaks.removeAllElements();
    int32_t max_स्थितिः = 0;
    प्रति (int32_t i = 0; i < fBI->f_दत्त->f_स्थितिः_Max_Idx; ++i) {
        max_स्थितिः = std::max(max_स्थितिः, fBI->f_दत्त->f_नियमः_स्थितिः_सारणी[i]);
    }
    isWordLike = max_स्थितिः > 100;
}

UBool नियमः_Based_Break_Iterator::DictionaryCache::following(int32_t fromPos, int32_t *परिणाम, int32_t *स्थितिः_सूचकांक) {
    यदि (fromPos >= f_सीमा || fromPos < fStart) {
        f_Position_अन्तर्_Cache = -1;
        प्रत्यागम असत्य;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    यदि (f_Position_अन्तर्_Cache >= 0 && f_Position_अन्तर्_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_अन्तर्_Cache) == fromPos) {
        ++f_Position_अन्तर्_Cache;
        यदि (f_Position_अन्तर्_Cache >= fBreaks.size()) {
            f_Position_अन्तर्_Cache = -1;
            प्रत्यागम असत्य;
        }
        r = fBreaks.elementAti(f_Position_अन्तर्_Cache);
        U_ASSERT(r > fromPos);
        *परिणाम = r;
        *स्थितिः_सूचकांक = isWordLike || r == f_सीमा ? f_Other_नियमः_स्थितिः_सूचकांक : 0;
        प्रत्यागम सत्य;
    }

    // Random indexing. Linear search for the boundary following the given position.

    प्रति (f_Position_अन्तर्_Cache = 0; f_Position_अन्तर्_Cache < fBreaks.size(); ++f_Position_अन्तर्_Cache) {
        r= fBreaks.elementAti(f_Position_अन्तर्_Cache);
        यदि (r > fromPos) {
            *परिणाम = r;
            *स्थितिः_सूचकांक = isWordLike || r == f_सीमा ? f_Other_नियमः_स्थितिः_सूचकांक : 0;
            प्रत्यागम सत्य;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool नियमः_Based_Break_Iterator::DictionaryCache::preceding(int32_t fromPos, int32_t *परिणाम, int32_t *स्थितिः_सूचकांक) {
    यदि (fromPos <= fStart || fromPos > f_सीमा) {
        f_Position_अन्तर्_Cache = -1;
        प्रत्यागम असत्य;
    }

    यदि (fromPos == f_सीमा) {
        f_Position_अन्तर्_Cache = fBreaks.size() - 1;
        यदि (f_Position_अन्तर्_Cache >= 0) {
            U_ASSERT(fBreaks.elementAti(f_Position_अन्तर्_Cache) == fromPos);
        }
    }

    int32_t r;
    यदि (f_Position_अन्तर्_Cache > 0 && f_Position_अन्तर्_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_अन्तर्_Cache) == fromPos) {
        --f_Position_अन्तर्_Cache;
        r = fBreaks.elementAti(f_Position_अन्तर्_Cache);
        U_ASSERT(r < fromPos);
        *परिणाम = r;
        *स्थितिः_सूचकांक = (r == fStart) ? f_First_नियमः_स्थितिः_सूचकांक
                       : isWordLike  ? f_Other_नियमः_स्थितिः_सूचकांक
                                     : 0;
        प्रत्यागम सत्य;
    }

    यदि (f_Position_अन्तर्_Cache == 0) {
        f_Position_अन्तर्_Cache = -1;
        प्रत्यागम असत्य;
    }

    प्रति (f_Position_अन्तर्_Cache = fBreaks.size()-1; f_Position_अन्तर्_Cache >= 0; --f_Position_अन्तर्_Cache) {
        r = fBreaks.elementAti(f_Position_अन्तर्_Cache);
        यदि (r < fromPos) {
            *परिणाम = r;
            *स्थितिः_सूचकांक = (r == fStart) ? f_First_नियमः_स्थितिः_सूचकांक
                           : isWordLike  ? f_Other_नियमः_स्थितिः_सूचकांक
                                         : 0;
            प्रत्यागम सत्य;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

शून्य नियमः_Based_Break_Iterator::DictionaryCache::populateDictionary(int32_t startPos, int32_t अन्तः_Pos,
                                       int32_t first_नियमः_स्थितिः, int32_t other_नियमः_स्थितिः) {
    यदि ((अन्तः_Pos - startPos) <= 1) {
        प्रत्यागम;
    }

    reset();
    f_First_नियमः_स्थितिः_सूचकांक = first_नियमः_स्थितिः;
    f_Other_नियमः_स्थितिः_सूचकांक = other_नियमः_स्थितिः;

    int32_t rangeStart = startPos;
    int32_t range_अन्तः = अन्तः_Pos;

    uint16_t    category;
    int32_t     current;
    UError_संहिता  स्थितिः = U_ZERO_ERROR;
    int32_t     प्राप्तम्_Break_गणनम् = 0;
    UText      *text = &fBI->fText;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_समुच्चयः_Native_सूचकांक(text, rangeStart);
    UChar32     c = utext_current32(text);
    category = ucptrie_get(fBI->f_दत्त->fTrie, c);
    uint32_t dictStart = fBI->f_दत्त->f_Forward_सारणी->fDictCategoriesStart;

    यावत्(U_SUCCESS(स्थितिः)) {
        यावत् ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(text))) < range_अन्तः
                && (category < dictStart)) {
            utext_next32(text);           // TODO: cleaner loop structure.
            c = utext_current32(text);
            category = ucptrie_get(fBI->f_दत्त->fTrie, c);
        }
        यदि (current >= range_अन्तः) {
            भङ्ग;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        ध्रुव LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, स्थितिः));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        यदि (lbe != शून्यसूचकः) {
            प्राप्तम्_Break_गणनम् += lbe->findBreaks(text, current, range_अन्तः, fBreaks, fBI->fIsPhraseBreaking, स्थितिः);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(text);
        category = ucptrie_get(fBI->f_दत्त->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    यदि (प्राप्तम्_Break_गणनम् > 0) {
        U_ASSERT(प्राप्तम्_Break_गणनम् == fBreaks.size());
        यदि (startPos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.निवेशय_Element_At(startPos, 0, स्थितिः);
        }
        यदि (अन्तः_Pos > fBreaks.peeki()) {
            fBreaks.push(अन्तः_Pos, स्थितिः);
        }
        f_Position_अन्तर्_Cache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        fStart = fBreaks.elementAti(0);
        f_सीमा = fBreaks.peeki();
    } अन्यथा {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

नियमः_Based_Break_Iterator::BreakCache::BreakCache(नियमः_Based_Break_Iterator *bi, UError_संहिता &स्थितिः) :
        fBI(bi), f_Side_बफर(स्थितिः) {
    reset();
}


नियमः_Based_Break_Iterator::BreakCache::~BreakCache() {
}


शून्य नियमः_Based_Break_Iterator::BreakCache::reset(int32_t pos, int32_t नियमः_स्थितिः) {
    fStartBufIdx = 0;
    f_अन्तः_Buf_Idx = 0;
    fTextIdx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(नियमः_स्थितिः);
}


int32_t  नियमः_Based_Break_Iterator::BreakCache::current() {
    fBI->fPosition = fTextIdx;
    fBI->f_नियमः_स्थितिः_सूचकांक = fStatuses[fBufIdx];
    fBI->f_समाप्तम् = असत्य;
    प्रत्यागम fTextIdx;
}


शून्य नियमः_Based_Break_Iterator::BreakCache::following(int32_t startPos, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (startPos == fTextIdx || seek(startPos) || populateNear(startPos, स्थितिः)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->f_समाप्तम् = असत्य;
        next();
    }
}


शून्य नियमः_Based_Break_Iterator::BreakCache::preceding(int32_t startPos, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (startPos == fTextIdx || seek(startPos) || populateNear(startPos, स्थितिः)) {
        यदि (startPos == fTextIdx) {
            previous(स्थितिः);
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
शून्य नियमः_Based_Break_Iterator::BreakCache::अग्र_OL() {
    fBI->f_समाप्तम् = !populateFollowing();
    fBI->fPosition = fTextIdx;
    fBI->f_नियमः_स्थितिः_सूचकांक = fStatuses[fBufIdx];
}


शून्य नियमः_Based_Break_Iterator::BreakCache::previous(UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    int32_t initialBufIdx = fBufIdx;
    यदि (fBufIdx == fStartBufIdx) {
        // At start of cache. Prepend to it.
        populatePreceding(स्थितिः);
    } अन्यथा {
        // Cache already holds the next boundary
        fBufIdx = mod_Chunk_आकार(fBufIdx - 1);
        fTextIdx = fBoundaries[fBufIdx];
    }
    fBI->f_समाप्तम् = (fBufIdx == initialBufIdx);
    fBI->fPosition = fTextIdx;
    fBI->f_नियमः_स्थितिः_सूचकांक = fStatuses[fBufIdx];
}


UBool नियमः_Based_Break_Iterator::BreakCache::seek(int32_t pos) {
    यदि (pos < fBoundaries[fStartBufIdx] || pos > fBoundaries[f_अन्तः_Buf_Idx]) {
        प्रत्यागम असत्य;
    }
    यदि (pos == fBoundaries[fStartBufIdx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = fStartBufIdx;
        fTextIdx = fBoundaries[fBufIdx];
        प्रत्यागम सत्य;
    }
    यदि (pos == fBoundaries[f_अन्तः_Buf_Idx]) {
        fBufIdx = f_अन्तः_Buf_Idx;
        fTextIdx = fBoundaries[fBufIdx];
        प्रत्यागम सत्य;
    }

    int32_t min = fStartBufIdx;
    int32_t max = f_अन्तः_Buf_Idx;
    यावत् (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_आकार : 0)) / 2;
        probe = mod_Chunk_आकार(probe);
        यदि (fBoundaries[probe] > pos) {
            max = probe;
        } अन्यथा {
            min = mod_Chunk_आकार(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = mod_Chunk_आकार(max - 1);
    fTextIdx = fBoundaries[fBufIdx];
    U_ASSERT(fTextIdx <= pos);
    प्रत्यागम सत्य;
}


UBool नियमः_Based_Break_Iterator::BreakCache::populateNear(int32_t position, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    U_ASSERT(position < fBoundaries[fStartBufIdx] || position > fBoundaries[f_अन्तः_Buf_Idx]);

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
    स्थिर constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t नियमः_स्थितिः_सूचकांक = 0;
    तार्किक retainCache = असत्य;
    यदि ((position > fBoundaries[fStartBufIdx] - CACHE_NEAR) && position < (fBoundaries[f_अन्तः_Buf_Idx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = सत्य;
    } अन्यथा यदि (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = असत्य;
        aBoundary = 0;
    } अन्यथा {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handleSafePrevious(position);

        यदि (fBoundaries[f_अन्तः_Buf_Idx] < position && fBoundaries[f_अन्तः_Buf_Idx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = सत्य;
        } अन्यथा यदि (backupPos < CACHE_NEAR) {
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
            retainCache = असत्य;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handle_अग्र();
            यदि (aBoundary != UBRK_समाप्तम् && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_समुच्चयः_Native_सूचकांक(&fBI->fText, aBoundary);
                यदि (backupPos == utext_get_Previous_Native_सूचकांक(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handle_अग्र();   // Safe rules identify safe pairs.
                }
            }
            यदि (aBoundary == UBRK_समाप्तम्) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_native_दीर्घता(&fBI->fText);
            }
            नियमः_स्थितिः_सूचकांक = fBI->f_नियमः_स्थितिः_सूचकांक;
        }
    }

    यदि (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, नियमः_स्थितिः_सूचकांक);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    यदि (fBoundaries[f_अन्तः_Buf_Idx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        यावत् (fBoundaries[f_अन्तः_Buf_Idx] < position) {
            यदि (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = f_अन्तः_Buf_Idx;                      // Set iterator position to the end of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        यावत् (fTextIdx > position) {              // Move backwards to a position at or preceding the requested pos.
            previous(स्थितिः);
        }
        प्रत्यागम सत्य;
    }

    यदि (fBoundaries[fStartBufIdx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        यावत् (fBoundaries[fStartBufIdx] > position) {
            populatePreceding(स्थितिः);
        }
        fBufIdx = fStartBufIdx;                    // Set iterator position to the start of the buffer.
        fTextIdx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        यावत् (fTextIdx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        यदि (fTextIdx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            previous(स्थितिः);
        }
        प्रत्यागम सत्य;
    }

    U_ASSERT(fTextIdx == position);
    प्रत्यागम सत्य;
}



UBool नियमः_Based_Break_Iterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[f_अन्तः_Buf_Idx];
    int32_t from_नियमः_स्थितिः_Idx = fStatuses[f_अन्तः_Buf_Idx];
    int32_t pos = 0;
    int32_t नियमः_स्थितिः_Idx = 0;

    यदि (fBI->fDictionaryCache->following(fromPosition, &pos, &नियमः_स्थितिः_Idx)) {
        सङ्कलय_Following(pos, नियमः_स्थितिः_Idx, UpdateCachePosition);
        प्रत्यागम सत्य;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handle_अग्र();
    यदि (pos == UBRK_समाप्तम्) {
        प्रत्यागम असत्य;
    }

    नियमः_स्थितिः_Idx = fBI->f_नियमः_स्थितिः_सूचकांक;
    यदि (fBI->f_Dictionary_Char_गणनम् > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, from_नियमः_स्थितिः_Idx, नियमः_स्थितिः_Idx);
        यदि (fBI->fDictionaryCache->following(fromPosition, &pos, &नियमः_स्थितिः_Idx)) {
            सङ्कलय_Following(pos, नियमः_स्थितिः_Idx, UpdateCachePosition);
            प्रत्यागम सत्य;
            // TODO: may want to move a sizable chunk of dictionary cache to break cache at this point.
            //       But be careful with interactions with populateNear().
        }
    }

    // Rule based segment did not include dictionary characters.
    // Or, it did contain dictionary chars, but the dictionary segmenter didn't handle them,
    //    meaning that we didn't take the return, above.
    // Add its end point to the cache.
    सङ्कलय_Following(pos, नियमः_स्थितिः_Idx, UpdateCachePosition);

    // Add several non-dictionary boundaries at this point, to optimize straight forward iteration.
    //    (subsequent calls to BreakIterator::next() will take the fast path, getting cached results.
    //
    प्रति (पूर्णाङ्क count=0; count<6; ++count) {
        pos = fBI->handle_अग्र();
        यदि (pos == UBRK_समाप्तम् || fBI->f_Dictionary_Char_गणनम् > 0) {
            भङ्ग;
        }
        सङ्कलय_Following(pos, fBI->f_नियमः_स्थितिः_सूचकांक, RetainCachePosition);
    }

    प्रत्यागम सत्य;
}


UBool नियमः_Based_Break_Iterator::BreakCache::populatePreceding(UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }

    int32_t fromPosition = fBoundaries[fStartBufIdx];
    यदि (fromPosition == 0) {
        प्रत्यागम असत्य;
    }

    int32_t position = 0;
    int32_t position_स्थितिः_Idx = 0;

    यदि (fBI->fDictionaryCache->preceding(fromPosition, &position, &position_स्थितिः_Idx)) {
        सङ्कलय_Preceding(position, position_स्थितिः_Idx, UpdateCachePosition);
        प्रत्यागम सत्य;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    कुरु {
        backupPosition = backupPosition - 30;
        यदि (backupPosition <= 0) {
            backupPosition = 0;
        } अन्यथा {
            backupPosition = fBI->handleSafePrevious(backupPosition);
        }
        यदि (backupPosition == UBRK_समाप्तम् || backupPosition == 0) {
            position = 0;
            position_स्थितिः_Idx = 0;
        } अन्यथा {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handle_अग्र();
            यदि (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_समुच्चयः_Native_सूचकांक(&fBI->fText, position);
                यदि (backupPosition == utext_get_Previous_Native_सूचकांक(&fBI->fText)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handle_अग्र();   // Safe rules identify safe pairs.
                }
            }
            position_स्थितिः_Idx = fBI->f_नियमः_स्थितिः_सूचकांक;
        }
    } यावत् (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    f_Side_बफर.removeAllElements();
    f_Side_बफर.सङ्कलय_Element(position, स्थितिः);
    f_Side_बफर.सङ्कलय_Element(position_स्थितिः_Idx, स्थितिः);

    कुरु {
        int32_t पूर्व_Position = fBI->fPosition = position;
        int32_t पूर्व_स्थितिः_Idx = position_स्थितिः_Idx;
        position = fBI->handle_अग्र();
        position_स्थितिः_Idx = fBI->f_नियमः_स्थितिः_सूचकांक;
        यदि (position == UBRK_समाप्तम्) {
            भङ्ग;
        }

        UBool segmentHandledByDictionary = असत्य;
        यदि (fBI->f_Dictionary_Char_गणनम् != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dict_Seg_अन्तः_Position = position;
            fBI->fDictionaryCache->populateDictionary(पूर्व_Position, dict_Seg_अन्तः_Position, पूर्व_स्थितिः_Idx, position_स्थितिः_Idx);
            यावत् (fBI->fDictionaryCache->following(पूर्व_Position, &position, &position_स्थितिः_Idx)) {
                segmentHandledByDictionary = सत्य;
                U_ASSERT(position > पूर्व_Position);
                यदि (position >= fromPosition) {
                    भङ्ग;
                }
                U_ASSERT(position <= dict_Seg_अन्तः_Position);
                f_Side_बफर.सङ्कलय_Element(position, स्थितिः);
                f_Side_बफर.सङ्कलय_Element(position_स्थितिः_Idx, स्थितिः);
                पूर्व_Position = position;
            }
            U_ASSERT(position==dict_Seg_अन्तः_Position || position>=fromPosition);
        }

        यदि (!segmentHandledByDictionary && position < fromPosition) {
            f_Side_बफर.सङ्कलय_Element(position, स्थितिः);
            f_Side_बफर.सङ्कलय_Element(position_स्थितिः_Idx, स्थितिः);
        }
    } यावत् (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = असत्य;
    यदि (!f_Side_बफर.is_रिक्तानि()) {
        position_स्थितिः_Idx = f_Side_बफर.popi();
        position = f_Side_बफर.popi();
        सङ्कलय_Preceding(position, position_स्थितिः_Idx, UpdateCachePosition);
        success = सत्य;
    }

    यावत् (!f_Side_बफर.is_रिक्तानि()) {
        position_स्थितिः_Idx = f_Side_बफर.popi();
        position = f_Side_बफर.popi();
        यदि (!सङ्कलय_Preceding(position, position_स्थितिः_Idx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            भङ्ग;
        }
    }

    प्रत्यागम success;
}


शून्य नियमः_Based_Break_Iterator::BreakCache::सङ्कलय_Following(int32_t position, int32_t नियमः_स्थितिः_Idx, Update_Position_मानानि update) {
    U_ASSERT(position > fBoundaries[f_अन्तः_Buf_Idx]);
    U_ASSERT(नियमः_स्थितिः_Idx <= UINT16_MAX);
    int32_t अग्र_Idx = mod_Chunk_आकार(f_अन्तः_Buf_Idx + 1);
    यदि (अग्र_Idx == fStartBufIdx) {
        fStartBufIdx = mod_Chunk_आकार(fStartBufIdx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[अग्र_Idx] = position;
    fStatuses[अग्र_Idx] = static_cast<uint16_t>(नियमः_स्थितिः_Idx);
    f_अन्तः_Buf_Idx = अग्र_Idx;
    यदि (update == UpdateCachePosition) {
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

तार्किक नियमः_Based_Break_Iterator::BreakCache::सङ्कलय_Preceding(int32_t position, int32_t नियमः_स्थितिः_Idx, Update_Position_मानानि update) {
    U_ASSERT(position < fBoundaries[fStartBufIdx]);
    U_ASSERT(नियमः_स्थितिः_Idx <= UINT16_MAX);
    int32_t अग्र_Idx = mod_Chunk_आकार(fStartBufIdx - 1);
    यदि (अग्र_Idx == f_अन्तः_Buf_Idx) {
        यदि (fBufIdx == f_अन्तः_Buf_Idx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            प्रत्यागम असत्य;
        }
        f_अन्तः_Buf_Idx = mod_Chunk_आकार(f_अन्तः_Buf_Idx - 1);
    }
    fBoundaries[अग्र_Idx] = position;
    fStatuses[अग्र_Idx] = static_cast<uint16_t>(नियमः_स्थितिः_Idx);
    fStartBufIdx = अग्र_Idx;
    यदि (update == UpdateCachePosition) {
        fBufIdx = अग्र_Idx;
        fTextIdx = position;
    }
    प्रत्यागम सत्य;
}


शून्य नियमः_Based_Break_Iterator::BreakCache::dumpCache() {
#यदि_परिभाषितम् RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", fTextIdx, fBufIdx);
    प्रति (int32_t i=fStartBufIdx; ; i=mod_Chunk_आकार(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        यदि (i == f_अन्तः_Buf_Idx) {
            भङ्ग;
        }
    }
#यदि_अन्तः
}

U_NAMESPACE_अन्तः

#यदि_अन्तः // #if !UCONFIG_NO_BREAK_ITERATION
