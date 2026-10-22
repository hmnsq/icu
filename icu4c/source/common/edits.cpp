// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util.h"

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
ਅਚਲ int32_t MAX_UNCHANGED_ਲੰਮਾਈ = 0x1000;
ਅਚਲ int32_t MAX_UNCHANGED = MAX_UNCHANGED_ਲੰਮਾਈ - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ਅਚਲ int32_t MAX_SHORT_CHANGE_OLD_ਲੰਮਾਈ = 6;
ਅਚਲ int32_t MAX_SHORT_CHANGE_NEW_ਲੰਮਾਈ = 7;
ਅਚਲ int32_t SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ = 0x1ff;
ਅਚਲ int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ਅਚਲ int32_t ਲੰਮਾਈ_ਵਿੱਚ_1_TRAIL = 61;
ਅਚਲ int32_t ਲੰਮਾਈ_ਵਿੱਚ_2_TRAIL = 62;

}  // namespace

ਖਾਲੀ Edits::releaseArray() noexcept {
    ਜੇ (array != ਢੇਰ_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ਅਚਲ Edits& other) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) {
        length = delta = ਅੰਕ_Changes = 0;
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        ਜੇ (newArray == ਖਾਲੀ_ਸੂਚਕ) {
            length = delta = ਅੰਕ_Changes = 0;
            error_ਕੋਡ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ *ਇਹ;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    ਜੇ (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    ਵਾਪਸ *ਇਹ;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) {
        length = delta = ਅੰਕ_Changes = 0;
        ਵਾਪਸ *ਇਹ;
    }
    releaseArray();
    ਜੇ (length > ਢੇਰ_ਸਮਰੱਥਾ_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ਢੇਰ_Array;
        src.capacity = ਢੇਰ_ਸਮਰੱਥਾ_2;
        src.reset();
        ਵਾਪਸ *ਇਹ;
    }
    array = ਢੇਰ_Array;
    capacity = ਢੇਰ_ਸਮਰੱਥਾ_2;
    ਜੇ (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    ਵਾਪਸ *ਇਹ;
}

Edits& Edits::ਸੰਚਾਲਕ=(ਅਚਲ Edits& other) U_LIFETIME_BOUND {
    ਜੇ (ਇਹ == &other) { ਵਾਪਸ *ਇਹ; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    ਅੰਕ_Changes = other.ਅੰਕ_Changes;
    error_ਕੋਡ_2 = other.error_ਕੋਡ_2;
    ਵਾਪਸ copyArray(other);
}

Edits& Edits::ਸੰਚਾਲਕ=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    ਅੰਕ_Changes = src.ਅੰਕ_Changes;
    error_ਕੋਡ_2 = src.error_ਕੋਡ_2;
    ਵਾਪਸ moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

ਖਾਲੀ Edits::reset() noexcept {
    length = delta = ਅੰਕ_Changes = 0;
    error_ਕੋਡ_2 = U_ZERO_ERROR;
}

ਖਾਲੀ Edits::ਜੋੜੋ_Unchanged(int32_t unchanged_ਲੰਮਾਈ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ_2) || unchanged_ਲੰਮਾਈ == 0) { ਵਾਪਸ; }
    ਜੇ(unchanged_ਲੰਮਾਈ < 0) {
        error_ਕੋਡ_2 = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t ਆਖਰੀ = ਆਖਰੀ_Unit();
    ਜੇ(ਆਖਰੀ < MAX_UNCHANGED) {
        int32_t ਬਚਿਆ = MAX_UNCHANGED - ਆਖਰੀ;
        ਜੇ (ਬਚਿਆ >= unchanged_ਲੰਮਾਈ) {
            ਸੈੱਟ_ਆਖਰੀ_Unit(ਆਖਰੀ + unchanged_ਲੰਮਾਈ);
            ਵਾਪਸ;
        }
        ਸੈੱਟ_ਆਖਰੀ_Unit(MAX_UNCHANGED);
        unchanged_ਲੰਮਾਈ -= ਬਚਿਆ;
    }
    // Split large lengths into multiple units.
    ਜਦੋਂ(unchanged_ਲੰਮਾਈ >= MAX_UNCHANGED_ਲੰਮਾਈ) {
        append(MAX_UNCHANGED);
        unchanged_ਲੰਮਾਈ -= MAX_UNCHANGED_ਲੰਮਾਈ;
    }
    // Write a small (remaining) length.
    ਜੇ(unchanged_ਲੰਮਾਈ > 0) {
        append(unchanged_ਲੰਮਾਈ - 1);
    }
}

ਖਾਲੀ Edits::ਜੋੜੋ_Replace(int32_t old_ਲੰਮਾਈ, int32_t new_ਲੰਮਾਈ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ; }
    ਜੇ(old_ਲੰਮਾਈ < 0 || new_ਲੰਮਾਈ < 0) {
        error_ਕੋਡ_2 = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (old_ਲੰਮਾਈ == 0 && new_ਲੰਮਾਈ == 0) {
        ਵਾਪਸ;
    }
    ++ਅੰਕ_Changes;
    int32_t newDelta = new_ਲੰਮਾਈ - old_ਲੰਮਾਈ;
    ਜੇ (newDelta != 0) {
        ਜੇ ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_ਕੋਡ_2 = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ;
        }
        delta += newDelta;
    }

    ਜੇ(0 < old_ਲੰਮਾਈ && old_ਲੰਮਾਈ <= MAX_SHORT_CHANGE_OLD_ਲੰਮਾਈ &&
            new_ਲੰਮਾਈ <= MAX_SHORT_CHANGE_NEW_ਲੰਮਾਈ) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_ਲੰਮਾਈ << 12) | (new_ਲੰਮਾਈ << 9);
        int32_t ਆਖਰੀ = ਆਖਰੀ_Unit();
        ਜੇ(MAX_UNCHANGED < ਆਖਰੀ && ਆਖਰੀ < MAX_SHORT_CHANGE &&
                (ਆਖਰੀ & ~SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ) == u &&
                (ਆਖਰੀ & SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ) < SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ) {
            ਸੈੱਟ_ਆਖਰੀ_Unit(ਆਖਰੀ + 1);
            ਵਾਪਸ;
        }
        append(u);
        ਵਾਪਸ;
    }

    int32_t ਸਿਰ = 0x7000;
    ਜੇ (old_ਲੰਮਾਈ < ਲੰਮਾਈ_ਵਿੱਚ_1_TRAIL && new_ਲੰਮਾਈ < ਲੰਮਾਈ_ਵਿੱਚ_1_TRAIL) {
        ਸਿਰ |= old_ਲੰਮਾਈ << 6;
        ਸਿਰ |= new_ਲੰਮਾਈ;
        append(ਸਿਰ);
    } ਨਹੀਂਤਾਂ ਜੇ ((capacity - length) >= 5 || growArray()) {
        int32_t ਹੱਦਬੰਦੀ = length + 1;
        ਜੇ(old_ਲੰਮਾਈ < ਲੰਮਾਈ_ਵਿੱਚ_1_TRAIL) {
            ਸਿਰ |= old_ਲੰਮਾਈ << 6;
        } ਨਹੀਂਤਾਂ ਜੇ(old_ਲੰਮਾਈ <= 0x7fff) {
            ਸਿਰ |= ਲੰਮਾਈ_ਵਿੱਚ_1_TRAIL << 6;
            array[ਹੱਦਬੰਦੀ++] = static_cast<uint16_t>(0x8000 | old_ਲੰਮਾਈ);
        } ਨਹੀਂਤਾਂ {
            ਸਿਰ |= (ਲੰਮਾਈ_ਵਿੱਚ_2_TRAIL + (old_ਲੰਮਾਈ >> 30)) << 6;
            array[ਹੱਦਬੰਦੀ++] = static_cast<uint16_t>(0x8000 | (old_ਲੰਮਾਈ >> 15));
            array[ਹੱਦਬੰਦੀ++] = static_cast<uint16_t>(0x8000 | old_ਲੰਮਾਈ);
        }
        ਜੇ(new_ਲੰਮਾਈ < ਲੰਮਾਈ_ਵਿੱਚ_1_TRAIL) {
            ਸਿਰ |= new_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ(new_ਲੰਮਾਈ <= 0x7fff) {
            ਸਿਰ |= ਲੰਮਾਈ_ਵਿੱਚ_1_TRAIL;
            array[ਹੱਦਬੰਦੀ++] = static_cast<uint16_t>(0x8000 | new_ਲੰਮਾਈ);
        } ਨਹੀਂਤਾਂ {
            ਸਿਰ |= ਲੰਮਾਈ_ਵਿੱਚ_2_TRAIL + (new_ਲੰਮਾਈ >> 30);
            array[ਹੱਦਬੰਦੀ++] = static_cast<uint16_t>(0x8000 | (new_ਲੰਮਾਈ >> 15));
            array[ਹੱਦਬੰਦੀ++] = static_cast<uint16_t>(0x8000 | new_ਲੰਮਾਈ);
        }
        array[length] = static_cast<uint16_t>(ਸਿਰ);
        length = ਹੱਦਬੰਦੀ;
    }
}

