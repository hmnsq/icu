// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/ubrk.h"
#అన్తర్భావ "unicode/rbbi.h"

#అన్తర్భావ "rbbi_cache.h"

#అన్తర్భావ "brkeng.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "rbbidata.h"
#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

నియమం_Based_Break_Iterator::DictionaryCache::DictionaryCache(నియమం_Based_Break_Iterator *bi, UError_సంకేతం &స్థితి_2) :
        fBI(bi), fBreaks(స్థితి_2), f_Position_లోపల_Cache(-1),
        f_ప్రారంభం(0), f_హద్దు(0), f_మొదటి_నియమం_స్థితి_సూచీ(0), f_Other_నియమం_స్థితి_సూచీ(0) {
}

నియమం_Based_Break_Iterator::DictionaryCache::~DictionaryCache() {
}

శూన్య నియమం_Based_Break_Iterator::DictionaryCache::reset() {
    f_Position_లోపల_Cache = -1;
    f_ప్రారంభం = 0;
    f_హద్దు = 0;
    f_మొదటి_నియమం_స్థితి_సూచీ = 0;
    f_Other_నియమం_స్థితి_సూచీ = 0;
    fBreaks.removeAllElements();
    int32_t max_స్థితి = 0;
    కొరకు (int32_t i = 0; i < fBI->f_దత్తాంశం->f_స్థితి_Max_Idx; ++i) {
        max_స్థితి = std::max(max_స్థితి, fBI->f_దత్తాంశం->f_నియమం_స్థితి_పట్టిక[i]);
    }
    is_మాట_Like = max_స్థితి > 100;
}

