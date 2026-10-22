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
UChar32 کد_نقطه_From_Valid_UTF8(ثابت uint8_t *cp_شروع, ثابت uint8_t *cp_حد) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cp_شروع < cp_حد);
    uint8_t c = *cp_شروع;
    بدل(cp_حد-cp_شروع) {
    حالة 1:
        أرجع c;
    حالة 2:
        أرجع ((c&0x1f)<<6) | (cp_شروع[1]&0x3f);
    حالة 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        أرجع static_cast<char16_t>((c << 12) | ((cp_شروع[1] & 0x3f) << 6) | (cp_شروع[2] & 0x3f));
    حالة 4:
        أرجع ((c&7)<<18) | ((cp_شروع[1]&0x3f)<<12) | ((cp_شروع[2]&0x3f)<<6) | (cp_شروع[3]&0x3f);
    افتراضي:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 قبلی_Hangul_Or_Jamo(ثابت uint8_t *شروع, ثابت uint8_t *p) {
    إذا ((p - شروع) >= 3) {
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
int32_t get_Jamo_TMinus_پایه(ثابت uint8_t *src, ثابت uint8_t *حد) {
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
append_کد_نقطه_Delta(ثابت uint8_t *cp_شروع, ثابت uint8_t *cp_حد, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    حرف میانگیر[U8_MAX_درازی];
    int32_t length;
    int32_t cp_درازی = static_cast<int32_t>(cp_حد - cp_شروع);
    إذا (cp_درازی == 1) {
        // The builder makes ASCII map to ASCII.
        میانگیر[0] = static_cast<uint8_t>(*cp_شروع + delta);
        length = 1;
    } وإلا {
        int32_t trail = *(cp_حد-1) + delta;
        إذا (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cp_حد;
            length = 0;
            افعل { میانگیر[length++] = *cp_شروع++; } طالما (cp_شروع < cp_حد);
            میانگیر[length++] = static_cast<uint8_t>(trail);
        } وإلا {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = کد_نقطه_From_Valid_UTF8(cp_شروع, cp_حد) + delta;
            length = 0;
            U8_APPEND_UNSAFE(میانگیر, length, c);
        }
    }
    إذا (edits != اشاره_تهی) {
        edits->جمع_کن_Replace(cp_درازی, length);
    }
    sink.Append(میانگیر, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_میانگیر::Reordering_میانگیر(ثابت Normalizer2Impl &ni, UnicodeString &dest,
                                   UError_کد &error_کد) :
        impl(ni), str(dest),
        شروع(str.get_میانگیر(8)), reorder_شروع(شروع), حد(شروع),
        مانده_ظرفیت(str.get_ظرفیت()), آخرین_CC(0) {
    إذا (شروع == اشاره_تهی && U_SUCCESS(error_کد)) {
        // getBuffer() already did str.setToBogus()
        error_کد = U_حافظه_تخصیص_ERROR;
    }
}

UBool Reordering_میانگیر::init(int32_t dest_ظرفیت, UError_کد &error_کد) {
    int32_t length=str.length();
    شروع=str.get_میانگیر(dest_ظرفیت);
    إذا(شروع==اشاره_تهی) {
        // getBuffer() already did str.setToBogus()
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع خطأ;
    }
    حد=شروع+length;
    مانده_ظرفیت=str.get_ظرفیت()-length;
    reorder_شروع=شروع;
    إذا(شروع==حد) {
        آخرین_CC=0;
    } وإلا {
        مجموعه_Iterator();
        آخرین_CC=قبلی_CC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        إذا(آخرین_CC>1) {
            طالما(قبلی_CC()>1) {}
        }
        reorder_شروع=کد_نقطه_حد;
    }
    أرجع درست;
}

UBool Reordering_میانگیر::equals(ثابت char16_t *other_شروع, ثابت char16_t *other_حد) ثابت {
    int32_t length = static_cast<int32_t>(حد - شروع);
    أرجع
        length == static_cast<int32_t>(other_حد - other_شروع) &&
        0==u_memcmp(شروع, other_شروع, length);
}

UBool Reordering_میانگیر::equals(ثابت uint8_t *other_شروع, ثابت uint8_t *other_حد) ثابت {
    U_ASSERT((other_حد - other_شروع) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(حد - شروع);
    int32_t other_درازی = static_cast<int32_t>(other_حد - other_شروع);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    إذا (other_درازی < length || (other_درازی / 3) > length) {
        أرجع خطأ;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    لكل (int32_t i = 0, j = 0;;) {
        إذا (i >= length) {
            أرجع j >= other_درازی;
        } وإلا إذا (j >= other_درازی) {
            أرجع خطأ;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_بعدی_UNSAFE(شروع, i, c);
        U8_بعدی_UNSAFE(other_شروع, j, other);
        إذا (c != other) {
            أرجع خطأ;
        }
    }
}

UBool Reordering_میانگیر::appendSupplementary(UChar32 c, uint8_t cc, UError_کد &error_کد) {
    إذا(مانده_ظرفیت<2 && !resize(2, error_کد)) {
        أرجع خطأ;
    }
    إذا(آخرین_CC<=cc || cc==0) {
        حد[0]=U16_LEAD(c);
        حد[1]=U16_TRAIL(c);
        حد+=2;
        آخرین_CC=cc;
        إذا(cc<=1) {
            reorder_شروع=حد;
        }
    } وإلا {
        insert(c, cc);
    }
    مانده_ظرفیت-=2;
    أرجع درست;
}

UBool Reordering_میانگیر::append(ثابت char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UError_کد &error_کد) {
    إذا(length==0) {
        أرجع درست;
    }
    إذا(مانده_ظرفیت<length && !resize(length, error_کد)) {
        أرجع خطأ;
    }
    مانده_ظرفیت-=length;
    إذا(آخرین_CC<=leadCC || leadCC==0) {
        إذا(trailCC<=1) {
            reorder_شروع=حد+length;
        } وإلا إذا(leadCC<=1) {
            reorder_شروع=حد+1;  // Ok if not a code point boundary.
        }
        ثابت char16_t *s_حد=s+length;
        افعل { *حد++=*s++; } طالما(s!=s_حد);
        آخرین_CC=trailCC;
    } وإلا {
        int32_t i=0;
        UChar32 c;
        U16_بعدی(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        طالما(i<length) {
            U16_بعدی(s, i, length, c);
            إذا(i<length) {
                إذا (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } وإلا {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } وإلا {
                leadCC=trailCC;
            }
            append(c, leadCC, error_کد);
        }
    }
    أرجع درست;
}

UBool Reordering_میانگیر::appendZeroCC(UChar32 c, UError_کد &error_کد) {
    int32_t cp_درازی=U16_درازی(c);
    إذا(مانده_ظرفیت<cp_درازی && !resize(cp_درازی, error_کد)) {
        أرجع خطأ;
    }
    مانده_ظرفیت-=cp_درازی;
    إذا(cp_درازی==1) {
        *حد++ = static_cast<char16_t>(c);
    } وإلا {
        حد[0]=U16_LEAD(c);
        حد[1]=U16_TRAIL(c);
        حد+=2;
    }
    آخرین_CC=0;
    reorder_شروع=حد;
    أرجع درست;
}

UBool Reordering_میانگیر::appendZeroCC(ثابت char16_t *s, ثابت char16_t *s_حد, UError_کد &error_کد) {
    إذا(s==s_حد) {
        أرجع درست;
    }
    int32_t length = static_cast<int32_t>(s_حد - s);
    إذا(مانده_ظرفیت<length && !resize(length, error_کد)) {
        أرجع خطأ;
    }
    u_memcpy(حد, s, length);
    حد+=length;
    مانده_ظرفیت-=length;
    آخرین_CC=0;
    reorder_شروع=حد;
    أرجع درست;
}

فراغ Reordering_میانگیر::remove() {
    reorder_شروع=حد=شروع;
    مانده_ظرفیت=str.get_ظرفیت();
    آخرین_CC=0;
}

فراغ Reordering_میانگیر::removeSuffix(int32_t suffix_درازی) {
    إذا(suffix_درازی<(حد-شروع)) {
        حد-=suffix_درازی;
        مانده_ظرفیت+=suffix_درازی;
    } وإلا {
        حد=شروع;
        مانده_ظرفیت=str.get_ظرفیت();
    }
    آخرین_CC=0;
    reorder_شروع=حد;
}

UBool Reordering_میانگیر::resize(int32_t append_درازی, UError_کد &error_کد) {
    int32_t reorder_شروع_شاخص = static_cast<int32_t>(reorder_شروع - شروع);
    int32_t length = static_cast<int32_t>(حد - شروع);
    str.release_میانگیر(length);
    int32_t new_ظرفیت=length+append_درازی;
    int32_t double_ظرفیت=2*str.get_ظرفیت();
    إذا(new_ظرفیت<double_ظرفیت) {
        new_ظرفیت=double_ظرفیت;
    }
    إذا(new_ظرفیت<256) {
        new_ظرفیت=256;
    }
    شروع=str.get_میانگیر(new_ظرفیت);
    إذا(شروع==اشاره_تهی) {
        // getBuffer() already did str.setToBogus()
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع خطأ;
    }
    reorder_شروع=شروع+reorder_شروع_شاخص;
    حد=شروع+length;
    مانده_ظرفیت=str.get_ظرفیت()-length;
    أرجع درست;
}

فراغ Reordering_میانگیر::skip_قبلی() {
    کد_نقطه_حد=کد_نقطه_شروع;
    char16_t c=*--کد_نقطه_شروع;
    إذا(U16_IS_TRAIL(c) && شروع<کد_نقطه_شروع && U16_IS_LEAD(*(کد_نقطه_شروع-1))) {
        --کد_نقطه_شروع;
    }
}

uint8_t Reordering_میانگیر::قبلی_CC() {
    کد_نقطه_حد=کد_نقطه_شروع;
    إذا(reorder_شروع>=کد_نقطه_شروع) {
        أرجع 0;
    }
    UChar32 c=*--کد_نقطه_شروع;
    char16_t c2;
    إذا(U16_IS_TRAIL(c) && شروع<کد_نقطه_شروع && U16_IS_LEAD(c2=*(کد_نقطه_شروع-1))) {
        --کد_نقطه_شروع;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    أرجع impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
فراغ Reordering_میانگیر::insert(UChar32 c, uint8_t cc) {
    لكل(مجموعه_Iterator(), skip_قبلی(); قبلی_CC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=حد;
    char16_t *r=حد+=U16_درازی(c);
    افعل {
        *--r=*--q;
    } طالما(کد_نقطه_حد!=q);
    write_کد_نقطه(q, c);
    إذا(cc<=1) {
        reorder_شروع=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

بنية Canon_Iter_داده : عام UMemory {
    Canon_Iter_داده(UError_کد &error_کد);
    ~Canon_Iter_داده();
    فراغ جمع_کن_مقصد_شروع_مجموعه(UChar32 origin, UChar32 decompLead, UError_کد &error_کد);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canon_شروع_Sets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    حذف f_Canon_Iter_داده;
}

فراغ
Normalizer2Impl::init(ثابت int32_t *درون_Indexes, ثابت UCPTrie *درون_Trie,
                      ثابت uint16_t *درون_Extra_داده, ثابت uint8_t *درون_Small_FCD) {
    minDecompNoCP = static_cast<char16_t>(درون_Indexes[IX_MIN_DECOMP_NO_CP]);
    minCompNoMaybeCP = static_cast<char16_t>(درون_Indexes[IX_MIN_COMP_NO_MAYBE_CP]);
    minLcccCP = static_cast<char16_t>(درون_Indexes[IX_MIN_LCCC_CP]);

    minYesNo = static_cast<uint16_t>(درون_Indexes[IX_MIN_YES_NO]);
    minYesNoMappingsOnly = static_cast<uint16_t>(درون_Indexes[IX_MIN_YES_NO_MAPPINGS_ONLY]);
    minNoNo = static_cast<uint16_t>(درون_Indexes[IX_MIN_NO_NO]);
    minNoNoCompBoundaryBefore = static_cast<uint16_t>(درون_Indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE]);
    minNoNoCompNoMaybeCC = static_cast<uint16_t>(درون_Indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC]);
    min_No_No_خالی = static_cast<uint16_t>(درون_Indexes[IX_MIN_NO_NO_خالی]);
    حد_No_No = static_cast<uint16_t>(درون_Indexes[IX_حد_NO_NO]);
    minMaybeNo = static_cast<uint16_t>(درون_Indexes[IX_MIN_MAYBE_NO]);
    minMaybeNoCombinesFwd = static_cast<uint16_t>(درون_Indexes[IX_MIN_MAYBE_NO_COMBINES_FWD]);
    minMaybeYes = static_cast<uint16_t>(درون_Indexes[IX_MIN_MAYBE_YES]);
    U_ASSERT((minMaybeNo & 7) == 0);  // 8-aligned for noNoDelta bit fields
    centerNoNoDelta = (minMaybeNo >> DELTA_لغزش) - MAX_DELTA - 1;

    normTrie=درون_Trie;
    extra_داده=درون_Extra_داده;
    smallFCD=درون_Small_FCD;
}

U_CDECL_BEGIN

ساكن uint32_t U_CALLCONV
segmentStarterMapper(ثابت فراغ * /*context*/, uint32_t value) {
    أرجع value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_انتها

فراغ
Normalizer2Impl::جمع_کن_Lccc_Chars(Unicode_مجموعه &مجموعه) ثابت {
    UChar32 شروع = 0, انتها;
    uint32_t norm16;
    طالما ((انتها = ucptrie_getRange(normTrie, شروع, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   اشاره_تهی, اشاره_تهی, &norm16)) >= 0) {
        إذا (norm16 > Normalizer2Impl::MIN_نرمال_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            مجموعه.جمع_کن(شروع, انتها);
        } وإلا إذا (minNoNoCompNoMaybeCC <= norm16 && norm16 < حد_No_No) {
            uint16_t fcd16 = getFCD16(شروع);
            إذا (fcd16 > 0xff) { مجموعه.جمع_کن(شروع, انتها); }
        }
        شروع = انتها + 1;
    }
}

فراغ
Normalizer2Impl::جمع_کن_Property_Starts(ثابت USetAdder *sa, UError_کد & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 شروع = 0, انتها;
    uint32_t value;
    طالما ((انتها = ucptrie_getRange(normTrie, شروع, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   اشاره_تهی, اشاره_تهی, &value)) >= 0) {
        sa->جمع_کن(sa->مجموعه, شروع);
        إذا (شروع != انتها && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_ماسک) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(شروع);
            طالما (++شروع <= انتها) {
                uint16_t fcd16 = getFCD16(شروع);
                إذا (fcd16 != prevFCD16) {
                    sa->جمع_کن(sa->مجموعه, شروع);
                    prevFCD16 = fcd16;
                }
            }
        }
        شروع = انتها + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    لكل(char16_t c=Hangul::HANGUL_پایه; c<Hangul::HANGUL_حد; c+=Hangul::JAMO_T_شمار) {
        sa->جمع_کن(sa->مجموعه, c);
        sa->جمع_کن(sa->مجموعه, c+1);
    }
    sa->جمع_کن(sa->مجموعه, Hangul::HANGUL_حد); /* add Hangul+1 to continue with other properties */
}

فراغ
Normalizer2Impl::جمع_کن_Canon_Iter_Property_Starts(ثابت USetAdder *sa, UError_کد &error_کد) ثابت {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    إذا (!ensure_Canon_Iter_داده(error_کد)) { أرجع; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 شروع = 0, انتها;
    uint32_t value;
    طالما ((انتها = ucptrie_getRange(f_Canon_Iter_داده->trie, شروع, UCPMAP_RANGE_نرمال, 0,
                                   segmentStarterMapper, اشاره_تهی, &value)) >= 0) {
        sa->جمع_کن(sa->مجموعه, شروع);
        شروع = انتها + 1;
    }
}

ثابت char16_t *
Normalizer2Impl::copy_کم_پیشوند_From_Nul_Terminated(ثابت char16_t *src,
                                                UChar32 min_Need_داده_CP,
                                                Reordering_میانگیر *میانگیر,
                                                UError_کد &error_کد) ثابت {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ثابت char16_t *prevSrc=src;
    char16_t c;
    طالما((c=*src++)<min_Need_داده_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    إذا(--src!=prevSrc) {
        إذا(میانگیر!=اشاره_تهی) {
            میانگیر->appendZeroCC(prevSrc, src, error_کد);
        }
    }
    أرجع src;
}

UnicodeString &
Normalizer2Impl::decompose(ثابت UnicodeString &src, UnicodeString &dest,
                           UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        dest.مجموعه_مقصد_Bogus();
        أرجع dest;
    }
    ثابت char16_t *sArray=src.get_میانگیر();
    إذا(&dest==&src || sArray==اشاره_تهی) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        dest.مجموعه_مقصد_Bogus();
        أرجع dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), error_کد);
    أرجع dest;
}

فراغ
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *حد,
                           UnicodeString &dest,
                           int32_t dest_درازی_Estimate,
                           UError_کد &error_کد) ثابت {
    إذا(dest_درازی_Estimate<0 && حد!=اشاره_تهی) {
        dest_درازی_Estimate = static_cast<int32_t>(حد - src);
    }
    dest.remove();
    Reordering_میانگیر میانگیر(*هذا, dest);
    إذا(میانگیر.init(dest_درازی_Estimate, error_کد)) {
        decompose(src, حد, &میانگیر, error_کد);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ثابت char16_t *
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *حد,
                           Reordering_میانگیر *میانگیر,
                           UError_کد &error_کد) ثابت {
    إذا(src==اشاره_تهی) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع src;
    }
    UChar32 minNoCP=minDecompNoCP;
    إذا(حد==اشاره_تهی) {
        src=copy_کم_پیشوند_From_Nul_Terminated(src, minNoCP, میانگیر, error_کد);
        إذا(U_FAILURE(error_کد)) {
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
            إذا(میانگیر!=اشاره_تهی) {
                إذا(!میانگیر->appendZeroCC(prevSrc, src, error_کد)) {
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
        src+=U16_درازی(c);
        إذا(میانگیر!=اشاره_تهی) {
            إذا(!decompose(c, norm16, *میانگیر, error_کد)) {
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
                                Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    طالما(src<حد) {
        إذا (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            أرجع src;
        }
        ثابت char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_بعدی(normTrie, UCPTRIE_16, src, حد, c, norm16);
        إذا (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            أرجع prevSrc;
        }
        إذا(!decompose(c, norm16, میانگیر, error_کد)) {
            أرجع اشاره_تهی;
        }
        إذا (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            أرجع src;
        }
    }
    أرجع src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_میانگیر &میانگیر,
                                 UError_کد &error_کد) ثابت {
    // get the decomposition and the lead and trail cc's
    إذا (norm16 >= حد_No_No) {
        إذا (isMaybeYesOrNonZeroCC(norm16)) {
            أرجع میانگیر.append(c, getCCFromYesOrMaybeYes(norm16), error_کد);
        } وإلا إذا (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    إذا (norm16 < minYesNo) {
        // c does not decompose
        أرجع میانگیر.append(c, 0, error_کد);
    } وإلا إذا(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        أرجع میانگیر.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_کد);
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_داده(norm16);
    uint16_t اولی_Unit=*mapping;
    int32_t length=اولی_Unit&MAPPING_درازی_ماسک;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(اولی_Unit >> 8);
    إذا(اولی_Unit&MAPPING_HAS_CCC_LCCC_کلمه) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } وإلا {
        leadCC=0;
    }
    أرجع میانگیر.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, درست, leadCC, trailCC, error_کد);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ثابت uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ثابت uint8_t *src, ثابت uint8_t *حد,
                               ByteSink *sink, Edits *edits, UError_کد &error_کد) ثابت {
    U_ASSERT(حد != اشاره_تهی);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ثابت uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    لكل (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ثابت uint8_t *fast_شروع = src;
        ثابت uint8_t *prevSrc;
        uint16_t norm16 = 0;

        لكل (;;) {
            إذا (src == حد) {
                إذا (prevBoundary != حد && sink != اشاره_تهی) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, حد,
                                                  *sink, options, edits, error_کد);
                }
                أرجع src;
            }
            إذا (*src < minNoLead) {
                ++src;
            } وإلا {
                prevSrc = src;
                UCPTRIE_FAST_U8_بعدی(normTrie, UCPTRIE_16, src, حد, norm16);
                إذا (!isMostDecompYesAndZeroCC(norm16)) {
                    اكسر;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        إذا (prevSrc != fast_شروع) {
            // The fast path looped over yes/0 characters before the current one.
            إذا (sink != اشاره_تهی &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_کد)) {
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
                    إذا (sink != اشاره_تهی &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, error_کد)) {
                        اكسر;
                    }
                    prevBoundary = src;
                }
                استمر;
            }
        }
        إذا (sink == اشاره_تهی) {
            أرجع prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        إذا (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            إذا (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, error_کد)) {
                اكسر;
            }
            prevBoundary = prevSrc;
        }
        Reordering_میانگیر میانگیر(*هذا, s16, error_کد);
        إذا (U_FAILURE(error_کد)) {
            اكسر;
        }
        decomposeShort(prevBoundary, src, STOP_AT_حد, خطأ /* onlyContiguous */,
                       میانگیر, error_کد);
        // Decompose until the next boundary.
        إذا (میانگیر.get_آخرین_CC() > 1) {
            src = decomposeShort(src, حد, STOP_AT_DECOMP_BOUNDARY, خطأ /* onlyContiguous */,
                                 میانگیر, error_کد);
        }
        إذا (U_FAILURE(error_کد)) {
            اكسر;
        }
        إذا ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
            اكسر;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        إذا (isMaybeYesOrNonZeroCC(norm16) && میانگیر.equals(prevBoundary, src)) {
            إذا (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, error_کد)) {
                اكسر;
            }
        } وإلا {
            إذا (!ByteSinkUtil::appendChange(prevBoundary, src, میانگیر.get_شروع(), میانگیر.length(),
                                            *sink, edits, error_کد)) {
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
                                Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    طالما (src < حد) {
        ثابت uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_بعدی(normTrie, UCPTRIE_16, src, حد, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        إذا (norm16 >= حد_No_No) {
            إذا (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                إذا (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    أرجع prevSrc;
                }
                c = کد_نقطه_From_Valid_UTF8(prevSrc, src);
                إذا (!میانگیر.append(c, cc, error_کد)) {
                    أرجع اشاره_تهی;
                }
                إذا (stopAt == STOP_AT_DECOMP_BOUNDARY && میانگیر.get_آخرین_CC() <= 1) {
                    أرجع src;
                }
                استمر;
            } وإلا إذا (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                إذا (stopAt != STOP_AT_حد) {
                    أرجع prevSrc;
                }
                c = کد_نقطه_From_Valid_UTF8(prevSrc, src);
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
                c = کد_نقطه_From_Valid_UTF8(prevSrc, src);
            }
            // does not decompose
            إذا (!میانگیر.append(c, 0, error_کد)) {
                أرجع اشاره_تهی;
            }
        } وإلا إذا (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            إذا (c < 0) {
                c = کد_نقطه_From_Valid_UTF8(prevSrc, src);
            }
            char16_t jamos[3];
            إذا (!میانگیر.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_کد)) {
                أرجع اشاره_تهی;
            }
        } وإلا {
            // The character decomposes, get everything from the variable-length extra data.
            ثابت uint16_t *mapping = get_داده(norm16);
            uint16_t اولی_Unit = *mapping;
            int32_t length = اولی_Unit & MAPPING_درازی_ماسک;
            uint8_t trailCC = static_cast<uint8_t>(اولی_Unit >> 8);
            uint8_t leadCC;
            إذا (اولی_Unit & MAPPING_HAS_CCC_LCCC_کلمه) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } وإلا {
                leadCC = 0;
            }
            إذا (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                أرجع prevSrc;
            }
            إذا (!میانگیر.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, درست, leadCC, trailCC, error_کد)) {
                أرجع اشاره_تهی;
            }
        }
        إذا ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && میانگیر.get_آخرین_CC() <= 1)) {
            أرجع src;
        }
    }
    أرجع src;
}

ثابت char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t میانگیر[4], int32_t &length) ثابت {
    uint16_t norm16;
    إذا(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        أرجع اشاره_تهی;
    }
    ثابت char16_t *decomp = اشاره_تهی;
    إذا(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=میانگیر;
        length=0;
        U16_APPEND_UNSAFE(میانگیر, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    إذا (norm16 < minYesNo) {
        أرجع decomp;
    } وإلا إذا(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, میانگیر);
        أرجع میانگیر;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_داده(norm16);
    length=*mapping&MAPPING_درازی_ماسک;
    أرجع reinterpret_cast<ثابت char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ثابت char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t میانگیر[30], int32_t &length) ثابت {
    uint16_t norm16;
    إذا(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        أرجع اشاره_تهی;
    } وإلا إذا(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, میانگیر);
        length=2;
        أرجع میانگیر;
    } وإلا إذا(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(میانگیر, length, c);
        أرجع میانگیر;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_داده(norm16);
    uint16_t اولی_Unit=*mapping;
    int32_t m_درازی=اولی_Unit&MAPPING_درازی_ماسک;  // length of normal mapping
    إذا(اولی_Unit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ثابت uint16_t *rawMapping=mapping-((اولی_Unit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        إذا(rm0<=MAPPING_درازی_ماسک) {
            length=rm0;
            أرجع reinterpret_cast<ثابت char16_t*>(rawMapping) - rm0;
        } وإلا {
            // Copy the normal mapping and replace its first two code units with rm0.
            میانگیر[0] = static_cast<char16_t>(rm0);
            u_memcpy(میانگیر + 1, reinterpret_cast<ثابت char16_t*>(mapping) + 1 + 2, m_درازی - 2);
            length=m_درازی-1;
            أرجع میانگیر;
        }
    } وإلا {
        length=m_درازی;
        أرجع reinterpret_cast<ثابت char16_t*>(mapping) + 1;
    }
}

فراغ Normalizer2Impl::decomposeAndAppend(ثابت char16_t *src, ثابت char16_t *حد,
                                         UBool doDecompose,
                                         UnicodeString &امن_میانه,
                                         Reordering_میانگیر &میانگیر,
                                         UError_کد &error_کد) ثابت {
    میانگیر.copy_Reorderable_Suffix_مقصد(امن_میانه);
    إذا(doDecompose) {
        decompose(src, حد, &میانگیر, error_کد);
        أرجع;
    }
    // Just merge the strings at the boundary.
    منطقي is_اولی = درست;
    uint8_t اولی_CC = 0, prevCC = 0, cc;
    ثابت char16_t *p = src;
    طالما (p != حد) {
        ثابت char16_t *کد_نقطه_شروع = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_بعدی(normTrie, UCPTRIE_16, p, حد, c, norm16);
        إذا ((cc = getCC(norm16)) == 0) {
            p = کد_نقطه_شروع;
            اكسر;
        }
        إذا (is_اولی) {
            اولی_CC = cc;
            is_اولی = خطأ;
        }
        prevCC = cc;
    }
    إذا(حد==اشاره_تهی) {  // appendZeroCC() needs limit!=nullptr
        حد=u_strchr(p, 0);
    }

    إذا (میانگیر.append(src, static_cast<int32_t>(p - src), خطأ, اولی_CC, prevCC, error_کد)) {
        میانگیر.appendZeroCC(p, حد, error_کد);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ثابت {
    أرجع c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ثابت {
    إذا (norm16 < minNoNoCompNoMaybeCC) {
        أرجع درست;
    }
    إذا (norm16 >= حد_No_No) {
        أرجع norm16 <= MIN_نرمال_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_داده_For_Yes_Or_No(norm16);
    uint16_t اولی_Unit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    أرجع (اولی_Unit&MAPPING_HAS_CCC_LCCC_کلمه)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ثابت {
    إذا (c < minDecompNoCP) {
        أرجع درست;
    }
    إذا (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        أرجع درست;
    }
    أرجع norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ثابت {
    إذا(norm16 <= minYesNo || isHangulLVT(norm16)) {
        أرجع درست;
    }
    إذا (norm16 >= حد_No_No) {
        إذا (isMaybeYesOrNonZeroCC(norm16)) {
            أرجع norm16 <= MIN_نرمال_MAYBE_YES || norm16 == JAMO_VT;
        } وإلا إذا (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            أرجع (norm16 & DELTA_TCCC_ماسک) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=get_داده(norm16);
    uint16_t اولی_Unit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    إذا(اولی_Unit>0x1ff) {
        أرجع خطأ;  // trailCC>1
    }
    إذا(اولی_Unit<=0xff) {
        أرجع درست;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    أرجع (اولی_Unit&MAPPING_HAS_CCC_LCCC_کلمه)==0 || (*(mapping-1)&0xff00)==0;
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
    uint16_t key1, اولی_Unit;
    إذا(trail<COMP_1_TRAIL_حد) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        طالما(key1>(اولی_Unit=*list)) {
            list+=2+(اولی_Unit&COMP_1_TRIPLE);
        }
        إذا(key1==(اولی_Unit&COMP_1_TRAIL_ماسک)) {
            إذا(اولی_Unit&COMP_1_TRIPLE) {
                أرجع (static_cast<int32_t>(list[1]) << 16) | list[2];
            } وإلا {
                أرجع list[1];
            }
        }
    } وإلا {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_حد +
                        (((trail>>COMP_1_TRAIL_لغزش))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_لغزش);
        uint16_t دومی_Unit;
        لكل(;;) {
            إذا(key1>(اولی_Unit=*list)) {
                list+=2+(اولی_Unit&COMP_1_TRIPLE);
            } وإلا إذا(key1==(اولی_Unit&COMP_1_TRAIL_ماسک)) {
                إذا(key2>(دومی_Unit=list[1])) {
                    إذا(اولی_Unit&COMP_1_آخرین_TUPLE) {
                        اكسر;
                    } وإلا {
                        list+=3;
                    }
                } وإلا إذا(key2==(دومی_Unit&COMP_2_TRAIL_ماسک)) {
                    أرجع (static_cast<int32_t>(دومی_Unit & ~COMP_2_TRAIL_ماسک) << 16) | list[2];
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
فراغ Normalizer2Impl::جمع_کن_Composites(ثابت uint16_t *list, Unicode_مجموعه &مجموعه) ثابت {
    uint16_t اولی_Unit;
    int32_t مرکب_And_Fwd;
    افعل {
        اولی_Unit=*list;
        إذا((اولی_Unit&COMP_1_TRIPLE)==0) {
            مرکب_And_Fwd=list[1];
            list+=2;
        } وإلا {
            مرکب_And_Fwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_ماسک) << 16) | list[2];
            list+=3;
        }
        UChar32 مرکب=مرکب_And_Fwd>>1;
        إذا((مرکب_And_Fwd&1)!=0) {
            جمع_کن_Composites(get_Compositions_List_For_مرکب(getRawNorm16(مرکب)), مجموعه);
        }
        مجموعه.جمع_کن(مرکب);
    } طالما((اولی_Unit&COMP_1_آخرین_TUPLE)==0);
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
فراغ Normalizer2Impl::recompose(Reordering_میانگیر &میانگیر, int32_t recompose_شروع_شاخص,
                                UBool onlyContiguous) ثابت {
    char16_t *p=میانگیر.get_شروع()+recompose_شروع_شاخص;
    char16_t *حد=میانگیر.get_حد();
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
    compositionsList=اشاره_تهی;  // used as indicator for whether we have a forward-combining starter
    starter=اشاره_تهی;
    starterIsSupplementary=خطأ;
    prevCC=0;

    لكل(;;) {
        UCPTRIE_FAST_U16_بعدی(normTrie, UCPTRIE_16, p, حد, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        إذا( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=اشاره_تهی &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            إذا(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                إذا(c<Hangul::JAMO_T_پایه) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_پایه);
                    إذا(prev<Hangul::JAMO_L_شمار) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_پایه +
                             (prev*Hangul::JAMO_V_شمار+(c-Hangul::JAMO_V_پایه))*
                             Hangul::JAMO_T_شمار);
                        char16_t t;
                        إذا (p != حد && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_پایه)) < Hangul::JAMO_T_شمار) {
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
                compositionsList=اشاره_تهی;
                استمر;
            } وإلا إذا((مرکب_And_Fwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 مرکب=مرکب_And_Fwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_درازی(c);  // pRemove & p: start & limit of the combining mark
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
                    starterIsSupplementary=درست;
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
                    compositionsList=اشاره_تهی;
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
            إذا((compositionsList=getCompositionsListForDecompYes(norm16))!=اشاره_تهی) {
                // It may combine with something, prepare for it.
                إذا(U_IS_BMP(c)) {
                    starterIsSupplementary=خطأ;
                    starter=p-1;
                } وإلا {
                    starterIsSupplementary=درست;
                    starter=p-2;
                }
            }
        } وإلا إذا(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=اشاره_تهی;
        }
    }
    میانگیر.مجموعه_Reordering_حد(حد);
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
            إذا (b < Hangul::JAMO_V_پایه) {
                أرجع U_SENTINEL;
            }
            b-=Hangul::JAMO_V_پایه;
            إذا(b<Hangul::JAMO_V_شمار) {
                أرجع
                    (Hangul::HANGUL_پایه+
                     ((a-Hangul::JAMO_L_پایه)*Hangul::JAMO_V_شمار+b)*
                     Hangul::JAMO_T_شمار);
            } وإلا {
                أرجع U_SENTINEL;
            }
        } وإلا إذا(isHangulLV(norm16)) {
            إذا (b <= Hangul::JAMO_T_پایه) {
               أرجع U_SENTINEL;
            }
            b-=Hangul::JAMO_T_پایه;
            إذا(b<Hangul::JAMO_T_شمار) {  // not b==0!
                أرجع a+b;
            } وإلا {
                أرجع U_SENTINEL;
            }
        } وإلا {
            // 'a' has a compositions list in extraData
            list=get_داده_For_Yes_Or_No(norm16);
            إذا(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_درازی_ماسک);  // + mapping length
            }
        }
    } وإلا إذا(norm16<minMaybeNoCombinesFwd || MIN_نرمال_MAYBE_YES<=norm16) {
        أرجع U_SENTINEL;
    } وإلا {
        list=get_داده_For_Maybe(norm16);
        إذا(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_درازی_ماسک);  // + mapping length
        }
    }
    إذا(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        أرجع U_SENTINEL;
    }
#إذا U_SIGNED_راست_لغزش_IS_ARITHMETIC
    أرجع combine(list, b)>>1;
#وإلا
    int32_t مرکب_And_Fwd=combine(list, b);
    أرجع مرکب_And_Fwd>=0 ? مرکب_And_Fwd>>1 : U_SENTINEL;
#پایان_اگر
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ثابت char16_t *src, ثابت char16_t *حد,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_میانگیر &میانگیر,
                         UError_کد &error_کد) ثابت {
    ثابت char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    إذا(حد==اشاره_تهی) {
        src=copy_کم_پیشوند_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &میانگیر : اشاره_تهی,
                                           error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع خطأ;
        }
        حد=u_strchr(src, 0);
        إذا (prevBoundary != src) {
            إذا (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } وإلا {
                میانگیر.removeSuffix(1);
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
                    میانگیر.appendZeroCC(prevBoundary, حد, error_کد);
                }
                أرجع درست;
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
                    إذا (prevBoundary != prevSrc && !میانگیر.appendZeroCC(prevBoundary, prevSrc, error_کد)) {
                        اكسر;
                    }
                    إذا(!میانگیر.append(mapAlgorithmic(c, norm16), 0, error_کد)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } وإلا إذا (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد)) {
                    إذا (prevBoundary != prevSrc && !میانگیر.appendZeroCC(prevBoundary, prevSrc, error_کد)) {
                        اكسر;
                    }
                    ثابت char16_t *mapping = reinterpret_cast<ثابت char16_t *>(get_داده_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_درازی_ماسک;
                    إذا(!میانگیر.appendZeroCC(mapping, mapping + length, error_کد)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } وإلا إذا (norm16 >= min_No_No_خالی) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                إذا (hasCompBoundaryBefore(src, حد) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    إذا (prevBoundary != prevSrc && !میانگیر.appendZeroCC(prevBoundary, prevSrc, error_کد)) {
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
            إذا(c<Hangul::JAMO_T_پایه) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_پایه);
                إذا(l<Hangul::JAMO_L_شمار) {
                    إذا (!doCompose) {
                        أرجع خطأ;
                    }
                    int32_t t;
                    إذا (src != حد &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_پایه)) &&
                            t < Hangul::JAMO_T_شمار) {
                        // The next character is a Jamo T.
                        ++src;
                    } وإلا إذا (hasCompBoundaryBefore(src, حد)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } وإلا {
                        t = -1;
                    }
                    إذا (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_پایه +
                            (l*Hangul::JAMO_V_شمار + (c-Hangul::JAMO_V_پایه)) *
                            Hangul::JAMO_T_شمار + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        إذا (prevBoundary != prevSrc && !میانگیر.appendZeroCC(prevBoundary, prevSrc, error_کد)) {
                            اكسر;
                        }
                        إذا (!میانگیر.appendBMP(static_cast<char16_t>(syllable), 0, error_کد)) {
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
                UChar32 syllable = prev + c - Hangul::JAMO_T_پایه;
                --prevSrc;  // Replace the Hangul LV as well.
                إذا (prevBoundary != prevSrc && !میانگیر.appendZeroCC(prevBoundary, prevSrc, error_کد)) {
                    اكسر;
                }
                إذا (!میانگیر.appendBMP(static_cast<char16_t>(syllable), 0, error_کد)) {
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
            uint8_t cc = get_CCFrom_نرمال_Yes_Or_Maybe(norm16);  // cc!=0
            إذا (onlyContiguous /* FCC */ && get_قبلی_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                إذا (!doCompose) {
                    أرجع خطأ;
                }
            } وإلا {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *بعدی_Src;
                uint16_t n16;
                لكل (;;) {
                    إذا (src == حد) {
                        إذا (doCompose) {
                            میانگیر.appendZeroCC(prevBoundary, حد, error_کد);
                        }
                        أرجع درست;
                    }
                    uint8_t prevCC = cc;
                    بعدی_Src = src;
                    UCPTRIE_FAST_U16_بعدی(normTrie, UCPTRIE_16, بعدی_Src, حد, c, n16);
                    إذا (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_نرمال_Yes_Or_Maybe(n16);
                        إذا (prevCC > cc) {
                            إذا (!doCompose) {
                                أرجع خطأ;
                            }
                            اكسر;
                        }
                    } وإلا {
                        اكسر;
                    }
                    src = بعدی_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                إذا (norm16HasCompBoundaryBefore(n16)) {
                    إذا (isCompYesAndZeroCC(n16)) {
                        src = بعدی_Src;
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
        إذا (doCompose && prevBoundary != prevSrc && !میانگیر.appendZeroCC(prevBoundary, prevSrc, error_کد)) {
            اكسر;
        }
        int32_t recompose_شروع_شاخص=میانگیر.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, خطأ /* !stopAtCompBoundary */, onlyContiguous,
                       میانگیر, error_کد);
        // Decompose until the next boundary.
        src = decomposeShort(src, حد, درست /* stopAtCompBoundary */, onlyContiguous,
                             میانگیر, error_کد);
        إذا (U_FAILURE(error_کد)) {
            اكسر;
        }
        إذا ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع درست;
        }
        recompose(میانگیر, recompose_شروع_شاخص, onlyContiguous);
        إذا(!doCompose) {
            إذا(!میانگیر.equals(prevSrc, src)) {
                أرجع خطأ;
            }
            میانگیر.remove();
        }
        prevBoundary=src;
    }
    أرجع درست;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ثابت char16_t *
Normalizer2Impl::compose_سریع_Check(ثابت char16_t *src, ثابت char16_t *حد,
                                   UBool onlyContiguous,
                                   UNormalization_Check_نتیجه *pQCResult) ثابت {
    ثابت char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    إذا(حد==اشاره_تهی) {
        UError_کد error_کد=U_ZERO_ERROR;
        src=copy_کم_پیشوند_From_Nul_Terminated(src, minNoMaybeCP, اشاره_تهی, error_کد);
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
                ثابت char16_t *بعدی_Src;
                لكل (;;) {
                    إذا (norm16 < MIN_YES_YES_WITH_CC) {
                        إذا (pQCResult != اشاره_تهی) {
                            *pQCResult = UNORM_MAYBE;
                        } وإلا {
                            أرجع prevBoundary;
                        }
                    }
                    إذا (src == حد) {
                        أرجع src;
                    }
                    uint8_t prevCC = fcd16;
                    بعدی_Src = src;
                    UCPTRIE_FAST_U16_بعدی(normTrie, UCPTRIE_16, بعدی_Src, حد, c, norm16);
                    إذا (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        إذا (!(prevCC <= cc || cc == 0)) {
                            اكسر;
                        }
                    } وإلا {
                        اكسر;
                    }
                    src = بعدی_Src;
                }
                // src is after the last in-order combining mark.
                إذا (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = بعدی_Src;
                    استمر;
                }
            }
        }
        إذا(pQCResult!=اشاره_تهی) {
            *pQCResult=UNORM_NO;
        }
        أرجع prevBoundary;
    }
}

فراغ Normalizer2Impl::composeAndAppend(ثابت char16_t *src, ثابت char16_t *حد,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &امن_میانه,
                                       Reordering_میانگیر &میانگیر,
                                       UError_کد &error_کد) ثابت {
    إذا(!میانگیر.is_خالی()) {
        ثابت char16_t *اولی_Starter_درون_Src=پیدا_کن_بعدی_Comp_Boundary(src, حد, onlyContiguous);
        إذا(src!=اولی_Starter_درون_Src) {
            ثابت char16_t *آخرین_Starter_درون_Dest=پیدا_کن_قبلی_Comp_Boundary(میانگیر.get_شروع(),
                                                                    میانگیر.get_حد(), onlyContiguous);
            int32_t dest_Suffix_درازی = static_cast<int32_t>(میانگیر.get_حد() - آخرین_Starter_درون_Dest);
            UnicodeString میانه(آخرین_Starter_درون_Dest, dest_Suffix_درازی);
            میانگیر.removeSuffix(dest_Suffix_درازی);
            امن_میانه=میانه;
            میانه.append(src, static_cast<int32_t>(اولی_Starter_درون_Src - src));
            ثابت char16_t *میانه_شروع=میانه.get_میانگیر();
            compose(میانه_شروع, میانه_شروع+میانه.length(), onlyContiguous,
                    درست, میانگیر, error_کد);
            إذا(U_FAILURE(error_کد)) {
                أرجع;
            }
            src=اولی_Starter_درون_Src;
        }
    }
    إذا(doCompose) {
        compose(src, حد, onlyContiguous, درست, میانگیر, error_کد);
    } وإلا {
        إذا(حد==اشاره_تهی) {  // appendZeroCC() needs limit!=nullptr
            حد=u_strchr(src, 0);
        }
        میانگیر.appendZeroCC(src, حد, error_کد);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ثابت uint8_t *src, ثابت uint8_t *حد,
                             ByteSink *sink, Edits *edits, UError_کد &error_کد) ثابت {
    U_ASSERT(حد != اشاره_تهی);
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
                إذا (prevBoundary != حد && sink != اشاره_تهی) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, حد,
                                                  *sink, options, edits, error_کد);
                }
                أرجع درست;
            }
            إذا (*src < minNoMaybeLead) {
                ++src;
            } وإلا {
                prevSrc = src;
                UCPTRIE_FAST_U8_بعدی(normTrie, UCPTRIE_16, src, حد, norm16);
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
            إذا (sink == اشاره_تهی) {
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
                                                           *sink, options, edits, error_کد)) {
                        اكسر;
                    }
                    append_کد_نقطه_Delta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    استمر;
                }
            } وإلا إذا (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, حد)) {
                    إذا (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_کد)) {
                        اكسر;
                    }
                    ثابت uint16_t *mapping = get_داده_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_درازی_ماسک;
                    إذا (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ثابت char16_t*>(mapping), length,
                                                    *sink, edits, error_کد)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } وإلا إذا (norm16 >= min_No_No_خالی) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                إذا (hasCompBoundaryBefore(src, حد) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    إذا (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_کد)) {
                        اكسر;
                    }
                    إذا (edits != اشاره_تهی) {
                        edits->جمع_کن_Replace(static_cast<int32_t>(src - prevSrc), 0);
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
            UChar32 prev = قبلی_Hangul_Or_Jamo(prevBoundary, prevSrc);
            إذا (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_پایه;
                إذا (static_cast<uint32_t>(l) < Hangul::JAMO_L_شمار) {
                    إذا (sink == اشاره_تهی) {
                        أرجع خطأ;
                    }
                    int32_t t = get_Jamo_TMinus_پایه(src, حد);
                    إذا (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } وإلا إذا (hasCompBoundaryBefore(src, حد)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    إذا (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_پایه +
                            (l*Hangul::JAMO_V_شمار + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_شمار + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        إذا (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, error_کد)) {
                            اكسر;
                        }
                        ByteSinkUtil::append_کد_نقطه(prevSrc, src, syllable, *sink, edits);
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
                إذا (sink == اشاره_تهی) {
                    أرجع خطأ;
                }
                UChar32 syllable = prev + get_Jamo_TMinus_پایه(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                إذا (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, error_کد)) {
                    اكسر;
                }
                ByteSinkUtil::append_کد_نقطه(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                استمر;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } وإلا إذا (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_نرمال_Yes_Or_Maybe(norm16);  // cc!=0
            إذا (onlyContiguous /* FCC */ && get_قبلی_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                إذا (sink == اشاره_تهی) {
                    أرجع خطأ;
                }
            } وإلا {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت uint8_t *بعدی_Src;
                uint16_t n16;
                لكل (;;) {
                    إذا (src == حد) {
                        إذا (sink != اشاره_تهی) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, حد,
                                                          *sink, options, edits, error_کد);
                        }
                        أرجع درست;
                    }
                    uint8_t prevCC = cc;
                    بعدی_Src = src;
                    UCPTRIE_FAST_U8_بعدی(normTrie, UCPTRIE_16, بعدی_Src, حد, n16);
                    إذا (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_نرمال_Yes_Or_Maybe(n16);
                        إذا (prevCC > cc) {
                            إذا (sink == اشاره_تهی) {
                                أرجع خطأ;
                            }
                            اكسر;
                        }
                    } وإلا {
                        اكسر;
                    }
                    src = بعدی_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                إذا (norm16HasCompBoundaryBefore(n16)) {
                    إذا (isCompYesAndZeroCC(n16)) {
                        src = بعدی_Src;
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
        Reordering_میانگیر میانگیر(*هذا, s16, error_کد);
        إذا (U_FAILURE(error_کد)) {
            اكسر;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_حد, onlyContiguous,
                       میانگیر, error_کد);
        // Decompose until the next boundary.
        src = decomposeShort(src, حد, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             میانگیر, error_کد);
        إذا (U_FAILURE(error_کد)) {
            اكسر;
        }
        إذا ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع درست;
        }
        recompose(میانگیر, 0, onlyContiguous);
        إذا (!میانگیر.equals(prevSrc, src)) {
            إذا (sink == اشاره_تهی) {
                أرجع خطأ;
            }
            إذا (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_کد)) {
                اكسر;
            }
            إذا (!ByteSinkUtil::appendChange(prevSrc, src, میانگیر.get_شروع(), میانگیر.length(),
                                            *sink, edits, error_کد)) {
                اكسر;
            }
            prevBoundary = src;
        }
    }
    أرجع درست;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت char16_t *src, ثابت char16_t *حد) ثابت {
    إذا (src == حد || *src < minCompNoMaybeCP) {
        أرجع درست;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_بعدی(normTrie, UCPTRIE_16, src, حد, c, norm16);
    أرجع norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت uint8_t *src, ثابت uint8_t *حد) ثابت {
    إذا (src == حد) {
        أرجع درست;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_بعدی(normTrie, UCPTRIE_16, src, حد, norm16);
    أرجع norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت char16_t *شروع, ثابت char16_t *p,
                                            UBool onlyContiguous) ثابت {
    إذا (شروع == p) {
        أرجع درست;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, شروع, p, c, norm16);
    أرجع norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت uint8_t *شروع, ثابت uint8_t *p,
                                            UBool onlyContiguous) ثابت {
    إذا (شروع == p) {
        أرجع درست;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, شروع, p, norm16);
    أرجع norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ثابت char16_t *Normalizer2Impl::پیدا_کن_قبلی_Comp_Boundary(ثابت char16_t *شروع, ثابت char16_t *p,
                                                       UBool onlyContiguous) ثابت {
    طالما (p != شروع) {
        ثابت char16_t *کد_نقطه_حد = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, شروع, p, c, norm16);
        إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            أرجع کد_نقطه_حد;
        }
        إذا (hasCompBoundaryBefore(c, norm16)) {
            أرجع p;
        }
    }
    أرجع p;
}

ثابت char16_t *Normalizer2Impl::پیدا_کن_بعدی_Comp_Boundary(ثابت char16_t *p, ثابت char16_t *حد,
                                                   UBool onlyContiguous) ثابت {
    طالما (p != حد) {
        ثابت char16_t *کد_نقطه_شروع = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_بعدی(normTrie, UCPTRIE_16, p, حد, c, norm16);
        إذا (hasCompBoundaryBefore(c, norm16)) {
            أرجع کد_نقطه_شروع;
        }
        إذا (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            أرجع p;
        }
    }
    أرجع p;
}

uint8_t Normalizer2Impl::get_قبلی_Trail_CC(ثابت char16_t *شروع, ثابت char16_t *p) ثابت {
    إذا (شروع == p) {
        أرجع 0;
    }
    int32_t i = static_cast<int32_t>(p - شروع);
    UChar32 c;
    U16_PREV(شروع, 0, i, c);
    أرجع static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::get_قبلی_Trail_CC(ثابت uint8_t *شروع, ثابت uint8_t *p) ثابت {
    إذا (شروع == p) {
        أرجع 0;
    }
    int32_t i = static_cast<int32_t>(p - شروع);
    UChar32 c;
    U8_PREV(شروع, 0, i, c);
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
#رهنمود optimize( "", off )
#پایان_اگر
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_داده(UChar32 c) ثابت {
    uint16_t norm16=getNorm16(c);
    إذا (norm16 >= حد_No_No) {
        إذا(norm16>=MIN_نرمال_MAYBE_YES) {
            // combining mark
            norm16=get_CCFrom_نرمال_Yes_Or_Maybe(norm16);
            أرجع norm16|(norm16<<8);
        } وإلا إذا(norm16>=minMaybeYes) {
            أرجع 0;
        } وإلا إذا(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_ماسک;
            إذا (deltaTrailCC <= DELTA_TCCC_1) {
                أرجع deltaTrailCC >> جابه_جایی_لغزش;
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
    ثابت uint16_t *mapping=get_داده(norm16);
    uint16_t اولی_Unit=*mapping;
    norm16=اولی_Unit>>8;  // tccc
    إذا(اولی_Unit&MAPPING_HAS_CCC_LCCC_کلمه) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    أرجع norm16;
}
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#رهنمود optimize( "", on )
#پایان_اگر

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ثابت {
    U_ASSERT(norm16 >= minMaybeNo);
    إذا (norm16 >= MIN_نرمال_MAYBE_YES) {
        // combining mark
        norm16 = get_CCFrom_نرمال_Yes_Or_Maybe(norm16);
        أرجع norm16 | (norm16<<8);
    } وإلا إذا (norm16 >= minMaybeYes) {
        أرجع 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping = get_داده_For_Maybe(norm16);
    uint16_t اولی_Unit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((اولی_Unit & MAPPING_HAS_CCC_LCCC_کلمه) == 0 || (*(mapping - 1) & 0xff00) == 0);
    أرجع اولی_Unit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ثابت char16_t *
Normalizer2Impl::makeFCD(ثابت char16_t *src, ثابت char16_t *حد,
                         Reordering_میانگیر *میانگیر,
                         UError_کد &error_کد) ثابت {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ثابت char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    إذا(حد==اشاره_تهی) {
        src=copy_کم_پیشوند_From_Nul_Terminated(src, minLcccCP, میانگیر, error_کد);
        إذا(U_FAILURE(error_کد)) {
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
                إذا((fcd16=get_FCD16_From_Norm_داده(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_درازی(c);
                } وإلا {
                    اكسر;
                }
            }
        }
        // copy these code units all at once
        إذا(src!=prevSrc) {
            إذا(میانگیر!=اشاره_تهی && !میانگیر->appendZeroCC(prevSrc, src, error_کد)) {
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
                    prevFCD16=get_FCD16_From_Norm_داده(prev);
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
                    prevFCD16=get_FCD16_From_Norm_داده(U16_GET_SUPPLEMENTARY(p[0], p[1]));
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

        src+=U16_درازی(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        إذا((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            إذا((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            إذا(میانگیر!=اشاره_تهی && !میانگیر->appendZeroCC(c, error_کد)) {
                اكسر;
            }
            prevFCD16=fcd16;
            استمر;
        } وإلا إذا(میانگیر==اشاره_تهی) {
            أرجع prevBoundary;  // quick check "no"
        } وإلا {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            میانگیر->removeSuffix(static_cast<int32_t>(prevSrc - prevBoundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=پیدا_کن_بعدی_FCDBoundary(src, حد);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(prevBoundary, src, خطأ, خطأ, *میانگیر, error_کد);
            إذا (U_FAILURE(error_کد)) {
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
                                       UnicodeString &امن_میانه,
                                       Reordering_میانگیر &میانگیر,
                                       UError_کد &error_کد) ثابت {
    إذا(!میانگیر.is_خالی()) {
        ثابت char16_t *اولی_Boundary_درون_Src=پیدا_کن_بعدی_FCDBoundary(src, حد);
        إذا(src!=اولی_Boundary_درون_Src) {
            ثابت char16_t *آخرین_Boundary_درون_Dest=پیدا_کن_قبلی_FCDBoundary(میانگیر.get_شروع(),
                                                                    میانگیر.get_حد());
            int32_t dest_Suffix_درازی = static_cast<int32_t>(میانگیر.get_حد() - آخرین_Boundary_درون_Dest);
            UnicodeString میانه(آخرین_Boundary_درون_Dest, dest_Suffix_درازی);
            میانگیر.removeSuffix(dest_Suffix_درازی);
            امن_میانه=میانه;
            میانه.append(src, static_cast<int32_t>(اولی_Boundary_درون_Src - src));
            ثابت char16_t *میانه_شروع=میانه.get_میانگیر();
            makeFCD(میانه_شروع, میانه_شروع+میانه.length(), &میانگیر, error_کد);
            إذا(U_FAILURE(error_کد)) {
                أرجع;
            }
            src=اولی_Boundary_درون_Src;
        }
    }
    إذا(doMakeFCD) {
        makeFCD(src, حد, &میانگیر, error_کد);
    } وإلا {
        إذا(حد==اشاره_تهی) {  // appendZeroCC() needs limit!=nullptr
            حد=u_strchr(src, 0);
        }
        میانگیر.appendZeroCC(src, حد, error_کد);
    }
}

ثابت char16_t *Normalizer2Impl::پیدا_کن_قبلی_FCDBoundary(ثابت char16_t *شروع, ثابت char16_t *p) ثابت {
    طالما(شروع<p) {
        ثابت char16_t *کد_نقطه_حد = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, شروع, p, c, norm16);
        إذا (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            أرجع کد_نقطه_حد;
        }
        إذا (norm16HasDecompBoundaryBefore(norm16)) {
            أرجع p;
        }
    }
    أرجع p;
}

ثابت char16_t *Normalizer2Impl::پیدا_کن_بعدی_FCDBoundary(ثابت char16_t *p, ثابت char16_t *حد) ثابت {
    طالما(p<حد) {
        ثابت char16_t *کد_نقطه_شروع=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_بعدی(normTrie, UCPTRIE_16, p, حد, c, norm16);
        إذا (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            أرجع کد_نقطه_شروع;
        }
        إذا (norm16HasDecompBoundaryAfter(norm16)) {
            أرجع p;
        }
    }
    أرجع p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_داده::Canon_Iter_داده(UError_کد &error_کد) :
        mutableTrie(umutablecptrie_open(0, 0, &error_کد)), trie(اشاره_تهی),
        canon_شروع_Sets(uprv_deleteUObject, اشاره_تهی, error_کد) {}

Canon_Iter_داده::~Canon_Iter_داده() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

فراغ Canon_Iter_داده::جمع_کن_مقصد_شروع_مجموعه(UChar32 origin, UChar32 decompLead, UError_کد &error_کد) {
    uint32_t canon_مقدار = umutablecptrie_get(mutableTrie, decompLead);
    إذا((canon_مقدار&(CANON_HAS_مجموعه|CANON_مقدار_ماسک))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_مجموعه(mutableTrie, decompLead, canon_مقدار|origin, &error_کد);
    } وإلا {
        // origin is not the first character, or it is U+0000.
        Unicode_مجموعه *مجموعه;
        إذا((canon_مقدار&CANON_HAS_مجموعه)==0) {
            Local_اشاره_گر<Unicode_مجموعه> lp_مجموعه(جديد Unicode_مجموعه, error_کد);
            مجموعه=lp_مجموعه.getAlias();
            إذا(U_FAILURE(error_کد)) {
                أرجع;
            }
            UChar32 اولی_Origin = static_cast<UChar32>(canon_مقدار & CANON_مقدار_ماسک);
            canon_مقدار = (canon_مقدار & ~CANON_مقدار_ماسک) | CANON_HAS_مجموعه | static_cast<uint32_t>(canon_شروع_Sets.size());
            umutablecptrie_مجموعه(mutableTrie, decompLead, canon_مقدار, &error_کد);
            canon_شروع_Sets.adoptElement(lp_مجموعه.orphan(), error_کد);
            إذا (U_FAILURE(error_کد)) {
                أرجع;
            }
            إذا(اولی_Origin!=0) {
                مجموعه->جمع_کن(اولی_Origin);
            }
        } وإلا {
            مجموعه = static_cast<Unicode_مجموعه*>(canon_شروع_Sets[static_cast<int32_t>(canon_مقدار & CANON_مقدار_ماسک)]);
        }
        مجموعه->جمع_کن(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
صنف Init_Canon_Iter_داده {
عام:
    ساكن فراغ doInit(Normalizer2Impl *impl, UError_کد &error_کد);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ساكن فراغ U_CALLCONV
init_Canon_Iter_داده(Normalizer2Impl *impl, UError_کد &error_کد) {
    Init_Canon_Iter_داده::doInit(impl, error_کد);
}

U_CDECL_انتها

فراغ Init_Canon_Iter_داده::doInit(Normalizer2Impl *impl, UError_کد &error_کد) {
    U_ASSERT(impl->f_Canon_Iter_داده == اشاره_تهی);
    impl->f_Canon_Iter_داده = جديد Canon_Iter_داده(error_کد);
    إذا (impl->f_Canon_Iter_داده == اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
    }
    إذا (U_SUCCESS(error_کد)) {
        UChar32 شروع = 0, انتها;
        uint32_t value;
        طالما ((انتها = ucptrie_getRange(impl->normTrie, شروع,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       اشاره_تهی, اشاره_تهی, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            إذا (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_داده_From_Norm16(شروع, انتها, value, *impl->f_Canon_Iter_داده, error_کد);
            }
            شروع = انتها + 1;
        }
#اگر_تعریف_شده UCPTRIE_DEBUG
        umutablecptrie_مجموعه_Name(impl->f_Canon_Iter_داده->mutableTrie, "CanonIterData");
#پایان_اگر
        impl->f_Canon_Iter_داده->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_داده->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_مقدار_بیتها_32, &error_کد);
        umutablecptrie_close(impl->f_Canon_Iter_داده->mutableTrie);
        impl->f_Canon_Iter_داده->mutableTrie = اشاره_تهی;
    }
    إذا (U_FAILURE(error_کد)) {
        حذف impl->f_Canon_Iter_داده;
        impl->f_Canon_Iter_داده = اشاره_تهی;
    }
}

فراغ Normalizer2Impl::make_Canon_Iter_داده_From_Norm16(UChar32 شروع, UChar32 انتها, ثابت uint16_t norm16,
                                                  Canon_Iter_داده &new_داده,
                                                  UError_کد &error_کد) ثابت {
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
    لكل(UChar32 c=شروع; c<=انتها; ++c) {
        uint32_t old_مقدار = umutablecptrie_get(new_داده.mutableTrie, c);
        uint32_t new_مقدار=old_مقدار;
        إذا(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_مقدار|=CANON_NOT_SEGMENT_STARTER;
            إذا(norm16<MIN_نرمال_MAYBE_YES) {
                new_مقدار|=CANON_HAS_COMPOSITIONS;
            }
        } وإلا إذا(norm16<minYesNo) {
            new_مقدار|=CANON_HAS_COMPOSITIONS;
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
                ثابت uint16_t *mapping=get_داده_For_Yes_Or_No(norm16_2);
                uint16_t اولی_Unit=*mapping;
                int32_t length=اولی_Unit&MAPPING_درازی_ماسک;
                إذا((اولی_Unit&MAPPING_HAS_CCC_LCCC_کلمه)!=0) {
                    إذا(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_مقدار|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                إذا(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_بعدی_UNSAFE(mapping, i, c2);
                    new_داده.جمع_کن_مقصد_شروع_مجموعه(c, c2, error_کد);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    إذا(norm16_2>=minNoNo) {
                        طالما(i<length) {
                            U16_بعدی_UNSAFE(mapping, i, c2);
                            uint32_t c2_مقدار = umutablecptrie_get(new_داده.mutableTrie, c2);
                            إذا((c2_مقدار&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_مجموعه(new_داده.mutableTrie, c2,
                                                   c2_مقدار|CANON_NOT_SEGMENT_STARTER, &error_کد);
                            }
                        }
                    }
                }
            } وإلا {
                // c decomposed to c2 algorithmically; c has cc==0
                new_داده.جمع_کن_مقصد_شروع_مجموعه(c, c2, error_کد);
            }
        }
        إذا(new_مقدار!=old_مقدار) {
            umutablecptrie_مجموعه(new_داده.mutableTrie, c, new_مقدار, &error_کد);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_داده(UError_کد &error_کد) ثابت {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(هذا);
    umtx_initOnce(me->f_Canon_Iter_داده_Init_Once, &init_Canon_Iter_داده, me, error_کد);
    أرجع U_SUCCESS(error_کد);
}

int32_t Normalizer2Impl::get_Canon_مقدار(UChar32 c) ثابت {
    أرجع static_cast<int32_t>(ucptrie_get(f_Canon_Iter_داده->trie, c));
}

ثابت Unicode_مجموعه &Normalizer2Impl::get_Canon_شروع_مجموعه(int32_t n) ثابت {
    أرجع *static_cast<ثابت Unicode_مجموعه*>(f_Canon_Iter_داده->canon_شروع_Sets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ثابت {
    أرجع get_Canon_مقدار(c)>=0;
}

UBool Normalizer2Impl::get_Canon_شروع_مجموعه(UChar32 c, Unicode_مجموعه &مجموعه) ثابت {
    int32_t canon_مقدار=get_Canon_مقدار(c)&~CANON_NOT_SEGMENT_STARTER;
    إذا(canon_مقدار==0) {
        أرجع خطأ;
    }
    مجموعه.clear();
    int32_t value=canon_مقدار&CANON_مقدار_ماسک;
    إذا((canon_مقدار&CANON_HAS_مجموعه)!=0) {
        مجموعه.جمع_کن_All(get_Canon_شروع_مجموعه(value));
    } وإلا إذا(value!=0) {
        مجموعه.جمع_کن(value);
    }
    إذا((canon_مقدار&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        إذا(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_پایه + (c - Hangul::JAMO_L_پایه) * Hangul::JAMO_VT_شمار);
            مجموعه.جمع_کن(syllable, syllable+Hangul::JAMO_VT_شمار-1);
        } وإلا {
            جمع_کن_Composites(getCompositionsList(norm16), مجموعه);
        }
    }
    أرجع درست;
}

U_NAMESPACE_انتها

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_جابجا(ثابت UDataSwapper *ds,
            ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
            UError_کد *p_Error_کد) {
    ثابت UDataInfo *pInfo;
    int32_t سرآیند_اندازه;

    ثابت uint8_t *درون_بایتها;
    uint8_t *بیرون_بایتها;

    ثابت int32_t *درون_Indexes;
    int32_t indexes[Normalizer2Impl::IX_مجموع_اندازه+1];

    int32_t i, جابه_جایی, بعدی_جابه_جایی, size;

    /* udata_swapDataHeader checks the arguments */
    سرآیند_اندازه=udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
    uint8_t formatVersion0=pInfo->format_نسخه[0];
    إذا(!(
        pInfo->داده_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->داده_Format[1]==0x72 &&
        pInfo->داده_Format[2]==0x6d &&
        pInfo->داده_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->داده_Format[0], pInfo->داده_Format[1],
                         pInfo->داده_Format[2], pInfo->داده_Format[3],
                         pInfo->format_نسخه[0]);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    درون_بایتها=(ثابت uint8_t *)درون_داده+سرآیند_اندازه;
    بیرون_بایتها=(بیرون_داده == اشاره_تهی) ? اشاره_تهی : (uint8_t *)بیرون_داده+سرآیند_اندازه;

    درون_Indexes=(ثابت int32_t *)درون_بایتها;
    int32_t min_Indexes_درازی;
    إذا(formatVersion0==1) {
        min_Indexes_درازی=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } وإلا إذا(formatVersion0==2) {
        min_Indexes_درازی=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } وإلا إذا(formatVersion0<=4) {
        min_Indexes_درازی=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } وإلا {
        min_Indexes_درازی=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    إذا(length>=0) {
        length-=سرآیند_اندازه;
        إذا(length<min_Indexes_درازی*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    /* read the first few indexes */
    لكل(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, درون_Indexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_مجموع_اندازه];

    إذا(length>=0) {
        إذا(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        /* copy the data for inaccessible bytes */
        إذا(درون_بایتها!=بیرون_بایتها) {
            uprv_memcpy(بیرون_بایتها, درون_بایتها, size);
        }

        جابه_جایی=0;

        /* swap the int32_t indexes[] */
        بعدی_جابه_جایی=indexes[Normalizer2Impl::IX_NORM_TRIE_جابه_جایی];
        ds->جابجا_Array32(ds, درون_بایتها, بعدی_جابه_جایی-جابه_جایی, بیرون_بایتها, p_Error_کد);
        جابه_جایی=بعدی_جابه_جایی;

        /* swap the trie */
        بعدی_جابه_جایی=indexes[Normalizer2Impl::IX_EXTRA_داده_جابه_جایی];
        utrie_جابجا_Any_نسخه(ds, درون_بایتها+جابه_جایی, بعدی_جابه_جایی-جابه_جایی, بیرون_بایتها+جابه_جایی, p_Error_کد);
        جابه_جایی=بعدی_جابه_جایی;

        /* swap the uint16_t extraData[] */
        بعدی_جابه_جایی=indexes[Normalizer2Impl::IX_SMALL_FCD_جابه_جایی];
        ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, بعدی_جابه_جایی-جابه_جایی, بیرون_بایتها+جابه_جایی, p_Error_کد);
        جابه_جایی=بعدی_جابه_جایی;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        بعدی_جابه_جایی=indexes[Normalizer2Impl::IX_SMALL_FCD_جابه_جایی+1];
        جابه_جایی=بعدی_جابه_جایی;

        U_ASSERT(جابه_جایی==size);
    }

    أرجع سرآیند_اندازه+size;
}

#پایان_اگر  // !UCONFIG_NO_NORMALIZATION
