// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#include "unicode/edits.h"
#include "unicode/unistr.h"
#include "unicode/utypes.h"
#include "cmemory.h"
#include "uassert.h"
#include "util.h"

U_NAMESPACE_BEGIN

namespace {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
const int32_t MAX_UNCHANGED_LENGTH = 0x1000;
const int32_t MAX_UNCHANGED = MAX_UNCHANGED_LENGTH - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
const int32_t MAX_SHORT_CHANGE_OLD_LENGTH = 6;
const int32_t MAX_SHORT_CHANGE_NEW_LENGTH = 7;
const int32_t SHORT_CHANGE_NUM_MASK = 0x1ff;
const int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
const int32_t LENGTH_IN_1TRAIL = 61;
const int32_t LENGTH_IN_2TRAIL = 62;

}  // namespace

void Edits::releaseArray() noexcept {
    නම් (array != stackArray) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(const Edits& other) U_LIFETIME_BOUND {
    නම් (U_FAILURE(errorCode_)) {
        length = delta = numChanges = 0;
        ආපසු *this;
    }
    නම් (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        නම් (newArray == nullptr) {
            length = delta = numChanges = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ආපසු *this;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    නම් (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    ආපසු *this;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    නම් (U_FAILURE(errorCode_)) {
        length = delta = numChanges = 0;
        ආපසු *this;
    }
    releaseArray();
    නම් (length > STACK_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.stackArray;
        src.capacity = STACK_CAPACITY;
        src.reset();
        ආපසු *this;
    }
    array = stackArray;
    capacity = STACK_CAPACITY;
    නම් (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    ආපසු *this;
}

Edits& Edits::operator=(const Edits& other) U_LIFETIME_BOUND {
    නම් (this == &other) { ආපසු *this; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    numChanges = other.numChanges;
    errorCode_ = other.errorCode_;
    ආපසු copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    numChanges = src.numChanges;
    errorCode_ = src.errorCode_;
    ආපසු moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

void Edits::reset() noexcept {
    length = delta = numChanges = 0;
    errorCode_ = U_ZERO_ERROR;
}

void Edits::addUnchanged(int32_t unchangedLength) {
    නම්(U_FAILURE(errorCode_) || unchangedLength == 0) { ආපසු; }
    නම්(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        ආපසු;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    නම්(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        නම් (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            ආපසු;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    අතරතුර(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    නම්(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

void Edits::addReplace(int32_t oldLength, int32_t newLength) {
    නම්(U_FAILURE(errorCode_)) { ආපසු; }
    නම්(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        ආපසු;
    }
    නම් (oldLength == 0 && newLength == 0) {
        ආපසු;
    }
    ++numChanges;
    int32_t newDelta = newLength - oldLength;
    නම් (newDelta != 0) {
        නම් ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            ආපසු;
        }
        delta += newDelta;
    }

    නම්(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        නම්(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_NUM_MASK) == u &&
                (last & SHORT_CHANGE_NUM_MASK) < SHORT_CHANGE_NUM_MASK) {
            setLastUnit(last + 1);
            ආපසු;
        }
        append(u);
        ආපසු;
    }

    int32_t head = 0x7000;
    නම් (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } නැතහොත් නම් ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        නම්(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } නැතහොත් නම්(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } නැතහොත් {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        නම්(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } නැතහොත් නම්(newLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        } නැතහොත් {
            head |= LENGTH_IN_2TRAIL + (newLength >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (newLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        }
        array[length] = static_cast<uint16_t>(head);
        length = limit;
    }
}

void Edits::append(int32_t r) {
    නම්(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    නම් (array == stackArray) {
        newCapacity = 2000;
    } නැතහොත් නම් (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        ආපසු false;
    } නැතහොත් නම් (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } නැතහොත් {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    නම් ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        ආපසු false;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    නම් (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ආපසු false;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    ආපසු true;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) const {
    නම් (U_FAILURE(outErrorCode)) { ආපසු true; }
    නම් (U_SUCCESS(errorCode_)) { ආපසු false; }
    outErrorCode = errorCode_;
    ආපසු true;
}

Edits& Edits::mergeAndAppend(const Edits& ab,
                             const Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    නම් (copyErrorTo(errorCode)) { ආපසු *this; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool abHasNext = true, bcHasNext = true;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t aLength = 0, ab_bLength = 0, bc_bLength = 0, cLength = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_aLength = 0, pending_cLength = 0;
    සඳහා (;;) {
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
        නම් (bc_bLength == 0) {
            නම් (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                නම් (bc_bLength == 0) {
                    // insertion
                    නම් (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } නැතහොත් {
                        pending_cLength += cLength;
                    }
                    continue;
                }
            }
            // else see if the other iterator is done, too.
        }
        නම් (ab_bLength == 0) {
            නම් (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                නම් (ab_bLength == 0) {
                    // deletion
                    නම් (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } නැතහොත් {
                        pending_aLength += aLength;
                    }
                    continue;
                }
            } නැතහොත් නම් (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                bha_nga;
            } නැතහොත් {
                // The ab output string is shorter than the bc input string.
                නම් (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                ආපසු *this;
            }
        }
        නම් (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            නම් (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            ආපසු *this;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        නම් (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            නම් (pending_aLength != 0 || pending_cLength != 0) {
                addReplace(pending_aLength, pending_cLength);
                pending_aLength = pending_cLength = 0;
            }
            int32_t unchangedLength = aLength <= cLength ? aLength : cLength;
            addUnchanged(unchangedLength);
            ab_bLength = aLength -= unchangedLength;
            bc_bLength = cLength -= unchangedLength;
            // At least one of the unchanged spans is now empty.
            continue;
        }
        නම් (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            නම් (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                continue;
            }
            // Handle the shorter unchanged span below like a change.
        } නැතහොත් නම් (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            නම් (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                continue;
            }
            // Handle the shorter unchanged span below like a change.
        } නැතහොත් {  // both abIter.hasChange() && bcIter.hasChange()
            නම් (ab_bLength == bc_bLength) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_aLength + aLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                ab_bLength = bc_bLength = 0;
                continue;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_aLength += aLength;
        pending_cLength += cLength;
        නම් (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } නැතහොත් {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    නම් (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    ආපසු *this;
}

Edits::Iterator::Iterator(const uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(false), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    නම් (head < LENGTH_IN_1TRAIL) {
        ආපසු head;
    } නැතහොත් නම් (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        ආපසු array[index++] & 0x7fff;
    } නැතහොත් {
        U_ASSERT((index + 2) <= length);
        U_ASSERT(array[index] >= 0x8000);
        U_ASSERT(array[index + 1] >= 0x8000);
        int32_t len = ((head & 1) << 30) |
                (static_cast<int32_t>(array[index] & 0x7fff) << 15) |
                (array[index + 1] & 0x7fff);
        index += 2;
        ආපසු len;
    }
}

void Edits::Iterator::updateNextIndexes() {
    srcIndex += oldLength_;
    නම් (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

void Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    නම් (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = false;
    oldLength_ = newLength_ = 0;
    ආපසු false;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    නම් (U_FAILURE(errorCode)) { ආපසු false; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    නම් (dir > 0) {
        updateNextIndexes();
    } නැතහොත් {
        නම් (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            නම් (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                ආපසු true;
            }
        }
        dir = 1;
    }
    නම් (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        නම් (remaining > 1) {
            --remaining;
            ආපසු true;
        }
        remaining = 0;
    }
    නම් (index >= length) {
        ආපසු noNext();
    }
    int32_t u = array[index++];
    නම් (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = false;
        oldLength_ = u + 1;
        අතරතුර (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        නම් (onlyChanges) {
            updateNextIndexes();
            නම් (index >= length) {
                ආපසු noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } නැතහොත් {
            ආපසු true;
        }
    }
    changed = true;
    නම් (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_NUM_MASK) + 1;
        නම් (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } නැතහොත් {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            නම් (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            ආපසු true;
        }
    } නැතහොත් {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        නම් (!coarse) {
            ආපසු true;
        }
    }
    // Combine adjacent changes.
    අතරතුර (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        නම් (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_NUM_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } නැතහොත් {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    ආපසු true;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    නම් (U_FAILURE(errorCode)) { ආපසු false; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    නම් (dir >= 0) {
        නම් (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            නම් (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                ආපසු true;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    නම් (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        නම් (remaining <= (u & SHORT_CHANGE_NUM_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            ආපසු true;
        }
        remaining = 0;
    }
    නම් (index <= 0) {
        ආපසු noNext();
    }
    int32_t u = array[--index];
    නම් (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = false;
        oldLength_ = u + 1;
        අතරතුර (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        ආපසු true;
    }
    changed = true;
    නම් (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_NUM_MASK) + 1;
        නම් (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } නැතහොත් {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            නම් (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            ආපසු true;
        }
    } නැතහොත් {
        නම් (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } නැතහොත් {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            අතරතුර ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        නම් (!coarse) {
            updatePreviousIndexes();
            ආපසු true;
        }
    }
    // Combine adjacent changes.
    අතරතුර (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        නම් (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_NUM_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } නැතහොත් නම් (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    ආපසු true;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    නම් (U_FAILURE(errorCode) || i < 0) { ආපසු -1; }
    int32_t spanStart, spanLength;
    නම් (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } නැතහොත් {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    නම් (i < spanStart) {
        නම් (i >= (spanStart / 2)) {
            // Search backwards.
            සඳහා (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (void)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                නම් (i >= spanStart) {
                    // The index is in the current span.
                    ආපසු 0;
                }
                නම් (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_NUM_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    නම් (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / spanLength) + 1;
                        // 1 <= n <= num
                        srcIndex -= n * oldLength_;
                        replIndex -= n * newLength_;
                        destIndex -= n * newLength_;
                        remaining += n;
                        ආපසු 0;
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
    } නැතහොත් නම් (i < (spanStart + spanLength)) {
        // The index is in the current span.
        ආපසු 0;
    }
    අතරතුර (next(false, errorCode)) {
        නම් (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } නැතහොත් {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        නම් (i < (spanStart + spanLength)) {
            // The index is in the current span.
            ආපසු 0;
        }
        නම් (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            නම් (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / spanLength;  // 1 <= n <= remaining - 1
                srcIndex += n * oldLength_;
                replIndex += n * newLength_;
                destIndex += n * newLength_;
                remaining -= n;
                ආපසු 0;
            }
            // Make next() skip all of these edits at once.
            oldLength_ *= remaining;
            newLength_ *= remaining;
            remaining = 0;
        }
    }
    ආපසු 1;
}

int32_t Edits::Iterator::destinationIndexFromSourceIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, true, errorCode);
    නම් (where < 0) {
        // Error or before the string.
        ආපසු 0;
    }
    නම් (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        ආපසු destIndex;
    }
    නම් (changed) {
        // In a change span, map to its end.
        ආපසු destIndex + newLength_;
    } නැතහොත් {
        // In an unchanged span, offset 1:1 within it.
        ආපසු destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, false, errorCode);
    නම් (where < 0) {
        // Error or before the string.
        ආපසු 0;
    }
    නම් (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        ආපසු srcIndex;
    }
    නම් (changed) {
        // In a change span, map to its end.
        ආපසු srcIndex + oldLength_;
    } නැතහොත් {
        // In an unchanged span, offset within it.
        ආපසු srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) const {
    sb.append(u"{ src[", -1);
    ICU_Utility::appendNumber(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::appendNumber(sb, srcIndex + oldLength_);
    නම් (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } නැතහොත් {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::appendNumber(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::appendNumber(sb, destIndex + newLength_);
    නම් (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::appendNumber(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::appendNumber(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } නැතහොත් {
        sb.append(u"] (no-change) }", -1);
    }
    ආපසු sb;
}

U_NAMESPACE_END
