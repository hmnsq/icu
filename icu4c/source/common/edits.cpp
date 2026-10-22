// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.cpp
// created: 2017feb08 Markus W. Scherer

#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "util.h"

U_NAMESPACE_BEGIN

నామాకాశ {

// 0000uuuuuuuuuuuu records u+1 unchanged text units.
స్థిరరాశి int32_t MAX_UNCHANGED_నిడివి = 0x1000;
స్థిరరాశి int32_t MAX_UNCHANGED = MAX_UNCHANGED_నిడివి - 1;

// 0mmmnnnccccccccc with m=1..6 records ccc+1 replacements of m:n text units.
స్థిరరాశి int32_t MAX_SHORT_CHANGE_OLD_నిడివి = 6;
స్థిరరాశి int32_t MAX_SHORT_CHANGE_NEW_నిడివి = 7;
స్థిరరాశి int32_t SHORT_CHANGE_అంకె_ముసుగు = 0x1ff;
స్థిరరాశి int32_t MAX_SHORT_CHANGE = 0x6fff;

// 0111mmmmmmnnnnnn records a replacement of m text units with n.
// m or n = 61: actual length follows in the next edits array unit.
// m or n = 62..63: actual length follows in the next two edits array units.
// Bit 30 of the actual length is in the head unit.
// Trailing units have bit 15 set.
స్థిరరాశి int32_t నిడివి_లోపల_1_TRAIL = 61;
స్థిరరాశి int32_t నిడివి_లోపల_2_TRAIL = 62;

}  // namespace

శూన్య Edits::releaseArray() noexcept {
    ఒకవేళ (array != దొంతర_Array) {
        uprv_free(array);
    }
}

Edits& Edits::copyArray(స్థిరరాశి Edits& other) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) {
        length = delta = అంకె_Changes = 0;
        తిరిగి *ఇది;
    }
    ఒకవేళ (length > capacity) {
        uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(length) * 2));
        ఒకవేళ (newArray == శూన్యసూచిక) {
            length = delta = అంకె_Changes = 0;
            error_సంకేతం_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి *ఇది;
        }
        releaseArray();
        array = newArray;
        capacity = length;
    }
    ఒకవేళ (length > 0) {
        uprv_memcpy(array, other.array, (size_t)length * 2);
    }
    తిరిగి *ఇది;
}

Edits& Edits::moveArray(Edits& src) noexcept U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) {
        length = delta = అంకె_Changes = 0;
        తిరిగి *ఇది;
    }
    releaseArray();
    ఒకవేళ (length > దొంతర_సామర్థ్యం_2) {
        array = src.array;
        capacity = src.capacity;
        src.array = src.దొంతర_Array;
        src.capacity = దొంతర_సామర్థ్యం_2;
        src.reset();
        తిరిగి *ఇది;
    }
    array = దొంతర_Array;
    capacity = దొంతర_సామర్థ్యం_2;
    ఒకవేళ (length > 0) {
        uprv_memcpy(array, src.array, (size_t)length * 2);
    }
    తిరిగి *ఇది;
}

Edits& Edits::క్రియాకారకం=(స్థిరరాశి Edits& other) U_LIFETIME_BOUND {
    ఒకవేళ (ఇది == &other) { తిరిగి *ఇది; }  // self-assignment: no-op
    length = other.length;
    delta = other.delta;
    అంకె_Changes = other.అంకె_Changes;
    error_సంకేతం_2 = other.error_సంకేతం_2;
    తిరిగి copyArray(other);
}

Edits& Edits::క్రియాకారకం=(Edits&& src) noexcept U_LIFETIME_BOUND {
    length = src.length;
    delta = src.delta;
    అంకె_Changes = src.అంకె_Changes;
    error_సంకేతం_2 = src.error_సంకేతం_2;
    తిరిగి moveArray(src);
}

Edits::~Edits() {
    releaseArray();
}

శూన్య Edits::reset() noexcept {
    length = delta = అంకె_Changes = 0;
    error_సంకేతం_2 = U_ZERO_ERROR;
}

