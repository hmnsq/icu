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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/bytestream.h"
#ضمن "unicode/edits.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/stringoptions.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "unicode/utf8.h"
#ضمن "bytesinkutil.h"
#ضمن "cmemory.h"
#ضمن "mutex.h"
#ضمن "normalizer2impl.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"
#ضمن "uset_imp.h"
#ضمن "uvector.h"

U_NAMESPACE_BEGIN

نطاق {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
مضمن uint8_t leadByteForCP(UChar32 c) {
    إذا (c <= 0x7f) {
        أرجع static_cast<uint8_t>(c);
    } وإلا إذا (c <= 0x7ff) {
        أرجع static_cast<uint8_t>(0xc0 + (c >> 6));
    } وإلا {
        // Should not occur because ccc(U+0300)!=0.
        أرجع 0xe0;
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
UChar32 کوډ_ټکی_From_Valid_UTF8(ثابت uint8_t *cp_پیل, ثابت uint8_t *cp_حد) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cp_پیل < cp_حد);
    uint8_t c = *cp_پیل;
    بدل(cp_حد-cp_پیل) {
    حالة 1:
        أرجع c;
    حالة 2:
        أرجع ((c&0x1f)<<6) | (cp_پیل[1]&0x3f);
    حالة 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        أرجع static_cast<char16_t>((c << 12) | ((cp_پیل[1] & 0x3f) << 6) | (cp_پیل[2] & 0x3f));
    حالة 4:
        أرجع ((c&7)<<18) | ((cp_پیل[1]&0x3f)<<12) | ((cp_پیل[2]&0x3f)<<6) | (cp_پیل[3]&0x3f);
    افتراضي:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 پخوانی_Hangul_Or_Jamo(ثابت uint8_t *پیل, ثابت uint8_t *p) {
    إذا ((p - پیل) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        إذا (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            أرجع ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    أرجع U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t get_Jamo_TMinus_بنسټ(ثابت uint8_t *src, ثابت uint8_t *حد) {
    // Jamo T: E1 86 A8..E1 87 82
    إذا ((حد - src) >= 3 && *src == 0xe1) {
        إذا (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            إذا (0xa8 <= t && t <= 0xbf) {
                أرجع t - 0xa7;
            }
        } وإلا إذا (src[1] == 0x87) {
            uint8_t t = src[2];
            إذا (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                أرجع t - (0xa7 - 0x40);
            }
        }
    }
    أرجع -1;
}

فراغ
append_کوډ_ټکی_Delta(ثابت uint8_t *cp_پیل, ثابت uint8_t *cp_حد, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    حرف بفر[U8_MAX_اوږدتیا];
    int32_t length;
    int32_t cp_اوږدتیا = static_cast<int32_t>(cp_حد - cp_پیل);
    إذا (cp_اوږدتیا == 1) {
        // The builder makes ASCII map to ASCII.
        بفر[0] = static_cast<uint8_t>(*cp_پیل + delta);
        length = 1;
    } وإلا {
        int32_t trail = *(cp_حد-1) + delta;
        إذا (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cp_حد;
            length = 0;
            افعل { بفر[length++] = *cp_پیل++; } طالما (cp_پیل < cp_حد);
            بفر[length++] = static_cast<uint8_t>(trail);
        } وإلا {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = کوډ_ټکی_From_Valid_UTF8(cp_پیل, cp_حد) + delta;
            length = 0;
            U8_APPEND_UNSAFE(بفر, length, c);
        }
    }
    إذا (edits != تش_اشاره) {
        edits->جمع_کړه_Replace(cp_اوږدتیا, length);
    }
    sink.Append(بفر, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_بفر::Reordering_بفر(ثابت Normalizer2Impl &ni, UnicodeString &dest,
                                   UError_کوډ &error_کوډ) :
        impl(ni), str(dest),
        پیل(str.get_بفر(8)), reorder_پیل(پیل), حد(پیل),
        پاتې_شوی_ظرفیت(str.get_ظرفیت()), وروستی_CC(0) {
    إذا (پیل == تش_اشاره && U_SUCCESS(error_کوډ)) {
        // getBuffer() already did str.setToBogus()
        error_کوډ = U_حافظه_ویش_ERROR;
    }
}

UBool Reordering_بفر::init(int32_t dest_ظرفیت, UError_کوډ &error_کوډ) {
    int32_t length=str.length();
    پیل=str.get_بفر(dest_ظرفیت);
    إذا(پیل==تش_اشاره) {
        // getBuffer() already did str.setToBogus()
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع خطأ;
    }
    حد=پیل+length;
    پاتې_شوی_ظرفیت=str.get_ظرفیت()-length;
    reorder_پیل=پیل;
    إذا(پیل==حد) {
        وروستی_CC=0;
    } وإلا {
        سېټ_Iterator();
        وروستی_CC=پخوانی_CC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        إذا(وروستی_CC>1) {
            طالما(پخوانی_CC()>1) {}
        }
        reorder_پیل=کوډ_ټکی_حد;
    }
    أرجع ریښتیا;
}

UBool Reordering_بفر::equals(ثابت char16_t *other_پیل, ثابت char16_t *other_حد) ثابت {
    int32_t length = static_cast<int32_t>(حد - پیل);
    أرجع
        length == static_cast<int32_t>(other_حد - other_پیل) &&
        0==u_memcmp(پیل, other_پیل, length);
}

UBool Reordering_بفر::equals(ثابت uint8_t *other_پیل, ثابت uint8_t *other_حد) ثابت {
    U_ASSERT((other_حد - other_پیل) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(حد - پیل);
    int32_t other_اوږدتیا = static_cast<int32_t>(other_حد - other_پیل);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    إذا (other_اوږدتیا < length || (other_اوږدتیا / 3) > length) {
        أرجع خطأ;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    لكل (int32_t i = 0, j = 0;;) {
        إذا (i >= length) {
            أرجع j >= other_اوږدتیا;
        } وإلا إذا (j >= other_اوږدتیا) {
            أرجع خطأ;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_بل_UNSAFE(پیل, i, c);
        U8_بل_UNSAFE(other_پیل, j, other);
        إذا (c != other) {
            أرجع خطأ;
        }
    }
}

UBool Reordering_بفر::appendSupplementary(UChar32 c, uint8_t cc, UError_کوډ &error_کوډ) {
    إذا(پاتې_شوی_ظرفیت<2 && !resize(2, error_کوډ)) {
        أرجع خطأ;
    }
    إذا(وروستی_CC<=cc || cc==0) {
        حد[0]=U16_LEAD(c);
        حد[1]=U16_TRAIL(c);
        حد+=2;
        وروستی_CC=cc;
        إذا(cc<=1) {
            reorder_پیل=حد;
        }
    } وإلا {
        insert(c, cc);
    }
    پاتې_شوی_ظرفیت-=2;
    أرجع ریښتیا;
}

UBool Reordering_بفر::append(ثابت char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UError_کوډ &error_کوډ) {
    إذا(length==0) {
        أرجع ریښتیا;
    }
    إذا(پاتې_شوی_ظرفیت<length && !resize(length, error_کوډ)) {
        أرجع خطأ;
    }
    پاتې_شوی_ظرفیت-=length;
    إذا(وروستی_CC<=leadCC || leadCC==0) {
        إذا(trailCC<=1) {
            reorder_پیل=حد+length;
        } وإلا إذا(leadCC<=1) {
            reorder_پیل=حد+1;  // Ok if not a code point boundary.
        }
        ثابت char16_t *s_حد=s+length;
        افعل { *حد++=*s++; } طالما(s!=s_حد);
        وروستی_CC=trailCC;
    } وإلا {
        int32_t i=0;
        UChar32 c;
        U16_بل(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        طالما(i<length) {
            U16_بل(s, i, length, c);
            إذا(i<length) {
                إذا (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } وإلا {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } وإلا {
                leadCC=trailCC;
            }
            append(c, leadCC, error_کوډ);
        }
    }
    أرجع ریښتیا;
}

UBool Reordering_بفر::appendZeroCC(UChar32 c, UError_کوډ &error_کوډ) {
    int32_t cp_اوږدتیا=U16_اوږدتیا(c);
    إذا(پاتې_شوی_ظرفیت<cp_اوږدتیا && !resize(cp_اوږدتیا, error_کوډ)) {
        أرجع خطأ;
    }
    پاتې_شوی_ظرفیت-=cp_اوږدتیا;
    إذا(cp_اوږدتیا==1) {
        *حد++ = static_cast<char16_t>(c);
    } وإلا {
        حد[0]=U16_LEAD(c);
        حد[1]=U16_TRAIL(c);
        حد+=2;
    }
    وروستی_CC=0;
    reorder_پیل=حد;
    أرجع ریښتیا;
}

UBool Reordering_بفر::appendZeroCC(ثابت char16_t *s, ثابت char16_t *s_حد, UError_کوډ &error_کوډ) {
    إذا(s==s_حد) {
        أرجع ریښتیا;
    }
    int32_t length = static_cast<int32_t>(s_حد - s);
    إذا(پاتې_شوی_ظرفیت<length && !resize(length, error_کوډ)) {
        أرجع خطأ;
    }
    u_memcpy(حد, s, length);
    حد+=length;
    پاتې_شوی_ظرفیت-=length;
    وروستی_CC=0;
    reorder_پیل=حد;
    أرجع ریښتیا;
}

فراغ Reordering_بفر::remove() {
    reorder_پیل=حد=پیل;
    پاتې_شوی_ظرفیت=str.get_ظرفیت();
    وروستی_CC=0;
}

فراغ Reordering_بفر::removeSuffix(int32_t suffix_اوږدتیا) {
    إذا(suffix_اوږدتیا<(حد-پیل)) {
        حد-=suffix_اوږدتیا;
        پاتې_شوی_ظرفیت+=suffix_اوږدتیا;
    } وإلا {
        حد=پیل;
        پاتې_شوی_ظرفیت=str.get_ظرفیت();
    }
    وروستی_CC=0;
    reorder_پیل=حد;
}

UBool Reordering_بفر::resize(int32_t append_اوږدتیا, UError_کوډ &error_کوډ) {
    int32_t reorder_پیل_شاخص = static_cast<int32_t>(reorder_پیل - پیل);
    int32_t length = static_cast<int32_t>(حد - پیل);
    str.release_بفر(length);
    int32_t new_ظرفیت=length+append_اوږدتیا;
    int32_t double_ظرفیت=2*str.get_ظرفیت();
    إذا(new_ظرفیت<double_ظرفیت) {
        new_ظرفیت=double_ظرفیت;
    }
    إذا(new_ظرفیت<256) {
        new_ظرفیت=256;
    }
    پیل=str.get_بفر(new_ظرفیت);
    إذا(پیل==تش_اشاره) {
        // getBuffer() already did str.setToBogus()
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع خطأ;
    }
    reorder_پیل=پیل+reorder_پیل_شاخص;
    حد=پیل+length;
    پاتې_شوی_ظرفیت=str.get_ظرفیت()-length;
    أرجع ریښتیا;
}

فراغ Reordering_بفر::skip_پخوانی() {
    کوډ_ټکی_حد=کوډ_ټکی_پیل;
    char16_t c=*--کوډ_ټکی_پیل;
    إذا(U16_IS_TRAIL(c) && پیل<کوډ_ټکی_پیل && U16_IS_LEAD(*(کوډ_ټکی_پیل-1))) {
        --کوډ_ټکی_پیل;
    }
}

uint8_t Reordering_بفر::پخوانی_CC() {
    کوډ_ټکی_حد=کوډ_ټکی_پیل;
    إذا(reorder_پیل>=کوډ_ټکی_پیل) {
        أرجع 0;
    }
    UChar32 c=*--کوډ_ټکی_پیل;
    char16_t c2;
    إذا(U16_IS_TRAIL(c) && پیل<کوډ_ټکی_پیل && U16_IS_LEAD(c2=*(کوډ_ټکی_پیل-1))) {
        --کوډ_ټکی_پیل;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    أرجع impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
فراغ Reordering_بفر::insert(UChar32 c, uint8_t cc) {
    لكل(سېټ_Iterator(), skip_پخوانی(); پخوانی_CC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=حد;
    char16_t *r=حد+=U16_اوږدتیا(c);
    افعل {
        *--r=*--q;
    } طالما(کوډ_ټکی_حد!=q);
    write_کوډ_ټکی(q, c);
    إذا(cc<=1) {
        reorder_پیل=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

بنية Canon_Iter_معلومات : عام UMemory {
    Canon_Iter_معلومات(UError_کوډ &error_کوډ);
    ~Canon_Iter_معلومات();
    فراغ جمع_کړه_منزل_پیل_سېټ(UChar32 origin, UChar32 decompLead, UError_کوډ &error_کوډ);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canon_پیل_Sets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    ړنګول f_Canon_Iter_معلومات;
}

فراغ
Normalizer2Impl::init(ثابت int32_t *دننه_Indexes, ثابت UCPTrie *دننه_Trie,
                      ثابت uint16_t *دننه_Extra_معلومات, ثابت uint8_t *دننه_Small_FCD) {
    minDecompNoCP = static_cast<char16_t>(دننه_Indexes[IX_MIN_DECOMP_NO_CP]);
    minCompNoMaybeCP = static_cast<char16_t>(دننه_Indexes[IX_MIN_COMP_NO_MAYBE_CP]);
    minLcccCP = static_cast<char16_t>(دننه_Indexes[IX_MIN_LCCC_CP]);

    minYesNo = static_cast<uint16_t>(دننه_Indexes[IX_MIN_YES_NO]);
    minYesNoMappingsOnly = static_cast<uint16_t>(دننه_Indexes[IX_MIN_YES_NO_MAPPINGS_ONLY]);
    minNoNo = static_cast<uint16_t>(دننه_Indexes[IX_MIN_NO_NO]);
    minNoNoCompBoundaryBefore = static_cast<uint16_t>(دننه_Indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE]);
    minNoNoCompNoMaybeCC = static_cast<uint16_t>(دننه_Indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC]);
    min_No_No_تشه = static_cast<uint16_t>(دننه_Indexes[IX_MIN_NO_NO_تشه]);
    حد_No_No = static_cast<uint16_t>(دننه_Indexes[IX_حد_NO_NO]);
    minMaybeNo = static_cast<uint16_t>(دننه_Indexes[IX_MIN_MAYBE_NO]);
    minMaybeNoCombinesFwd = static_cast<uint16_t>(دننه_Indexes[IX_MIN_MAYBE_NO_COMBINES_FWD]);
    minMaybeYes = static_cast<uint16_t>(دننه_Indexes[IX_MIN_MAYBE_YES]);
    U_ASSERT((minMaybeNo & 7) == 0);  // 8-aligned for noNoDelta bit fields
    centerNoNoDelta = (minMaybeNo >> DELTA_ښويدنه) - MAX_DELTA - 1;

    normTrie=دننه_Trie;
    extra_معلومات=دننه_Extra_معلومات;
    smallFCD=دننه_Small_FCD;
}

U_CDECL_BEGIN

ساكن uint32_t U_CALLCONV
segmentStarterMapper(ثابت فراغ * /*context*/, uint32_t value) {
    أرجع value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_پای_ټکی

فراغ
Normalizer2Impl::جمع_کړه_Lccc_Chars(Unicode_سېټ &سېټ) ثابت {
    UChar32 پیل = 0, پای_ټکی;
    uint32_t norm16;
    طالما ((پای_ټکی = ucptrie_getRange(normTrie, پیل, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   تش_اشاره, تش_اشاره, &norm16)) >= 0) {
        إذا (norm16 > Normalizer2Impl::MIN_نورمال_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            سېټ.جمع_کړه(پیل, پای_ټکی);
        } وإلا إذا (minNoNoCompNoMaybeCC <= norm16 && norm16 < حد_No_No) {
            uint16_t fcd16 = getFCD16(پیل);
            إذا (fcd16 > 0xff) { سېټ.جمع_کړه(پیل, پای_ټکی); }
        }
        پیل = پای_ټکی + 1;
    }
}

فراغ
Normalizer2Impl::جمع_کړه_Property_Starts(ثابت USetAdder *sa, UError_کوډ & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 پیل = 0, پای_ټکی;
    uint32_t value;
    طالما ((پای_ټکی = ucptrie_getRange(normTrie, پیل, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   تش_اشاره, تش_اشاره, &value)) >= 0) {
        sa->جمع_کړه(sa->سېټ, پیل);
        إذا (پیل != پای_ټکی && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_ماسک) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(پیل);
            طالما (++پیل <= پای_ټکی) {
                uint16_t fcd16 = getFCD16(پیل);
                إذا (fcd16 != prevFCD16) {
                    sa->جمع_کړه(sa->سېټ, پیل);
                    prevFCD16 = fcd16;
                }
            }
        }
        پیل = پای_ټکی + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    لكل(char16_t c=Hangul::HANGUL_بنسټ; c<Hangul::HANGUL_حد; c+=Hangul::JAMO_T_شمېر) {
        sa->جمع_کړه(sa->سېټ, c);
        sa->جمع_کړه(sa->سېټ, c+1);
    }
    sa->جمع_کړه(sa->سېټ, Hangul::HANGUL_حد); /* add Hangul+1 to continue with other properties */
}

فراغ
Normalizer2Impl::جمع_کړه_Canon_Iter_Property_Starts(ثابت USetAdder *sa, UError_کوډ &error_کوډ) ثابت {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    إذا (!ensure_Canon_Iter_معلومات(error_کوډ)) { أرجع; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 پیل = 0, پای_ټکی;
    uint32_t value;
    طالما ((پای_ټکی = ucptrie_getRange(f_Canon_Iter_معلومات->trie, پیل, UCPMAP_RANGE_نورمال, 0,
                                   segmentStarterMapper, تش_اشاره, &value)) >= 0) {
        sa->جمع_کړه(sa->سېټ, پیل);
        پیل = پای_ټکی + 1;
    }
}

ثابت char16_t *
Normalizer2Impl::copy_ټیټ_مختاړی_From_Nul_Terminated(ثابت char16_t *src,
                                                UChar32 min_Need_معلومات_CP,
                                                Reordering_بفر *بفر,
                                                UError_کوډ &error_کوډ) ثابت {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ثابت char16_t *prevSrc=src;
    char16_t c;
    طالما((c=*src++)<min_Need_معلومات_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    إذا(--src!=prevSrc) {
        إذا(بفر!=تش_اشاره) {
            بفر->appendZeroCC(prevSrc, src, error_کوډ);
        }
    }
    أرجع src;
}

UnicodeString &
Normalizer2Impl::decompose(ثابت UnicodeString &src, UnicodeString &dest,
                           UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        dest.سېټ_منزل_Bogus();
        أرجع dest;
    }
    ثابت char16_t *sArray=src.get_بفر();
    إذا(&dest==&src || sArray==تش_اشاره) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        dest.سېټ_منزل_Bogus();
        أرجع dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), error_کوډ);
    أرجع dest;
}

فراغ
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *حد,
                           UnicodeString &dest,
                           int32_t dest_اوږدتیا_Estimate,
                           UError_کوډ &error_کوډ) ثابت {
    إذا(dest_اوږدتیا_Estimate<0 && حد!=تش_اشاره) {
        dest_اوږدتیا_Estimate = static_cast<int32_t>(حد - src);
    }
    dest.remove();
    Reordering_بفر بفر(*هذا, dest);
    إذا(بفر.init(dest_اوږدتیا_Estimate, error_کوډ)) {
        decompose(src, حد, &بفر, error_کوډ);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ثابت char16_t *
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *حد,
                           Reordering_بفر *بفر,
                           UError_کوډ &error_کوډ) ثابت {
    إذا(src==تش_اشاره) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع src;
    }
    UChar32 minNoCP=minDecompNoCP;
    إذا(حد==تش_اشاره) {
        src=copy_ټیټ_مختاړی_From_Nul_Terminated(src, minNoCP, بفر, error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع src;
        }
        حد=u_strchr(src, 0);
    }

    ثابت char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ثابت char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    لكل(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        لكل(prevSrc=src; src!=حد;) {
            إذا( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } وإلا إذا(!U16_IS_LEAD(c)) {
                اكسر;
            } وإلا {
                char16_t c2;
                إذا((src+1)!=حد && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    إذا(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } وإلا {
                        اكسر;
                    }
                } وإلا {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        إذا(src!=prevSrc) {
            إذا(بفر!=تش_اشاره) {
                إذا(!بفر->appendZeroCC(prevSrc, src, error_کوډ)) {
                    اكسر;
                }
            } وإلا {
                prevCC=0;
                prevBoundary=src;
            }
        }
        إذا(src==حد) {
            اكسر;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_اوږدتیا(c);
        إذا(بفر!=تش_اشاره) {
            إذا(!decompose(c, norm16, *بفر, error_کوډ)) {
                اكسر;
            }
        } وإلا {
            إذا(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                إذا(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    إذا(cc<=1) {
                        prevBoundary=src;
                    }
                    استمر;
                }
            }
            أرجع prevBoundary;  // "no" or cc out of order
        }
    }
    أرجع src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ثابت char16_t *
Normalizer2Impl::decomposeShort(ثابت char16_t *src, ثابت char16_t *حد,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    طالما(src<حد) {
        إذا (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            أرجع src;
        }
        ثابت char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_بل(normTrie, UCPTRIE_16, src, حد, c, norm16);
        إذا (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            أرجع prevSrc;
        }
        إذا(!decompose(c, norm16, بفر, error_کوډ)) {
            أرجع تش_اشاره;
        }
        إذا (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            أرجع src;
        }
    }
    أرجع src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_بفر &بفر,
                                 UError_کوډ &error_کوډ) ثابت {
    // get the decomposition and the lead and trail cc's
    إذا (norm16 >= حد_No_No) {
        إذا (isMaybeYesOrNonZeroCC(norm16)) {
            أرجع بفر.append(c, getCCFromYesOrMaybeYes(norm16), error_کوډ);
        } وإلا إذا (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    إذا (norm16 < minYesNo) {
        // c does not decompose
        أرجع بفر.append(c, 0, error_کوډ);
    } وإلا إذا(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        أرجع بفر.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_کوډ);
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_معلومات(norm16);
    uint16_t لومړی_Unit=*mapping;
    int32_t length=لومړی_Unit&MAPPING_اوږدتیا_ماسک;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(لومړی_Unit >> 8);
    إذا(لومړی_Unit&MAPPING_HAS_CCC_LCCC_کلمه) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } وإلا {
        leadCC=0;
    }
    أرجع بفر.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, ریښتیا, leadCC, trailCC, error_کوډ);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ثابت uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ثابت uint8_t *src, ثابت uint8_t *حد,
                               ByteSink *sink, Edits *edits, UError_کوډ &error_کوډ) ثابت {
    U_ASSERT(حد != تش_اشاره);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ثابت uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    لكل (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ثابت uint8_t *fast_پیل = src;
        ثابت uint8_t *prevSrc;
        uint16_t norm16 = 0;

        لكل (;;) {
            إذا (src == حد) {
                إذا (prevBoundary != حد && sink != تش_اشاره) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, حد,
                                                  *sink, options, edits, error_کوډ);
                }
                أرجع src;
            }
            إذا (*src < minNoLead) {
                ++src;
            } وإلا {
                prevSrc = src;
                UCPTRIE_FAST_U8_بل(normTrie, UCPTRIE_16, src, حد, norm16);
                إذا (!isMostDecompYesAndZeroCC(norm16)) {
                    اكسر;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        إذا (prevSrc != fast_پیل) {
            // The fast path looped over yes/0 characters before the current one.
            إذا (sink != تش_اشاره &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_کوډ)) {
                اكسر;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        إذا (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            إذا (prevCC <= cc || cc == 0) {
                prevCC = cc;
                إذا (cc <= 1) {
                    إذا (sink != تش_اشاره &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, error_کوډ)) {
                        اكسر;
                    }
                    prevBoundary = src;
                }
                استمر;
            }
        }
        إذا (sink == تش_اشاره) {
            أرجع prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        إذا (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            إذا (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, error_کوډ)) {
                اكسر;
            }
            prevBoundary = prevSrc;
        }
        Reordering_بفر بفر(*هذا, s16, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) {
            اكسر;
        }
        decomposeShort(prevBoundary, src, STOP_AT_حد, خطأ /* onlyContiguous */,
                       بفر, error_کوډ);
        // Decompose until the next boundary.
        إذا (بفر.get_وروستی_CC() > 1) {
            src = decomposeShort(src, حد, STOP_AT_DECOMP_BOUNDARY, خطأ /* onlyContiguous */,
                                 بفر, error_کوډ);
        }
        إذا (U_FAILURE(error_کوډ)) {
            اكسر;
        }
        إذا ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
            اكسر;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        إذا (isMaybeYesOrNonZeroCC(norm16) && بفر.equals(prevBoundary, src)) {
            إذا (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, error_کوډ)) {
                اكسر;
            }
        } وإلا {
            إذا (!ByteSinkUtil::appendChange(prevBoundary, src, بفر.get_پیل(), بفر.length(),
                                            *sink, edits, error_کوډ)) {
                اكسر;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    أرجع src;
}

ثابت uint8_t *
Normalizer2Impl::decomposeShort(ثابت uint8_t *src, ثابت uint8_t *حد,
                                StopAt stopAt, UBool onlyContiguous,
                                Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    طالما (src < حد) {
        ثابت uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_بل(normTrie, UCPTRIE_16, src, حد, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        إذا (norm16 >= حد_No_No) {
            إذا (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                إذا (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    أرجع prevSrc;
                }
                c = کوډ_ټکی_From_Valid_UTF8(prevSrc, src);
                إذا (!بفر.append(c, cc, error_کوډ)) {
                    أرجع تش_اشاره;
                }
                إذا (stopAt == STOP_AT_DECOMP_BOUNDARY && بفر.get_وروستی_CC() <= 1) {
                    أرجع src;
                }
                استمر;
            } وإلا إذا (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                إذا (stopAt != STOP_AT_حد) {
                    أرجع prevSrc;
                }
                c = کوډ_ټکی_From_Valid_UTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } وإلا إذا (stopAt != STOP_AT_حد && norm16 < minNoNoCompNoMaybeCC) {
            أرجع prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        إذا (norm16 < minYesNo) {
            إذا (c < 0) {
                c = کوډ_ټکی_From_Valid_UTF8(prevSrc, src);
            }
            // does not decompose
            إذا (!بفر.append(c, 0, error_کوډ)) {
                أرجع تش_اشاره;
            }
        } وإلا إذا (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            إذا (c < 0) {
                c = کوډ_ټکی_From_Valid_UTF8(prevSrc, src);
            }
            char16_t jamos[3];
            إذا (!بفر.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_کوډ)) {
                أرجع تش_اشاره;
            }
        } وإلا {
            // The character decomposes, get everything from the variable-length extra data.
            ثابت uint16_t *mapping = get_معلومات(norm16);
            uint16_t لومړی_Unit = *mapping;
            int32_t length = لومړی_Unit & MAPPING_اوږدتیا_ماسک;
            uint8_t trailCC = static_cast<uint8_t>(لومړی_Unit >> 8);
            uint8_t leadCC;
            إذا (لومړی_Unit & MAPPING_HAS_CCC_LCCC_کلمه) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } وإلا {
                leadCC = 0;
            }
            إذا (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                أرجع prevSrc;
            }
            إذا (!بفر.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, ریښتیا, leadCC, trailCC, error_کوډ)) {
                أرجع تش_اشاره;
            }
        }
        إذا ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && بفر.get_وروستی_CC() <= 1)) {
            أرجع src;
        }
    }
    أرجع src;
}

ثابت char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t بفر[4], int32_t &length) ثابت {
    uint16_t norm16;
    إذا(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        أرجع تش_اشاره;
    }
    ثابت char16_t *decomp = تش_اشاره;
    إذا(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=بفر;
        length=0;
        U16_APPEND_UNSAFE(بفر, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    إذا (norm16 < minYesNo) {
        أرجع decomp;
    } وإلا إذا(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, بفر);
        أرجع بفر;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_معلومات(norm16);
    length=*mapping&MAPPING_اوږدتیا_ماسک;
    أرجع reinterpret_cast<ثابت char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ثابت char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t بفر[30], int32_t &length) ثابت {
    uint16_t norm16;
    إذا(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        أرجع تش_اشاره;
    } وإلا إذا(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, بفر);
        length=2;
        أرجع بفر;
    } وإلا إذا(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(بفر, length, c);
        أرجع بفر;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_معلومات(norm16);
    uint16_t لومړی_Unit=*mapping;
    int32_t m_اوږدتیا=لومړی_Unit&MAPPING_اوږدتیا_ماسک;  // length of normal mapping
    إذا(لومړی_Unit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ثابت uint16_t *rawMapping=mapping-((لومړی_Unit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        إذا(rm0<=MAPPING_اوږدتیا_ماسک) {
            length=rm0;
            أرجع reinterpret_cast<ثابت char16_t*>(rawMapping) - rm0;
        } وإلا {
            // Copy the normal mapping and replace its first two code units with rm0.
            بفر[0] = static_cast<char16_t>(rm0);
            u_memcpy(بفر + 1, reinterpret_cast<ثابت char16_t*>(mapping) + 1 + 2, m_اوږدتیا - 2);
            length=m_اوږدتیا-1;
            أرجع بفر;
        }
    } وإلا {
        length=m_اوږدتیا;
        أرجع reinterpret_cast<ثابت char16_t*>(mapping) + 1;
    }
}

فراغ Normalizer2Impl::decomposeAndAppend(ثابت char16_t *src, ثابت char16_t *حد,
                                         UBool doDecompose,
                                         UnicodeString &خوندي_منځ,
                                         Reordering_بفر &بفر,
                                         UError_کوډ &error_کوډ) ثابت {
    بفر.copy_Reorderable_Suffix_منزل(خوندي_منځ);
    إذا(doDecompose) {
        decompose(src, حد, &بفر, error_کوډ);
        أرجع;
    }
    // Just merge the strings at the boundary.
    منطقي is_لومړی = ریښتیا;
    uint8_t لومړی_CC = 0, prevCC = 0, cc;
    ثابت char16_t *p = src;
    طالما (p != حد) {
        ثابت char16_t *کوډ_ټکی_پیل = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_بل(normTrie, UCPTRIE_16, p, حد, c, norm16);
        إذا ((cc = getCC(norm16)) == 0) {
            p = کوډ_ټکی_پیل;
            اكسر;
        }
        إذا (is_لومړی) {
            لومړی_CC = cc;
            is_لومړی = خطأ;
        }
        prevCC = cc;
    }
    إذا(حد==تش_اشاره) {  // appendZeroCC() needs limit!=nullptr
        حد=u_strchr(p, 0);
    }

    إذا (بفر.append(src, static_cast<int32_t>(p - src), خطأ, لومړی_CC, prevCC, error_کوډ)) {
        بفر.appendZeroCC(p, حد, error_کوډ);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ثابت {
    أرجع c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ثابت {
    إذا (norm16 < minNoNoCompNoMaybeCC) {
        أرجع ریښتیا;
    }
    إذا (norm16 >= حد_No_No) {
        أرجع norm16 <= MIN_نورمال_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_معلومات_For_Yes_Or_No(norm16);
    uint16_t لومړی_Unit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    أرجع (لومړی_Unit&MAPPING_HAS_CCC_LCCC_کلمه)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ثابت {
    إذا (c < minDecompNoCP) {
        أرجع ریښتیا;
    }
    إذا (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        أرجع ریښتیا;
    }
    أرجع norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ثابت {
    إذا(norm16 <= minYesNo || isHangulLVT(norm16)) {
        أرجع ریښتیا;
    }
    إذا (norm16 >= حد_No_No) {
        إذا (isMaybeYesOrNonZeroCC(norm16)) {
            أرجع norm16 <= MIN_نورمال_MAYBE_YES || norm16 == JAMO_VT;
        } وإلا إذا (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            أرجع (norm16 & DELTA_TCCC_ماسک) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_معلومات(norm16);
    uint16_t لومړی_Unit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    إذا(لومړی_Unit>0x1ff) {
        أرجع خطأ;  // trailCC>1
    }
    إذا(لومړی_Unit<=0xff) {
        أرجع ریښتیا;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    أرجع (لومړی_Unit&MAPPING_HAS_CCC_LCCC_کلمه)==0 || (*(mapping-1)&0xff00)==0;
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
    uint16_t key1, لومړی_Unit;
    إذا(trail<COMP_1_TRAIL_حد) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        طالما(key1>(لومړی_Unit=*list)) {
            list+=2+(لومړی_Unit&COMP_1_TRIPLE);
        }
        إذا(key1==(لومړی_Unit&COMP_1_TRAIL_ماسک)) {
            إذا(لومړی_Unit&COMP_1_TRIPLE) {
                أرجع (static_cast<int32_t>(list[1]) << 16) | list[2];
            } وإلا {
                أرجع list[1];
            }
        }
    } وإلا {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_حد +
                        (((trail>>COMP_1_TRAIL_ښويدنه))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_ښويدنه);
        uint16_t دوهم_Unit;
        لكل(;;) {
            إذا(key1>(لومړی_Unit=*list)) {
                list+=2+(لومړی_Unit&COMP_1_TRIPLE);
            } وإلا إذا(key1==(لومړی_Unit&COMP_1_TRAIL_ماسک)) {
                إذا(key2>(دوهم_Unit=list[1])) {
                    إذا(لومړی_Unit&COMP_1_وروستی_TUPLE) {
                        اكسر;
                    } وإلا {
                        list+=3;
                    }
                } وإلا إذا(key2==(دوهم_Unit&COMP_2_TRAIL_ماسک)) {
                    أرجع (static_cast<int32_t>(دوهم_Unit & ~COMP_2_TRAIL_ماسک) << 16) | list[2];
                } وإلا {
                    اكسر;
                }
            } وإلا {
                اكسر;
            }
        }
    }
    أرجع -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
فراغ Normalizer2Impl::جمع_کړه_Composites(ثابت uint16_t *list, Unicode_سېټ &سېټ) ثابت {
    uint16_t لومړی_Unit;
    int32_t مرکب_And_Fwd;
    افعل {
        لومړی_Unit=*list;
        إذا((لومړی_Unit&COMP_1_TRIPLE)==0) {
            مرکب_And_Fwd=list[1];
            list+=2;
        } وإلا {
            مرکب_And_Fwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_ماسک) << 16) | list[2];
            list+=3;
        }
        UChar32 مرکب=مرکب_And_Fwd>>1;
        إذا((مرکب_And_Fwd&1)!=0) {
            جمع_کړه_Composites(get_Compositions_List_For_مرکب(getRawNorm16(مرکب)), سېټ);
        }
        سېټ.جمع_کړه(مرکب);
    } طالما((لومړی_Unit&COMP_1_وروستی_TUPLE)==0);
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
فراغ Normalizer2Impl::recompose(Reordering_بفر &بفر, int32_t recompose_پیل_شاخص,
                                UBool onlyContiguous) ثابت {
    char16_t *p=بفر.get_پیل()+recompose_پیل_شاخص;
    char16_t *حد=بفر.get_حد();
    إذا(p==حد) {
        أرجع;
    }

    char16_t *starter, *pRemove, *q, *r;
    ثابت uint16_t *compositionsList;
    UChar32 c, مرکب_And_Fwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=تش_اشاره;  // used as indicator for whether we have a forward-combining starter
    starter=تش_اشاره;
    starterIsSupplementary=خطأ;
    prevCC=0;

    لكل(;;) {
        UCPTRIE_FAST_U16_بل(normTrie, UCPTRIE_16, p, حد, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        إذا( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=تش_اشاره &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            إذا(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                إذا(c<Hangul::JAMO_T_بنسټ) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_بنسټ);
                    إذا(prev<Hangul::JAMO_L_شمېر) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_بنسټ +
                             (prev*Hangul::JAMO_V_شمېر+(c-Hangul::JAMO_V_بنسټ))*
                             Hangul::JAMO_T_شمېر);
                        char16_t t;
                        إذا (p != حد && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_بنسټ)) < Hangul::JAMO_T_شمېر) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        طالما(r<حد) {
                            *q++=*r++;
                        }
                        حد=q;
                        p=pRemove;
                    }
                }
                /*
                 * No "else" for Jamo T:
                 * Since the input is in NFD, there are no Hangul LV syllables that
                 * a Jamo T could combine with.
                 * All Jamo Ts are combined above when handling Jamo Vs.
                 */
                إذا(p==حد) {
                    اكسر;
                }
                compositionsList=تش_اشاره;
                استمر;
            } وإلا إذا((مرکب_And_Fwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 مرکب=مرکب_And_Fwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_اوږدتیا(c);  // pRemove & p: start & limit of the combining mark
                إذا(starterIsSupplementary) {
                    إذا(U_IS_SUPPLEMENTARY(مرکب)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(مرکب);
                        starter[1]=U16_TRAIL(مرکب);
                    } وإلا {
                        *starter = static_cast<char16_t>(مرکب);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=خطأ;
                        q=starter+1;
                        r=q+1;
                        طالما(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } وإلا إذا(U_IS_SUPPLEMENTARY(مرکب)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=ریښتیا;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    طالما(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(مرکب);
                    *--starter=U16_LEAD(مرکب);  // undo the temporary increment
                } وإلا {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(مرکب);
                }

                /* remove the combining mark by moving the following text over it */
                إذا(pRemove<p) {
                    q=pRemove;
                    r=p;
                    طالما(r<حد) {
                        *q++=*r++;
                    }
                    حد=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                إذا(p==حد) {
                    اكسر;
                }
                // Is the composite a starter that combines forward?
                إذا(مرکب_And_Fwd&1) {
                    compositionsList=
                        get_Compositions_List_For_مرکب(getRawNorm16(مرکب));
                } وإلا {
                    compositionsList=تش_اشاره;
                }

                // We combined; continue with looking for compositions.
                استمر;
            }
        }

        // no combination this time
        prevCC=cc;
        إذا(p==حد) {
            اكسر;
        }

        // If c did not combine, then check if it is a starter.
        إذا(cc==0) {
            // Found a new starter.
            إذا((compositionsList=getCompositionsListForDecompYes(norm16))!=تش_اشاره) {
                // It may combine with something, prepare for it.
                إذا(U_IS_BMP(c)) {
                    starterIsSupplementary=خطأ;
                    starter=p-1;
                } وإلا {
                    starterIsSupplementary=ریښتیا;
                    starter=p-2;
                }
            }
        } وإلا إذا(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=تش_اشاره;
        }
    }
    بفر.سېټ_Reordering_حد(حد);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ثابت {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ثابت uint16_t *list;
    إذا(isInert(norm16)) {
        أرجع U_SENTINEL;
    } وإلا إذا(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        إذا(isJamoL(norm16)) {
            إذا (b < Hangul::JAMO_V_بنسټ) {
                أرجع U_SENTINEL;
            }
            b-=Hangul::JAMO_V_بنسټ;
            إذا(b<Hangul::JAMO_V_شمېر) {
                أرجع
                    (Hangul::HANGUL_بنسټ+
                     ((a-Hangul::JAMO_L_بنسټ)*Hangul::JAMO_V_شمېر+b)*
                     Hangul::JAMO_T_شمېر);
            } وإلا {
                أرجع U_SENTINEL;
            }
        } وإلا إذا(isHangulLV(norm16)) {
            إذا (b <= Hangul::JAMO_T_بنسټ) {
               أرجع U_SENTINEL;
            }
            b-=Hangul::JAMO_T_بنسټ;
            إذا(b<Hangul::JAMO_T_شمېر) {  // not b==0!
                أرجع a+b;
            } وإلا {
                أرجع U_SENTINEL;
            }
        } وإلا {
            // 'a' has a compositions list in extraData
            list=get_معلومات_For_Yes_Or_No(norm16);
            إذا(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_اوږدتیا_ماسک);  // + mapping length
            }
        }
    } وإلا إذا(norm16<minMaybeNoCombinesFwd || MIN_نورمال_MAYBE_YES<=norm16) {
        أرجع U_SENTINEL;
    } وإلا {
        list=get_معلومات_For_Maybe(norm16);
        إذا(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_اوږدتیا_ماسک);  // + mapping length
        }
    }
    إذا(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        أرجع U_SENTINEL;
    }
#إذا U_SIGNED_ښی_ښويدنه_IS_ARITHMETIC
    أرجع combine(list, b)>>1;
#وإلا
    int32_t مرکب_And_Fwd=combine(list, b);
    أرجع مرکب_And_Fwd>=0 ? مرکب_And_Fwd>>1 : U_SENTINEL;
#د_که_پای
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ثابت char16_t *src, ثابت char16_t *حد,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_بفر &بفر,
                         UError_کوډ &error_کوډ) ثابت {
    ثابت char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    إذا(حد==تش_اشاره) {
        src=copy_ټیټ_مختاړی_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &بفر : تش_اشاره,
                                           error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع خطأ;
        }
        حد=u_strchr(src, 0);
        إذا (prevBoundary != src) {
            إذا (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } وإلا {
                بفر.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    لكل (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        لكل (;;) {
            إذا (src == حد) {
                إذا (prevBoundary != حد && doCompose) {
                    بفر.appendZeroCC(prevBoundary, حد, error_کوډ);
                }
                أرجع ریښتیا;
            }
            إذا( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } وإلا {
                prevSrc = src++;
                إذا(!U16_IS_LEAD(c)) {
                    اكسر;
                } وإلا {
                    char16_t c2;
                    إذا(src!=حد && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        إذا(!isCompYesAndZeroCC(norm16)) {
                            اكسر;
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
        إذا (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            إذا (!doCompose) {
                أرجع خطأ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            إذا (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد)) {
                    إذا (prevBoundary != prevSrc && !بفر.appendZeroCC(prevBoundary, prevSrc, error_کوډ)) {
                        اكسر;
                    }
                    إذا(!بفر.append(mapAlgorithmic(c, norm16), 0, error_کوډ)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } وإلا إذا (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد)) {
                    إذا (prevBoundary != prevSrc && !بفر.appendZeroCC(prevBoundary, prevSrc, error_کوډ)) {
                        اكسر;
                    }
                    ثابت char16_t *mapping = reinterpret_cast<ثابت char16_t *>(get_معلومات_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_اوږدتیا_ماسک;
                    إذا(!بفر.appendZeroCC(mapping, mapping + length, error_کوډ)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } وإلا إذا (norm16 >= min_No_No_تشه) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                إذا (hasCompBoundaryBefore(src, حد) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    إذا (prevBoundary != prevSrc && !بفر.appendZeroCC(prevBoundary, prevSrc, error_کوډ)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } وإلا إذا (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            إذا(c<Hangul::JAMO_T_بنسټ) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_بنسټ);
                إذا(l<Hangul::JAMO_L_شمېر) {
                    إذا (!doCompose) {
                        أرجع خطأ;
                    }
                    int32_t t;
                    إذا (src != حد &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_بنسټ)) &&
                            t < Hangul::JAMO_T_شمېر) {
                        // The next character is a Jamo T.
                        ++src;
                    } وإلا إذا (hasCompBoundaryBefore(src, حد)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } وإلا {
                        t = -1;
                    }
                    إذا (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_بنسټ +
                            (l*Hangul::JAMO_V_شمېر + (c-Hangul::JAMO_V_بنسټ)) *
                            Hangul::JAMO_T_شمېر + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        إذا (prevBoundary != prevSrc && !بفر.appendZeroCC(prevBoundary, prevSrc, error_کوډ)) {
                            اكسر;
                        }
                        إذا (!بفر.appendBMP(static_cast<char16_t>(syllable), 0, error_کوډ)) {
                            اكسر;
                        }
                        prevBoundary = src;
                        استمر;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } وإلا إذا (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                إذا (!doCompose) {
                    أرجع خطأ;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_بنسټ;
                --prevSrc;  // Replace the Hangul LV as well.
                إذا (prevBoundary != prevSrc && !بفر.appendZeroCC(prevBoundary, prevSrc, error_کوډ)) {
                    اكسر;
                }
                إذا (!بفر.appendBMP(static_cast<char16_t>(syllable), 0, error_کوډ)) {
                    اكسر;
                }
                prevBoundary = src;
                استمر;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } وإلا إذا (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_نورمال_Yes_Or_Maybe(norm16);  // cc!=0
            إذا (onlyContiguous /* FCC */ && get_پخوانی_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                إذا (!doCompose) {
                    أرجع خطأ;
                }
            } وإلا {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *بل_Src;
                uint16_t n16;
                لكل (;;) {
                    إذا (src == حد) {
                        إذا (doCompose) {
                            بفر.appendZeroCC(prevBoundary, حد, error_کوډ);
                        }
                        أرجع ریښتیا;
                    }
                    uint8_t prevCC = cc;
                    بل_Src = src;
                    UCPTRIE_FAST_U16_بل(normTrie, UCPTRIE_16, بل_Src, حد, c, n16);
                    إذا (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_نورمال_Yes_Or_Maybe(n16);
                        إذا (prevCC > cc) {
                            إذا (!doCompose) {
                                أرجع خطأ;
                            }
                            اكسر;
                        }
                    } وإلا {
                        اكسر;
                    }
                    src = بل_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                إذا (norm16HasCompBoundaryBefore(n16)) {
                    إذا (isCompYesAndZeroCC(n16)) {
                        src = بل_Src;
                    }
                    استمر;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        إذا (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ثابت char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            إذا (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        إذا (doCompose && prevBoundary != prevSrc && !بفر.appendZeroCC(prevBoundary, prevSrc, error_کوډ)) {
            اكسر;
        }
        int32_t recompose_پیل_شاخص=بفر.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, خطأ /* !stopAtCompBoundary */, onlyContiguous,
                       بفر, error_کوډ);
        // Decompose until the next boundary.
        src = decomposeShort(src, حد, ریښتیا /* stopAtCompBoundary */, onlyContiguous,
                             بفر, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) {
            اكسر;
        }
        إذا ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع ریښتیا;
        }
        recompose(بفر, recompose_پیل_شاخص, onlyContiguous);
        إذا(!doCompose) {
            إذا(!بفر.equals(prevSrc, src)) {
                أرجع خطأ;
            }
            بفر.remove();
        }
        prevBoundary=src;
    }
    أرجع ریښتیا;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ثابت char16_t *
Normalizer2Impl::compose_چټک_Check(ثابت char16_t *src, ثابت char16_t *حد,
                                   UBool onlyContiguous,
                                   UNormalization_Check_پایله *pQCResult) ثابت {
    ثابت char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    إذا(حد==تش_اشاره) {
        UError_کوډ error_کوډ=U_ZERO_ERROR;
        src=copy_ټیټ_مختاړی_From_Nul_Terminated(src, minNoMaybeCP, تش_اشاره, error_کوډ);
        حد=u_strchr(src, 0);
        إذا (prevBoundary != src) {
            إذا (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } وإلا {
                prevBoundary = --src;
            }
        }
    }

    لكل(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        لكل (;;) {
            إذا(src==حد) {
                أرجع src;
            }
            إذا( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } وإلا {
                prevSrc = src++;
                إذا(!U16_IS_LEAD(c)) {
                    اكسر;
                } وإلا {
                    char16_t c2;
                    إذا(src!=حد && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        إذا(!isCompYesAndZeroCC(norm16)) {
                            اكسر;
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
        إذا (prevBoundary != prevSrc) {
            إذا (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } وإلا {
                ثابت char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                إذا (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } وإلا {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        إذا (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            إذا (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } وإلا {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *بل_Src;
                لكل (;;) {
                    إذا (norm16 < MIN_YES_YES_WITH_CC) {
                        إذا (pQCResult != تش_اشاره) {
                            *pQCResult = UNORM_MAYBE;
                        } وإلا {
                            أرجع prevBoundary;
                        }
                    }
                    إذا (src == حد) {
                        أرجع src;
                    }
                    uint8_t prevCC = fcd16;
                    بل_Src = src;
                    UCPTRIE_FAST_U16_بل(normTrie, UCPTRIE_16, بل_Src, حد, c, norm16);
                    إذا (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        إذا (!(prevCC <= cc || cc == 0)) {
                            اكسر;
                        }
                    } وإلا {
                        اكسر;
                    }
                    src = بل_Src;
                }
                // src is after the last in-order combining mark.
                إذا (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = بل_Src;
                    استمر;
                }
            }
        }
        إذا(pQCResult!=تش_اشاره) {
            *pQCResult=UNORM_NO;
        }
        أرجع prevBoundary;
    }
}

فراغ Normalizer2Impl::composeAndAppend(ثابت char16_t *src, ثابت char16_t *حد,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &خوندي_منځ,
                                       Reordering_بفر &بفر,
                                       UError_کوډ &error_کوډ) ثابت {
    إذا(!بفر.is_تشه()) {
        ثابت char16_t *لومړی_Starter_دننه_Src=پیدا_کړه_بل_Comp_Boundary(src, حد, onlyContiguous);
        إذا(src!=لومړی_Starter_دننه_Src) {
            ثابت char16_t *وروستی_Starter_دننه_Dest=پیدا_کړه_پخوانی_Comp_Boundary(بفر.get_پیل(),
                                                                    بفر.get_حد(), onlyContiguous);
            int32_t dest_Suffix_اوږدتیا = static_cast<int32_t>(بفر.get_حد() - وروستی_Starter_دننه_Dest);
            UnicodeString منځ(وروستی_Starter_دننه_Dest, dest_Suffix_اوږدتیا);
            بفر.removeSuffix(dest_Suffix_اوږدتیا);
            خوندي_منځ=منځ;
            منځ.append(src, static_cast<int32_t>(لومړی_Starter_دننه_Src - src));
            ثابت char16_t *منځ_پیل=منځ.get_بفر();
            compose(منځ_پیل, منځ_پیل+منځ.length(), onlyContiguous,
                    ریښتیا, بفر, error_کوډ);
            إذا(U_FAILURE(error_کوډ)) {
                أرجع;
            }
            src=لومړی_Starter_دننه_Src;
        }
    }
    إذا(doCompose) {
        compose(src, حد, onlyContiguous, ریښتیا, بفر, error_کوډ);
    } وإلا {
        إذا(حد==تش_اشاره) {  // appendZeroCC() needs limit!=nullptr
            حد=u_strchr(src, 0);
        }
        بفر.appendZeroCC(src, حد, error_کوډ);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ثابت uint8_t *src, ثابت uint8_t *حد,
                             ByteSink *sink, Edits *edits, UError_کوډ &error_کوډ) ثابت {
    U_ASSERT(حد != تش_اشاره);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ثابت uint8_t *prevBoundary = src;

    لكل (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت uint8_t *prevSrc;
        uint16_t norm16 = 0;
        لكل (;;) {
            إذا (src == حد) {
                إذا (prevBoundary != حد && sink != تش_اشاره) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, حد,
                                                  *sink, options, edits, error_کوډ);
                }
                أرجع ریښتیا;
            }
            إذا (*src < minNoMaybeLead) {
                ++src;
            } وإلا {
                prevSrc = src;
                UCPTRIE_FAST_U8_بل(normTrie, UCPTRIE_16, src, حد, norm16);
                إذا (!isCompYesAndZeroCC(norm16)) {
                    اكسر;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        إذا (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            إذا (sink == تش_اشاره) {
                أرجع خطأ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            إذا (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد)) {
                    إذا (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_کوډ)) {
                        اكسر;
                    }
                    append_کوډ_ټکی_Delta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    استمر;
                }
            } وإلا إذا (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد)) {
                    إذا (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_کوډ)) {
                        اكسر;
                    }
                    ثابت uint16_t *mapping = get_معلومات_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_اوږدتیا_ماسک;
                    إذا (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ثابت char16_t*>(mapping), length,
                                                    *sink, edits, error_کوډ)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } وإلا إذا (norm16 >= min_No_No_تشه) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                إذا (hasCompBoundaryBefore(src, حد) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    إذا (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_کوډ)) {
                        اكسر;
                    }
                    إذا (edits != تش_اشاره) {
                        edits->جمع_کړه_Replace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    استمر;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } وإلا إذا (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = پخوانی_Hangul_Or_Jamo(prevBoundary, prevSrc);
            إذا (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_بنسټ;
                إذا (static_cast<uint32_t>(l) < Hangul::JAMO_L_شمېر) {
                    إذا (sink == تش_اشاره) {
                        أرجع خطأ;
                    }
                    int32_t t = get_Jamo_TMinus_بنسټ(src, حد);
                    إذا (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } وإلا إذا (hasCompBoundaryBefore(src, حد)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    إذا (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_بنسټ +
                            (l*Hangul::JAMO_V_شمېر + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_شمېر + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        إذا (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, error_کوډ)) {
                            اكسر;
                        }
                        ByteSinkUtil::append_کوډ_ټکی(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        استمر;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } وإلا إذا (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                إذا (sink == تش_اشاره) {
                    أرجع خطأ;
                }
                UChar32 syllable = prev + get_Jamo_TMinus_بنسټ(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                إذا (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, error_کوډ)) {
                    اكسر;
                }
                ByteSinkUtil::append_کوډ_ټکی(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                استمر;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } وإلا إذا (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_نورمال_Yes_Or_Maybe(norm16);  // cc!=0
            إذا (onlyContiguous /* FCC */ && get_پخوانی_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                إذا (sink == تش_اشاره) {
                    أرجع خطأ;
                }
            } وإلا {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت uint8_t *بل_Src;
                uint16_t n16;
                لكل (;;) {
                    إذا (src == حد) {
                        إذا (sink != تش_اشاره) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, حد,
                                                          *sink, options, edits, error_کوډ);
                        }
                        أرجع ریښتیا;
                    }
                    uint8_t prevCC = cc;
                    بل_Src = src;
                    UCPTRIE_FAST_U8_بل(normTrie, UCPTRIE_16, بل_Src, حد, n16);
                    إذا (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_نورمال_Yes_Or_Maybe(n16);
                        إذا (prevCC > cc) {
                            إذا (sink == تش_اشاره) {
                                أرجع خطأ;
                            }
                            اكسر;
                        }
                    } وإلا {
                        اكسر;
                    }
                    src = بل_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                إذا (norm16HasCompBoundaryBefore(n16)) {
                    إذا (isCompYesAndZeroCC(n16)) {
                        src = بل_Src;
                    }
                    استمر;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        إذا (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ثابت uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            إذا (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        Reordering_بفر بفر(*هذا, s16, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) {
            اكسر;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_حد, onlyContiguous,
                       بفر, error_کوډ);
        // Decompose until the next boundary.
        src = decomposeShort(src, حد, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             بفر, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) {
            اكسر;
        }
        إذا ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع ریښتیا;
        }
        recompose(بفر, 0, onlyContiguous);
        إذا (!بفر.equals(prevSrc, src)) {
            إذا (sink == تش_اشاره) {
                أرجع خطأ;
            }
            إذا (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_کوډ)) {
                اكسر;
            }
            إذا (!ByteSinkUtil::appendChange(prevSrc, src, بفر.get_پیل(), بفر.length(),
                                            *sink, edits, error_کوډ)) {
                اكسر;
            }
            prevBoundary = src;
        }
    }
    أرجع ریښتیا;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت char16_t *src, ثابت char16_t *حد) ثابت {
    إذا (src == حد || *src < minCompNoMaybeCP) {
        أرجع ریښتیا;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_بل(normTrie, UCPTRIE_16, src, حد, c, norm16);
    أرجع norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت uint8_t *src, ثابت uint8_t *حد) ثابت {
    إذا (src == حد) {
        أرجع ریښتیا;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_بل(normTrie, UCPTRIE_16, src, حد, norm16);
    أرجع norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت char16_t *پیل, ثابت char16_t *p,
                                            UBool onlyContiguous) ثابت {
    إذا (پیل == p) {
        أرجع ریښتیا;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, پیل, p, c, norm16);
    أرجع norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت uint8_t *پیل, ثابت uint8_t *p,
                                            UBool onlyContiguous) ثابت {
    إذا (پیل == p) {
        أرجع ریښتیا;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, پیل, p, norm16);
    أرجع norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ثابت char16_t *Normalizer2Impl::پیدا_کړه_پخوانی_Comp_Boundary(ثابت char16_t *پیل, ثابت char16_t *p,
                                                       UBool onlyContiguous) ثابت {
    طالما (p != پیل) {
        ثابت char16_t *کوډ_ټکی_حد = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, پیل, p, c, norm16);
        إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            أرجع کوډ_ټکی_حد;
        }
        إذا (hasCompBoundaryBefore(c, norm16)) {
            أرجع p;
        }
    }
    أرجع p;
}

ثابت char16_t *Normalizer2Impl::پیدا_کړه_بل_Comp_Boundary(ثابت char16_t *p, ثابت char16_t *حد,
                                                   UBool onlyContiguous) ثابت {
    طالما (p != حد) {
        ثابت char16_t *کوډ_ټکی_پیل = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_بل(normTrie, UCPTRIE_16, p, حد, c, norm16);
        إذا (hasCompBoundaryBefore(c, norm16)) {
            أرجع کوډ_ټکی_پیل;
        }
        إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            أرجع p;
        }
    }
    أرجع p;
}

uint8_t Normalizer2Impl::get_پخوانی_Trail_CC(ثابت char16_t *پیل, ثابت char16_t *p) ثابت {
    إذا (پیل == p) {
        أرجع 0;
    }
    int32_t i = static_cast<int32_t>(p - پیل);
    UChar32 c;
    U16_PREV(پیل, 0, i, c);
    أرجع static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::get_پخوانی_Trail_CC(ثابت uint8_t *پیل, ثابت uint8_t *p) ثابت {
    إذا (پیل == p) {
        أرجع 0;
    }
    int32_t i = static_cast<int32_t>(p - پیل);
    UChar32 c;
    U8_PREV(پیل, 0, i, c);
    أرجع static_cast<uint8_t>(getFCD16(c));
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
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#لارښوونه optimize( "", off )
#د_که_پای
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_معلومات(UChar32 c) ثابت {
    uint16_t norm16=getNorm16(c);
    إذا (norm16 >= حد_No_No) {
        إذا(norm16>=MIN_نورمال_MAYBE_YES) {
            // combining mark
            norm16=get_CCFrom_نورمال_Yes_Or_Maybe(norm16);
            أرجع norm16|(norm16<<8);
        } وإلا إذا(norm16>=minMaybeYes) {
            أرجع 0;
        } وإلا إذا(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_ماسک;
            إذا (deltaTrailCC <= DELTA_TCCC_1) {
                أرجع deltaTrailCC >> دننه_واټن_ښويدنه;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    إذا(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        أرجع 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_معلومات(norm16);
    uint16_t لومړی_Unit=*mapping;
    norm16=لومړی_Unit>>8;  // tccc
    إذا(لومړی_Unit&MAPPING_HAS_CCC_LCCC_کلمه) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    أرجع norm16;
}
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#لارښوونه optimize( "", on )
#د_که_پای

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ثابت {
    U_ASSERT(norm16 >= minMaybeNo);
    إذا (norm16 >= MIN_نورمال_MAYBE_YES) {
        // combining mark
        norm16 = get_CCFrom_نورمال_Yes_Or_Maybe(norm16);
        أرجع norm16 | (norm16<<8);
    } وإلا إذا (norm16 >= minMaybeYes) {
        أرجع 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping = get_معلومات_For_Maybe(norm16);
    uint16_t لومړی_Unit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((لومړی_Unit & MAPPING_HAS_CCC_LCCC_کلمه) == 0 || (*(mapping - 1) & 0xff00) == 0);
    أرجع لومړی_Unit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ثابت char16_t *
Normalizer2Impl::makeFCD(ثابت char16_t *src, ثابت char16_t *حد,
                         Reordering_بفر *بفر,
                         UError_کوډ &error_کوډ) ثابت {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ثابت char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    إذا(حد==تش_اشاره) {
        src=copy_ټیټ_مختاړی_From_Nul_Terminated(src, minLcccCP, بفر, error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع src;
        }
        إذا(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            إذا(prevFCD16>1) {
                --prevBoundary;
            }
        }
        حد=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    ثابت char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    لكل(;;) {
        // count code units with lccc==0
        لكل(prevSrc=src; src!=حد;) {
            إذا((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } وإلا إذا(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } وإلا {
                إذا(U16_IS_LEAD(c)) {
                    char16_t c2;
                    إذا((src+1)!=حد && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                إذا((fcd16=get_FCD16_From_Norm_معلومات(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_اوږدتیا(c);
                } وإلا {
                    اكسر;
                }
            }
        }
        // copy these code units all at once
        إذا(src!=prevSrc) {
            إذا(بفر!=تش_اشاره && !بفر->appendZeroCC(prevSrc, src, error_کوډ)) {
                اكسر;
            }
            إذا(src==حد) {
                اكسر;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            إذا(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                إذا(prev<minDecompNoCP) {
                    prevFCD16=0;
                } وإلا {
                    prevFCD16=get_FCD16_From_Norm_معلومات(prev);
                    إذا(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } وإلا {
                ثابت char16_t *p=src-1;
                إذا(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=get_FCD16_From_Norm_معلومات(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                إذا(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } وإلا إذا(src==حد) {
            اكسر;
        }

        src+=U16_اوږدتیا(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        إذا((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            إذا((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            إذا(بفر!=تش_اشاره && !بفر->appendZeroCC(c, error_کوډ)) {
                اكسر;
            }
            prevFCD16=fcd16;
            استمر;
        } وإلا إذا(بفر==تش_اشاره) {
            أرجع prevBoundary;  // quick check "no"
        } وإلا {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            بفر->removeSuffix(static_cast<int32_t>(prevSrc - prevBoundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=پیدا_کړه_بل_FCDBoundary(src, حد);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(prevBoundary, src, خطأ, خطأ, *بفر, error_کوډ);
            إذا (U_FAILURE(error_کوډ)) {
                اكسر;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    أرجع src;
}

فراغ Normalizer2Impl::makeFCDAndAppend(ثابت char16_t *src, ثابت char16_t *حد,
                                       UBool doMakeFCD,
                                       UnicodeString &خوندي_منځ,
                                       Reordering_بفر &بفر,
                                       UError_کوډ &error_کوډ) ثابت {
    إذا(!بفر.is_تشه()) {
        ثابت char16_t *لومړی_Boundary_دننه_Src=پیدا_کړه_بل_FCDBoundary(src, حد);
        إذا(src!=لومړی_Boundary_دننه_Src) {
            ثابت char16_t *وروستی_Boundary_دننه_Dest=پیدا_کړه_پخوانی_FCDBoundary(بفر.get_پیل(),
                                                                    بفر.get_حد());
            int32_t dest_Suffix_اوږدتیا = static_cast<int32_t>(بفر.get_حد() - وروستی_Boundary_دننه_Dest);
            UnicodeString منځ(وروستی_Boundary_دننه_Dest, dest_Suffix_اوږدتیا);
            بفر.removeSuffix(dest_Suffix_اوږدتیا);
            خوندي_منځ=منځ;
            منځ.append(src, static_cast<int32_t>(لومړی_Boundary_دننه_Src - src));
            ثابت char16_t *منځ_پیل=منځ.get_بفر();
            makeFCD(منځ_پیل, منځ_پیل+منځ.length(), &بفر, error_کوډ);
            إذا(U_FAILURE(error_کوډ)) {
                أرجع;
            }
            src=لومړی_Boundary_دننه_Src;
        }
    }
    إذا(doMakeFCD) {
        makeFCD(src, حد, &بفر, error_کوډ);
    } وإلا {
        إذا(حد==تش_اشاره) {  // appendZeroCC() needs limit!=nullptr
            حد=u_strchr(src, 0);
        }
        بفر.appendZeroCC(src, حد, error_کوډ);
    }
}

ثابت char16_t *Normalizer2Impl::پیدا_کړه_پخوانی_FCDBoundary(ثابت char16_t *پیل, ثابت char16_t *p) ثابت {
    طالما(پیل<p) {
        ثابت char16_t *کوډ_ټکی_حد = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, پیل, p, c, norm16);
        إذا (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            أرجع کوډ_ټکی_حد;
        }
        إذا (norm16HasDecompBoundaryBefore(norm16)) {
            أرجع p;
        }
    }
    أرجع p;
}

ثابت char16_t *Normalizer2Impl::پیدا_کړه_بل_FCDBoundary(ثابت char16_t *p, ثابت char16_t *حد) ثابت {
    طالما(p<حد) {
        ثابت char16_t *کوډ_ټکی_پیل=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_بل(normTrie, UCPTRIE_16, p, حد, c, norm16);
        إذا (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            أرجع کوډ_ټکی_پیل;
        }
        إذا (norm16HasDecompBoundaryAfter(norm16)) {
            أرجع p;
        }
    }
    أرجع p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_معلومات::Canon_Iter_معلومات(UError_کوډ &error_کوډ) :
        mutableTrie(umutablecptrie_open(0, 0, &error_کوډ)), trie(تش_اشاره),
        canon_پیل_Sets(uprv_deleteUObject, تش_اشاره, error_کوډ) {}

Canon_Iter_معلومات::~Canon_Iter_معلومات() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

فراغ Canon_Iter_معلومات::جمع_کړه_منزل_پیل_سېټ(UChar32 origin, UChar32 decompLead, UError_کوډ &error_کوډ) {
    uint32_t canon_ارزښت = umutablecptrie_get(mutableTrie, decompLead);
    إذا((canon_ارزښت&(CANON_HAS_سېټ|CANON_ارزښت_ماسک))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_سېټ(mutableTrie, decompLead, canon_ارزښت|origin, &error_کوډ);
    } وإلا {
        // origin is not the first character, or it is U+0000.
        Unicode_سېټ *سېټ;
        إذا((canon_ارزښت&CANON_HAS_سېټ)==0) {
            Local_اشاره_ګر<Unicode_سېټ> lp_سېټ(جديد Unicode_سېټ, error_کوډ);
            سېټ=lp_سېټ.getAlias();
            إذا(U_FAILURE(error_کوډ)) {
                أرجع;
            }
            UChar32 لومړی_Origin = static_cast<UChar32>(canon_ارزښت & CANON_ارزښت_ماسک);
            canon_ارزښت = (canon_ارزښت & ~CANON_ارزښت_ماسک) | CANON_HAS_سېټ | static_cast<uint32_t>(canon_پیل_Sets.size());
            umutablecptrie_سېټ(mutableTrie, decompLead, canon_ارزښت, &error_کوډ);
            canon_پیل_Sets.adoptElement(lp_سېټ.orphan(), error_کوډ);
            إذا (U_FAILURE(error_کوډ)) {
                أرجع;
            }
            إذا(لومړی_Origin!=0) {
                سېټ->جمع_کړه(لومړی_Origin);
            }
        } وإلا {
            سېټ = static_cast<Unicode_سېټ*>(canon_پیل_Sets[static_cast<int32_t>(canon_ارزښت & CANON_ارزښت_ماسک)]);
        }
        سېټ->جمع_کړه(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
صنف Init_Canon_Iter_معلومات {
عام:
    ساكن فراغ doInit(Normalizer2Impl *impl, UError_کوډ &error_کوډ);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ساكن فراغ U_CALLCONV
init_Canon_Iter_معلومات(Normalizer2Impl *impl, UError_کوډ &error_کوډ) {
    Init_Canon_Iter_معلومات::doInit(impl, error_کوډ);
}

U_CDECL_پای_ټکی

فراغ Init_Canon_Iter_معلومات::doInit(Normalizer2Impl *impl, UError_کوډ &error_کوډ) {
    U_ASSERT(impl->f_Canon_Iter_معلومات == تش_اشاره);
    impl->f_Canon_Iter_معلومات = جديد Canon_Iter_معلومات(error_کوډ);
    إذا (impl->f_Canon_Iter_معلومات == تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
    }
    إذا (U_SUCCESS(error_کوډ)) {
        UChar32 پیل = 0, پای_ټکی;
        uint32_t value;
        طالما ((پای_ټکی = ucptrie_getRange(impl->normTrie, پیل,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       تش_اشاره, تش_اشاره, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            إذا (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_معلومات_From_Norm16(پیل, پای_ټکی, value, *impl->f_Canon_Iter_معلومات, error_کوډ);
            }
            پیل = پای_ټکی + 1;
        }
#که_تعریف_شوی UCPTRIE_DEBUG
        umutablecptrie_سېټ_Name(impl->f_Canon_Iter_معلومات->mutableTrie, "CanonIterData");
#د_که_پای
        impl->f_Canon_Iter_معلومات->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_معلومات->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_ارزښت_بټونه_32, &error_کوډ);
        umutablecptrie_close(impl->f_Canon_Iter_معلومات->mutableTrie);
        impl->f_Canon_Iter_معلومات->mutableTrie = تش_اشاره;
    }
    إذا (U_FAILURE(error_کوډ)) {
        ړنګول impl->f_Canon_Iter_معلومات;
        impl->f_Canon_Iter_معلومات = تش_اشاره;
    }
}

فراغ Normalizer2Impl::make_Canon_Iter_معلومات_From_Norm16(UChar32 پیل, UChar32 پای_ټکی, ثابت uint16_t norm16,
                                                  Canon_Iter_معلومات &new_معلومات,
                                                  UError_کوډ &error_کوډ) ثابت {
    إذا(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        أرجع;
    }
    لكل(UChar32 c=پیل; c<=پای_ټکی; ++c) {
        uint32_t old_ارزښت = umutablecptrie_get(new_معلومات.mutableTrie, c);
        uint32_t new_ارزښت=old_ارزښت;
        إذا(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_ارزښت|=CANON_NOT_SEGMENT_STARTER;
            إذا(norm16<MIN_نورمال_MAYBE_YES) {
                new_ارزښت|=CANON_HAS_COMPOSITIONS;
            }
        } وإلا إذا(norm16<minYesNo) {
            new_ارزښت|=CANON_HAS_COMPOSITIONS;
        } وإلا {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            إذا (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            إذا (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ثابت uint16_t *mapping=get_معلومات_For_Yes_Or_No(norm16_2);
                uint16_t لومړی_Unit=*mapping;
                int32_t length=لومړی_Unit&MAPPING_اوږدتیا_ماسک;
                إذا((لومړی_Unit&MAPPING_HAS_CCC_LCCC_کلمه)!=0) {
                    إذا(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_ارزښت|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                إذا(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_بل_UNSAFE(mapping, i, c2);
                    new_معلومات.جمع_کړه_منزل_پیل_سېټ(c, c2, error_کوډ);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    إذا(norm16_2>=minNoNo) {
                        طالما(i<length) {
                            U16_بل_UNSAFE(mapping, i, c2);
                            uint32_t c2_ارزښت = umutablecptrie_get(new_معلومات.mutableTrie, c2);
                            إذا((c2_ارزښت&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_سېټ(new_معلومات.mutableTrie, c2,
                                                   c2_ارزښت|CANON_NOT_SEGMENT_STARTER, &error_کوډ);
                            }
                        }
                    }
                }
            } وإلا {
                // c decomposed to c2 algorithmically; c has cc==0
                new_معلومات.جمع_کړه_منزل_پیل_سېټ(c, c2, error_کوډ);
            }
        }
        إذا(new_ارزښت!=old_ارزښت) {
            umutablecptrie_سېټ(new_معلومات.mutableTrie, c, new_ارزښت, &error_کوډ);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_معلومات(UError_کوډ &error_کوډ) ثابت {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(هذا);
    umtx_initOnce(me->f_Canon_Iter_معلومات_Init_Once, &init_Canon_Iter_معلومات, me, error_کوډ);
    أرجع U_SUCCESS(error_کوډ);
}

int32_t Normalizer2Impl::get_Canon_ارزښت(UChar32 c) ثابت {
    أرجع static_cast<int32_t>(ucptrie_get(f_Canon_Iter_معلومات->trie, c));
}

ثابت Unicode_سېټ &Normalizer2Impl::get_Canon_پیل_سېټ(int32_t n) ثابت {
    أرجع *static_cast<ثابت Unicode_سېټ*>(f_Canon_Iter_معلومات->canon_پیل_Sets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ثابت {
    أرجع get_Canon_ارزښت(c)>=0;
}

UBool Normalizer2Impl::get_Canon_پیل_سېټ(UChar32 c, Unicode_سېټ &سېټ) ثابت {
    int32_t canon_ارزښت=get_Canon_ارزښت(c)&~CANON_NOT_SEGMENT_STARTER;
    إذا(canon_ارزښت==0) {
        أرجع خطأ;
    }
    سېټ.clear();
    int32_t value=canon_ارزښت&CANON_ارزښت_ماسک;
    إذا((canon_ارزښت&CANON_HAS_سېټ)!=0) {
        سېټ.جمع_کړه_All(get_Canon_پیل_سېټ(value));
    } وإلا إذا(value!=0) {
        سېټ.جمع_کړه(value);
    }
    إذا((canon_ارزښت&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        إذا(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_بنسټ + (c - Hangul::JAMO_L_بنسټ) * Hangul::JAMO_VT_شمېر);
            سېټ.جمع_کړه(syllable, syllable+Hangul::JAMO_VT_شمېر-1);
        } وإلا {
            جمع_کړه_Composites(getCompositionsList(norm16), سېټ);
        }
    }
    أرجع ریښتیا;
}

U_NAMESPACE_پای_ټکی

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_بدلون(ثابت UDataSwapper *ds,
            ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
            UError_کوډ *p_Error_کوډ) {
    ثابت UDataInfo *pInfo;
    int32_t سرلیک_اندازه;

    ثابت uint8_t *دننه_بایټونه;
    uint8_t *بهر_بایټونه;

    ثابت int32_t *دننه_Indexes;
    int32_t indexes[Normalizer2Impl::IX_ټول_اندازه+1];

    int32_t i, دننه_واټن, بل_دننه_واټن, size;

    /* udata_swapDataHeader checks the arguments */
    سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    uint8_t formatVersion0=pInfo->format_نسخه[0];
    إذا(!(
        pInfo->معلومات_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->معلومات_Format[1]==0x72 &&
        pInfo->معلومات_Format[2]==0x6d &&
        pInfo->معلومات_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخه[0]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    دننه_بایټونه=(ثابت uint8_t *)دننه_معلومات+سرلیک_اندازه;
    بهر_بایټونه=(بهر_معلومات == تش_اشاره) ? تش_اشاره : (uint8_t *)بهر_معلومات+سرلیک_اندازه;

    دننه_Indexes=(ثابت int32_t *)دننه_بایټونه;
    int32_t min_Indexes_اوږدتیا;
    إذا(formatVersion0==1) {
        min_Indexes_اوږدتیا=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } وإلا إذا(formatVersion0==2) {
        min_Indexes_اوږدتیا=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } وإلا إذا(formatVersion0<=4) {
        min_Indexes_اوږدتیا=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } وإلا {
        min_Indexes_اوږدتیا=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    إذا(length>=0) {
        length-=سرلیک_اندازه;
        إذا(length<min_Indexes_اوږدتیا*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    /* read the first few indexes */
    لكل(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, دننه_Indexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_ټول_اندازه];

    إذا(length>=0) {
        إذا(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        /* copy the data for inaccessible bytes */
        إذا(دننه_بایټونه!=بهر_بایټونه) {
            uprv_memcpy(بهر_بایټونه, دننه_بایټونه, size);
        }

        دننه_واټن=0;

        /* swap the int32_t indexes[] */
        بل_دننه_واټن=indexes[Normalizer2Impl::IX_NORM_TRIE_دننه_واټن];
        ds->بدلون_Array32(ds, دننه_بایټونه, بل_دننه_واټن-دننه_واټن, بهر_بایټونه, p_Error_کوډ);
        دننه_واټن=بل_دننه_واټن;

        /* swap the trie */
        بل_دننه_واټن=indexes[Normalizer2Impl::IX_EXTRA_معلومات_دننه_واټن];
        utrie_بدلون_Any_نسخه(ds, دننه_بایټونه+دننه_واټن, بل_دننه_واټن-دننه_واټن, بهر_بایټونه+دننه_واټن, p_Error_کوډ);
        دننه_واټن=بل_دننه_واټن;

        /* swap the uint16_t extraData[] */
        بل_دننه_واټن=indexes[Normalizer2Impl::IX_SMALL_FCD_دننه_واټن];
        ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, بل_دننه_واټن-دننه_واټن, بهر_بایټونه+دننه_واټن, p_Error_کوډ);
        دننه_واټن=بل_دننه_واټن;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        بل_دننه_واټن=indexes[Normalizer2Impl::IX_SMALL_FCD_دننه_واټن+1];
        دننه_واټن=بل_دننه_واټن;

        U_ASSERT(دننه_واټن==size);
    }

    أرجع سرلیک_اندازه+size;
}

#د_که_پای  // !UCONFIG_NO_NORMALIZATION
