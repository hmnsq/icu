// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#شامل "unicode/edits.h"
#شامل "unicode/unistr.h"
#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "util.h"

U_NAMESPACE_BEGIN

نامخلا {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
ثابت int32_t MAX_UNCHANGED_لمبائی = 0x1000;
ثابت int32_t MAX_UNCHANGED = MAX_UNCHANGED_لمبائی - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ثابت int32_t MAX_SHORT_CHANGE_OLD_لمبائی = 6;
ثابت int32_t MAX_SHORT_CHANGE_NEW_لمبائی = 7;
ثابت int32_t SHORT_CHANGE_عدد_نقاب = 0x1ff;
ثابت int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ثابت int32_t لمبائی_اندر_1_TRAIL = 61;
ثابت int32_t لمبائی_اندر_2_TRAIL = 62;

}  // namespace

خالی Edits::releaseArray() noexcept {
    اگر (array != ڈھیر_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ثابت Edits& other) U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز_2)) {
        length = delta = عدد_Changes = 0;
        واپس *یہ;
    }
    اگر (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        اگر (newArray == خالی_اشاریہ) {
            length = delta = عدد_Changes = 0;
            error_رمز_2 = U_حافظہ_تخصیص_ERROR;
            واپس *یہ;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    اگر (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    واپس *یہ;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز_2)) {
        length = delta = عدد_Changes = 0;
        واپس *یہ;
    }
    releaseArray();
    اگر (length > ڈھیر_استعداد_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.ڈھیر_Array;
        src.capacity = ڈھیر_استعداد_2;
        src.reset();
        واپس *یہ;
    }
    array = ڈھیر_Array;
    capacity = ڈھیر_استعداد_2;
    اگر (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    واپس *یہ;
}

Edits& Edits::عامل=(ثابت Edits& other) U_LIFETIME_BOUND {
    اگر (یہ == &other) { واپس *یہ; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    عدد_Changes = other.عدد_Changes;
    error_رمز_2 = other.error_رمز_2;
    واپس copyArray(other);
}

Edits& Edits::عامل=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    عدد_Changes = src.عدد_Changes;
    error_رمز_2 = src.error_رمز_2;
    واپس moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

خالی Edits::reset() noexcept {
    length = delta = عدد_Changes = 0;
    error_رمز_2 = U_ZERO_ERROR;
}

خالی Edits::جوڑو_Unchanged(int32_t unchanged_لمبائی) {
    اگر(U_FAILURE(error_رمز_2) || unchanged_لمبائی == 0) { واپس; }
    اگر(unchanged_لمبائی < 0) {
        error_رمز_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t آخری = آخری_Unit();
    اگر(آخری < MAX_UNCHANGED) {
        int32_t بقایا = MAX_UNCHANGED - آخری;
        اگر (بقایا >= unchanged_لمبائی) {
            سیٹ_آخری_Unit(آخری + unchanged_لمبائی);
            واپس;
        }
        سیٹ_آخری_Unit(MAX_UNCHANGED);
        unchanged_لمبائی -= بقایا;
    }
    // Split large lengths into multiple units.
    جب(unchanged_لمبائی >= MAX_UNCHANGED_لمبائی) {
        append(MAX_UNCHANGED);
        unchanged_لمبائی -= MAX_UNCHANGED_لمبائی;
    }
    // Write a small (remaining) length.
    اگر(unchanged_لمبائی > 0) {
        append(unchanged_لمبائی - 1);
    }
}

خالی Edits::جوڑو_Replace(int32_t old_لمبائی, int32_t new_لمبائی) {
    اگر(U_FAILURE(error_رمز_2)) { واپس; }
    اگر(old_لمبائی < 0 || new_لمبائی < 0) {
        error_رمز_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر (old_لمبائی == 0 && new_لمبائی == 0) {
        واپس;
    }
    ++عدد_Changes;
    int32_t newDelta = new_لمبائی - old_لمبائی;
    اگر (newDelta != 0) {
        اگر ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_رمز_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس;
        }
        delta += newDelta;
    }

    اگر(0 < old_لمبائی && old_لمبائی <= MAX_SHORT_CHANGE_OLD_لمبائی &&
            new_لمبائی <= MAX_SHORT_CHANGE_NEW_لمبائی) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_لمبائی << 12) | (new_لمبائی << 9);
        int32_t آخری = آخری_Unit();
        اگر(MAX_UNCHANGED < آخری && آخری < MAX_SHORT_CHANGE &&
                (آخری & ~SHORT_CHANGE_عدد_نقاب) == u &&
                (آخری & SHORT_CHANGE_عدد_نقاب) < SHORT_CHANGE_عدد_نقاب) {
            سیٹ_آخری_Unit(آخری + 1);
            واپس;
        }
        append(u);
        واپس;
    }

    int32_t سر = 0x7000;
    اگر (old_لمبائی < لمبائی_اندر_1_TRAIL && new_لمبائی < لمبائی_اندر_1_TRAIL) {
        سر |= old_لمبائی << 6;
        سر |= new_لمبائی;
        append(سر);
    } ورنہ اگر ((capacity - length) >= 5 || growArray()) {
        int32_t حد_بندی = length + 1;
        اگر(old_لمبائی < لمبائی_اندر_1_TRAIL) {
            سر |= old_لمبائی << 6;
        } ورنہ اگر(old_لمبائی <= 0x7fff) {
            سر |= لمبائی_اندر_1_TRAIL << 6;
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | old_لمبائی);
        } ورنہ {
            سر |= (لمبائی_اندر_2_TRAIL + (old_لمبائی >> 30)) << 6;
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | (old_لمبائی >> 15));
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | old_لمبائی);
        }
        اگر(new_لمبائی < لمبائی_اندر_1_TRAIL) {
            سر |= new_لمبائی;
        } ورنہ اگر(new_لمبائی <= 0x7fff) {
            سر |= لمبائی_اندر_1_TRAIL;
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | new_لمبائی);
        } ورنہ {
            سر |= لمبائی_اندر_2_TRAIL + (new_لمبائی >> 30);
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | (new_لمبائی >> 15));
            array[حد_بندی++] = static_cast<uint16_t>(0x8000 | new_لمبائی);
        }
        array[length] = static_cast<uint16_t>(سر);
        length = حد_بندی;
    }
}

