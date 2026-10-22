// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "util.h"

U_NAMESPACE_BEGIN

नामाकाश {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
ध्रुव int32_t MAX_UNCHANGED_LENGTH = 0x1000;
ध्रुव int32_t MAX_UNCHANGED = MAX_UNCHANGED_LENGTH - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ध्रुव int32_t MAX_SHORT_CHANGE_OLD_LENGTH = 6;
ध्रुव int32_t MAX_SHORT_CHANGE_NEW_LENGTH = 7;
ध्रुव int32_t SHORT_CHANGE_अंक_MASK = 0x1ff;
ध्रुव int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ध्रुव int32_t LENGTH_IN_1TRAIL = 61;
ध्रुव int32_t LENGTH_IN_2TRAIL = 62;

}  // namespace

शून्य Edits::releaseArray() noexcept {
    यदि (array != ढेर_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ध्रुव Edits& other) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) {
        length = delta = अंक_Changes = 0;
        प्रत्यागम *एतत्;
    }
    यदि (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        यदि (newArray == nullptr) {
            length = delta = अंक_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    यदि (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    प्रत्यागम *एतत्;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) {
        length = delta = अंक_Changes = 0;
        प्रत्यागम *एतत्;
    }
    releaseArray();
    यदि (length > ढेर_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ढेर_Array;
        src.capacity = ढेर_CAPACITY;
        src.reset();
        प्रत्यागम *एतत्;
    }
    array = ढेर_Array;
    capacity = ढेर_CAPACITY;
    यदि (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    प्रत्यागम *एतत्;
}

Edits& Edits::operator=(ध्रुव Edits& other) U_LIFETIME_BOUND {
    यदि (एतत् == &other) { प्रत्यागम *एतत्; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    अंक_Changes = other.अंक_Changes;
    errorCode_ = other.errorCode_;
    प्रत्यागम copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    अंक_Changes = src.अंक_Changes;
    errorCode_ = src.errorCode_;
    प्रत्यागम moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

शून्य Edits::reset() noexcept {
    length = delta = अंक_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

शून्य Edits::addUnchanged(int32_t unchangedLength) {
    यदि(U_FAILURE(errorCode_) || unchangedLength == 0) { प्रत्यागम; }
    यदि(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    यदि(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        यदि (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            प्रत्यागम;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    यावत्(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    यदि(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

शून्य Edits::addReplace(int32_t oldLength, int32_t newLength) {
    यदि(U_FAILURE(errorCode_)) { प्रत्यागम; }
    यदि(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    यदि (oldLength == 0 && newLength == 0) {
        प्रत्यागम;
    }
    ++अंक_Changes;
    int32_t newDelta = newLength - oldLength;
    यदि (newDelta != 0) {
        यदि ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम;
        }
        delta += newDelta;
    }

    यदि(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        यदि(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_अंक_MASK) == u &&
                (last & SHORT_CHANGE_अंक_MASK) < SHORT_CHANGE_अंक_MASK) {
            setLastUnit(last + 1);
            प्रत्यागम;
        }
        append(u);
        प्रत्यागम;
    }

    int32_t head = 0x7000;
    यदि (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } अन्यथा यदि ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        यदि(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } अन्यथा यदि(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } अन्यथा {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        यदि(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } अन्यथा यदि(newLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        } अन्यथा {
            head |= LENGTH_IN_2TRAIL + (newLength >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (newLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        }
        array[length] = static_cast<uint16_t>(head);
        length = limit;
    }
}

शून्य Edits::append(int32_t r) {
    यदि(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    यदि (array == ढेर_Array) {
        newCapacity = 2000;
    } अन्यथा यदि (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम असत्य;
    } अन्यथा यदि (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } अन्यथा {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    यदि ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम असत्य;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    यदि (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम असत्य;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    प्रत्यागम सत्य;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) ध्रुव {
    यदि (U_FAILURE(outErrorCode)) { प्रत्यागम सत्य; }
    यदि (U_SUCCESS(errorCode_)) { प्रत्यागम असत्य; }
    outErrorCode = errorCode_;
    प्रत्यागम सत्य;
}

Edits& Edits::mergeAndAppend(ध्रुव Edits& ab,
                             ध्रुव Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि (copyErrorTo(errorCode)) { प्रत्यागम *एतत्; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool abHasNext = सत्य, bcHasNext = सत्य;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t aLength = 0, ab_bLength = 0, bc_bLength = 0, cLength = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_aLength = 0, pending_cLength = 0;
    प्रति (;;) {
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
        यदि (bc_bLength == 0) {
            यदि (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                यदि (bc_bLength == 0) {
                    // insertion
                    यदि (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } अन्यथा {
                        pending_cLength += cLength;
                    }
                    अनुवर्त;
                }
            }
            // else see if the other iterator is done, too.
        }
        यदि (ab_bLength == 0) {
            यदि (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                यदि (ab_bLength == 0) {
                    // deletion
                    यदि (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } अन्यथा {
                        pending_aLength += aLength;
                    }
                    अनुवर्त;
                }
            } अन्यथा यदि (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                भङ्ग;
            } अन्यथा {
                // The ab output string is shorter than the bc input string.
                यदि (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                प्रत्यागम *एतत्;
            }
        }
        यदि (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            यदि (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            प्रत्यागम *एतत्;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        यदि (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            यदि (pending_aLength != 0 || pending_cLength != 0) {
                addReplace(pending_aLength, pending_cLength);
                pending_aLength = pending_cLength = 0;
            }
            int32_t unchangedLength = aLength <= cLength ? aLength : cLength;
            addUnchanged(unchangedLength);
            ab_bLength = aLength -= unchangedLength;
            bc_bLength = cLength -= unchangedLength;
            // At least one of the unchanged spans is now empty.
            अनुवर्त;
        }
        यदि (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            यदि (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                अनुवर्त;
            }
            // Handle the shorter unchanged span below like a change.
        } अन्यथा यदि (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            यदि (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                अनुवर्त;
            }
            // Handle the shorter unchanged span below like a change.
        } अन्यथा {  // both abIter.hasChange() && bcIter.hasChange()
            यदि (ab_bLength == bc_bLength) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_aLength + aLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                ab_bLength = bc_bLength = 0;
                अनुवर्त;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_aLength += aLength;
        pending_cLength += cLength;
        यदि (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } अन्यथा {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    यदि (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    प्रत्यागम *एतत्;
}

Edits::Iterator::Iterator(ध्रुव uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(असत्य), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    यदि (head < LENGTH_IN_1TRAIL) {
        प्रत्यागम head;
    } अन्यथा यदि (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        प्रत्यागम array[index++] & 0x7fff;
    } अन्यथा {
        U_ASSERT((index + 2) <= length);
        U_ASSERT(array[index] >= 0x8000);
        U_ASSERT(array[index + 1] >= 0x8000);
        int32_t len = ((head & 1) << 30) |
                (static_cast<int32_t>(array[index] & 0x7fff) << 15) |
                (array[index + 1] & 0x7fff);
        index += 2;
        प्रत्यागम len;
    }
}

शून्य Edits::Iterator::updateNextIndexes() {
    srcIndex += oldLength_;
    यदि (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

शून्य Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    यदि (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = असत्य;
    oldLength_ = newLength_ = 0;
    प्रत्यागम असत्य;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    यदि (dir > 0) {
        updateNextIndexes();
    } अन्यथा {
        यदि (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            यदि (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                प्रत्यागम सत्य;
            }
        }
        dir = 1;
    }
    यदि (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        यदि (remaining > 1) {
            --remaining;
            प्रत्यागम सत्य;
        }
        remaining = 0;
    }
    यदि (index >= length) {
        प्रत्यागम noNext();
    }
    int32_t u = array[index++];
    यदि (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = असत्य;
        oldLength_ = u + 1;
        यावत् (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        यदि (onlyChanges) {
            updateNextIndexes();
            यदि (index >= length) {
                प्रत्यागम noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } अन्यथा {
            प्रत्यागम सत्य;
        }
    }
    changed = सत्य;
    यदि (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
        यदि (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } अन्यथा {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            यदि (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            प्रत्यागम सत्य;
        }
    } अन्यथा {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        यदि (!coarse) {
            प्रत्यागम सत्य;
        }
    }
    // Combine adjacent changes.
    यावत् (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        यदि (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } अन्यथा {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    प्रत्यागम सत्य;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    यदि (dir >= 0) {
        यदि (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            यदि (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                प्रत्यागम सत्य;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    यदि (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        यदि (remaining <= (u & SHORT_CHANGE_अंक_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            प्रत्यागम सत्य;
        }
        remaining = 0;
    }
    यदि (index <= 0) {
        प्रत्यागम noNext();
    }
    int32_t u = array[--index];
    यदि (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = असत्य;
        oldLength_ = u + 1;
        यावत् (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        प्रत्यागम सत्य;
    }
    changed = सत्य;
    यदि (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
        यदि (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } अन्यथा {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            यदि (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            प्रत्यागम सत्य;
        }
    } अन्यथा {
        यदि (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } अन्यथा {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            यावत् ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        यदि (!coarse) {
            updatePreviousIndexes();
            प्रत्यागम सत्य;
        }
    }
    // Combine adjacent changes.
    यावत् (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        यदि (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } अन्यथा यदि (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    प्रत्यागम सत्य;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode) || i < 0) { प्रत्यागम -1; }
    int32_t spanStart, spanLength;
    यदि (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } अन्यथा {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    यदि (i < spanStart) {
        यदि (i >= (spanStart / 2)) {
            // Search backwards.
            प्रति (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (शून्य)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                यदि (i >= spanStart) {
                    // The index is in the current span.
                    प्रत्यागम 0;
                }
                यदि (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    यदि (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / spanLength) + 1;
                        // 1 <= n <= num
                        srcIndex -= n * oldLength_;
                        replIndex -= n * newLength_;
                        destIndex -= n * newLength_;
                        remaining += n;
                        प्रत्यागम 0;
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
    } अन्यथा यदि (i < (spanStart + spanLength)) {
        // The index is in the current span.
        प्रत्यागम 0;
    }
    यावत् (next(असत्य, errorCode)) {
        यदि (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } अन्यथा {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        यदि (i < (spanStart + spanLength)) {
            // The index is in the current span.
            प्रत्यागम 0;
        }
        यदि (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            यदि (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / spanLength;  // 1 <= n <= remaining - 1
                srcIndex += n * oldLength_;
                replIndex += n * newLength_;
                destIndex += n * newLength_;
                remaining -= n;
                प्रत्यागम 0;
            }
            // Make next() skip all of these edits at once.
            oldLength_ *= remaining;
            newLength_ *= remaining;
            remaining = 0;
        }
    }
    प्रत्यागम 1;
}

int32_t Edits::Iterator::destinationIndexFromSourceIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, सत्य, errorCode);
    यदि (where < 0) {
        // Error or before the string.
        प्रत्यागम 0;
    }
    यदि (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        प्रत्यागम destIndex;
    }
    यदि (changed) {
        // In a change span, map to its end.
        प्रत्यागम destIndex + newLength_;
    } अन्यथा {
        // In an unchanged span, offset 1:1 within it.
        प्रत्यागम destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, असत्य, errorCode);
    यदि (where < 0) {
        // Error or before the string.
        प्रत्यागम 0;
    }
    यदि (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        प्रत्यागम srcIndex;
    }
    यदि (changed) {
        // In a change span, map to its end.
        प्रत्यागम srcIndex + oldLength_;
    } अन्यथा {
        // In an unchanged span, offset within it.
        प्रत्यागम srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) ध्रुव {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_संख्या(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_संख्या(sb, srcIndex + oldLength_);
    यदि (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } अन्यथा {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_संख्या(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_संख्या(sb, destIndex + newLength_);
    यदि (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_संख्या(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::append_संख्या(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } अन्यथा {
        sb.append(u"] (no-change) }", -1);
    }
    प्रत्यागम sb;
}

U_NAMESPACE_END
