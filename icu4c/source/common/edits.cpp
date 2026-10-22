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
ध्रुव int32_t MAX_UNCHANGED_दीर्घता = 0x1000;
ध्रुव int32_t MAX_UNCHANGED = MAX_UNCHANGED_दीर्घता - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
ध्रुव int32_t MAX_SHORT_CHANGE_OLD_दीर्घता = 6;
ध्रुव int32_t MAX_SHORT_CHANGE_NEW_दीर्घता = 7;
ध्रुव int32_t SHORT_CHANGE_अङ्कः_आवरणम् = 0x1ff;
ध्रुव int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
ध्रुव int32_t दीर्घता_अन्तर्_1_TRAIL = 61;
ध्रुव int32_t दीर्घता_अन्तर्_2_TRAIL = 62;

}  // namespace

शून्य Edits::releaseArray() noexcept {
    यदि (array != राशिः_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(ध्रुव Edits& other) U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता_2)) {
        length = delta = अङ्कः_Changes = 0;
        प्रत्यागम *एतत्;
    }
    यदि (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        यदि (newArray == शून्यसूचकः) {
            length = delta = अङ्कः_Changes = 0;
            error_संहिता_2 = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम *एतत्;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    यदि (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    प्रत्यागम *एतत्;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता_2)) {
        length = delta = अङ्कः_Changes = 0;
        प्रत्यागम *एतत्;
    }
    releaseArray();
    यदि (length > राशिः_क्षमता_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.राशिः_Array;
        src.capacity = राशिः_क्षमता_2;
        src.reset();
        प्रत्यागम *एतत्;
    }
    array = राशिः_Array;
    capacity = राशिः_क्षमता_2;
    यदि (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    प्रत्यागम *एतत्;
}

Edits& Edits::संक्रियकः=(ध्रुव Edits& other) U_LIFETIME_BOUND {
    यदि (एतत् == &other) { प्रत्यागम *एतत्; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    अङ्कः_Changes = other.अङ्कः_Changes;
    error_संहिता_2 = other.error_संहिता_2;
    प्रत्यागम copyArray(other);
}

Edits& Edits::संक्रियकः=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    अङ्कः_Changes = src.अङ्कः_Changes;
    error_संहिता_2 = src.error_संहिता_2;
    प्रत्यागम moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

शून्य Edits::reset() noexcept {
    length = delta = अङ्कः_Changes = 0;
    error_संहिता_2 = U_ZERO_ERROR;
}

शून्य Edits::सङ्कलय_Unchanged(int32_t unchanged_दीर्घता) {
    यदि(U_FAILURE(error_संहिता_2) || unchanged_दीर्घता == 0) { प्रत्यागम; }
    यदि(unchanged_दीर्घता < 0) {
        error_संहिता_2 = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t चरमः = चरमः_Unit();
    यदि(चरमः < MAX_UNCHANGED) {
        int32_t remaining = MAX_UNCHANGED - चरमः;
        यदि (remaining >= unchanged_दीर्घता) {
            समुच्चयः_चरमः_Unit(चरमः + unchanged_दीर्घता);
            प्रत्यागम;
        }
        समुच्चयः_चरमः_Unit(MAX_UNCHANGED);
        unchanged_दीर्घता -= remaining;
    }
    // Split large lengths into multiple units.
    यावत्(unchanged_दीर्घता >= MAX_UNCHANGED_दीर्घता) {
        append(MAX_UNCHANGED);
        unchanged_दीर्घता -= MAX_UNCHANGED_दीर्घता;
    }
    // Write a small (remaining) length.
    यदि(unchanged_दीर्घता > 0) {
        append(unchanged_दीर्घता - 1);
    }
}

शून्य Edits::सङ्कलय_Replace(int32_t old_दीर्घता, int32_t new_दीर्घता) {
    यदि(U_FAILURE(error_संहिता_2)) { प्रत्यागम; }
    यदि(old_दीर्घता < 0 || new_दीर्घता < 0) {
        error_संहिता_2 = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि (old_दीर्घता == 0 && new_दीर्घता == 0) {
        प्रत्यागम;
    }
    ++अङ्कः_Changes;
    int32_t newDelta = new_दीर्घता - old_दीर्घता;
    यदि (newDelta != 0) {
        यदि ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_संहिता_2 = U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम;
        }
        delta += newDelta;
    }

    यदि(0 < old_दीर्घता && old_दीर्घता <= MAX_SHORT_CHANGE_OLD_दीर्घता &&
            new_दीर्घता <= MAX_SHORT_CHANGE_NEW_दीर्घता) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_दीर्घता << 12) | (new_दीर्घता << 9);
        int32_t चरमः = चरमः_Unit();
        यदि(MAX_UNCHANGED < चरमः && चरमः < MAX_SHORT_CHANGE &&
                (चरमः & ~SHORT_CHANGE_अङ्कः_आवरणम्) == u &&
                (चरमः & SHORT_CHANGE_अङ्कः_आवरणम्) < SHORT_CHANGE_अङ्कः_आवरणम्) {
            समुच्चयः_चरमः_Unit(चरमः + 1);
            प्रत्यागम;
        }
        append(u);
        प्रत्यागम;
    }

    int32_t शीर्ष = 0x7000;
    यदि (old_दीर्घता < दीर्घता_अन्तर्_1_TRAIL && new_दीर्घता < दीर्घता_अन्तर्_1_TRAIL) {
        शीर्ष |= old_दीर्घता << 6;
        शीर्ष |= new_दीर्घता;
        append(शीर्ष);
    } अन्यथा यदि ((capacity - length) >= 5 || growArray()) {
        int32_t सीमा = length + 1;
        यदि(old_दीर्घता < दीर्घता_अन्तर्_1_TRAIL) {
            शीर्ष |= old_दीर्घता << 6;
        } अन्यथा यदि(old_दीर्घता <= 0x7fff) {
            शीर्ष |= दीर्घता_अन्तर्_1_TRAIL << 6;
            array[सीमा++] = static_cast<uint16_t>(0x8000 | old_दीर्घता);
        } अन्यथा {
            शीर्ष |= (दीर्घता_अन्तर्_2_TRAIL + (old_दीर्घता >> 30)) << 6;
            array[सीमा++] = static_cast<uint16_t>(0x8000 | (old_दीर्घता >> 15));
            array[सीमा++] = static_cast<uint16_t>(0x8000 | old_दीर्घता);
        }
        यदि(new_दीर्घता < दीर्घता_अन्तर्_1_TRAIL) {
            शीर्ष |= new_दीर्घता;
        } अन्यथा यदि(new_दीर्घता <= 0x7fff) {
            शीर्ष |= दीर्घता_अन्तर्_1_TRAIL;
            array[सीमा++] = static_cast<uint16_t>(0x8000 | new_दीर्घता);
        } अन्यथा {
            शीर्ष |= दीर्घता_अन्तर्_2_TRAIL + (new_दीर्घता >> 30);
            array[सीमा++] = static_cast<uint16_t>(0x8000 | (new_दीर्घता >> 15));
            array[सीमा++] = static_cast<uint16_t>(0x8000 | new_दीर्घता);
        }
        array[length] = static_cast<uint16_t>(शीर्ष);
        length = सीमा;
    }
}

शून्य Edits::append(int32_t r) {
    यदि(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t new_क्षमता;
    यदि (array == राशिः_Array) {
        new_क्षमता = 2000;
    } अन्यथा यदि (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_संहिता_2 = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम असत्य;
    } अन्यथा यदि (capacity >= (INT32_MAX / 2)) {
        new_क्षमता = INT32_MAX;
    } अन्यथा {
        new_क्षमता = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    यदि ((new_क्षमता - capacity) < 5) {
        error_संहिता_2 = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम असत्य;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_क्षमता) * 2));
    यदि (newArray == शून्यसूचकः) {
        error_संहिता_2 = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम असत्य;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_क्षमता;
    प्रत्यागम सत्य;
}

UBool Edits::copy_Error_गन्तव्यम्(UError_संहिता &बहिर्गमनम्_Error_संहिता) ध्रुव {
    यदि (U_FAILURE(बहिर्गमनम्_Error_संहिता)) { प्रत्यागम सत्य; }
    यदि (U_SUCCESS(error_संहिता_2)) { प्रत्यागम असत्य; }
    बहिर्गमनम्_Error_संहिता = error_संहिता_2;
    प्रत्यागम सत्य;
}

Edits& Edits::mergeAndAppend(ध्रुव Edits& ab,
                             ध्रुव Edits& bc,
                             UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    यदि (copy_Error_गन्तव्यम्(error_संहिता)) { प्रत्यागम *एतत्; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_अग्र = सत्य, bc_Has_अग्र = सत्य;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_दीर्घता = 0, ab_b_दीर्घता = 0, bc_b_दीर्घता = 0, c_दीर्घता = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_दीर्घता = 0, pending_c_दीर्घता = 0;
    प्रति (;;) {
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
        यदि (bc_b_दीर्घता == 0) {
            यदि (bc_Has_अग्र && (bc_Has_अग्र = bcIter.next(error_संहिता)) != 0) {
                bc_b_दीर्घता = bcIter.old_दीर्घता();
                c_दीर्घता = bcIter.new_दीर्घता();
                यदि (bc_b_दीर्घता == 0) {
                    // insertion
                    यदि (ab_b_दीर्घता == 0 || !abIter.hasChange()) {
                        सङ्कलय_Replace(pending_a_दीर्घता, pending_c_दीर्घता + c_दीर्घता);
                        pending_a_दीर्घता = pending_c_दीर्घता = 0;
                    } अन्यथा {
                        pending_c_दीर्घता += c_दीर्घता;
                    }
                    अनुवर्त;
                }
            }
            // else see if the other iterator is done, too.
        }
        यदि (ab_b_दीर्घता == 0) {
            यदि (ab_Has_अग्र && (ab_Has_अग्र = abIter.next(error_संहिता)) != 0) {
                a_दीर्घता = abIter.old_दीर्घता();
                ab_b_दीर्घता = abIter.new_दीर्घता();
                यदि (ab_b_दीर्घता == 0) {
                    // deletion
                    यदि (bc_b_दीर्घता == bcIter.old_दीर्घता() || !bcIter.hasChange()) {
                        सङ्कलय_Replace(pending_a_दीर्घता + a_दीर्घता, pending_c_दीर्घता);
                        pending_a_दीर्घता = pending_c_दीर्घता = 0;
                    } अन्यथा {
                        pending_a_दीर्घता += a_दीर्घता;
                    }
                    अनुवर्त;
                }
            } अन्यथा यदि (bc_b_दीर्घता == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                भङ्ग;
            } अन्यथा {
                // The ab output string is shorter than the bc input string.
                यदि (!copy_Error_गन्तव्यम्(error_संहिता)) {
                    error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                }
                प्रत्यागम *एतत्;
            }
        }
        यदि (bc_b_दीर्घता == 0) {
            // The bc input string is shorter than the ab output string.
            यदि (!copy_Error_गन्तव्यम्(error_संहिता)) {
                error_संहिता = U_ILLEGAL_तर्कः_ERROR;
            }
            प्रत्यागम *एतत्;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        यदि (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            यदि (pending_a_दीर्घता != 0 || pending_c_दीर्घता != 0) {
                सङ्कलय_Replace(pending_a_दीर्घता, pending_c_दीर्घता);
                pending_a_दीर्घता = pending_c_दीर्घता = 0;
            }
            int32_t unchanged_दीर्घता = a_दीर्घता <= c_दीर्घता ? a_दीर्घता : c_दीर्घता;
            सङ्कलय_Unchanged(unchanged_दीर्घता);
            ab_b_दीर्घता = a_दीर्घता -= unchanged_दीर्घता;
            bc_b_दीर्घता = c_दीर्घता -= unchanged_दीर्घता;
            // At least one of the unchanged spans is now empty.
            अनुवर्त;
        }
        यदि (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            यदि (ab_b_दीर्घता >= bc_b_दीर्घता) {
                // Split the longer unchanged span into change + remainder.
                सङ्कलय_Replace(pending_a_दीर्घता + bc_b_दीर्घता, pending_c_दीर्घता + c_दीर्घता);
                pending_a_दीर्घता = pending_c_दीर्घता = 0;
                a_दीर्घता = ab_b_दीर्घता -= bc_b_दीर्घता;
                bc_b_दीर्घता = 0;
                अनुवर्त;
            }
            // Handle the shorter unchanged span below like a change.
        } अन्यथा यदि (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            यदि (ab_b_दीर्घता <= bc_b_दीर्घता) {
                // Split the longer unchanged span into change + remainder.
                सङ्कलय_Replace(pending_a_दीर्घता + a_दीर्घता, pending_c_दीर्घता + ab_b_दीर्घता);
                pending_a_दीर्घता = pending_c_दीर्घता = 0;
                c_दीर्घता = bc_b_दीर्घता -= ab_b_दीर्घता;
                ab_b_दीर्घता = 0;
                अनुवर्त;
            }
            // Handle the shorter unchanged span below like a change.
        } अन्यथा {  // both abIter.hasChange() && bcIter.hasChange()
            यदि (ab_b_दीर्घता == bc_b_दीर्घता) {
                // Changes on both sides up to the same position. Emit & reset.
                सङ्कलय_Replace(pending_a_दीर्घता + a_दीर्घता, pending_c_दीर्घता + c_दीर्घता);
                pending_a_दीर्घता = pending_c_दीर्घता = 0;
                ab_b_दीर्घता = bc_b_दीर्घता = 0;
                अनुवर्त;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_दीर्घता += a_दीर्घता;
        pending_c_दीर्घता += c_दीर्घता;
        यदि (ab_b_दीर्घता < bc_b_दीर्घता) {
            bc_b_दीर्घता -= ab_b_दीर्घता;
            c_दीर्घता = ab_b_दीर्घता = 0;
        } अन्यथा {  // ab_bLength > bc_bLength
            ab_b_दीर्घता -= bc_b_दीर्घता;
            a_दीर्घता = bc_b_दीर्घता = 0;
        }
    }
    यदि (pending_a_दीर्घता != 0 || pending_c_दीर्घता != 0) {
        सङ्कलय_Replace(pending_a_दीर्घता, pending_c_दीर्घता);
    }
    copy_Error_गन्तव्यम्(error_संहिता);
    प्रत्यागम *एतत्;
}

Edits::Iterator::Iterator(ध्रुव uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), सूचकांक(0), length(len), remaining(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), परिवर्तितम्(असत्य), old_दीर्घता_2(0), new_दीर्घता_2(0),
        src_सूचकांक(0), repl_सूचकांक(0), dest_सूचकांक(0) {}

int32_t Edits::Iterator::read_दीर्घता(int32_t शीर्ष) {
    यदि (शीर्ष < दीर्घता_अन्तर्_1_TRAIL) {
        प्रत्यागम शीर्ष;
    } अन्यथा यदि (शीर्ष < दीर्घता_अन्तर्_2_TRAIL) {
        U_ASSERT(सूचकांक < length);
        U_ASSERT(array[सूचकांक] >= 0x8000);
        प्रत्यागम array[सूचकांक++] & 0x7fff;
    } अन्यथा {
        U_ASSERT((सूचकांक + 2) <= length);
        U_ASSERT(array[सूचकांक] >= 0x8000);
        U_ASSERT(array[सूचकांक + 1] >= 0x8000);
        int32_t len = ((शीर्ष & 1) << 30) |
                (static_cast<int32_t>(array[सूचकांक] & 0x7fff) << 15) |
                (array[सूचकांक + 1] & 0x7fff);
        सूचकांक += 2;
        प्रत्यागम len;
    }
}

शून्य Edits::Iterator::update_अग्र_Indexes() {
    src_सूचकांक += old_दीर्घता_2;
    यदि (परिवर्तितम्) {
        repl_सूचकांक += new_दीर्घता_2;
    }
    dest_सूचकांक += new_दीर्घता_2;
}

शून्य Edits::Iterator::updatePreviousIndexes() {
    src_सूचकांक -= old_दीर्घता_2;
    यदि (परिवर्तितम्) {
        repl_सूचकांक -= new_दीर्घता_2;
    }
    dest_सूचकांक -= new_दीर्घता_2;
}

UBool Edits::Iterator::no_अग्र() {
    // No change before or beyond the string.
    dir = 0;
    परिवर्तितम् = असत्य;
    old_दीर्घता_2 = new_दीर्घता_2 = 0;
    प्रत्यागम असत्य;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_संहिता &error_संहिता) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    यदि (dir > 0) {
        update_अग्र_Indexes();
    } अन्यथा {
        यदि (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            यदि (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++सूचकांक;  // next() rests on the index after the sequence unit.
                dir = 1;
                प्रत्यागम सत्य;
            }
        }
        dir = 1;
    }
    यदि (remaining >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        यदि (remaining > 1) {
            --remaining;
            प्रत्यागम सत्य;
        }
        remaining = 0;
    }
    यदि (सूचकांक >= length) {
        प्रत्यागम no_अग्र();
    }
    int32_t u = array[सूचकांक++];
    यदि (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        परिवर्तितम् = असत्य;
        old_दीर्घता_2 = u + 1;
        यावत् (सूचकांक < length && (u = array[सूचकांक]) <= MAX_UNCHANGED) {
            ++सूचकांक;
            old_दीर्घता_2 += u + 1;
        }
        new_दीर्घता_2 = old_दीर्घता_2;
        यदि (onlyChanges) {
            update_अग्र_Indexes();
            यदि (सूचकांक >= length) {
                प्रत्यागम no_अग्र();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++सूचकांक;
        } अन्यथा {
            प्रत्यागम सत्य;
        }
    }
    परिवर्तितम् = सत्य;
    यदि (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_दीर्घता;
        int32_t num = (u & SHORT_CHANGE_अङ्कः_आवरणम्) + 1;
        यदि (coarse) {
            old_दीर्घता_2 = num * oldLen;
            new_दीर्घता_2 = num * newLen;
        } अन्यथा {
            // Split a sequence of changes that was compressed into one unit.
            old_दीर्घता_2 = oldLen;
            new_दीर्घता_2 = newLen;
            यदि (num > 1) {
                remaining = num;  // This is the first of two or more changes.
            }
            प्रत्यागम सत्य;
        }
    } अन्यथा {
        U_ASSERT(u <= 0x7fff);
        old_दीर्घता_2 = read_दीर्घता((u >> 6) & 0x3f);
        new_दीर्घता_2 = read_दीर्घता(u & 0x3f);
        यदि (!coarse) {
            प्रत्यागम सत्य;
        }
    }
    // Combine adjacent changes.
    यावत् (सूचकांक < length && (u = array[सूचकांक]) > MAX_UNCHANGED) {
        ++सूचकांक;
        यदि (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अङ्कः_आवरणम्) + 1;
            old_दीर्घता_2 += (u >> 12) * num;
            new_दीर्घता_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_दीर्घता) * num;
        } अन्यथा {
            U_ASSERT(u <= 0x7fff);
            old_दीर्घता_2 += read_दीर्घता((u >> 6) & 0x3f);
            new_दीर्घता_2 += read_दीर्घता(u & 0x3f);
        }
    }
    प्रत्यागम सत्य;
}

UBool Edits::Iterator::previous(UError_संहिता &error_संहिता) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    यदि (dir >= 0) {
        यदि (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            यदि (remaining > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --सूचकांक;  // previous() rests on the sequence unit.
                dir = -1;
                प्रत्यागम सत्य;
            }
            update_अग्र_Indexes();
        }
        dir = -1;
    }
    यदि (remaining > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[सूचकांक];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        यदि (remaining <= (u & SHORT_CHANGE_अङ्कः_आवरणम्)) {
            ++remaining;
            updatePreviousIndexes();
            प्रत्यागम सत्य;
        }
        remaining = 0;
    }
    यदि (सूचकांक <= 0) {
        प्रत्यागम no_अग्र();
    }
    int32_t u = array[--सूचकांक];
    यदि (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        परिवर्तितम् = असत्य;
        old_दीर्घता_2 = u + 1;
        यावत् (सूचकांक > 0 && (u = array[सूचकांक - 1]) <= MAX_UNCHANGED) {
            --सूचकांक;
            old_दीर्घता_2 += u + 1;
        }
        new_दीर्घता_2 = old_दीर्घता_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        updatePreviousIndexes();
        प्रत्यागम सत्य;
    }
    परिवर्तितम् = सत्य;
    यदि (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_दीर्घता;
        int32_t num = (u & SHORT_CHANGE_अङ्कः_आवरणम्) + 1;
        यदि (coarse) {
            old_दीर्घता_2 = num * oldLen;
            new_दीर्घता_2 = num * newLen;
        } अन्यथा {
            // Split a sequence of changes that was compressed into one unit.
            old_दीर्घता_2 = oldLen;
            new_दीर्घता_2 = newLen;
            यदि (num > 1) {
                remaining = 1;  // This is the last of two or more changes.
            }
            updatePreviousIndexes();
            प्रत्यागम सत्य;
        }
    } अन्यथा {
        यदि (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_दीर्घता_2 = read_दीर्घता((u >> 6) & 0x3f);
            new_दीर्घता_2 = read_दीर्घता(u & 0x3f);
        } अन्यथा {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(सूचकांक > 0);
            यावत् ((u = array[--सूचकांक]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t शीर्ष_सूचकांक = सूचकांक++;
            old_दीर्घता_2 = read_दीर्घता((u >> 6) & 0x3f);
            new_दीर्घता_2 = read_दीर्घता(u & 0x3f);
            सूचकांक = शीर्ष_सूचकांक;
        }
        यदि (!coarse) {
            updatePreviousIndexes();
            प्रत्यागम सत्य;
        }
    }
    // Combine adjacent changes.
    यावत् (सूचकांक > 0 && (u = array[सूचकांक - 1]) > MAX_UNCHANGED) {
        --सूचकांक;
        यदि (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_अङ्कः_आवरणम्) + 1;
            old_दीर्घता_2 += (u >> 12) * num;
            new_दीर्घता_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_दीर्घता) * num;
        } अन्यथा यदि (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t शीर्ष_सूचकांक = सूचकांक++;
            old_दीर्घता_2 += read_दीर्घता((u >> 6) & 0x3f);
            new_दीर्घता_2 += read_दीर्घता(u & 0x3f);
            सूचकांक = शीर्ष_सूचकांक;
        }
    }
    updatePreviousIndexes();
    प्रत्यागम सत्य;
}

int32_t Edits::Iterator::find_सूचकांक(int32_t i, UBool findSource, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता) || i < 0) { प्रत्यागम -1; }
    int32_t विस्तृतिः_Start, विस्तृतिः_दीर्घता;
    यदि (findSource) {  // find source index
        विस्तृतिः_Start = src_सूचकांक;
        विस्तृतिः_दीर्घता = old_दीर्घता_2;
    } अन्यथा {  // find destination index
        विस्तृतिः_Start = dest_सूचकांक;
        विस्तृतिः_दीर्घता = new_दीर्घता_2;
    }
    यदि (i < विस्तृतिः_Start) {
        यदि (i >= (विस्तृतिः_Start / 2)) {
            // Search backwards.
            प्रति (;;) {
                UBool hasPrevious = previous(error_संहिता);
                U_ASSERT(hasPrevious);  // because i>=0 and the first span starts at 0
                (शून्य)hasPrevious;  // avoid unused-variable warning
                विस्तृतिः_Start = findSource ? src_सूचकांक : dest_सूचकांक;
                यदि (i >= विस्तृतिः_Start) {
                    // The index is in the current span.
                    प्रत्यागम 0;
                }
                यदि (remaining > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    विस्तृतिः_दीर्घता = findSource ? old_दीर्घता_2 : new_दीर्घता_2;
                    int32_t u = array[सूचकांक];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_अङ्कः_आवरणम्) + 1 - remaining;
                    int32_t len = num * विस्तृतिः_दीर्घता;
                    यदि (i >= (विस्तृतिः_Start - len)) {
                        int32_t n = ((विस्तृतिः_Start - i - 1) / विस्तृतिः_दीर्घता) + 1;
                        // 1 <= n <= num
                        src_सूचकांक -= n * old_दीर्घता_2;
                        repl_सूचकांक -= n * new_दीर्घता_2;
                        dest_सूचकांक -= n * new_दीर्घता_2;
                        remaining += n;
                        प्रत्यागम 0;
                    }
                    // Skip all of these edits at once.
                    src_सूचकांक -= num * old_दीर्घता_2;
                    repl_सूचकांक -= num * new_दीर्घता_2;
                    dest_सूचकांक -= num * new_दीर्घता_2;
                    remaining = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        सूचकांक = remaining = old_दीर्घता_2 = new_दीर्घता_2 = src_सूचकांक = repl_सूचकांक = dest_सूचकांक = 0;
    } अन्यथा यदि (i < (विस्तृतिः_Start + विस्तृतिः_दीर्घता)) {
        // The index is in the current span.
        प्रत्यागम 0;
    }
    यावत् (next(असत्य, error_संहिता)) {
        यदि (findSource) {
            विस्तृतिः_Start = src_सूचकांक;
            विस्तृतिः_दीर्घता = old_दीर्घता_2;
        } अन्यथा {
            विस्तृतिः_Start = dest_सूचकांक;
            विस्तृतिः_दीर्घता = new_दीर्घता_2;
        }
        यदि (i < (विस्तृतिः_Start + विस्तृतिः_दीर्घता)) {
            // The index is in the current span.
            प्रत्यागम 0;
        }
        यदि (remaining > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = remaining * विस्तृतिः_दीर्घता;
            यदि (i < (विस्तृतिः_Start + len)) {
                int32_t n = (i - विस्तृतिः_Start) / विस्तृतिः_दीर्घता;  // 1 <= n <= remaining - 1
                src_सूचकांक += n * old_दीर्घता_2;
                repl_सूचकांक += n * new_दीर्घता_2;
                dest_सूचकांक += n * new_दीर्घता_2;
                remaining -= n;
                प्रत्यागम 0;
            }
            // Make next() skip all of these edits at once.
            old_दीर्घता_2 *= remaining;
            new_दीर्घता_2 *= remaining;
            remaining = 0;
        }
    }
    प्रत्यागम 1;
}

int32_t Edits::Iterator::destination_सूचकांक_From_Source_सूचकांक(int32_t i, UError_संहिता &error_संहिता) {
    int32_t where = find_सूचकांक(i, सत्य, error_संहिता);
    यदि (where < 0) {
        // Error or before the string.
        प्रत्यागम 0;
    }
    यदि (where > 0 || i == src_सूचकांक) {
        // At or after string length, or at start of the found span.
        प्रत्यागम dest_सूचकांक;
    }
    यदि (परिवर्तितम्) {
        // In a change span, map to its end.
        प्रत्यागम dest_सूचकांक + new_दीर्घता_2;
    } अन्यथा {
        // In an unchanged span, offset 1:1 within it.
        प्रत्यागम dest_सूचकांक + (i - src_सूचकांक);
    }
}

int32_t Edits::Iterator::source_सूचकांक_From_Destination_सूचकांक(int32_t i, UError_संहिता &error_संहिता) {
    int32_t where = find_सूचकांक(i, असत्य, error_संहिता);
    यदि (where < 0) {
        // Error or before the string.
        प्रत्यागम 0;
    }
    यदि (where > 0 || i == dest_सूचकांक) {
        // At or after string length, or at start of the found span.
        प्रत्यागम src_सूचकांक;
    }
    यदि (परिवर्तितम्) {
        // In a change span, map to its end.
        प्रत्यागम src_सूचकांक + old_दीर्घता_2;
    } अन्यथा {
        // In an unchanged span, offset within it.
        प्रत्यागम src_सूचकांक + (i - dest_सूचकांक);
    }
}

UnicodeString& Edits::Iterator::गन्तव्यम्_String(UnicodeString& sb U_LIFETIME_BOUND) ध्रुव {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_सङ्ख्या(sb, src_सूचकांक);
    sb.append(u"..", -1);
    ICU_Utility::append_सङ्ख्या(sb, src_सूचकांक + old_दीर्घता_2);
    यदि (परिवर्तितम्) {
        sb.append(u"] ⇝ dest[", -1);
    } अन्यथा {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_सङ्ख्या(sb, dest_सूचकांक);
    sb.append(u"..", -1);
    ICU_Utility::append_सङ्ख्या(sb, dest_सूचकांक + new_दीर्घता_2);
    यदि (परिवर्तितम्) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_सङ्ख्या(sb, repl_सूचकांक);
        sb.append(u"..", -1);
        ICU_Utility::append_सङ्ख्या(sb, repl_सूचकांक + new_दीर्घता_2);
        sb.append(u"] }", -1);
    } अन्यथा {
        sb.append(u"] (no-change) }", -1);
    }
    प्रत्यागम sb;
}

U_NAMESPACE_अन्तः
