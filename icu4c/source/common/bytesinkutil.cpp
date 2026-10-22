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
ByteSinkUtil::appendChange(int32_t length, ثابت char16_t *s16, int32_t s16_درازی,
                           ByteSink &sink, Edits *edits, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    حرف scratch[200];
    int32_t s8_درازی = 0;
    لكل (int32_t i = 0; i < s16_درازی;) {
        int32_t capacity;
        int32_t desired_ظرفیت = s16_درازی - i;
        إذا (desired_ظرفیت < (INT32_MAX / 3)) {
            desired_ظرفیت *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } وإلا إذا (desired_ظرفیت < (INT32_MAX / 2)) {
            desired_ظرفیت *= 2;
        } وإلا {
            desired_ظرفیت = INT32_MAX;
        }
        حرف *میانگیر = sink.Get_Append_میانگیر(U8_MAX_درازی, desired_ظرفیت,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_درازی - 1;
        int32_t j = 0;
        لكل (; i < s16_درازی && j < capacity;) {
            UChar32 c;
            U16_بعدی_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(میانگیر, j, c);
        }
        إذا (j > (INT32_MAX - s8_درازی)) {
            error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع خطأ;
        }
        sink.Append(میانگیر, j);
        s8_درازی += j;
    }
    إذا (edits != اشاره_تهی) {
        edits->جمع_کن_Replace(length, s8_درازی);
    }
    أرجع درست;
}

UBool
ByteSinkUtil::appendChange(ثابت uint8_t *s, ثابت uint8_t *حد,
                           ثابت char16_t *s16, int32_t s16_درازی,
                           ByteSink &sink, Edits *edits, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    إذا ((حد - s) > INT32_MAX) {
        error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    أرجع appendChange(static_cast<int32_t>(حد - s), s16, s16_درازی, sink, edits, error_کد);
}

فراغ
ByteSinkUtil::append_کد_نقطه(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    حرف s8[U8_MAX_درازی];
    int32_t s8_درازی = 0;
    U8_APPEND_UNSAFE(s8, s8_درازی, c);
    إذا (edits != اشاره_تهی) {
        edits->جمع_کن_Replace(length, s8_درازی);
    }
    sink.Append(s8, s8_درازی);
}

نطاق {

// See unicode/utf8.h U8_APPEND_UNSAFE().
مضمن uint8_t getTwoByteLead(UChar32 c) { أرجع static_cast<uint8_t>((c >> 6) | 0xc0); }
مضمن uint8_t getTwoByteTrail(UChar32 c) { أرجع static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

فراغ
ByteSinkUtil::append_Two_بایتها(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    حرف s8[2] = {static_cast<حرف>(getTwoByteLead(c)), static_cast<حرف>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

فراغ
ByteSinkUtil::append_Non_خالی_Unchanged(ثابت uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    إذا (edits != اشاره_تهی) {
        edits->جمع_کن_Unchanged(length);
    }
    إذا ((options & U_OMIT_UNCHANGED_متن) == 0) {
        sink.Append(reinterpret_cast<ثابت حرف *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *حد,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    إذا ((حد - s) > INT32_MAX) {
        error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    int32_t length = static_cast<int32_t>(حد - s);
    إذا (length > 0) {
        append_Non_خالی_Unchanged(s, length, sink, options, edits);
    }
    أرجع درست;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = افتراضي;

فراغ
CharStringByteSink::Append(ثابت حرف* بایتها, int32_t n) {
    UError_کد وضعیت = U_ZERO_ERROR;
    dest_.append(بایتها, n, وضعیت);
    // Any errors are silently ignored.
}

حرف*
CharStringByteSink::Get_Append_میانگیر(int32_t min_ظرفیت_2,
                                    int32_t desired_ظرفیت_hint,
                                    حرف* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_ظرفیت_2,
                                    int32_t* نتیجه_ظرفیت_2) U_LIFETIME_BOUND {
    إذا (min_ظرفیت_2 < 1 || scratch_ظرفیت_2 < min_ظرفیت_2) {
        *نتیجه_ظرفیت_2 = 0;
        أرجع اشاره_تهی;
    }

    UError_کد وضعیت = U_ZERO_ERROR;
    حرف* نتیجه = dest_.get_Append_میانگیر(
            min_ظرفیت_2,
            desired_ظرفیت_hint,
            *نتیجه_ظرفیت_2,
            وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        أرجع نتیجه;
    }

    *نتیجه_ظرفیت_2 = scratch_ظرفیت_2;
    أرجع scratch;
}

U_NAMESPACE_انتها
