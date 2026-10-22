// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  normalizer2impl.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009nov22
*   created by: Markus W. Scherer
*/

// #define UCPTRIE_DEBUG

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringoptions.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/umutablecptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucptrie_impl.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
ಸಾಲೊಳಗೆ uint8_t leadByteForCP(UChar32 c) {
    ಆದರೆ (c <= 0x7f) {
        ಹಿಂತಿರುಗಿ static_cast<uint8_t>(c);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= 0x7ff) {
        ಹಿಂತಿರುಗಿ static_cast<uint8_t>(0xc0 + (c >> 6));
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Should not occur because ccc(U+0300)!=0.
        ಹಿಂತಿರುಗಿ 0xe0;
    }
}

/**
 * Returns the code point from one single well-formed UTF-8 byte sequence
 * between cpStart and cpLimit.
 *
 * Trie UTF-8 macros do not assemble whole code points (for efficiency).
 * When we do need the code point, we call this function.
 * We should not need it for normalization-inert data (norm16==0).
 * Illegal sequences yield the error value norm16==0 just like real normalization-inert code points.
 */
UChar32 ಸಂಕೇತ_ಬಿಂದು_From_Valid_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *cp_ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *cp_ಎಲ್ಲೆ) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cp_ಆರಂಭ < cp_ಎಲ್ಲೆ);
    uint8_t c = *cp_ಆರಂಭ;
    ಆಯ್ಕೆ(cp_ಎಲ್ಲೆ-cp_ಆರಂಭ) {
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ c;
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ ((c&0x1f)<<6) | (cp_ಆರಂಭ[1]&0x3f);
    ಸ್ಥಿತಿ 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        ಹಿಂತಿರುಗಿ static_cast<char16_t>((c << 12) | ((cp_ಆರಂಭ[1] & 0x3f) << 6) | (cp_ಆರಂಭ[2] & 0x3f));
    ಸ್ಥಿತಿ 4:
        ಹಿಂತಿರುಗಿ ((c&7)<<18) | ((cp_ಆರಂಭ[1]&0x3f)<<12) | ((cp_ಆರಂಭ[2]&0x3f)<<6) | (cp_ಆರಂಭ[3]&0x3f);
    ಪೂರ್ವನಿಯೋಜಿತ:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 ಹಿಂದಿನ_Hangul_Or_Jamo(ಸ್ಥಿರಾಂಕ uint8_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *p) {
    ಆದರೆ ((p - ಆರಂಭ) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        ಆದರೆ (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            ಹಿಂತಿರುಗಿ ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    ಹಿಂತಿರುಗಿ U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t get_Jamo_TMinus_ತಳ(ಸ್ಥಿರಾಂಕ uint8_t *src, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ) {
    // Jamo T: E1 86 A8..E1 87 82
    ಆದರೆ ((ಎಲ್ಲೆ - src) >= 3 && *src == 0xe1) {
        ಆದರೆ (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            ಆದರೆ (0xa8 <= t && t <= 0xbf) {
                ಹಿಂತಿರುಗಿ t - 0xa7;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (src[1] == 0x87) {
            uint8_t t = src[2];
            ಆದರೆ (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                ಹಿಂತಿರುಗಿ t - (0xa7 - 0x40);
            }
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

ಶೂನ್ಯ
append_ಸಂಕೇತ_ಬಿಂದು_Delta(ಸ್ಥಿರಾಂಕ uint8_t *cp_ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *cp_ಎಲ್ಲೆ, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    ಅಕ್ಷರ ಬಫರ್[U8_MAX_ಉದ್ದಳತೆ];
    int32_t length;
    int32_t cp_ಉದ್ದಳತೆ = static_cast<int32_t>(cp_ಎಲ್ಲೆ - cp_ಆರಂಭ);
    ಆದರೆ (cp_ಉದ್ದಳತೆ == 1) {
        // The builder makes ASCII map to ASCII.
        ಬಫರ್[0] = static_cast<uint8_t>(*cp_ಆರಂಭ + delta);
        length = 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t trail = *(cp_ಎಲ್ಲೆ-1) + delta;
        ಆದರೆ (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cp_ಎಲ್ಲೆ;
            length = 0;
            ಮಾಡು { ಬಫರ್[length++] = *cp_ಆರಂಭ++; } ಆಗ (cp_ಆರಂಭ < cp_ಎಲ್ಲೆ);
            ಬಫರ್[length++] = static_cast<uint8_t>(trail);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = ಸಂಕೇತ_ಬಿಂದು_From_Valid_UTF8(cp_ಆರಂಭ, cp_ಎಲ್ಲೆ) + delta;
            length = 0;
            U8_APPEND_UNSAFE(ಬಫರ್, length, c);
        }
    }
    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
        edits->ಕೂಡಿಸು_Replace(cp_ಉದ್ದಳತೆ, length);
    }
    sink.Append(ಬಫರ್, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_ಬಫರ್::Reordering_ಬಫರ್(ಸ್ಥಿರಾಂಕ Normalizer2Impl &ni, UnicodeString &dest,
                                   UError_ಸಂಕೇತ &error_ಸಂಕೇತ) :
        impl(ni), str(dest),
        ಆರಂಭ(str.get_ಬಫರ್(8)), reorder_ಆರಂಭ(ಆರಂಭ), ಎಲ್ಲೆ(ಆರಂಭ),
        ಉಳಿದ_ಸಾಮರ್ಥ್ಯ(str.get_ಸಾಮರ್ಥ್ಯ()), ಕೊನೆಯ_CC(0) {
    ಆದರೆ (ಆರಂಭ == ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(error_ಸಂಕೇತ)) {
        // getBuffer() already did str.setToBogus()
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}

UBool Reordering_ಬಫರ್::init(int32_t dest_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t length=str.length();
    ಆರಂಭ=str.get_ಬಫರ್(dest_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ(ಆರಂಭ==ಶೂನ್ಯಸೂಚಕ) {
        // getBuffer() already did str.setToBogus()
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಎಲ್ಲೆ=ಆರಂಭ+length;
    ಉಳಿದ_ಸಾಮರ್ಥ್ಯ=str.get_ಸಾಮರ್ಥ್ಯ()-length;
    reorder_ಆರಂಭ=ಆರಂಭ;
    ಆದರೆ(ಆರಂಭ==ಎಲ್ಲೆ) {
        ಕೊನೆಯ_CC=0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಣ_Iterator();
        ಕೊನೆಯ_CC=ಹಿಂದಿನ_CC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        ಆದರೆ(ಕೊನೆಯ_CC>1) {
            ಆಗ(ಹಿಂದಿನ_CC()>1) {}
        }
        reorder_ಆರಂಭ=ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool Reordering_ಬಫರ್::equals(ಸ್ಥಿರಾಂಕ char16_t *other_ಆರಂಭ, ಸ್ಥಿರಾಂಕ char16_t *other_ಎಲ್ಲೆ) ಸ್ಥಿರಾಂಕ {
    int32_t length = static_cast<int32_t>(ಎಲ್ಲೆ - ಆರಂಭ);
    ಹಿಂತಿರುಗಿ
        length == static_cast<int32_t>(other_ಎಲ್ಲೆ - other_ಆರಂಭ) &&
        0==u_memcmp(ಆರಂಭ, other_ಆರಂಭ, length);
}

UBool Reordering_ಬಫರ್::equals(ಸ್ಥಿರಾಂಕ uint8_t *other_ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *other_ಎಲ್ಲೆ) ಸ್ಥಿರಾಂಕ {
    U_ASSERT((other_ಎಲ್ಲೆ - other_ಆರಂಭ) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(ಎಲ್ಲೆ - ಆರಂಭ);
    int32_t other_ಉದ್ದಳತೆ = static_cast<int32_t>(other_ಎಲ್ಲೆ - other_ಆರಂಭ);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    ಆದರೆ (other_ಉದ್ದಳತೆ < length || (other_ಉದ್ದಳತೆ / 3) > length) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    ಗಾಗಿ (int32_t i = 0, j = 0;;) {
        ಆದರೆ (i >= length) {
            ಹಿಂತಿರುಗಿ j >= other_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (j >= other_ಉದ್ದಳತೆ) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_ಮುಂದಿನ_UNSAFE(ಆರಂಭ, i, c);
        U8_ಮುಂದಿನ_UNSAFE(other_ಆರಂಭ, j, other);
        ಆದರೆ (c != other) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
}

UBool Reordering_ಬಫರ್::appendSupplementary(UChar32 c, uint8_t cc, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(ಉಳಿದ_ಸಾಮರ್ಥ್ಯ<2 && !resize(2, error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(ಕೊನೆಯ_CC<=cc || cc==0) {
        ಎಲ್ಲೆ[0]=U16_LEAD(c);
        ಎಲ್ಲೆ[1]=U16_TRAIL(c);
        ಎಲ್ಲೆ+=2;
        ಕೊನೆಯ_CC=cc;
        ಆದರೆ(cc<=1) {
            reorder_ಆರಂಭ=ಎಲ್ಲೆ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        insert(c, cc);
    }
    ಉಳಿದ_ಸಾಮರ್ಥ್ಯ-=2;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool Reordering_ಬಫರ್::append(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(length==0) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(ಉಳಿದ_ಸಾಮರ್ಥ್ಯ<length && !resize(length, error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಉಳಿದ_ಸಾಮರ್ಥ್ಯ-=length;
    ಆದರೆ(ಕೊನೆಯ_CC<=leadCC || leadCC==0) {
        ಆದರೆ(trailCC<=1) {
            reorder_ಆರಂಭ=ಎಲ್ಲೆ+length;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(leadCC<=1) {
            reorder_ಆರಂಭ=ಎಲ್ಲೆ+1;  // Ok if not a code point boundary.
        }
        ಸ್ಥಿರಾಂಕ char16_t *s_ಎಲ್ಲೆ=s+length;
        ಮಾಡು { *ಎಲ್ಲೆ++=*s++; } ಆಗ(s!=s_ಎಲ್ಲೆ);
        ಕೊನೆಯ_CC=trailCC;
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t i=0;
        UChar32 c;
        U16_ಮುಂದಿನ(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        ಆಗ(i<length) {
            U16_ಮುಂದಿನ(s, i, length, c);
            ಆದರೆ(i<length) {
                ಆದರೆ (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } ಇಲ್ಲದಿದ್ದರೆ {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                leadCC=trailCC;
            }
            append(c, leadCC, error_ಸಂಕೇತ);
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool Reordering_ಬಫರ್::appendZeroCC(UChar32 c, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t cp_ಉದ್ದಳತೆ=U16_ಉದ್ದಳತೆ(c);
    ಆದರೆ(ಉಳಿದ_ಸಾಮರ್ಥ್ಯ<cp_ಉದ್ದಳತೆ && !resize(cp_ಉದ್ದಳತೆ, error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಉಳಿದ_ಸಾಮರ್ಥ್ಯ-=cp_ಉದ್ದಳತೆ;
    ಆದರೆ(cp_ಉದ್ದಳತೆ==1) {
        *ಎಲ್ಲೆ++ = static_cast<char16_t>(c);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಎಲ್ಲೆ[0]=U16_LEAD(c);
        ಎಲ್ಲೆ[1]=U16_TRAIL(c);
        ಎಲ್ಲೆ+=2;
    }
    ಕೊನೆಯ_CC=0;
    reorder_ಆರಂಭ=ಎಲ್ಲೆ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool Reordering_ಬಫರ್::appendZeroCC(ಸ್ಥಿರಾಂಕ char16_t *s, ಸ್ಥಿರಾಂಕ char16_t *s_ಎಲ್ಲೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(s==s_ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    int32_t length = static_cast<int32_t>(s_ಎಲ್ಲೆ - s);
    ಆದರೆ(ಉಳಿದ_ಸಾಮರ್ಥ್ಯ<length && !resize(length, error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    u_memcpy(ಎಲ್ಲೆ, s, length);
    ಎಲ್ಲೆ+=length;
    ಉಳಿದ_ಸಾಮರ್ಥ್ಯ-=length;
    ಕೊನೆಯ_CC=0;
    reorder_ಆರಂಭ=ಎಲ್ಲೆ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ Reordering_ಬಫರ್::remove() {
    reorder_ಆರಂಭ=ಎಲ್ಲೆ=ಆರಂಭ;
    ಉಳಿದ_ಸಾಮರ್ಥ್ಯ=str.get_ಸಾಮರ್ಥ್ಯ();
    ಕೊನೆಯ_CC=0;
}

ಶೂನ್ಯ Reordering_ಬಫರ್::removeSuffix(int32_t suffix_ಉದ್ದಳತೆ) {
    ಆದರೆ(suffix_ಉದ್ದಳತೆ<(ಎಲ್ಲೆ-ಆರಂಭ)) {
        ಎಲ್ಲೆ-=suffix_ಉದ್ದಳತೆ;
        ಉಳಿದ_ಸಾಮರ್ಥ್ಯ+=suffix_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಎಲ್ಲೆ=ಆರಂಭ;
        ಉಳಿದ_ಸಾಮರ್ಥ್ಯ=str.get_ಸಾಮರ್ಥ್ಯ();
    }
    ಕೊನೆಯ_CC=0;
    reorder_ಆರಂಭ=ಎಲ್ಲೆ;
}

UBool Reordering_ಬಫರ್::resize(int32_t append_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t reorder_ಆರಂಭ_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(reorder_ಆರಂಭ - ಆರಂಭ);
    int32_t length = static_cast<int32_t>(ಎಲ್ಲೆ - ಆರಂಭ);
    str.release_ಬಫರ್(length);
    int32_t new_ಸಾಮರ್ಥ್ಯ=length+append_ಉದ್ದಳತೆ;
    int32_t double_ಸಾಮರ್ಥ್ಯ=2*str.get_ಸಾಮರ್ಥ್ಯ();
    ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ<double_ಸಾಮರ್ಥ್ಯ) {
        new_ಸಾಮರ್ಥ್ಯ=double_ಸಾಮರ್ಥ್ಯ;
    }
    ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ<256) {
        new_ಸಾಮರ್ಥ್ಯ=256;
    }
    ಆರಂಭ=str.get_ಬಫರ್(new_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ(ಆರಂಭ==ಶೂನ್ಯಸೂಚಕ) {
        // getBuffer() already did str.setToBogus()
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    reorder_ಆರಂಭ=ಆರಂಭ+reorder_ಆರಂಭ_ಸೂಚ್ಯಂಕ;
    ಎಲ್ಲೆ=ಆರಂಭ+length;
    ಉಳಿದ_ಸಾಮರ್ಥ್ಯ=str.get_ಸಾಮರ್ಥ್ಯ()-length;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ Reordering_ಬಫರ್::skip_ಹಿಂದಿನ() {
    ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ=ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
    char16_t c=*--ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
    ಆದರೆ(U16_IS_TRAIL(c) && ಆರಂಭ<ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ && U16_IS_LEAD(*(ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ-1))) {
        --ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
    }
}

uint8_t Reordering_ಬಫರ್::ಹಿಂದಿನ_CC() {
    ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ=ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
    ಆದರೆ(reorder_ಆರಂಭ>=ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UChar32 c=*--ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
    char16_t c2;
    ಆದರೆ(U16_IS_TRAIL(c) && ಆರಂಭ<ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ && U16_IS_LEAD(c2=*(ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ-1))) {
        --ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    ಹಿಂತಿರುಗಿ impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
ಶೂನ್ಯ Reordering_ಬಫರ್::insert(UChar32 c, uint8_t cc) {
    ಗಾಗಿ(ಗಣ_Iterator(), skip_ಹಿಂದಿನ(); ಹಿಂದಿನ_CC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=ಎಲ್ಲೆ;
    char16_t *r=ಎಲ್ಲೆ+=U16_ಉದ್ದಳತೆ(c);
    ಮಾಡು {
        *--r=*--q;
    } ಆಗ(ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ!=q);
    write_ಸಂಕೇತ_ಬಿಂದು(q, c);
    ಆದರೆ(cc<=1) {
        reorder_ಆರಂಭ=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

ರಚನೆ Canon_Iter_ದತ್ತಾಂಶ : ಸಾರ್ವಜನಿಕ UMemory {
    Canon_Iter_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ~Canon_Iter_ದತ್ತಾಂಶ();
    ಶೂನ್ಯ ಕೂಡಿಸು_ಗಮ್ಯ_ಆರಂಭ_ಗಣ(UChar32 origin, UChar32 decompLead, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canon_ಆರಂಭ_Sets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    ಅಳಿಸು f_Canon_Iter_ದತ್ತಾಂಶ;
}

ಶೂನ್ಯ
Normalizer2Impl::init(ಸ್ಥಿರಾಂಕ int32_t *ಒಳ_Indexes, ಸ್ಥಿರಾಂಕ UCPTrie *ಒಳ_Trie,
                      ಸ್ಥಿರಾಂಕ uint16_t *ಒಳ_Extra_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_Small_FCD) {
    minDecompNoCP = static_cast<char16_t>(ಒಳ_Indexes[IX_MIN_DECOMP_NO_CP]);
    minCompNoMaybeCP = static_cast<char16_t>(ಒಳ_Indexes[IX_MIN_COMP_NO_MAYBE_CP]);
    minLcccCP = static_cast<char16_t>(ಒಳ_Indexes[IX_MIN_LCCC_CP]);

    minYesNo = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_YES_NO]);
    minYesNoMappingsOnly = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_YES_NO_MAPPINGS_ONLY]);
    minNoNo = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_NO_NO]);
    minNoNoCompBoundaryBefore = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE]);
    minNoNoCompNoMaybeCC = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC]);
    min_No_No_ಖಾಲಿಗಳು = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_NO_NO_ಖಾಲಿಗಳು]);
    ಎಲ್ಲೆ_No_No = static_cast<uint16_t>(ಒಳ_Indexes[IX_ಎಲ್ಲೆ_NO_NO]);
    minMaybeNo = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_MAYBE_NO]);
    minMaybeNoCombinesFwd = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_MAYBE_NO_COMBINES_FWD]);
    minMaybeYes = static_cast<uint16_t>(ಒಳ_Indexes[IX_MIN_MAYBE_YES]);
    U_ASSERT((minMaybeNo & 7) == 0);  // 8-aligned for noNoDelta bit fields
    centerNoNoDelta = (minMaybeNo >> DELTA_ಸರಿಸು) - MAX_DELTA - 1;

    normTrie=ಒಳ_Trie;
    extra_ದತ್ತಾಂಶ=ಒಳ_Extra_ದತ್ತಾಂಶ;
    smallFCD=ಒಳ_Small_FCD;
}

U_CDECL_BEGIN

ಸ್ಥಿರ uint32_t U_CALLCONV
segmentStarterMapper(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * /*context*/, uint32_t value) {
    ಹಿಂತಿರುಗಿ value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_ಕೊನೆ

ಶೂನ್ಯ
Normalizer2Impl::ಕೂಡಿಸು_Lccc_Chars(Unicode_ಗಣ &ಗಣ) ಸ್ಥಿರಾಂಕ {
    UChar32 ಆರಂಭ = 0, ಕೊನೆ;
    uint32_t norm16;
    ಆಗ ((ಕೊನೆ = ucptrie_getRange(normTrie, ಆರಂಭ, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &norm16)) >= 0) {
        ಆದರೆ (norm16 > Normalizer2Impl::MIN_ಸಾಮಾನ್ಯ_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            ಗಣ.ಕೂಡಿಸು(ಆರಂಭ, ಕೊನೆ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (minNoNoCompNoMaybeCC <= norm16 && norm16 < ಎಲ್ಲೆ_No_No) {
            uint16_t fcd16 = getFCD16(ಆರಂಭ);
            ಆದರೆ (fcd16 > 0xff) { ಗಣ.ಕೂಡಿಸು(ಆರಂಭ, ಕೊನೆ); }
        }
        ಆರಂಭ = ಕೊನೆ + 1;
    }
}

ಶೂನ್ಯ
Normalizer2Impl::ಕೂಡಿಸು_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ & /*errorCode*/) ಸ್ಥಿರಾಂಕ {
    // Add the start code point of each same-value range of the trie.
    UChar32 ಆರಂಭ = 0, ಕೊನೆ;
    uint32_t value;
    ಆಗ ((ಕೊನೆ = ucptrie_getRange(normTrie, ಆರಂಭ, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &value)) >= 0) {
        sa->ಕೂಡಿಸು(sa->ಗಣ, ಆರಂಭ);
        ಆದರೆ (ಆರಂಭ != ಕೊನೆ && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_ಮುಖವಾಡ) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(ಆರಂಭ);
            ಆಗ (++ಆರಂಭ <= ಕೊನೆ) {
                uint16_t fcd16 = getFCD16(ಆರಂಭ);
                ಆದರೆ (fcd16 != prevFCD16) {
                    sa->ಕೂಡಿಸು(sa->ಗಣ, ಆರಂಭ);
                    prevFCD16 = fcd16;
                }
            }
        }
        ಆರಂಭ = ಕೊನೆ + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    ಗಾಗಿ(char16_t c=Hangul::HANGUL_ತಳ; c<Hangul::HANGUL_ಎಲ್ಲೆ; c+=Hangul::JAMO_T_ಎಣಿಕೆ) {
        sa->ಕೂಡಿಸು(sa->ಗಣ, c);
        sa->ಕೂಡಿಸು(sa->ಗಣ, c+1);
    }
    sa->ಕೂಡಿಸು(sa->ಗಣ, Hangul::HANGUL_ಎಲ್ಲೆ); /* add Hangul+1 to continue with other properties */
}

ಶೂನ್ಯ
Normalizer2Impl::ಕೂಡಿಸು_Canon_Iter_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    ಆದರೆ (!ensure_Canon_Iter_ದತ್ತಾಂಶ(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 ಆರಂಭ = 0, ಕೊನೆ;
    uint32_t value;
    ಆಗ ((ಕೊನೆ = ucptrie_getRange(f_Canon_Iter_ದತ್ತಾಂಶ->trie, ಆರಂಭ, UCPMAP_RANGE_ಸಾಮಾನ್ಯ, 0,
                                   segmentStarterMapper, ಶೂನ್ಯಸೂಚಕ, &value)) >= 0) {
        sa->ಕೂಡಿಸು(sa->ಗಣ, ಆರಂಭ);
        ಆರಂಭ = ಕೊನೆ + 1;
    }
}

ಸ್ಥಿರಾಂಕ char16_t *
Normalizer2Impl::copy_ಕೆಳಗಿನ_ಪೂರ್ವಪ್ರತ್ಯಯ_From_Nul_Terminated(ಸ್ಥಿರಾಂಕ char16_t *src,
                                                UChar32 min_Need_ದತ್ತಾಂಶ_CP,
                                                Reordering_ಬಫರ್ *ಬಫರ್,
                                                UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ಸ್ಥಿರಾಂಕ char16_t *prevSrc=src;
    char16_t c;
    ಆಗ((c=*src++)<min_Need_ದತ್ತಾಂಶ_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    ಆದರೆ(--src!=prevSrc) {
        ಆದರೆ(ಬಫರ್!=ಶೂನ್ಯಸೂಚಕ) {
            ಬಫರ್->appendZeroCC(prevSrc, src, error_ಸಂಕೇತ);
        }
    }
    ಹಿಂತಿರುಗಿ src;
}

UnicodeString &
Normalizer2Impl::decompose(ಸ್ಥಿರಾಂಕ UnicodeString &src, UnicodeString &dest,
                           UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        dest.ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ dest;
    }
    ಸ್ಥಿರಾಂಕ char16_t *sArray=src.get_ಬಫರ್();
    ಆದರೆ(&dest==&src || sArray==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        dest.ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ dest;
}

ಶೂನ್ಯ
Normalizer2Impl::decompose(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                           UnicodeString &dest,
                           int32_t dest_ಉದ್ದಳತೆ_Estimate,
                           UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(dest_ಉದ್ದಳತೆ_Estimate<0 && ಎಲ್ಲೆ!=ಶೂನ್ಯಸೂಚಕ) {
        dest_ಉದ್ದಳತೆ_Estimate = static_cast<int32_t>(ಎಲ್ಲೆ - src);
    }
    dest.remove();
    Reordering_ಬಫರ್ ಬಫರ್(*ಇದು, dest);
    ಆದರೆ(ಬಫರ್.init(dest_ಉದ್ದಳತೆ_Estimate, error_ಸಂಕೇತ)) {
        decompose(src, ಎಲ್ಲೆ, &ಬಫರ್, error_ಸಂಕೇತ);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ಸ್ಥಿರಾಂಕ char16_t *
Normalizer2Impl::decompose(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                           Reordering_ಬಫರ್ *ಬಫರ್,
                           UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(src==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ src;
    }
    UChar32 minNoCP=minDecompNoCP;
    ಆದರೆ(ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ) {
        src=copy_ಕೆಳಗಿನ_ಪೂರ್ವಪ್ರತ್ಯಯ_From_Nul_Terminated(src, minNoCP, ಬಫರ್, error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ src;
        }
        ಎಲ್ಲೆ=u_strchr(src, 0);
    }

    ಸ್ಥಿರಾಂಕ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ಸ್ಥಿರಾಂಕ char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    ಗಾಗಿ(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        ಗಾಗಿ(prevSrc=src; src!=ಎಲ್ಲೆ;) {
            ಆದರೆ( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!U16_IS_LEAD(c)) {
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                char16_t c2;
                ಆದರೆ((src+1)!=ಎಲ್ಲೆ && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    ಆದರೆ(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        ಆದರೆ(src!=prevSrc) {
            ಆದರೆ(ಬಫರ್!=ಶೂನ್ಯಸೂಚಕ) {
                ಆದರೆ(!ಬಫರ್->appendZeroCC(prevSrc, src, error_ಸಂಕೇತ)) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                prevCC=0;
                prevBoundary=src;
            }
        }
        ಆದರೆ(src==ಎಲ್ಲೆ) {
            ನಿಲ್ಲಿಸು;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_ಉದ್ದಳತೆ(c);
        ಆದರೆ(ಬಫರ್!=ಶೂನ್ಯಸೂಚಕ) {
            ಆದರೆ(!decompose(c, norm16, *ಬಫರ್, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                ಆದರೆ(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    ಆದರೆ(cc<=1) {
                        prevBoundary=src;
                    }
                    ಮುಂದುವರಿ;
                }
            }
            ಹಿಂತಿರುಗಿ prevBoundary;  // "no" or cc out of order
        }
    }
    ಹಿಂತಿರುಗಿ src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ಸ್ಥಿರಾಂಕ char16_t *
Normalizer2Impl::decomposeShort(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆಗ(src<ಎಲ್ಲೆ) {
        ಆದರೆ (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            ಹಿಂತಿರುಗಿ src;
        }
        ಸ್ಥಿರಾಂಕ char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_ಮುಂದಿನ(normTrie, UCPTRIE_16, src, ಎಲ್ಲೆ, c, norm16);
        ಆದರೆ (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            ಹಿಂತಿರುಗಿ prevSrc;
        }
        ಆದರೆ(!decompose(c, norm16, ಬಫರ್, error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಆದರೆ (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ಹಿಂತಿರುಗಿ src;
        }
    }
    ಹಿಂತಿರುಗಿ src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_ಬಫರ್ &ಬಫರ್,
                                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    // get the decomposition and the lead and trail cc's
    ಆದರೆ (norm16 >= ಎಲ್ಲೆ_No_No) {
        ಆದರೆ (isMaybeYesOrNonZeroCC(norm16)) {
            ಹಿಂತಿರುಗಿ ಬಫರ್.append(c, getCCFromYesOrMaybeYes(norm16), error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ಆದರೆ (norm16 < minYesNo) {
        // c does not decompose
        ಹಿಂತಿರುಗಿ ಬಫರ್.append(c, 0, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        ಹಿಂತಿರುಗಿ ಬಫರ್.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_ಸಂಕೇತ);
    }
    // c decomposes, get everything from the variable-length extra data
    ಸ್ಥಿರಾಂಕ uint16_t *mapping=get_ದತ್ತಾಂಶ(norm16);
    uint16_t ಮೊದಲ_Unit=*mapping;
    int32_t length=ಮೊದಲ_Unit&MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(ಮೊದಲ_Unit >> 8);
    ಆದರೆ(ಮೊದಲ_Unit&MAPPING_HAS_CCC_LCCC_ಶಬ್ದ) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } ಇಲ್ಲದಿದ್ದರೆ {
        leadCC=0;
    }
    ಹಿಂತಿರುಗಿ ಬಫರ್.append(reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(mapping) + 1, length, ನಿಜ, leadCC, trailCC, error_ಸಂಕೇತ);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ಸ್ಥಿರಾಂಕ uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ಸ್ಥಿರಾಂಕ uint8_t *src, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ,
                               ByteSink *sink, Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(ಎಲ್ಲೆ != ಶೂನ್ಯಸೂಚಕ);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ಸ್ಥಿರಾಂಕ uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    ಗಾಗಿ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ಸ್ಥಿರಾಂಕ uint8_t *fast_ಆರಂಭ = src;
        ಸ್ಥಿರಾಂಕ uint8_t *prevSrc;
        uint16_t norm16 = 0;

        ಗಾಗಿ (;;) {
            ಆದರೆ (src == ಎಲ್ಲೆ) {
                ಆದರೆ (prevBoundary != ಎಲ್ಲೆ && sink != ಶೂನ್ಯಸೂಚಕ) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, ಎಲ್ಲೆ,
                                                  *sink, options, edits, error_ಸಂಕೇತ);
                }
                ಹಿಂತಿರುಗಿ src;
            }
            ಆದರೆ (*src < minNoLead) {
                ++src;
            } ಇಲ್ಲದಿದ್ದರೆ {
                prevSrc = src;
                UCPTRIE_FAST_U8_ಮುಂದಿನ(normTrie, UCPTRIE_16, src, ಎಲ್ಲೆ, norm16);
                ಆದರೆ (!isMostDecompYesAndZeroCC(norm16)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        ಆದರೆ (prevSrc != fast_ಆರಂಭ) {
            // The fast path looped over yes/0 characters before the current one.
            ಆದರೆ (sink != ಶೂನ್ಯಸೂಚಕ &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        ಆದರೆ (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            ಆದರೆ (prevCC <= cc || cc == 0) {
                prevCC = cc;
                ಆದರೆ (cc <= 1) {
                    ಆದರೆ (sink != ಶೂನ್ಯಸೂಚಕ &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    prevBoundary = src;
                }
                ಮುಂದುವರಿ;
            }
        }
        ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        ಆದರೆ (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            ಆದರೆ (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
            prevBoundary = prevSrc;
        }
        Reordering_ಬಫರ್ ಬಫರ್(*ಇದು, s16, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ನಿಲ್ಲಿಸು;
        }
        decomposeShort(prevBoundary, src, STOP_AT_ಎಲ್ಲೆ, ಸುಳ್ಳು /* onlyContiguous */,
                       ಬಫರ್, error_ಸಂಕೇತ);
        // Decompose until the next boundary.
        ಆದರೆ (ಬಫರ್.get_ಕೊನೆಯ_CC() > 1) {
            src = decomposeShort(src, ಎಲ್ಲೆ, STOP_AT_DECOMP_BOUNDARY, ಸುಳ್ಳು /* onlyContiguous */,
                                 ಬಫರ್, error_ಸಂಕೇತ);
        }
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ನಿಲ್ಲಿಸು;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        ಆದರೆ (isMaybeYesOrNonZeroCC(norm16) && ಬಫರ್.equals(prevBoundary, src)) {
            ಆದರೆ (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (!ByteSinkUtil::appendChange(prevBoundary, src, ಬಫರ್.get_ಆರಂಭ(), ಬಫರ್.length(),
                                            *sink, edits, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    ಹಿಂತಿರುಗಿ src;
}

ಸ್ಥಿರಾಂಕ uint8_t *
Normalizer2Impl::decomposeShort(ಸ್ಥಿರಾಂಕ uint8_t *src, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ,
                                StopAt stopAt, UBool onlyContiguous,
                                Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆಗ (src < ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_ಮುಂದಿನ(normTrie, UCPTRIE_16, src, ಎಲ್ಲೆ, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        ಆದರೆ (norm16 >= ಎಲ್ಲೆ_No_No) {
            ಆದರೆ (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                ಆದರೆ (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    ಹಿಂತಿರುಗಿ prevSrc;
                }
                c = ಸಂಕೇತ_ಬಿಂದು_From_Valid_UTF8(prevSrc, src);
                ಆದರೆ (!ಬಫರ್.append(c, cc, error_ಸಂಕೇತ)) {
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
                ಆದರೆ (stopAt == STOP_AT_DECOMP_BOUNDARY && ಬಫರ್.get_ಕೊನೆಯ_CC() <= 1) {
                    ಹಿಂತಿರುಗಿ src;
                }
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                ಆದರೆ (stopAt != STOP_AT_ಎಲ್ಲೆ) {
                    ಹಿಂತಿರುಗಿ prevSrc;
                }
                c = ಸಂಕೇತ_ಬಿಂದು_From_Valid_UTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (stopAt != STOP_AT_ಎಲ್ಲೆ && norm16 < minNoNoCompNoMaybeCC) {
            ಹಿಂತಿರುಗಿ prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        ಆದರೆ (norm16 < minYesNo) {
            ಆದರೆ (c < 0) {
                c = ಸಂಕೇತ_ಬಿಂದು_From_Valid_UTF8(prevSrc, src);
            }
            // does not decompose
            ಆದರೆ (!ಬಫರ್.append(c, 0, error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            ಆದರೆ (c < 0) {
                c = ಸಂಕೇತ_ಬಿಂದು_From_Valid_UTF8(prevSrc, src);
            }
            char16_t jamos[3];
            ಆದರೆ (!ಬಫರ್.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The character decomposes, get everything from the variable-length extra data.
            ಸ್ಥಿರಾಂಕ uint16_t *mapping = get_ದತ್ತಾಂಶ(norm16);
            uint16_t ಮೊದಲ_Unit = *mapping;
            int32_t length = ಮೊದಲ_Unit & MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ;
            uint8_t trailCC = static_cast<uint8_t>(ಮೊದಲ_Unit >> 8);
            uint8_t leadCC;
            ಆದರೆ (ಮೊದಲ_Unit & MAPPING_HAS_CCC_LCCC_ಶಬ್ದ) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } ಇಲ್ಲದಿದ್ದರೆ {
                leadCC = 0;
            }
            ಆದರೆ (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                ಹಿಂತಿರುಗಿ prevSrc;
            }
            ಆದರೆ (!ಬಫರ್.append(reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(mapping) + 1, length, ನಿಜ, leadCC, trailCC, error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        }
        ಆದರೆ ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && ಬಫರ್.get_ಕೊನೆಯ_CC() <= 1)) {
            ಹಿಂತಿರುಗಿ src;
        }
    }
    ಹಿಂತಿರುಗಿ src;
}

ಸ್ಥಿರಾಂಕ char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t ಬಫರ್[4], int32_t &length) ಸ್ಥಿರಾಂಕ {
    uint16_t norm16;
    ಆದರೆ(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ char16_t *decomp = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=ಬಫರ್;
        length=0;
        U16_APPEND_UNSAFE(ಬಫರ್, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    ಆದರೆ (norm16 < minYesNo) {
        ಹಿಂತಿರುಗಿ decomp;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, ಬಫರ್);
        ಹಿಂತಿರುಗಿ ಬಫರ್;
    }
    // c decomposes, get everything from the variable-length extra data
    ಸ್ಥಿರಾಂಕ uint16_t *mapping=get_ದತ್ತಾಂಶ(norm16);
    length=*mapping&MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ;
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ಸ್ಥಿರಾಂಕ char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t ಬಫರ್[30], int32_t &length) ಸ್ಥಿರಾಂಕ {
    uint16_t norm16;
    ಆದರೆ(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, ಬಫರ್);
        length=2;
        ಹಿಂತಿರುಗಿ ಬಫರ್;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(ಬಫರ್, length, c);
        ಹಿಂತಿರುಗಿ ಬಫರ್;
    }
    // c decomposes, get everything from the variable-length extra data
    ಸ್ಥಿರಾಂಕ uint16_t *mapping=get_ದತ್ತಾಂಶ(norm16);
    uint16_t ಮೊದಲ_Unit=*mapping;
    int32_t m_ಉದ್ದಳತೆ=ಮೊದಲ_Unit&MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ;  // length of normal mapping
    ಆದರೆ(ಮೊದಲ_Unit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ಸ್ಥಿರಾಂಕ uint16_t *rawMapping=mapping-((ಮೊದಲ_Unit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        ಆದರೆ(rm0<=MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ) {
            length=rm0;
            ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(rawMapping) - rm0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Copy the normal mapping and replace its first two code units with rm0.
            ಬಫರ್[0] = static_cast<char16_t>(rm0);
            u_memcpy(ಬಫರ್ + 1, reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(mapping) + 1 + 2, m_ಉದ್ದಳತೆ - 2);
            length=m_ಉದ್ದಳತೆ-1;
            ಹಿಂತಿರುಗಿ ಬಫರ್;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        length=m_ಉದ್ದಳತೆ;
        ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(mapping) + 1;
    }
}

ಶೂನ್ಯ Normalizer2Impl::decomposeAndAppend(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                                         UBool doDecompose,
                                         UnicodeString &ಸುರಕ್ಷಿತ_ಮಧ್ಯ,
                                         Reordering_ಬಫರ್ &ಬಫರ್,
                                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಬಫರ್.copy_Reorderable_Suffix_ಗಮ್ಯ(ಸುರಕ್ಷಿತ_ಮಧ್ಯ);
    ಆದರೆ(doDecompose) {
        decompose(src, ಎಲ್ಲೆ, &ಬಫರ್, error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ;
    }
    // Just merge the strings at the boundary.
    ತಾರ್ಕಿಕ is_ಮೊದಲ = ನಿಜ;
    uint8_t ಮೊದಲ_CC = 0, prevCC = 0, cc;
    ಸ್ಥಿರಾಂಕ char16_t *p = src;
    ಆಗ (p != ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ char16_t *ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_ಮುಂದಿನ(normTrie, UCPTRIE_16, p, ಎಲ್ಲೆ, c, norm16);
        ಆದರೆ ((cc = getCC(norm16)) == 0) {
            p = ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (is_ಮೊದಲ) {
            ಮೊದಲ_CC = cc;
            is_ಮೊದಲ = ಸುಳ್ಳು;
        }
        prevCC = cc;
    }
    ಆದರೆ(ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ) {  // appendZeroCC() needs limit!=nullptr
        ಎಲ್ಲೆ=u_strchr(p, 0);
    }

    ಆದರೆ (ಬಫರ್.append(src, static_cast<int32_t>(p - src), ಸುಳ್ಳು, ಮೊದಲ_CC, prevCC, error_ಸಂಕೇತ)) {
        ಬಫರ್.appendZeroCC(p, ಎಲ್ಲೆ, error_ಸಂಕೇತ);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (norm16 < minNoNoCompNoMaybeCC) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (norm16 >= ಎಲ್ಲೆ_No_No) {
        ಹಿಂತಿರುಗಿ norm16 <= MIN_ಸಾಮಾನ್ಯ_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ಸ್ಥಿರಾಂಕ uint16_t *mapping=get_ದತ್ತಾಂಶ_For_Yes_Or_No(norm16);
    uint16_t ಮೊದಲ_Unit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_Unit&MAPPING_HAS_CCC_LCCC_ಶಬ್ದ)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (c < minDecompNoCP) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(norm16 <= minYesNo || isHangulLVT(norm16)) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (norm16 >= ಎಲ್ಲೆ_No_No) {
        ಆದರೆ (isMaybeYesOrNonZeroCC(norm16)) {
            ಹಿಂತಿರುಗಿ norm16 <= MIN_ಸಾಮಾನ್ಯ_MAYBE_YES || norm16 == JAMO_VT;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            ಹಿಂತಿರುಗಿ (norm16 & DELTA_TCCC_ಮುಖವಾಡ) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ಸ್ಥಿರಾಂಕ uint16_t *mapping=get_ದತ್ತಾಂಶ(norm16);
    uint16_t ಮೊದಲ_Unit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    ಆದರೆ(ಮೊದಲ_Unit>0x1ff) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // trailCC>1
    }
    ಆದರೆ(ಮೊದಲ_Unit<=0xff) {
        ಹಿಂತಿರುಗಿ ನಿಜ;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_Unit&MAPPING_HAS_CCC_LCCC_ಶಬ್ದ)==0 || (*(mapping-1)&0xff00)==0;
}

/*
 * Finds the recomposition result for
 * a forward-combining "lead" character,
 * specified with a pointer to its compositions list,
 * and a backward-combining "trail" character.
 *
 * If the lead and trail characters combine, then this function returns
 * the following "compositeAndFwd" value:
 * Bits 21..1  composite character
 * Bit      0  set if the composite is a forward-combining starter
 * otherwise it returns -1.
 *
 * The compositions list has (trail, compositeAndFwd) pair entries,
 * encoded as either pairs or triples of 16-bit units.
 * The last entry has the high bit of its first unit set.
 *
 * The list is sorted by ascending trail characters (there are no duplicates).
 * A linear search is used.
 *
 * See normalizer2impl.h for a more detailed description
 * of the compositions list format.
 */
int32_t Normalizer2Impl::combine(ಸ್ಥಿರಾಂಕ uint16_t *list, UChar32 trail) {
    uint16_t key1, ಮೊದಲ_Unit;
    ಆದರೆ(trail<COMP_1_TRAIL_ಎಲ್ಲೆ) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        ಆಗ(key1>(ಮೊದಲ_Unit=*list)) {
            list+=2+(ಮೊದಲ_Unit&COMP_1_TRIPLE);
        }
        ಆದರೆ(key1==(ಮೊದಲ_Unit&COMP_1_TRAIL_ಮುಖವಾಡ)) {
            ಆದರೆ(ಮೊದಲ_Unit&COMP_1_TRIPLE) {
                ಹಿಂತಿರುಗಿ (static_cast<int32_t>(list[1]) << 16) | list[2];
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ list[1];
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_ಎಲ್ಲೆ +
                        (((trail>>COMP_1_TRAIL_ಸರಿಸು))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_ಸರಿಸು);
        uint16_t ಎರಡನೇ_Unit;
        ಗಾಗಿ(;;) {
            ಆದರೆ(key1>(ಮೊದಲ_Unit=*list)) {
                list+=2+(ಮೊದಲ_Unit&COMP_1_TRIPLE);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(key1==(ಮೊದಲ_Unit&COMP_1_TRAIL_ಮುಖವಾಡ)) {
                ಆದರೆ(key2>(ಎರಡನೇ_Unit=list[1])) {
                    ಆದರೆ(ಮೊದಲ_Unit&COMP_1_ಕೊನೆಯ_TUPLE) {
                        ನಿಲ್ಲಿಸು;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        list+=3;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(key2==(ಎರಡನೇ_Unit&COMP_2_TRAIL_ಮುಖವಾಡ)) {
                    ಹಿಂತಿರುಗಿ (static_cast<int32_t>(ಎರಡನೇ_Unit & ~COMP_2_TRAIL_ಮುಖವಾಡ) << 16) | list[2];
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
ಶೂನ್ಯ Normalizer2Impl::ಕೂಡಿಸು_Composites(ಸ್ಥಿರಾಂಕ uint16_t *list, Unicode_ಗಣ &ಗಣ) ಸ್ಥಿರಾಂಕ {
    uint16_t ಮೊದಲ_Unit;
    int32_t ಸಂಯುಕ್ತ_And_Fwd;
    ಮಾಡು {
        ಮೊದಲ_Unit=*list;
        ಆದರೆ((ಮೊದಲ_Unit&COMP_1_TRIPLE)==0) {
            ಸಂಯುಕ್ತ_And_Fwd=list[1];
            list+=2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸಂಯುಕ್ತ_And_Fwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_ಮುಖವಾಡ) << 16) | list[2];
            list+=3;
        }
        UChar32 ಸಂಯುಕ್ತ=ಸಂಯುಕ್ತ_And_Fwd>>1;
        ಆದರೆ((ಸಂಯುಕ್ತ_And_Fwd&1)!=0) {
            ಕೂಡಿಸು_Composites(get_Compositions_List_For_ಸಂಯುಕ್ತ(getRawNorm16(ಸಂಯುಕ್ತ)), ಗಣ);
        }
        ಗಣ.ಕೂಡಿಸು(ಸಂಯುಕ್ತ);
    } ಆಗ((ಮೊದಲ_Unit&COMP_1_ಕೊನೆಯ_TUPLE)==0);
}

/*
 * Recomposes the buffer text starting at recomposeStartIndex
 * (which is in NFD - decomposed and canonically ordered),
 * and truncates the buffer contents.
 *
 * Note that recomposition never lengthens the text:
 * Any character consists of either one or two code units;
 * a composition may contain at most one more code unit than the original starter,
 * while the combining mark that is removed has at least one code unit.
 */
ಶೂನ್ಯ Normalizer2Impl::recompose(Reordering_ಬಫರ್ &ಬಫರ್, int32_t recompose_ಆರಂಭ_ಸೂಚ್ಯಂಕ,
                                UBool onlyContiguous) ಸ್ಥಿರಾಂಕ {
    char16_t *p=ಬಫರ್.get_ಆರಂಭ()+recompose_ಆರಂಭ_ಸೂಚ್ಯಂಕ;
    char16_t *ಎಲ್ಲೆ=ಬಫರ್.get_ಎಲ್ಲೆ();
    ಆದರೆ(p==ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ;
    }

    char16_t *starter, *pRemove, *q, *r;
    ಸ್ಥಿರಾಂಕ uint16_t *compositionsList;
    UChar32 c, ಸಂಯುಕ್ತ_And_Fwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=ಶೂನ್ಯಸೂಚಕ;  // used as indicator for whether we have a forward-combining starter
    starter=ಶೂನ್ಯಸೂಚಕ;
    starterIsSupplementary=ಸುಳ್ಳು;
    prevCC=0;

    ಗಾಗಿ(;;) {
        UCPTRIE_FAST_U16_ಮುಂದಿನ(normTrie, UCPTRIE_16, p, ಎಲ್ಲೆ, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        ಆದರೆ( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=ಶೂನ್ಯಸೂಚಕ &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            ಆದರೆ(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                ಆದರೆ(c<Hangul::JAMO_T_ತಳ) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_ತಳ);
                    ಆದರೆ(prev<Hangul::JAMO_L_ಎಣಿಕೆ) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_ತಳ +
                             (prev*Hangul::JAMO_V_ಎಣಿಕೆ+(c-Hangul::JAMO_V_ತಳ))*
                             Hangul::JAMO_T_ಎಣಿಕೆ);
                        char16_t t;
                        ಆದರೆ (p != ಎಲ್ಲೆ && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_ತಳ)) < Hangul::JAMO_T_ಎಣಿಕೆ) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        ಆಗ(r<ಎಲ್ಲೆ) {
                            *q++=*r++;
                        }
                        ಎಲ್ಲೆ=q;
                        p=pRemove;
                    }
                }
                /*
                 * No "else" for Jamo T:
                 * Since the input is in NFD, there are no Hangul LV syllables that
                 * a Jamo T could combine with.
                 * All Jamo Ts are combined above when handling Jamo Vs.
                 */
                ಆದರೆ(p==ಎಲ್ಲೆ) {
                    ನಿಲ್ಲಿಸು;
                }
                compositionsList=ಶೂನ್ಯಸೂಚಕ;
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಸಂಯುಕ್ತ_And_Fwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 ಸಂಯುಕ್ತ=ಸಂಯುಕ್ತ_And_Fwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_ಉದ್ದಳತೆ(c);  // pRemove & p: start & limit of the combining mark
                ಆದರೆ(starterIsSupplementary) {
                    ಆದರೆ(U_IS_SUPPLEMENTARY(ಸಂಯುಕ್ತ)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(ಸಂಯುಕ್ತ);
                        starter[1]=U16_TRAIL(ಸಂಯುಕ್ತ);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        *starter = static_cast<char16_t>(ಸಂಯುಕ್ತ);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=ಸುಳ್ಳು;
                        q=starter+1;
                        r=q+1;
                        ಆಗ(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_IS_SUPPLEMENTARY(ಸಂಯುಕ್ತ)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=ನಿಜ;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    ಆಗ(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(ಸಂಯುಕ್ತ);
                    *--starter=U16_LEAD(ಸಂಯುಕ್ತ);  // undo the temporary increment
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(ಸಂಯುಕ್ತ);
                }

                /* remove the combining mark by moving the following text over it */
                ಆದರೆ(pRemove<p) {
                    q=pRemove;
                    r=p;
                    ಆಗ(r<ಎಲ್ಲೆ) {
                        *q++=*r++;
                    }
                    ಎಲ್ಲೆ=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                ಆದರೆ(p==ಎಲ್ಲೆ) {
                    ನಿಲ್ಲಿಸು;
                }
                // Is the composite a starter that combines forward?
                ಆದರೆ(ಸಂಯುಕ್ತ_And_Fwd&1) {
                    compositionsList=
                        get_Compositions_List_For_ಸಂಯುಕ್ತ(getRawNorm16(ಸಂಯುಕ್ತ));
                } ಇಲ್ಲದಿದ್ದರೆ {
                    compositionsList=ಶೂನ್ಯಸೂಚಕ;
                }

                // We combined; continue with looking for compositions.
                ಮುಂದುವರಿ;
            }
        }

        // no combination this time
        prevCC=cc;
        ಆದರೆ(p==ಎಲ್ಲೆ) {
            ನಿಲ್ಲಿಸು;
        }

        // If c did not combine, then check if it is a starter.
        ಆದರೆ(cc==0) {
            // Found a new starter.
            ಆದರೆ((compositionsList=getCompositionsListForDecompYes(norm16))!=ಶೂನ್ಯಸೂಚಕ) {
                // It may combine with something, prepare for it.
                ಆದರೆ(U_IS_BMP(c)) {
                    starterIsSupplementary=ಸುಳ್ಳು;
                    starter=p-1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    starterIsSupplementary=ನಿಜ;
                    starter=p-2;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=ಶೂನ್ಯಸೂಚಕ;
        }
    }
    ಬಫರ್.ಗಣ_Reordering_ಎಲ್ಲೆ(ಎಲ್ಲೆ);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ಸ್ಥಿರಾಂಕ {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ಸ್ಥಿರಾಂಕ uint16_t *list;
    ಆದರೆ(isInert(norm16)) {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        ಆದರೆ(isJamoL(norm16)) {
            ಆದರೆ (b < Hangul::JAMO_V_ತಳ) {
                ಹಿಂತಿರುಗಿ U_SENTINEL;
            }
            b-=Hangul::JAMO_V_ತಳ;
            ಆದರೆ(b<Hangul::JAMO_V_ಎಣಿಕೆ) {
                ಹಿಂತಿರುಗಿ
                    (Hangul::HANGUL_ತಳ+
                     ((a-Hangul::JAMO_L_ತಳ)*Hangul::JAMO_V_ಎಣಿಕೆ+b)*
                     Hangul::JAMO_T_ಎಣಿಕೆ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ U_SENTINEL;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isHangulLV(norm16)) {
            ಆದರೆ (b <= Hangul::JAMO_T_ತಳ) {
               ಹಿಂತಿರುಗಿ U_SENTINEL;
            }
            b-=Hangul::JAMO_T_ತಳ;
            ಆದರೆ(b<Hangul::JAMO_T_ಎಣಿಕೆ) {  // not b==0!
                ಹಿಂತಿರುಗಿ a+b;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ U_SENTINEL;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // 'a' has a compositions list in extraData
            list=get_ದತ್ತಾಂಶ_For_Yes_Or_No(norm16);
            ಆದರೆ(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ);  // + mapping length
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(norm16<minMaybeNoCombinesFwd || MIN_ಸಾಮಾನ್ಯ_MAYBE_YES<=norm16) {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    } ಇಲ್ಲದಿದ್ದರೆ {
        list=get_ದತ್ತಾಂಶ_For_Maybe(norm16);
        ಆದರೆ(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ);  // + mapping length
        }
    }
    ಆದರೆ(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
#ಆದರೆ U_SIGNED_ಬಲ_ಸರಿಸು_IS_ARITHMETIC
    ಹಿಂತಿರುಗಿ combine(list, b)>>1;
#ಇಲ್ಲದಿದ್ದರೆ
    int32_t ಸಂಯುಕ್ತ_And_Fwd=combine(list, b);
    ಹಿಂತಿರುಗಿ ಸಂಯುಕ್ತ_And_Fwd>=0 ? ಸಂಯುಕ್ತ_And_Fwd>>1 : U_SENTINEL;
#ಆದರೆ_ಅಂತ್ಯ
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_ಬಫರ್ &ಬಫರ್,
                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ಆದರೆ(ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ) {
        src=copy_ಕೆಳಗಿನ_ಪೂರ್ವಪ್ರತ್ಯಯ_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &ಬಫರ್ : ಶೂನ್ಯಸೂಚಕ,
                                           error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಎಲ್ಲೆ=u_strchr(src, 0);
        ಆದರೆ (prevBoundary != src) {
            ಆದರೆ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಬಫರ್.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    ಗಾಗಿ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ಸ್ಥಿರಾಂಕ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        ಗಾಗಿ (;;) {
            ಆದರೆ (src == ಎಲ್ಲೆ) {
                ಆದರೆ (prevBoundary != ಎಲ್ಲೆ && doCompose) {
                    ಬಫರ್.appendZeroCC(prevBoundary, ಎಲ್ಲೆ, error_ಸಂಕೇತ);
                }
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
            ಆದರೆ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ಇಲ್ಲದಿದ್ದರೆ {
                prevSrc = src++;
                ಆದರೆ(!U16_IS_LEAD(c)) {
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    char16_t c2;
                    ಆದರೆ(src!=ಎಲ್ಲೆ && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ಆದರೆ(!isCompYesAndZeroCC(norm16)) {
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        ಆದರೆ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ಆದರೆ (!doCompose) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ಆದರೆ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ಆದರೆ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, ಎಲ್ಲೆ)) {
                    ಆದರೆ (prevBoundary != prevSrc && !ಬಫರ್.appendZeroCC(prevBoundary, prevSrc, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    ಆದರೆ(!ಬಫರ್.append(mapAlgorithmic(c, norm16), 0, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    prevBoundary = src;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ಆದರೆ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, ಎಲ್ಲೆ)) {
                    ಆದರೆ (prevBoundary != prevSrc && !ಬಫರ್.appendZeroCC(prevBoundary, prevSrc, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    ಸ್ಥಿರಾಂಕ char16_t *mapping = reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t *>(get_ದತ್ತಾಂಶ_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ;
                    ಆದರೆ(!ಬಫರ್.appendZeroCC(mapping, mapping + length, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    prevBoundary = src;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 >= min_No_No_ಖಾಲಿಗಳು) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ಆದರೆ (hasCompBoundaryBefore(src, ಎಲ್ಲೆ) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ಆದರೆ (prevBoundary != prevSrc && !ಬಫರ್.appendZeroCC(prevBoundary, prevSrc, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    prevBoundary = src;
                    ಮುಂದುವರಿ;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            ಆದರೆ(c<Hangul::JAMO_T_ತಳ) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_ತಳ);
                ಆದರೆ(l<Hangul::JAMO_L_ಎಣಿಕೆ) {
                    ಆದರೆ (!doCompose) {
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                    }
                    int32_t t;
                    ಆದರೆ (src != ಎಲ್ಲೆ &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_ತಳ)) &&
                            t < Hangul::JAMO_T_ಎಣಿಕೆ) {
                        // The next character is a Jamo T.
                        ++src;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hasCompBoundaryBefore(src, ಎಲ್ಲೆ)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        t = -1;
                    }
                    ಆದರೆ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_ತಳ +
                            (l*Hangul::JAMO_V_ಎಣಿಕೆ + (c-Hangul::JAMO_V_ತಳ)) *
                            Hangul::JAMO_T_ಎಣಿಕೆ + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        ಆದರೆ (prevBoundary != prevSrc && !ಬಫರ್.appendZeroCC(prevBoundary, prevSrc, error_ಸಂಕೇತ)) {
                            ನಿಲ್ಲಿಸು;
                        }
                        ಆದರೆ (!ಬಫರ್.appendBMP(static_cast<char16_t>(syllable), 0, error_ಸಂಕೇತ)) {
                            ನಿಲ್ಲಿಸು;
                        }
                        prevBoundary = src;
                        ಮುಂದುವರಿ;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ಆದರೆ (!doCompose) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_ತಳ;
                --prevSrc;  // Replace the Hangul LV as well.
                ಆದರೆ (prevBoundary != prevSrc && !ಬಫರ್.appendZeroCC(prevBoundary, prevSrc, error_ಸಂಕೇತ)) {
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (!ಬಫರ್.appendBMP(static_cast<char16_t>(syllable), 0, error_ಸಂಕೇತ)) {
                    ನಿಲ್ಲಿಸು;
                }
                prevBoundary = src;
                ಮುಂದುವರಿ;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_ಸಾಮಾನ್ಯ_Yes_Or_Maybe(norm16);  // cc!=0
            ಆದರೆ (onlyContiguous /* FCC */ && get_ಹಿಂದಿನ_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ಆದರೆ (!doCompose) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ಸ್ಥಿರಾಂಕ char16_t *ಮುಂದಿನ_Src;
                uint16_t n16;
                ಗಾಗಿ (;;) {
                    ಆದರೆ (src == ಎಲ್ಲೆ) {
                        ಆದರೆ (doCompose) {
                            ಬಫರ್.appendZeroCC(prevBoundary, ಎಲ್ಲೆ, error_ಸಂಕೇತ);
                        }
                        ಹಿಂತಿರುಗಿ ನಿಜ;
                    }
                    uint8_t prevCC = cc;
                    ಮುಂದಿನ_Src = src;
                    UCPTRIE_FAST_U16_ಮುಂದಿನ(normTrie, UCPTRIE_16, ಮುಂದಿನ_Src, ಎಲ್ಲೆ, c, n16);
                    ಆದರೆ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_ಸಾಮಾನ್ಯ_Yes_Or_Maybe(n16);
                        ಆದರೆ (prevCC > cc) {
                            ಆದರೆ (!doCompose) {
                                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                            }
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ನಿಲ್ಲಿಸು;
                    }
                    src = ಮುಂದಿನ_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ಆದರೆ (norm16HasCompBoundaryBefore(n16)) {
                    ಆದರೆ (isCompYesAndZeroCC(n16)) {
                        src = ಮುಂದಿನ_Src;
                    }
                    ಮುಂದುವರಿ;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ಆದರೆ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ಸ್ಥಿರಾಂಕ char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            ಆದರೆ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ಆದರೆ (doCompose && prevBoundary != prevSrc && !ಬಫರ್.appendZeroCC(prevBoundary, prevSrc, error_ಸಂಕೇತ)) {
            ನಿಲ್ಲಿಸು;
        }
        int32_t recompose_ಆರಂಭ_ಸೂಚ್ಯಂಕ=ಬಫರ್.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, ಸುಳ್ಳು /* !stopAtCompBoundary */, onlyContiguous,
                       ಬಫರ್, error_ಸಂಕೇತ);
        // Decompose until the next boundary.
        src = decomposeShort(src, ಎಲ್ಲೆ, ನಿಜ /* stopAtCompBoundary */, onlyContiguous,
                             ಬಫರ್, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        recompose(ಬಫರ್, recompose_ಆರಂಭ_ಸೂಚ್ಯಂಕ, onlyContiguous);
        ಆದರೆ(!doCompose) {
            ಆದರೆ(!ಬಫರ್.equals(prevSrc, src)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಬಫರ್.remove();
        }
        prevBoundary=src;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ಸ್ಥಿರಾಂಕ char16_t *
Normalizer2Impl::compose_ಶೀಘ್ರ_Check(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                                   UBool onlyContiguous,
                                   UNormalization_Check_ಫಲಿತಾಂಶ *pQCResult) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ಆದರೆ(ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ) {
        UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
        src=copy_ಕೆಳಗಿನ_ಪೂರ್ವಪ್ರತ್ಯಯ_From_Nul_Terminated(src, minNoMaybeCP, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
        ಎಲ್ಲೆ=u_strchr(src, 0);
        ಆದರೆ (prevBoundary != src) {
            ಆದರೆ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ಇಲ್ಲದಿದ್ದರೆ {
                prevBoundary = --src;
            }
        }
    }

    ಗಾಗಿ(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ಸ್ಥಿರಾಂಕ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        ಗಾಗಿ (;;) {
            ಆದರೆ(src==ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ src;
            }
            ಆದರೆ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ಇಲ್ಲದಿದ್ದರೆ {
                prevSrc = src++;
                ಆದರೆ(!U16_IS_LEAD(c)) {
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    char16_t c2;
                    ಆದರೆ(src!=ಎಲ್ಲೆ && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ಆದರೆ(!isCompYesAndZeroCC(norm16)) {
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        uint16_t prevNorm16 = INERT;
        ಆದರೆ (prevBoundary != prevSrc) {
            ಆದರೆ (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಸ್ಥಿರಾಂಕ char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                ಆದರೆ (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        ಆದರೆ (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            ಆದರೆ (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } ಇಲ್ಲದಿದ್ದರೆ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ಸ್ಥಿರಾಂಕ char16_t *ಮುಂದಿನ_Src;
                ಗಾಗಿ (;;) {
                    ಆದರೆ (norm16 < MIN_YES_YES_WITH_CC) {
                        ಆದರೆ (pQCResult != ಶೂನ್ಯಸೂಚಕ) {
                            *pQCResult = UNORM_MAYBE;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ಹಿಂತಿರುಗಿ prevBoundary;
                        }
                    }
                    ಆದರೆ (src == ಎಲ್ಲೆ) {
                        ಹಿಂತಿರುಗಿ src;
                    }
                    uint8_t prevCC = fcd16;
                    ಮುಂದಿನ_Src = src;
                    UCPTRIE_FAST_U16_ಮುಂದಿನ(normTrie, UCPTRIE_16, ಮುಂದಿನ_Src, ಎಲ್ಲೆ, c, norm16);
                    ಆದರೆ (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        ಆದರೆ (!(prevCC <= cc || cc == 0)) {
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ನಿಲ್ಲಿಸು;
                    }
                    src = ಮುಂದಿನ_Src;
                }
                // src is after the last in-order combining mark.
                ಆದರೆ (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = ಮುಂದಿನ_Src;
                    ಮುಂದುವರಿ;
                }
            }
        }
        ಆದರೆ(pQCResult!=ಶೂನ್ಯಸೂಚಕ) {
            *pQCResult=UNORM_NO;
        }
        ಹಿಂತಿರುಗಿ prevBoundary;
    }
}

ಶೂನ್ಯ Normalizer2Impl::composeAndAppend(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &ಸುರಕ್ಷಿತ_ಮಧ್ಯ,
                                       Reordering_ಬಫರ್ &ಬಫರ್,
                                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(!ಬಫರ್.is_ಖಾಲಿಗಳು()) {
        ಸ್ಥಿರಾಂಕ char16_t *ಮೊದಲ_Starter_ಒಳ_Src=ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_Comp_Boundary(src, ಎಲ್ಲೆ, onlyContiguous);
        ಆದರೆ(src!=ಮೊದಲ_Starter_ಒಳ_Src) {
            ಸ್ಥಿರಾಂಕ char16_t *ಕೊನೆಯ_Starter_ಒಳ_Dest=ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_Comp_Boundary(ಬಫರ್.get_ಆರಂಭ(),
                                                                    ಬಫರ್.get_ಎಲ್ಲೆ(), onlyContiguous);
            int32_t dest_Suffix_ಉದ್ದಳತೆ = static_cast<int32_t>(ಬಫರ್.get_ಎಲ್ಲೆ() - ಕೊನೆಯ_Starter_ಒಳ_Dest);
            UnicodeString ಮಧ್ಯ(ಕೊನೆಯ_Starter_ಒಳ_Dest, dest_Suffix_ಉದ್ದಳತೆ);
            ಬಫರ್.removeSuffix(dest_Suffix_ಉದ್ದಳತೆ);
            ಸುರಕ್ಷಿತ_ಮಧ್ಯ=ಮಧ್ಯ;
            ಮಧ್ಯ.append(src, static_cast<int32_t>(ಮೊದಲ_Starter_ಒಳ_Src - src));
            ಸ್ಥಿರಾಂಕ char16_t *ಮಧ್ಯ_ಆರಂಭ=ಮಧ್ಯ.get_ಬಫರ್();
            compose(ಮಧ್ಯ_ಆರಂಭ, ಮಧ್ಯ_ಆರಂಭ+ಮಧ್ಯ.length(), onlyContiguous,
                    ನಿಜ, ಬಫರ್, error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
            src=ಮೊದಲ_Starter_ಒಳ_Src;
        }
    }
    ಆದರೆ(doCompose) {
        compose(src, ಎಲ್ಲೆ, onlyContiguous, ನಿಜ, ಬಫರ್, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ) {  // appendZeroCC() needs limit!=nullptr
            ಎಲ್ಲೆ=u_strchr(src, 0);
        }
        ಬಫರ್.appendZeroCC(src, ಎಲ್ಲೆ, error_ಸಂಕೇತ);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ಸ್ಥಿರಾಂಕ uint8_t *src, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ,
                             ByteSink *sink, Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(ಎಲ್ಲೆ != ಶೂನ್ಯಸೂಚಕ);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ಸ್ಥಿರಾಂಕ uint8_t *prevBoundary = src;

    ಗಾಗಿ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ಸ್ಥಿರಾಂಕ uint8_t *prevSrc;
        uint16_t norm16 = 0;
        ಗಾಗಿ (;;) {
            ಆದರೆ (src == ಎಲ್ಲೆ) {
                ಆದರೆ (prevBoundary != ಎಲ್ಲೆ && sink != ಶೂನ್ಯಸೂಚಕ) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, ಎಲ್ಲೆ,
                                                  *sink, options, edits, error_ಸಂಕೇತ);
                }
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
            ಆದರೆ (*src < minNoMaybeLead) {
                ++src;
            } ಇಲ್ಲದಿದ್ದರೆ {
                prevSrc = src;
                UCPTRIE_FAST_U8_ಮುಂದಿನ(normTrie, UCPTRIE_16, src, ಎಲ್ಲೆ, norm16);
                ಆದರೆ (!isCompYesAndZeroCC(norm16)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        ಆದರೆ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ಆದರೆ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ಆದರೆ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, ಎಲ್ಲೆ)) {
                    ಆದರೆ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    append_ಸಂಕೇತ_ಬಿಂದು_Delta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ಆದರೆ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, ಎಲ್ಲೆ)) {
                    ಆದರೆ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    ಸ್ಥಿರಾಂಕ uint16_t *mapping = get_ದತ್ತಾಂಶ_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ;
                    ಆದರೆ (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(mapping), length,
                                                    *sink, edits, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    prevBoundary = src;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 >= min_No_No_ಖಾಲಿಗಳು) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ಆದರೆ (hasCompBoundaryBefore(src, ಎಲ್ಲೆ) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ಆದರೆ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_ಸಂಕೇತ)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                        edits->ಕೂಡಿಸು_Replace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    ಮುಂದುವರಿ;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = ಹಿಂದಿನ_Hangul_Or_Jamo(prevBoundary, prevSrc);
            ಆದರೆ (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_ತಳ;
                ಆದರೆ (static_cast<uint32_t>(l) < Hangul::JAMO_L_ಎಣಿಕೆ) {
                    ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) {
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                    }
                    int32_t t = get_Jamo_TMinus_ತಳ(src, ಎಲ್ಲೆ);
                    ಆದರೆ (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hasCompBoundaryBefore(src, ಎಲ್ಲೆ)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    ಆದರೆ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_ತಳ +
                            (l*Hangul::JAMO_V_ಎಣಿಕೆ + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_ಎಣಿಕೆ + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        ಆದರೆ (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, error_ಸಂಕೇತ)) {
                            ನಿಲ್ಲಿಸು;
                        }
                        ByteSinkUtil::append_ಸಂಕೇತ_ಬಿಂದು(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        ಮುಂದುವರಿ;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                UChar32 syllable = prev + get_Jamo_TMinus_ತಳ(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                ಆದರೆ (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, error_ಸಂಕೇತ)) {
                    ನಿಲ್ಲಿಸು;
                }
                ByteSinkUtil::append_ಸಂಕೇತ_ಬಿಂದು(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                ಮುಂದುವರಿ;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_ಸಾಮಾನ್ಯ_Yes_Or_Maybe(norm16);  // cc!=0
            ಆದರೆ (onlyContiguous /* FCC */ && get_ಹಿಂದಿನ_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ಸ್ಥಿರಾಂಕ uint8_t *ಮುಂದಿನ_Src;
                uint16_t n16;
                ಗಾಗಿ (;;) {
                    ಆದರೆ (src == ಎಲ್ಲೆ) {
                        ಆದರೆ (sink != ಶೂನ್ಯಸೂಚಕ) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, ಎಲ್ಲೆ,
                                                          *sink, options, edits, error_ಸಂಕೇತ);
                        }
                        ಹಿಂತಿರುಗಿ ನಿಜ;
                    }
                    uint8_t prevCC = cc;
                    ಮುಂದಿನ_Src = src;
                    UCPTRIE_FAST_U8_ಮುಂದಿನ(normTrie, UCPTRIE_16, ಮುಂದಿನ_Src, ಎಲ್ಲೆ, n16);
                    ಆದರೆ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_ಸಾಮಾನ್ಯ_Yes_Or_Maybe(n16);
                        ಆದರೆ (prevCC > cc) {
                            ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) {
                                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                            }
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ನಿಲ್ಲಿಸು;
                    }
                    src = ಮುಂದಿನ_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ಆದರೆ (norm16HasCompBoundaryBefore(n16)) {
                    ಆದರೆ (isCompYesAndZeroCC(n16)) {
                        src = ಮುಂದಿನ_Src;
                    }
                    ಮುಂದುವರಿ;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ಆದರೆ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ಸ್ಥಿರಾಂಕ uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            ಆದರೆ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        Reordering_ಬಫರ್ ಬಫರ್(*ಇದು, s16, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ನಿಲ್ಲಿಸು;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_ಎಲ್ಲೆ, onlyContiguous,
                       ಬಫರ್, error_ಸಂಕೇತ);
        // Decompose until the next boundary.
        src = decomposeShort(src, ಎಲ್ಲೆ, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             ಬಫರ್, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        recompose(ಬಫರ್, 0, onlyContiguous);
        ಆದರೆ (!ಬಫರ್.equals(prevSrc, src)) {
            ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಆದರೆ (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ (!ByteSinkUtil::appendChange(prevSrc, src, ಬಫರ್.get_ಆರಂಭ(), ಬಫರ್.length(),
                                            *sink, edits, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
            prevBoundary = src;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (src == ಎಲ್ಲೆ || *src < minCompNoMaybeCP) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_ಮುಂದಿನ(normTrie, UCPTRIE_16, src, ಎಲ್ಲೆ, c, norm16);
    ಹಿಂತಿರುಗಿ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ಸ್ಥಿರಾಂಕ uint8_t *src, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (src == ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_ಮುಂದಿನ(normTrie, UCPTRIE_16, src, ಎಲ್ಲೆ, norm16);
    ಹಿಂತಿರುಗಿ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ char16_t *p,
                                            UBool onlyContiguous) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಆರಂಭ == p) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ಆರಂಭ, p, c, norm16);
    ಹಿಂತಿರುಗಿ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ಸ್ಥಿರಾಂಕ uint8_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *p,
                                            UBool onlyContiguous) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಆರಂಭ == p) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, ಆರಂಭ, p, norm16);
    ಹಿಂತಿರುಗಿ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ಸ್ಥಿರಾಂಕ char16_t *Normalizer2Impl::ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_Comp_Boundary(ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ char16_t *p,
                                                       UBool onlyContiguous) ಸ್ಥಿರಾಂಕ {
    ಆಗ (p != ಆರಂಭ) {
        ಸ್ಥಿರಾಂಕ char16_t *ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ಆರಂಭ, p, c, norm16);
        ಆದರೆ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ಹಿಂತಿರುಗಿ ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ;
        }
        ಆದರೆ (hasCompBoundaryBefore(c, norm16)) {
            ಹಿಂತಿರುಗಿ p;
        }
    }
    ಹಿಂತಿರುಗಿ p;
}

ಸ್ಥಿರಾಂಕ char16_t *Normalizer2Impl::ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_Comp_Boundary(ಸ್ಥಿರಾಂಕ char16_t *p, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                                                   UBool onlyContiguous) ಸ್ಥಿರಾಂಕ {
    ಆಗ (p != ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ char16_t *ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_ಮುಂದಿನ(normTrie, UCPTRIE_16, p, ಎಲ್ಲೆ, c, norm16);
        ಆದರೆ (hasCompBoundaryBefore(c, norm16)) {
            ಹಿಂತಿರುಗಿ ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
        }
        ಆದರೆ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ಹಿಂತಿರುಗಿ p;
        }
    }
    ಹಿಂತಿರುಗಿ p;
}

uint8_t Normalizer2Impl::get_ಹಿಂದಿನ_Trail_CC(ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ char16_t *p) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಆರಂಭ == p) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t i = static_cast<int32_t>(p - ಆರಂಭ);
    UChar32 c;
    U16_PREV(ಆರಂಭ, 0, i, c);
    ಹಿಂತಿರುಗಿ static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::get_ಹಿಂದಿನ_Trail_CC(ಸ್ಥಿರಾಂಕ uint8_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *p) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಆರಂಭ == p) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t i = static_cast<int32_t>(p - ಆರಂಭ);
    UChar32 c;
    U8_PREV(ಆರಂಭ, 0, i, c);
    ಹಿಂತಿರುಗಿ static_cast<uint8_t>(getFCD16(c));
}

// Note: normalizer2impl.cpp r30982 (2011-nov-27)
// still had getFCDTrie() which built and cached an FCD trie.
// That provided faster access to FCD data than getFCD16FromNormData()
// but required synchronization and consumed some 10kB of heap memory
// in any process that uses FCD (e.g., via collation).
// minDecompNoCP etc. and smallFCD[] are intended to help with any loss of performance,
// at least for ASCII & CJK.

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#ಆದರೆ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ಸೂಚನೆ optimize( "", off )
#ಆದರೆ_ಅಂತ್ಯ
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_ದತ್ತಾಂಶ(UChar32 c) ಸ್ಥಿರಾಂಕ {
    uint16_t norm16=getNorm16(c);
    ಆದರೆ (norm16 >= ಎಲ್ಲೆ_No_No) {
        ಆದರೆ(norm16>=MIN_ಸಾಮಾನ್ಯ_MAYBE_YES) {
            // combining mark
            norm16=get_CCFrom_ಸಾಮಾನ್ಯ_Yes_Or_Maybe(norm16);
            ಹಿಂತಿರುಗಿ norm16|(norm16<<8);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(norm16>=minMaybeYes) {
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_ಮುಖವಾಡ;
            ಆದರೆ (deltaTrailCC <= DELTA_TCCC_1) {
                ಹಿಂತಿರುಗಿ deltaTrailCC >> ಸ್ಥಾನಾಂತರ_ಸರಿಸು;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ಆದರೆ(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        ಹಿಂತಿರುಗಿ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ಸ್ಥಿರಾಂಕ uint16_t *mapping=get_ದತ್ತಾಂಶ(norm16);
    uint16_t ಮೊದಲ_Unit=*mapping;
    norm16=ಮೊದಲ_Unit>>8;  // tccc
    ಆದರೆ(ಮೊದಲ_Unit&MAPPING_HAS_CCC_LCCC_ಶಬ್ದ) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    ಹಿಂತಿರುಗಿ norm16;
}
#ಆದರೆ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ಸೂಚನೆ optimize( "", on )
#ಆದರೆ_ಅಂತ್ಯ

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(norm16 >= minMaybeNo);
    ಆದರೆ (norm16 >= MIN_ಸಾಮಾನ್ಯ_MAYBE_YES) {
        // combining mark
        norm16 = get_CCFrom_ಸಾಮಾನ್ಯ_Yes_Or_Maybe(norm16);
        ಹಿಂತಿರುಗಿ norm16 | (norm16<<8);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (norm16 >= minMaybeYes) {
        ಹಿಂತಿರುಗಿ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ಸ್ಥಿರಾಂಕ uint16_t *mapping = get_ದತ್ತಾಂಶ_For_Maybe(norm16);
    uint16_t ಮೊದಲ_Unit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((ಮೊದಲ_Unit & MAPPING_HAS_CCC_LCCC_ಶಬ್ದ) == 0 || (*(mapping - 1) & 0xff00) == 0);
    ಹಿಂತಿರುಗಿ ಮೊದಲ_Unit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ಸ್ಥಿರಾಂಕ char16_t *
Normalizer2Impl::makeFCD(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                         Reordering_ಬಫರ್ *ಬಫರ್,
                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ಸ್ಥಿರಾಂಕ char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    ಆದರೆ(ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ) {
        src=copy_ಕೆಳಗಿನ_ಪೂರ್ವಪ್ರತ್ಯಯ_From_Nul_Terminated(src, minLcccCP, ಬಫರ್, error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ src;
        }
        ಆದರೆ(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            ಆದರೆ(prevFCD16>1) {
                --prevBoundary;
            }
        }
        ಎಲ್ಲೆ=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    ಸ್ಥಿರಾಂಕ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    ಗಾಗಿ(;;) {
        // count code units with lccc==0
        ಗಾಗಿ(prevSrc=src; src!=ಎಲ್ಲೆ;) {
            ಆದರೆ((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ(U16_IS_LEAD(c)) {
                    char16_t c2;
                    ಆದರೆ((src+1)!=ಎಲ್ಲೆ && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                ಆದರೆ((fcd16=get_FCD16_From_Norm_ದತ್ತಾಂಶ(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_ಉದ್ದಳತೆ(c);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        // copy these code units all at once
        ಆದರೆ(src!=prevSrc) {
            ಆದರೆ(ಬಫರ್!=ಶೂನ್ಯಸೂಚಕ && !ಬಫರ್->appendZeroCC(prevSrc, src, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(src==ಎಲ್ಲೆ) {
                ನಿಲ್ಲಿಸು;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            ಆದರೆ(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                ಆದರೆ(prev<minDecompNoCP) {
                    prevFCD16=0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    prevFCD16=get_FCD16_From_Norm_ದತ್ತಾಂಶ(prev);
                    ಆದರೆ(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಸ್ಥಿರಾಂಕ char16_t *p=src-1;
                ಆದರೆ(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=get_FCD16_From_Norm_ದತ್ತಾಂಶ(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                ಆದರೆ(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(src==ಎಲ್ಲೆ) {
            ನಿಲ್ಲಿಸು;
        }

        src+=U16_ಉದ್ದಳತೆ(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        ಆದರೆ((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            ಆದರೆ((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            ಆದರೆ(ಬಫರ್!=ಶೂನ್ಯಸೂಚಕ && !ಬಫರ್->appendZeroCC(c, error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
            prevFCD16=fcd16;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಬಫರ್==ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ prevBoundary;  // quick check "no"
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            ಬಫರ್->removeSuffix(static_cast<int32_t>(prevSrc - prevBoundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_FCDBoundary(src, ಎಲ್ಲೆ);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(prevBoundary, src, ಸುಳ್ಳು, ಸುಳ್ಳು, *ಬಫರ್, error_ಸಂಕೇತ);
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
                ನಿಲ್ಲಿಸು;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    ಹಿಂತಿರುಗಿ src;
}

ಶೂನ್ಯ Normalizer2Impl::makeFCDAndAppend(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
                                       UBool doMakeFCD,
                                       UnicodeString &ಸುರಕ್ಷಿತ_ಮಧ್ಯ,
                                       Reordering_ಬಫರ್ &ಬಫರ್,
                                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(!ಬಫರ್.is_ಖಾಲಿಗಳು()) {
        ಸ್ಥಿರಾಂಕ char16_t *ಮೊದಲ_Boundary_ಒಳ_Src=ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_FCDBoundary(src, ಎಲ್ಲೆ);
        ಆದರೆ(src!=ಮೊದಲ_Boundary_ಒಳ_Src) {
            ಸ್ಥಿರಾಂಕ char16_t *ಕೊನೆಯ_Boundary_ಒಳ_Dest=ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_FCDBoundary(ಬಫರ್.get_ಆರಂಭ(),
                                                                    ಬಫರ್.get_ಎಲ್ಲೆ());
            int32_t dest_Suffix_ಉದ್ದಳತೆ = static_cast<int32_t>(ಬಫರ್.get_ಎಲ್ಲೆ() - ಕೊನೆಯ_Boundary_ಒಳ_Dest);
            UnicodeString ಮಧ್ಯ(ಕೊನೆಯ_Boundary_ಒಳ_Dest, dest_Suffix_ಉದ್ದಳತೆ);
            ಬಫರ್.removeSuffix(dest_Suffix_ಉದ್ದಳತೆ);
            ಸುರಕ್ಷಿತ_ಮಧ್ಯ=ಮಧ್ಯ;
            ಮಧ್ಯ.append(src, static_cast<int32_t>(ಮೊದಲ_Boundary_ಒಳ_Src - src));
            ಸ್ಥಿರಾಂಕ char16_t *ಮಧ್ಯ_ಆರಂಭ=ಮಧ್ಯ.get_ಬಫರ್();
            makeFCD(ಮಧ್ಯ_ಆರಂಭ, ಮಧ್ಯ_ಆರಂಭ+ಮಧ್ಯ.length(), &ಬಫರ್, error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
            src=ಮೊದಲ_Boundary_ಒಳ_Src;
        }
    }
    ಆದರೆ(doMakeFCD) {
        makeFCD(src, ಎಲ್ಲೆ, &ಬಫರ್, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ) {  // appendZeroCC() needs limit!=nullptr
            ಎಲ್ಲೆ=u_strchr(src, 0);
        }
        ಬಫರ್.appendZeroCC(src, ಎಲ್ಲೆ, error_ಸಂಕೇತ);
    }
}

ಸ್ಥಿರಾಂಕ char16_t *Normalizer2Impl::ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_FCDBoundary(ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ char16_t *p) ಸ್ಥಿರಾಂಕ {
    ಆಗ(ಆರಂಭ<p) {
        ಸ್ಥಿರಾಂಕ char16_t *ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ಆರಂಭ, p, c, norm16);
        ಆದರೆ (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            ಹಿಂತಿರುಗಿ ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ;
        }
        ಆದರೆ (norm16HasDecompBoundaryBefore(norm16)) {
            ಹಿಂತಿರುಗಿ p;
        }
    }
    ಹಿಂತಿರುಗಿ p;
}

ಸ್ಥಿರಾಂಕ char16_t *Normalizer2Impl::ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_FCDBoundary(ಸ್ಥಿರಾಂಕ char16_t *p, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ) ಸ್ಥಿರಾಂಕ {
    ಆಗ(p<ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ char16_t *ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_ಮುಂದಿನ(normTrie, UCPTRIE_16, p, ಎಲ್ಲೆ, c, norm16);
        ಆದರೆ (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            ಹಿಂತಿರುಗಿ ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
        }
        ಆದರೆ (norm16HasDecompBoundaryAfter(norm16)) {
            ಹಿಂತಿರುಗಿ p;
        }
    }
    ಹಿಂತಿರುಗಿ p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_ದತ್ತಾಂಶ::Canon_Iter_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) :
        mutableTrie(umutablecptrie_open(0, 0, &error_ಸಂಕೇತ)), trie(ಶೂನ್ಯಸೂಚಕ),
        canon_ಆರಂಭ_Sets(uprv_deleteUObject, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ) {}

Canon_Iter_ದತ್ತಾಂಶ::~Canon_Iter_ದತ್ತಾಂಶ() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

ಶೂನ್ಯ Canon_Iter_ದತ್ತಾಂಶ::ಕೂಡಿಸು_ಗಮ್ಯ_ಆರಂಭ_ಗಣ(UChar32 origin, UChar32 decompLead, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    uint32_t canon_ಮೌಲ್ಯ = umutablecptrie_get(mutableTrie, decompLead);
    ಆದರೆ((canon_ಮೌಲ್ಯ&(CANON_HAS_ಗಣ|CANON_ಮೌಲ್ಯ_ಮುಖವಾಡ))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_ಗಣ(mutableTrie, decompLead, canon_ಮೌಲ್ಯ|origin, &error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // origin is not the first character, or it is U+0000.
        Unicode_ಗಣ *ಗಣ;
        ಆದರೆ((canon_ಮೌಲ್ಯ&CANON_HAS_ಗಣ)==0) {
            Local_ಸೂಚಕ<Unicode_ಗಣ> lp_ಗಣ(ಹೊಸ Unicode_ಗಣ, error_ಸಂಕೇತ);
            ಗಣ=lp_ಗಣ.getAlias();
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
            UChar32 ಮೊದಲ_Origin = static_cast<UChar32>(canon_ಮೌಲ್ಯ & CANON_ಮೌಲ್ಯ_ಮುಖವಾಡ);
            canon_ಮೌಲ್ಯ = (canon_ಮೌಲ್ಯ & ~CANON_ಮೌಲ್ಯ_ಮುಖವಾಡ) | CANON_HAS_ಗಣ | static_cast<uint32_t>(canon_ಆರಂಭ_Sets.size());
            umutablecptrie_ಗಣ(mutableTrie, decompLead, canon_ಮೌಲ್ಯ, &error_ಸಂಕೇತ);
            canon_ಆರಂಭ_Sets.adoptElement(lp_ಗಣ.orphan(), error_ಸಂಕೇತ);
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
            ಆದರೆ(ಮೊದಲ_Origin!=0) {
                ಗಣ->ಕೂಡಿಸು(ಮೊದಲ_Origin);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಣ = static_cast<Unicode_ಗಣ*>(canon_ಆರಂಭ_Sets[static_cast<int32_t>(canon_ಮೌಲ್ಯ & CANON_ಮೌಲ್ಯ_ಮುಖವಾಡ)]);
        }
        ಗಣ->ಕೂಡಿಸು(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
ವರ್ಗ Init_Canon_Iter_ದತ್ತಾಂಶ {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಶೂನ್ಯ doInit(Normalizer2Impl *impl, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
init_Canon_Iter_ದತ್ತಾಂಶ(Normalizer2Impl *impl, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    Init_Canon_Iter_ದತ್ತಾಂಶ::doInit(impl, error_ಸಂಕೇತ);
}

U_CDECL_ಕೊನೆ

ಶೂನ್ಯ Init_Canon_Iter_ದತ್ತಾಂಶ::doInit(Normalizer2Impl *impl, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    U_ASSERT(impl->f_Canon_Iter_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ);
    impl->f_Canon_Iter_ದತ್ತಾಂಶ = ಹೊಸ Canon_Iter_ದತ್ತಾಂಶ(error_ಸಂಕೇತ);
    ಆದರೆ (impl->f_Canon_Iter_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        UChar32 ಆರಂಭ = 0, ಕೊನೆ;
        uint32_t value;
        ಆಗ ((ಕೊನೆ = ucptrie_getRange(impl->normTrie, ಆರಂಭ,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            ಆದರೆ (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_ದತ್ತಾಂಶ_From_Norm16(ಆರಂಭ, ಕೊನೆ, value, *impl->f_Canon_Iter_ದತ್ತಾಂಶ, error_ಸಂಕೇತ);
            }
            ಆರಂಭ = ಕೊನೆ + 1;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
        umutablecptrie_ಗಣ_Name(impl->f_Canon_Iter_ದತ್ತಾಂಶ->mutableTrie, "CanonIterData");
#ಆದರೆ_ಅಂತ್ಯ
        impl->f_Canon_Iter_ದತ್ತಾಂಶ->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_ದತ್ತಾಂಶ->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32, &error_ಸಂಕೇತ);
        umutablecptrie_close(impl->f_Canon_Iter_ದತ್ತಾಂಶ->mutableTrie);
        impl->f_Canon_Iter_ದತ್ತಾಂಶ->mutableTrie = ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಅಳಿಸು impl->f_Canon_Iter_ದತ್ತಾಂಶ;
        impl->f_Canon_Iter_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    }
}

ಶೂನ್ಯ Normalizer2Impl::make_Canon_Iter_ದತ್ತಾಂಶ_From_Norm16(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ, ಸ್ಥಿರಾಂಕ uint16_t norm16,
                                                  Canon_Iter_ದತ್ತಾಂಶ &new_ದತ್ತಾಂಶ,
                                                  UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ(UChar32 c=ಆರಂಭ; c<=ಕೊನೆ; ++c) {
        uint32_t old_ಮೌಲ್ಯ = umutablecptrie_get(new_ದತ್ತಾಂಶ.mutableTrie, c);
        uint32_t new_ಮೌಲ್ಯ=old_ಮೌಲ್ಯ;
        ಆದರೆ(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_ಮೌಲ್ಯ|=CANON_NOT_SEGMENT_STARTER;
            ಆದರೆ(norm16<MIN_ಸಾಮಾನ್ಯ_MAYBE_YES) {
                new_ಮೌಲ್ಯ|=CANON_HAS_COMPOSITIONS;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(norm16<minYesNo) {
            new_ಮೌಲ್ಯ|=CANON_HAS_COMPOSITIONS;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            ಆದರೆ (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            ಆದರೆ (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ಸ್ಥಿರಾಂಕ uint16_t *mapping=get_ದತ್ತಾಂಶ_For_Yes_Or_No(norm16_2);
                uint16_t ಮೊದಲ_Unit=*mapping;
                int32_t length=ಮೊದಲ_Unit&MAPPING_ಉದ್ದಳತೆ_ಮುಖವಾಡ;
                ಆದರೆ((ಮೊದಲ_Unit&MAPPING_HAS_CCC_LCCC_ಶಬ್ದ)!=0) {
                    ಆದರೆ(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_ಮೌಲ್ಯ|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                ಆದರೆ(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_ಮುಂದಿನ_UNSAFE(mapping, i, c2);
                    new_ದತ್ತಾಂಶ.ಕೂಡಿಸು_ಗಮ್ಯ_ಆರಂಭ_ಗಣ(c, c2, error_ಸಂಕೇತ);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    ಆದರೆ(norm16_2>=minNoNo) {
                        ಆಗ(i<length) {
                            U16_ಮುಂದಿನ_UNSAFE(mapping, i, c2);
                            uint32_t c2_ಮೌಲ್ಯ = umutablecptrie_get(new_ದತ್ತಾಂಶ.mutableTrie, c2);
                            ಆದರೆ((c2_ಮೌಲ್ಯ&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_ಗಣ(new_ದತ್ತಾಂಶ.mutableTrie, c2,
                                                   c2_ಮೌಲ್ಯ|CANON_NOT_SEGMENT_STARTER, &error_ಸಂಕೇತ);
                            }
                        }
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // c decomposed to c2 algorithmically; c has cc==0
                new_ದತ್ತಾಂಶ.ಕೂಡಿಸು_ಗಮ್ಯ_ಆರಂಭ_ಗಣ(c, c2, error_ಸಂಕೇತ);
            }
        }
        ಆದರೆ(new_ಮೌಲ್ಯ!=old_ಮೌಲ್ಯ) {
            umutablecptrie_ಗಣ(new_ದತ್ತಾಂಶ.mutableTrie, c, new_ಮೌಲ್ಯ, &error_ಸಂಕೇತ);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(ಇದು);
    umtx_initOnce(me->f_Canon_Iter_ದತ್ತಾಂಶ_Init_Once, &init_Canon_Iter_ದತ್ತಾಂಶ, me, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ);
}

int32_t Normalizer2Impl::get_Canon_ಮೌಲ್ಯ(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ucptrie_get(f_Canon_Iter_ದತ್ತಾಂಶ->trie, c));
}

ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &Normalizer2Impl::get_Canon_ಆರಂಭ_ಗಣ(int32_t n) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ *static_cast<ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*>(f_Canon_Iter_ದತ್ತಾಂಶ->canon_ಆರಂಭ_Sets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ get_Canon_ಮೌಲ್ಯ(c)>=0;
}

UBool Normalizer2Impl::get_Canon_ಆರಂಭ_ಗಣ(UChar32 c, Unicode_ಗಣ &ಗಣ) ಸ್ಥಿರಾಂಕ {
    int32_t canon_ಮೌಲ್ಯ=get_Canon_ಮೌಲ್ಯ(c)&~CANON_NOT_SEGMENT_STARTER;
    ಆದರೆ(canon_ಮೌಲ್ಯ==0) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಗಣ.clear();
    int32_t value=canon_ಮೌಲ್ಯ&CANON_ಮೌಲ್ಯ_ಮುಖವಾಡ;
    ಆದರೆ((canon_ಮೌಲ್ಯ&CANON_HAS_ಗಣ)!=0) {
        ಗಣ.ಕೂಡಿಸು_All(get_Canon_ಆರಂಭ_ಗಣ(value));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value!=0) {
        ಗಣ.ಕೂಡಿಸು(value);
    }
    ಆದರೆ((canon_ಮೌಲ್ಯ&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        ಆದರೆ(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_ತಳ + (c - Hangul::JAMO_L_ತಳ) * Hangul::JAMO_VT_ಎಣಿಕೆ);
            ಗಣ.ಕೂಡಿಸು(syllable, syllable+Hangul::JAMO_VT_ಎಣಿಕೆ-1);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಕೂಡಿಸು_Composites(getCompositionsList(norm16), ಗಣ);
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_NAMESPACE_ಕೊನೆ

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_ಬೈಟುಗಳು;
    uint8_t *ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    ಸ್ಥಿರಾಂಕ int32_t *ಒಳ_Indexes;
    int32_t indexes[Normalizer2Impl::IX_ಒಟ್ಟು_ಗಾತ್ರ+1];

    int32_t i, ಸ್ಥಾನಾಂತರ, ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ, size;

    /* udata_swapDataHeader checks the arguments */
    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check data format and format version */
    pInfo=(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    uint8_t formatVersion0=pInfo->format_ಆವೃತ್ತಿ[0];
    ಆದರೆ(!(
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x72 &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x6d &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                         pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3],
                         pInfo->format_ಆವೃತ್ತಿ[0]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_ಬೈಟುಗಳು=(ಸ್ಥಿರಾಂಕ uint8_t *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಹೊರಹೋಗು_ಬೈಟುಗಳು=(ಹೊರಹೋಗು_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) ? ಶೂನ್ಯಸೂಚಕ : (uint8_t *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಒಳ_Indexes=(ಸ್ಥಿರಾಂಕ int32_t *)ಒಳ_ಬೈಟುಗಳು;
    int32_t min_Indexes_ಉದ್ದಳತೆ;
    ಆದರೆ(formatVersion0==1) {
        min_Indexes_ಉದ್ದಳತೆ=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(formatVersion0==2) {
        min_Indexes_ಉದ್ದಳತೆ=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(formatVersion0<=4) {
        min_Indexes_ಉದ್ದಳತೆ=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        min_Indexes_ಉದ್ದಳತೆ=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    ಆದರೆ(length>=0) {
        length-=ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
        ಆದರೆ(length<min_Indexes_ಉದ್ದಳತೆ*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    /* read the first few indexes */
    ಗಾಗಿ(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, ಒಳ_Indexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_ಒಟ್ಟು_ಗಾತ್ರ];

    ಆದರೆ(length>=0) {
        ಆದರೆ(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        /* copy the data for inaccessible bytes */
        ಆದರೆ(ಒಳ_ಬೈಟುಗಳು!=ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
            uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, size);
        }

        ಸ್ಥಾನಾಂತರ=0;

        /* swap the int32_t indexes[] */
        ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ=indexes[Normalizer2Impl::IX_NORM_TRIE_ಸ್ಥಾನಾಂತರ];
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು, ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ-ಸ್ಥಾನಾಂತರ, ಹೊರಹೋಗು_ಬೈಟುಗಳು, p_Error_ಸಂಕೇತ);
        ಸ್ಥಾನಾಂತರ=ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;

        /* swap the trie */
        ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ=indexes[Normalizer2Impl::IX_EXTRA_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        utrie_ಅದಲುಬದಲು_Any_ಆವೃತ್ತಿ(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ-ಸ್ಥಾನಾಂತರ, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
        ಸ್ಥಾನಾಂತರ=ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;

        /* swap the uint16_t extraData[] */
        ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ=indexes[Normalizer2Impl::IX_SMALL_FCD_ಸ್ಥಾನಾಂತರ];
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ-ಸ್ಥಾನಾಂತರ, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
        ಸ್ಥಾನಾಂತರ=ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ=indexes[Normalizer2Impl::IX_SMALL_FCD_ಸ್ಥಾನಾಂತರ+1];
        ಸ್ಥಾನಾಂತರ=ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;

        U_ASSERT(ಸ್ಥಾನಾಂತರ==size);
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+size;
}

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_NORMALIZATION
