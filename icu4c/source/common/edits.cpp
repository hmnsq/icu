// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "util.h"

U_NAMESPACE_BEGIN

നാമാകാശ {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
സ്ഥിരം int32_t MAX_UNCHANGED_LENGTH = 0x1000;
സ്ഥിരം int32_t MAX_UNCHANGED = MAX_UNCHANGED_LENGTH - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
സ്ഥിരം int32_t MAX_SHORT_CHANGE_OLD_LENGTH = 6;
സ്ഥിരം int32_t MAX_SHORT_CHANGE_NEW_LENGTH = 7;
സ്ഥിരം int32_t SHORT_CHANGE_അക്കം_MASK = 0x1ff;
സ്ഥിരം int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
സ്ഥിരം int32_t LENGTH_IN_1TRAIL = 61;
സ്ഥിരം int32_t LENGTH_IN_2TRAIL = 62;

}  // namespace

ശൂന്യം Edits::releaseArray() noexcept {
    എങ്കിൽ (array != അട്ടി_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(സ്ഥിരം Edits& other) U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode_)) {
        length = delta = അക്കം_Changes = 0;
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        എങ്കിൽ (newArray == nullptr) {
            length = delta = അക്കം_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ *ഇത്;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    എങ്കിൽ (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    തിരികെ *ഇത്;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode_)) {
        length = delta = അക്കം_Changes = 0;
        തിരികെ *ഇത്;
    }
    releaseArray();
    എങ്കിൽ (length > അട്ടി_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.അട്ടി_Array;
        src.capacity = അട്ടി_CAPACITY;
        src.reset();
        തിരികെ *ഇത്;
    }
    array = അട്ടി_Array;
    capacity = അട്ടി_CAPACITY;
    എങ്കിൽ (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    തിരികെ *ഇത്;
}

Edits& Edits::operator=(സ്ഥിരം Edits& other) U_LIFETIME_BOUND {
    എങ്കിൽ (ഇത് == &other) { തിരികെ *ഇത്; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    അക്കം_Changes = other.അക്കം_Changes;
    errorCode_ = other.errorCode_;
    തിരികെ copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    അക്കം_Changes = src.അക്കം_Changes;
    errorCode_ = src.errorCode_;
    തിരികെ moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

ശൂന്യം Edits::reset() noexcept {
    length = delta = അക്കം_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

ശൂന്യം Edits::addUnchanged(int32_t unchangedLength) {
    എങ്കിൽ(U_FAILURE(errorCode_) || unchangedLength == 0) { തിരികെ; }
    എങ്കിൽ(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    എങ്കിൽ(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        എങ്കിൽ (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            തിരികെ;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    ആകുമ്പോൾ(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    എങ്കിൽ(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

ശൂന്യം Edits::addReplace(int32_t oldLength, int32_t newLength) {
    എങ്കിൽ(U_FAILURE(errorCode_)) { തിരികെ; }
    എങ്കിൽ(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    എങ്കിൽ (oldLength == 0 && newLength == 0) {
        തിരികെ;
    }
    ++അക്കം_Changes;
    int32_t newDelta = newLength - oldLength;
    എങ്കിൽ (newDelta != 0) {
        എങ്കിൽ ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ;
        }
        delta += newDelta;
    }

    എങ്കിൽ(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        എങ്കിൽ(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_അക്കം_MASK) == u &&
                (last & SHORT_CHANGE_അക്കം_MASK) < SHORT_CHANGE_അക്കം_MASK) {
            setLastUnit(last + 1);
            തിരികെ;
        }
        append(u);
        തിരികെ;
    }

    int32_t head = 0x7000;
    എങ്കിൽ (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } അല്ലെങ്കിൽ എങ്കിൽ ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        എങ്കിൽ(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } അല്ലെങ്കിൽ എങ്കിൽ(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } അല്ലെങ്കിൽ {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        എങ്കിൽ(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } അല്ലെങ്കിൽ എങ്കിൽ(newLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        } അല്ലെങ്കിൽ {
            head |= LENGTH_IN_2TRAIL + (newLength >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (newLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        }
        array[length] = static_cast<uint16_t>(head);
        length = limit;
    }
}

ശൂന്യം Edits::append(int32_t r) {
    എങ്കിൽ(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    എങ്കിൽ (array == അട്ടി_Array) {
        newCapacity = 2000;
    } അല്ലെങ്കിൽ എങ്കിൽ (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ അസത്യം;
    } അല്ലെങ്കിൽ എങ്കിൽ (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } അല്ലെങ്കിൽ {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    എങ്കിൽ ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ അസത്യം;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    എങ്കിൽ (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ അസത്യം;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    തിരികെ സത്യം;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(outErrorCode)) { തിരികെ സത്യം; }
    എങ്കിൽ (U_SUCCESS(errorCode_)) { തിരികെ അസത്യം; }
    outErrorCode = errorCode_;
    തിരികെ സത്യം;
}

Edits& Edits::mergeAndAppend(സ്ഥിരം Edits& ab,
                             സ്ഥിരം Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    എങ്കിൽ (copyErrorTo(errorCode)) { തിരികെ *ഇത്; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool abHasNext = സത്യം, bcHasNext = സത്യം;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t aLength = 0, ab_bLength = 0, bc_bLength = 0, cLength = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_aLength = 0, pending_cLength = 0;
    വേണ്ടി (;;) {
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
        എങ്കിൽ (bc_bLength == 0) {
            എങ്കിൽ (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                എങ്കിൽ (bc_bLength == 0) {
                    // insertion
                    എങ്കിൽ (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } അല്ലെങ്കിൽ {
                        pending_cLength += cLength;
                    }
                    തുടരുക;
                }
            }
            // else see if the other iterator is done, too.
        }
        എങ്കിൽ (ab_bLength == 0) {
            എങ്കിൽ (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                എങ്കിൽ (ab_bLength == 0) {
                    // deletion
                    എങ്കിൽ (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } അല്ലെങ്കിൽ {
                        pending_aLength += aLength;
                    }
                    തുടരുക;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                നിർത്തുക;
            } അല്ലെങ്കിൽ {
                // The ab output string is shorter than the bc input string.
                എങ്കിൽ (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                തിരികെ *ഇത്;
            }
        }
        എങ്കിൽ (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            എങ്കിൽ (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            തിരികെ *ഇത്;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        എങ്കിൽ (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            എങ്കിൽ (pending_aLength != 0 || pending_cLength != 0) {
                addReplace(pending_aLength, pending_cLength);
                pending_aLength = pending_cLength = 0;
            }
            int32_t unchangedLength = aLength <= cLength ? aLength : cLength;
            addUnchanged(unchangedLength);
            ab_bLength = aLength -= unchangedLength;
            bc_bLength = cLength -= unchangedLength;
            // At least one of the unchanged spans is now empty.
            തുടരുക;
        }
        എങ്കിൽ (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            എങ്കിൽ (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                തുടരുക;
            }
            // Handle the shorter unchanged span below like a change.
        } അല്ലെങ്കിൽ എങ്കിൽ (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            എങ്കിൽ (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                തുടരുക;
            }
            // Handle the shorter unchanged span below like a change.
        } അല്ലെങ്കിൽ {  // both abIter.hasChange() && bcIter.hasChange()
            എങ്കിൽ (ab_bLength == bc_bLength) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_aLength + aLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                ab_bLength = bc_bLength = 0;
                തുടരുക;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_aLength += aLength;
        pending_cLength += cLength;
        എങ്കിൽ (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } അല്ലെങ്കിൽ {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    എങ്കിൽ (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    തിരികെ *ഇത്;
}

Edits::Iterator::Iterator(സ്ഥിരം uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(അസത്യം), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    എങ്കിൽ (head < LENGTH_IN_1TRAIL) {
        തിരികെ head;
    } അല്ലെങ്കിൽ എങ്കിൽ (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        തിരികെ array[index++] & 0x7fff;
    } അല്ലെങ്കിൽ {
        U_ASSERT((index + 2) <= length);
        U_ASSERT(array[index] >= 0x8000);
        U_ASSERT(array[index + 1] >= 0x8000);
        int32_t len = ((head & 1) << 30) |
                (static_cast<int32_t>(array[index] & 0x7fff) << 15) |
                (array[index + 1] & 0x7fff);
        index += 2;
        തിരികെ len;
    }
}

ശൂന്യം Edits::Iterator::updateNextIndexes() {
    srcIndex += oldLength_;
    എങ്കിൽ (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

ശൂന്യം Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    എങ്കിൽ (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = അസത്യം;
    oldLength_ = newLength_ = 0;
    തിരികെ അസത്യം;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    എങ്കിൽ (dir > 0) {
        updateNextIndexes();
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            എങ്കിൽ (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                തിരികെ സത്യം;
            }
        }
        dir = 1;
    }
    എങ്കിൽ (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        എങ്കിൽ (remaining > 1) {
            --remaining;
            തിരികെ സത്യം;
        }
        remaining = 0;
    }
    എങ്കിൽ (index >= length) {
        തിരികെ noNext();
    }
    int32_t u = array[index++];
    എങ്കിൽ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = അസത്യം;
        oldLength_ = u + 1;
        ആകുമ്പോൾ (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        എങ്കിൽ (onlyChanges) {
            updateNextIndexes();
            എങ്കിൽ (index >= length) {
                തിരികെ noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } അല്ലെങ്കിൽ {
            തിരികെ സത്യം;
        }
    }
    changed = സത്യം;
    എങ്കിൽ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_അക്കം_MASK) + 1;
        എങ്കിൽ (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } അല്ലെങ്കിൽ {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            എങ്കിൽ (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            തിരികെ സത്യം;
        }
    } അല്ലെങ്കിൽ {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        എങ്കിൽ (!coarse) {
            തിരികെ സത്യം;
        }
    }
    // Combine adjacent changes.
    ആകുമ്പോൾ (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        എങ്കിൽ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_അക്കം_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } അല്ലെങ്കിൽ {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    തിരികെ സത്യം;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    എങ്കിൽ (dir >= 0) {
        എങ്കിൽ (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            എങ്കിൽ (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                തിരികെ സത്യം;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    എങ്കിൽ (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        എങ്കിൽ (remaining <= (u & SHORT_CHANGE_അക്കം_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            തിരികെ സത്യം;
        }
        remaining = 0;
    }
    എങ്കിൽ (index <= 0) {
        തിരികെ noNext();
    }
    int32_t u = array[--index];
    എങ്കിൽ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = അസത്യം;
        oldLength_ = u + 1;
        ആകുമ്പോൾ (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        തിരികെ സത്യം;
    }
    changed = സത്യം;
    എങ്കിൽ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_അക്കം_MASK) + 1;
        എങ്കിൽ (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } അല്ലെങ്കിൽ {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            എങ്കിൽ (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            തിരികെ സത്യം;
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } അല്ലെങ്കിൽ {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            ആകുമ്പോൾ ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        എങ്കിൽ (!coarse) {
            updatePreviousIndexes();
            തിരികെ സത്യം;
        }
    }
    // Combine adjacent changes.
    ആകുമ്പോൾ (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        എങ്കിൽ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_അക്കം_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } അല്ലെങ്കിൽ എങ്കിൽ (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    തിരികെ സത്യം;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode) || i < 0) { തിരികെ -1; }
    int32_t spanStart, spanLength;
    എങ്കിൽ (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } അല്ലെങ്കിൽ {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    എങ്കിൽ (i < spanStart) {
        എങ്കിൽ (i >= (spanStart / 2)) {
            // Search backwards.
            വേണ്ടി (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (ശൂന്യം)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                എങ്കിൽ (i >= spanStart) {
                    // The index is in the current span.
                    തിരികെ 0;
                }
                എങ്കിൽ (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_അക്കം_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    എങ്കിൽ (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / spanLength) + 1;
                        // 1 <= n <= num
                        srcIndex -= n * oldLength_;
                        replIndex -= n * newLength_;
                        destIndex -= n * newLength_;
                        remaining += n;
                        തിരികെ 0;
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
    } അല്ലെങ്കിൽ എങ്കിൽ (i < (spanStart + spanLength)) {
        // The index is in the current span.
        തിരികെ 0;
    }
    ആകുമ്പോൾ (next(അസത്യം, errorCode)) {
        എങ്കിൽ (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } അല്ലെങ്കിൽ {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        എങ്കിൽ (i < (spanStart + spanLength)) {
            // The index is in the current span.
            തിരികെ 0;
        }
        എങ്കിൽ (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            എങ്കിൽ (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / spanLength;  // 1 <= n <= remaining - 1
                srcIndex += n * oldLength_;
                replIndex += n * newLength_;
                destIndex += n * newLength_;
                remaining -= n;
                തിരികെ 0;
            }
            // Make next() skip all of these edits at once.
            oldLength_ *= remaining;
            newLength_ *= remaining;
            remaining = 0;
        }
    }
    തിരികെ 1;
}

int32_t Edits::Iterator::destinationIndexFromSourceIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, സത്യം, errorCode);
    എങ്കിൽ (where < 0) {
        // Error or before the string.
        തിരികെ 0;
    }
    എങ്കിൽ (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        തിരികെ destIndex;
    }
    എങ്കിൽ (changed) {
        // In a change span, map to its end.
        തിരികെ destIndex + newLength_;
    } അല്ലെങ്കിൽ {
        // In an unchanged span, offset 1:1 within it.
        തിരികെ destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, അസത്യം, errorCode);
    എങ്കിൽ (where < 0) {
        // Error or before the string.
        തിരികെ 0;
    }
    എങ്കിൽ (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        തിരികെ srcIndex;
    }
    എങ്കിൽ (changed) {
        // In a change span, map to its end.
        തിരികെ srcIndex + oldLength_;
    } അല്ലെങ്കിൽ {
        // In an unchanged span, offset within it.
        തിരികെ srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) സ്ഥിരം {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_സംഖ്യ(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_സംഖ്യ(sb, srcIndex + oldLength_);
    എങ്കിൽ (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } അല്ലെങ്കിൽ {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_സംഖ്യ(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_സംഖ്യ(sb, destIndex + newLength_);
    എങ്കിൽ (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_സംഖ്യ(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::append_സംഖ്യ(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } അല്ലെങ്കിൽ {
        sb.append(u"] (no-change) }", -1);
    }
    തിരികെ sb;
}

U_NAMESPACE_END
