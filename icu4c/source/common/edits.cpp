// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "util.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
ಸ್ಥಿರಾಂಕ int32_t MAX_UNCHANGED_ಉದ್ದಳತೆ = 0x1000;
ಸ್ಥಿರಾಂಕ int32_t MAX_UNCHANGED = MAX_UNCHANGED_ಉದ್ದಳತೆ - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ಸ್ಥಿರಾಂಕ int32_t MAX_SHORT_CHANGE_OLD_ಉದ್ದಳತೆ = 6;
ಸ್ಥಿರಾಂಕ int32_t MAX_SHORT_CHANGE_NEW_ಉದ್ದಳತೆ = 7;
ಸ್ಥಿರಾಂಕ int32_t SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ = 0x1ff;
ಸ್ಥಿರಾಂಕ int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ಸ್ಥಿರಾಂಕ int32_t ಉದ್ದಳತೆ_ಒಳ_1_TRAIL = 61;
ಸ್ಥಿರಾಂಕ int32_t ಉದ್ದಳತೆ_ಒಳ_2_TRAIL = 62;

}  // namespace

ಶೂನ್ಯ Edits::releaseArray() noexcept {
    ಆದರೆ (array != ರಾಶಿ_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ಸ್ಥಿರಾಂಕ Edits& other) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) {
        length = delta = ಅಂಕೆ_Changes = 0;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        ಆದರೆ (newArray == ಶೂನ್ಯಸೂಚಕ) {
            length = delta = ಅಂಕೆ_Changes = 0;
            error_ಸಂಕೇತ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    ಆದರೆ (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) {
        length = delta = ಅಂಕೆ_Changes = 0;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    releaseArray();
    ಆದರೆ (length > ರಾಶಿ_ಸಾಮರ್ಥ್ಯ_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ರಾಶಿ_Array;
        src.capacity = ರಾಶಿ_ಸಾಮರ್ಥ್ಯ_2;
        src.reset();
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    array = ರಾಶಿ_Array;
    capacity = ರಾಶಿ_ಸಾಮರ್ಥ್ಯ_2;
    ಆದರೆ (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Edits& Edits::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Edits& other) U_LIFETIME_BOUND {
    ಆದರೆ (ಇದು == &other) { ಹಿಂತಿರುಗಿ *ಇದು; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    ಅಂಕೆ_Changes = other.ಅಂಕೆ_Changes;
    error_ಸಂಕೇತ_2 = other.error_ಸಂಕೇತ_2;
    ಹಿಂತಿರುಗಿ copyArray(other);
}

Edits& Edits::ಕ್ರಿಯಾಕಾರಕ=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    ಅಂಕೆ_Changes = src.ಅಂಕೆ_Changes;
    error_ಸಂಕೇತ_2 = src.error_ಸಂಕೇತ_2;
    ಹಿಂತಿರುಗಿ moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

ಶೂನ್ಯ Edits::reset() noexcept {
    length = delta = ಅಂಕೆ_Changes = 0;
    error_ಸಂಕೇತ_2 = U_ZERO_ERROR;
}

ಶೂನ್ಯ Edits::ಕೂಡಿಸು_Unchanged(int32_t unchanged_ಉದ್ದಳತೆ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ_2) || unchanged_ಉದ್ದಳತೆ == 0) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ(unchanged_ಉದ್ದಳತೆ < 0) {
        error_ಸಂಕೇತ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t ಕೊನೆಯ = ಕೊನೆಯ_Unit();
    ಆದರೆ(ಕೊನೆಯ < MAX_UNCHANGED) {
        int32_t ಉಳಿದ = MAX_UNCHANGED - ಕೊನೆಯ;
        ಆದರೆ (ಉಳಿದ >= unchanged_ಉದ್ದಳತೆ) {
            ಗಣ_ಕೊನೆಯ_Unit(ಕೊನೆಯ + unchanged_ಉದ್ದಳತೆ);
            ಹಿಂತಿರುಗಿ;
        }
        ಗಣ_ಕೊನೆಯ_Unit(MAX_UNCHANGED);
        unchanged_ಉದ್ದಳತೆ -= ಉಳಿದ;
    }
    // Split large lengths into multiple units.
    ಆಗ(unchanged_ಉದ್ದಳತೆ >= MAX_UNCHANGED_ಉದ್ದಳತೆ) {
        append(MAX_UNCHANGED);
        unchanged_ಉದ್ದಳತೆ -= MAX_UNCHANGED_ಉದ್ದಳತೆ;
    }
    // Write a small (remaining) length.
    ಆದರೆ(unchanged_ಉದ್ದಳತೆ > 0) {
        append(unchanged_ಉದ್ದಳತೆ - 1);
    }
}

ಶೂನ್ಯ Edits::ಕೂಡಿಸು_Replace(int32_t old_ಉದ್ದಳತೆ, int32_t new_ಉದ್ದಳತೆ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ(old_ಉದ್ದಳತೆ < 0 || new_ಉದ್ದಳತೆ < 0) {
        error_ಸಂಕೇತ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (old_ಉದ್ದಳತೆ == 0 && new_ಉದ್ದಳತೆ == 0) {
        ಹಿಂತಿರುಗಿ;
    }
    ++ಅಂಕೆ_Changes;
    int32_t newDelta = new_ಉದ್ದಳತೆ - old_ಉದ್ದಳತೆ;
    ಆದರೆ (newDelta != 0) {
        ಆದರೆ ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_ಸಂಕೇತ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        delta += newDelta;
    }

    ಆದರೆ(0 < old_ಉದ್ದಳತೆ && old_ಉದ್ದಳತೆ <= MAX_SHORT_CHANGE_OLD_ಉದ್ದಳತೆ &&
            new_ಉದ್ದಳತೆ <= MAX_SHORT_CHANGE_NEW_ಉದ್ದಳತೆ) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_ಉದ್ದಳತೆ << 12) | (new_ಉದ್ದಳತೆ << 9);
        int32_t ಕೊನೆಯ = ಕೊನೆಯ_Unit();
        ಆದರೆ(MAX_UNCHANGED < ಕೊನೆಯ && ಕೊನೆಯ < MAX_SHORT_CHANGE &&
                (ಕೊನೆಯ & ~SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ) == u &&
                (ಕೊನೆಯ & SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ) < SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ) {
            ಗಣ_ಕೊನೆಯ_Unit(ಕೊನೆಯ + 1);
            ಹಿಂತಿರುಗಿ;
        }
        append(u);
        ಹಿಂತಿರುಗಿ;
    }

    int32_t ತಲೆ = 0x7000;
    ಆದರೆ (old_ಉದ್ದಳತೆ < ಉದ್ದಳತೆ_ಒಳ_1_TRAIL && new_ಉದ್ದಳತೆ < ಉದ್ದಳತೆ_ಒಳ_1_TRAIL) {
        ತಲೆ |= old_ಉದ್ದಳತೆ << 6;
        ತಲೆ |= new_ಉದ್ದಳತೆ;
        append(ತಲೆ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((capacity - length) >= 5 || growArray()) {
        int32_t ಎಲ್ಲೆ = length + 1;
        ಆದರೆ(old_ಉದ್ದಳತೆ < ಉದ್ದಳತೆ_ಒಳ_1_TRAIL) {
            ತಲೆ |= old_ಉದ್ದಳತೆ << 6;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(old_ಉದ್ದಳತೆ <= 0x7fff) {
            ತಲೆ |= ಉದ್ದಳತೆ_ಒಳ_1_TRAIL << 6;
            array[ಎಲ್ಲೆ++] = static_cast<uint16_t>(0x8000 | old_ಉದ್ದಳತೆ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ತಲೆ |= (ಉದ್ದಳತೆ_ಒಳ_2_TRAIL + (old_ಉದ್ದಳತೆ >> 30)) << 6;
            array[ಎಲ್ಲೆ++] = static_cast<uint16_t>(0x8000 | (old_ಉದ್ದಳತೆ >> 15));
            array[ಎಲ್ಲೆ++] = static_cast<uint16_t>(0x8000 | old_ಉದ್ದಳತೆ);
        }
        ಆದರೆ(new_ಉದ್ದಳತೆ < ಉದ್ದಳತೆ_ಒಳ_1_TRAIL) {
            ತಲೆ |= new_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(new_ಉದ್ದಳತೆ <= 0x7fff) {
            ತಲೆ |= ಉದ್ದಳತೆ_ಒಳ_1_TRAIL;
            array[ಎಲ್ಲೆ++] = static_cast<uint16_t>(0x8000 | new_ಉದ್ದಳತೆ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ತಲೆ |= ಉದ್ದಳತೆ_ಒಳ_2_TRAIL + (new_ಉದ್ದಳತೆ >> 30);
            array[ಎಲ್ಲೆ++] = static_cast<uint16_t>(0x8000 | (new_ಉದ್ದಳತೆ >> 15));
            array[ಎಲ್ಲೆ++] = static_cast<uint16_t>(0x8000 | new_ಉದ್ದಳತೆ);
        }
        array[length] = static_cast<uint16_t>(ತಲೆ);
        length = ಎಲ್ಲೆ;
    }
}

ಶೂನ್ಯ Edits::append(int32_t r) {
    ಆದರೆ(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t new_ಸಾಮರ್ಥ್ಯ;
    ಆದರೆ (array == ರಾಶಿ_Array) {
        new_ಸಾಮರ್ಥ್ಯ = 2000;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_ಸಂಕೇತ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (capacity >= (INT32_MAX / 2)) {
        new_ಸಾಮರ್ಥ್ಯ = INT32_MAX;
    } ಇಲ್ಲದಿದ್ದರೆ {
        new_ಸಾಮರ್ಥ್ಯ = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    ಆದರೆ ((new_ಸಾಮರ್ಥ್ಯ - capacity) < 5) {
        error_ಸಂಕೇತ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_ಸಾಮರ್ಥ್ಯ) * 2));
    ಆದರೆ (newArray == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_ಸಾಮರ್ಥ್ಯ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool Edits::copy_Error_ಗಮ್ಯ(UError_ಸಂಕೇತ &ಹೊರಹೋಗು_Error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಹೊರಹೋಗು_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ನಿಜ; }
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಹೊರಹೋಗು_Error_ಸಂಕೇತ = error_ಸಂಕೇತ_2;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

Edits& Edits::ವಿಲೀನ_And_Append(ಸ್ಥಿರಾಂಕ Edits& ab,
                             ಸ್ಥಿರಾಂಕ Edits& bc,
                             UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    ಆದರೆ (copy_Error_ಗಮ್ಯ(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_ಮುಂದಿನ = ನಿಜ, bc_Has_ಮುಂದಿನ = ನಿಜ;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_ಉದ್ದಳತೆ = 0, ab_b_ಉದ್ದಳತೆ = 0, bc_b_ಉದ್ದಳತೆ = 0, c_ಉದ್ದಳತೆ = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_ಉದ್ದಳತೆ = 0, pending_c_ಉದ್ದಳತೆ = 0;
    ಗಾಗಿ (;;) {
        // At this point, for each of the two iterators:
        // Either we are done with the locally cached current edit,
        // and its intermediate-string length has been reset,
        // or we will continue to work with a truncated remainder of this edit.
        //
        // If the current edit is done, and the iterator has not yet reached the end,
        // then we fetch the next edit. This is true for at least one of the iterators.
        //
        // Normally it does not matter whether we fetch from ab and then bc or vice versa.
        // However, the result is observably different when
        // ab deletions meet bc insertions at the same intermediate-string index.
        // Some users expect the bc insertions to come first, so we fetch from bc first.
        ಆದರೆ (bc_b_ಉದ್ದಳತೆ == 0) {
            ಆದರೆ (bc_Has_ಮುಂದಿನ && (bc_Has_ಮುಂದಿನ = bcIter.next(error_ಸಂಕೇತ)) != 0) {
                bc_b_ಉದ್ದಳತೆ = bcIter.old_ಉದ್ದಳತೆ();
                c_ಉದ್ದಳತೆ = bcIter.new_ಉದ್ದಳತೆ();
                ಆದರೆ (bc_b_ಉದ್ದಳತೆ == 0) {
                    // insertion
                    ಆದರೆ (ab_b_ಉದ್ದಳತೆ == 0 || !abIter.hasChange()) {
                        ಕೂಡಿಸು_Replace(pending_a_ಉದ್ದಳತೆ, pending_c_ಉದ್ದಳತೆ + c_ಉದ್ದಳತೆ);
                        pending_a_ಉದ್ದಳತೆ = pending_c_ಉದ್ದಳತೆ = 0;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        pending_c_ಉದ್ದಳತೆ += c_ಉದ್ದಳತೆ;
                    }
                    ಮುಂದುವರಿ;
                }
            }
            // else see if the other iterator is done, too.
        }
        ಆದರೆ (ab_b_ಉದ್ದಳತೆ == 0) {
            ಆದರೆ (ab_Has_ಮುಂದಿನ && (ab_Has_ಮುಂದಿನ = abIter.next(error_ಸಂಕೇತ)) != 0) {
                a_ಉದ್ದಳತೆ = abIter.old_ಉದ್ದಳತೆ();
                ab_b_ಉದ್ದಳತೆ = abIter.new_ಉದ್ದಳತೆ();
                ಆದರೆ (ab_b_ಉದ್ದಳತೆ == 0) {
                    // deletion
                    ಆದರೆ (bc_b_ಉದ್ದಳತೆ == bcIter.old_ಉದ್ದಳತೆ() || !bcIter.hasChange()) {
                        ಕೂಡಿಸು_Replace(pending_a_ಉದ್ದಳತೆ + a_ಉದ್ದಳತೆ, pending_c_ಉದ್ದಳತೆ);
                        pending_a_ಉದ್ದಳತೆ = pending_c_ಉದ್ದಳತೆ = 0;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        pending_a_ಉದ್ದಳತೆ += a_ಉದ್ದಳತೆ;
                    }
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bc_b_ಉದ್ದಳತೆ == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // The ab output string is shorter than the bc input string.
                ಆದರೆ (!copy_Error_ಗಮ್ಯ(error_ಸಂಕೇತ)) {
                    error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                }
                ಹಿಂತಿರುಗಿ *ಇದು;
            }
        }
        ಆದರೆ (bc_b_ಉದ್ದಳತೆ == 0) {
            // The bc input string is shorter than the ab output string.
            ಆದರೆ (!copy_Error_ಗಮ್ಯ(error_ಸಂಕೇತ)) {
                error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
            }
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        ಆದರೆ (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            ಆದರೆ (pending_a_ಉದ್ದಳತೆ != 0 || pending_c_ಉದ್ದಳತೆ != 0) {
                ಕೂಡಿಸು_Replace(pending_a_ಉದ್ದಳತೆ, pending_c_ಉದ್ದಳತೆ);
                pending_a_ಉದ್ದಳತೆ = pending_c_ಉದ್ದಳತೆ = 0;
            }
            int32_t unchanged_ಉದ್ದಳತೆ = a_ಉದ್ದಳತೆ <= c_ಉದ್ದಳತೆ ? a_ಉದ್ದಳತೆ : c_ಉದ್ದಳತೆ;
            ಕೂಡಿಸು_Unchanged(unchanged_ಉದ್ದಳತೆ);
            ab_b_ಉದ್ದಳತೆ = a_ಉದ್ದಳತೆ -= unchanged_ಉದ್ದಳತೆ;
            bc_b_ಉದ್ದಳತೆ = c_ಉದ್ದಳತೆ -= unchanged_ಉದ್ದಳತೆ;
            // At least one of the unchanged spans is now empty.
            ಮುಂದುವರಿ;
        }
        ಆದರೆ (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            ಆದರೆ (ab_b_ಉದ್ದಳತೆ >= bc_b_ಉದ್ದಳತೆ) {
                // Split the longer unchanged span into change + remainder.
                ಕೂಡಿಸು_Replace(pending_a_ಉದ್ದಳತೆ + bc_b_ಉದ್ದಳತೆ, pending_c_ಉದ್ದಳತೆ + c_ಉದ್ದಳತೆ);
                pending_a_ಉದ್ದಳತೆ = pending_c_ಉದ್ದಳತೆ = 0;
                a_ಉದ್ದಳತೆ = ab_b_ಉದ್ದಳತೆ -= bc_b_ಉದ್ದಳತೆ;
                bc_b_ಉದ್ದಳತೆ = 0;
                ಮುಂದುವರಿ;
            }
            // Handle the shorter unchanged span below like a change.
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            ಆದರೆ (ab_b_ಉದ್ದಳತೆ <= bc_b_ಉದ್ದಳತೆ) {
                // Split the longer unchanged span into change + remainder.
                ಕೂಡಿಸು_Replace(pending_a_ಉದ್ದಳತೆ + a_ಉದ್ದಳತೆ, pending_c_ಉದ್ದಳತೆ + ab_b_ಉದ್ದಳತೆ);
                pending_a_ಉದ್ದಳತೆ = pending_c_ಉದ್ದಳತೆ = 0;
                c_ಉದ್ದಳತೆ = bc_b_ಉದ್ದಳತೆ -= ab_b_ಉದ್ದಳತೆ;
                ab_b_ಉದ್ದಳತೆ = 0;
                ಮುಂದುವರಿ;
            }
            // Handle the shorter unchanged span below like a change.
        } ಇಲ್ಲದಿದ್ದರೆ {  // both abIter.hasChange() && bcIter.hasChange()
            ಆದರೆ (ab_b_ಉದ್ದಳತೆ == bc_b_ಉದ್ದಳತೆ) {
                // Changes on both sides up to the same position. Emit & reset.
                ಕೂಡಿಸು_Replace(pending_a_ಉದ್ದಳತೆ + a_ಉದ್ದಳತೆ, pending_c_ಉದ್ದಳತೆ + c_ಉದ್ದಳತೆ);
                pending_a_ಉದ್ದಳತೆ = pending_c_ಉದ್ದಳತೆ = 0;
                ab_b_ಉದ್ದಳತೆ = bc_b_ಉದ್ದಳತೆ = 0;
                ಮುಂದುವರಿ;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_ಉದ್ದಳತೆ += a_ಉದ್ದಳತೆ;
        pending_c_ಉದ್ದಳತೆ += c_ಉದ್ದಳತೆ;
        ಆದರೆ (ab_b_ಉದ್ದಳತೆ < bc_b_ಉದ್ದಳತೆ) {
            bc_b_ಉದ್ದಳತೆ -= ab_b_ಉದ್ದಳತೆ;
            c_ಉದ್ದಳತೆ = ab_b_ಉದ್ದಳತೆ = 0;
        } ಇಲ್ಲದಿದ್ದರೆ {  // ab_bLength > bc_bLength
            ab_b_ಉದ್ದಳತೆ -= bc_b_ಉದ್ದಳತೆ;
            a_ಉದ್ದಳತೆ = bc_b_ಉದ್ದಳತೆ = 0;
        }
    }
    ಆದರೆ (pending_a_ಉದ್ದಳತೆ != 0 || pending_c_ಉದ್ದಳತೆ != 0) {
        ಕೂಡಿಸು_Replace(pending_a_ಉದ್ದಳತೆ, pending_c_ಉದ್ದಳತೆ);
    }
    copy_Error_ಗಮ್ಯ(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Edits::Iterator::Iterator(ಸ್ಥಿರಾಂಕ uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), ಸೂಚ್ಯಂಕ(0), length(len), ಉಳಿದ(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), ಬದಲಾಯಿತು(ಸುಳ್ಳು), old_ಉದ್ದಳತೆ_2(0), new_ಉದ್ದಳತೆ_2(0),
        src_ಸೂಚ್ಯಂಕ(0), repl_ಸೂಚ್ಯಂಕ(0), dest_ಸೂಚ್ಯಂಕ(0) {}

int32_t Edits::Iterator::read_ಉದ್ದಳತೆ(int32_t ತಲೆ) {
    ಆದರೆ (ತಲೆ < ಉದ್ದಳತೆ_ಒಳ_1_TRAIL) {
        ಹಿಂತಿರುಗಿ ತಲೆ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ತಲೆ < ಉದ್ದಳತೆ_ಒಳ_2_TRAIL) {
        U_ASSERT(ಸೂಚ್ಯಂಕ < length);
        U_ASSERT(array[ಸೂಚ್ಯಂಕ] >= 0x8000);
        ಹಿಂತಿರುಗಿ array[ಸೂಚ್ಯಂಕ++] & 0x7fff;
    } ಇಲ್ಲದಿದ್ದರೆ {
        U_ASSERT((ಸೂಚ್ಯಂಕ + 2) <= length);
        U_ASSERT(array[ಸೂಚ್ಯಂಕ] >= 0x8000);
        U_ASSERT(array[ಸೂಚ್ಯಂಕ + 1] >= 0x8000);
        int32_t len = ((ತಲೆ & 1) << 30) |
                (static_cast<int32_t>(array[ಸೂಚ್ಯಂಕ] & 0x7fff) << 15) |
                (array[ಸೂಚ್ಯಂಕ + 1] & 0x7fff);
        ಸೂಚ್ಯಂಕ += 2;
        ಹಿಂತಿರುಗಿ len;
    }
}

ಶೂನ್ಯ Edits::Iterator::update_ಮುಂದಿನ_Indexes() {
    src_ಸೂಚ್ಯಂಕ += old_ಉದ್ದಳತೆ_2;
    ಆದರೆ (ಬದಲಾಯಿತು) {
        repl_ಸೂಚ್ಯಂಕ += new_ಉದ್ದಳತೆ_2;
    }
    dest_ಸೂಚ್ಯಂಕ += new_ಉದ್ದಳತೆ_2;
}

ಶೂನ್ಯ Edits::Iterator::update_ಹಿಂದಿನ_Indexes() {
    src_ಸೂಚ್ಯಂಕ -= old_ಉದ್ದಳತೆ_2;
    ಆದರೆ (ಬದಲಾಯಿತು) {
        repl_ಸೂಚ್ಯಂಕ -= new_ಉದ್ದಳತೆ_2;
    }
    dest_ಸೂಚ್ಯಂಕ -= new_ಉದ್ದಳತೆ_2;
}

UBool Edits::Iterator::no_ಮುಂದಿನ() {
    // No change before or beyond the string.
    dir = 0;
    ಬದಲಾಯಿತು = ಸುಳ್ಳು;
    old_ಉದ್ದಳತೆ_2 = new_ಉದ್ದಳತೆ_2 = 0;
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ಆದರೆ (dir > 0) {
        update_ಮುಂದಿನ_Indexes();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            ಆದರೆ (ಉಳಿದ > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++ಸೂಚ್ಯಂಕ;  // next() rests on the index after the sequence unit.
                dir = 1;
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
        dir = 1;
    }
    ಆದರೆ (ಉಳಿದ >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        ಆದರೆ (ಉಳಿದ > 1) {
            --ಉಳಿದ;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಉಳಿದ = 0;
    }
    ಆದರೆ (ಸೂಚ್ಯಂಕ >= length) {
        ಹಿಂತಿರುಗಿ no_ಮುಂದಿನ();
    }
    int32_t u = array[ಸೂಚ್ಯಂಕ++];
    ಆದರೆ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        ಬದಲಾಯಿತು = ಸುಳ್ಳು;
        old_ಉದ್ದಳತೆ_2 = u + 1;
        ಆಗ (ಸೂಚ್ಯಂಕ < length && (u = array[ಸೂಚ್ಯಂಕ]) <= MAX_UNCHANGED) {
            ++ಸೂಚ್ಯಂಕ;
            old_ಉದ್ದಳತೆ_2 += u + 1;
        }
        new_ಉದ್ದಳತೆ_2 = old_ಉದ್ದಳತೆ_2;
        ಆದರೆ (onlyChanges) {
            update_ಮುಂದಿನ_Indexes();
            ಆದರೆ (ಸೂಚ್ಯಂಕ >= length) {
                ಹಿಂತಿರುಗಿ no_ಮುಂದಿನ();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++ಸೂಚ್ಯಂಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    ಬದಲಾಯಿತು = ನಿಜ;
    ಆದರೆ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_ಉದ್ದಳತೆ;
        int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ) + 1;
        ಆದರೆ (coarse) {
            old_ಉದ್ದಳತೆ_2 = num * oldLen;
            new_ಉದ್ದಳತೆ_2 = num * newLen;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Split a sequence of changes that was compressed into one unit.
            old_ಉದ್ದಳತೆ_2 = oldLen;
            new_ಉದ್ದಳತೆ_2 = newLen;
            ಆದರೆ (num > 1) {
                ಉಳಿದ = num;  // This is the first of two or more changes.
            }
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        U_ASSERT(u <= 0x7fff);
        old_ಉದ್ದಳತೆ_2 = read_ಉದ್ದಳತೆ((u >> 6) & 0x3f);
        new_ಉದ್ದಳತೆ_2 = read_ಉದ್ದಳತೆ(u & 0x3f);
        ಆದರೆ (!coarse) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    // Combine adjacent changes.
    ಆಗ (ಸೂಚ್ಯಂಕ < length && (u = array[ಸೂಚ್ಯಂಕ]) > MAX_UNCHANGED) {
        ++ಸೂಚ್ಯಂಕ;
        ಆದರೆ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ) + 1;
            old_ಉದ್ದಳತೆ_2 += (u >> 12) * num;
            new_ಉದ್ದಳತೆ_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_ಉದ್ದಳತೆ) * num;
        } ಇಲ್ಲದಿದ್ದರೆ {
            U_ASSERT(u <= 0x7fff);
            old_ಉದ್ದಳತೆ_2 += read_ಉದ್ದಳತೆ((u >> 6) & 0x3f);
            new_ಉದ್ದಳತೆ_2 += read_ಉದ್ದಳತೆ(u & 0x3f);
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool Edits::Iterator::ಹಿಂದಿನ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ಆದರೆ (dir >= 0) {
        ಆದರೆ (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            ಆದರೆ (ಉಳಿದ > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --ಸೂಚ್ಯಂಕ;  // previous() rests on the sequence unit.
                dir = -1;
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
            update_ಮುಂದಿನ_Indexes();
        }
        dir = -1;
    }
    ಆದರೆ (ಉಳಿದ > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[ಸೂಚ್ಯಂಕ];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        ಆದರೆ (ಉಳಿದ <= (u & SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ)) {
            ++ಉಳಿದ;
            update_ಹಿಂದಿನ_Indexes();
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಉಳಿದ = 0;
    }
    ಆದರೆ (ಸೂಚ್ಯಂಕ <= 0) {
        ಹಿಂತಿರುಗಿ no_ಮುಂದಿನ();
    }
    int32_t u = array[--ಸೂಚ್ಯಂಕ];
    ಆದರೆ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        ಬದಲಾಯಿತು = ಸುಳ್ಳು;
        old_ಉದ್ದಳತೆ_2 = u + 1;
        ಆಗ (ಸೂಚ್ಯಂಕ > 0 && (u = array[ಸೂಚ್ಯಂಕ - 1]) <= MAX_UNCHANGED) {
            --ಸೂಚ್ಯಂಕ;
            old_ಉದ್ದಳತೆ_2 += u + 1;
        }
        new_ಉದ್ದಳತೆ_2 = old_ಉದ್ದಳತೆ_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        update_ಹಿಂದಿನ_Indexes();
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಬದಲಾಯಿತು = ನಿಜ;
    ಆದರೆ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_ಉದ್ದಳತೆ;
        int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ) + 1;
        ಆದರೆ (coarse) {
            old_ಉದ್ದಳತೆ_2 = num * oldLen;
            new_ಉದ್ದಳತೆ_2 = num * newLen;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Split a sequence of changes that was compressed into one unit.
            old_ಉದ್ದಳತೆ_2 = oldLen;
            new_ಉದ್ದಳತೆ_2 = newLen;
            ಆದರೆ (num > 1) {
                ಉಳಿದ = 1;  // This is the last of two or more changes.
            }
            update_ಹಿಂದಿನ_Indexes();
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_ಉದ್ದಳತೆ_2 = read_ಉದ್ದಳತೆ((u >> 6) & 0x3f);
            new_ಉದ್ದಳತೆ_2 = read_ಉದ್ದಳತೆ(u & 0x3f);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(ಸೂಚ್ಯಂಕ > 0);
            ಆಗ ((u = array[--ಸೂಚ್ಯಂಕ]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t ತಲೆ_ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ++;
            old_ಉದ್ದಳತೆ_2 = read_ಉದ್ದಳತೆ((u >> 6) & 0x3f);
            new_ಉದ್ದಳತೆ_2 = read_ಉದ್ದಳತೆ(u & 0x3f);
            ಸೂಚ್ಯಂಕ = ತಲೆ_ಸೂಚ್ಯಂಕ;
        }
        ಆದರೆ (!coarse) {
            update_ಹಿಂದಿನ_Indexes();
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    // Combine adjacent changes.
    ಆಗ (ಸೂಚ್ಯಂಕ > 0 && (u = array[ಸೂಚ್ಯಂಕ - 1]) > MAX_UNCHANGED) {
        --ಸೂಚ್ಯಂಕ;
        ಆದರೆ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ) + 1;
            old_ಉದ್ದಳತೆ_2 += (u >> 12) * num;
            new_ಉದ್ದಳತೆ_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_ಉದ್ದಳತೆ) * num;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t ತಲೆ_ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ++;
            old_ಉದ್ದಳತೆ_2 += read_ಉದ್ದಳತೆ((u >> 6) & 0x3f);
            new_ಉದ್ದಳತೆ_2 += read_ಉದ್ದಳತೆ(u & 0x3f);
            ಸೂಚ್ಯಂಕ = ತಲೆ_ಸೂಚ್ಯಂಕ;
        }
    }
    update_ಹಿಂದಿನ_Indexes();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

int32_t Edits::Iterator::ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ(int32_t i, UBool ಕಂಡುಹಿಡಿ_ಮೂಲ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || i < 0) { ಹಿಂತಿರುಗಿ -1; }
    int32_t ಹರವು_ಆರಂಭ, ಹರವು_ಉದ್ದಳತೆ;
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಮೂಲ) {  // find source index
        ಹರವು_ಆರಂಭ = src_ಸೂಚ್ಯಂಕ;
        ಹರವು_ಉದ್ದಳತೆ = old_ಉದ್ದಳತೆ_2;
    } ಇಲ್ಲದಿದ್ದರೆ {  // find destination index
        ಹರವು_ಆರಂಭ = dest_ಸೂಚ್ಯಂಕ;
        ಹರವು_ಉದ್ದಳತೆ = new_ಉದ್ದಳತೆ_2;
    }
    ಆದರೆ (i < ಹರವು_ಆರಂಭ) {
        ಆದರೆ (i >= (ಹರವು_ಆರಂಭ / 2)) {
            // Search backwards.
            ಗಾಗಿ (;;) {
                UBool has_ಹಿಂದಿನ = ಹಿಂದಿನ(error_ಸಂಕೇತ);
                U_ASSERT(has_ಹಿಂದಿನ);  // because i>=0 and the first span starts at 0
                (ಶೂನ್ಯ)has_ಹಿಂದಿನ;  // avoid unused-variable warning
                ಹರವು_ಆರಂಭ = ಕಂಡುಹಿಡಿ_ಮೂಲ ? src_ಸೂಚ್ಯಂಕ : dest_ಸೂಚ್ಯಂಕ;
                ಆದರೆ (i >= ಹರವು_ಆರಂಭ) {
                    // The index is in the current span.
                    ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (ಉಳಿದ > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    ಹರವು_ಉದ್ದಳತೆ = ಕಂಡುಹಿಡಿ_ಮೂಲ ? old_ಉದ್ದಳತೆ_2 : new_ಉದ್ದಳತೆ_2;
                    int32_t u = array[ಸೂಚ್ಯಂಕ];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_ಮುಖವಾಡ) + 1 - ಉಳಿದ;
                    int32_t len = num * ಹರವು_ಉದ್ದಳತೆ;
                    ಆದರೆ (i >= (ಹರವು_ಆರಂಭ - len)) {
                        int32_t n = ((ಹರವು_ಆರಂಭ - i - 1) / ಹರವು_ಉದ್ದಳತೆ) + 1;
                        // 1 <= n <= num
                        src_ಸೂಚ್ಯಂಕ -= n * old_ಉದ್ದಳತೆ_2;
                        repl_ಸೂಚ್ಯಂಕ -= n * new_ಉದ್ದಳತೆ_2;
                        dest_ಸೂಚ್ಯಂಕ -= n * new_ಉದ್ದಳತೆ_2;
                        ಉಳಿದ += n;
                        ಹಿಂತಿರುಗಿ 0;
                    }
                    // Skip all of these edits at once.
                    src_ಸೂಚ್ಯಂಕ -= num * old_ಉದ್ದಳತೆ_2;
                    repl_ಸೂಚ್ಯಂಕ -= num * new_ಉದ್ದಳತೆ_2;
                    dest_ಸೂಚ್ಯಂಕ -= num * new_ಉದ್ದಳತೆ_2;
                    ಉಳಿದ = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        ಸೂಚ್ಯಂಕ = ಉಳಿದ = old_ಉದ್ದಳತೆ_2 = new_ಉದ್ದಳತೆ_2 = src_ಸೂಚ್ಯಂಕ = repl_ಸೂಚ್ಯಂಕ = dest_ಸೂಚ್ಯಂಕ = 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i < (ಹರವು_ಆರಂಭ + ಹರವು_ಉದ್ದಳತೆ)) {
        // The index is in the current span.
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆಗ (next(ಸುಳ್ಳು, error_ಸಂಕೇತ)) {
        ಆದರೆ (ಕಂಡುಹಿಡಿ_ಮೂಲ) {
            ಹರವು_ಆರಂಭ = src_ಸೂಚ್ಯಂಕ;
            ಹರವು_ಉದ್ದಳತೆ = old_ಉದ್ದಳತೆ_2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹರವು_ಆರಂಭ = dest_ಸೂಚ್ಯಂಕ;
            ಹರವು_ಉದ್ದಳತೆ = new_ಉದ್ದಳತೆ_2;
        }
        ಆದರೆ (i < (ಹರವು_ಆರಂಭ + ಹರವು_ಉದ್ದಳತೆ)) {
            // The index is in the current span.
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (ಉಳಿದ > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = ಉಳಿದ * ಹರವು_ಉದ್ದಳತೆ;
            ಆದರೆ (i < (ಹರವು_ಆರಂಭ + len)) {
                int32_t n = (i - ಹರವು_ಆರಂಭ) / ಹರವು_ಉದ್ದಳತೆ;  // 1 <= n <= remaining - 1
                src_ಸೂಚ್ಯಂಕ += n * old_ಉದ್ದಳತೆ_2;
                repl_ಸೂಚ್ಯಂಕ += n * new_ಉದ್ದಳತೆ_2;
                dest_ಸೂಚ್ಯಂಕ += n * new_ಉದ್ದಳತೆ_2;
                ಉಳಿದ -= n;
                ಹಿಂತಿರುಗಿ 0;
            }
            // Make next() skip all of these edits at once.
            old_ಉದ್ದಳತೆ_2 *= ಉಳಿದ;
            new_ಉದ್ದಳತೆ_2 *= ಉಳಿದ;
            ಉಳಿದ = 0;
        }
    }
    ಹಿಂತಿರುಗಿ 1;
}

int32_t Edits::Iterator::destination_ಸೂಚ್ಯಂಕ_From_ಮೂಲ_ಸೂಚ್ಯಂಕ(int32_t i, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t where = ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ(i, ನಿಜ, error_ಸಂಕೇತ);
    ಆದರೆ (where < 0) {
        // Error or before the string.
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (where > 0 || i == src_ಸೂಚ್ಯಂಕ) {
        // At or after string length, or at start of the found span.
        ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
    }
    ಆದರೆ (ಬದಲಾಯಿತು) {
        // In a change span, map to its end.
        ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ + new_ಉದ್ದಳತೆ_2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // In an unchanged span, offset 1:1 within it.
        ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ + (i - src_ಸೂಚ್ಯಂಕ);
    }
}

int32_t Edits::Iterator::ಮೂಲ_ಸೂಚ್ಯಂಕ_From_Destination_ಸೂಚ್ಯಂಕ(int32_t i, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t where = ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ(i, ಸುಳ್ಳು, error_ಸಂಕೇತ);
    ಆದರೆ (where < 0) {
        // Error or before the string.
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (where > 0 || i == dest_ಸೂಚ್ಯಂಕ) {
        // At or after string length, or at start of the found span.
        ಹಿಂತಿರುಗಿ src_ಸೂಚ್ಯಂಕ;
    }
    ಆದರೆ (ಬದಲಾಯಿತು) {
        // In a change span, map to its end.
        ಹಿಂತಿರುಗಿ src_ಸೂಚ್ಯಂಕ + old_ಉದ್ದಳತೆ_2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // In an unchanged span, offset within it.
        ಹಿಂತಿರುಗಿ src_ಸೂಚ್ಯಂಕ + (i - dest_ಸೂಚ್ಯಂಕ);
    }
}

UnicodeString& Edits::Iterator::ಗಮ್ಯ_String(UnicodeString& sb U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_ಸಂಖ್ಯೆ(sb, src_ಸೂಚ್ಯಂಕ);
    sb.append(u"..", -1);
    ICU_Utility::append_ಸಂಖ್ಯೆ(sb, src_ಸೂಚ್ಯಂಕ + old_ಉದ್ದಳತೆ_2);
    ಆದರೆ (ಬದಲಾಯಿತು) {
        sb.append(u"] ⇝ dest[", -1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_ಸಂಖ್ಯೆ(sb, dest_ಸೂಚ್ಯಂಕ);
    sb.append(u"..", -1);
    ICU_Utility::append_ಸಂಖ್ಯೆ(sb, dest_ಸೂಚ್ಯಂಕ + new_ಉದ್ದಳತೆ_2);
    ಆದರೆ (ಬದಲಾಯಿತು) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_ಸಂಖ್ಯೆ(sb, repl_ಸೂಚ್ಯಂಕ);
        sb.append(u"..", -1);
        ICU_Utility::append_ಸಂಖ್ಯೆ(sb, repl_ಸೂಚ್ಯಂಕ + new_ಉದ್ದಳತೆ_2);
        sb.append(u"] }", -1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        sb.append(u"] (no-change) }", -1);
    }
    ಹಿಂತಿರುಗಿ sb;
}

U_NAMESPACE_ಕೊನೆ
