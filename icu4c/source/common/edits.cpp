// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#समावेश "unicode/edits.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"
#समावेश "util.h"

U_NAMESPACE_BEGIN

namespace {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
स्थिर int32_t MAX_UNCHANGED_लंबाई = 0x1000;
स्थिर int32_t MAX_UNCHANGED = MAX_UNCHANGED_लंबाई - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
स्थिर int32_t MAX_SHORT_CHANGE_OLD_लंबाई = 6;
स्थिर int32_t MAX_SHORT_CHANGE_NEW_लंबाई = 7;
स्थिर int32_t SHORT_CHANGE_अंक_MASK = 0x1ff;
स्थिर int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
स्थिर int32_t लंबाई_IN_1_TRAIL = 61;
स्थिर int32_t लंबाई_IN_2_TRAIL = 62;

}  // namespace

व्योम Edits::releaseArray() noexcept {
    अगर (array != ढेर_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(स्थिर Edits& other) U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode_)) {
        length = delta = अंक_Changes = 0;
        वापस *यह;
    }
    अगर (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<माप_प्रकार>(length) * 2));
        अगर (newArray == nullptr) {
            length = delta = अंक_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            वापस *यह;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    अगर (length > 0) {
        uprv_memcpy(array, other.array, (माप_प्रकार)length * 2);
    }
    वापस *यह;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode_)) {
        length = delta = अंक_Changes = 0;
        वापस *यह;
    }
    releaseArray();
    अगर (length > ढेर_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ढेर_Array;
        src.capacity = ढेर_CAPACITY;
        src.reset();
        वापस *यह;
    }
    array = ढेर_Array;
    capacity = ढेर_CAPACITY;
    अगर (length > 0) {
        uprv_memcpy(array, src.array, (माप_प्रकार)length * 2);
    }
    वापस *यह;
}

Edits& Edits::चालक=(स्थिर Edits& other) U_LIFETIME_BOUND {
    अगर (यह == &other) { वापस *यह; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    अंक_Changes = other.अंक_Changes;
    errorCode_ = other.errorCode_;
    वापस copyArray(other);
}

Edits& Edits::चालक=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    अंक_Changes = src.अंक_Changes;
    errorCode_ = src.errorCode_;
    वापस moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

व्योम Edits::reset() noexcept {
    length = delta = अंक_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

व्योम Edits::addUnchanged(int32_t unchanged_लंबाई) {
    अगर(U_FAILURE(errorCode_) || unchanged_लंबाई == 0) { वापस; }
    अगर(unchanged_लंबाई < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    अगर(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        अगर (remaining >= unchanged_लंबाई) {
            setLastUnit(last + unchanged_लंबाई);
            वापस;
        }
        setLastUnit(MAX_UNCHANGED);
        unchanged_लंबाई -= remaining;
    }
    // Split large lengths into multiple units.
    जबतक(unchanged_लंबाई >= MAX_UNCHANGED_लंबाई) {
        append(MAX_UNCHANGED);
        unchanged_लंबाई -= MAX_UNCHANGED_लंबाई;
    }
    // Write a small (remaining) length.
    अगर(unchanged_लंबाई > 0) {
        append(unchanged_लंबाई - 1);
    }
}

व्योम Edits::addReplace(int32_t old_लंबाई, int32_t new_लंबाई) {
    अगर(U_FAILURE(errorCode_)) { वापस; }
    अगर(old_लंबाई < 0 || new_लंबाई < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर (old_लंबाई == 0 && new_लंबाई == 0) {
        वापस;
    }
    ++अंक_Changes;
    int32_t newDelta = new_लंबाई - old_लंबाई;
    अगर (newDelta != 0) {
        अगर ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस;
        }
        delta += newDelta;
    }

    अगर(0 < old_लंबाई && old_लंबाई <= MAX_SHORT_CHANGE_OLD_लंबाई &&
            new_लंबाई <= MAX_SHORT_CHANGE_NEW_लंबाई) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_लंबाई << 12) | (new_लंबाई << 9);
        int32_t last = lastUnit();
        अगर(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_अंक_MASK) == u &&
                (last & SHORT_CHANGE_अंक_MASK) < SHORT_CHANGE_अंक_MASK) {
            setLastUnit(last + 1);
            वापस;
        }
        append(u);
        वापस;
    }

    int32_t शीर्ष = 0x7000;
    अगर (old_लंबाई < लंबाई_IN_1_TRAIL && new_लंबाई < लंबाई_IN_1_TRAIL) {
        शीर्ष |= old_लंबाई << 6;
        शीर्ष |= new_लंबाई;
        append(शीर्ष);
    } अन्यथा अगर ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        अगर(old_लंबाई < लंबाई_IN_1_TRAIL) {
            शीर्ष |= old_लंबाई << 6;
        } अन्यथा अगर(old_लंबाई <= 0x7fff) {
            शीर्ष |= लंबाई_IN_1_TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | old_लंबाई);
        } अन्यथा {
            शीर्ष |= (लंबाई_IN_2_TRAIL + (old_लंबाई >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (old_लंबाई >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | old_लंबाई);
        }
        अगर(new_लंबाई < लंबाई_IN_1_TRAIL) {
            शीर्ष |= new_लंबाई;
        } अन्यथा अगर(new_लंबाई <= 0x7fff) {
            शीर्ष |= लंबाई_IN_1_TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | new_लंबाई);
        } अन्यथा {
            शीर्ष |= लंबाई_IN_2_TRAIL + (new_लंबाई >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (new_लंबाई >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | new_लंबाई);
        }
        array[length] = static_cast<uint16_t>(शीर्ष);
        length = limit;
    }
}

