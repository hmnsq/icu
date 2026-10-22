// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/edits.h"
#அந்தர்பாவ "unicode/stringoptions.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, மாறிலி char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    எழுத்து scratch[200];
    int32_t s8Length = 0;
    க்கு (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        எனில் (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } இல்லையெனில் எனில் (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } இல்லையெனில் {
            desiredCapacity = INT32_MAX;
        }
        எழுத்து *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        க்கு (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        எனில் (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு பொய்;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    எனில் (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    திரும்பு உண்மை;
}

UBool
ByteSinkUtil::appendChange(மாறிலி uint8_t *s, மாறிலி uint8_t *limit,
                           மாறிலி char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    எனில் ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு பொய்;
    }
    திரும்பு appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

வெறுமை
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    எழுத்து s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    எனில் (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

நாமாகாஶ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
உள்வரி uint8_t getTwoByteLead(UChar32 c) { திரும்பு static_cast<uint8_t>((c >> 6) | 0xc0); }
உள்வரி uint8_t getTwoByteTrail(UChar32 c) { திரும்பு static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

வெறுமை
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    எழுத்து s8[2] = {static_cast<எழுத்து>(getTwoByteLead(c)), static_cast<எழுத்து>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

வெறுமை
ByteSinkUtil::appendNonEmptyUnchanged(மாறிலி uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    எனில் (edits != nullptr) {
        edits->addUnchanged(length);
    }
    எனில் ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<மாறிலி எழுத்து *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(மாறிலி uint8_t *s, மாறிலி uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    எனில் ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு பொய்;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    எனில் (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    திரும்பு உண்மை;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = இயல்பு;

வெறுமை
CharStringByteSink::Append(மாறிலி எழுத்து* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

எழுத்து*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    எழுத்து* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    எனில் (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        திரும்பு nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    எழுத்து* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    எனில் (U_SUCCESS(status)) {
        திரும்பு result;
    }

    *result_capacity = scratch_capacity;
    திரும்பு scratch;
}

U_NAMESPACE_END
