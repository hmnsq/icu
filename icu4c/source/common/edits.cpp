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
स्थिरांक int32_t MAX_UNCHANGED_LENGTH = 0x1000;
स्थिरांक int32_t MAX_UNCHANGED = MAX_UNCHANGED_LENGTH - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
स्थिरांक int32_t MAX_SHORT_CHANGE_OLD_LENGTH = 6;
स्थिरांक int32_t MAX_SHORT_CHANGE_NEW_LENGTH = 7;
स्थिरांक int32_t SHORT_CHANGE_अंक_MASK = 0x1ff;
स्थिरांक int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
स्थिरांक int32_t LENGTH_IN_1TRAIL = 61;
स्थिरांक int32_t LENGTH_IN_2TRAIL = 62;

}  // namespace

शून्य Edits::releaseArray() noexcept {
    जर (array != चळत_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(स्थिरांक Edits& other) U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode_)) {
        length = delta = अंक_Changes = 0;
        परत *हे;
    }
    जर (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<माप_प्रकार>(length) * 2));
        जर (newArray == nullptr) {
            length = delta = अंक_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            परत *हे;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    जर (length > 0) {
        uprv_memcpy(array, other.array, (माप_प्रकार)length * 2);
    }
    परत *हे;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode_)) {
        length = delta = अंक_Changes = 0;
        परत *हे;
    }
    releaseArray();
    जर (length > चळत_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.चळत_Array;
        src.capacity = चळत_CAPACITY;
        src.reset();
        परत *हे;
    }
    array = चळत_Array;
    capacity = चळत_CAPACITY;
    जर (length > 0) {
        uprv_memcpy(array, src.array, (माप_प्रकार)length * 2);
    }
    परत *हे;
}

