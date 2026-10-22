// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rbbi.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "rbbi_cache.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "brkeng.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbidata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

ਨਿਯਮ_Based_Break_Iterator::DictionaryCache::DictionaryCache(ਨਿਯਮ_Based_Break_Iterator *bi, UError_ਕੋਡ &ਸਥਿਤੀ) :
        fBI(bi), fBreaks(ਸਥਿਤੀ), f_Position_ਵਿੱਚ_Cache(-1),
        f_ਸ਼ੁਰੂ(0), f_ਹੱਦਬੰਦੀ(0), f_ਪਹਿਲਾ_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ(0), f_Other_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ(0) {
}

ਨਿਯਮ_Based_Break_Iterator::DictionaryCache::~DictionaryCache() {
}

ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::DictionaryCache::reset() {
    f_Position_ਵਿੱਚ_Cache = -1;
    f_ਸ਼ੁਰੂ = 0;
    f_ਹੱਦਬੰਦੀ = 0;
    f_ਪਹਿਲਾ_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = 0;
    f_Other_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = 0;
    fBreaks.removeAllElements();
    int32_t max_ਸਥਿਤੀ = 0;
    ਲਈ (int32_t i = 0; i < fBI->f_ਅੰਕੜੇ->f_ਸਥਿਤੀ_Max_Idx; ++i) {
        max_ਸਥਿਤੀ = std::max(max_ਸਥਿਤੀ, fBI->f_ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ[i]);
    }
    is_ਸ਼ਬਦ_Like = max_ਸਥਿਤੀ > 100;
}