व्योम Edits::append(int32_t r) {
    अगर(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t newCapacity;
    अगर (array == ढेर_Array) {
        newCapacity = 2000;
    } अन्यथा अगर (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        errorCode_ = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस false;
    } अन्यथा अगर (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } अन्यथा {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    अगर ((newCapacity - capacity) < 5) {
        errorCode_ = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस false;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<माप_प्रकार>(newCapacity) * 2));
    अगर (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        वापस false;
    }
    uprv_memcpy(newArray, array, (माप_प्रकार)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    वापस true;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) स्थिर {
    अगर (U_FAILURE(outErrorCode)) { वापस true; }
    अगर (U_SUCCESS(errorCode_)) { वापस false; }
    outErrorCode = errorCode_;
    वापस true;
}

Edits& Edits::mergeAndAppend(स्थिर Edits& ab,
                             स्थिर Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    अगर (copyErrorTo(errorCode)) { वापस *यह; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_अग्र = true, bc_Has_अग्र = true;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_लंबाई = 0, ab_b_लंबाई = 0, bc_b_लंबाई = 0, c_लंबाई = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_लंबाई = 0, pending_c_लंबाई = 0;
    क्रम (;;) {
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
        अगर (bc_b_लंबाई == 0) {
            अगर (bc_Has_अग्र && (bc_Has_अग्र = bcIter.next(errorCode)) != 0) {
                bc_b_लंबाई = bcIter.old_लंबाई();
                c_लंबाई = bcIter.new_लंबाई();
                अगर (bc_b_लंबाई == 0) {
                    // insertion
                    अगर (ab_b_लंबाई == 0 || !abIter.hasChange()) {
                        addReplace(pending_a_लंबाई, pending_c_लंबाई + c_लंबाई);
                        pending_a_लंबाई = pending_c_लंबाई = 0;
                    } अन्यथा {
                        pending_c_लंबाई += c_लंबाई;
                    }
                    जारी;
                }
            }
            // else see if the other iterator is done, too.
        }
        अगर (ab_b_लंबाई == 0) {
            अगर (ab_Has_अग्र && (ab_Has_अग्र = abIter.next(errorCode)) != 0) {
                a_लंबाई = abIter.old_लंबाई();
                ab_b_लंबाई = abIter.new_लंबाई();
                अगर (ab_b_लंबाई == 0) {
                    // deletion
                    अगर (bc_b_लंबाई == bcIter.old_लंबाई() || !bcIter.hasChange()) {
                        addReplace(pending_a_लंबाई + a_लंबाई, pending_c_लंबाई);
                        pending_a_लंबाई = pending_c_लंबाई = 0;
                    } अन्यथा {
                        pending_a_लंबाई += a_लंबाई;
                    }
                    जारी;
                }
            } अन्यथा अगर (bc_b_लंबाई == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                अवरोध;
            } अन्यथा {
                // The ab output string is shorter than the bc input string.
                अगर (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                वापस *यह;
            }
        }
        अगर (bc_b_लंबाई == 0) {
            // The bc input string is shorter than the ab output string.
            अगर (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            वापस *यह;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        अगर (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            अगर (pending_a_लंबाई != 0 || pending_c_लंबाई != 0) {
                addReplace(pending_a_लंबाई, pending_c_लंबाई);
                pending_a_लंबाई = pending_c_लंबाई = 0;
            }
            int32_t unchanged_लंबाई = a_लंबाई <= c_लंबाई ? a_लंबाई : c_लंबाई;
            addUnchanged(unchanged_लंबाई);
            ab_b_लंबाई = a_लंबाई -= unchanged_लंबाई;
            bc_b_लंबाई = c_लंबाई -= unchanged_लंबाई;
            // At least one of the unchanged spans is now empty.
            जारी;
        }
        अगर (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            अगर (ab_b_लंबाई >= bc_b_लंबाई) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_a_लंबाई + bc_b_लंबाई, pending_c_लंबाई + c_लंबाई);
                pending_a_लंबाई = pending_c_लंबाई = 0;
                a_लंबाई = ab_b_लंबाई -= bc_b_लंबाई;
                bc_b_लंबाई = 0;
                जारी;
            }
            // Handle the shorter unchanged span below like a change.
        } अन्यथा अगर (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            अगर (ab_b_लंबाई <= bc_b_लंबाई) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_a_लंबाई + a_लंबाई, pending_c_लंबाई + ab_b_लंबाई);
                pending_a_लंबाई = pending_c_लंबाई = 0;
                c_लंबाई = bc_b_लंबाई -= ab_b_लंबाई;
                ab_b_लंबाई = 0;
                जारी;
            }
            // Handle the shorter unchanged span below like a change.
        } अन्यथा {  // both abIter.hasChange() && bcIter.hasChange()
            अगर (ab_b_लंबाई == bc_b_लंबाई) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_a_लंबाई + a_लंबाई, pending_c_लंबाई + c_लंबाई);
                pending_a_लंबाई = pending_c_लंबाई = 0;
                ab_b_लंबाई = bc_b_लंबाई = 0;
                जारी;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_लंबाई += a_लंबाई;
        pending_c_लंबाई += c_लंबाई;
        अगर (ab_b_लंबाई < bc_b_लंबाई) {
            bc_b_लंबाई -= ab_b_लंबाई;
            c_लंबाई = ab_b_लंबाई = 0;
        } अन्यथा {  // ab_bLength > bc_bLength
            ab_b_लंबाई -= bc_b_लंबाई;
            a_लंबाई = bc_b_लंबाई = 0;
        }
    }
    अगर (pending_a_लंबाई != 0 || pending_c_लंबाई != 0) {
        addReplace(pending_a_लंबाई, pending_c_लंबाई);
    }
    copyErrorTo(errorCode);
    वापस *यह;
}

Edits::Iterator::Iterator(स्थिर uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), सूचकांक(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(false), old_लंबाई_2(0), new_लंबाई_2(0),
        src_सूचकांक(0), repl_सूचकांक(0), dest_सूचकांक(0) {}

int32_t Edits::Iterator::read_लंबाई(int32_t शीर्ष) {
    अगर (शीर्ष < लंबाई_IN_1_TRAIL) {
        वापस शीर्ष;
    } अन्यथा अगर (शीर्ष < लंबाई_IN_2_TRAIL) {
        U_ASSERT(सूचकांक < length);
        U_ASSERT(array[सूचकांक] >= 0x8000);
        वापस array[सूचकांक++] & 0x7fff;
    } अन्यथा {
        U_ASSERT((सूचकांक + 2) <= length);
        U_ASSERT(array[सूचकांक] >= 0x8000);
        U_ASSERT(array[सूचकांक + 1] >= 0x8000);
        int32_t len = ((शीर्ष & 1) << 30) |
                (static_cast<int32_t>(array[सूचकांक] & 0x7fff) << 15) |
                (array[सूचकांक + 1] & 0x7fff);
        सूचकांक += 2;
        वापस len;
    }
}

व्योम Edits::Iterator::update_अग्र_Indexes() {
    src_सूचकांक += old_लंबाई_2;
    अगर (changed) {
        repl_सूचकांक += new_लंबाई_2;
    }
    dest_सूचकांक += new_लंबाई_2;
}

व्योम Edits::Iterator::updatePreviousIndexes() {
    src_सूचकांक -= old_लंबाई_2;
    अगर (changed) {
        repl_सूचकांक -= new_लंबाई_2;
    }
    dest_सूचकांक -= new_लंबाई_2;
}

UBool Edits::Iterator::no_अग्र() {
    // No change before or beyond the string.
    dir = 0;
    changed = false;
    old_लंबाई_2 = new_लंबाई_2 = 0;
    वापस false;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    अगर (U_FAILURE(errorCode)) { वापस false; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    अगर (dir > 0) {
        update_अग्र_Indexes();
    } अन्यथा {
        अगर (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            अगर (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++सूचकांक;  // next() rests on the index after the sequence unit.
                dir = 1;
                वापस true;
            }
        }
        dir = 1;
    }
    अगर (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        अगर (remaining > 1) {
            --remaining;
            वापस true;
        }
        remaining = 0;
    }
    अगर (सूचकांक >= length) {
        वापस no_अग्र();
    }
    int32_t u = array[सूचकांक++];
    अगर (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = false;
        old_लंबाई_2 = u + 1;
        जबतक (सूचकांक < length && (u = array[सूचकांक]) <= MAX_UNCHANGED) {
            ++सूचकांक;
            old_लंबाई_2 += u + 1;
        }
        new_लंबाई_2 = old_लंबाई_2;
        अगर (onlyChanges) {
            update_अग्र_Indexes();
            अगर (सूचकांक >= length) {
                वापस no_अग्र();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++सूचकांक;
        } अन्यथा {
            वापस true;
        }
    }
    changed = true;
    अगर (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_लंबाई;
        int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
        अगर (coarse) {
            old_लंबाई_2 = num * oldLen;
            new_लंबाई_2 = num * newLen;
        } अन्यथा {
            // Split a sequence of changes that was compressed into one unit.
            old_लंबाई_2 = oldLen;
            new_लंबाई_2 = newLen;
            अगर (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            वापस true;
        }
    } अन्यथा {
        U_ASSERT(u <= 0x7fff);
        old_लंबाई_2 = read_लंबाई((u >> 6) & 0x3f);
        new_लंबाई_2 = read_लंबाई(u & 0x3f);
        अगर (!coarse) {
            वापस true;
        }
    }
    // Combine adjacent changes.
    जबतक (सूचकांक < length && (u = array[सूचकांक]) > MAX_UNCHANGED) {
        ++सूचकांक;
        अगर (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
            old_लंबाई_2 += (u >> 12) * num;
            new_लंबाई_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_लंबाई) * num;
        } अन्यथा {
            U_ASSERT(u <= 0x7fff);
            old_लंबाई_2 += read_लंबाई((u >> 6) & 0x3f);
            new_लंबाई_2 += read_लंबाई(u & 0x3f);
        }
    }
    वापस true;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    अगर (U_FAILURE(errorCode)) { वापस false; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    अगर (dir >= 0) {
        अगर (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            अगर (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --सूचकांक;  // previous() rests on the sequence unit.
                dir = -1;
                वापस true;
            }
            update_अग्र_Indexes();
        }
        dir = -1;
    }
    अगर (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[सूचकांक];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        अगर (remaining <= (u & SHORT_CHANGE_अंक_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            वापस true;
        }
        remaining = 0;
    }
    अगर (सूचकांक <= 0) {
        वापस no_अग्र();
    }
    int32_t u = array[--सूचकांक];
    अगर (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = false;
        old_लंबाई_2 = u + 1;
        जबतक (सूचकांक > 0 && (u = array[सूचकांक - 1]) <= MAX_UNCHANGED) {
            --सूचकांक;
            old_लंबाई_2 += u + 1;
        }
        new_लंबाई_2 = old_लंबाई_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        वापस true;
    }
    changed = true;
    अगर (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_लंबाई;
        int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
        अगर (coarse) {
            old_लंबाई_2 = num * oldLen;
            new_लंबाई_2 = num * newLen;
        } अन्यथा {
            // Split a sequence of changes that was compressed into one unit.
            old_लंबाई_2 = oldLen;
            new_लंबाई_2 = newLen;
            अगर (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            वापस true;
        }
    } अन्यथा {
        अगर (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_लंबाई_2 = read_लंबाई((u >> 6) & 0x3f);
            new_लंबाई_2 = read_लंबाई(u & 0x3f);
        } अन्यथा {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(सूचकांक > 0);
            जबतक ((u = array[--सूचकांक]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t शीर्ष_सूचकांक = सूचकांक++;
            old_लंबाई_2 = read_लंबाई((u >> 6) & 0x3f);
            new_लंबाई_2 = read_लंबाई(u & 0x3f);
            सूचकांक = शीर्ष_सूचकांक;
        }
        अगर (!coarse) {
            updatePreviousIndexes();
            वापस true;
        }
    }
    // Combine adjacent changes.
    जबतक (सूचकांक > 0 && (u = array[सूचकांक - 1]) > MAX_UNCHANGED) {
        --सूचकांक;
        अगर (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1;
            old_लंबाई_2 += (u >> 12) * num;
            new_लंबाई_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_लंबाई) * num;
        } अन्यथा अगर (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t शीर्ष_सूचकांक = सूचकांक++;
            old_लंबाई_2 += read_लंबाई((u >> 6) & 0x3f);
            new_लंबाई_2 += read_लंबाई(u & 0x3f);
            सूचकांक = शीर्ष_सूचकांक;
        }
    }
    updatePreviousIndexes();
    वापस true;
}

int32_t Edits::Iterator::find_सूचकांक(int32_t i, UBool findSource, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode) || i < 0) { वापस -1; }
    int32_t spanStart, span_लंबाई;
    अगर (findSource) {  // find source index
        spanStart = src_सूचकांक;
        span_लंबाई = old_लंबाई_2;
    } अन्यथा {  // find destination index
        spanStart = dest_सूचकांक;
        span_लंबाई = new_लंबाई_2;
    }
    अगर (i < spanStart) {
        अगर (i >= (spanStart / 2)) {
            // Search backwards.
            क्रम (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (व्योम)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? src_सूचकांक : dest_सूचकांक;
                अगर (i >= spanStart) {
                    // The index is in the current span.
                    वापस 0;
                }
                अगर (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    span_लंबाई = findSource ? old_लंबाई_2 : new_लंबाई_2;
                    int32_t u = array[सूचकांक];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_अंक_MASK) + 1 - remaining;
                    int32_t len = num * span_लंबाई;
                    अगर (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / span_लंबाई) + 1;
                        // 1 <= n <= num
                        src_सूचकांक -= n * old_लंबाई_2;
                        repl_सूचकांक -= n * new_लंबाई_2;
                        dest_सूचकांक -= n * new_लंबाई_2;
                        remaining += n;
                        वापस 0;
                    }
                    // Skip all of these edits at once.
                    src_सूचकांक -= num * old_लंबाई_2;
                    repl_सूचकांक -= num * new_लंबाई_2;
                    dest_सूचकांक -= num * new_लंबाई_2;
                    remaining = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        सूचकांक = remaining = old_लंबाई_2 = new_लंबाई_2 = src_सूचकांक = repl_सूचकांक = dest_सूचकांक = 0;
    } अन्यथा अगर (i < (spanStart + span_लंबाई)) {
        // The index is in the current span.
        वापस 0;
    }
    जबतक (next(false, errorCode)) {
        अगर (findSource) {
            spanStart = src_सूचकांक;
            span_लंबाई = old_लंबाई_2;
        } अन्यथा {
            spanStart = dest_सूचकांक;
            span_लंबाई = new_लंबाई_2;
        }
        अगर (i < (spanStart + span_लंबाई)) {
            // The index is in the current span.
            वापस 0;
        }
        अगर (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * span_लंबाई;
            अगर (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / span_लंबाई;  // 1 <= n <= remaining - 1
                src_सूचकांक += n * old_लंबाई_2;
                repl_सूचकांक += n * new_लंबाई_2;
                dest_सूचकांक += n * new_लंबाई_2;
                remaining -= n;
                वापस 0;
            }
            // Make next() skip all of these edits at once.
            old_लंबाई_2 *= remaining;
            new_लंबाई_2 *= remaining;
            remaining = 0;
        }
    }
    वापस 1;
}

int32_t Edits::Iterator::destination_सूचकांक_From_Source_सूचकांक(int32_t i, UErrorCode &errorCode) {
    int32_t where = find_सूचकांक(i, true, errorCode);
    अगर (where < 0) {
        // Error or before the string.
        वापस 0;
    }
    अगर (where > 0 || i == src_सूचकांक) {
        // At or after string length, or at start of the found span.
        वापस dest_सूचकांक;
    }
    अगर (changed) {
        // In a change span, map to its end.
        वापस dest_सूचकांक + new_लंबाई_2;
    } अन्यथा {
        // In an unchanged span, offset 1:1 within it.
        वापस dest_सूचकांक + (i - src_सूचकांक);
    }
}

int32_t Edits::Iterator::source_सूचकांक_From_Destination_सूचकांक(int32_t i, UErrorCode &errorCode) {
    int32_t where = find_सूचकांक(i, false, errorCode);
    अगर (where < 0) {
        // Error or before the string.
        वापस 0;
    }
    अगर (where > 0 || i == dest_सूचकांक) {
        // At or after string length, or at start of the found span.
        वापस src_सूचकांक;
    }
    अगर (changed) {
        // In a change span, map to its end.
        वापस src_सूचकांक + old_लंबाई_2;
    } अन्यथा {
        // In an unchanged span, offset within it.
        वापस src_सूचकांक + (i - dest_सूचकांक);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) स्थिर {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_संख्या(sb, src_सूचकांक);
    sb.append(u"..", -1);
    ICU_Utility::append_संख्या(sb, src_सूचकांक + old_लंबाई_2);
    अगर (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } अन्यथा {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_संख्या(sb, dest_सूचकांक);
    sb.append(u"..", -1);
    ICU_Utility::append_संख्या(sb, dest_सूचकांक + new_लंबाई_2);
    अगर (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_संख्या(sb, repl_सूचकांक);
        sb.append(u"..", -1);
        ICU_Utility::append_संख्या(sb, repl_सूचकांक + new_लंबाई_2);
        sb.append(u"] }", -1);
    } अन्यथा {
        sb.append(u"] (no-change) }", -1);
    }
    वापस sb;
}

U_NAMESPACE_END
