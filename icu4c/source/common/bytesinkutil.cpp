// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestream.h"
#ضمن "unicode/edits.h"
#ضمن "unicode/stringoptions.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "bytesinkutil.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ثابت char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
    حرف scratch[200];
    int32_t s8Length = 0;
    لكل (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        𐤀𐤌 (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } وإلا 𐤀𐤌 (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } وإلا {
            desiredCapacity = INT32_MAX;
        }
        حرف *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        لكل (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        𐤀𐤌 (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع خطأ;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    𐤀𐤌 (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    أرجع true;
}

UBool
ByteSinkUtil::appendChange(ثابت uint8_t *s, ثابت uint8_t *limit,
                           ثابت char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
    𐤀𐤌 ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    أرجع appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

فراغ
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    حرف s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    𐤀𐤌 (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

نطاق {

// See unicode/utf8.h U8_APPEND_UNSAFE().
مضمن uint8_t getTwoByteLead(UChar32 c) { أرجع static_cast<uint8_t>((c >> 6) | 0xc0); }
مضمن uint8_t getTwoByteTrail(UChar32 c) { أرجع static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

فراغ
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    حرف s8[2] = {static_cast<حرف>(getTwoByteLead(c)), static_cast<حرف>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

فراغ
ByteSinkUtil::appendNonEmptyUnchanged(ثابت uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    𐤀𐤌 (edits != nullptr) {
        edits->addUnchanged(length);
    }
    𐤀𐤌 ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<ثابت حرف *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
    𐤀𐤌 ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    𐤀𐤌 (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    أرجع true;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = افتراضي;

فراغ
CharStringByteSink::Append(ثابت حرف* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

حرف*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    حرف* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    𐤀𐤌 (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        أرجع nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    حرف* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    𐤀𐤌 (U_SUCCESS(status)) {
        أرجع result;
    }

    *result_capacity = scratch_capacity;
    أرجع scratch;
}

U_NAMESPACE_END
