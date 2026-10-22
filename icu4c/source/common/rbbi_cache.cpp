// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/ubrk.h"
#ضمن "unicode/rbbi.h"

#ضمن "rbbi_cache.h"

#ضمن "brkeng.h"
#ضمن "cmemory.h"
#ضمن "rbbidata.h"
#ضمن "rbbirb.h"
#ضمن "uassert.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

قاعدہ_Based_Break_Iterator::DictionaryCache::DictionaryCache(قاعدہ_Based_Break_Iterator *bi, UError_کوڈ &حالت) :
        fBI(bi), fBreaks(حالت), f_Position_وچ_Cache(-1),
        f_شروع(0), f_حد_بندی(0), f_پہلا_قاعدہ_حالت_اشاریہ(0), f_Other_قاعدہ_حالت_اشاریہ(0) {
}

قاعدہ_Based_Break_Iterator::DictionaryCache::~DictionaryCache() {
}

فراغ قاعدہ_Based_Break_Iterator::DictionaryCache::reset() {
    f_Position_وچ_Cache = -1;
    f_شروع = 0;
    f_حد_بندی = 0;
    f_پہلا_قاعدہ_حالت_اشاریہ = 0;
    f_Other_قاعدہ_حالت_اشاریہ = 0;
    fBreaks.removeAllElements();
    int32_t max_حالت = 0;
    لكل (int32_t i = 0; i < fBI->f_انکڑے->f_حالت_Max_Idx; ++i) {
        max_حالت = std::max(max_حالت, fBI->f_انکڑے->f_قاعدہ_حالت_جدول[i]);
    }
    is_لفظ_Like = max_حالت > 100;
}

