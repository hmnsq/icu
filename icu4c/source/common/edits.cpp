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
ثابت int32_t MAX_UNCHANGED_اوږدتیا = 0x1000;
ثابت int32_t MAX_UNCHANGED = MAX_UNCHANGED_اوږدتیا - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ثابت int32_t MAX_SHORT_CHANGE_OLD_اوږدتیا = 6;
ثابت int32_t MAX_SHORT_CHANGE_NEW_اوږدتیا = 7;
ثابت int32_t SHORT_CHANGE_عدد_ماسک = 0x1ff;
ثابت int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ثابت int32_t اوږدتیا_دننه_1_TRAIL = 61;
ثابت int32_t اوږدتیا_دننه_2_TRAIL = 62;

}  // namespace

فراغ Edits::releaseArray() noexcept {
    إذا (array != ډېری_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ثابت Edits& other) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ_2)) {
        length = delta = عدد_Changes = 0;
        أرجع *هذا;
    }
    إذا (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        إذا (newArray == تش_اشاره) {
            length = delta = عدد_Changes = 0;
            error_کوډ_2 = U_حافظه_ویش_ERROR;
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
    إذا (U_FAILURE(error_کوډ_2)) {
        length = delta = عدد_Changes = 0;
        أرجع *هذا;
    }
    releaseArray();
    إذا (length > ډېری_ظرفیت_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ډېری_Array;
        src.capacity = ډېری_ظرفیت_2;
        src.reset();
        أرجع *هذا;
    }
    array = ډېری_Array;
    capacity = ډېری_ظرفیت_2;
    إذا (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    أرجع *هذا;
}

Edits& Edits::چلوونکی=(ثابت Edits& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) { أرجع *هذا; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    عدد_Changes = other.عدد_Changes;
    error_کوډ_2 = other.error_کوډ_2;
    أرجع copyArray(other);
}

Edits& Edits::چلوونکی=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    عدد_Changes = src.عدد_Changes;
    error_کوډ_2 = src.error_کوډ_2;
    أرجع moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

فراغ Edits::reset() noexcept {
    length = delta = عدد_Changes = 0;
    error_کوډ_2 = U_ZERO_ERROR;
}

فراغ Edits::جمع_کړه_Unchanged(int32_t unchanged_اوږدتیا) {
    إذا(U_FAILURE(error_کوډ_2) || unchanged_اوږدتیا == 0) { أرجع; }
    إذا(unchanged_اوږدتیا < 0) {
        error_کوډ_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t وروستی = وروستی_Unit();
    إذا(وروستی < MAX_UNCHANGED) {
        int32_t پاتې_شوی = MAX_UNCHANGED - وروستی;
        إذا (پاتې_شوی >= unchanged_اوږدتیا) {
            سېټ_وروستی_Unit(وروستی + unchanged_اوږدتیا);
            أرجع;
        }
        سېټ_وروستی_Unit(MAX_UNCHANGED);
        unchanged_اوږدتیا -= پاتې_شوی;
    }
    // Split large lengths into multiple units.
    طالما(unchanged_اوږدتیا >= MAX_UNCHANGED_اوږدتیا) {
        append(MAX_UNCHANGED);
        unchanged_اوږدتیا -= MAX_UNCHANGED_اوږدتیا;
    }
    // Write a small (remaining) length.
    إذا(unchanged_اوږدتیا > 0) {
        append(unchanged_اوږدتیا - 1);
    }
}

فراغ Edits::جمع_کړه_Replace(int32_t old_اوږدتیا, int32_t new_اوږدتیا) {
    إذا(U_FAILURE(error_کوډ_2)) { أرجع; }
    إذا(old_اوږدتیا < 0 || new_اوږدتیا < 0) {
        error_کوډ_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (old_اوږدتیا == 0 && new_اوږدتیا == 0) {
        أرجع;
    }
    ++عدد_Changes;
    int32_t newDelta = new_اوږدتیا - old_اوږدتیا;
    إذا (newDelta != 0) {
        إذا ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_کوډ_2 = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        delta += newDelta;
    }

    إذا(0 < old_اوږدتیا && old_اوږدتیا <= MAX_SHORT_CHANGE_OLD_اوږدتیا &&
            new_اوږدتیا <= MAX_SHORT_CHANGE_NEW_اوږدتیا) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_اوږدتیا << 12) | (new_اوږدتیا << 9);
        int32_t وروستی = وروستی_Unit();
        إذا(MAX_UNCHANGED < وروستی && وروستی < MAX_SHORT_CHANGE &&
                (وروستی & ~SHORT_CHANGE_عدد_ماسک) == u &&
                (وروستی & SHORT_CHANGE_عدد_ماسک) < SHORT_CHANGE_عدد_ماسک) {
            سېټ_وروستی_Unit(وروستی + 1);
            أرجع;
        }
        append(u);
        أرجع;
    }

    int32_t سر = 0x7000;
    إذا (old_اوږدتیا < اوږدتیا_دننه_1_TRAIL && new_اوږدتیا < اوږدتیا_دننه_1_TRAIL) {
        سر |= old_اوږدتیا << 6;
        سر |= new_اوږدتیا;
        append(سر);
    } وإلا إذا ((capacity - length) >= 5 || growArray()) {
        int32_t حد = length + 1;
        إذا(old_اوږدتیا < اوږدتیا_دننه_1_TRAIL) {
            سر |= old_اوږدتیا << 6;
        } وإلا إذا(old_اوږدتیا <= 0x7fff) {
            سر |= اوږدتیا_دننه_1_TRAIL << 6;
            array[حد++] = static_cast<uint16_t>(0x8000 | old_اوږدتیا);
        } وإلا {
            سر |= (اوږدتیا_دننه_2_TRAIL + (old_اوږدتیا >> 30)) << 6;
            array[حد++] = static_cast<uint16_t>(0x8000 | (old_اوږدتیا >> 15));
            array[حد++] = static_cast<uint16_t>(0x8000 | old_اوږدتیا);
        }
        إذا(new_اوږدتیا < اوږدتیا_دننه_1_TRAIL) {
            سر |= new_اوږدتیا;
        } وإلا إذا(new_اوږدتیا <= 0x7fff) {
            سر |= اوږدتیا_دننه_1_TRAIL;
            array[حد++] = static_cast<uint16_t>(0x8000 | new_اوږدتیا);
        } وإلا {
            سر |= اوږدتیا_دننه_2_TRAIL + (new_اوږدتیا >> 30);
            array[حد++] = static_cast<uint16_t>(0x8000 | (new_اوږدتیا >> 15));
            array[حد++] = static_cast<uint16_t>(0x8000 | new_اوږدتیا);
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
    إذا (array == ډېری_Array) {
        new_ظرفیت = 2000;
    } وإلا إذا (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_کوډ_2 = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    } وإلا إذا (capacity >= (INT32_MAX / 2)) {
        new_ظرفیت = INT32_MAX;
    } وإلا {
        new_ظرفیت = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    إذا ((new_ظرفیت - capacity) < 5) {
        error_کوډ_2 = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_ظرفیت) * 2));
    إذا (newArray == تش_اشاره) {
        error_کوډ_2 = U_حافظه_ویش_ERROR;
        أرجع خطأ;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_ظرفیت;
    أرجع ریښتیا;
}

UBool Edits::copy_Error_منزل(UError_کوډ &بهر_Error_کوډ) ثابت {
    إذا (U_FAILURE(بهر_Error_کوډ)) { أرجع ریښتیا; }
    إذا (U_SUCCESS(error_کوډ_2)) { أرجع خطأ; }
    بهر_Error_کوډ = error_کوډ_2;
    أرجع ریښتیا;
}

Edits& Edits::یوځای_And_Append(ثابت Edits& ab,
                             ثابت Edits& bc,
                             UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    إذا (copy_Error_منزل(error_کوډ)) { أرجع *هذا; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_بل = ریښتیا, bc_Has_بل = ریښتیا;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_اوږدتیا = 0, ab_b_اوږدتیا = 0, bc_b_اوږدتیا = 0, c_اوږدتیا = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_اوږدتیا = 0, pending_c_اوږدتیا = 0;
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
        إذا (bc_b_اوږدتیا == 0) {
            إذا (bc_Has_بل && (bc_Has_بل = bcIter.next(error_کوډ)) != 0) {
                bc_b_اوږدتیا = bcIter.old_اوږدتیا();
                c_اوږدتیا = bcIter.new_اوږدتیا();
                إذا (bc_b_اوږدتیا == 0) {
                    // insertion
                    إذا (ab_b_اوږدتیا == 0 || !abIter.hasChange()) {
                        جمع_کړه_Replace(pending_a_اوږدتیا, pending_c_اوږدتیا + c_اوږدتیا);
                        pending_a_اوږدتیا = pending_c_اوږدتیا = 0;
                    } وإلا {
                        pending_c_اوږدتیا += c_اوږدتیا;
                    }
                    استمر;
                }
            }
            // else see if the other iterator is done, too.
        }
        إذا (ab_b_اوږدتیا == 0) {
            إذا (ab_Has_بل && (ab_Has_بل = abIter.next(error_کوډ)) != 0) {
                a_اوږدتیا = abIter.old_اوږدتیا();
                ab_b_اوږدتیا = abIter.new_اوږدتیا();
                إذا (ab_b_اوږدتیا == 0) {
                    // deletion
                    إذا (bc_b_اوږدتیا == bcIter.old_اوږدتیا() || !bcIter.hasChange()) {
                        جمع_کړه_Replace(pending_a_اوږدتیا + a_اوږدتیا, pending_c_اوږدتیا);
                        pending_a_اوږدتیا = pending_c_اوږدتیا = 0;
                    } وإلا {
                        pending_a_اوږدتیا += a_اوږدتیا;
                    }
                    استمر;
                }
            } وإلا إذا (bc_b_اوږدتیا == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                اكسر;
            } وإلا {
                // The ab output string is shorter than the bc input string.
                إذا (!copy_Error_منزل(error_کوډ)) {
                    error_کوډ = U_ILLEGAL_دلیل_ERROR;
                }
                أرجع *هذا;
            }
        }
        إذا (bc_b_اوږدتیا == 0) {
            // The bc input string is shorter than the ab output string.
            إذا (!copy_Error_منزل(error_کوډ)) {
                error_کوډ = U_ILLEGAL_دلیل_ERROR;
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
            إذا (pending_a_اوږدتیا != 0 || pending_c_اوږدتیا != 0) {
                جمع_کړه_Replace(pending_a_اوږدتیا, pending_c_اوږدتیا);
                pending_a_اوږدتیا = pending_c_اوږدتیا = 0;
            }
            int32_t unchanged_اوږدتیا = a_اوږدتیا <= c_اوږدتیا ? a_اوږدتیا : c_اوږدتیا;
            جمع_کړه_Unchanged(unchanged_اوږدتیا);
            ab_b_اوږدتیا = a_اوږدتیا -= unchanged_اوږدتیا;
            bc_b_اوږدتیا = c_اوږدتیا -= unchanged_اوږدتیا;
            // At least one of the unchanged spans is now empty.
            استمر;
        }
        إذا (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            إذا (ab_b_اوږدتیا >= bc_b_اوږدتیا) {
                // Split the longer unchanged span into change + remainder.
                جمع_کړه_Replace(pending_a_اوږدتیا + bc_b_اوږدتیا, pending_c_اوږدتیا + c_اوږدتیا);
                pending_a_اوږدتیا = pending_c_اوږدتیا = 0;
                a_اوږدتیا = ab_b_اوږدتیا -= bc_b_اوږدتیا;
                bc_b_اوږدتیا = 0;
                استمر;
            }
            // Handle the shorter unchanged span below like a change.
        } وإلا إذا (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            إذا (ab_b_اوږدتیا <= bc_b_اوږدتیا) {
                // Split the longer unchanged span into change + remainder.
                جمع_کړه_Replace(pending_a_اوږدتیا + a_اوږدتیا, pending_c_اوږدتیا + ab_b_اوږدتیا);
                pending_a_اوږدتیا = pending_c_اوږدتیا = 0;
                c_اوږدتیا = bc_b_اوږدتیا -= ab_b_اوږدتیا;
                ab_b_اوږدتیا = 0;
                استمر;
            }
            // Handle the shorter unchanged span below like a change.
        } وإلا {  // both abIter.hasChange() && bcIter.hasChange()
            إذا (ab_b_اوږدتیا == bc_b_اوږدتیا) {
                // Changes on both sides up to the same position. Emit & reset.
                جمع_کړه_Replace(pending_a_اوږدتیا + a_اوږدتیا, pending_c_اوږدتیا + c_اوږدتیا);
                pending_a_اوږدتیا = pending_c_اوږدتیا = 0;
                ab_b_اوږدتیا = bc_b_اوږدتیا = 0;
                استمر;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_اوږدتیا += a_اوږدتیا;
        pending_c_اوږدتیا += c_اوږدتیا;
        إذا (ab_b_اوږدتیا < bc_b_اوږدتیا) {
            bc_b_اوږدتیا -= ab_b_اوږدتیا;
            c_اوږدتیا = ab_b_اوږدتیا = 0;
        } وإلا {  // ab_bLength > bc_bLength
            ab_b_اوږدتیا -= bc_b_اوږدتیا;
            a_اوږدتیا = bc_b_اوږدتیا = 0;
        }
    }
    إذا (pending_a_اوږدتیا != 0 || pending_c_اوږدتیا != 0) {
        جمع_کړه_Replace(pending_a_اوږدتیا, pending_c_اوږدتیا);
    }
    copy_Error_منزل(error_کوډ);
    أرجع *هذا;
}

Edits::Iterator::Iterator(ثابت uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), شاخص(0), length(len), پاتې_شوی(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), بدل_شوی(خطأ), old_اوږدتیا_2(0), new_اوږدتیا_2(0),
        src_شاخص(0), repl_شاخص(0), dest_شاخص(0) {}

int32_t Edits::Iterator::read_اوږدتیا(int32_t سر) {
    إذا (سر < اوږدتیا_دننه_1_TRAIL) {
        أرجع سر;
    } وإلا إذا (سر < اوږدتیا_دننه_2_TRAIL) {
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

فراغ Edits::Iterator::update_بل_Indexes() {
    src_شاخص += old_اوږدتیا_2;
    إذا (بدل_شوی) {
        repl_شاخص += new_اوږدتیا_2;
    }
    dest_شاخص += new_اوږدتیا_2;
}

فراغ Edits::Iterator::update_پخوانی_Indexes() {
    src_شاخص -= old_اوږدتیا_2;
    إذا (بدل_شوی) {
        repl_شاخص -= new_اوږدتیا_2;
    }
    dest_شاخص -= new_اوږدتیا_2;
}

UBool Edits::Iterator::no_بل() {
    // No change before or beyond the string.
    dir = 0;
    بدل_شوی = خطأ;
    old_اوږدتیا_2 = new_اوږدتیا_2 = 0;
    أرجع خطأ;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_کوډ &error_کوډ) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    إذا (dir > 0) {
        update_بل_Indexes();
    } وإلا {
        إذا (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            إذا (پاتې_شوی > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++شاخص;  // next() rests on the index after the sequence unit.
                dir = 1;
                أرجع ریښتیا;
            }
        }
        dir = 1;
    }
    إذا (پاتې_شوی >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        إذا (پاتې_شوی > 1) {
            --پاتې_شوی;
            أرجع ریښتیا;
        }
        پاتې_شوی = 0;
    }
    إذا (شاخص >= length) {
        أرجع no_بل();
    }
    int32_t u = array[شاخص++];
    إذا (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        بدل_شوی = خطأ;
        old_اوږدتیا_2 = u + 1;
        طالما (شاخص < length && (u = array[شاخص]) <= MAX_UNCHANGED) {
            ++شاخص;
            old_اوږدتیا_2 += u + 1;
        }
        new_اوږدتیا_2 = old_اوږدتیا_2;
        إذا (onlyChanges) {
            update_بل_Indexes();
            إذا (شاخص >= length) {
                أرجع no_بل();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++شاخص;
        } وإلا {
            أرجع ریښتیا;
        }
    }
    بدل_شوی = ریښتیا;
    إذا (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_اوږدتیا;
        int32_t num = (u & SHORT_CHANGE_عدد_ماسک) + 1;
        إذا (coarse) {
            old_اوږدتیا_2 = num * oldLen;
            new_اوږدتیا_2 = num * newLen;
        } وإلا {
            // Split a sequence of changes that was compressed into one unit.
            old_اوږدتیا_2 = oldLen;
            new_اوږدتیا_2 = newLen;
            إذا (num > 1) {
                پاتې_شوی = num;  // This is the first of two or more changes.
            }
            أرجع ریښتیا;
        }
    } وإلا {
        U_ASSERT(u <= 0x7fff);
        old_اوږدتیا_2 = read_اوږدتیا((u >> 6) & 0x3f);
        new_اوږدتیا_2 = read_اوږدتیا(u & 0x3f);
        إذا (!coarse) {
            أرجع ریښتیا;
        }
    }
    // Combine adjacent changes.
    طالما (شاخص < length && (u = array[شاخص]) > MAX_UNCHANGED) {
        ++شاخص;
        إذا (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_عدد_ماسک) + 1;
            old_اوږدتیا_2 += (u >> 12) * num;
            new_اوږدتیا_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_اوږدتیا) * num;
        } وإلا {
            U_ASSERT(u <= 0x7fff);
            old_اوږدتیا_2 += read_اوږدتیا((u >> 6) & 0x3f);
            new_اوږدتیا_2 += read_اوږدتیا(u & 0x3f);
        }
    }
    أرجع ریښتیا;
}

UBool Edits::Iterator::پخوانی(UError_کوډ &error_کوډ) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    إذا (dir >= 0) {
        إذا (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            إذا (پاتې_شوی > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --شاخص;  // previous() rests on the sequence unit.
                dir = -1;
                أرجع ریښتیا;
            }
            update_بل_Indexes();
        }
        dir = -1;
    }
    إذا (پاتې_شوی > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[شاخص];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        إذا (پاتې_شوی <= (u & SHORT_CHANGE_عدد_ماسک)) {
            ++پاتې_شوی;
            update_پخوانی_Indexes();
            أرجع ریښتیا;
        }
        پاتې_شوی = 0;
    }
    إذا (شاخص <= 0) {
        أرجع no_بل();
    }
    int32_t u = array[--شاخص];
    إذا (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        بدل_شوی = خطأ;
        old_اوږدتیا_2 = u + 1;
        طالما (شاخص > 0 && (u = array[شاخص - 1]) <= MAX_UNCHANGED) {
            --شاخص;
            old_اوږدتیا_2 += u + 1;
        }
        new_اوږدتیا_2 = old_اوږدتیا_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        update_پخوانی_Indexes();
        أرجع ریښتیا;
    }
    بدل_شوی = ریښتیا;
    إذا (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_اوږدتیا;
        int32_t num = (u & SHORT_CHANGE_عدد_ماسک) + 1;
        إذا (coarse) {
            old_اوږدتیا_2 = num * oldLen;
            new_اوږدتیا_2 = num * newLen;
        } وإلا {
            // Split a sequence of changes that was compressed into one unit.
            old_اوږدتیا_2 = oldLen;
            new_اوږدتیا_2 = newLen;
            إذا (num > 1) {
                پاتې_شوی = 1;  // This is the last of two or more changes.
            }
            update_پخوانی_Indexes();
            أرجع ریښتیا;
        }
    } وإلا {
        إذا (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_اوږدتیا_2 = read_اوږدتیا((u >> 6) & 0x3f);
            new_اوږدتیا_2 = read_اوږدتیا(u & 0x3f);
        } وإلا {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(شاخص > 0);
            طالما ((u = array[--شاخص]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t سر_شاخص = شاخص++;
            old_اوږدتیا_2 = read_اوږدتیا((u >> 6) & 0x3f);
            new_اوږدتیا_2 = read_اوږدتیا(u & 0x3f);
            شاخص = سر_شاخص;
        }
        إذا (!coarse) {
            update_پخوانی_Indexes();
            أرجع ریښتیا;
        }
    }
    // Combine adjacent changes.
    طالما (شاخص > 0 && (u = array[شاخص - 1]) > MAX_UNCHANGED) {
        --شاخص;
        إذا (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_عدد_ماسک) + 1;
            old_اوږدتیا_2 += (u >> 12) * num;
            new_اوږدتیا_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_اوږدتیا) * num;
        } وإلا إذا (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t سر_شاخص = شاخص++;
            old_اوږدتیا_2 += read_اوږدتیا((u >> 6) & 0x3f);
            new_اوږدتیا_2 += read_اوږدتیا(u & 0x3f);
            شاخص = سر_شاخص;
        }
    }
    update_پخوانی_Indexes();
    أرجع ریښتیا;
}

int32_t Edits::Iterator::پیدا_کړه_شاخص(int32_t i, UBool پیدا_کړه_سرچینه, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ) || i < 0) { أرجع -1; }
    int32_t پراختیا_پیل, پراختیا_اوږدتیا;
    إذا (پیدا_کړه_سرچینه) {  // find source index
        پراختیا_پیل = src_شاخص;
        پراختیا_اوږدتیا = old_اوږدتیا_2;
    } وإلا {  // find destination index
        پراختیا_پیل = dest_شاخص;
        پراختیا_اوږدتیا = new_اوږدتیا_2;
    }
    إذا (i < پراختیا_پیل) {
        إذا (i >= (پراختیا_پیل / 2)) {
            // Search backwards.
            لكل (;;) {
                UBool has_پخوانی = پخوانی(error_کوډ);
                U_ASSERT(has_پخوانی);  // because i>=0 and the first span starts at 0
                (فراغ)has_پخوانی;  // avoid unused-variable warning
                پراختیا_پیل = پیدا_کړه_سرچینه ? src_شاخص : dest_شاخص;
                إذا (i >= پراختیا_پیل) {
                    // The index is in the current span.
                    أرجع 0;
                }
                إذا (پاتې_شوی > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    پراختیا_اوږدتیا = پیدا_کړه_سرچینه ? old_اوږدتیا_2 : new_اوږدتیا_2;
                    int32_t u = array[شاخص];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_عدد_ماسک) + 1 - پاتې_شوی;
                    int32_t len = num * پراختیا_اوږدتیا;
                    إذا (i >= (پراختیا_پیل - len)) {
                        int32_t n = ((پراختیا_پیل - i - 1) / پراختیا_اوږدتیا) + 1;
                        // 1 <= n <= num
                        src_شاخص -= n * old_اوږدتیا_2;
                        repl_شاخص -= n * new_اوږدتیا_2;
                        dest_شاخص -= n * new_اوږدتیا_2;
                        پاتې_شوی += n;
                        أرجع 0;
                    }
                    // Skip all of these edits at once.
                    src_شاخص -= num * old_اوږدتیا_2;
                    repl_شاخص -= num * new_اوږدتیا_2;
                    dest_شاخص -= num * new_اوږدتیا_2;
                    پاتې_شوی = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        شاخص = پاتې_شوی = old_اوږدتیا_2 = new_اوږدتیا_2 = src_شاخص = repl_شاخص = dest_شاخص = 0;
    } وإلا إذا (i < (پراختیا_پیل + پراختیا_اوږدتیا)) {
        // The index is in the current span.
        أرجع 0;
    }
    طالما (next(خطأ, error_کوډ)) {
        إذا (پیدا_کړه_سرچینه) {
            پراختیا_پیل = src_شاخص;
            پراختیا_اوږدتیا = old_اوږدتیا_2;
        } وإلا {
            پراختیا_پیل = dest_شاخص;
            پراختیا_اوږدتیا = new_اوږدتیا_2;
        }
        إذا (i < (پراختیا_پیل + پراختیا_اوږدتیا)) {
            // The index is in the current span.
            أرجع 0;
        }
        إذا (پاتې_شوی > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = پاتې_شوی * پراختیا_اوږدتیا;
            إذا (i < (پراختیا_پیل + len)) {
                int32_t n = (i - پراختیا_پیل) / پراختیا_اوږدتیا;  // 1 <= n <= remaining - 1
                src_شاخص += n * old_اوږدتیا_2;
                repl_شاخص += n * new_اوږدتیا_2;
                dest_شاخص += n * new_اوږدتیا_2;
                پاتې_شوی -= n;
                أرجع 0;
            }
            // Make next() skip all of these edits at once.
            old_اوږدتیا_2 *= پاتې_شوی;
            new_اوږدتیا_2 *= پاتې_شوی;
            پاتې_شوی = 0;
        }
    }
    أرجع 1;
}

int32_t Edits::Iterator::destination_شاخص_From_سرچینه_شاخص(int32_t i, UError_کوډ &error_کوډ) {
    int32_t where = پیدا_کړه_شاخص(i, ریښتیا, error_کوډ);
    إذا (where < 0) {
        // Error or before the string.
        أرجع 0;
    }
    إذا (where > 0 || i == src_شاخص) {
        // At or after string length, or at start of the found span.
        أرجع dest_شاخص;
    }
    إذا (بدل_شوی) {
        // In a change span, map to its end.
        أرجع dest_شاخص + new_اوږدتیا_2;
    } وإلا {
        // In an unchanged span, offset 1:1 within it.
        أرجع dest_شاخص + (i - src_شاخص);
    }
}

int32_t Edits::Iterator::سرچینه_شاخص_From_Destination_شاخص(int32_t i, UError_کوډ &error_کوډ) {
    int32_t where = پیدا_کړه_شاخص(i, خطأ, error_کوډ);
    إذا (where < 0) {
        // Error or before the string.
        أرجع 0;
    }
    إذا (where > 0 || i == dest_شاخص) {
        // At or after string length, or at start of the found span.
        أرجع src_شاخص;
    }
    إذا (بدل_شوی) {
        // In a change span, map to its end.
        أرجع src_شاخص + old_اوږدتیا_2;
    } وإلا {
        // In an unchanged span, offset within it.
        أرجع src_شاخص + (i - dest_شاخص);
    }
}

UnicodeString& Edits::Iterator::منزل_String(UnicodeString& sb U_LIFETIME_BOUND) ثابت {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_شمېره(sb, src_شاخص);
    sb.append(u"..", -1);
    ICU_Utility::append_شمېره(sb, src_شاخص + old_اوږدتیا_2);
    إذا (بدل_شوی) {
        sb.append(u"] ⇝ dest[", -1);
    } وإلا {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_شمېره(sb, dest_شاخص);
    sb.append(u"..", -1);
    ICU_Utility::append_شمېره(sb, dest_شاخص + new_اوږدتیا_2);
    إذا (بدل_شوی) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_شمېره(sb, repl_شاخص);
        sb.append(u"..", -1);
        ICU_Utility::append_شمېره(sb, repl_شاخص + new_اوږدتیا_2);
        sb.append(u"] }", -1);
    } وإلا {
        sb.append(u"] (no-change) }", -1);
    }
    أرجع sb;
}

U_NAMESPACE_پای_ټکی