Edits& Edits::चालक=(स्थिरांक Edits& other) U_LIFETIME_BOUND {
    जर (हे == &other) { परत *हे; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    अंक_Changes = other.अंक_Changes;
    errorCode_ = other.errorCode_;
    परत copyArray(other);
}

Edits& Edits::चालक=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    अंक_Changes = src.अंक_Changes;
    errorCode_ = src.errorCode_;
    परत moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

शून्य Edits::reset() noexcept {
    length = delta = अंक_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

शून्य Edits::addUnchanged(int32_t unchangedLength) {
    जर(U_FAILURE(errorCode_) || unchangedLength == 0) { परत; }
    जर(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    जर(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        जर (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            परत;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    पर्यंत(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    जर(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

शून्य Edits::addReplace(int32_t oldLength, int32_t newLength) {
    जर(U_FAILURE(errorCode_)) { परत; }
    जर(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    जर (oldLength == 0 && newLength == 0) {
        परत;
    }
    ++अंक_Changes;
    int32_t newDelta = newLength - oldLength;
    जर (newDelta != 0) {
        जर ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            परत;
        }
        delta += newDelta;
    }

    जर(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        जर(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_अंक_MASK) == u &&
                (last & SHORT_CHANGE_अंक_MASK) < SHORT_CHANGE_अंक_MASK) {
            setLastUnit(last + 1);
            परत;
        }
        append(u);
        परत;
    }

    int32_t head = 0x7000;
    जर (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } अन्यथा जर ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        जर(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } अन्यथा जर(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } अन्यथा {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        जर(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } अन्यथा जर(newLength <= 0x7fff) {
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
    जर(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    जर (array == चळत_Array) {
        newCapacity = 2000;
    } अन्यथा जर (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        परत असत्य;
    } अन्यथा जर (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } अन्यथा {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    जर ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        परत असत्य;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<माप_प्रकार>(newCapacity) * 2));
    जर (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        परत असत्य;
    }
    uprv_memcpy(newArray, array, (माप_प्रकार)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    परत सत्य;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) स्थिरांक {
    जर (U_FAILURE(outErrorCode)) { परत सत्य; }
    जर (U_SUCCESS(errorCode_)) { परत असत्य; }
    outErrorCode = errorCode_;
    परत सत्य;
}

Edits& Edits::mergeAndAppend(स्थिरांक Edits& ab,
                             स्थिरांक Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    जर (copyErrorTo(errorCode)) { परत *हे; }
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
    साठी (;;) {
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
        जर (bc_bLength == 0) {
            जर (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                जर (bc_bLength == 0) {
                    // insertion
                    जर (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } अन्यथा {
                        pending_cLength += cLength;
                    }
                    सुरूठेवा;
                }
            }
            // else see if the other iterator is done, too.
        }
        जर (ab_bLength == 0) {
            जर (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                जर (ab_bLength == 0) {
                    // deletion
                    जर (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } अन्यथा {
                        pending_aLength += aLength;
                    }
                    सुरूठेवा;
                }
            } अन्यथा जर (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                थांबा;
            } अन्यथा {
                // The ab output string is shorter than the bc input string.
                जर (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                परत *हे;
            }
        }
        जर (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            जर (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            परत *हे;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        जर (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            जर (pending_aLength != 0 || pending_cLength != 0) {
                addReplace(pending_aLength, pending_cLength);
                pending_aLength = pending_cLength = 0;
            }
            int32_t unchangedLength = aLength <= cLength ? aLength : cLength;
            addUnchanged(unchangedLength);
            ab_bLength = aLength -= unchangedLength;
            bc_bLength = cLength -= unchangedLength;
            // At least one of the unchanged spans is now empty.
            सुरूठेवा;
        }
        जर (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            जर (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                सुरूठेवा;
            }
            // Handle the shorter unchanged span below like a change.
        } अन्यथा जर (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            जर (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                सुरूठेवा;
            }
            // Handle the shorter unchanged span below like a change.
        } अन्यथा {  // both abIter.hasChange() && bcIter.hasChange()
            जर (ab_bLength == bc_bLength) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_aLength + aLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                ab_bLength = bc_bLength = 0;
                सुरूठेवा;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_aLength += aLength;
        pending_cLength += cLength;
        जर (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } अन्यथा {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    जर (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    परत *हे;
}

Edits::Iterator::Iterator(स्थिरांक uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(असत्य), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    जर (head < LENGTH_IN_1TRAIL) {
        परत head;
    } अन्यथा जर (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        परत array[index++] & 0x7fff;
    } अन्यथा {
        U_ASSERT((index + 2) <= length);
        U_ASSERT(array[index] >= 0x8000);
        U_ASSERT(array[index + 1] >= 0x8000);
        int32_t len = ((head & 1) << 30) |
                (static_cast<int32_t>(array[index] & 0x7fff) << 15) |
                (array[index + 1] & 0x7fff);
        index += 2;
        परत len;
    }
}

शून्य Edits::Iterator::updateNextIndexes() {
    srcIndex += oldLength_;
    जर (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

शून्य Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    जर (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = असत्य;
    oldLength_ = newLength_ = 0;
    परत असत्य;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    जर (U_FAILURE(errorCode)) { परत असत्य; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    जर (dir > 0) {
        updateNextIndexes();
    } अन्यथा {
        जर (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            जर (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                परत सत्य;
            }
        }
        dir = 1;
    }
    जर (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        जर (remaining > 1) {
            --remaining;
            परत सत्य;
        }
        remaining = 0;
    }
    जर (index >= length) {
        परत noNext();
    }
    int32_t u = array[index++];
    जर (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = असत्य;
        oldLength_ = u + 1;
        पर्यंत (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        जर (onlyChanges) {
            updateNextIndexes();
            जर (index >= length) {
                परत noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } अन्यथा {
            परत सत्य;
        }
    }
    changed = सत्य;
    जर (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
        जर (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } अन्यथा {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            जर (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            परत सत्य;
        }
    } अन्यथा {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        जर (!coarse) {
            परत सत्य;
        }
    }
    // Combine adjacent changes.
    पर्यंत (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        जर (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } अन्यथा {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    परत सत्य;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    जर (U_FAILURE(errorCode)) { परत असत्य; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    जर (dir >= 0) {
        जर (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            जर (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                परत सत्य;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    जर (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        जर (remaining <= (u & SHORT_CHANGE_अंक_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            परत सत्य;
        }
        remaining = 0;
    }
    जर (index <= 0) {
        परत noNext();
    }
    int32_t u = array[--index];
    जर (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = असत्य;
        oldLength_ = u + 1;
        पर्यंत (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        परत सत्य;
    }
    changed = सत्य;
    जर (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
        जर (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } अन्यथा {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            जर (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            परत सत्य;
        }
    } अन्यथा {
        जर (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } अन्यथा {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            पर्यंत ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        जर (!coarse) {
            updatePreviousIndexes();
            परत सत्य;
        }
    }
    // Combine adjacent changes.
    पर्यंत (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        जर (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } अन्यथा जर (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    परत सत्य;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode) || i < 0) { परत -1; }
    int32_t spanStart, spanLength;
    जर (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } अन्यथा {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    जर (i < spanStart) {
        जर (i >= (spanStart / 2)) {
            // Search backwards.
            साठी (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (शून्य)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                जर (i >= spanStart) {
                    // The index is in the current span.
                    परत 0;
                }
                जर (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    जर (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / spanLength) + 1;
                        // 1 <= n <= num
                        srcIndex -= n * oldLength_;
                        replIndex -= n * newLength_;
                        destIndex -= n * newLength_;
                        remaining += n;
                        परत 0;
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
    } अन्यथा जर (i < (spanStart + spanLength)) {
        // The index is in the current span.
        परत 0;
    }
    पर्यंत (next(असत्य, errorCode)) {
        जर (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } अन्यथा {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        जर (i < (spanStart + spanLength)) {
            // The index is in the current span.
            परत 0;
        }
        जर (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            जर (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / spanLength;  // 1 <= n <= remaining - 1
                srcIndex += n * oldLength_;
                replIndex += n * newLength_;
                destIndex += n * newLength_;
                remaining -= n;
                परत 0;
            }
            // Make next() skip all of these edits at once.
            oldLength_ *= remaining;
            newLength_ *= remaining;
            remaining = 0;
        }
    }
    परत 1;
}

int32_t Edits::Iterator::destinationIndexFromSourceIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, सत्य, errorCode);
    जर (where < 0) {
        // Error or before the string.
        परत 0;
    }
    जर (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        परत destIndex;
    }
    जर (changed) {
        // In a change span, map to its end.
        परत destIndex + newLength_;
    } अन्यथा {
        // In an unchanged span, offset 1:1 within it.
        परत destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, असत्य, errorCode);
    जर (where < 0) {
        // Error or before the string.
        परत 0;
    }
    जर (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        परत srcIndex;
    }
    जर (changed) {
        // In a change span, map to its end.
        परत srcIndex + oldLength_;
    } अन्यथा {
        // In an unchanged span, offset within it.
        परत srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) स्थिरांक {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_संख्या(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_संख्या(sb, srcIndex + oldLength_);
    जर (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } अन्यथा {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_संख्या(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_संख्या(sb, destIndex + newLength_);
    जर (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_संख्या(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::append_संख्या(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } अन्यथा {
        sb.append(u"] (no-change) }", -1);
    }
    परत sb;
}

U_NAMESPACE_END
