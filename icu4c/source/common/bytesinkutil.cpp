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
ByteSinkUtil::appendChange(int32_t length, ثابت char16_t *s16, int32_t s16_ڊيگهائي,
                           ByteSink &sink, Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    حرف scratch[200];
    int32_t s8_ڊيگهائي = 0;
    لكل (int32_t i = 0; i < s16_ڊيگهائي;) {
        int32_t capacity;
        int32_t desired_صلاحيت = s16_ڊيگهائي - i;
        إذا (desired_صلاحيت < (INT32_MAX / 3)) {
            desired_صلاحيت *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } وإلا إذا (desired_صلاحيت < (INT32_MAX / 2)) {
            desired_صلاحيت *= 2;
        } وإلا {
            desired_صلاحيت = INT32_MAX;
        }
        حرف *بفر = sink.Get_Append_بفر(U8_MAX_ڊيگهائي, desired_صلاحيت,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_ڊيگهائي - 1;
        int32_t j = 0;
        لكل (; i < s16_ڊيگهائي && j < capacity;) {
            UChar32 c;
            U16_ايندڙ_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(بفر, j, c);
        }
        إذا (j > (INT32_MAX - s8_ڊيگهائي)) {
            error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع خطأ;
        }
        sink.Append(بفر, j);
        s8_ڊيگهائي += j;
    }
    إذا (edits != خالي_اشارو) {
        edits->جوڙيو_Replace(length, s8_ڊيگهائي);
    }
    أرجع سچ;
}

UBool
ByteSinkUtil::appendChange(ثابت uint8_t *s, ثابت uint8_t *حد_بندي,
                           ثابت char16_t *s16, int32_t s16_ڊيگهائي,
                           ByteSink &sink, Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    إذا ((حد_بندي - s) > INT32_MAX) {
        error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    أرجع appendChange(static_cast<int32_t>(حد_بندي - s), s16, s16_ڊيگهائي, sink, edits, error_ڪوڊ);
}

فراغ
ByteSinkUtil::append_ڪوڊ_نقطو(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    حرف s8[U8_MAX_ڊيگهائي];
    int32_t s8_ڊيگهائي = 0;
    U8_APPEND_UNSAFE(s8, s8_ڊيگهائي, c);
    إذا (edits != خالي_اشارو) {
        edits->جوڙيو_Replace(length, s8_ڊيگهائي);
    }
    sink.Append(s8, s8_ڊيگهائي);
}

نطاق {

// See unicode/utf8.h U8_APPEND_UNSAFE().
مضمن uint8_t getTwoByteLead(UChar32 c) { أرجع static_cast<uint8_t>((c >> 6) | 0xc0); }
مضمن uint8_t getTwoByteTrail(UChar32 c) { أرجع static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

فراغ
ByteSinkUtil::append_Two_بائيٽون(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    حرف s8[2] = {static_cast<حرف>(getTwoByteLead(c)), static_cast<حرف>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

فراغ
ByteSinkUtil::append_Non_خالي_جايون_Unchanged(ثابت uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    إذا (edits != خالي_اشارو) {
        edits->جوڙيو_Unchanged(length);
    }
    إذا ((options & U_OMIT_UNCHANGED_متن) == 0) {
        sink.Append(reinterpret_cast<ثابت حرف *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *حد_بندي,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    إذا ((حد_بندي - s) > INT32_MAX) {
        error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    int32_t length = static_cast<int32_t>(حد_بندي - s);
    إذا (length > 0) {
        append_Non_خالي_جايون_Unchanged(s, length, sink, options, edits);
    }
    أرجع سچ;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = افتراضي;

فراغ
CharStringByteSink::Append(ثابت حرف* بائيٽون, int32_t n) {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    dest_.append(بائيٽون, n, حالت);
    // Any errors are silently ignored.
}

حرف*
CharStringByteSink::Get_Append_بفر(int32_t min_صلاحيت_2,
                                    int32_t desired_صلاحيت_hint,
                                    حرف* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_صلاحيت_2,
                                    int32_t* نتيجو_صلاحيت_2) U_LIFETIME_BOUND {
    إذا (min_صلاحيت_2 < 1 || scratch_صلاحيت_2 < min_صلاحيت_2) {
        *نتيجو_صلاحيت_2 = 0;
        أرجع خالي_اشارو;
    }

    UError_ڪوڊ حالت = U_ZERO_ERROR;
    حرف* نتيجو = dest_.get_Append_بفر(
            min_صلاحيت_2,
            desired_صلاحيت_hint,
            *نتيجو_صلاحيت_2,
            حالت);
    إذا (U_SUCCESS(حالت)) {
        أرجع نتيجو;
    }

    *نتيجو_صلاحيت_2 = scratch_صلاحيت_2;
    أرجع scratch;
}

U_NAMESPACE_پڇاڙي