శూన్య Edits::కూడు_Unchanged(int32_t unchanged_నిడివి) {
    ఒకవేళ(U_FAILURE(error_సంకేతం_2) || unchanged_నిడివి == 0) { తిరిగి; }
    ఒకవేళ(unchanged_నిడివి < 0) {
        error_సంకేతం_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    // Merge into previous unchanged-text record, if any.
    int32_t చివరి = చివరి_Unit();
    ఒకవేళ(చివరి < MAX_UNCHANGED) {
        int32_t మిగిలిన = MAX_UNCHANGED - చివరి;
        ఒకవేళ (మిగిలిన >= unchanged_నిడివి) {
            సమితి_చివరి_Unit(చివరి + unchanged_నిడివి);
            తిరిగి;
        }
        సమితి_చివరి_Unit(MAX_UNCHANGED);
        unchanged_నిడివి -= మిగిలిన;
    }
    // Split large lengths into multiple units.
    అయితే(unchanged_నిడివి >= MAX_UNCHANGED_నిడివి) {
        append(MAX_UNCHANGED);
        unchanged_నిడివి -= MAX_UNCHANGED_నిడివి;
    }
    // Write a small (remaining) length.
    ఒకవేళ(unchanged_నిడివి > 0) {
        append(unchanged_నిడివి - 1);
    }
}

శూన్య Edits::కూడు_Replace(int32_t old_నిడివి, int32_t new_నిడివి) {
    ఒకవేళ(U_FAILURE(error_సంకేతం_2)) { తిరిగి; }
    ఒకవేళ(old_నిడివి < 0 || new_నిడివి < 0) {
        error_సంకేతం_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ (old_నిడివి == 0 && new_నిడివి == 0) {
        తిరిగి;
    }
    ++అంకె_Changes;
    int32_t newDelta = new_నిడివి - old_నిడివి;
    ఒకవేళ (newDelta != 0) {
        ఒకవేళ ((newDelta > 0 && delta >= 0 && newDelta > (INT32_MAX - delta)) ||
                (newDelta < 0 && delta < 0 && newDelta < (INT32_MIN - delta))) {
            // Integer overflow or underflow.
            error_సంకేతం_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి;
        }
        delta += newDelta;
    }

    ఒకవేళ(0 < old_నిడివి && old_నిడివి <= MAX_SHORT_CHANGE_OLD_నిడివి &&
            new_నిడివి <= MAX_SHORT_CHANGE_NEW_నిడివి) {
        // Merge into previous same-lengths short-replacement record, if any.
        int32_t u = (old_నిడివి << 12) | (new_నిడివి << 9);
        int32_t చివరి = చివరి_Unit();
        ఒకవేళ(MAX_UNCHANGED < చివరి && చివరి < MAX_SHORT_CHANGE &&
                (చివరి & ~SHORT_CHANGE_అంకె_ముసుగు) == u &&
                (చివరి & SHORT_CHANGE_అంకె_ముసుగు) < SHORT_CHANGE_అంకె_ముసుగు) {
            సమితి_చివరి_Unit(చివరి + 1);
            తిరిగి;
        }
        append(u);
        తిరిగి;
    }

    int32_t తల = 0x7000;
    ఒకవేళ (old_నిడివి < నిడివి_లోపల_1_TRAIL && new_నిడివి < నిడివి_లోపల_1_TRAIL) {
        తల |= old_నిడివి << 6;
        తల |= new_నిడివి;
        append(తల);
    } లేకపోతే ఒకవేళ ((capacity - length) >= 5 || growArray()) {
        int32_t హద్దు = length + 1;
        ఒకవేళ(old_నిడివి < నిడివి_లోపల_1_TRAIL) {
            తల |= old_నిడివి << 6;
        } లేకపోతే ఒకవేళ(old_నిడివి <= 0x7fff) {
            తల |= నిడివి_లోపల_1_TRAIL << 6;
            array[హద్దు++] = static_cast<uint16_t>(0x8000 | old_నిడివి);
        } లేకపోతే {
            తల |= (నిడివి_లోపల_2_TRAIL + (old_నిడివి >> 30)) << 6;
            array[హద్దు++] = static_cast<uint16_t>(0x8000 | (old_నిడివి >> 15));
            array[హద్దు++] = static_cast<uint16_t>(0x8000 | old_నిడివి);
        }
        ఒకవేళ(new_నిడివి < నిడివి_లోపల_1_TRAIL) {
            తల |= new_నిడివి;
        } లేకపోతే ఒకవేళ(new_నిడివి <= 0x7fff) {
            తల |= నిడివి_లోపల_1_TRAIL;
            array[హద్దు++] = static_cast<uint16_t>(0x8000 | new_నిడివి);
        } లేకపోతే {
            తల |= నిడివి_లోపల_2_TRAIL + (new_నిడివి >> 30);
            array[హద్దు++] = static_cast<uint16_t>(0x8000 | (new_నిడివి >> 15));
            array[హద్దు++] = static_cast<uint16_t>(0x8000 | new_నిడివి);
        }
        array[length] = static_cast<uint16_t>(తల);
        length = హద్దు;
    }
}

శూన్య Edits::append(int32_t r) {
    ఒకవేళ(length < capacity || growArray()) {
        array[length++] = static_cast<uint16_t>(r);
    }
}

UBool Edits::growArray() {
    int32_t new_సామర్థ్యం;
    ఒకవేళ (array == దొంతర_Array) {
        new_సామర్థ్యం = 2000;
    } లేకపోతే ఒకవేళ (capacity == INT32_MAX) {
        // Not U_BUFFER_OVERFLOW_ERROR because that could be confused on a string transform API
        // with a result-string-buffer overflow.
        error_సంకేతం_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి అబద్ధం;
    } లేకపోతే ఒకవేళ (capacity >= (INT32_MAX / 2)) {
        new_సామర్థ్యం = INT32_MAX;
    } లేకపోతే {
        new_సామర్థ్యం = 2 * capacity;
    }
    // Grow by at least 5 units so that a maximal change record will fit.
    ఒకవేళ ((new_సామర్థ్యం - capacity) < 5) {
        error_సంకేతం_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి అబద్ధం;
    }
    uint16_t* newArray = static_cast<uint16_t*>(uprv_malloc(static_cast<size_t>(new_సామర్థ్యం) * 2));
    ఒకవేళ (newArray == శూన్యసూచిక) {
        error_సంకేతం_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి అబద్ధం;
    }
    uprv_memcpy(newArray, array, (size_t)length * 2);
    releaseArray();
    array = newArray;
    capacity = new_సామర్థ్యం;
    తిరిగి నిజం;
}

UBool Edits::copy_Error_గమ్యం(UError_సంకేతం &బహిర్గతం_Error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(బహిర్గతం_Error_సంకేతం)) { తిరిగి నిజం; }
    ఒకవేళ (U_SUCCESS(error_సంకేతం_2)) { తిరిగి అబద్ధం; }
    బహిర్గతం_Error_సంకేతం = error_సంకేతం_2;
    తిరిగి నిజం;
}

Edits& Edits::విలీనం_And_Append(స్థిరరాశి Edits& ab,
                             స్థిరరాశి Edits& bc,
                             UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    ఒకవేళ (copy_Error_గమ్యం(error_సంకేతం)) { తిరిగి *ఇది; }
    // Picture string a --(Edits ab)--> string b --(Edits bc)--> string c.
    // Parallel iteration over both Edits.
    Iterator abIter = ab.getFineIterator();
    Iterator bcIter = bc.getFineIterator();
    UBool ab_Has_తదుపరి = నిజం, bc_Has_తదుపరి = నిజం;
    // Copy iterator state into local variables, so that we can modify and subdivide spans.
    // ab old & new length, bc old & new length
    int32_t a_నిడివి = 0, ab_b_నిడివి = 0, bc_b_నిడివి = 0, c_నిడివి = 0;
    // When we have different-intermediate-length changes, we accumulate a larger change.
    int32_t pending_a_నిడివి = 0, pending_c_నిడివి = 0;
    కొరకు (;;) {
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
        ఒకవేళ (bc_b_నిడివి == 0) {
            ఒకవేళ (bc_Has_తదుపరి && (bc_Has_తదుపరి = bcIter.next(error_సంకేతం)) != 0) {
                bc_b_నిడివి = bcIter.old_నిడివి();
                c_నిడివి = bcIter.new_నిడివి();
                ఒకవేళ (bc_b_నిడివి == 0) {
                    // insertion
                    ఒకవేళ (ab_b_నిడివి == 0 || !abIter.hasChange()) {
                        కూడు_Replace(pending_a_నిడివి, pending_c_నిడివి + c_నిడివి);
                        pending_a_నిడివి = pending_c_నిడివి = 0;
                    } లేకపోతే {
                        pending_c_నిడివి += c_నిడివి;
                    }
                    కొనసాగు;
                }
            }
            // else see if the other iterator is done, too.
        }
        ఒకవేళ (ab_b_నిడివి == 0) {
            ఒకవేళ (ab_Has_తదుపరి && (ab_Has_తదుపరి = abIter.next(error_సంకేతం)) != 0) {
                a_నిడివి = abIter.old_నిడివి();
                ab_b_నిడివి = abIter.new_నిడివి();
                ఒకవేళ (ab_b_నిడివి == 0) {
                    // deletion
                    ఒకవేళ (bc_b_నిడివి == bcIter.old_నిడివి() || !bcIter.hasChange()) {
                        కూడు_Replace(pending_a_నిడివి + a_నిడివి, pending_c_నిడివి);
                        pending_a_నిడివి = pending_c_నిడివి = 0;
                    } లేకపోతే {
                        pending_a_నిడివి += a_నిడివి;
                    }
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ (bc_b_నిడివి == 0) {
                // Both iterators are done at the same time:
                // The intermediate-string lengths match.
                ఆపు;
            } లేకపోతే {
                // The ab output string is shorter than the bc input string.
                ఒకవేళ (!copy_Error_గమ్యం(error_సంకేతం)) {
                    error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                }
                తిరిగి *ఇది;
            }
        }
        ఒకవేళ (bc_b_నిడివి == 0) {
            // The bc input string is shorter than the ab output string.
            ఒకవేళ (!copy_Error_గమ్యం(error_సంకేతం)) {
                error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
            }
            తిరిగి *ఇది;
        }
        //  Done fetching: ab_bLength > 0 && bc_bLength > 0

        // The current state has two parts:
        // - Past: We accumulate a longer ac edit in the "pending" variables.
        // - Current: We have copies of the current ab/bc edits in local variables.
        //   At least one side is newly fetched.
        //   One side might be a truncated remainder of an edit we fetched earlier.

        ఒకవేళ (!abIter.hasChange() && !bcIter.hasChange()) {
            // An unchanged span all the way from string a to string c.
            ఒకవేళ (pending_a_నిడివి != 0 || pending_c_నిడివి != 0) {
                కూడు_Replace(pending_a_నిడివి, pending_c_నిడివి);
                pending_a_నిడివి = pending_c_నిడివి = 0;
            }
            int32_t unchanged_నిడివి = a_నిడివి <= c_నిడివి ? a_నిడివి : c_నిడివి;
            కూడు_Unchanged(unchanged_నిడివి);
            ab_b_నిడివి = a_నిడివి -= unchanged_నిడివి;
            bc_b_నిడివి = c_నిడివి -= unchanged_నిడివి;
            // At least one of the unchanged spans is now empty.
            కొనసాగు;
        }
        ఒకవేళ (!abIter.hasChange() && bcIter.hasChange()) {
            // Unchanged a->b but changed b->c.
            ఒకవేళ (ab_b_నిడివి >= bc_b_నిడివి) {
                // Split the longer unchanged span into change + remainder.
                కూడు_Replace(pending_a_నిడివి + bc_b_నిడివి, pending_c_నిడివి + c_నిడివి);
                pending_a_నిడివి = pending_c_నిడివి = 0;
                a_నిడివి = ab_b_నిడివి -= bc_b_నిడివి;
                bc_b_నిడివి = 0;
                కొనసాగు;
            }
            // Handle the shorter unchanged span below like a change.
        } లేకపోతే ఒకవేళ (abIter.hasChange() && !bcIter.hasChange()) {
            // Changed a->b and then unchanged b->c.
            ఒకవేళ (ab_b_నిడివి <= bc_b_నిడివి) {
                // Split the longer unchanged span into change + remainder.
                కూడు_Replace(pending_a_నిడివి + a_నిడివి, pending_c_నిడివి + ab_b_నిడివి);
                pending_a_నిడివి = pending_c_నిడివి = 0;
                c_నిడివి = bc_b_నిడివి -= ab_b_నిడివి;
                ab_b_నిడివి = 0;
                కొనసాగు;
            }
            // Handle the shorter unchanged span below like a change.
        } లేకపోతే {  // both abIter.hasChange() && bcIter.hasChange()
            ఒకవేళ (ab_b_నిడివి == bc_b_నిడివి) {
                // Changes on both sides up to the same position. Emit & reset.
                కూడు_Replace(pending_a_నిడివి + a_నిడివి, pending_c_నిడివి + c_నిడివి);
                pending_a_నిడివి = pending_c_నిడివి = 0;
                ab_b_నిడివి = bc_b_నిడివి = 0;
                కొనసాగు;
            }
        }
        // Accumulate the a->c change, reset the shorter side,
        // keep a remainder of the longer one.
        pending_a_నిడివి += a_నిడివి;
        pending_c_నిడివి += c_నిడివి;
        ఒకవేళ (ab_b_నిడివి < bc_b_నిడివి) {
            bc_b_నిడివి -= ab_b_నిడివి;
            c_నిడివి = ab_b_నిడివి = 0;
        } లేకపోతే {  // ab_bLength > bc_bLength
            ab_b_నిడివి -= bc_b_నిడివి;
            a_నిడివి = bc_b_నిడివి = 0;
        }
    }
    ఒకవేళ (pending_a_నిడివి != 0 || pending_c_నిడివి != 0) {
        కూడు_Replace(pending_a_నిడివి, pending_c_నిడివి);
    }
    copy_Error_గమ్యం(error_సంకేతం);
    తిరిగి *ఇది;
}

Edits::Iterator::Iterator(స్థిరరాశి uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs) :
        array(a), సూచీ(0), length(len), మిగిలిన(0),
        onlyChanges_(oc), coarse(crs),
        dir(0), మారింది(అబద్ధం), old_నిడివి_2(0), new_నిడివి_2(0),
        src_సూచీ(0), repl_సూచీ(0), dest_సూచీ(0) {}

int32_t Edits::Iterator::read_నిడివి(int32_t తల) {
    ఒకవేళ (తల < నిడివి_లోపల_1_TRAIL) {
        తిరిగి తల;
    } లేకపోతే ఒకవేళ (తల < నిడివి_లోపల_2_TRAIL) {
        U_ASSERT(సూచీ < length);
        U_ASSERT(array[సూచీ] >= 0x8000);
        తిరిగి array[సూచీ++] & 0x7fff;
    } లేకపోతే {
        U_ASSERT((సూచీ + 2) <= length);
        U_ASSERT(array[సూచీ] >= 0x8000);
        U_ASSERT(array[సూచీ + 1] >= 0x8000);
        int32_t len = ((తల & 1) << 30) |
                (static_cast<int32_t>(array[సూచీ] & 0x7fff) << 15) |
                (array[సూచీ + 1] & 0x7fff);
        సూచీ += 2;
        తిరిగి len;
    }
}

శూన్య Edits::Iterator::update_తదుపరి_Indexes() {
    src_సూచీ += old_నిడివి_2;
    ఒకవేళ (మారింది) {
        repl_సూచీ += new_నిడివి_2;
    }
    dest_సూచీ += new_నిడివి_2;
}

శూన్య Edits::Iterator::update_మునుపటి_Indexes() {
    src_సూచీ -= old_నిడివి_2;
    ఒకవేళ (మారింది) {
        repl_సూచీ -= new_నిడివి_2;
    }
    dest_సూచీ -= new_నిడివి_2;
}

UBool Edits::Iterator::no_తదుపరి() {
    // No change before or beyond the string.
    dir = 0;
    మారింది = అబద్ధం;
    old_నిడివి_2 = new_నిడివి_2 = 0;
    తిరిగి అబద్ధం;
}

UBool Edits::Iterator::next(UBool onlyChanges, UError_సంకేతం &error_సంకేతం) {
    // Forward iteration: Update the string indexes to the limit of the current span,
    // and post-increment-read array units to assemble a new span.
    // Leaves the array index one after the last unit of that span.
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ఒకవేళ (dir > 0) {
        update_తదుపరి_Indexes();
    } లేకపోతే {
        ఒకవేళ (dir < 0) {
            // Turn around from previous() to next().
            // Post-increment-read the same span again.
            ఒకవేళ (మిగిలిన > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                ++సూచీ;  // next() rests on the index after the sequence unit.
                dir = 1;
                తిరిగి నిజం;
            }
        }
        dir = 1;
    }
    ఒకవేళ (మిగిలిన >= 1) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        ఒకవేళ (మిగిలిన > 1) {
            --మిగిలిన;
            తిరిగి నిజం;
        }
        మిగిలిన = 0;
    }
    ఒకవేళ (సూచీ >= length) {
        తిరిగి no_తదుపరి();
    }
    int32_t u = array[సూచీ++];
    ఒకవేళ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        మారింది = అబద్ధం;
        old_నిడివి_2 = u + 1;
        అయితే (సూచీ < length && (u = array[సూచీ]) <= MAX_UNCHANGED) {
            ++సూచీ;
            old_నిడివి_2 += u + 1;
        }
        new_నిడివి_2 = old_నిడివి_2;
        ఒకవేళ (onlyChanges) {
            update_తదుపరి_Indexes();
            ఒకవేళ (సూచీ >= length) {
                తిరిగి no_తదుపరి();
            }
            // already fetched u > MAX_UNCHANGED at index
            ++సూచీ;
        } లేకపోతే {
            తిరిగి నిజం;
        }
    }
    మారింది = నిజం;
    ఒకవేళ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_నిడివి;
        int32_t num = (u & SHORT_CHANGE_అంకె_ముసుగు) + 1;
        ఒకవేళ (coarse) {
            old_నిడివి_2 = num * oldLen;
            new_నిడివి_2 = num * newLen;
        } లేకపోతే {
            // Split a sequence of changes that was compressed into one unit.
            old_నిడివి_2 = oldLen;
            new_నిడివి_2 = newLen;
            ఒకవేళ (num > 1) {
                మిగిలిన = num;  // This is the first of two or more changes.
            }
            తిరిగి నిజం;
        }
    } లేకపోతే {
        U_ASSERT(u <= 0x7fff);
        old_నిడివి_2 = read_నిడివి((u >> 6) & 0x3f);
        new_నిడివి_2 = read_నిడివి(u & 0x3f);
        ఒకవేళ (!coarse) {
            తిరిగి నిజం;
        }
    }
    // Combine adjacent changes.
    అయితే (సూచీ < length && (u = array[సూచీ]) > MAX_UNCHANGED) {
        ++సూచీ;
        ఒకవేళ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_అంకె_ముసుగు) + 1;
            old_నిడివి_2 += (u >> 12) * num;
            new_నిడివి_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_నిడివి) * num;
        } లేకపోతే {
            U_ASSERT(u <= 0x7fff);
            old_నిడివి_2 += read_నిడివి((u >> 6) & 0x3f);
            new_నిడివి_2 += read_నిడివి(u & 0x3f);
        }
    }
    తిరిగి నిజం;
}

