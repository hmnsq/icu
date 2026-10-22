// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ध्रुव char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
    वर्ण scratch[200];
    int32_t s8Length = 0;
    प्रति (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        जइ (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } अण्णहा जइ (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } अण्णहा {
            desiredCapacity = INT32_MAX;
        }
        वर्ण *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        प्रति (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        जइ (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम मिच्छ;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    जइ (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    प्रत्यागम सच्च;
}

UBool
ByteSinkUtil::appendChange(ध्रुव uint8_t *s, ध्रुव uint8_t *limit,
                           ध्रुव char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
    जइ ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम मिच्छ;
    }
    प्रत्यागम appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

शून्य
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    वर्ण s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    जइ (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

नामाकाश {

// See unicode/utf8.h U8_APPEND_UNSAFE().
अन्तःस्थ uint8_t getTwoByteLead(UChar32 c) { प्रत्यागम static_cast<uint8_t>((c >> 6) | 0xc0); }
अन्तःस्थ uint8_t getTwoByteTrail(UChar32 c) { प्रत्यागम static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

शून्य
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    वर्ण s8[2] = {static_cast<वर्ण>(getTwoByteLead(c)), static_cast<वर्ण>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

शून्य
ByteSinkUtil::appendNonEmptyUnchanged(ध्रुव uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    जइ (edits != nullptr) {
        edits->addUnchanged(length);
    }
    जइ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<ध्रुव वर्ण *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ध्रुव uint8_t *s, ध्रुव uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम मिच्छ; }
    जइ ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम मिच्छ;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    जइ (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    प्रत्यागम सच्च;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = स्वभाव;

शून्य
CharStringByteSink::Append(ध्रुव वर्ण* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

वर्ण*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    वर्ण* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    जइ (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        प्रत्यागम nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    वर्ण* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    जइ (U_SUCCESS(status)) {
        प्रत्यागम result;
    }

    *result_capacity = scratch_capacity;
    प्रत्यागम scratch;
}

U_NAMESPACE_END
