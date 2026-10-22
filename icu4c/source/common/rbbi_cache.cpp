// Copyright (C) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// file: rbbi_cache.cpp

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"

#ಅನ್ತರ್ಭಾವ "rbbi_cache.h"

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "rbbidata.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

/*
 * DictionaryCache implementation
 */

ನಿಯಮ_Based_Break_Iterator::DictionaryCache::DictionaryCache(ನಿಯಮ_Based_Break_Iterator *bi, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
        fBI(bi), fBreaks(ಸ್ಥಿತಿ_2), f_Position_ಒಳ_Cache(-1),
        f_ಆರಂಭ(0), f_ಎಲ್ಲೆ(0), f_ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ(0), f_Other_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ(0) {
}

ನಿಯಮ_Based_Break_Iterator::DictionaryCache::~DictionaryCache() {
}

ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::DictionaryCache::reset() {
    f_Position_ಒಳ_Cache = -1;
    f_ಆರಂಭ = 0;
    f_ಎಲ್ಲೆ = 0;
    f_ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = 0;
    f_Other_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = 0;
    fBreaks.removeAllElements();
    int32_t max_ಸ್ಥಿತಿ = 0;
    ಗಾಗಿ (int32_t i = 0; i < fBI->f_ದತ್ತಾಂಶ->f_ಸ್ಥಿತಿ_Max_Idx; ++i) {
        max_ಸ್ಥಿತಿ = std::max(max_ಸ್ಥಿತಿ, fBI->f_ದತ್ತಾಂಶ->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ[i]);
    }
    is_ಶಬ್ದ_Like = max_ಸ್ಥಿತಿ > 100;
}

UBool ನಿಯಮ_Based_Break_Iterator::DictionaryCache::following(int32_t fromPos, int32_t *ಫಲಿತಾಂಶ, int32_t *ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ) {
    ಆದರೆ (fromPos >= f_ಎಲ್ಲೆ || fromPos < f_ಆರಂಭ) {
        f_Position_ಒಳ_Cache = -1;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    // Sequential iteration, move from previous boundary to the following

    int32_t r = 0;
    ಆದರೆ (f_Position_ಒಳ_Cache >= 0 && f_Position_ಒಳ_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_ಒಳ_Cache) == fromPos) {
        ++f_Position_ಒಳ_Cache;
        ಆದರೆ (f_Position_ಒಳ_Cache >= fBreaks.size()) {
            f_Position_ಒಳ_Cache = -1;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        r = fBreaks.elementAti(f_Position_ಒಳ_Cache);
        U_ASSERT(r > fromPos);
        *ಫಲಿತಾಂಶ = r;
        *ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = is_ಶಬ್ದ_Like || r == f_ಎಲ್ಲೆ ? f_Other_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ : 0;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    // Random indexing. Linear search for the boundary following the given position.

    ಗಾಗಿ (f_Position_ಒಳ_Cache = 0; f_Position_ಒಳ_Cache < fBreaks.size(); ++f_Position_ಒಳ_Cache) {
        r= fBreaks.elementAti(f_Position_ಒಳ_Cache);
        ಆದರೆ (r > fromPos) {
            *ಫಲಿತಾಂಶ = r;
            *ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = is_ಶಬ್ದ_Like || r == f_ಎಲ್ಲೆ ? f_Other_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ : 0;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}


UBool ನಿಯಮ_Based_Break_Iterator::DictionaryCache::preceding(int32_t fromPos, int32_t *ಫಲಿತಾಂಶ, int32_t *ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ) {
    ಆದರೆ (fromPos <= f_ಆರಂಭ || fromPos > f_ಎಲ್ಲೆ) {
        f_Position_ಒಳ_Cache = -1;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ (fromPos == f_ಎಲ್ಲೆ) {
        f_Position_ಒಳ_Cache = fBreaks.size() - 1;
        ಆದರೆ (f_Position_ಒಳ_Cache >= 0) {
            U_ASSERT(fBreaks.elementAti(f_Position_ಒಳ_Cache) == fromPos);
        }
    }

    int32_t r;
    ಆದರೆ (f_Position_ಒಳ_Cache > 0 && f_Position_ಒಳ_Cache < fBreaks.size() && fBreaks.elementAti(f_Position_ಒಳ_Cache) == fromPos) {
        --f_Position_ಒಳ_Cache;
        r = fBreaks.elementAti(f_Position_ಒಳ_Cache);
        U_ASSERT(r < fromPos);
        *ಫಲಿತಾಂಶ = r;
        *ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = (r == f_ಆರಂಭ) ? f_ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ
                       : is_ಶಬ್ದ_Like  ? f_Other_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ
                                     : 0;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಆದರೆ (f_Position_ಒಳ_Cache == 0) {
        f_Position_ಒಳ_Cache = -1;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಗಾಗಿ (f_Position_ಒಳ_Cache = fBreaks.size()-1; f_Position_ಒಳ_Cache >= 0; --f_Position_ಒಳ_Cache) {
        r = fBreaks.elementAti(f_Position_ಒಳ_Cache);
        ಆದರೆ (r < fromPos) {
            *ಫಲಿತಾಂಶ = r;
            *ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = (r == f_ಆರಂಭ) ? f_ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ
                           : is_ಶಬ್ದ_Like  ? f_Other_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ
                                         : 0;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    UPRV_UNREACHABLE_EXIT;
}

ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::DictionaryCache::populateDictionary(int32_t ಆರಂಭ_Pos, int32_t ಕೊನೆ_Pos,
                                       int32_t ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ, int32_t other_ನಿಯಮ_ಸ್ಥಿತಿ) {
    ಆದರೆ ((ಕೊನೆ_Pos - ಆರಂಭ_Pos) <= 1) {
        ಹಿಂತಿರುಗಿ;
    }

    reset();
    f_ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = ಮೊದಲ_ನಿಯಮ_ಸ್ಥಿತಿ;
    f_Other_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = other_ನಿಯಮ_ಸ್ಥಿತಿ;

    int32_t range_ಆರಂಭ = ಆರಂಭ_Pos;
    int32_t range_ಕೊನೆ = ಕೊನೆ_Pos;

    uint16_t    category;
    int32_t     current;
    UError_ಸಂಕೇತ  ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    int32_t     ಸಿಕ್ಕಿತು_Break_ಎಣಿಕೆ = 0;
    UText      *ಪಠ್ಯ = &fBI->f_ಪಠ್ಯ;

    // Loop through the text, looking for ranges of dictionary characters.
    // For each span, find the appropriate break engine, and ask it to find
    // any breaks within the span.

    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, range_ಆರಂಭ);
    UChar32     c = utext_current32(ಪಠ್ಯ);
    category = ucptrie_get(fBI->f_ದತ್ತಾಂಶ->fTrie, c);
    uint32_t dict_ಆರಂಭ = fBI->f_ದತ್ತಾಂಶ->f_Forward_ಕೋಷ್ಟಕ->f_Dict_Categories_ಆರಂಭ;

    ಆಗ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಆಗ ((current = static_cast<int32_t>(UTEXT_GETNATIVEINDEX(ಪಠ್ಯ))) < range_ಕೊನೆ
                && (category < dict_ಆರಂಭ)) {
            utext_next32(ಪಠ್ಯ);           // TODO: cleaner loop structure.
            c = utext_current32(ಪಠ್ಯ);
            category = ucptrie_get(fBI->f_ದತ್ತಾಂಶ->fTrie, c);
        }
        ಆದರೆ (current >= range_ಕೊನೆ) {
            ನಿಲ್ಲಿಸು;
        }

        // We now have a dictionary character. Get the appropriate language object
        // to deal with it.
        ಸ್ಥಿರಾಂಕ LanguageBreakEngine *lbe = fBI->getLanguageBreakEngine(
            c, fBI->getLocaleID(ULOC_REQUESTED_LOCALE, ಸ್ಥಿತಿ_2));

        // Ask the language object if there are any breaks. It will add them to the cache and
        // leave the text pointer on the other side of its range, ready to search for the next one.
        ಆದರೆ (lbe != ಶೂನ್ಯಸೂಚಕ) {
            ಸಿಕ್ಕಿತು_Break_ಎಣಿಕೆ += lbe->ಕಂಡುಹಿಡಿ_Breaks(ಪಠ್ಯ, current, range_ಕೊನೆ, fBreaks, fBI->fIsPhraseBreaking, ಸ್ಥಿತಿ_2);
        }

        // Reload the loop variables for the next go-round
        c = utext_current32(ಪಠ್ಯ);
        category = ucptrie_get(fBI->f_ದತ್ತಾಂಶ->fTrie, c);
    }

    // If we found breaks, ensure that the first and last entries are
    // the original starting and ending position. And initialize the
    // cache iteration position to the first entry.

    // printf("foundBreakCount = %d\n", foundBreakCount);
    ಆದರೆ (ಸಿಕ್ಕಿತು_Break_ಎಣಿಕೆ > 0) {
        U_ASSERT(ಸಿಕ್ಕಿತು_Break_ಎಣಿಕೆ == fBreaks.size());
        ಆದರೆ (ಆರಂಭ_Pos < fBreaks.elementAti(0)) {
            // The dictionary did not place a boundary at the start of the segment of text.
            // Add one now. This should not commonly happen, but it would be easy for interactions
            // of the rules for dictionary segments and the break engine implementations to
            // inadvertently cause it. Cover it here, just in case.
            fBreaks.ಸೇರಿಸು_Element_At(ಆರಂಭ_Pos, 0, ಸ್ಥಿತಿ_2);
        }
        ಆದರೆ (ಕೊನೆ_Pos > fBreaks.peeki()) {
            fBreaks.push(ಕೊನೆ_Pos, ಸ್ಥಿತಿ_2);
        }
        f_Position_ಒಳ_Cache = 0;
        // Note: Dictionary matching may extend beyond the original limit.
        f_ಆರಂಭ = fBreaks.elementAti(0);
        f_ಎಲ್ಲೆ = fBreaks.peeki();
    } ಇಲ್ಲದಿದ್ದರೆ {
        // there were no language-based breaks, even though the segment contained
        // dictionary characters. Subsequent attempts to fetch boundaries from the dictionary cache
        // for this range will fail, and the calling code will fall back to the rule based boundaries.
    }
}


/*
 *   BreakCache implementation
 */

ನಿಯಮ_Based_Break_Iterator::BreakCache::BreakCache(ನಿಯಮ_Based_Break_Iterator *bi, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
        fBI(bi), f_Side_ಬಫರ್(ಸ್ಥಿತಿ_2) {
    reset();
}


ನಿಯಮ_Based_Break_Iterator::BreakCache::~BreakCache() {
}


ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::BreakCache::reset(int32_t pos, int32_t ನಿಯಮ_ಸ್ಥಿತಿ) {
    f_ಆರಂಭ_Buf_Idx = 0;
    f_ಕೊನೆ_Buf_Idx = 0;
    f_ಪಠ್ಯ_Idx = pos;
    fBufIdx = 0;
    fBoundaries[0] = pos;
    fStatuses[0] = static_cast<uint16_t>(ನಿಯಮ_ಸ್ಥಿತಿ);
}


int32_t  ನಿಯಮ_Based_Break_Iterator::BreakCache::current() {
    fBI->fPosition = f_ಪಠ್ಯ_Idx;
    fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = fStatuses[fBufIdx];
    fBI->f_ಮುಗಿದ = ಸುಳ್ಳು;
    ಹಿಂತಿರುಗಿ f_ಪಠ್ಯ_Idx;
}


ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::BreakCache::following(int32_t ಆರಂಭ_Pos, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಆರಂಭ_Pos == f_ಪಠ್ಯ_Idx || seek(ಆರಂಭ_Pos) || populateNear(ಆರಂಭ_Pos, ಸ್ಥಿತಿ_2)) {
        // startPos is in the cache. Do a next() from that position.
        // TODO: an awkward set of interactions with bi->fDone
        //       seek() does not clear it; it can't because of interactions with populateNear().
        //       next() does not clear it in the fast-path case, where everything matters. Maybe it should.
        //       So clear it here, for the case where seek() succeeded on an iterator that had previously run off the end.
        fBI->f_ಮುಗಿದ = ಸುಳ್ಳು;
        next();
    }
}


ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::BreakCache::preceding(int32_t ಆರಂಭ_Pos, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಆರಂಭ_Pos == f_ಪಠ್ಯ_Idx || seek(ಆರಂಭ_Pos) || populateNear(ಆರಂಭ_Pos, ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (ಆರಂಭ_Pos == f_ಪಠ್ಯ_Idx) {
            ಹಿಂದಿನ(ಸ್ಥಿತಿ_2);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // seek() leaves the BreakCache positioned at the preceding boundary
            //        if the requested position is between two boundaries.
            // current() pushes the BreakCache position out to the BreakIterator itself.
            U_ASSERT(ಆರಂಭ_Pos > f_ಪಠ್ಯ_Idx);
            current();
        }
    }
}


/*
 * Out-of-line code for BreakCache::next().
 * Cache does not already contain the boundary
 */
ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::BreakCache::ಮುಂದಿನ_OL() {
    fBI->f_ಮುಗಿದ = !populateFollowing();
    fBI->fPosition = f_ಪಠ್ಯ_Idx;
    fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = fStatuses[fBufIdx];
}


ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::BreakCache::ಹಿಂದಿನ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    int32_t initialBufIdx = fBufIdx;
    ಆದರೆ (fBufIdx == f_ಆರಂಭ_Buf_Idx) {
        // At start of cache. Prepend to it.
        populatePreceding(ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Cache already holds the next boundary
        fBufIdx = mod_Chunk_ಗಾತ್ರ(fBufIdx - 1);
        f_ಪಠ್ಯ_Idx = fBoundaries[fBufIdx];
    }
    fBI->f_ಮುಗಿದ = (fBufIdx == initialBufIdx);
    fBI->fPosition = f_ಪಠ್ಯ_Idx;
    fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = fStatuses[fBufIdx];
}


UBool ನಿಯಮ_Based_Break_Iterator::BreakCache::seek(int32_t pos) {
    ಆದರೆ (pos < fBoundaries[f_ಆರಂಭ_Buf_Idx] || pos > fBoundaries[f_ಕೊನೆ_Buf_Idx]) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (pos == fBoundaries[f_ಆರಂಭ_Buf_Idx]) {
        // Common case: seek(0), from BreakIterator::first()
        fBufIdx = f_ಆರಂಭ_Buf_Idx;
        f_ಪಠ್ಯ_Idx = fBoundaries[fBufIdx];
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (pos == fBoundaries[f_ಕೊನೆ_Buf_Idx]) {
        fBufIdx = f_ಕೊನೆ_Buf_Idx;
        f_ಪಠ್ಯ_Idx = fBoundaries[fBufIdx];
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    int32_t min = f_ಆರಂಭ_Buf_Idx;
    int32_t max = f_ಕೊನೆ_Buf_Idx;
    ಆಗ (min != max) {
        int32_t probe = (min + max + (min>max ? CACHE_ಗಾತ್ರ : 0)) / 2;
        probe = mod_Chunk_ಗಾತ್ರ(probe);
        ಆದರೆ (fBoundaries[probe] > pos) {
            max = probe;
        } ಇಲ್ಲದಿದ್ದರೆ {
            min = mod_Chunk_ಗಾತ್ರ(probe + 1);
        }
    }
    U_ASSERT(fBoundaries[max] > pos);
    fBufIdx = mod_Chunk_ಗಾತ್ರ(max - 1);
    f_ಪಠ್ಯ_Idx = fBoundaries[fBufIdx];
    U_ASSERT(f_ಪಠ್ಯ_Idx <= pos);
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


UBool ನಿಯಮ_Based_Break_Iterator::BreakCache::populateNear(int32_t position, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    U_ASSERT(position < fBoundaries[f_ಆರಂಭ_Buf_Idx] || position > fBoundaries[f_ಕೊನೆ_Buf_Idx]);

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
    ಸ್ಥಿರ constexpr int32_t CACHE_NEAR = 15;

    int32_t aBoundary = -1;
    int32_t ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = 0;
    ತಾರ್ಕಿಕ retainCache = ಸುಳ್ಳು;
    ಆದರೆ ((position > fBoundaries[f_ಆರಂಭ_Buf_Idx] - CACHE_NEAR) && position < (fBoundaries[f_ಕೊನೆ_Buf_Idx] + CACHE_NEAR)) {
        // Requested position is near the existing cache. Retain it.
        retainCache = ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (position <= CACHE_NEAR) {
        // Requested position is near the start of the text. Fill cache from start, skipping
        // the need to find a safe point.
        retainCache = ಸುಳ್ಳು;
        aBoundary = 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Requested position is not near the existing cache.
        // Find a safe point to refill the cache from.
        int32_t backupPos = fBI->handle_ಸುರಕ್ಷಿತ_ಹಿಂದಿನ(position);

        ಆದರೆ (fBoundaries[f_ಕೊನೆ_Buf_Idx] < position && fBoundaries[f_ಕೊನೆ_Buf_Idx] >= (backupPos - CACHE_NEAR)) {
            // The requested position is beyond the end of the existing cache, but the
            // reverse rules produced a position near or before the cached region.
            // Retain the existing cache, and fill from the end of it.
            retainCache = ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (backupPos < CACHE_NEAR) {
            // The safe reverse rules moved us to near the start of text.
            // Take that (index 0) as the backup boundary, avoiding the complication
            // (in the following block) of moving forward from the safe point to a known boundary.
            //
            // Retain the cache if it begins not too far from the requested position.
            aBoundary = 0;
            retainCache = (fBoundaries[f_ಆರಂಭ_Buf_Idx] <= (position + CACHE_NEAR));
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The safe reverse rules produced a position that is neither near the existing
            // cache, nor near the start of text.
            // Advance to the boundary following.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            retainCache = ಸುಳ್ಳು;
            fBI->fPosition = backupPos;
            aBoundary = fBI->handle_ಮುಂದಿನ();
            ಆದರೆ (aBoundary != UBRK_ಮುಗಿದ && aBoundary <= backupPos + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(&fBI->f_ಪಠ್ಯ, aBoundary);
                ಆದರೆ (backupPos == utext_get_ಹಿಂದಿನ_Native_ಸೂಚ್ಯಂಕ(&fBI->f_ಪಠ್ಯ)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    aBoundary = fBI->handle_ಮುಂದಿನ();   // Safe rules identify safe pairs.
                }
            }
            ಆದರೆ (aBoundary == UBRK_ಮುಗಿದ) {
                // Note (Andy Heninger): I don't think this condition can occur, but it's hard
                // to prove that it can't. We ran off the end of the string looking a boundary
                // following a safe point; choose the end of the string as that boundary.
                aBoundary = utext_native_ಉದ್ದಳತೆ(&fBI->f_ಪಠ್ಯ);
            }
            ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ = fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ;
        }
    }

    ಆದರೆ (!retainCache) {
        U_ASSERT(aBoundary != -1);
        reset(aBoundary, ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ);        // Reset cache to hold aBoundary as a single starting point.
    }

    // Fill in boundaries between existing cache content and the new requested position.

    ಆದರೆ (fBoundaries[f_ಕೊನೆ_Buf_Idx] < position) {
        // The last position in the cache precedes the requested position.
        // Add following position(s) to the cache.
        ಆಗ (fBoundaries[f_ಕೊನೆ_Buf_Idx] < position) {
            ಆದರೆ (!populateFollowing()) {
                UPRV_UNREACHABLE_EXIT;
            }
        }
        fBufIdx = f_ಕೊನೆ_Buf_Idx;                      // Set iterator position to the end of the buffer.
        f_ಪಠ್ಯ_Idx = fBoundaries[fBufIdx];           // Required because populateFollowing may add extra boundaries.
        ಆಗ (f_ಪಠ್ಯ_Idx > position) {              // Move backwards to a position at or preceding the requested pos.
            ಹಿಂದಿನ(ಸ್ಥಿತಿ_2);
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಆದರೆ (fBoundaries[f_ಆರಂಭ_Buf_Idx] > position) {
        // The first position in the cache is beyond the requested position.
        // back up more until we get a boundary <= the requested position.
        ಆಗ (fBoundaries[f_ಆರಂಭ_Buf_Idx] > position) {
            populatePreceding(ಸ್ಥಿತಿ_2);
        }
        fBufIdx = f_ಆರಂಭ_Buf_Idx;                    // Set iterator position to the start of the buffer.
        f_ಪಠ್ಯ_Idx = fBoundaries[fBufIdx];           // Required because populatePreceding may add extra boundaries.
        ಆಗ (f_ಪಠ್ಯ_Idx < position) {              // Move forwards to a position at or following the requested pos.
            next();
        }
        ಆದರೆ (f_ಪಠ್ಯ_Idx > position) {
            // If position is not itself a boundary, the next() loop above will overshoot.
            // Back up one, leaving cache position at the boundary preceding the requested position.
            ಹಿಂದಿನ(ಸ್ಥಿತಿ_2);
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    U_ASSERT(f_ಪಠ್ಯ_Idx == position);
    ಹಿಂತಿರುಗಿ ನಿಜ;
}



UBool ನಿಯಮ_Based_Break_Iterator::BreakCache::populateFollowing() {
    int32_t fromPosition = fBoundaries[f_ಕೊನೆ_Buf_Idx];
    int32_t from_ನಿಯಮ_ಸ್ಥಿತಿ_Idx = fStatuses[f_ಕೊನೆ_Buf_Idx];
    int32_t pos = 0;
    int32_t ನಿಯಮ_ಸ್ಥಿತಿ_Idx = 0;

    ಆದರೆ (fBI->fDictionaryCache->following(fromPosition, &pos, &ನಿಯಮ_ಸ್ಥಿತಿ_Idx)) {
        ಕೂಡಿಸು_Following(pos, ನಿಯಮ_ಸ್ಥಿತಿ_Idx, UpdateCachePosition);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    fBI->fPosition = fromPosition;
    pos = fBI->handle_ಮುಂದಿನ();
    ಆದರೆ (pos == UBRK_ಮುಗಿದ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ನಿಯಮ_ಸ್ಥಿತಿ_Idx = fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ;
    ಆದರೆ (fBI->f_Dictionary_Char_ಎಣಿಕೆ > 0) {
        // The text segment obtained from the rules includes dictionary characters.
        // Subdivide it, with subdivided results going into the dictionary cache.
        fBI->fDictionaryCache->populateDictionary(fromPosition, pos, from_ನಿಯಮ_ಸ್ಥಿತಿ_Idx, ನಿಯಮ_ಸ್ಥಿತಿ_Idx);
        ಆದರೆ (fBI->fDictionaryCache->following(fromPosition, &pos, &ನಿಯಮ_ಸ್ಥಿತಿ_Idx)) {
            ಕೂಡಿಸು_Following(pos, ನಿಯಮ_ಸ್ಥಿತಿ_Idx, UpdateCachePosition);
            ಹಿಂತಿರುಗಿ ನಿಜ;
            // TODO: may want to move a sizable chunk of dictionary cache to break cache at this point.
            //       But be careful with interactions with populateNear().
        }
    }

    // Rule based segment did not include dictionary characters.
    // Or, it did contain dictionary chars, but the dictionary segmenter didn't handle them,
    //    meaning that we didn't take the return, above.
    // Add its end point to the cache.
    ಕೂಡಿಸು_Following(pos, ನಿಯಮ_ಸ್ಥಿತಿ_Idx, UpdateCachePosition);

    // Add several non-dictionary boundaries at this point, to optimize straight forward iteration.
    //    (subsequent calls to BreakIterator::next() will take the fast path, getting cached results.
    //
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ count=0; count<6; ++count) {
        pos = fBI->handle_ಮುಂದಿನ();
        ಆದರೆ (pos == UBRK_ಮುಗಿದ || fBI->f_Dictionary_Char_ಎಣಿಕೆ > 0) {
            ನಿಲ್ಲಿಸು;
        }
        ಕೂಡಿಸು_Following(pos, fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ, RetainCachePosition);
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}


UBool ನಿಯಮ_Based_Break_Iterator::BreakCache::populatePreceding(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    int32_t fromPosition = fBoundaries[f_ಆರಂಭ_Buf_Idx];
    ಆದರೆ (fromPosition == 0) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    int32_t position = 0;
    int32_t position_ಸ್ಥಿತಿ_Idx = 0;

    ಆದರೆ (fBI->fDictionaryCache->preceding(fromPosition, &position, &position_ಸ್ಥಿತಿ_Idx)) {
        ಕೂಡಿಸು_Preceding(position, position_ಸ್ಥಿತಿ_Idx, UpdateCachePosition);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    int32_t backupPosition = fromPosition;

    // Find a boundary somewhere preceding the first already-cached boundary
    ಮಾಡು {
        backupPosition = backupPosition - 30;
        ಆದರೆ (backupPosition <= 0) {
            backupPosition = 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            backupPosition = fBI->handle_ಸುರಕ್ಷಿತ_ಹಿಂದಿನ(backupPosition);
        }
        ಆದರೆ (backupPosition == UBRK_ಮುಗಿದ || backupPosition == 0) {
            position = 0;
            position_ಸ್ಥಿತಿ_Idx = 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Advance to the boundary following the backup position.
            // There is a complication: the safe reverse rules identify pairs of code points
            // that are safe. If advancing from the safe point moves forwards by less than
            // two code points, we need to advance one more time to ensure that the boundary
            // is good, including a correct rules status value.
            //
            fBI->fPosition = backupPosition;
            position = fBI->handle_ಮುಂದಿನ();
            ಆದರೆ (position <= backupPosition + 4) {
                // +4 is a quick test for possibly having advanced only one codepoint.
                // Four being the length of the longest potential code point, a supplementary in UTF-8
                utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(&fBI->f_ಪಠ್ಯ, position);
                ಆದರೆ (backupPosition == utext_get_ಹಿಂದಿನ_Native_ಸೂಚ್ಯಂಕ(&fBI->f_ಪಠ್ಯ)) {
                    // The initial handleNext() only advanced by a single code point. Go again.
                    position = fBI->handle_ಮುಂದಿನ();   // Safe rules identify safe pairs.
                }
            }
            position_ಸ್ಥಿತಿ_Idx = fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ;
        }
    } ಆಗ (position >= fromPosition);

    // Find boundaries between the one we just located and the first already-cached boundary
    // Put them in a side buffer, because we don't yet know where they will fall in the circular cache buffer..

    f_Side_ಬಫರ್.removeAllElements();
    f_Side_ಬಫರ್.ಕೂಡಿಸು_Element(position, ಸ್ಥಿತಿ_2);
    f_Side_ಬಫರ್.ಕೂಡಿಸು_Element(position_ಸ್ಥಿತಿ_Idx, ಸ್ಥಿತಿ_2);

    ಮಾಡು {
        int32_t prevPosition = fBI->fPosition = position;
        int32_t prev_ಸ್ಥಿತಿ_Idx = position_ಸ್ಥಿತಿ_Idx;
        position = fBI->handle_ಮುಂದಿನ();
        position_ಸ್ಥಿತಿ_Idx = fBI->f_ನಿಯಮ_ಸ್ಥಿತಿ_ಸೂಚ್ಯಂಕ;
        ಆದರೆ (position == UBRK_ಮುಗಿದ) {
            ನಿಲ್ಲಿಸು;
        }

        UBool segmentHandledByDictionary = ಸುಳ್ಳು;
        ಆದರೆ (fBI->f_Dictionary_Char_ಎಣಿಕೆ != 0) {
            // Segment from the rules includes dictionary characters.
            // Subdivide it, with subdivided results going into the dictionary cache.
            int32_t dict_Seg_ಕೊನೆ_Position = position;
            fBI->fDictionaryCache->populateDictionary(prevPosition, dict_Seg_ಕೊನೆ_Position, prev_ಸ್ಥಿತಿ_Idx, position_ಸ್ಥಿತಿ_Idx);
            ಆಗ (fBI->fDictionaryCache->following(prevPosition, &position, &position_ಸ್ಥಿತಿ_Idx)) {
                segmentHandledByDictionary = ನಿಜ;
                U_ASSERT(position > prevPosition);
                ಆದರೆ (position >= fromPosition) {
                    ನಿಲ್ಲಿಸು;
                }
                U_ASSERT(position <= dict_Seg_ಕೊನೆ_Position);
                f_Side_ಬಫರ್.ಕೂಡಿಸು_Element(position, ಸ್ಥಿತಿ_2);
                f_Side_ಬಫರ್.ಕೂಡಿಸು_Element(position_ಸ್ಥಿತಿ_Idx, ಸ್ಥಿತಿ_2);
                prevPosition = position;
            }
            U_ASSERT(position==dict_Seg_ಕೊನೆ_Position || position>=fromPosition);
        }

        ಆದರೆ (!segmentHandledByDictionary && position < fromPosition) {
            f_Side_ಬಫರ್.ಕೂಡಿಸು_Element(position, ಸ್ಥಿತಿ_2);
            f_Side_ಬಫರ್.ಕೂಡಿಸು_Element(position_ಸ್ಥಿತಿ_Idx, ಸ್ಥಿತಿ_2);
        }
    } ಆಗ (position < fromPosition);

    // Move boundaries from the side buffer to the main circular buffer.
    UBool success = ಸುಳ್ಳು;
    ಆದರೆ (!f_Side_ಬಫರ್.is_ಖಾಲಿಗಳು()) {
        position_ಸ್ಥಿತಿ_Idx = f_Side_ಬಫರ್.popi();
        position = f_Side_ಬಫರ್.popi();
        ಕೂಡಿಸು_Preceding(position, position_ಸ್ಥಿತಿ_Idx, UpdateCachePosition);
        success = ನಿಜ;
    }

    ಆಗ (!f_Side_ಬಫರ್.is_ಖಾಲಿಗಳು()) {
        position_ಸ್ಥಿತಿ_Idx = f_Side_ಬಫರ್.popi();
        position = f_Side_ಬಫರ್.popi();
        ಆದರೆ (!ಕೂಡಿಸು_Preceding(position, position_ಸ್ಥಿತಿ_Idx, RetainCachePosition)) {
            // No space in circular buffer to hold a new preceding result while
            // also retaining the current cache (iteration) position.
            // Bailing out is safe; the cache will refill again if needed.
            ನಿಲ್ಲಿಸು;
        }
    }

    ಹಿಂತಿರುಗಿ success;
}


ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::BreakCache::ಕೂಡಿಸು_Following(int32_t position, int32_t ನಿಯಮ_ಸ್ಥಿತಿ_Idx, Update_Position_ಮೌಲ್ಯಗಳು update) {
    U_ASSERT(position > fBoundaries[f_ಕೊನೆ_Buf_Idx]);
    U_ASSERT(ನಿಯಮ_ಸ್ಥಿತಿ_Idx <= UINT16_MAX);
    int32_t ಮುಂದಿನ_Idx = mod_Chunk_ಗಾತ್ರ(f_ಕೊನೆ_Buf_Idx + 1);
    ಆದರೆ (ಮುಂದಿನ_Idx == f_ಆರಂಭ_Buf_Idx) {
        f_ಆರಂಭ_Buf_Idx = mod_Chunk_ಗಾತ್ರ(f_ಆರಂಭ_Buf_Idx + 6);    // TODO: experiment. Probably revert to 1.
    }
    fBoundaries[ಮುಂದಿನ_Idx] = position;
    fStatuses[ಮುಂದಿನ_Idx] = static_cast<uint16_t>(ನಿಯಮ_ಸ್ಥಿತಿ_Idx);
    f_ಕೊನೆ_Buf_Idx = ಮುಂದಿನ_Idx;
    ಆದರೆ (update == UpdateCachePosition) {
        // Set current position to the newly added boundary.
        fBufIdx = ಮುಂದಿನ_Idx;
        f_ಪಠ್ಯ_Idx = position;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Retaining the original cache position.
        // Check if the added boundary wraps around the buffer, and would over-write the original position.
        // It's the responsibility of callers of this function to not add too many.
        U_ASSERT(ಮುಂದಿನ_Idx != fBufIdx);
    }
}

ತಾರ್ಕಿಕ ನಿಯಮ_Based_Break_Iterator::BreakCache::ಕೂಡಿಸು_Preceding(int32_t position, int32_t ನಿಯಮ_ಸ್ಥಿತಿ_Idx, Update_Position_ಮೌಲ್ಯಗಳು update) {
    U_ASSERT(position < fBoundaries[f_ಆರಂಭ_Buf_Idx]);
    U_ASSERT(ನಿಯಮ_ಸ್ಥಿತಿ_Idx <= UINT16_MAX);
    int32_t ಮುಂದಿನ_Idx = mod_Chunk_ಗಾತ್ರ(f_ಆರಂಭ_Buf_Idx - 1);
    ಆದರೆ (ಮುಂದಿನ_Idx == f_ಕೊನೆ_Buf_Idx) {
        ಆದರೆ (fBufIdx == f_ಕೊನೆ_Buf_Idx && update == RetainCachePosition) {
            // Failure. The insertion of the new boundary would claim the buffer position that is the
            // current iteration position. And we also want to retain the current iteration position.
            // (The buffer is already completely full of entries that precede the iteration position.)
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        f_ಕೊನೆ_Buf_Idx = mod_Chunk_ಗಾತ್ರ(f_ಕೊನೆ_Buf_Idx - 1);
    }
    fBoundaries[ಮುಂದಿನ_Idx] = position;
    fStatuses[ಮುಂದಿನ_Idx] = static_cast<uint16_t>(ನಿಯಮ_ಸ್ಥಿತಿ_Idx);
    f_ಆರಂಭ_Buf_Idx = ಮುಂದಿನ_Idx;
    ಆದರೆ (update == UpdateCachePosition) {
        fBufIdx = ಮುಂದಿನ_Idx;
        f_ಪಠ್ಯ_Idx = position;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


ಶೂನ್ಯ ನಿಯಮ_Based_Break_Iterator::BreakCache::dumpCache() {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    RBBIDebugPrintf("fTextIdx:%d   fBufIdx:%d\n", f_ಪಠ್ಯ_Idx, fBufIdx);
    ಗಾಗಿ (int32_t i=f_ಆರಂಭ_Buf_Idx; ; i=mod_Chunk_ಗಾತ್ರ(i+1)) {
        RBBIDebugPrintf("%d  %d\n", i, fBoundaries[i]);
        ಆದರೆ (i == f_ಕೊನೆ_Buf_Idx) {
            ನಿಲ್ಲಿಸು;
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // #if !UCONFIG_NO_BREAK_ITERATION
