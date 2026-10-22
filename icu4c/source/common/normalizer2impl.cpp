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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/normalizer2.h"
#অন্তর্ভাব "unicode/stringoptions.h"
#অন্তর্ভাব "unicode/ucptrie.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/umutablecptrie.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "bytesinkutil.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "normalizer2impl.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucptrie_impl.h"
#অন্তর্ভাব "uset_imp.h"
#অন্তর্ভাব "uvector.h"

U_NAMESPACE_BEGIN

নামাকাশ {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
অন্তঃরেখ uint8_t leadByteForCP(UChar32 c) {
    যদি (c <= 0x7f) {
        ফেরত static_cast<uint8_t>(c);
    } অন্যথা যদি (c <= 0x7ff) {
        ফেরত static_cast<uint8_t>(0xc0 + (c >> 6));
    } অন্যথা {
        // Should not occur because ccc(U+0300)!=0.
        ফেরত 0xe0;
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
UChar32 codePointFromValidUTF8(ধ্রুবক uint8_t *cpStart, ধ্রুবক uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    নির্বাচন(cpLimit-cpStart) {
    ক্ষেত্র 1:
        ফেরত c;
    ক্ষেত্র 2:
        ফেরত ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    ক্ষেত্র 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        ফেরত static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    ক্ষেত্র 4:
        ফেরত ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    পূর্বনির্ধারিত:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(ধ্রুবক uint8_t *start, ধ্রুবক uint8_t *p) {
    যদি ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        যদি (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            ফেরত ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    ফেরত U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(ধ্রুবক uint8_t *src, ধ্রুবক uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    যদি ((limit - src) >= 3 && *src == 0xe1) {
        যদি (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            যদি (0xa8 <= t && t <= 0xbf) {
                ফেরত t - 0xa7;
            }
        } অন্যথা যদি (src[1] == 0x87) {
            uint8_t t = src[2];
            যদি (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                ফেরত t - (0xa7 - 0x40);
            }
        }
    }
    ফেরত -1;
}

শূন্য
appendCodePointDelta(ধ্রুবক uint8_t *cpStart, ধ্রুবক uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    অক্ষর বাফার[U8_MAX_দৈর্ঘ্য];
    int32_t length;
    int32_t cp_দৈর্ঘ্য = static_cast<int32_t>(cpLimit - cpStart);
    যদি (cp_দৈর্ঘ্য == 1) {
        // The builder makes ASCII map to ASCII.
        বাফার[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } অন্যথা {
        int32_t trail = *(cpLimit-1) + delta;
        যদি (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            করো { বাফার[length++] = *cpStart++; } যখন (cpStart < cpLimit);
            বাফার[length++] = static_cast<uint8_t>(trail);
        } অন্যথা {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(বাফার, length, c);
        }
    }
    যদি (edits != nullptr) {
        edits->addReplace(cp_দৈর্ঘ্য, length);
    }
    sink.Append(বাফার, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_বাফার::Reordering_বাফার(ধ্রুবক Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.get_বাফার(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    যদি (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool Reordering_বাফার::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.get_বাফার(destCapacity);
    যদি(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত মিথ্যা;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    যদি(start==limit) {
        lastCC=0;
    } অন্যথা {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        যদি(lastCC>1) {
            যখন(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    ফেরত সত্য;
}

UBool Reordering_বাফার::equals(ধ্রুবক char16_t *otherStart, ধ্রুবক char16_t *otherLimit) ধ্রুবক {
    int32_t length = static_cast<int32_t>(limit - start);
    ফেরত
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool Reordering_বাফার::equals(ধ্রুবক uint8_t *otherStart, ধ্রুবক uint8_t *otherLimit) ধ্রুবক {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t other_দৈর্ঘ্য = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    যদি (other_দৈর্ঘ্য < length || (other_দৈর্ঘ্য / 3) > length) {
        ফেরত মিথ্যা;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    জন্য (int32_t i = 0, j = 0;;) {
        যদি (i >= length) {
            ফেরত j >= other_দৈর্ঘ্য;
        } অন্যথা যদি (j >= other_দৈর্ঘ্য) {
            ফেরত মিথ্যা;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_পরবর্তী_UNSAFE(start, i, c);
        U8_পরবর্তী_UNSAFE(otherStart, j, other);
        যদি (c != other) {
            ফেরত মিথ্যা;
        }
    }
}

UBool Reordering_বাফার::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    যদি(remainingCapacity<2 && !resize(2, errorCode)) {
        ফেরত মিথ্যা;
    }
    যদি(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        যদি(cc<=1) {
            reorderStart=limit;
        }
    } অন্যথা {
        insert(c, cc);
    }
    remainingCapacity-=2;
    ফেরত সত্য;
}

UBool Reordering_বাফার::append(ধ্রুবক char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    যদি(length==0) {
        ফেরত সত্য;
    }
    যদি(remainingCapacity<length && !resize(length, errorCode)) {
        ফেরত মিথ্যা;
    }
    remainingCapacity-=length;
    যদি(lastCC<=leadCC || leadCC==0) {
        যদি(trailCC<=1) {
            reorderStart=limit+length;
        } অন্যথা যদি(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        ধ্রুবক char16_t *sLimit=s+length;
        করো { *limit++=*s++; } যখন(s!=sLimit);
        lastCC=trailCC;
    } অন্যথা {
        int32_t i=0;
        UChar32 c;
        U16_পরবর্তী(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        যখন(i<length) {
            U16_পরবর্তী(s, i, length, c);
            যদি(i<length) {
                যদি (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } অন্যথা {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } অন্যথা {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    ফেরত সত্য;
}

UBool Reordering_বাফার::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cp_দৈর্ঘ্য=U16_দৈর্ঘ্য(c);
    যদি(remainingCapacity<cp_দৈর্ঘ্য && !resize(cp_দৈর্ঘ্য, errorCode)) {
        ফেরত মিথ্যা;
    }
    remainingCapacity-=cp_দৈর্ঘ্য;
    যদি(cp_দৈর্ঘ্য==1) {
        *limit++ = static_cast<char16_t>(c);
    } অন্যথা {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    ফেরত সত্য;
}

UBool Reordering_বাফার::appendZeroCC(ধ্রুবক char16_t *s, ধ্রুবক char16_t *sLimit, UErrorCode &errorCode) {
    যদি(s==sLimit) {
        ফেরত সত্য;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    যদি(remainingCapacity<length && !resize(length, errorCode)) {
        ফেরত মিথ্যা;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    ফেরত সত্য;
}

শূন্য Reordering_বাফার::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

শূন্য Reordering_বাফার::removeSuffix(int32_t suffix_দৈর্ঘ্য) {
    যদি(suffix_দৈর্ঘ্য<(limit-start)) {
        limit-=suffix_দৈর্ঘ্য;
        remainingCapacity+=suffix_দৈর্ঘ্য;
    } অন্যথা {
        limit=start;
        remainingCapacity=str.getCapacity();
    }
    lastCC=0;
    reorderStart=limit;
}

UBool Reordering_বাফার::resize(int32_t append_দৈর্ঘ্য, UErrorCode &errorCode) {
    int32_t reorder_Start_সূচক = static_cast<int32_t>(reorderStart - start);
    int32_t length = static_cast<int32_t>(limit - start);
    str.release_বাফার(length);
    int32_t newCapacity=length+append_দৈর্ঘ্য;
    int32_t doubleCapacity=2*str.getCapacity();
    যদি(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    যদি(newCapacity<256) {
        newCapacity=256;
    }
    start=str.get_বাফার(newCapacity);
    যদি(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত মিথ্যা;
    }
    reorderStart=start+reorder_Start_সূচক;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    ফেরত সত্য;
}

শূন্য Reordering_বাফার::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    যদি(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t Reordering_বাফার::previousCC() {
    codePointLimit=codePointStart;
    যদি(reorderStart>=codePointStart) {
        ফেরত 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    যদি(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    ফেরত impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
শূন্য Reordering_বাফার::insert(UChar32 c, uint8_t cc) {
    জন্য(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_দৈর্ঘ্য(c);
    করো {
        *--r=*--q;
    } যখন(codePointLimit!=q);
    writeCodePoint(q, c);
    যদি(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

কাঠামো Canon_Iter_তথ্য : সর্বজনীন UMemory {
    Canon_Iter_তথ্য(UErrorCode &errorCode);
    ~Canon_Iter_তথ্য();
    শূন্য addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete f_Canon_Iter_তথ্য;
}

শূন্য
Normalizer2Impl::init(ধ্রুবক int32_t *inIndexes, ধ্রুবক UCPTrie *inTrie,
                      ধ্রুবক uint16_t *in_Extra_তথ্য, ধ্রুবক uint8_t *inSmallFCD) {
    minDecompNoCP = static_cast<char16_t>(inIndexes[IX_MIN_DECOMP_NO_CP]);
    minCompNoMaybeCP = static_cast<char16_t>(inIndexes[IX_MIN_COMP_NO_MAYBE_CP]);
    minLcccCP = static_cast<char16_t>(inIndexes[IX_MIN_LCCC_CP]);

    minYesNo = static_cast<uint16_t>(inIndexes[IX_MIN_YES_NO]);
    minYesNoMappingsOnly = static_cast<uint16_t>(inIndexes[IX_MIN_YES_NO_MAPPINGS_ONLY]);
    minNoNo = static_cast<uint16_t>(inIndexes[IX_MIN_NO_NO]);
    minNoNoCompBoundaryBefore = static_cast<uint16_t>(inIndexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE]);
    minNoNoCompNoMaybeCC = static_cast<uint16_t>(inIndexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC]);
    minNoNoEmpty = static_cast<uint16_t>(inIndexes[IX_MIN_NO_NO_EMPTY]);
    limitNoNo = static_cast<uint16_t>(inIndexes[IX_LIMIT_NO_NO]);
    minMaybeNo = static_cast<uint16_t>(inIndexes[IX_MIN_MAYBE_NO]);
    minMaybeNoCombinesFwd = static_cast<uint16_t>(inIndexes[IX_MIN_MAYBE_NO_COMBINES_FWD]);
    minMaybeYes = static_cast<uint16_t>(inIndexes[IX_MIN_MAYBE_YES]);
    U_ASSERT((minMaybeNo & 7) == 0);  // 8-aligned for noNoDelta bit fields
    centerNoNoDelta = (minMaybeNo >> DELTA_SHIFT) - MAX_DELTA - 1;

    normTrie=inTrie;
    extra_তথ্য=in_Extra_তথ্য;
    smallFCD=inSmallFCD;
}

U_CDECL_BEGIN

স্থির uint32_t U_CALLCONV
segmentStarterMapper(ধ্রুবক শূন্য * /*context*/, uint32_t value) {
    ফেরত value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

শূন্য
Normalizer2Impl::addLcccChars(UnicodeSet &set) ধ্রুবক {
    UChar32 start = 0, end;
    uint32_t norm16;
    যখন ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        যদি (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } অন্যথা যদি (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            যদি (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

শূন্য
Normalizer2Impl::addPropertyStarts(ধ্রুবক USetAdder *sa, UErrorCode & /*errorCode*/) ধ্রুবক {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    যখন ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        যদি (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t পূর্ব_FCD16 = getFCD16(start);
            যখন (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                যদি (fcd16 != পূর্ব_FCD16) {
                    sa->add(sa->set, start);
                    পূর্ব_FCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    জন্য(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_গুনতি) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

শূন্য
Normalizer2Impl::addCanonIterPropertyStarts(ধ্রুবক USetAdder *sa, UErrorCode &errorCode) ধ্রুবক {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    যদি (!ensure_Canon_Iter_তথ্য(errorCode)) { ফেরত; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    যখন ((end = ucptrie_getRange(f_Canon_Iter_তথ্য->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

ধ্রুবক char16_t *
Normalizer2Impl::copy_নিচের_Prefix_From_Nul_Terminated(ধ্রুবক char16_t *src,
                                                UChar32 min_Need_তথ্য_CP,
                                                Reordering_বাফার *বাফার,
                                                UErrorCode &errorCode) ধ্রুবক {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ধ্রুবক char16_t *পূর্ব_Src=src;
    char16_t c;
    যখন((c=*src++)<min_Need_তথ্য_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    যদি(--src!=পূর্ব_Src) {
        যদি(বাফার!=nullptr) {
            বাফার->appendZeroCC(পূর্ব_Src, src, errorCode);
        }
    }
    ফেরত src;
}

UnicodeString &
Normalizer2Impl::decompose(ধ্রুবক UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ফেরত dest;
    }
    ধ্রুবক char16_t *sArray=src.get_বাফার();
    যদি(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        ফেরত dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    ফেরত dest;
}

শূন্য
Normalizer2Impl::decompose(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                           UnicodeString &dest,
                           int32_t dest_দৈর্ঘ্য_Estimate,
                           UErrorCode &errorCode) ধ্রুবক {
    যদি(dest_দৈর্ঘ্য_Estimate<0 && limit!=nullptr) {
        dest_দৈর্ঘ্য_Estimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    Reordering_বাফার বাফার(*এই, dest);
    যদি(বাফার.init(dest_দৈর্ঘ্য_Estimate, errorCode)) {
        decompose(src, limit, &বাফার, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ধ্রুবক char16_t *
Normalizer2Impl::decompose(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                           Reordering_বাফার *বাফার,
                           UErrorCode &errorCode) ধ্রুবক {
    যদি(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত src;
    }
    UChar32 minNoCP=minDecompNoCP;
    যদি(limit==nullptr) {
        src=copy_নিচের_Prefix_From_Nul_Terminated(src, minNoCP, বাফার, errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত src;
        }
        limit=u_strchr(src, 0);
    }

    ধ্রুবক char16_t *পূর্ব_Src;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ধ্রুবক char16_t *পূর্ব_Boundary=src;
    uint8_t পূর্ব_CC=0;

    জন্য(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        জন্য(পূর্ব_Src=src; src!=limit;) {
            যদি( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } অন্যথা যদি(!U16_IS_LEAD(c)) {
                বিরতি;
            } অন্যথা {
                char16_t c2;
                যদি((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    যদি(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } অন্যথা {
                        বিরতি;
                    }
                } অন্যথা {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        যদি(src!=পূর্ব_Src) {
            যদি(বাফার!=nullptr) {
                যদি(!বাফার->appendZeroCC(পূর্ব_Src, src, errorCode)) {
                    বিরতি;
                }
            } অন্যথা {
                পূর্ব_CC=0;
                পূর্ব_Boundary=src;
            }
        }
        যদি(src==limit) {
            বিরতি;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_দৈর্ঘ্য(c);
        যদি(বাফার!=nullptr) {
            যদি(!decompose(c, norm16, *বাফার, errorCode)) {
                বিরতি;
            }
        } অন্যথা {
            যদি(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                যদি(পূর্ব_CC<=cc || cc==0) {
                    পূর্ব_CC=cc;
                    যদি(cc<=1) {
                        পূর্ব_Boundary=src;
                    }
                    চালু;
                }
            }
            ফেরত পূর্ব_Boundary;  // "no" or cc out of order
        }
    }
    ফেরত src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ধ্রুবক char16_t *
Normalizer2Impl::decomposeShort(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    যখন(src<limit) {
        যদি (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            ফেরত src;
        }
        ধ্রুবক char16_t *পূর্ব_Src = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_পরবর্তী(normTrie, UCPTRIE_16, src, limit, c, norm16);
        যদি (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            ফেরত পূর্ব_Src;
        }
        যদি(!decompose(c, norm16, বাফার, errorCode)) {
            ফেরত nullptr;
        }
        যদি (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ফেরত src;
        }
    }
    ফেরত src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_বাফার &বাফার,
                                 UErrorCode &errorCode) ধ্রুবক {
    // get the decomposition and the lead and trail cc's
    যদি (norm16 >= limitNoNo) {
        যদি (isMaybeYesOrNonZeroCC(norm16)) {
            ফেরত বাফার.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } অন্যথা যদি (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    যদি (norm16 < minYesNo) {
        // c does not decompose
        ফেরত বাফার.append(c, 0, errorCode);
    } অন্যথা যদি(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        ফেরত বাফার.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    ধ্রুবক uint16_t *mapping=get_তথ্য(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_দৈর্ঘ্য_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    যদি(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } অন্যথা {
        leadCC=0;
    }
    ফেরত বাফার.append(reinterpret_cast<ধ্রুবক char16_t*>(mapping) + 1, length, সত্য, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ধ্রুবক uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ধ্রুবক uint8_t *src, ধ্রুবক uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) ধ্রুবক {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ধ্রুবক uint8_t *পূর্ব_Boundary = src;
    // only for quick check
    uint8_t পূর্ব_CC = 0;

    জন্য (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ধ্রুবক uint8_t *fastStart = src;
        ধ্রুবক uint8_t *পূর্ব_Src;
        uint16_t norm16 = 0;

        জন্য (;;) {
            যদি (src == limit) {
                যদি (পূর্ব_Boundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                ফেরত src;
            }
            যদি (*src < minNoLead) {
                ++src;
            } অন্যথা {
                পূর্ব_Src = src;
                UCPTRIE_FAST_U8_পরবর্তী(normTrie, UCPTRIE_16, src, limit, norm16);
                যদি (!isMostDecompYesAndZeroCC(norm16)) {
                    বিরতি;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        যদি (পূর্ব_Src != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            যদি (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, পূর্ব_Src,
                                                   *sink, options, edits, errorCode)) {
                বিরতি;
            }
            পূর্ব_Boundary = পূর্ব_Src;
            পূর্ব_CC = 0;
        }

        // Medium-fast path: Quick check.
        যদি (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            যদি (পূর্ব_CC <= cc || cc == 0) {
                পূর্ব_CC = cc;
                যদি (cc <= 1) {
                    যদি (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, src,
                                                           *sink, options, edits, errorCode)) {
                        বিরতি;
                    }
                    পূর্ব_Boundary = src;
                }
                চালু;
            }
        }
        যদি (sink == nullptr) {
            ফেরত পূর্ব_Boundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        যদি (পূর্ব_Boundary != পূর্ব_Src && norm16HasDecompBoundaryBefore(norm16)) {
            যদি (!ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, পূর্ব_Src,
                                               *sink, options, edits, errorCode)) {
                বিরতি;
            }
            পূর্ব_Boundary = পূর্ব_Src;
        }
        Reordering_বাফার বাফার(*এই, s16, errorCode);
        যদি (U_FAILURE(errorCode)) {
            বিরতি;
        }
        decomposeShort(পূর্ব_Boundary, src, STOP_AT_LIMIT, মিথ্যা /* onlyContiguous */,
                       বাফার, errorCode);
        // Decompose until the next boundary.
        যদি (বাফার.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, মিথ্যা /* onlyContiguous */,
                                 বাফার, errorCode);
        }
        যদি (U_FAILURE(errorCode)) {
            বিরতি;
        }
        যদি ((src - পূর্ব_Src) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
            বিরতি;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        যদি (isMaybeYesOrNonZeroCC(norm16) && বাফার.equals(পূর্ব_Boundary, src)) {
            যদি (!ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, src,
                                               *sink, options, edits, errorCode)) {
                বিরতি;
            }
        } অন্যথা {
            যদি (!ByteSinkUtil::appendChange(পূর্ব_Boundary, src, বাফার.getStart(), বাফার.length(),
                                            *sink, edits, errorCode)) {
                বিরতি;
            }
        }
        পূর্ব_Boundary = src;
        পূর্ব_CC = 0;
    }
    ফেরত src;
}

ধ্রুবক uint8_t *
Normalizer2Impl::decomposeShort(ধ্রুবক uint8_t *src, ধ্রুবক uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    যখন (src < limit) {
        ধ্রুবক uint8_t *পূর্ব_Src = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_পরবর্তী(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        যদি (norm16 >= limitNoNo) {
            যদি (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                যদি (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    ফেরত পূর্ব_Src;
                }
                c = codePointFromValidUTF8(পূর্ব_Src, src);
                যদি (!বাফার.append(c, cc, errorCode)) {
                    ফেরত nullptr;
                }
                যদি (stopAt == STOP_AT_DECOMP_BOUNDARY && বাফার.getLastCC() <= 1) {
                    ফেরত src;
                }
                চালু;
            } অন্যথা যদি (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                যদি (stopAt != STOP_AT_LIMIT) {
                    ফেরত পূর্ব_Src;
                }
                c = codePointFromValidUTF8(পূর্ব_Src, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } অন্যথা যদি (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            ফেরত পূর্ব_Src;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        যদি (norm16 < minYesNo) {
            যদি (c < 0) {
                c = codePointFromValidUTF8(পূর্ব_Src, src);
            }
            // does not decompose
            যদি (!বাফার.append(c, 0, errorCode)) {
                ফেরত nullptr;
            }
        } অন্যথা যদি (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            যদি (c < 0) {
                c = codePointFromValidUTF8(পূর্ব_Src, src);
            }
            char16_t jamos[3];
            যদি (!বাফার.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                ফেরত nullptr;
            }
        } অন্যথা {
            // The character decomposes, get everything from the variable-length extra data.
            ধ্রুবক uint16_t *mapping = get_তথ্য(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_দৈর্ঘ্য_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            যদি (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } অন্যথা {
                leadCC = 0;
            }
            যদি (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                ফেরত পূর্ব_Src;
            }
            যদি (!বাফার.append(reinterpret_cast<ধ্রুবক char16_t*>(mapping) + 1, length, সত্য, leadCC, trailCC, errorCode)) {
                ফেরত nullptr;
            }
        }
        যদি ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && বাফার.getLastCC() <= 1)) {
            ফেরত src;
        }
    }
    ফেরত src;
}

ধ্রুবক char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t বাফার[4], int32_t &length) ধ্রুবক {
    uint16_t norm16;
    যদি(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        ফেরত nullptr;
    }
    ধ্রুবক char16_t *decomp = nullptr;
    যদি(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=বাফার;
        length=0;
        U16_APPEND_UNSAFE(বাফার, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    যদি (norm16 < minYesNo) {
        ফেরত decomp;
    } অন্যথা যদি(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, বাফার);
        ফেরত বাফার;
    }
    // c decomposes, get everything from the variable-length extra data
    ধ্রুবক uint16_t *mapping=get_তথ্য(norm16);
    length=*mapping&MAPPING_দৈর্ঘ্য_MASK;
    ফেরত reinterpret_cast<ধ্রুবক char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ধ্রুবক char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t বাফার[30], int32_t &length) ধ্রুবক {
    uint16_t norm16;
    যদি(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        ফেরত nullptr;
    } অন্যথা যদি(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, বাফার);
        length=2;
        ফেরত বাফার;
    } অন্যথা যদি(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(বাফার, length, c);
        ফেরত বাফার;
    }
    // c decomposes, get everything from the variable-length extra data
    ধ্রুবক uint16_t *mapping=get_তথ্য(norm16);
    uint16_t firstUnit=*mapping;
    int32_t m_দৈর্ঘ্য=firstUnit&MAPPING_দৈর্ঘ্য_MASK;  // length of normal mapping
    যদি(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ধ্রুবক uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        যদি(rm0<=MAPPING_দৈর্ঘ্য_MASK) {
            length=rm0;
            ফেরত reinterpret_cast<ধ্রুবক char16_t*>(rawMapping) - rm0;
        } অন্যথা {
            // Copy the normal mapping and replace its first two code units with rm0.
            বাফার[0] = static_cast<char16_t>(rm0);
            u_memcpy(বাফার + 1, reinterpret_cast<ধ্রুবক char16_t*>(mapping) + 1 + 2, m_দৈর্ঘ্য - 2);
            length=m_দৈর্ঘ্য-1;
            ফেরত বাফার;
        }
    } অন্যথা {
        length=m_দৈর্ঘ্য;
        ফেরত reinterpret_cast<ধ্রুবক char16_t*>(mapping) + 1;
    }
}

শূন্য Normalizer2Impl::decomposeAndAppend(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_মধ্য,
                                         Reordering_বাফার &বাফার,
                                         UErrorCode &errorCode) ধ্রুবক {
    বাফার.copyReorderableSuffixTo(safe_মধ্য);
    যদি(doDecompose) {
        decompose(src, limit, &বাফার, errorCode);
        ফেরত;
    }
    // Just merge the strings at the boundary.
    যৌক্তিক isFirst = সত্য;
    uint8_t firstCC = 0, পূর্ব_CC = 0, cc;
    ধ্রুবক char16_t *p = src;
    যখন (p != limit) {
        ধ্রুবক char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_পরবর্তী(normTrie, UCPTRIE_16, p, limit, c, norm16);
        যদি ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            বিরতি;
        }
        যদি (isFirst) {
            firstCC = cc;
            isFirst = মিথ্যা;
        }
        পূর্ব_CC = cc;
    }
    যদি(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    যদি (বাফার.append(src, static_cast<int32_t>(p - src), মিথ্যা, firstCC, পূর্ব_CC, errorCode)) {
        বাফার.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ধ্রুবক {
    ফেরত c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ধ্রুবক {
    যদি (norm16 < minNoNoCompNoMaybeCC) {
        ফেরত সত্য;
    }
    যদি (norm16 >= limitNoNo) {
        ফেরত norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ধ্রুবক uint16_t *mapping=get_তথ্য_For_Yes_Or_No(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ফেরত (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ধ্রুবক {
    যদি (c < minDecompNoCP) {
        ফেরত সত্য;
    }
    যদি (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        ফেরত সত্য;
    }
    ফেরত norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ধ্রুবক {
    যদি(norm16 <= minYesNo || isHangulLVT(norm16)) {
        ফেরত সত্য;
    }
    যদি (norm16 >= limitNoNo) {
        যদি (isMaybeYesOrNonZeroCC(norm16)) {
            ফেরত norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } অন্যথা যদি (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            ফেরত (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ধ্রুবক uint16_t *mapping=get_তথ্য(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    যদি(firstUnit>0x1ff) {
        ফেরত মিথ্যা;  // trailCC>1
    }
    যদি(firstUnit<=0xff) {
        ফেরত সত্য;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ফেরত (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(ধ্রুবক uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    যদি(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        যখন(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        যদি(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            যদি(firstUnit&COMP_1_TRIPLE) {
                ফেরত (static_cast<int32_t>(list[1]) << 16) | list[2];
            } অন্যথা {
                ফেরত list[1];
            }
        }
    } অন্যথা {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        জন্য(;;) {
            যদি(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } অন্যথা যদি(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                যদি(key2>(secondUnit=list[1])) {
                    যদি(firstUnit&COMP_1_LAST_TUPLE) {
                        বিরতি;
                    } অন্যথা {
                        list+=3;
                    }
                } অন্যথা যদি(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    ফেরত (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } অন্যথা {
                    বিরতি;
                }
            } অন্যথা {
                বিরতি;
            }
        }
    }
    ফেরত -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
শূন্য Normalizer2Impl::addComposites(ধ্রুবক uint16_t *list, UnicodeSet &set) ধ্রুবক {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    করো {
        firstUnit=*list;
        যদি((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } অন্যথা {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        যদি((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } যখন((firstUnit&COMP_1_LAST_TUPLE)==0);
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
শূন্য Normalizer2Impl::recompose(Reordering_বাফার &বাফার, int32_t recompose_Start_সূচক,
                                UBool onlyContiguous) ধ্রুবক {
    char16_t *p=বাফার.getStart()+recompose_Start_সূচক;
    char16_t *limit=বাফার.getLimit();
    যদি(p==limit) {
        ফেরত;
    }

    char16_t *starter, *pRemove, *q, *r;
    ধ্রুবক uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, পূর্ব_CC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=মিথ্যা;
    পূর্ব_CC=0;

    জন্য(;;) {
        UCPTRIE_FAST_U16_পরবর্তী(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        যদি( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (পূর্ব_CC<cc || পূর্ব_CC==0)
        ) {
            যদি(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                যদি(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t পূর্ব = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    যদি(পূর্ব<Hangul::JAMO_L_গুনতি) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (পূর্ব*Hangul::JAMO_V_গুনতি+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_গুনতি);
                        char16_t t;
                        যদি (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_গুনতি) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        যখন(r<limit) {
                            *q++=*r++;
                        }
                        limit=q;
                        p=pRemove;
                    }
                }
                /*
                 * No "else" for Jamo T:
                 * Since the input is in NFD, there are no Hangul LV syllables that
                 * a Jamo T could combine with.
                 * All Jamo Ts are combined above when handling Jamo Vs.
                 */
                যদি(p==limit) {
                    বিরতি;
                }
                compositionsList=nullptr;
                চালু;
            } অন্যথা যদি((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_দৈর্ঘ্য(c);  // pRemove & p: start & limit of the combining mark
                যদি(starterIsSupplementary) {
                    যদি(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } অন্যথা {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=মিথ্যা;
                        q=starter+1;
                        r=q+1;
                        যখন(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } অন্যথা যদি(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=সত্য;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    যখন(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } অন্যথা {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                যদি(pRemove<p) {
                    q=pRemove;
                    r=p;
                    যখন(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                যদি(p==limit) {
                    বিরতি;
                }
                // Is the composite a starter that combines forward?
                যদি(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } অন্যথা {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                চালু;
            }
        }

        // no combination this time
        পূর্ব_CC=cc;
        যদি(p==limit) {
            বিরতি;
        }

        // If c did not combine, then check if it is a starter.
        যদি(cc==0) {
            // Found a new starter.
            যদি((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                যদি(U_IS_BMP(c)) {
                    starterIsSupplementary=মিথ্যা;
                    starter=p-1;
                } অন্যথা {
                    starterIsSupplementary=সত্য;
                    starter=p-2;
                }
            }
        } অন্যথা যদি(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    বাফার.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ধ্রুবক {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ধ্রুবক uint16_t *list;
    যদি(isInert(norm16)) {
        ফেরত U_SENTINEL;
    } অন্যথা যদি(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        যদি(isJamoL(norm16)) {
            যদি (b < Hangul::JAMO_V_BASE) {
                ফেরত U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            যদি(b<Hangul::JAMO_V_গুনতি) {
                ফেরত
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_গুনতি+b)*
                     Hangul::JAMO_T_গুনতি);
            } অন্যথা {
                ফেরত U_SENTINEL;
            }
        } অন্যথা যদি(isHangulLV(norm16)) {
            যদি (b <= Hangul::JAMO_T_BASE) {
               ফেরত U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            যদি(b<Hangul::JAMO_T_গুনতি) {  // not b==0!
                ফেরত a+b;
            } অন্যথা {
                ফেরত U_SENTINEL;
            }
        } অন্যথা {
            // 'a' has a compositions list in extraData
            list=get_তথ্য_For_Yes_Or_No(norm16);
            যদি(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_দৈর্ঘ্য_MASK);  // + mapping length
            }
        }
    } অন্যথা যদি(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        ফেরত U_SENTINEL;
    } অন্যথা {
        list=get_তথ্য_For_Maybe(norm16);
        যদি(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_দৈর্ঘ্য_MASK);  // + mapping length
        }
    }
    যদি(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        ফেরত U_SENTINEL;
    }
#যদি U_SIGNED_ডান_SHIFT_IS_ARITHMETIC
    ফেরত combine(list, b)>>1;
#অন্যথা
    int32_t compositeAndFwd=combine(list, b);
    ফেরত compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_বাফার &বাফার,
                         UErrorCode &errorCode) ধ্রুবক {
    ধ্রুবক char16_t *পূর্ব_Boundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    যদি(limit==nullptr) {
        src=copy_নিচের_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &বাফার : nullptr,
                                           errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত মিথ্যা;
        }
        limit=u_strchr(src, 0);
        যদি (পূর্ব_Boundary != src) {
            যদি (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                পূর্ব_Boundary = src;
            } অন্যথা {
                বাফার.removeSuffix(1);
                পূর্ব_Boundary = --src;
            }
        }
    }

    জন্য (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ধ্রুবক char16_t *পূর্ব_Src;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        জন্য (;;) {
            যদি (src == limit) {
                যদি (পূর্ব_Boundary != limit && doCompose) {
                    বাফার.appendZeroCC(পূর্ব_Boundary, limit, errorCode);
                }
                ফেরত সত্য;
            }
            যদি( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } অন্যথা {
                পূর্ব_Src = src++;
                যদি(!U16_IS_LEAD(c)) {
                    বিরতি;
                } অন্যথা {
                    char16_t c2;
                    যদি(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        যদি(!isCompYesAndZeroCC(norm16)) {
                            বিরতি;
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
        যদি (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            যদি (!doCompose) {
                ফেরত মিথ্যা;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            যদি (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                যদি (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    যদি (পূর্ব_Boundary != পূর্ব_Src && !বাফার.appendZeroCC(পূর্ব_Boundary, পূর্ব_Src, errorCode)) {
                        বিরতি;
                    }
                    যদি(!বাফার.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        বিরতি;
                    }
                    পূর্ব_Boundary = src;
                    চালু;
                }
            } অন্যথা যদি (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                যদি (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    যদি (পূর্ব_Boundary != পূর্ব_Src && !বাফার.appendZeroCC(পূর্ব_Boundary, পূর্ব_Src, errorCode)) {
                        বিরতি;
                    }
                    ধ্রুবক char16_t *mapping = reinterpret_cast<ধ্রুবক char16_t *>(get_তথ্য_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_দৈর্ঘ্য_MASK;
                    যদি(!বাফার.appendZeroCC(mapping, mapping + length, errorCode)) {
                        বিরতি;
                    }
                    পূর্ব_Boundary = src;
                    চালু;
                }
            } অন্যথা যদি (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                যদি (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(পূর্ব_Boundary, পূর্ব_Src, onlyContiguous)) {
                    যদি (পূর্ব_Boundary != পূর্ব_Src && !বাফার.appendZeroCC(পূর্ব_Boundary, পূর্ব_Src, errorCode)) {
                        বিরতি;
                    }
                    পূর্ব_Boundary = src;
                    চালু;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } অন্যথা যদি (isJamoVT(norm16) && পূর্ব_Boundary != পূর্ব_Src) {
            char16_t পূর্ব=*(পূর্ব_Src-1);
            যদি(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(পূর্ব - Hangul::JAMO_L_BASE);
                যদি(l<Hangul::JAMO_L_গুনতি) {
                    যদি (!doCompose) {
                        ফেরত মিথ্যা;
                    }
                    int32_t t;
                    যদি (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_গুনতি) {
                        // The next character is a Jamo T.
                        ++src;
                    } অন্যথা যদি (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } অন্যথা {
                        t = -1;
                    }
                    যদি (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_গুনতি + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_গুনতি + t;
                        --পূর্ব_Src;  // Replace the Jamo L as well.
                        যদি (পূর্ব_Boundary != পূর্ব_Src && !বাফার.appendZeroCC(পূর্ব_Boundary, পূর্ব_Src, errorCode)) {
                            বিরতি;
                        }
                        যদি (!বাফার.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            বিরতি;
                        }
                        পূর্ব_Boundary = src;
                        চালু;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } অন্যথা যদি (Hangul::isHangulLV(পূর্ব)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                যদি (!doCompose) {
                    ফেরত মিথ্যা;
                }
                UChar32 syllable = পূর্ব + c - Hangul::JAMO_T_BASE;
                --পূর্ব_Src;  // Replace the Hangul LV as well.
                যদি (পূর্ব_Boundary != পূর্ব_Src && !বাফার.appendZeroCC(পূর্ব_Boundary, পূর্ব_Src, errorCode)) {
                    বিরতি;
                }
                যদি (!বাফার.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    বিরতি;
                }
                পূর্ব_Boundary = src;
                চালু;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } অন্যথা যদি (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            যদি (onlyContiguous /* FCC */ && getPreviousTrailCC(পূর্ব_Boundary, পূর্ব_Src) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                যদি (!doCompose) {
                    ফেরত মিথ্যা;
                }
            } অন্যথা {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ধ্রুবক char16_t *পরবর্তী_Src;
                uint16_t n16;
                জন্য (;;) {
                    যদি (src == limit) {
                        যদি (doCompose) {
                            বাফার.appendZeroCC(পূর্ব_Boundary, limit, errorCode);
                        }
                        ফেরত সত্য;
                    }
                    uint8_t পূর্ব_CC = cc;
                    পরবর্তী_Src = src;
                    UCPTRIE_FAST_U16_পরবর্তী(normTrie, UCPTRIE_16, পরবর্তী_Src, limit, c, n16);
                    যদি (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        যদি (পূর্ব_CC > cc) {
                            যদি (!doCompose) {
                                ফেরত মিথ্যা;
                            }
                            বিরতি;
                        }
                    } অন্যথা {
                        বিরতি;
                    }
                    src = পরবর্তী_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                যদি (norm16HasCompBoundaryBefore(n16)) {
                    যদি (isCompYesAndZeroCC(n16)) {
                        src = পরবর্তী_Src;
                    }
                    চালু;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        যদি (পূর্ব_Boundary != পূর্ব_Src && !norm16HasCompBoundaryBefore(norm16)) {
            ধ্রুবক char16_t *p = পূর্ব_Src;
            UCPTRIE_FAST_U16_পূর্ব(normTrie, UCPTRIE_16, পূর্ব_Boundary, p, c, norm16);
            যদি (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                পূর্ব_Src = p;
            }
        }
        যদি (doCompose && পূর্ব_Boundary != পূর্ব_Src && !বাফার.appendZeroCC(পূর্ব_Boundary, পূর্ব_Src, errorCode)) {
            বিরতি;
        }
        int32_t recompose_Start_সূচক=বাফার.length();
        // We know there is not a boundary here.
        decomposeShort(পূর্ব_Src, src, মিথ্যা /* !stopAtCompBoundary */, onlyContiguous,
                       বাফার, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, সত্য /* stopAtCompBoundary */, onlyContiguous,
                             বাফার, errorCode);
        যদি (U_FAILURE(errorCode)) {
            বিরতি;
        }
        যদি ((src - পূর্ব_Src) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত সত্য;
        }
        recompose(বাফার, recompose_Start_সূচক, onlyContiguous);
        যদি(!doCompose) {
            যদি(!বাফার.equals(পূর্ব_Src, src)) {
                ফেরত মিথ্যা;
            }
            বাফার.remove();
        }
        পূর্ব_Boundary=src;
    }
    ফেরত সত্য;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ধ্রুবক char16_t *
Normalizer2Impl::composeQuickCheck(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalization_Check_ফলাফল *pQCResult) ধ্রুবক {
    ধ্রুবক char16_t *পূর্ব_Boundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    যদি(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_নিচের_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        যদি (পূর্ব_Boundary != src) {
            যদি (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                পূর্ব_Boundary = src;
            } অন্যথা {
                পূর্ব_Boundary = --src;
            }
        }
    }

    জন্য(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ধ্রুবক char16_t *পূর্ব_Src;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        জন্য (;;) {
            যদি(src==limit) {
                ফেরত src;
            }
            যদি( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } অন্যথা {
                পূর্ব_Src = src++;
                যদি(!U16_IS_LEAD(c)) {
                    বিরতি;
                } অন্যথা {
                    char16_t c2;
                    যদি(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        যদি(!isCompYesAndZeroCC(norm16)) {
                            বিরতি;
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

        uint16_t পূর্ব_Norm16 = INERT;
        যদি (পূর্ব_Boundary != পূর্ব_Src) {
            যদি (norm16HasCompBoundaryBefore(norm16)) {
                পূর্ব_Boundary = পূর্ব_Src;
            } অন্যথা {
                ধ্রুবক char16_t *p = পূর্ব_Src;
                uint16_t n16;
                UCPTRIE_FAST_U16_পূর্ব(normTrie, UCPTRIE_16, পূর্ব_Boundary, p, c, n16);
                যদি (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    পূর্ব_Boundary = পূর্ব_Src;
                } অন্যথা {
                    পূর্ব_Boundary = p;
                    পূর্ব_Norm16 = n16;
                }
            }
        }

        যদি (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            যদি (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(পূর্ব_Norm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } অন্যথা {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ধ্রুবক char16_t *পরবর্তী_Src;
                জন্য (;;) {
                    যদি (norm16 < MIN_YES_YES_WITH_CC) {
                        যদি (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } অন্যথা {
                            ফেরত পূর্ব_Boundary;
                        }
                    }
                    যদি (src == limit) {
                        ফেরত src;
                    }
                    uint8_t পূর্ব_CC = fcd16;
                    পরবর্তী_Src = src;
                    UCPTRIE_FAST_U16_পরবর্তী(normTrie, UCPTRIE_16, পরবর্তী_Src, limit, c, norm16);
                    যদি (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        যদি (!(পূর্ব_CC <= cc || cc == 0)) {
                            বিরতি;
                        }
                    } অন্যথা {
                        বিরতি;
                    }
                    src = পরবর্তী_Src;
                }
                // src is after the last in-order combining mark.
                যদি (isCompYesAndZeroCC(norm16)) {
                    পূর্ব_Boundary = src;
                    src = পরবর্তী_Src;
                    চালু;
                }
            }
        }
        যদি(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        ফেরত পূর্ব_Boundary;
    }
}

শূন্য Normalizer2Impl::composeAndAppend(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_মধ্য,
                                       Reordering_বাফার &বাফার,
                                       UErrorCode &errorCode) ধ্রুবক {
    যদি(!বাফার.isEmpty()) {
        ধ্রুবক char16_t *firstStarterInSrc=find_পরবর্তী_Comp_Boundary(src, limit, onlyContiguous);
        যদি(src!=firstStarterInSrc) {
            ধ্রুবক char16_t *lastStarterInDest=findPreviousCompBoundary(বাফার.getStart(),
                                                                    বাফার.getLimit(), onlyContiguous);
            int32_t dest_Suffix_দৈর্ঘ্য = static_cast<int32_t>(বাফার.getLimit() - lastStarterInDest);
            UnicodeString মধ্য(lastStarterInDest, dest_Suffix_দৈর্ঘ্য);
            বাফার.removeSuffix(dest_Suffix_দৈর্ঘ্য);
            safe_মধ্য=মধ্য;
            মধ্য.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            ধ্রুবক char16_t *মধ্য_Start=মধ্য.get_বাফার();
            compose(মধ্য_Start, মধ্য_Start+মধ্য.length(), onlyContiguous,
                    সত্য, বাফার, errorCode);
            যদি(U_FAILURE(errorCode)) {
                ফেরত;
            }
            src=firstStarterInSrc;
        }
    }
    যদি(doCompose) {
        compose(src, limit, onlyContiguous, সত্য, বাফার, errorCode);
    } অন্যথা {
        যদি(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        বাফার.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ধ্রুবক uint8_t *src, ধ্রুবক uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) ধ্রুবক {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ধ্রুবক uint8_t *পূর্ব_Boundary = src;

    জন্য (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ধ্রুবক uint8_t *পূর্ব_Src;
        uint16_t norm16 = 0;
        জন্য (;;) {
            যদি (src == limit) {
                যদি (পূর্ব_Boundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                ফেরত সত্য;
            }
            যদি (*src < minNoMaybeLead) {
                ++src;
            } অন্যথা {
                পূর্ব_Src = src;
                UCPTRIE_FAST_U8_পরবর্তী(normTrie, UCPTRIE_16, src, limit, norm16);
                যদি (!isCompYesAndZeroCC(norm16)) {
                    বিরতি;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        যদি (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            যদি (sink == nullptr) {
                ফেরত মিথ্যা;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            যদি (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                যদি (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    যদি (পূর্ব_Boundary != পূর্ব_Src &&
                            !ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, পূর্ব_Src,
                                                           *sink, options, edits, errorCode)) {
                        বিরতি;
                    }
                    appendCodePointDelta(পূর্ব_Src, src, getAlgorithmicDelta(norm16), *sink, edits);
                    পূর্ব_Boundary = src;
                    চালু;
                }
            } অন্যথা যদি (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                যদি (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    যদি (পূর্ব_Boundary != পূর্ব_Src &&
                            !ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, পূর্ব_Src,
                                                           *sink, options, edits, errorCode)) {
                        বিরতি;
                    }
                    ধ্রুবক uint16_t *mapping = get_তথ্য_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_দৈর্ঘ্য_MASK;
                    যদি (!ByteSinkUtil::appendChange(পূর্ব_Src, src, reinterpret_cast<ধ্রুবক char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        বিরতি;
                    }
                    পূর্ব_Boundary = src;
                    চালু;
                }
            } অন্যথা যদি (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                যদি (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(পূর্ব_Boundary, পূর্ব_Src, onlyContiguous)) {
                    যদি (পূর্ব_Boundary != পূর্ব_Src &&
                            !ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, পূর্ব_Src,
                                                           *sink, options, edits, errorCode)) {
                        বিরতি;
                    }
                    যদি (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - পূর্ব_Src), 0);
                    }
                    পূর্ব_Boundary = src;
                    চালু;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } অন্যথা যদি (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - পূর্ব_Src) == 3 && *পূর্ব_Src == 0xe1);
            UChar32 পূর্ব = previousHangulOrJamo(পূর্ব_Boundary, পূর্ব_Src);
            যদি (পূর্ব_Src[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = পূর্ব - Hangul::JAMO_L_BASE;
                যদি (static_cast<uint32_t>(l) < Hangul::JAMO_L_গুনতি) {
                    যদি (sink == nullptr) {
                        ফেরত মিথ্যা;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    যদি (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } অন্যথা যদি (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    যদি (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_গুনতি + (পূর্ব_Src[2]-0xa1)) *
                            Hangul::JAMO_T_গুনতি + t;
                        পূর্ব_Src -= 3;  // Replace the Jamo L as well.
                        যদি (পূর্ব_Boundary != পূর্ব_Src &&
                                !ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, পূর্ব_Src,
                                                               *sink, options, edits, errorCode)) {
                            বিরতি;
                        }
                        ByteSinkUtil::appendCodePoint(পূর্ব_Src, src, syllable, *sink, edits);
                        পূর্ব_Boundary = src;
                        চালু;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } অন্যথা যদি (Hangul::isHangulLV(পূর্ব)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                যদি (sink == nullptr) {
                    ফেরত মিথ্যা;
                }
                UChar32 syllable = পূর্ব + getJamoTMinusBase(পূর্ব_Src, src);
                পূর্ব_Src -= 3;  // Replace the Hangul LV as well.
                যদি (পূর্ব_Boundary != পূর্ব_Src &&
                        !ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, পূর্ব_Src,
                                                       *sink, options, edits, errorCode)) {
                    বিরতি;
                }
                ByteSinkUtil::appendCodePoint(পূর্ব_Src, src, syllable, *sink, edits);
                পূর্ব_Boundary = src;
                চালু;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } অন্যথা যদি (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            যদি (onlyContiguous /* FCC */ && getPreviousTrailCC(পূর্ব_Boundary, পূর্ব_Src) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                যদি (sink == nullptr) {
                    ফেরত মিথ্যা;
                }
            } অন্যথা {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ধ্রুবক uint8_t *পরবর্তী_Src;
                uint16_t n16;
                জন্য (;;) {
                    যদি (src == limit) {
                        যদি (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        ফেরত সত্য;
                    }
                    uint8_t পূর্ব_CC = cc;
                    পরবর্তী_Src = src;
                    UCPTRIE_FAST_U8_পরবর্তী(normTrie, UCPTRIE_16, পরবর্তী_Src, limit, n16);
                    যদি (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        যদি (পূর্ব_CC > cc) {
                            যদি (sink == nullptr) {
                                ফেরত মিথ্যা;
                            }
                            বিরতি;
                        }
                    } অন্যথা {
                        বিরতি;
                    }
                    src = পরবর্তী_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                যদি (norm16HasCompBoundaryBefore(n16)) {
                    যদি (isCompYesAndZeroCC(n16)) {
                        src = পরবর্তী_Src;
                    }
                    চালু;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        যদি (পূর্ব_Boundary != পূর্ব_Src && !norm16HasCompBoundaryBefore(norm16)) {
            ধ্রুবক uint8_t *p = পূর্ব_Src;
            UCPTRIE_FAST_U8_পূর্ব(normTrie, UCPTRIE_16, পূর্ব_Boundary, p, norm16);
            যদি (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                পূর্ব_Src = p;
            }
        }
        Reordering_বাফার বাফার(*এই, s16, errorCode);
        যদি (U_FAILURE(errorCode)) {
            বিরতি;
        }
        // We know there is not a boundary here.
        decomposeShort(পূর্ব_Src, src, STOP_AT_LIMIT, onlyContiguous,
                       বাফার, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             বাফার, errorCode);
        যদি (U_FAILURE(errorCode)) {
            বিরতি;
        }
        যদি ((src - পূর্ব_Src) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত সত্য;
        }
        recompose(বাফার, 0, onlyContiguous);
        যদি (!বাফার.equals(পূর্ব_Src, src)) {
            যদি (sink == nullptr) {
                ফেরত মিথ্যা;
            }
            যদি (পূর্ব_Boundary != পূর্ব_Src &&
                    !ByteSinkUtil::appendUnchanged(পূর্ব_Boundary, পূর্ব_Src,
                                                   *sink, options, edits, errorCode)) {
                বিরতি;
            }
            যদি (!ByteSinkUtil::appendChange(পূর্ব_Src, src, বাফার.getStart(), বাফার.length(),
                                            *sink, edits, errorCode)) {
                বিরতি;
            }
            পূর্ব_Boundary = src;
        }
    }
    ফেরত সত্য;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit) ধ্রুবক {
    যদি (src == limit || *src < minCompNoMaybeCP) {
        ফেরত সত্য;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_পরবর্তী(normTrie, UCPTRIE_16, src, limit, c, norm16);
    ফেরত norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ধ্রুবক uint8_t *src, ধ্রুবক uint8_t *limit) ধ্রুবক {
    যদি (src == limit) {
        ফেরত সত্য;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_পরবর্তী(normTrie, UCPTRIE_16, src, limit, norm16);
    ফেরত norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ধ্রুবক char16_t *start, ধ্রুবক char16_t *p,
                                            UBool onlyContiguous) ধ্রুবক {
    যদি (start == p) {
        ফেরত সত্য;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_পূর্ব(normTrie, UCPTRIE_16, start, p, c, norm16);
    ফেরত norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ধ্রুবক uint8_t *start, ধ্রুবক uint8_t *p,
                                            UBool onlyContiguous) ধ্রুবক {
    যদি (start == p) {
        ফেরত সত্য;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_পূর্ব(normTrie, UCPTRIE_16, start, p, norm16);
    ফেরত norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ধ্রুবক char16_t *Normalizer2Impl::findPreviousCompBoundary(ধ্রুবক char16_t *start, ধ্রুবক char16_t *p,
                                                       UBool onlyContiguous) ধ্রুবক {
    যখন (p != start) {
        ধ্রুবক char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_পূর্ব(normTrie, UCPTRIE_16, start, p, c, norm16);
        যদি (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ফেরত codePointLimit;
        }
        যদি (hasCompBoundaryBefore(c, norm16)) {
            ফেরত p;
        }
    }
    ফেরত p;
}

ধ্রুবক char16_t *Normalizer2Impl::find_পরবর্তী_Comp_Boundary(ধ্রুবক char16_t *p, ধ্রুবক char16_t *limit,
                                                   UBool onlyContiguous) ধ্রুবক {
    যখন (p != limit) {
        ধ্রুবক char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_পরবর্তী(normTrie, UCPTRIE_16, p, limit, c, norm16);
        যদি (hasCompBoundaryBefore(c, norm16)) {
            ফেরত codePointStart;
        }
        যদি (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ফেরত p;
        }
    }
    ফেরত p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ধ্রুবক char16_t *start, ধ্রুবক char16_t *p) ধ্রুবক {
    যদি (start == p) {
        ফেরত 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_পূর্ব(start, 0, i, c);
    ফেরত static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ধ্রুবক uint8_t *start, ধ্রুবক uint8_t *p) ধ্রুবক {
    যদি (start == p) {
        ফেরত 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_পূর্ব(start, 0, i, c);
    ফেরত static_cast<uint8_t>(getFCD16(c));
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
#যদি (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_তথ্য(UChar32 c) ধ্রুবক {
    uint16_t norm16=getNorm16(c);
    যদি (norm16 >= limitNoNo) {
        যদি(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            ফেরত norm16|(norm16<<8);
        } অন্যথা যদি(norm16>=minMaybeYes) {
            ফেরত 0;
        } অন্যথা যদি(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            যদি (deltaTrailCC <= DELTA_TCCC_1) {
                ফেরত deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    যদি(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        ফেরত 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ধ্রুবক uint16_t *mapping=get_তথ্য(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    যদি(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    ফেরত norm16;
}
#যদি (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ধ্রুবক {
    U_ASSERT(norm16 >= minMaybeNo);
    যদি (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        ফেরত norm16 | (norm16<<8);
    } অন্যথা যদি (norm16 >= minMaybeYes) {
        ফেরত 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ধ্রুবক uint16_t *mapping = get_তথ্য_For_Maybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    ফেরত firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ধ্রুবক char16_t *
Normalizer2Impl::makeFCD(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                         Reordering_বাফার *বাফার,
                         UErrorCode &errorCode) ধ্রুবক {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ধ্রুবক char16_t *পূর্ব_Boundary=src;
    int32_t পূর্ব_FCD16=0;
    যদি(limit==nullptr) {
        src=copy_নিচের_Prefix_From_Nul_Terminated(src, minLcccCP, বাফার, errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত src;
        }
        যদি(পূর্ব_Boundary<src) {
            পূর্ব_Boundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            পূর্ব_FCD16=getFCD16(*(src-1));
            যদি(পূর্ব_FCD16>1) {
                --পূর্ব_Boundary;
            }
        }
        limit=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    ধ্রুবক char16_t *পূর্ব_Src;
    UChar32 c=0;
    uint16_t fcd16=0;

    জন্য(;;) {
        // count code units with lccc==0
        জন্য(পূর্ব_Src=src; src!=limit;) {
            যদি((c=*src)<minLcccCP) {
                পূর্ব_FCD16=~c;
                ++src;
            } অন্যথা যদি(!singleLeadMightHaveNonZeroFCD16(c)) {
                পূর্ব_FCD16=0;
                ++src;
            } অন্যথা {
                যদি(U16_IS_LEAD(c)) {
                    char16_t c2;
                    যদি((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                যদি((fcd16=get_FCD16_From_Norm_তথ্য(c))<=0xff) {
                    পূর্ব_FCD16=fcd16;
                    src+=U16_দৈর্ঘ্য(c);
                } অন্যথা {
                    বিরতি;
                }
            }
        }
        // copy these code units all at once
        যদি(src!=পূর্ব_Src) {
            যদি(বাফার!=nullptr && !বাফার->appendZeroCC(পূর্ব_Src, src, errorCode)) {
                বিরতি;
            }
            যদি(src==limit) {
                বিরতি;
            }
            পূর্ব_Boundary=src;
            // We know that the previous character's lccc==0.
            যদি(পূর্ব_FCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 পূর্ব=~পূর্ব_FCD16;
                যদি(পূর্ব<minDecompNoCP) {
                    পূর্ব_FCD16=0;
                } অন্যথা {
                    পূর্ব_FCD16=get_FCD16_From_Norm_তথ্য(পূর্ব);
                    যদি(পূর্ব_FCD16>1) {
                        --পূর্ব_Boundary;
                    }
                }
            } অন্যথা {
                ধ্রুবক char16_t *p=src-1;
                যদি(U16_IS_TRAIL(*p) && পূর্ব_Src<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    পূর্ব_FCD16=get_FCD16_From_Norm_তথ্য(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                যদি(পূর্ব_FCD16>1) {
                    পূর্ব_Boundary=p;
                }
            }
            // The start of the current character (c).
            পূর্ব_Src=src;
        } অন্যথা যদি(src==limit) {
            বিরতি;
        }

        src+=U16_দৈর্ঘ্য(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        যদি((পূর্ব_FCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            যদি((fcd16&0xff)<=1) {
                পূর্ব_Boundary=src;
            }
            যদি(বাফার!=nullptr && !বাফার->appendZeroCC(c, errorCode)) {
                বিরতি;
            }
            পূর্ব_FCD16=fcd16;
            চালু;
        } অন্যথা যদি(বাফার==nullptr) {
            ফেরত পূর্ব_Boundary;  // quick check "no"
        } অন্যথা {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            বাফার->removeSuffix(static_cast<int32_t>(পূর্ব_Src - পূর্ব_Boundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=find_পরবর্তী_FCDBoundary(src, limit);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(পূর্ব_Boundary, src, মিথ্যা, মিথ্যা, *বাফার, errorCode);
            যদি (U_FAILURE(errorCode)) {
                বিরতি;
            }
            পূর্ব_Boundary=src;
            পূর্ব_FCD16=0;
        }
    }
    ফেরত src;
}

শূন্য Normalizer2Impl::makeFCDAndAppend(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_মধ্য,
                                       Reordering_বাফার &বাফার,
                                       UErrorCode &errorCode) ধ্রুবক {
    যদি(!বাফার.isEmpty()) {
        ধ্রুবক char16_t *firstBoundaryInSrc=find_পরবর্তী_FCDBoundary(src, limit);
        যদি(src!=firstBoundaryInSrc) {
            ধ্রুবক char16_t *lastBoundaryInDest=findPreviousFCDBoundary(বাফার.getStart(),
                                                                    বাফার.getLimit());
            int32_t dest_Suffix_দৈর্ঘ্য = static_cast<int32_t>(বাফার.getLimit() - lastBoundaryInDest);
            UnicodeString মধ্য(lastBoundaryInDest, dest_Suffix_দৈর্ঘ্য);
            বাফার.removeSuffix(dest_Suffix_দৈর্ঘ্য);
            safe_মধ্য=মধ্য;
            মধ্য.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            ধ্রুবক char16_t *মধ্য_Start=মধ্য.get_বাফার();
            makeFCD(মধ্য_Start, মধ্য_Start+মধ্য.length(), &বাফার, errorCode);
            যদি(U_FAILURE(errorCode)) {
                ফেরত;
            }
            src=firstBoundaryInSrc;
        }
    }
    যদি(doMakeFCD) {
        makeFCD(src, limit, &বাফার, errorCode);
    } অন্যথা {
        যদি(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        বাফার.appendZeroCC(src, limit, errorCode);
    }
}

ধ্রুবক char16_t *Normalizer2Impl::findPreviousFCDBoundary(ধ্রুবক char16_t *start, ধ্রুবক char16_t *p) ধ্রুবক {
    যখন(start<p) {
        ধ্রুবক char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_পূর্ব(normTrie, UCPTRIE_16, start, p, c, norm16);
        যদি (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            ফেরত codePointLimit;
        }
        যদি (norm16HasDecompBoundaryBefore(norm16)) {
            ফেরত p;
        }
    }
    ফেরত p;
}

ধ্রুবক char16_t *Normalizer2Impl::find_পরবর্তী_FCDBoundary(ধ্রুবক char16_t *p, ধ্রুবক char16_t *limit) ধ্রুবক {
    যখন(p<limit) {
        ধ্রুবক char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_পরবর্তী(normTrie, UCPTRIE_16, p, limit, c, norm16);
        যদি (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            ফেরত codePointStart;
        }
        যদি (norm16HasDecompBoundaryAfter(norm16)) {
            ফেরত p;
        }
    }
    ফেরত p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_তথ্য::Canon_Iter_তথ্য(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

Canon_Iter_তথ্য::~Canon_Iter_তথ্য() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

শূন্য Canon_Iter_তথ্য::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_মান = umutablecptrie_get(mutableTrie, decompLead);
    যদি((canon_মান&(CANON_HAS_SET|CANON_মান_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_মান|origin, &errorCode);
    } অন্যথা {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        যদি((canon_মান&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(নতুন UnicodeSet, errorCode);
            set=lpSet.getAlias();
            যদি(U_FAILURE(errorCode)) {
                ফেরত;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_মান & CANON_মান_MASK);
            canon_মান = (canon_মান & ~CANON_মান_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_মান, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            যদি (U_FAILURE(errorCode)) {
                ফেরত;
            }
            যদি(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } অন্যথা {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_মান & CANON_মান_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
শ্রেণী Init_Canon_Iter_তথ্য {
সর্বজনীন:
    স্থির শূন্য doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
স্থির শূন্য U_CALLCONV
init_Canon_Iter_তথ্য(Normalizer2Impl *impl, UErrorCode &errorCode) {
    Init_Canon_Iter_তথ্য::doInit(impl, errorCode);
}

U_CDECL_END

শূন্য Init_Canon_Iter_তথ্য::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->f_Canon_Iter_তথ্য == nullptr);
    impl->f_Canon_Iter_তথ্য = নতুন Canon_Iter_তথ্য(errorCode);
    যদি (impl->f_Canon_Iter_তথ্য == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        যখন ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            যদি (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_তথ্য_From_Norm16(start, end, value, *impl->f_Canon_Iter_তথ্য, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->f_Canon_Iter_তথ্য->mutableTrie, "CanonIterData");
#endif
        impl->f_Canon_Iter_তথ্য->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_তথ্য->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_মান_BITS_32, &errorCode);
        umutablecptrie_close(impl->f_Canon_Iter_তথ্য->mutableTrie);
        impl->f_Canon_Iter_তথ্য->mutableTrie = nullptr;
    }
    যদি (U_FAILURE(errorCode)) {
        delete impl->f_Canon_Iter_তথ্য;
        impl->f_Canon_Iter_তথ্য = nullptr;
    }
}

শূন্য Normalizer2Impl::make_Canon_Iter_তথ্য_From_Norm16(UChar32 start, UChar32 end, ধ্রুবক uint16_t norm16,
                                                  Canon_Iter_তথ্য &new_তথ্য,
                                                  UErrorCode &errorCode) ধ্রুবক {
    যদি(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        ফেরত;
    }
    জন্য(UChar32 c=start; c<=end; ++c) {
        uint32_t old_মান = umutablecptrie_get(new_তথ্য.mutableTrie, c);
        uint32_t new_মান=old_মান;
        যদি(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_মান|=CANON_NOT_SEGMENT_STARTER;
            যদি(norm16<MIN_NORMAL_MAYBE_YES) {
                new_মান|=CANON_HAS_COMPOSITIONS;
            }
        } অন্যথা যদি(norm16<minYesNo) {
            new_মান|=CANON_HAS_COMPOSITIONS;
        } অন্যথা {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            যদি (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            যদি (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ধ্রুবক uint16_t *mapping=get_তথ্য_For_Yes_Or_No(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_দৈর্ঘ্য_MASK;
                যদি((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    যদি(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_মান|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                যদি(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_পরবর্তী_UNSAFE(mapping, i, c2);
                    new_তথ্য.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    যদি(norm16_2>=minNoNo) {
                        যখন(i<length) {
                            U16_পরবর্তী_UNSAFE(mapping, i, c2);
                            uint32_t c2_মান = umutablecptrie_get(new_তথ্য.mutableTrie, c2);
                            যদি((c2_মান&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(new_তথ্য.mutableTrie, c2,
                                                   c2_মান|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } অন্যথা {
                // c decomposed to c2 algorithmically; c has cc==0
                new_তথ্য.addToStartSet(c, c2, errorCode);
            }
        }
        যদি(new_মান!=old_মান) {
            umutablecptrie_set(new_তথ্য.mutableTrie, c, new_মান, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_তথ্য(UErrorCode &errorCode) ধ্রুবক {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(এই);
    umtx_initOnce(me->f_Canon_Iter_তথ্য_Init_Once, &init_Canon_Iter_তথ্য, me, errorCode);
    ফেরত U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_মান(UChar32 c) ধ্রুবক {
    ফেরত static_cast<int32_t>(ucptrie_get(f_Canon_Iter_তথ্য->trie, c));
}

ধ্রুবক UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) ধ্রুবক {
    ফেরত *static_cast<ধ্রুবক UnicodeSet*>(f_Canon_Iter_তথ্য->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ধ্রুবক {
    ফেরত get_Canon_মান(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) ধ্রুবক {
    int32_t canon_মান=get_Canon_মান(c)&~CANON_NOT_SEGMENT_STARTER;
    যদি(canon_মান==0) {
        ফেরত মিথ্যা;
    }
    set.clear();
    int32_t value=canon_মান&CANON_মান_MASK;
    যদি((canon_মান&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } অন্যথা যদি(value!=0) {
        set.add(value);
    }
    যদি((canon_মান&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        যদি(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_গুনতি);
            set.add(syllable, syllable+Hangul::JAMO_VT_গুনতি-1);
        } অন্যথা {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    ফেরত সত্য;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(ধ্রুবক UDataSwapper *ds,
            ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
            UErrorCode *pErrorCode) {
    ধ্রুবক UDataInfo *pInfo;
    int32_t header_আকার;

    ধ্রুবক uint8_t *inBytes;
    uint8_t *outBytes;

    ধ্রুবক int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_যোগ_আকার+1];

    int32_t i, offset, পরবর্তী_Offset, size;

    /* udata_swapDataHeader checks the arguments */
    header_আকার=udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    যদি(!(
        pInfo->তথ্য_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->তথ্য_Format[1]==0x72 &&
        pInfo->তথ্য_Format[2]==0x6d &&
        pInfo->তথ্য_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1],
                         pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    inBytes=(ধ্রুবক uint8_t *)in_তথ্য+header_আকার;
    outBytes=(out_তথ্য == nullptr) ? nullptr : (uint8_t *)out_তথ্য+header_আকার;

    inIndexes=(ধ্রুবক int32_t *)inBytes;
    int32_t min_Indexes_দৈর্ঘ্য;
    যদি(formatVersion0==1) {
        min_Indexes_দৈর্ঘ্য=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } অন্যথা যদি(formatVersion0==2) {
        min_Indexes_দৈর্ঘ্য=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } অন্যথা যদি(formatVersion0<=4) {
        min_Indexes_দৈর্ঘ্য=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } অন্যথা {
        min_Indexes_দৈর্ঘ্য=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    যদি(length>=0) {
        length-=header_আকার;
        যদি(length<min_Indexes_দৈর্ঘ্য*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
    }

    /* read the first few indexes */
    জন্য(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_যোগ_আকার];

    যদি(length>=0) {
        যদি(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }

        /* copy the data for inaccessible bytes */
        যদি(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset=0;

        /* swap the int32_t indexes[] */
        পরবর্তী_Offset=indexes[Normalizer2Impl::IX_NORM_TRIE_OFFSET];
        ds->swapArray32(ds, inBytes, পরবর্তী_Offset-offset, outBytes, pErrorCode);
        offset=পরবর্তী_Offset;

        /* swap the trie */
        পরবর্তী_Offset=indexes[Normalizer2Impl::IX_EXTRA_তথ্য_OFFSET];
        utrie_swapAnyVersion(ds, inBytes+offset, পরবর্তী_Offset-offset, outBytes+offset, pErrorCode);
        offset=পরবর্তী_Offset;

        /* swap the uint16_t extraData[] */
        পরবর্তী_Offset=indexes[Normalizer2Impl::IX_SMALL_FCD_OFFSET];
        ds->swapArray16(ds, inBytes+offset, পরবর্তী_Offset-offset, outBytes+offset, pErrorCode);
        offset=পরবর্তী_Offset;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        পরবর্তী_Offset=indexes[Normalizer2Impl::IX_SMALL_FCD_OFFSET+1];
        offset=পরবর্তী_Offset;

        U_ASSERT(offset==size);
    }

    ফেরত header_আকার+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
