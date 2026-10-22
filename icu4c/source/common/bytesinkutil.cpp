// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringoptions.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ಸ್ಥಿರಾಂಕ char16_t *s16, int32_t s16_ಉದ್ದಳತೆ,
                           ByteSink &sink, Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಅಕ್ಷರ scratch[200];
    int32_t s8_ಉದ್ದಳತೆ = 0;
    ಗಾಗಿ (int32_t i = 0; i < s16_ಉದ್ದಳತೆ;) {
        int32_t capacity;
        int32_t desired_ಸಾಮರ್ಥ್ಯ = s16_ಉದ್ದಳತೆ - i;
        ಆದರೆ (desired_ಸಾಮರ್ಥ್ಯ < (INT32_MAX / 3)) {
            desired_ಸಾಮರ್ಥ್ಯ *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (desired_ಸಾಮರ್ಥ್ಯ < (INT32_MAX / 2)) {
            desired_ಸಾಮರ್ಥ್ಯ *= 2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            desired_ಸಾಮರ್ಥ್ಯ = INT32_MAX;
        }
        ಅಕ್ಷರ *ಬಫರ್ = sink.Get_Append_ಬಫರ್(U8_MAX_ಉದ್ದಳತೆ, desired_ಸಾಮರ್ಥ್ಯ,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_ಉದ್ದಳತೆ - 1;
        int32_t j = 0;
        ಗಾಗಿ (; i < s16_ಉದ್ದಳತೆ && j < capacity;) {
            UChar32 c;
            U16_ಮುಂದಿನ_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(ಬಫರ್, j, c);
        }
        ಆದರೆ (j > (INT32_MAX - s8_ಉದ್ದಳತೆ)) {
            error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        sink.Append(ಬಫರ್, j);
        s8_ಉದ್ದಳತೆ += j;
    }
    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
        edits->ಕೂಡಿಸು_Replace(length, s8_ಉದ್ದಳತೆ);
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool
ByteSinkUtil::appendChange(ಸ್ಥಿರಾಂಕ uint8_t *s, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ,
                           ಸ್ಥಿರಾಂಕ char16_t *s16, int32_t s16_ಉದ್ದಳತೆ,
                           ByteSink &sink, Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಆದರೆ ((ಎಲ್ಲೆ - s) > INT32_MAX) {
        error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ appendChange(static_cast<int32_t>(ಎಲ್ಲೆ - s), s16, s16_ಉದ್ದಳತೆ, sink, edits, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
ByteSinkUtil::append_ಸಂಕೇತ_ಬಿಂದು(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    ಅಕ್ಷರ s8[U8_MAX_ಉದ್ದಳತೆ];
    int32_t s8_ಉದ್ದಳತೆ = 0;
    U8_APPEND_UNSAFE(s8, s8_ಉದ್ದಳತೆ, c);
    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
        edits->ಕೂಡಿಸು_Replace(length, s8_ಉದ್ದಳತೆ);
    }
    sink.Append(s8, s8_ಉದ್ದಳತೆ);
}

ನಾಮಾಕಾಶ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
ಸಾಲೊಳಗೆ uint8_t getTwoByteLead(UChar32 c) { ಹಿಂತಿರುಗಿ static_cast<uint8_t>((c >> 6) | 0xc0); }
ಸಾಲೊಳಗೆ uint8_t getTwoByteTrail(UChar32 c) { ಹಿಂತಿರುಗಿ static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

ಶೂನ್ಯ
ByteSinkUtil::append_Two_ಬೈಟುಗಳು(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    ಅಕ್ಷರ s8[2] = {static_cast<ಅಕ್ಷರ>(getTwoByteLead(c)), static_cast<ಅಕ್ಷರ>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

ಶೂನ್ಯ
ByteSinkUtil::append_Non_ಖಾಲಿಗಳು_Unchanged(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
        edits->ಕೂಡಿಸು_Unchanged(length);
    }
    ಆದರೆ ((options & U_OMIT_UNCHANGED_ಪಠ್ಯ) == 0) {
        sink.Append(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ಸ್ಥಿರಾಂಕ uint8_t *s, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಆದರೆ ((ಎಲ್ಲೆ - s) > INT32_MAX) {
        error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    int32_t length = static_cast<int32_t>(ಎಲ್ಲೆ - s);
    ಆದರೆ (length > 0) {
        append_Non_ಖಾಲಿಗಳು_Unchanged(s, length, sink, options, edits);
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = ಪೂರ್ವನಿಯೋಜಿತ;

ಶೂನ್ಯ
CharStringByteSink::Append(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಬೈಟುಗಳು, int32_t n) {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    dest_.append(ಬೈಟುಗಳು, n, ಸ್ಥಿತಿ_2);
    // Any errors are silently ignored.
}

ಅಕ್ಷರ*
CharStringByteSink::Get_Append_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ_2,
                                    int32_t desired_ಸಾಮರ್ಥ್ಯ_hint,
                                    ಅಕ್ಷರ* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_ಸಾಮರ್ಥ್ಯ_2,
                                    int32_t* ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2) U_LIFETIME_BOUND {
    ಆದರೆ (min_ಸಾಮರ್ಥ್ಯ_2 < 1 || scratch_ಸಾಮರ್ಥ್ಯ_2 < min_ಸಾಮರ್ಥ್ಯ_2) {
        *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2 = 0;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ಅಕ್ಷರ* ಫಲಿತಾಂಶ = dest_.get_Append_ಬಫರ್(
            min_ಸಾಮರ್ಥ್ಯ_2,
            desired_ಸಾಮರ್ಥ್ಯ_hint,
            *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2,
            ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2 = scratch_ಸಾಮರ್ಥ್ಯ_2;
    ಹಿಂತಿರುಗಿ scratch;
}

U_NAMESPACE_ಕೊನೆ