UBool Edits::Iterator::మునుపటి(UError_సంకేతం &error_సంకేతం) {
    // Backward iteration: Pre-decrement-read array units to assemble a new span,
    // then update the string indexes to the start of that span.
    // Leaves the array index on the head unit of that span.
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    // We have an errorCode in case we need to start guarding against integer overflows.
    // It is also convenient for caller loops if we bail out when an error was set elsewhere.
    ఒకవేళ (dir >= 0) {
        ఒకవేళ (dir > 0) {
            // Turn around from next() to previous().
            // Set the string indexes to the span limit and
            // pre-decrement-read the same span again.
            ఒకవేళ (మిగిలిన > 0) {
                // Fine-grained iterator:
                // Stay on the current one of a sequence of compressed changes.
                --సూచీ;  // previous() rests on the sequence unit.
                dir = -1;
                తిరిగి నిజం;
            }
            update_తదుపరి_Indexes();
        }
        dir = -1;
    }
    ఒకవేళ (మిగిలిన > 0) {
        // Fine-grained iterator: Continue a sequence of compressed changes.
        int32_t u = array[సూచీ];
        U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
        ఒకవేళ (మిగిలిన <= (u & SHORT_CHANGE_అంకె_ముసుగు)) {
            ++మిగిలిన;
            update_మునుపటి_Indexes();
            తిరిగి నిజం;
        }
        మిగిలిన = 0;
    }
    ఒకవేళ (సూచీ <= 0) {
        తిరిగి no_తదుపరి();
    }
    int32_t u = array[--సూచీ];
    ఒకవేళ (u <= MAX_UNCHANGED) {
        // Combine adjacent unchanged ranges.
        మారింది = అబద్ధం;
        old_నిడివి_2 = u + 1;
        అయితే (సూచీ > 0 && (u = array[సూచీ - 1]) <= MAX_UNCHANGED) {
            --సూచీ;
            old_నిడివి_2 += u + 1;
        }
        new_నిడివి_2 = old_నిడివి_2;
        // No need to handle onlyChanges as long as previous() is called only from findIndex().
        update_మునుపటి_Indexes();
        తిరిగి నిజం;
    }
    మారింది = నిజం;
    ఒకవేళ (u <= MAX_SHORT_CHANGE) {
        int32_t oldLen = u >> 12;
        int32_t newLen = (u >> 9) & MAX_SHORT_CHANGE_NEW_నిడివి;
        int32_t num = (u & SHORT_CHANGE_అంకె_ముసుగు) + 1;
        ఒకవేళ (coarse) {
            old_నిడివి_2 = num * oldLen;
            new_నిడివి_2 = num * newLen;
        } లేకపోతే {
            // Split a sequence of changes that was compressed into one unit.
            old_నిడివి_2 = oldLen;
            new_నిడివి_2 = newLen;
            ఒకవేళ (num > 1) {
                మిగిలిన = 1;  // This is the last of two or more changes.
            }
            update_మునుపటి_Indexes();
            తిరిగి నిజం;
        }
    } లేకపోతే {
        ఒకవేళ (u <= 0x7fff) {
            // The change is encoded in u alone.
            old_నిడివి_2 = read_నిడివి((u >> 6) & 0x3f);
            new_నిడివి_2 = read_నిడివి(u & 0x3f);
        } లేకపోతే {
            // Back up to the head of the change, read the lengths,
            // and reset the index to the head again.
            U_ASSERT(సూచీ > 0);
            అయితే ((u = array[--సూచీ]) > 0x7fff) {}
            U_ASSERT(u > MAX_SHORT_CHANGE);
            int32_t తల_సూచీ = సూచీ++;
            old_నిడివి_2 = read_నిడివి((u >> 6) & 0x3f);
            new_నిడివి_2 = read_నిడివి(u & 0x3f);
            సూచీ = తల_సూచీ;
        }
        ఒకవేళ (!coarse) {
            update_మునుపటి_Indexes();
            తిరిగి నిజం;
        }
    }
    // Combine adjacent changes.
    అయితే (సూచీ > 0 && (u = array[సూచీ - 1]) > MAX_UNCHANGED) {
        --సూచీ;
        ఒకవేళ (u <= MAX_SHORT_CHANGE) {
            int32_t num = (u & SHORT_CHANGE_అంకె_ముసుగు) + 1;
            old_నిడివి_2 += (u >> 12) * num;
            new_నిడివి_2 += ((u >> 9) & MAX_SHORT_CHANGE_NEW_నిడివి) * num;
        } లేకపోతే ఒకవేళ (u <= 0x7fff) {
            // Read the lengths, and reset the index to the head again.
            int32_t తల_సూచీ = సూచీ++;
            old_నిడివి_2 += read_నిడివి((u >> 6) & 0x3f);
            new_నిడివి_2 += read_నిడివి(u & 0x3f);
            సూచీ = తల_సూచీ;
        }
    }
    update_మునుపటి_Indexes();
    తిరిగి నిజం;
}

