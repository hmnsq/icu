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
ثابت int32_t MAX_UNCHANGED_لمائی = 0x1000;
ثابت int32_t MAX_UNCHANGED = MAX_UNCHANGED_لمائی - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ثابت int32_t MAX_SHORT_CHANGE_OLD_لمائی = 6;
ثابت int32_t MAX_SHORT_CHANGE_NEW_لمائی = 7;
ثابت int32_t SHORT_CHANGE_عدد_نقاب = 0x1ff;
ثابت int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ثابت int32_t لمائی_وچ_1_TRAIL = 61;
ثابت int32_t لمائی_وچ_2_TRAIL = 62;

}  // namespace

فراغ Edits::releaseArray() noexcept {
    إذا (array != ڈھیر_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ثابت Edits& other) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ_2)) {
        length = delta = عدد_Changes = 0;
        أرجع *هذا;
    }
    إذا (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        إذا (newArray == خالی_سوچک) {
            length = delta = عدد_Changes = 0;
            error_کوڈ_2 = U_حافظہ_ونڈ_ERROR;
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
    إذا (U_FAILURE(error_کوڈ_2)) {
        length = delta = عدد_Changes = 0;
        أرجع *هذا;
    }
    releaseArray();
    إذا (length > ڈھیر_سمرتھا_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ڈھیر_Array;
        src.capacity = ڈھیر_سمرتھا_2;
        src.reset();
        أرجع *هذا;
    }
    array = ڈھیر_Array;
    capacity = ڈھیر_سمرتھا_2;
    إذا (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    أرجع *هذا;
}

Edits& Edits::عامل=(ثابت Edits& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) { أرجع *هذا; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    عدد_Changes = other.عدد_Changes;
    error_کوڈ_2 = other.error_کوڈ_2;
    أرجع copyArray(other);
}

Edits& Edits::عامل=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    عدد_Changes = src.عدد_Changes;
    error_کوڈ_2 = src.error_کوڈ_2;
    أرجع moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

فراغ Edits::reset() noexcept {
    length = delta = عدد_Changes = 0;
    error_کوڈ_2 = U_ZERO_ERROR;
}

فراغ Edits::جوڑو_Unchanged(int32_t unchanged_لمائی) {
    إذا(U_FAILURE(error_کوڈ_2) || unchanged_لمائی == 0) { أرجع; }
    إذا(unchanged_لمائی < 0) {
        error_کوڈ_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t آخری = آخری_Unit();
    إذا(آخری < MAX_UNCHANGED) {
        int32_t بچیا = MAX_UNCHANGED - آخری;
        إذا (بچیا >= unchanged_لمائی) {
            سیٹ_آخری_Unit(آخری + unchanged_لمائی);
            أرجع;
        }
        سیٹ_آخری_Unit(MAX_UNCHANGED);
        unchanged_لمائی -= بچیا;
    }
    // Split large lengths into multiple units.
    طالما(unchanged_لمائی >= MAX_UNCHANGED_لمائی) {
        append(MAX_UNCHANGED);
        unchanged_لمائی -= MAX_UNCHANGED_لمائی;
    }
    // Write a small (remaining) length.
    إذا(unchanged_لمائی > 0) {
        append(unchanged_لمائی - 1);
    }
}

فراغ Edits::جوڑو_Replace(int32_t old_لمائی, int32_t new_لمائی) {
    إذا(U_FAILURE(error_کوڈ_2)) { أرجع; }
    إذا(old_لمائی < 0 || new_لمائی < 0) {
        error_کوڈ_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (old_لمائی == 0 && new_لمائی == 0) {
        أرجع;
    }
    ++عدد_Changes;
    int32_t newDelta = new_لمائی - old_لمائی;
    إذا (newDelta != 0) {
        إذا ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_کوڈ_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        delta += newDelta;
    }

    إذا(0 < old_لمائی && old_لمائی <= MAX_SHORT_CHANGE_OLD_لمائی &&
            new_لمائی <= MAX_SHORT_CHANGE_NEW_لمائی) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_لمائی << 12) | (new_لمائی << 9);
        int32_t آخری = آخری_Unit();
        إذا(MAX_UNCHANGED < آخری && آخری < MAX_SHORT_CHANGE &&
                (آخری & ~SHORT_CHANGE_عدد_نقاب) == u &&
                (آخری & SHORT_CHANGE_عدد_نقاب) < SHORT_CHANGE_عدد_نقاب) {
            سیٹ_آخری_Unit(آخری + 1);
            أرجع;
        }
        append(u);
        أرجع;
    }

    int32_t سر = 0x7000;
    إذا (old_لمائی < لمائی_وچ_1_TRAIL && new_لمائی < لمائی_وچ_1_TRAIL) {
        سر |= old_لمائی << 6;
        سر |= new_لمائی;
        append(سر);
    } وإلا إذا ((capacity - length) >= 5 || growArray()) {
        int32_t حد_بندی = length + 1;
        إذا(old_لمائی < لمائی_وچ_1_TRAIL) {
            سر |= old_لمائی << 6;
        } وإلا إذا(old_لمائی <= 0x7fff) {
            سر |= لمائی_وچ_1_TRAIL << 6;
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | old_لمائی);
        } وإلا {
            سر |= (لمائی_وچ_2_TRAIL + (old_لمائی >> 30)) << 6;
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | (old_لمائی >> 15));
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | old_لمائی);
        }
        إذا(new_لمائی < لمائی_وچ_1_TRAIL) {
            سر |= new_لمائی;
        } وإلا إذا(new_لمائی <= 0x7fff) {
            سر |= لمائی_وچ_1_TRAIL;
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | new_لمائی);
        } وإلا {
            سر |= لمائی_وچ_2_TRAIL + (new_لمائی >> 30);
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | (new_لمائی >> 15));
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | new_لمائی);
        }
        array[length] = static_cast<uint16_t>(سر);
        length = حد_بندی;
    }
}

