// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringoptions.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ಧ್ರುವ char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ವರ್ಣ scratch[200];
    int32_t s8Length = 0;
    ಪ್ರತಿ (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        ಯದಿ (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } ಅನ್ಯಥಾ ಯದಿ (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } ಅನ್ಯಥಾ {
            desiredCapacity = INT32_MAX;
        }
        ವರ್ಣ *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        ಪ್ರತಿ (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        ಯದಿ (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    ಯದಿ (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool
ByteSinkUtil::appendChange(ಧ್ರುವ uint8_t *s, ಧ್ರುವ uint8_t *limit,
                           ಧ್ರುವ char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಯದಿ ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

ಶೂನ್ಯ
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    ವರ್ಣ s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    ಯದಿ (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

ನಾಮಾಕಾಶ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
ಅನ್ತಃಸ್ಥ uint8_t getTwoByteLead(UChar32 c) { ಪ್ರತ್ಯಾಗಮ static_cast<uint8_t>((c >> 6) | 0xc0); }
ಅನ್ತಃಸ್ಥ uint8_t getTwoByteTrail(UChar32 c) { ಪ್ರತ್ಯಾಗಮ static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

ಶೂನ್ಯ
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    ವರ್ಣ s8[2] = {static_cast<ವರ್ಣ>(getTwoByteLead(c)), static_cast<ವರ್ಣ>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

ಶೂನ್ಯ
ByteSinkUtil::appendNonEmptyUnchanged(ಧ್ರುವ uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    ಯದಿ (edits != nullptr) {
        edits->addUnchanged(length);
    }
    ಯದಿ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<ಧ್ರುವ ವರ್ಣ *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ಧ್ರುವ uint8_t *s, ಧ್ರುವ uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಯದಿ ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    ಯದಿ (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = ಸ್ವಭಾವ;

ಶೂನ್ಯ
CharStringByteSink::Append(ಧ್ರುವ ವರ್ಣ* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

ವರ್ಣ*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    ವರ್ಣ* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    ಯದಿ (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    ವರ್ಣ* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    ಯದಿ (U_SUCCESS(status)) {
        ಪ್ರತ್ಯಾಗಮ result;
    }

    *result_capacity = scratch_capacity;
    ಪ್ರತ್ಯಾಗಮ scratch;
}

U_NAMESPACE_END
