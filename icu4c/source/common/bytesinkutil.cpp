// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "unicode/bytestream.h"
#شامل "unicode/edits.h"
#شامل "unicode/stringoptions.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "bytesinkutil.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ثابت char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    نویسه scratch[200];
    int32_t s8Length = 0;
    برای (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        اگر (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } وگرنه اگر (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } وگرنه {
            desiredCapacity = INT32_MAX;
        }
        نویسه *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        برای (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        اگر (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت نادرست;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    اگر (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    بازگشت درست;
}

UBool
ByteSinkUtil::appendChange(ثابت uint8_t *s, ثابت uint8_t *limit,
                           ثابت char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    اگر ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت نادرست;
    }
    بازگشت appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

تهی
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    نویسه s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    اگر (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

فضاینام {

// See unicode/utf8.h U8_APPEND_UNSAFE().
درونخطی uint8_t getTwoByteLead(UChar32 c) { بازگشت static_cast<uint8_t>((c >> 6) | 0xc0); }
درونخطی uint8_t getTwoByteTrail(UChar32 c) { بازگشت static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

تهی
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    نویسه s8[2] = {static_cast<نویسه>(getTwoByteLead(c)), static_cast<نویسه>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

تهی
ByteSinkUtil::appendNonEmptyUnchanged(ثابت uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    اگر (edits != nullptr) {
        edits->addUnchanged(length);
    }
    اگر ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<ثابت نویسه *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    اگر ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت نادرست;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    اگر (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    بازگشت درست;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = پیشفرض;

تهی
CharStringByteSink::Append(ثابت نویسه* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

نویسه*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    نویسه* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    اگر (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        بازگشت nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    نویسه* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    اگر (U_SUCCESS(status)) {
        بازگشت result;
    }

    *result_capacity = scratch_capacity;
    بازگشت scratch;
}

U_NAMESPACE_END
