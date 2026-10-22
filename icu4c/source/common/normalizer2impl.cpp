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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_NORMALIZATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normalizer2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringoptions.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/umutablecptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucptrie_impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uset_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
ਸਤਰਅੰਦਰ uint8_t leadByteForCP(UChar32 c) {
    ਜੇ (c <= 0x7f) {
        ਵਾਪਸ static_cast<uint8_t>(c);
    } ਨਹੀਂਤਾਂ ਜੇ (c <= 0x7ff) {
        ਵਾਪਸ static_cast<uint8_t>(0xc0 + (c >> 6));
    } ਨਹੀਂਤਾਂ {
        // Should not occur because ccc(U+0300)!=0.
        ਵਾਪਸ 0xe0;
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
UChar32 ਕੋਡ_ਬਿੰਦੂ_From_Valid_UTF8(ਅਚਲ uint8_t *cp_ਸ਼ੁਰੂ, ਅਚਲ uint8_t *cp_ਹੱਦਬੰਦੀ) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cp_ਸ਼ੁਰੂ < cp_ਹੱਦਬੰਦੀ);
    uint8_t c = *cp_ਸ਼ੁਰੂ;
    ਚੋਣ(cp_ਹੱਦਬੰਦੀ-cp_ਸ਼ੁਰੂ) {
    ਹਾਲਤ 1:
        ਵਾਪਸ c;
    ਹਾਲਤ 2:
        ਵਾਪਸ ((c&0x1f)<<6) | (cp_ਸ਼ੁਰੂ[1]&0x3f);
    ਹਾਲਤ 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        ਵਾਪਸ static_cast<char16_t>((c << 12) | ((cp_ਸ਼ੁਰੂ[1] & 0x3f) << 6) | (cp_ਸ਼ੁਰੂ[2] & 0x3f));
    ਹਾਲਤ 4:
        ਵਾਪਸ ((c&7)<<18) | ((cp_ਸ਼ੁਰੂ[1]&0x3f)<<12) | ((cp_ਸ਼ੁਰੂ[2]&0x3f)<<6) | (cp_ਸ਼ੁਰੂ[3]&0x3f);
    ਮੂਲ:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 ਪਿਛਲਾ_Hangul_Or_Jamo(ਅਚਲ uint8_t *ਸ਼ੁਰੂ, ਅਚਲ uint8_t *p) {
    ਜੇ ((p - ਸ਼ੁਰੂ) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        ਜੇ (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            ਵਾਪਸ ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    ਵਾਪਸ U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t get_Jamo_TMinus_ਅਧਾਰ(ਅਚਲ uint8_t *src, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ) {
    // Jamo T: E1 86 A8..E1 87 82
    ਜੇ ((ਹੱਦਬੰਦੀ - src) >= 3 && *src == 0xe1) {
        ਜੇ (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            ਜੇ (0xa8 <= t && t <= 0xbf) {
                ਵਾਪਸ t - 0xa7;
            }
        } ਨਹੀਂਤਾਂ ਜੇ (src[1] == 0x87) {
            uint8_t t = src[2];
            ਜੇ (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                ਵਾਪਸ t - (0xa7 - 0x40);
            }
        }
    }
    ਵਾਪਸ -1;
}

ਖਾਲੀ
append_ਕੋਡ_ਬਿੰਦੂ_Delta(ਅਚਲ uint8_t *cp_ਸ਼ੁਰੂ, ਅਚਲ uint8_t *cp_ਹੱਦਬੰਦੀ, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    ਅੱਖਰ ਬਫਰ[U8_MAX_ਲੰਮਾਈ];
    int32_t length;
    int32_t cp_ਲੰਮਾਈ = static_cast<int32_t>(cp_ਹੱਦਬੰਦੀ - cp_ਸ਼ੁਰੂ);
    ਜੇ (cp_ਲੰਮਾਈ == 1) {
        // The builder makes ASCII map to ASCII.
        ਬਫਰ[0] = static_cast<uint8_t>(*cp_ਸ਼ੁਰੂ + delta);
        length = 1;
    } ਨਹੀਂਤਾਂ {
        int32_t trail = *(cp_ਹੱਦਬੰਦੀ-1) + delta;
        ਜੇ (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cp_ਹੱਦਬੰਦੀ;
            length = 0;
            ਕਰੋ { ਬਫਰ[length++] = *cp_ਸ਼ੁਰੂ++; } ਜਦੋਂ (cp_ਸ਼ੁਰੂ < cp_ਹੱਦਬੰਦੀ);
            ਬਫਰ[length++] = static_cast<uint8_t>(trail);
        } ਨਹੀਂਤਾਂ {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = ਕੋਡ_ਬਿੰਦੂ_From_Valid_UTF8(cp_ਸ਼ੁਰੂ, cp_ਹੱਦਬੰਦੀ) + delta;
            length = 0;
            U8_APPEND_UNSAFE(ਬਫਰ, length, c);
        }
    }
    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
        edits->ਜੋੜੋ_Replace(cp_ਲੰਮਾਈ, length);
    }
    sink.Append(ਬਫਰ, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_ਬਫਰ::Reordering_ਬਫਰ(ਅਚਲ Normalizer2Impl &ni, UnicodeString &dest,
                                   UError_ਕੋਡ &error_ਕੋਡ) :
        impl(ni), str(dest),
        ਸ਼ੁਰੂ(str.get_ਬਫਰ(8)), reorder_ਸ਼ੁਰੂ(ਸ਼ੁਰੂ), ਹੱਦਬੰਦੀ(ਸ਼ੁਰੂ),
        ਬਚਿਆ_ਸਮਰੱਥਾ(str.get_ਸਮਰੱਥਾ()), ਆਖਰੀ_CC(0) {
    ਜੇ (ਸ਼ੁਰੂ == ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(error_ਕੋਡ)) {
        // getBuffer() already did str.setToBogus()
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}

UBool Reordering_ਬਫਰ::init(int32_t dest_ਸਮਰੱਥਾ, UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t length=str.length();
    ਸ਼ੁਰੂ=str.get_ਬਫਰ(dest_ਸਮਰੱਥਾ);
    ਜੇ(ਸ਼ੁਰੂ==ਖਾਲੀ_ਸੂਚਕ) {
        // getBuffer() already did str.setToBogus()
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ+length;
    ਬਚਿਆ_ਸਮਰੱਥਾ=str.get_ਸਮਰੱਥਾ()-length;
    reorder_ਸ਼ੁਰੂ=ਸ਼ੁਰੂ;
    ਜੇ(ਸ਼ੁਰੂ==ਹੱਦਬੰਦੀ) {
        ਆਖਰੀ_CC=0;
    } ਨਹੀਂਤਾਂ {
        ਸੈੱਟ_Iterator();
        ਆਖਰੀ_CC=ਪਿਛਲਾ_CC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        ਜੇ(ਆਖਰੀ_CC>1) {
            ਜਦੋਂ(ਪਿਛਲਾ_CC()>1) {}
        }
        reorder_ਸ਼ੁਰੂ=ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ;
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool Reordering_ਬਫਰ::equals(ਅਚਲ char16_t *other_ਸ਼ੁਰੂ, ਅਚਲ char16_t *other_ਹੱਦਬੰਦੀ) ਅਚਲ {
    int32_t length = static_cast<int32_t>(ਹੱਦਬੰਦੀ - ਸ਼ੁਰੂ);
    ਵਾਪਸ
        length == static_cast<int32_t>(other_ਹੱਦਬੰਦੀ - other_ਸ਼ੁਰੂ) &&
        0==u_memcmp(ਸ਼ੁਰੂ, other_ਸ਼ੁਰੂ, length);
}

UBool Reordering_ਬਫਰ::equals(ਅਚਲ uint8_t *other_ਸ਼ੁਰੂ, ਅਚਲ uint8_t *other_ਹੱਦਬੰਦੀ) ਅਚਲ {
    U_ASSERT((other_ਹੱਦਬੰਦੀ - other_ਸ਼ੁਰੂ) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(ਹੱਦਬੰਦੀ - ਸ਼ੁਰੂ);
    int32_t other_ਲੰਮਾਈ = static_cast<int32_t>(other_ਹੱਦਬੰਦੀ - other_ਸ਼ੁਰੂ);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    ਜੇ (other_ਲੰਮਾਈ < length || (other_ਲੰਮਾਈ / 3) > length) {
        ਵਾਪਸ ਝੂਠ;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    ਲਈ (int32_t i = 0, j = 0;;) {
        ਜੇ (i >= length) {
            ਵਾਪਸ j >= other_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ (j >= other_ਲੰਮਾਈ) {
            ਵਾਪਸ ਝੂਠ;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_ਅਗਲਾ_UNSAFE(ਸ਼ੁਰੂ, i, c);
        U8_ਅਗਲਾ_UNSAFE(other_ਸ਼ੁਰੂ, j, other);
        ਜੇ (c != other) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
}

UBool Reordering_ਬਫਰ::appendSupplementary(UChar32 c, uint8_t cc, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(ਬਚਿਆ_ਸਮਰੱਥਾ<2 && !resize(2, error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(ਆਖਰੀ_CC<=cc || cc==0) {
        ਹੱਦਬੰਦੀ[0]=U16_LEAD(c);
        ਹੱਦਬੰਦੀ[1]=U16_TRAIL(c);
        ਹੱਦਬੰਦੀ+=2;
        ਆਖਰੀ_CC=cc;
        ਜੇ(cc<=1) {
            reorder_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ;
        }
    } ਨਹੀਂਤਾਂ {
        insert(c, cc);
    }
    ਬਚਿਆ_ਸਮਰੱਥਾ-=2;
    ਵਾਪਸ ਸੱਚ;
}

UBool Reordering_ਬਫਰ::append(ਅਚਲ char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(length==0) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(ਬਚਿਆ_ਸਮਰੱਥਾ<length && !resize(length, error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਬਚਿਆ_ਸਮਰੱਥਾ-=length;
    ਜੇ(ਆਖਰੀ_CC<=leadCC || leadCC==0) {
        ਜੇ(trailCC<=1) {
            reorder_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ+length;
        } ਨਹੀਂਤਾਂ ਜੇ(leadCC<=1) {
            reorder_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ+1;  // Ok if not a code point boundary.
        }
        ਅਚਲ char16_t *s_ਹੱਦਬੰਦੀ=s+length;
        ਕਰੋ { *ਹੱਦਬੰਦੀ++=*s++; } ਜਦੋਂ(s!=s_ਹੱਦਬੰਦੀ);
        ਆਖਰੀ_CC=trailCC;
    } ਨਹੀਂਤਾਂ {
        int32_t i=0;
        UChar32 c;
        U16_ਅਗਲਾ(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        ਜਦੋਂ(i<length) {
            U16_ਅਗਲਾ(s, i, length, c);
            ਜੇ(i<length) {
                ਜੇ (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } ਨਹੀਂਤਾਂ {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } ਨਹੀਂਤਾਂ {
                leadCC=trailCC;
            }
            append(c, leadCC, error_ਕੋਡ);
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool Reordering_ਬਫਰ::appendZeroCC(UChar32 c, UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t cp_ਲੰਮਾਈ=U16_ਲੰਮਾਈ(c);
    ਜੇ(ਬਚਿਆ_ਸਮਰੱਥਾ<cp_ਲੰਮਾਈ && !resize(cp_ਲੰਮਾਈ, error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਬਚਿਆ_ਸਮਰੱਥਾ-=cp_ਲੰਮਾਈ;
    ਜੇ(cp_ਲੰਮਾਈ==1) {
        *ਹੱਦਬੰਦੀ++ = static_cast<char16_t>(c);
    } ਨਹੀਂਤਾਂ {
        ਹੱਦਬੰਦੀ[0]=U16_LEAD(c);
        ਹੱਦਬੰਦੀ[1]=U16_TRAIL(c);
        ਹੱਦਬੰਦੀ+=2;
    }
    ਆਖਰੀ_CC=0;
    reorder_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ;
    ਵਾਪਸ ਸੱਚ;
}

UBool Reordering_ਬਫਰ::appendZeroCC(ਅਚਲ char16_t *s, ਅਚਲ char16_t *s_ਹੱਦਬੰਦੀ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(s==s_ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ ਸੱਚ;
    }
    int32_t length = static_cast<int32_t>(s_ਹੱਦਬੰਦੀ - s);
    ਜੇ(ਬਚਿਆ_ਸਮਰੱਥਾ<length && !resize(length, error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    u_memcpy(ਹੱਦਬੰਦੀ, s, length);
    ਹੱਦਬੰਦੀ+=length;
    ਬਚਿਆ_ਸਮਰੱਥਾ-=length;
    ਆਖਰੀ_CC=0;
    reorder_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ;
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ Reordering_ਬਫਰ::remove() {
    reorder_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ;
    ਬਚਿਆ_ਸਮਰੱਥਾ=str.get_ਸਮਰੱਥਾ();
    ਆਖਰੀ_CC=0;
}

ਖਾਲੀ Reordering_ਬਫਰ::removeSuffix(int32_t suffix_ਲੰਮਾਈ) {
    ਜੇ(suffix_ਲੰਮਾਈ<(ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ)) {
        ਹੱਦਬੰਦੀ-=suffix_ਲੰਮਾਈ;
        ਬਚਿਆ_ਸਮਰੱਥਾ+=suffix_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ;
        ਬਚਿਆ_ਸਮਰੱਥਾ=str.get_ਸਮਰੱਥਾ();
    }
    ਆਖਰੀ_CC=0;
    reorder_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ;
}

UBool Reordering_ਬਫਰ::resize(int32_t append_ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t reorder_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = static_cast<int32_t>(reorder_ਸ਼ੁਰੂ - ਸ਼ੁਰੂ);
    int32_t length = static_cast<int32_t>(ਹੱਦਬੰਦੀ - ਸ਼ੁਰੂ);
    str.release_ਬਫਰ(length);
    int32_t new_ਸਮਰੱਥਾ=length+append_ਲੰਮਾਈ;
    int32_t double_ਸਮਰੱਥਾ=2*str.get_ਸਮਰੱਥਾ();
    ਜੇ(new_ਸਮਰੱਥਾ<double_ਸਮਰੱਥਾ) {
        new_ਸਮਰੱਥਾ=double_ਸਮਰੱਥਾ;
    }
    ਜੇ(new_ਸਮਰੱਥਾ<256) {
        new_ਸਮਰੱਥਾ=256;
    }
    ਸ਼ੁਰੂ=str.get_ਬਫਰ(new_ਸਮਰੱਥਾ);
    ਜੇ(ਸ਼ੁਰੂ==ਖਾਲੀ_ਸੂਚਕ) {
        // getBuffer() already did str.setToBogus()
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    reorder_ਸ਼ੁਰੂ=ਸ਼ੁਰੂ+reorder_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ;
    ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ+length;
    ਬਚਿਆ_ਸਮਰੱਥਾ=str.get_ਸਮਰੱਥਾ()-length;
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ Reordering_ਬਫਰ::skip_ਪਿਛਲਾ() {
    ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ=ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
    char16_t c=*--ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
    ਜੇ(U16_IS_TRAIL(c) && ਸ਼ੁਰੂ<ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ && U16_IS_LEAD(*(ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ-1))) {
        --ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
    }
}

uint8_t Reordering_ਬਫਰ::ਪਿਛਲਾ_CC() {
    ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ=ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
    ਜੇ(reorder_ਸ਼ੁਰੂ>=ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ) {
        ਵਾਪਸ 0;
    }
    UChar32 c=*--ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
    char16_t c2;
    ਜੇ(U16_IS_TRAIL(c) && ਸ਼ੁਰੂ<ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ && U16_IS_LEAD(c2=*(ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ-1))) {
        --ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    ਵਾਪਸ impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
ਖਾਲੀ Reordering_ਬਫਰ::insert(UChar32 c, uint8_t cc) {
    ਲਈ(ਸੈੱਟ_Iterator(), skip_ਪਿਛਲਾ(); ਪਿਛਲਾ_CC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=ਹੱਦਬੰਦੀ;
    char16_t *r=ਹੱਦਬੰਦੀ+=U16_ਲੰਮਾਈ(c);
    ਕਰੋ {
        *--r=*--q;
    } ਜਦੋਂ(ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ!=q);
    write_ਕੋਡ_ਬਿੰਦੂ(q, c);
    ਜੇ(cc<=1) {
        reorder_ਸ਼ੁਰੂ=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

ਬਣਤਰ Canon_Iter_ਅੰਕੜੇ : ਜਨਤਕ UMemory {
    Canon_Iter_ਅੰਕੜੇ(UError_ਕੋਡ &error_ਕੋਡ);
    ~Canon_Iter_ਅੰਕੜੇ();
    ਖਾਲੀ ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_ਸੈੱਟ(UChar32 origin, UChar32 decompLead, UError_ਕੋਡ &error_ਕੋਡ);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canon_ਸ਼ੁਰੂ_Sets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    ਮਿਟਾਓ f_Canon_Iter_ਅੰਕੜੇ;
}

ਖਾਲੀ
Normalizer2Impl::init(ਅਚਲ int32_t *ਵਿੱਚ_Indexes, ਅਚਲ UCPTrie *ਵਿੱਚ_Trie,
                      ਅਚਲ uint16_t *ਵਿੱਚ_Extra_ਅੰਕੜੇ, ਅਚਲ uint8_t *ਵਿੱਚ_Small_FCD) {
    minDecompNoCP = static_cast<char16_t>(ਵਿੱਚ_Indexes[IX_MIN_DECOMP_NO_CP]);
    minCompNoMaybeCP = static_cast<char16_t>(ਵਿੱਚ_Indexes[IX_MIN_COMP_NO_MAYBE_CP]);
    minLcccCP = static_cast<char16_t>(ਵਿੱਚ_Indexes[IX_MIN_LCCC_CP]);

    minYesNo = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_YES_NO]);
    minYesNoMappingsOnly = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_YES_NO_MAPPINGS_ONLY]);
    minNoNo = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_NO_NO]);
    minNoNoCompBoundaryBefore = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE]);
    minNoNoCompNoMaybeCC = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC]);
    min_No_No_ਖਾਲੀ_ਥਾਵਾਂ = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_NO_NO_ਖਾਲੀ_ਥਾਵਾਂ]);
    ਹੱਦਬੰਦੀ_No_No = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_ਹੱਦਬੰਦੀ_NO_NO]);
    minMaybeNo = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_MAYBE_NO]);
    minMaybeNoCombinesFwd = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_MAYBE_NO_COMBINES_FWD]);
    minMaybeYes = static_cast<uint16_t>(ਵਿੱਚ_Indexes[IX_MIN_MAYBE_YES]);
    U_ASSERT((minMaybeNo & 7) == 0);  // 8-aligned for noNoDelta bit fields
    centerNoNoDelta = (minMaybeNo >> DELTA_ਖਿਸਕਾਓ) - MAX_DELTA - 1;

    normTrie=ਵਿੱਚ_Trie;
    extra_ਅੰਕੜੇ=ਵਿੱਚ_Extra_ਅੰਕੜੇ;
    smallFCD=ਵਿੱਚ_Small_FCD;
}

U_CDECL_BEGIN

ਸਥਿਰ uint32_t U_CALLCONV
segmentStarterMapper(ਅਚਲ ਖਾਲੀ * /*context*/, uint32_t value) {
    ਵਾਪਸ value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_ਸਿਰਾ

ਖਾਲੀ
Normalizer2Impl::ਜੋੜੋ_Lccc_Chars(Unicode_ਸੈੱਟ &ਸੈੱਟ) ਅਚਲ {
    UChar32 ਸ਼ੁਰੂ = 0, ਸਿਰਾ;
    uint32_t norm16;
    ਜਦੋਂ ((ਸਿਰਾ = ucptrie_getRange(normTrie, ਸ਼ੁਰੂ, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &norm16)) >= 0) {
        ਜੇ (norm16 > Normalizer2Impl::MIN_ਸਾਧਾਰਨ_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            ਸੈੱਟ.ਜੋੜੋ(ਸ਼ੁਰੂ, ਸਿਰਾ);
        } ਨਹੀਂਤਾਂ ਜੇ (minNoNoCompNoMaybeCC <= norm16 && norm16 < ਹੱਦਬੰਦੀ_No_No) {
            uint16_t fcd16 = getFCD16(ਸ਼ੁਰੂ);
            ਜੇ (fcd16 > 0xff) { ਸੈੱਟ.ਜੋੜੋ(ਸ਼ੁਰੂ, ਸਿਰਾ); }
        }
        ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
    }
}

ਖਾਲੀ
Normalizer2Impl::ਜੋੜੋ_Property_Starts(ਅਚਲ USetAdder *sa, UError_ਕੋਡ & /*errorCode*/) ਅਚਲ {
    // Add the start code point of each same-value range of the trie.
    UChar32 ਸ਼ੁਰੂ = 0, ਸਿਰਾ;
    uint32_t value;
    ਜਦੋਂ ((ਸਿਰਾ = ucptrie_getRange(normTrie, ਸ਼ੁਰੂ, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &value)) >= 0) {
        sa->ਜੋੜੋ(sa->ਸੈੱਟ, ਸ਼ੁਰੂ);
        ਜੇ (ਸ਼ੁਰੂ != ਸਿਰਾ && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_ਨਕਾਬ) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(ਸ਼ੁਰੂ);
            ਜਦੋਂ (++ਸ਼ੁਰੂ <= ਸਿਰਾ) {
                uint16_t fcd16 = getFCD16(ਸ਼ੁਰੂ);
                ਜੇ (fcd16 != prevFCD16) {
                    sa->ਜੋੜੋ(sa->ਸੈੱਟ, ਸ਼ੁਰੂ);
                    prevFCD16 = fcd16;
                }
            }
        }
        ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    ਲਈ(char16_t c=Hangul::HANGUL_ਅਧਾਰ; c<Hangul::HANGUL_ਹੱਦਬੰਦੀ; c+=Hangul::JAMO_T_ਗਿਣਤੀ) {
        sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
        sa->ਜੋੜੋ(sa->ਸੈੱਟ, c+1);
    }
    sa->ਜੋੜੋ(sa->ਸੈੱਟ, Hangul::HANGUL_ਹੱਦਬੰਦੀ); /* add Hangul+1 to continue with other properties */
}

ਖਾਲੀ
Normalizer2Impl::ਜੋੜੋ_Canon_Iter_Property_Starts(ਅਚਲ USetAdder *sa, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    ਜੇ (!ensure_Canon_Iter_ਅੰਕੜੇ(error_ਕੋਡ)) { ਵਾਪਸ; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 ਸ਼ੁਰੂ = 0, ਸਿਰਾ;
    uint32_t value;
    ਜਦੋਂ ((ਸਿਰਾ = ucptrie_getRange(f_Canon_Iter_ਅੰਕੜੇ->trie, ਸ਼ੁਰੂ, UCPMAP_RANGE_ਸਾਧਾਰਨ, 0,
                                   segmentStarterMapper, ਖਾਲੀ_ਸੂਚਕ, &value)) >= 0) {
        sa->ਜੋੜੋ(sa->ਸੈੱਟ, ਸ਼ੁਰੂ);
        ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
    }
}

ਅਚਲ char16_t *
Normalizer2Impl::copy_ਨੀਵਾਂ_ਅਗੇਤਰ_From_Nul_Terminated(ਅਚਲ char16_t *src,
                                                UChar32 min_Need_ਅੰਕੜੇ_CP,
                                                Reordering_ਬਫਰ *ਬਫਰ,
                                                UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ਅਚਲ char16_t *prevSrc=src;
    char16_t c;
    ਜਦੋਂ((c=*src++)<min_Need_ਅੰਕੜੇ_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    ਜੇ(--src!=prevSrc) {
        ਜੇ(ਬਫਰ!=ਖਾਲੀ_ਸੂਚਕ) {
            ਬਫਰ->appendZeroCC(prevSrc, src, error_ਕੋਡ);
        }
    }
    ਵਾਪਸ src;
}

UnicodeString &
Normalizer2Impl::decompose(ਅਚਲ UnicodeString &src, UnicodeString &dest,
                           UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        dest.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ dest;
    }
    ਅਚਲ char16_t *sArray=src.get_ਬਫਰ();
    ਜੇ(&dest==&src || sArray==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        dest.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), error_ਕੋਡ);
    ਵਾਪਸ dest;
}

ਖਾਲੀ
Normalizer2Impl::decompose(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                           UnicodeString &dest,
                           int32_t dest_ਲੰਮਾਈ_Estimate,
                           UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(dest_ਲੰਮਾਈ_Estimate<0 && ਹੱਦਬੰਦੀ!=ਖਾਲੀ_ਸੂਚਕ) {
        dest_ਲੰਮਾਈ_Estimate = static_cast<int32_t>(ਹੱਦਬੰਦੀ - src);
    }
    dest.remove();
    Reordering_ਬਫਰ ਬਫਰ(*ਇਹ, dest);
    ਜੇ(ਬਫਰ.init(dest_ਲੰਮਾਈ_Estimate, error_ਕੋਡ)) {
        decompose(src, ਹੱਦਬੰਦੀ, &ਬਫਰ, error_ਕੋਡ);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ਅਚਲ char16_t *
Normalizer2Impl::decompose(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                           Reordering_ਬਫਰ *ਬਫਰ,
                           UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(src==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ src;
    }
    UChar32 minNoCP=minDecompNoCP;
    ਜੇ(ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ) {
        src=copy_ਨੀਵਾਂ_ਅਗੇਤਰ_From_Nul_Terminated(src, minNoCP, ਬਫਰ, error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ src;
        }
        ਹੱਦਬੰਦੀ=u_strchr(src, 0);
    }

    ਅਚਲ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ਅਚਲ char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    ਲਈ(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        ਲਈ(prevSrc=src; src!=ਹੱਦਬੰਦੀ;) {
            ਜੇ( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ਨਹੀਂਤਾਂ ਜੇ(!U16_IS_LEAD(c)) {
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                char16_t c2;
                ਜੇ((src+1)!=ਹੱਦਬੰਦੀ && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    ਜੇ(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } ਨਹੀਂਤਾਂ {
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        ਜੇ(src!=prevSrc) {
            ਜੇ(ਬਫਰ!=ਖਾਲੀ_ਸੂਚਕ) {
                ਜੇ(!ਬਫਰ->appendZeroCC(prevSrc, src, error_ਕੋਡ)) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                prevCC=0;
                prevBoundary=src;
            }
        }
        ਜੇ(src==ਹੱਦਬੰਦੀ) {
            ਰੋਕੋ;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_ਲੰਮਾਈ(c);
        ਜੇ(ਬਫਰ!=ਖਾਲੀ_ਸੂਚਕ) {
            ਜੇ(!decompose(c, norm16, *ਬਫਰ, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                ਜੇ(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    ਜੇ(cc<=1) {
                        prevBoundary=src;
                    }
                    ਜਾਰੀ;
                }
            }
            ਵਾਪਸ prevBoundary;  // "no" or cc out of order
        }
    }
    ਵਾਪਸ src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ਅਚਲ char16_t *
Normalizer2Impl::decomposeShort(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜਦੋਂ(src<ਹੱਦਬੰਦੀ) {
        ਜੇ (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            ਵਾਪਸ src;
        }
        ਅਚਲ char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_ਅਗਲਾ(normTrie, UCPTRIE_16, src, ਹੱਦਬੰਦੀ, c, norm16);
        ਜੇ (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            ਵਾਪਸ prevSrc;
        }
        ਜੇ(!decompose(c, norm16, ਬਫਰ, error_ਕੋਡ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਜੇ (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ਵਾਪਸ src;
        }
    }
    ਵਾਪਸ src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_ਬਫਰ &ਬਫਰ,
                                 UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    // get the decomposition and the lead and trail cc's
    ਜੇ (norm16 >= ਹੱਦਬੰਦੀ_No_No) {
        ਜੇ (isMaybeYesOrNonZeroCC(norm16)) {
            ਵਾਪਸ ਬਫਰ.append(c, getCCFromYesOrMaybeYes(norm16), error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ਜੇ (norm16 < minYesNo) {
        // c does not decompose
        ਵਾਪਸ ਬਫਰ.append(c, 0, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ ਜੇ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        ਵਾਪਸ ਬਫਰ.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_ਕੋਡ);
    }
    // c decomposes, get everything from the variable-length extra data
    ਅਚਲ uint16_t *mapping=get_ਅੰਕੜੇ(norm16);
    uint16_t ਪਹਿਲਾ_Unit=*mapping;
    int32_t length=ਪਹਿਲਾ_Unit&MAPPING_ਲੰਮਾਈ_ਨਕਾਬ;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(ਪਹਿਲਾ_Unit >> 8);
    ਜੇ(ਪਹਿਲਾ_Unit&MAPPING_HAS_CCC_LCCC_ਸ਼ਬਦ) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } ਨਹੀਂਤਾਂ {
        leadCC=0;
    }
    ਵਾਪਸ ਬਫਰ.append(reinterpret_cast<ਅਚਲ char16_t*>(mapping) + 1, length, ਸੱਚ, leadCC, trailCC, error_ਕੋਡ);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ਅਚਲ uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ਅਚਲ uint8_t *src, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ,
                               ByteSink *sink, Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    U_ASSERT(ਹੱਦਬੰਦੀ != ਖਾਲੀ_ਸੂਚਕ);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ਅਚਲ uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    ਲਈ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ਅਚਲ uint8_t *fast_ਸ਼ੁਰੂ = src;
        ਅਚਲ uint8_t *prevSrc;
        uint16_t norm16 = 0;

        ਲਈ (;;) {
            ਜੇ (src == ਹੱਦਬੰਦੀ) {
                ਜੇ (prevBoundary != ਹੱਦਬੰਦੀ && sink != ਖਾਲੀ_ਸੂਚਕ) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, ਹੱਦਬੰਦੀ,
                                                  *sink, options, edits, error_ਕੋਡ);
                }
                ਵਾਪਸ src;
            }
            ਜੇ (*src < minNoLead) {
                ++src;
            } ਨਹੀਂਤਾਂ {
                prevSrc = src;
                UCPTRIE_FAST_U8_ਅਗਲਾ(normTrie, UCPTRIE_16, src, ਹੱਦਬੰਦੀ, norm16);
                ਜੇ (!isMostDecompYesAndZeroCC(norm16)) {
                    ਰੋਕੋ;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        ਜੇ (prevSrc != fast_ਸ਼ੁਰੂ) {
            // The fast path looped over yes/0 characters before the current one.
            ਜੇ (sink != ਖਾਲੀ_ਸੂਚਕ &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        ਜੇ (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            ਜੇ (prevCC <= cc || cc == 0) {
                prevCC = cc;
                ਜੇ (cc <= 1) {
                    ਜੇ (sink != ਖਾਲੀ_ਸੂਚਕ &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    prevBoundary = src;
                }
                ਜਾਰੀ;
            }
        }
        ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        ਜੇ (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            ਜੇ (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
            prevBoundary = prevSrc;
        }
        Reordering_ਬਫਰ ਬਫਰ(*ਇਹ, s16, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਰੋਕੋ;
        }
        decomposeShort(prevBoundary, src, STOP_AT_ਹੱਦਬੰਦੀ, ਝੂਠ /* onlyContiguous */,
                       ਬਫਰ, error_ਕੋਡ);
        // Decompose until the next boundary.
        ਜੇ (ਬਫਰ.get_ਆਖਰੀ_CC() > 1) {
            src = decomposeShort(src, ਹੱਦਬੰਦੀ, STOP_AT_DECOMP_BOUNDARY, ਝੂਠ /* onlyContiguous */,
                                 ਬਫਰ, error_ਕੋਡ);
        }
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਰੋਕੋ;
        }
        ਜੇ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਰੋਕੋ;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        ਜੇ (isMaybeYesOrNonZeroCC(norm16) && ਬਫਰ.equals(prevBoundary, src)) {
            ਜੇ (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ (!ByteSinkUtil::appendChange(prevBoundary, src, ਬਫਰ.get_ਸ਼ੁਰੂ(), ਬਫਰ.length(),
                                            *sink, edits, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    ਵਾਪਸ src;
}

ਅਚਲ uint8_t *
Normalizer2Impl::decomposeShort(ਅਚਲ uint8_t *src, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ,
                                StopAt stopAt, UBool onlyContiguous,
                                Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜਦੋਂ (src < ਹੱਦਬੰਦੀ) {
        ਅਚਲ uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_ਅਗਲਾ(normTrie, UCPTRIE_16, src, ਹੱਦਬੰਦੀ, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        ਜੇ (norm16 >= ਹੱਦਬੰਦੀ_No_No) {
            ਜੇ (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                ਜੇ (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    ਵਾਪਸ prevSrc;
                }
                c = ਕੋਡ_ਬਿੰਦੂ_From_Valid_UTF8(prevSrc, src);
                ਜੇ (!ਬਫਰ.append(c, cc, error_ਕੋਡ)) {
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
                ਜੇ (stopAt == STOP_AT_DECOMP_BOUNDARY && ਬਫਰ.get_ਆਖਰੀ_CC() <= 1) {
                    ਵਾਪਸ src;
                }
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                ਜੇ (stopAt != STOP_AT_ਹੱਦਬੰਦੀ) {
                    ਵਾਪਸ prevSrc;
                }
                c = ਕੋਡ_ਬਿੰਦੂ_From_Valid_UTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } ਨਹੀਂਤਾਂ ਜੇ (stopAt != STOP_AT_ਹੱਦਬੰਦੀ && norm16 < minNoNoCompNoMaybeCC) {
            ਵਾਪਸ prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        ਜੇ (norm16 < minYesNo) {
            ਜੇ (c < 0) {
                c = ਕੋਡ_ਬਿੰਦੂ_From_Valid_UTF8(prevSrc, src);
            }
            // does not decompose
            ਜੇ (!ਬਫਰ.append(c, 0, error_ਕੋਡ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            ਜੇ (c < 0) {
                c = ਕੋਡ_ਬਿੰਦੂ_From_Valid_UTF8(prevSrc, src);
            }
            char16_t jamos[3];
            ਜੇ (!ਬਫਰ.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_ਕੋਡ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        } ਨਹੀਂਤਾਂ {
            // The character decomposes, get everything from the variable-length extra data.
            ਅਚਲ uint16_t *mapping = get_ਅੰਕੜੇ(norm16);
            uint16_t ਪਹਿਲਾ_Unit = *mapping;
            int32_t length = ਪਹਿਲਾ_Unit & MAPPING_ਲੰਮਾਈ_ਨਕਾਬ;
            uint8_t trailCC = static_cast<uint8_t>(ਪਹਿਲਾ_Unit >> 8);
            uint8_t leadCC;
            ਜੇ (ਪਹਿਲਾ_Unit & MAPPING_HAS_CCC_LCCC_ਸ਼ਬਦ) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } ਨਹੀਂਤਾਂ {
                leadCC = 0;
            }
            ਜੇ (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                ਵਾਪਸ prevSrc;
            }
            ਜੇ (!ਬਫਰ.append(reinterpret_cast<ਅਚਲ char16_t*>(mapping) + 1, length, ਸੱਚ, leadCC, trailCC, error_ਕੋਡ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        }
        ਜੇ ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && ਬਫਰ.get_ਆਖਰੀ_CC() <= 1)) {
            ਵਾਪਸ src;
        }
    }
    ਵਾਪਸ src;
}

ਅਚਲ char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t ਬਫਰ[4], int32_t &length) ਅਚਲ {
    uint16_t norm16;
    ਜੇ(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ char16_t *decomp = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=ਬਫਰ;
        length=0;
        U16_APPEND_UNSAFE(ਬਫਰ, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    ਜੇ (norm16 < minYesNo) {
        ਵਾਪਸ decomp;
    } ਨਹੀਂਤਾਂ ਜੇ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, ਬਫਰ);
        ਵਾਪਸ ਬਫਰ;
    }
    // c decomposes, get everything from the variable-length extra data
    ਅਚਲ uint16_t *mapping=get_ਅੰਕੜੇ(norm16);
    length=*mapping&MAPPING_ਲੰਮਾਈ_ਨਕਾਬ;
    ਵਾਪਸ reinterpret_cast<ਅਚਲ char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ਅਚਲ char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t ਬਫਰ[30], int32_t &length) ਅਚਲ {
    uint16_t norm16;
    ਜੇ(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, ਬਫਰ);
        length=2;
        ਵਾਪਸ ਬਫਰ;
    } ਨਹੀਂਤਾਂ ਜੇ(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(ਬਫਰ, length, c);
        ਵਾਪਸ ਬਫਰ;
    }
    // c decomposes, get everything from the variable-length extra data
    ਅਚਲ uint16_t *mapping=get_ਅੰਕੜੇ(norm16);
    uint16_t ਪਹਿਲਾ_Unit=*mapping;
    int32_t m_ਲੰਮਾਈ=ਪਹਿਲਾ_Unit&MAPPING_ਲੰਮਾਈ_ਨਕਾਬ;  // length of normal mapping
    ਜੇ(ਪਹਿਲਾ_Unit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ਅਚਲ uint16_t *rawMapping=mapping-((ਪਹਿਲਾ_Unit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        ਜੇ(rm0<=MAPPING_ਲੰਮਾਈ_ਨਕਾਬ) {
            length=rm0;
            ਵਾਪਸ reinterpret_cast<ਅਚਲ char16_t*>(rawMapping) - rm0;
        } ਨਹੀਂਤਾਂ {
            // Copy the normal mapping and replace its first two code units with rm0.
            ਬਫਰ[0] = static_cast<char16_t>(rm0);
            u_memcpy(ਬਫਰ + 1, reinterpret_cast<ਅਚਲ char16_t*>(mapping) + 1 + 2, m_ਲੰਮਾਈ - 2);
            length=m_ਲੰਮਾਈ-1;
            ਵਾਪਸ ਬਫਰ;
        }
    } ਨਹੀਂਤਾਂ {
        length=m_ਲੰਮਾਈ;
        ਵਾਪਸ reinterpret_cast<ਅਚਲ char16_t*>(mapping) + 1;
    }
}

ਖਾਲੀ Normalizer2Impl::decomposeAndAppend(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                                         UBool doDecompose,
                                         UnicodeString &ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ,
                                         Reordering_ਬਫਰ &ਬਫਰ,
                                         UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਬਫਰ.copy_Reorderable_Suffix_ਮੰਜ਼ਿਲ(ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ);
    ਜੇ(doDecompose) {
        decompose(src, ਹੱਦਬੰਦੀ, &ਬਫਰ, error_ਕੋਡ);
        ਵਾਪਸ;
    }
    // Just merge the strings at the boundary.
    ਤਾਰਕਿਕ is_ਪਹਿਲਾ = ਸੱਚ;
    uint8_t ਪਹਿਲਾ_CC = 0, prevCC = 0, cc;
    ਅਚਲ char16_t *p = src;
    ਜਦੋਂ (p != ਹੱਦਬੰਦੀ) {
        ਅਚਲ char16_t *ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_ਅਗਲਾ(normTrie, UCPTRIE_16, p, ਹੱਦਬੰਦੀ, c, norm16);
        ਜੇ ((cc = getCC(norm16)) == 0) {
            p = ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
            ਰੋਕੋ;
        }
        ਜੇ (is_ਪਹਿਲਾ) {
            ਪਹਿਲਾ_CC = cc;
            is_ਪਹਿਲਾ = ਝੂਠ;
        }
        prevCC = cc;
    }
    ਜੇ(ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ) {  // appendZeroCC() needs limit!=nullptr
        ਹੱਦਬੰਦੀ=u_strchr(p, 0);
    }

    ਜੇ (ਬਫਰ.append(src, static_cast<int32_t>(p - src), ਝੂਠ, ਪਹਿਲਾ_CC, prevCC, error_ਕੋਡ)) {
        ਬਫਰ.appendZeroCC(p, ਹੱਦਬੰਦੀ, error_ਕੋਡ);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ਅਚਲ {
    ਵਾਪਸ c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ਅਚਲ {
    ਜੇ (norm16 < minNoNoCompNoMaybeCC) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (norm16 >= ਹੱਦਬੰਦੀ_No_No) {
        ਵਾਪਸ norm16 <= MIN_ਸਾਧਾਰਨ_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ਅਚਲ uint16_t *mapping=get_ਅੰਕੜੇ_For_Yes_Or_No(norm16);
    uint16_t ਪਹਿਲਾ_Unit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ਵਾਪਸ (ਪਹਿਲਾ_Unit&MAPPING_HAS_CCC_LCCC_ਸ਼ਬਦ)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ਅਚਲ {
    ਜੇ (c < minDecompNoCP) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ਅਚਲ {
    ਜੇ(norm16 <= minYesNo || isHangulLVT(norm16)) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (norm16 >= ਹੱਦਬੰਦੀ_No_No) {
        ਜੇ (isMaybeYesOrNonZeroCC(norm16)) {
            ਵਾਪਸ norm16 <= MIN_ਸਾਧਾਰਨ_MAYBE_YES || norm16 == JAMO_VT;
        } ਨਹੀਂਤਾਂ ਜੇ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            ਵਾਪਸ (norm16 & DELTA_TCCC_ਨਕਾਬ) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ਅਚਲ uint16_t *mapping=get_ਅੰਕੜੇ(norm16);
    uint16_t ਪਹਿਲਾ_Unit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    ਜੇ(ਪਹਿਲਾ_Unit>0x1ff) {
        ਵਾਪਸ ਝੂਠ;  // trailCC>1
    }
    ਜੇ(ਪਹਿਲਾ_Unit<=0xff) {
        ਵਾਪਸ ਸੱਚ;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ਵਾਪਸ (ਪਹਿਲਾ_Unit&MAPPING_HAS_CCC_LCCC_ਸ਼ਬਦ)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(ਅਚਲ uint16_t *list, UChar32 trail) {
    uint16_t key1, ਪਹਿਲਾ_Unit;
    ਜੇ(trail<COMP_1_TRAIL_ਹੱਦਬੰਦੀ) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        ਜਦੋਂ(key1>(ਪਹਿਲਾ_Unit=*list)) {
            list+=2+(ਪਹਿਲਾ_Unit&COMP_1_TRIPLE);
        }
        ਜੇ(key1==(ਪਹਿਲਾ_Unit&COMP_1_TRAIL_ਨਕਾਬ)) {
            ਜੇ(ਪਹਿਲਾ_Unit&COMP_1_TRIPLE) {
                ਵਾਪਸ (static_cast<int32_t>(list[1]) << 16) | list[2];
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ list[1];
            }
        }
    } ਨਹੀਂਤਾਂ {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_ਹੱਦਬੰਦੀ +
                        (((trail>>COMP_1_TRAIL_ਖਿਸਕਾਓ))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_ਖਿਸਕਾਓ);
        uint16_t ਦੂਜਾ_Unit;
        ਲਈ(;;) {
            ਜੇ(key1>(ਪਹਿਲਾ_Unit=*list)) {
                list+=2+(ਪਹਿਲਾ_Unit&COMP_1_TRIPLE);
            } ਨਹੀਂਤਾਂ ਜੇ(key1==(ਪਹਿਲਾ_Unit&COMP_1_TRAIL_ਨਕਾਬ)) {
                ਜੇ(key2>(ਦੂਜਾ_Unit=list[1])) {
                    ਜੇ(ਪਹਿਲਾ_Unit&COMP_1_ਆਖਰੀ_TUPLE) {
                        ਰੋਕੋ;
                    } ਨਹੀਂਤਾਂ {
                        list+=3;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(key2==(ਦੂਜਾ_Unit&COMP_2_TRAIL_ਨਕਾਬ)) {
                    ਵਾਪਸ (static_cast<int32_t>(ਦੂਜਾ_Unit & ~COMP_2_TRAIL_ਨਕਾਬ) << 16) | list[2];
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        }
    }
    ਵਾਪਸ -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
ਖਾਲੀ Normalizer2Impl::ਜੋੜੋ_Composites(ਅਚਲ uint16_t *list, Unicode_ਸੈੱਟ &ਸੈੱਟ) ਅਚਲ {
    uint16_t ਪਹਿਲਾ_Unit;
    int32_t ਸੰਯੁਕਤ_And_Fwd;
    ਕਰੋ {
        ਪਹਿਲਾ_Unit=*list;
        ਜੇ((ਪਹਿਲਾ_Unit&COMP_1_TRIPLE)==0) {
            ਸੰਯੁਕਤ_And_Fwd=list[1];
            list+=2;
        } ਨਹੀਂਤਾਂ {
            ਸੰਯੁਕਤ_And_Fwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_ਨਕਾਬ) << 16) | list[2];
            list+=3;
        }
        UChar32 ਸੰਯੁਕਤ=ਸੰਯੁਕਤ_And_Fwd>>1;
        ਜੇ((ਸੰਯੁਕਤ_And_Fwd&1)!=0) {
            ਜੋੜੋ_Composites(get_Compositions_List_For_ਸੰਯੁਕਤ(getRawNorm16(ਸੰਯੁਕਤ)), ਸੈੱਟ);
        }
        ਸੈੱਟ.ਜੋੜੋ(ਸੰਯੁਕਤ);
    } ਜਦੋਂ((ਪਹਿਲਾ_Unit&COMP_1_ਆਖਰੀ_TUPLE)==0);
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
ਖਾਲੀ Normalizer2Impl::recompose(Reordering_ਬਫਰ &ਬਫਰ, int32_t recompose_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ,
                                UBool onlyContiguous) ਅਚਲ {
    char16_t *p=ਬਫਰ.get_ਸ਼ੁਰੂ()+recompose_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ;
    char16_t *ਹੱਦਬੰਦੀ=ਬਫਰ.get_ਹੱਦਬੰਦੀ();
    ਜੇ(p==ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ;
    }

    char16_t *starter, *pRemove, *q, *r;
    ਅਚਲ uint16_t *compositionsList;
    UChar32 c, ਸੰਯੁਕਤ_And_Fwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=ਖਾਲੀ_ਸੂਚਕ;  // used as indicator for whether we have a forward-combining starter
    starter=ਖਾਲੀ_ਸੂਚਕ;
    starterIsSupplementary=ਝੂਠ;
    prevCC=0;

    ਲਈ(;;) {
        UCPTRIE_FAST_U16_ਅਗਲਾ(normTrie, UCPTRIE_16, p, ਹੱਦਬੰਦੀ, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        ਜੇ( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=ਖਾਲੀ_ਸੂਚਕ &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            ਜੇ(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                ਜੇ(c<Hangul::JAMO_T_ਅਧਾਰ) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_ਅਧਾਰ);
                    ਜੇ(prev<Hangul::JAMO_L_ਗਿਣਤੀ) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_ਅਧਾਰ +
                             (prev*Hangul::JAMO_V_ਗਿਣਤੀ+(c-Hangul::JAMO_V_ਅਧਾਰ))*
                             Hangul::JAMO_T_ਗਿਣਤੀ);
                        char16_t t;
                        ਜੇ (p != ਹੱਦਬੰਦੀ && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_ਅਧਾਰ)) < Hangul::JAMO_T_ਗਿਣਤੀ) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        ਜਦੋਂ(r<ਹੱਦਬੰਦੀ) {
                            *q++=*r++;
                        }
                        ਹੱਦਬੰਦੀ=q;
                        p=pRemove;
                    }
                }
                /*
                 * No "else" for Jamo T:
                 * Since the input is in NFD, there are no Hangul LV syllables that
                 * a Jamo T could combine with.
                 * All Jamo Ts are combined above when handling Jamo Vs.
                 */
                ਜੇ(p==ਹੱਦਬੰਦੀ) {
                    ਰੋਕੋ;
                }
                compositionsList=ਖਾਲੀ_ਸੂਚਕ;
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ((ਸੰਯੁਕਤ_And_Fwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 ਸੰਯੁਕਤ=ਸੰਯੁਕਤ_And_Fwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_ਲੰਮਾਈ(c);  // pRemove & p: start & limit of the combining mark
                ਜੇ(starterIsSupplementary) {
                    ਜੇ(U_IS_SUPPLEMENTARY(ਸੰਯੁਕਤ)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(ਸੰਯੁਕਤ);
                        starter[1]=U16_TRAIL(ਸੰਯੁਕਤ);
                    } ਨਹੀਂਤਾਂ {
                        *starter = static_cast<char16_t>(ਸੰਯੁਕਤ);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=ਝੂਠ;
                        q=starter+1;
                        r=q+1;
                        ਜਦੋਂ(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(U_IS_SUPPLEMENTARY(ਸੰਯੁਕਤ)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=ਸੱਚ;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    ਜਦੋਂ(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(ਸੰਯੁਕਤ);
                    *--starter=U16_LEAD(ਸੰਯੁਕਤ);  // undo the temporary increment
                } ਨਹੀਂਤਾਂ {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(ਸੰਯੁਕਤ);
                }

                /* remove the combining mark by moving the following text over it */
                ਜੇ(pRemove<p) {
                    q=pRemove;
                    r=p;
                    ਜਦੋਂ(r<ਹੱਦਬੰਦੀ) {
                        *q++=*r++;
                    }
                    ਹੱਦਬੰਦੀ=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                ਜੇ(p==ਹੱਦਬੰਦੀ) {
                    ਰੋਕੋ;
                }
                // Is the composite a starter that combines forward?
                ਜੇ(ਸੰਯੁਕਤ_And_Fwd&1) {
                    compositionsList=
                        get_Compositions_List_For_ਸੰਯੁਕਤ(getRawNorm16(ਸੰਯੁਕਤ));
                } ਨਹੀਂਤਾਂ {
                    compositionsList=ਖਾਲੀ_ਸੂਚਕ;
                }

                // We combined; continue with looking for compositions.
                ਜਾਰੀ;
            }
        }

        // no combination this time
        prevCC=cc;
        ਜੇ(p==ਹੱਦਬੰਦੀ) {
            ਰੋਕੋ;
        }

        // If c did not combine, then check if it is a starter.
        ਜੇ(cc==0) {
            // Found a new starter.
            ਜੇ((compositionsList=getCompositionsListForDecompYes(norm16))!=ਖਾਲੀ_ਸੂਚਕ) {
                // It may combine with something, prepare for it.
                ਜੇ(U_IS_BMP(c)) {
                    starterIsSupplementary=ਝੂਠ;
                    starter=p-1;
                } ਨਹੀਂਤਾਂ {
                    starterIsSupplementary=ਸੱਚ;
                    starter=p-2;
                }
            }
        } ਨਹੀਂਤਾਂ ਜੇ(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=ਖਾਲੀ_ਸੂਚਕ;
        }
    }
    ਬਫਰ.ਸੈੱਟ_Reordering_ਹੱਦਬੰਦੀ(ਹੱਦਬੰਦੀ);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ਅਚਲ {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ਅਚਲ uint16_t *list;
    ਜੇ(isInert(norm16)) {
        ਵਾਪਸ U_SENTINEL;
    } ਨਹੀਂਤਾਂ ਜੇ(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        ਜੇ(isJamoL(norm16)) {
            ਜੇ (b < Hangul::JAMO_V_ਅਧਾਰ) {
                ਵਾਪਸ U_SENTINEL;
            }
            b-=Hangul::JAMO_V_ਅਧਾਰ;
            ਜੇ(b<Hangul::JAMO_V_ਗਿਣਤੀ) {
                ਵਾਪਸ
                    (Hangul::HANGUL_ਅਧਾਰ+
                     ((a-Hangul::JAMO_L_ਅਧਾਰ)*Hangul::JAMO_V_ਗਿਣਤੀ+b)*
                     Hangul::JAMO_T_ਗਿਣਤੀ);
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ U_SENTINEL;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(isHangulLV(norm16)) {
            ਜੇ (b <= Hangul::JAMO_T_ਅਧਾਰ) {
               ਵਾਪਸ U_SENTINEL;
            }
            b-=Hangul::JAMO_T_ਅਧਾਰ;
            ਜੇ(b<Hangul::JAMO_T_ਗਿਣਤੀ) {  // not b==0!
                ਵਾਪਸ a+b;
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ U_SENTINEL;
            }
        } ਨਹੀਂਤਾਂ {
            // 'a' has a compositions list in extraData
            list=get_ਅੰਕੜੇ_For_Yes_Or_No(norm16);
            ਜੇ(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_ਲੰਮਾਈ_ਨਕਾਬ);  // + mapping length
            }
        }
    } ਨਹੀਂਤਾਂ ਜੇ(norm16<minMaybeNoCombinesFwd || MIN_ਸਾਧਾਰਨ_MAYBE_YES<=norm16) {
        ਵਾਪਸ U_SENTINEL;
    } ਨਹੀਂਤਾਂ {
        list=get_ਅੰਕੜੇ_For_Maybe(norm16);
        ਜੇ(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_ਲੰਮਾਈ_ਨਕਾਬ);  // + mapping length
        }
    }
    ਜੇ(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        ਵਾਪਸ U_SENTINEL;
    }
#ਜੇ U_SIGNED_ਸੱਜਾ_ਖਿਸਕਾਓ_IS_ARITHMETIC
    ਵਾਪਸ combine(list, b)>>1;
#ਨਹੀਂਤਾਂ
    int32_t ਸੰਯੁਕਤ_And_Fwd=combine(list, b);
    ਵਾਪਸ ਸੰਯੁਕਤ_And_Fwd>=0 ? ਸੰਯੁਕਤ_And_Fwd>>1 : U_SENTINEL;
#ਜੇ_ਅੰਤ
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_ਬਫਰ &ਬਫਰ,
                         UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਅਚਲ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ਜੇ(ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ) {
        src=copy_ਨੀਵਾਂ_ਅਗੇਤਰ_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &ਬਫਰ : ਖਾਲੀ_ਸੂਚਕ,
                                           error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        ਹੱਦਬੰਦੀ=u_strchr(src, 0);
        ਜੇ (prevBoundary != src) {
            ਜੇ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ਨਹੀਂਤਾਂ {
                ਬਫਰ.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    ਲਈ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ਅਚਲ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        ਲਈ (;;) {
            ਜੇ (src == ਹੱਦਬੰਦੀ) {
                ਜੇ (prevBoundary != ਹੱਦਬੰਦੀ && doCompose) {
                    ਬਫਰ.appendZeroCC(prevBoundary, ਹੱਦਬੰਦੀ, error_ਕੋਡ);
                }
                ਵਾਪਸ ਸੱਚ;
            }
            ਜੇ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ਨਹੀਂਤਾਂ {
                prevSrc = src++;
                ਜੇ(!U16_IS_LEAD(c)) {
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    char16_t c2;
                    ਜੇ(src!=ਹੱਦਬੰਦੀ && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ਜੇ(!isCompYesAndZeroCC(norm16)) {
                            ਰੋਕੋ;
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
        ਜੇ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ਜੇ (!doCompose) {
                ਵਾਪਸ ਝੂਠ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ਜੇ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ਜੇ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, ਹੱਦਬੰਦੀ)) {
                    ਜੇ (prevBoundary != prevSrc && !ਬਫਰ.appendZeroCC(prevBoundary, prevSrc, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    ਜੇ(!ਬਫਰ.append(mapAlgorithmic(c, norm16), 0, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    prevBoundary = src;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ਜੇ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, ਹੱਦਬੰਦੀ)) {
                    ਜੇ (prevBoundary != prevSrc && !ਬਫਰ.appendZeroCC(prevBoundary, prevSrc, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    ਅਚਲ char16_t *mapping = reinterpret_cast<ਅਚਲ char16_t *>(get_ਅੰਕੜੇ_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_ਲੰਮਾਈ_ਨਕਾਬ;
                    ਜੇ(!ਬਫਰ.appendZeroCC(mapping, mapping + length, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    prevBoundary = src;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (norm16 >= min_No_No_ਖਾਲੀ_ਥਾਵਾਂ) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ਜੇ (hasCompBoundaryBefore(src, ਹੱਦਬੰਦੀ) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ਜੇ (prevBoundary != prevSrc && !ਬਫਰ.appendZeroCC(prevBoundary, prevSrc, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    prevBoundary = src;
                    ਜਾਰੀ;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ਨਹੀਂਤਾਂ ਜੇ (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            ਜੇ(c<Hangul::JAMO_T_ਅਧਾਰ) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_ਅਧਾਰ);
                ਜੇ(l<Hangul::JAMO_L_ਗਿਣਤੀ) {
                    ਜੇ (!doCompose) {
                        ਵਾਪਸ ਝੂਠ;
                    }
                    int32_t t;
                    ਜੇ (src != ਹੱਦਬੰਦੀ &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_ਅਧਾਰ)) &&
                            t < Hangul::JAMO_T_ਗਿਣਤੀ) {
                        // The next character is a Jamo T.
                        ++src;
                    } ਨਹੀਂਤਾਂ ਜੇ (hasCompBoundaryBefore(src, ਹੱਦਬੰਦੀ)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } ਨਹੀਂਤਾਂ {
                        t = -1;
                    }
                    ਜੇ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_ਅਧਾਰ +
                            (l*Hangul::JAMO_V_ਗਿਣਤੀ + (c-Hangul::JAMO_V_ਅਧਾਰ)) *
                            Hangul::JAMO_T_ਗਿਣਤੀ + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        ਜੇ (prevBoundary != prevSrc && !ਬਫਰ.appendZeroCC(prevBoundary, prevSrc, error_ਕੋਡ)) {
                            ਰੋਕੋ;
                        }
                        ਜੇ (!ਬਫਰ.appendBMP(static_cast<char16_t>(syllable), 0, error_ਕੋਡ)) {
                            ਰੋਕੋ;
                        }
                        prevBoundary = src;
                        ਜਾਰੀ;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ਨਹੀਂਤਾਂ ਜੇ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ਜੇ (!doCompose) {
                    ਵਾਪਸ ਝੂਠ;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_ਅਧਾਰ;
                --prevSrc;  // Replace the Hangul LV as well.
                ਜੇ (prevBoundary != prevSrc && !ਬਫਰ.appendZeroCC(prevBoundary, prevSrc, error_ਕੋਡ)) {
                    ਰੋਕੋ;
                }
                ਜੇ (!ਬਫਰ.appendBMP(static_cast<char16_t>(syllable), 0, error_ਕੋਡ)) {
                    ਰੋਕੋ;
                }
                prevBoundary = src;
                ਜਾਰੀ;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ਨਹੀਂਤਾਂ ਜੇ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_ਸਾਧਾਰਨ_Yes_Or_Maybe(norm16);  // cc!=0
            ਜੇ (onlyContiguous /* FCC */ && get_ਪਿਛਲਾ_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ਜੇ (!doCompose) {
                    ਵਾਪਸ ਝੂਠ;
                }
            } ਨਹੀਂਤਾਂ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ਅਚਲ char16_t *ਅਗਲਾ_Src;
                uint16_t n16;
                ਲਈ (;;) {
                    ਜੇ (src == ਹੱਦਬੰਦੀ) {
                        ਜੇ (doCompose) {
                            ਬਫਰ.appendZeroCC(prevBoundary, ਹੱਦਬੰਦੀ, error_ਕੋਡ);
                        }
                        ਵਾਪਸ ਸੱਚ;
                    }
                    uint8_t prevCC = cc;
                    ਅਗਲਾ_Src = src;
                    UCPTRIE_FAST_U16_ਅਗਲਾ(normTrie, UCPTRIE_16, ਅਗਲਾ_Src, ਹੱਦਬੰਦੀ, c, n16);
                    ਜੇ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_ਸਾਧਾਰਨ_Yes_Or_Maybe(n16);
                        ਜੇ (prevCC > cc) {
                            ਜੇ (!doCompose) {
                                ਵਾਪਸ ਝੂਠ;
                            }
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਰੋਕੋ;
                    }
                    src = ਅਗਲਾ_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ਜੇ (norm16HasCompBoundaryBefore(n16)) {
                    ਜੇ (isCompYesAndZeroCC(n16)) {
                        src = ਅਗਲਾ_Src;
                    }
                    ਜਾਰੀ;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ਜੇ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ਅਚਲ char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            ਜੇ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ਜੇ (doCompose && prevBoundary != prevSrc && !ਬਫਰ.appendZeroCC(prevBoundary, prevSrc, error_ਕੋਡ)) {
            ਰੋਕੋ;
        }
        int32_t recompose_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ=ਬਫਰ.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, ਝੂਠ /* !stopAtCompBoundary */, onlyContiguous,
                       ਬਫਰ, error_ਕੋਡ);
        // Decompose until the next boundary.
        src = decomposeShort(src, ਹੱਦਬੰਦੀ, ਸੱਚ /* stopAtCompBoundary */, onlyContiguous,
                             ਬਫਰ, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਰੋਕੋ;
        }
        ਜੇ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ ਸੱਚ;
        }
        recompose(ਬਫਰ, recompose_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ, onlyContiguous);
        ਜੇ(!doCompose) {
            ਜੇ(!ਬਫਰ.equals(prevSrc, src)) {
                ਵਾਪਸ ਝੂਠ;
            }
            ਬਫਰ.remove();
        }
        prevBoundary=src;
    }
    ਵਾਪਸ ਸੱਚ;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ਅਚਲ char16_t *
Normalizer2Impl::compose_ਤੇਜ਼_Check(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                                   UBool onlyContiguous,
                                   UNormalization_Check_ਨਤੀਜਾ *pQCResult) ਅਚਲ {
    ਅਚਲ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ਜੇ(ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ) {
        UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
        src=copy_ਨੀਵਾਂ_ਅਗੇਤਰ_From_Nul_Terminated(src, minNoMaybeCP, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
        ਹੱਦਬੰਦੀ=u_strchr(src, 0);
        ਜੇ (prevBoundary != src) {
            ਜੇ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ਨਹੀਂਤਾਂ {
                prevBoundary = --src;
            }
        }
    }

    ਲਈ(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ਅਚਲ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        ਲਈ (;;) {
            ਜੇ(src==ਹੱਦਬੰਦੀ) {
                ਵਾਪਸ src;
            }
            ਜੇ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ਨਹੀਂਤਾਂ {
                prevSrc = src++;
                ਜੇ(!U16_IS_LEAD(c)) {
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    char16_t c2;
                    ਜੇ(src!=ਹੱਦਬੰਦੀ && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ਜੇ(!isCompYesAndZeroCC(norm16)) {
                            ਰੋਕੋ;
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
        ਜੇ (prevBoundary != prevSrc) {
            ਜੇ (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } ਨਹੀਂਤਾਂ {
                ਅਚਲ char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                ਜੇ (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } ਨਹੀਂਤਾਂ {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        ਜੇ (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            ਜੇ (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } ਨਹੀਂਤਾਂ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ਅਚਲ char16_t *ਅਗਲਾ_Src;
                ਲਈ (;;) {
                    ਜੇ (norm16 < MIN_YES_YES_WITH_CC) {
                        ਜੇ (pQCResult != ਖਾਲੀ_ਸੂਚਕ) {
                            *pQCResult = UNORM_MAYBE;
                        } ਨਹੀਂਤਾਂ {
                            ਵਾਪਸ prevBoundary;
                        }
                    }
                    ਜੇ (src == ਹੱਦਬੰਦੀ) {
                        ਵਾਪਸ src;
                    }
                    uint8_t prevCC = fcd16;
                    ਅਗਲਾ_Src = src;
                    UCPTRIE_FAST_U16_ਅਗਲਾ(normTrie, UCPTRIE_16, ਅਗਲਾ_Src, ਹੱਦਬੰਦੀ, c, norm16);
                    ਜੇ (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        ਜੇ (!(prevCC <= cc || cc == 0)) {
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਰੋਕੋ;
                    }
                    src = ਅਗਲਾ_Src;
                }
                // src is after the last in-order combining mark.
                ਜੇ (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = ਅਗਲਾ_Src;
                    ਜਾਰੀ;
                }
            }
        }
        ਜੇ(pQCResult!=ਖਾਲੀ_ਸੂਚਕ) {
            *pQCResult=UNORM_NO;
        }
        ਵਾਪਸ prevBoundary;
    }
}

ਖਾਲੀ Normalizer2Impl::composeAndAppend(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ,
                                       Reordering_ਬਫਰ &ਬਫਰ,
                                       UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(!ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਅਚਲ char16_t *ਪਹਿਲਾ_Starter_ਵਿੱਚ_Src=ਲੱਭੋ_ਅਗਲਾ_Comp_Boundary(src, ਹੱਦਬੰਦੀ, onlyContiguous);
        ਜੇ(src!=ਪਹਿਲਾ_Starter_ਵਿੱਚ_Src) {
            ਅਚਲ char16_t *ਆਖਰੀ_Starter_ਵਿੱਚ_Dest=ਲੱਭੋ_ਪਿਛਲਾ_Comp_Boundary(ਬਫਰ.get_ਸ਼ੁਰੂ(),
                                                                    ਬਫਰ.get_ਹੱਦਬੰਦੀ(), onlyContiguous);
            int32_t dest_Suffix_ਲੰਮਾਈ = static_cast<int32_t>(ਬਫਰ.get_ਹੱਦਬੰਦੀ() - ਆਖਰੀ_Starter_ਵਿੱਚ_Dest);
            UnicodeString ਵਿਚਕਾਰ(ਆਖਰੀ_Starter_ਵਿੱਚ_Dest, dest_Suffix_ਲੰਮਾਈ);
            ਬਫਰ.removeSuffix(dest_Suffix_ਲੰਮਾਈ);
            ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ=ਵਿਚਕਾਰ;
            ਵਿਚਕਾਰ.append(src, static_cast<int32_t>(ਪਹਿਲਾ_Starter_ਵਿੱਚ_Src - src));
            ਅਚਲ char16_t *ਵਿਚਕਾਰ_ਸ਼ੁਰੂ=ਵਿਚਕਾਰ.get_ਬਫਰ();
            compose(ਵਿਚਕਾਰ_ਸ਼ੁਰੂ, ਵਿਚਕਾਰ_ਸ਼ੁਰੂ+ਵਿਚਕਾਰ.length(), onlyContiguous,
                    ਸੱਚ, ਬਫਰ, error_ਕੋਡ);
            ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
            src=ਪਹਿਲਾ_Starter_ਵਿੱਚ_Src;
        }
    }
    ਜੇ(doCompose) {
        compose(src, ਹੱਦਬੰਦੀ, onlyContiguous, ਸੱਚ, ਬਫਰ, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਜੇ(ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ) {  // appendZeroCC() needs limit!=nullptr
            ਹੱਦਬੰਦੀ=u_strchr(src, 0);
        }
        ਬਫਰ.appendZeroCC(src, ਹੱਦਬੰਦੀ, error_ਕੋਡ);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ਅਚਲ uint8_t *src, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ,
                             ByteSink *sink, Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    U_ASSERT(ਹੱਦਬੰਦੀ != ਖਾਲੀ_ਸੂਚਕ);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ਅਚਲ uint8_t *prevBoundary = src;

    ਲਈ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ਅਚਲ uint8_t *prevSrc;
        uint16_t norm16 = 0;
        ਲਈ (;;) {
            ਜੇ (src == ਹੱਦਬੰਦੀ) {
                ਜੇ (prevBoundary != ਹੱਦਬੰਦੀ && sink != ਖਾਲੀ_ਸੂਚਕ) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, ਹੱਦਬੰਦੀ,
                                                  *sink, options, edits, error_ਕੋਡ);
                }
                ਵਾਪਸ ਸੱਚ;
            }
            ਜੇ (*src < minNoMaybeLead) {
                ++src;
            } ਨਹੀਂਤਾਂ {
                prevSrc = src;
                UCPTRIE_FAST_U8_ਅਗਲਾ(normTrie, UCPTRIE_16, src, ਹੱਦਬੰਦੀ, norm16);
                ਜੇ (!isCompYesAndZeroCC(norm16)) {
                    ਰੋਕੋ;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        ਜੇ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਝੂਠ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ਜੇ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ਜੇ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, ਹੱਦਬੰਦੀ)) {
                    ਜੇ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    append_ਕੋਡ_ਬਿੰਦੂ_Delta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ਜੇ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, ਹੱਦਬੰਦੀ)) {
                    ਜੇ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    ਅਚਲ uint16_t *mapping = get_ਅੰਕੜੇ_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_ਲੰਮਾਈ_ਨਕਾਬ;
                    ਜੇ (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ਅਚਲ char16_t*>(mapping), length,
                                                    *sink, edits, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    prevBoundary = src;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (norm16 >= min_No_No_ਖਾਲੀ_ਥਾਵਾਂ) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ਜੇ (hasCompBoundaryBefore(src, ਹੱਦਬੰਦੀ) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ਜੇ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_ਕੋਡ)) {
                        ਰੋਕੋ;
                    }
                    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                        edits->ਜੋੜੋ_Replace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    ਜਾਰੀ;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ਨਹੀਂਤਾਂ ਜੇ (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = ਪਿਛਲਾ_Hangul_Or_Jamo(prevBoundary, prevSrc);
            ਜੇ (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_ਅਧਾਰ;
                ਜੇ (static_cast<uint32_t>(l) < Hangul::JAMO_L_ਗਿਣਤੀ) {
                    ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) {
                        ਵਾਪਸ ਝੂਠ;
                    }
                    int32_t t = get_Jamo_TMinus_ਅਧਾਰ(src, ਹੱਦਬੰਦੀ);
                    ਜੇ (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } ਨਹੀਂਤਾਂ ਜੇ (hasCompBoundaryBefore(src, ਹੱਦਬੰਦੀ)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    ਜੇ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_ਅਧਾਰ +
                            (l*Hangul::JAMO_V_ਗਿਣਤੀ + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_ਗਿਣਤੀ + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        ਜੇ (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, error_ਕੋਡ)) {
                            ਰੋਕੋ;
                        }
                        ByteSinkUtil::append_ਕੋਡ_ਬਿੰਦੂ(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        ਜਾਰੀ;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ਨਹੀਂਤਾਂ ਜੇ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) {
                    ਵਾਪਸ ਝੂਠ;
                }
                UChar32 syllable = prev + get_Jamo_TMinus_ਅਧਾਰ(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                ਜੇ (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, error_ਕੋਡ)) {
                    ਰੋਕੋ;
                }
                ByteSinkUtil::append_ਕੋਡ_ਬਿੰਦੂ(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                ਜਾਰੀ;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ਨਹੀਂਤਾਂ ਜੇ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_ਸਾਧਾਰਨ_Yes_Or_Maybe(norm16);  // cc!=0
            ਜੇ (onlyContiguous /* FCC */ && get_ਪਿਛਲਾ_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) {
                    ਵਾਪਸ ਝੂਠ;
                }
            } ਨਹੀਂਤਾਂ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ਅਚਲ uint8_t *ਅਗਲਾ_Src;
                uint16_t n16;
                ਲਈ (;;) {
                    ਜੇ (src == ਹੱਦਬੰਦੀ) {
                        ਜੇ (sink != ਖਾਲੀ_ਸੂਚਕ) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, ਹੱਦਬੰਦੀ,
                                                          *sink, options, edits, error_ਕੋਡ);
                        }
                        ਵਾਪਸ ਸੱਚ;
                    }
                    uint8_t prevCC = cc;
                    ਅਗਲਾ_Src = src;
                    UCPTRIE_FAST_U8_ਅਗਲਾ(normTrie, UCPTRIE_16, ਅਗਲਾ_Src, ਹੱਦਬੰਦੀ, n16);
                    ਜੇ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_ਸਾਧਾਰਨ_Yes_Or_Maybe(n16);
                        ਜੇ (prevCC > cc) {
                            ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) {
                                ਵਾਪਸ ਝੂਠ;
                            }
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਰੋਕੋ;
                    }
                    src = ਅਗਲਾ_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ਜੇ (norm16HasCompBoundaryBefore(n16)) {
                    ਜੇ (isCompYesAndZeroCC(n16)) {
                        src = ਅਗਲਾ_Src;
                    }
                    ਜਾਰੀ;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ਜੇ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ਅਚਲ uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            ਜੇ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        Reordering_ਬਫਰ ਬਫਰ(*ਇਹ, s16, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਰੋਕੋ;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_ਹੱਦਬੰਦੀ, onlyContiguous,
                       ਬਫਰ, error_ਕੋਡ);
        // Decompose until the next boundary.
        src = decomposeShort(src, ਹੱਦਬੰਦੀ, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             ਬਫਰ, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਰੋਕੋ;
        }
        ਜੇ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ ਸੱਚ;
        }
        recompose(ਬਫਰ, 0, onlyContiguous);
        ਜੇ (!ਬਫਰ.equals(prevSrc, src)) {
            ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਝੂਠ;
            }
            ਜੇ (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
            ਜੇ (!ByteSinkUtil::appendChange(prevSrc, src, ਬਫਰ.get_ਸ਼ੁਰੂ(), ਬਫਰ.length(),
                                            *sink, edits, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
            prevBoundary = src;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ) ਅਚਲ {
    ਜੇ (src == ਹੱਦਬੰਦੀ || *src < minCompNoMaybeCP) {
        ਵਾਪਸ ਸੱਚ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_ਅਗਲਾ(normTrie, UCPTRIE_16, src, ਹੱਦਬੰਦੀ, c, norm16);
    ਵਾਪਸ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ਅਚਲ uint8_t *src, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ) ਅਚਲ {
    ਜੇ (src == ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ ਸੱਚ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_ਅਗਲਾ(normTrie, UCPTRIE_16, src, ਹੱਦਬੰਦੀ, norm16);
    ਵਾਪਸ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ਅਚਲ char16_t *ਸ਼ੁਰੂ, ਅਚਲ char16_t *p,
                                            UBool onlyContiguous) ਅਚਲ {
    ਜੇ (ਸ਼ੁਰੂ == p) {
        ਵਾਪਸ ਸੱਚ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ਸ਼ੁਰੂ, p, c, norm16);
    ਵਾਪਸ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ਅਚਲ uint8_t *ਸ਼ੁਰੂ, ਅਚਲ uint8_t *p,
                                            UBool onlyContiguous) ਅਚਲ {
    ਜੇ (ਸ਼ੁਰੂ == p) {
        ਵਾਪਸ ਸੱਚ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, ਸ਼ੁਰੂ, p, norm16);
    ਵਾਪਸ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ਅਚਲ char16_t *Normalizer2Impl::ਲੱਭੋ_ਪਿਛਲਾ_Comp_Boundary(ਅਚਲ char16_t *ਸ਼ੁਰੂ, ਅਚਲ char16_t *p,
                                                       UBool onlyContiguous) ਅਚਲ {
    ਜਦੋਂ (p != ਸ਼ੁਰੂ) {
        ਅਚਲ char16_t *ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ਸ਼ੁਰੂ, p, c, norm16);
        ਜੇ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ਵਾਪਸ ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ;
        }
        ਜੇ (hasCompBoundaryBefore(c, norm16)) {
            ਵਾਪਸ p;
        }
    }
    ਵਾਪਸ p;
}

ਅਚਲ char16_t *Normalizer2Impl::ਲੱਭੋ_ਅਗਲਾ_Comp_Boundary(ਅਚਲ char16_t *p, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                                                   UBool onlyContiguous) ਅਚਲ {
    ਜਦੋਂ (p != ਹੱਦਬੰਦੀ) {
        ਅਚਲ char16_t *ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_ਅਗਲਾ(normTrie, UCPTRIE_16, p, ਹੱਦਬੰਦੀ, c, norm16);
        ਜੇ (hasCompBoundaryBefore(c, norm16)) {
            ਵਾਪਸ ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
        }
        ਜੇ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ਵਾਪਸ p;
        }
    }
    ਵਾਪਸ p;
}

uint8_t Normalizer2Impl::get_ਪਿਛਲਾ_Trail_CC(ਅਚਲ char16_t *ਸ਼ੁਰੂ, ਅਚਲ char16_t *p) ਅਚਲ {
    ਜੇ (ਸ਼ੁਰੂ == p) {
        ਵਾਪਸ 0;
    }
    int32_t i = static_cast<int32_t>(p - ਸ਼ੁਰੂ);
    UChar32 c;
    U16_PREV(ਸ਼ੁਰੂ, 0, i, c);
    ਵਾਪਸ static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::get_ਪਿਛਲਾ_Trail_CC(ਅਚਲ uint8_t *ਸ਼ੁਰੂ, ਅਚਲ uint8_t *p) ਅਚਲ {
    ਜੇ (ਸ਼ੁਰੂ == p) {
        ਵਾਪਸ 0;
    }
    int32_t i = static_cast<int32_t>(p - ਸ਼ੁਰੂ);
    UChar32 c;
    U8_PREV(ਸ਼ੁਰੂ, 0, i, c);
    ਵਾਪਸ static_cast<uint8_t>(getFCD16(c));
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
#ਜੇ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ਹਦਾਇਤ optimize( "", off )
#ਜੇ_ਅੰਤ
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_ਅੰਕੜੇ(UChar32 c) ਅਚਲ {
    uint16_t norm16=getNorm16(c);
    ਜੇ (norm16 >= ਹੱਦਬੰਦੀ_No_No) {
        ਜੇ(norm16>=MIN_ਸਾਧਾਰਨ_MAYBE_YES) {
            // combining mark
            norm16=get_CCFrom_ਸਾਧਾਰਨ_Yes_Or_Maybe(norm16);
            ਵਾਪਸ norm16|(norm16<<8);
        } ਨਹੀਂਤਾਂ ਜੇ(norm16>=minMaybeYes) {
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ ਜੇ(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_ਨਕਾਬ;
            ਜੇ (deltaTrailCC <= DELTA_TCCC_1) {
                ਵਾਪਸ deltaTrailCC >> ਵਿੱਥ_ਖਿਸਕਾਓ;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ਜੇ(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        ਵਾਪਸ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ਅਚਲ uint16_t *mapping=get_ਅੰਕੜੇ(norm16);
    uint16_t ਪਹਿਲਾ_Unit=*mapping;
    norm16=ਪਹਿਲਾ_Unit>>8;  // tccc
    ਜੇ(ਪਹਿਲਾ_Unit&MAPPING_HAS_CCC_LCCC_ਸ਼ਬਦ) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    ਵਾਪਸ norm16;
}
#ਜੇ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ਹਦਾਇਤ optimize( "", on )
#ਜੇ_ਅੰਤ

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ਅਚਲ {
    U_ASSERT(norm16 >= minMaybeNo);
    ਜੇ (norm16 >= MIN_ਸਾਧਾਰਨ_MAYBE_YES) {
        // combining mark
        norm16 = get_CCFrom_ਸਾਧਾਰਨ_Yes_Or_Maybe(norm16);
        ਵਾਪਸ norm16 | (norm16<<8);
    } ਨਹੀਂਤਾਂ ਜੇ (norm16 >= minMaybeYes) {
        ਵਾਪਸ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ਅਚਲ uint16_t *mapping = get_ਅੰਕੜੇ_For_Maybe(norm16);
    uint16_t ਪਹਿਲਾ_Unit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((ਪਹਿਲਾ_Unit & MAPPING_HAS_CCC_LCCC_ਸ਼ਬਦ) == 0 || (*(mapping - 1) & 0xff00) == 0);
    ਵਾਪਸ ਪਹਿਲਾ_Unit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ਅਚਲ char16_t *
Normalizer2Impl::makeFCD(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                         Reordering_ਬਫਰ *ਬਫਰ,
                         UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ਅਚਲ char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    ਜੇ(ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ) {
        src=copy_ਨੀਵਾਂ_ਅਗੇਤਰ_From_Nul_Terminated(src, minLcccCP, ਬਫਰ, error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ src;
        }
        ਜੇ(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            ਜੇ(prevFCD16>1) {
                --prevBoundary;
            }
        }
        ਹੱਦਬੰਦੀ=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    ਅਚਲ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    ਲਈ(;;) {
        // count code units with lccc==0
        ਲਈ(prevSrc=src; src!=ਹੱਦਬੰਦੀ;) {
            ਜੇ((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } ਨਹੀਂਤਾਂ ਜੇ(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } ਨਹੀਂਤਾਂ {
                ਜੇ(U16_IS_LEAD(c)) {
                    char16_t c2;
                    ਜੇ((src+1)!=ਹੱਦਬੰਦੀ && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                ਜੇ((fcd16=get_FCD16_From_Norm_ਅੰਕੜੇ(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_ਲੰਮਾਈ(c);
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
            }
        }
        // copy these code units all at once
        ਜੇ(src!=prevSrc) {
            ਜੇ(ਬਫਰ!=ਖਾਲੀ_ਸੂਚਕ && !ਬਫਰ->appendZeroCC(prevSrc, src, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
            ਜੇ(src==ਹੱਦਬੰਦੀ) {
                ਰੋਕੋ;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            ਜੇ(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                ਜੇ(prev<minDecompNoCP) {
                    prevFCD16=0;
                } ਨਹੀਂਤਾਂ {
                    prevFCD16=get_FCD16_From_Norm_ਅੰਕੜੇ(prev);
                    ਜੇ(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } ਨਹੀਂਤਾਂ {
                ਅਚਲ char16_t *p=src-1;
                ਜੇ(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=get_FCD16_From_Norm_ਅੰਕੜੇ(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                ਜੇ(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } ਨਹੀਂਤਾਂ ਜੇ(src==ਹੱਦਬੰਦੀ) {
            ਰੋਕੋ;
        }

        src+=U16_ਲੰਮਾਈ(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        ਜੇ((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            ਜੇ((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            ਜੇ(ਬਫਰ!=ਖਾਲੀ_ਸੂਚਕ && !ਬਫਰ->appendZeroCC(c, error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
            prevFCD16=fcd16;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(ਬਫਰ==ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ prevBoundary;  // quick check "no"
        } ਨਹੀਂਤਾਂ {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            ਬਫਰ->removeSuffix(static_cast<int32_t>(prevSrc - prevBoundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=ਲੱਭੋ_ਅਗਲਾ_FCDBoundary(src, ਹੱਦਬੰਦੀ);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(prevBoundary, src, ਝੂਠ, ਝੂਠ, *ਬਫਰ, error_ਕੋਡ);
            ਜੇ (U_FAILURE(error_ਕੋਡ)) {
                ਰੋਕੋ;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    ਵਾਪਸ src;
}

ਖਾਲੀ Normalizer2Impl::makeFCDAndAppend(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
                                       UBool doMakeFCD,
                                       UnicodeString &ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ,
                                       Reordering_ਬਫਰ &ਬਫਰ,
                                       UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(!ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਅਚਲ char16_t *ਪਹਿਲਾ_Boundary_ਵਿੱਚ_Src=ਲੱਭੋ_ਅਗਲਾ_FCDBoundary(src, ਹੱਦਬੰਦੀ);
        ਜੇ(src!=ਪਹਿਲਾ_Boundary_ਵਿੱਚ_Src) {
            ਅਚਲ char16_t *ਆਖਰੀ_Boundary_ਵਿੱਚ_Dest=ਲੱਭੋ_ਪਿਛਲਾ_FCDBoundary(ਬਫਰ.get_ਸ਼ੁਰੂ(),
                                                                    ਬਫਰ.get_ਹੱਦਬੰਦੀ());
            int32_t dest_Suffix_ਲੰਮਾਈ = static_cast<int32_t>(ਬਫਰ.get_ਹੱਦਬੰਦੀ() - ਆਖਰੀ_Boundary_ਵਿੱਚ_Dest);
            UnicodeString ਵਿਚਕਾਰ(ਆਖਰੀ_Boundary_ਵਿੱਚ_Dest, dest_Suffix_ਲੰਮਾਈ);
            ਬਫਰ.removeSuffix(dest_Suffix_ਲੰਮਾਈ);
            ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ=ਵਿਚਕਾਰ;
            ਵਿਚਕਾਰ.append(src, static_cast<int32_t>(ਪਹਿਲਾ_Boundary_ਵਿੱਚ_Src - src));
            ਅਚਲ char16_t *ਵਿਚਕਾਰ_ਸ਼ੁਰੂ=ਵਿਚਕਾਰ.get_ਬਫਰ();
            makeFCD(ਵਿਚਕਾਰ_ਸ਼ੁਰੂ, ਵਿਚਕਾਰ_ਸ਼ੁਰੂ+ਵਿਚਕਾਰ.length(), &ਬਫਰ, error_ਕੋਡ);
            ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
            src=ਪਹਿਲਾ_Boundary_ਵਿੱਚ_Src;
        }
    }
    ਜੇ(doMakeFCD) {
        makeFCD(src, ਹੱਦਬੰਦੀ, &ਬਫਰ, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਜੇ(ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ) {  // appendZeroCC() needs limit!=nullptr
            ਹੱਦਬੰਦੀ=u_strchr(src, 0);
        }
        ਬਫਰ.appendZeroCC(src, ਹੱਦਬੰਦੀ, error_ਕੋਡ);
    }
}

ਅਚਲ char16_t *Normalizer2Impl::ਲੱਭੋ_ਪਿਛਲਾ_FCDBoundary(ਅਚਲ char16_t *ਸ਼ੁਰੂ, ਅਚਲ char16_t *p) ਅਚਲ {
    ਜਦੋਂ(ਸ਼ੁਰੂ<p) {
        ਅਚਲ char16_t *ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ਸ਼ੁਰੂ, p, c, norm16);
        ਜੇ (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            ਵਾਪਸ ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ;
        }
        ਜੇ (norm16HasDecompBoundaryBefore(norm16)) {
            ਵਾਪਸ p;
        }
    }
    ਵਾਪਸ p;
}

ਅਚਲ char16_t *Normalizer2Impl::ਲੱਭੋ_ਅਗਲਾ_FCDBoundary(ਅਚਲ char16_t *p, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ) ਅਚਲ {
    ਜਦੋਂ(p<ਹੱਦਬੰਦੀ) {
        ਅਚਲ char16_t *ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_ਅਗਲਾ(normTrie, UCPTRIE_16, p, ਹੱਦਬੰਦੀ, c, norm16);
        ਜੇ (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            ਵਾਪਸ ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
        }
        ਜੇ (norm16HasDecompBoundaryAfter(norm16)) {
            ਵਾਪਸ p;
        }
    }
    ਵਾਪਸ p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_ਅੰਕੜੇ::Canon_Iter_ਅੰਕੜੇ(UError_ਕੋਡ &error_ਕੋਡ) :
        mutableTrie(umutablecptrie_open(0, 0, &error_ਕੋਡ)), trie(ਖਾਲੀ_ਸੂਚਕ),
        canon_ਸ਼ੁਰੂ_Sets(uprv_deleteUObject, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ) {}

Canon_Iter_ਅੰਕੜੇ::~Canon_Iter_ਅੰਕੜੇ() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

ਖਾਲੀ Canon_Iter_ਅੰਕੜੇ::ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_ਸੈੱਟ(UChar32 origin, UChar32 decompLead, UError_ਕੋਡ &error_ਕੋਡ) {
    uint32_t canon_ਮੁੱਲ = umutablecptrie_get(mutableTrie, decompLead);
    ਜੇ((canon_ਮੁੱਲ&(CANON_HAS_ਸੈੱਟ|CANON_ਮੁੱਲ_ਨਕਾਬ))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_ਸੈੱਟ(mutableTrie, decompLead, canon_ਮੁੱਲ|origin, &error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        // origin is not the first character, or it is U+0000.
        Unicode_ਸੈੱਟ *ਸੈੱਟ;
        ਜੇ((canon_ਮੁੱਲ&CANON_HAS_ਸੈੱਟ)==0) {
            Local_ਸੂਚਕ<Unicode_ਸੈੱਟ> lp_ਸੈੱਟ(ਨਵਾਂ Unicode_ਸੈੱਟ, error_ਕੋਡ);
            ਸੈੱਟ=lp_ਸੈੱਟ.getAlias();
            ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
            UChar32 ਪਹਿਲਾ_Origin = static_cast<UChar32>(canon_ਮੁੱਲ & CANON_ਮੁੱਲ_ਨਕਾਬ);
            canon_ਮੁੱਲ = (canon_ਮੁੱਲ & ~CANON_ਮੁੱਲ_ਨਕਾਬ) | CANON_HAS_ਸੈੱਟ | static_cast<uint32_t>(canon_ਸ਼ੁਰੂ_Sets.size());
            umutablecptrie_ਸੈੱਟ(mutableTrie, decompLead, canon_ਮੁੱਲ, &error_ਕੋਡ);
            canon_ਸ਼ੁਰੂ_Sets.adoptElement(lp_ਸੈੱਟ.orphan(), error_ਕੋਡ);
            ਜੇ (U_FAILURE(error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
            ਜੇ(ਪਹਿਲਾ_Origin!=0) {
                ਸੈੱਟ->ਜੋੜੋ(ਪਹਿਲਾ_Origin);
            }
        } ਨਹੀਂਤਾਂ {
            ਸੈੱਟ = static_cast<Unicode_ਸੈੱਟ*>(canon_ਸ਼ੁਰੂ_Sets[static_cast<int32_t>(canon_ਮੁੱਲ & CANON_ਮੁੱਲ_ਨਕਾਬ)]);
        }
        ਸੈੱਟ->ਜੋੜੋ(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
ਵਰਗ Init_Canon_Iter_ਅੰਕੜੇ {
ਜਨਤਕ:
    ਸਥਿਰ ਖਾਲੀ doInit(Normalizer2Impl *impl, UError_ਕੋਡ &error_ਕੋਡ);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
init_Canon_Iter_ਅੰਕੜੇ(Normalizer2Impl *impl, UError_ਕੋਡ &error_ਕੋਡ) {
    Init_Canon_Iter_ਅੰਕੜੇ::doInit(impl, error_ਕੋਡ);
}

U_CDECL_ਸਿਰਾ

ਖਾਲੀ Init_Canon_Iter_ਅੰਕੜੇ::doInit(Normalizer2Impl *impl, UError_ਕੋਡ &error_ਕੋਡ) {
    U_ASSERT(impl->f_Canon_Iter_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ);
    impl->f_Canon_Iter_ਅੰਕੜੇ = ਨਵਾਂ Canon_Iter_ਅੰਕੜੇ(error_ਕੋਡ);
    ਜੇ (impl->f_Canon_Iter_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        UChar32 ਸ਼ੁਰੂ = 0, ਸਿਰਾ;
        uint32_t value;
        ਜਦੋਂ ((ਸਿਰਾ = ucptrie_getRange(impl->normTrie, ਸ਼ੁਰੂ,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            ਜੇ (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_ਅੰਕੜੇ_From_Norm16(ਸ਼ੁਰੂ, ਸਿਰਾ, value, *impl->f_Canon_Iter_ਅੰਕੜੇ, error_ਕੋਡ);
            }
            ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
        umutablecptrie_ਸੈੱਟ_Name(impl->f_Canon_Iter_ਅੰਕੜੇ->mutableTrie, "CanonIterData");
#ਜੇ_ਅੰਤ
        impl->f_Canon_Iter_ਅੰਕੜੇ->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_ਅੰਕੜੇ->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32, &error_ਕੋਡ);
        umutablecptrie_close(impl->f_Canon_Iter_ਅੰਕੜੇ->mutableTrie);
        impl->f_Canon_Iter_ਅੰਕੜੇ->mutableTrie = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਮਿਟਾਓ impl->f_Canon_Iter_ਅੰਕੜੇ;
        impl->f_Canon_Iter_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    }
}

ਖਾਲੀ Normalizer2Impl::make_Canon_Iter_ਅੰਕੜੇ_From_Norm16(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ, ਅਚਲ uint16_t norm16,
                                                  Canon_Iter_ਅੰਕੜੇ &new_ਅੰਕੜੇ,
                                                  UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        ਵਾਪਸ;
    }
    ਲਈ(UChar32 c=ਸ਼ੁਰੂ; c<=ਸਿਰਾ; ++c) {
        uint32_t old_ਮੁੱਲ = umutablecptrie_get(new_ਅੰਕੜੇ.mutableTrie, c);
        uint32_t new_ਮੁੱਲ=old_ਮੁੱਲ;
        ਜੇ(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_ਮੁੱਲ|=CANON_NOT_SEGMENT_STARTER;
            ਜੇ(norm16<MIN_ਸਾਧਾਰਨ_MAYBE_YES) {
                new_ਮੁੱਲ|=CANON_HAS_COMPOSITIONS;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(norm16<minYesNo) {
            new_ਮੁੱਲ|=CANON_HAS_COMPOSITIONS;
        } ਨਹੀਂਤਾਂ {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            ਜੇ (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            ਜੇ (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ਅਚਲ uint16_t *mapping=get_ਅੰਕੜੇ_For_Yes_Or_No(norm16_2);
                uint16_t ਪਹਿਲਾ_Unit=*mapping;
                int32_t length=ਪਹਿਲਾ_Unit&MAPPING_ਲੰਮਾਈ_ਨਕਾਬ;
                ਜੇ((ਪਹਿਲਾ_Unit&MAPPING_HAS_CCC_LCCC_ਸ਼ਬਦ)!=0) {
                    ਜੇ(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_ਮੁੱਲ|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                ਜੇ(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_ਅਗਲਾ_UNSAFE(mapping, i, c2);
                    new_ਅੰਕੜੇ.ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_ਸੈੱਟ(c, c2, error_ਕੋਡ);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    ਜੇ(norm16_2>=minNoNo) {
                        ਜਦੋਂ(i<length) {
                            U16_ਅਗਲਾ_UNSAFE(mapping, i, c2);
                            uint32_t c2_ਮੁੱਲ = umutablecptrie_get(new_ਅੰਕੜੇ.mutableTrie, c2);
                            ਜੇ((c2_ਮੁੱਲ&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_ਸੈੱਟ(new_ਅੰਕੜੇ.mutableTrie, c2,
                                                   c2_ਮੁੱਲ|CANON_NOT_SEGMENT_STARTER, &error_ਕੋਡ);
                            }
                        }
                    }
                }
            } ਨਹੀਂਤਾਂ {
                // c decomposed to c2 algorithmically; c has cc==0
                new_ਅੰਕੜੇ.ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_ਸੈੱਟ(c, c2, error_ਕੋਡ);
            }
        }
        ਜੇ(new_ਮੁੱਲ!=old_ਮੁੱਲ) {
            umutablecptrie_ਸੈੱਟ(new_ਅੰਕੜੇ.mutableTrie, c, new_ਮੁੱਲ, &error_ਕੋਡ);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_ਅੰਕੜੇ(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(ਇਹ);
    umtx_initOnce(me->f_Canon_Iter_ਅੰਕੜੇ_Init_Once, &init_Canon_Iter_ਅੰਕੜੇ, me, error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ);
}

int32_t Normalizer2Impl::get_Canon_ਮੁੱਲ(UChar32 c) ਅਚਲ {
    ਵਾਪਸ static_cast<int32_t>(ucptrie_get(f_Canon_Iter_ਅੰਕੜੇ->trie, c));
}

ਅਚਲ Unicode_ਸੈੱਟ &Normalizer2Impl::get_Canon_ਸ਼ੁਰੂ_ਸੈੱਟ(int32_t n) ਅਚਲ {
    ਵਾਪਸ *static_cast<ਅਚਲ Unicode_ਸੈੱਟ*>(f_Canon_Iter_ਅੰਕੜੇ->canon_ਸ਼ੁਰੂ_Sets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ਅਚਲ {
    ਵਾਪਸ get_Canon_ਮੁੱਲ(c)>=0;
}

UBool Normalizer2Impl::get_Canon_ਸ਼ੁਰੂ_ਸੈੱਟ(UChar32 c, Unicode_ਸੈੱਟ &ਸੈੱਟ) ਅਚਲ {
    int32_t canon_ਮੁੱਲ=get_Canon_ਮੁੱਲ(c)&~CANON_NOT_SEGMENT_STARTER;
    ਜੇ(canon_ਮੁੱਲ==0) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਸੈੱਟ.clear();
    int32_t value=canon_ਮੁੱਲ&CANON_ਮੁੱਲ_ਨਕਾਬ;
    ਜੇ((canon_ਮੁੱਲ&CANON_HAS_ਸੈੱਟ)!=0) {
        ਸੈੱਟ.ਜੋੜੋ_All(get_Canon_ਸ਼ੁਰੂ_ਸੈੱਟ(value));
    } ਨਹੀਂਤਾਂ ਜੇ(value!=0) {
        ਸੈੱਟ.ਜੋੜੋ(value);
    }
    ਜੇ((canon_ਮੁੱਲ&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        ਜੇ(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_ਅਧਾਰ + (c - Hangul::JAMO_L_ਅਧਾਰ) * Hangul::JAMO_VT_ਗਿਣਤੀ);
            ਸੈੱਟ.ਜੋੜੋ(syllable, syllable+Hangul::JAMO_VT_ਗਿਣਤੀ-1);
        } ਨਹੀਂਤਾਂ {
            ਜੋੜੋ_Composites(getCompositionsList(norm16), ਸੈੱਟ);
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

U_NAMESPACE_ਸਿਰਾ

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
            ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UDataInfo *pInfo;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ;

    ਅਚਲ uint8_t *ਵਿੱਚ_ਬਾਈਟਾਂ;
    uint8_t *ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    ਅਚਲ int32_t *ਵਿੱਚ_Indexes;
    int32_t indexes[Normalizer2Impl::IX_ਕੁੱਲ_ਮਾਪ+1];

    int32_t i, ਵਿੱਥ, ਅਗਲਾ_ਵਿੱਥ, size;

    /* udata_swapDataHeader checks the arguments */
    ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* check data format and format version */
    pInfo=(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    uint8_t formatVersion0=pInfo->format_ਸੰਸਕਰਣ[0];
    ਜੇ(!(
        pInfo->ਅੰਕੜੇ_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x72 &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x6d &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                         pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3],
                         pInfo->format_ਸੰਸਕਰਣ[0]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_ਬਾਈਟਾਂ=(ਅਚਲ uint8_t *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    ਬਾਹਰਲਾ_ਬਾਈਟਾਂ=(ਬਾਹਰਲਾ_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) ? ਖਾਲੀ_ਸੂਚਕ : (uint8_t *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;

    ਵਿੱਚ_Indexes=(ਅਚਲ int32_t *)ਵਿੱਚ_ਬਾਈਟਾਂ;
    int32_t min_Indexes_ਲੰਮਾਈ;
    ਜੇ(formatVersion0==1) {
        min_Indexes_ਲੰਮਾਈ=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } ਨਹੀਂਤਾਂ ਜੇ(formatVersion0==2) {
        min_Indexes_ਲੰਮਾਈ=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } ਨਹੀਂਤਾਂ ਜੇ(formatVersion0<=4) {
        min_Indexes_ਲੰਮਾਈ=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } ਨਹੀਂਤਾਂ {
        min_Indexes_ਲੰਮਾਈ=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    ਜੇ(length>=0) {
        length-=ਸਿਰਲੇਖ_ਮਾਪ;
        ਜੇ(length<min_Indexes_ਲੰਮਾਈ*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
    }

    /* read the first few indexes */
    ਲਈ(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, ਵਿੱਚ_Indexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_ਕੁੱਲ_ਮਾਪ];

    ਜੇ(length>=0) {
        ਜੇ(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }

        /* copy the data for inaccessible bytes */
        ਜੇ(ਵਿੱਚ_ਬਾਈਟਾਂ!=ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
            uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, size);
        }

        ਵਿੱਥ=0;

        /* swap the int32_t indexes[] */
        ਅਗਲਾ_ਵਿੱਥ=indexes[Normalizer2Impl::IX_NORM_TRIE_ਵਿੱਥ];
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ, ਅਗਲਾ_ਵਿੱਥ-ਵਿੱਥ, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, p_Error_ਕੋਡ);
        ਵਿੱਥ=ਅਗਲਾ_ਵਿੱਥ;

        /* swap the trie */
        ਅਗਲਾ_ਵਿੱਥ=indexes[Normalizer2Impl::IX_EXTRA_ਅੰਕੜੇ_ਵਿੱਥ];
        utrie_ਅਦਲਾ_ਬਦਲੀ_Any_ਸੰਸਕਰਣ(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, ਅਗਲਾ_ਵਿੱਥ-ਵਿੱਥ, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
        ਵਿੱਥ=ਅਗਲਾ_ਵਿੱਥ;

        /* swap the uint16_t extraData[] */
        ਅਗਲਾ_ਵਿੱਥ=indexes[Normalizer2Impl::IX_SMALL_FCD_ਵਿੱਥ];
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, ਅਗਲਾ_ਵਿੱਥ-ਵਿੱਥ, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
        ਵਿੱਥ=ਅਗਲਾ_ਵਿੱਥ;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        ਅਗਲਾ_ਵਿੱਥ=indexes[Normalizer2Impl::IX_SMALL_FCD_ਵਿੱਥ+1];
        ਵਿੱਥ=ਅਗਲਾ_ਵਿੱਥ;

        U_ASSERT(ਵਿੱਥ==size);
    }

    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ+size;
}

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_NORMALIZATION