فراغ Edits::append(int32_t r) {
    إذا(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t new_سمرتھا;
    إذا (array == ڈھیر_Array) {
        new_سمرتھا = 2000;
    } وإلا إذا (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_کوڈ_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    } وإلا إذا (capacity >= (INT32_MAX / 2)) {
        new_سمرتھا = INT32_MAX;
    } وإلا {
        new_سمرتھا = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    إذا ((new_سمرتھا - capacity) < 5) {
        error_کوڈ_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_سمرتھا) * 2));
    إذا (newArray == خالی_سوچک) {
        error_کوڈ_2 = U_حافظہ_ونڈ_ERROR;
        أرجع خطأ;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_سمرتھا;
    أرجع سچ;
}

UBool Edits::copy_Error_منزل(UError_کوڈ &باہرلا_Error_کوڈ) ثابت {
    إذا (U_FAILURE(باہرلا_Error_کوڈ)) { أرجع سچ; }
    إذا (U_SUCCESS(error_کوڈ_2)) { أرجع خطأ; }
    باہرلا_Error_کوڈ = error_کوڈ_2;
    أرجع سچ;
}

Edits& Edits::ملاؤ_And_Append(ثابت Edits& ab,
                             ثابت Edits& bc,
                             UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    إذا (copy_Error_منزل(error_کوڈ)) { أرجع *هذا; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_اگلا = سچ, bc_Has_اگلا = سچ;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_لمائی = 0, ab_b_لمائی = 0, bc_b_لمائی = 0, c_لمائی = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_لمائی = 0, pending_c_لمائی = 0;
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
        إذا (bc_b_لمائی == 0) {
            إذا (bc_Has_اگلا && (bc_Has_اگلا = bcIter.next(error_کوڈ)) != 0) {
                bc_b_لمائی = bcIter.old_لمائی();
                c_لمائی = bcIter.new_لمائی();
                إذا (bc_b_لمائی == 0) {
                    // insertion
                    إذا (ab_b_لمائی == 0 || !abIter.hasChange()) {
                        جوڑو_Replace(pending_a_لمائی, pending_c_لمائی + c_لمائی);
                        pending_a_لمائی = pending_c_لمائی = 0;
                    } وإلا {
                        pending_c_لمائی += c_لمائی;
                    }
                    استمر;
                }
            }
            // else see if the other iterator is done, too.
        }
        إذا (ab_b_لمائی == 0) {
            إذا (ab_Has_اگلا && (ab_Has_اگلا = abIter.next(error_کوڈ)) != 0) {
                a_لمائی = abIter.old_لمائی();
                ab_b_لمائی = abIter.new_لمائی();
                إذا (ab_b_لمائی == 0) {
                    // deletion
                    إذا (bc_b_لمائی == bcIter.old_لمائی() || !bcIter.hasChange()) {
                        جوڑو_Replace(pending_a_لمائی + a_لمائی, pending_c_لمائی);
                        pending_a_لمائی = pending_c_لمائی = 0;
                    } وإلا {
                        pending_a_لمائی += a_لمائی;
                    }
                    استمر;
                }
            } وإلا إذا (bc_b_لمائی == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                اكسر;
            } وإلا {
                // The ab output string is shorter than the bc input string.
                إذا (!copy_Error_منزل(error_کوڈ)) {
                    error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                }
                أرجع *هذا;
            }
        }
        إذا (bc_b_لمائی == 0) {
            // The bc input string is shorter than the ab output string.
            إذا (!copy_Error_منزل(error_کوڈ)) {
                error_کوڈ = U_ILLEGAL_دلیل_ERROR;
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
            إذا (pending_a_لمائی != 0 || pending_c_لمائی != 0) {
                جوڑو_Replace(pending_a_لمائی, pending_c_لمائی);
                pending_a_لمائی = pending_c_لمائی = 0;
            }
            int32_t unchanged_لمائی = a_لمائی <= c_لمائی ? a_لمائی : c_لمائی;
            جوڑو_Unchanged(unchanged_لمائی);
            ab_b_لمائی = a_لمائی -= unchanged_لمائی;
            bc_b_لمائی = c_لمائی -= unchanged_لمائی;
            // At least one of the unchanged spans is now empty.
            استمر;
        }
        إذا (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            إذا (ab_b_لمائی >= bc_b_لمائی) {
                // Split the longer unchanged span into change + remainder.
                جوڑو_Replace(pending_a_لمائی + bc_b_لمائی, pending_c_لمائی + c_لمائی);
                pending_a_لمائی = pending_c_لمائی = 0;
                a_لمائی = ab_b_لمائی -= bc_b_لمائی;
                bc_b_لمائی = 0;
                استمر;
            }
            // Handle the shorter unchanged span below like a change.
        } وإلا إذا (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            إذا (ab_b_لمائی <= bc_b_لمائی) {
                // Split the longer unchanged span into change + remainder.
                جوڑو_Replace(pending_a_لمائی + a_لمائی, pending_c_لمائی + ab_b_لمائی);
                pending_a_لمائی = pending_c_لمائی = 0;
                c_لمائی = bc_b_لمائی -= ab_b_لمائی;
                ab_b_لمائی = 0;
                استمر;
            }
            // Handle the shorter unchanged span below like a change.
        } وإلا {  // both abIter.hasChange() && bcIter.hasChange()
            إذا (ab_b_لمائی == bc_b_لمائی) {
                // Changes on both sides up to the same position. Emit & reset.
                جوڑو_Replace(pending_a_لمائی + a_لمائی, pending_c_لمائی + c_لمائی);
                pending_a_لمائی = pending_c_لمائی = 0;
                ab_b_لمائی = bc_b_لمائی = 0;
                استمر;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_لمائی += a_لمائی;
        pending_c_لمائی += c_لمائی;
        إذا (ab_b_لمائی < bc_b_لمائی) {
            bc_b_لمائی -= ab_b_لمائی;
            c_لمائی = ab_b_لمائی = 0;
        } وإلا {  // ab_bLength > bc_bLength
            ab_b_لمائی -= bc_b_لمائی;
            a_لمائی = bc_b_لمائی = 0;
        }
    }
    إذا (pending_a_لمائی != 0 || pending_c_لمائی != 0) {
        جوڑو_Replace(pending_a_لمائی, pending_c_لمائی);
    }
    copy_Error_منزل(error_کوڈ);
    أرجع *هذا;
}

Edits::Iterator::Iterator(ثابت uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), اشاریہ(0), length(len), بچیا(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), بدلیا(خطأ), old_لمائی_2(0), new_لمائی_2(0),
        src_اشاریہ(0), repl_اشاریہ(0), dest_اشاریہ(0) {}

int32_t Edits::Iterator::read_لمائی(int32_t سر) {
    إذا (سر < لمائی_وچ_1_TRAIL) {
        أرجع سر;
    } وإلا إذا (سر < لمائی_وچ_2_TRAIL) {
        U_ASSERT(اشاریہ < length);
        U_ASSERT(array[اشاریہ] >= 0x8000);
        أرجع array[اشاریہ++] & 0x7fff;
    } وإلا {
        U_ASSERT((اشاریہ + 2) <= length);
        U_ASSERT(array[اشاریہ] >= 0x8000);
        U_ASSERT(array[اشاریہ + 1] >= 0x8000);
        int32_t len = ((سر & 1) << 30) |
                (static_cast<int32_t>(array[اشاریہ] & 0x7fff) << 15) |
                (array[اشاریہ + 1] & 0x7fff);
        اشاریہ += 2;
        أرجع len;
    }
}

فراغ Edits::Iterator::update_اگلا_Indexes() {
    src_اشاریہ += old_لمائی_2;
    إذا (بدلیا) {
        repl_اشاریہ += new_لمائی_2;
    }
    dest_اشاریہ += new_لمائی_2;
}

فراغ Edits::Iterator::update_پچھلا_Indexes() {
    src_اشاریہ -= old_لمائی_2;
    إذا (بدلیا) {
        repl_اشاریہ -= new_لمائی_2;
    }
    dest_اشاریہ -= new_لمائی_2;
}

UBool Edits::Iterator::no_اگلا() {
    // No change before or beyond the string.
    dir = 0;
    بدلیا = خطأ;
    old_لمائی_2 = new_لمائی_2 = 0;
    أرجع خطأ;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_کوڈ &error_کوڈ) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    إذا (dir > 0) {
        update_اگلا_Indexes();
    } وإلا {
        إذا (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            إذا (بچیا > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++اشاریہ;  // next() rests on the index after the sequence unit.
                dir = 1;
                أرجع سچ;
            }
        }
        dir = 1;
    }
    إذا (بچیا >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        إذا (بچیا > 1) {
            --بچیا;
            أرجع سچ;
        }
        بچیا = 0;
    }
    إذا (اشاریہ >= length) {
        أرجع no_اگلا();
    }
    int32_t u = array[اشاریہ++];
    إذا (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        بدلیا = خطأ;
        old_لمائی_2 = u + 1;
        طالما (اشاریہ < length && (u = array[اشاریہ]) <= MAX_UNCHANGED) {
            ++اشاریہ;
            old_لمائی_2 += u + 1;
        }
        new_لمائی_2 = old_لمائی_2;
        إذا (onlyChanges) {
            update_اگلا_Indexes();
            إذا (اشاریہ >= length) {
                أرجع no_اگلا();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++اشاریہ;
        } وإلا {
            أرجع سچ;
        }
    }
    بدلیا = سچ;
    إذا (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_لمائی;
        int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1;
        إذا (coarse) {
            old_لمائی_2 = num * oldLen;
            new_لمائی_2 = num * newLen;
        } وإلا {
            // Split a sequence of changes that was compressed into one unit.
            old_لمائی_2 = oldLen;
            new_لمائی_2 = newLen;
            إذا (num > 1) {
                بچیا = num;  // This is the first of two or more changes.
            }
            أرجع سچ;
        }
    } وإلا {
        U_ASSERT(u <= 0x7fff);
        old_لمائی_2 = read_لمائی((u >> 6) & 0x3f);
        new_لمائی_2 = read_لمائی(u & 0x3f);
        إذا (!coarse) {
            أرجع سچ;
        }
    }
    // Combine adjacent changes.
    طالما (اشاریہ < length && (u = array[اشاریہ]) > MAX_UNCHANGED) {
        ++اشاریہ;
        إذا (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1;
            old_لمائی_2 += (u >> 12) * num;
            new_لمائی_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_لمائی) * num;
        } وإلا {
            U_ASSERT(u <= 0x7fff);
            old_لمائی_2 += read_لمائی((u >> 6) & 0x3f);
            new_لمائی_2 += read_لمائی(u & 0x3f);
        }
    }
    أرجع سچ;
}

UBool Edits::Iterator::پچھلا(UError_کوڈ &error_کوڈ) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    إذا (dir >= 0) {
        إذا (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            إذا (بچیا > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --اشاریہ;  // previous() rests on the sequence unit.
                dir = -1;
                أرجع سچ;
            }
            update_اگلا_Indexes();
        }
        dir = -1;
    }
    إذا (بچیا > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[اشاریہ];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        إذا (بچیا <= (u & SHORT_CHANGE_عدد_نقاب)) {
            ++بچیا;
            update_پچھلا_Indexes();
            أرجع سچ;
        }
        بچیا = 0;
    }
    إذا (اشاریہ <= 0) {
        أرجع no_اگلا();
    }
    int32_t u = array[--اشاریہ];
    إذا (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        بدلیا = خطأ;
        old_لمائی_2 = u + 1;
        طالما (اشاریہ > 0 && (u = array[اشاریہ - 1]) <= MAX_UNCHANGED) {
            --اشاریہ;
            old_لمائی_2 += u + 1;
        }
        new_لمائی_2 = old_لمائی_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        update_پچھلا_Indexes();
        أرجع سچ;
    }
    بدلیا = سچ;
    إذا (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_لمائی;
        int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1;
        إذا (coarse) {
            old_لمائی_2 = num * oldLen;
            new_لمائی_2 = num * newLen;
        } وإلا {
            // Split a sequence of changes that was compressed into one unit.
            old_لمائی_2 = oldLen;
            new_لمائی_2 = newLen;
            إذا (num > 1) {
                بچیا = 1;  // This is the last of two or more changes.
            }
            update_پچھلا_Indexes();
            أرجع سچ;
        }
    } وإلا {
        إذا (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_لمائی_2 = read_لمائی((u >> 6) & 0x3f);
            new_لمائی_2 = read_لمائی(u & 0x3f);
        } وإلا {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(اشاریہ > 0);
            طالما ((u = array[--اشاریہ]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t سر_اشاریہ = اشاریہ++;
            old_لمائی_2 = read_لمائی((u >> 6) & 0x3f);
            new_لمائی_2 = read_لمائی(u & 0x3f);
            اشاریہ = سر_اشاریہ;
        }
        إذا (!coarse) {
            update_پچھلا_Indexes();
            أرجع سچ;
        }
    }
    // Combine adjacent changes.
    طالما (اشاریہ > 0 && (u = array[اشاریہ - 1]) > MAX_UNCHANGED) {
        --اشاریہ;
        إذا (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1;
            old_لمائی_2 += (u >> 12) * num;
            new_لمائی_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_لمائی) * num;
        } وإلا إذا (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t سر_اشاریہ = اشاریہ++;
            old_لمائی_2 += read_لمائی((u >> 6) & 0x3f);
            new_لمائی_2 += read_لمائی(u & 0x3f);
            اشاریہ = سر_اشاریہ;
        }
    }
    update_پچھلا_Indexes();
    أرجع سچ;
}

int32_t Edits::Iterator::لبھو_اشاریہ(int32_t i, UBool لبھو_سروت, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ) || i < 0) { أرجع -1; }
    int32_t وسعت_شروع, وسعت_لمائی;
    إذا (لبھو_سروت) {  // find source index
        وسعت_شروع = src_اشاریہ;
        وسعت_لمائی = old_لمائی_2;
    } وإلا {  // find destination index
        وسعت_شروع = dest_اشاریہ;
        وسعت_لمائی = new_لمائی_2;
    }
    إذا (i < وسعت_شروع) {
        إذا (i >= (وسعت_شروع / 2)) {
            // Search backwards.
            لكل (;;) {
                UBool has_پچھلا = پچھلا(error_کوڈ);
                U_ASSERT(has_پچھلا);  // because i>=0 and the first span starts at 0
                (فراغ)has_پچھلا;  // avoid unused-variable warning
                وسعت_شروع = لبھو_سروت ? src_اشاریہ : dest_اشاریہ;
                إذا (i >= وسعت_شروع) {
                    // The index is in the current span.
                    أرجع 0;
                }
                إذا (بچیا > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    وسعت_لمائی = لبھو_سروت ? old_لمائی_2 : new_لمائی_2;
                    int32_t u = array[اشاریہ];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1 - بچیا;
                    int32_t len = num * وسعت_لمائی;
                    إذا (i >= (وسعت_شروع - len)) {
                        int32_t n = ((وسعت_شروع - i - 1) / وسعت_لمائی) + 1;
                        // 1 <= n <= num
                        src_اشاریہ -= n * old_لمائی_2;
                        repl_اشاریہ -= n * new_لمائی_2;
                        dest_اشاریہ -= n * new_لمائی_2;
                        بچیا += n;
                        أرجع 0;
                    }
                    // Skip all of these edits at once.
                    src_اشاریہ -= num * old_لمائی_2;
                    repl_اشاریہ -= num * new_لمائی_2;
                    dest_اشاریہ -= num * new_لمائی_2;
                    بچیا = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        اشاریہ = بچیا = old_لمائی_2 = new_لمائی_2 = src_اشاریہ = repl_اشاریہ = dest_اشاریہ = 0;
    } وإلا إذا (i < (وسعت_شروع + وسعت_لمائی)) {
        // The index is in the current span.
        أرجع 0;
    }
    طالما (next(خطأ, error_کوڈ)) {
        إذا (لبھو_سروت) {
            وسعت_شروع = src_اشاریہ;
            وسعت_لمائی = old_لمائی_2;
        } وإلا {
            وسعت_شروع = dest_اشاریہ;
            وسعت_لمائی = new_لمائی_2;
        }
        إذا (i < (وسعت_شروع + وسعت_لمائی)) {
            // The index is in the current span.
            أرجع 0;
        }
        إذا (بچیا > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = بچیا * وسعت_لمائی;
            إذا (i < (وسعت_شروع + len)) {
                int32_t n = (i - وسعت_شروع) / وسعت_لمائی;  // 1 <= n <= remaining - 1
                src_اشاریہ += n * old_لمائی_2;
                repl_اشاریہ += n * new_لمائی_2;
                dest_اشاریہ += n * new_لمائی_2;
                بچیا -= n;
                أرجع 0;
            }
            // Make next() skip all of these edits at once.
            old_لمائی_2 *= بچیا;
            new_لمائی_2 *= بچیا;
            بچیا = 0;
        }
    }
    أرجع 1;
}

int32_t Edits::Iterator::destination_اشاریہ_From_سروت_اشاریہ(int32_t i, UError_کوڈ &error_کوڈ) {
    int32_t where = لبھو_اشاریہ(i, سچ, error_کوڈ);
    إذا (where < 0) {
        // Error or before the string.
        أرجع 0;
    }
    إذا (where > 0 || i == src_اشاریہ) {
        // At or after string length, or at start of the found span.
        أرجع dest_اشاریہ;
    }
    إذا (بدلیا) {
        // In a change span, map to its end.
        أرجع dest_اشاریہ + new_لمائی_2;
    } وإلا {
        // In an unchanged span, offset 1:1 within it.
        أرجع dest_اشاریہ + (i - src_اشاریہ);
    }
}

int32_t Edits::Iterator::سروت_اشاریہ_From_Destination_اشاریہ(int32_t i, UError_کوڈ &error_کوڈ) {
    int32_t where = لبھو_اشاریہ(i, خطأ, error_کوڈ);
    إذا (where < 0) {
        // Error or before the string.
        أرجع 0;
    }
    إذا (where > 0 || i == dest_اشاریہ) {
        // At or after string length, or at start of the found span.
        أرجع src_اشاریہ;
    }
    إذا (بدلیا) {
        // In a change span, map to its end.
        أرجع src_اشاریہ + old_لمائی_2;
    } وإلا {
        // In an unchanged span, offset within it.
        أرجع src_اشاریہ + (i - dest_اشاریہ);
    }
}

UnicodeString& Edits::Iterator::منزل_String(UnicodeString& sb U_LIFETIME_BOUND) ثابت {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_نمبر(sb, src_اشاریہ);
    sb.append(u"..", -1);
    ICU_Utility::append_نمبر(sb, src_اشاریہ + old_لمائی_2);
    إذا (بدلیا) {
        sb.append(u"] ⇝ dest[", -1);
    } وإلا {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_نمبر(sb, dest_اشاریہ);
    sb.append(u"..", -1);
    ICU_Utility::append_نمبر(sb, dest_اشاریہ + new_لمائی_2);
    إذا (بدلیا) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_نمبر(sb, repl_اشاریہ);
        sb.append(u"..", -1);
        ICU_Utility::append_نمبر(sb, repl_اشاریہ + new_لمائی_2);
        sb.append(u"] }", -1);
    } وإلا {
        sb.append(u"] (no-change) }", -1);
    }
    أرجع sb;
}

U_NAMESPACE_سرا