UBool قاعدہ_Based_Break_Iterator::DictionaryCache::following(int32_t fromPos, int32_t *نتیجہ, int32_t *حالت_اشاریہ) {
    إذا (fromPos >= f_حد_بندی || fromPos < f_شروع) {
        f_Position_وچ_Cache = -1;
        أرجع خطأ;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    إذا (f_Position_وچ_Cache >= 0 && f_Position_وچ_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_وچ_Cache) == fromPos) {
        ++f_Position_وچ_Cache;
        إذا (f_Position_وچ_Cache >= fBreaks.size()) {
            f_Position_وچ_Cache = -1;
            أرجع خطأ;
        }
        r = fBreaks.elementAti(f_Position_وچ_Cache);
        U_ASSERT(r > fromPos);
        *نتیجہ = r;
        *حالت_اشاریہ = is_لفظ_Like || r == f_حد_بندی ? f_Other_قاعدہ_حالت_اشاریہ : 0;
        أرجع سچ;
    }

    // Random indexing. Linear search for the boundary following the given position.

    لكل (f_Position_وچ_Cache = 0; f_Position_وچ_Cache < fBreaks.size(); ++f_Position_وچ_Cache) {
        r= fBreaks.elementAti(f_Position_وچ_Cache);
        إذا (r > fromPos) {
            *نتیجہ = r;
            *حالت_اشاریہ = is_لفظ_Like || r == f_حد_بندی ? f_Other_قاعدہ_حالت_اشاریہ : 0;
            أرجع سچ;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool قاعدہ_Based_Break_Iterator::DictionaryCache::preceding(int32_t fromPos, int32_t *نتیجہ, int32_t *حالت_اشاریہ) {
    إذا (fromPos <= f_شروع || fromPos > f_حد_بندی) {
        f_Position_وچ_Cache = -1;
        أرجع خطأ;
    }

    إذا (fromPos == f_حد_بندی) {
        f_Position_وچ_Cache = fBreaks.size() - 1;
        إذا (f_Position_وچ_Cache >= 0) {
            U_ASSERT(fBreaks.elementAti(f_Position_وچ_Cache) == fromPos);
        }
    }

    int32_t r;
    إذا (f_Position_وچ_Cache > 0 && f_Position_وچ_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_وچ_Cache) == fromPos) {
        --f_Position_وچ_Cache;
        r = fBreaks.elementAti(f_Position_وچ_Cache);
        U_ASSERT(r < fromPos);
        *نتیجہ = r;
        *حالت_اشاریہ = (r == f_شروع) ? f_پہلا_قاعدہ_حالت_اشاریہ
                       : is_لفظ_Like  ? f_Other_قاعدہ_حالت_اشاریہ
                                     : 0;
        أرجع سچ;
    }

    إذا (f_Position_وچ_Cache == 0) {
        f_Position_وچ_Cache = -1;
        أرجع خطأ;
    }

    لكل (f_Position_وچ_Cache = fBreaks.size()-1; f_Position_وچ_Cache >= 0; --f_Position_وچ_Cache) {
        r = fBreaks.elementAti(f_Position_وچ_Cache);
        إذا (r < fromPos) {
            *نتیجہ = r;
            *حالت_اشاریہ = (r == f_شروع) ? f_پہلا_قاعدہ_حالت_اشاریہ
                           : is_لفظ_Like  ? f_Other_قاعدہ_حالت_اشاریہ
                                         : 0;
            أرجع سچ;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

فراغ قاعدہ_Based_Break_Iterator::DictionaryCache::populateDictionary(int32_t شروع_Pos, int32_t سرا_Pos,
                                       int32_t پہلا_قاعدہ_حالت, int32_t other_قاعدہ_حالت) {
    إذا ((سرا_Pos - شروع_Pos) <= 1) {
        أرجع;
    }

    reset();
    f_پہلا_قاعدہ_حالت_اشاریہ = پہلا_قاعدہ_حالت;
    f_Other_قاعدہ_حالت_اشاریہ = other_قاعدہ_حالت;

    int32_t range_شروع = شروع_Pos;
    int32_t range_سرا = سرا_Pos;

    uint16_t    category;
    int32_t     current;
    UError_کوڈ  حالت = U_ZERO_ERROR;
    int32_t     لبھیا_Break_گنتی = 0;
    UText      *متن = &fBI->f_متن;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_سیٹ_Native_اشاریہ(متن, range_شروع);
    UChar32     c = utext_current32(متن);
    category = ucptrie_get(fBI->f_انکڑے->fTrie, c);
    uint32_t dict_شروع = fBI->f_انکڑے->f_Forward_جدول->f_Dict_Categories_شروع;

    طالما(U_SUCCESS(حالت)) {
        طالما ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(متن))) < range_سرا
                && (category < dict_شروع)) {
            utext_next32(متن);           // TODO: cleaner loop structure.
            c = utext_current32(متن);
            category = ucptrie_get(fBI->f_انکڑے->fTrie, c);
        }
        إذا (current >= range_سرا) {
            اكسر;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        ثابت LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, حالت));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        إذا (lbe != خالی_سوچک) {
            لبھیا_Break_گنتی += lbe->لبھو_Breaks(متن, current, range_سرا, fBreaks, fBI->fIsPhraseBreaking, حالت);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(متن);
        category = ucptrie_get(fBI->f_انکڑے->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    إذا (لبھیا_Break_گنتی > 0) {
        U_ASSERT(لبھیا_Break_گنتی == fBreaks.size());
        إذا (شروع_Pos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.داخل_کرو_Element_At(شروع_Pos, 0, حالت);
        }
        إذا (سرا_Pos > fBreaks.peeki()) {
            fBreaks.push(سرا_Pos, حالت);
        }
        f_Position_وچ_Cache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        f_شروع = fBreaks.elementAti(0);
        f_حد_بندی = fBreaks.peeki();
    } وإلا {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

قاعدہ_Based_Break_Iterator::BreakCache::BreakCache(قاعدہ_Based_Break_Iterator *bi, UError_کوڈ &حالت) :
        fBI(bi), f_Side_بفر(حالت) {
    reset();
}


قاعدہ_Based_Break_Iterator::BreakCache::~BreakCache() {
}


فراغ قاعدہ_Based_Break_Iterator::BreakCache::reset(int32_t pos, int32_t قاعدہ_حالت) {
    f_شروع_Buf_Idx = 0;
    f_سرا_Buf_Idx = 0;
    f_متن_Idx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(قاعدہ_حالت);
}


int32_t  قاعدہ_Based_Break_Iterator::BreakCache::current() {
    fBI->fPosition = f_متن_Idx;
    fBI->f_قاعدہ_حالت_اشاریہ = fStatuses[fBufIdx];
    fBI->f_پورا = خطأ;
    أرجع f_متن_Idx;
}


فراغ قاعدہ_Based_Break_Iterator::BreakCache::following(int32_t شروع_Pos, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (شروع_Pos == f_متن_Idx || seek(شروع_Pos) || populateNear(شروع_Pos, حالت)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->f_پورا = خطأ;
        next();
    }
}


فراغ قاعدہ_Based_Break_Iterator::BreakCache::preceding(int32_t شروع_Pos, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (شروع_Pos == f_متن_Idx || seek(شروع_Pos) || populateNear(شروع_Pos, حالت)) {
        إذا (شروع_Pos == f_متن_Idx) {
            پچھلا(حالت);
        } وإلا {
            // seek() leaves the BreakCache positioned at the preceding boundary
            //        if the requested position is between two boundaries.
            // current() pushes the BreakCache position out to the BreakIterator itself.
            U_ASSERT(شروع_Pos > f_متن_Idx);
            current();
        }
    }
}


/*
 * Out-of-line code for BreakCache::next().
 * Cache does not already contain the boundary
 */
فراغ قاعدہ_Based_Break_Iterator::BreakCache::اگلا_OL() {
    fBI->f_پورا = !populateFollowing();
    fBI->fPosition = f_متن_Idx;
    fBI->f_قاعدہ_حالت_اشاریہ = fStatuses[fBufIdx];
}


فراغ قاعدہ_Based_Break_Iterator::BreakCache::پچھلا(UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    int32_t initialBufIdx = fBufIdx;
    إذا (fBufIdx == f_شروع_Buf_Idx) {
        // At start of cache. Prepend to it.
        populatePreceding(حالت);
    } وإلا {
        // Cache already holds the next boundary
        fBufIdx = mod_Chunk_ناپ(fBufIdx - 1);
        f_متن_Idx = fBoundaries[fBufIdx];
    }
    fBI->f_پورا = (fBufIdx == initialBufIdx);
    fBI->fPosition = f_متن_Idx;
    fBI->f_قاعدہ_حالت_اشاریہ = fStatuses[fBufIdx];
}


UBool قاعدہ_Based_Break_Iterator::BreakCache::seek(int32_t pos) {
    إذا (pos < fBoundaries[f_شروع_Buf_Idx] || pos > fBoundaries[f_سرا_Buf_Idx]) {
        أرجع خطأ;
    }
    إذا (pos == fBoundaries[f_شروع_Buf_Idx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = f_شروع_Buf_Idx;
        f_متن_Idx = fBoundaries[fBufIdx];
        أرجع سچ;
    }
    إذا (pos == fBoundaries[f_سرا_Buf_Idx]) {
        fBufIdx = f_سرا_Buf_Idx;
        f_متن_Idx = fBoundaries[fBufIdx];
        أرجع سچ;
    }

    int32_t min = f_شروع_Buf_Idx;
    int32_t max = f_سرا_Buf_Idx;
    طالما (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_ناپ : 0)) / 2;
        probe = mod_Chunk_ناپ(probe);
        إذا (fBoundaries[probe] > pos) {
            max = probe;
        } وإلا {
            min = mod_Chunk_ناپ(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = mod_Chunk_ناپ(max - 1);
    f_متن_Idx = fBoundaries[fBufIdx];
    U_ASSERT(f_متن_Idx <= pos);
    أرجع سچ;
}


UBool قاعدہ_Based_Break_Iterator::BreakCache::populateNear(int32_t position, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    U_ASSERT(position < fBoundaries[f_شروع_Buf_Idx] || position > fBoundaries[f_سرا_Buf_Idx]);

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
    ساكن constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t قاعدہ_حالت_اشاریہ = 0;
    منطقي retainCache = خطأ;
    إذا ((position > fBoundaries[f_شروع_Buf_Idx] - CACHE_NEAR) && position < (fBoundaries[f_سرا_Buf_Idx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = سچ;
    } وإلا إذا (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = خطأ;
        aBoundary = 0;
    } وإلا {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handle_محفوظ_پچھلا(position);

        إذا (fBoundaries[f_سرا_Buf_Idx] < position && fBoundaries[f_سرا_Buf_Idx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = سچ;
        } وإلا إذا (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[f_شروع_Buf_Idx] <= (position + CACHE_NEAR));
        } وإلا {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = خطأ;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handle_اگلا();
            إذا (aBoundary != UBRK_پورا && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_سیٹ_Native_اشاریہ(&fBI->f_متن, aBoundary);
                إذا (backupPos == utext_get_پچھلا_Native_اشاریہ(&fBI->f_متن)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handle_اگلا();   // Safe rules identify safe pairs.
                }
            }
            إذا (aBoundary == UBRK_پورا) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_native_لمائی(&fBI->f_متن);
            }
            قاعدہ_حالت_اشاریہ = fBI->f_قاعدہ_حالت_اشاریہ;
        }
    }

    إذا (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, قاعدہ_حالت_اشاریہ);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    إذا (fBoundaries[f_سرا_Buf_Idx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        طالما (fBoundaries[f_سرا_Buf_Idx] < position) {
            إذا (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = f_سرا_Buf_Idx;                      // Set iterator position to the end of the buffer.
        f_متن_Idx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        طالما (f_متن_Idx > position) {              // Move backwards to a position at or preceding the requested pos.
            پچھلا(حالت);
        }
        أرجع سچ;
    }

    إذا (fBoundaries[f_شروع_Buf_Idx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        طالما (fBoundaries[f_شروع_Buf_Idx] > position) {
            populatePreceding(حالت);
        }
        fBufIdx = f_شروع_Buf_Idx;                    // Set iterator position to the start of the buffer.
        f_متن_Idx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        طالما (f_متن_Idx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        إذا (f_متن_Idx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            پچھلا(حالت);
        }
        أرجع سچ;
    }

    U_ASSERT(f_متن_Idx == position);
    أرجع سچ;
}



UBool قاعدہ_Based_Break_Iterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[f_سرا_Buf_Idx];
    int32_t from_قاعدہ_حالت_Idx = fStatuses[f_سرا_Buf_Idx];
    int32_t pos = 0;
    int32_t قاعدہ_حالت_Idx = 0;

    إذا (fBI->fDictionaryCache->following(fromPosition, &pos, &قاعدہ_حالت_Idx)) {
        جوڑو_Following(pos, قاعدہ_حالت_Idx, UpdateCachePosition);
        أرجع سچ;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handle_اگلا();
    إذا (pos == UBRK_پورا) {
        أرجع خطأ;
    }

    قاعدہ_حالت_Idx = fBI->f_قاعدہ_حالت_اشاریہ;
    إذا (fBI->f_Dictionary_Char_گنتی > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, from_قاعدہ_حالت_Idx, قاعدہ_حالت_Idx);
        إذا (fBI->fDictionaryCache->following(fromPosition, &pos, &قاعدہ_حالت_Idx)) {
            جوڑو_Following(pos, قاعدہ_حالت_Idx, UpdateCachePosition);
            أرجع سچ;
            // TODO: may want to move a sizable chunk of dictionary cache to break cache at this point.
            //       But be careful with interactions with populateNear().
        }
    }

    // Rule based segment did not include dictionary characters.
    // Or, it did contain dictionary chars, but the dictionary segmenter didn't handle them,
    //    meaning that we didn't take the return, above.
    // Add its end point to the cache.
    جوڑو_Following(pos, قاعدہ_حالت_Idx, UpdateCachePosition);

    // Add several non-dictionary boundaries at this point, to optimize straight forward iteration.
    //    (subsequent calls to BreakIterator::next() will take the fast path, getting cached results.
    //
    لكل (صحيح count=0; count<6; ++count) {
        pos = fBI->handle_اگلا();
        إذا (pos == UBRK_پورا || fBI->f_Dictionary_Char_گنتی > 0) {
            اكسر;
        }
        جوڑو_Following(pos, fBI->f_قاعدہ_حالت_اشاریہ, RetainCachePosition);
    }

    أرجع سچ;
}


UBool قاعدہ_Based_Break_Iterator::BreakCache::populatePreceding(UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }

    int32_t fromPosition = fBoundaries[f_شروع_Buf_Idx];
    إذا (fromPosition == 0) {
        أرجع خطأ;
    }

    int32_t position = 0;
    int32_t position_حالت_Idx = 0;

    إذا (fBI->fDictionaryCache->preceding(fromPosition, &position, &position_حالت_Idx)) {
        جوڑو_Preceding(position, position_حالت_Idx, UpdateCachePosition);
        أرجع سچ;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    افعل {
        backupPosition = backupPosition - 30;
        إذا (backupPosition <= 0) {
            backupPosition = 0;
        } وإلا {
            backupPosition = fBI->handle_محفوظ_پچھلا(backupPosition);
        }
        إذا (backupPosition == UBRK_پورا || backupPosition == 0) {
            position = 0;
            position_حالت_Idx = 0;
        } وإلا {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handle_اگلا();
            إذا (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_سیٹ_Native_اشاریہ(&fBI->f_متن, position);
                إذا (backupPosition == utext_get_پچھلا_Native_اشاریہ(&fBI->f_متن)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handle_اگلا();   // Safe rules identify safe pairs.
                }
            }
            position_حالت_Idx = fBI->f_قاعدہ_حالت_اشاریہ;
        }
    } طالما (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    f_Side_بفر.removeAllElements();
    f_Side_بفر.جوڑو_Element(position, حالت);
    f_Side_بفر.جوڑو_Element(position_حالت_Idx, حالت);

    افعل {
        int32_t prevPosition = fBI->fPosition = position;
        int32_t prev_حالت_Idx = position_حالت_Idx;
        position = fBI->handle_اگلا();
        position_حالت_Idx = fBI->f_قاعدہ_حالت_اشاریہ;
        إذا (position == UBRK_پورا) {
            اكسر;
        }

        UBool segmentHandledByDictionary = خطأ;
        إذا (fBI->f_Dictionary_Char_گنتی != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dict_Seg_سرا_Position = position;
            fBI->fDictionaryCache->populateDictionary(prevPosition, dict_Seg_سرا_Position, prev_حالت_Idx, position_حالت_Idx);
            طالما (fBI->fDictionaryCache->following(prevPosition, &position, &position_حالت_Idx)) {
                segmentHandledByDictionary = سچ;
                U_ASSERT(position > prevPosition);
                إذا (position >= fromPosition) {
                    اكسر;
                }
                U_ASSERT(position <= dict_Seg_سرا_Position);
                f_Side_بفر.جوڑو_Element(position, حالت);
                f_Side_بفر.جوڑو_Element(position_حالت_Idx, حالت);
                prevPosition = position;
            }
            U_ASSERT(position==dict_Seg_سرا_Position || position>=fromPosition);
        }

        إذا (!segmentHandledByDictionary && position < fromPosition) {
            f_Side_بفر.جوڑو_Element(position, حالت);
            f_Side_بفر.جوڑو_Element(position_حالت_Idx, حالت);
        }
    } طالما (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = خطأ;
    إذا (!f_Side_بفر.is_خالی_تھاواں()) {
        position_حالت_Idx = f_Side_بفر.popi();
        position = f_Side_بفر.popi();
        جوڑو_Preceding(position, position_حالت_Idx, UpdateCachePosition);
        success = سچ;
    }

    طالما (!f_Side_بفر.is_خالی_تھاواں()) {
        position_حالت_Idx = f_Side_بفر.popi();
        position = f_Side_بفر.popi();
        إذا (!جوڑو_Preceding(position, position_حالت_Idx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            اكسر;
        }
    }

    أرجع success;
}


فراغ قاعدہ_Based_Break_Iterator::BreakCache::جوڑو_Following(int32_t position, int32_t قاعدہ_حالت_Idx, Update_Position_اقدار update) {
    U_ASSERT(position > fBoundaries[f_سرا_Buf_Idx]);
    U_ASSERT(قاعدہ_حالت_Idx <= UINT16_MAX);
    int32_t اگلا_Idx = mod_Chunk_ناپ(f_سرا_Buf_Idx + 1);
    إذا (اگلا_Idx == f_شروع_Buf_Idx) {
        f_شروع_Buf_Idx = mod_Chunk_ناپ(f_شروع_Buf_Idx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[اگلا_Idx] = position;
    fStatuses[اگلا_Idx] = static_cast<uint16_t>(قاعدہ_حالت_Idx);
    f_سرا_Buf_Idx = اگلا_Idx;
    إذا (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = اگلا_Idx;
        f_متن_Idx = position;
    } وإلا {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(اگلا_Idx != fBufIdx);
    }
}

منطقي قاعدہ_Based_Break_Iterator::BreakCache::جوڑو_Preceding(int32_t position, int32_t قاعدہ_حالت_Idx, Update_Position_اقدار update) {
    U_ASSERT(position < fBoundaries[f_شروع_Buf_Idx]);
    U_ASSERT(قاعدہ_حالت_Idx <= UINT16_MAX);
    int32_t اگلا_Idx = mod_Chunk_ناپ(f_شروع_Buf_Idx - 1);
    إذا (اگلا_Idx == f_سرا_Buf_Idx) {
        إذا (fBufIdx == f_سرا_Buf_Idx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            أرجع خطأ;
        }
        f_سرا_Buf_Idx = mod_Chunk_ناپ(f_سرا_Buf_Idx - 1);
    }
    fBoundaries[اگلا_Idx] = position;
    fStatuses[اگلا_Idx] = static_cast<uint16_t>(قاعدہ_حالت_Idx);
    f_شروع_Buf_Idx = اگلا_Idx;
    إذا (update == UpdateCachePosition) {
        fBufIdx = اگلا_Idx;
        f_متن_Idx = position;
    }
    أرجع سچ;
}


فراغ قاعدہ_Based_Break_Iterator::BreakCache::dumpCache() {
#جے_تعریف_شدہ RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", f_متن_Idx, fBufIdx);
    لكل (int32_t i=f_شروع_Buf_Idx; ; i=mod_Chunk_ناپ(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        إذا (i == f_سرا_Buf_Idx) {
            اكسر;
        }
    }
#جے_انت
}

U_NAMESPACE_سرا

#جے_انت // #if !UCONFIG_NO_BREAK_ITERATION