ਖਾਲੀ Edits::append(int32_t r) {
    ਜੇ(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t new_ਸਮਰੱਥਾ;
    ਜੇ (array == ਢੇਰ_Array) {
        new_ਸਮਰੱਥਾ = 2000;
    } ਨਹੀਂਤਾਂ ਜੇ (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_ਕੋਡ_2 = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ ਜੇ (capacity >= (INT32_MAX / 2)) {
        new_ਸਮਰੱਥਾ = INT32_MAX;
    } ਨਹੀਂਤਾਂ {
        new_ਸਮਰੱਥਾ = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    ਜੇ ((new_ਸਮਰੱਥਾ - capacity) < 5) {
        error_ਕੋਡ_2 = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_ਸਮਰੱਥਾ) * 2));
    ਜੇ (newArray == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_ਸਮਰੱਥਾ;
    ਵਾਪਸ ਸੱਚ;
}

UBool Edits::copy_Error_ਮੰਜ਼ਿਲ(UError_ਕੋਡ &ਬਾਹਰਲਾ_Error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਬਾਹਰਲਾ_Error_ਕੋਡ)) { ਵਾਪਸ ਸੱਚ; }
    ਜੇ (U_SUCCESS(error_ਕੋਡ_2)) { ਵਾਪਸ ਝੂਠ; }
    ਬਾਹਰਲਾ_Error_ਕੋਡ = error_ਕੋਡ_2;
    ਵਾਪਸ ਸੱਚ;
}

