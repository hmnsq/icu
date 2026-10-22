// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ "unicode/edits.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "util.h"

U_NAMESPACE_BEGIN

ନାମାକାଶ {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
ଅଚଳ int32_t MAX_UNCHANGED_LENGTH = 0x1000;
ଅଚଳ int32_t MAX_UNCHANGED = MAX_UNCHANGED_LENGTH - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ଅଚଳ int32_t MAX_SHORT_CHANGE_OLD_LENGTH = 6;
ଅଚଳ int32_t MAX_SHORT_CHANGE_NEW_LENGTH = 7;
ଅଚଳ int32_t SHORT_CHANGE_ଅଙ୍କ_MASK = 0x1ff;
ଅଚଳ int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ଅଚଳ int32_t LENGTH_IN_1TRAIL = 61;
ଅଚଳ int32_t LENGTH_IN_2TRAIL = 62;

}  // namespace

ଶୂନ୍ୟ Edits::releaseArray() noexcept {
    ଯଦି (array != ଗଦା_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ଅଚଳ Edits& other) U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode_)) {
        length = delta = ଅଙ୍କ_Changes = 0;
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        ଯଦି (newArray == nullptr) {
            length = delta = ଅଙ୍କ_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ *ଏହି;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    ଯଦି (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode_)) {
        length = delta = ଅଙ୍କ_Changes = 0;
        ଫେରନ୍ତୁ *ଏହି;
    }
    releaseArray();
    ଯଦି (length > ଗଦା_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ଗଦା_Array;
        src.capacity = ଗଦା_CAPACITY;
        src.reset();
        ଫେରନ୍ତୁ *ଏହି;
    }
    array = ଗଦା_Array;
    capacity = ଗଦା_CAPACITY;
    ଯଦି (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

Edits& Edits::operator=(ଅଚଳ Edits& other) U_LIFETIME_BOUND {
    ଯଦି (ଏହି == &other) { ଫେରନ୍ତୁ *ଏହି; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    ଅଙ୍କ_Changes = other.ଅଙ୍କ_Changes;
    errorCode_ = other.errorCode_;
    ଫେରନ୍ତୁ copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    ଅଙ୍କ_Changes = src.ଅଙ୍କ_Changes;
    errorCode_ = src.errorCode_;
    ଫେରନ୍ତୁ moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

ଶୂନ୍ୟ Edits::reset() noexcept {
    length = delta = ଅଙ୍କ_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

ଶୂନ୍ୟ Edits::addUnchanged(int32_t unchangedLength) {
    ଯଦି(U_FAILURE(errorCode_) || unchangedLength == 0) { ଫେରନ୍ତୁ; }
    ଯଦି(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    ଯଦି(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        ଯଦି (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            ଫେରନ୍ତୁ;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    ଯେତେବେଳେ(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    ଯଦି(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

ଶୂନ୍ୟ Edits::addReplace(int32_t oldLength, int32_t newLength) {
    ଯଦି(U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ; }
    ଯଦି(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଯଦି (oldLength == 0 && newLength == 0) {
        ଫେରନ୍ତୁ;
    }
    ++ଅଙ୍କ_Changes;
    int32_t newDelta = newLength - oldLength;
    ଯଦି (newDelta != 0) {
        ଯଦି ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ;
        }
        delta += newDelta;
    }

    ଯଦି(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        ଯଦି(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_ଅଙ୍କ_MASK) == u &&
                (last & SHORT_CHANGE_ଅଙ୍କ_MASK) < SHORT_CHANGE_ଅଙ୍କ_MASK) {
            setLastUnit(last + 1);
            ଫେରନ୍ତୁ;
        }
        append(u);
        ଫେରନ୍ତୁ;
    }

    int32_t head = 0x7000;
    ଯଦି (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } ଅନ୍ୟଥା ଯଦି ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        ଯଦି(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } ଅନ୍ୟଥା ଯଦି(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } ଅନ୍ୟଥା {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        ଯଦି(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } ଅନ୍ୟଥା ଯଦି(newLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        } ଅନ୍ୟଥା {
            head |= LENGTH_IN_2TRAIL + (newLength >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (newLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        }
        array[length] = static_cast<uint16_t>(head);
        length = limit;
    }
}

ଶୂନ୍ୟ Edits::append(int32_t r) {
    ଯଦି(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    ଯଦି (array == ଗଦା_Array) {
        newCapacity = 2000;
    } ଅନ୍ୟଥା ଯଦି (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    } ଅନ୍ୟଥା ଯଦି (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } ଅନ୍ୟଥା {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    ଯଦି ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    ଯଦି (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(outErrorCode)) { ଫେରନ୍ତୁ ସତ୍ୟ; }
    ଯଦି (U_SUCCESS(errorCode_)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    outErrorCode = errorCode_;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

Edits& Edits::mergeAndAppend(ଅଚଳ Edits& ab,
                             ଅଚଳ Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    ଯଦି (copyErrorTo(errorCode)) { ଫେରନ୍ତୁ *ଏହି; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool abHasNext = ସତ୍ୟ, bcHasNext = ସତ୍ୟ;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t aLength = 0, ab_bLength = 0, bc_bLength = 0, cLength = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_aLength = 0, pending_cLength = 0;
    ପାଇଁ (;;) {
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
        ଯଦି (bc_bLength == 0) {
            ଯଦି (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                ଯଦି (bc_bLength == 0) {
                    // insertion
                    ଯଦି (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } ଅନ୍ୟଥା {
                        pending_cLength += cLength;
                    }
                    ଜାରି;
                }
            }
            // else see if the other iterator is done, too.
        }
        ଯଦି (ab_bLength == 0) {
            ଯଦି (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                ଯଦି (ab_bLength == 0) {
                    // deletion
                    ଯଦି (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } ଅନ୍ୟଥା {
                        pending_aLength += aLength;
                    }
                    ଜାରି;
                }
            } ଅନ୍ୟଥା ଯଦି (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                ଭାଙ୍ଗ;
            } ଅନ୍ୟଥା {
                // The ab output string is shorter than the bc input string.
                ଯଦି (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                ଫେରନ୍ତୁ *ଏହି;
            }
        }
        ଯଦି (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            ଯଦି (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            ଫେରନ୍ତୁ *ଏହି;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        ଯଦି (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            ଯଦି (pending_aLength != 0 || pending_cLength != 0) {
                addReplace(pending_aLength, pending_cLength);
                pending_aLength = pending_cLength = 0;
            }
            int32_t unchangedLength = aLength <= cLength ? aLength : cLength;
            addUnchanged(unchangedLength);
            ab_bLength = aLength -= unchangedLength;
            bc_bLength = cLength -= unchangedLength;
            // At least one of the unchanged spans is now empty.
            ଜାରି;
        }
        ଯଦି (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            ଯଦି (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                ଜାରି;
            }
            // Handle the shorter unchanged span below like a change.
        } ଅନ୍ୟଥା ଯଦି (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            ଯଦି (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                ଜାରି;
            }
            // Handle the shorter unchanged span below like a change.
        } ଅନ୍ୟଥା {  // both abIter.hasChange() && bcIter.hasChange()
            ଯଦି (ab_bLength == bc_bLength) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_aLength + aLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                ab_bLength = bc_bLength = 0;
                ଜାରି;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_aLength += aLength;
        pending_cLength += cLength;
        ଯଦି (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } ଅନ୍ୟଥା {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    ଯଦି (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    ଫେରନ୍ତୁ *ଏହି;
}

Edits::Iterator::Iterator(ଅଚଳ uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(ମିଥ୍ୟା), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    ଯଦି (head < LENGTH_IN_1TRAIL) {
        ଫେରନ୍ତୁ head;
    } ଅନ୍ୟଥା ଯଦି (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        ଫେରନ୍ତୁ array[index++] & 0x7fff;
    } ଅନ୍ୟଥା {
        U_ASSERT((index + 2) <= length);
        U_ASSERT(array[index] >= 0x8000);
        U_ASSERT(array[index + 1] >= 0x8000);
        int32_t len = ((head & 1) << 30) |
                (static_cast<int32_t>(array[index] & 0x7fff) << 15) |
                (array[index + 1] & 0x7fff);
        index += 2;
        ଫେରନ୍ତୁ len;
    }
}

ଶୂନ୍ୟ Edits::Iterator::updateNextIndexes() {
    srcIndex += oldLength_;
    ଯଦି (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

ଶୂନ୍ୟ Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    ଯଦି (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = ମିଥ୍ୟା;
    oldLength_ = newLength_ = 0;
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ଯଦି (dir > 0) {
        updateNextIndexes();
    } ଅନ୍ୟଥା {
        ଯଦି (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            ଯଦି (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
        }
        dir = 1;
    }
    ଯଦି (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        ଯଦି (remaining > 1) {
            --remaining;
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
        remaining = 0;
    }
    ଯଦି (index >= length) {
        ଫେରନ୍ତୁ noNext();
    }
    int32_t u = array[index++];
    ଯଦି (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = ମିଥ୍ୟା;
        oldLength_ = u + 1;
        ଯେତେବେଳେ (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        ଯଦି (onlyChanges) {
            updateNextIndexes();
            ଯଦି (index >= length) {
                ଫେରନ୍ତୁ noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }
    changed = ସତ୍ୟ;
    ଯଦି (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_ଅଙ୍କ_MASK) + 1;
        ଯଦି (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } ଅନ୍ୟଥା {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            ଯଦି (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    } ଅନ୍ୟଥା {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        ଯଦି (!coarse) {
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }
    // Combine adjacent changes.
    ଯେତେବେଳେ (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        ଯଦି (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_ଅଙ୍କ_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } ଅନ୍ୟଥା {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ଯଦି (dir >= 0) {
        ଯଦି (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            ଯଦି (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    ଯଦି (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        ଯଦି (remaining <= (u & SHORT_CHANGE_ଅଙ୍କ_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
        remaining = 0;
    }
    ଯଦି (index <= 0) {
        ଫେରନ୍ତୁ noNext();
    }
    int32_t u = array[--index];
    ଯଦି (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = ମିଥ୍ୟା;
        oldLength_ = u + 1;
        ଯେତେବେଳେ (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    changed = ସତ୍ୟ;
    ଯଦି (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_ଅଙ୍କ_MASK) + 1;
        ଯଦି (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } ଅନ୍ୟଥା {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            ଯଦି (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    } ଅନ୍ୟଥା {
        ଯଦି (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } ଅନ୍ୟଥା {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            ଯେତେବେଳେ ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        ଯଦି (!coarse) {
            updatePreviousIndexes();
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }
    // Combine adjacent changes.
    ଯେତେବେଳେ (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        ଯଦି (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_ଅଙ୍କ_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } ଅନ୍ୟଥା ଯଦି (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode) || i < 0) { ଫେରନ୍ତୁ -1; }
    int32_t spanStart, spanLength;
    ଯଦି (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } ଅନ୍ୟଥା {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    ଯଦି (i < spanStart) {
        ଯଦି (i >= (spanStart / 2)) {
            // Search backwards.
            ପାଇଁ (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (ଶୂନ୍ୟ)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                ଯଦି (i >= spanStart) {
                    // The index is in the current span.
                    ଫେରନ୍ତୁ 0;
                }
                ଯଦି (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_ଅଙ୍କ_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    ଯଦି (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / spanLength) + 1;
                        // 1 <= n <= num
                        srcIndex -= n * oldLength_;
                        replIndex -= n * newLength_;
                        destIndex -= n * newLength_;
                        remaining += n;
                        ଫେରନ୍ତୁ 0;
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
    } ଅନ୍ୟଥା ଯଦି (i < (spanStart + spanLength)) {
        // The index is in the current span.
        ଫେରନ୍ତୁ 0;
    }
    ଯେତେବେଳେ (next(ମିଥ୍ୟା, errorCode)) {
        ଯଦି (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } ଅନ୍ୟଥା {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        ଯଦି (i < (spanStart + spanLength)) {
            // The index is in the current span.
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            ଯଦି (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / spanLength;  // 1 <= n <= remaining - 1
                srcIndex += n * oldLength_;
                replIndex += n * newLength_;
                destIndex += n * newLength_;
                remaining -= n;
                ଫେରନ୍ତୁ 0;
            }
            // Make next() skip all of these edits at once.
            oldLength_ *= remaining;
            newLength_ *= remaining;
            remaining = 0;
        }
    }
    ଫେରନ୍ତୁ 1;
}

int32_t Edits::Iterator::destinationIndexFromSourceIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, ସତ୍ୟ, errorCode);
    ଯଦି (where < 0) {
        // Error or before the string.
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        ଫେରନ୍ତୁ destIndex;
    }
    ଯଦି (changed) {
        // In a change span, map to its end.
        ଫେରନ୍ତୁ destIndex + newLength_;
    } ଅନ୍ୟଥା {
        // In an unchanged span, offset 1:1 within it.
        ଫେରନ୍ତୁ destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, ମିଥ୍ୟା, errorCode);
    ଯଦି (where < 0) {
        // Error or before the string.
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        ଫେରନ୍ତୁ srcIndex;
    }
    ଯଦି (changed) {
        // In a change span, map to its end.
        ଫେରନ୍ତୁ srcIndex + oldLength_;
    } ଅନ୍ୟଥା {
        // In an unchanged span, offset within it.
        ଫେରନ୍ତୁ srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) ଅଚଳ {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_ସଂଖ୍ୟା(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_ସଂଖ୍ୟା(sb, srcIndex + oldLength_);
    ଯଦି (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } ଅନ୍ୟଥା {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_ସଂଖ୍ୟା(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_ସଂଖ୍ୟା(sb, destIndex + newLength_);
    ଯଦି (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_ସଂଖ୍ୟା(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::append_ସଂଖ୍ୟା(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } ଅନ୍ୟଥା {
        sb.append(u"] (no-change) }", -1);
    }
    ଫେରନ୍ତୁ sb;
}

U_NAMESPACE_END
