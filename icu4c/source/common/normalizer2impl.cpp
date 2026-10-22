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

#ಯದಿ !UCONFIG_NO_NORMALIZATION

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
ಅನ್ತಃಸ್ಥ uint8_t leadByteForCP(UChar32 c) {
    ಯದಿ (c <= 0x7f) {
        ಪ್ರತ್ಯಾಗಮ static_cast<uint8_t>(c);
    } ಅನ್ಯಥಾ ಯದಿ (c <= 0x7ff) {
        ಪ್ರತ್ಯಾಗಮ static_cast<uint8_t>(0xc0 + (c >> 6));
    } ಅನ್ಯಥಾ {
        // Should not occur because ccc(U+0300)!=0.
        ಪ್ರತ್ಯಾಗಮ 0xe0;
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
UChar32 codePointFromValidUTF8(ಧ್ರುವ uint8_t *cpStart, ಧ್ರುವ uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    ವಿಕಲ್ಪ(cpLimit-cpStart) {
    ಪಕ್ಷ 1:
        ಪ್ರತ್ಯಾಗಮ c;
    ಪಕ್ಷ 2:
        ಪ್ರತ್ಯಾಗಮ ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    ಪಕ್ಷ 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        ಪ್ರತ್ಯಾಗಮ static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    ಪಕ್ಷ 4:
        ಪ್ರತ್ಯಾಗಮ ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    ಸ್ವಭಾವ:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(ಧ್ರುವ uint8_t *start, ಧ್ರುವ uint8_t *p) {
    ಯದಿ ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        ಯದಿ (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            ಪ್ರತ್ಯಾಗಮ ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(ಧ್ರುವ uint8_t *src, ಧ್ರುವ uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    ಯದಿ ((limit - src) >= 3 && *src == 0xe1) {
        ಯದಿ (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            ಯದಿ (0xa8 <= t && t <= 0xbf) {
                ಪ್ರತ್ಯಾಗಮ t - 0xa7;
            }
        } ಅನ್ಯಥಾ ಯದಿ (src[1] == 0x87) {
            uint8_t t = src[2];
            ಯದಿ (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                ಪ್ರತ್ಯಾಗಮ t - (0xa7 - 0x40);
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

ಶೂನ್ಯ
appendCodePointDelta(ಧ್ರುವ uint8_t *cpStart, ಧ್ರುವ uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    ವರ್ಣ buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    ಯದಿ (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } ಅನ್ಯಥಾ {
        int32_t trail = *(cpLimit-1) + delta;
        ಯದಿ (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            ಕುರು { buffer[length++] = *cpStart++; } ಯಾವತ್ (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } ಅನ್ಯಥಾ {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    ಯದಿ (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(ಧ್ರುವ Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    ಯದಿ (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    ಯದಿ(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    ಯದಿ(start==limit) {
        lastCC=0;
    } ಅನ್ಯಥಾ {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        ಯದಿ(lastCC>1) {
            ಯಾವತ್(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool ReorderingBuffer::equals(ಧ್ರುವ char16_t *otherStart, ಧ್ರುವ char16_t *otherLimit) ಧ್ರುವ {
    int32_t length = static_cast<int32_t>(limit - start);
    ಪ್ರತ್ಯಾಗಮ
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(ಧ್ರುವ uint8_t *otherStart, ಧ್ರುವ uint8_t *otherLimit) ಧ್ರುವ {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    ಯದಿ (otherLength < length || (otherLength / 3) > length) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    ಪ್ರತಿ (int32_t i = 0, j = 0;;) {
        ಯದಿ (i >= length) {
            ಪ್ರತ್ಯಾಗಮ j >= otherLength;
        } ಅನ್ಯಥಾ ಯದಿ (j >= otherLength) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        ಯದಿ (c != other) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    ಯದಿ(remainingCapacity<2 && !resize(2, errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        ಯದಿ(cc<=1) {
            reorderStart=limit;
        }
    } ಅನ್ಯಥಾ {
        insert(c, cc);
    }
    remainingCapacity-=2;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool ReorderingBuffer::append(ಧ್ರುವ char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    ಯದಿ(length==0) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(remainingCapacity<length && !resize(length, errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    remainingCapacity-=length;
    ಯದಿ(lastCC<=leadCC || leadCC==0) {
        ಯದಿ(trailCC<=1) {
            reorderStart=limit+length;
        } ಅನ್ಯಥಾ ಯದಿ(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        ಧ್ರುವ char16_t *sLimit=s+length;
        ಕುರು { *limit++=*s++; } ಯಾವತ್(s!=sLimit);
        lastCC=trailCC;
    } ಅನ್ಯಥಾ {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        ಯಾವತ್(i<length) {
            U16_NEXT(s, i, length, c);
            ಯದಿ(i<length) {
                ಯದಿ (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } ಅನ್ಯಥಾ {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } ಅನ್ಯಥಾ {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    ಯದಿ(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    remainingCapacity-=cpLength;
    ಯದಿ(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } ಅನ್ಯಥಾ {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool ReorderingBuffer::appendZeroCC(ಧ್ರುವ char16_t *s, ಧ್ರುವ char16_t *sLimit, UErrorCode &errorCode) {
    ಯದಿ(s==sLimit) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    ಯದಿ(remainingCapacity<length && !resize(length, errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ ReorderingBuffer::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

ಶೂನ್ಯ ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    ಯದಿ(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } ಅನ್ಯಥಾ {
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
    ಯದಿ(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    ಯದಿ(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    ಯದಿ(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    ಯದಿ(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    ಯದಿ(reorderStart>=codePointStart) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    ಯದಿ(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    ಪ್ರತ್ಯಾಗಮ impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
ಶೂನ್ಯ ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    ಪ್ರತಿ(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    ಕುರು {
        *--r=*--q;
    } ಯಾವತ್(codePointLimit!=q);
    writeCodePoint(q, c);
    ಯದಿ(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

ಸಂರಚನಾ CanonIterData : ಸಾರ್ವಜನಿಕ UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    ಶೂನ್ಯ addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

ಶೂನ್ಯ
Normalizer2Impl::init(ಧ್ರುವ int32_t *inIndexes, ಧ್ರುವ UCPTrie *inTrie,
                      ಧ್ರುವ uint16_t *inExtraData, ಧ್ರುವ uint8_t *inSmallFCD) {
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

ಸ್ಥಿರ uint32_t U_CALLCONV
segmentStarterMapper(ಧ್ರುವ ಶೂನ್ಯ * /*context*/, uint32_t value) {
    ಪ್ರತ್ಯಾಗಮ value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

ಶೂನ್ಯ
Normalizer2Impl::addLcccChars(UnicodeSet &set) ಧ್ರುವ {
    UChar32 start = 0, end;
    uint32_t norm16;
    ಯಾವತ್ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        ಯದಿ (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } ಅನ್ಯಥಾ ಯದಿ (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            ಯದಿ (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

ಶೂನ್ಯ
Normalizer2Impl::addPropertyStarts(ಧ್ರುವ USetAdder *sa, UErrorCode & /*errorCode*/) ಧ್ರುವ {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    ಯಾವತ್ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        ಯದಿ (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            ಯಾವತ್ (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                ಯದಿ (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    ಪ್ರತಿ(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_ಎಣಿಕೆ) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

ಶೂನ್ಯ
Normalizer2Impl::addCanonIterPropertyStarts(ಧ್ರುವ USetAdder *sa, UErrorCode &errorCode) ಧ್ರುವ {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    ಯದಿ (!ensureCanonIterData(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    ಯಾವತ್ ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

ಧ್ರುವ char16_t *
Normalizer2Impl::copy_ಕೆಳಗಿನ_Prefix_From_Nul_Terminated(ಧ್ರುವ char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) ಧ್ರುವ {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ಧ್ರುವ char16_t *prevSrc=src;
    char16_t c;
    ಯಾವತ್((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    ಯದಿ(--src!=prevSrc) {
        ಯದಿ(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    ಪ್ರತ್ಯಾಗಮ src;
}

UnicodeString &
Normalizer2Impl::decompose(ಧ್ರುವ UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    ಧ್ರುವ char16_t *sArray=src.getBuffer();
    ಯದಿ(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    ಪ್ರತ್ಯಾಗಮ dest;
}

ಶೂನ್ಯ
Normalizer2Impl::decompose(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    ReorderingBuffer buffer(*ಏತತ್, dest);
    ಯದಿ(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ಧ್ರುವ char16_t *
Normalizer2Impl::decompose(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ src;
    }
    UChar32 minNoCP=minDecompNoCP;
    ಯದಿ(limit==nullptr) {
        src=copy_ಕೆಳಗಿನ_Prefix_From_Nul_Terminated(src, minNoCP, buffer, errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ src;
        }
        limit=u_strchr(src, 0);
    }

    ಧ್ರುವ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ಧ್ರುವ char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    ಪ್ರತಿ(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        ಪ್ರತಿ(prevSrc=src; src!=limit;) {
            ಯದಿ( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ಅನ್ಯಥಾ ಯದಿ(!U16_IS_LEAD(c)) {
                ಭಙ್ಗ;
            } ಅನ್ಯಥಾ {
                char16_t c2;
                ಯದಿ((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    ಯದಿ(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } ಅನ್ಯಥಾ {
                        ಭಙ್ಗ;
                    }
                } ಅನ್ಯಥಾ {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        ಯದಿ(src!=prevSrc) {
            ಯದಿ(buffer!=nullptr) {
                ಯದಿ(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ {
                prevCC=0;
                prevBoundary=src;
            }
        }
        ಯದಿ(src==limit) {
            ಭಙ್ಗ;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        ಯದಿ(buffer!=nullptr) {
            ಯದಿ(!decompose(c, norm16, *buffer, errorCode)) {
                ಭಙ್ಗ;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                ಯದಿ(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    ಯದಿ(cc<=1) {
                        prevBoundary=src;
                    }
                    ಅನುವರ್ತ;
                }
            }
            ಪ್ರತ್ಯಾಗಮ prevBoundary;  // "no" or cc out of order
        }
    }
    ಪ್ರತ್ಯಾಗಮ src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ಧ್ರುವ char16_t *
Normalizer2Impl::decomposeShort(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯಾವತ್(src<limit) {
        ಯದಿ (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            ಪ್ರತ್ಯಾಗಮ src;
        }
        ಧ್ರುವ char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        ಯದಿ (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            ಪ್ರತ್ಯಾಗಮ prevSrc;
        }
        ಯದಿ(!decompose(c, norm16, buffer, errorCode)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಯದಿ (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ಪ್ರತ್ಯಾಗಮ src;
        }
    }
    ಪ್ರತ್ಯಾಗಮ src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) ಧ್ರುವ {
    // get the decomposition and the lead and trail cc's
    ಯದಿ (norm16 >= limitNoNo) {
        ಯದಿ (isMaybeYesOrNonZeroCC(norm16)) {
            ಪ್ರತ್ಯಾಗಮ buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } ಅನ್ಯಥಾ ಯದಿ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ಯದಿ (norm16 < minYesNo) {
        // c does not decompose
        ಪ್ರತ್ಯಾಗಮ buffer.append(c, 0, errorCode);
    } ಅನ್ಯಥಾ ಯದಿ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        ಪ್ರತ್ಯಾಗಮ buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    ಧ್ರುವ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    ಯದಿ(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } ಅನ್ಯಥಾ {
        leadCC=0;
    }
    ಪ್ರತ್ಯಾಗಮ buffer.append(reinterpret_cast<ಧ್ರುವ char16_t*>(mapping) + 1, length, ಸತ್ಯ, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ಧ್ರುವ uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ಧ್ರುವ uint8_t *src, ಧ್ರುವ uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) ಧ್ರುವ {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ಧ್ರುವ uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    ಪ್ರತಿ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ಧ್ರುವ uint8_t *fastStart = src;
        ಧ್ರುವ uint8_t *prevSrc;
        uint16_t norm16 = 0;

        ಪ್ರತಿ (;;) {
            ಯದಿ (src == limit) {
                ಯದಿ (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                ಪ್ರತ್ಯಾಗಮ src;
            }
            ಯದಿ (*src < minNoLead) {
                ++src;
            } ಅನ್ಯಥಾ {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                ಯದಿ (!isMostDecompYesAndZeroCC(norm16)) {
                    ಭಙ್ಗ;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        ಯದಿ (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            ಯದಿ (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                ಭಙ್ಗ;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        ಯದಿ (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            ಯದಿ (prevCC <= cc || cc == 0) {
                prevCC = cc;
                ಯದಿ (cc <= 1) {
                    ಯದಿ (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    prevBoundary = src;
                }
                ಅನುವರ್ತ;
            }
        }
        ಯದಿ (sink == nullptr) {
            ಪ್ರತ್ಯಾಗಮ prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        ಯದಿ (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            ಯದಿ (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                ಭಙ್ಗ;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*ಏತತ್, s16, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) {
            ಭಙ್ಗ;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, ಅಸತ್ಯ /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        ಯದಿ (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, ಅಸತ್ಯ /* onlyContiguous */,
                                 buffer, errorCode);
        }
        ಯದಿ (U_FAILURE(errorCode)) {
            ಭಙ್ಗ;
        }
        ಯದಿ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ಭಙ್ಗ;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        ಯದಿ (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            ಯದಿ (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                ಭಙ್ಗ;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                ಭಙ್ಗ;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    ಪ್ರತ್ಯಾಗಮ src;
}

ಧ್ರುವ uint8_t *
Normalizer2Impl::decomposeShort(ಧ್ರುವ uint8_t *src, ಧ್ರುವ uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯಾವತ್ (src < limit) {
        ಧ್ರುವ uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        ಯದಿ (norm16 >= limitNoNo) {
            ಯದಿ (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                ಯದಿ (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    ಪ್ರತ್ಯಾಗಮ prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                ಯದಿ (!buffer.append(c, cc, errorCode)) {
                    ಪ್ರತ್ಯಾಗಮ nullptr;
                }
                ಯದಿ (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    ಪ್ರತ್ಯಾಗಮ src;
                }
                ಅನುವರ್ತ;
            } ಅನ್ಯಥಾ ಯದಿ (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                ಯದಿ (stopAt != STOP_AT_LIMIT) {
                    ಪ್ರತ್ಯಾಗಮ prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } ಅನ್ಯಥಾ ಯದಿ (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            ಪ್ರತ್ಯಾಗಮ prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        ಯದಿ (norm16 < minYesNo) {
            ಯದಿ (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            ಯದಿ (!buffer.append(c, 0, errorCode)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
        } ಅನ್ಯಥಾ ಯದಿ (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            ಯದಿ (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            ಯದಿ (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
        } ಅನ್ಯಥಾ {
            // The character decomposes, get everything from the variable-length extra data.
            ಧ್ರುವ uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            ಯದಿ (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } ಅನ್ಯಥಾ {
                leadCC = 0;
            }
            ಯದಿ (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                ಪ್ರತ್ಯಾಗಮ prevSrc;
            }
            ಯದಿ (!buffer.append(reinterpret_cast<ಧ್ರುವ char16_t*>(mapping) + 1, length, ಸತ್ಯ, leadCC, trailCC, errorCode)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
        }
        ಯದಿ ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            ಪ್ರತ್ಯಾಗಮ src;
        }
    }
    ಪ್ರತ್ಯಾಗಮ src;
}

ಧ್ರುವ char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) ಧ್ರುವ {
    uint16_t norm16;
    ಯದಿ(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ char16_t *decomp = nullptr;
    ಯದಿ(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    ಯದಿ (norm16 < minYesNo) {
        ಪ್ರತ್ಯಾಗಮ decomp;
    } ಅನ್ಯಥಾ ಯದಿ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        ಪ್ರತ್ಯಾಗಮ buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ಧ್ರುವ uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ಧ್ರುವ char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) ಧ್ರುವ {
    uint16_t norm16;
    ಯದಿ(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        ಪ್ರತ್ಯಾಗಮ nullptr;
    } ಅನ್ಯಥಾ ಯದಿ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        ಪ್ರತ್ಯಾಗಮ buffer;
    } ಅನ್ಯಥಾ ಯದಿ(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        ಪ್ರತ್ಯಾಗಮ buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ಧ್ರುವ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    ಯದಿ(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ಧ್ರುವ uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        ಯದಿ(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ char16_t*>(rawMapping) - rm0;
        } ಅನ್ಯಥಾ {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<ಧ್ರುವ char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            ಪ್ರತ್ಯಾಗಮ buffer;
        }
    } ಅನ್ಯಥಾ {
        length=mLength;
        ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ char16_t*>(mapping) + 1;
    }
}

ಶೂನ್ಯ Normalizer2Impl::decomposeAndAppend(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_ಮಧ್ಯ,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) ಧ್ರುವ {
    buffer.copyReorderableSuffixTo(safe_ಮಧ್ಯ);
    ಯದಿ(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Just merge the strings at the boundary.
    ತಾರ್ಕಿಕ isFirst = ಸತ್ಯ;
    uint8_t firstCC = 0, prevCC = 0, cc;
    ಧ್ರುವ char16_t *p = src;
    ಯಾವತ್ (p != limit) {
        ಧ್ರುವ char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ಯದಿ ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            ಭಙ್ಗ;
        }
        ಯದಿ (isFirst) {
            firstCC = cc;
            isFirst = ಅಸತ್ಯ;
        }
        prevCC = cc;
    }
    ಯದಿ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    ಯದಿ (buffer.append(src, static_cast<int32_t>(p - src), ಅಸತ್ಯ, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ಧ್ರುವ {
    ಯದಿ (norm16 < minNoNoCompNoMaybeCC) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (norm16 >= limitNoNo) {
        ಪ್ರತ್ಯಾಗಮ norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ಧ್ರುವ uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ಪ್ರತ್ಯಾಗಮ (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ಧ್ರುವ {
    ಯದಿ (c < minDecompNoCP) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ಧ್ರುವ {
    ಯದಿ(norm16 <= minYesNo || isHangulLVT(norm16)) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (norm16 >= limitNoNo) {
        ಯದಿ (isMaybeYesOrNonZeroCC(norm16)) {
            ಪ್ರತ್ಯಾಗಮ norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } ಅನ್ಯಥಾ ಯದಿ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            ಪ್ರತ್ಯಾಗಮ (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ಧ್ರುವ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    ಯದಿ(firstUnit>0x1ff) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;  // trailCC>1
    }
    ಯದಿ(firstUnit<=0xff) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ಪ್ರತ್ಯಾಗಮ (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(ಧ್ರುವ uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    ಯದಿ(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        ಯಾವತ್(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        ಯದಿ(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            ಯದಿ(firstUnit&COMP_1_TRIPLE) {
                ಪ್ರತ್ಯಾಗಮ (static_cast<int32_t>(list[1]) << 16) | list[2];
            } ಅನ್ಯಥಾ {
                ಪ್ರತ್ಯಾಗಮ list[1];
            }
        }
    } ಅನ್ಯಥಾ {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        ಪ್ರತಿ(;;) {
            ಯದಿ(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } ಅನ್ಯಥಾ ಯದಿ(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                ಯದಿ(key2>(secondUnit=list[1])) {
                    ಯದಿ(firstUnit&COMP_1_LAST_TUPLE) {
                        ಭಙ್ಗ;
                    } ಅನ್ಯಥಾ {
                        list+=3;
                    }
                } ಅನ್ಯಥಾ ಯದಿ(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    ಪ್ರತ್ಯಾಗಮ (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ {
                ಭಙ್ಗ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
ಶೂನ್ಯ Normalizer2Impl::addComposites(ಧ್ರುವ uint16_t *list, UnicodeSet &set) ಧ್ರುವ {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    ಕುರು {
        firstUnit=*list;
        ಯದಿ((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } ಅನ್ಯಥಾ {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        ಯದಿ((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } ಯಾವತ್((firstUnit&COMP_1_LAST_TUPLE)==0);
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
ಶೂನ್ಯ Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) ಧ್ರುವ {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    ಯದಿ(p==limit) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    char16_t *starter, *pRemove, *q, *r;
    ಧ್ರುವ uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=ಅಸತ್ಯ;
    prevCC=0;

    ಪ್ರತಿ(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        ಯದಿ( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            ಯದಿ(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                ಯದಿ(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    ಯದಿ(prev<Hangul::JAMO_L_ಎಣಿಕೆ) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_ಎಣಿಕೆ+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_ಎಣಿಕೆ);
                        char16_t t;
                        ಯದಿ (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_ಎಣಿಕೆ) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        ಯಾವತ್(r<limit) {
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
                ಯದಿ(p==limit) {
                    ಭಙ್ಗ;
                }
                compositionsList=nullptr;
                ಅನುವರ್ತ;
            } ಅನ್ಯಥಾ ಯದಿ((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                ಯದಿ(starterIsSupplementary) {
                    ಯದಿ(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } ಅನ್ಯಥಾ {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=ಅಸತ್ಯ;
                        q=starter+1;
                        r=q+1;
                        ಯಾವತ್(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } ಅನ್ಯಥಾ ಯದಿ(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=ಸತ್ಯ;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    ಯಾವತ್(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } ಅನ್ಯಥಾ {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                ಯದಿ(pRemove<p) {
                    q=pRemove;
                    r=p;
                    ಯಾವತ್(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                ಯದಿ(p==limit) {
                    ಭಙ್ಗ;
                }
                // Is the composite a starter that combines forward?
                ಯದಿ(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } ಅನ್ಯಥಾ {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                ಅನುವರ್ತ;
            }
        }

        // no combination this time
        prevCC=cc;
        ಯದಿ(p==limit) {
            ಭಙ್ಗ;
        }

        // If c did not combine, then check if it is a starter.
        ಯದಿ(cc==0) {
            // Found a new starter.
            ಯದಿ((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                ಯದಿ(U_IS_BMP(c)) {
                    starterIsSupplementary=ಅಸತ್ಯ;
                    starter=p-1;
                } ಅನ್ಯಥಾ {
                    starterIsSupplementary=ಸತ್ಯ;
                    starter=p-2;
                }
            }
        } ಅನ್ಯಥಾ ಯದಿ(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ಧ್ರುವ {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ಧ್ರುವ uint16_t *list;
    ಯದಿ(isInert(norm16)) {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    } ಅನ್ಯಥಾ ಯದಿ(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        ಯದಿ(isJamoL(norm16)) {
            ಯದಿ (b < Hangul::JAMO_V_BASE) {
                ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            ಯದಿ(b<Hangul::JAMO_V_ಎಣಿಕೆ) {
                ಪ್ರತ್ಯಾಗಮ
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_ಎಣಿಕೆ+b)*
                     Hangul::JAMO_T_ಎಣಿಕೆ);
            } ಅನ್ಯಥಾ {
                ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
            }
        } ಅನ್ಯಥಾ ಯದಿ(isHangulLV(norm16)) {
            ಯದಿ (b <= Hangul::JAMO_T_BASE) {
               ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            ಯದಿ(b<Hangul::JAMO_T_ಎಣಿಕೆ) {  // not b==0!
                ಪ್ರತ್ಯಾಗಮ a+b;
            } ಅನ್ಯಥಾ {
                ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
            }
        } ಅನ್ಯಥಾ {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            ಯದಿ(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } ಅನ್ಯಥಾ ಯದಿ(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    } ಅನ್ಯಥಾ {
        list=getDataForMaybe(norm16);
        ಯದಿ(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    ಯದಿ(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
#ಯದಿ U_SIGNED_ಬಲ_SHIFT_IS_ARITHMETIC
    ಪ್ರತ್ಯಾಗಮ combine(list, b)>>1;
#ಅನ್ಯಥಾ
    int32_t compositeAndFwd=combine(list, b);
    ಪ್ರತ್ಯಾಗಮ compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) ಧ್ರುವ {
    ಧ್ರುವ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ಯದಿ(limit==nullptr) {
        src=copy_ಕೆಳಗಿನ_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        limit=u_strchr(src, 0);
        ಯದಿ (prevBoundary != src) {
            ಯದಿ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ಅನ್ಯಥಾ {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    ಪ್ರತಿ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ಧ್ರುವ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        ಪ್ರತಿ (;;) {
            ಯದಿ (src == limit) {
                ಯದಿ (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
            ಯದಿ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ಅನ್ಯಥಾ {
                prevSrc = src++;
                ಯದಿ(!U16_IS_LEAD(c)) {
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ {
                    char16_t c2;
                    ಯದಿ(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ಯದಿ(!isCompYesAndZeroCC(norm16)) {
                            ಭಙ್ಗ;
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
        ಯದಿ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ಯದಿ (!doCompose) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ಯದಿ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ಯದಿ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ಯದಿ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    ಯದಿ(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    prevBoundary = src;
                    ಅನುವರ್ತ;
                }
            } ಅನ್ಯಥಾ ಯದಿ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ಯದಿ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ಯದಿ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    ಧ್ರುವ char16_t *mapping = reinterpret_cast<ಧ್ರುವ char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    ಯದಿ(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    prevBoundary = src;
                    ಅನುವರ್ತ;
                }
            } ಅನ್ಯಥಾ ಯದಿ (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ಯದಿ (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ಯದಿ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    prevBoundary = src;
                    ಅನುವರ್ತ;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ಅನ್ಯಥಾ ಯದಿ (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            ಯದಿ(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                ಯದಿ(l<Hangul::JAMO_L_ಎಣಿಕೆ) {
                    ಯದಿ (!doCompose) {
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                    }
                    int32_t t;
                    ಯದಿ (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_ಎಣಿಕೆ) {
                        // The next character is a Jamo T.
                        ++src;
                    } ಅನ್ಯಥಾ ಯದಿ (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } ಅನ್ಯಥಾ {
                        t = -1;
                    }
                    ಯದಿ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_ಎಣಿಕೆ + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_ಎಣಿಕೆ + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        ಯದಿ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            ಭಙ್ಗ;
                        }
                        ಯದಿ (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            ಭಙ್ಗ;
                        }
                        prevBoundary = src;
                        ಅನುವರ್ತ;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ಅನ್ಯಥಾ ಯದಿ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ಯದಿ (!doCompose) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                ಯದಿ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    ಭಙ್ಗ;
                }
                ಯದಿ (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    ಭಙ್ಗ;
                }
                prevBoundary = src;
                ಅನುವರ್ತ;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ಅನ್ಯಥಾ ಯದಿ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            ಯದಿ (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ಯದಿ (!doCompose) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            } ಅನ್ಯಥಾ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ಧ್ರುವ char16_t *nextSrc;
                uint16_t n16;
                ಪ್ರತಿ (;;) {
                    ಯದಿ (src == limit) {
                        ಯದಿ (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    ಯದಿ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        ಯದಿ (prevCC > cc) {
                            ಯದಿ (!doCompose) {
                                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                            }
                            ಭಙ್ಗ;
                        }
                    } ಅನ್ಯಥಾ {
                        ಭಙ್ಗ;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ಯದಿ (norm16HasCompBoundaryBefore(n16)) {
                    ಯದಿ (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    ಅನುವರ್ತ;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ಯದಿ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ಧ್ರುವ char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            ಯದಿ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ಯದಿ (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            ಭಙ್ಗ;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, ಅಸತ್ಯ /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, ಸತ್ಯ /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) {
            ಭಙ್ಗ;
        }
        ಯದಿ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        ಯದಿ(!doCompose) {
            ಯದಿ(!buffer.equals(prevSrc, src)) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            buffer.remove();
        }
        prevBoundary=src;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ಧ್ರುವ char16_t *
Normalizer2Impl::composeQuickCheck(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) ಧ್ರುವ {
    ಧ್ರುವ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ಯದಿ(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_ಕೆಳಗಿನ_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        ಯದಿ (prevBoundary != src) {
            ಯದಿ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ಅನ್ಯಥಾ {
                prevBoundary = --src;
            }
        }
    }

    ಪ್ರತಿ(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ಧ್ರುವ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        ಪ್ರತಿ (;;) {
            ಯದಿ(src==limit) {
                ಪ್ರತ್ಯಾಗಮ src;
            }
            ಯದಿ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ಅನ್ಯಥಾ {
                prevSrc = src++;
                ಯದಿ(!U16_IS_LEAD(c)) {
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ {
                    char16_t c2;
                    ಯದಿ(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ಯದಿ(!isCompYesAndZeroCC(norm16)) {
                            ಭಙ್ಗ;
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
        ಯದಿ (prevBoundary != prevSrc) {
            ಯದಿ (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } ಅನ್ಯಥಾ {
                ಧ್ರುವ char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                ಯದಿ (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } ಅನ್ಯಥಾ {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        ಯದಿ (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            ಯದಿ (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } ಅನ್ಯಥಾ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ಧ್ರುವ char16_t *nextSrc;
                ಪ್ರತಿ (;;) {
                    ಯದಿ (norm16 < MIN_YES_YES_WITH_CC) {
                        ಯದಿ (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } ಅನ್ಯಥಾ {
                            ಪ್ರತ್ಯಾಗಮ prevBoundary;
                        }
                    }
                    ಯದಿ (src == limit) {
                        ಪ್ರತ್ಯಾಗಮ src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    ಯದಿ (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        ಯದಿ (!(prevCC <= cc || cc == 0)) {
                            ಭಙ್ಗ;
                        }
                    } ಅನ್ಯಥಾ {
                        ಭಙ್ಗ;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                ಯದಿ (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    ಅನುವರ್ತ;
                }
            }
        }
        ಯದಿ(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        ಪ್ರತ್ಯಾಗಮ prevBoundary;
    }
}

ಶೂನ್ಯ Normalizer2Impl::composeAndAppend(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_ಮಧ್ಯ,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(!buffer.isEmpty()) {
        ಧ್ರುವ char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        ಯದಿ(src!=firstStarterInSrc) {
            ಧ್ರುವ char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString ಮಧ್ಯ(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_ಮಧ್ಯ=ಮಧ್ಯ;
            ಮಧ್ಯ.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            ಧ್ರುವ char16_t *ಮಧ್ಯ_Start=ಮಧ್ಯ.getBuffer();
            compose(ಮಧ್ಯ_Start, ಮಧ್ಯ_Start+ಮಧ್ಯ.length(), onlyContiguous,
                    ಸತ್ಯ, buffer, errorCode);
            ಯದಿ(U_FAILURE(errorCode)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            src=firstStarterInSrc;
        }
    }
    ಯದಿ(doCompose) {
        compose(src, limit, onlyContiguous, ಸತ್ಯ, buffer, errorCode);
    } ಅನ್ಯಥಾ {
        ಯದಿ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ಧ್ರುವ uint8_t *src, ಧ್ರುವ uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) ಧ್ರುವ {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ಧ್ರುವ uint8_t *prevBoundary = src;

    ಪ್ರತಿ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ಧ್ರುವ uint8_t *prevSrc;
        uint16_t norm16 = 0;
        ಪ್ರತಿ (;;) {
            ಯದಿ (src == limit) {
                ಯದಿ (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
            ಯದಿ (*src < minNoMaybeLead) {
                ++src;
            } ಅನ್ಯಥಾ {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                ಯದಿ (!isCompYesAndZeroCC(norm16)) {
                    ಭಙ್ಗ;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        ಯದಿ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ಯದಿ (sink == nullptr) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ಯದಿ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ಯದಿ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ಯದಿ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    ಅನುವರ್ತ;
                }
            } ಅನ್ಯಥಾ ಯದಿ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ಯದಿ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ಯದಿ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    ಧ್ರುವ uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    ಯದಿ (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ಧ್ರುವ char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    prevBoundary = src;
                    ಅನುವರ್ತ;
                }
            } ಅನ್ಯಥಾ ಯದಿ (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ಯದಿ (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ಯದಿ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        ಭಙ್ಗ;
                    }
                    ಯದಿ (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    ಅನುವರ್ತ;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ಅನ್ಯಥಾ ಯದಿ (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            ಯದಿ (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                ಯದಿ (static_cast<uint32_t>(l) < Hangul::JAMO_L_ಎಣಿಕೆ) {
                    ಯದಿ (sink == nullptr) {
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    ಯದಿ (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } ಅನ್ಯಥಾ ಯದಿ (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    ಯದಿ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_ಎಣಿಕೆ + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_ಎಣಿಕೆ + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        ಯದಿ (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            ಭಙ್ಗ;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        ಅನುವರ್ತ;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } ಅನ್ಯಥಾ ಯದಿ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ಯದಿ (sink == nullptr) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                ಯದಿ (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    ಭಙ್ಗ;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                ಅನುವರ್ತ;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ಅನ್ಯಥಾ ಯದಿ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            ಯದಿ (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ಯದಿ (sink == nullptr) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            } ಅನ್ಯಥಾ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ಧ್ರುವ uint8_t *nextSrc;
                uint16_t n16;
                ಪ್ರತಿ (;;) {
                    ಯದಿ (src == limit) {
                        ಯದಿ (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    ಯದಿ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        ಯದಿ (prevCC > cc) {
                            ಯದಿ (sink == nullptr) {
                                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                            }
                            ಭಙ್ಗ;
                        }
                    } ಅನ್ಯಥಾ {
                        ಭಙ್ಗ;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ಯದಿ (norm16HasCompBoundaryBefore(n16)) {
                    ಯದಿ (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    ಅನುವರ್ತ;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ಯದಿ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ಧ್ರುವ uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            ಯದಿ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*ಏತತ್, s16, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) {
            ಭಙ್ಗ;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) {
            ಭಙ್ಗ;
        }
        ಯದಿ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
        recompose(buffer, 0, onlyContiguous);
        ಯದಿ (!buffer.equals(prevSrc, src)) {
            ಯದಿ (sink == nullptr) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            ಯದಿ (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                ಭಙ್ಗ;
            }
            ಯದಿ (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                ಭಙ್ಗ;
            }
            prevBoundary = src;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit) ಧ್ರುವ {
    ಯದಿ (src == limit || *src < minCompNoMaybeCP) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    ಪ್ರತ್ಯಾಗಮ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ಧ್ರುವ uint8_t *src, ಧ್ರುವ uint8_t *limit) ಧ್ರುವ {
    ಯದಿ (src == limit) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    ಪ್ರತ್ಯಾಗಮ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ಧ್ರುವ char16_t *start, ಧ್ರುವ char16_t *p,
                                            UBool onlyContiguous) ಧ್ರುವ {
    ಯದಿ (start == p) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    ಪ್ರತ್ಯಾಗಮ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ಧ್ರುವ uint8_t *start, ಧ್ರುವ uint8_t *p,
                                            UBool onlyContiguous) ಧ್ರುವ {
    ಯದಿ (start == p) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    ಪ್ರತ್ಯಾಗಮ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ಧ್ರುವ char16_t *Normalizer2Impl::findPreviousCompBoundary(ಧ್ರುವ char16_t *start, ಧ್ರುವ char16_t *p,
                                                       UBool onlyContiguous) ಧ್ರುವ {
    ಯಾವತ್ (p != start) {
        ಧ್ರುವ char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        ಯದಿ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ಪ್ರತ್ಯಾಗಮ codePointLimit;
        }
        ಯದಿ (hasCompBoundaryBefore(c, norm16)) {
            ಪ್ರತ್ಯಾಗಮ p;
        }
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

ಧ್ರುವ char16_t *Normalizer2Impl::findNextCompBoundary(ಧ್ರುವ char16_t *p, ಧ್ರುವ char16_t *limit,
                                                   UBool onlyContiguous) ಧ್ರುವ {
    ಯಾವತ್ (p != limit) {
        ಧ್ರುವ char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ಯದಿ (hasCompBoundaryBefore(c, norm16)) {
            ಪ್ರತ್ಯಾಗಮ codePointStart;
        }
        ಯದಿ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ಪ್ರತ್ಯಾಗಮ p;
        }
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ಧ್ರುವ char16_t *start, ಧ್ರುವ char16_t *p) ಧ್ರುವ {
    ಯದಿ (start == p) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    ಪ್ರತ್ಯಾಗಮ static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ಧ್ರುವ uint8_t *start, ಧ್ರುವ uint8_t *p) ಧ್ರುವ {
    ಯದಿ (start == p) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    ಪ್ರತ್ಯಾಗಮ static_cast<uint8_t>(getFCD16(c));
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
#ಯದಿ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) ಧ್ರುವ {
    uint16_t norm16=getNorm16(c);
    ಯದಿ (norm16 >= limitNoNo) {
        ಯದಿ(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            ಪ್ರತ್ಯಾಗಮ norm16|(norm16<<8);
        } ಅನ್ಯಥಾ ಯದಿ(norm16>=minMaybeYes) {
            ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ ಯದಿ(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            ಯದಿ (deltaTrailCC <= DELTA_TCCC_1) {
                ಪ್ರತ್ಯಾಗಮ deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ಯದಿ(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ಧ್ರುವ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    ಯದಿ(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    ಪ್ರತ್ಯಾಗಮ norm16;
}
#ಯದಿ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ಧ್ರುವ {
    U_ASSERT(norm16 >= minMaybeNo);
    ಯದಿ (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        ಪ್ರತ್ಯಾಗಮ norm16 | (norm16<<8);
    } ಅನ್ಯಥಾ ಯದಿ (norm16 >= minMaybeYes) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ಧ್ರುವ uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    ಪ್ರತ್ಯಾಗಮ firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ಧ್ರುವ char16_t *
Normalizer2Impl::makeFCD(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) ಧ್ರುವ {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ಧ್ರುವ char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    ಯದಿ(limit==nullptr) {
        src=copy_ಕೆಳಗಿನ_Prefix_From_Nul_Terminated(src, minLcccCP, buffer, errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ src;
        }
        ಯದಿ(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            ಯದಿ(prevFCD16>1) {
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

    ಧ್ರುವ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    ಪ್ರತಿ(;;) {
        // count code units with lccc==0
        ಪ್ರತಿ(prevSrc=src; src!=limit;) {
            ಯದಿ((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } ಅನ್ಯಥಾ ಯದಿ(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } ಅನ್ಯಥಾ {
                ಯದಿ(U16_IS_LEAD(c)) {
                    char16_t c2;
                    ಯದಿ((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                ಯದಿ((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
            }
        }
        // copy these code units all at once
        ಯದಿ(src!=prevSrc) {
            ಯದಿ(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                ಭಙ್ಗ;
            }
            ಯದಿ(src==limit) {
                ಭಙ್ಗ;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            ಯದಿ(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                ಯದಿ(prev<minDecompNoCP) {
                    prevFCD16=0;
                } ಅನ್ಯಥಾ {
                    prevFCD16=getFCD16FromNormData(prev);
                    ಯದಿ(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } ಅನ್ಯಥಾ {
                ಧ್ರುವ char16_t *p=src-1;
                ಯದಿ(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                ಯದಿ(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } ಅನ್ಯಥಾ ಯದಿ(src==limit) {
            ಭಙ್ಗ;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        ಯದಿ((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            ಯದಿ((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            ಯದಿ(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                ಭಙ್ಗ;
            }
            prevFCD16=fcd16;
            ಅನುವರ್ತ;
        } ಅನ್ಯಥಾ ಯದಿ(buffer==nullptr) {
            ಪ್ರತ್ಯಾಗಮ prevBoundary;  // quick check "no"
        } ಅನ್ಯಥಾ {
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
            decomposeShort(prevBoundary, src, ಅಸತ್ಯ, ಅಸತ್ಯ, *buffer, errorCode);
            ಯದಿ (U_FAILURE(errorCode)) {
                ಭಙ್ಗ;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ src;
}

ಶೂನ್ಯ Normalizer2Impl::makeFCDAndAppend(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_ಮಧ್ಯ,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(!buffer.isEmpty()) {
        ಧ್ರುವ char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        ಯದಿ(src!=firstBoundaryInSrc) {
            ಧ್ರುವ char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString ಮಧ್ಯ(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_ಮಧ್ಯ=ಮಧ್ಯ;
            ಮಧ್ಯ.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            ಧ್ರುವ char16_t *ಮಧ್ಯ_Start=ಮಧ್ಯ.getBuffer();
            makeFCD(ಮಧ್ಯ_Start, ಮಧ್ಯ_Start+ಮಧ್ಯ.length(), &buffer, errorCode);
            ಯದಿ(U_FAILURE(errorCode)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            src=firstBoundaryInSrc;
        }
    }
    ಯದಿ(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } ಅನ್ಯಥಾ {
        ಯದಿ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

ಧ್ರುವ char16_t *Normalizer2Impl::findPreviousFCDBoundary(ಧ್ರುವ char16_t *start, ಧ್ರುವ char16_t *p) ಧ್ರುವ {
    ಯಾವತ್(start<p) {
        ಧ್ರುವ char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        ಯದಿ (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            ಪ್ರತ್ಯಾಗಮ codePointLimit;
        }
        ಯದಿ (norm16HasDecompBoundaryBefore(norm16)) {
            ಪ್ರತ್ಯಾಗಮ p;
        }
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

ಧ್ರುವ char16_t *Normalizer2Impl::findNextFCDBoundary(ಧ್ರುವ char16_t *p, ಧ್ರುವ char16_t *limit) ಧ್ರುವ {
    ಯಾವತ್(p<limit) {
        ಧ್ರುವ char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ಯದಿ (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            ಪ್ರತ್ಯಾಗಮ codePointStart;
        }
        ಯದಿ (norm16HasDecompBoundaryAfter(norm16)) {
            ಪ್ರತ್ಯಾಗಮ p;
        }
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

ಶೂನ್ಯ CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_ಮೌಲ್ಯ = umutablecptrie_get(mutableTrie, decompLead);
    ಯದಿ((canon_ಮೌಲ್ಯ&(CANON_HAS_SET|CANON_ಮೌಲ್ಯ_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_ಮೌಲ್ಯ|origin, &errorCode);
    } ಅನ್ಯಥಾ {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        ಯದಿ((canon_ಮೌಲ್ಯ&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(ನೂತನ UnicodeSet, errorCode);
            set=lpSet.getAlias();
            ಯದಿ(U_FAILURE(errorCode)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_ಮೌಲ್ಯ & CANON_ಮೌಲ್ಯ_MASK);
            canon_ಮೌಲ್ಯ = (canon_ಮೌಲ್ಯ & ~CANON_ಮೌಲ್ಯ_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_ಮೌಲ್ಯ, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            ಯದಿ (U_FAILURE(errorCode)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಯದಿ(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } ಅನ್ಯಥಾ {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_ಮೌಲ್ಯ & CANON_ಮೌಲ್ಯ_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
ವರ್ಗ InitCanonIterData {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಶೂನ್ಯ doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

ಶೂನ್ಯ InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = ನೂತನ CanonIterData(errorCode);
    ಯದಿ (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        ಯಾವತ್ ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            ಯದಿ (value != Normalizer2Impl::INERT) {
                impl->makeCanonIterDataFromNorm16(start, end, value, *impl->fCanonIterData, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->fCanonIterData->mutableTrie, "CanonIterData");
#endif
        impl->fCanonIterData->trie = umutablecptrie_buildImmutable(
            impl->fCanonIterData->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_ಮೌಲ್ಯ_BITS_32, &errorCode);
        umutablecptrie_close(impl->fCanonIterData->mutableTrie);
        impl->fCanonIterData->mutableTrie = nullptr;
    }
    ಯದಿ (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

ಶೂನ್ಯ Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, ಧ್ರುವ uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ(UChar32 c=start; c<=end; ++c) {
        uint32_t old_ಮೌಲ್ಯ = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t new_ಮೌಲ್ಯ=old_ಮೌಲ್ಯ;
        ಯದಿ(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_ಮೌಲ್ಯ|=CANON_NOT_SEGMENT_STARTER;
            ಯದಿ(norm16<MIN_NORMAL_MAYBE_YES) {
                new_ಮೌಲ್ಯ|=CANON_HAS_COMPOSITIONS;
            }
        } ಅನ್ಯಥಾ ಯದಿ(norm16<minYesNo) {
            new_ಮೌಲ್ಯ|=CANON_HAS_COMPOSITIONS;
        } ಅನ್ಯಥಾ {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            ಯದಿ (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            ಯದಿ (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ಧ್ರುವ uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                ಯದಿ((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    ಯದಿ(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_ಮೌಲ್ಯ|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                ಯದಿ(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    ಯದಿ(norm16_2>=minNoNo) {
                        ಯಾವತ್(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2_ಮೌಲ್ಯ = umutablecptrie_get(newData.mutableTrie, c2);
                            ಯದಿ((c2_ಮೌಲ್ಯ&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2_ಮೌಲ್ಯ|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } ಅನ್ಯಥಾ {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        ಯದಿ(new_ಮೌಲ್ಯ!=old_ಮೌಲ್ಯ) {
            umutablecptrie_set(newData.mutableTrie, c, new_ಮೌಲ್ಯ, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) ಧ್ರುವ {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(ಏತತ್);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_ಮೌಲ್ಯ(UChar32 c) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

ಧ್ರುವ UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ *static_cast<ಧ್ರುವ UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ get_Canon_ಮೌಲ್ಯ(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) ಧ್ರುವ {
    int32_t canon_ಮೌಲ್ಯ=get_Canon_ಮೌಲ್ಯ(c)&~CANON_NOT_SEGMENT_STARTER;
    ಯದಿ(canon_ಮೌಲ್ಯ==0) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    set.clear();
    int32_t value=canon_ಮೌಲ್ಯ&CANON_ಮೌಲ್ಯ_MASK;
    ಯದಿ((canon_ಮೌಲ್ಯ&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } ಅನ್ಯಥಾ ಯದಿ(value!=0) {
        set.add(value);
    }
    ಯದಿ((canon_ಮೌಲ್ಯ&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        ಯದಿ(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_ಎಣಿಕೆ);
            set.add(syllable, syllable+Hangul::JAMO_VT_ಎಣಿಕೆ-1);
        } ಅನ್ಯಥಾ {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(ಧ್ರುವ UDataSwapper *ds,
            ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
            UErrorCode *pErrorCode) {
    ಧ್ರುವ UDataInfo *pInfo;
    int32_t headerSize;

    ಧ್ರುವ uint8_t *inBytes;
    uint8_t *outBytes;

    ಧ್ರುವ int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* check data format and format version */
    pInfo=(ಧ್ರುವ UDataInfo *)((ಧ್ರುವ ವರ್ಣ *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    ಯದಿ(!(
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
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    inBytes=(ಧ್ರುವ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(ಧ್ರುವ int32_t *)inBytes;
    int32_t minIndexesLength;
    ಯದಿ(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } ಅನ್ಯಥಾ ಯದಿ(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } ಅನ್ಯಥಾ ಯದಿ(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } ಅನ್ಯಥಾ {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    ಯದಿ(length>=0) {
        length-=headerSize;
        ಯದಿ(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    /* read the first few indexes */
    ಪ್ರತಿ(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    ಯದಿ(length>=0) {
        ಯದಿ(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* copy the data for inaccessible bytes */
        ಯದಿ(inBytes!=outBytes) {
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

    ಪ್ರತ್ಯಾಗಮ headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
