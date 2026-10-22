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
ধ্রুবক int32_t MAX_UNCHANGED_দৈর্ঘ্য = 0x1000;
ধ্রুবক int32_t MAX_UNCHANGED = MAX_UNCHANGED_দৈর্ঘ্য - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ধ্রুবক int32_t MAX_SHORT_CHANGE_OLD_দৈর্ঘ্য = 6;
ধ্রুবক int32_t MAX_SHORT_CHANGE_NEW_দৈর্ঘ্য = 7;
ধ্রুবক int32_t SHORT_CHANGE_অঙ্ক_MASK = 0x1ff;
ধ্রুবক int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ধ্রুবক int32_t দৈর্ঘ্য_IN_1_TRAIL = 61;
ধ্রুবক int32_t দৈর্ঘ্য_IN_2_TRAIL = 62;

}  // namespace

শূন্য Edits::releaseArray() noexcept {
    যদি (array != স্তূপ_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ধ্রুবক Edits& other) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) {
        length = delta = অঙ্ক_Changes = 0;
        ফেরত *এই;
    }
    যদি (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        যদি (newArray == nullptr) {
            length = delta = অঙ্ক_Changes = 0;
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ফেরত *এই;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    যদি (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    ফেরত *এই;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) {
        length = delta = অঙ্ক_Changes = 0;
        ফেরত *এই;
    }
    releaseArray();
    যদি (length > স্তূপ_CAPACITY) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.স্তূপ_Array;
        src.capacity = স্তূপ_CAPACITY;
        src.reset();
        ফেরত *এই;
    }
    array = স্তূপ_Array;
    capacity = স্তূপ_CAPACITY;
    যদি (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    ফেরত *এই;
}

Edits& Edits::operator=(ধ্রুবক Edits& other) U_LIFETIME_BOUND {
    যদি (এই == &other) { ফেরত *এই; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    অঙ্ক_Changes = other.অঙ্ক_Changes;
    errorCode_ = other.errorCode_;
    ফেরত copyArray(other);
}

Edits& Edits::operator=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    অঙ্ক_Changes = src.অঙ্ক_Changes;
    errorCode_ = src.errorCode_;
    ফেরত moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

শূন্য Edits::reset() noexcept {
    length = delta = অঙ্ক_Changes = 0;
    errorCode_ = U_ZERO_ERROR;
}

শূন্য Edits::addUnchanged(int32_t unchanged_দৈর্ঘ্য) {
    যদি(U_FAILURE(errorCode_) || unchanged_দৈর্ঘ্য == 0) { ফেরত; }
    যদি(unchanged_দৈর্ঘ্য < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t last = lastUnit();
    যদি(last < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - last;
        যদি (remaining >= unchanged_দৈর্ঘ্য) {
            setLastUnit(last + unchanged_দৈর্ঘ্য);
            ফেরত;
        }
        setLastUnit(MAX_UNCHANGED);
        unchanged_দৈর্ঘ্য -= remaining;
    }
    // Split large lengths into multiple units.
    যখন(unchanged_দৈর্ঘ্য >= MAX_UNCHANGED_দৈর্ঘ্য) {
        append(MAX_UNCHANGED);
        unchanged_দৈর্ঘ্য -= MAX_UNCHANGED_দৈর্ঘ্য;
    }
    // Write a small (remaining) length.
    যদি(unchanged_দৈর্ঘ্য > 0) {
        append(unchanged_দৈর্ঘ্য - 1);
    }
}

শূন্য Edits::addReplace(int32_t old_দৈর্ঘ্য, int32_t new_দৈর্ঘ্য) {
    যদি(U_FAILURE(errorCode_)) { ফেরত; }
    যদি(old_দৈর্ঘ্য < 0 || new_দৈর্ঘ্য < 0) {
        errorCode_ = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    যদি (old_দৈর্ঘ্য == 0 && new_দৈর্ঘ্য == 0) {
        ফেরত;
    }
    ++অঙ্ক_Changes;
    int32_t newDelta = new_দৈর্ঘ্য - old_দৈর্ঘ্য;
    যদি (newDelta != 0) {
        যদি ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            errorCode_ = U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত;
        }
        delta += newDelta;
    }

    যদি(0 < old_দৈর্ঘ্য && old_দৈর্ঘ্য <= MAX_SHORT_CHANGE_OLD_দৈর্ঘ্য &&
            new_দৈর্ঘ্য <= MAX_SHORT_CHANGE_NEW_দৈর্ঘ্য) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_দৈর্ঘ্য << 12) | (new_দৈর্ঘ্য << 9);
        int32_t last = lastUnit();
        যদি(MAX_UNCHANGED < last && last < MAX_SHORT_CHANGE &&
                (last & ~SHORT_CHANGE_অঙ্ক_MASK) == u &&
                (last & SHORT_CHANGE_অঙ্ক_MASK) < SHORT_CHANGE_অঙ্ক_MASK) {
            setLastUnit(last + 1);
            ফেরত;
        }
        append(u);
        ফেরত;
    }

    int32_t শীর্ষ = 0x7000;
    যদি (old_দৈর্ঘ্য < দৈর্ঘ্য_IN_1_TRAIL && new_দৈর্ঘ্য < দৈর্ঘ্য_IN_1_TRAIL) {
        শীর্ষ |= old_দৈর্ঘ্য << 6;
        শীর্ষ |= new_দৈর্ঘ্য;
        append(শীর্ষ);
    } অন্যথা যদি ((capacity - length) >= 5 || growArray()) {
        int32_t limit = length + 1;
        যদি(old_দৈর্ঘ্য < দৈর্ঘ্য_IN_1_TRAIL) {
            শীর্ষ |= old_দৈর্ঘ্য << 6;
        } অন্যথা যদি(old_দৈর্ঘ্য <= 0x7fff) {
            শীর্ষ |= দৈর্ঘ্য_IN_1_TRAIL << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | old_দৈর্ঘ্য);
        } অন্যথা {
            শীর্ষ |= (দৈর্ঘ্য_IN_2_TRAIL + (old_দৈর্ঘ্য >> 30)) << 6;
            array[limit++] = static_cast<uint16_t>(0x8000 | (old_দৈর্ঘ্য >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | old_দৈর্ঘ্য);
        }
        যদি(new_দৈর্ঘ্য < দৈর্ঘ্য_IN_1_TRAIL) {
            শীর্ষ |= new_দৈর্ঘ্য;
        } অন্যথা যদি(new_দৈর্ঘ্য <= 0x7fff) {
            শীর্ষ |= দৈর্ঘ্য_IN_1_TRAIL;
            array[limit++] = static_cast<uint16_t>(0x8000 | new_দৈর্ঘ্য);
        } অন্যথা {
            শীর্ষ |= দৈর্ঘ্য_IN_2_TRAIL + (new_দৈর্ঘ্য >> 30);
            array[limit++] = static_cast<uint16_t>(0x8000 | (new_দৈর্ঘ্য >> 15));
            array[limit++] = static_cast<uint16_t>(0x8000 | new_দৈর্ঘ্য);
        }
        array[length] = static_cast<uint16_t>(শীর্ষ);
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
        errorCode_ = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত মিথ্যা;
    } অন্যথা যদি (capacity >= (INT32_MAX / 2)) {
        newCapacity = INT32_MAX;
    } অন্যথা {
        newCapacity = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    যদি ((newCapacity - capacity) < 5) {
        errorCode_ = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত মিথ্যা;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(newCapacity) * 2));
    যদি (newArray == nullptr) {
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ফেরত মিথ্যা;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = newCapacity;
    ফেরত সত্য;
}

UBool Edits::copyErrorTo(UErrorCode &outErrorCode) ধ্রুবক {
    যদি (U_FAILURE(outErrorCode)) { ফেরত সত্য; }
    যদি (U_SUCCESS(errorCode_)) { ফেরত মিথ্যা; }
    outErrorCode = errorCode_;
    ফেরত সত্য;
}

Edits& Edits::mergeAndAppend(ধ্রুবক Edits& ab,
                             ধ্রুবক Edits& bc,
                             UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি (copyErrorTo(errorCode)) { ফেরত *এই; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_পরবর্তী = সত্য, bc_Has_পরবর্তী = সত্য;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_দৈর্ঘ্য = 0, ab_b_দৈর্ঘ্য = 0, bc_b_দৈর্ঘ্য = 0, c_দৈর্ঘ্য = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_দৈর্ঘ্য = 0, pending_c_দৈর্ঘ্য = 0;
    জন্য (;;) {
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
        যদি (bc_b_দৈর্ঘ্য == 0) {
            যদি (bc_Has_পরবর্তী && (bc_Has_পরবর্তী = bcIter.next(errorCode)) != 0) {
                bc_b_দৈর্ঘ্য = bcIter.old_দৈর্ঘ্য();
                c_দৈর্ঘ্য = bcIter.new_দৈর্ঘ্য();
                যদি (bc_b_দৈর্ঘ্য == 0) {
                    // insertion
                    যদি (ab_b_দৈর্ঘ্য == 0 || !abIter.hasChange()) {
                        addReplace(pending_a_দৈর্ঘ্য, pending_c_দৈর্ঘ্য + c_দৈর্ঘ্য);
                        pending_a_দৈর্ঘ্য = pending_c_দৈর্ঘ্য = 0;
                    } অন্যথা {
                        pending_c_দৈর্ঘ্য += c_দৈর্ঘ্য;
                    }
                    চালু;
                }
            }
            // else see if the other iterator is done, too.
        }
        যদি (ab_b_দৈর্ঘ্য == 0) {
            যদি (ab_Has_পরবর্তী && (ab_Has_পরবর্তী = abIter.next(errorCode)) != 0) {
                a_দৈর্ঘ্য = abIter.old_দৈর্ঘ্য();
                ab_b_দৈর্ঘ্য = abIter.new_দৈর্ঘ্য();
                যদি (ab_b_দৈর্ঘ্য == 0) {
                    // deletion
                    যদি (bc_b_দৈর্ঘ্য == bcIter.old_দৈর্ঘ্য() || !bcIter.hasChange()) {
                        addReplace(pending_a_দৈর্ঘ্য + a_দৈর্ঘ্য, pending_c_দৈর্ঘ্য);
                        pending_a_দৈর্ঘ্য = pending_c_দৈর্ঘ্য = 0;
                    } অন্যথা {
                        pending_a_দৈর্ঘ্য += a_দৈর্ঘ্য;
                    }
                    চালু;
                }
            } অন্যথা যদি (bc_b_দৈর্ঘ্য == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                বিরতি;
            } অন্যথা {
                // The ab output string is shorter than the bc input string.
                যদি (!copyErrorTo(errorCode)) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                }
                ফেরত *এই;
            }
        }
        যদি (bc_b_দৈর্ঘ্য == 0) {
            // The bc input string is shorter than the ab output string.
            যদি (!copyErrorTo(errorCode)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            }
            ফেরত *এই;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        যদি (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            যদি (pending_a_দৈর্ঘ্য != 0 || pending_c_দৈর্ঘ্য != 0) {
                addReplace(pending_a_দৈর্ঘ্য, pending_c_দৈর্ঘ্য);
                pending_a_দৈর্ঘ্য = pending_c_দৈর্ঘ্য = 0;
            }
            int32_t unchanged_দৈর্ঘ্য = a_দৈর্ঘ্য <= c_দৈর্ঘ্য ? a_দৈর্ঘ্য : c_দৈর্ঘ্য;
            addUnchanged(unchanged_দৈর্ঘ্য);
            ab_b_দৈর্ঘ্য = a_দৈর্ঘ্য -= unchanged_দৈর্ঘ্য;
            bc_b_দৈর্ঘ্য = c_দৈর্ঘ্য -= unchanged_দৈর্ঘ্য;
            // At least one of the unchanged spans is now empty.
            চালু;
        }
        যদি (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            যদি (ab_b_দৈর্ঘ্য >= bc_b_দৈর্ঘ্য) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_a_দৈর্ঘ্য + bc_b_দৈর্ঘ্য, pending_c_দৈর্ঘ্য + c_দৈর্ঘ্য);
                pending_a_দৈর্ঘ্য = pending_c_দৈর্ঘ্য = 0;
                a_দৈর্ঘ্য = ab_b_দৈর্ঘ্য -= bc_b_দৈর্ঘ্য;
                bc_b_দৈর্ঘ্য = 0;
                চালু;
            }
            // Handle the shorter unchanged span below like a change.
        } অন্যথা যদি (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            যদি (ab_b_দৈর্ঘ্য <= bc_b_দৈর্ঘ্য) {
                // Split the longer unchanged span into change + remainder.
                addReplace(pending_a_দৈর্ঘ্য + a_দৈর্ঘ্য, pending_c_দৈর্ঘ্য + ab_b_দৈর্ঘ্য);
                pending_a_দৈর্ঘ্য = pending_c_দৈর্ঘ্য = 0;
                c_দৈর্ঘ্য = bc_b_দৈর্ঘ্য -= ab_b_দৈর্ঘ্য;
                ab_b_দৈর্ঘ্য = 0;
                চালু;
            }
            // Handle the shorter unchanged span below like a change.
        } অন্যথা {  // both abIter.hasChange() && bcIter.hasChange()
            যদি (ab_b_দৈর্ঘ্য == bc_b_দৈর্ঘ্য) {
                // Changes on both sides up to the same position. Emit & reset.
                addReplace(pending_a_দৈর্ঘ্য + a_দৈর্ঘ্য, pending_c_দৈর্ঘ্য + c_দৈর্ঘ্য);
                pending_a_দৈর্ঘ্য = pending_c_দৈর্ঘ্য = 0;
                ab_b_দৈর্ঘ্য = bc_b_দৈর্ঘ্য = 0;
                চালু;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_দৈর্ঘ্য += a_দৈর্ঘ্য;
        pending_c_দৈর্ঘ্য += c_দৈর্ঘ্য;
        যদি (ab_b_দৈর্ঘ্য < bc_b_দৈর্ঘ্য) {
            bc_b_দৈর্ঘ্য -= ab_b_দৈর্ঘ্য;
            c_দৈর্ঘ্য = ab_b_দৈর্ঘ্য = 0;
        } অন্যথা {  // ab_bLength > bc_bLength
            ab_b_দৈর্ঘ্য -= bc_b_দৈর্ঘ্য;
            a_দৈর্ঘ্য = bc_b_দৈর্ঘ্য = 0;
        }
    }
    যদি (pending_a_দৈর্ঘ্য != 0 || pending_c_দৈর্ঘ্য != 0) {
        addReplace(pending_a_দৈর্ঘ্য, pending_c_দৈর্ঘ্য);
    }
    copyErrorTo(errorCode);
    ফেরত *এই;
}

Edits::Iterator::Iterator(ধ্রুবক uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), সূচক(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), changed(মিথ্যা), old_দৈর্ঘ্য_2(0), new_দৈর্ঘ্য_2(0),
        src_সূচক(0), repl_সূচক(0), dest_সূচক(0) {}

int32_t Edits::Iterator::read_দৈর্ঘ্য(int32_t শীর্ষ) {
    যদি (শীর্ষ < দৈর্ঘ্য_IN_1_TRAIL) {
        ফেরত শীর্ষ;
    } অন্যথা যদি (শীর্ষ < দৈর্ঘ্য_IN_2_TRAIL) {
        U_ASSERT(সূচক < length);
        U_ASSERT(array[সূচক] >= 0x8000);
        ফেরত array[সূচক++] & 0x7fff;
    } অন্যথা {
        U_ASSERT((সূচক + 2) <= length);
        U_ASSERT(array[সূচক] >= 0x8000);
        U_ASSERT(array[সূচক + 1] >= 0x8000);
        int32_t len = ((শীর্ষ & 1) << 30) |
                (static_cast<int32_t>(array[সূচক] & 0x7fff) << 15) |
                (array[সূচক + 1] & 0x7fff);
        সূচক += 2;
        ফেরত len;
    }
}

শূন্য Edits::Iterator::update_পরবর্তী_Indexes() {
    src_সূচক += old_দৈর্ঘ্য_2;
    যদি (changed) {
        repl_সূচক += new_দৈর্ঘ্য_2;
    }
    dest_সূচক += new_দৈর্ঘ্য_2;
}

শূন্য Edits::Iterator::updatePreviousIndexes() {
    src_সূচক -= old_দৈর্ঘ্য_2;
    যদি (changed) {
        repl_সূচক -= new_দৈর্ঘ্য_2;
    }
    dest_সূচক -= new_দৈর্ঘ্য_2;
}

UBool Edits::Iterator::no_পরবর্তী() {
    // No change before or beyond the string.
    dir = 0;
    changed = মিথ্যা;
    old_দৈর্ঘ্য_2 = new_দৈর্ঘ্য_2 = 0;
    ফেরত মিথ্যা;
}

UBool Edits::Iterator::next(UBool onlyChanges, UErrorCode &errorCode) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    যদি (dir > 0) {
        update_পরবর্তী_Indexes();
    } অন্যথা {
        যদি (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            যদি (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++সূচক;  // next() rests on the index after the sequence unit.
                dir = 1;
                ফেরত সত্য;
            }
        }
        dir = 1;
    }
    যদি (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        যদি (remaining > 1) {
            --remaining;
            ফেরত সত্য;
        }
        remaining = 0;
    }
    যদি (সূচক >= length) {
        ফেরত no_পরবর্তী();
    }
    int32_t u = array[সূচক++];
    যদি (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = মিথ্যা;
        old_দৈর্ঘ্য_2 = u + 1;
        যখন (সূচক < length && (u = array[সূচক]) <= MAX_UNCHANGED) {
            ++সূচক;
            old_দৈর্ঘ্য_2 += u + 1;
        }
        new_দৈর্ঘ্য_2 = old_দৈর্ঘ্য_2;
        যদি (onlyChanges) {
            update_পরবর্তী_Indexes();
            যদি (সূচক >= length) {
                ফেরত no_পরবর্তী();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++সূচক;
        } অন্যথা {
            ফেরত সত্য;
        }
    }
    changed = সত্য;
    যদি (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_দৈর্ঘ্য;
        int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1;
        যদি (coarse) {
            old_দৈর্ঘ্য_2 = num * oldLen;
            new_দৈর্ঘ্য_2 = num * newLen;
        } অন্যথা {
            // Split a sequence of changes that was compressed into one unit.
            old_দৈর্ঘ্য_2 = oldLen;
            new_দৈর্ঘ্য_2 = newLen;
            যদি (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            ফেরত সত্য;
        }
    } অন্যথা {
        U_ASSERT(u <= 0x7fff);
        old_দৈর্ঘ্য_2 = read_দৈর্ঘ্য((u >> 6) & 0x3f);
        new_দৈর্ঘ্য_2 = read_দৈর্ঘ্য(u & 0x3f);
        যদি (!coarse) {
            ফেরত সত্য;
        }
    }
    // Combine adjacent changes.
    যখন (সূচক < length && (u = array[সূচক]) > MAX_UNCHANGED) {
        ++সূচক;
        যদি (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1;
            old_দৈর্ঘ্য_2 += (u >> 12) * num;
            new_দৈর্ঘ্য_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_দৈর্ঘ্য) * num;
        } অন্যথা {
            U_ASSERT(u <= 0x7fff);
            old_দৈর্ঘ্য_2 += read_দৈর্ঘ্য((u >> 6) & 0x3f);
            new_দৈর্ঘ্য_2 += read_দৈর্ঘ্য(u & 0x3f);
        }
    }
    ফেরত সত্য;
}

UBool Edits::Iterator::previous(UErrorCode &errorCode) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
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
                --সূচক;  // previous() rests on the sequence unit.
                dir = -1;
                ফেরত সত্য;
            }
            update_পরবর্তী_Indexes();
        }
        dir = -1;
    }
    যদি (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[সূচক];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        যদি (remaining <= (u & SHORT_CHANGE_অঙ্ক_MASK)) {
            ++remaining;
            updatePreviousIndexes();
            ফেরত সত্য;
        }
        remaining = 0;
    }
    যদি (সূচক <= 0) {
        ফেরত no_পরবর্তী();
    }
    int32_t u = array[--সূচক];
    যদি (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        changed = মিথ্যা;
        old_দৈর্ঘ্য_2 = u + 1;
        যখন (সূচক > 0 && (u = array[সূচক - 1]) <= MAX_UNCHANGED) {
            --সূচক;
            old_দৈর্ঘ্য_2 += u + 1;
        }
        new_দৈর্ঘ্য_2 = old_দৈর্ঘ্য_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        ফেরত সত্য;
    }
    changed = সত্য;
    যদি (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_দৈর্ঘ্য;
        int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1;
        যদি (coarse) {
            old_দৈর্ঘ্য_2 = num * oldLen;
            new_দৈর্ঘ্য_2 = num * newLen;
        } অন্যথা {
            // Split a sequence of changes that was compressed into one unit.
            old_দৈর্ঘ্য_2 = oldLen;
            new_দৈর্ঘ্য_2 = newLen;
            যদি (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            ফেরত সত্য;
        }
    } অন্যথা {
        যদি (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_দৈর্ঘ্য_2 = read_দৈর্ঘ্য((u >> 6) & 0x3f);
            new_দৈর্ঘ্য_2 = read_দৈর্ঘ্য(u & 0x3f);
        } অন্যথা {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(সূচক > 0);
            যখন ((u = array[--সূচক]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t শীর্ষ_সূচক = সূচক++;
            old_দৈর্ঘ্য_2 = read_দৈর্ঘ্য((u >> 6) & 0x3f);
            new_দৈর্ঘ্য_2 = read_দৈর্ঘ্য(u & 0x3f);
            সূচক = শীর্ষ_সূচক;
        }
        যদি (!coarse) {
            updatePreviousIndexes();
            ফেরত সত্য;
        }
    }
    // Combine adjacent changes.
    যখন (সূচক > 0 && (u = array[সূচক - 1]) > MAX_UNCHANGED) {
        --সূচক;
        যদি (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1;
            old_দৈর্ঘ্য_2 += (u >> 12) * num;
            new_দৈর্ঘ্য_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_দৈর্ঘ্য) * num;
        } অন্যথা যদি (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t শীর্ষ_সূচক = সূচক++;
            old_দৈর্ঘ্য_2 += read_দৈর্ঘ্য((u >> 6) & 0x3f);
            new_দৈর্ঘ্য_2 += read_দৈর্ঘ্য(u & 0x3f);
            সূচক = শীর্ষ_সূচক;
        }
    }
    updatePreviousIndexes();
    ফেরত সত্য;
}

int32_t Edits::Iterator::find_সূচক(int32_t i, UBool findSource, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode) || i < 0) { ফেরত -1; }
    int32_t spanStart, span_দৈর্ঘ্য;
    যদি (findSource) {  // find source index
        spanStart = src_সূচক;
        span_দৈর্ঘ্য = old_দৈর্ঘ্য_2;
    } অন্যথা {  // find destination index
        spanStart = dest_সূচক;
        span_দৈর্ঘ্য = new_দৈর্ঘ্য_2;
    }
    যদি (i < spanStart) {
        যদি (i >= (spanStart / 2)) {
            // Search backwards.
            জন্য (;;) {
                UBool hasPrevious = previous(errorCode);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (শূন্য)hasPrevious;  // avoid unused-variable warning
                spanStart = findSource ? src_সূচক : dest_সূচক;
                যদি (i >= spanStart) {
                    // The index is in the current span.
                    ফেরত 0;
                }
                যদি (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    span_দৈর্ঘ্য = findSource ? old_দৈর্ঘ্য_2 : new_দৈর্ঘ্য_2;
                    int32_t u = array[সূচক];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_অঙ্ক_MASK) + 1 - remaining;
                    int32_t len = num * span_দৈর্ঘ্য;
                    যদি (i >= (spanStart - len)) {
                        int32_t n = ((spanStart - i - 1) / span_দৈর্ঘ্য) + 1;
                        // 1 <= n <= num
                        src_সূচক -= n * old_দৈর্ঘ্য_2;
                        repl_সূচক -= n * new_দৈর্ঘ্য_2;
                        dest_সূচক -= n * new_দৈর্ঘ্য_2;
                        remaining += n;
                        ফেরত 0;
                    }
                    // Skip all of these edits at once.
                    src_সূচক -= num * old_দৈর্ঘ্য_2;
                    repl_সূচক -= num * new_দৈর্ঘ্য_2;
                    dest_সূচক -= num * new_দৈর্ঘ্য_2;
                    remaining = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        সূচক = remaining = old_দৈর্ঘ্য_2 = new_দৈর্ঘ্য_2 = src_সূচক = repl_সূচক = dest_সূচক = 0;
    } অন্যথা যদি (i < (spanStart + span_দৈর্ঘ্য)) {
        // The index is in the current span.
        ফেরত 0;
    }
    যখন (next(মিথ্যা, errorCode)) {
        যদি (findSource) {
            spanStart = src_সূচক;
            span_দৈর্ঘ্য = old_দৈর্ঘ্য_2;
        } অন্যথা {
            spanStart = dest_সূচক;
            span_দৈর্ঘ্য = new_দৈর্ঘ্য_2;
        }
        যদি (i < (spanStart + span_দৈর্ঘ্য)) {
            // The index is in the current span.
            ফেরত 0;
        }
        যদি (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * span_দৈর্ঘ্য;
            যদি (i < (spanStart + len)) {
                int32_t n = (i - spanStart) / span_দৈর্ঘ্য;  // 1 <= n <= remaining - 1
                src_সূচক += n * old_দৈর্ঘ্য_2;
                repl_সূচক += n * new_দৈর্ঘ্য_2;
                dest_সূচক += n * new_দৈর্ঘ্য_2;
                remaining -= n;
                ফেরত 0;
            }
            // Make next() skip all of these edits at once.
            old_দৈর্ঘ্য_2 *= remaining;
            new_দৈর্ঘ্য_2 *= remaining;
            remaining = 0;
        }
    }
    ফেরত 1;
}

int32_t Edits::Iterator::destination_সূচক_From_Source_সূচক(int32_t i, UErrorCode &errorCode) {
    int32_t where = find_সূচক(i, সত্য, errorCode);
    যদি (where < 0) {
        // Error or before the string.
        ফেরত 0;
    }
    যদি (where > 0 || i == src_সূচক) {
        // At or after string length, or at start of the found span.
        ফেরত dest_সূচক;
    }
    যদি (changed) {
        // In a change span, map to its end.
        ফেরত dest_সূচক + new_দৈর্ঘ্য_2;
    } অন্যথা {
        // In an unchanged span, offset 1:1 within it.
        ফেরত dest_সূচক + (i - src_সূচক);
    }
}

int32_t Edits::Iterator::source_সূচক_From_Destination_সূচক(int32_t i, UErrorCode &errorCode) {
    int32_t where = find_সূচক(i, মিথ্যা, errorCode);
    যদি (where < 0) {
        // Error or before the string.
        ফেরত 0;
    }
    যদি (where > 0 || i == dest_সূচক) {
        // At or after string length, or at start of the found span.
        ফেরত src_সূচক;
    }
    যদি (changed) {
        // In a change span, map to its end.
        ফেরত src_সূচক + old_দৈর্ঘ্য_2;
    } অন্যথা {
        // In an unchanged span, offset within it.
        ফেরত src_সূচক + (i - dest_সূচক);
    }
}

UnicodeString& Edits::Iterator::toString(UnicodeString& sb U_LIFETIME_BOUND) ধ্রুবক {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_সংখ্যা(sb, src_সূচক);
    sb.append(u"..", -1);
    ICU_Utility::append_সংখ্যা(sb, src_সূচক + old_দৈর্ঘ্য_2);
    যদি (changed) {
        sb.append(u"] ⇝ dest[", -1);
    } অন্যথা {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_সংখ্যা(sb, dest_সূচক);
    sb.append(u"..", -1);
    ICU_Utility::append_সংখ্যা(sb, dest_সূচক + new_দৈর্ঘ্য_2);
    যদি (changed) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_সংখ্যা(sb, repl_সূচক);
        sb.append(u"..", -1);
        ICU_Utility::append_সংখ্যা(sb, repl_সূচক + new_দৈর্ঘ্য_2);
        sb.append(u"] }", -1);
    } অন্যথা {
        sb.append(u"] (no-change) }", -1);
    }
    ফেরত sb;
}

U_NAMESPACE_END
