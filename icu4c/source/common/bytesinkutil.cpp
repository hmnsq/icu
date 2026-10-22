// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestream.h"
#समावेश "unicode/edits.h"
#समावेश "unicode/stringoptions.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "bytesinkutil.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, स्थिर char16_t *s16, int32_t s16_लंबाई,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस false; }
    अक्षर scratch[200];
    int32_t s8_लंबाई = 0;
    क्रम (int32_t i = 0; i < s16_लंबाई;) {
        int32_t capacity;
        int32_t desiredCapacity = s16_लंबाई - i;
        अगर (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } अन्यथा अगर (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } अन्यथा {
            desiredCapacity = INT32_MAX;
        }
        अक्षर *बफर = sink.Get_Append_बफर(U8_MAX_लंबाई, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_लंबाई - 1;
        int32_t j = 0;
        क्रम (; i < s16_लंबाई && j < capacity;) {
            UChar32 c;
            U16_अग्र_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(बफर, j, c);
        }
        अगर (j > (INT32_MAX - s8_लंबाई)) {
            errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस false;
        }
        sink.Append(बफर, j);
        s8_लंबाई += j;
    }
    अगर (edits != nullptr) {
        edits->addReplace(length, s8_लंबाई);
    }
    वापस true;
}

UBool
ByteSinkUtil::appendChange(स्थिर uint8_t *s, स्थिर uint8_t *limit,
                           स्थिर char16_t *s16, int32_t s16_लंबाई,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस false; }
    अगर ((limit - s) > INT32_MAX) {
        errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस false;
    }
    वापस appendChange(static_cast<int32_t>(limit - s), s16, s16_लंबाई, sink, edits, errorCode);
}

व्योम
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    अक्षर s8[U8_MAX_लंबाई];
    int32_t s8_लंबाई = 0;
    U8_APPEND_UNSAFE(s8, s8_लंबाई, c);
    अगर (edits != nullptr) {
        edits->addReplace(length, s8_लंबाई);
    }
    sink.Append(s8, s8_लंबाई);
}

namespace {

// See unicode/utf8.h U8_APPEND_UNSAFE().
अंतरभूत uint8_t getTwoByteLead(UChar32 c) { वापस static_cast<uint8_t>((c >> 6) | 0xc0); }
अंतरभूत uint8_t getTwoByteTrail(UChar32 c) { वापस static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

व्योम
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    अक्षर s8[2] = {static_cast<अक्षर>(getTwoByteLead(c)), static_cast<अक्षर>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

व्योम
ByteSinkUtil::appendNonEmptyUnchanged(स्थिर uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    अगर (edits != nullptr) {
        edits->addUnchanged(length);
    }
    अगर ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<स्थिर अक्षर *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(स्थिर uint8_t *s, स्थिर uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस false; }
    अगर ((limit - s) > INT32_MAX) {
        errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस false;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    अगर (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    वापस true;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = शेष;

व्योम
CharStringByteSink::Append(स्थिर अक्षर* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

अक्षर*
CharStringByteSink::Get_Append_बफर(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    अक्षर* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* परिणाम_capacity) U_LIFETIME_BOUND {
    अगर (min_capacity < 1 || scratch_capacity < min_capacity) {
        *परिणाम_capacity = 0;
        वापस nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    अक्षर* परिणाम = dest_.get_Append_बफर(
            min_capacity,
            desired_capacity_hint,
            *परिणाम_capacity,
            status);
    अगर (U_SUCCESS(status)) {
        वापस परिणाम;
    }

    *परिणाम_capacity = scratch_capacity;
    वापस scratch;
}

U_NAMESPACE_END
