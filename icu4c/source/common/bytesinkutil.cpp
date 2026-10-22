// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/bytestream.h"
#অন্তর্ভাৱ "unicode/edits.h"
#অন্তর্ভাৱ "unicode/stringoptions.h"
#অন্তর্ভাৱ "unicode/utf8.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "bytesinkutil.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ধ্রুৱ char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    ৱর্ণ scratch[200];
    int32_t s8Length = 0;
    বাবে (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        যদি (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } অন্যথা যদি (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } অন্যথা {
            desiredCapacity = INT32_MAX;
        }
        ৱর্ণ *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        বাবে (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        যদি (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই মিছা;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    যদি (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    ঘূৰাই সঁচা;
}

UBool
ByteSinkUtil::appendChange(ধ্রুৱ uint8_t *s, ধ্রুৱ uint8_t *limit,
                           ধ্রুৱ char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    যদি ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ঘূৰাই মিছা;
    }
    ঘূৰাই appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

শূন্য
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    ৱর্ণ s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    যদি (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

নামাকাশ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
অন্তঃস্থ uint8_t getTwoByteLead(UChar32 c) { ঘূৰাই static_cast<uint8_t>((c >> 6) | 0xc0); }
অন্তঃস্থ uint8_t getTwoByteTrail(UChar32 c) { ঘূৰাই static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

শূন্য
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    ৱর্ণ s8[2] = {static_cast<ৱর্ণ>(getTwoByteLead(c)), static_cast<ৱর্ণ>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

শূন্য
ByteSinkUtil::appendNonEmptyUnchanged(ধ্রুৱ uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    যদি (edits != nullptr) {
        edits->addUnchanged(length);
    }
    যদি ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<ধ্রুৱ ৱর্ণ *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ধ্রুৱ uint8_t *s, ধ্রুৱ uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    যদি ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ঘূৰাই মিছা;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    যদি (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    ঘূৰাই সঁচা;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = স্ৱভাৱ;

শূন্য
CharStringByteSink::Append(ধ্রুৱ ৱর্ণ* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

ৱর্ণ*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    ৱর্ণ* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    যদি (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        ঘূৰাই nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    ৱর্ণ* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    যদি (U_SUCCESS(status)) {
        ঘূৰাই result;
    }

    *result_capacity = scratch_capacity;
    ঘূৰাই scratch;
}

U_NAMESPACE_END