Edits& Edits::ਮਿਲਾਓ_And_Append(ਅਚਲ Edits& ab,
                             ਅਚਲ Edits& bc,
                             UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    ਜੇ (copy_Error_ਮੰਜ਼ਿਲ(error_ਕੋਡ)) { ਵਾਪਸ *ਇਹ; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_ਅਗਲਾ = ਸੱਚ, bc_Has_ਅਗਲਾ = ਸੱਚ;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_ਲੰਮਾਈ = 0, ab_b_ਲੰਮਾਈ = 0, bc_b_ਲੰਮਾਈ = 0, c_ਲੰਮਾਈ = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_ਲੰਮਾਈ = 0, pending_c_ਲੰਮਾਈ = 0;
    ਲਈ (;;) {
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
        ਜੇ (bc_b_ਲੰਮਾਈ == 0) {
            ਜੇ (bc_Has_ਅਗਲਾ && (bc_Has_ਅਗਲਾ = bcIter.next(error_ਕੋਡ)) != 0) {
                bc_b_ਲੰਮਾਈ = bcIter.old_ਲੰਮਾਈ();
                c_ਲੰਮਾਈ = bcIter.new_ਲੰਮਾਈ();
                ਜੇ (bc_b_ਲੰਮਾਈ == 0) {
                    // insertion
                    ਜੇ (ab_b_ਲੰਮਾਈ == 0 || !abIter.hasChange()) {
                        ਜੋੜੋ_Replace(pending_a_ਲੰਮਾਈ, pending_c_ਲੰਮਾਈ + c_ਲੰਮਾਈ);
                        pending_a_ਲੰਮਾਈ = pending_c_ਲੰਮਾਈ = 0;
                    } ਨਹੀਂਤਾਂ {
                        pending_c_ਲੰਮਾਈ += c_ਲੰਮਾਈ;
                    }
                    ਜਾਰੀ;
                }
            }
            // else see if the other iterator is done, too.
        }
        ਜੇ (ab_b_ਲੰਮਾਈ == 0) {
            ਜੇ (ab_Has_ਅਗਲਾ && (ab_Has_ਅਗਲਾ = abIter.next(error_ਕੋਡ)) != 0) {
                a_ਲੰਮਾਈ = abIter.old_ਲੰਮਾਈ();
                ab_b_ਲੰਮਾਈ = abIter.new_ਲੰਮਾਈ();
                ਜੇ (ab_b_ਲੰਮਾਈ == 0) {
                    // deletion
                    ਜੇ (bc_b_ਲੰਮਾਈ == bcIter.old_ਲੰਮਾਈ() || !bcIter.hasChange()) {
                        ਜੋੜੋ_Replace(pending_a_ਲੰਮਾਈ + a_ਲੰਮਾਈ, pending_c_ਲੰਮਾਈ);
                        pending_a_ਲੰਮਾਈ = pending_c_ਲੰਮਾਈ = 0;
                    } ਨਹੀਂਤਾਂ {
                        pending_a_ਲੰਮਾਈ += a_ਲੰਮਾਈ;
                    }
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (bc_b_ਲੰਮਾਈ == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                // The ab output string is shorter than the bc input string.
                ਜੇ (!copy_Error_ਮੰਜ਼ਿਲ(error_ਕੋਡ)) {
                    error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                }
                ਵਾਪਸ *ਇਹ;
            }
        }
        ਜੇ (bc_b_ਲੰਮਾਈ == 0) {
            // The bc input string is shorter than the ab output string.
            ਜੇ (!copy_Error_ਮੰਜ਼ਿਲ(error_ਕੋਡ)) {
                error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            }
            ਵਾਪਸ *ਇਹ;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        ਜੇ (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            ਜੇ (pending_a_ਲੰਮਾਈ != 0 || pending_c_ਲੰਮਾਈ != 0) {
                ਜੋੜੋ_Replace(pending_a_ਲੰਮਾਈ, pending_c_ਲੰਮਾਈ);
                pending_a_ਲੰਮਾਈ = pending_c_ਲੰਮਾਈ = 0;
            }
            int32_t unchanged_ਲੰਮਾਈ = a_ਲੰਮਾਈ <= c_ਲੰਮਾਈ ? a_ਲੰਮਾਈ : c_ਲੰਮਾਈ;
            ਜੋੜੋ_Unchanged(unchanged_ਲੰਮਾਈ);
            ab_b_ਲੰਮਾਈ = a_ਲੰਮਾਈ -= unchanged_ਲੰਮਾਈ;
            bc_b_ਲੰਮਾਈ = c_ਲੰਮਾਈ -= unchanged_ਲੰਮਾਈ;
            // At least one of the unchanged spans is now empty.
            ਜਾਰੀ;
        }
        ਜੇ (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            ਜੇ (ab_b_ਲੰਮਾਈ >= bc_b_ਲੰਮਾਈ) {
                // Split the longer unchanged span into change + remainder.
                ਜੋੜੋ_Replace(pending_a_ਲੰਮਾਈ + bc_b_ਲੰਮਾਈ, pending_c_ਲੰਮਾਈ + c_ਲੰਮਾਈ);
                pending_a_ਲੰਮਾਈ = pending_c_ਲੰਮਾਈ = 0;
                a_ਲੰਮਾਈ = ab_b_ਲੰਮਾਈ -= bc_b_ਲੰਮਾਈ;
                bc_b_ਲੰਮਾਈ = 0;
                ਜਾਰੀ;
            }
            // Handle the shorter unchanged span below like a change.
        } ਨਹੀਂਤਾਂ ਜੇ (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            ਜੇ (ab_b_ਲੰਮਾਈ <= bc_b_ਲੰਮਾਈ) {
                // Split the longer unchanged span into change + remainder.
                ਜੋੜੋ_Replace(pending_a_ਲੰਮਾਈ + a_ਲੰਮਾਈ, pending_c_ਲੰਮਾਈ + ab_b_ਲੰਮਾਈ);
                pending_a_ਲੰਮਾਈ = pending_c_ਲੰਮਾਈ = 0;
                c_ਲੰਮਾਈ = bc_b_ਲੰਮਾਈ -= ab_b_ਲੰਮਾਈ;
                ab_b_ਲੰਮਾਈ = 0;
                ਜਾਰੀ;
            }
            // Handle the shorter unchanged span below like a change.
        } ਨਹੀਂਤਾਂ {  // both abIter.hasChange() && bcIter.hasChange()
            ਜੇ (ab_b_ਲੰਮਾਈ == bc_b_ਲੰਮਾਈ) {
                // Changes on both sides up to the same position. Emit & reset.
                ਜੋੜੋ_Replace(pending_a_ਲੰਮਾਈ + a_ਲੰਮਾਈ, pending_c_ਲੰਮਾਈ + c_ਲੰਮਾਈ);
                pending_a_ਲੰਮਾਈ = pending_c_ਲੰਮਾਈ = 0;
                ab_b_ਲੰਮਾਈ = bc_b_ਲੰਮਾਈ = 0;
                ਜਾਰੀ;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_ਲੰਮਾਈ += a_ਲੰਮਾਈ;
        pending_c_ਲੰਮਾਈ += c_ਲੰਮਾਈ;
        ਜੇ (ab_b_ਲੰਮਾਈ < bc_b_ਲੰਮਾਈ) {
            bc_b_ਲੰਮਾਈ -= ab_b_ਲੰਮਾਈ;
            c_ਲੰਮਾਈ = ab_b_ਲੰਮਾਈ = 0;
        } ਨਹੀਂਤਾਂ {  // ab_bLength > bc_bLength
            ab_b_ਲੰਮਾਈ -= bc_b_ਲੰਮਾਈ;
            a_ਲੰਮਾਈ = bc_b_ਲੰਮਾਈ = 0;
        }
    }
    ਜੇ (pending_a_ਲੰਮਾਈ != 0 || pending_c_ਲੰਮਾਈ != 0) {
        ਜੋੜੋ_Replace(pending_a_ਲੰਮਾਈ, pending_c_ਲੰਮਾਈ);
    }
    copy_Error_ਮੰਜ਼ਿਲ(error_ਕੋਡ);
    ਵਾਪਸ *ਇਹ;
}

Edits::Iterator::Iterator(ਅਚਲ uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), ਸੂਚਕਾਂਕ(0), length(len), ਬਚਿਆ(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), ਬਦਲਿਆ(ਝੂਠ), old_ਲੰਮਾਈ_2(0), new_ਲੰਮਾਈ_2(0),
        src_ਸੂਚਕਾਂਕ(0), repl_ਸੂਚਕਾਂਕ(0), dest_ਸੂਚਕਾਂਕ(0) {}

int32_t Edits::Iterator::read_ਲੰਮਾਈ(int32_t ਸਿਰ) {
    ਜੇ (ਸਿਰ < ਲੰਮਾਈ_ਵਿੱਚ_1_TRAIL) {
        ਵਾਪਸ ਸਿਰ;
    } ਨਹੀਂਤਾਂ ਜੇ (ਸਿਰ < ਲੰਮਾਈ_ਵਿੱਚ_2_TRAIL) {
        U_ASSERT(ਸੂਚਕਾਂਕ < length);
        U_ASSERT(array[ਸੂਚਕਾਂਕ] >= 0x8000);
        ਵਾਪਸ array[ਸੂਚਕਾਂਕ++] & 0x7fff;
    } ਨਹੀਂਤਾਂ {
        U_ASSERT((ਸੂਚਕਾਂਕ + 2) <= length);
        U_ASSERT(array[ਸੂਚਕਾਂਕ] >= 0x8000);
        U_ASSERT(array[ਸੂਚਕਾਂਕ + 1] >= 0x8000);
        int32_t len = ((ਸਿਰ & 1) << 30) |
                (static_cast<int32_t>(array[ਸੂਚਕਾਂਕ] & 0x7fff) << 15) |
                (array[ਸੂਚਕਾਂਕ + 1] & 0x7fff);
        ਸੂਚਕਾਂਕ += 2;
        ਵਾਪਸ len;
    }
}

ਖਾਲੀ Edits::Iterator::update_ਅਗਲਾ_Indexes() {
    src_ਸੂਚਕਾਂਕ += old_ਲੰਮਾਈ_2;
    ਜੇ (ਬਦਲਿਆ) {
        repl_ਸੂਚਕਾਂਕ += new_ਲੰਮਾਈ_2;
    }
    dest_ਸੂਚਕਾਂਕ += new_ਲੰਮਾਈ_2;
}

ਖਾਲੀ Edits::Iterator::update_ਪਿਛਲਾ_Indexes() {
    src_ਸੂਚਕਾਂਕ -= old_ਲੰਮਾਈ_2;
    ਜੇ (ਬਦਲਿਆ) {
        repl_ਸੂਚਕਾਂਕ -= new_ਲੰਮਾਈ_2;
    }
    dest_ਸੂਚਕਾਂਕ -= new_ਲੰਮਾਈ_2;
}

UBool Edits::Iterator::no_ਅਗਲਾ() {
    // No change before or beyond the string.
    dir = 0;
    ਬਦਲਿਆ = ਝੂਠ;
    old_ਲੰਮਾਈ_2 = new_ਲੰਮਾਈ_2 = 0;
    ਵਾਪਸ ਝੂਠ;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_ਕੋਡ &error_ਕੋਡ) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ਜੇ (dir > 0) {
        update_ਅਗਲਾ_Indexes();
    } ਨਹੀਂਤਾਂ {
        ਜੇ (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            ਜੇ (ਬਚਿਆ > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++ਸੂਚਕਾਂਕ;  // next() rests on the index after the sequence unit.
                dir = 1;
                ਵਾਪਸ ਸੱਚ;
            }
        }
        dir = 1;
    }
    ਜੇ (ਬਚਿਆ >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        ਜੇ (ਬਚਿਆ > 1) {
            --ਬਚਿਆ;
            ਵਾਪਸ ਸੱਚ;
        }
        ਬਚਿਆ = 0;
    }
    ਜੇ (ਸੂਚਕਾਂਕ >= length) {
        ਵਾਪਸ no_ਅਗਲਾ();
    }
    int32_t u = array[ਸੂਚਕਾਂਕ++];
    ਜੇ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        ਬਦਲਿਆ = ਝੂਠ;
        old_ਲੰਮਾਈ_2 = u + 1;
        ਜਦੋਂ (ਸੂਚਕਾਂਕ < length && (u = array[ਸੂਚਕਾਂਕ]) <= MAX_UNCHANGED) {
            ++ਸੂਚਕਾਂਕ;
            old_ਲੰਮਾਈ_2 += u + 1;
        }
        new_ਲੰਮਾਈ_2 = old_ਲੰਮਾਈ_2;
        ਜੇ (onlyChanges) {
            update_ਅਗਲਾ_Indexes();
            ਜੇ (ਸੂਚਕਾਂਕ >= length) {
                ਵਾਪਸ no_ਅਗਲਾ();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++ਸੂਚਕਾਂਕ;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਸੱਚ;
        }
    }
    ਬਦਲਿਆ = ਸੱਚ;
    ਜੇ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_ਲੰਮਾਈ;
        int32_t num = (u & SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ) + 1;
        ਜੇ (coarse) {
            old_ਲੰਮਾਈ_2 = num * oldLen;
            new_ਲੰਮਾਈ_2 = num * newLen;
        } ਨਹੀਂਤਾਂ {
            // Split a sequence of changes that was compressed into one unit.
            old_ਲੰਮਾਈ_2 = oldLen;
            new_ਲੰਮਾਈ_2 = newLen;
            ਜੇ (num > 1) {
                ਬਚਿਆ = num;  // This is the first of two or more changes.
            }
            ਵਾਪਸ ਸੱਚ;
        }
    } ਨਹੀਂਤਾਂ {
        U_ASSERT(u <= 0x7fff);
        old_ਲੰਮਾਈ_2 = read_ਲੰਮਾਈ((u >> 6) & 0x3f);
        new_ਲੰਮਾਈ_2 = read_ਲੰਮਾਈ(u & 0x3f);
        ਜੇ (!coarse) {
            ਵਾਪਸ ਸੱਚ;
        }
    }
    // Combine adjacent changes.
    ਜਦੋਂ (ਸੂਚਕਾਂਕ < length && (u = array[ਸੂਚਕਾਂਕ]) > MAX_UNCHANGED) {
        ++ਸੂਚਕਾਂਕ;
        ਜੇ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ) + 1;
            old_ਲੰਮਾਈ_2 += (u >> 12) * num;
            new_ਲੰਮਾਈ_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_ਲੰਮਾਈ) * num;
        } ਨਹੀਂਤਾਂ {
            U_ASSERT(u <= 0x7fff);
            old_ਲੰਮਾਈ_2 += read_ਲੰਮਾਈ((u >> 6) & 0x3f);
            new_ਲੰਮਾਈ_2 += read_ਲੰਮਾਈ(u & 0x3f);
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool Edits::Iterator::ਪਿਛਲਾ(UError_ਕੋਡ &error_ਕੋਡ) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ਜੇ (dir >= 0) {
        ਜੇ (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            ਜੇ (ਬਚਿਆ > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --ਸੂਚਕਾਂਕ;  // previous() rests on the sequence unit.
                dir = -1;
                ਵਾਪਸ ਸੱਚ;
            }
            update_ਅਗਲਾ_Indexes();
        }
        dir = -1;
    }
    ਜੇ (ਬਚਿਆ > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[ਸੂਚਕਾਂਕ];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        ਜੇ (ਬਚਿਆ <= (u & SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ)) {
            ++ਬਚਿਆ;
            update_ਪਿਛਲਾ_Indexes();
            ਵਾਪਸ ਸੱਚ;
        }
        ਬਚਿਆ = 0;
    }
    ਜੇ (ਸੂਚਕਾਂਕ <= 0) {
        ਵਾਪਸ no_ਅਗਲਾ();
    }
    int32_t u = array[--ਸੂਚਕਾਂਕ];
    ਜੇ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        ਬਦਲਿਆ = ਝੂਠ;
        old_ਲੰਮਾਈ_2 = u + 1;
        ਜਦੋਂ (ਸੂਚਕਾਂਕ > 0 && (u = array[ਸੂਚਕਾਂਕ - 1]) <= MAX_UNCHANGED) {
            --ਸੂਚਕਾਂਕ;
            old_ਲੰਮਾਈ_2 += u + 1;
        }
        new_ਲੰਮਾਈ_2 = old_ਲੰਮਾਈ_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        update_ਪਿਛਲਾ_Indexes();
        ਵਾਪਸ ਸੱਚ;
    }
    ਬਦਲਿਆ = ਸੱਚ;
    ਜੇ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_ਲੰਮਾਈ;
        int32_t num = (u & SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ) + 1;
        ਜੇ (coarse) {
            old_ਲੰਮਾਈ_2 = num * oldLen;
            new_ਲੰਮਾਈ_2 = num * newLen;
        } ਨਹੀਂਤਾਂ {
            // Split a sequence of changes that was compressed into one unit.
            old_ਲੰਮਾਈ_2 = oldLen;
            new_ਲੰਮਾਈ_2 = newLen;
            ਜੇ (num > 1) {
                ਬਚਿਆ = 1;  // This is the last of two or more changes.
            }
            update_ਪਿਛਲਾ_Indexes();
            ਵਾਪਸ ਸੱਚ;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_ਲੰਮਾਈ_2 = read_ਲੰਮਾਈ((u >> 6) & 0x3f);
            new_ਲੰਮਾਈ_2 = read_ਲੰਮਾਈ(u & 0x3f);
        } ਨਹੀਂਤਾਂ {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(ਸੂਚਕਾਂਕ > 0);
            ਜਦੋਂ ((u = array[--ਸੂਚਕਾਂਕ]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t ਸਿਰ_ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ++;
            old_ਲੰਮਾਈ_2 = read_ਲੰਮਾਈ((u >> 6) & 0x3f);
            new_ਲੰਮਾਈ_2 = read_ਲੰਮਾਈ(u & 0x3f);
            ਸੂਚਕਾਂਕ = ਸਿਰ_ਸੂਚਕਾਂਕ;
        }
        ਜੇ (!coarse) {
            update_ਪਿਛਲਾ_Indexes();
            ਵਾਪਸ ਸੱਚ;
        }
    }
    // Combine adjacent changes.
    ਜਦੋਂ (ਸੂਚਕਾਂਕ > 0 && (u = array[ਸੂਚਕਾਂਕ - 1]) > MAX_UNCHANGED) {
        --ਸੂਚਕਾਂਕ;
        ਜੇ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ) + 1;
            old_ਲੰਮਾਈ_2 += (u >> 12) * num;
            new_ਲੰਮਾਈ_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_ਲੰਮਾਈ) * num;
        } ਨਹੀਂਤਾਂ ਜੇ (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t ਸਿਰ_ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ++;
            old_ਲੰਮਾਈ_2 += read_ਲੰਮਾਈ((u >> 6) & 0x3f);
            new_ਲੰਮਾਈ_2 += read_ਲੰਮਾਈ(u & 0x3f);
            ਸੂਚਕਾਂਕ = ਸਿਰ_ਸੂਚਕਾਂਕ;
        }
    }
    update_ਪਿਛਲਾ_Indexes();
    ਵਾਪਸ ਸੱਚ;
}

