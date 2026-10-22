// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#அந்தர்பாவ "unicode/edits.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "util.h"

U_NAMESPACE_BEGIN

நாமாகாஶ {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
மாறிலி int32_t MAX_UNCHANGED_LENGTH = 0x1000;
மாறிலி int32_t MAX_UNCHANGED = MAX_UNCHANGED_LENGTH - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
மாறிலி int32_t MAX_SHORT_CHANGE_OLD_LENGTH = 6;
மாறிலி int32_t MAX_SHORT_CHANGE_NEW_LENGTH = 7;
மாறிலி int32_t SHORT_CHANGE_இலக்கம்_MASK = 0x1ff;
மாறிலி int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
மாறிலி int32_t LENGTH_IN_1TRAIL = 61;
மாறிலி int32_t LENGTH_IN_2TRAIL = 62;

}  // namespace

வெறுமை Edits::releaseArray() noexcept {
    எனில் (array != அடுக்கு_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(மாறிலி Edits& other) U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode_)) {
        length = delta = இலக்கம்_Changes = 0;
        திரும்பு *இது;
    }
    எனில் (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        எனில் (newArray == nullptr) {
            length = delta = இலக்கம்_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு *இது;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    எனில் (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    திரும்பு *இது;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode_)) {
        length = delta = இலக்கம்_Changes = 0;
        திரும்பு *இது;
    }
    releaseArray();
    எனில் (length > அடுக்கு_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.அடுக்கு_Array;
        src.capacity = அடுக்கு_CAPACITY;
        src.reset();
        திரும்பு *இது;
    }
    array = அடுக்கு_Array;
    capacity = அடுக்கு_CAPACITY;
    எனில் (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    திரும்பு *இது;
}

Edits& Edits::operator=(மாறிலி Edits& other) U_LIFETIME_BOUND {
    எனில் (இது == &other) { திரும்பு *இது; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    இலக்கம்_Changes = other.இலக்கம்_Changes;
    errorCode_ = other.errorCode_;
    திரும்பு copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    இலக்கம்_Changes = src.இலக்கம்_Changes;
    errorCode_ = src.errorCode_;
    திரும்பு moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

வெறுமை Edits::reset() noexcept {
    length = delta = இலக்கம்_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

வெறுமை Edits::addUnchanged(int32_t unchangedLength) {
    எனில்(U_FAILURE(errorCode_) || unchangedLength == 0) { திரும்பு; }
    எனில்(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    எனில்(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        எனில் (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            திரும்பு;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    போது(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    எனில்(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

வெறுமை Edits::addReplace(int32_t oldLength, int32_t newLength) {
    எனில்(U_FAILURE(errorCode_)) { திரும்பு; }
    எனில்(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    எனில் (oldLength == 0 && newLength == 0) {
        திரும்பு;
    }
    ++இலக்கம்_Changes;
    int32_t newDelta = newLength - oldLength;
    எனில் (newDelta != 0) {
        எனில் ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு;
        }
        delta += newDelta;
    }

    எனில்(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        எனில்(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_இலக்கம்_MASK) == u &&
                (last & SHORT_CHANGE_இலக்கம்_MASK) < SHORT_CHANGE_இலக்கம்_MASK) {
            setLastUnit(last + 1);
            திரும்பு;
        }
        append(u);
        திரும்பு;
    }

    int32_t head = 0x7000;
    எனில் (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } இல்லையெனில் எனில் ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        எனில்(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } இல்லையெனில் எனில்(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } இல்லையெனில் {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        எனில்(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } இல்லையெனில் எனில்(newLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        } இல்லையெனில் {
            head |= LENGTH_IN_2TRAIL + (newLength >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (newLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        }
        array[length] = static_cast<uint16_t>(head);
        length = limit;
    }
}

வெறுமை Edits::append(int32_t r) {
    எனில்(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    எனில் (array == அடுக்கு_Array) {
        newCapacity = 2000;
    } இல்லையெனில் எனில் (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு பொய்;
    } இல்லையெனில் எனில் (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } இல்லையெனில் {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    எனில் ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு பொய்;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    எனில் (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு பொய்;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    திரும்பு உண்மை;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) மாறிலி {
    எனில் (U_FAILURE(outErrorCode)) { திரும்பு உண்மை; }
    எனில் (U_SUCCESS(errorCode_)) { திரும்பு பொய்; }
    outErrorCode = errorCode_;
    திரும்பு உண்மை;
}

Edits& Edits::mergeAndAppend(மாறிலி Edits& ab,
                             மாறிலி Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    எனில் (copyErrorTo(errorCode)) { திரும்பு *இது; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool abHasNext = உண்மை, bcHasNext = உண்மை;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t aLength = 0, ab_bLength = 0, bc_bLength = 0, cLength = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_aLength = 0, pending_cLength = 0;
    க்கு (;;) {
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
        எனில் (bc_bLength == 0) {
            எனில் (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                எனில் (bc_bLength == 0) {
                    // insertion
                    எனில் (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } இல்லையெனில் {
                        pending_cLength += cLength;
                    }
                    தொடர்;
                }
            }
            // else see if the other iterator is done, too.
        }
        எனில் (ab_bLength == 0) {
            எனில் (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                எனில் (ab_bLength == 0) {
                    // deletion
                    எனில் (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } இல்லையெனில் {
                        pending_aLength += aLength;
                    }
                    தொடர்;
                }
            } இல்லையெனில் எனில் (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                நிறுத்து;
            } இல்லையெனில் {
                // The ab output string is shorter than the bc input string.
                எனில் (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                திரும்பு *இது;
            }
        }
        எனில் (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            எனில் (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            திரும்பு *இது;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        எனில் (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            எனில் (pending_aLength != 0 || pending_cLength != 0) {
                addReplace(pending_aLength, pending_cLength);
                pending_aLength = pending_cLength = 0;
            }
            int32_t unchangedLength = aLength <= cLength ? aLength : cLength;
            addUnchanged(unchangedLength);
            ab_bLength = aLength -= unchangedLength;
            bc_bLength = cLength -= unchangedLength;
            // At least one of the unchanged spans is now empty.
            தொடர்;
        }
        எனில் (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            எனில் (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                தொடர்;
            }
            // Handle the shorter unchanged span below like a change.
        } இல்லையெனில் எனில் (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            எனில் (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                தொடர்;
            }
            // Handle the shorter unchanged span below like a change.
        } இல்லையெனில் {  // both abIter.hasChange() && bcIter.hasChange()
            எனில் (ab_bLength == bc_bLength) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_aLength + aLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                ab_bLength = bc_bLength = 0;
                தொடர்;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_aLength += aLength;
        pending_cLength += cLength;
        எனில் (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } இல்லையெனில் {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    எனில் (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    திரும்பு *இது;
}

Edits::Iterator::Iterator(மாறிலி uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(பொய்), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    எனில் (head < LENGTH_IN_1TRAIL) {
        திரும்பு head;
    } இல்லையெனில் எனில் (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        திரும்பு array[index++] & 0x7fff;
    } இல்லையெனில் {
        U_ASSERT((index + 2) <= length);
        U_ASSERT(array[index] >= 0x8000);
        U_ASSERT(array[index + 1] >= 0x8000);
        int32_t len = ((head & 1) << 30) |
                (static_cast<int32_t>(array[index] & 0x7fff) << 15) |
                (array[index + 1] & 0x7fff);
        index += 2;
        திரும்பு len;
    }
}

வெறுமை Edits::Iterator::updateNextIndexes() {
    srcIndex += oldLength_;
    எனில் (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

வெறுமை Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    எனில் (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = பொய்;
    oldLength_ = newLength_ = 0;
    திரும்பு பொய்;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    எனில் (dir > 0) {
        updateNextIndexes();
    } இல்லையெனில் {
        எனில் (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            எனில் (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                திரும்பு உண்மை;
            }
        }
        dir = 1;
    }
    எனில் (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        எனில் (remaining > 1) {
            --remaining;
            திரும்பு உண்மை;
        }
        remaining = 0;
    }
    எனில் (index >= length) {
        திரும்பு noNext();
    }
    int32_t u = array[index++];
    எனில் (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = பொய்;
        oldLength_ = u + 1;
        போது (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        எனில் (onlyChanges) {
            updateNextIndexes();
            எனில் (index >= length) {
                திரும்பு noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } இல்லையெனில் {
            திரும்பு உண்மை;
        }
    }
    changed = உண்மை;
    எனில் (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_இலக்கம்_MASK) + 1;
        எனில் (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } இல்லையெனில் {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            எனில் (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            திரும்பு உண்மை;
        }
    } இல்லையெனில் {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        எனில் (!coarse) {
            திரும்பு உண்மை;
        }
    }
    // Combine adjacent changes.
    போது (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        எனில் (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_இலக்கம்_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } இல்லையெனில் {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    திரும்பு உண்மை;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    எனில் (dir >= 0) {
        எனில் (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            எனில் (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                திரும்பு உண்மை;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    எனில் (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        எனில் (remaining <= (u & SHORT_CHANGE_இலக்கம்_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            திரும்பு உண்மை;
        }
        remaining = 0;
    }
    எனில் (index <= 0) {
        திரும்பு noNext();
    }
    int32_t u = array[--index];
    எனில் (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = பொய்;
        oldLength_ = u + 1;
        போது (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        திரும்பு உண்மை;
    }
    changed = உண்மை;
    எனில் (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_இலக்கம்_MASK) + 1;
        எனில் (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } இல்லையெனில் {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            எனில் (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            திரும்பு உண்மை;
        }
    } இல்லையெனில் {
        எனில் (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } இல்லையெனில் {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            போது ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        எனில் (!coarse) {
            updatePreviousIndexes();
            திரும்பு உண்மை;
        }
    }
    // Combine adjacent changes.
    போது (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        எனில் (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_இலக்கம்_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } இல்லையெனில் எனில் (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    திரும்பு உண்மை;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode) || i < 0) { திரும்பு -1; }
    int32_t spanStart, spanLength;
    எனில் (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } இல்லையெனில் {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    எனில் (i < spanStart) {
        எனில் (i >= (spanStart / 2)) {
            // Search backwards.
            க்கு (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (வெறுமை)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                எனில் (i >= spanStart) {
                    // The index is in the current span.
                    திரும்பு 0;
                }
                எனில் (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_இலக்கம்_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    எனில் (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / spanLength) + 1;
                        // 1 <= n <= num
                        srcIndex -= n * oldLength_;
                        replIndex -= n * newLength_;
                        destIndex -= n * newLength_;
                        remaining += n;
                        திரும்பு 0;
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
    } இல்லையெனில் எனில் (i < (spanStart + spanLength)) {
        // The index is in the current span.
        திரும்பு 0;
    }
    போது (next(பொய், errorCode)) {
        எனில் (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } இல்லையெனில் {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        எனில் (i < (spanStart + spanLength)) {
            // The index is in the current span.
            திரும்பு 0;
        }
        எனில் (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            எனில் (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / spanLength;  // 1 <= n <= remaining - 1
                srcIndex += n * oldLength_;
                replIndex += n * newLength_;
                destIndex += n * newLength_;
                remaining -= n;
                திரும்பு 0;
            }
            // Make next() skip all of these edits at once.
            oldLength_ *= remaining;
            newLength_ *= remaining;
            remaining = 0;
        }
    }
    திரும்பு 1;
}

int32_t Edits::Iterator::destinationIndexFromSourceIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, உண்மை, errorCode);
    எனில் (where < 0) {
        // Error or before the string.
        திரும்பு 0;
    }
    எனில் (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        திரும்பு destIndex;
    }
    எனில் (changed) {
        // In a change span, map to its end.
        திரும்பு destIndex + newLength_;
    } இல்லையெனில் {
        // In an unchanged span, offset 1:1 within it.
        திரும்பு destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, பொய், errorCode);
    எனில் (where < 0) {
        // Error or before the string.
        திரும்பு 0;
    }
    எனில் (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        திரும்பு srcIndex;
    }
    எனில் (changed) {
        // In a change span, map to its end.
        திரும்பு srcIndex + oldLength_;
    } இல்லையெனில் {
        // In an unchanged span, offset within it.
        திரும்பு srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) மாறிலி {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_எண்(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_எண்(sb, srcIndex + oldLength_);
    எனில் (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } இல்லையெனில் {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_எண்(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_எண்(sb, destIndex + newLength_);
    எனில் (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_எண்(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::append_எண்(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } இல்லையெனில் {
        sb.append(u"] (no-change) }", -1);
    }
    திரும்பு sb;
}

U_NAMESPACE_END
