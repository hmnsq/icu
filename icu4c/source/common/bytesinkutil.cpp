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
ByteSinkUtil::appendChange(int32_t length, ثابت char16_t *s16, int32_t s16_لمائی,
                           ByteSink &sink, Edits *edits, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    حرف scratch[200];
    int32_t s8_لمائی = 0;
    لكل (int32_t i = 0; i < s16_لمائی;) {
        int32_t capacity;
        int32_t desired_سمرتھا = s16_لمائی - i;
        إذا (desired_سمرتھا < (INT32_MAX / 3)) {
            desired_سمرتھا *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } وإلا إذا (desired_سمرتھا < (INT32_MAX / 2)) {
            desired_سمرتھا *= 2;
        } وإلا {
            desired_سمرتھا = INT32_MAX;
        }
        حرف *بفر = sink.Get_Append_بفر(U8_MAX_لمائی, desired_سمرتھا,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_لمائی - 1;
        int32_t j = 0;
        لكل (; i < s16_لمائی && j < capacity;) {
            UChar32 c;
            U16_اگلا_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(بفر, j, c);
        }
        إذا (j > (INT32_MAX - s8_لمائی)) {
            error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع خطأ;
        }
        sink.Append(بفر, j);
        s8_لمائی += j;
    }
    إذا (edits != خالی_سوچک) {
        edits->جوڑو_Replace(length, s8_لمائی);
    }
    أرجع سچ;
}

UBool
ByteSinkUtil::appendChange(ثابت uint8_t *s, ثابت uint8_t *حد_بندی,
                           ثابت char16_t *s16, int32_t s16_لمائی,
                           ByteSink &sink, Edits *edits, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    إذا ((حد_بندی - s) > INT32_MAX) {
        error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    أرجع appendChange(static_cast<int32_t>(حد_بندی - s), s16, s16_لمائی, sink, edits, error_کوڈ);
}

فراغ
ByteSinkUtil::append_کوڈ_نقطہ(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    حرف s8[U8_MAX_لمائی];
    int32_t s8_لمائی = 0;
    U8_APPEND_UNSAFE(s8, s8_لمائی, c);
    إذا (edits != خالی_سوچک) {
        edits->جوڑو_Replace(length, s8_لمائی);
    }
    sink.Append(s8, s8_لمائی);
}

نطاق {

// See unicode/utf8.h U8_APPEND_UNSAFE().
مضمن uint8_t getTwoByteLead(UChar32 c) { أرجع static_cast<uint8_t>((c >> 6) | 0xc0); }
مضمن uint8_t getTwoByteTrail(UChar32 c) { أرجع static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

فراغ
ByteSinkUtil::append_Two_بائٹاں(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    حرف s8[2] = {static_cast<حرف>(getTwoByteLead(c)), static_cast<حرف>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

فراغ
ByteSinkUtil::append_Non_خالی_تھاواں_Unchanged(ثابت uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    إذا (edits != خالی_سوچک) {
        edits->جوڑو_Unchanged(length);
    }
    إذا ((options & U_OMIT_UNCHANGED_متن) == 0) {
        sink.Append(reinterpret_cast<ثابت حرف *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *حد_بندی,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    إذا ((حد_بندی - s) > INT32_MAX) {
        error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    int32_t length = static_cast<int32_t>(حد_بندی - s);
    إذا (length > 0) {
        append_Non_خالی_تھاواں_Unchanged(s, length, sink, options, edits);
    }
    أرجع سچ;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = افتراضي;

فراغ
CharStringByteSink::Append(ثابت حرف* بائٹاں, int32_t n) {
    UError_کوڈ حالت = U_ZERO_ERROR;
    dest_.append(بائٹاں, n, حالت);
    // Any errors are silently ignored.
}

حرف*
CharStringByteSink::Get_Append_بفر(int32_t min_سمرتھا_2,
                                    int32_t desired_سمرتھا_hint,
                                    حرف* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_سمرتھا_2,
                                    int32_t* نتیجہ_سمرتھا_2) U_LIFETIME_BOUND {
    إذا (min_سمرتھا_2 < 1 || scratch_سمرتھا_2 < min_سمرتھا_2) {
        *نتیجہ_سمرتھا_2 = 0;
        أرجع خالی_سوچک;
    }

    UError_کوڈ حالت = U_ZERO_ERROR;
    حرف* نتیجہ = dest_.get_Append_بفر(
            min_سمرتھا_2,
            desired_سمرتھا_hint,
            *نتیجہ_سمرتھا_2,
            حالت);
    إذا (U_SUCCESS(حالت)) {
        أرجع نتیجہ;
    }

    *نتیجہ_سمرتھا_2 = scratch_سمرتھا_2;
    أرجع scratch;
}

U_NAMESPACE_سرا
