// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#ضمن "unicode/edits.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "util.h"

U_NAMESPACE_BEGIN

نطاق {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
ثابت int32_t MAX_UNCHANGED_ڊيگهائي = 0x1000;
ثابت int32_t MAX_UNCHANGED = MAX_UNCHANGED_ڊيگهائي - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ثابت int32_t MAX_SHORT_CHANGE_OLD_ڊيگهائي = 6;
ثابت int32_t MAX_SHORT_CHANGE_NEW_ڊيگهائي = 7;
ثابت int32_t SHORT_CHANGE_انگ_نقاب = 0x1ff;
ثابت int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ثابت int32_t ڊيگهائي_اندريون_1_TRAIL = 61;
ثابت int32_t ڊيگهائي_اندريون_2_TRAIL = 62;

}  // namespace

فراغ Edits::releaseArray() noexcept {
    إذا (array != ڍير_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ثابت Edits& other) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ_2)) {
        length = delta = انگ_Changes = 0;
        أرجع *هذا;
    }
    إذا (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        إذا (newArray == خالي_اشارو) {
            length = delta = انگ_Changes = 0;
            error_ڪوڊ_2 = U_يادگيري_ورهاست_ERROR;
            أرجع *هذا;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    إذا (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    أرجع *هذا;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ_2)) {
        length = delta = انگ_Changes = 0;
        أرجع *هذا;
    }
    releaseArray();
    إذا (length > ڍير_صلاحيت_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ڍير_Array;
        src.capacity = ڍير_صلاحيت_2;
        src.reset();
        أرجع *هذا;
    }
    array = ڍير_Array;
    capacity = ڍير_صلاحيت_2;
    إذا (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    أرجع *هذا;
}

Edits& Edits::عامل=(ثابت Edits& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) { أرجع *هذا; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    انگ_Changes = other.انگ_Changes;
    error_ڪوڊ_2 = other.error_ڪوڊ_2;
    أرجع copyArray(other);
}

Edits& Edits::عامل=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    انگ_Changes = src.انگ_Changes;
    error_ڪوڊ_2 = src.error_ڪوڊ_2;
    أرجع moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

فراغ Edits::reset() noexcept {
    length = delta = انگ_Changes = 0;
    error_ڪوڊ_2 = U_ZERO_ERROR;
}

فراغ Edits::جوڙيو_Unchanged(int32_t unchanged_ڊيگهائي) {
    إذا(U_FAILURE(error_ڪوڊ_2) || unchanged_ڊيگهائي == 0) { أرجع; }
    إذا(unchanged_ڊيگهائي < 0) {
        error_ڪوڊ_2 = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t آخري = آخري_Unit();
    إذا(آخري < MAX_UNCHANGED) {
        int32_t بچيل = MAX_UNCHANGED - آخري;
        إذا (بچيل >= unchanged_ڊيگهائي) {
            سيٽ_آخري_Unit(آخري + unchanged_ڊيگهائي);
            أرجع;
        }
        سيٽ_آخري_Unit(MAX_UNCHANGED);
        unchanged_ڊيگهائي -= بچيل;
    }
    // Split large lengths into multiple units.
    طالما(unchanged_ڊيگهائي >= MAX_UNCHANGED_ڊيگهائي) {
        append(MAX_UNCHANGED);
        unchanged_ڊيگهائي -= MAX_UNCHANGED_ڊيگهائي;
    }
    // Write a small (remaining) length.
    إذا(unchanged_ڊيگهائي > 0) {
        append(unchanged_ڊيگهائي - 1);
    }
}

فراغ Edits::جوڙيو_Replace(int32_t old_ڊيگهائي, int32_t new_ڊيگهائي) {
    إذا(U_FAILURE(error_ڪوڊ_2)) { أرجع; }
    إذا(old_ڊيگهائي < 0 || new_ڊيگهائي < 0) {
        error_ڪوڊ_2 = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا (old_ڊيگهائي == 0 && new_ڊيگهائي == 0) {
        أرجع;
    }
    ++انگ_Changes;
    int32_t newDelta = new_ڊيگهائي - old_ڊيگهائي;
    إذا (newDelta != 0) {
        إذا ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_ڪوڊ_2 = U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        delta += newDelta;
    }

    إذا(0 < old_ڊيگهائي && old_ڊيگهائي <= MAX_SHORT_CHANGE_OLD_ڊيگهائي &&
            new_ڊيگهائي <= MAX_SHORT_CHANGE_NEW_ڊيگهائي) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_ڊيگهائي << 12) | (new_ڊيگهائي << 9);
        int32_t آخري = آخري_Unit();
        إذا(MAX_UNCHANGED < آخري && آخري < MAX_SHORT_CHANGE &&
                (آخري & ~SHORT_CHANGE_انگ_نقاب) == u &&
                (آخري & SHORT_CHANGE_انگ_نقاب) < SHORT_CHANGE_انگ_نقاب) {
            سيٽ_آخري_Unit(آخري + 1);
            أرجع;
        }
        append(u);
        أرجع;
    }

    int32_t سر = 0x7000;
    إذا (old_ڊيگهائي < ڊيگهائي_اندريون_1_TRAIL && new_ڊيگهائي < ڊيگهائي_اندريون_1_TRAIL) {
        سر |= old_ڊيگهائي << 6;
        سر |= new_ڊيگهائي;
        append(سر);
    } وإلا إذا ((capacity - length) >= 5 || growArray()) {
        int32_t حد_بندي = length + 1;
        إذا(old_ڊيگهائي < ڊيگهائي_اندريون_1_TRAIL) {
            سر |= old_ڊيگهائي << 6;
        } وإلا إذا(old_ڊيگهائي <= 0x7fff) {
            سر |= ڊيگهائي_اندريون_1_TRAIL << 6;
            array[حد_بندي++] = static_cast<uint16_t>(0x8000 | old_ڊيگهائي);
        } وإلا {
            سر |= (ڊيگهائي_اندريون_2_TRAIL + (old_ڊيگهائي >> 30)) << 6;
            array[حد_بندي++] = static_cast<uint16_t>(0x8000 | (old_ڊيگهائي >> 15));
            array[حد_بندي++] = static_cast<uint16_t>(0x8000 | old_ڊيگهائي);
        }
        إذا(new_ڊيگهائي < ڊيگهائي_اندريون_1_TRAIL) {
            سر |= new_ڊيگهائي;
        } وإلا إذا(new_ڊيگهائي <= 0x7fff) {
            سر |= ڊيگهائي_اندريون_1_TRAIL;
            array[حد_بندي++] = static_cast<uint16_t>(0x8000 | new_ڊيگهائي);
        } وإلا {
            سر |= ڊيگهائي_اندريون_2_TRAIL + (new_ڊيگهائي >> 30);
            array[حد_بندي++] = static_cast<uint16_t>(0x8000 | (new_ڊيگهائي >> 15));
            array[حد_بندي++] = static_cast<uint16_t>(0x8000 | new_ڊيگهائي);
        }
        array[length] = static_cast<uint16_t>(سر);
        length = حد_بندي;
    }
}

فراغ Edits::append(int32_t r) {
    إذا(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t new_صلاحيت;
    إذا (array == ڍير_Array) {
        new_صلاحيت = 2000;
    } وإلا إذا (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_ڪوڊ_2 = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    } وإلا إذا (capacity >= (INT32_MAX / 2)) {
        new_صلاحيت = INT32_MAX;
    } وإلا {
        new_صلاحيت = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    إذا ((new_صلاحيت - capacity) < 5) {
        error_ڪوڊ_2 = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_صلاحيت) * 2));
    إذا (newArray == خالي_اشارو) {
        error_ڪوڊ_2 = U_يادگيري_ورهاست_ERROR;
        أرجع خطأ;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_صلاحيت;
    أرجع سچ;
}

UBool Edits::copy_Error_منزل(UError_ڪوڊ &ٻاهريون_Error_ڪوڊ) ثابت {
    إذا (U_FAILURE(ٻاهريون_Error_ڪوڊ)) { أرجع سچ; }
    إذا (U_SUCCESS(error_ڪوڊ_2)) { أرجع خطأ; }
    ٻاهريون_Error_ڪوڊ = error_ڪوڊ_2;
    أرجع سچ;
}

Edits& Edits::ملايو_And_Append(ثابت Edits& ab,
                             ثابت Edits& bc,
                             UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    إذا (copy_Error_منزل(error_ڪوڊ)) { أرجع *هذا; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_ايندڙ = سچ, bc_Has_ايندڙ = سچ;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_ڊيگهائي = 0, ab_b_ڊيگهائي = 0, bc_b_ڊيگهائي = 0, c_ڊيگهائي = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_ڊيگهائي = 0, pending_c_ڊيگهائي = 0;
    لكل (;;) {
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
        إذا (bc_b_ڊيگهائي == 0) {
            إذا (bc_Has_ايندڙ && (bc_Has_ايندڙ = bcIter.next(error_ڪوڊ)) != 0) {
                bc_b_ڊيگهائي = bcIter.old_ڊيگهائي();
                c_ڊيگهائي = bcIter.new_ڊيگهائي();
                إذا (bc_b_ڊيگهائي == 0) {
                    // insertion
                    إذا (ab_b_ڊيگهائي == 0 || !abIter.hasChange()) {
                        جوڙيو_Replace(pending_a_ڊيگهائي, pending_c_ڊيگهائي + c_ڊيگهائي);
                        pending_a_ڊيگهائي = pending_c_ڊيگهائي = 0;
                    } وإلا {
                        pending_c_ڊيگهائي += c_ڊيگهائي;
                    }
                    استمر;
                }
            }
            // else see if the other iterator is done, too.
        }
        إذا (ab_b_ڊيگهائي == 0) {
            إذا (ab_Has_ايندڙ && (ab_Has_ايندڙ = abIter.next(error_ڪوڊ)) != 0) {
                a_ڊيگهائي = abIter.old_ڊيگهائي();
                ab_b_ڊيگهائي = abIter.new_ڊيگهائي();
                إذا (ab_b_ڊيگهائي == 0) {
                    // deletion
                    إذا (bc_b_ڊيگهائي == bcIter.old_ڊيگهائي() || !bcIter.hasChange()) {
                        جوڙيو_Replace(pending_a_ڊيگهائي + a_ڊيگهائي, pending_c_ڊيگهائي);
                        pending_a_ڊيگهائي = pending_c_ڊيگهائي = 0;
                    } وإلا {
                        pending_a_ڊيگهائي += a_ڊيگهائي;
                    }
                    استمر;
                }
            } وإلا إذا (bc_b_ڊيگهائي == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                اكسر;
            } وإلا {
                // The ab output string is shorter than the bc input string.
                إذا (!copy_Error_منزل(error_ڪوڊ)) {
                    error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                }
                أرجع *هذا;
            }
        }
        إذا (bc_b_ڊيگهائي == 0) {
            // The bc input string is shorter than the ab output string.
            إذا (!copy_Error_منزل(error_ڪوڊ)) {
                error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
            }
            أرجع *هذا;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        إذا (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            إذا (pending_a_ڊيگهائي != 0 || pending_c_ڊيگهائي != 0) {
                جوڙيو_Replace(pending_a_ڊيگهائي, pending_c_ڊيگهائي);
                pending_a_ڊيگهائي = pending_c_ڊيگهائي = 0;
            }
            int32_t unchanged_ڊيگهائي = a_ڊيگهائي <= c_ڊيگهائي ? a_ڊيگهائي : c_ڊيگهائي;
            جوڙيو_Unchanged(unchanged_ڊيگهائي);
            ab_b_ڊيگهائي = a_ڊيگهائي -= unchanged_ڊيگهائي;
            bc_b_ڊيگهائي = c_ڊيگهائي -= unchanged_ڊيگهائي;
            // At least one of the unchanged spans is now empty.
            استمر;
        }
        إذا (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            إذا (ab_b_ڊيگهائي >= bc_b_ڊيگهائي) {
                // Split the longer unchanged span into change + remainder.
                جوڙيو_Replace(pending_a_ڊيگهائي + bc_b_ڊيگهائي, pending_c_ڊيگهائي + c_ڊيگهائي);
                pending_a_ڊيگهائي = pending_c_ڊيگهائي = 0;
                a_ڊيگهائي = ab_b_ڊيگهائي -= bc_b_ڊيگهائي;
                bc_b_ڊيگهائي = 0;
                استمر;
            }
            // Handle the shorter unchanged span below like a change.
        } وإلا إذا (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            إذا (ab_b_ڊيگهائي <= bc_b_ڊيگهائي) {
                // Split the longer unchanged span into change + remainder.
                جوڙيو_Replace(pending_a_ڊيگهائي + a_ڊيگهائي, pending_c_ڊيگهائي + ab_b_ڊيگهائي);
                pending_a_ڊيگهائي = pending_c_ڊيگهائي = 0;
                c_ڊيگهائي = bc_b_ڊيگهائي -= ab_b_ڊيگهائي;
                ab_b_ڊيگهائي = 0;
                استمر;
            }
            // Handle the shorter unchanged span below like a change.
        } وإلا {  // both abIter.hasChange() && bcIter.hasChange()
            إذا (ab_b_ڊيگهائي == bc_b_ڊيگهائي) {
                // Changes on both sides up to the same position. Emit & reset.
                جوڙيو_Replace(pending_a_ڊيگهائي + a_ڊيگهائي, pending_c_ڊيگهائي + c_ڊيگهائي);
                pending_a_ڊيگهائي = pending_c_ڊيگهائي = 0;
                ab_b_ڊيگهائي = bc_b_ڊيگهائي = 0;
                استمر;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_ڊيگهائي += a_ڊيگهائي;
        pending_c_ڊيگهائي += c_ڊيگهائي;
        إذا (ab_b_ڊيگهائي < bc_b_ڊيگهائي) {
            bc_b_ڊيگهائي -= ab_b_ڊيگهائي;
            c_ڊيگهائي = ab_b_ڊيگهائي = 0;
        } وإلا {  // ab_bLength > bc_bLength
            ab_b_ڊيگهائي -= bc_b_ڊيگهائي;
            a_ڊيگهائي = bc_b_ڊيگهائي = 0;
        }
    }
    إذا (pending_a_ڊيگهائي != 0 || pending_c_ڊيگهائي != 0) {
        جوڙيو_Replace(pending_a_ڊيگهائي, pending_c_ڊيگهائي);
    }
    copy_Error_منزل(error_ڪوڊ);
    أرجع *هذا;
}

Edits::Iterator::Iterator(ثابت uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), اشاريو(0), length(len), بچيل(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), بدليو(خطأ), old_ڊيگهائي_2(0), new_ڊيگهائي_2(0),
        src_اشاريو(0), repl_اشاريو(0), dest_اشاريو(0) {}

int32_t Edits::Iterator::read_ڊيگهائي(int32_t سر) {
    إذا (سر < ڊيگهائي_اندريون_1_TRAIL) {
        أرجع سر;
    } وإلا إذا (سر < ڊيگهائي_اندريون_2_TRAIL) {
        U_ASSERT(اشاريو < length);
        U_ASSERT(array[اشاريو] >= 0x8000);
        أرجع array[اشاريو++] & 0x7fff;
    } وإلا {
        U_ASSERT((اشاريو + 2) <= length);
        U_ASSERT(array[اشاريو] >= 0x8000);
        U_ASSERT(array[اشاريو + 1] >= 0x8000);
        int32_t len = ((سر & 1) << 30) |
                (static_cast<int32_t>(array[اشاريو] & 0x7fff) << 15) |
                (array[اشاريو + 1] & 0x7fff);
        اشاريو += 2;
        أرجع len;
    }
}

فراغ Edits::Iterator::update_ايندڙ_Indexes() {
    src_اشاريو += old_ڊيگهائي_2;
    إذا (بدليو) {
        repl_اشاريو += new_ڊيگهائي_2;
    }
    dest_اشاريو += new_ڊيگهائي_2;
}

فراغ Edits::Iterator::update_اڳيون_Indexes() {
    src_اشاريو -= old_ڊيگهائي_2;
    إذا (بدليو) {
        repl_اشاريو -= new_ڊيگهائي_2;
    }
    dest_اشاريو -= new_ڊيگهائي_2;
}

UBool Edits::Iterator::no_ايندڙ() {
    // No change before or beyond the string.
    dir = 0;
    بدليو = خطأ;
    old_ڊيگهائي_2 = new_ڊيگهائي_2 = 0;
    أرجع خطأ;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_ڪوڊ &error_ڪوڊ) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    إذا (dir > 0) {
        update_ايندڙ_Indexes();
    } وإلا {
        إذا (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            إذا (بچيل > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++اشاريو;  // next() rests on the index after the sequence unit.
                dir = 1;
                أرجع سچ;
            }
        }
        dir = 1;
    }
    إذا (بچيل >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        إذا (بچيل > 1) {
            --بچيل;
            أرجع سچ;
        }
        بچيل = 0;
    }
    إذا (اشاريو >= length) {
        أرجع no_ايندڙ();
    }
    int32_t u = array[اشاريو++];
    إذا (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        بدليو = خطأ;
        old_ڊيگهائي_2 = u + 1;
        طالما (اشاريو < length && (u = array[اشاريو]) <= MAX_UNCHANGED) {
            ++اشاريو;
            old_ڊيگهائي_2 += u + 1;
        }
        new_ڊيگهائي_2 = old_ڊيگهائي_2;
        إذا (onlyChanges) {
            update_ايندڙ_Indexes();
            إذا (اشاريو >= length) {
                أرجع no_ايندڙ();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++اشاريو;
        } وإلا {
            أرجع سچ;
        }
    }
    بدليو = سچ;
    إذا (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_ڊيگهائي;
        int32_t num = (u & SHORT_CHANGE_انگ_نقاب) + 1;
        إذا (coarse) {
            old_ڊيگهائي_2 = num * oldLen;
            new_ڊيگهائي_2 = num * newLen;
        } وإلا {
            // Split a sequence of changes that was compressed into one unit.
            old_ڊيگهائي_2 = oldLen;
            new_ڊيگهائي_2 = newLen;
            إذا (num > 1) {
                بچيل = num;  // This is the first of two or more changes.
            }
            أرجع سچ;
        }
    } وإلا {
        U_ASSERT(u <= 0x7fff);
        old_ڊيگهائي_2 = read_ڊيگهائي((u >> 6) & 0x3f);
        new_ڊيگهائي_2 = read_ڊيگهائي(u & 0x3f);
        إذا (!coarse) {
            أرجع سچ;
        }
    }
    // Combine adjacent changes.
    طالما (اشاريو < length && (u = array[اشاريو]) > MAX_UNCHANGED) {
        ++اشاريو;
        إذا (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_انگ_نقاب) + 1;
            old_ڊيگهائي_2 += (u >> 12) * num;
            new_ڊيگهائي_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_ڊيگهائي) * num;
        } وإلا {
            U_ASSERT(u <= 0x7fff);
            old_ڊيگهائي_2 += read_ڊيگهائي((u >> 6) & 0x3f);
            new_ڊيگهائي_2 += read_ڊيگهائي(u & 0x3f);
        }
    }
    أرجع سچ;
}

UBool Edits::Iterator::اڳيون(UError_ڪوڊ &error_ڪوڊ) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    إذا (dir >= 0) {
        إذا (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            إذا (بچيل > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --اشاريو;  // previous() rests on the sequence unit.
                dir = -1;
                أرجع سچ;
            }
            update_ايندڙ_Indexes();
        }
        dir = -1;
    }
    إذا (بچيل > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[اشاريو];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        إذا (بچيل <= (u & SHORT_CHANGE_انگ_نقاب)) {
            ++بچيل;
            update_اڳيون_Indexes();
            أرجع سچ;
        }
        بچيل = 0;
    }
    إذا (اشاريو <= 0) {
        أرجع no_ايندڙ();
    }
    int32_t u = array[--اشاريو];
    إذا (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        بدليو = خطأ;
        old_ڊيگهائي_2 = u + 1;
        طالما (اشاريو > 0 && (u = array[اشاريو - 1]) <= MAX_UNCHANGED) {
            --اشاريو;
            old_ڊيگهائي_2 += u + 1;
        }
        new_ڊيگهائي_2 = old_ڊيگهائي_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        update_اڳيون_Indexes();
        أرجع سچ;
    }
    بدليو = سچ;
    إذا (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_ڊيگهائي;
        int32_t num = (u & SHORT_CHANGE_انگ_نقاب) + 1;
        إذا (coarse) {
            old_ڊيگهائي_2 = num * oldLen;
            new_ڊيگهائي_2 = num * newLen;
        } وإلا {
            // Split a sequence of changes that was compressed into one unit.
            old_ڊيگهائي_2 = oldLen;
            new_ڊيگهائي_2 = newLen;
            إذا (num > 1) {
                بچيل = 1;  // This is the last of two or more changes.
            }
            update_اڳيون_Indexes();
            أرجع سچ;
        }
    } وإلا {
        إذا (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_ڊيگهائي_2 = read_ڊيگهائي((u >> 6) & 0x3f);
            new_ڊيگهائي_2 = read_ڊيگهائي(u & 0x3f);
        } وإلا {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(اشاريو > 0);
            طالما ((u = array[--اشاريو]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t سر_اشاريو = اشاريو++;
            old_ڊيگهائي_2 = read_ڊيگهائي((u >> 6) & 0x3f);
            new_ڊيگهائي_2 = read_ڊيگهائي(u & 0x3f);
            اشاريو = سر_اشاريو;
        }
        إذا (!coarse) {
            update_اڳيون_Indexes();
            أرجع سچ;
        }
    }
    // Combine adjacent changes.
    طالما (اشاريو > 0 && (u = array[اشاريو - 1]) > MAX_UNCHANGED) {
        --اشاريو;
        إذا (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_انگ_نقاب) + 1;
            old_ڊيگهائي_2 += (u >> 12) * num;
            new_ڊيگهائي_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_ڊيگهائي) * num;
        } وإلا إذا (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t سر_اشاريو = اشاريو++;
            old_ڊيگهائي_2 += read_ڊيگهائي((u >> 6) & 0x3f);
            new_ڊيگهائي_2 += read_ڊيگهائي(u & 0x3f);
            اشاريو = سر_اشاريو;
        }
    }
    update_اڳيون_Indexes();
    أرجع سچ;
}

int32_t Edits::Iterator::ڳوليو_اشاريو(int32_t i, UBool ڳوليو_ذريعو, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ) || i < 0) { أرجع -1; }
    int32_t وسعت_شروعات, وسعت_ڊيگهائي;
    إذا (ڳوليو_ذريعو) {  // find source index
        وسعت_شروعات = src_اشاريو;
        وسعت_ڊيگهائي = old_ڊيگهائي_2;
    } وإلا {  // find destination index
        وسعت_شروعات = dest_اشاريو;
        وسعت_ڊيگهائي = new_ڊيگهائي_2;
    }
    إذا (i < وسعت_شروعات) {
        إذا (i >= (وسعت_شروعات / 2)) {
            // Search backwards.
            لكل (;;) {
                UBool has_اڳيون = اڳيون(error_ڪوڊ);
                U_ASSERT(has_اڳيون);  // because i>=0 and the first span starts at 0
                (فراغ)has_اڳيون;  // avoid unused-variable warning
                وسعت_شروعات = ڳوليو_ذريعو ? src_اشاريو : dest_اشاريو;
                إذا (i >= وسعت_شروعات) {
                    // The index is in the current span.
                    أرجع 0;
                }
                إذا (بچيل > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    وسعت_ڊيگهائي = ڳوليو_ذريعو ? old_ڊيگهائي_2 : new_ڊيگهائي_2;
                    int32_t u = array[اشاريو];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_انگ_نقاب) + 1 - بچيل;
                    int32_t len = num * وسعت_ڊيگهائي;
                    إذا (i >= (وسعت_شروعات - len)) {
                        int32_t n = ((وسعت_شروعات - i - 1) / وسعت_ڊيگهائي) + 1;
                        // 1 <= n <= num
                        src_اشاريو -= n * old_ڊيگهائي_2;
                        repl_اشاريو -= n * new_ڊيگهائي_2;
                        dest_اشاريو -= n * new_ڊيگهائي_2;
                        بچيل += n;
                        أرجع 0;
                    }
                    // Skip all of these edits at once.
                    src_اشاريو -= num * old_ڊيگهائي_2;
                    repl_اشاريو -= num * new_ڊيگهائي_2;
                    dest_اشاريو -= num * new_ڊيگهائي_2;
                    بچيل = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        اشاريو = بچيل = old_ڊيگهائي_2 = new_ڊيگهائي_2 = src_اشاريو = repl_اشاريو = dest_اشاريو = 0;
    } وإلا إذا (i < (وسعت_شروعات + وسعت_ڊيگهائي)) {
        // The index is in the current span.
        أرجع 0;
    }
    طالما (next(خطأ, error_ڪوڊ)) {
        إذا (ڳوليو_ذريعو) {
            وسعت_شروعات = src_اشاريو;
            وسعت_ڊيگهائي = old_ڊيگهائي_2;
        } وإلا {
            وسعت_شروعات = dest_اشاريو;
            وسعت_ڊيگهائي = new_ڊيگهائي_2;
        }
        إذا (i < (وسعت_شروعات + وسعت_ڊيگهائي)) {
            // The index is in the current span.
            أرجع 0;
        }
        إذا (بچيل > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = بچيل * وسعت_ڊيگهائي;
            إذا (i < (وسعت_شروعات + len)) {
                int32_t n = (i - وسعت_شروعات) / وسعت_ڊيگهائي;  // 1 <= n <= remaining - 1
                src_اشاريو += n * old_ڊيگهائي_2;
                repl_اشاريو += n * new_ڊيگهائي_2;
                dest_اشاريو += n * new_ڊيگهائي_2;
                بچيل -= n;
                أرجع 0;
            }
            // Make next() skip all of these edits at once.
            old_ڊيگهائي_2 *= بچيل;
            new_ڊيگهائي_2 *= بچيل;
            بچيل = 0;
        }
    }
    أرجع 1;
}

int32_t Edits::Iterator::destination_اشاريو_From_ذريعو_اشاريو(int32_t i, UError_ڪوڊ &error_ڪوڊ) {
    int32_t where = ڳوليو_اشاريو(i, سچ, error_ڪوڊ);
    إذا (where < 0) {
        // Error or before the string.
        أرجع 0;
    }
    إذا (where > 0 || i == src_اشاريو) {
        // At or after string length, or at start of the found span.
        أرجع dest_اشاريو;
    }
    إذا (بدليو) {
        // In a change span, map to its end.
        أرجع dest_اشاريو + new_ڊيگهائي_2;
    } وإلا {
        // In an unchanged span, offset 1:1 within it.
        أرجع dest_اشاريو + (i - src_اشاريو);
    }
}

int32_t Edits::Iterator::ذريعو_اشاريو_From_Destination_اشاريو(int32_t i, UError_ڪوڊ &error_ڪوڊ) {
    int32_t where = ڳوليو_اشاريو(i, خطأ, error_ڪوڊ);
    إذا (where < 0) {
        // Error or before the string.
        أرجع 0;
    }
    إذا (where > 0 || i == dest_اشاريو) {
        // At or after string length, or at start of the found span.
        أرجع src_اشاريو;
    }
    إذا (بدليو) {
        // In a change span, map to its end.
        أرجع src_اشاريو + old_ڊيگهائي_2;
    } وإلا {
        // In an unchanged span, offset within it.
        أرجع src_اشاريو + (i - dest_اشاريو);
    }
}

UnicodeString& Edits::Iterator::منزل_String(UnicodeString& sb U_LIFETIME_BOUND) ثابت {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_عدد(sb, src_اشاريو);
    sb.append(u"..", -1);
    ICU_Utility::append_عدد(sb, src_اشاريو + old_ڊيگهائي_2);
    إذا (بدليو) {
        sb.append(u"] ⇝ dest[", -1);
    } وإلا {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_عدد(sb, dest_اشاريو);
    sb.append(u"..", -1);
    ICU_Utility::append_عدد(sb, dest_اشاريو + new_ڊيگهائي_2);
    إذا (بدليو) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_عدد(sb, repl_اشاريو);
        sb.append(u"..", -1);
        ICU_Utility::append_عدد(sb, repl_اشاريو + new_ڊيگهائي_2);
        sb.append(u"] }", -1);
    } وإلا {
        sb.append(u"] (no-change) }", -1);
    }
    أرجع sb;
}

U_NAMESPACE_پڇاڙي