int32_t Edits::Iterator::ਲੱਭੋ_ਸੂਚਕਾਂਕ(int32_t i, UBool ਲੱਭੋ_ਸਰੋਤ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ) || i < 0) { ਵਾਪਸ -1; }
    int32_t ਵਿਸਤਾਰ_ਸ਼ੁਰੂ, ਵਿਸਤਾਰ_ਲੰਮਾਈ;
    ਜੇ (ਲੱਭੋ_ਸਰੋਤ) {  // find source index
        ਵਿਸਤਾਰ_ਸ਼ੁਰੂ = src_ਸੂਚਕਾਂਕ;
        ਵਿਸਤਾਰ_ਲੰਮਾਈ = old_ਲੰਮਾਈ_2;
    } ਨਹੀਂਤਾਂ {  // find destination index
        ਵਿਸਤਾਰ_ਸ਼ੁਰੂ = dest_ਸੂਚਕਾਂਕ;
        ਵਿਸਤਾਰ_ਲੰਮਾਈ = new_ਲੰਮਾਈ_2;
    }
    ਜੇ (i < ਵਿਸਤਾਰ_ਸ਼ੁਰੂ) {
        ਜੇ (i >= (ਵਿਸਤਾਰ_ਸ਼ੁਰੂ / 2)) {
            // Search backwards.
            ਲਈ (;;) {
                UBool has_ਪਿਛਲਾ = ਪਿਛਲਾ(error_ਕੋਡ);
                U_ASSERT(has_ਪਿਛਲਾ);  // because i>=0 and the first span starts at 0
                (ਖਾਲੀ)has_ਪਿਛਲਾ;  // avoid unused-variable warning
                ਵਿਸਤਾਰ_ਸ਼ੁਰੂ = ਲੱਭੋ_ਸਰੋਤ ? src_ਸੂਚਕਾਂਕ : dest_ਸੂਚਕਾਂਕ;
                ਜੇ (i >= ਵਿਸਤਾਰ_ਸ਼ੁਰੂ) {
                    // The index is in the current span.
                    ਵਾਪਸ 0;
                }
                ਜੇ (ਬਚਿਆ > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ = ਲੱਭੋ_ਸਰੋਤ ? old_ਲੰਮਾਈ_2 : new_ਲੰਮਾਈ_2;
                    int32_t u = array[ਸੂਚਕਾਂਕ];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_ਅੰਕ_ਨਕਾਬ) + 1 - ਬਚਿਆ;
                    int32_t len = num * ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                    ਜੇ (i >= (ਵਿਸਤਾਰ_ਸ਼ੁਰੂ - len)) {
                        int32_t n = ((ਵਿਸਤਾਰ_ਸ਼ੁਰੂ - i - 1) / ਵਿਸਤਾਰ_ਲੰਮਾਈ) + 1;
                        // 1 <= n <= num
                        src_ਸੂਚਕਾਂਕ -= n * old_ਲੰਮਾਈ_2;
                        repl_ਸੂਚਕਾਂਕ -= n * new_ਲੰਮਾਈ_2;
                        dest_ਸੂਚਕਾਂਕ -= n * new_ਲੰਮਾਈ_2;
                        ਬਚਿਆ += n;
                        ਵਾਪਸ 0;
                    }
                    // Skip all of these edits at once.
                    src_ਸੂਚਕਾਂਕ -= num * old_ਲੰਮਾਈ_2;
                    repl_ਸੂਚਕਾਂਕ -= num * new_ਲੰਮਾਈ_2;
                    dest_ਸੂਚਕਾਂਕ -= num * new_ਲੰਮਾਈ_2;
                    ਬਚਿਆ = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        ਸੂਚਕਾਂਕ = ਬਚਿਆ = old_ਲੰਮਾਈ_2 = new_ਲੰਮਾਈ_2 = src_ਸੂਚਕਾਂਕ = repl_ਸੂਚਕਾਂਕ = dest_ਸੂਚਕਾਂਕ = 0;
    } ਨਹੀਂਤਾਂ ਜੇ (i < (ਵਿਸਤਾਰ_ਸ਼ੁਰੂ + ਵਿਸਤਾਰ_ਲੰਮਾਈ)) {
        // The index is in the current span.
        ਵਾਪਸ 0;
    }
    ਜਦੋਂ (next(ਝੂਠ, error_ਕੋਡ)) {
        ਜੇ (ਲੱਭੋ_ਸਰੋਤ) {
            ਵਿਸਤਾਰ_ਸ਼ੁਰੂ = src_ਸੂਚਕਾਂਕ;
            ਵਿਸਤਾਰ_ਲੰਮਾਈ = old_ਲੰਮਾਈ_2;
        } ਨਹੀਂਤਾਂ {
            ਵਿਸਤਾਰ_ਸ਼ੁਰੂ = dest_ਸੂਚਕਾਂਕ;
            ਵਿਸਤਾਰ_ਲੰਮਾਈ = new_ਲੰਮਾਈ_2;
        }
        ਜੇ (i < (ਵਿਸਤਾਰ_ਸ਼ੁਰੂ + ਵਿਸਤਾਰ_ਲੰਮਾਈ)) {
            // The index is in the current span.
            ਵਾਪਸ 0;
        }
        ਜੇ (ਬਚਿਆ > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = ਬਚਿਆ * ਵਿਸਤਾਰ_ਲੰਮਾਈ;
            ਜੇ (i < (ਵਿਸਤਾਰ_ਸ਼ੁਰੂ + len)) {
                int32_t n = (i - ਵਿਸਤਾਰ_ਸ਼ੁਰੂ) / ਵਿਸਤਾਰ_ਲੰਮਾਈ;  // 1 <= n <= remaining - 1
                src_ਸੂਚਕਾਂਕ += n * old_ਲੰਮਾਈ_2;
                repl_ਸੂਚਕਾਂਕ += n * new_ਲੰਮਾਈ_2;
                dest_ਸੂਚਕਾਂਕ += n * new_ਲੰਮਾਈ_2;
                ਬਚਿਆ -= n;
                ਵਾਪਸ 0;
            }
            // Make next() skip all of these edits at once.
            old_ਲੰਮਾਈ_2 *= ਬਚਿਆ;
            new_ਲੰਮਾਈ_2 *= ਬਚਿਆ;
            ਬਚਿਆ = 0;
        }
    }
    ਵਾਪਸ 1;
}

int32_t Edits::Iterator::destination_ਸੂਚਕਾਂਕ_From_ਸਰੋਤ_ਸੂਚਕਾਂਕ(int32_t i, UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t where = ਲੱਭੋ_ਸੂਚਕਾਂਕ(i, ਸੱਚ, error_ਕੋਡ);
    ਜੇ (where < 0) {
        // Error or before the string.
        ਵਾਪਸ 0;
    }
    ਜੇ (where > 0 || i == src_ਸੂਚਕਾਂਕ) {
        // At or after string length, or at start of the found span.
        ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
    }
    ਜੇ (ਬਦਲਿਆ) {
        // In a change span, map to its end.
        ਵਾਪਸ dest_ਸੂਚਕਾਂਕ + new_ਲੰਮਾਈ_2;
    } ਨਹੀਂਤਾਂ {
        // In an unchanged span, offset 1:1 within it.
        ਵਾਪਸ dest_ਸੂਚਕਾਂਕ + (i - src_ਸੂਚਕਾਂਕ);
    }
}

int32_t Edits::Iterator::ਸਰੋਤ_ਸੂਚਕਾਂਕ_From_Destination_ਸੂਚਕਾਂਕ(int32_t i, UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t where = ਲੱਭੋ_ਸੂਚਕਾਂਕ(i, ਝੂਠ, error_ਕੋਡ);
    ਜੇ (where < 0) {
        // Error or before the string.
        ਵਾਪਸ 0;
    }
    ਜੇ (where > 0 || i == dest_ਸੂਚਕਾਂਕ) {
        // At or after string length, or at start of the found span.
        ਵਾਪਸ src_ਸੂਚਕਾਂਕ;
    }
    ਜੇ (ਬਦਲਿਆ) {
        // In a change span, map to its end.
        ਵਾਪਸ src_ਸੂਚਕਾਂਕ + old_ਲੰਮਾਈ_2;
    } ਨਹੀਂਤਾਂ {
        // In an unchanged span, offset within it.
        ਵਾਪਸ src_ਸੂਚਕਾਂਕ + (i - dest_ਸੂਚਕਾਂਕ);
    }
}

UnicodeString& Edits::Iterator::ਮੰਜ਼ਿਲ_String(UnicodeString& sb U_LIFETIME_BOUND) ਅਚਲ {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_ਸੰਖਿਆ(sb, src_ਸੂਚਕਾਂਕ);
    sb.append(u"..", -1);
    ICU_Utility::append_ਸੰਖਿਆ(sb, src_ਸੂਚਕਾਂਕ + old_ਲੰਮਾਈ_2);
    ਜੇ (ਬਦਲਿਆ) {
        sb.append(u"] ⇝ dest[", -1);
    } ਨਹੀਂਤਾਂ {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_ਸੰਖਿਆ(sb, dest_ਸੂਚਕਾਂਕ);
    sb.append(u"..", -1);
    ICU_Utility::append_ਸੰਖਿਆ(sb, dest_ਸੂਚਕਾਂਕ + new_ਲੰਮਾਈ_2);
    ਜੇ (ਬਦਲਿਆ) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_ਸੰਖਿਆ(sb, repl_ਸੂਚਕਾਂਕ);
        sb.append(u"..", -1);
        ICU_Utility::append_ਸੰਖਿਆ(sb, repl_ਸੂਚਕਾਂਕ + new_ਲੰਮਾਈ_2);
        sb.append(u"] }", -1);
    } ਨਹੀਂਤਾਂ {
        sb.append(u"] (no-change) }", -1);
    }
    ਵਾਪਸ sb;
}

U_NAMESPACE_ਸਿਰਾ
