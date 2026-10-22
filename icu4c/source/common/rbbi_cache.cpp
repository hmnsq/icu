// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/ubrk.h"
#شامل "unicode/rbbi.h"

#شامل "rbbi_cache.h"

#شامل "brkeng.h"
#شامل "cmemory.h"
#شامل "rbbidata.h"
#شامل "rbbirb.h"
#شامل "uassert.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

قاعدہ_Based_Break_Iterator::DictionaryCache::DictionaryCache(قاعدہ_Based_Break_Iterator *bi, UError_رمز &حالت_2) :
        fBI(bi), fBreaks(حالت_2), f_Position_اندر_Cache(-1),
        f_آغاز(0), f_حد_بندی(0), f_پہلا_قاعدہ_حالت_اشاریہ(0), f_Other_قاعدہ_حالت_اشاریہ(0) {
}

قاعدہ_Based_Break_Iterator::DictionaryCache::~DictionaryCache() {
}

خالی قاعدہ_Based_Break_Iterator::DictionaryCache::reset() {
    f_Position_اندر_Cache = -1;
    f_آغاز = 0;
    f_حد_بندی = 0;
    f_پہلا_قاعدہ_حالت_اشاریہ = 0;
    f_Other_قاعدہ_حالت_اشاریہ = 0;
    fBreaks.removeAllElements();
    int32_t max_حالت = 0;
    برائے (int32_t i = 0; i < fBI->f_ڈیٹا->f_حالت_Max_Idx; ++i) {
        max_حالت = std::max(max_حالت, fBI->f_ڈیٹا->f_قاعدہ_حالت_جدول[i]);
    }
    is_لفظ_Like = max_حالت > 100;
}