int32_t Edits::Iterator::కనుగొను_సూచీ(int32_t i, UBool కనుగొను_ఆధారం, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం) || i < 0) { తిరిగి -1; }
    int32_t పరిధి_ప్రారంభం, పరిధి_నిడివి;
    ఒకవేళ (కనుగొను_ఆధారం) {  // find source index
        పరిధి_ప్రారంభం = src_సూచీ;
        పరిధి_నిడివి = old_నిడివి_2;
    } లేకపోతే {  // find destination index
        పరిధి_ప్రారంభం = dest_సూచీ;
        పరిధి_నిడివి = new_నిడివి_2;
    }
    ఒకవేళ (i < పరిధి_ప్రారంభం) {
        ఒకవేళ (i >= (పరిధి_ప్రారంభం / 2)) {
            // Search backwards.
            కొరకు (;;) {
                UBool has_మునుపటి = మునుపటి(error_సంకేతం);
                U_ASSERT(has_మునుపటి);  // because i>=0 and the first span starts at 0
                (శూన్య)has_మునుపటి;  // avoid unused-variable warning
                పరిధి_ప్రారంభం = కనుగొను_ఆధారం ? src_సూచీ : dest_సూచీ;
                ఒకవేళ (i >= పరిధి_ప్రారంభం) {
                    // The index is in the current span.
                    తిరిగి 0;
                }
                ఒకవేళ (మిగిలిన > 0) {
                    // Is the index in one of the remaining compressed edits?
                    // spanStart is the start of the current span, first of the remaining ones.
                    పరిధి_నిడివి = కనుగొను_ఆధారం ? old_నిడివి_2 : new_నిడివి_2;
                    int32_t u = array[సూచీ];
                    U_ASSERT(MAX_UNCHANGED < u && u <= MAX_SHORT_CHANGE);
                    int32_t num = (u & SHORT_CHANGE_అంకె_ముసుగు) + 1 - మిగిలిన;
                    int32_t len = num * పరిధి_నిడివి;
                    ఒకవేళ (i >= (పరిధి_ప్రారంభం - len)) {
                        int32_t n = ((పరిధి_ప్రారంభం - i - 1) / పరిధి_నిడివి) + 1;
                        // 1 <= n <= num
                        src_సూచీ -= n * old_నిడివి_2;
                        repl_సూచీ -= n * new_నిడివి_2;
                        dest_సూచీ -= n * new_నిడివి_2;
                        మిగిలిన += n;
                        తిరిగి 0;
                    }
                    // Skip all of these edits at once.
                    src_సూచీ -= num * old_నిడివి_2;
                    repl_సూచీ -= num * new_నిడివి_2;
                    dest_సూచీ -= num * new_నిడివి_2;
                    మిగిలిన = 0;
                }
            }
        }
        // Reset the iterator to the start.
        dir = 0;
        సూచీ = మిగిలిన = old_నిడివి_2 = new_నిడివి_2 = src_సూచీ = repl_సూచీ = dest_సూచీ = 0;
    } లేకపోతే ఒకవేళ (i < (పరిధి_ప్రారంభం + పరిధి_నిడివి)) {
        // The index is in the current span.
        తిరిగి 0;
    }
    అయితే (next(అబద్ధం, error_సంకేతం)) {
        ఒకవేళ (కనుగొను_ఆధారం) {
            పరిధి_ప్రారంభం = src_సూచీ;
            పరిధి_నిడివి = old_నిడివి_2;
        } లేకపోతే {
            పరిధి_ప్రారంభం = dest_సూచీ;
            పరిధి_నిడివి = new_నిడివి_2;
        }
        ఒకవేళ (i < (పరిధి_ప్రారంభం + పరిధి_నిడివి)) {
            // The index is in the current span.
            తిరిగి 0;
        }
        ఒకవేళ (మిగిలిన > 1) {
            // Is the index in one of the remaining compressed edits?
            // spanStart is the start of the current span, first of the remaining ones.
            int32_t len = మిగిలిన * పరిధి_నిడివి;
            ఒకవేళ (i < (పరిధి_ప్రారంభం + len)) {
                int32_t n = (i - పరిధి_ప్రారంభం) / పరిధి_నిడివి;  // 1 <= n <= remaining - 1
                src_సూచీ += n * old_నిడివి_2;
                repl_సూచీ += n * new_నిడివి_2;
                dest_సూచీ += n * new_నిడివి_2;
                మిగిలిన -= n;
                తిరిగి 0;
            }
            // Make next() skip all of these edits at once.
            old_నిడివి_2 *= మిగిలిన;
            new_నిడివి_2 *= మిగిలిన;
            మిగిలిన = 0;
        }
    }
    తిరిగి 1;
}