UBool నియమం_Based_Break_Iterator::DictionaryCache::following(int32_t fromPos, int32_t *ఫలితం, int32_t *స్థితి_సూచీ) {
    ఒకవేళ (fromPos >= f_హద్దు || fromPos < f_ప్రారంభం) {
        f_Position_లోపల_Cache = -1;
        తిరిగి అబద్ధం;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    ఒకవేళ (f_Position_లోపల_Cache >= 0 && f_Position_లోపల_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_లోపల_Cache) == fromPos) {
        ++f_Position_లోపల_Cache;
        ఒకవేళ (f_Position_లోపల_Cache >= fBreaks.size()) {
            f_Position_లోపల_Cache = -1;
            తిరిగి అబద్ధం;
        }
        r = fBreaks.elementAti(f_Position_లోపల_Cache);
        U_ASSERT(r > fromPos);
        *ఫలితం = r;
        *స్థితి_సూచీ = is_మాట_Like || r == f_హద్దు ? f_Other_నియమం_స్థితి_సూచీ : 0;
        తిరిగి నిజం;
    }

    // Random indexing. Linear search for the boundary following the given position.

    కొరకు (f_Position_లోపల_Cache = 0; f_Position_లోపల_Cache < fBreaks.size(); ++f_Position_లోపల_Cache) {
        r= fBreaks.elementAti(f_Position_లోపల_Cache);
        ఒకవేళ (r > fromPos) {
            *ఫలితం = r;
            *స్థితి_సూచీ = is_మాట_Like || r == f_హద్దు ? f_Other_నియమం_స్థితి_సూచీ : 0;
            తిరిగి నిజం;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool నియమం_Based_Break_Iterator::DictionaryCache::preceding(int32_t fromPos, int32_t *ఫలితం, int32_t *స్థితి_సూచీ) {
    ఒకవేళ (fromPos <= f_ప్రారంభం || fromPos > f_హద్దు) {
        f_Position_లోపల_Cache = -1;
        తిరిగి అబద్ధం;
    }

    ఒకవేళ (fromPos == f_హద్దు) {
        f_Position_లోపల_Cache = fBreaks.size() - 1;
        ఒకవేళ (f_Position_లోపల_Cache >= 0) {
            U_ASSERT(fBreaks.elementAti(f_Position_లోపల_Cache) == fromPos);
        }
    }

    int32_t r;
    ఒకవేళ (f_Position_లోపల_Cache > 0 && f_Position_లోపల_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_లోపల_Cache) == fromPos) {
        --f_Position_లోపల_Cache;
        r = fBreaks.elementAti(f_Position_లోపల_Cache);
        U_ASSERT(r < fromPos);
        *ఫలితం = r;
        *స్థితి_సూచీ = (r == f_ప్రారంభం) ? f_మొదటి_నియమం_స్థితి_సూచీ
                       : is_మాట_Like  ? f_Other_నియమం_స్థితి_సూచీ
                                     : 0;
        తిరిగి నిజం;
    }

    ఒకవేళ (f_Position_లోపల_Cache == 0) {
        f_Position_లోపల_Cache = -1;
        తిరిగి అబద్ధం;
    }

    కొరకు (f_Position_లోపల_Cache = fBreaks.size()-1; f_Position_లోపల_Cache >= 0; --f_Position_లోపల_Cache) {
        r = fBreaks.elementAti(f_Position_లోపల_Cache);
        ఒకవేళ (r < fromPos) {
            *ఫలితం = r;
            *స్థితి_సూచీ = (r == f_ప్రారంభం) ? f_మొదటి_నియమం_స్థితి_సూచీ
                           : is_మాట_Like  ? f_Other_నియమం_స్థితి_సూచీ
                                         : 0;
            తిరిగి నిజం;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

శూన్య నియమం_Based_Break_Iterator::DictionaryCache::populateDictionary(int32_t ప్రారంభం_Pos, int32_t చివర_Pos,
                                       int32_t మొదటి_నియమం_స్థితి, int32_t other_నియమం_స్థితి) {
    ఒకవేళ ((చివర_Pos - ప్రారంభం_Pos) <= 1) {
        తిరిగి;
    }

    reset();
    f_మొదటి_నియమం_స్థితి_సూచీ = మొదటి_నియమం_స్థితి;
    f_Other_నియమం_స్థితి_సూచీ = other_నియమం_స్థితి;

    int32_t range_ప్రారంభం = ప్రారంభం_Pos;
    int32_t range_చివర = చివర_Pos;

    uint16_t    category;
    int32_t     current;
    UError_సంకేతం  స్థితి_2 = U_ZERO_ERROR;
    int32_t     దొరికింది_Break_లెక్క = 0;
    UText      *పాఠ్యం = &fBI->f_పాఠ్యం;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_సమితి_Native_సూచీ(పాఠ్యం, range_ప్రారంభం);
    UChar32     c = utext_current32(పాఠ్యం);
    category = ucptrie_get(fBI->f_దత్తాంశం->fTrie, c);
    uint32_t dict_ప్రారంభం = fBI->f_దత్తాంశం->f_Forward_పట్టిక->f_Dict_Categories_ప్రారంభం;

    అయితే(U_SUCCESS(స్థితి_2)) {
        అయితే ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(పాఠ్యం))) < range_చివర
                && (category < dict_ప్రారంభం)) {
            utext_next32(పాఠ్యం);           // TODO: cleaner loop structure.
            c = utext_current32(పాఠ్యం);
            category = ucptrie_get(fBI->f_దత్తాంశం->fTrie, c);
        }
        ఒకవేళ (current >= range_చివర) {
            ఆపు;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        స్థిరరాశి LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, స్థితి_2));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        ఒకవేళ (lbe != శూన్యసూచిక) {
            దొరికింది_Break_లెక్క += lbe->కనుగొను_Breaks(పాఠ్యం, current, range_చివర, fBreaks, fBI->fIsPhraseBreaking, స్థితి_2);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(పాఠ్యం);
        category = ucptrie_get(fBI->f_దత్తాంశం->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    ఒకవేళ (దొరికింది_Break_లెక్క > 0) {
        U_ASSERT(దొరికింది_Break_లెక్క == fBreaks.size());
        ఒకవేళ (ప్రారంభం_Pos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.చొప్పించు_Element_At(ప్రారంభం_Pos, 0, స్థితి_2);
        }
        ఒకవేళ (చివర_Pos > fBreaks.peeki()) {
            fBreaks.push(చివర_Pos, స్థితి_2);
        }
        f_Position_లోపల_Cache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        f_ప్రారంభం = fBreaks.elementAti(0);
        f_హద్దు = fBreaks.peeki();
    } లేకపోతే {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

నియమం_Based_Break_Iterator::BreakCache::BreakCache(నియమం_Based_Break_Iterator *bi, UError_సంకేతం &స్థితి_2) :
        fBI(bi), f_Side_బఫరు(స్థితి_2) {
    reset();
}


నియమం_Based_Break_Iterator::BreakCache::~BreakCache() {
}


శూన్య నియమం_Based_Break_Iterator::BreakCache::reset(int32_t pos, int32_t నియమం_స్థితి) {
    f_ప్రారంభం_Buf_Idx = 0;
    f_చివర_Buf_Idx = 0;
    f_పాఠ్యం_Idx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(నియమం_స్థితి);
}


int32_t  నియమం_Based_Break_Iterator::BreakCache::current() {
    fBI->fPosition = f_పాఠ్యం_Idx;
    fBI->f_నియమం_స్థితి_సూచీ = fStatuses[fBufIdx];
    fBI->f_పూర్తి = అబద్ధం;
    తిరిగి f_పాఠ్యం_Idx;
}


శూన్య నియమం_Based_Break_Iterator::BreakCache::following(int32_t ప్రారంభం_Pos, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (ప్రారంభం_Pos == f_పాఠ్యం_Idx || seek(ప్రారంభం_Pos) || populateNear(ప్రారంభం_Pos, స్థితి_2)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->f_పూర్తి = అబద్ధం;
        next();
    }
}


శూన్య నియమం_Based_Break_Iterator::BreakCache::preceding(int32_t ప్రారంభం_Pos, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (ప్రారంభం_Pos == f_పాఠ్యం_Idx || seek(ప్రారంభం_Pos) || populateNear(ప్రారంభం_Pos, స్థితి_2)) {
        ఒకవేళ (ప్రారంభం_Pos == f_పాఠ్యం_Idx) {
            మునుపటి(స్థితి_2);
        } లేకపోతే {
            // seek() leaves the BreakCache positioned at the preceding boundary
            //        if the requested position is between two boundaries.
            // current() pushes the BreakCache position out to the BreakIterator itself.
            U_ASSERT(ప్రారంభం_Pos > f_పాఠ్యం_Idx);
            current();
        }
    }
}


/*
 * Out-of-line code for BreakCache::next().
 * Cache does not already contain the boundary
 */
శూన్య నియమం_Based_Break_Iterator::BreakCache::తదుపరి_OL() {
    fBI->f_పూర్తి = !populateFollowing();
    fBI->fPosition = f_పాఠ్యం_Idx;
    fBI->f_నియమం_స్థితి_సూచీ = fStatuses[fBufIdx];
}


శూన్య నియమం_Based_Break_Iterator::BreakCache::మునుపటి(UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    int32_t initialBufIdx = fBufIdx;
    ఒకవేళ (fBufIdx == f_ప్రారంభం_Buf_Idx) {
        // At start of cache. Prepend to it.
        populatePreceding(స్థితి_2);
    } లేకపోతే {
        // Cache already holds the next boundary
        fBufIdx = mod_Chunk_కొలత(fBufIdx - 1);
        f_పాఠ్యం_Idx = fBoundaries[fBufIdx];
    }
    fBI->f_పూర్తి = (fBufIdx == initialBufIdx);
    fBI->fPosition = f_పాఠ్యం_Idx;
    fBI->f_నియమం_స్థితి_సూచీ = fStatuses[fBufIdx];
}


UBool నియమం_Based_Break_Iterator::BreakCache::seek(int32_t pos) {
    ఒకవేళ (pos < fBoundaries[f_ప్రారంభం_Buf_Idx] || pos > fBoundaries[f_చివర_Buf_Idx]) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (pos == fBoundaries[f_ప్రారంభం_Buf_Idx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = f_ప్రారంభం_Buf_Idx;
        f_పాఠ్యం_Idx = fBoundaries[fBufIdx];
        తిరిగి నిజం;
    }
    ఒకవేళ (pos == fBoundaries[f_చివర_Buf_Idx]) {
        fBufIdx = f_చివర_Buf_Idx;
        f_పాఠ్యం_Idx = fBoundaries[fBufIdx];
        తిరిగి నిజం;
    }

    int32_t min = f_ప్రారంభం_Buf_Idx;
    int32_t max = f_చివర_Buf_Idx;
    అయితే (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_కొలత : 0)) / 2;
        probe = mod_Chunk_కొలత(probe);
        ఒకవేళ (fBoundaries[probe] > pos) {
            max = probe;
        } లేకపోతే {
            min = mod_Chunk_కొలత(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = mod_Chunk_కొలత(max - 1);
    f_పాఠ్యం_Idx = fBoundaries[fBufIdx];
    U_ASSERT(f_పాఠ్యం_Idx <= pos);
    తిరిగి నిజం;
}


UBool నియమం_Based_Break_Iterator::BreakCache::populateNear(int32_t position, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    U_ASSERT(position < fBoundaries[f_ప్రారంభం_Buf_Idx] || position > fBoundaries[f_చివర_Buf_Idx]);

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
    స్థిర constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t నియమం_స్థితి_సూచీ = 0;
    తార్కిక retainCache = అబద్ధం;
    ఒకవేళ ((position > fBoundaries[f_ప్రారంభం_Buf_Idx] - CACHE_NEAR) && position < (fBoundaries[f_చివర_Buf_Idx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = నిజం;
    } లేకపోతే ఒకవేళ (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = అబద్ధం;
        aBoundary = 0;
    } లేకపోతే {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handle_సురక్షిత_మునుపటి(position);

        ఒకవేళ (fBoundaries[f_చివర_Buf_Idx] < position && fBoundaries[f_చివర_Buf_Idx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = నిజం;
        } లేకపోతే ఒకవేళ (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[f_ప్రారంభం_Buf_Idx] <= (position + CACHE_NEAR));
        } లేకపోతే {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = అబద్ధం;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handle_తదుపరి();
            ఒకవేళ (aBoundary != UBRK_పూర్తి && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_సమితి_Native_సూచీ(&fBI->f_పాఠ్యం, aBoundary);
                ఒకవేళ (backupPos == utext_get_మునుపటి_Native_సూచీ(&fBI->f_పాఠ్యం)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handle_తదుపరి();   // Safe rules identify safe pairs.
                }
            }
            ఒకవేళ (aBoundary == UBRK_పూర్తి) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_native_నిడివి(&fBI->f_పాఠ్యం);
            }
            నియమం_స్థితి_సూచీ = fBI->f_నియమం_స్థితి_సూచీ;
        }
    }

    ఒకవేళ (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, నియమం_స్థితి_సూచీ);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    ఒకవేళ (fBoundaries[f_చివర_Buf_Idx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        అయితే (fBoundaries[f_చివర_Buf_Idx] < position) {
            ఒకవేళ (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = f_చివర_Buf_Idx;                      // Set iterator position to the end of the buffer.
        f_పాఠ్యం_Idx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        అయితే (f_పాఠ్యం_Idx > position) {              // Move backwards to a position at or preceding the requested pos.
            మునుపటి(స్థితి_2);
        }
        తిరిగి నిజం;
    }

    ఒకవేళ (fBoundaries[f_ప్రారంభం_Buf_Idx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        అయితే (fBoundaries[f_ప్రారంభం_Buf_Idx] > position) {
            populatePreceding(స్థితి_2);
        }
        fBufIdx = f_ప్రారంభం_Buf_Idx;                    // Set iterator position to the start of the buffer.
        f_పాఠ్యం_Idx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        అయితే (f_పాఠ్యం_Idx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        ఒకవేళ (f_పాఠ్యం_Idx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            మునుపటి(స్థితి_2);
        }
        తిరిగి నిజం;
    }

    U_ASSERT(f_పాఠ్యం_Idx == position);
    తిరిగి నిజం;
}



UBool నియమం_Based_Break_Iterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[f_చివర_Buf_Idx];
    int32_t from_నియమం_స్థితి_Idx = fStatuses[f_చివర_Buf_Idx];
    int32_t pos = 0;
    int32_t నియమం_స్థితి_Idx = 0;

    ఒకవేళ (fBI->fDictionaryCache->following(fromPosition, &pos, &నియమం_స్థితి_Idx)) {
        కూడు_Following(pos, నియమం_స్థితి_Idx, UpdateCachePosition);
        తిరిగి నిజం;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handle_తదుపరి();
    ఒకవేళ (pos == UBRK_పూర్తి) {
        తిరిగి అబద్ధం;
    }

    నియమం_స్థితి_Idx = fBI->f_నియమం_స్థితి_సూచీ;
    ఒకవేళ (fBI->f_Dictionary_Char_లెక్క > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, from_నియమం_స్థితి_Idx, నియమం_స్థితి_Idx);
        ఒకవేళ (fBI->fDictionaryCache->following(fromPosition, &pos, &నియమం_స్థితి_Idx)) {
            కూడు_Following(pos, నియమం_స్థితి_Idx, UpdateCachePosition);
            తిరిగి నిజం;
            // TODO: may want to move a sizable chunk of dictionary cache to break cache at this point.
            //       But be careful with interactions with populateNear().
        }
    }

    // Rule based segment did not include dictionary characters.
    // Or, it did contain dictionary chars, but the dictionary segmenter didn't handle them,
    //    meaning that we didn't take the return, above.
    // Add its end point to the cache.
    కూడు_Following(pos, నియమం_స్థితి_Idx, UpdateCachePosition);

    // Add several non-dictionary boundaries at this point, to optimize straight forward iteration.
    //    (subsequent calls to BreakIterator::next() will take the fast path, getting cached results.
    //
    కొరకు (పూర్ణాంకం count=0; count<6; ++count) {
        pos = fBI->handle_తదుపరి();
        ఒకవేళ (pos == UBRK_పూర్తి || fBI->f_Dictionary_Char_లెక్క > 0) {
            ఆపు;
        }
        కూడు_Following(pos, fBI->f_నియమం_స్థితి_సూచీ, RetainCachePosition);
    }

    తిరిగి నిజం;
}


UBool నియమం_Based_Break_Iterator::BreakCache::populatePreceding(UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }

    int32_t fromPosition = fBoundaries[f_ప్రారంభం_Buf_Idx];
    ఒకవేళ (fromPosition == 0) {
        తిరిగి అబద్ధం;
    }

    int32_t position = 0;
    int32_t position_స్థితి_Idx = 0;

    ఒకవేళ (fBI->fDictionaryCache->preceding(fromPosition, &position, &position_స్థితి_Idx)) {
        కూడు_Preceding(position, position_స్థితి_Idx, UpdateCachePosition);
        తిరిగి నిజం;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    చేయి {
        backupPosition = backupPosition - 30;
        ఒకవేళ (backupPosition <= 0) {
            backupPosition = 0;
        } లేకపోతే {
            backupPosition = fBI->handle_సురక్షిత_మునుపటి(backupPosition);
        }
        ఒకవేళ (backupPosition == UBRK_పూర్తి || backupPosition == 0) {
            position = 0;
            position_స్థితి_Idx = 0;
        } లేకపోతే {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handle_తదుపరి();
            ఒకవేళ (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_సమితి_Native_సూచీ(&fBI->f_పాఠ్యం, position);
                ఒకవేళ (backupPosition == utext_get_మునుపటి_Native_సూచీ(&fBI->f_పాఠ్యం)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handle_తదుపరి();   // Safe rules identify safe pairs.
                }
            }
            position_స్థితి_Idx = fBI->f_నియమం_స్థితి_సూచీ;
        }
    } అయితే (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    f_Side_బఫరు.removeAllElements();
    f_Side_బఫరు.కూడు_Element(position, స్థితి_2);
    f_Side_బఫరు.కూడు_Element(position_స్థితి_Idx, స్థితి_2);

    చేయి {
        int32_t prevPosition = fBI->fPosition = position;
        int32_t prev_స్థితి_Idx = position_స్థితి_Idx;
        position = fBI->handle_తదుపరి();
        position_స్థితి_Idx = fBI->f_నియమం_స్థితి_సూచీ;
        ఒకవేళ (position == UBRK_పూర్తి) {
            ఆపు;
        }

        UBool segmentHandledByDictionary = అబద్ధం;
        ఒకవేళ (fBI->f_Dictionary_Char_లెక్క != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dict_Seg_చివర_Position = position;
            fBI->fDictionaryCache->populateDictionary(prevPosition, dict_Seg_చివర_Position, prev_స్థితి_Idx, position_స్థితి_Idx);
            అయితే (fBI->fDictionaryCache->following(prevPosition, &position, &position_స్థితి_Idx)) {
                segmentHandledByDictionary = నిజం;
                U_ASSERT(position > prevPosition);
                ఒకవేళ (position >= fromPosition) {
                    ఆపు;
                }
                U_ASSERT(position <= dict_Seg_చివర_Position);
                f_Side_బఫరు.కూడు_Element(position, స్థితి_2);
                f_Side_బఫరు.కూడు_Element(position_స్థితి_Idx, స్థితి_2);
                prevPosition = position;
            }
            U_ASSERT(position==dict_Seg_చివర_Position || position>=fromPosition);
        }

        ఒకవేళ (!segmentHandledByDictionary && position < fromPosition) {
            f_Side_బఫరు.కూడు_Element(position, స్థితి_2);
            f_Side_బఫరు.కూడు_Element(position_స్థితి_Idx, స్థితి_2);
        }
    } అయితే (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = అబద్ధం;
    ఒకవేళ (!f_Side_బఫరు.is_ఖాళీలు()) {
        position_స్థితి_Idx = f_Side_బఫరు.popi();
        position = f_Side_బఫరు.popi();
        కూడు_Preceding(position, position_స్థితి_Idx, UpdateCachePosition);
        success = నిజం;
    }

    అయితే (!f_Side_బఫరు.is_ఖాళీలు()) {
        position_స్థితి_Idx = f_Side_బఫరు.popi();
        position = f_Side_బఫరు.popi();
        ఒకవేళ (!కూడు_Preceding(position, position_స్థితి_Idx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            ఆపు;
        }
    }

    తిరిగి success;
}


శూన్య నియమం_Based_Break_Iterator::BreakCache::కూడు_Following(int32_t position, int32_t నియమం_స్థితి_Idx, Update_Position_విలువలు update) {
    U_ASSERT(position > fBoundaries[f_చివర_Buf_Idx]);
    U_ASSERT(నియమం_స్థితి_Idx <= UINT16_MAX);
    int32_t తదుపరి_Idx = mod_Chunk_కొలత(f_చివర_Buf_Idx + 1);
    ఒకవేళ (తదుపరి_Idx == f_ప్రారంభం_Buf_Idx) {
        f_ప్రారంభం_Buf_Idx = mod_Chunk_కొలత(f_ప్రారంభం_Buf_Idx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[తదుపరి_Idx] = position;
    fStatuses[తదుపరి_Idx] = static_cast<uint16_t>(నియమం_స్థితి_Idx);
    f_చివర_Buf_Idx = తదుపరి_Idx;
    ఒకవేళ (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = తదుపరి_Idx;
        f_పాఠ్యం_Idx = position;
    } లేకపోతే {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(తదుపరి_Idx != fBufIdx);
    }
}

తార్కిక నియమం_Based_Break_Iterator::BreakCache::కూడు_Preceding(int32_t position, int32_t నియమం_స్థితి_Idx, Update_Position_విలువలు update) {
    U_ASSERT(position < fBoundaries[f_ప్రారంభం_Buf_Idx]);
    U_ASSERT(నియమం_స్థితి_Idx <= UINT16_MAX);
    int32_t తదుపరి_Idx = mod_Chunk_కొలత(f_ప్రారంభం_Buf_Idx - 1);
    ఒకవేళ (తదుపరి_Idx == f_చివర_Buf_Idx) {
        ఒకవేళ (fBufIdx == f_చివర_Buf_Idx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            తిరిగి అబద్ధం;
        }
        f_చివర_Buf_Idx = mod_Chunk_కొలత(f_చివర_Buf_Idx - 1);
    }
    fBoundaries[తదుపరి_Idx] = position;
    fStatuses[తదుపరి_Idx] = static_cast<uint16_t>(నియమం_స్థితి_Idx);
    f_ప్రారంభం_Buf_Idx = తదుపరి_Idx;
    ఒకవేళ (update == UpdateCachePosition) {
        fBufIdx = తదుపరి_Idx;
        f_పాఠ్యం_Idx = position;
    }
    తిరిగి నిజం;
}


శూన్య నియమం_Based_Break_Iterator::BreakCache::dumpCache() {
#నిర్వచితమైతే RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", f_పాఠ్యం_Idx, fBufIdx);
    కొరకు (int32_t i=f_ప్రారంభం_Buf_Idx; ; i=mod_Chunk_కొలత(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        ఒకవేళ (i == f_చివర_Buf_Idx) {
            ఆపు;
        }
    }
#అయితే_ముగింపు
}

U_NAMESPACE_చివర

#అయితే_ముగింపు // #if !UCONFIG_NO_BREAK_ITERATION
