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
ಧ್ರುವ int32_t MAX_UNCHANGED_LENGTH = 0x1000;
ಧ್ರುವ int32_t MAX_UNCHANGED = MAX_UNCHANGED_LENGTH - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ಧ್ರುವ int32_t MAX_SHORT_CHANGE_OLD_LENGTH = 6;
ಧ್ರುವ int32_t MAX_SHORT_CHANGE_NEW_LENGTH = 7;
ಧ್ರುವ int32_t SHORT_CHANGE_ಅಂಕೆ_MASK = 0x1ff;
ಧ್ರುವ int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ಧ್ರುವ int32_t LENGTH_IN_1TRAIL = 61;
ಧ್ರುವ int32_t LENGTH_IN_2TRAIL = 62;

}  // namespace

ಶೂನ್ಯ Edits::releaseArray() noexcept {
    ಯದಿ (array != ರಾಶಿ_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ಧ್ರುವ Edits& other) U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode_)) {
        length = delta = ಅಂಕೆ_Changes = 0;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        ಯದಿ (newArray == nullptr) {
            length = delta = ಅಂಕೆ_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    ಯದಿ (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode_)) {
        length = delta = ಅಂಕೆ_Changes = 0;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    releaseArray();
    ಯದಿ (length > ರಾಶಿ_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ರಾಶಿ_Array;
        src.capacity = ರಾಶಿ_CAPACITY;
        src.reset();
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    array = ರಾಶಿ_Array;
    capacity = ರಾಶಿ_CAPACITY;
    ಯದಿ (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

Edits& Edits::operator=(ಧ್ರುವ Edits& other) U_LIFETIME_BOUND {
    ಯದಿ (ಏತತ್ == &other) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    ಅಂಕೆ_Changes = other.ಅಂಕೆ_Changes;
    errorCode_ = other.errorCode_;
    ಪ್ರತ್ಯಾಗಮ copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    ಅಂಕೆ_Changes = src.ಅಂಕೆ_Changes;
    errorCode_ = src.errorCode_;
    ಪ್ರತ್ಯಾಗಮ moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

ಶೂನ್ಯ Edits::reset() noexcept {
    length = delta = ಅಂಕೆ_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

ಶೂನ್ಯ Edits::addUnchanged(int32_t unchangedLength) {
    ಯದಿ(U_FAILURE(errorCode_) || unchangedLength == 0) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    ಯದಿ(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        ಯದಿ (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            ಪ್ರತ್ಯಾಗಮ;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    ಯಾವತ್(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    ಯದಿ(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

ಶೂನ್ಯ Edits::addReplace(int32_t oldLength, int32_t newLength) {
    ಯದಿ(U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (oldLength == 0 && newLength == 0) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ++ಅಂಕೆ_Changes;
    int32_t newDelta = newLength - oldLength;
    ಯದಿ (newDelta != 0) {
        ಯದಿ ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        delta += newDelta;
    }

    ಯದಿ(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        ಯದಿ(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_ಅಂಕೆ_MASK) == u &&
                (last & SHORT_CHANGE_ಅಂಕೆ_MASK) < SHORT_CHANGE_ಅಂಕೆ_MASK) {
            setLastUnit(last + 1);
            ಪ್ರತ್ಯಾಗಮ;
        }
        append(u);
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t head = 0x7000;
    ಯದಿ (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } ಅನ್ಯಥಾ ಯದಿ ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        ಯದಿ(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } ಅನ್ಯಥಾ ಯದಿ(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } ಅನ್ಯಥಾ {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        ಯದಿ(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } ಅನ್ಯಥಾ ಯದಿ(newLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        } ಅನ್ಯಥಾ {
            head |= LENGTH_IN_2TRAIL + (newLength >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (newLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        }
        array[length] = static_cast<uint16_t>(head);
        length = limit;
    }
}

ಶೂನ್ಯ Edits::append(int32_t r) {
    ಯದಿ(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    ಯದಿ (array == ರಾಶಿ_Array) {
        newCapacity = 2000;
    } ಅನ್ಯಥಾ ಯದಿ (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } ಅನ್ಯಥಾ {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    ಯದಿ ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    ಯದಿ (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(outErrorCode)) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }
    ಯದಿ (U_SUCCESS(errorCode_)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    outErrorCode = errorCode_;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

Edits& Edits::mergeAndAppend(ಧ್ರುವ Edits& ab,
                             ಧ್ರುವ Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    ಯದಿ (copyErrorTo(errorCode)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool abHasNext = ಸತ್ಯ, bcHasNext = ಸತ್ಯ;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t aLength = 0, ab_bLength = 0, bc_bLength = 0, cLength = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_aLength = 0, pending_cLength = 0;
    ಪ್ರತಿ (;;) {
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
        ಯದಿ (bc_bLength == 0) {
            ಯದಿ (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                ಯದಿ (bc_bLength == 0) {
                    // insertion
                    ಯದಿ (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } ಅನ್ಯಥಾ {
                        pending_cLength += cLength;
                    }
                    ಅನುವರ್ತ;
                }
            }
            // else see if the other iterator is done, too.
        }
        ಯದಿ (ab_bLength == 0) {
            ಯದಿ (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                ಯದಿ (ab_bLength == 0) {
                    // deletion
                    ಯದಿ (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } ಅನ್ಯಥಾ {
                        pending_aLength += aLength;
                    }
                    ಅನುವರ್ತ;
                }
            } ಅನ್ಯಥಾ ಯದಿ (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                ಭಙ್ಗ;
            } ಅನ್ಯಥಾ {
                // The ab output string is shorter than the bc input string.
                ಯದಿ (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
            }
        }
        ಯದಿ (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            ಯದಿ (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        ಯದಿ (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            ಯದಿ (pending_aLength != 0 || pending_cLength != 0) {
                addReplace(pending_aLength, pending_cLength);
                pending_aLength = pending_cLength = 0;
            }
            int32_t unchangedLength = aLength <= cLength ? aLength : cLength;
            addUnchanged(unchangedLength);
            ab_bLength = aLength -= unchangedLength;
            bc_bLength = cLength -= unchangedLength;
            // At least one of the unchanged spans is now empty.
            ಅನುವರ್ತ;
        }
        ಯದಿ (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            ಯದಿ (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                ಅನುವರ್ತ;
            }
            // Handle the shorter unchanged span below like a change.
        } ಅನ್ಯಥಾ ಯದಿ (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            ಯದಿ (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                ಅನುವರ್ತ;
            }
            // Handle the shorter unchanged span below like a change.
        } ಅನ್ಯಥಾ {  // both abIter.hasChange() && bcIter.hasChange()
            ಯದಿ (ab_bLength == bc_bLength) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_aLength + aLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                ab_bLength = bc_bLength = 0;
                ಅನುವರ್ತ;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_aLength += aLength;
        pending_cLength += cLength;
        ಯದಿ (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } ಅನ್ಯಥಾ {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    ಯದಿ (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

Edits::Iterator::Iterator(ಧ್ರುವ uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(ಅಸತ್ಯ), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    ಯದಿ (head < LENGTH_IN_1TRAIL) {
        ಪ್ರತ್ಯಾಗಮ head;
    } ಅನ್ಯಥಾ ಯದಿ (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        ಪ್ರತ್ಯಾಗಮ array[index++] & 0x7fff;
    } ಅನ್ಯಥಾ {
        U_ASSERT((index + 2) <= length);
        U_ASSERT(array[index] >= 0x8000);
        U_ASSERT(array[index + 1] >= 0x8000);
        int32_t len = ((head & 1) << 30) |
                (static_cast<int32_t>(array[index] & 0x7fff) << 15) |
                (array[index + 1] & 0x7fff);
        index += 2;
        ಪ್ರತ್ಯಾಗಮ len;
    }
}

ಶೂನ್ಯ Edits::Iterator::updateNextIndexes() {
    srcIndex += oldLength_;
    ಯದಿ (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

ಶೂನ್ಯ Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    ಯದಿ (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = ಅಸತ್ಯ;
    oldLength_ = newLength_ = 0;
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ಯದಿ (dir > 0) {
        updateNextIndexes();
    } ಅನ್ಯಥಾ {
        ಯದಿ (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            ಯದಿ (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }
        dir = 1;
    }
    ಯದಿ (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        ಯದಿ (remaining > 1) {
            --remaining;
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
        remaining = 0;
    }
    ಯದಿ (index >= length) {
        ಪ್ರತ್ಯಾಗಮ noNext();
    }
    int32_t u = array[index++];
    ಯದಿ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = ಅಸತ್ಯ;
        oldLength_ = u + 1;
        ಯಾವತ್ (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        ಯದಿ (onlyChanges) {
            updateNextIndexes();
            ಯದಿ (index >= length) {
                ಪ್ರತ್ಯಾಗಮ noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }
    changed = ಸತ್ಯ;
    ಯದಿ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_MASK) + 1;
        ಯದಿ (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } ಅನ್ಯಥಾ {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            ಯದಿ (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    } ಅನ್ಯಥಾ {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        ಯದಿ (!coarse) {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }
    // Combine adjacent changes.
    ಯಾವತ್ (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        ಯದಿ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } ಅನ್ಯಥಾ {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ಯದಿ (dir >= 0) {
        ಯದಿ (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            ಯದಿ (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    ಯದಿ (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        ಯದಿ (remaining <= (u & SHORT_CHANGE_ಅಂಕೆ_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
        remaining = 0;
    }
    ಯದಿ (index <= 0) {
        ಪ್ರತ್ಯಾಗಮ noNext();
    }
    int32_t u = array[--index];
    ಯದಿ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = ಅಸತ್ಯ;
        oldLength_ = u + 1;
        ಯಾವತ್ (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    changed = ಸತ್ಯ;
    ಯದಿ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_MASK) + 1;
        ಯದಿ (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } ಅನ್ಯಥಾ {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            ಯದಿ (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    } ಅನ್ಯಥಾ {
        ಯದಿ (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } ಅನ್ಯಥಾ {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            ಯಾವತ್ ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        ಯದಿ (!coarse) {
            updatePreviousIndexes();
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }
    // Combine adjacent changes.
    ಯಾವತ್ (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        ಯದಿ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } ಅನ್ಯಥಾ ಯದಿ (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode) || i < 0) { ಪ್ರತ್ಯಾಗಮ -1; }
    int32_t spanStart, spanLength;
    ಯದಿ (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } ಅನ್ಯಥಾ {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    ಯದಿ (i < spanStart) {
        ಯದಿ (i >= (spanStart / 2)) {
            // Search backwards.
            ಪ್ರತಿ (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (ಶೂನ್ಯ)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                ಯದಿ (i >= spanStart) {
                    // The index is in the current span.
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_ಅಂಕೆ_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    ಯದಿ (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / spanLength) + 1;
                        // 1 <= n <= num
                        srcIndex -= n * oldLength_;
                        replIndex -= n * newLength_;
                        destIndex -= n * newLength_;
                        remaining += n;
                        ಪ್ರತ್ಯಾಗಮ 0;
                    }
                    // Skip all of these edits at once.
                    srcIndex -= num * oldLength_;
                    replIndex -= num * newLength_;
                    destIndex -= num * newLength_;
                    remaining = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        index = remaining = oldLength_ = newLength_ = srcIndex = replIndex = destIndex = 0;
    } ಅನ್ಯಥಾ ಯದಿ (i < (spanStart + spanLength)) {
        // The index is in the current span.
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯಾವತ್ (next(ಅಸತ್ಯ, errorCode)) {
        ಯದಿ (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } ಅನ್ಯಥಾ {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        ಯದಿ (i < (spanStart + spanLength)) {
            // The index is in the current span.
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            ಯದಿ (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / spanLength;  // 1 <= n <= remaining - 1
                srcIndex += n * oldLength_;
                replIndex += n * newLength_;
                destIndex += n * newLength_;
                remaining -= n;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            // Make next() skip all of these edits at once.
            oldLength_ *= remaining;
            newLength_ *= remaining;
            remaining = 0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

int32_t Edits::Iterator::destinationIndexFromSourceIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, ಸತ್ಯ, errorCode);
    ಯದಿ (where < 0) {
        // Error or before the string.
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        ಪ್ರತ್ಯಾಗಮ destIndex;
    }
    ಯದಿ (changed) {
        // In a change span, map to its end.
        ಪ್ರತ್ಯಾಗಮ destIndex + newLength_;
    } ಅನ್ಯಥಾ {
        // In an unchanged span, offset 1:1 within it.
        ಪ್ರತ್ಯಾಗಮ destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, ಅಸತ್ಯ, errorCode);
    ಯದಿ (where < 0) {
        // Error or before the string.
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        ಪ್ರತ್ಯಾಗಮ srcIndex;
    }
    ಯದಿ (changed) {
        // In a change span, map to its end.
        ಪ್ರತ್ಯಾಗಮ srcIndex + oldLength_;
    } ಅನ್ಯಥಾ {
        // In an unchanged span, offset within it.
        ಪ್ರತ್ಯಾಗಮ srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) ಧ್ರುವ {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_ಸಂಖ್ಯೆ(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_ಸಂಖ್ಯೆ(sb, srcIndex + oldLength_);
    ಯದಿ (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } ಅನ್ಯಥಾ {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_ಸಂಖ್ಯೆ(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_ಸಂಖ್ಯೆ(sb, destIndex + newLength_);
    ಯದಿ (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_ಸಂಖ್ಯೆ(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::append_ಸಂಖ್ಯೆ(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } ಅನ್ಯಥಾ {
        sb.append(u"] (no-change) }", -1);
    }
    ಪ್ರತ್ಯಾಗಮ sb;
}

U_NAMESPACE_END
