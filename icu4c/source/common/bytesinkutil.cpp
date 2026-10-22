// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/stringoptions.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "bytesinkutil.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ধ্রুব char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    বর্ণ scratch[200];
    int32_t s8Length = 0;
    প্রতি (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        যদি (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } অন্যথা যদি (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } অন্যথা {
            desiredCapacity = INT32_MAX;
        }
        বর্ণ *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        প্রতি (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        যদি (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম অসত্য;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    যদি (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    প্রত্যাগম সত্য;
}

UBool
ByteSinkUtil::appendChange(ধ্রুব uint8_t *s, ধ্রুব uint8_t *limit,
                           ধ্রুব char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    যদি ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম অসত্য;
    }
    প্রত্যাগম appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

শূন্য
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    বর্ণ s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    যদি (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

নামাকাশ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
অন্তঃস্থ uint8_t getTwoByteLead(UChar32 c) { প্রত্যাগম static_cast<uint8_t>((c >> 6) | 0xc0); }
অন্তঃস্থ uint8_t getTwoByteTrail(UChar32 c) { প্রত্যাগম static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

শূন্য
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    বর্ণ s8[2] = {static_cast<বর্ণ>(getTwoByteLead(c)), static_cast<বর্ণ>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

শূন্য
ByteSinkUtil::appendNonEmptyUnchanged(ধ্রুব uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    যদি (edits != nullptr) {
        edits->addUnchanged(length);
    }
    যদি ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<ধ্রুব বর্ণ *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ধ্রুব uint8_t *s, ধ্রুব uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    যদি ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম অসত্য;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    যদি (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    প্রত্যাগম সত্য;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = স্বভাব;

শূন্য
CharStringByteSink::Append(ধ্রুব বর্ণ* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

বর্ণ*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    বর্ণ* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    যদি (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        প্রত্যাগম nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    বর্ণ* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    যদি (U_SUCCESS(status)) {
        প্রত্যাগম result;
    }

    *result_capacity = scratch_capacity;
    প্রত্যাগম scratch;
}

U_NAMESPACE_END
