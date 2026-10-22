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
ثابت int32_t MAX_UNCHANGED_درازی = 0x1000;
ثابت int32_t MAX_UNCHANGED = MAX_UNCHANGED_درازی - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ثابت int32_t MAX_SHORT_CHANGE_OLD_درازی = 6;
ثابت int32_t MAX_SHORT_CHANGE_NEW_درازی = 7;
ثابت int32_t SHORT_CHANGE_رقم_ماسک = 0x1ff;
ثابت int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ثابت int32_t درازی_درون_1_TRAIL = 61;
ثابت int32_t درازی_درون_2_TRAIL = 62;

}  // namespace

فراغ Edits::releaseArray() noexcept {
    إذا (array != پشته_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ثابت Edits& other) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد_2)) {
        length = delta = رقم_Changes = 0;
        أرجع *هذا;
    }
    إذا (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        إذا (newArray == اشاره_تهی) {
            length = delta = رقم_Changes = 0;
            error_کد_2 = U_حافظه_تخصیص_ERROR;
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
    إذا (U_FAILURE(error_کد_2)) {
        length = delta = رقم_Changes = 0;
        أرجع *هذا;
    }
    releaseArray();
    إذا (length > پشته_ظرفیت_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.پشته_Array;
        src.capacity = پشته_ظرفیت_2;
        src.reset();
        أرجع *هذا;
    }
    array = پشته_Array;
    capacity = پشته_ظرفیت_2;
    إذا (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    أرجع *هذا;
}

Edits& Edits::عملگر=(ثابت Edits& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) { أرجع *هذا; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    رقم_Changes = other.رقم_Changes;
    error_کد_2 = other.error_کد_2;
    أرجع copyArray(other);
}

Edits& Edits::عملگر=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    رقم_Changes = src.رقم_Changes;
    error_کد_2 = src.error_کد_2;
    أرجع moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

فراغ Edits::reset() noexcept {
    length = delta = رقم_Changes = 0;
    error_کد_2 = U_ZERO_ERROR;
}

فراغ Edits::جمع_کن_Unchanged(int32_t unchanged_درازی) {
    إذا(U_FAILURE(error_کد_2) || unchanged_درازی == 0) { أرجع; }
    إذا(unchanged_درازی < 0) {
        error_کد_2 = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t آخرین = آخرین_Unit();
    إذا(آخرین < MAX_UNCHANGED) {
        int32_t مانده = MAX_UNCHANGED - آخرین;
        إذا (مانده >= unchanged_درازی) {
            مجموعه_آخرین_Unit(آخرین + unchanged_درازی);
            أرجع;
        }
        مجموعه_آخرین_Unit(MAX_UNCHANGED);
        unchanged_درازی -= مانده;
    }
    // Split large lengths into multiple units.
    طالما(unchanged_درازی >= MAX_UNCHANGED_درازی) {
        append(MAX_UNCHANGED);
        unchanged_درازی -= MAX_UNCHANGED_درازی;
    }
    // Write a small (remaining) length.
    إذا(unchanged_درازی > 0) {
        append(unchanged_درازی - 1);
    }
}

فراغ Edits::جمع_کن_Replace(int32_t old_درازی, int32_t new_درازی) {
    إذا(U_FAILURE(error_کد_2)) { أرجع; }
    إذا(old_درازی < 0 || new_درازی < 0) {
        error_کد_2 = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    إذا (old_درازی == 0 && new_درازی == 0) {
        أرجع;
    }
    ++رقم_Changes;
    int32_t newDelta = new_درازی - old_درازی;
    إذا (newDelta != 0) {
        إذا ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_کد_2 = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        delta += newDelta;
    }

    إذا(0 < old_درازی && old_درازی <= MAX_SHORT_CHANGE_OLD_درازی &&
            new_درازی <= MAX_SHORT_CHANGE_NEW_درازی) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_درازی << 12) | (new_درازی << 9);
        int32_t آخرین = آخرین_Unit();
        إذا(MAX_UNCHANGED < آخرین && آخرین < MAX_SHORT_CHANGE &&
                (آخرین & ~SHORT_CHANGE_رقم_ماسک) == u &&
                (آخرین & SHORT_CHANGE_رقم_ماسک) < SHORT_CHANGE_رقم_ماسک) {
            مجموعه_آخرین_Unit(آخرین + 1);
            أرجع;
        }
        append(u);
        أرجع;
    }

    int32_t سر = 0x7000;
    إذا (old_درازی < درازی_درون_1_TRAIL && new_درازی < درازی_درون_1_TRAIL) {
        سر |= old_درازی << 6;
        سر |= new_درازی;
        append(سر);
    } وإلا إذا ((capacity - length) >= 5 || growArray()) {
        int32_t حد = length + 1;
        إذا(old_درازی < درازی_درون_1_TRAIL) {
            سر |= old_درازی << 6;
        } وإلا إذا(old_درازی <= 0x7fff) {
            سر |= درازی_درون_1_TRAIL << 6;
            array[حد++] = static_cast<uint16_t>(0x8000 | old_درازی);
        } وإلا {
            سر |= (درازی_درون_2_TRAIL + (old_درازی >> 30)) << 6;
            array[حد++] = static_cast<uint16_t>(0x8000 | (old_درازی >> 15));
            array[حد++] = static_cast<uint16_t>(0x8000 | old_درازی);
        }
        إذا(new_درازی < درازی_درون_1_TRAIL) {
            سر |= new_درازی;
        } وإلا إذا(new_درازی <= 0x7fff) {
            سر |= درازی_درون_1_TRAIL;
            array[حد++] = static_cast<uint16_t>(0x8000 | new_درازی);
        } وإلا {
            سر |= درازی_درون_2_TRAIL + (new_درازی >> 30);
            array[حد++] = static_cast<uint16_t>(0x8000 | (new_درازی >> 15));
            array[حد++] = static_cast<uint16_t>(0x8000 | new_درازی);
        }
        array[length] = static_cast<uint16_t>(سر);
        length = حد;
    }
}

فراغ Edits::append(int32_t r) {
    إذا(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t new_ظرفیت;
    إذا (array == پشته_Array) {
        new_ظرفیت = 2000;
    } وإلا إذا (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_کد_2 = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    } وإلا إذا (capacity >= (INT32_MAX / 2)) {
        new_ظرفیت = INT32_MAX;
    } وإلا {
        new_ظرفیت = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    إذا ((new_ظرفیت - capacity) < 5) {
        error_کد_2 = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_ظرفیت) * 2));
    إذا (newArray == اشاره_تهی) {
        error_کد_2 = U_حافظه_تخصیص_ERROR;
        أرجع خطأ;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_ظرفیت;
    أرجع درست;
}

UBool Edits::copy_Error_مقصد(UError_کد &بیرون_Error_کد) ثابت {
    إذا (U_FAILURE(بیرون_Error_کد)) { أرجع درست; }
    إذا (U_SUCCESS(error_کد_2)) { أرجع خطأ; }
    بیرون_Error_کد = error_کد_2;
    أرجع درست;
}

Edits& Edits::ادغام_And_Append(ثابت Edits& ab,
                             ثابت Edits& bc,
                             UError_کد& error_کد) U_LIFETIME_BOUND {
    إذا (copy_Error_مقصد(error_کد)) { أرجع *هذا; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_بعدی = درست, bc_Has_بعدی = درست;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_درازی = 0, ab_b_درازی = 0, bc_b_درازی = 0, c_درازی = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_درازی = 0, pending_c_درازی = 0;
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
        إذا (bc_b_درازی == 0) {
            إذا (bc_Has_بعدی && (bc_Has_بعدی = bcIter.next(error_کد)) != 0) {
                bc_b_درازی = bcIter.old_درازی();
                c_درازی = bcIter.new_درازی();
                إذا (bc_b_درازی == 0) {
                    // insertion
                    إذا (ab_b_درازی == 0 || !abIter.hasChange()) {
                        جمع_کن_Replace(pending_a_درازی, pending_c_درازی + c_درازی);
                        pending_a_درازی = pending_c_درازی = 0;
                    } وإلا {
                        pending_c_درازی += c_درازی;
                    }
                    استمر;
                }
            }
            // else see if the other iterator is done, too.
        }
        إذا (ab_b_درازی == 0) {
            إذا (ab_Has_بعدی && (ab_Has_بعدی = abIter.next(error_کد)) != 0) {
                a_درازی = abIter.old_درازی();
                ab_b_درازی = abIter.new_درازی();
                إذا (ab_b_درازی == 0) {
                    // deletion
                    إذا (bc_b_درازی == bcIter.old_درازی() || !bcIter.hasChange()) {
                        جمع_کن_Replace(pending_a_درازی + a_درازی, pending_c_درازی);
                        pending_a_درازی = pending_c_درازی = 0;
                    } وإلا {
                        pending_a_درازی += a_درازی;
                    }
                    استمر;
                }
            } وإلا إذا (bc_b_درازی == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                اكسر;
            } وإلا {
                // The ab output string is shorter than the bc input string.
                إذا (!copy_Error_مقصد(error_کد)) {
                    error_کد = U_ILLEGAL_آرگومان_ERROR;
                }
                أرجع *هذا;
            }
        }
        إذا (bc_b_درازی == 0) {
            // The bc input string is shorter than the ab output string.
            إذا (!copy_Error_مقصد(error_کد)) {
                error_کد = U_ILLEGAL_آرگومان_ERROR;
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
            إذا (pending_a_درازی != 0 || pending_c_درازی != 0) {
                جمع_کن_Replace(pending_a_درازی, pending_c_درازی);
                pending_a_درازی = pending_c_درازی = 0;
            }
            int32_t unchanged_درازی = a_درازی <= c_درازی ? a_درازی : c_درازی;
            جمع_کن_Unchanged(unchanged_درازی);
            ab_b_درازی = a_درازی -= unchanged_درازی;
            bc_b_درازی = c_درازی -= unchanged_درازی;
            // At least one of the unchanged spans is now empty.
            استمر;
        }
        إذا (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            إذا (ab_b_درازی >= bc_b_درازی) {
                // Split the longer unchanged span into change + remainder.
                جمع_کن_Replace(pending_a_درازی + bc_b_درازی, pending_c_درازی + c_درازی);
                pending_a_درازی = pending_c_درازی = 0;
                a_درازی = ab_b_درازی -= bc_b_درازی;
                bc_b_درازی = 0;
                استمر;
            }
            // Handle the shorter unchanged span below like a change.
        } وإلا إذا (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            إذا (ab_b_درازی <= bc_b_درازی) {
                // Split the longer unchanged span into change + remainder.
                جمع_کن_Replace(pending_a_درازی + a_درازی, pending_c_درازی + ab_b_درازی);
                pending_a_درازی = pending_c_درازی = 0;
                c_درازی = bc_b_درازی -= ab_b_درازی;
                ab_b_درازی = 0;
                استمر;
            }
            // Handle the shorter unchanged span below like a change.
        } وإلا {  // both abIter.hasChange() && bcIter.hasChange()
            إذا (ab_b_درازی == bc_b_درازی) {
                // Changes on both sides up to the same position. Emit & reset.
                جمع_کن_Replace(pending_a_درازی + a_درازی, pending_c_درازی + c_درازی);
                pending_a_درازی = pending_c_درازی = 0;
                ab_b_درازی = bc_b_درازی = 0;
                استمر;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_درازی += a_درازی;
        pending_c_درازی += c_درازی;
        إذا (ab_b_درازی < bc_b_درازی) {
            bc_b_درازی -= ab_b_درازی;
            c_درازی = ab_b_درازی = 0;
        } وإلا {  // ab_bLength > bc_bLength
            ab_b_درازی -= bc_b_درازی;
            a_درازی = bc_b_درازی = 0;
        }
    }
    إذا (pending_a_درازی != 0 || pending_c_درازی != 0) {
        جمع_کن_Replace(pending_a_درازی, pending_c_درازی);
    }
    copy_Error_مقصد(error_کد);
    أرجع *هذا;
}

Edits::Iterator::Iterator(ثابت uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), شاخص(0), length(len), مانده(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), تغییریافته(خطأ), old_درازی_2(0), new_درازی_2(0),
        src_شاخص(0), repl_شاخص(0), dest_شاخص(0) {}

int32_t Edits::Iterator::read_درازی(int32_t سر) {
    إذا (سر < درازی_درون_1_TRAIL) {
        أرجع سر;
    } وإلا إذا (سر < درازی_درون_2_TRAIL) {
        U_ASSERT(شاخص < length);
        U_ASSERT(array[شاخص] >= 0x8000);
        أرجع array[شاخص++] & 0x7fff;
    } وإلا {
        U_ASSERT((شاخص + 2) <= length);
        U_ASSERT(array[شاخص] >= 0x8000);
        U_ASSERT(array[شاخص + 1] >= 0x8000);
        int32_t len = ((سر & 1) << 30) |
                (static_cast<int32_t>(array[شاخص] & 0x7fff) << 15) |
                (array[شاخص + 1] & 0x7fff);
        شاخص += 2;
        أرجع len;
    }
}

فراغ Edits::Iterator::update_بعدی_Indexes() {
    src_شاخص += old_درازی_2;
    إذا (تغییریافته) {
        repl_شاخص += new_درازی_2;
    }
    dest_شاخص += new_درازی_2;
}

فراغ Edits::Iterator::update_قبلی_Indexes() {
    src_شاخص -= old_درازی_2;
    إذا (تغییریافته) {
        repl_شاخص -= new_درازی_2;
    }
    dest_شاخص -= new_درازی_2;
}

UBool Edits::Iterator::no_بعدی() {
    // No change before or beyond the string.
    dir = 0;
    تغییریافته = خطأ;
    old_درازی_2 = new_درازی_2 = 0;
    أرجع خطأ;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_کد &error_کد) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    إذا (dir > 0) {
        update_بعدی_Indexes();
    } وإلا {
        إذا (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            إذا (مانده > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++شاخص;  // next() rests on the index after the sequence unit.
                dir = 1;
                أرجع درست;
            }
        }
        dir = 1;
    }
    إذا (مانده >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        إذا (مانده > 1) {
            --مانده;
            أرجع درست;
        }
        مانده = 0;
    }
    إذا (شاخص >= length) {
        أرجع no_بعدی();
    }
    int32_t u = array[شاخص++];
    إذا (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        تغییریافته = خطأ;
        old_درازی_2 = u + 1;
        طالما (شاخص < length && (u = array[شاخص]) <= MAX_UNCHANGED) {
            ++شاخص;
            old_درازی_2 += u + 1;
        }
        new_درازی_2 = old_درازی_2;
        إذا (onlyChanges) {
            update_بعدی_Indexes();
            إذا (شاخص >= length) {
                أرجع no_بعدی();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++شاخص;
        } وإلا {
            أرجع درست;
        }
    }
    تغییریافته = درست;
    إذا (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_درازی;
        int32_t num = (u & SHORT_CHANGE_رقم_ماسک) + 1;
        إذا (coarse) {
            old_درازی_2 = num * oldLen;
            new_درازی_2 = num * newLen;
        } وإلا {
            // Split a sequence of changes that was compressed into one unit.
            old_درازی_2 = oldLen;
            new_درازی_2 = newLen;
            إذا (num > 1) {
                مانده = num;  // This is the first of two or more changes.
            }
            أرجع درست;
        }
    } وإلا {
        U_ASSERT(u <= 0x7fff);
        old_درازی_2 = read_درازی((u >> 6) & 0x3f);
        new_درازی_2 = read_درازی(u & 0x3f);
        إذا (!coarse) {
            أرجع درست;
        }
    }
    // Combine adjacent changes.
    طالما (شاخص < length && (u = array[شاخص]) > MAX_UNCHANGED) {
        ++شاخص;
        إذا (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_رقم_ماسک) + 1;
            old_درازی_2 += (u >> 12) * num;
            new_درازی_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_درازی) * num;
        } وإلا {
            U_ASSERT(u <= 0x7fff);
            old_درازی_2 += read_درازی((u >> 6) & 0x3f);
            new_درازی_2 += read_درازی(u & 0x3f);
        }
    }
    أرجع درست;
}

UBool Edits::Iterator::قبلی(UError_کد &error_کد) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    إذا (dir >= 0) {
        إذا (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            إذا (مانده > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --شاخص;  // previous() rests on the sequence unit.
                dir = -1;
                أرجع درست;
            }
            update_بعدی_Indexes();
        }
        dir = -1;
    }
    إذا (مانده > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[شاخص];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        إذا (مانده <= (u & SHORT_CHANGE_رقم_ماسک)) {
            ++مانده;
            update_قبلی_Indexes();
            أرجع درست;
        }
        مانده = 0;
    }
    إذا (شاخص <= 0) {
        أرجع no_بعدی();
    }
    int32_t u = array[--شاخص];
    إذا (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        تغییریافته = خطأ;
        old_درازی_2 = u + 1;
        طالما (شاخص > 0 && (u = array[شاخص - 1]) <= MAX_UNCHANGED) {
            --شاخص;
            old_درازی_2 += u + 1;
        }
        new_درازی_2 = old_درازی_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        update_قبلی_Indexes();
        أرجع درست;
    }
    تغییریافته = درست;
    إذا (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_درازی;
        int32_t num = (u & SHORT_CHANGE_رقم_ماسک) + 1;
        إذا (coarse) {
            old_درازی_2 = num * oldLen;
            new_درازی_2 = num * newLen;
        } وإلا {
            // Split a sequence of changes that was compressed into one unit.
            old_درازی_2 = oldLen;
            new_درازی_2 = newLen;
            إذا (num > 1) {
                مانده = 1;  // This is the last of two or more changes.
            }
            update_قبلی_Indexes();
            أرجع درست;
        }
    } وإلا {
        إذا (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_درازی_2 = read_درازی((u >> 6) & 0x3f);
            new_درازی_2 = read_درازی(u & 0x3f);
        } وإلا {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(شاخص > 0);
            طالما ((u = array[--شاخص]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t سر_شاخص = شاخص++;
            old_درازی_2 = read_درازی((u >> 6) & 0x3f);
            new_درازی_2 = read_درازی(u & 0x3f);
            شاخص = سر_شاخص;
        }
        إذا (!coarse) {
            update_قبلی_Indexes();
            أرجع درست;
        }
    }
    // Combine adjacent changes.
    طالما (شاخص > 0 && (u = array[شاخص - 1]) > MAX_UNCHANGED) {
        --شاخص;
        إذا (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_رقم_ماسک) + 1;
            old_درازی_2 += (u >> 12) * num;
            new_درازی_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_درازی) * num;
        } وإلا إذا (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t سر_شاخص = شاخص++;
            old_درازی_2 += read_درازی((u >> 6) & 0x3f);
            new_درازی_2 += read_درازی(u & 0x3f);
            شاخص = سر_شاخص;
        }
    }
    update_قبلی_Indexes();
    أرجع درست;
}

int32_t Edits::Iterator::پیدا_کن_شاخص(int32_t i, UBool پیدا_کن_منبع, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد) || i < 0) { أرجع -1; }
    int32_t بازه_شروع, بازه_درازی;
    إذا (پیدا_کن_منبع) {  // find source index
        بازه_شروع = src_شاخص;
        بازه_درازی = old_درازی_2;
    } وإلا {  // find destination index
        بازه_شروع = dest_شاخص;
        بازه_درازی = new_درازی_2;
    }
    إذا (i < بازه_شروع) {
        إذا (i >= (بازه_شروع / 2)) {
            // Search backwards.
            لكل (;;) {
                UBool has_قبلی = قبلی(error_کد);
                U_ASSERT(has_قبلی);  // because i>=0 and the first span starts at 0
                (فراغ)has_قبلی;  // avoid unused-variable warning
                بازه_شروع = پیدا_کن_منبع ? src_شاخص : dest_شاخص;
                إذا (i >= بازه_شروع) {
                    // The index is in the current span.
                    أرجع 0;
                }
                إذا (مانده > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    بازه_درازی = پیدا_کن_منبع ? old_درازی_2 : new_درازی_2;
                    int32_t u = array[شاخص];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_رقم_ماسک) + 1 - مانده;
                    int32_t len = num * بازه_درازی;
                    إذا (i >= (بازه_شروع - len)) {
                        int32_t n = ((بازه_شروع - i - 1) / بازه_درازی) + 1;
                        // 1 <= n <= num
                        src_شاخص -= n * old_درازی_2;
                        repl_شاخص -= n * new_درازی_2;
                        dest_شاخص -= n * new_درازی_2;
                        مانده += n;
                        أرجع 0;
                    }
                    // Skip all of these edits at once.
                    src_شاخص -= num * old_درازی_2;
                    repl_شاخص -= num * new_درازی_2;
                    dest_شاخص -= num * new_درازی_2;
                    مانده = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        شاخص = مانده = old_درازی_2 = new_درازی_2 = src_شاخص = repl_شاخص = dest_شاخص = 0;
    } وإلا إذا (i < (بازه_شروع + بازه_درازی)) {
        // The index is in the current span.
        أرجع 0;
    }
    طالما (next(خطأ, error_کد)) {
        إذا (پیدا_کن_منبع) {
            بازه_شروع = src_شاخص;
            بازه_درازی = old_درازی_2;
        } وإلا {
            بازه_شروع = dest_شاخص;
            بازه_درازی = new_درازی_2;
        }
        إذا (i < (بازه_شروع + بازه_درازی)) {
            // The index is in the current span.
            أرجع 0;
        }
        إذا (مانده > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = مانده * بازه_درازی;
            إذا (i < (بازه_شروع + len)) {
                int32_t n = (i - بازه_شروع) / بازه_درازی;  // 1 <= n <= remaining - 1
                src_شاخص += n * old_درازی_2;
                repl_شاخص += n * new_درازی_2;
                dest_شاخص += n * new_درازی_2;
                مانده -= n;
                أرجع 0;
            }
            // Make next() skip all of these edits at once.
            old_درازی_2 *= مانده;
            new_درازی_2 *= مانده;
            مانده = 0;
        }
    }
    أرجع 1;
}

int32_t Edits::Iterator::destination_شاخص_From_منبع_شاخص(int32_t i, UError_کد &error_کد) {
    int32_t where = پیدا_کن_شاخص(i, درست, error_کد);
    إذا (where < 0) {
        // Error or before the string.
        أرجع 0;
    }
    إذا (where > 0 || i == src_شاخص) {
        // At or after string length, or at start of the found span.
        أرجع dest_شاخص;
    }
    إذا (تغییریافته) {
        // In a change span, map to its end.
        أرجع dest_شاخص + new_درازی_2;
    } وإلا {
        // In an unchanged span, offset 1:1 within it.
        أرجع dest_شاخص + (i - src_شاخص);
    }
}

int32_t Edits::Iterator::منبع_شاخص_From_Destination_شاخص(int32_t i, UError_کد &error_کد) {
    int32_t where = پیدا_کن_شاخص(i, خطأ, error_کد);
    إذا (where < 0) {
        // Error or before the string.
        أرجع 0;
    }
    إذا (where > 0 || i == dest_شاخص) {
        // At or after string length, or at start of the found span.
        أرجع src_شاخص;
    }
    إذا (تغییریافته) {
        // In a change span, map to its end.
        أرجع src_شاخص + old_درازی_2;
    } وإلا {
        // In an unchanged span, offset within it.
        أرجع src_شاخص + (i - dest_شاخص);
    }
}

UnicodeString& Edits::Iterator::مقصد_String(UnicodeString& sb U_LIFETIME_BOUND) ثابت {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_عدد(sb, src_شاخص);
    sb.append(u"..", -1);
    ICU_Utility::append_عدد(sb, src_شاخص + old_درازی_2);
    إذا (تغییریافته) {
        sb.append(u"] ⇝ dest[", -1);
    } وإلا {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_عدد(sb, dest_شاخص);
    sb.append(u"..", -1);
    ICU_Utility::append_عدد(sb, dest_شاخص + new_درازی_2);
    إذا (تغییریافته) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_عدد(sb, repl_شاخص);
        sb.append(u"..", -1);
        ICU_Utility::append_عدد(sb, repl_شاخص + new_درازی_2);
        sb.append(u"] }", -1);
    } وإلا {
        sb.append(u"] (no-change) }", -1);
    }
    أرجع sb;
}

U_NAMESPACE_انتها
