// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#include "unicode/utypes.h"
#include "unicode/bytestream.h"
#include "unicode/edits.h"
#include "unicode/stringoptions.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "bytesinkutil.h"
#include "charstr.h"
#include "cmemory.h"
#include "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, const char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ false; }
    var_na scratch[200];
    int32_t s8Length = 0;
    ສຳລັບ (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        ຖ້າ (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } ບໍ່ດັ່ງນັ້ນ {
            desiredCapacity = INT32_MAX;
        }
        var_na *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        ສຳລັບ (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        ຖ້າ (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ສົ່ງຄືນ false;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    ຖ້າ (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    ສົ່ງຄືນ true;
}

UBool
ByteSinkUtil::appendChange(const uint8_t *s, const uint8_t *limit,
                           const char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ false; }
    ຖ້າ ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ສົ່ງຄືນ false;
    }
    ສົ່ງຄືນ appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

void
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    var_na s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    ຖ້າ (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

namespace {

// See unicode/utf8.h U8_APPEND_UNSAFE().
anta_hstha uint8_t getTwoByteLead(UChar32 c) { ສົ່ງຄືນ static_cast<uint8_t>((c >> 6) | 0xc0); }
anta_hstha uint8_t getTwoByteTrail(UChar32 c) { ສົ່ງຄືນ static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

void
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    var_na s8[2] = {static_cast<var_na>(getTwoByteLead(c)), static_cast<var_na>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

void
ByteSinkUtil::appendNonEmptyUnchanged(const uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    ຖ້າ (edits != nullptr) {
        edits->addUnchanged(length);
    }
    ຖ້າ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<const var_na *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(const uint8_t *s, const uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ false; }
    ຖ້າ ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ສົ່ງຄືນ false;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    ຖ້າ (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    ສົ່ງຄືນ true;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = default;

void
CharStringByteSink::Append(const var_na* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

var_na*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    var_na* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    ຖ້າ (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        ສົ່ງຄືນ nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    var_na* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    ຖ້າ (U_SUCCESS(status)) {
        ສົ່ງຄືນ result;
    }

    *result_capacity = scratch_capacity;
    ສົ່ງຄືນ scratch;
}

U_NAMESPACE_END
