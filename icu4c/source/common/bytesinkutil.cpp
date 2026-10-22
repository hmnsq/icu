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
ByteSinkUtil::appendChange(int32_t length, ثابت char16_t *s16, int32_t s16_اوږدتیا,
                           ByteSink &sink, Edits *edits, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    حرف scratch[200];
    int32_t s8_اوږدتیا = 0;
    لكل (int32_t i = 0; i < s16_اوږدتیا;) {
        int32_t capacity;
        int32_t desired_ظرفیت = s16_اوږدتیا - i;
        إذا (desired_ظرفیت < (INT32_MAX / 3)) {
            desired_ظرفیت *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } وإلا إذا (desired_ظرفیت < (INT32_MAX / 2)) {
            desired_ظرفیت *= 2;
        } وإلا {
            desired_ظرفیت = INT32_MAX;
        }
        حرف *بفر = sink.Get_Append_بفر(U8_MAX_اوږدتیا, desired_ظرفیت,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_اوږدتیا - 1;
        int32_t j = 0;
        لكل (; i < s16_اوږدتیا && j < capacity;) {
            UChar32 c;
            U16_بل_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(بفر, j, c);
        }
        إذا (j > (INT32_MAX - s8_اوږدتیا)) {
            error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع خطأ;
        }
        sink.Append(بفر, j);
        s8_اوږدتیا += j;
    }
    إذا (edits != تش_اشاره) {
        edits->جمع_کړه_Replace(length, s8_اوږدتیا);
    }
    أرجع ریښتیا;
}

UBool
ByteSinkUtil::appendChange(ثابت uint8_t *s, ثابت uint8_t *حد,
                           ثابت char16_t *s16, int32_t s16_اوږدتیا,
                           ByteSink &sink, Edits *edits, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    إذا ((حد - s) > INT32_MAX) {
        error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    أرجع appendChange(static_cast<int32_t>(حد - s), s16, s16_اوږدتیا, sink, edits, error_کوډ);
}

فراغ
ByteSinkUtil::append_کوډ_ټکی(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    حرف s8[U8_MAX_اوږدتیا];
    int32_t s8_اوږدتیا = 0;
    U8_APPEND_UNSAFE(s8, s8_اوږدتیا, c);
    إذا (edits != تش_اشاره) {
        edits->جمع_کړه_Replace(length, s8_اوږدتیا);
    }
    sink.Append(s8, s8_اوږدتیا);
}

نطاق {

// See unicode/utf8.h U8_APPEND_UNSAFE().
مضمن uint8_t getTwoByteLead(UChar32 c) { أرجع static_cast<uint8_t>((c >> 6) | 0xc0); }
مضمن uint8_t getTwoByteTrail(UChar32 c) { أرجع static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

فراغ
ByteSinkUtil::append_Two_بایټونه(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    حرف s8[2] = {static_cast<حرف>(getTwoByteLead(c)), static_cast<حرف>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

فراغ
ByteSinkUtil::append_Non_تشه_Unchanged(ثابت uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    إذا (edits != تش_اشاره) {
        edits->جمع_کړه_Unchanged(length);
    }
    إذا ((options & U_OMIT_UNCHANGED_متن) == 0) {
        sink.Append(reinterpret_cast<ثابت حرف *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *حد,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    إذا ((حد - s) > INT32_MAX) {
        error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع خطأ;
    }
    int32_t length = static_cast<int32_t>(حد - s);
    إذا (length > 0) {
        append_Non_تشه_Unchanged(s, length, sink, options, edits);
    }
    أرجع ریښتیا;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = افتراضي;

فراغ
CharStringByteSink::Append(ثابت حرف* بایټونه, int32_t n) {
    UError_کوډ حالت = U_ZERO_ERROR;
    dest_.append(بایټونه, n, حالت);
    // Any errors are silently ignored.
}

حرف*
CharStringByteSink::Get_Append_بفر(int32_t min_ظرفیت_2,
                                    int32_t desired_ظرفیت_hint,
                                    حرف* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_ظرفیت_2,
                                    int32_t* پایله_ظرفیت_2) U_LIFETIME_BOUND {
    إذا (min_ظرفیت_2 < 1 || scratch_ظرفیت_2 < min_ظرفیت_2) {
        *پایله_ظرفیت_2 = 0;
        أرجع تش_اشاره;
    }

    UError_کوډ حالت = U_ZERO_ERROR;
    حرف* پایله = dest_.get_Append_بفر(
            min_ظرفیت_2,
            desired_ظرفیت_hint,
            *پایله_ظرفیت_2,
            حالت);
    إذا (U_SUCCESS(حالت)) {
        أرجع پایله;
    }

    *پایله_ظرفیت_2 = scratch_ظرفیت_2;
    أرجع scratch;
}

U_NAMESPACE_پای_ټکی