UBool ਨਿਯਮ_Based_Break_Iterator::DictionaryCache::following(int32_t fromPos, int32_t *ਨਤੀਜਾ, int32_t *ਸਥਿਤੀ_ਸੂਚਕਾਂਕ) {
    ਜੇ (fromPos >= f_ਹੱਦਬੰਦੀ || fromPos < f_ਸ਼ੁਰੂ) {
        f_Position_ਵਿੱਚ_Cache = -1;
        ਵਾਪਸ ਝੂਠ;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    ਜੇ (f_Position_ਵਿੱਚ_Cache >= 0 && f_Position_ਵਿੱਚ_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_ਵਿੱਚ_Cache) == fromPos) {
        ++f_Position_ਵਿੱਚ_Cache;
        ਜੇ (f_Position_ਵਿੱਚ_Cache >= fBreaks.size()) {
            f_Position_ਵਿੱਚ_Cache = -1;
            ਵਾਪਸ ਝੂਠ;
        }
        r = fBreaks.elementAti(f_Position_ਵਿੱਚ_Cache);
        U_ASSERT(r > fromPos);
        *ਨਤੀਜਾ = r;
        *ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = is_ਸ਼ਬਦ_Like || r == f_ਹੱਦਬੰਦੀ ? f_Other_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ : 0;
        ਵਾਪਸ ਸੱਚ;
    }

    // Random indexing. Linear search for the boundary following the given position.

    ਲਈ (f_Position_ਵਿੱਚ_Cache = 0; f_Position_ਵਿੱਚ_Cache < fBreaks.size(); ++f_Position_ਵਿੱਚ_Cache) {
        r= fBreaks.elementAti(f_Position_ਵਿੱਚ_Cache);
        ਜੇ (r > fromPos) {
            *ਨਤੀਜਾ = r;
            *ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = is_ਸ਼ਬਦ_Like || r == f_ਹੱਦਬੰਦੀ ? f_Other_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ : 0;
            ਵਾਪਸ ਸੱਚ;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool ਨਿਯਮ_Based_Break_Iterator::DictionaryCache::preceding(int32_t fromPos, int32_t *ਨਤੀਜਾ, int32_t *ਸਥਿਤੀ_ਸੂਚਕਾਂਕ) {
    ਜੇ (fromPos <= f_ਸ਼ੁਰੂ || fromPos > f_ਹੱਦਬੰਦੀ) {
        f_Position_ਵਿੱਚ_Cache = -1;
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ (fromPos == f_ਹੱਦਬੰਦੀ) {
        f_Position_ਵਿੱਚ_Cache = fBreaks.size() - 1;
        ਜੇ (f_Position_ਵਿੱਚ_Cache >= 0) {
            U_ASSERT(fBreaks.elementAti(f_Position_ਵਿੱਚ_Cache) == fromPos);
        }
    }

    int32_t r;
    ਜੇ (f_Position_ਵਿੱਚ_Cache > 0 && f_Position_ਵਿੱਚ_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_ਵਿੱਚ_Cache) == fromPos) {
        --f_Position_ਵਿੱਚ_Cache;
        r = fBreaks.elementAti(f_Position_ਵਿੱਚ_Cache);
        U_ASSERT(r < fromPos);
        *ਨਤੀਜਾ = r;
        *ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = (r == f_ਸ਼ੁਰੂ) ? f_ਪਹਿਲਾ_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ
                       : is_ਸ਼ਬਦ_Like  ? f_Other_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ
                                     : 0;
        ਵਾਪਸ ਸੱਚ;
    }

    ਜੇ (f_Position_ਵਿੱਚ_Cache == 0) {
        f_Position_ਵਿੱਚ_Cache = -1;
        ਵਾਪਸ ਝੂਠ;
    }

    ਲਈ (f_Position_ਵਿੱਚ_Cache = fBreaks.size()-1; f_Position_ਵਿੱਚ_Cache >= 0; --f_Position_ਵਿੱਚ_Cache) {
        r = fBreaks.elementAti(f_Position_ਵਿੱਚ_Cache);
        ਜੇ (r < fromPos) {
            *ਨਤੀਜਾ = r;
            *ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = (r == f_ਸ਼ੁਰੂ) ? f_ਪਹਿਲਾ_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ
                           : is_ਸ਼ਬਦ_Like  ? f_Other_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ
                                         : 0;
            ਵਾਪਸ ਸੱਚ;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::DictionaryCache::populateDictionary(int32_t ਸ਼ੁਰੂ_Pos, int32_t ਸਿਰਾ_Pos,
                                       int32_t ਪਹਿਲਾ_ਨਿਯਮ_ਸਥਿਤੀ, int32_t other_ਨਿਯਮ_ਸਥਿਤੀ) {
    ਜੇ ((ਸਿਰਾ_Pos - ਸ਼ੁਰੂ_Pos) <= 1) {
        ਵਾਪਸ;
    }

    reset();
    f_ਪਹਿਲਾ_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = ਪਹਿਲਾ_ਨਿਯਮ_ਸਥਿਤੀ;
    f_Other_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = other_ਨਿਯਮ_ਸਥਿਤੀ;

    int32_t range_ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_Pos;
    int32_t range_ਸਿਰਾ = ਸਿਰਾ_Pos;

    uint16_t    category;
    int32_t     current;
    UError_ਕੋਡ  ਸਥਿਤੀ = U_ZERO_ERROR;
    int32_t     ਮਿਲਿਆ_Break_ਗਿਣਤੀ = 0;
    UText      *ਪਾਠ = &fBI->f_ਪਾਠ;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, range_ਸ਼ੁਰੂ);
    UChar32     c = utext_current32(ਪਾਠ);
    category = ucptrie_get(fBI->f_ਅੰਕੜੇ->fTrie, c);
    uint32_t dict_ਸ਼ੁਰੂ = fBI->f_ਅੰਕੜੇ->f_Forward_ਸਾਰਣੀ->f_Dict_Categories_ਸ਼ੁਰੂ;

    ਜਦੋਂ(U_SUCCESS(ਸਥਿਤੀ)) {
        ਜਦੋਂ ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(ਪਾਠ))) < range_ਸਿਰਾ
                && (category < dict_ਸ਼ੁਰੂ)) {
            utext_next32(ਪਾਠ);           // TODO: cleaner loop structure.
            c = utext_current32(ਪਾਠ);
            category = ucptrie_get(fBI->f_ਅੰਕੜੇ->fTrie, c);
        }
        ਜੇ (current >= range_ਸਿਰਾ) {
            ਰੋਕੋ;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        ਅਚਲ LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, ਸਥਿਤੀ));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        ਜੇ (lbe != ਖਾਲੀ_ਸੂਚਕ) {
            ਮਿਲਿਆ_Break_ਗਿਣਤੀ += lbe->ਲੱਭੋ_Breaks(ਪਾਠ, current, range_ਸਿਰਾ, fBreaks, fBI->fIsPhraseBreaking, ਸਥਿਤੀ);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(ਪਾਠ);
        category = ucptrie_get(fBI->f_ਅੰਕੜੇ->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    ਜੇ (ਮਿਲਿਆ_Break_ਗਿਣਤੀ > 0) {
        U_ASSERT(ਮਿਲਿਆ_Break_ਗਿਣਤੀ == fBreaks.size());
        ਜੇ (ਸ਼ੁਰੂ_Pos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.ਪਾਓ_Element_At(ਸ਼ੁਰੂ_Pos, 0, ਸਥਿਤੀ);
        }
        ਜੇ (ਸਿਰਾ_Pos > fBreaks.peeki()) {
            fBreaks.push(ਸਿਰਾ_Pos, ਸਥਿਤੀ);
        }
        f_Position_ਵਿੱਚ_Cache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        f_ਸ਼ੁਰੂ = fBreaks.elementAti(0);
        f_ਹੱਦਬੰਦੀ = fBreaks.peeki();
    } ਨਹੀਂਤਾਂ {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

ਨਿਯਮ_Based_Break_Iterator::BreakCache::BreakCache(ਨਿਯਮ_Based_Break_Iterator *bi, UError_ਕੋਡ &ਸਥਿਤੀ) :
        fBI(bi), f_Side_ਬਫਰ(ਸਥਿਤੀ) {
    reset();
}


ਨਿਯਮ_Based_Break_Iterator::BreakCache::~BreakCache() {
}


ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::BreakCache::reset(int32_t pos, int32_t ਨਿਯਮ_ਸਥਿਤੀ) {
    f_ਸ਼ੁਰੂ_Buf_Idx = 0;
    f_ਸਿਰਾ_Buf_Idx = 0;
    f_ਪਾਠ_Idx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(ਨਿਯਮ_ਸਥਿਤੀ);
}


int32_t  ਨਿਯਮ_Based_Break_Iterator::BreakCache::current() {
    fBI->fPosition = f_ਪਾਠ_Idx;
    fBI->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = fStatuses[fBufIdx];
    fBI->f_ਪੂਰਾ = ਝੂਠ;
    ਵਾਪਸ f_ਪਾਠ_Idx;
}


ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::BreakCache::following(int32_t ਸ਼ੁਰੂ_Pos, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (ਸ਼ੁਰੂ_Pos == f_ਪਾਠ_Idx || seek(ਸ਼ੁਰੂ_Pos) || populateNear(ਸ਼ੁਰੂ_Pos, ਸਥਿਤੀ)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->f_ਪੂਰਾ = ਝੂਠ;
        next();
    }
}


ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::BreakCache::preceding(int32_t ਸ਼ੁਰੂ_Pos, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (ਸ਼ੁਰੂ_Pos == f_ਪਾਠ_Idx || seek(ਸ਼ੁਰੂ_Pos) || populateNear(ਸ਼ੁਰੂ_Pos, ਸਥਿਤੀ)) {
        ਜੇ (ਸ਼ੁਰੂ_Pos == f_ਪਾਠ_Idx) {
            ਪਿਛਲਾ(ਸਥਿਤੀ);
        } ਨਹੀਂਤਾਂ {
            // seek() leaves the BreakCache positioned at the preceding boundary
            //        if the requested position is between two boundaries.
            // current() pushes the BreakCache position out to the BreakIterator itself.
            U_ASSERT(ਸ਼ੁਰੂ_Pos > f_ਪਾਠ_Idx);
            current();
        }
    }
}


/*
 * Out-of-line code for BreakCache::next().
 * Cache does not already contain the boundary
 */
ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::BreakCache::ਅਗਲਾ_OL() {
    fBI->f_ਪੂਰਾ = !populateFollowing();
    fBI->fPosition = f_ਪਾਠ_Idx;
    fBI->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = fStatuses[fBufIdx];
}


ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::BreakCache::ਪਿਛਲਾ(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    int32_t initialBufIdx = fBufIdx;
    ਜੇ (fBufIdx == f_ਸ਼ੁਰੂ_Buf_Idx) {
        // At start of cache. Prepend to it.
        populatePreceding(ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        // Cache already holds the next boundary
        fBufIdx = mod_Chunk_ਮਾਪ(fBufIdx - 1);
        f_ਪਾਠ_Idx = fBoundaries[fBufIdx];
    }
    fBI->f_ਪੂਰਾ = (fBufIdx == initialBufIdx);
    fBI->fPosition = f_ਪਾਠ_Idx;
    fBI->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = fStatuses[fBufIdx];
}


UBool ਨਿਯਮ_Based_Break_Iterator::BreakCache::seek(int32_t pos) {
    ਜੇ (pos < fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx] || pos > fBoundaries[f_ਸਿਰਾ_Buf_Idx]) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (pos == fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = f_ਸ਼ੁਰੂ_Buf_Idx;
        f_ਪਾਠ_Idx = fBoundaries[fBufIdx];
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (pos == fBoundaries[f_ਸਿਰਾ_Buf_Idx]) {
        fBufIdx = f_ਸਿਰਾ_Buf_Idx;
        f_ਪਾਠ_Idx = fBoundaries[fBufIdx];
        ਵਾਪਸ ਸੱਚ;
    }

    int32_t min = f_ਸ਼ੁਰੂ_Buf_Idx;
    int32_t max = f_ਸਿਰਾ_Buf_Idx;
    ਜਦੋਂ (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_ਮਾਪ : 0)) / 2;
        probe = mod_Chunk_ਮਾਪ(probe);
        ਜੇ (fBoundaries[probe] > pos) {
            max = probe;
        } ਨਹੀਂਤਾਂ {
            min = mod_Chunk_ਮਾਪ(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = mod_Chunk_ਮਾਪ(max - 1);
    f_ਪਾਠ_Idx = fBoundaries[fBufIdx];
    U_ASSERT(f_ਪਾਠ_Idx <= pos);
    ਵਾਪਸ ਸੱਚ;
}


UBool ਨਿਯਮ_Based_Break_Iterator::BreakCache::populateNear(int32_t position, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    U_ASSERT(position < fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx] || position > fBoundaries[f_ਸਿਰਾ_Buf_Idx]);

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
    ਸਥਿਰ constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = 0;
    ਤਾਰਕਿਕ retainCache = ਝੂਠ;
    ਜੇ ((position > fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx] - CACHE_NEAR) && position < (fBoundaries[f_ਸਿਰਾ_Buf_Idx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = ਸੱਚ;
    } ਨਹੀਂਤਾਂ ਜੇ (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = ਝੂਠ;
        aBoundary = 0;
    } ਨਹੀਂਤਾਂ {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handle_ਸੁਰੱਖਿਅਤ_ਪਿਛਲਾ(position);

        ਜੇ (fBoundaries[f_ਸਿਰਾ_Buf_Idx] < position && fBoundaries[f_ਸਿਰਾ_Buf_Idx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = ਸੱਚ;
        } ਨਹੀਂਤਾਂ ਜੇ (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx] <= (position + CACHE_NEAR));
        } ਨਹੀਂਤਾਂ {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = ਝੂਠ;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handle_ਅਗਲਾ();
            ਜੇ (aBoundary != UBRK_ਪੂਰਾ && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(&fBI->f_ਪਾਠ, aBoundary);
                ਜੇ (backupPos == utext_get_ਪਿਛਲਾ_Native_ਸੂਚਕਾਂਕ(&fBI->f_ਪਾਠ)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handle_ਅਗਲਾ();   // Safe rules identify safe pairs.
                }
            }
            ਜੇ (aBoundary == UBRK_ਪੂਰਾ) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_native_ਲੰਮਾਈ(&fBI->f_ਪਾਠ);
            }
            ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ = fBI->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ;
        }
    }

    ਜੇ (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    ਜੇ (fBoundaries[f_ਸਿਰਾ_Buf_Idx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        ਜਦੋਂ (fBoundaries[f_ਸਿਰਾ_Buf_Idx] < position) {
            ਜੇ (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = f_ਸਿਰਾ_Buf_Idx;                      // Set iterator position to the end of the buffer.
        f_ਪਾਠ_Idx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        ਜਦੋਂ (f_ਪਾਠ_Idx > position) {              // Move backwards to a position at or preceding the requested pos.
            ਪਿਛਲਾ(ਸਥਿਤੀ);
        }
        ਵਾਪਸ ਸੱਚ;
    }

    ਜੇ (fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        ਜਦੋਂ (fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx] > position) {
            populatePreceding(ਸਥਿਤੀ);
        }
        fBufIdx = f_ਸ਼ੁਰੂ_Buf_Idx;                    // Set iterator position to the start of the buffer.
        f_ਪਾਠ_Idx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        ਜਦੋਂ (f_ਪਾਠ_Idx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        ਜੇ (f_ਪਾਠ_Idx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            ਪਿਛਲਾ(ਸਥਿਤੀ);
        }
        ਵਾਪਸ ਸੱਚ;
    }

    U_ASSERT(f_ਪਾਠ_Idx == position);
    ਵਾਪਸ ਸੱਚ;
}



UBool ਨਿਯਮ_Based_Break_Iterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[f_ਸਿਰਾ_Buf_Idx];
    int32_t from_ਨਿਯਮ_ਸਥਿਤੀ_Idx = fStatuses[f_ਸਿਰਾ_Buf_Idx];
    int32_t pos = 0;
    int32_t ਨਿਯਮ_ਸਥਿਤੀ_Idx = 0;

    ਜੇ (fBI->fDictionaryCache->following(fromPosition, &pos, &ਨਿਯਮ_ਸਥਿਤੀ_Idx)) {
        ਜੋੜੋ_Following(pos, ਨਿਯਮ_ਸਥਿਤੀ_Idx, UpdateCachePosition);
        ਵਾਪਸ ਸੱਚ;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handle_ਅਗਲਾ();
    ਜੇ (pos == UBRK_ਪੂਰਾ) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਨਿਯਮ_ਸਥਿਤੀ_Idx = fBI->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ;
    ਜੇ (fBI->f_Dictionary_Char_ਗਿਣਤੀ > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, from_ਨਿਯਮ_ਸਥਿਤੀ_Idx, ਨਿਯਮ_ਸਥਿਤੀ_Idx);
        ਜੇ (fBI->fDictionaryCache->following(fromPosition, &pos, &ਨਿਯਮ_ਸਥਿਤੀ_Idx)) {
            ਜੋੜੋ_Following(pos, ਨਿਯਮ_ਸਥਿਤੀ_Idx, UpdateCachePosition);
            ਵਾਪਸ ਸੱਚ;
            // TODO: may want to move a sizable chunk of dictionary cache to break cache at this point.
            //       But be careful with interactions with populateNear().
        }
    }

    // Rule based segment did not include dictionary characters.
    // Or, it did contain dictionary chars, but the dictionary segmenter didn't handle them,
    //    meaning that we didn't take the return, above.
    // Add its end point to the cache.
    ਜੋੜੋ_Following(pos, ਨਿਯਮ_ਸਥਿਤੀ_Idx, UpdateCachePosition);

    // Add several non-dictionary boundaries at this point, to optimize straight forward iteration.
    //    (subsequent calls to BreakIterator::next() will take the fast path, getting cached results.
    //
    ਲਈ (ਪੂਰਨ count=0; count<6; ++count) {
        pos = fBI->handle_ਅਗਲਾ();
        ਜੇ (pos == UBRK_ਪੂਰਾ || fBI->f_Dictionary_Char_ਗਿਣਤੀ > 0) {
            ਰੋਕੋ;
        }
        ਜੋੜੋ_Following(pos, fBI->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ, RetainCachePosition);
    }

    ਵਾਪਸ ਸੱਚ;
}


UBool ਨਿਯਮ_Based_Break_Iterator::BreakCache::populatePreceding(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }

    int32_t fromPosition = fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx];
    ਜੇ (fromPosition == 0) {
        ਵਾਪਸ ਝੂਠ;
    }

    int32_t position = 0;
    int32_t position_ਸਥਿਤੀ_Idx = 0;

    ਜੇ (fBI->fDictionaryCache->preceding(fromPosition, &position, &position_ਸਥਿਤੀ_Idx)) {
        ਜੋੜੋ_Preceding(position, position_ਸਥਿਤੀ_Idx, UpdateCachePosition);
        ਵਾਪਸ ਸੱਚ;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    ਕਰੋ {
        backupPosition = backupPosition - 30;
        ਜੇ (backupPosition <= 0) {
            backupPosition = 0;
        } ਨਹੀਂਤਾਂ {
            backupPosition = fBI->handle_ਸੁਰੱਖਿਅਤ_ਪਿਛਲਾ(backupPosition);
        }
        ਜੇ (backupPosition == UBRK_ਪੂਰਾ || backupPosition == 0) {
            position = 0;
            position_ਸਥਿਤੀ_Idx = 0;
        } ਨਹੀਂਤਾਂ {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handle_ਅਗਲਾ();
            ਜੇ (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(&fBI->f_ਪਾਠ, position);
                ਜੇ (backupPosition == utext_get_ਪਿਛਲਾ_Native_ਸੂਚਕਾਂਕ(&fBI->f_ਪਾਠ)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handle_ਅਗਲਾ();   // Safe rules identify safe pairs.
                }
            }
            position_ਸਥਿਤੀ_Idx = fBI->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ;
        }
    } ਜਦੋਂ (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    f_Side_ਬਫਰ.removeAllElements();
    f_Side_ਬਫਰ.ਜੋੜੋ_Element(position, ਸਥਿਤੀ);
    f_Side_ਬਫਰ.ਜੋੜੋ_Element(position_ਸਥਿਤੀ_Idx, ਸਥਿਤੀ);

    ਕਰੋ {
        int32_t prevPosition = fBI->fPosition = position;
        int32_t prev_ਸਥਿਤੀ_Idx = position_ਸਥਿਤੀ_Idx;
        position = fBI->handle_ਅਗਲਾ();
        position_ਸਥਿਤੀ_Idx = fBI->f_ਨਿਯਮ_ਸਥਿਤੀ_ਸੂਚਕਾਂਕ;
        ਜੇ (position == UBRK_ਪੂਰਾ) {
            ਰੋਕੋ;
        }

        UBool segmentHandledByDictionary = ਝੂਠ;
        ਜੇ (fBI->f_Dictionary_Char_ਗਿਣਤੀ != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dict_Seg_ਸਿਰਾ_Position = position;
            fBI->fDictionaryCache->populateDictionary(prevPosition, dict_Seg_ਸਿਰਾ_Position, prev_ਸਥਿਤੀ_Idx, position_ਸਥਿਤੀ_Idx);
            ਜਦੋਂ (fBI->fDictionaryCache->following(prevPosition, &position, &position_ਸਥਿਤੀ_Idx)) {
                segmentHandledByDictionary = ਸੱਚ;
                U_ASSERT(position > prevPosition);
                ਜੇ (position >= fromPosition) {
                    ਰੋਕੋ;
                }
                U_ASSERT(position <= dict_Seg_ਸਿਰਾ_Position);
                f_Side_ਬਫਰ.ਜੋੜੋ_Element(position, ਸਥਿਤੀ);
                f_Side_ਬਫਰ.ਜੋੜੋ_Element(position_ਸਥਿਤੀ_Idx, ਸਥਿਤੀ);
                prevPosition = position;
            }
            U_ASSERT(position==dict_Seg_ਸਿਰਾ_Position || position>=fromPosition);
        }

        ਜੇ (!segmentHandledByDictionary && position < fromPosition) {
            f_Side_ਬਫਰ.ਜੋੜੋ_Element(position, ਸਥਿਤੀ);
            f_Side_ਬਫਰ.ਜੋੜੋ_Element(position_ਸਥਿਤੀ_Idx, ਸਥਿਤੀ);
        }
    } ਜਦੋਂ (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = ਝੂਠ;
    ਜੇ (!f_Side_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        position_ਸਥਿਤੀ_Idx = f_Side_ਬਫਰ.popi();
        position = f_Side_ਬਫਰ.popi();
        ਜੋੜੋ_Preceding(position, position_ਸਥਿਤੀ_Idx, UpdateCachePosition);
        success = ਸੱਚ;
    }

    ਜਦੋਂ (!f_Side_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        position_ਸਥਿਤੀ_Idx = f_Side_ਬਫਰ.popi();
        position = f_Side_ਬਫਰ.popi();
        ਜੇ (!ਜੋੜੋ_Preceding(position, position_ਸਥਿਤੀ_Idx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            ਰੋਕੋ;
        }
    }

    ਵਾਪਸ success;
}


ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::BreakCache::ਜੋੜੋ_Following(int32_t position, int32_t ਨਿਯਮ_ਸਥਿਤੀ_Idx, Update_Position_ਮੁੱਲਾਂ update) {
    U_ASSERT(position > fBoundaries[f_ਸਿਰਾ_Buf_Idx]);
    U_ASSERT(ਨਿਯਮ_ਸਥਿਤੀ_Idx <= UINT16_MAX);
    int32_t ਅਗਲਾ_Idx = mod_Chunk_ਮਾਪ(f_ਸਿਰਾ_Buf_Idx + 1);
    ਜੇ (ਅਗਲਾ_Idx == f_ਸ਼ੁਰੂ_Buf_Idx) {
        f_ਸ਼ੁਰੂ_Buf_Idx = mod_Chunk_ਮਾਪ(f_ਸ਼ੁਰੂ_Buf_Idx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[ਅਗਲਾ_Idx] = position;
    fStatuses[ਅਗਲਾ_Idx] = static_cast<uint16_t>(ਨਿਯਮ_ਸਥਿਤੀ_Idx);
    f_ਸਿਰਾ_Buf_Idx = ਅਗਲਾ_Idx;
    ਜੇ (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = ਅਗਲਾ_Idx;
        f_ਪਾਠ_Idx = position;
    } ਨਹੀਂਤਾਂ {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(ਅਗਲਾ_Idx != fBufIdx);
    }
}

ਤਾਰਕਿਕ ਨਿਯਮ_Based_Break_Iterator::BreakCache::ਜੋੜੋ_Preceding(int32_t position, int32_t ਨਿਯਮ_ਸਥਿਤੀ_Idx, Update_Position_ਮੁੱਲਾਂ update) {
    U_ASSERT(position < fBoundaries[f_ਸ਼ੁਰੂ_Buf_Idx]);
    U_ASSERT(ਨਿਯਮ_ਸਥਿਤੀ_Idx <= UINT16_MAX);
    int32_t ਅਗਲਾ_Idx = mod_Chunk_ਮਾਪ(f_ਸ਼ੁਰੂ_Buf_Idx - 1);
    ਜੇ (ਅਗਲਾ_Idx == f_ਸਿਰਾ_Buf_Idx) {
        ਜੇ (fBufIdx == f_ਸਿਰਾ_Buf_Idx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            ਵਾਪਸ ਝੂਠ;
        }
        f_ਸਿਰਾ_Buf_Idx = mod_Chunk_ਮਾਪ(f_ਸਿਰਾ_Buf_Idx - 1);
    }
    fBoundaries[ਅਗਲਾ_Idx] = position;
    fStatuses[ਅਗਲਾ_Idx] = static_cast<uint16_t>(ਨਿਯਮ_ਸਥਿਤੀ_Idx);
    f_ਸ਼ੁਰੂ_Buf_Idx = ਅਗਲਾ_Idx;
    ਜੇ (update == UpdateCachePosition) {
        fBufIdx = ਅਗਲਾ_Idx;
        f_ਪਾਠ_Idx = position;
    }
    ਵਾਪਸ ਸੱਚ;
}


ਖਾਲੀ ਨਿਯਮ_Based_Break_Iterator::BreakCache::dumpCache() {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", f_ਪਾਠ_Idx, fBufIdx);
    ਲਈ (int32_t i=f_ਸ਼ੁਰੂ_Buf_Idx; ; i=mod_Chunk_ਮਾਪ(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        ਜੇ (i == f_ਸਿਰਾ_Buf_Idx) {
            ਰੋਕੋ;
        }
    }
#ਜੇ_ਅੰਤ
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // #if !UCONFIG_NO_BREAK_ITERATION
