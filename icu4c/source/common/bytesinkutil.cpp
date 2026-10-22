// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringoptions.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ਅਚਲ char16_t *s16, int32_t s16_ਲੰਮਾਈ,
                           ByteSink &sink, Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    ਅੱਖਰ scratch[200];
    int32_t s8_ਲੰਮਾਈ = 0;
    ਲਈ (int32_t i = 0; i < s16_ਲੰਮਾਈ;) {
        int32_t capacity;
        int32_t desired_ਸਮਰੱਥਾ = s16_ਲੰਮਾਈ - i;
        ਜੇ (desired_ਸਮਰੱਥਾ < (INT32_MAX / 3)) {
            desired_ਸਮਰੱਥਾ *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } ਨਹੀਂਤਾਂ ਜੇ (desired_ਸਮਰੱਥਾ < (INT32_MAX / 2)) {
            desired_ਸਮਰੱਥਾ *= 2;
        } ਨਹੀਂਤਾਂ {
            desired_ਸਮਰੱਥਾ = INT32_MAX;
        }
        ਅੱਖਰ *ਬਫਰ = sink.Get_Append_ਬਫਰ(U8_MAX_ਲੰਮਾਈ, desired_ਸਮਰੱਥਾ,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_ਲੰਮਾਈ - 1;
        int32_t j = 0;
        ਲਈ (; i < s16_ਲੰਮਾਈ && j < capacity;) {
            UChar32 c;
            U16_ਅਗਲਾ_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(ਬਫਰ, j, c);
        }
        ਜੇ (j > (INT32_MAX - s8_ਲੰਮਾਈ)) {
            error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }
        sink.Append(ਬਫਰ, j);
        s8_ਲੰਮਾਈ += j;
    }
    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
        edits->ਜੋੜੋ_Replace(length, s8_ਲੰਮਾਈ);
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool
ByteSinkUtil::appendChange(ਅਚਲ uint8_t *s, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ,
                           ਅਚਲ char16_t *s16, int32_t s16_ਲੰਮਾਈ,
                           ByteSink &sink, Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    ਜੇ ((ਹੱਦਬੰਦੀ - s) > INT32_MAX) {
        error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ appendChange(static_cast<int32_t>(ਹੱਦਬੰਦੀ - s), s16, s16_ਲੰਮਾਈ, sink, edits, error_ਕੋਡ);
}

ਖਾਲੀ
ByteSinkUtil::append_ਕੋਡ_ਬਿੰਦੂ(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    ਅੱਖਰ s8[U8_MAX_ਲੰਮਾਈ];
    int32_t s8_ਲੰਮਾਈ = 0;
    U8_APPEND_UNSAFE(s8, s8_ਲੰਮਾਈ, c);
    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
        edits->ਜੋੜੋ_Replace(length, s8_ਲੰਮਾਈ);
    }
    sink.Append(s8, s8_ਲੰਮਾਈ);
}

ਨਾਮਾਕਾਸ਼ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
ਸਤਰਅੰਦਰ uint8_t getTwoByteLead(UChar32 c) { ਵਾਪਸ static_cast<uint8_t>((c >> 6) | 0xc0); }
ਸਤਰਅੰਦਰ uint8_t getTwoByteTrail(UChar32 c) { ਵਾਪਸ static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

ਖਾਲੀ
ByteSinkUtil::append_Two_ਬਾਈਟਾਂ(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    ਅੱਖਰ s8[2] = {static_cast<ਅੱਖਰ>(getTwoByteLead(c)), static_cast<ਅੱਖਰ>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

ਖਾਲੀ
ByteSinkUtil::append_Non_ਖਾਲੀ_ਥਾਵਾਂ_Unchanged(ਅਚਲ uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
        edits->ਜੋੜੋ_Unchanged(length);
    }
    ਜੇ ((options & U_OMIT_UNCHANGED_ਪਾਠ) == 0) {
        sink.Append(reinterpret_cast<ਅਚਲ ਅੱਖਰ *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ਅਚਲ uint8_t *s, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    ਜੇ ((ਹੱਦਬੰਦੀ - s) > INT32_MAX) {
        error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    int32_t length = static_cast<int32_t>(ਹੱਦਬੰਦੀ - s);
    ਜੇ (length > 0) {
        append_Non_ਖਾਲੀ_ਥਾਵਾਂ_Unchanged(s, length, sink, options, edits);
    }
    ਵਾਪਸ ਸੱਚ;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = ਮੂਲ;

ਖਾਲੀ
CharStringByteSink::Append(ਅਚਲ ਅੱਖਰ* ਬਾਈਟਾਂ, int32_t n) {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    dest_.append(ਬਾਈਟਾਂ, n, ਸਥਿਤੀ);
    // Any errors are silently ignored.
}

ਅੱਖਰ*
CharStringByteSink::Get_Append_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ_2,
                                    int32_t desired_ਸਮਰੱਥਾ_hint,
                                    ਅੱਖਰ* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_ਸਮਰੱਥਾ_2,
                                    int32_t* ਨਤੀਜਾ_ਸਮਰੱਥਾ_2) U_LIFETIME_BOUND {
    ਜੇ (min_ਸਮਰੱਥਾ_2 < 1 || scratch_ਸਮਰੱਥਾ_2 < min_ਸਮਰੱਥਾ_2) {
        *ਨਤੀਜਾ_ਸਮਰੱਥਾ_2 = 0;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ਅੱਖਰ* ਨਤੀਜਾ = dest_.get_Append_ਬਫਰ(
            min_ਸਮਰੱਥਾ_2,
            desired_ਸਮਰੱਥਾ_hint,
            *ਨਤੀਜਾ_ਸਮਰੱਥਾ_2,
            ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    *ਨਤੀਜਾ_ਸਮਰੱਥਾ_2 = scratch_ਸਮਰੱਥਾ_2;
    ਵਾਪਸ scratch;
}

U_NAMESPACE_ਸਿਰਾ
