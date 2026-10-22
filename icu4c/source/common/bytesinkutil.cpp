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
ByteSinkUtil::appendChange(int32_t length, ধ্রুবক char16_t *s16, int32_t s16_দৈর্ঘ্য,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    অক্ষর scratch[200];
    int32_t s8_দৈর্ঘ্য = 0;
    জন্য (int32_t i = 0; i < s16_দৈর্ঘ্য;) {
        int32_t capacity;
        int32_t desiredCapacity = s16_দৈর্ঘ্য - i;
        যদি (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } অন্যথা যদি (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } অন্যথা {
            desiredCapacity = INT32_MAX;
        }
        অক্ষর *বাফার = sink.Get_Append_বাফার(U8_MAX_দৈর্ঘ্য, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_দৈর্ঘ্য - 1;
        int32_t j = 0;
        জন্য (; i < s16_দৈর্ঘ্য && j < capacity;) {
            UChar32 c;
            U16_পরবর্তী_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(বাফার, j, c);
        }
        যদি (j > (INT32_MAX - s8_দৈর্ঘ্য)) {
            errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত মিথ্যা;
        }
        sink.Append(বাফার, j);
        s8_দৈর্ঘ্য += j;
    }
    যদি (edits != nullptr) {
        edits->addReplace(length, s8_দৈর্ঘ্য);
    }
    ফেরত সত্য;
}

UBool
ByteSinkUtil::appendChange(ধ্রুবক uint8_t *s, ধ্রুবক uint8_t *limit,
                           ধ্রুবক char16_t *s16, int32_t s16_দৈর্ঘ্য,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    যদি ((limit - s) > INT32_MAX) {
        errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত মিথ্যা;
    }
    ফেরত appendChange(static_cast<int32_t>(limit - s), s16, s16_দৈর্ঘ্য, sink, edits, errorCode);
}

শূন্য
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    অক্ষর s8[U8_MAX_দৈর্ঘ্য];
    int32_t s8_দৈর্ঘ্য = 0;
    U8_APPEND_UNSAFE(s8, s8_দৈর্ঘ্য, c);
    যদি (edits != nullptr) {
        edits->addReplace(length, s8_দৈর্ঘ্য);
    }
    sink.Append(s8, s8_দৈর্ঘ্য);
}

নামাকাশ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
অন্তঃরেখ uint8_t getTwoByteLead(UChar32 c) { ফেরত static_cast<uint8_t>((c >> 6) | 0xc0); }
অন্তঃরেখ uint8_t getTwoByteTrail(UChar32 c) { ফেরত static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

শূন্য
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    অক্ষর s8[2] = {static_cast<অক্ষর>(getTwoByteLead(c)), static_cast<অক্ষর>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

শূন্য
ByteSinkUtil::appendNonEmptyUnchanged(ধ্রুবক uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    যদি (edits != nullptr) {
        edits->addUnchanged(length);
    }
    যদি ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<ধ্রুবক অক্ষর *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ধ্রুবক uint8_t *s, ধ্রুবক uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    যদি ((limit - s) > INT32_MAX) {
        errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত মিথ্যা;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    যদি (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    ফেরত সত্য;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = পূর্বনির্ধারিত;

শূন্য
CharStringByteSink::Append(ধ্রুবক অক্ষর* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

অক্ষর*
CharStringByteSink::Get_Append_বাফার(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    অক্ষর* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* ফলাফল_capacity) U_LIFETIME_BOUND {
    যদি (min_capacity < 1 || scratch_capacity < min_capacity) {
        *ফলাফল_capacity = 0;
        ফেরত nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    অক্ষর* ফলাফল = dest_.get_Append_বাফার(
            min_capacity,
            desired_capacity_hint,
            *ফলাফল_capacity,
            status);
    যদি (U_SUCCESS(status)) {
        ফেরত ফলাফল;
    }

    *ফলাফল_capacity = scratch_capacity;
    ফেরত scratch;
}

U_NAMESPACE_END