UBool قاعدہ_Based_Break_Iterator::DictionaryCache::following(int32_t fromPos, int32_t *نتیجہ, int32_t *حالت_اشاریہ) {
    اگر (fromPos >= f_حد_بندی || fromPos < f_آغاز) {
        f_Position_اندر_Cache = -1;
        واپس جھوٹ;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    اگر (f_Position_اندر_Cache >= 0 && f_Position_اندر_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_اندر_Cache) == fromPos) {
        ++f_Position_اندر_Cache;
        اگر (f_Position_اندر_Cache >= fBreaks.size()) {
            f_Position_اندر_Cache = -1;
            واپس جھوٹ;
        }
        r = fBreaks.elementAti(f_Position_اندر_Cache);
        U_ASSERT(r > fromPos);
        *نتیجہ = r;
        *حالت_اشاریہ = is_لفظ_Like || r == f_حد_بندی ? f_Other_قاعدہ_حالت_اشاریہ : 0;
        واپس سچ;
    }

    // Random indexing. Linear search for the boundary following the given position.

    برائے (f_Position_اندر_Cache = 0; f_Position_اندر_Cache < fBreaks.size(); ++f_Position_اندر_Cache) {
        r= fBreaks.elementAti(f_Position_اندر_Cache);
        اگر (r > fromPos) {
            *نتیجہ = r;
            *حالت_اشاریہ = is_لفظ_Like || r == f_حد_بندی ? f_Other_قاعدہ_حالت_اشاریہ : 0;
            واپس سچ;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool قاعدہ_Based_Break_Iterator::DictionaryCache::preceding(int32_t fromPos, int32_t *نتیجہ, int32_t *حالت_اشاریہ) {
    اگر (fromPos <= f_آغاز || fromPos > f_حد_بندی) {
        f_Position_اندر_Cache = -1;
        واپس جھوٹ;
    }

    اگر (fromPos == f_حد_بندی) {
        f_Position_اندر_Cache = fBreaks.size() - 1;
        اگر (f_Position_اندر_Cache >= 0) {
            U_ASSERT(fBreaks.elementAti(f_Position_اندر_Cache) == fromPos);
        }
    }

    int32_t r;
    اگر (f_Position_اندر_Cache > 0 && f_Position_اندر_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_اندر_Cache) == fromPos) {
        --f_Position_اندر_Cache;
        r = fBreaks.elementAti(f_Position_اندر_Cache);
        U_ASSERT(r < fromPos);
        *نتیجہ = r;
        *حالت_اشاریہ = (r == f_آغاز) ? f_پہلا_قاعدہ_حالت_اشاریہ
                       : is_لفظ_Like  ? f_Other_قاعدہ_حالت_اشاریہ
                                     : 0;
        واپس سچ;
    }

    اگر (f_Position_اندر_Cache == 0) {
        f_Position_اندر_Cache = -1;
        واپس جھوٹ;
    }

    برائے (f_Position_اندر_Cache = fBreaks.size()-1; f_Position_اندر_Cache >= 0; --f_Position_اندر_Cache) {
        r = fBreaks.elementAti(f_Position_اندر_Cache);
        اگر (r < fromPos) {
            *نتیجہ = r;
            *حالت_اشاریہ = (r == f_آغاز) ? f_پہلا_قاعدہ_حالت_اشاریہ
                           : is_لفظ_Like  ? f_Other_قاعدہ_حالت_اشاریہ
                                         : 0;
            واپس سچ;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

خالی قاعدہ_Based_Break_Iterator::DictionaryCache::populateDictionary(int32_t آغاز_Pos, int32_t انتہا_Pos,
                                       int32_t پہلا_قاعدہ_حالت, int32_t other_قاعدہ_حالت) {
    اگر ((انتہا_Pos - آغاز_Pos) <= 1) {
        واپس;
    }

    reset();
    f_پہلا_قاعدہ_حالت_اشاریہ = پہلا_قاعدہ_حالت;
    f_Other_قاعدہ_حالت_اشاریہ = other_قاعدہ_حالت;

    int32_t range_آغاز = آغاز_Pos;
    int32_t range_انتہا = انتہا_Pos;

    uint16_t    category;
    int32_t     current;
    UError_رمز  حالت_2 = U_ZERO_ERROR;
    int32_t     ملا_Break_گنتی = 0;
    UText      *متن = &fBI->f_متن;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_سیٹ_Native_اشاریہ(متن, range_آغاز);
    UChar32     c = utext_current32(متن);
    category = ucptrie_get(fBI->f_ڈیٹا->fTrie, c);
    uint32_t dict_آغاز = fBI->f_ڈیٹا->f_Forward_جدول->f_Dict_Categories_آغاز;

    جب(U_SUCCESS(حالت_2)) {
        جب ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(متن))) < range_انتہا
                && (category < dict_آغاز)) {
            utext_next32(متن);           // TODO: cleaner loop structure.
            c = utext_current32(متن);
            category = ucptrie_get(fBI->f_ڈیٹا->fTrie, c);
        }
        اگر (current >= range_انتہا) {
            توڑو;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        ثابت LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, حالت_2));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        اگر (lbe != خالی_اشاریہ) {
            ملا_Break_گنتی += lbe->ڈھونڈو_Breaks(متن, current, range_انتہا, fBreaks, fBI->fIsPhraseBreaking, حالت_2);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(متن);
        category = ucptrie_get(fBI->f_ڈیٹا->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    اگر (ملا_Break_گنتی > 0) {
        U_ASSERT(ملا_Break_گنتی == fBreaks.size());
        اگر (آغاز_Pos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.داخل_کرو_Element_At(آغاز_Pos, 0, حالت_2);
        }
        اگر (انتہا_Pos > fBreaks.peeki()) {
            fBreaks.push(انتہا_Pos, حالت_2);
        }
        f_Position_اندر_Cache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        f_آغاز = fBreaks.elementAti(0);
        f_حد_بندی = fBreaks.peeki();
    } ورنہ {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

قاعدہ_Based_Break_Iterator::BreakCache::BreakCache(قاعدہ_Based_Break_Iterator *bi, UError_رمز &حالت_2) :
        fBI(bi), f_Side_بفر(حالت_2) {
    reset();
}


قاعدہ_Based_Break_Iterator::BreakCache::~BreakCache() {
}


خالی قاعدہ_Based_Break_Iterator::BreakCache::reset(int32_t pos, int32_t قاعدہ_حالت) {
    f_آغاز_Buf_Idx = 0;
    f_انتہا_Buf_Idx = 0;
    f_متن_Idx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(قاعدہ_حالت);
}


int32_t  قاعدہ_Based_Break_Iterator::BreakCache::current() {
    fBI->fPosition = f_متن_Idx;
    fBI->f_قاعدہ_حالت_اشاریہ = fStatuses[fBufIdx];
    fBI->f_مکمل = جھوٹ;
    واپس f_متن_Idx;
}


خالی قاعدہ_Based_Break_Iterator::BreakCache::following(int32_t آغاز_Pos, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (آغاز_Pos == f_متن_Idx || seek(آغاز_Pos) || populateNear(آغاز_Pos, حالت_2)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->f_مکمل = جھوٹ;
        next();
    }
}


خالی قاعدہ_Based_Break_Iterator::BreakCache::preceding(int32_t آغاز_Pos, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (آغاز_Pos == f_متن_Idx || seek(آغاز_Pos) || populateNear(آغاز_Pos, حالت_2)) {
        اگر (آغاز_Pos == f_متن_Idx) {
            پچھلا_2(حالت_2);
        } ورنہ {
            // seek() leaves the BreakCache positioned at the preceding boundary
            //        if the requested position is between two boundaries.
            // current() pushes the BreakCache position out to the BreakIterator itself.
            U_ASSERT(آغاز_Pos > f_متن_Idx);
            current();
        }
    }
}


/*
 * Out-of-line code for BreakCache::next().
 * Cache does not already contain the boundary
 */
خالی قاعدہ_Based_Break_Iterator::BreakCache::اگلا_OL() {
    fBI->f_مکمل = !populateFollowing();
    fBI->fPosition = f_متن_Idx;
    fBI->f_قاعدہ_حالت_اشاریہ = fStatuses[fBufIdx];
}


خالی قاعدہ_Based_Break_Iterator::BreakCache::پچھلا_2(UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    int32_t initialBufIdx = fBufIdx;
    اگر (fBufIdx == f_آغاز_Buf_Idx) {
        // At start of cache. Prepend to it.
        populatePreceding(حالت_2);
    } ورنہ {
        // Cache already holds the next boundary
        fBufIdx = mod_Chunk_سائز(fBufIdx - 1);
        f_متن_Idx = fBoundaries[fBufIdx];
    }
    fBI->f_مکمل = (fBufIdx == initialBufIdx);
    fBI->fPosition = f_متن_Idx;
    fBI->f_قاعدہ_حالت_اشاریہ = fStatuses[fBufIdx];
}


UBool قاعدہ_Based_Break_Iterator::BreakCache::seek(int32_t pos) {
    اگر (pos < fBoundaries[f_آغاز_Buf_Idx] || pos > fBoundaries[f_انتہا_Buf_Idx]) {
        واپس جھوٹ;
    }
    اگر (pos == fBoundaries[f_آغاز_Buf_Idx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = f_آغاز_Buf_Idx;
        f_متن_Idx = fBoundaries[fBufIdx];
        واپس سچ;
    }
    اگر (pos == fBoundaries[f_انتہا_Buf_Idx]) {
        fBufIdx = f_انتہا_Buf_Idx;
        f_متن_Idx = fBoundaries[fBufIdx];
        واپس سچ;
    }

    int32_t min = f_آغاز_Buf_Idx;
    int32_t max = f_انتہا_Buf_Idx;
    جب (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_سائز : 0)) / 2;
        probe = mod_Chunk_سائز(probe);
        اگر (fBoundaries[probe] > pos) {
            max = probe;
        } ورنہ {
            min = mod_Chunk_سائز(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = mod_Chunk_سائز(max - 1);
    f_متن_Idx = fBoundaries[fBufIdx];
    U_ASSERT(f_متن_Idx <= pos);
    واپس سچ;
}


UBool قاعدہ_Based_Break_Iterator::BreakCache::populateNear(int32_t position, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    U_ASSERT(position < fBoundaries[f_آغاز_Buf_Idx] || position > fBoundaries[f_انتہا_Buf_Idx]);

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
    ساکن constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t قاعدہ_حالت_اشاریہ = 0;
    منطقی retainCache = جھوٹ;
    اگر ((position > fBoundaries[f_آغاز_Buf_Idx] - CACHE_NEAR) && position < (fBoundaries[f_انتہا_Buf_Idx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = سچ;
    } ورنہ اگر (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = جھوٹ;
        aBoundary = 0;
    } ورنہ {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handle_محفوظ_پچھلا(position);

        اگر (fBoundaries[f_انتہا_Buf_Idx] < position && fBoundaries[f_انتہا_Buf_Idx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = سچ;
        } ورنہ اگر (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[f_آغاز_Buf_Idx] <= (position + CACHE_NEAR));
        } ورنہ {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = جھوٹ;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handle_اگلا();
            اگر (aBoundary != UBRK_مکمل && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_سیٹ_Native_اشاریہ(&fBI->f_متن, aBoundary);
                اگر (backupPos == utext_get_پچھلا_Native_اشاریہ(&fBI->f_متن)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handle_اگلا();   // Safe rules identify safe pairs.
                }
            }
            اگر (aBoundary == UBRK_مکمل) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_native_لمبائی(&fBI->f_متن);
            }
            قاعدہ_حالت_اشاریہ = fBI->f_قاعدہ_حالت_اشاریہ;
        }
    }

    اگر (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, قاعدہ_حالت_اشاریہ);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    اگر (fBoundaries[f_انتہا_Buf_Idx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        جب (fBoundaries[f_انتہا_Buf_Idx] < position) {
            اگر (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = f_انتہا_Buf_Idx;                      // Set iterator position to the end of the buffer.
        f_متن_Idx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        جب (f_متن_Idx > position) {              // Move backwards to a position at or preceding the requested pos.
            پچھلا_2(حالت_2);
        }
        واپس سچ;
    }

    اگر (fBoundaries[f_آغاز_Buf_Idx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        جب (fBoundaries[f_آغاز_Buf_Idx] > position) {
            populatePreceding(حالت_2);
        }
        fBufIdx = f_آغاز_Buf_Idx;                    // Set iterator position to the start of the buffer.
        f_متن_Idx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        جب (f_متن_Idx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        اگر (f_متن_Idx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            پچھلا_2(حالت_2);
        }
        واپس سچ;
    }

    U_ASSERT(f_متن_Idx == position);
    واپس سچ;
}



UBool قاعدہ_Based_Break_Iterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[f_انتہا_Buf_Idx];
    int32_t from_قاعدہ_حالت_Idx = fStatuses[f_انتہا_Buf_Idx];
    int32_t pos = 0;
    int32_t قاعدہ_حالت_Idx = 0;

    اگر (fBI->fDictionaryCache->following(fromPosition, &pos, &قاعدہ_حالت_Idx)) {
        جوڑو_Following(pos, قاعدہ_حالت_Idx, UpdateCachePosition);
        واپس سچ;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handle_اگلا();
    اگر (pos == UBRK_مکمل) {
        واپس جھوٹ;
    }

    قاعدہ_حالت_Idx = fBI->f_قاعدہ_حالت_اشاریہ;
    اگر (fBI->f_Dictionary_Char_گنتی > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, from_قاعدہ_حالت_Idx, قاعدہ_حالت_Idx);
        اگر (fBI->fDictionaryCache->following(fromPosition, &pos, &قاعدہ_حالت_Idx)) {
            جوڑو_Following(pos, قاعدہ_حالت_Idx, UpdateCachePosition);
            واپس سچ;
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
    برائے (صحیح count=0; count<6; ++count) {
        pos = fBI->handle_اگلا();
        اگر (pos == UBRK_مکمل || fBI->f_Dictionary_Char_گنتی > 0) {
            توڑو;
        }
        جوڑو_Following(pos, fBI->f_قاعدہ_حالت_اشاریہ, RetainCachePosition);
    }

    واپس سچ;
}


UBool قاعدہ_Based_Break_Iterator::BreakCache::populatePreceding(UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }

    int32_t fromPosition = fBoundaries[f_آغاز_Buf_Idx];
    اگر (fromPosition == 0) {
        واپس جھوٹ;
    }

    int32_t position = 0;
    int32_t position_حالت_Idx = 0;

    اگر (fBI->fDictionaryCache->preceding(fromPosition, &position, &position_حالت_Idx)) {
        جوڑو_Preceding(position, position_حالت_Idx, UpdateCachePosition);
        واپس سچ;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    کرو {
        backupPosition = backupPosition - 30;
        اگر (backupPosition <= 0) {
            backupPosition = 0;
        } ورنہ {
            backupPosition = fBI->handle_محفوظ_پچھلا(backupPosition);
        }
        اگر (backupPosition == UBRK_مکمل || backupPosition == 0) {
            position = 0;
            position_حالت_Idx = 0;
        } ورنہ {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handle_اگلا();
            اگر (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_سیٹ_Native_اشاریہ(&fBI->f_متن, position);
                اگر (backupPosition == utext_get_پچھلا_Native_اشاریہ(&fBI->f_متن)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handle_اگلا();   // Safe rules identify safe pairs.
                }
            }
            position_حالت_Idx = fBI->f_قاعدہ_حالت_اشاریہ;
        }
    } جب (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    f_Side_بفر.removeAllElements();
    f_Side_بفر.جوڑو_Element(position, حالت_2);
    f_Side_بفر.جوڑو_Element(position_حالت_Idx, حالت_2);

    کرو {
        int32_t پچھلا_Position = fBI->fPosition = position;
        int32_t پچھلا_حالت_Idx = position_حالت_Idx;
        position = fBI->handle_اگلا();
        position_حالت_Idx = fBI->f_قاعدہ_حالت_اشاریہ;
        اگر (position == UBRK_مکمل) {
            توڑو;
        }

        UBool segmentHandledByDictionary = جھوٹ;
        اگر (fBI->f_Dictionary_Char_گنتی != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dict_Seg_انتہا_Position = position;
            fBI->fDictionaryCache->populateDictionary(پچھلا_Position, dict_Seg_انتہا_Position, پچھلا_حالت_Idx, position_حالت_Idx);
            جب (fBI->fDictionaryCache->following(پچھلا_Position, &position, &position_حالت_Idx)) {
                segmentHandledByDictionary = سچ;
                U_ASSERT(position > پچھلا_Position);
                اگر (position >= fromPosition) {
                    توڑو;
                }
                U_ASSERT(position <= dict_Seg_انتہا_Position);
                f_Side_بفر.جوڑو_Element(position, حالت_2);
                f_Side_بفر.جوڑو_Element(position_حالت_Idx, حالت_2);
                پچھلا_Position = position;
            }
            U_ASSERT(position==dict_Seg_انتہا_Position || position>=fromPosition);
        }

        اگر (!segmentHandledByDictionary && position < fromPosition) {
            f_Side_بفر.جوڑو_Element(position, حالت_2);
            f_Side_بفر.جوڑو_Element(position_حالت_Idx, حالت_2);
        }
    } جب (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = جھوٹ;
    اگر (!f_Side_بفر.is_خالی_خانے()) {
        position_حالت_Idx = f_Side_بفر.popi();
        position = f_Side_بفر.popi();
        جوڑو_Preceding(position, position_حالت_Idx, UpdateCachePosition);
        success = سچ;
    }

    جب (!f_Side_بفر.is_خالی_خانے()) {
        position_حالت_Idx = f_Side_بفر.popi();
        position = f_Side_بفر.popi();
        اگر (!جوڑو_Preceding(position, position_حالت_Idx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            توڑو;
        }
    }

    واپس success;
}


خالی قاعدہ_Based_Break_Iterator::BreakCache::جوڑو_Following(int32_t position, int32_t قاعدہ_حالت_Idx, Update_Position_اقدار update) {
    U_ASSERT(position > fBoundaries[f_انتہا_Buf_Idx]);
    U_ASSERT(قاعدہ_حالت_Idx <= UINT16_MAX);
    int32_t اگلا_Idx = mod_Chunk_سائز(f_انتہا_Buf_Idx + 1);
    اگر (اگلا_Idx == f_آغاز_Buf_Idx) {
        f_آغاز_Buf_Idx = mod_Chunk_سائز(f_آغاز_Buf_Idx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[اگلا_Idx] = position;
    fStatuses[اگلا_Idx] = static_cast<uint16_t>(قاعدہ_حالت_Idx);
    f_انتہا_Buf_Idx = اگلا_Idx;
    اگر (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = اگلا_Idx;
        f_متن_Idx = position;
    } ورنہ {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(اگلا_Idx != fBufIdx);
    }
}

منطقی قاعدہ_Based_Break_Iterator::BreakCache::جوڑو_Preceding(int32_t position, int32_t قاعدہ_حالت_Idx, Update_Position_اقدار update) {
    U_ASSERT(position < fBoundaries[f_آغاز_Buf_Idx]);
    U_ASSERT(قاعدہ_حالت_Idx <= UINT16_MAX);
    int32_t اگلا_Idx = mod_Chunk_سائز(f_آغاز_Buf_Idx - 1);
    اگر (اگلا_Idx == f_انتہا_Buf_Idx) {
        اگر (fBufIdx == f_انتہا_Buf_Idx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            واپس جھوٹ;
        }
        f_انتہا_Buf_Idx = mod_Chunk_سائز(f_انتہا_Buf_Idx - 1);
    }
    fBoundaries[اگلا_Idx] = position;
    fStatuses[اگلا_Idx] = static_cast<uint16_t>(قاعدہ_حالت_Idx);
    f_آغاز_Buf_Idx = اگلا_Idx;
    اگر (update == UpdateCachePosition) {
        fBufIdx = اگلا_Idx;
        f_متن_Idx = position;
    }
    واپس سچ;
}


خالی قاعدہ_Based_Break_Iterator::BreakCache::dumpCache() {
#اگر_تعریف_شدہ RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", f_متن_Idx, fBufIdx);
    برائے (int32_t i=f_آغاز_Buf_Idx; ; i=mod_Chunk_سائز(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        اگر (i == f_انتہا_Buf_Idx) {
            توڑو;
        }
    }
#اگر_ختم
}

U_NAMESPACE_انتہا

#اگر_ختم // #if !UCONFIG_NO_BREAK_ITERATION
