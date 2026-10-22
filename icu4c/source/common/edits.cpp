// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "util.h"

U_NAMESPACE_BEGIN

নামাকাশ {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
ধ্রুব int32_t MAX_UNCHANGED_LENGTH = 0x1000;
ধ্রুব int32_t MAX_UNCHANGED = MAX_UNCHANGED_LENGTH - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ধ্রুব int32_t MAX_SHORT_CHANGE_OLD_LENGTH = 6;
ধ্রুব int32_t MAX_SHORT_CHANGE_NEW_LENGTH = 7;
ধ্রুব int32_t SHORT_CHANGE_অঙ্ক_MASK = 0x1ff;
ধ্রুব int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ধ্রুব int32_t LENGTH_IN_1TRAIL = 61;
ধ্রুব int32_t LENGTH_IN_2TRAIL = 62;

}  // namespace

শূন্য Edits::releaseArray() noexcept {
    যদি (array != স্তূপ_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ধ্রুব Edits& other) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) {
        length = delta = অঙ্ক_Changes = 0;
        প্রত্যাগম *এতত্;
    }
    যদি (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        যদি (newArray == nullptr) {
            length = delta = অঙ্ক_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম *এতত্;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    যদি (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    প্রত্যাগম *এতত্;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) {
        length = delta = অঙ্ক_Changes = 0;
        প্রত্যাগম *এতত্;
    }
    releaseArray();
    যদি (length > স্তূপ_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.স্তূপ_Array;
        src.capacity = স্তূপ_CAPACITY;
        src.reset();
        প্রত্যাগম *এতত্;
    }
    array = স্তূপ_Array;
    capacity = স্তূপ_CAPACITY;
    যদি (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    প্রত্যাগম *এতত্;
}

Edits& Edits::operator=(ধ্রুব Edits& other) U_LIFETIME_BOUND {
    যদি (এতত্ == &other) { প্রত্যাগম *এতত্; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    অঙ্ক_Changes = other.অঙ্ক_Changes;
    errorCode_ = other.errorCode_;
    প্রত্যাগম copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    অঙ্ক_Changes = src.অঙ্ক_Changes;
    errorCode_ = src.errorCode_;
    প্রত্যাগম moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

শূন্য Edits::reset() noexcept {
    length = delta = অঙ্ক_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

শূন্য Edits::addUnchanged(int32_t unchangedLength) {
    যদি(U_FAILURE(errorCode_) || unchangedLength == 0) { প্রত্যাগম; }
    যদি(unchangedLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    যদি(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        যদি (remaining >= unchangedLength) {
            setLastUnit(last + unchangedLength);
            প্রত্যাগম;
        }
        setLastUnit(MAX_UNCHANGED);
        unchangedLength -= remaining;
    }
    // Split large lengths into multiple units.
    যাবত্(unchangedLength >= MAX_UNCHANGED_LENGTH) {
        append(MAX_UNCHANGED);
        unchangedLength -= MAX_UNCHANGED_LENGTH;
    }
    // Write a small (remaining) length.
    যদি(unchangedLength > 0) {
        append(unchangedLength - 1);
    }
}

শূন্য Edits::addReplace(int32_t oldLength, int32_t newLength) {
    যদি(U_FAILURE(errorCode_)) { প্রত্যাগম; }
    যদি(oldLength < 0 || newLength < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম;
    }
    যদি (oldLength == 0 && newLength == 0) {
        প্রত্যাগম;
    }
    ++অঙ্ক_Changes;
    int32_t newDelta = newLength - oldLength;
    যদি (newDelta != 0) {
        যদি ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম;
        }
        delta += newDelta;
    }

    যদি(0 < oldLength && oldLength <= MAX_SHORT_CHANGE_OLD_LENGTH &&
            newLength <= MAX_SHORT_CHANGE_NEW_LENGTH) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (oldLength << 12) | (newLength << 9);
        int32_t last = lastUnit();
        যদি(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_অঙ্ক_MASK) == u &&
                (last & SHORT_CHANGE_অঙ্ক_MASK) < SHORT_CHANGE_অঙ্ক_MASK) {
            setLastUnit(last + 1);
            প্রত্যাগম;
        }
        append(u);
        প্রত্যাগম;
    }

    int32_t head = 0x7000;
    যদি (oldLength < LENGTH_IN_1TRAIL && newLength < LENGTH_IN_1TRAIL) {
        head |= oldLength << 6;
        head |= newLength;
        append(head);
    } অন্যথা যদি ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        যদি(oldLength < LENGTH_IN_1TRAIL) {
            head |= oldLength << 6;
        } অন্যথা যদি(oldLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        } অন্যথা {
            head |= (LENGTH_IN_2TRAIL + (oldLength >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (oldLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | oldLength);
        }
        যদি(newLength < LENGTH_IN_1TRAIL) {
            head |= newLength;
        } অন্যথা যদি(newLength <= 0x7fff) {
            head |= LENGTH_IN_1TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        } অন্যথা {
            head |= LENGTH_IN_2TRAIL + (newLength >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (newLength >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | newLength);
        }
        array[length] = static_cast<uint16_t>(head);
        length = limit;
    }
}

শূন্য Edits::append(int32_t r) {
    যদি(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    যদি (array == স্তূপ_Array) {
        newCapacity = 2000;
    } অন্যথা যদি (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম অসত্য;
    } অন্যথা যদি (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } অন্যথা {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    যদি ((newCapacity - capacity) < 5) {
        errorCode_ = U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম অসত্য;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    যদি (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম অসত্য;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    প্রত্যাগম সত্য;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) ধ্রুব {
    যদি (U_FAILURE(outErrorCode)) { প্রত্যাগম সত্য; }
    যদি (U_SUCCESS(errorCode_)) { প্রত্যাগম অসত্য; }
    outErrorCode = errorCode_;
    প্রত্যাগম সত্য;
}

Edits& Edits::mergeAndAppend(ধ্রুব Edits& ab,
                             ধ্রুব Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি (copyErrorTo(errorCode)) { প্রত্যাগম *এতত্; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool abHasNext = সত্য, bcHasNext = সত্য;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t aLength = 0, ab_bLength = 0, bc_bLength = 0, cLength = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_aLength = 0, pending_cLength = 0;
    প্রতি (;;) {
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
        যদি (bc_bLength == 0) {
            যদি (bcHasNext && (bcHasNext = bcIter.next(errorCode)) != 0) {
                bc_bLength = bcIter.oldLength();
                cLength = bcIter.newLength();
                যদি (bc_bLength == 0) {
                    // insertion
                    যদি (ab_bLength == 0 || !abIter.hasChange()) {
                        addReplace(pending_aLength, pending_cLength + cLength);
                        pending_aLength = pending_cLength = 0;
                    } অন্যথা {
                        pending_cLength += cLength;
                    }
                    অনুবর্ত;
                }
            }
            // else see if the other iterator is done, too.
        }
        যদি (ab_bLength == 0) {
            যদি (abHasNext && (abHasNext = abIter.next(errorCode)) != 0) {
                aLength = abIter.oldLength();
                ab_bLength = abIter.newLength();
                যদি (ab_bLength == 0) {
                    // deletion
                    যদি (bc_bLength == bcIter.oldLength() || !bcIter.hasChange()) {
                        addReplace(pending_aLength + aLength, pending_cLength);
                        pending_aLength = pending_cLength = 0;
                    } অন্যথা {
                        pending_aLength += aLength;
                    }
                    অনুবর্ত;
                }
            } অন্যথা যদি (bc_bLength == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                ভঙ্গ;
            } অন্যথা {
                // The ab output string is shorter than the bc input string.
                যদি (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                প্রত্যাগম *এতত্;
            }
        }
        যদি (bc_bLength == 0) {
            // The bc input string is shorter than the ab output string.
            যদি (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            প্রত্যাগম *এতত্;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        যদি (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            যদি (pending_aLength != 0 || pending_cLength != 0) {
                addReplace(pending_aLength, pending_cLength);
                pending_aLength = pending_cLength = 0;
            }
            int32_t unchangedLength = aLength <= cLength ? aLength : cLength;
            addUnchanged(unchangedLength);
            ab_bLength = aLength -= unchangedLength;
            bc_bLength = cLength -= unchangedLength;
            // At least one of the unchanged spans is now empty.
            অনুবর্ত;
        }
        যদি (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            যদি (ab_bLength >= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + bc_bLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                aLength = ab_bLength -= bc_bLength;
                bc_bLength = 0;
                অনুবর্ত;
            }
            // Handle the shorter unchanged span below like a change.
        } অন্যথা যদি (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            যদি (ab_bLength <= bc_bLength) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_aLength + aLength, pending_cLength + ab_bLength);
                pending_aLength = pending_cLength = 0;
                cLength = bc_bLength -= ab_bLength;
                ab_bLength = 0;
                অনুবর্ত;
            }
            // Handle the shorter unchanged span below like a change.
        } অন্যথা {  // both abIter.hasChange() && bcIter.hasChange()
            যদি (ab_bLength == bc_bLength) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_aLength + aLength, pending_cLength + cLength);
                pending_aLength = pending_cLength = 0;
                ab_bLength = bc_bLength = 0;
                অনুবর্ত;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_aLength += aLength;
        pending_cLength += cLength;
        যদি (ab_bLength < bc_bLength) {
            bc_bLength -= ab_bLength;
            cLength = ab_bLength = 0;
        } অন্যথা {  // ab_bLength > bc_bLength
            ab_bLength -= bc_bLength;
            aLength = bc_bLength = 0;
        }
    }
    যদি (pending_aLength != 0 || pending_cLength != 0) {
        addReplace(pending_aLength, pending_cLength);
    }
    copyErrorTo(errorCode);
    প্রত্যাগম *এতত্;
}

Edits::Iterator::Iterator(ধ্রুব uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), index(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(অসত্য), oldLength_(0), newLength_(0),
        srcIndex(0), replIndex(0), destIndex(0) {}

int32_t Edits::Iterator::readLength(int32_t head) {
    যদি (head < LENGTH_IN_1TRAIL) {
        প্রত্যাগম head;
    } অন্যথা যদি (head < LENGTH_IN_2TRAIL) {
        U_ASSERT(index < length);
        U_ASSERT(array[index] >= 0x8000);
        প্রত্যাগম array[index++] & 0x7fff;
    } অন্যথা {
        U_ASSERT((index + 2) <= length);
        U_ASSERT(array[index] >= 0x8000);
        U_ASSERT(array[index + 1] >= 0x8000);
        int32_t len = ((head & 1) << 30) |
                (static_cast<int32_t>(array[index] & 0x7fff) << 15) |
                (array[index + 1] & 0x7fff);
        index += 2;
        প্রত্যাগম len;
    }
}

শূন্য Edits::Iterator::updateNextIndexes() {
    srcIndex += oldLength_;
    যদি (changed) {
        replIndex += newLength_;
    }
    destIndex += newLength_;
}

শূন্য Edits::Iterator::updatePreviousIndexes() {
    srcIndex -= oldLength_;
    যদি (changed) {
        replIndex -= newLength_;
    }
    destIndex -= newLength_;
}

UBool Edits::Iterator::noNext() {
    // No change before or beyond the string.
    dir = 0;
    changed = অসত্য;
    oldLength_ = newLength_ = 0;
    প্রত্যাগম অসত্য;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    যদি (dir > 0) {
        updateNextIndexes();
    } অন্যথা {
        যদি (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            যদি (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++index;  // next() rests on the index after the sequence unit.
                dir = 1;
                প্রত্যাগম সত্য;
            }
        }
        dir = 1;
    }
    যদি (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        যদি (remaining > 1) {
            --remaining;
            প্রত্যাগম সত্য;
        }
        remaining = 0;
    }
    যদি (index >= length) {
        প্রত্যাগম noNext();
    }
    int32_t u = array[index++];
    যদি (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = অসত্য;
        oldLength_ = u + 1;
        যাবত্ (index < length && (u = array[index]) <= MAX_UNCHANGED) {
            ++index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        যদি (onlyChanges) {
            updateNextIndexes();
            যদি (index >= length) {
                প্রত্যাগম noNext();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++index;
        } অন্যথা {
            প্রত্যাগম সত্য;
        }
    }
    changed = সত্য;
    যদি (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1;
        যদি (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } অন্যথা {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            যদি (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            প্রত্যাগম সত্য;
        }
    } অন্যথা {
        U_ASSERT(u <= 0x7fff);
        oldLength_ = readLength((u >> 6) & 0x3f);
        newLength_ = readLength(u & 0x3f);
        যদি (!coarse) {
            প্রত্যাগম সত্য;
        }
    }
    // Combine adjacent changes.
    যাবত্ (index < length && (u = array[index]) > MAX_UNCHANGED) {
        ++index;
        যদি (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } অন্যথা {
            U_ASSERT(u <= 0x7fff);
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
        }
    }
    প্রত্যাগম সত্য;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    যদি (dir >= 0) {
        যদি (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            যদি (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --index;  // previous() rests on the sequence unit.
                dir = -1;
                প্রত্যাগম সত্য;
            }
            updateNextIndexes();
        }
        dir = -1;
    }
    যদি (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[index];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        যদি (remaining <= (u & SHORT_CHANGE_অঙ্ক_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            প্রত্যাগম সত্য;
        }
        remaining = 0;
    }
    যদি (index <= 0) {
        প্রত্যাগম noNext();
    }
    int32_t u = array[--index];
    যদি (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = অসত্য;
        oldLength_ = u + 1;
        যাবত্ (index > 0 && (u = array[index - 1]) <= MAX_UNCHANGED) {
            --index;
            oldLength_ += u + 1;
        }
        newLength_ = oldLength_;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        প্রত্যাগম সত্য;
    }
    changed = সত্য;
    যদি (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH;
        int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1;
        যদি (coarse) {
            oldLength_ = num * oldLen;
            newLength_ = num * newLen;
        } অন্যথা {
            // Split a sequence of changes that was compressed into one unit.
            oldLength_ = oldLen;
            newLength_ = newLen;
            যদি (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            প্রত্যাগম সত্য;
        }
    } অন্যথা {
        যদি (u <= 0x7fff) {
            // The change is encoded in u alone.
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
        } অন্যথা {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(index > 0);
            যাবত্ ((u = array[--index]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t headIndex = index++;
            oldLength_ = readLength((u >> 6) & 0x3f);
            newLength_ = readLength(u & 0x3f);
            index = headIndex;
        }
        যদি (!coarse) {
            updatePreviousIndexes();
            প্রত্যাগম সত্য;
        }
    }
    // Combine adjacent changes.
    যাবত্ (index > 0 && (u = array[index - 1]) > MAX_UNCHANGED) {
        --index;
        যদি (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1;
            oldLength_ += (u >> 12) * num;
            newLength_ += ((u >> 9) & MAX_SHORT_CHANGE_NEW_LENGTH) * num;
        } অন্যথা যদি (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t headIndex = index++;
            oldLength_ += readLength((u >> 6) & 0x3f);
            newLength_ += readLength(u & 0x3f);
            index = headIndex;
        }
    }
    updatePreviousIndexes();
    প্রত্যাগম সত্য;
}

int32_t Edits::Iterator::findIndex(int32_t i, UBool findSource, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode) || i < 0) { প্রত্যাগম -1; }
    int32_t spanStart, spanLength;
    যদি (findSource) {  // find source index
        spanStart = srcIndex;
        spanLength = oldLength_;
    } অন্যথা {  // find destination index
        spanStart = destIndex;
        spanLength = newLength_;
    }
    যদি (i < spanStart) {
        যদি (i >= (spanStart / 2)) {
            // Search backwards.
            প্রতি (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (শূন্য)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? srcIndex : destIndex;
                যদি (i >= spanStart) {
                    // The index is in the current span.
                    প্রত্যাগম 0;
                }
                যদি (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    spanLength = findSource ? oldLength_ : newLength_;
                    int32_t u = array[index];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1 - remaining;
                    int32_t len = num * spanLength;
                    যদি (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / spanLength) + 1;
                        // 1 <= n <= num
                        srcIndex -= n * oldLength_;
                        replIndex -= n * newLength_;
                        destIndex -= n * newLength_;
                        remaining += n;
                        প্রত্যাগম 0;
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
    } অন্যথা যদি (i < (spanStart + spanLength)) {
        // The index is in the current span.
        প্রত্যাগম 0;
    }
    যাবত্ (next(অসত্য, errorCode)) {
        যদি (findSource) {
            spanStart = srcIndex;
            spanLength = oldLength_;
        } অন্যথা {
            spanStart = destIndex;
            spanLength = newLength_;
        }
        যদি (i < (spanStart + spanLength)) {
            // The index is in the current span.
            প্রত্যাগম 0;
        }
        যদি (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * spanLength;
            যদি (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / spanLength;  // 1 <= n <= remaining - 1
                srcIndex += n * oldLength_;
                replIndex += n * newLength_;
                destIndex += n * newLength_;
                remaining -= n;
                প্রত্যাগম 0;
            }
            // Make next() skip all of these edits at once.
            oldLength_ *= remaining;
            newLength_ *= remaining;
            remaining = 0;
        }
    }
    প্রত্যাগম 1;
}

int32_t Edits::Iterator::destinationIndexFromSourceIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, সত্য, errorCode);
    যদি (where < 0) {
        // Error or before the string.
        প্রত্যাগম 0;
    }
    যদি (where > 0 || i == srcIndex) {
        // At or after string length, or at start of the found span.
        প্রত্যাগম destIndex;
    }
    যদি (changed) {
        // In a change span, map to its end.
        প্রত্যাগম destIndex + newLength_;
    } অন্যথা {
        // In an unchanged span, offset 1:1 within it.
        প্রত্যাগম destIndex + (i - srcIndex);
    }
}

int32_t Edits::Iterator::sourceIndexFromDestinationIndex(int32_t i, UErrorCode &errorCode) {
    int32_t where = findIndex(i, অসত্য, errorCode);
    যদি (where < 0) {
        // Error or before the string.
        প্রত্যাগম 0;
    }
    যদি (where > 0 || i == destIndex) {
        // At or after string length, or at start of the found span.
        প্রত্যাগম srcIndex;
    }
    যদি (changed) {
        // In a change span, map to its end.
        প্রত্যাগম srcIndex + oldLength_;
    } অন্যথা {
        // In an unchanged span, offset within it.
        প্রত্যাগম srcIndex + (i - destIndex);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) ধ্রুব {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_সংখ্যা(sb, srcIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_সংখ্যা(sb, srcIndex + oldLength_);
    যদি (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } অন্যথা {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_সংখ্যা(sb, destIndex);
    sb.append(u"..", -1);
    ICU_Utility::append_সংখ্যা(sb, destIndex + newLength_);
    যদি (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_সংখ্যা(sb, replIndex);
        sb.append(u"..", -1);
        ICU_Utility::append_সংখ্যা(sb, replIndex + newLength_);
        sb.append(u"] }", -1);
    } অন্যথা {
        sb.append(u"] (no-change) }", -1);
    }
    প্রত্যাগম sb;
}

U_NAMESPACE_END
