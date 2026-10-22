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
ByteSinkUtil::appendChange(int32_t length, ثابت char16_t *s16, int32_t s16_لمبائی,
                           ByteSink &sink, Edits *edits, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    حرف scratch[200];
    int32_t s8_لمبائی = 0;
    برائے (int32_t i = 0; i < s16_لمبائی;) {
        int32_t capacity;
        int32_t desired_استعداد = s16_لمبائی - i;
        اگر (desired_استعداد < (INT32_MAX / 3)) {
            desired_استعداد *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } ورنہ اگر (desired_استعداد < (INT32_MAX / 2)) {
            desired_استعداد *= 2;
        } ورنہ {
            desired_استعداد = INT32_MAX;
        }
        حرف *بفر = sink.Get_Append_بفر(U8_MAX_لمبائی, desired_استعداد,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_لمبائی - 1;
        int32_t j = 0;
        برائے (; i < s16_لمبائی && j < capacity;) {
            UChar32 c;
            U16_اگلا_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(بفر, j, c);
        }
        اگر (j > (INT32_MAX - s8_لمبائی)) {
            error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس جھوٹ;
        }
        sink.Append(بفر, j);
        s8_لمبائی += j;
    }
    اگر (edits != خالی_اشاریہ) {
        edits->جوڑو_Replace(length, s8_لمبائی);
    }
    واپس سچ;
}

UBool
ByteSinkUtil::appendChange(ثابت uint8_t *s, ثابت uint8_t *حد_بندی,
                           ثابت char16_t *s16, int32_t s16_لمبائی,
                           ByteSink &sink, Edits *edits, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    اگر ((حد_بندی - s) > INT32_MAX) {
        error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس جھوٹ;
    }
    واپس appendChange(static_cast<int32_t>(حد_بندی - s), s16, s16_لمبائی, sink, edits, error_رمز);
}

خالی
ByteSinkUtil::append_رمز_نقطہ(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    حرف s8[U8_MAX_لمبائی];
    int32_t s8_لمبائی = 0;
    U8_APPEND_UNSAFE(s8, s8_لمبائی, c);
    اگر (edits != خالی_اشاریہ) {
        edits->جوڑو_Replace(length, s8_لمبائی);
    }
    sink.Append(s8, s8_لمبائی);
}

نامخلا {

// See unicode/utf8.h U8_APPEND_UNSAFE().
اندرونی uint8_t getTwoByteLead(UChar32 c) { واپس static_cast<uint8_t>((c >> 6) | 0xc0); }
اندرونی uint8_t getTwoByteTrail(UChar32 c) { واپس static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

خالی
ByteSinkUtil::append_Two_بائٹیں(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    حرف s8[2] = {static_cast<حرف>(getTwoByteLead(c)), static_cast<حرف>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

خالی
ByteSinkUtil::append_Non_خالی_خانے_Unchanged(ثابت uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    اگر (edits != خالی_اشاریہ) {
        edits->جوڑو_Unchanged(length);
    }
    اگر ((options & U_OMIT_UNCHANGED_متن) == 0) {
        sink.Append(reinterpret_cast<ثابت حرف *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *حد_بندی,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    اگر ((حد_بندی - s) > INT32_MAX) {
        error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس جھوٹ;
    }
    int32_t length = static_cast<int32_t>(حد_بندی - s);
    اگر (length > 0) {
        append_Non_خالی_خانے_Unchanged(s, length, sink, options, edits);
    }
    واپس سچ;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = طےشدہ;

خالی
CharStringByteSink::Append(ثابت حرف* بائٹیں, int32_t n) {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    dest_.append(بائٹیں, n, حالت_2);
    // Any errors are silently ignored.
}

حرف*
CharStringByteSink::Get_Append_بفر(int32_t min_استعداد_2,
                                    int32_t desired_استعداد_hint,
                                    حرف* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_استعداد_2,
                                    int32_t* نتیجہ_استعداد_2) U_LIFETIME_BOUND {
    اگر (min_استعداد_2 < 1 || scratch_استعداد_2 < min_استعداد_2) {
        *نتیجہ_استعداد_2 = 0;
        واپس خالی_اشاریہ;
    }

    UError_رمز حالت_2 = U_ZERO_ERROR;
    حرف* نتیجہ = dest_.get_Append_بفر(
            min_استعداد_2,
            desired_استعداد_hint,
            *نتیجہ_استعداد_2,
            حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        واپس نتیجہ;
    }

    *نتیجہ_استعداد_2 = scratch_استعداد_2;
    واپس scratch;
}

U_NAMESPACE_انتہا