int32_t Edits::Iterator::destination_సూచీ_From_ఆధారం_సూచీ(int32_t i, UError_సంకేతం &error_సంకేతం) {
    int32_t where = కనుగొను_సూచీ(i, నిజం, error_సంకేతం);
    ఒకవేళ (where < 0) {
        // Error or before the string.
        తిరిగి 0;
    }
    ఒకవేళ (where > 0 || i == src_సూచీ) {
        // At or after string length, or at start of the found span.
        తిరిగి dest_సూచీ;
    }
    ఒకవేళ (మారింది) {
        // In a change span, map to its end.
        తిరిగి dest_సూచీ + new_నిడివి_2;
    } లేకపోతే {
        // In an unchanged span, offset 1:1 within it.
        తిరిగి dest_సూచీ + (i - src_సూచీ);
    }
}

int32_t Edits::Iterator::ఆధారం_సూచీ_From_Destination_సూచీ(int32_t i, UError_సంకేతం &error_సంకేతం) {
    int32_t where = కనుగొను_సూచీ(i, అబద్ధం, error_సంకేతం);
    ఒకవేళ (where < 0) {
        // Error or before the string.
        తిరిగి 0;
    }
    ఒకవేళ (where > 0 || i == dest_సూచీ) {
        // At or after string length, or at start of the found span.
        తిరిగి src_సూచీ;
    }
    ఒకవేళ (మారింది) {
        // In a change span, map to its end.
        తిరిగి src_సూచీ + old_నిడివి_2;
    } లేకపోతే {
        // In an unchanged span, offset within it.
        తిరిగి src_సూచీ + (i - dest_సూచీ);
    }
}

