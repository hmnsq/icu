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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/bytestream.h"
#شامل "unicode/edits.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/stringoptions.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/udata.h"
#شامل "unicode/umutablecptrie.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "unicode/utf8.h"
#شامل "bytesinkutil.h"
#شامل "cmemory.h"
#شامل "mutex.h"
#شامل "normalizer2impl.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل "ucptrie_impl.h"
#شامل "uset_imp.h"
#شامل "uvector.h"

U_NAMESPACE_BEGIN

نامخلا {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
اندرونی uint8_t leadByteForCP(UChar32 c) {
    اگر (c <= 0x7f) {
        واپس static_cast<uint8_t>(c);
    } ورنہ اگر (c <= 0x7ff) {
        واپس static_cast<uint8_t>(0xc0 + (c >> 6));
    } ورنہ {
        // Should not occur because ccc(U+0300)!=0.
        واپس 0xe0;
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
UChar32 رمز_نقطہ_From_Valid_UTF8(ثابت uint8_t *cp_آغاز, ثابت uint8_t *cp_حد_بندی) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cp_آغاز < cp_حد_بندی);
    uint8_t c = *cp_آغاز;
    انتخاب(cp_حد_بندی-cp_آغاز) {
    حالت 1:
        واپس c;
    حالت 2:
        واپس ((c&0x1f)<<6) | (cp_آغاز[1]&0x3f);
    حالت 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        واپس static_cast<char16_t>((c << 12) | ((cp_آغاز[1] & 0x3f) << 6) | (cp_آغاز[2] & 0x3f));
    حالت 4:
        واپس ((c&7)<<18) | ((cp_آغاز[1]&0x3f)<<12) | ((cp_آغاز[2]&0x3f)<<6) | (cp_آغاز[3]&0x3f);
    طےشدہ:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 پچھلا_Hangul_Or_Jamo(ثابت uint8_t *آغاز, ثابت uint8_t *p) {
    اگر ((p - آغاز) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        اگر (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            واپس ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    واپس U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t get_Jamo_TMinus_بنیاد(ثابت uint8_t *src, ثابت uint8_t *حد_بندی) {
    // Jamo T: E1 86 A8..E1 87 82
    اگر ((حد_بندی - src) >= 3 && *src == 0xe1) {
        اگر (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            اگر (0xa8 <= t && t <= 0xbf) {
                واپس t - 0xa7;
            }
        } ورنہ اگر (src[1] == 0x87) {
            uint8_t t = src[2];
            اگر (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                واپس t - (0xa7 - 0x40);
            }
        }
    }
    واپس -1;
}

خالی
append_رمز_نقطہ_Delta(ثابت uint8_t *cp_آغاز, ثابت uint8_t *cp_حد_بندی, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    حرف بفر[U8_MAX_لمبائی];
    int32_t length;
    int32_t cp_لمبائی = static_cast<int32_t>(cp_حد_بندی - cp_آغاز);
    اگر (cp_لمبائی == 1) {
        // The builder makes ASCII map to ASCII.
        بفر[0] = static_cast<uint8_t>(*cp_آغاز + delta);
        length = 1;
    } ورنہ {
        int32_t trail = *(cp_حد_بندی-1) + delta;
        اگر (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cp_حد_بندی;
            length = 0;
            کرو { بفر[length++] = *cp_آغاز++; } جب (cp_آغاز < cp_حد_بندی);
            بفر[length++] = static_cast<uint8_t>(trail);
        } ورنہ {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = رمز_نقطہ_From_Valid_UTF8(cp_آغاز, cp_حد_بندی) + delta;
            length = 0;
            U8_APPEND_UNSAFE(بفر, length, c);
        }
    }
    اگر (edits != خالی_اشاریہ) {
        edits->جوڑو_Replace(cp_لمبائی, length);
    }
    sink.Append(بفر, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_بفر::Reordering_بفر(ثابت Normalizer2Impl &ni, UnicodeString &dest,
                                   UError_رمز &error_رمز) :
        impl(ni), str(dest),
        آغاز(str.get_بفر(8)), reorder_آغاز(آغاز), حد_بندی(آغاز),
        بقایا_استعداد(str.get_استعداد()), آخری_CC(0) {
    اگر (آغاز == خالی_اشاریہ && U_SUCCESS(error_رمز)) {
        // getBuffer() already did str.setToBogus()
        error_رمز = U_حافظہ_تخصیص_ERROR;
    }
}

UBool Reordering_بفر::init(int32_t dest_استعداد, UError_رمز &error_رمز) {
    int32_t length=str.length();
    آغاز=str.get_بفر(dest_استعداد);
    اگر(آغاز==خالی_اشاریہ) {
        // getBuffer() already did str.setToBogus()
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس جھوٹ;
    }
    حد_بندی=آغاز+length;
    بقایا_استعداد=str.get_استعداد()-length;
    reorder_آغاز=آغاز;
    اگر(آغاز==حد_بندی) {
        آخری_CC=0;
    } ورنہ {
        سیٹ_Iterator();
        آخری_CC=پچھلا_CC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        اگر(آخری_CC>1) {
            جب(پچھلا_CC()>1) {}
        }
        reorder_آغاز=رمز_نقطہ_حد_بندی;
    }
    واپس سچ;
}

UBool Reordering_بفر::equals(ثابت char16_t *other_آغاز, ثابت char16_t *other_حد_بندی) ثابت {
    int32_t length = static_cast<int32_t>(حد_بندی - آغاز);
    واپس
        length == static_cast<int32_t>(other_حد_بندی - other_آغاز) &&
        0==u_memcmp(آغاز, other_آغاز, length);
}

UBool Reordering_بفر::equals(ثابت uint8_t *other_آغاز, ثابت uint8_t *other_حد_بندی) ثابت {
    U_ASSERT((other_حد_بندی - other_آغاز) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(حد_بندی - آغاز);
    int32_t other_لمبائی = static_cast<int32_t>(other_حد_بندی - other_آغاز);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    اگر (other_لمبائی < length || (other_لمبائی / 3) > length) {
        واپس جھوٹ;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    برائے (int32_t i = 0, j = 0;;) {
        اگر (i >= length) {
            واپس j >= other_لمبائی;
        } ورنہ اگر (j >= other_لمبائی) {
            واپس جھوٹ;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_اگلا_UNSAFE(آغاز, i, c);
        U8_اگلا_UNSAFE(other_آغاز, j, other);
        اگر (c != other) {
            واپس جھوٹ;
        }
    }
}

UBool Reordering_بفر::appendSupplementary(UChar32 c, uint8_t cc, UError_رمز &error_رمز) {
    اگر(بقایا_استعداد<2 && !resize(2, error_رمز)) {
        واپس جھوٹ;
    }
    اگر(آخری_CC<=cc || cc==0) {
        حد_بندی[0]=U16_LEAD(c);
        حد_بندی[1]=U16_TRAIL(c);
        حد_بندی+=2;
        آخری_CC=cc;
        اگر(cc<=1) {
            reorder_آغاز=حد_بندی;
        }
    } ورنہ {
        insert(c, cc);
    }
    بقایا_استعداد-=2;
    واپس سچ;
}

UBool Reordering_بفر::append(ثابت char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UError_رمز &error_رمز) {
    اگر(length==0) {
        واپس سچ;
    }
    اگر(بقایا_استعداد<length && !resize(length, error_رمز)) {
        واپس جھوٹ;
    }
    بقایا_استعداد-=length;
    اگر(آخری_CC<=leadCC || leadCC==0) {
        اگر(trailCC<=1) {
            reorder_آغاز=حد_بندی+length;
        } ورنہ اگر(leadCC<=1) {
            reorder_آغاز=حد_بندی+1;  // Ok if not a code point boundary.
        }
        ثابت char16_t *s_حد_بندی=s+length;
        کرو { *حد_بندی++=*s++; } جب(s!=s_حد_بندی);
        آخری_CC=trailCC;
    } ورنہ {
        int32_t i=0;
        UChar32 c;
        U16_اگلا(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        جب(i<length) {
            U16_اگلا(s, i, length, c);
            اگر(i<length) {
                اگر (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } ورنہ {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } ورنہ {
                leadCC=trailCC;
            }
            append(c, leadCC, error_رمز);
        }
    }
    واپس سچ;
}

UBool Reordering_بفر::appendZeroCC(UChar32 c, UError_رمز &error_رمز) {
    int32_t cp_لمبائی=U16_لمبائی(c);
    اگر(بقایا_استعداد<cp_لمبائی && !resize(cp_لمبائی, error_رمز)) {
        واپس جھوٹ;
    }
    بقایا_استعداد-=cp_لمبائی;
    اگر(cp_لمبائی==1) {
        *حد_بندی++ = static_cast<char16_t>(c);
    } ورنہ {
        حد_بندی[0]=U16_LEAD(c);
        حد_بندی[1]=U16_TRAIL(c);
        حد_بندی+=2;
    }
    آخری_CC=0;
    reorder_آغاز=حد_بندی;
    واپس سچ;
}

UBool Reordering_بفر::appendZeroCC(ثابت char16_t *s, ثابت char16_t *s_حد_بندی, UError_رمز &error_رمز) {
    اگر(s==s_حد_بندی) {
        واپس سچ;
    }
    int32_t length = static_cast<int32_t>(s_حد_بندی - s);
    اگر(بقایا_استعداد<length && !resize(length, error_رمز)) {
        واپس جھوٹ;
    }
    u_memcpy(حد_بندی, s, length);
    حد_بندی+=length;
    بقایا_استعداد-=length;
    آخری_CC=0;
    reorder_آغاز=حد_بندی;
    واپس سچ;
}

خالی Reordering_بفر::remove() {
    reorder_آغاز=حد_بندی=آغاز;
    بقایا_استعداد=str.get_استعداد();
    آخری_CC=0;
}

خالی Reordering_بفر::removeSuffix(int32_t suffix_لمبائی) {
    اگر(suffix_لمبائی<(حد_بندی-آغاز)) {
        حد_بندی-=suffix_لمبائی;
        بقایا_استعداد+=suffix_لمبائی;
    } ورنہ {
        حد_بندی=آغاز;
        بقایا_استعداد=str.get_استعداد();
    }
    آخری_CC=0;
    reorder_آغاز=حد_بندی;
}

UBool Reordering_بفر::resize(int32_t append_لمبائی, UError_رمز &error_رمز) {
    int32_t reorder_آغاز_اشاریہ = static_cast<int32_t>(reorder_آغاز - آغاز);
    int32_t length = static_cast<int32_t>(حد_بندی - آغاز);
    str.release_بفر(length);
    int32_t new_استعداد=length+append_لمبائی;
    int32_t double_استعداد=2*str.get_استعداد();
    اگر(new_استعداد<double_استعداد) {
        new_استعداد=double_استعداد;
    }
    اگر(new_استعداد<256) {
        new_استعداد=256;
    }
    آغاز=str.get_بفر(new_استعداد);
    اگر(آغاز==خالی_اشاریہ) {
        // getBuffer() already did str.setToBogus()
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس جھوٹ;
    }
    reorder_آغاز=آغاز+reorder_آغاز_اشاریہ;
    حد_بندی=آغاز+length;
    بقایا_استعداد=str.get_استعداد()-length;
    واپس سچ;
}

خالی Reordering_بفر::skip_پچھلا() {
    رمز_نقطہ_حد_بندی=رمز_نقطہ_آغاز;
    char16_t c=*--رمز_نقطہ_آغاز;
    اگر(U16_IS_TRAIL(c) && آغاز<رمز_نقطہ_آغاز && U16_IS_LEAD(*(رمز_نقطہ_آغاز-1))) {
        --رمز_نقطہ_آغاز;
    }
}

uint8_t Reordering_بفر::پچھلا_CC() {
    رمز_نقطہ_حد_بندی=رمز_نقطہ_آغاز;
    اگر(reorder_آغاز>=رمز_نقطہ_آغاز) {
        واپس 0;
    }
    UChar32 c=*--رمز_نقطہ_آغاز;
    char16_t c2;
    اگر(U16_IS_TRAIL(c) && آغاز<رمز_نقطہ_آغاز && U16_IS_LEAD(c2=*(رمز_نقطہ_آغاز-1))) {
        --رمز_نقطہ_آغاز;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    واپس impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
خالی Reordering_بفر::insert(UChar32 c, uint8_t cc) {
    برائے(سیٹ_Iterator(), skip_پچھلا(); پچھلا_CC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=حد_بندی;
    char16_t *r=حد_بندی+=U16_لمبائی(c);
    کرو {
        *--r=*--q;
    } جب(رمز_نقطہ_حد_بندی!=q);
    write_رمز_نقطہ(q, c);
    اگر(cc<=1) {
        reorder_آغاز=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

ساخت Canon_Iter_ڈیٹا : عوامی UMemory {
    Canon_Iter_ڈیٹا(UError_رمز &error_رمز);
    ~Canon_Iter_ڈیٹا();
    خالی جوڑو_مقصد_آغاز_سیٹ(UChar32 origin, UChar32 decompLead, UError_رمز &error_رمز);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canon_آغاز_Sets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    حذف f_Canon_Iter_ڈیٹا;
}

خالی
Normalizer2Impl::init(ثابت int32_t *اندر_Indexes, ثابت UCPTrie *اندر_Trie,
                      ثابت uint16_t *اندر_Extra_ڈیٹا, ثابت uint8_t *اندر_Small_FCD) {
    minDecompNoCP = static_cast<char16_t>(اندر_Indexes[IX_MIN_DECOMP_NO_CP]);
    minCompNoMaybeCP = static_cast<char16_t>(اندر_Indexes[IX_MIN_COMP_NO_MAYBE_CP]);
    minLcccCP = static_cast<char16_t>(اندر_Indexes[IX_MIN_LCCC_CP]);

    minYesNo = static_cast<uint16_t>(اندر_Indexes[IX_MIN_YES_NO]);
    minYesNoMappingsOnly = static_cast<uint16_t>(اندر_Indexes[IX_MIN_YES_NO_MAPPINGS_ONLY]);
    minNoNo = static_cast<uint16_t>(اندر_Indexes[IX_MIN_NO_NO]);
    minNoNoCompBoundaryBefore = static_cast<uint16_t>(اندر_Indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE]);
    minNoNoCompNoMaybeCC = static_cast<uint16_t>(اندر_Indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC]);
    min_No_No_خالی_خانے = static_cast<uint16_t>(اندر_Indexes[IX_MIN_NO_NO_خالی_خانے]);
    حد_بندی_No_No = static_cast<uint16_t>(اندر_Indexes[IX_حد_بندی_NO_NO]);
    minMaybeNo = static_cast<uint16_t>(اندر_Indexes[IX_MIN_MAYBE_NO]);
    minMaybeNoCombinesFwd = static_cast<uint16_t>(اندر_Indexes[IX_MIN_MAYBE_NO_COMBINES_FWD]);
    minMaybeYes = static_cast<uint16_t>(اندر_Indexes[IX_MIN_MAYBE_YES]);
    U_ASSERT((minMaybeNo & 7) == 0);  // 8-aligned for noNoDelta bit fields
    centerNoNoDelta = (minMaybeNo >> DELTA_سرکاؤ) - MAX_DELTA - 1;

    normTrie=اندر_Trie;
    extra_ڈیٹا=اندر_Extra_ڈیٹا;
    smallFCD=اندر_Small_FCD;
}

U_CDECL_BEGIN

ساکن uint32_t U_CALLCONV
segmentStarterMapper(ثابت خالی * /*context*/, uint32_t value) {
    واپس value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_انتہا

خالی
Normalizer2Impl::جوڑو_Lccc_Chars(Unicode_سیٹ &سیٹ) ثابت {
    UChar32 آغاز = 0, انتہا;
    uint32_t norm16;
    جب ((انتہا = ucptrie_getRange(normTrie, آغاز, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   خالی_اشاریہ, خالی_اشاریہ, &norm16)) >= 0) {
        اگر (norm16 > Normalizer2Impl::MIN_معمول_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            سیٹ.جوڑو(آغاز, انتہا);
        } ورنہ اگر (minNoNoCompNoMaybeCC <= norm16 && norm16 < حد_بندی_No_No) {
            uint16_t fcd16 = getFCD16(آغاز);
            اگر (fcd16 > 0xff) { سیٹ.جوڑو(آغاز, انتہا); }
        }
        آغاز = انتہا + 1;
    }
}

خالی
Normalizer2Impl::جوڑو_Property_Starts(ثابت USetAdder *sa, UError_رمز & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 آغاز = 0, انتہا;
    uint32_t value;
    جب ((انتہا = ucptrie_getRange(normTrie, آغاز, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   خالی_اشاریہ, خالی_اشاریہ, &value)) >= 0) {
        sa->جوڑو(sa->سیٹ, آغاز);
        اگر (آغاز != انتہا && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_نقاب) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t پچھلا_FCD16 = getFCD16(آغاز);
            جب (++آغاز <= انتہا) {
                uint16_t fcd16 = getFCD16(آغاز);
                اگر (fcd16 != پچھلا_FCD16) {
                    sa->جوڑو(sa->سیٹ, آغاز);
                    پچھلا_FCD16 = fcd16;
                }
            }
        }
        آغاز = انتہا + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    برائے(char16_t c=Hangul::HANGUL_بنیاد; c<Hangul::HANGUL_حد_بندی; c+=Hangul::JAMO_T_گنتی) {
        sa->جوڑو(sa->سیٹ, c);
        sa->جوڑو(sa->سیٹ, c+1);
    }
    sa->جوڑو(sa->سیٹ, Hangul::HANGUL_حد_بندی); /* add Hangul+1 to continue with other properties */
}

خالی
Normalizer2Impl::جوڑو_Canon_Iter_Property_Starts(ثابت USetAdder *sa, UError_رمز &error_رمز) ثابت {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    اگر (!ensure_Canon_Iter_ڈیٹا(error_رمز)) { واپس; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 آغاز = 0, انتہا;
    uint32_t value;
    جب ((انتہا = ucptrie_getRange(f_Canon_Iter_ڈیٹا->trie, آغاز, UCPMAP_RANGE_معمول, 0,
                                   segmentStarterMapper, خالی_اشاریہ, &value)) >= 0) {
        sa->جوڑو(sa->سیٹ, آغاز);
        آغاز = انتہا + 1;
    }
}

ثابت char16_t *
Normalizer2Impl::copy_کم_سابقہ_From_Nul_Terminated(ثابت char16_t *src,
                                                UChar32 min_Need_ڈیٹا_CP,
                                                Reordering_بفر *بفر,
                                                UError_رمز &error_رمز) ثابت {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ثابت char16_t *پچھلا_Src=src;
    char16_t c;
    جب((c=*src++)<min_Need_ڈیٹا_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    اگر(--src!=پچھلا_Src) {
        اگر(بفر!=خالی_اشاریہ) {
            بفر->appendZeroCC(پچھلا_Src, src, error_رمز);
        }
    }
    واپس src;
}

UnicodeString &
Normalizer2Impl::decompose(ثابت UnicodeString &src, UnicodeString &dest,
                           UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        dest.سیٹ_مقصد_Bogus();
        واپس dest;
    }
    ثابت char16_t *sArray=src.get_بفر();
    اگر(&dest==&src || sArray==خالی_اشاریہ) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        dest.سیٹ_مقصد_Bogus();
        واپس dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), error_رمز);
    واپس dest;
}

خالی
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                           UnicodeString &dest,
                           int32_t dest_لمبائی_Estimate,
                           UError_رمز &error_رمز) ثابت {
    اگر(dest_لمبائی_Estimate<0 && حد_بندی!=خالی_اشاریہ) {
        dest_لمبائی_Estimate = static_cast<int32_t>(حد_بندی - src);
    }
    dest.remove();
    Reordering_بفر بفر(*یہ, dest);
    اگر(بفر.init(dest_لمبائی_Estimate, error_رمز)) {
        decompose(src, حد_بندی, &بفر, error_رمز);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ثابت char16_t *
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                           Reordering_بفر *بفر,
                           UError_رمز &error_رمز) ثابت {
    اگر(src==خالی_اشاریہ) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس src;
    }
    UChar32 minNoCP=minDecompNoCP;
    اگر(حد_بندی==خالی_اشاریہ) {
        src=copy_کم_سابقہ_From_Nul_Terminated(src, minNoCP, بفر, error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس src;
        }
        حد_بندی=u_strchr(src, 0);
    }

    ثابت char16_t *پچھلا_Src;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ثابت char16_t *پچھلا_Boundary=src;
    uint8_t پچھلا_CC_2=0;

    برائے(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        برائے(پچھلا_Src=src; src!=حد_بندی;) {
            اگر( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ورنہ اگر(!U16_IS_LEAD(c)) {
                توڑو;
            } ورنہ {
                char16_t c2;
                اگر((src+1)!=حد_بندی && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    اگر(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } ورنہ {
                        توڑو;
                    }
                } ورنہ {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        اگر(src!=پچھلا_Src) {
            اگر(بفر!=خالی_اشاریہ) {
                اگر(!بفر->appendZeroCC(پچھلا_Src, src, error_رمز)) {
                    توڑو;
                }
            } ورنہ {
                پچھلا_CC_2=0;
                پچھلا_Boundary=src;
            }
        }
        اگر(src==حد_بندی) {
            توڑو;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_لمبائی(c);
        اگر(بفر!=خالی_اشاریہ) {
            اگر(!decompose(c, norm16, *بفر, error_رمز)) {
                توڑو;
            }
        } ورنہ {
            اگر(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                اگر(پچھلا_CC_2<=cc || cc==0) {
                    پچھلا_CC_2=cc;
                    اگر(cc<=1) {
                        پچھلا_Boundary=src;
                    }
                    جاری;
                }
            }
            واپس پچھلا_Boundary;  // "no" or cc out of order
        }
    }
    واپس src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ثابت char16_t *
Normalizer2Impl::decomposeShort(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    جب(src<حد_بندی) {
        اگر (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            واپس src;
        }
        ثابت char16_t *پچھلا_Src = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_اگلا(normTrie, UCPTRIE_16, src, حد_بندی, c, norm16);
        اگر (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            واپس پچھلا_Src;
        }
        اگر(!decompose(c, norm16, بفر, error_رمز)) {
            واپس خالی_اشاریہ;
        }
        اگر (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            واپس src;
        }
    }
    واپس src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_بفر &بفر,
                                 UError_رمز &error_رمز) ثابت {
    // get the decomposition and the lead and trail cc's
    اگر (norm16 >= حد_بندی_No_No) {
        اگر (isMaybeYesOrNonZeroCC(norm16)) {
            واپس بفر.append(c, getCCFromYesOrMaybeYes(norm16), error_رمز);
        } ورنہ اگر (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    اگر (norm16 < minYesNo) {
        // c does not decompose
        واپس بفر.append(c, 0, error_رمز);
    } ورنہ اگر(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        واپس بفر.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_رمز);
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_ڈیٹا(norm16);
    uint16_t پہلا_Unit=*mapping;
    int32_t length=پہلا_Unit&MAPPING_لمبائی_نقاب;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(پہلا_Unit >> 8);
    اگر(پہلا_Unit&MAPPING_HAS_CCC_LCCC_لفظ) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } ورنہ {
        leadCC=0;
    }
    واپس بفر.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, سچ, leadCC, trailCC, error_رمز);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ثابت uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ثابت uint8_t *src, ثابت uint8_t *حد_بندی,
                               ByteSink *sink, Edits *edits, UError_رمز &error_رمز) ثابت {
    U_ASSERT(حد_بندی != خالی_اشاریہ);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ثابت uint8_t *پچھلا_Boundary = src;
    // only for quick check
    uint8_t پچھلا_CC_2 = 0;

    برائے (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ثابت uint8_t *fast_آغاز = src;
        ثابت uint8_t *پچھلا_Src;
        uint16_t norm16 = 0;

        برائے (;;) {
            اگر (src == حد_بندی) {
                اگر (پچھلا_Boundary != حد_بندی && sink != خالی_اشاریہ) {
                    ByteSinkUtil::appendUnchanged(پچھلا_Boundary, حد_بندی,
                                                  *sink, options, edits, error_رمز);
                }
                واپس src;
            }
            اگر (*src < minNoLead) {
                ++src;
            } ورنہ {
                پچھلا_Src = src;
                UCPTRIE_FAST_U8_اگلا(normTrie, UCPTRIE_16, src, حد_بندی, norm16);
                اگر (!isMostDecompYesAndZeroCC(norm16)) {
                    توڑو;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        اگر (پچھلا_Src != fast_آغاز) {
            // The fast path looped over yes/0 characters before the current one.
            اگر (sink != خالی_اشاریہ &&
                    !ByteSinkUtil::appendUnchanged(پچھلا_Boundary, پچھلا_Src,
                                                   *sink, options, edits, error_رمز)) {
                توڑو;
            }
            پچھلا_Boundary = پچھلا_Src;
            پچھلا_CC_2 = 0;
        }

        // Medium-fast path: Quick check.
        اگر (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            اگر (پچھلا_CC_2 <= cc || cc == 0) {
                پچھلا_CC_2 = cc;
                اگر (cc <= 1) {
                    اگر (sink != خالی_اشاریہ &&
                            !ByteSinkUtil::appendUnchanged(پچھلا_Boundary, src,
                                                           *sink, options, edits, error_رمز)) {
                        توڑو;
                    }
                    پچھلا_Boundary = src;
                }
                جاری;
            }
        }
        اگر (sink == خالی_اشاریہ) {
            واپس پچھلا_Boundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        اگر (پچھلا_Boundary != پچھلا_Src && norm16HasDecompBoundaryBefore(norm16)) {
            اگر (!ByteSinkUtil::appendUnchanged(پچھلا_Boundary, پچھلا_Src,
                                               *sink, options, edits, error_رمز)) {
                توڑو;
            }
            پچھلا_Boundary = پچھلا_Src;
        }
        Reordering_بفر بفر(*یہ, s16, error_رمز);
        اگر (U_FAILURE(error_رمز)) {
            توڑو;
        }
        decomposeShort(پچھلا_Boundary, src, STOP_AT_حد_بندی, جھوٹ /* onlyContiguous */,
                       بفر, error_رمز);
        // Decompose until the next boundary.
        اگر (بفر.get_آخری_CC() > 1) {
            src = decomposeShort(src, حد_بندی, STOP_AT_DECOMP_BOUNDARY, جھوٹ /* onlyContiguous */,
                                 بفر, error_رمز);
        }
        اگر (U_FAILURE(error_رمز)) {
            توڑو;
        }
        اگر ((src - پچھلا_Src) > INT32_MAX) {  // guard before buffer.equals()
            error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
            توڑو;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        اگر (isMaybeYesOrNonZeroCC(norm16) && بفر.equals(پچھلا_Boundary, src)) {
            اگر (!ByteSinkUtil::appendUnchanged(پچھلا_Boundary, src,
                                               *sink, options, edits, error_رمز)) {
                توڑو;
            }
        } ورنہ {
            اگر (!ByteSinkUtil::appendChange(پچھلا_Boundary, src, بفر.get_آغاز(), بفر.length(),
                                            *sink, edits, error_رمز)) {
                توڑو;
            }
        }
        پچھلا_Boundary = src;
        پچھلا_CC_2 = 0;
    }
    واپس src;
}

ثابت uint8_t *
Normalizer2Impl::decomposeShort(ثابت uint8_t *src, ثابت uint8_t *حد_بندی,
                                StopAt stopAt, UBool onlyContiguous,
                                Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    جب (src < حد_بندی) {
        ثابت uint8_t *پچھلا_Src = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_اگلا(normTrie, UCPTRIE_16, src, حد_بندی, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        اگر (norm16 >= حد_بندی_No_No) {
            اگر (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                اگر (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    واپس پچھلا_Src;
                }
                c = رمز_نقطہ_From_Valid_UTF8(پچھلا_Src, src);
                اگر (!بفر.append(c, cc, error_رمز)) {
                    واپس خالی_اشاریہ;
                }
                اگر (stopAt == STOP_AT_DECOMP_BOUNDARY && بفر.get_آخری_CC() <= 1) {
                    واپس src;
                }
                جاری;
            } ورنہ اگر (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                اگر (stopAt != STOP_AT_حد_بندی) {
                    واپس پچھلا_Src;
                }
                c = رمز_نقطہ_From_Valid_UTF8(پچھلا_Src, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } ورنہ اگر (stopAt != STOP_AT_حد_بندی && norm16 < minNoNoCompNoMaybeCC) {
            واپس پچھلا_Src;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        اگر (norm16 < minYesNo) {
            اگر (c < 0) {
                c = رمز_نقطہ_From_Valid_UTF8(پچھلا_Src, src);
            }
            // does not decompose
            اگر (!بفر.append(c, 0, error_رمز)) {
                واپس خالی_اشاریہ;
            }
        } ورنہ اگر (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            اگر (c < 0) {
                c = رمز_نقطہ_From_Valid_UTF8(پچھلا_Src, src);
            }
            char16_t jamos[3];
            اگر (!بفر.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_رمز)) {
                واپس خالی_اشاریہ;
            }
        } ورنہ {
            // The character decomposes, get everything from the variable-length extra data.
            ثابت uint16_t *mapping = get_ڈیٹا(norm16);
            uint16_t پہلا_Unit = *mapping;
            int32_t length = پہلا_Unit & MAPPING_لمبائی_نقاب;
            uint8_t trailCC = static_cast<uint8_t>(پہلا_Unit >> 8);
            uint8_t leadCC;
            اگر (پہلا_Unit & MAPPING_HAS_CCC_LCCC_لفظ) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } ورنہ {
                leadCC = 0;
            }
            اگر (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                واپس پچھلا_Src;
            }
            اگر (!بفر.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, سچ, leadCC, trailCC, error_رمز)) {
                واپس خالی_اشاریہ;
            }
        }
        اگر ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && بفر.get_آخری_CC() <= 1)) {
            واپس src;
        }
    }
    واپس src;
}

ثابت char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t بفر[4], int32_t &length) ثابت {
    uint16_t norm16;
    اگر(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        واپس خالی_اشاریہ;
    }
    ثابت char16_t *decomp = خالی_اشاریہ;
    اگر(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=بفر;
        length=0;
        U16_APPEND_UNSAFE(بفر, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    اگر (norm16 < minYesNo) {
        واپس decomp;
    } ورنہ اگر(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, بفر);
        واپس بفر;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_ڈیٹا(norm16);
    length=*mapping&MAPPING_لمبائی_نقاب;
    واپس reinterpret_cast<ثابت char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ثابت char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t بفر[30], int32_t &length) ثابت {
    uint16_t norm16;
    اگر(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        واپس خالی_اشاریہ;
    } ورنہ اگر(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, بفر);
        length=2;
        واپس بفر;
    } ورنہ اگر(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(بفر, length, c);
        واپس بفر;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_ڈیٹا(norm16);
    uint16_t پہلا_Unit=*mapping;
    int32_t m_لمبائی=پہلا_Unit&MAPPING_لمبائی_نقاب;  // length of normal mapping
    اگر(پہلا_Unit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ثابت uint16_t *rawMapping=mapping-((پہلا_Unit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        اگر(rm0<=MAPPING_لمبائی_نقاب) {
            length=rm0;
            واپس reinterpret_cast<ثابت char16_t*>(rawMapping) - rm0;
        } ورنہ {
            // Copy the normal mapping and replace its first two code units with rm0.
            بفر[0] = static_cast<char16_t>(rm0);
            u_memcpy(بفر + 1, reinterpret_cast<ثابت char16_t*>(mapping) + 1 + 2, m_لمبائی - 2);
            length=m_لمبائی-1;
            واپس بفر;
        }
    } ورنہ {
        length=m_لمبائی;
        واپس reinterpret_cast<ثابت char16_t*>(mapping) + 1;
    }
}

خالی Normalizer2Impl::decomposeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                                         UBool doDecompose,
                                         UnicodeString &محفوظ_درمیان,
                                         Reordering_بفر &بفر,
                                         UError_رمز &error_رمز) ثابت {
    بفر.copy_Reorderable_Suffix_مقصد(محفوظ_درمیان);
    اگر(doDecompose) {
        decompose(src, حد_بندی, &بفر, error_رمز);
        واپس;
    }
    // Just merge the strings at the boundary.
    منطقی is_پہلا = سچ;
    uint8_t پہلا_CC = 0, پچھلا_CC_2 = 0, cc;
    ثابت char16_t *p = src;
    جب (p != حد_بندی) {
        ثابت char16_t *رمز_نقطہ_آغاز = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_اگلا(normTrie, UCPTRIE_16, p, حد_بندی, c, norm16);
        اگر ((cc = getCC(norm16)) == 0) {
            p = رمز_نقطہ_آغاز;
            توڑو;
        }
        اگر (is_پہلا) {
            پہلا_CC = cc;
            is_پہلا = جھوٹ;
        }
        پچھلا_CC_2 = cc;
    }
    اگر(حد_بندی==خالی_اشاریہ) {  // appendZeroCC() needs limit!=nullptr
        حد_بندی=u_strchr(p, 0);
    }

    اگر (بفر.append(src, static_cast<int32_t>(p - src), جھوٹ, پہلا_CC, پچھلا_CC_2, error_رمز)) {
        بفر.appendZeroCC(p, حد_بندی, error_رمز);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ثابت {
    واپس c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ثابت {
    اگر (norm16 < minNoNoCompNoMaybeCC) {
        واپس سچ;
    }
    اگر (norm16 >= حد_بندی_No_No) {
        واپس norm16 <= MIN_معمول_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_ڈیٹا_For_Yes_Or_No(norm16);
    uint16_t پہلا_Unit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    واپس (پہلا_Unit&MAPPING_HAS_CCC_LCCC_لفظ)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ثابت {
    اگر (c < minDecompNoCP) {
        واپس سچ;
    }
    اگر (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        واپس سچ;
    }
    واپس norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ثابت {
    اگر(norm16 <= minYesNo || isHangulLVT(norm16)) {
        واپس سچ;
    }
    اگر (norm16 >= حد_بندی_No_No) {
        اگر (isMaybeYesOrNonZeroCC(norm16)) {
            واپس norm16 <= MIN_معمول_MAYBE_YES || norm16 == JAMO_VT;
        } ورنہ اگر (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            واپس (norm16 & DELTA_TCCC_نقاب) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_ڈیٹا(norm16);
    uint16_t پہلا_Unit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    اگر(پہلا_Unit>0x1ff) {
        واپس جھوٹ;  // trailCC>1
    }
    اگر(پہلا_Unit<=0xff) {
        واپس سچ;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    واپس (پہلا_Unit&MAPPING_HAS_CCC_LCCC_لفظ)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(ثابت uint16_t *list, UChar32 trail) {
    uint16_t key1, پہلا_Unit;
    اگر(trail<COMP_1_TRAIL_حد_بندی) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        جب(key1>(پہلا_Unit=*list)) {
            list+=2+(پہلا_Unit&COMP_1_TRIPLE);
        }
        اگر(key1==(پہلا_Unit&COMP_1_TRAIL_نقاب)) {
            اگر(پہلا_Unit&COMP_1_TRIPLE) {
                واپس (static_cast<int32_t>(list[1]) << 16) | list[2];
            } ورنہ {
                واپس list[1];
            }
        }
    } ورنہ {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_حد_بندی +
                        (((trail>>COMP_1_TRAIL_سرکاؤ))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_سرکاؤ);
        uint16_t دوسرا_Unit;
        برائے(;;) {
            اگر(key1>(پہلا_Unit=*list)) {
                list+=2+(پہلا_Unit&COMP_1_TRIPLE);
            } ورنہ اگر(key1==(پہلا_Unit&COMP_1_TRAIL_نقاب)) {
                اگر(key2>(دوسرا_Unit=list[1])) {
                    اگر(پہلا_Unit&COMP_1_آخری_TUPLE) {
                        توڑو;
                    } ورنہ {
                        list+=3;
                    }
                } ورنہ اگر(key2==(دوسرا_Unit&COMP_2_TRAIL_نقاب)) {
                    واپس (static_cast<int32_t>(دوسرا_Unit & ~COMP_2_TRAIL_نقاب) << 16) | list[2];
                } ورنہ {
                    توڑو;
                }
            } ورنہ {
                توڑو;
            }
        }
    }
    واپس -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
خالی Normalizer2Impl::جوڑو_Composites(ثابت uint16_t *list, Unicode_سیٹ &سیٹ) ثابت {
    uint16_t پہلا_Unit;
    int32_t مرکب_And_Fwd;
    کرو {
        پہلا_Unit=*list;
        اگر((پہلا_Unit&COMP_1_TRIPLE)==0) {
            مرکب_And_Fwd=list[1];
            list+=2;
        } ورنہ {
            مرکب_And_Fwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_نقاب) << 16) | list[2];
            list+=3;
        }
        UChar32 مرکب=مرکب_And_Fwd>>1;
        اگر((مرکب_And_Fwd&1)!=0) {
            جوڑو_Composites(get_Compositions_List_For_مرکب(getRawNorm16(مرکب)), سیٹ);
        }
        سیٹ.جوڑو(مرکب);
    } جب((پہلا_Unit&COMP_1_آخری_TUPLE)==0);
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
خالی Normalizer2Impl::recompose(Reordering_بفر &بفر, int32_t recompose_آغاز_اشاریہ,
                                UBool onlyContiguous) ثابت {
    char16_t *p=بفر.get_آغاز()+recompose_آغاز_اشاریہ;
    char16_t *حد_بندی=بفر.get_حد_بندی();
    اگر(p==حد_بندی) {
        واپس;
    }

    char16_t *starter, *pRemove, *q, *r;
    ثابت uint16_t *compositionsList;
    UChar32 c, مرکب_And_Fwd;
    uint16_t norm16;
    uint8_t cc, پچھلا_CC_2;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=خالی_اشاریہ;  // used as indicator for whether we have a forward-combining starter
    starter=خالی_اشاریہ;
    starterIsSupplementary=جھوٹ;
    پچھلا_CC_2=0;

    برائے(;;) {
        UCPTRIE_FAST_U16_اگلا(normTrie, UCPTRIE_16, p, حد_بندی, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        اگر( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=خالی_اشاریہ &&
            // the backward-combining character is not blocked
            (پچھلا_CC_2<cc || پچھلا_CC_2==0)
        ) {
            اگر(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                اگر(c<Hangul::JAMO_T_بنیاد) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t پچھلا = static_cast<char16_t>(*starter - Hangul::JAMO_L_بنیاد);
                    اگر(پچھلا<Hangul::JAMO_L_گنتی) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_بنیاد +
                             (پچھلا*Hangul::JAMO_V_گنتی+(c-Hangul::JAMO_V_بنیاد))*
                             Hangul::JAMO_T_گنتی);
                        char16_t t;
                        اگر (p != حد_بندی && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_بنیاد)) < Hangul::JAMO_T_گنتی) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        جب(r<حد_بندی) {
                            *q++=*r++;
                        }
                        حد_بندی=q;
                        p=pRemove;
                    }
                }
                /*
                 * No "else" for Jamo T:
                 * Since the input is in NFD, there are no Hangul LV syllables that
                 * a Jamo T could combine with.
                 * All Jamo Ts are combined above when handling Jamo Vs.
                 */
                اگر(p==حد_بندی) {
                    توڑو;
                }
                compositionsList=خالی_اشاریہ;
                جاری;
            } ورنہ اگر((مرکب_And_Fwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 مرکب=مرکب_And_Fwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_لمبائی(c);  // pRemove & p: start & limit of the combining mark
                اگر(starterIsSupplementary) {
                    اگر(U_IS_SUPPLEMENTARY(مرکب)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(مرکب);
                        starter[1]=U16_TRAIL(مرکب);
                    } ورنہ {
                        *starter = static_cast<char16_t>(مرکب);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=جھوٹ;
                        q=starter+1;
                        r=q+1;
                        جب(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } ورنہ اگر(U_IS_SUPPLEMENTARY(مرکب)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=سچ;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    جب(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(مرکب);
                    *--starter=U16_LEAD(مرکب);  // undo the temporary increment
                } ورنہ {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(مرکب);
                }

                /* remove the combining mark by moving the following text over it */
                اگر(pRemove<p) {
                    q=pRemove;
                    r=p;
                    جب(r<حد_بندی) {
                        *q++=*r++;
                    }
                    حد_بندی=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                اگر(p==حد_بندی) {
                    توڑو;
                }
                // Is the composite a starter that combines forward?
                اگر(مرکب_And_Fwd&1) {
                    compositionsList=
                        get_Compositions_List_For_مرکب(getRawNorm16(مرکب));
                } ورنہ {
                    compositionsList=خالی_اشاریہ;
                }

                // We combined; continue with looking for compositions.
                جاری;
            }
        }

        // no combination this time
        پچھلا_CC_2=cc;
        اگر(p==حد_بندی) {
            توڑو;
        }

        // If c did not combine, then check if it is a starter.
        اگر(cc==0) {
            // Found a new starter.
            اگر((compositionsList=getCompositionsListForDecompYes(norm16))!=خالی_اشاریہ) {
                // It may combine with something, prepare for it.
                اگر(U_IS_BMP(c)) {
                    starterIsSupplementary=جھوٹ;
                    starter=p-1;
                } ورنہ {
                    starterIsSupplementary=سچ;
                    starter=p-2;
                }
            }
        } ورنہ اگر(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=خالی_اشاریہ;
        }
    }
    بفر.سیٹ_Reordering_حد_بندی(حد_بندی);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ثابت {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ثابت uint16_t *list;
    اگر(isInert(norm16)) {
        واپس U_SENTINEL;
    } ورنہ اگر(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        اگر(isJamoL(norm16)) {
            اگر (b < Hangul::JAMO_V_بنیاد) {
                واپس U_SENTINEL;
            }
            b-=Hangul::JAMO_V_بنیاد;
            اگر(b<Hangul::JAMO_V_گنتی) {
                واپس
                    (Hangul::HANGUL_بنیاد+
                     ((a-Hangul::JAMO_L_بنیاد)*Hangul::JAMO_V_گنتی+b)*
                     Hangul::JAMO_T_گنتی);
            } ورنہ {
                واپس U_SENTINEL;
            }
        } ورنہ اگر(isHangulLV(norm16)) {
            اگر (b <= Hangul::JAMO_T_بنیاد) {
               واپس U_SENTINEL;
            }
            b-=Hangul::JAMO_T_بنیاد;
            اگر(b<Hangul::JAMO_T_گنتی) {  // not b==0!
                واپس a+b;
            } ورنہ {
                واپس U_SENTINEL;
            }
        } ورنہ {
            // 'a' has a compositions list in extraData
            list=get_ڈیٹا_For_Yes_Or_No(norm16);
            اگر(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_لمبائی_نقاب);  // + mapping length
            }
        }
    } ورنہ اگر(norm16<minMaybeNoCombinesFwd || MIN_معمول_MAYBE_YES<=norm16) {
        واپس U_SENTINEL;
    } ورنہ {
        list=get_ڈیٹا_For_Maybe(norm16);
        اگر(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_لمبائی_نقاب);  // + mapping length
        }
    }
    اگر(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        واپس U_SENTINEL;
    }
#اگر U_SIGNED_دایاں_سرکاؤ_IS_ARITHMETIC
    واپس combine(list, b)>>1;
#ورنہ
    int32_t مرکب_And_Fwd=combine(list, b);
    واپس مرکب_And_Fwd>=0 ? مرکب_And_Fwd>>1 : U_SENTINEL;
#اگر_ختم
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_بفر &بفر,
                         UError_رمز &error_رمز) ثابت {
    ثابت char16_t *پچھلا_Boundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    اگر(حد_بندی==خالی_اشاریہ) {
        src=copy_کم_سابقہ_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &بفر : خالی_اشاریہ,
                                           error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس جھوٹ;
        }
        حد_بندی=u_strchr(src, 0);
        اگر (پچھلا_Boundary != src) {
            اگر (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                پچھلا_Boundary = src;
            } ورنہ {
                بفر.removeSuffix(1);
                پچھلا_Boundary = --src;
            }
        }
    }

    برائے (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت char16_t *پچھلا_Src;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        برائے (;;) {
            اگر (src == حد_بندی) {
                اگر (پچھلا_Boundary != حد_بندی && doCompose) {
                    بفر.appendZeroCC(پچھلا_Boundary, حد_بندی, error_رمز);
                }
                واپس سچ;
            }
            اگر( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ورنہ {
                پچھلا_Src = src++;
                اگر(!U16_IS_LEAD(c)) {
                    توڑو;
                } ورنہ {
                    char16_t c2;
                    اگر(src!=حد_بندی && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        اگر(!isCompYesAndZeroCC(norm16)) {
                            توڑو;
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
        اگر (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            اگر (!doCompose) {
                واپس جھوٹ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            اگر (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد_بندی)) {
                    اگر (پچھلا_Boundary != پچھلا_Src && !بفر.appendZeroCC(پچھلا_Boundary, پچھلا_Src, error_رمز)) {
                        توڑو;
                    }
                    اگر(!بفر.append(mapAlgorithmic(c, norm16), 0, error_رمز)) {
                        توڑو;
                    }
                    پچھلا_Boundary = src;
                    جاری;
                }
            } ورنہ اگر (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد_بندی)) {
                    اگر (پچھلا_Boundary != پچھلا_Src && !بفر.appendZeroCC(پچھلا_Boundary, پچھلا_Src, error_رمز)) {
                        توڑو;
                    }
                    ثابت char16_t *mapping = reinterpret_cast<ثابت char16_t *>(get_ڈیٹا_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_لمبائی_نقاب;
                    اگر(!بفر.appendZeroCC(mapping, mapping + length, error_رمز)) {
                        توڑو;
                    }
                    پچھلا_Boundary = src;
                    جاری;
                }
            } ورنہ اگر (norm16 >= min_No_No_خالی_خانے) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                اگر (hasCompBoundaryBefore(src, حد_بندی) ||
                        hasCompBoundaryAfter(پچھلا_Boundary, پچھلا_Src, onlyContiguous)) {
                    اگر (پچھلا_Boundary != پچھلا_Src && !بفر.appendZeroCC(پچھلا_Boundary, پچھلا_Src, error_رمز)) {
                        توڑو;
                    }
                    پچھلا_Boundary = src;
                    جاری;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ورنہ اگر (isJamoVT(norm16) && پچھلا_Boundary != پچھلا_Src) {
            char16_t پچھلا=*(پچھلا_Src-1);
            اگر(c<Hangul::JAMO_T_بنیاد) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(پچھلا - Hangul::JAMO_L_بنیاد);
                اگر(l<Hangul::JAMO_L_گنتی) {
                    اگر (!doCompose) {
                        واپس جھوٹ;
                    }
                    int32_t t;
                    اگر (src != حد_بندی &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_بنیاد)) &&
                            t < Hangul::JAMO_T_گنتی) {
                        // The next character is a Jamo T.
                        ++src;
                    } ورنہ اگر (hasCompBoundaryBefore(src, حد_بندی)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } ورنہ {
                        t = -1;
                    }
                    اگر (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_بنیاد +
                            (l*Hangul::JAMO_V_گنتی + (c-Hangul::JAMO_V_بنیاد)) *
                            Hangul::JAMO_T_گنتی + t;
                        --پچھلا_Src;  // Replace the Jamo L as well.
                        اگر (پچھلا_Boundary != پچھلا_Src && !بفر.appendZeroCC(پچھلا_Boundary, پچھلا_Src, error_رمز)) {
                            توڑو;
                        }
                        اگر (!بفر.appendBMP(static_cast<char16_t>(syllable), 0, error_رمز)) {
                            توڑو;
                        }
                        پچھلا_Boundary = src;
                        جاری;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ورنہ اگر (Hangul::isHangulLV(پچھلا)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                اگر (!doCompose) {
                    واپس جھوٹ;
                }
                UChar32 syllable = پچھلا + c - Hangul::JAMO_T_بنیاد;
                --پچھلا_Src;  // Replace the Hangul LV as well.
                اگر (پچھلا_Boundary != پچھلا_Src && !بفر.appendZeroCC(پچھلا_Boundary, پچھلا_Src, error_رمز)) {
                    توڑو;
                }
                اگر (!بفر.appendBMP(static_cast<char16_t>(syllable), 0, error_رمز)) {
                    توڑو;
                }
                پچھلا_Boundary = src;
                جاری;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ورنہ اگر (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_معمول_Yes_Or_Maybe(norm16);  // cc!=0
            اگر (onlyContiguous /* FCC */ && get_پچھلا_Trail_CC(پچھلا_Boundary, پچھلا_Src) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                اگر (!doCompose) {
                    واپس جھوٹ;
                }
            } ورنہ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *اگلا_Src;
                uint16_t n16;
                برائے (;;) {
                    اگر (src == حد_بندی) {
                        اگر (doCompose) {
                            بفر.appendZeroCC(پچھلا_Boundary, حد_بندی, error_رمز);
                        }
                        واپس سچ;
                    }
                    uint8_t پچھلا_CC_2 = cc;
                    اگلا_Src = src;
                    UCPTRIE_FAST_U16_اگلا(normTrie, UCPTRIE_16, اگلا_Src, حد_بندی, c, n16);
                    اگر (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_معمول_Yes_Or_Maybe(n16);
                        اگر (پچھلا_CC_2 > cc) {
                            اگر (!doCompose) {
                                واپس جھوٹ;
                            }
                            توڑو;
                        }
                    } ورنہ {
                        توڑو;
                    }
                    src = اگلا_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                اگر (norm16HasCompBoundaryBefore(n16)) {
                    اگر (isCompYesAndZeroCC(n16)) {
                        src = اگلا_Src;
                    }
                    جاری;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        اگر (پچھلا_Boundary != پچھلا_Src && !norm16HasCompBoundaryBefore(norm16)) {
            ثابت char16_t *p = پچھلا_Src;
            UCPTRIE_FAST_U16_پچھلا(normTrie, UCPTRIE_16, پچھلا_Boundary, p, c, norm16);
            اگر (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                پچھلا_Src = p;
            }
        }
        اگر (doCompose && پچھلا_Boundary != پچھلا_Src && !بفر.appendZeroCC(پچھلا_Boundary, پچھلا_Src, error_رمز)) {
            توڑو;
        }
        int32_t recompose_آغاز_اشاریہ=بفر.length();
        // We know there is not a boundary here.
        decomposeShort(پچھلا_Src, src, جھوٹ /* !stopAtCompBoundary */, onlyContiguous,
                       بفر, error_رمز);
        // Decompose until the next boundary.
        src = decomposeShort(src, حد_بندی, سچ /* stopAtCompBoundary */, onlyContiguous,
                             بفر, error_رمز);
        اگر (U_FAILURE(error_رمز)) {
            توڑو;
        }
        اگر ((src - پچھلا_Src) > INT32_MAX) {  // guard before buffer.equals()
            error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس سچ;
        }
        recompose(بفر, recompose_آغاز_اشاریہ, onlyContiguous);
        اگر(!doCompose) {
            اگر(!بفر.equals(پچھلا_Src, src)) {
                واپس جھوٹ;
            }
            بفر.remove();
        }
        پچھلا_Boundary=src;
    }
    واپس سچ;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ثابت char16_t *
Normalizer2Impl::compose_تیز_Check(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                                   UBool onlyContiguous,
                                   UNormalization_Check_نتیجہ *pQCResult) ثابت {
    ثابت char16_t *پچھلا_Boundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    اگر(حد_بندی==خالی_اشاریہ) {
        UError_رمز error_رمز=U_ZERO_ERROR;
        src=copy_کم_سابقہ_From_Nul_Terminated(src, minNoMaybeCP, خالی_اشاریہ, error_رمز);
        حد_بندی=u_strchr(src, 0);
        اگر (پچھلا_Boundary != src) {
            اگر (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                پچھلا_Boundary = src;
            } ورنہ {
                پچھلا_Boundary = --src;
            }
        }
    }

    برائے(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت char16_t *پچھلا_Src;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        برائے (;;) {
            اگر(src==حد_بندی) {
                واپس src;
            }
            اگر( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ورنہ {
                پچھلا_Src = src++;
                اگر(!U16_IS_LEAD(c)) {
                    توڑو;
                } ورنہ {
                    char16_t c2;
                    اگر(src!=حد_بندی && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        اگر(!isCompYesAndZeroCC(norm16)) {
                            توڑو;
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

        uint16_t پچھلا_Norm16 = INERT;
        اگر (پچھلا_Boundary != پچھلا_Src) {
            اگر (norm16HasCompBoundaryBefore(norm16)) {
                پچھلا_Boundary = پچھلا_Src;
            } ورنہ {
                ثابت char16_t *p = پچھلا_Src;
                uint16_t n16;
                UCPTRIE_FAST_U16_پچھلا(normTrie, UCPTRIE_16, پچھلا_Boundary, p, c, n16);
                اگر (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    پچھلا_Boundary = پچھلا_Src;
                } ورنہ {
                    پچھلا_Boundary = p;
                    پچھلا_Norm16 = n16;
                }
            }
        }

        اگر (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            اگر (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(پچھلا_Norm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } ورنہ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *اگلا_Src;
                برائے (;;) {
                    اگر (norm16 < MIN_YES_YES_WITH_CC) {
                        اگر (pQCResult != خالی_اشاریہ) {
                            *pQCResult = UNORM_MAYBE;
                        } ورنہ {
                            واپس پچھلا_Boundary;
                        }
                    }
                    اگر (src == حد_بندی) {
                        واپس src;
                    }
                    uint8_t پچھلا_CC_2 = fcd16;
                    اگلا_Src = src;
                    UCPTRIE_FAST_U16_اگلا(normTrie, UCPTRIE_16, اگلا_Src, حد_بندی, c, norm16);
                    اگر (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        اگر (!(پچھلا_CC_2 <= cc || cc == 0)) {
                            توڑو;
                        }
                    } ورنہ {
                        توڑو;
                    }
                    src = اگلا_Src;
                }
                // src is after the last in-order combining mark.
                اگر (isCompYesAndZeroCC(norm16)) {
                    پچھلا_Boundary = src;
                    src = اگلا_Src;
                    جاری;
                }
            }
        }
        اگر(pQCResult!=خالی_اشاریہ) {
            *pQCResult=UNORM_NO;
        }
        واپس پچھلا_Boundary;
    }
}

خالی Normalizer2Impl::composeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &محفوظ_درمیان,
                                       Reordering_بفر &بفر,
                                       UError_رمز &error_رمز) ثابت {
    اگر(!بفر.is_خالی_خانے()) {
        ثابت char16_t *پہلا_Starter_اندر_Src=ڈھونڈو_اگلا_Comp_Boundary(src, حد_بندی, onlyContiguous);
        اگر(src!=پہلا_Starter_اندر_Src) {
            ثابت char16_t *آخری_Starter_اندر_Dest=ڈھونڈو_پچھلا_Comp_Boundary(بفر.get_آغاز(),
                                                                    بفر.get_حد_بندی(), onlyContiguous);
            int32_t dest_Suffix_لمبائی = static_cast<int32_t>(بفر.get_حد_بندی() - آخری_Starter_اندر_Dest);
            UnicodeString درمیان(آخری_Starter_اندر_Dest, dest_Suffix_لمبائی);
            بفر.removeSuffix(dest_Suffix_لمبائی);
            محفوظ_درمیان=درمیان;
            درمیان.append(src, static_cast<int32_t>(پہلا_Starter_اندر_Src - src));
            ثابت char16_t *درمیان_آغاز=درمیان.get_بفر();
            compose(درمیان_آغاز, درمیان_آغاز+درمیان.length(), onlyContiguous,
                    سچ, بفر, error_رمز);
            اگر(U_FAILURE(error_رمز)) {
                واپس;
            }
            src=پہلا_Starter_اندر_Src;
        }
    }
    اگر(doCompose) {
        compose(src, حد_بندی, onlyContiguous, سچ, بفر, error_رمز);
    } ورنہ {
        اگر(حد_بندی==خالی_اشاریہ) {  // appendZeroCC() needs limit!=nullptr
            حد_بندی=u_strchr(src, 0);
        }
        بفر.appendZeroCC(src, حد_بندی, error_رمز);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ثابت uint8_t *src, ثابت uint8_t *حد_بندی,
                             ByteSink *sink, Edits *edits, UError_رمز &error_رمز) ثابت {
    U_ASSERT(حد_بندی != خالی_اشاریہ);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ثابت uint8_t *پچھلا_Boundary = src;

    برائے (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت uint8_t *پچھلا_Src;
        uint16_t norm16 = 0;
        برائے (;;) {
            اگر (src == حد_بندی) {
                اگر (پچھلا_Boundary != حد_بندی && sink != خالی_اشاریہ) {
                    ByteSinkUtil::appendUnchanged(پچھلا_Boundary, حد_بندی,
                                                  *sink, options, edits, error_رمز);
                }
                واپس سچ;
            }
            اگر (*src < minNoMaybeLead) {
                ++src;
            } ورنہ {
                پچھلا_Src = src;
                UCPTRIE_FAST_U8_اگلا(normTrie, UCPTRIE_16, src, حد_بندی, norm16);
                اگر (!isCompYesAndZeroCC(norm16)) {
                    توڑو;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        اگر (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            اگر (sink == خالی_اشاریہ) {
                واپس جھوٹ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            اگر (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد_بندی)) {
                    اگر (پچھلا_Boundary != پچھلا_Src &&
                            !ByteSinkUtil::appendUnchanged(پچھلا_Boundary, پچھلا_Src,
                                                           *sink, options, edits, error_رمز)) {
                        توڑو;
                    }
                    append_رمز_نقطہ_Delta(پچھلا_Src, src, getAlgorithmicDelta(norm16), *sink, edits);
                    پچھلا_Boundary = src;
                    جاری;
                }
            } ورنہ اگر (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد_بندی)) {
                    اگر (پچھلا_Boundary != پچھلا_Src &&
                            !ByteSinkUtil::appendUnchanged(پچھلا_Boundary, پچھلا_Src,
                                                           *sink, options, edits, error_رمز)) {
                        توڑو;
                    }
                    ثابت uint16_t *mapping = get_ڈیٹا_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_لمبائی_نقاب;
                    اگر (!ByteSinkUtil::appendChange(پچھلا_Src, src, reinterpret_cast<ثابت char16_t*>(mapping), length,
                                                    *sink, edits, error_رمز)) {
                        توڑو;
                    }
                    پچھلا_Boundary = src;
                    جاری;
                }
            } ورنہ اگر (norm16 >= min_No_No_خالی_خانے) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                اگر (hasCompBoundaryBefore(src, حد_بندی) ||
                        hasCompBoundaryAfter(پچھلا_Boundary, پچھلا_Src, onlyContiguous)) {
                    اگر (پچھلا_Boundary != پچھلا_Src &&
                            !ByteSinkUtil::appendUnchanged(پچھلا_Boundary, پچھلا_Src,
                                                           *sink, options, edits, error_رمز)) {
                        توڑو;
                    }
                    اگر (edits != خالی_اشاریہ) {
                        edits->جوڑو_Replace(static_cast<int32_t>(src - پچھلا_Src), 0);
                    }
                    پچھلا_Boundary = src;
                    جاری;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ورنہ اگر (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - پچھلا_Src) == 3 && *پچھلا_Src == 0xe1);
            UChar32 پچھلا = پچھلا_Hangul_Or_Jamo(پچھلا_Boundary, پچھلا_Src);
            اگر (پچھلا_Src[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = پچھلا - Hangul::JAMO_L_بنیاد;
                اگر (static_cast<uint32_t>(l) < Hangul::JAMO_L_گنتی) {
                    اگر (sink == خالی_اشاریہ) {
                        واپس جھوٹ;
                    }
                    int32_t t = get_Jamo_TMinus_بنیاد(src, حد_بندی);
                    اگر (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } ورنہ اگر (hasCompBoundaryBefore(src, حد_بندی)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    اگر (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_بنیاد +
                            (l*Hangul::JAMO_V_گنتی + (پچھلا_Src[2]-0xa1)) *
                            Hangul::JAMO_T_گنتی + t;
                        پچھلا_Src -= 3;  // Replace the Jamo L as well.
                        اگر (پچھلا_Boundary != پچھلا_Src &&
                                !ByteSinkUtil::appendUnchanged(پچھلا_Boundary, پچھلا_Src,
                                                               *sink, options, edits, error_رمز)) {
                            توڑو;
                        }
                        ByteSinkUtil::append_رمز_نقطہ(پچھلا_Src, src, syllable, *sink, edits);
                        پچھلا_Boundary = src;
                        جاری;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ورنہ اگر (Hangul::isHangulLV(پچھلا)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                اگر (sink == خالی_اشاریہ) {
                    واپس جھوٹ;
                }
                UChar32 syllable = پچھلا + get_Jamo_TMinus_بنیاد(پچھلا_Src, src);
                پچھلا_Src -= 3;  // Replace the Hangul LV as well.
                اگر (پچھلا_Boundary != پچھلا_Src &&
                        !ByteSinkUtil::appendUnchanged(پچھلا_Boundary, پچھلا_Src,
                                                       *sink, options, edits, error_رمز)) {
                    توڑو;
                }
                ByteSinkUtil::append_رمز_نقطہ(پچھلا_Src, src, syllable, *sink, edits);
                پچھلا_Boundary = src;
                جاری;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ورنہ اگر (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_معمول_Yes_Or_Maybe(norm16);  // cc!=0
            اگر (onlyContiguous /* FCC */ && get_پچھلا_Trail_CC(پچھلا_Boundary, پچھلا_Src) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                اگر (sink == خالی_اشاریہ) {
                    واپس جھوٹ;
                }
            } ورنہ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت uint8_t *اگلا_Src;
                uint16_t n16;
                برائے (;;) {
                    اگر (src == حد_بندی) {
                        اگر (sink != خالی_اشاریہ) {
                            ByteSinkUtil::appendUnchanged(پچھلا_Boundary, حد_بندی,
                                                          *sink, options, edits, error_رمز);
                        }
                        واپس سچ;
                    }
                    uint8_t پچھلا_CC_2 = cc;
                    اگلا_Src = src;
                    UCPTRIE_FAST_U8_اگلا(normTrie, UCPTRIE_16, اگلا_Src, حد_بندی, n16);
                    اگر (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_معمول_Yes_Or_Maybe(n16);
                        اگر (پچھلا_CC_2 > cc) {
                            اگر (sink == خالی_اشاریہ) {
                                واپس جھوٹ;
                            }
                            توڑو;
                        }
                    } ورنہ {
                        توڑو;
                    }
                    src = اگلا_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                اگر (norm16HasCompBoundaryBefore(n16)) {
                    اگر (isCompYesAndZeroCC(n16)) {
                        src = اگلا_Src;
                    }
                    جاری;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        اگر (پچھلا_Boundary != پچھلا_Src && !norm16HasCompBoundaryBefore(norm16)) {
            ثابت uint8_t *p = پچھلا_Src;
            UCPTRIE_FAST_U8_پچھلا(normTrie, UCPTRIE_16, پچھلا_Boundary, p, norm16);
            اگر (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                پچھلا_Src = p;
            }
        }
        Reordering_بفر بفر(*یہ, s16, error_رمز);
        اگر (U_FAILURE(error_رمز)) {
            توڑو;
        }
        // We know there is not a boundary here.
        decomposeShort(پچھلا_Src, src, STOP_AT_حد_بندی, onlyContiguous,
                       بفر, error_رمز);
        // Decompose until the next boundary.
        src = decomposeShort(src, حد_بندی, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             بفر, error_رمز);
        اگر (U_FAILURE(error_رمز)) {
            توڑو;
        }
        اگر ((src - پچھلا_Src) > INT32_MAX) {  // guard before buffer.equals()
            error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس سچ;
        }
        recompose(بفر, 0, onlyContiguous);
        اگر (!بفر.equals(پچھلا_Src, src)) {
            اگر (sink == خالی_اشاریہ) {
                واپس جھوٹ;
            }
            اگر (پچھلا_Boundary != پچھلا_Src &&
                    !ByteSinkUtil::appendUnchanged(پچھلا_Boundary, پچھلا_Src,
                                                   *sink, options, edits, error_رمز)) {
                توڑو;
            }
            اگر (!ByteSinkUtil::appendChange(پچھلا_Src, src, بفر.get_آغاز(), بفر.length(),
                                            *sink, edits, error_رمز)) {
                توڑو;
            }
            پچھلا_Boundary = src;
        }
    }
    واپس سچ;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت char16_t *src, ثابت char16_t *حد_بندی) ثابت {
    اگر (src == حد_بندی || *src < minCompNoMaybeCP) {
        واپس سچ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_اگلا(normTrie, UCPTRIE_16, src, حد_بندی, c, norm16);
    واپس norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت uint8_t *src, ثابت uint8_t *حد_بندی) ثابت {
    اگر (src == حد_بندی) {
        واپس سچ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_اگلا(normTrie, UCPTRIE_16, src, حد_بندی, norm16);
    واپس norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت char16_t *آغاز, ثابت char16_t *p,
                                            UBool onlyContiguous) ثابت {
    اگر (آغاز == p) {
        واپس سچ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_پچھلا(normTrie, UCPTRIE_16, آغاز, p, c, norm16);
    واپس norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت uint8_t *آغاز, ثابت uint8_t *p,
                                            UBool onlyContiguous) ثابت {
    اگر (آغاز == p) {
        واپس سچ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_پچھلا(normTrie, UCPTRIE_16, آغاز, p, norm16);
    واپس norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ثابت char16_t *Normalizer2Impl::ڈھونڈو_پچھلا_Comp_Boundary(ثابت char16_t *آغاز, ثابت char16_t *p,
                                                       UBool onlyContiguous) ثابت {
    جب (p != آغاز) {
        ثابت char16_t *رمز_نقطہ_حد_بندی = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_پچھلا(normTrie, UCPTRIE_16, آغاز, p, c, norm16);
        اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            واپس رمز_نقطہ_حد_بندی;
        }
        اگر (hasCompBoundaryBefore(c, norm16)) {
            واپس p;
        }
    }
    واپس p;
}

ثابت char16_t *Normalizer2Impl::ڈھونڈو_اگلا_Comp_Boundary(ثابت char16_t *p, ثابت char16_t *حد_بندی,
                                                   UBool onlyContiguous) ثابت {
    جب (p != حد_بندی) {
        ثابت char16_t *رمز_نقطہ_آغاز = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_اگلا(normTrie, UCPTRIE_16, p, حد_بندی, c, norm16);
        اگر (hasCompBoundaryBefore(c, norm16)) {
            واپس رمز_نقطہ_آغاز;
        }
        اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            واپس p;
        }
    }
    واپس p;
}

uint8_t Normalizer2Impl::get_پچھلا_Trail_CC(ثابت char16_t *آغاز, ثابت char16_t *p) ثابت {
    اگر (آغاز == p) {
        واپس 0;
    }
    int32_t i = static_cast<int32_t>(p - آغاز);
    UChar32 c;
    U16_پچھلا(آغاز, 0, i, c);
    واپس static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::get_پچھلا_Trail_CC(ثابت uint8_t *آغاز, ثابت uint8_t *p) ثابت {
    اگر (آغاز == p) {
        واپس 0;
    }
    int32_t i = static_cast<int32_t>(p - آغاز);
    UChar32 c;
    U8_پچھلا(آغاز, 0, i, c);
    واپس static_cast<uint8_t>(getFCD16(c));
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
#اگر (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ہدایت optimize( "", off )
#اگر_ختم
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_ڈیٹا(UChar32 c) ثابت {
    uint16_t norm16=getNorm16(c);
    اگر (norm16 >= حد_بندی_No_No) {
        اگر(norm16>=MIN_معمول_MAYBE_YES) {
            // combining mark
            norm16=get_CCFrom_معمول_Yes_Or_Maybe(norm16);
            واپس norm16|(norm16<<8);
        } ورنہ اگر(norm16>=minMaybeYes) {
            واپس 0;
        } ورنہ اگر(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_نقاب;
            اگر (deltaTrailCC <= DELTA_TCCC_1) {
                واپس deltaTrailCC >> انحراف_سرکاؤ;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    اگر(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        واپس 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_ڈیٹا(norm16);
    uint16_t پہلا_Unit=*mapping;
    norm16=پہلا_Unit>>8;  // tccc
    اگر(پہلا_Unit&MAPPING_HAS_CCC_LCCC_لفظ) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    واپس norm16;
}
#اگر (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ہدایت optimize( "", on )
#اگر_ختم

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ثابت {
    U_ASSERT(norm16 >= minMaybeNo);
    اگر (norm16 >= MIN_معمول_MAYBE_YES) {
        // combining mark
        norm16 = get_CCFrom_معمول_Yes_Or_Maybe(norm16);
        واپس norm16 | (norm16<<8);
    } ورنہ اگر (norm16 >= minMaybeYes) {
        واپس 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping = get_ڈیٹا_For_Maybe(norm16);
    uint16_t پہلا_Unit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((پہلا_Unit & MAPPING_HAS_CCC_LCCC_لفظ) == 0 || (*(mapping - 1) & 0xff00) == 0);
    واپس پہلا_Unit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ثابت char16_t *
Normalizer2Impl::makeFCD(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                         Reordering_بفر *بفر,
                         UError_رمز &error_رمز) ثابت {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ثابت char16_t *پچھلا_Boundary=src;
    int32_t پچھلا_FCD16=0;
    اگر(حد_بندی==خالی_اشاریہ) {
        src=copy_کم_سابقہ_From_Nul_Terminated(src, minLcccCP, بفر, error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس src;
        }
        اگر(پچھلا_Boundary<src) {
            پچھلا_Boundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            پچھلا_FCD16=getFCD16(*(src-1));
            اگر(پچھلا_FCD16>1) {
                --پچھلا_Boundary;
            }
        }
        حد_بندی=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    ثابت char16_t *پچھلا_Src;
    UChar32 c=0;
    uint16_t fcd16=0;

    برائے(;;) {
        // count code units with lccc==0
        برائے(پچھلا_Src=src; src!=حد_بندی;) {
            اگر((c=*src)<minLcccCP) {
                پچھلا_FCD16=~c;
                ++src;
            } ورنہ اگر(!singleLeadMightHaveNonZeroFCD16(c)) {
                پچھلا_FCD16=0;
                ++src;
            } ورنہ {
                اگر(U16_IS_LEAD(c)) {
                    char16_t c2;
                    اگر((src+1)!=حد_بندی && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                اگر((fcd16=get_FCD16_From_Norm_ڈیٹا(c))<=0xff) {
                    پچھلا_FCD16=fcd16;
                    src+=U16_لمبائی(c);
                } ورنہ {
                    توڑو;
                }
            }
        }
        // copy these code units all at once
        اگر(src!=پچھلا_Src) {
            اگر(بفر!=خالی_اشاریہ && !بفر->appendZeroCC(پچھلا_Src, src, error_رمز)) {
                توڑو;
            }
            اگر(src==حد_بندی) {
                توڑو;
            }
            پچھلا_Boundary=src;
            // We know that the previous character's lccc==0.
            اگر(پچھلا_FCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 پچھلا=~پچھلا_FCD16;
                اگر(پچھلا<minDecompNoCP) {
                    پچھلا_FCD16=0;
                } ورنہ {
                    پچھلا_FCD16=get_FCD16_From_Norm_ڈیٹا(پچھلا);
                    اگر(پچھلا_FCD16>1) {
                        --پچھلا_Boundary;
                    }
                }
            } ورنہ {
                ثابت char16_t *p=src-1;
                اگر(U16_IS_TRAIL(*p) && پچھلا_Src<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    پچھلا_FCD16=get_FCD16_From_Norm_ڈیٹا(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                اگر(پچھلا_FCD16>1) {
                    پچھلا_Boundary=p;
                }
            }
            // The start of the current character (c).
            پچھلا_Src=src;
        } ورنہ اگر(src==حد_بندی) {
            توڑو;
        }

        src+=U16_لمبائی(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        اگر((پچھلا_FCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            اگر((fcd16&0xff)<=1) {
                پچھلا_Boundary=src;
            }
            اگر(بفر!=خالی_اشاریہ && !بفر->appendZeroCC(c, error_رمز)) {
                توڑو;
            }
            پچھلا_FCD16=fcd16;
            جاری;
        } ورنہ اگر(بفر==خالی_اشاریہ) {
            واپس پچھلا_Boundary;  // quick check "no"
        } ورنہ {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            بفر->removeSuffix(static_cast<int32_t>(پچھلا_Src - پچھلا_Boundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=ڈھونڈو_اگلا_FCDBoundary(src, حد_بندی);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(پچھلا_Boundary, src, جھوٹ, جھوٹ, *بفر, error_رمز);
            اگر (U_FAILURE(error_رمز)) {
                توڑو;
            }
            پچھلا_Boundary=src;
            پچھلا_FCD16=0;
        }
    }
    واپس src;
}

خالی Normalizer2Impl::makeFCDAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی,
                                       UBool doMakeFCD,
                                       UnicodeString &محفوظ_درمیان,
                                       Reordering_بفر &بفر,
                                       UError_رمز &error_رمز) ثابت {
    اگر(!بفر.is_خالی_خانے()) {
        ثابت char16_t *پہلا_Boundary_اندر_Src=ڈھونڈو_اگلا_FCDBoundary(src, حد_بندی);
        اگر(src!=پہلا_Boundary_اندر_Src) {
            ثابت char16_t *آخری_Boundary_اندر_Dest=ڈھونڈو_پچھلا_FCDBoundary(بفر.get_آغاز(),
                                                                    بفر.get_حد_بندی());
            int32_t dest_Suffix_لمبائی = static_cast<int32_t>(بفر.get_حد_بندی() - آخری_Boundary_اندر_Dest);
            UnicodeString درمیان(آخری_Boundary_اندر_Dest, dest_Suffix_لمبائی);
            بفر.removeSuffix(dest_Suffix_لمبائی);
            محفوظ_درمیان=درمیان;
            درمیان.append(src, static_cast<int32_t>(پہلا_Boundary_اندر_Src - src));
            ثابت char16_t *درمیان_آغاز=درمیان.get_بفر();
            makeFCD(درمیان_آغاز, درمیان_آغاز+درمیان.length(), &بفر, error_رمز);
            اگر(U_FAILURE(error_رمز)) {
                واپس;
            }
            src=پہلا_Boundary_اندر_Src;
        }
    }
    اگر(doMakeFCD) {
        makeFCD(src, حد_بندی, &بفر, error_رمز);
    } ورنہ {
        اگر(حد_بندی==خالی_اشاریہ) {  // appendZeroCC() needs limit!=nullptr
            حد_بندی=u_strchr(src, 0);
        }
        بفر.appendZeroCC(src, حد_بندی, error_رمز);
    }
}

ثابت char16_t *Normalizer2Impl::ڈھونڈو_پچھلا_FCDBoundary(ثابت char16_t *آغاز, ثابت char16_t *p) ثابت {
    جب(آغاز<p) {
        ثابت char16_t *رمز_نقطہ_حد_بندی = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_پچھلا(normTrie, UCPTRIE_16, آغاز, p, c, norm16);
        اگر (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            واپس رمز_نقطہ_حد_بندی;
        }
        اگر (norm16HasDecompBoundaryBefore(norm16)) {
            واپس p;
        }
    }
    واپس p;
}

ثابت char16_t *Normalizer2Impl::ڈھونڈو_اگلا_FCDBoundary(ثابت char16_t *p, ثابت char16_t *حد_بندی) ثابت {
    جب(p<حد_بندی) {
        ثابت char16_t *رمز_نقطہ_آغاز=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_اگلا(normTrie, UCPTRIE_16, p, حد_بندی, c, norm16);
        اگر (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            واپس رمز_نقطہ_آغاز;
        }
        اگر (norm16HasDecompBoundaryAfter(norm16)) {
            واپس p;
        }
    }
    واپس p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_ڈیٹا::Canon_Iter_ڈیٹا(UError_رمز &error_رمز) :
        mutableTrie(umutablecptrie_open(0, 0, &error_رمز)), trie(خالی_اشاریہ),
        canon_آغاز_Sets(uprv_deleteUObject, خالی_اشاریہ, error_رمز) {}

Canon_Iter_ڈیٹا::~Canon_Iter_ڈیٹا() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

خالی Canon_Iter_ڈیٹا::جوڑو_مقصد_آغاز_سیٹ(UChar32 origin, UChar32 decompLead, UError_رمز &error_رمز) {
    uint32_t canon_قدر = umutablecptrie_get(mutableTrie, decompLead);
    اگر((canon_قدر&(CANON_HAS_سیٹ|CANON_قدر_نقاب))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_سیٹ(mutableTrie, decompLead, canon_قدر|origin, &error_رمز);
    } ورنہ {
        // origin is not the first character, or it is U+0000.
        Unicode_سیٹ *سیٹ;
        اگر((canon_قدر&CANON_HAS_سیٹ)==0) {
            Local_اشارہ_گر<Unicode_سیٹ> lp_سیٹ(نیا Unicode_سیٹ, error_رمز);
            سیٹ=lp_سیٹ.getAlias();
            اگر(U_FAILURE(error_رمز)) {
                واپس;
            }
            UChar32 پہلا_Origin = static_cast<UChar32>(canon_قدر & CANON_قدر_نقاب);
            canon_قدر = (canon_قدر & ~CANON_قدر_نقاب) | CANON_HAS_سیٹ | static_cast<uint32_t>(canon_آغاز_Sets.size());
            umutablecptrie_سیٹ(mutableTrie, decompLead, canon_قدر, &error_رمز);
            canon_آغاز_Sets.adoptElement(lp_سیٹ.orphan(), error_رمز);
            اگر (U_FAILURE(error_رمز)) {
                واپس;
            }
            اگر(پہلا_Origin!=0) {
                سیٹ->جوڑو(پہلا_Origin);
            }
        } ورنہ {
            سیٹ = static_cast<Unicode_سیٹ*>(canon_آغاز_Sets[static_cast<int32_t>(canon_قدر & CANON_قدر_نقاب)]);
        }
        سیٹ->جوڑو(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
جماعت Init_Canon_Iter_ڈیٹا {
عوامی:
    ساکن خالی doInit(Normalizer2Impl *impl, UError_رمز &error_رمز);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ساکن خالی U_CALLCONV
init_Canon_Iter_ڈیٹا(Normalizer2Impl *impl, UError_رمز &error_رمز) {
    Init_Canon_Iter_ڈیٹا::doInit(impl, error_رمز);
}

U_CDECL_انتہا

خالی Init_Canon_Iter_ڈیٹا::doInit(Normalizer2Impl *impl, UError_رمز &error_رمز) {
    U_ASSERT(impl->f_Canon_Iter_ڈیٹا == خالی_اشاریہ);
    impl->f_Canon_Iter_ڈیٹا = نیا Canon_Iter_ڈیٹا(error_رمز);
    اگر (impl->f_Canon_Iter_ڈیٹا == خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
    }
    اگر (U_SUCCESS(error_رمز)) {
        UChar32 آغاز = 0, انتہا;
        uint32_t value;
        جب ((انتہا = ucptrie_getRange(impl->normTrie, آغاز,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       خالی_اشاریہ, خالی_اشاریہ, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            اگر (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_ڈیٹا_From_Norm16(آغاز, انتہا, value, *impl->f_Canon_Iter_ڈیٹا, error_رمز);
            }
            آغاز = انتہا + 1;
        }
#اگر_تعریف_شدہ UCPTRIE_DEBUG
        umutablecptrie_سیٹ_Name(impl->f_Canon_Iter_ڈیٹا->mutableTrie, "CanonIterData");
#اگر_ختم
        impl->f_Canon_Iter_ڈیٹا->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_ڈیٹا->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_قدر_بٹ_32, &error_رمز);
        umutablecptrie_close(impl->f_Canon_Iter_ڈیٹا->mutableTrie);
        impl->f_Canon_Iter_ڈیٹا->mutableTrie = خالی_اشاریہ;
    }
    اگر (U_FAILURE(error_رمز)) {
        حذف impl->f_Canon_Iter_ڈیٹا;
        impl->f_Canon_Iter_ڈیٹا = خالی_اشاریہ;
    }
}

خالی Normalizer2Impl::make_Canon_Iter_ڈیٹا_From_Norm16(UChar32 آغاز, UChar32 انتہا, ثابت uint16_t norm16,
                                                  Canon_Iter_ڈیٹا &new_ڈیٹا,
                                                  UError_رمز &error_رمز) ثابت {
    اگر(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        واپس;
    }
    برائے(UChar32 c=آغاز; c<=انتہا; ++c) {
        uint32_t old_قدر = umutablecptrie_get(new_ڈیٹا.mutableTrie, c);
        uint32_t new_قدر=old_قدر;
        اگر(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_قدر|=CANON_NOT_SEGMENT_STARTER;
            اگر(norm16<MIN_معمول_MAYBE_YES) {
                new_قدر|=CANON_HAS_COMPOSITIONS;
            }
        } ورنہ اگر(norm16<minYesNo) {
            new_قدر|=CANON_HAS_COMPOSITIONS;
        } ورنہ {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            اگر (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            اگر (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ثابت uint16_t *mapping=get_ڈیٹا_For_Yes_Or_No(norm16_2);
                uint16_t پہلا_Unit=*mapping;
                int32_t length=پہلا_Unit&MAPPING_لمبائی_نقاب;
                اگر((پہلا_Unit&MAPPING_HAS_CCC_LCCC_لفظ)!=0) {
                    اگر(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_قدر|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                اگر(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_اگلا_UNSAFE(mapping, i, c2);
                    new_ڈیٹا.جوڑو_مقصد_آغاز_سیٹ(c, c2, error_رمز);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    اگر(norm16_2>=minNoNo) {
                        جب(i<length) {
                            U16_اگلا_UNSAFE(mapping, i, c2);
                            uint32_t c2_قدر = umutablecptrie_get(new_ڈیٹا.mutableTrie, c2);
                            اگر((c2_قدر&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_سیٹ(new_ڈیٹا.mutableTrie, c2,
                                                   c2_قدر|CANON_NOT_SEGMENT_STARTER, &error_رمز);
                            }
                        }
                    }
                }
            } ورنہ {
                // c decomposed to c2 algorithmically; c has cc==0
                new_ڈیٹا.جوڑو_مقصد_آغاز_سیٹ(c, c2, error_رمز);
            }
        }
        اگر(new_قدر!=old_قدر) {
            umutablecptrie_سیٹ(new_ڈیٹا.mutableTrie, c, new_قدر, &error_رمز);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_ڈیٹا(UError_رمز &error_رمز) ثابت {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(یہ);
    umtx_initOnce(me->f_Canon_Iter_ڈیٹا_Init_Once, &init_Canon_Iter_ڈیٹا, me, error_رمز);
    واپس U_SUCCESS(error_رمز);
}

int32_t Normalizer2Impl::get_Canon_قدر(UChar32 c) ثابت {
    واپس static_cast<int32_t>(ucptrie_get(f_Canon_Iter_ڈیٹا->trie, c));
}

ثابت Unicode_سیٹ &Normalizer2Impl::get_Canon_آغاز_سیٹ(int32_t n) ثابت {
    واپس *static_cast<ثابت Unicode_سیٹ*>(f_Canon_Iter_ڈیٹا->canon_آغاز_Sets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ثابت {
    واپس get_Canon_قدر(c)>=0;
}

UBool Normalizer2Impl::get_Canon_آغاز_سیٹ(UChar32 c, Unicode_سیٹ &سیٹ) ثابت {
    int32_t canon_قدر=get_Canon_قدر(c)&~CANON_NOT_SEGMENT_STARTER;
    اگر(canon_قدر==0) {
        واپس جھوٹ;
    }
    سیٹ.clear();
    int32_t value=canon_قدر&CANON_قدر_نقاب;
    اگر((canon_قدر&CANON_HAS_سیٹ)!=0) {
        سیٹ.جوڑو_All(get_Canon_آغاز_سیٹ(value));
    } ورنہ اگر(value!=0) {
        سیٹ.جوڑو(value);
    }
    اگر((canon_قدر&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        اگر(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_بنیاد + (c - Hangul::JAMO_L_بنیاد) * Hangul::JAMO_VT_گنتی);
            سیٹ.جوڑو(syllable, syllable+Hangul::JAMO_VT_گنتی-1);
        } ورنہ {
            جوڑو_Composites(getCompositionsList(norm16), سیٹ);
        }
    }
    واپس سچ;
}

U_NAMESPACE_انتہا

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_تبادلہ(ثابت UDataSwapper *ds,
            ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
            UError_رمز *p_Error_رمز) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_سائز;

    ثابت uint8_t *اندر_بائٹیں;
    uint8_t *برآمد_بائٹیں;

    ثابت int32_t *اندر_Indexes;
    int32_t indexes[Normalizer2Impl::IX_کل_سائز+1];

    int32_t i, انحراف, اگلا_انحراف, size;

    /* udata_swapDataHeader checks the arguments */
    سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    uint8_t formatVersion0=pInfo->format_نسخہ[0];
    اگر(!(
        pInfo->ڈیٹا_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->ڈیٹا_Format[1]==0x72 &&
        pInfo->ڈیٹا_Format[2]==0x6d &&
        pInfo->ڈیٹا_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                         pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3],
                         pInfo->format_نسخہ[0]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اندر_بائٹیں=(ثابت uint8_t *)اندر_ڈیٹا+سرخی_سائز;
    برآمد_بائٹیں=(برآمد_ڈیٹا == خالی_اشاریہ) ? خالی_اشاریہ : (uint8_t *)برآمد_ڈیٹا+سرخی_سائز;

    اندر_Indexes=(ثابت int32_t *)اندر_بائٹیں;
    int32_t min_Indexes_لمبائی;
    اگر(formatVersion0==1) {
        min_Indexes_لمبائی=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } ورنہ اگر(formatVersion0==2) {
        min_Indexes_لمبائی=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } ورنہ اگر(formatVersion0<=4) {
        min_Indexes_لمبائی=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } ورنہ {
        min_Indexes_لمبائی=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    اگر(length>=0) {
        length-=سرخی_سائز;
        اگر(length<min_Indexes_لمبائی*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
    }

    /* read the first few indexes */
    برائے(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, اندر_Indexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_کل_سائز];

    اگر(length>=0) {
        اگر(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }

        /* copy the data for inaccessible bytes */
        اگر(اندر_بائٹیں!=برآمد_بائٹیں) {
            uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, size);
        }

        انحراف=0;

        /* swap the int32_t indexes[] */
        اگلا_انحراف=indexes[Normalizer2Impl::IX_NORM_TRIE_انحراف];
        ds->تبادلہ_Array32(ds, اندر_بائٹیں, اگلا_انحراف-انحراف, برآمد_بائٹیں, p_Error_رمز);
        انحراف=اگلا_انحراف;

        /* swap the trie */
        اگلا_انحراف=indexes[Normalizer2Impl::IX_EXTRA_ڈیٹا_انحراف];
        utrie_تبادلہ_Any_نسخہ(ds, اندر_بائٹیں+انحراف, اگلا_انحراف-انحراف, برآمد_بائٹیں+انحراف, p_Error_رمز);
        انحراف=اگلا_انحراف;

        /* swap the uint16_t extraData[] */
        اگلا_انحراف=indexes[Normalizer2Impl::IX_SMALL_FCD_انحراف];
        ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, اگلا_انحراف-انحراف, برآمد_بائٹیں+انحراف, p_Error_رمز);
        انحراف=اگلا_انحراف;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        اگلا_انحراف=indexes[Normalizer2Impl::IX_SMALL_FCD_انحراف+1];
        انحراف=اگلا_انحراف;

        U_ASSERT(انحراف==size);
    }

    واپس سرخی_سائز+size;
}

#اگر_ختم  // !UCONFIG_NO_NORMALIZATION
