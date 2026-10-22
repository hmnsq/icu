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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "unicode/normalizer2.h"
#అన్తర్భావ "unicode/stringoptions.h"
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/umutablecptrie.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "normalizer2impl.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucptrie_impl.h"
#అన్తర్భావ "uset_imp.h"
#అన్తర్భావ "uvector.h"

U_NAMESPACE_BEGIN

నామాకాశ {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
అంతఃవరుస uint8_t leadByteForCP(UChar32 c) {
    ఒకవేళ (c <= 0x7f) {
        తిరిగి static_cast<uint8_t>(c);
    } లేకపోతే ఒకవేళ (c <= 0x7ff) {
        తిరిగి static_cast<uint8_t>(0xc0 + (c >> 6));
    } లేకపోతే {
        // Should not occur because ccc(U+0300)!=0.
        తిరిగి 0xe0;
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
UChar32 సంకేతం_బిందువు_From_Valid_UTF8(స్థిరరాశి uint8_t *cp_ప్రారంభం, స్థిరరాశి uint8_t *cp_హద్దు) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cp_ప్రారంభం < cp_హద్దు);
    uint8_t c = *cp_ప్రారంభం;
    ఎంపిక(cp_హద్దు-cp_ప్రారంభం) {
    స్థితి 1:
        తిరిగి c;
    స్థితి 2:
        తిరిగి ((c&0x1f)<<6) | (cp_ప్రారంభం[1]&0x3f);
    స్థితి 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        తిరిగి static_cast<char16_t>((c << 12) | ((cp_ప్రారంభం[1] & 0x3f) << 6) | (cp_ప్రారంభం[2] & 0x3f));
    స్థితి 4:
        తిరిగి ((c&7)<<18) | ((cp_ప్రారంభం[1]&0x3f)<<12) | ((cp_ప్రారంభం[2]&0x3f)<<6) | (cp_ప్రారంభం[3]&0x3f);
    అప్రమేయ:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 మునుపటి_Hangul_Or_Jamo(స్థిరరాశి uint8_t *ప్రారంభం, స్థిరరాశి uint8_t *p) {
    ఒకవేళ ((p - ప్రారంభం) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        ఒకవేళ (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            తిరిగి ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    తిరిగి U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t get_Jamo_TMinus_పునాది(స్థిరరాశి uint8_t *src, స్థిరరాశి uint8_t *హద్దు) {
    // Jamo T: E1 86 A8..E1 87 82
    ఒకవేళ ((హద్దు - src) >= 3 && *src == 0xe1) {
        ఒకవేళ (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            ఒకవేళ (0xa8 <= t && t <= 0xbf) {
                తిరిగి t - 0xa7;
            }
        } లేకపోతే ఒకవేళ (src[1] == 0x87) {
            uint8_t t = src[2];
            ఒకవేళ (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                తిరిగి t - (0xa7 - 0x40);
            }
        }
    }
    తిరిగి -1;
}

శూన్య
append_సంకేతం_బిందువు_Delta(స్థిరరాశి uint8_t *cp_ప్రారంభం, స్థిరరాశి uint8_t *cp_హద్దు, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    అక్షరం బఫరు[U8_MAX_నిడివి];
    int32_t length;
    int32_t cp_నిడివి = static_cast<int32_t>(cp_హద్దు - cp_ప్రారంభం);
    ఒకవేళ (cp_నిడివి == 1) {
        // The builder makes ASCII map to ASCII.
        బఫరు[0] = static_cast<uint8_t>(*cp_ప్రారంభం + delta);
        length = 1;
    } లేకపోతే {
        int32_t trail = *(cp_హద్దు-1) + delta;
        ఒకవేళ (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cp_హద్దు;
            length = 0;
            చేయి { బఫరు[length++] = *cp_ప్రారంభం++; } అయితే (cp_ప్రారంభం < cp_హద్దు);
            బఫరు[length++] = static_cast<uint8_t>(trail);
        } లేకపోతే {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = సంకేతం_బిందువు_From_Valid_UTF8(cp_ప్రారంభం, cp_హద్దు) + delta;
            length = 0;
            U8_APPEND_UNSAFE(బఫరు, length, c);
        }
    }
    ఒకవేళ (edits != శూన్యసూచిక) {
        edits->కూడు_Replace(cp_నిడివి, length);
    }
    sink.Append(బఫరు, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_బఫరు::Reordering_బఫరు(స్థిరరాశి Normalizer2Impl &ni, UnicodeString &dest,
                                   UError_సంకేతం &error_సంకేతం) :
        impl(ni), str(dest),
        ప్రారంభం(str.get_బఫరు(8)), reorder_ప్రారంభం(ప్రారంభం), హద్దు(ప్రారంభం),
        మిగిలిన_సామర్థ్యం(str.get_సామర్థ్యం()), చివరి_CC(0) {
    ఒకవేళ (ప్రారంభం == శూన్యసూచిక && U_SUCCESS(error_సంకేతం)) {
        // getBuffer() already did str.setToBogus()
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

UBool Reordering_బఫరు::init(int32_t dest_సామర్థ్యం, UError_సంకేతం &error_సంకేతం) {
    int32_t length=str.length();
    ప్రారంభం=str.get_బఫరు(dest_సామర్థ్యం);
    ఒకవేళ(ప్రారంభం==శూన్యసూచిక) {
        // getBuffer() already did str.setToBogus()
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి అబద్ధం;
    }
    హద్దు=ప్రారంభం+length;
    మిగిలిన_సామర్థ్యం=str.get_సామర్థ్యం()-length;
    reorder_ప్రారంభం=ప్రారంభం;
    ఒకవేళ(ప్రారంభం==హద్దు) {
        చివరి_CC=0;
    } లేకపోతే {
        సమితి_Iterator();
        చివరి_CC=మునుపటి_CC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        ఒకవేళ(చివరి_CC>1) {
            అయితే(మునుపటి_CC()>1) {}
        }
        reorder_ప్రారంభం=సంకేతం_బిందువు_హద్దు;
    }
    తిరిగి నిజం;
}

UBool Reordering_బఫరు::equals(స్థిరరాశి char16_t *other_ప్రారంభం, స్థిరరాశి char16_t *other_హద్దు) స్థిరరాశి {
    int32_t length = static_cast<int32_t>(హద్దు - ప్రారంభం);
    తిరిగి
        length == static_cast<int32_t>(other_హద్దు - other_ప్రారంభం) &&
        0==u_memcmp(ప్రారంభం, other_ప్రారంభం, length);
}

UBool Reordering_బఫరు::equals(స్థిరరాశి uint8_t *other_ప్రారంభం, స్థిరరాశి uint8_t *other_హద్దు) స్థిరరాశి {
    U_ASSERT((other_హద్దు - other_ప్రారంభం) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(హద్దు - ప్రారంభం);
    int32_t other_నిడివి = static_cast<int32_t>(other_హద్దు - other_ప్రారంభం);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    ఒకవేళ (other_నిడివి < length || (other_నిడివి / 3) > length) {
        తిరిగి అబద్ధం;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    కొరకు (int32_t i = 0, j = 0;;) {
        ఒకవేళ (i >= length) {
            తిరిగి j >= other_నిడివి;
        } లేకపోతే ఒకవేళ (j >= other_నిడివి) {
            తిరిగి అబద్ధం;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_తదుపరి_UNSAFE(ప్రారంభం, i, c);
        U8_తదుపరి_UNSAFE(other_ప్రారంభం, j, other);
        ఒకవేళ (c != other) {
            తిరిగి అబద్ధం;
        }
    }
}

UBool Reordering_బఫరు::appendSupplementary(UChar32 c, uint8_t cc, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(మిగిలిన_సామర్థ్యం<2 && !resize(2, error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(చివరి_CC<=cc || cc==0) {
        హద్దు[0]=U16_LEAD(c);
        హద్దు[1]=U16_TRAIL(c);
        హద్దు+=2;
        చివరి_CC=cc;
        ఒకవేళ(cc<=1) {
            reorder_ప్రారంభం=హద్దు;
        }
    } లేకపోతే {
        insert(c, cc);
    }
    మిగిలిన_సామర్థ్యం-=2;
    తిరిగి నిజం;
}

UBool Reordering_బఫరు::append(స్థిరరాశి char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(length==0) {
        తిరిగి నిజం;
    }
    ఒకవేళ(మిగిలిన_సామర్థ్యం<length && !resize(length, error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    మిగిలిన_సామర్థ్యం-=length;
    ఒకవేళ(చివరి_CC<=leadCC || leadCC==0) {
        ఒకవేళ(trailCC<=1) {
            reorder_ప్రారంభం=హద్దు+length;
        } లేకపోతే ఒకవేళ(leadCC<=1) {
            reorder_ప్రారంభం=హద్దు+1;  // Ok if not a code point boundary.
        }
        స్థిరరాశి char16_t *s_హద్దు=s+length;
        చేయి { *హద్దు++=*s++; } అయితే(s!=s_హద్దు);
        చివరి_CC=trailCC;
    } లేకపోతే {
        int32_t i=0;
        UChar32 c;
        U16_తదుపరి(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        అయితే(i<length) {
            U16_తదుపరి(s, i, length, c);
            ఒకవేళ(i<length) {
                ఒకవేళ (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } లేకపోతే {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } లేకపోతే {
                leadCC=trailCC;
            }
            append(c, leadCC, error_సంకేతం);
        }
    }
    తిరిగి నిజం;
}

UBool Reordering_బఫరు::appendZeroCC(UChar32 c, UError_సంకేతం &error_సంకేతం) {
    int32_t cp_నిడివి=U16_నిడివి(c);
    ఒకవేళ(మిగిలిన_సామర్థ్యం<cp_నిడివి && !resize(cp_నిడివి, error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    మిగిలిన_సామర్థ్యం-=cp_నిడివి;
    ఒకవేళ(cp_నిడివి==1) {
        *హద్దు++ = static_cast<char16_t>(c);
    } లేకపోతే {
        హద్దు[0]=U16_LEAD(c);
        హద్దు[1]=U16_TRAIL(c);
        హద్దు+=2;
    }
    చివరి_CC=0;
    reorder_ప్రారంభం=హద్దు;
    తిరిగి నిజం;
}

UBool Reordering_బఫరు::appendZeroCC(స్థిరరాశి char16_t *s, స్థిరరాశి char16_t *s_హద్దు, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(s==s_హద్దు) {
        తిరిగి నిజం;
    }
    int32_t length = static_cast<int32_t>(s_హద్దు - s);
    ఒకవేళ(మిగిలిన_సామర్థ్యం<length && !resize(length, error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    u_memcpy(హద్దు, s, length);
    హద్దు+=length;
    మిగిలిన_సామర్థ్యం-=length;
    చివరి_CC=0;
    reorder_ప్రారంభం=హద్దు;
    తిరిగి నిజం;
}

శూన్య Reordering_బఫరు::remove() {
    reorder_ప్రారంభం=హద్దు=ప్రారంభం;
    మిగిలిన_సామర్థ్యం=str.get_సామర్థ్యం();
    చివరి_CC=0;
}

శూన్య Reordering_బఫరు::removeSuffix(int32_t suffix_నిడివి) {
    ఒకవేళ(suffix_నిడివి<(హద్దు-ప్రారంభం)) {
        హద్దు-=suffix_నిడివి;
        మిగిలిన_సామర్థ్యం+=suffix_నిడివి;
    } లేకపోతే {
        హద్దు=ప్రారంభం;
        మిగిలిన_సామర్థ్యం=str.get_సామర్థ్యం();
    }
    చివరి_CC=0;
    reorder_ప్రారంభం=హద్దు;
}

UBool Reordering_బఫరు::resize(int32_t append_నిడివి, UError_సంకేతం &error_సంకేతం) {
    int32_t reorder_ప్రారంభం_సూచీ = static_cast<int32_t>(reorder_ప్రారంభం - ప్రారంభం);
    int32_t length = static_cast<int32_t>(హద్దు - ప్రారంభం);
    str.release_బఫరు(length);
    int32_t new_సామర్థ్యం=length+append_నిడివి;
    int32_t double_సామర్థ్యం=2*str.get_సామర్థ్యం();
    ఒకవేళ(new_సామర్థ్యం<double_సామర్థ్యం) {
        new_సామర్థ్యం=double_సామర్థ్యం;
    }
    ఒకవేళ(new_సామర్థ్యం<256) {
        new_సామర్థ్యం=256;
    }
    ప్రారంభం=str.get_బఫరు(new_సామర్థ్యం);
    ఒకవేళ(ప్రారంభం==శూన్యసూచిక) {
        // getBuffer() already did str.setToBogus()
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి అబద్ధం;
    }
    reorder_ప్రారంభం=ప్రారంభం+reorder_ప్రారంభం_సూచీ;
    హద్దు=ప్రారంభం+length;
    మిగిలిన_సామర్థ్యం=str.get_సామర్థ్యం()-length;
    తిరిగి నిజం;
}

శూన్య Reordering_బఫరు::skip_మునుపటి() {
    సంకేతం_బిందువు_హద్దు=సంకేతం_బిందువు_ప్రారంభం;
    char16_t c=*--సంకేతం_బిందువు_ప్రారంభం;
    ఒకవేళ(U16_IS_TRAIL(c) && ప్రారంభం<సంకేతం_బిందువు_ప్రారంభం && U16_IS_LEAD(*(సంకేతం_బిందువు_ప్రారంభం-1))) {
        --సంకేతం_బిందువు_ప్రారంభం;
    }
}

uint8_t Reordering_బఫరు::మునుపటి_CC() {
    సంకేతం_బిందువు_హద్దు=సంకేతం_బిందువు_ప్రారంభం;
    ఒకవేళ(reorder_ప్రారంభం>=సంకేతం_బిందువు_ప్రారంభం) {
        తిరిగి 0;
    }
    UChar32 c=*--సంకేతం_బిందువు_ప్రారంభం;
    char16_t c2;
    ఒకవేళ(U16_IS_TRAIL(c) && ప్రారంభం<సంకేతం_బిందువు_ప్రారంభం && U16_IS_LEAD(c2=*(సంకేతం_బిందువు_ప్రారంభం-1))) {
        --సంకేతం_బిందువు_ప్రారంభం;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    తిరిగి impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
శూన్య Reordering_బఫరు::insert(UChar32 c, uint8_t cc) {
    కొరకు(సమితి_Iterator(), skip_మునుపటి(); మునుపటి_CC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=హద్దు;
    char16_t *r=హద్దు+=U16_నిడివి(c);
    చేయి {
        *--r=*--q;
    } అయితే(సంకేతం_బిందువు_హద్దు!=q);
    write_సంకేతం_బిందువు(q, c);
    ఒకవేళ(cc<=1) {
        reorder_ప్రారంభం=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

నిర్మాణం Canon_Iter_దత్తాంశం : బహిరంగ UMemory {
    Canon_Iter_దత్తాంశం(UError_సంకేతం &error_సంకేతం);
    ~Canon_Iter_దత్తాంశం();
    శూన్య కూడు_గమ్యం_ప్రారంభం_సమితి(UChar32 origin, UChar32 decompLead, UError_సంకేతం &error_సంకేతం);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canon_ప్రారంభం_Sets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    తొలగించు f_Canon_Iter_దత్తాంశం;
}

శూన్య
Normalizer2Impl::init(స్థిరరాశి int32_t *లోపల_Indexes, స్థిరరాశి UCPTrie *లోపల_Trie,
                      స్థిరరాశి uint16_t *లోపల_Extra_దత్తాంశం, స్థిరరాశి uint8_t *లోపల_Small_FCD) {
    minDecompNoCP = static_cast<char16_t>(లోపల_Indexes[IX_MIN_DECOMP_NO_CP]);
    minCompNoMaybeCP = static_cast<char16_t>(లోపల_Indexes[IX_MIN_COMP_NO_MAYBE_CP]);
    minLcccCP = static_cast<char16_t>(లోపల_Indexes[IX_MIN_LCCC_CP]);

    minYesNo = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_YES_NO]);
    minYesNoMappingsOnly = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_YES_NO_MAPPINGS_ONLY]);
    minNoNo = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_NO_NO]);
    minNoNoCompBoundaryBefore = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE]);
    minNoNoCompNoMaybeCC = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC]);
    min_No_No_ఖాళీలు = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_NO_NO_ఖాళీలు]);
    హద్దు_No_No = static_cast<uint16_t>(లోపల_Indexes[IX_హద్దు_NO_NO]);
    minMaybeNo = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_MAYBE_NO]);
    minMaybeNoCombinesFwd = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_MAYBE_NO_COMBINES_FWD]);
    minMaybeYes = static_cast<uint16_t>(లోపల_Indexes[IX_MIN_MAYBE_YES]);
    U_ASSERT((minMaybeNo & 7) == 0);  // 8-aligned for noNoDelta bit fields
    centerNoNoDelta = (minMaybeNo >> DELTA_జరుపు) - MAX_DELTA - 1;

    normTrie=లోపల_Trie;
    extra_దత్తాంశం=లోపల_Extra_దత్తాంశం;
    smallFCD=లోపల_Small_FCD;
}

U_CDECL_BEGIN

స్థిర uint32_t U_CALLCONV
segmentStarterMapper(స్థిరరాశి శూన్య * /*context*/, uint32_t value) {
    తిరిగి value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_చివర

శూన్య
Normalizer2Impl::కూడు_Lccc_Chars(Unicode_సమితి &సమితి) స్థిరరాశి {
    UChar32 ప్రారంభం = 0, చివర;
    uint32_t norm16;
    అయితే ((చివర = ucptrie_getRange(normTrie, ప్రారంభం, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   శూన్యసూచిక, శూన్యసూచిక, &norm16)) >= 0) {
        ఒకవేళ (norm16 > Normalizer2Impl::MIN_సాధారణ_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            సమితి.కూడు(ప్రారంభం, చివర);
        } లేకపోతే ఒకవేళ (minNoNoCompNoMaybeCC <= norm16 && norm16 < హద్దు_No_No) {
            uint16_t fcd16 = getFCD16(ప్రారంభం);
            ఒకవేళ (fcd16 > 0xff) { సమితి.కూడు(ప్రారంభం, చివర); }
        }
        ప్రారంభం = చివర + 1;
    }
}

శూన్య
Normalizer2Impl::కూడు_Property_Starts(స్థిరరాశి USetAdder *sa, UError_సంకేతం & /*errorCode*/) స్థిరరాశి {
    // Add the start code point of each same-value range of the trie.
    UChar32 ప్రారంభం = 0, చివర;
    uint32_t value;
    అయితే ((చివర = ucptrie_getRange(normTrie, ప్రారంభం, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   శూన్యసూచిక, శూన్యసూచిక, &value)) >= 0) {
        sa->కూడు(sa->సమితి, ప్రారంభం);
        ఒకవేళ (ప్రారంభం != చివర && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_ముసుగు) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(ప్రారంభం);
            అయితే (++ప్రారంభం <= చివర) {
                uint16_t fcd16 = getFCD16(ప్రారంభం);
                ఒకవేళ (fcd16 != prevFCD16) {
                    sa->కూడు(sa->సమితి, ప్రారంభం);
                    prevFCD16 = fcd16;
                }
            }
        }
        ప్రారంభం = చివర + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    కొరకు(char16_t c=Hangul::HANGUL_పునాది; c<Hangul::HANGUL_హద్దు; c+=Hangul::JAMO_T_లెక్క) {
        sa->కూడు(sa->సమితి, c);
        sa->కూడు(sa->సమితి, c+1);
    }
    sa->కూడు(sa->సమితి, Hangul::HANGUL_హద్దు); /* add Hangul+1 to continue with other properties */
}

శూన్య
Normalizer2Impl::కూడు_Canon_Iter_Property_Starts(స్థిరరాశి USetAdder *sa, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    ఒకవేళ (!ensure_Canon_Iter_దత్తాంశం(error_సంకేతం)) { తిరిగి; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 ప్రారంభం = 0, చివర;
    uint32_t value;
    అయితే ((చివర = ucptrie_getRange(f_Canon_Iter_దత్తాంశం->trie, ప్రారంభం, UCPMAP_RANGE_సాధారణ, 0,
                                   segmentStarterMapper, శూన్యసూచిక, &value)) >= 0) {
        sa->కూడు(sa->సమితి, ప్రారంభం);
        ప్రారంభం = చివర + 1;
    }
}

స్థిరరాశి char16_t *
Normalizer2Impl::copy_దిగువ_ఉపసర్గ_From_Nul_Terminated(స్థిరరాశి char16_t *src,
                                                UChar32 min_Need_దత్తాంశం_CP,
                                                Reordering_బఫరు *బఫరు,
                                                UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    స్థిరరాశి char16_t *prevSrc=src;
    char16_t c;
    అయితే((c=*src++)<min_Need_దత్తాంశం_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    ఒకవేళ(--src!=prevSrc) {
        ఒకవేళ(బఫరు!=శూన్యసూచిక) {
            బఫరు->appendZeroCC(prevSrc, src, error_సంకేతం);
        }
    }
    తిరిగి src;
}

UnicodeString &
Normalizer2Impl::decompose(స్థిరరాశి UnicodeString &src, UnicodeString &dest,
                           UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        dest.సమితి_గమ్యం_Bogus();
        తిరిగి dest;
    }
    స్థిరరాశి char16_t *sArray=src.get_బఫరు();
    ఒకవేళ(&dest==&src || sArray==శూన్యసూచిక) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        dest.సమితి_గమ్యం_Bogus();
        తిరిగి dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), error_సంకేతం);
    తిరిగి dest;
}

శూన్య
Normalizer2Impl::decompose(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                           UnicodeString &dest,
                           int32_t dest_నిడివి_Estimate,
                           UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(dest_నిడివి_Estimate<0 && హద్దు!=శూన్యసూచిక) {
        dest_నిడివి_Estimate = static_cast<int32_t>(హద్దు - src);
    }
    dest.remove();
    Reordering_బఫరు బఫరు(*ఇది, dest);
    ఒకవేళ(బఫరు.init(dest_నిడివి_Estimate, error_సంకేతం)) {
        decompose(src, హద్దు, &బఫరు, error_సంకేతం);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
స్థిరరాశి char16_t *
Normalizer2Impl::decompose(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                           Reordering_బఫరు *బఫరు,
                           UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(src==శూన్యసూచిక) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి src;
    }
    UChar32 minNoCP=minDecompNoCP;
    ఒకవేళ(హద్దు==శూన్యసూచిక) {
        src=copy_దిగువ_ఉపసర్గ_From_Nul_Terminated(src, minNoCP, బఫరు, error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి src;
        }
        హద్దు=u_strchr(src, 0);
    }

    స్థిరరాశి char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    స్థిరరాశి char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    కొరకు(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        కొరకు(prevSrc=src; src!=హద్దు;) {
            ఒకవేళ( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } లేకపోతే ఒకవేళ(!U16_IS_LEAD(c)) {
                ఆపు;
            } లేకపోతే {
                char16_t c2;
                ఒకవేళ((src+1)!=హద్దు && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    ఒకవేళ(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } లేకపోతే {
                        ఆపు;
                    }
                } లేకపోతే {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        ఒకవేళ(src!=prevSrc) {
            ఒకవేళ(బఫరు!=శూన్యసూచిక) {
                ఒకవేళ(!బఫరు->appendZeroCC(prevSrc, src, error_సంకేతం)) {
                    ఆపు;
                }
            } లేకపోతే {
                prevCC=0;
                prevBoundary=src;
            }
        }
        ఒకవేళ(src==హద్దు) {
            ఆపు;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_నిడివి(c);
        ఒకవేళ(బఫరు!=శూన్యసూచిక) {
            ఒకవేళ(!decompose(c, norm16, *బఫరు, error_సంకేతం)) {
                ఆపు;
            }
        } లేకపోతే {
            ఒకవేళ(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                ఒకవేళ(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    ఒకవేళ(cc<=1) {
                        prevBoundary=src;
                    }
                    కొనసాగు;
                }
            }
            తిరిగి prevBoundary;  // "no" or cc out of order
        }
    }
    తిరిగి src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
స్థిరరాశి char16_t *
Normalizer2Impl::decomposeShort(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    అయితే(src<హద్దు) {
        ఒకవేళ (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            తిరిగి src;
        }
        స్థిరరాశి char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_తదుపరి(normTrie, UCPTRIE_16, src, హద్దు, c, norm16);
        ఒకవేళ (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            తిరిగి prevSrc;
        }
        ఒకవేళ(!decompose(c, norm16, బఫరు, error_సంకేతం)) {
            తిరిగి శూన్యసూచిక;
        }
        ఒకవేళ (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            తిరిగి src;
        }
    }
    తిరిగి src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_బఫరు &బఫరు,
                                 UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    // get the decomposition and the lead and trail cc's
    ఒకవేళ (norm16 >= హద్దు_No_No) {
        ఒకవేళ (isMaybeYesOrNonZeroCC(norm16)) {
            తిరిగి బఫరు.append(c, getCCFromYesOrMaybeYes(norm16), error_సంకేతం);
        } లేకపోతే ఒకవేళ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ఒకవేళ (norm16 < minYesNo) {
        // c does not decompose
        తిరిగి బఫరు.append(c, 0, error_సంకేతం);
    } లేకపోతే ఒకవేళ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        తిరిగి బఫరు.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_సంకేతం);
    }
    // c decomposes, get everything from the variable-length extra data
    స్థిరరాశి uint16_t *mapping=get_దత్తాంశం(norm16);
    uint16_t మొదటి_Unit=*mapping;
    int32_t length=మొదటి_Unit&MAPPING_నిడివి_ముసుగు;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(మొదటి_Unit >> 8);
    ఒకవేళ(మొదటి_Unit&MAPPING_HAS_CCC_LCCC_మాట) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } లేకపోతే {
        leadCC=0;
    }
    తిరిగి బఫరు.append(reinterpret_cast<స్థిరరాశి char16_t*>(mapping) + 1, length, నిజం, leadCC, trailCC, error_సంకేతం);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
స్థిరరాశి uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               స్థిరరాశి uint8_t *src, స్థిరరాశి uint8_t *హద్దు,
                               ByteSink *sink, Edits *edits, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    U_ASSERT(హద్దు != శూన్యసూచిక);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    స్థిరరాశి uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    కొరకు (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        స్థిరరాశి uint8_t *fast_ప్రారంభం = src;
        స్థిరరాశి uint8_t *prevSrc;
        uint16_t norm16 = 0;

        కొరకు (;;) {
            ఒకవేళ (src == హద్దు) {
                ఒకవేళ (prevBoundary != హద్దు && sink != శూన్యసూచిక) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, హద్దు,
                                                  *sink, options, edits, error_సంకేతం);
                }
                తిరిగి src;
            }
            ఒకవేళ (*src < minNoLead) {
                ++src;
            } లేకపోతే {
                prevSrc = src;
                UCPTRIE_FAST_U8_తదుపరి(normTrie, UCPTRIE_16, src, హద్దు, norm16);
                ఒకవేళ (!isMostDecompYesAndZeroCC(norm16)) {
                    ఆపు;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        ఒకవేళ (prevSrc != fast_ప్రారంభం) {
            // The fast path looped over yes/0 characters before the current one.
            ఒకవేళ (sink != శూన్యసూచిక &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_సంకేతం)) {
                ఆపు;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        ఒకవేళ (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            ఒకవేళ (prevCC <= cc || cc == 0) {
                prevCC = cc;
                ఒకవేళ (cc <= 1) {
                    ఒకవేళ (sink != శూన్యసూచిక &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, error_సంకేతం)) {
                        ఆపు;
                    }
                    prevBoundary = src;
                }
                కొనసాగు;
            }
        }
        ఒకవేళ (sink == శూన్యసూచిక) {
            తిరిగి prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        ఒకవేళ (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            ఒకవేళ (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, error_సంకేతం)) {
                ఆపు;
            }
            prevBoundary = prevSrc;
        }
        Reordering_బఫరు బఫరు(*ఇది, s16, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            ఆపు;
        }
        decomposeShort(prevBoundary, src, STOP_AT_హద్దు, అబద్ధం /* onlyContiguous */,
                       బఫరు, error_సంకేతం);
        // Decompose until the next boundary.
        ఒకవేళ (బఫరు.get_చివరి_CC() > 1) {
            src = decomposeShort(src, హద్దు, STOP_AT_DECOMP_BOUNDARY, అబద్ధం /* onlyContiguous */,
                                 బఫరు, error_సంకేతం);
        }
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            ఆపు;
        }
        ఒకవేళ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
            ఆపు;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        ఒకవేళ (isMaybeYesOrNonZeroCC(norm16) && బఫరు.equals(prevBoundary, src)) {
            ఒకవేళ (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, error_సంకేతం)) {
                ఆపు;
            }
        } లేకపోతే {
            ఒకవేళ (!ByteSinkUtil::appendChange(prevBoundary, src, బఫరు.get_ప్రారంభం(), బఫరు.length(),
                                            *sink, edits, error_సంకేతం)) {
                ఆపు;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    తిరిగి src;
}

స్థిరరాశి uint8_t *
Normalizer2Impl::decomposeShort(స్థిరరాశి uint8_t *src, స్థిరరాశి uint8_t *హద్దు,
                                StopAt stopAt, UBool onlyContiguous,
                                Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    అయితే (src < హద్దు) {
        స్థిరరాశి uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_తదుపరి(normTrie, UCPTRIE_16, src, హద్దు, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        ఒకవేళ (norm16 >= హద్దు_No_No) {
            ఒకవేళ (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                ఒకవేళ (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    తిరిగి prevSrc;
                }
                c = సంకేతం_బిందువు_From_Valid_UTF8(prevSrc, src);
                ఒకవేళ (!బఫరు.append(c, cc, error_సంకేతం)) {
                    తిరిగి శూన్యసూచిక;
                }
                ఒకవేళ (stopAt == STOP_AT_DECOMP_BOUNDARY && బఫరు.get_చివరి_CC() <= 1) {
                    తిరిగి src;
                }
                కొనసాగు;
            } లేకపోతే ఒకవేళ (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                ఒకవేళ (stopAt != STOP_AT_హద్దు) {
                    తిరిగి prevSrc;
                }
                c = సంకేతం_బిందువు_From_Valid_UTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } లేకపోతే ఒకవేళ (stopAt != STOP_AT_హద్దు && norm16 < minNoNoCompNoMaybeCC) {
            తిరిగి prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        ఒకవేళ (norm16 < minYesNo) {
            ఒకవేళ (c < 0) {
                c = సంకేతం_బిందువు_From_Valid_UTF8(prevSrc, src);
            }
            // does not decompose
            ఒకవేళ (!బఫరు.append(c, 0, error_సంకేతం)) {
                తిరిగి శూన్యసూచిక;
            }
        } లేకపోతే ఒకవేళ (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            ఒకవేళ (c < 0) {
                c = సంకేతం_బిందువు_From_Valid_UTF8(prevSrc, src);
            }
            char16_t jamos[3];
            ఒకవేళ (!బఫరు.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_సంకేతం)) {
                తిరిగి శూన్యసూచిక;
            }
        } లేకపోతే {
            // The character decomposes, get everything from the variable-length extra data.
            స్థిరరాశి uint16_t *mapping = get_దత్తాంశం(norm16);
            uint16_t మొదటి_Unit = *mapping;
            int32_t length = మొదటి_Unit & MAPPING_నిడివి_ముసుగు;
            uint8_t trailCC = static_cast<uint8_t>(మొదటి_Unit >> 8);
            uint8_t leadCC;
            ఒకవేళ (మొదటి_Unit & MAPPING_HAS_CCC_LCCC_మాట) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } లేకపోతే {
                leadCC = 0;
            }
            ఒకవేళ (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                తిరిగి prevSrc;
            }
            ఒకవేళ (!బఫరు.append(reinterpret_cast<స్థిరరాశి char16_t*>(mapping) + 1, length, నిజం, leadCC, trailCC, error_సంకేతం)) {
                తిరిగి శూన్యసూచిక;
            }
        }
        ఒకవేళ ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && బఫరు.get_చివరి_CC() <= 1)) {
            తిరిగి src;
        }
    }
    తిరిగి src;
}

స్థిరరాశి char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t బఫరు[4], int32_t &length) స్థిరరాశి {
    uint16_t norm16;
    ఒకవేళ(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి char16_t *decomp = శూన్యసూచిక;
    ఒకవేళ(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=బఫరు;
        length=0;
        U16_APPEND_UNSAFE(బఫరు, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    ఒకవేళ (norm16 < minYesNo) {
        తిరిగి decomp;
    } లేకపోతే ఒకవేళ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, బఫరు);
        తిరిగి బఫరు;
    }
    // c decomposes, get everything from the variable-length extra data
    స్థిరరాశి uint16_t *mapping=get_దత్తాంశం(norm16);
    length=*mapping&MAPPING_నిడివి_ముసుగు;
    తిరిగి reinterpret_cast<స్థిరరాశి char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
స్థిరరాశి char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t బఫరు[30], int32_t &length) స్థిరరాశి {
    uint16_t norm16;
    ఒకవేళ(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        తిరిగి శూన్యసూచిక;
    } లేకపోతే ఒకవేళ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, బఫరు);
        length=2;
        తిరిగి బఫరు;
    } లేకపోతే ఒకవేళ(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(బఫరు, length, c);
        తిరిగి బఫరు;
    }
    // c decomposes, get everything from the variable-length extra data
    స్థిరరాశి uint16_t *mapping=get_దత్తాంశం(norm16);
    uint16_t మొదటి_Unit=*mapping;
    int32_t m_నిడివి=మొదటి_Unit&MAPPING_నిడివి_ముసుగు;  // length of normal mapping
    ఒకవేళ(మొదటి_Unit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        స్థిరరాశి uint16_t *rawMapping=mapping-((మొదటి_Unit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        ఒకవేళ(rm0<=MAPPING_నిడివి_ముసుగు) {
            length=rm0;
            తిరిగి reinterpret_cast<స్థిరరాశి char16_t*>(rawMapping) - rm0;
        } లేకపోతే {
            // Copy the normal mapping and replace its first two code units with rm0.
            బఫరు[0] = static_cast<char16_t>(rm0);
            u_memcpy(బఫరు + 1, reinterpret_cast<స్థిరరాశి char16_t*>(mapping) + 1 + 2, m_నిడివి - 2);
            length=m_నిడివి-1;
            తిరిగి బఫరు;
        }
    } లేకపోతే {
        length=m_నిడివి;
        తిరిగి reinterpret_cast<స్థిరరాశి char16_t*>(mapping) + 1;
    }
}

శూన్య Normalizer2Impl::decomposeAndAppend(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                                         UBool doDecompose,
                                         UnicodeString &సురక్షిత_మధ్య,
                                         Reordering_బఫరు &బఫరు,
                                         UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    బఫరు.copy_Reorderable_Suffix_గమ్యం(సురక్షిత_మధ్య);
    ఒకవేళ(doDecompose) {
        decompose(src, హద్దు, &బఫరు, error_సంకేతం);
        తిరిగి;
    }
    // Just merge the strings at the boundary.
    తార్కిక is_మొదటి = నిజం;
    uint8_t మొదటి_CC = 0, prevCC = 0, cc;
    స్థిరరాశి char16_t *p = src;
    అయితే (p != హద్దు) {
        స్థిరరాశి char16_t *సంకేతం_బిందువు_ప్రారంభం = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_తదుపరి(normTrie, UCPTRIE_16, p, హద్దు, c, norm16);
        ఒకవేళ ((cc = getCC(norm16)) == 0) {
            p = సంకేతం_బిందువు_ప్రారంభం;
            ఆపు;
        }
        ఒకవేళ (is_మొదటి) {
            మొదటి_CC = cc;
            is_మొదటి = అబద్ధం;
        }
        prevCC = cc;
    }
    ఒకవేళ(హద్దు==శూన్యసూచిక) {  // appendZeroCC() needs limit!=nullptr
        హద్దు=u_strchr(p, 0);
    }

    ఒకవేళ (బఫరు.append(src, static_cast<int32_t>(p - src), అబద్ధం, మొదటి_CC, prevCC, error_సంకేతం)) {
        బఫరు.appendZeroCC(p, హద్దు, error_సంకేతం);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) స్థిరరాశి {
    తిరిగి c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) స్థిరరాశి {
    ఒకవేళ (norm16 < minNoNoCompNoMaybeCC) {
        తిరిగి నిజం;
    }
    ఒకవేళ (norm16 >= హద్దు_No_No) {
        తిరిగి norm16 <= MIN_సాధారణ_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    స్థిరరాశి uint16_t *mapping=get_దత్తాంశం_For_Yes_Or_No(norm16);
    uint16_t మొదటి_Unit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    తిరిగి (మొదటి_Unit&MAPPING_HAS_CCC_LCCC_మాట)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) స్థిరరాశి {
    ఒకవేళ (c < minDecompNoCP) {
        తిరిగి నిజం;
    }
    ఒకవేళ (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        తిరిగి నిజం;
    }
    తిరిగి norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) స్థిరరాశి {
    ఒకవేళ(norm16 <= minYesNo || isHangulLVT(norm16)) {
        తిరిగి నిజం;
    }
    ఒకవేళ (norm16 >= హద్దు_No_No) {
        ఒకవేళ (isMaybeYesOrNonZeroCC(norm16)) {
            తిరిగి norm16 <= MIN_సాధారణ_MAYBE_YES || norm16 == JAMO_VT;
        } లేకపోతే ఒకవేళ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            తిరిగి (norm16 & DELTA_TCCC_ముసుగు) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    స్థిరరాశి uint16_t *mapping=get_దత్తాంశం(norm16);
    uint16_t మొదటి_Unit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    ఒకవేళ(మొదటి_Unit>0x1ff) {
        తిరిగి అబద్ధం;  // trailCC>1
    }
    ఒకవేళ(మొదటి_Unit<=0xff) {
        తిరిగి నిజం;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    తిరిగి (మొదటి_Unit&MAPPING_HAS_CCC_LCCC_మాట)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(స్థిరరాశి uint16_t *list, UChar32 trail) {
    uint16_t key1, మొదటి_Unit;
    ఒకవేళ(trail<COMP_1_TRAIL_హద్దు) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        అయితే(key1>(మొదటి_Unit=*list)) {
            list+=2+(మొదటి_Unit&COMP_1_TRIPLE);
        }
        ఒకవేళ(key1==(మొదటి_Unit&COMP_1_TRAIL_ముసుగు)) {
            ఒకవేళ(మొదటి_Unit&COMP_1_TRIPLE) {
                తిరిగి (static_cast<int32_t>(list[1]) << 16) | list[2];
            } లేకపోతే {
                తిరిగి list[1];
            }
        }
    } లేకపోతే {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_హద్దు +
                        (((trail>>COMP_1_TRAIL_జరుపు))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_జరుపు);
        uint16_t రెండవ_Unit;
        కొరకు(;;) {
            ఒకవేళ(key1>(మొదటి_Unit=*list)) {
                list+=2+(మొదటి_Unit&COMP_1_TRIPLE);
            } లేకపోతే ఒకవేళ(key1==(మొదటి_Unit&COMP_1_TRAIL_ముసుగు)) {
                ఒకవేళ(key2>(రెండవ_Unit=list[1])) {
                    ఒకవేళ(మొదటి_Unit&COMP_1_చివరి_TUPLE) {
                        ఆపు;
                    } లేకపోతే {
                        list+=3;
                    }
                } లేకపోతే ఒకవేళ(key2==(రెండవ_Unit&COMP_2_TRAIL_ముసుగు)) {
                    తిరిగి (static_cast<int32_t>(రెండవ_Unit & ~COMP_2_TRAIL_ముసుగు) << 16) | list[2];
                } లేకపోతే {
                    ఆపు;
                }
            } లేకపోతే {
                ఆపు;
            }
        }
    }
    తిరిగి -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
శూన్య Normalizer2Impl::కూడు_Composites(స్థిరరాశి uint16_t *list, Unicode_సమితి &సమితి) స్థిరరాశి {
    uint16_t మొదటి_Unit;
    int32_t సంయుక్త_And_Fwd;
    చేయి {
        మొదటి_Unit=*list;
        ఒకవేళ((మొదటి_Unit&COMP_1_TRIPLE)==0) {
            సంయుక్త_And_Fwd=list[1];
            list+=2;
        } లేకపోతే {
            సంయుక్త_And_Fwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_ముసుగు) << 16) | list[2];
            list+=3;
        }
        UChar32 సంయుక్త=సంయుక్త_And_Fwd>>1;
        ఒకవేళ((సంయుక్త_And_Fwd&1)!=0) {
            కూడు_Composites(get_Compositions_List_For_సంయుక్త(getRawNorm16(సంయుక్త)), సమితి);
        }
        సమితి.కూడు(సంయుక్త);
    } అయితే((మొదటి_Unit&COMP_1_చివరి_TUPLE)==0);
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
శూన్య Normalizer2Impl::recompose(Reordering_బఫరు &బఫరు, int32_t recompose_ప్రారంభం_సూచీ,
                                UBool onlyContiguous) స్థిరరాశి {
    char16_t *p=బఫరు.get_ప్రారంభం()+recompose_ప్రారంభం_సూచీ;
    char16_t *హద్దు=బఫరు.get_హద్దు();
    ఒకవేళ(p==హద్దు) {
        తిరిగి;
    }

    char16_t *starter, *pRemove, *q, *r;
    స్థిరరాశి uint16_t *compositionsList;
    UChar32 c, సంయుక్త_And_Fwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=శూన్యసూచిక;  // used as indicator for whether we have a forward-combining starter
    starter=శూన్యసూచిక;
    starterIsSupplementary=అబద్ధం;
    prevCC=0;

    కొరకు(;;) {
        UCPTRIE_FAST_U16_తదుపరి(normTrie, UCPTRIE_16, p, హద్దు, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        ఒకవేళ( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=శూన్యసూచిక &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            ఒకవేళ(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                ఒకవేళ(c<Hangul::JAMO_T_పునాది) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_పునాది);
                    ఒకవేళ(prev<Hangul::JAMO_L_లెక్క) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_పునాది +
                             (prev*Hangul::JAMO_V_లెక్క+(c-Hangul::JAMO_V_పునాది))*
                             Hangul::JAMO_T_లెక్క);
                        char16_t t;
                        ఒకవేళ (p != హద్దు && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_పునాది)) < Hangul::JAMO_T_లెక్క) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        అయితే(r<హద్దు) {
                            *q++=*r++;
                        }
                        హద్దు=q;
                        p=pRemove;
                    }
                }
                /*
                 * No "else" for Jamo T:
                 * Since the input is in NFD, there are no Hangul LV syllables that
                 * a Jamo T could combine with.
                 * All Jamo Ts are combined above when handling Jamo Vs.
                 */
                ఒకవేళ(p==హద్దు) {
                    ఆపు;
                }
                compositionsList=శూన్యసూచిక;
                కొనసాగు;
            } లేకపోతే ఒకవేళ((సంయుక్త_And_Fwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 సంయుక్త=సంయుక్త_And_Fwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_నిడివి(c);  // pRemove & p: start & limit of the combining mark
                ఒకవేళ(starterIsSupplementary) {
                    ఒకవేళ(U_IS_SUPPLEMENTARY(సంయుక్త)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(సంయుక్త);
                        starter[1]=U16_TRAIL(సంయుక్త);
                    } లేకపోతే {
                        *starter = static_cast<char16_t>(సంయుక్త);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=అబద్ధం;
                        q=starter+1;
                        r=q+1;
                        అయితే(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } లేకపోతే ఒకవేళ(U_IS_SUPPLEMENTARY(సంయుక్త)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=నిజం;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    అయితే(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(సంయుక్త);
                    *--starter=U16_LEAD(సంయుక్త);  // undo the temporary increment
                } లేకపోతే {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(సంయుక్త);
                }

                /* remove the combining mark by moving the following text over it */
                ఒకవేళ(pRemove<p) {
                    q=pRemove;
                    r=p;
                    అయితే(r<హద్దు) {
                        *q++=*r++;
                    }
                    హద్దు=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                ఒకవేళ(p==హద్దు) {
                    ఆపు;
                }
                // Is the composite a starter that combines forward?
                ఒకవేళ(సంయుక్త_And_Fwd&1) {
                    compositionsList=
                        get_Compositions_List_For_సంయుక్త(getRawNorm16(సంయుక్త));
                } లేకపోతే {
                    compositionsList=శూన్యసూచిక;
                }

                // We combined; continue with looking for compositions.
                కొనసాగు;
            }
        }

        // no combination this time
        prevCC=cc;
        ఒకవేళ(p==హద్దు) {
            ఆపు;
        }

        // If c did not combine, then check if it is a starter.
        ఒకవేళ(cc==0) {
            // Found a new starter.
            ఒకవేళ((compositionsList=getCompositionsListForDecompYes(norm16))!=శూన్యసూచిక) {
                // It may combine with something, prepare for it.
                ఒకవేళ(U_IS_BMP(c)) {
                    starterIsSupplementary=అబద్ధం;
                    starter=p-1;
                } లేకపోతే {
                    starterIsSupplementary=నిజం;
                    starter=p-2;
                }
            }
        } లేకపోతే ఒకవేళ(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=శూన్యసూచిక;
        }
    }
    బఫరు.సమితి_Reordering_హద్దు(హద్దు);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) స్థిరరాశి {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    స్థిరరాశి uint16_t *list;
    ఒకవేళ(isInert(norm16)) {
        తిరిగి U_SENTINEL;
    } లేకపోతే ఒకవేళ(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        ఒకవేళ(isJamoL(norm16)) {
            ఒకవేళ (b < Hangul::JAMO_V_పునాది) {
                తిరిగి U_SENTINEL;
            }
            b-=Hangul::JAMO_V_పునాది;
            ఒకవేళ(b<Hangul::JAMO_V_లెక్క) {
                తిరిగి
                    (Hangul::HANGUL_పునాది+
                     ((a-Hangul::JAMO_L_పునాది)*Hangul::JAMO_V_లెక్క+b)*
                     Hangul::JAMO_T_లెక్క);
            } లేకపోతే {
                తిరిగి U_SENTINEL;
            }
        } లేకపోతే ఒకవేళ(isHangulLV(norm16)) {
            ఒకవేళ (b <= Hangul::JAMO_T_పునాది) {
               తిరిగి U_SENTINEL;
            }
            b-=Hangul::JAMO_T_పునాది;
            ఒకవేళ(b<Hangul::JAMO_T_లెక్క) {  // not b==0!
                తిరిగి a+b;
            } లేకపోతే {
                తిరిగి U_SENTINEL;
            }
        } లేకపోతే {
            // 'a' has a compositions list in extraData
            list=get_దత్తాంశం_For_Yes_Or_No(norm16);
            ఒకవేళ(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_నిడివి_ముసుగు);  // + mapping length
            }
        }
    } లేకపోతే ఒకవేళ(norm16<minMaybeNoCombinesFwd || MIN_సాధారణ_MAYBE_YES<=norm16) {
        తిరిగి U_SENTINEL;
    } లేకపోతే {
        list=get_దత్తాంశం_For_Maybe(norm16);
        ఒకవేళ(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_నిడివి_ముసుగు);  // + mapping length
        }
    }
    ఒకవేళ(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        తిరిగి U_SENTINEL;
    }
#ఒకవేళ U_SIGNED_కుడి_జరుపు_IS_ARITHMETIC
    తిరిగి combine(list, b)>>1;
#లేకపోతే
    int32_t సంయుక్త_And_Fwd=combine(list, b);
    తిరిగి సంయుక్త_And_Fwd>=0 ? సంయుక్త_And_Fwd>>1 : U_SENTINEL;
#అయితే_ముగింపు
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_బఫరు &బఫరు,
                         UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    స్థిరరాశి char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ఒకవేళ(హద్దు==శూన్యసూచిక) {
        src=copy_దిగువ_ఉపసర్గ_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &బఫరు : శూన్యసూచిక,
                                           error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి అబద్ధం;
        }
        హద్దు=u_strchr(src, 0);
        ఒకవేళ (prevBoundary != src) {
            ఒకవేళ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } లేకపోతే {
                బఫరు.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    కొరకు (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        స్థిరరాశి char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        కొరకు (;;) {
            ఒకవేళ (src == హద్దు) {
                ఒకవేళ (prevBoundary != హద్దు && doCompose) {
                    బఫరు.appendZeroCC(prevBoundary, హద్దు, error_సంకేతం);
                }
                తిరిగి నిజం;
            }
            ఒకవేళ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } లేకపోతే {
                prevSrc = src++;
                ఒకవేళ(!U16_IS_LEAD(c)) {
                    ఆపు;
                } లేకపోతే {
                    char16_t c2;
                    ఒకవేళ(src!=హద్దు && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ఒకవేళ(!isCompYesAndZeroCC(norm16)) {
                            ఆపు;
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
        ఒకవేళ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ఒకవేళ (!doCompose) {
                తిరిగి అబద్ధం;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ఒకవేళ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ఒకవేళ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, హద్దు)) {
                    ఒకవేళ (prevBoundary != prevSrc && !బఫరు.appendZeroCC(prevBoundary, prevSrc, error_సంకేతం)) {
                        ఆపు;
                    }
                    ఒకవేళ(!బఫరు.append(mapAlgorithmic(c, norm16), 0, error_సంకేతం)) {
                        ఆపు;
                    }
                    prevBoundary = src;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ఒకవేళ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, హద్దు)) {
                    ఒకవేళ (prevBoundary != prevSrc && !బఫరు.appendZeroCC(prevBoundary, prevSrc, error_సంకేతం)) {
                        ఆపు;
                    }
                    స్థిరరాశి char16_t *mapping = reinterpret_cast<స్థిరరాశి char16_t *>(get_దత్తాంశం_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_నిడివి_ముసుగు;
                    ఒకవేళ(!బఫరు.appendZeroCC(mapping, mapping + length, error_సంకేతం)) {
                        ఆపు;
                    }
                    prevBoundary = src;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ (norm16 >= min_No_No_ఖాళీలు) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ఒకవేళ (hasCompBoundaryBefore(src, హద్దు) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ఒకవేళ (prevBoundary != prevSrc && !బఫరు.appendZeroCC(prevBoundary, prevSrc, error_సంకేతం)) {
                        ఆపు;
                    }
                    prevBoundary = src;
                    కొనసాగు;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } లేకపోతే ఒకవేళ (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            ఒకవేళ(c<Hangul::JAMO_T_పునాది) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_పునాది);
                ఒకవేళ(l<Hangul::JAMO_L_లెక్క) {
                    ఒకవేళ (!doCompose) {
                        తిరిగి అబద్ధం;
                    }
                    int32_t t;
                    ఒకవేళ (src != హద్దు &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_పునాది)) &&
                            t < Hangul::JAMO_T_లెక్క) {
                        // The next character is a Jamo T.
                        ++src;
                    } లేకపోతే ఒకవేళ (hasCompBoundaryBefore(src, హద్దు)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } లేకపోతే {
                        t = -1;
                    }
                    ఒకవేళ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_పునాది +
                            (l*Hangul::JAMO_V_లెక్క + (c-Hangul::JAMO_V_పునాది)) *
                            Hangul::JAMO_T_లెక్క + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        ఒకవేళ (prevBoundary != prevSrc && !బఫరు.appendZeroCC(prevBoundary, prevSrc, error_సంకేతం)) {
                            ఆపు;
                        }
                        ఒకవేళ (!బఫరు.appendBMP(static_cast<char16_t>(syllable), 0, error_సంకేతం)) {
                            ఆపు;
                        }
                        prevBoundary = src;
                        కొనసాగు;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } లేకపోతే ఒకవేళ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ఒకవేళ (!doCompose) {
                    తిరిగి అబద్ధం;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_పునాది;
                --prevSrc;  // Replace the Hangul LV as well.
                ఒకవేళ (prevBoundary != prevSrc && !బఫరు.appendZeroCC(prevBoundary, prevSrc, error_సంకేతం)) {
                    ఆపు;
                }
                ఒకవేళ (!బఫరు.appendBMP(static_cast<char16_t>(syllable), 0, error_సంకేతం)) {
                    ఆపు;
                }
                prevBoundary = src;
                కొనసాగు;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } లేకపోతే ఒకవేళ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_సాధారణ_Yes_Or_Maybe(norm16);  // cc!=0
            ఒకవేళ (onlyContiguous /* FCC */ && get_మునుపటి_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ఒకవేళ (!doCompose) {
                    తిరిగి అబద్ధం;
                }
            } లేకపోతే {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                స్థిరరాశి char16_t *తదుపరి_Src;
                uint16_t n16;
                కొరకు (;;) {
                    ఒకవేళ (src == హద్దు) {
                        ఒకవేళ (doCompose) {
                            బఫరు.appendZeroCC(prevBoundary, హద్దు, error_సంకేతం);
                        }
                        తిరిగి నిజం;
                    }
                    uint8_t prevCC = cc;
                    తదుపరి_Src = src;
                    UCPTRIE_FAST_U16_తదుపరి(normTrie, UCPTRIE_16, తదుపరి_Src, హద్దు, c, n16);
                    ఒకవేళ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_సాధారణ_Yes_Or_Maybe(n16);
                        ఒకవేళ (prevCC > cc) {
                            ఒకవేళ (!doCompose) {
                                తిరిగి అబద్ధం;
                            }
                            ఆపు;
                        }
                    } లేకపోతే {
                        ఆపు;
                    }
                    src = తదుపరి_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ఒకవేళ (norm16HasCompBoundaryBefore(n16)) {
                    ఒకవేళ (isCompYesAndZeroCC(n16)) {
                        src = తదుపరి_Src;
                    }
                    కొనసాగు;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ఒకవేళ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            స్థిరరాశి char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            ఒకవేళ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ఒకవేళ (doCompose && prevBoundary != prevSrc && !బఫరు.appendZeroCC(prevBoundary, prevSrc, error_సంకేతం)) {
            ఆపు;
        }
        int32_t recompose_ప్రారంభం_సూచీ=బఫరు.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, అబద్ధం /* !stopAtCompBoundary */, onlyContiguous,
                       బఫరు, error_సంకేతం);
        // Decompose until the next boundary.
        src = decomposeShort(src, హద్దు, నిజం /* stopAtCompBoundary */, onlyContiguous,
                             బఫరు, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            ఆపు;
        }
        ఒకవేళ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి నిజం;
        }
        recompose(బఫరు, recompose_ప్రారంభం_సూచీ, onlyContiguous);
        ఒకవేళ(!doCompose) {
            ఒకవేళ(!బఫరు.equals(prevSrc, src)) {
                తిరిగి అబద్ధం;
            }
            బఫరు.remove();
        }
        prevBoundary=src;
    }
    తిరిగి నిజం;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
స్థిరరాశి char16_t *
Normalizer2Impl::compose_శీఘ్ర_Check(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                                   UBool onlyContiguous,
                                   UNormalization_Check_ఫలితం *pQCResult) స్థిరరాశి {
    స్థిరరాశి char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ఒకవేళ(హద్దు==శూన్యసూచిక) {
        UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
        src=copy_దిగువ_ఉపసర్గ_From_Nul_Terminated(src, minNoMaybeCP, శూన్యసూచిక, error_సంకేతం);
        హద్దు=u_strchr(src, 0);
        ఒకవేళ (prevBoundary != src) {
            ఒకవేళ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } లేకపోతే {
                prevBoundary = --src;
            }
        }
    }

    కొరకు(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        స్థిరరాశి char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        కొరకు (;;) {
            ఒకవేళ(src==హద్దు) {
                తిరిగి src;
            }
            ఒకవేళ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } లేకపోతే {
                prevSrc = src++;
                ఒకవేళ(!U16_IS_LEAD(c)) {
                    ఆపు;
                } లేకపోతే {
                    char16_t c2;
                    ఒకవేళ(src!=హద్దు && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ఒకవేళ(!isCompYesAndZeroCC(norm16)) {
                            ఆపు;
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
        ఒకవేళ (prevBoundary != prevSrc) {
            ఒకవేళ (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } లేకపోతే {
                స్థిరరాశి char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                ఒకవేళ (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } లేకపోతే {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        ఒకవేళ (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            ఒకవేళ (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } లేకపోతే {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                స్థిరరాశి char16_t *తదుపరి_Src;
                కొరకు (;;) {
                    ఒకవేళ (norm16 < MIN_YES_YES_WITH_CC) {
                        ఒకవేళ (pQCResult != శూన్యసూచిక) {
                            *pQCResult = UNORM_MAYBE;
                        } లేకపోతే {
                            తిరిగి prevBoundary;
                        }
                    }
                    ఒకవేళ (src == హద్దు) {
                        తిరిగి src;
                    }
                    uint8_t prevCC = fcd16;
                    తదుపరి_Src = src;
                    UCPTRIE_FAST_U16_తదుపరి(normTrie, UCPTRIE_16, తదుపరి_Src, హద్దు, c, norm16);
                    ఒకవేళ (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        ఒకవేళ (!(prevCC <= cc || cc == 0)) {
                            ఆపు;
                        }
                    } లేకపోతే {
                        ఆపు;
                    }
                    src = తదుపరి_Src;
                }
                // src is after the last in-order combining mark.
                ఒకవేళ (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = తదుపరి_Src;
                    కొనసాగు;
                }
            }
        }
        ఒకవేళ(pQCResult!=శూన్యసూచిక) {
            *pQCResult=UNORM_NO;
        }
        తిరిగి prevBoundary;
    }
}

శూన్య Normalizer2Impl::composeAndAppend(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &సురక్షిత_మధ్య,
                                       Reordering_బఫరు &బఫరు,
                                       UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(!బఫరు.is_ఖాళీలు()) {
        స్థిరరాశి char16_t *మొదటి_Starter_లోపల_Src=కనుగొను_తదుపరి_Comp_Boundary(src, హద్దు, onlyContiguous);
        ఒకవేళ(src!=మొదటి_Starter_లోపల_Src) {
            స్థిరరాశి char16_t *చివరి_Starter_లోపల_Dest=కనుగొను_మునుపటి_Comp_Boundary(బఫరు.get_ప్రారంభం(),
                                                                    బఫరు.get_హద్దు(), onlyContiguous);
            int32_t dest_Suffix_నిడివి = static_cast<int32_t>(బఫరు.get_హద్దు() - చివరి_Starter_లోపల_Dest);
            UnicodeString మధ్య(చివరి_Starter_లోపల_Dest, dest_Suffix_నిడివి);
            బఫరు.removeSuffix(dest_Suffix_నిడివి);
            సురక్షిత_మధ్య=మధ్య;
            మధ్య.append(src, static_cast<int32_t>(మొదటి_Starter_లోపల_Src - src));
            స్థిరరాశి char16_t *మధ్య_ప్రారంభం=మధ్య.get_బఫరు();
            compose(మధ్య_ప్రారంభం, మధ్య_ప్రారంభం+మధ్య.length(), onlyContiguous,
                    నిజం, బఫరు, error_సంకేతం);
            ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                తిరిగి;
            }
            src=మొదటి_Starter_లోపల_Src;
        }
    }
    ఒకవేళ(doCompose) {
        compose(src, హద్దు, onlyContiguous, నిజం, బఫరు, error_సంకేతం);
    } లేకపోతే {
        ఒకవేళ(హద్దు==శూన్యసూచిక) {  // appendZeroCC() needs limit!=nullptr
            హద్దు=u_strchr(src, 0);
        }
        బఫరు.appendZeroCC(src, హద్దు, error_సంకేతం);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             స్థిరరాశి uint8_t *src, స్థిరరాశి uint8_t *హద్దు,
                             ByteSink *sink, Edits *edits, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    U_ASSERT(హద్దు != శూన్యసూచిక);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    స్థిరరాశి uint8_t *prevBoundary = src;

    కొరకు (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        స్థిరరాశి uint8_t *prevSrc;
        uint16_t norm16 = 0;
        కొరకు (;;) {
            ఒకవేళ (src == హద్దు) {
                ఒకవేళ (prevBoundary != హద్దు && sink != శూన్యసూచిక) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, హద్దు,
                                                  *sink, options, edits, error_సంకేతం);
                }
                తిరిగి నిజం;
            }
            ఒకవేళ (*src < minNoMaybeLead) {
                ++src;
            } లేకపోతే {
                prevSrc = src;
                UCPTRIE_FAST_U8_తదుపరి(normTrie, UCPTRIE_16, src, హద్దు, norm16);
                ఒకవేళ (!isCompYesAndZeroCC(norm16)) {
                    ఆపు;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        ఒకవేళ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ఒకవేళ (sink == శూన్యసూచిక) {
                తిరిగి అబద్ధం;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ఒకవేళ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ఒకవేళ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, హద్దు)) {
                    ఒకవేళ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_సంకేతం)) {
                        ఆపు;
                    }
                    append_సంకేతం_బిందువు_Delta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ఒకవేళ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, హద్దు)) {
                    ఒకవేళ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_సంకేతం)) {
                        ఆపు;
                    }
                    స్థిరరాశి uint16_t *mapping = get_దత్తాంశం_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_నిడివి_ముసుగు;
                    ఒకవేళ (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<స్థిరరాశి char16_t*>(mapping), length,
                                                    *sink, edits, error_సంకేతం)) {
                        ఆపు;
                    }
                    prevBoundary = src;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ (norm16 >= min_No_No_ఖాళీలు) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ఒకవేళ (hasCompBoundaryBefore(src, హద్దు) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ఒకవేళ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, error_సంకేతం)) {
                        ఆపు;
                    }
                    ఒకవేళ (edits != శూన్యసూచిక) {
                        edits->కూడు_Replace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    కొనసాగు;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } లేకపోతే ఒకవేళ (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = మునుపటి_Hangul_Or_Jamo(prevBoundary, prevSrc);
            ఒకవేళ (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_పునాది;
                ఒకవేళ (static_cast<uint32_t>(l) < Hangul::JAMO_L_లెక్క) {
                    ఒకవేళ (sink == శూన్యసూచిక) {
                        తిరిగి అబద్ధం;
                    }
                    int32_t t = get_Jamo_TMinus_పునాది(src, హద్దు);
                    ఒకవేళ (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } లేకపోతే ఒకవేళ (hasCompBoundaryBefore(src, హద్దు)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    ఒకవేళ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_పునాది +
                            (l*Hangul::JAMO_V_లెక్క + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_లెక్క + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        ఒకవేళ (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, error_సంకేతం)) {
                            ఆపు;
                        }
                        ByteSinkUtil::append_సంకేతం_బిందువు(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        కొనసాగు;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } లేకపోతే ఒకవేళ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ఒకవేళ (sink == శూన్యసూచిక) {
                    తిరిగి అబద్ధం;
                }
                UChar32 syllable = prev + get_Jamo_TMinus_పునాది(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                ఒకవేళ (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, error_సంకేతం)) {
                    ఆపు;
                }
                ByteSinkUtil::append_సంకేతం_బిందువు(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                కొనసాగు;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } లేకపోతే ఒకవేళ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_సాధారణ_Yes_Or_Maybe(norm16);  // cc!=0
            ఒకవేళ (onlyContiguous /* FCC */ && get_మునుపటి_Trail_CC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ఒకవేళ (sink == శూన్యసూచిక) {
                    తిరిగి అబద్ధం;
                }
            } లేకపోతే {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                స్థిరరాశి uint8_t *తదుపరి_Src;
                uint16_t n16;
                కొరకు (;;) {
                    ఒకవేళ (src == హద్దు) {
                        ఒకవేళ (sink != శూన్యసూచిక) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, హద్దు,
                                                          *sink, options, edits, error_సంకేతం);
                        }
                        తిరిగి నిజం;
                    }
                    uint8_t prevCC = cc;
                    తదుపరి_Src = src;
                    UCPTRIE_FAST_U8_తదుపరి(normTrie, UCPTRIE_16, తదుపరి_Src, హద్దు, n16);
                    ఒకవేళ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_సాధారణ_Yes_Or_Maybe(n16);
                        ఒకవేళ (prevCC > cc) {
                            ఒకవేళ (sink == శూన్యసూచిక) {
                                తిరిగి అబద్ధం;
                            }
                            ఆపు;
                        }
                    } లేకపోతే {
                        ఆపు;
                    }
                    src = తదుపరి_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ఒకవేళ (norm16HasCompBoundaryBefore(n16)) {
                    ఒకవేళ (isCompYesAndZeroCC(n16)) {
                        src = తదుపరి_Src;
                    }
                    కొనసాగు;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ఒకవేళ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            స్థిరరాశి uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            ఒకవేళ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        Reordering_బఫరు బఫరు(*ఇది, s16, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            ఆపు;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_హద్దు, onlyContiguous,
                       బఫరు, error_సంకేతం);
        // Decompose until the next boundary.
        src = decomposeShort(src, హద్దు, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             బఫరు, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            ఆపు;
        }
        ఒకవేళ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి నిజం;
        }
        recompose(బఫరు, 0, onlyContiguous);
        ఒకవేళ (!బఫరు.equals(prevSrc, src)) {
            ఒకవేళ (sink == శూన్యసూచిక) {
                తిరిగి అబద్ధం;
            }
            ఒకవేళ (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, error_సంకేతం)) {
                ఆపు;
            }
            ఒకవేళ (!ByteSinkUtil::appendChange(prevSrc, src, బఫరు.get_ప్రారంభం(), బఫరు.length(),
                                            *sink, edits, error_సంకేతం)) {
                ఆపు;
            }
            prevBoundary = src;
        }
    }
    తిరిగి నిజం;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు) స్థిరరాశి {
    ఒకవేళ (src == హద్దు || *src < minCompNoMaybeCP) {
        తిరిగి నిజం;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_తదుపరి(normTrie, UCPTRIE_16, src, హద్దు, c, norm16);
    తిరిగి norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(స్థిరరాశి uint8_t *src, స్థిరరాశి uint8_t *హద్దు) స్థిరరాశి {
    ఒకవేళ (src == హద్దు) {
        తిరిగి నిజం;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_తదుపరి(normTrie, UCPTRIE_16, src, హద్దు, norm16);
    తిరిగి norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(స్థిరరాశి char16_t *ప్రారంభం, స్థిరరాశి char16_t *p,
                                            UBool onlyContiguous) స్థిరరాశి {
    ఒకవేళ (ప్రారంభం == p) {
        తిరిగి నిజం;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ప్రారంభం, p, c, norm16);
    తిరిగి norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(స్థిరరాశి uint8_t *ప్రారంభం, స్థిరరాశి uint8_t *p,
                                            UBool onlyContiguous) స్థిరరాశి {
    ఒకవేళ (ప్రారంభం == p) {
        తిరిగి నిజం;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, ప్రారంభం, p, norm16);
    తిరిగి norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

స్థిరరాశి char16_t *Normalizer2Impl::కనుగొను_మునుపటి_Comp_Boundary(స్థిరరాశి char16_t *ప్రారంభం, స్థిరరాశి char16_t *p,
                                                       UBool onlyContiguous) స్థిరరాశి {
    అయితే (p != ప్రారంభం) {
        స్థిరరాశి char16_t *సంకేతం_బిందువు_హద్దు = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ప్రారంభం, p, c, norm16);
        ఒకవేళ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            తిరిగి సంకేతం_బిందువు_హద్దు;
        }
        ఒకవేళ (hasCompBoundaryBefore(c, norm16)) {
            తిరిగి p;
        }
    }
    తిరిగి p;
}

స్థిరరాశి char16_t *Normalizer2Impl::కనుగొను_తదుపరి_Comp_Boundary(స్థిరరాశి char16_t *p, స్థిరరాశి char16_t *హద్దు,
                                                   UBool onlyContiguous) స్థిరరాశి {
    అయితే (p != హద్దు) {
        స్థిరరాశి char16_t *సంకేతం_బిందువు_ప్రారంభం = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_తదుపరి(normTrie, UCPTRIE_16, p, హద్దు, c, norm16);
        ఒకవేళ (hasCompBoundaryBefore(c, norm16)) {
            తిరిగి సంకేతం_బిందువు_ప్రారంభం;
        }
        ఒకవేళ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            తిరిగి p;
        }
    }
    తిరిగి p;
}

uint8_t Normalizer2Impl::get_మునుపటి_Trail_CC(స్థిరరాశి char16_t *ప్రారంభం, స్థిరరాశి char16_t *p) స్థిరరాశి {
    ఒకవేళ (ప్రారంభం == p) {
        తిరిగి 0;
    }
    int32_t i = static_cast<int32_t>(p - ప్రారంభం);
    UChar32 c;
    U16_PREV(ప్రారంభం, 0, i, c);
    తిరిగి static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::get_మునుపటి_Trail_CC(స్థిరరాశి uint8_t *ప్రారంభం, స్థిరరాశి uint8_t *p) స్థిరరాశి {
    ఒకవేళ (ప్రారంభం == p) {
        తిరిగి 0;
    }
    int32_t i = static_cast<int32_t>(p - ప్రారంభం);
    UChar32 c;
    U8_PREV(ప్రారంభం, 0, i, c);
    తిరిగి static_cast<uint8_t>(getFCD16(c));
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
#ఒకవేళ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#సూచన optimize( "", off )
#అయితే_ముగింపు
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_దత్తాంశం(UChar32 c) స్థిరరాశి {
    uint16_t norm16=getNorm16(c);
    ఒకవేళ (norm16 >= హద్దు_No_No) {
        ఒకవేళ(norm16>=MIN_సాధారణ_MAYBE_YES) {
            // combining mark
            norm16=get_CCFrom_సాధారణ_Yes_Or_Maybe(norm16);
            తిరిగి norm16|(norm16<<8);
        } లేకపోతే ఒకవేళ(norm16>=minMaybeYes) {
            తిరిగి 0;
        } లేకపోతే ఒకవేళ(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_ముసుగు;
            ఒకవేళ (deltaTrailCC <= DELTA_TCCC_1) {
                తిరిగి deltaTrailCC >> స్థానభ్రంశం_జరుపు;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ఒకవేళ(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        తిరిగి 0;
    }
    // c decomposes, get everything from the variable-length extra data
    స్థిరరాశి uint16_t *mapping=get_దత్తాంశం(norm16);
    uint16_t మొదటి_Unit=*mapping;
    norm16=మొదటి_Unit>>8;  // tccc
    ఒకవేళ(మొదటి_Unit&MAPPING_HAS_CCC_LCCC_మాట) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    తిరిగి norm16;
}
#ఒకవేళ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#సూచన optimize( "", on )
#అయితే_ముగింపు

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) స్థిరరాశి {
    U_ASSERT(norm16 >= minMaybeNo);
    ఒకవేళ (norm16 >= MIN_సాధారణ_MAYBE_YES) {
        // combining mark
        norm16 = get_CCFrom_సాధారణ_Yes_Or_Maybe(norm16);
        తిరిగి norm16 | (norm16<<8);
    } లేకపోతే ఒకవేళ (norm16 >= minMaybeYes) {
        తిరిగి 0;
    }
    // c decomposes, get everything from the variable-length extra data
    స్థిరరాశి uint16_t *mapping = get_దత్తాంశం_For_Maybe(norm16);
    uint16_t మొదటి_Unit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((మొదటి_Unit & MAPPING_HAS_CCC_LCCC_మాట) == 0 || (*(mapping - 1) & 0xff00) == 0);
    తిరిగి మొదటి_Unit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
స్థిరరాశి char16_t *
Normalizer2Impl::makeFCD(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                         Reordering_బఫరు *బఫరు,
                         UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    స్థిరరాశి char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    ఒకవేళ(హద్దు==శూన్యసూచిక) {
        src=copy_దిగువ_ఉపసర్గ_From_Nul_Terminated(src, minLcccCP, బఫరు, error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి src;
        }
        ఒకవేళ(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            ఒకవేళ(prevFCD16>1) {
                --prevBoundary;
            }
        }
        హద్దు=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    స్థిరరాశి char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    కొరకు(;;) {
        // count code units with lccc==0
        కొరకు(prevSrc=src; src!=హద్దు;) {
            ఒకవేళ((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } లేకపోతే ఒకవేళ(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } లేకపోతే {
                ఒకవేళ(U16_IS_LEAD(c)) {
                    char16_t c2;
                    ఒకవేళ((src+1)!=హద్దు && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                ఒకవేళ((fcd16=get_FCD16_From_Norm_దత్తాంశం(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_నిడివి(c);
                } లేకపోతే {
                    ఆపు;
                }
            }
        }
        // copy these code units all at once
        ఒకవేళ(src!=prevSrc) {
            ఒకవేళ(బఫరు!=శూన్యసూచిక && !బఫరు->appendZeroCC(prevSrc, src, error_సంకేతం)) {
                ఆపు;
            }
            ఒకవేళ(src==హద్దు) {
                ఆపు;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            ఒకవేళ(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                ఒకవేళ(prev<minDecompNoCP) {
                    prevFCD16=0;
                } లేకపోతే {
                    prevFCD16=get_FCD16_From_Norm_దత్తాంశం(prev);
                    ఒకవేళ(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } లేకపోతే {
                స్థిరరాశి char16_t *p=src-1;
                ఒకవేళ(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=get_FCD16_From_Norm_దత్తాంశం(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                ఒకవేళ(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } లేకపోతే ఒకవేళ(src==హద్దు) {
            ఆపు;
        }

        src+=U16_నిడివి(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        ఒకవేళ((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            ఒకవేళ((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            ఒకవేళ(బఫరు!=శూన్యసూచిక && !బఫరు->appendZeroCC(c, error_సంకేతం)) {
                ఆపు;
            }
            prevFCD16=fcd16;
            కొనసాగు;
        } లేకపోతే ఒకవేళ(బఫరు==శూన్యసూచిక) {
            తిరిగి prevBoundary;  // quick check "no"
        } లేకపోతే {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            బఫరు->removeSuffix(static_cast<int32_t>(prevSrc - prevBoundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=కనుగొను_తదుపరి_FCDBoundary(src, హద్దు);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(prevBoundary, src, అబద్ధం, అబద్ధం, *బఫరు, error_సంకేతం);
            ఒకవేళ (U_FAILURE(error_సంకేతం)) {
                ఆపు;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    తిరిగి src;
}

శూన్య Normalizer2Impl::makeFCDAndAppend(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
                                       UBool doMakeFCD,
                                       UnicodeString &సురక్షిత_మధ్య,
                                       Reordering_బఫరు &బఫరు,
                                       UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(!బఫరు.is_ఖాళీలు()) {
        స్థిరరాశి char16_t *మొదటి_Boundary_లోపల_Src=కనుగొను_తదుపరి_FCDBoundary(src, హద్దు);
        ఒకవేళ(src!=మొదటి_Boundary_లోపల_Src) {
            స్థిరరాశి char16_t *చివరి_Boundary_లోపల_Dest=కనుగొను_మునుపటి_FCDBoundary(బఫరు.get_ప్రారంభం(),
                                                                    బఫరు.get_హద్దు());
            int32_t dest_Suffix_నిడివి = static_cast<int32_t>(బఫరు.get_హద్దు() - చివరి_Boundary_లోపల_Dest);
            UnicodeString మధ్య(చివరి_Boundary_లోపల_Dest, dest_Suffix_నిడివి);
            బఫరు.removeSuffix(dest_Suffix_నిడివి);
            సురక్షిత_మధ్య=మధ్య;
            మధ్య.append(src, static_cast<int32_t>(మొదటి_Boundary_లోపల_Src - src));
            స్థిరరాశి char16_t *మధ్య_ప్రారంభం=మధ్య.get_బఫరు();
            makeFCD(మధ్య_ప్రారంభం, మధ్య_ప్రారంభం+మధ్య.length(), &బఫరు, error_సంకేతం);
            ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                తిరిగి;
            }
            src=మొదటి_Boundary_లోపల_Src;
        }
    }
    ఒకవేళ(doMakeFCD) {
        makeFCD(src, హద్దు, &బఫరు, error_సంకేతం);
    } లేకపోతే {
        ఒకవేళ(హద్దు==శూన్యసూచిక) {  // appendZeroCC() needs limit!=nullptr
            హద్దు=u_strchr(src, 0);
        }
        బఫరు.appendZeroCC(src, హద్దు, error_సంకేతం);
    }
}

స్థిరరాశి char16_t *Normalizer2Impl::కనుగొను_మునుపటి_FCDBoundary(స్థిరరాశి char16_t *ప్రారంభం, స్థిరరాశి char16_t *p) స్థిరరాశి {
    అయితే(ప్రారంభం<p) {
        స్థిరరాశి char16_t *సంకేతం_బిందువు_హద్దు = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, ప్రారంభం, p, c, norm16);
        ఒకవేళ (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            తిరిగి సంకేతం_బిందువు_హద్దు;
        }
        ఒకవేళ (norm16HasDecompBoundaryBefore(norm16)) {
            తిరిగి p;
        }
    }
    తిరిగి p;
}

స్థిరరాశి char16_t *Normalizer2Impl::కనుగొను_తదుపరి_FCDBoundary(స్థిరరాశి char16_t *p, స్థిరరాశి char16_t *హద్దు) స్థిరరాశి {
    అయితే(p<హద్దు) {
        స్థిరరాశి char16_t *సంకేతం_బిందువు_ప్రారంభం=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_తదుపరి(normTrie, UCPTRIE_16, p, హద్దు, c, norm16);
        ఒకవేళ (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            తిరిగి సంకేతం_బిందువు_ప్రారంభం;
        }
        ఒకవేళ (norm16HasDecompBoundaryAfter(norm16)) {
            తిరిగి p;
        }
    }
    తిరిగి p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_దత్తాంశం::Canon_Iter_దత్తాంశం(UError_సంకేతం &error_సంకేతం) :
        mutableTrie(umutablecptrie_open(0, 0, &error_సంకేతం)), trie(శూన్యసూచిక),
        canon_ప్రారంభం_Sets(uprv_deleteUObject, శూన్యసూచిక, error_సంకేతం) {}

Canon_Iter_దత్తాంశం::~Canon_Iter_దత్తాంశం() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

శూన్య Canon_Iter_దత్తాంశం::కూడు_గమ్యం_ప్రారంభం_సమితి(UChar32 origin, UChar32 decompLead, UError_సంకేతం &error_సంకేతం) {
    uint32_t canon_విలువ = umutablecptrie_get(mutableTrie, decompLead);
    ఒకవేళ((canon_విలువ&(CANON_HAS_సమితి|CANON_విలువ_ముసుగు))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_సమితి(mutableTrie, decompLead, canon_విలువ|origin, &error_సంకేతం);
    } లేకపోతే {
        // origin is not the first character, or it is U+0000.
        Unicode_సమితి *సమితి;
        ఒకవేళ((canon_విలువ&CANON_HAS_సమితి)==0) {
            Local_సూచిక<Unicode_సమితి> lp_సమితి(కొత్త Unicode_సమితి, error_సంకేతం);
            సమితి=lp_సమితి.getAlias();
            ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                తిరిగి;
            }
            UChar32 మొదటి_Origin = static_cast<UChar32>(canon_విలువ & CANON_విలువ_ముసుగు);
            canon_విలువ = (canon_విలువ & ~CANON_విలువ_ముసుగు) | CANON_HAS_సమితి | static_cast<uint32_t>(canon_ప్రారంభం_Sets.size());
            umutablecptrie_సమితి(mutableTrie, decompLead, canon_విలువ, &error_సంకేతం);
            canon_ప్రారంభం_Sets.adoptElement(lp_సమితి.orphan(), error_సంకేతం);
            ఒకవేళ (U_FAILURE(error_సంకేతం)) {
                తిరిగి;
            }
            ఒకవేళ(మొదటి_Origin!=0) {
                సమితి->కూడు(మొదటి_Origin);
            }
        } లేకపోతే {
            సమితి = static_cast<Unicode_సమితి*>(canon_ప్రారంభం_Sets[static_cast<int32_t>(canon_విలువ & CANON_విలువ_ముసుగు)]);
        }
        సమితి->కూడు(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
తరగతి Init_Canon_Iter_దత్తాంశం {
బహిరంగ:
    స్థిర శూన్య doInit(Normalizer2Impl *impl, UError_సంకేతం &error_సంకేతం);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
స్థిర శూన్య U_CALLCONV
init_Canon_Iter_దత్తాంశం(Normalizer2Impl *impl, UError_సంకేతం &error_సంకేతం) {
    Init_Canon_Iter_దత్తాంశం::doInit(impl, error_సంకేతం);
}

U_CDECL_చివర

శూన్య Init_Canon_Iter_దత్తాంశం::doInit(Normalizer2Impl *impl, UError_సంకేతం &error_సంకేతం) {
    U_ASSERT(impl->f_Canon_Iter_దత్తాంశం == శూన్యసూచిక);
    impl->f_Canon_Iter_దత్తాంశం = కొత్త Canon_Iter_దత్తాంశం(error_సంకేతం);
    ఒకవేళ (impl->f_Canon_Iter_దత్తాంశం == శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        UChar32 ప్రారంభం = 0, చివర;
        uint32_t value;
        అయితే ((చివర = ucptrie_getRange(impl->normTrie, ప్రారంభం,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       శూన్యసూచిక, శూన్యసూచిక, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            ఒకవేళ (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_దత్తాంశం_From_Norm16(ప్రారంభం, చివర, value, *impl->f_Canon_Iter_దత్తాంశం, error_సంకేతం);
            }
            ప్రారంభం = చివర + 1;
        }
#నిర్వచితమైతే UCPTRIE_DEBUG
        umutablecptrie_సమితి_Name(impl->f_Canon_Iter_దత్తాంశం->mutableTrie, "CanonIterData");
#అయితే_ముగింపు
        impl->f_Canon_Iter_దత్తాంశం->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_దత్తాంశం->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_విలువ_బిట్లు_32, &error_సంకేతం);
        umutablecptrie_close(impl->f_Canon_Iter_దత్తాంశం->mutableTrie);
        impl->f_Canon_Iter_దత్తాంశం->mutableTrie = శూన్యసూచిక;
    }
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తొలగించు impl->f_Canon_Iter_దత్తాంశం;
        impl->f_Canon_Iter_దత్తాంశం = శూన్యసూచిక;
    }
}

శూన్య Normalizer2Impl::make_Canon_Iter_దత్తాంశం_From_Norm16(UChar32 ప్రారంభం, UChar32 చివర, స్థిరరాశి uint16_t norm16,
                                                  Canon_Iter_దత్తాంశం &new_దత్తాంశం,
                                                  UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        తిరిగి;
    }
    కొరకు(UChar32 c=ప్రారంభం; c<=చివర; ++c) {
        uint32_t old_విలువ = umutablecptrie_get(new_దత్తాంశం.mutableTrie, c);
        uint32_t new_విలువ=old_విలువ;
        ఒకవేళ(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_విలువ|=CANON_NOT_SEGMENT_STARTER;
            ఒకవేళ(norm16<MIN_సాధారణ_MAYBE_YES) {
                new_విలువ|=CANON_HAS_COMPOSITIONS;
            }
        } లేకపోతే ఒకవేళ(norm16<minYesNo) {
            new_విలువ|=CANON_HAS_COMPOSITIONS;
        } లేకపోతే {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            ఒకవేళ (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            ఒకవేళ (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                స్థిరరాశి uint16_t *mapping=get_దత్తాంశం_For_Yes_Or_No(norm16_2);
                uint16_t మొదటి_Unit=*mapping;
                int32_t length=మొదటి_Unit&MAPPING_నిడివి_ముసుగు;
                ఒకవేళ((మొదటి_Unit&MAPPING_HAS_CCC_LCCC_మాట)!=0) {
                    ఒకవేళ(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_విలువ|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                ఒకవేళ(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_తదుపరి_UNSAFE(mapping, i, c2);
                    new_దత్తాంశం.కూడు_గమ్యం_ప్రారంభం_సమితి(c, c2, error_సంకేతం);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    ఒకవేళ(norm16_2>=minNoNo) {
                        అయితే(i<length) {
                            U16_తదుపరి_UNSAFE(mapping, i, c2);
                            uint32_t c2_విలువ = umutablecptrie_get(new_దత్తాంశం.mutableTrie, c2);
                            ఒకవేళ((c2_విలువ&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_సమితి(new_దత్తాంశం.mutableTrie, c2,
                                                   c2_విలువ|CANON_NOT_SEGMENT_STARTER, &error_సంకేతం);
                            }
                        }
                    }
                }
            } లేకపోతే {
                // c decomposed to c2 algorithmically; c has cc==0
                new_దత్తాంశం.కూడు_గమ్యం_ప్రారంభం_సమితి(c, c2, error_సంకేతం);
            }
        }
        ఒకవేళ(new_విలువ!=old_విలువ) {
            umutablecptrie_సమితి(new_దత్తాంశం.mutableTrie, c, new_విలువ, &error_సంకేతం);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_దత్తాంశం(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(ఇది);
    umtx_initOnce(me->f_Canon_Iter_దత్తాంశం_Init_Once, &init_Canon_Iter_దత్తాంశం, me, error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం);
}

int32_t Normalizer2Impl::get_Canon_విలువ(UChar32 c) స్థిరరాశి {
    తిరిగి static_cast<int32_t>(ucptrie_get(f_Canon_Iter_దత్తాంశం->trie, c));
}

స్థిరరాశి Unicode_సమితి &Normalizer2Impl::get_Canon_ప్రారంభం_సమితి(int32_t n) స్థిరరాశి {
    తిరిగి *static_cast<స్థిరరాశి Unicode_సమితి*>(f_Canon_Iter_దత్తాంశం->canon_ప్రారంభం_Sets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) స్థిరరాశి {
    తిరిగి get_Canon_విలువ(c)>=0;
}

UBool Normalizer2Impl::get_Canon_ప్రారంభం_సమితి(UChar32 c, Unicode_సమితి &సమితి) స్థిరరాశి {
    int32_t canon_విలువ=get_Canon_విలువ(c)&~CANON_NOT_SEGMENT_STARTER;
    ఒకవేళ(canon_విలువ==0) {
        తిరిగి అబద్ధం;
    }
    సమితి.clear();
    int32_t value=canon_విలువ&CANON_విలువ_ముసుగు;
    ఒకవేళ((canon_విలువ&CANON_HAS_సమితి)!=0) {
        సమితి.కూడు_All(get_Canon_ప్రారంభం_సమితి(value));
    } లేకపోతే ఒకవేళ(value!=0) {
        సమితి.కూడు(value);
    }
    ఒకవేళ((canon_విలువ&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        ఒకవేళ(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_పునాది + (c - Hangul::JAMO_L_పునాది) * Hangul::JAMO_VT_లెక్క);
            సమితి.కూడు(syllable, syllable+Hangul::JAMO_VT_లెక్క-1);
        } లేకపోతే {
            కూడు_Composites(getCompositionsList(norm16), సమితి);
        }
    }
    తిరిగి నిజం;
}

U_NAMESPACE_చివర

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
            స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
            UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UDataInfo *pInfo;
    int32_t శీర్షిక_కొలత;

    స్థిరరాశి uint8_t *లోపల_బైట్లు;
    uint8_t *బహిర్గతం_బైట్లు;

    స్థిరరాశి int32_t *లోపల_Indexes;
    int32_t indexes[Normalizer2Impl::IX_మొత్తం_కొలత+1];

    int32_t i, స్థానభ్రంశం, తదుపరి_స్థానభ్రంశం, size;

    /* udata_swapDataHeader checks the arguments */
    శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* check data format and format version */
    pInfo=(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    uint8_t formatVersion0=pInfo->format_సంస్కరణ[0];
    ఒకవేళ(!(
        pInfo->దత్తాంశం_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->దత్తాంశం_Format[1]==0x72 &&
        pInfo->దత్తాంశం_Format[2]==0x6d &&
        pInfo->దత్తాంశం_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                         pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3],
                         pInfo->format_సంస్కరణ[0]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    లోపల_బైట్లు=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం+శీర్షిక_కొలత;
    బహిర్గతం_బైట్లు=(బహిర్గతం_దత్తాంశం == శూన్యసూచిక) ? శూన్యసూచిక : (uint8_t *)బహిర్గతం_దత్తాంశం+శీర్షిక_కొలత;

    లోపల_Indexes=(స్థిరరాశి int32_t *)లోపల_బైట్లు;
    int32_t min_Indexes_నిడివి;
    ఒకవేళ(formatVersion0==1) {
        min_Indexes_నిడివి=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } లేకపోతే ఒకవేళ(formatVersion0==2) {
        min_Indexes_నిడివి=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } లేకపోతే ఒకవేళ(formatVersion0<=4) {
        min_Indexes_నిడివి=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } లేకపోతే {
        min_Indexes_నిడివి=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    ఒకవేళ(length>=0) {
        length-=శీర్షిక_కొలత;
        ఒకవేళ(length<min_Indexes_నిడివి*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
    }

    /* read the first few indexes */
    కొరకు(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, లోపల_Indexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_మొత్తం_కొలత];

    ఒకవేళ(length>=0) {
        ఒకవేళ(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }

        /* copy the data for inaccessible bytes */
        ఒకవేళ(లోపల_బైట్లు!=బహిర్గతం_బైట్లు) {
            uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, size);
        }

        స్థానభ్రంశం=0;

        /* swap the int32_t indexes[] */
        తదుపరి_స్థానభ్రంశం=indexes[Normalizer2Impl::IX_NORM_TRIE_స్థానభ్రంశం];
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు, తదుపరి_స్థానభ్రంశం-స్థానభ్రంశం, బహిర్గతం_బైట్లు, p_Error_సంకేతం);
        స్థానభ్రంశం=తదుపరి_స్థానభ్రంశం;

        /* swap the trie */
        తదుపరి_స్థానభ్రంశం=indexes[Normalizer2Impl::IX_EXTRA_దత్తాంశం_స్థానభ్రంశం];
        utrie_మార్పిడి_Any_సంస్కరణ(ds, లోపల_బైట్లు+స్థానభ్రంశం, తదుపరి_స్థానభ్రంశం-స్థానభ్రంశం, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
        స్థానభ్రంశం=తదుపరి_స్థానభ్రంశం;

        /* swap the uint16_t extraData[] */
        తదుపరి_స్థానభ్రంశం=indexes[Normalizer2Impl::IX_SMALL_FCD_స్థానభ్రంశం];
        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, తదుపరి_స్థానభ్రంశం-స్థానభ్రంశం, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
        స్థానభ్రంశం=తదుపరి_స్థానభ్రంశం;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        తదుపరి_స్థానభ్రంశం=indexes[Normalizer2Impl::IX_SMALL_FCD_స్థానభ్రంశం+1];
        స్థానభ్రంశం=తదుపరి_స్థానభ్రంశం;

        U_ASSERT(స్థానభ్రంశం==size);
    }

    తిరిగి శీర్షిక_కొలత+size;
}

#అయితే_ముగింపు  // !UCONFIG_NO_NORMALIZATION