UnicodeString& Edits::Iterator::గమ్యం_String(UnicodeString& sb U_LIFETIME_BOUND) స్థిరరాశి {
    sb.append(u"{ src[", -1);
    ICU_Utility::append_సంఖ్య(sb, src_సూచీ);
    sb.append(u"..", -1);
    ICU_Utility::append_సంఖ్య(sb, src_సూచీ + old_నిడివి_2);
    ఒకవేళ (మారింది) {
        sb.append(u"] ⇝ dest[", -1);
    } లేకపోతే {
        sb.append(u"] ≡ dest[", -1);
    }
    ICU_Utility::append_సంఖ్య(sb, dest_సూచీ);
    sb.append(u"..", -1);
    ICU_Utility::append_సంఖ్య(sb, dest_సూచీ + new_నిడివి_2);
    ఒకవేళ (మారింది) {
        sb.append(u"], repl[", -1);
        ICU_Utility::append_సంఖ్య(sb, repl_సూచీ);
        sb.append(u"..", -1);
        ICU_Utility::append_సంఖ్య(sb, repl_సూచీ + new_నిడివి_2);
        sb.append(u"] }", -1);
    } లేకపోతే {
        sb.append(u"] (no-change) }", -1);
    }
    తిరిగి sb;
}

U_NAMESPACE_చివర