خالی Edits::append(int32_t r) {
    اگر(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t new_استعداد;
    اگر (array == ڈھیر_Array) {
        new_استعداد = 2000;
    } ورنہ اگر (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_رمز_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس جھوٹ;
    } ورنہ اگر (capacity >= (INT32_MAX / 2)) {
        new_استعداد = INT32_MAX;
    } ورنہ {
        new_استعداد = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    اگر ((new_استعداد - capacity) < 5) {
        error_رمز_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس جھوٹ;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_استعداد) * 2));
    اگر (newArray == خالی_اشاریہ) {
        error_رمز_2 = U_حافظہ_تخصیص_ERROR;
        واپس جھوٹ;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_استعداد;
    واپس سچ;
}

UBool Edits::copy_Error_مقصد(UError_رمز &برآمد_Error_رمز) ثابت {
    اگر (U_FAILURE(برآمد_Error_رمز)) { واپس سچ; }
    اگر (U_SUCCESS(error_رمز_2)) { واپس جھوٹ; }
    برآمد_Error_رمز = error_رمز_2;
    واپس سچ;
}

Edits& Edits::ملاؤ_And_Append(ثابت Edits& ab,
                             ثابت Edits& bc,
                             UError_رمز& error_رمز) U_LIFETIME_BOUND {
    اگر (copy_Error_مقصد(error_رمز)) { واپس *یہ; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_اگلا = سچ, bc_Has_اگلا = سچ;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_لمبائی = 0, ab_b_لمبائی = 0, bc_b_لمبائی = 0, c_لمبائی = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_لمبائی = 0, pending_c_لمبائی = 0;
    برائے (;;) {
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
        اگر (bc_b_لمبائی == 0) {
            اگر (bc_Has_اگلا && (bc_Has_اگلا = bcIter.next(error_رمز)) != 0) {
                bc_b_لمبائی = bcIter.old_لمبائی();
                c_لمبائی = bcIter.new_لمبائی();
                اگر (bc_b_لمبائی == 0) {
                    // insertion
                    اگر (ab_b_لمبائی == 0 || !abIter.hasChange()) {
                        جوڑو_Replace(pending_a_لمبائی, pending_c_لمبائی + c_لمبائی);
                        pending_a_لمبائی = pending_c_لمبائی = 0;
                    } ورنہ {
                        pending_c_لمبائی += c_لمبائی;
                    }
                    جاری;
                }
            }
            // else see if the other iterator is done, too.
        }
        اگر (ab_b_لمبائی == 0) {
            اگر (ab_Has_اگلا && (ab_Has_اگلا = abIter.next(error_رمز)) != 0) {
                a_لمبائی = abIter.old_لمبائی();
                ab_b_لمبائی = abIter.new_لمبائی();
                اگر (ab_b_لمبائی == 0) {
                    // deletion
                    اگر (bc_b_لمبائی == bcIter.old_لمبائی() || !bcIter.hasChange()) {
                        جوڑو_Replace(pending_a_لمبائی + a_لمبائی, pending_c_لمبائی);
                        pending_a_لمبائی = pending_c_لمبائی = 0;
                    } ورنہ {
                        pending_a_لمبائی += a_لمبائی;
                    }
                    جاری;
                }
            } ورنہ اگر (bc_b_لمبائی == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                توڑو;
            } ورنہ {
                // The ab output string is shorter than the bc input string.
                اگر (!copy_Error_مقصد(error_رمز)) {
                    error_رمز = U_ILLEGAL_دلیل_ERROR;
                }
                واپس *یہ;
            }
        }
        اگر (bc_b_لمبائی == 0) {
            // The bc input string is shorter than the ab output string.
            اگر (!copy_Error_مقصد(error_رمز)) {
                error_رمز = U_ILLEGAL_دلیل_ERROR;
            }
            واپس *یہ;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        اگر (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            اگر (pending_a_لمبائی != 0 || pending_c_لمبائی != 0) {
                جوڑو_Replace(pending_a_لمبائی, pending_c_لمبائی);
                pending_a_لمبائی = pending_c_لمبائی = 0;
            }
            int32_t unchanged_لمبائی = a_لمبائی <= c_لمبائی ? a_لمبائی : c_لمبائی;
            جوڑو_Unchanged(unchanged_لمبائی);
            ab_b_لمبائی = a_لمبائی -= unchanged_لمبائی;
            bc_b_لمبائی = c_لمبائی -= unchanged_لمبائی;
            // At least one of the unchanged spans is now empty.
            جاری;
        }
        اگر (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            اگر (ab_b_لمبائی >= bc_b_لمبائی) {
                // Split the longer unchanged span into change + remainder.
                جوڑو_Replace(pending_a_لمبائی + bc_b_لمبائی, pending_c_لمبائی + c_لمبائی);
                pending_a_لمبائی = pending_c_لمبائی = 0;
                a_لمبائی = ab_b_لمبائی -= bc_b_لمبائی;
                bc_b_لمبائی = 0;
                جاری;
            }
            // Handle the shorter unchanged span below like a change.
        } ورنہ اگر (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            اگر (ab_b_لمبائی <= bc_b_لمبائی) {
                // Split the longer unchanged span into change + remainder.
                جوڑو_Replace(pending_a_لمبائی + a_لمبائی, pending_c_لمبائی + ab_b_لمبائی);
                pending_a_لمبائی = pending_c_لمبائی = 0;
                c_لمبائی = bc_b_لمبائی -= ab_b_لمبائی;
                ab_b_لمبائی = 0;
                جاری;
            }
            // Handle the shorter unchanged span below like a change.
        } ورنہ {  // both abIter.hasChange() && bcIter.hasChange()
            اگر (ab_b_لمبائی == bc_b_لمبائی) {
                // Changes on both sides up to the same position. Emit & reset.
                جوڑو_Replace(pending_a_لمبائی + a_لمبائی, pending_c_لمبائی + c_لمبائی);
                pending_a_لمبائی = pending_c_لمبائی = 0;
                ab_b_لمبائی = bc_b_لمبائی = 0;
                جاری;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_لمبائی += a_لمبائی;
        pending_c_لمبائی += c_لمبائی;
        اگر (ab_b_لمبائی < bc_b_لمبائی) {
            bc_b_لمبائی -= ab_b_لمبائی;
            c_لمبائی = ab_b_لمبائی = 0;
        } ورنہ {  // ab_bLength > bc_bLength
            ab_b_لمبائی -= bc_b_لمبائی;
            a_لمبائی = bc_b_لمبائی = 0;
        }
    }
    اگر (pending_a_لمبائی != 0 || pending_c_لمبائی != 0) {
        جوڑو_Replace(pending_a_لمبائی, pending_c_لمبائی);
    }
    copy_Error_مقصد(error_رمز);
    واپس *یہ;
}

Edits::Iterator::Iterator(ثابت uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), اشاریہ(0), length(len), بقایا(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), بدلا(جھوٹ), old_لمبائی_2(0), new_لمبائی_2(0),
        src_اشاریہ(0), repl_اشاریہ(0), dest_اشاریہ(0) {}

int32_t Edits::Iterator::read_لمبائی(int32_t سر) {
    اگر (سر < لمبائی_اندر_1_TRAIL) {
        واپس سر;
    } ورنہ اگر (سر < لمبائی_اندر_2_TRAIL) {
        U_ASSERT(اشاریہ < length);
        U_ASSERT(array[اشاریہ] >= 0x8000);
        واپس array[اشاریہ++] & 0x7fff;
    } ورنہ {
        U_ASSERT((اشاریہ + 2) <= length);
        U_ASSERT(array[اشاریہ] >= 0x8000);
        U_ASSERT(array[اشاریہ + 1] >= 0x8000);
        int32_t len = ((سر & 1) << 30) |
                (static_cast<int32_t>(array[اشاریہ] & 0x7fff) << 15) |
                (array[اشاریہ + 1] & 0x7fff);
        اشاریہ += 2;
        واپس len;
    }
}

خالی Edits::Iterator::update_اگلا_Indexes() {
    src_اشاریہ += old_لمبائی_2;
    اگر (بدلا) {
        repl_اشاریہ += new_لمبائی_2;
    }
    dest_اشاریہ += new_لمبائی_2;
}

خالی Edits::Iterator::update_پچھلا_Indexes() {
    src_اشاریہ -= old_لمبائی_2;
    اگر (بدلا) {
        repl_اشاریہ -= new_لمبائی_2;
    }
    dest_اشاریہ -= new_لمبائی_2;
}

UBool Edits::Iterator::no_اگلا() {
    // No change before or beyond the string.
    dir = 0;
    بدلا = جھوٹ;
    old_لمبائی_2 = new_لمبائی_2 = 0;
    واپس جھوٹ;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_رمز &error_رمز) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    اگر (dir > 0) {
        update_اگلا_Indexes();
    } ورنہ {
        اگر (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            اگر (بقایا > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++اشاریہ;  // next() rests on the index after the sequence unit.
                dir = 1;
                واپس سچ;
            }
        }
        dir = 1;
    }
    اگر (بقایا >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        اگر (بقایا > 1) {
            --بقایا;
            واپس سچ;
        }
        بقایا = 0;
    }
    اگر (اشاریہ >= length) {
        واپس no_اگلا();
    }
    int32_t u = array[اشاریہ++];
    اگر (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        بدلا = جھوٹ;
        old_لمبائی_2 = u + 1;
        جب (اشاریہ < length && (u = array[اشاریہ]) <= MAX_UNCHANGED) {
            ++اشاریہ;
            old_لمبائی_2 += u + 1;
        }
        new_لمبائی_2 = old_لمبائی_2;
        اگر (onlyChanges) {
            update_اگلا_Indexes();
            اگر (اشاریہ >= length) {
                واپس no_اگلا();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++اشاریہ;
        } ورنہ {
            واپس سچ;
        }
    }
    بدلا = سچ;
    اگر (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_لمبائی;
        int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1;
        اگر (coarse) {
            old_لمبائی_2 = num * oldLen;
            new_لمبائی_2 = num * newLen;
        } ورنہ {
            // Split a sequence of changes that was compressed into one unit.
            old_لمبائی_2 = oldLen;
            new_لمبائی_2 = newLen;
            اگر (num > 1) {
                بقایا = num;  // This is the first of two or more changes.
            }
            واپس سچ;
        }
    } ورنہ {
        U_ASSERT(u <= 0x7fff);
        old_لمبائی_2 = read_لمبائی((u >> 6) & 0x3f);
        new_لمبائی_2 = read_لمبائی(u & 0x3f);
        اگر (!coarse) {
            واپس سچ;
        }
    }
    // Combine adjacent changes.
    جب (اشاریہ < length && (u = array[اشاریہ]) > MAX_UNCHANGED) {
        ++اشاریہ;
        اگر (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1;
            old_لمبائی_2 += (u >> 12) * num;
            new_لمبائی_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_لمبائی) * num;
        } ورنہ {
            U_ASSERT(u <= 0x7fff);
            old_لمبائی_2 += read_لمبائی((u >> 6) & 0x3f);
            new_لمبائی_2 += read_لمبائی(u & 0x3f);
        }
    }
    واپس سچ;
}

UBool Edits::Iterator::پچھلا_2(UError_رمز &error_رمز) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    اگر (dir >= 0) {
        اگر (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            اگر (بقایا > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --اشاریہ;  // previous() rests on the sequence unit.
                dir = -1;
                واپس سچ;
            }
            update_اگلا_Indexes();
        }
        dir = -1;
    }
    اگر (بقایا > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[اشاریہ];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        اگر (بقایا <= (u & SHORT_CHANGE_عدد_نقاب)) {
            ++بقایا;
            update_پچھلا_Indexes();
            واپس سچ;
        }
        بقایا = 0;
    }
    اگر (اشاریہ <= 0) {
        واپس no_اگلا();
    }
    int32_t u = array[--اشاریہ];
    اگر (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        بدلا = جھوٹ;
        old_لمبائی_2 = u + 1;
        جب (اشاریہ > 0 && (u = array[اشاریہ - 1]) <= MAX_UNCHANGED) {
            --اشاریہ;
            old_لمبائی_2 += u + 1;
        }
        new_لمبائی_2 = old_لمبائی_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        update_پچھلا_Indexes();
        واپس سچ;
    }
    بدلا = سچ;
    اگر (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_لمبائی;
        int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1;
        اگر (coarse) {
            old_لمبائی_2 = num * oldLen;
            new_لمبائی_2 = num * newLen;
        } ورنہ {
            // Split a sequence of changes that was compressed into one unit.
            old_لمبائی_2 = oldLen;
            new_لمبائی_2 = newLen;
            اگر (num > 1) {
                بقایا = 1;  // This is the last of two or more changes.
            }
            update_پچھلا_Indexes();
            واپس سچ;
        }
    } ورنہ {
        اگر (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_لمبائی_2 = read_لمبائی((u >> 6) & 0x3f);
            new_لمبائی_2 = read_لمبائی(u & 0x3f);
        } ورنہ {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(اشاریہ > 0);
            جب ((u = array[--اشاریہ]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t سر_اشاریہ = اشاریہ++;
            old_لمبائی_2 = read_لمبائی((u >> 6) & 0x3f);
            new_لمبائی_2 = read_لمبائی(u & 0x3f);
            اشاریہ = سر_اشاریہ;
        }
        اگر (!coarse) {
            update_پچھلا_Indexes();
            واپس سچ;
        }
    }
    // Combine adjacent changes.
    جب (اشاریہ > 0 && (u = array[اشاریہ - 1]) > MAX_UNCHANGED) {
        --اشاریہ;
        اگر (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1;
            old_لمبائی_2 += (u >> 12) * num;
            new_لمبائی_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_لمبائی) * num;
        } ورنہ اگر (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t سر_اشاریہ = اشاریہ++;
            old_لمبائی_2 += read_لمبائی((u >> 6) & 0x3f);
            new_لمبائی_2 += read_لمبائی(u & 0x3f);
            اشاریہ = سر_اشاریہ;
        }
    }
    update_پچھلا_Indexes();
    واپس سچ;
}

int32_t Edits::Iterator::ڈھونڈو_اشاریہ(int32_t i, UBool ڈھونڈو_ماخذ, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز) || i < 0) { واپس -1; }
    int32_t وسعت_آغاز, وسعت_لمبائی;
    اگر (ڈھونڈو_ماخذ) {  // find source index
        وسعت_آغاز = src_اشاریہ;
        وسعت_لمبائی = old_لمبائی_2;
    } ورنہ {  // find destination index
        وسعت_آغاز = dest_اشاریہ;
        وسعت_لمبائی = new_لمبائی_2;
    }
    اگر (i < وسعت_آغاز) {
        اگر (i >= (وسعت_آغاز / 2)) {
            // Search backwards.
            برائے (;;) {
                UBool has_پچھلا = پچھلا_2(error_رمز);
                U_ASSERT(has_پچھلا);  // because i>=0 and the first span starts at 0
                (خالی)has_پچھلا;  // avoid unused-variable warning
                وسعت_آغاز = ڈھونڈو_ماخذ ? src_اشاریہ : dest_اشاریہ;
                اگر (i >= وسعت_آغاز) {
                    // The index is in the current span.
                    واپس 0;
                }
                اگر (بقایا > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    وسعت_لمبائی = ڈھونڈو_ماخذ ? old_لمبائی_2 : new_لمبائی_2;
                    int32_t u = array[اشاریہ];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_عدد_نقاب) + 1 - بقایا;
                    int32_t len = num * وسعت_لمبائی;
                    اگر (i >= (وسعت_آغاز - len)) {
                        int32_t n = ((وسعت_آغاز - i - 1) / وسعت_لمبائی) + 1;
                        // 1 <= n <= num
                        src_اشاریہ -= n * old_لمبائی_2;
                        repl_اشاریہ -= n * new_لمبائی_2;
                        dest_اشاریہ -= n * new_لمبائی_2;
                        بقایا += n;
                        واپس 0;
                    }
                    // Skip all of these edits at once.
                    src_اشاریہ -= num * old_لمبائی_2;
                    repl_اشاریہ -= num * new_لمبائی_2;
                    dest_اشاریہ -= num * new_لمبائی_2;
                    بقایا = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        اشاریہ = بقایا = old_لمبائی_2 = new_لمبائی_2 = src_اشاریہ = repl_اشاریہ = dest_اشاریہ = 0;
    } ورنہ اگر (i < (وسعت_آغاز + وسعت_لمبائی)) {
        // The index is in the current span.
        واپس 0;
    }
    جب (next(جھوٹ, error_رمز)) {
        اگر (ڈھونڈو_ماخذ) {
            وسعت_آغاز = src_اشاریہ;
            وسعت_لمبائی = old_لمبائی_2;
        } ورنہ {
            وسعت_آغاز = dest_اشاریہ;
            وسعت_لمبائی = new_لمبائی_2;
        }
        اگر (i < (وسعت_آغاز + وسعت_لمبائی)) {
            // The index is in the current span.
            واپس 0;
        }
        اگر (بقایا > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = بقایا * وسعت_لمبائی;
            اگر (i < (وسعت_آغاز + len)) {
                int32_t n = (i - وسعت_آغاز) / وسعت_لمبائی;  // 1 <= n <= remaining - 1
                src_اشاریہ += n * old_لمبائی_2;
                repl_اشاریہ += n * new_لمبائی_2;
                dest_اشاریہ += n * new_لمبائی_2;
                بقایا -= n;
                واپس 0;
            }
            // Make next() skip all of these edits at once.
            old_لمبائی_2 *= بقایا;
            new_لمبائی_2 *= بقایا;
            بقایا = 0;
        }
    }
    واپس 1;
}

int32_t Edits::Iterator::destination_اشاریہ_From_ماخذ_اشاریہ(int32_t i, UError_رمز &error_رمز) {
    int32_t where = ڈھونڈو_اشاریہ(i, سچ, error_رمز);
    اگر (where < 0) {
        // Error or before the string.
        واپس 0;
    }
    اگر (where > 0 || i == src_اشاریہ) {
        // At or after string length, or at start of the found span.
        واپس dest_اشاریہ;
    }
    اگر (بدلا) {
        // In a change span, map to its end.
        واپس dest_اشاریہ + new_لمبائی_2;
    } ورنہ {
        // In an unchanged span, offset 1:1 within it.
        واپس dest_اشاریہ + (i - src_اشاریہ);
    }
}

int32_t Edits::Iterator::ماخذ_اشاریہ_From_Destination_اشاریہ(int32_t i, UError_رمز &error_رمز) {
    int32_t where = ڈھونڈو_اشاریہ(i, جھوٹ, error_رمز);
    اگر (where < 0) {
        // Error or before the string.
        واپس 0;
    }
    اگر (where > 0 || i == dest_اشاریہ) {
        // At or after string length, or at start of the found span.
        واپس src_اشاریہ;
    }
    اگر (بدلا) {
        // In a change span, map to its end.
        واپس src_اشاریہ + old_لمبائی_2;
    } ورنہ {
        // In an unchanged span, offset within it.
        واپس src_اشاریہ + (i - dest_اشاریہ);
    }
}

UnicodeString& Edits::Iterator::مقصد_String(UnicodeString& sb U_LIFETIME_BOUND) ثابت {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_نمبر(sb, src_اشاریہ);
    sb.append(u"..", -1);
    ICU_Utility::append_نمبر(sb, src_اشاریہ + old_لمبائی_2);
    اگر (بدلا) {
        sb.append(u"] ⇝ dest[", -1);
    } ورنہ {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_نمبر(sb, dest_اشاریہ);
    sb.append(u"..", -1);
    ICU_Utility::append_نمبر(sb, dest_اشاریہ + new_لمبائی_2);
    اگر (بدلا) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_نمبر(sb, repl_اشاریہ);
        sb.append(u"..", -1);
        ICU_Utility::append_نمبر(sb, repl_اشاریہ + new_لمبائی_2);
        sb.append(u"] }", -1);
    } ورنہ {
        sb.append(u"] (no-change) }", -1);
    }
    واپس sb;
}

U_NAMESPACE_انتہا
