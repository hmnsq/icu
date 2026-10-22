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
ध्रुव int32_t SHORT_CHANGE_अङ्कः_MASK = 0x1ff;
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
    जइ (array != राशिः_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ध्रुव Edits& other) U_LIFETIME_BOUND {
    जइ (U_FAILURE(errorCode_)) {
        length = delta = अङ्कः_Changes = 0;
        प्रत्यागम *एतत्;
    }
    जइ (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        जइ (newArray == nullptr) {
            length = delta = अङ्कः_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    जइ (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    प्रत्यागम *एतत्;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    जइ (U_FAILURE(errorCode_)) {
        length = delta = अङ्कः_Changes = 0;
        प्रत्यागम *एतत्;
    }
    releaseArray();
    जइ (length > राशिः_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.राशिः_Array;
        src.capacity = राशिः_CAPACITY;
        src.reset();
        प्रत्यागम *एतत्;
    }
    array = राशिः_Array;
    capacity = राशिः_CAPACITY;
    जइ (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    प्रत्यागम *एतत्;
}

Edits& Edits::operator=(ध्रुव Edits& other) U_LIFETIME_BOUND {
    जइ (एतत् == &other) { प्रत्यागम *एतत्; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    अङ्कः_Changes = other.अङ्कः_Changes;
    errorCode_ = other.errorCode_;
    प्रत्यागम copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    अङ्कः_Changes = src.अङ्कः_Changes;
    errorCode_ = src.errorCode_;
    प्रत्यागम moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

शून्य Edits::reset() noexcept {
    length = delta = अङ्कः_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

शून्य Edits::addUnchanged(int32_t unchangedLength) {
    जइ(U_FAILURE(errorCode_) || unchangedLength == 0) { प्रत्यागम; }
    जइ(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    जइ(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        जइ (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            प्रत्यागम;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    जाव(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    जइ(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

शून्य Edits::addReplace(int32_t oldLength, int32_t newLength) {
    जइ(U_FAILURE(errorCode_)) { प्रत्यागम; }
    जइ(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    जइ (oldLength == 0 && newLength == 0) {
        प्रत्यागम;
    }
    ++अङ्कः_Changes;
    int32_t newDelta = newLength - oldLength;
    जइ (newDelta != 0) {
        जइ ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम;
        }
        delta += newDelta;
    }

    जइ(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        जइ(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_अङ्कः_MASK) == u &&
                (last & SHORT_CHANGE_अङ्कः_MASK) < SHORT_CHANGE_अङ्कः_MASK) {
            setLastUnit(last + 1);
            प्रत्यागम;
        }
        append(u);
        प्रत्यागम;
    }

    int32_t head = 0x7000;
    जइ (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } अण्णहा जइ ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        जइ(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } अण्णहा जइ(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } अण्णहा {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        जइ(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } अण्णहा जइ(newLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        } अण्णहा {
            head |= LENGTH_IN_2TRAIL + (newLength >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (newLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        }
        array[length] = static_cast<uint16_t>(head);
        length = limit;
    }
}

शून्य Edits::append(int32_t r) {
    जइ(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    जइ (array == राशिः_Array) {
        newCapacity = 2000;
    } अण्णहा जइ (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम मिच्छ;
    } अण्णहा जइ (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } अण्णहा {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    जइ ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम मिच्छ;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    जइ (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम मिच्छ;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    प्रत्यागम सच्च;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) ध्रुव {
    जइ (U_FAILURE(outErrorCode)) { प्रत्यागम सच्च; }
    जइ (U_SUCCESS(errorCode_)) { प्रत्यागम मिच्छ; }
    outErrorCode = errorCode_;
    प्रत्यागम सच्च;
}

Edits& Edits::mergeAndAppend(ध्रुव Edits& ab,
                             ध्रुव Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    जइ (copyErrorTo(errorCode)) { प्रत्यागम *एतत्; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool abHasNext = सच्च, bcHasNext = सच्च;
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
        जइ (bc_bLength == 0) {
            जइ (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                जइ (bc_bLength == 0) {
                    // insertion
                    जइ (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } अण्णहा {
                        pending_cLength += cLength;
                    }
                    अनुवर्त;
                }
            }
            // else see if the other iterator is done, too.
        }
        जइ (ab_bLength == 0) {
            जइ (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                जइ (ab_bLength == 0) {
                    // deletion
                    जइ (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } अण्णहा {
                        pending_aLength += aLength;
                    }
                    अनुवर्त;
                }
            } अण्णहा जइ (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                भङ्ग;
            } अण्णहा {
                // The ab output string is shorter than the bc input string.
                जइ (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                प्रत्यागम *एतत्;
            }
        }
        जइ (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            जइ (!copyErrorTo(errorCode)) {
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

        जइ (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            जइ (pending_aLength != 0 || pending_cLength != 0) {
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
        जइ (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            जइ (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                अनुवर्त;
            }
            // Handle the shorter unchanged span below like a change.
        } अण्णहा जइ (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            जइ (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                अनुवर्त;
            }
            // Handle the shorter unchanged span below like a change.
        } अण्णहा {  // both abIter.hasChange() && bcIter.hasChange()
            जइ (ab_bLength == bc_bLength) {
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
        जइ (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } अण्णहा {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    जइ (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    प्रत्यागम *एतत्;
}

Edits::Iterator::Iterator(ध्रुव uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(मिच्छ), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    जइ (head < LENGTH_IN_1TRAIL) {
        प्रत्यागम head;
    } अण्णहा जइ (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        प्रत्यागम array[index++] & 0x7fff;
    } अण्णहा {
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
    जइ (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

शून्य Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    जइ (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = मिच्छ;
    oldLength_ = newLength_ = 0;
    प्रत्यागम मिच्छ;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    जइ (dir > 0) {
        updateNextIndexes();
    } अण्णहा {
        जइ (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            जइ (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                प्रत्यागम सच्च;
            }
        }
        dir = 1;
    }
    जइ (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        जइ (remaining > 1) {
            --remaining;
            प्रत्यागम सच्च;
        }
        remaining = 0;
    }
    जइ (index >= length) {
        प्रत्यागम noNext();
    }
    int32_t u = array[index++];
    जइ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = मिच्छ;
        oldLength_ = u + 1;
        जाव (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        जइ (onlyChanges) {
            updateNextIndexes();
            जइ (index >= length) {
                प्रत्यागम noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } अण्णहा {
            प्रत्यागम सच्च;
        }
    }
    changed = सच्च;
    जइ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_अङ्कः_MASK) + 1;
        जइ (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } अण्णहा {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            जइ (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            प्रत्यागम सच्च;
        }
    } अण्णहा {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        जइ (!coarse) {
            प्रत्यागम सच्च;
        }
    }
    // Combine adjacent changes.
    जाव (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        जइ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अङ्कः_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } अण्णहा {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    प्रत्यागम सच्च;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    जइ (dir >= 0) {
        जइ (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            जइ (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                प्रत्यागम सच्च;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    जइ (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        जइ (remaining <= (u & SHORT_CHANGE_अङ्कः_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            प्रत्यागम सच्च;
        }
        remaining = 0;
    }
    जइ (index <= 0) {
        प्रत्यागम noNext();
    }
    int32_t u = array[--index];
    जइ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = मिच्छ;
        oldLength_ = u + 1;
        जाव (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        प्रत्यागम सच्च;
    }
    changed = सच्च;
    जइ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_अङ्कः_MASK) + 1;
        जइ (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } अण्णहा {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            जइ (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            प्रत्यागम सच्च;
        }
    } अण्णहा {
        जइ (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } अण्णहा {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            जाव ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        जइ (!coarse) {
            updatePreviousIndexes();
            प्रत्यागम सच्च;
        }
    }
    // Combine adjacent changes.
    जाव (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        जइ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अङ्कः_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } अण्णहा जइ (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    प्रत्यागम सच्च;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode) || i < 0) { प्रत्यागम -1; }
    int32_t spanStart, spanLength;
    जइ (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } अण्णहा {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    जइ (i < spanStart) {
        जइ (i >= (spanStart / 2)) {
            // Search backwards.
            प्रति (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (शून्य)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                जइ (i >= spanStart) {
                    // The index is in the current span.
                    प्रत्यागम 0;
                }
                जइ (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_अङ्कः_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    जइ (i >= (spanStart - len)) {
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
    } अण्णहा जइ (i < (spanStart + spanLength)) {
        // The index is in the current span.
        प्रत्यागम 0;
    }
    जाव (next(मिच्छ, errorCode)) {
        जइ (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } अण्णहा {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        जइ (i < (spanStart + spanLength)) {
            // The index is in the current span.
            प्रत्यागम 0;
        }
        जइ (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            जइ (i < (spanStart + len)) {
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
    int32_t where = findIndex(i, सच्च, errorCode);
    जइ (where < 0) {
        // Error or before the string.
        प्रत्यागम 0;
    }
    जइ (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        प्रत्यागम destIndex;
    }
    जइ (changed) {
        // In a change span, map to its end.
        प्रत्यागम destIndex + newLength_;
    } अण्णहा {
        // In an unchanged span, offset 1:1 within it.
        प्रत्यागम destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, मिच्छ, errorCode);
    जइ (where < 0) {
        // Error or before the string.
        प्रत्यागम 0;
    }
    जइ (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        प्रत्यागम srcIndex;
    }
    जइ (changed) {
        // In a change span, map to its end.
        प्रत्यागम srcIndex + oldLength_;
    } अण्णहा {
        // In an unchanged span, offset within it.
        प्रत्यागम srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) ध्रुव {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_सङ्ख्या(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_सङ्ख्या(sb, srcIndex + oldLength_);
    जइ (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } अण्णहा {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_सङ्ख्या(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_सङ्ख्या(sb, destIndex + newLength_);
    जइ (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_सङ्ख्या(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::append_सङ्ख्या(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } अण्णहा {
        sb.append(u"] (no-change) }", -1);
    }
    प्रत्यागम sb;
}

U_NAMESPACE_END
