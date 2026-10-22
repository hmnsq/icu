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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_NORMALIZATION

#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/edits.h"
#ଅନ୍ତର୍ଭାଵ "unicode/normalizer2.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringoptions.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/umutablecptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "normalizer2impl.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucptrie_impl.h"
#ଅନ୍ତର୍ଭାଵ "uset_imp.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

U_NAMESPACE_BEGIN

ନାମାକାଶ {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
ଧାଡ଼ିଭିତରେ uint8_t leadByteForCP(UChar32 c) {
    ଯଦି (c <= 0x7f) {
        ଫେରନ୍ତୁ static_cast<uint8_t>(c);
    } ଅନ୍ୟଥା ଯଦି (c <= 0x7ff) {
        ଫେରନ୍ତୁ static_cast<uint8_t>(0xc0 + (c >> 6));
    } ଅନ୍ୟଥା {
        // Should not occur because ccc(U+0300)!=0.
        ଫେରନ୍ତୁ 0xe0;
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
UChar32 codePointFromValidUTF8(ଅଚଳ uint8_t *cpStart, ଅଚଳ uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    ବାଛ(cpLimit-cpStart) {
    ସ୍ଥିତି 1:
        ଫେରନ୍ତୁ c;
    ସ୍ଥିତି 2:
        ଫେରନ୍ତୁ ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    ସ୍ଥିତି 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        ଫେରନ୍ତୁ static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    ସ୍ଥିତି 4:
        ଫେରନ୍ତୁ ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(ଅଚଳ uint8_t *start, ଅଚଳ uint8_t *p) {
    ଯଦି ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        ଯଦି (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            ଫେରନ୍ତୁ ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    ଫେରନ୍ତୁ U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(ଅଚଳ uint8_t *src, ଅଚଳ uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    ଯଦି ((limit - src) >= 3 && *src == 0xe1) {
        ଯଦି (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            ଯଦି (0xa8 <= t && t <= 0xbf) {
                ଫେରନ୍ତୁ t - 0xa7;
            }
        } ଅନ୍ୟଥା ଯଦି (src[1] == 0x87) {
            uint8_t t = src[2];
            ଯଦି (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                ଫେରନ୍ତୁ t - (0xa7 - 0x40);
            }
        }
    }
    ଫେରନ୍ତୁ -1;
}

ଶୂନ୍ୟ
appendCodePointDelta(ଅଚଳ uint8_t *cpStart, ଅଚଳ uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    ଅକ୍ଷର buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    ଯଦି (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } ଅନ୍ୟଥା {
        int32_t trail = *(cpLimit-1) + delta;
        ଯଦି (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            କର { buffer[length++] = *cpStart++; } ଯେତେବେଳେ (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } ଅନ୍ୟଥା {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    ଯଦି (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(ଅଚଳ Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    ଯଦି (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    ଯଦି(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    ଯଦି(start==limit) {
        lastCC=0;
    } ଅନ୍ୟଥା {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        ଯଦି(lastCC>1) {
            ଯେତେବେଳେ(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool ReorderingBuffer::equals(ଅଚଳ char16_t *otherStart, ଅଚଳ char16_t *otherLimit) ଅଚଳ {
    int32_t length = static_cast<int32_t>(limit - start);
    ଫେରନ୍ତୁ
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(ଅଚଳ uint8_t *otherStart, ଅଚଳ uint8_t *otherLimit) ଅଚଳ {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    ଯଦି (otherLength < length || (otherLength / 3) > length) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    ପାଇଁ (int32_t i = 0, j = 0;;) {
        ଯଦି (i >= length) {
            ଫେରନ୍ତୁ j >= otherLength;
        } ଅନ୍ୟଥା ଯଦି (j >= otherLength) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        ଯଦି (c != other) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    ଯଦି(remainingCapacity<2 && !resize(2, errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        ଯଦି(cc<=1) {
            reorderStart=limit;
        }
    } ଅନ୍ୟଥା {
        insert(c, cc);
    }
    remainingCapacity-=2;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool ReorderingBuffer::append(ଅଚଳ char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    ଯଦି(length==0) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(remainingCapacity<length && !resize(length, errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    remainingCapacity-=length;
    ଯଦି(lastCC<=leadCC || leadCC==0) {
        ଯଦି(trailCC<=1) {
            reorderStart=limit+length;
        } ଅନ୍ୟଥା ଯଦି(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        ଅଚଳ char16_t *sLimit=s+length;
        କର { *limit++=*s++; } ଯେତେବେଳେ(s!=sLimit);
        lastCC=trailCC;
    } ଅନ୍ୟଥା {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        ଯେତେବେଳେ(i<length) {
            U16_NEXT(s, i, length, c);
            ଯଦି(i<length) {
                ଯଦି (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } ଅନ୍ୟଥା {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } ଅନ୍ୟଥା {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    ଯଦି(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    remainingCapacity-=cpLength;
    ଯଦି(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } ଅନ୍ୟଥା {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool ReorderingBuffer::appendZeroCC(ଅଚଳ char16_t *s, ଅଚଳ char16_t *sLimit, UErrorCode &errorCode) {
    ଯଦି(s==sLimit) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    ଯଦି(remainingCapacity<length && !resize(length, errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ ReorderingBuffer::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

ଶୂନ୍ୟ ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    ଯଦି(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } ଅନ୍ୟଥା {
        limit=start;
        remainingCapacity=str.getCapacity();
    }
    lastCC=0;
    reorderStart=limit;
}

UBool ReorderingBuffer::resize(int32_t appendLength, UErrorCode &errorCode) {
    int32_t reorderStartIndex = static_cast<int32_t>(reorderStart - start);
    int32_t length = static_cast<int32_t>(limit - start);
    str.releaseBuffer(length);
    int32_t newCapacity=length+appendLength;
    int32_t doubleCapacity=2*str.getCapacity();
    ଯଦି(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    ଯଦି(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    ଯଦି(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    ଯଦି(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    ଯଦି(reorderStart>=codePointStart) {
        ଫେରନ୍ତୁ 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    ଯଦି(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    ଫେରନ୍ତୁ impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
ଶୂନ୍ୟ ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    ପାଇଁ(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    କର {
        *--r=*--q;
    } ଯେତେବେଳେ(codePointLimit!=q);
    writeCodePoint(q, c);
    ଯଦି(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

ସଂରଚନା CanonIterData : ସର୍ବସାଧାରଣ UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    ଶୂନ୍ୟ addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

ଶୂନ୍ୟ
Normalizer2Impl::init(ଅଚଳ int32_t *inIndexes, ଅଚଳ UCPTrie *inTrie,
                      ଅଚଳ uint16_t *inExtraData, ଅଚଳ uint8_t *inSmallFCD) {
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
    extraData=inExtraData;
    smallFCD=inSmallFCD;
}

U_CDECL_BEGIN

ସ୍ଥିର uint32_t U_CALLCONV
segmentStarterMapper(ଅଚଳ ଶୂନ୍ୟ * /*context*/, uint32_t value) {
    ଫେରନ୍ତୁ value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

ଶୂନ୍ୟ
Normalizer2Impl::addLcccChars(UnicodeSet &set) ଅଚଳ {
    UChar32 start = 0, end;
    uint32_t norm16;
    ଯେତେବେଳେ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        ଯଦି (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } ଅନ୍ୟଥା ଯଦି (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            ଯଦି (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

ଶୂନ୍ୟ
Normalizer2Impl::addPropertyStarts(ଅଚଳ USetAdder *sa, UErrorCode & /*errorCode*/) ଅଚଳ {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    ଯେତେବେଳେ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        ଯଦି (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            ଯେତେବେଳେ (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                ଯଦି (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    ପାଇଁ(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_ଗଣନା) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

ଶୂନ୍ୟ
Normalizer2Impl::addCanonIterPropertyStarts(ଅଚଳ USetAdder *sa, UErrorCode &errorCode) ଅଚଳ {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    ଯଦି (!ensureCanonIterData(errorCode)) { ଫେରନ୍ତୁ; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    ଯେତେବେଳେ ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

ଅଚଳ char16_t *
Normalizer2Impl::copy_ତଳ_Prefix_From_Nul_Terminated(ଅଚଳ char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) ଅଚଳ {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ଅଚଳ char16_t *prevSrc=src;
    char16_t c;
    ଯେତେବେଳେ((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    ଯଦି(--src!=prevSrc) {
        ଯଦି(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    ଫେରନ୍ତୁ src;
}

UnicodeString &
Normalizer2Impl::decompose(ଅଚଳ UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ଫେରନ୍ତୁ dest;
    }
    ଅଚଳ char16_t *sArray=src.getBuffer();
    ଯଦି(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        ଫେରନ୍ତୁ dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    ଫେରନ୍ତୁ dest;
}

ଶୂନ୍ୟ
Normalizer2Impl::decompose(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    ReorderingBuffer buffer(*ଏହି, dest);
    ଯଦି(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ଅଚଳ char16_t *
Normalizer2Impl::decompose(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ src;
    }
    UChar32 minNoCP=minDecompNoCP;
    ଯଦି(limit==nullptr) {
        src=copy_ତଳ_Prefix_From_Nul_Terminated(src, minNoCP, buffer, errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ src;
        }
        limit=u_strchr(src, 0);
    }

    ଅଚଳ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ଅଚଳ char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    ପାଇଁ(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        ପାଇଁ(prevSrc=src; src!=limit;) {
            ଯଦି( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ଅନ୍ୟଥା ଯଦି(!U16_IS_LEAD(c)) {
                ଭାଙ୍ଗ;
            } ଅନ୍ୟଥା {
                char16_t c2;
                ଯଦି((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    ଯଦି(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } ଅନ୍ୟଥା {
                        ଭାଙ୍ଗ;
                    }
                } ଅନ୍ୟଥା {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        ଯଦି(src!=prevSrc) {
            ଯଦି(buffer!=nullptr) {
                ଯଦି(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା {
                prevCC=0;
                prevBoundary=src;
            }
        }
        ଯଦି(src==limit) {
            ଭାଙ୍ଗ;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        ଯଦି(buffer!=nullptr) {
            ଯଦି(!decompose(c, norm16, *buffer, errorCode)) {
                ଭାଙ୍ଗ;
            }
        } ଅନ୍ୟଥା {
            ଯଦି(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                ଯଦି(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    ଯଦି(cc<=1) {
                        prevBoundary=src;
                    }
                    ଜାରି;
                }
            }
            ଫେରନ୍ତୁ prevBoundary;  // "no" or cc out of order
        }
    }
    ଫେରନ୍ତୁ src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ଅଚଳ char16_t *
Normalizer2Impl::decomposeShort(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯେତେବେଳେ(src<limit) {
        ଯଦି (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            ଫେରନ୍ତୁ src;
        }
        ଅଚଳ char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        ଯଦି (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            ଫେରନ୍ତୁ prevSrc;
        }
        ଯଦି(!decompose(c, norm16, buffer, errorCode)) {
            ଫେରନ୍ତୁ nullptr;
        }
        ଯଦି (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ଫେରନ୍ତୁ src;
        }
    }
    ଫେରନ୍ତୁ src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) ଅଚଳ {
    // get the decomposition and the lead and trail cc's
    ଯଦି (norm16 >= limitNoNo) {
        ଯଦି (isMaybeYesOrNonZeroCC(norm16)) {
            ଫେରନ୍ତୁ buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } ଅନ୍ୟଥା ଯଦି (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ଯଦି (norm16 < minYesNo) {
        // c does not decompose
        ଫେରନ୍ତୁ buffer.append(c, 0, errorCode);
    } ଅନ୍ୟଥା ଯଦି(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        ଫେରନ୍ତୁ buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    ଅଚଳ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    ଯଦି(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } ଅନ୍ୟଥା {
        leadCC=0;
    }
    ଫେରନ୍ତୁ buffer.append(reinterpret_cast<ଅଚଳ char16_t*>(mapping) + 1, length, ସତ୍ୟ, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ଅଚଳ uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ଅଚଳ uint8_t *src, ଅଚଳ uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) ଅଚଳ {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ଅଚଳ uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    ପାଇଁ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ଅଚଳ uint8_t *fastStart = src;
        ଅଚଳ uint8_t *prevSrc;
        uint16_t norm16 = 0;

        ପାଇଁ (;;) {
            ଯଦି (src == limit) {
                ଯଦି (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                ଫେରନ୍ତୁ src;
            }
            ଯଦି (*src < minNoLead) {
                ++src;
            } ଅନ୍ୟଥା {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                ଯଦି (!isMostDecompYesAndZeroCC(norm16)) {
                    ଭାଙ୍ଗ;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        ଯଦି (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            ଯଦି (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                ଭାଙ୍ଗ;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        ଯଦି (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            ଯଦି (prevCC <= cc || cc == 0) {
                prevCC = cc;
                ଯଦି (cc <= 1) {
                    ଯଦି (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    prevBoundary = src;
                }
                ଜାରି;
            }
        }
        ଯଦି (sink == nullptr) {
            ଫେରନ୍ତୁ prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        ଯଦି (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            ଯଦି (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                ଭାଙ୍ଗ;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*ଏହି, s16, errorCode);
        ଯଦି (U_FAILURE(errorCode)) {
            ଭାଙ୍ଗ;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, ମିଥ୍ୟା /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        ଯଦି (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, ମିଥ୍ୟା /* onlyContiguous */,
                                 buffer, errorCode);
        }
        ଯଦି (U_FAILURE(errorCode)) {
            ଭାଙ୍ଗ;
        }
        ଯଦି ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ଭାଙ୍ଗ;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        ଯଦି (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            ଯଦି (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                ଭାଙ୍ଗ;
            }
        } ଅନ୍ୟଥା {
            ଯଦି (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                ଭାଙ୍ଗ;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    ଫେରନ୍ତୁ src;
}

ଅଚଳ uint8_t *
Normalizer2Impl::decomposeShort(ଅଚଳ uint8_t *src, ଅଚଳ uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯେତେବେଳେ (src < limit) {
        ଅଚଳ uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        ଯଦି (norm16 >= limitNoNo) {
            ଯଦି (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                ଯଦି (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    ଫେରନ୍ତୁ prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                ଯଦି (!buffer.append(c, cc, errorCode)) {
                    ଫେରନ୍ତୁ nullptr;
                }
                ଯଦି (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    ଫେରନ୍ତୁ src;
                }
                ଜାରି;
            } ଅନ୍ୟଥା ଯଦି (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                ଯଦି (stopAt != STOP_AT_LIMIT) {
                    ଫେରନ୍ତୁ prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } ଅନ୍ୟଥା ଯଦି (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            ଫେରନ୍ତୁ prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        ଯଦି (norm16 < minYesNo) {
            ଯଦି (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            ଯଦି (!buffer.append(c, 0, errorCode)) {
                ଫେରନ୍ତୁ nullptr;
            }
        } ଅନ୍ୟଥା ଯଦି (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            ଯଦି (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            ଯଦି (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                ଫେରନ୍ତୁ nullptr;
            }
        } ଅନ୍ୟଥା {
            // The character decomposes, get everything from the variable-length extra data.
            ଅଚଳ uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            ଯଦି (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } ଅନ୍ୟଥା {
                leadCC = 0;
            }
            ଯଦି (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                ଫେରନ୍ତୁ prevSrc;
            }
            ଯଦି (!buffer.append(reinterpret_cast<ଅଚଳ char16_t*>(mapping) + 1, length, ସତ୍ୟ, leadCC, trailCC, errorCode)) {
                ଫେରନ୍ତୁ nullptr;
            }
        }
        ଯଦି ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            ଫେରନ୍ତୁ src;
        }
    }
    ଫେରନ୍ତୁ src;
}

ଅଚଳ char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) ଅଚଳ {
    uint16_t norm16;
    ଯଦି(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ char16_t *decomp = nullptr;
    ଯଦି(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    ଯଦି (norm16 < minYesNo) {
        ଫେରନ୍ତୁ decomp;
    } ଅନ୍ୟଥା ଯଦି(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        ଫେରନ୍ତୁ buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ଅଚଳ uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ଅଚଳ char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) ଅଚଳ {
    uint16_t norm16;
    ଯଦି(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା ଯଦି(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        ଫେରନ୍ତୁ buffer;
    } ଅନ୍ୟଥା ଯଦି(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        ଫେରନ୍ତୁ buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ଅଚଳ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    ଯଦି(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ଅଚଳ uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        ଯଦି(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ char16_t*>(rawMapping) - rm0;
        } ଅନ୍ୟଥା {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<ଅଚଳ char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            ଫେରନ୍ତୁ buffer;
        }
    } ଅନ୍ୟଥା {
        length=mLength;
        ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ char16_t*>(mapping) + 1;
    }
}

ଶୂନ୍ୟ Normalizer2Impl::decomposeAndAppend(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_ମଧ୍ୟ,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) ଅଚଳ {
    buffer.copyReorderableSuffixTo(safe_ମଧ୍ୟ);
    ଯଦି(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        ଫେରନ୍ତୁ;
    }
    // Just merge the strings at the boundary.
    ତାର୍କିକ isFirst = ସତ୍ୟ;
    uint8_t firstCC = 0, prevCC = 0, cc;
    ଅଚଳ char16_t *p = src;
    ଯେତେବେଳେ (p != limit) {
        ଅଚଳ char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ଯଦି ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            ଭାଙ୍ଗ;
        }
        ଯଦି (isFirst) {
            firstCC = cc;
            isFirst = ମିଥ୍ୟା;
        }
        prevCC = cc;
    }
    ଯଦି(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    ଯଦି (buffer.append(src, static_cast<int32_t>(p - src), ମିଥ୍ୟା, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ଅଚଳ {
    ଫେରନ୍ତୁ c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ଅଚଳ {
    ଯଦି (norm16 < minNoNoCompNoMaybeCC) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (norm16 >= limitNoNo) {
        ଫେରନ୍ତୁ norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ଅଚଳ uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ଫେରନ୍ତୁ (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ଅଚଳ {
    ଯଦି (c < minDecompNoCP) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ଅଚଳ {
    ଯଦି(norm16 <= minYesNo || isHangulLVT(norm16)) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (norm16 >= limitNoNo) {
        ଯଦି (isMaybeYesOrNonZeroCC(norm16)) {
            ଫେରନ୍ତୁ norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } ଅନ୍ୟଥା ଯଦି (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            ଫେରନ୍ତୁ (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ଅଚଳ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    ଯଦି(firstUnit>0x1ff) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;  // trailCC>1
    }
    ଯଦି(firstUnit<=0xff) {
        ଫେରନ୍ତୁ ସତ୍ୟ;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ଫେରନ୍ତୁ (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(ଅଚଳ uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    ଯଦି(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        ଯେତେବେଳେ(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        ଯଦି(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            ଯଦି(firstUnit&COMP_1_TRIPLE) {
                ଫେରନ୍ତୁ (static_cast<int32_t>(list[1]) << 16) | list[2];
            } ଅନ୍ୟଥା {
                ଫେରନ୍ତୁ list[1];
            }
        }
    } ଅନ୍ୟଥା {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        ପାଇଁ(;;) {
            ଯଦି(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } ଅନ୍ୟଥା ଯଦି(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                ଯଦି(key2>(secondUnit=list[1])) {
                    ଯଦି(firstUnit&COMP_1_LAST_TUPLE) {
                        ଭାଙ୍ଗ;
                    } ଅନ୍ୟଥା {
                        list+=3;
                    }
                } ଅନ୍ୟଥା ଯଦି(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    ଫେରନ୍ତୁ (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } ଅନ୍ୟଥା {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        }
    }
    ଫେରନ୍ତୁ -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
ଶୂନ୍ୟ Normalizer2Impl::addComposites(ଅଚଳ uint16_t *list, UnicodeSet &set) ଅଚଳ {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    କର {
        firstUnit=*list;
        ଯଦି((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } ଅନ୍ୟଥା {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        ଯଦି((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } ଯେତେବେଳେ((firstUnit&COMP_1_LAST_TUPLE)==0);
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
ଶୂନ୍ୟ Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) ଅଚଳ {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    ଯଦି(p==limit) {
        ଫେରନ୍ତୁ;
    }

    char16_t *starter, *pRemove, *q, *r;
    ଅଚଳ uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=ମିଥ୍ୟା;
    prevCC=0;

    ପାଇଁ(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        ଯଦି( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            ଯଦି(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                ଯଦି(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    ଯଦି(prev<Hangul::JAMO_L_ଗଣନା) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_ଗଣନା+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_ଗଣନା);
                        char16_t t;
                        ଯଦି (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_ଗଣନା) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        ଯେତେବେଳେ(r<limit) {
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
                ଯଦି(p==limit) {
                    ଭାଙ୍ଗ;
                }
                compositionsList=nullptr;
                ଜାରି;
            } ଅନ୍ୟଥା ଯଦି((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                ଯଦି(starterIsSupplementary) {
                    ଯଦି(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } ଅନ୍ୟଥା {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=ମିଥ୍ୟା;
                        q=starter+1;
                        r=q+1;
                        ଯେତେବେଳେ(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } ଅନ୍ୟଥା ଯଦି(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=ସତ୍ୟ;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    ଯେତେବେଳେ(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } ଅନ୍ୟଥା {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                ଯଦି(pRemove<p) {
                    q=pRemove;
                    r=p;
                    ଯେତେବେଳେ(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                ଯଦି(p==limit) {
                    ଭାଙ୍ଗ;
                }
                // Is the composite a starter that combines forward?
                ଯଦି(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } ଅନ୍ୟଥା {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                ଜାରି;
            }
        }

        // no combination this time
        prevCC=cc;
        ଯଦି(p==limit) {
            ଭାଙ୍ଗ;
        }

        // If c did not combine, then check if it is a starter.
        ଯଦି(cc==0) {
            // Found a new starter.
            ଯଦି((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                ଯଦି(U_IS_BMP(c)) {
                    starterIsSupplementary=ମିଥ୍ୟା;
                    starter=p-1;
                } ଅନ୍ୟଥା {
                    starterIsSupplementary=ସତ୍ୟ;
                    starter=p-2;
                }
            }
        } ଅନ୍ୟଥା ଯଦି(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ଅଚଳ {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ଅଚଳ uint16_t *list;
    ଯଦି(isInert(norm16)) {
        ଫେରନ୍ତୁ U_SENTINEL;
    } ଅନ୍ୟଥା ଯଦି(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        ଯଦି(isJamoL(norm16)) {
            ଯଦି (b < Hangul::JAMO_V_BASE) {
                ଫେରନ୍ତୁ U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            ଯଦି(b<Hangul::JAMO_V_ଗଣନା) {
                ଫେରନ୍ତୁ
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_ଗଣନା+b)*
                     Hangul::JAMO_T_ଗଣନା);
            } ଅନ୍ୟଥା {
                ଫେରନ୍ତୁ U_SENTINEL;
            }
        } ଅନ୍ୟଥା ଯଦି(isHangulLV(norm16)) {
            ଯଦି (b <= Hangul::JAMO_T_BASE) {
               ଫେରନ୍ତୁ U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            ଯଦି(b<Hangul::JAMO_T_ଗଣନା) {  // not b==0!
                ଫେରନ୍ତୁ a+b;
            } ଅନ୍ୟଥା {
                ଫେରନ୍ତୁ U_SENTINEL;
            }
        } ଅନ୍ୟଥା {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            ଯଦି(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } ଅନ୍ୟଥା ଯଦି(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        ଫେରନ୍ତୁ U_SENTINEL;
    } ଅନ୍ୟଥା {
        list=getDataForMaybe(norm16);
        ଯଦି(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    ଯଦି(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        ଫେରନ୍ତୁ U_SENTINEL;
    }
#ଯଦି U_SIGNED_ଡାହାଣ_SHIFT_IS_ARITHMETIC
    ଫେରନ୍ତୁ combine(list, b)>>1;
#ଅନ୍ୟଥା
    int32_t compositeAndFwd=combine(list, b);
    ଫେରନ୍ତୁ compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) ଅଚଳ {
    ଅଚଳ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ଯଦି(limit==nullptr) {
        src=copy_ତଳ_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        limit=u_strchr(src, 0);
        ଯଦି (prevBoundary != src) {
            ଯଦି (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ଅନ୍ୟଥା {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    ପାଇଁ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ଅଚଳ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        ପାଇଁ (;;) {
            ଯଦି (src == limit) {
                ଯଦି (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
            ଯଦି( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ଅନ୍ୟଥା {
                prevSrc = src++;
                ଯଦି(!U16_IS_LEAD(c)) {
                    ଭାଙ୍ଗ;
                } ଅନ୍ୟଥା {
                    char16_t c2;
                    ଯଦି(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ଯଦି(!isCompYesAndZeroCC(norm16)) {
                            ଭାଙ୍ଗ;
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
        ଯଦି (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ଯଦି (!doCompose) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ଯଦି (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ଯଦି (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ଯଦି (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    ଯଦି(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    prevBoundary = src;
                    ଜାରି;
                }
            } ଅନ୍ୟଥା ଯଦି (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ଯଦି (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ଯଦି (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    ଅଚଳ char16_t *mapping = reinterpret_cast<ଅଚଳ char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    ଯଦି(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    prevBoundary = src;
                    ଜାରି;
                }
            } ଅନ୍ୟଥା ଯଦି (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ଯଦି (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ଯଦି (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    prevBoundary = src;
                    ଜାରି;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ଅନ୍ୟଥା ଯଦି (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            ଯଦି(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                ଯଦି(l<Hangul::JAMO_L_ଗଣନା) {
                    ଯଦି (!doCompose) {
                        ଫେରନ୍ତୁ ମିଥ୍ୟା;
                    }
                    int32_t t;
                    ଯଦି (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_ଗଣନା) {
                        // The next character is a Jamo T.
                        ++src;
                    } ଅନ୍ୟଥା ଯଦି (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } ଅନ୍ୟଥା {
                        t = -1;
                    }
                    ଯଦି (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_ଗଣନା + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_ଗଣନା + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        ଯଦି (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            ଭାଙ୍ଗ;
                        }
                        ଯଦି (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            ଭାଙ୍ଗ;
                        }
                        prevBoundary = src;
                        ଜାରି;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ଅନ୍ୟଥା ଯଦି (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ଯଦି (!doCompose) {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                ଯଦି (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    ଭାଙ୍ଗ;
                }
                ଯଦି (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    ଭାଙ୍ଗ;
                }
                prevBoundary = src;
                ଜାରି;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ଅନ୍ୟଥା ଯଦି (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            ଯଦି (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ଯଦି (!doCompose) {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            } ଅନ୍ୟଥା {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ଅଚଳ char16_t *nextSrc;
                uint16_t n16;
                ପାଇଁ (;;) {
                    ଯଦି (src == limit) {
                        ଯଦି (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        ଫେରନ୍ତୁ ସତ୍ୟ;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    ଯଦି (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        ଯଦି (prevCC > cc) {
                            ଯଦି (!doCompose) {
                                ଫେରନ୍ତୁ ମିଥ୍ୟା;
                            }
                            ଭାଙ୍ଗ;
                        }
                    } ଅନ୍ୟଥା {
                        ଭାଙ୍ଗ;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ଯଦି (norm16HasCompBoundaryBefore(n16)) {
                    ଯଦି (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    ଜାରି;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ଯଦି (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ଅଚଳ char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            ଯଦି (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ଯଦି (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            ଭାଙ୍ଗ;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, ମିଥ୍ୟା /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, ସତ୍ୟ /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        ଯଦି (U_FAILURE(errorCode)) {
            ଭାଙ୍ଗ;
        }
        ଯଦି ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        ଯଦି(!doCompose) {
            ଯଦି(!buffer.equals(prevSrc, src)) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            buffer.remove();
        }
        prevBoundary=src;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ଅଚଳ char16_t *
Normalizer2Impl::composeQuickCheck(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) ଅଚଳ {
    ଅଚଳ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ଯଦି(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_ତଳ_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        ଯଦି (prevBoundary != src) {
            ଯଦି (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ଅନ୍ୟଥା {
                prevBoundary = --src;
            }
        }
    }

    ପାଇଁ(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ଅଚଳ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        ପାଇଁ (;;) {
            ଯଦି(src==limit) {
                ଫେରନ୍ତୁ src;
            }
            ଯଦି( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ଅନ୍ୟଥା {
                prevSrc = src++;
                ଯଦି(!U16_IS_LEAD(c)) {
                    ଭାଙ୍ଗ;
                } ଅନ୍ୟଥା {
                    char16_t c2;
                    ଯଦି(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ଯଦି(!isCompYesAndZeroCC(norm16)) {
                            ଭାଙ୍ଗ;
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
        ଯଦି (prevBoundary != prevSrc) {
            ଯଦି (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } ଅନ୍ୟଥା {
                ଅଚଳ char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                ଯଦି (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } ଅନ୍ୟଥା {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        ଯଦି (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            ଯଦି (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } ଅନ୍ୟଥା {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ଅଚଳ char16_t *nextSrc;
                ପାଇଁ (;;) {
                    ଯଦି (norm16 < MIN_YES_YES_WITH_CC) {
                        ଯଦି (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } ଅନ୍ୟଥା {
                            ଫେରନ୍ତୁ prevBoundary;
                        }
                    }
                    ଯଦି (src == limit) {
                        ଫେରନ୍ତୁ src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    ଯଦି (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        ଯଦି (!(prevCC <= cc || cc == 0)) {
                            ଭାଙ୍ଗ;
                        }
                    } ଅନ୍ୟଥା {
                        ଭାଙ୍ଗ;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                ଯଦି (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    ଜାରି;
                }
            }
        }
        ଯଦି(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        ଫେରନ୍ତୁ prevBoundary;
    }
}

ଶୂନ୍ୟ Normalizer2Impl::composeAndAppend(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_ମଧ୍ୟ,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(!buffer.isEmpty()) {
        ଅଚଳ char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        ଯଦି(src!=firstStarterInSrc) {
            ଅଚଳ char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString ମଧ୍ୟ(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_ମଧ୍ୟ=ମଧ୍ୟ;
            ମଧ୍ୟ.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            ଅଚଳ char16_t *ମଧ୍ୟ_Start=ମଧ୍ୟ.getBuffer();
            compose(ମଧ୍ୟ_Start, ମଧ୍ୟ_Start+ମଧ୍ୟ.length(), onlyContiguous,
                    ସତ୍ୟ, buffer, errorCode);
            ଯଦି(U_FAILURE(errorCode)) {
                ଫେରନ୍ତୁ;
            }
            src=firstStarterInSrc;
        }
    }
    ଯଦି(doCompose) {
        compose(src, limit, onlyContiguous, ସତ୍ୟ, buffer, errorCode);
    } ଅନ୍ୟଥା {
        ଯଦି(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ଅଚଳ uint8_t *src, ଅଚଳ uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) ଅଚଳ {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ଅଚଳ uint8_t *prevBoundary = src;

    ପାଇଁ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ଅଚଳ uint8_t *prevSrc;
        uint16_t norm16 = 0;
        ପାଇଁ (;;) {
            ଯଦି (src == limit) {
                ଯଦି (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
            ଯଦି (*src < minNoMaybeLead) {
                ++src;
            } ଅନ୍ୟଥା {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                ଯଦି (!isCompYesAndZeroCC(norm16)) {
                    ଭାଙ୍ଗ;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        ଯଦି (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ଯଦି (sink == nullptr) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ଯଦି (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ଯଦି (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ଯଦି (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    ଜାରି;
                }
            } ଅନ୍ୟଥା ଯଦି (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ଯଦି (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ଯଦି (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    ଅଚଳ uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    ଯଦି (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ଅଚଳ char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    prevBoundary = src;
                    ଜାରି;
                }
            } ଅନ୍ୟଥା ଯଦି (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ଯଦି (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ଯଦି (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        ଭାଙ୍ଗ;
                    }
                    ଯଦି (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    ଜାରି;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ଅନ୍ୟଥା ଯଦି (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            ଯଦି (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                ଯଦି (static_cast<uint32_t>(l) < Hangul::JAMO_L_ଗଣନା) {
                    ଯଦି (sink == nullptr) {
                        ଫେରନ୍ତୁ ମିଥ୍ୟା;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    ଯଦି (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } ଅନ୍ୟଥା ଯଦି (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    ଯଦି (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_ଗଣନା + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_ଗଣନା + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        ଯଦି (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            ଭାଙ୍ଗ;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        ଜାରି;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ଅନ୍ୟଥା ଯଦି (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ଯଦି (sink == nullptr) {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                ଯଦି (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    ଭାଙ୍ଗ;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                ଜାରି;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ଅନ୍ୟଥା ଯଦି (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            ଯଦି (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ଯଦି (sink == nullptr) {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            } ଅନ୍ୟଥା {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ଅଚଳ uint8_t *nextSrc;
                uint16_t n16;
                ପାଇଁ (;;) {
                    ଯଦି (src == limit) {
                        ଯଦି (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        ଫେରନ୍ତୁ ସତ୍ୟ;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    ଯଦି (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        ଯଦି (prevCC > cc) {
                            ଯଦି (sink == nullptr) {
                                ଫେରନ୍ତୁ ମିଥ୍ୟା;
                            }
                            ଭାଙ୍ଗ;
                        }
                    } ଅନ୍ୟଥା {
                        ଭାଙ୍ଗ;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ଯଦି (norm16HasCompBoundaryBefore(n16)) {
                    ଯଦି (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    ଜାରି;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ଯଦି (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ଅଚଳ uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            ଯଦି (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*ଏହି, s16, errorCode);
        ଯଦି (U_FAILURE(errorCode)) {
            ଭାଙ୍ଗ;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        ଯଦି (U_FAILURE(errorCode)) {
            ଭାଙ୍ଗ;
        }
        ଯଦି ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
        recompose(buffer, 0, onlyContiguous);
        ଯଦି (!buffer.equals(prevSrc, src)) {
            ଯଦି (sink == nullptr) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            ଯଦି (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                ଭାଙ୍ଗ;
            }
            ଯଦି (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                ଭାଙ୍ଗ;
            }
            prevBoundary = src;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit) ଅଚଳ {
    ଯଦି (src == limit || *src < minCompNoMaybeCP) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    ଫେରନ୍ତୁ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ଅଚଳ uint8_t *src, ଅଚଳ uint8_t *limit) ଅଚଳ {
    ଯଦି (src == limit) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    ଫେରନ୍ତୁ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ଅଚଳ char16_t *start, ଅଚଳ char16_t *p,
                                            UBool onlyContiguous) ଅଚଳ {
    ଯଦି (start == p) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    ଫେରନ୍ତୁ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ଅଚଳ uint8_t *start, ଅଚଳ uint8_t *p,
                                            UBool onlyContiguous) ଅଚଳ {
    ଯଦି (start == p) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    ଫେରନ୍ତୁ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ଅଚଳ char16_t *Normalizer2Impl::findPreviousCompBoundary(ଅଚଳ char16_t *start, ଅଚଳ char16_t *p,
                                                       UBool onlyContiguous) ଅଚଳ {
    ଯେତେବେଳେ (p != start) {
        ଅଚଳ char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        ଯଦି (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ଫେରନ୍ତୁ codePointLimit;
        }
        ଯଦି (hasCompBoundaryBefore(c, norm16)) {
            ଫେରନ୍ତୁ p;
        }
    }
    ଫେରନ୍ତୁ p;
}

ଅଚଳ char16_t *Normalizer2Impl::findNextCompBoundary(ଅଚଳ char16_t *p, ଅଚଳ char16_t *limit,
                                                   UBool onlyContiguous) ଅଚଳ {
    ଯେତେବେଳେ (p != limit) {
        ଅଚଳ char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ଯଦି (hasCompBoundaryBefore(c, norm16)) {
            ଫେରନ୍ତୁ codePointStart;
        }
        ଯଦି (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ଫେରନ୍ତୁ p;
        }
    }
    ଫେରନ୍ତୁ p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ଅଚଳ char16_t *start, ଅଚଳ char16_t *p) ଅଚଳ {
    ଯଦି (start == p) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    ଫେରନ୍ତୁ static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ଅଚଳ uint8_t *start, ଅଚଳ uint8_t *p) ଅଚଳ {
    ଯଦି (start == p) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    ଫେରନ୍ତୁ static_cast<uint8_t>(getFCD16(c));
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
#ଯଦି (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) ଅଚଳ {
    uint16_t norm16=getNorm16(c);
    ଯଦି (norm16 >= limitNoNo) {
        ଯଦି(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            ଫେରନ୍ତୁ norm16|(norm16<<8);
        } ଅନ୍ୟଥା ଯଦି(norm16>=minMaybeYes) {
            ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା ଯଦି(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            ଯଦି (deltaTrailCC <= DELTA_TCCC_1) {
                ଫେରନ୍ତୁ deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ଯଦି(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        ଫେରନ୍ତୁ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ଅଚଳ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    ଯଦି(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    ଫେରନ୍ତୁ norm16;
}
#ଯଦି (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ଅଚଳ {
    U_ASSERT(norm16 >= minMaybeNo);
    ଯଦି (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        ଫେରନ୍ତୁ norm16 | (norm16<<8);
    } ଅନ୍ୟଥା ଯଦି (norm16 >= minMaybeYes) {
        ଫେରନ୍ତୁ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ଅଚଳ uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    ଫେରନ୍ତୁ firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ଅଚଳ char16_t *
Normalizer2Impl::makeFCD(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) ଅଚଳ {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ଅଚଳ char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    ଯଦି(limit==nullptr) {
        src=copy_ତଳ_Prefix_From_Nul_Terminated(src, minLcccCP, buffer, errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ src;
        }
        ଯଦି(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            ଯଦି(prevFCD16>1) {
                --prevBoundary;
            }
        }
        limit=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    ଅଚଳ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    ପାଇଁ(;;) {
        // count code units with lccc==0
        ପାଇଁ(prevSrc=src; src!=limit;) {
            ଯଦି((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } ଅନ୍ୟଥା ଯଦି(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } ଅନ୍ୟଥା {
                ଯଦି(U16_IS_LEAD(c)) {
                    char16_t c2;
                    ଯଦି((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                ଯଦି((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } ଅନ୍ୟଥା {
                    ଭାଙ୍ଗ;
                }
            }
        }
        // copy these code units all at once
        ଯଦି(src!=prevSrc) {
            ଯଦି(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(src==limit) {
                ଭାଙ୍ଗ;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            ଯଦି(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                ଯଦି(prev<minDecompNoCP) {
                    prevFCD16=0;
                } ଅନ୍ୟଥା {
                    prevFCD16=getFCD16FromNormData(prev);
                    ଯଦି(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } ଅନ୍ୟଥା {
                ଅଚଳ char16_t *p=src-1;
                ଯଦି(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                ଯଦି(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } ଅନ୍ୟଥା ଯଦି(src==limit) {
            ଭାଙ୍ଗ;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        ଯଦି((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            ଯଦି((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            ଯଦି(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                ଭାଙ୍ଗ;
            }
            prevFCD16=fcd16;
            ଜାରି;
        } ଅନ୍ୟଥା ଯଦି(buffer==nullptr) {
            ଫେରନ୍ତୁ prevBoundary;  // quick check "no"
        } ଅନ୍ୟଥା {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            buffer->removeSuffix(static_cast<int32_t>(prevSrc - prevBoundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=findNextFCDBoundary(src, limit);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(prevBoundary, src, ମିଥ୍ୟା, ମିଥ୍ୟା, *buffer, errorCode);
            ଯଦି (U_FAILURE(errorCode)) {
                ଭାଙ୍ଗ;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    ଫେରନ୍ତୁ src;
}

ଶୂନ୍ୟ Normalizer2Impl::makeFCDAndAppend(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_ମଧ୍ୟ,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(!buffer.isEmpty()) {
        ଅଚଳ char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        ଯଦି(src!=firstBoundaryInSrc) {
            ଅଚଳ char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString ମଧ୍ୟ(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_ମଧ୍ୟ=ମଧ୍ୟ;
            ମଧ୍ୟ.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            ଅଚଳ char16_t *ମଧ୍ୟ_Start=ମଧ୍ୟ.getBuffer();
            makeFCD(ମଧ୍ୟ_Start, ମଧ୍ୟ_Start+ମଧ୍ୟ.length(), &buffer, errorCode);
            ଯଦି(U_FAILURE(errorCode)) {
                ଫେରନ୍ତୁ;
            }
            src=firstBoundaryInSrc;
        }
    }
    ଯଦି(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } ଅନ୍ୟଥା {
        ଯଦି(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

ଅଚଳ char16_t *Normalizer2Impl::findPreviousFCDBoundary(ଅଚଳ char16_t *start, ଅଚଳ char16_t *p) ଅଚଳ {
    ଯେତେବେଳେ(start<p) {
        ଅଚଳ char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        ଯଦି (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            ଫେରନ୍ତୁ codePointLimit;
        }
        ଯଦି (norm16HasDecompBoundaryBefore(norm16)) {
            ଫେରନ୍ତୁ p;
        }
    }
    ଫେରନ୍ତୁ p;
}

ଅଚଳ char16_t *Normalizer2Impl::findNextFCDBoundary(ଅଚଳ char16_t *p, ଅଚଳ char16_t *limit) ଅଚଳ {
    ଯେତେବେଳେ(p<limit) {
        ଅଚଳ char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ଯଦି (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            ଫେରନ୍ତୁ codePointStart;
        }
        ଯଦି (norm16HasDecompBoundaryAfter(norm16)) {
            ଫେରନ୍ତୁ p;
        }
    }
    ଫେରନ୍ତୁ p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

ଶୂନ୍ୟ CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_ମୂଲ୍ୟ = umutablecptrie_get(mutableTrie, decompLead);
    ଯଦି((canon_ମୂଲ୍ୟ&(CANON_HAS_SET|CANON_ମୂଲ୍ୟ_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_ମୂଲ୍ୟ|origin, &errorCode);
    } ଅନ୍ୟଥା {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        ଯଦି((canon_ମୂଲ୍ୟ&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(ନୂତନ UnicodeSet, errorCode);
            set=lpSet.getAlias();
            ଯଦି(U_FAILURE(errorCode)) {
                ଫେରନ୍ତୁ;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_ମୂଲ୍ୟ & CANON_ମୂଲ୍ୟ_MASK);
            canon_ମୂଲ୍ୟ = (canon_ମୂଲ୍ୟ & ~CANON_ମୂଲ୍ୟ_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_ମୂଲ୍ୟ, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            ଯଦି (U_FAILURE(errorCode)) {
                ଫେରନ୍ତୁ;
            }
            ଯଦି(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } ଅନ୍ୟଥା {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_ମୂଲ୍ୟ & CANON_ମୂଲ୍ୟ_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
ଶ୍ରେଣୀ InitCanonIterData {
ସର୍ବସାଧାରଣ:
    ସ୍ଥିର ଶୂନ୍ୟ doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

ଶୂନ୍ୟ InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = ନୂତନ CanonIterData(errorCode);
    ଯଦି (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ଯଦି (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        ଯେତେବେଳେ ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            ଯଦି (value != Normalizer2Impl::INERT) {
                impl->makeCanonIterDataFromNorm16(start, end, value, *impl->fCanonIterData, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->fCanonIterData->mutableTrie, "CanonIterData");
#endif
        impl->fCanonIterData->trie = umutablecptrie_buildImmutable(
            impl->fCanonIterData->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_ମୂଲ୍ୟ_BITS_32, &errorCode);
        umutablecptrie_close(impl->fCanonIterData->mutableTrie);
        impl->fCanonIterData->mutableTrie = nullptr;
    }
    ଯଦି (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

ଶୂନ୍ୟ Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, ଅଚଳ uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        ଫେରନ୍ତୁ;
    }
    ପାଇଁ(UChar32 c=start; c<=end; ++c) {
        uint32_t old_ମୂଲ୍ୟ = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t new_ମୂଲ୍ୟ=old_ମୂଲ୍ୟ;
        ଯଦି(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_ମୂଲ୍ୟ|=CANON_NOT_SEGMENT_STARTER;
            ଯଦି(norm16<MIN_NORMAL_MAYBE_YES) {
                new_ମୂଲ୍ୟ|=CANON_HAS_COMPOSITIONS;
            }
        } ଅନ୍ୟଥା ଯଦି(norm16<minYesNo) {
            new_ମୂଲ୍ୟ|=CANON_HAS_COMPOSITIONS;
        } ଅନ୍ୟଥା {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            ଯଦି (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            ଯଦି (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ଅଚଳ uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                ଯଦି((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    ଯଦି(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_ମୂଲ୍ୟ|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                ଯଦି(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    ଯଦି(norm16_2>=minNoNo) {
                        ଯେତେବେଳେ(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2_ମୂଲ୍ୟ = umutablecptrie_get(newData.mutableTrie, c2);
                            ଯଦି((c2_ମୂଲ୍ୟ&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2_ମୂଲ୍ୟ|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } ଅନ୍ୟଥା {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        ଯଦି(new_ମୂଲ୍ୟ!=old_ମୂଲ୍ୟ) {
            umutablecptrie_set(newData.mutableTrie, c, new_ମୂଲ୍ୟ, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) ଅଚଳ {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(ଏହି);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_ମୂଲ୍ୟ(UChar32 c) ଅଚଳ {
    ଫେରନ୍ତୁ static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

ଅଚଳ UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) ଅଚଳ {
    ଫେରନ୍ତୁ *static_cast<ଅଚଳ UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ଅଚଳ {
    ଫେରନ୍ତୁ get_Canon_ମୂଲ୍ୟ(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) ଅଚଳ {
    int32_t canon_ମୂଲ୍ୟ=get_Canon_ମୂଲ୍ୟ(c)&~CANON_NOT_SEGMENT_STARTER;
    ଯଦି(canon_ମୂଲ୍ୟ==0) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    set.clear();
    int32_t value=canon_ମୂଲ୍ୟ&CANON_ମୂଲ୍ୟ_MASK;
    ଯଦି((canon_ମୂଲ୍ୟ&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } ଅନ୍ୟଥା ଯଦି(value!=0) {
        set.add(value);
    }
    ଯଦି((canon_ମୂଲ୍ୟ&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        ଯଦି(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_ଗଣନା);
            set.add(syllable, syllable+Hangul::JAMO_VT_ଗଣନା-1);
        } ଅନ୍ୟଥା {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(ଅଚଳ UDataSwapper *ds,
            ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
            UErrorCode *pErrorCode) {
    ଅଚଳ UDataInfo *pInfo;
    int32_t headerSize;

    ଅଚଳ uint8_t *inBytes;
    uint8_t *outBytes;

    ଅଚଳ int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    /* check data format and format version */
    pInfo=(ଅଚଳ UDataInfo *)((ଅଚଳ ଅକ୍ଷର *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    ଯଦି(!(
        pInfo->dataFormat[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->dataFormat[1]==0x72 &&
        pInfo->dataFormat[2]==0x6d &&
        pInfo->dataFormat[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    inBytes=(ଅଚଳ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(ଅଚଳ int32_t *)inBytes;
    int32_t minIndexesLength;
    ଯଦି(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } ଅନ୍ୟଥା ଯଦି(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } ଅନ୍ୟଥା ଯଦି(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } ଅନ୍ୟଥା {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    ଯଦି(length>=0) {
        length-=headerSize;
        ଯଦି(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
    }

    /* read the first few indexes */
    ପାଇଁ(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    ଯଦି(length>=0) {
        ଯଦି(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        /* copy the data for inaccessible bytes */
        ଯଦି(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset=0;

        /* swap the int32_t indexes[] */
        nextOffset=indexes[Normalizer2Impl::IX_NORM_TRIE_OFFSET];
        ds->swapArray32(ds, inBytes, nextOffset-offset, outBytes, pErrorCode);
        offset=nextOffset;

        /* swap the trie */
        nextOffset=indexes[Normalizer2Impl::IX_EXTRA_DATA_OFFSET];
        utrie_swapAnyVersion(ds, inBytes+offset, nextOffset-offset, outBytes+offset, pErrorCode);
        offset=nextOffset;

        /* swap the uint16_t extraData[] */
        nextOffset=indexes[Normalizer2Impl::IX_SMALL_FCD_OFFSET];
        ds->swapArray16(ds, inBytes+offset, nextOffset-offset, outBytes+offset, pErrorCode);
        offset=nextOffset;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        nextOffset=indexes[Normalizer2Impl::IX_SMALL_FCD_OFFSET+1];
        offset=nextOffset;

        U_ASSERT(offset==size);
    }

    ଫେରନ୍ତୁ headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
