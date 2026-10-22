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

#include "unicode/utypes.h"

#ถ้า !UCONFIG_NO_NORMALIZATION

#include "unicode/bytestream.h"
#include "unicode/edits.h"
#include "unicode/normalizer2.h"
#include "unicode/stringoptions.h"
#include "unicode/ucptrie.h"
#include "unicode/udata.h"
#include "unicode/umutablecptrie.h"
#include "unicode/ustring.h"
#include "unicode/utf16.h"
#include "unicode/utf8.h"
#include "bytesinkutil.h"
#include "cmemory.h"
#include "mutex.h"
#include "normalizer2impl.h"
#include "putilimp.h"
#include "uassert.h"
#include "ucptrie_impl.h"
#include "uset_imp.h"
#include "uvector.h"

U_NAMESPACE_BEGIN

namespace {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
anta_hstha uint8_t leadByteForCP(UChar32 c) {
    ถ้า (c <= 0x7f) {
        คืน static_cast<uint8_t>(c);
    } มิฉะนั้น ถ้า (c <= 0x7ff) {
        คืน static_cast<uint8_t>(0xc0 + (c >> 6));
    } มิฉะนั้น {
        // Should not occur because ccc(U+0300)!=0.
        คืน 0xe0;
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
UChar32 codePointFromValidUTF8(const uint8_t *cpStart, const uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    switch(cpLimit-cpStart) {
    pak_sa 1:
        คืน c;
    pak_sa 2:
        คืน ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    pak_sa 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        คืน static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    pak_sa 4:
        คืน ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    default:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(const uint8_t *start, const uint8_t *p) {
    ถ้า ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        ถ้า (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            คืน ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    คืน U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(const uint8_t *src, const uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    ถ้า ((limit - src) >= 3 && *src == 0xe1) {
        ถ้า (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            ถ้า (0xa8 <= t && t <= 0xbf) {
                คืน t - 0xa7;
            }
        } มิฉะนั้น ถ้า (src[1] == 0x87) {
            uint8_t t = src[2];
            ถ้า (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                คืน t - (0xa7 - 0x40);
            }
        }
    }
    คืน -1;
}

void
appendCodePointDelta(const uint8_t *cpStart, const uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    var_na buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    ถ้า (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } มิฉะนั้น {
        int32_t trail = *(cpLimit-1) + delta;
        ถ้า (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            do { buffer[length++] = *cpStart++; } ขณะ (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } มิฉะนั้น {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    ถ้า (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(const Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    ถ้า (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    ถ้า(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        คืน false;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    ถ้า(start==limit) {
        lastCC=0;
    } มิฉะนั้น {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        ถ้า(lastCC>1) {
            ขณะ(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    คืน true;
}

UBool ReorderingBuffer::equals(const char16_t *otherStart, const char16_t *otherLimit) const {
    int32_t length = static_cast<int32_t>(limit - start);
    คืน
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(const uint8_t *otherStart, const uint8_t *otherLimit) const {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    ถ้า (otherLength < length || (otherLength / 3) > length) {
        คืน false;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    สำหรับ (int32_t i = 0, j = 0;;) {
        ถ้า (i >= length) {
            คืน j >= otherLength;
        } มิฉะนั้น ถ้า (j >= otherLength) {
            คืน false;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        ถ้า (c != other) {
            คืน false;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    ถ้า(remainingCapacity<2 && !resize(2, errorCode)) {
        คืน false;
    }
    ถ้า(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        ถ้า(cc<=1) {
            reorderStart=limit;
        }
    } มิฉะนั้น {
        insert(c, cc);
    }
    remainingCapacity-=2;
    คืน true;
}

UBool ReorderingBuffer::append(const char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    ถ้า(length==0) {
        คืน true;
    }
    ถ้า(remainingCapacity<length && !resize(length, errorCode)) {
        คืน false;
    }
    remainingCapacity-=length;
    ถ้า(lastCC<=leadCC || leadCC==0) {
        ถ้า(trailCC<=1) {
            reorderStart=limit+length;
        } มิฉะนั้น ถ้า(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        const char16_t *sLimit=s+length;
        do { *limit++=*s++; } ขณะ(s!=sLimit);
        lastCC=trailCC;
    } มิฉะนั้น {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        ขณะ(i<length) {
            U16_NEXT(s, i, length, c);
            ถ้า(i<length) {
                ถ้า (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } มิฉะนั้น {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } มิฉะนั้น {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    คืน true;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    ถ้า(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        คืน false;
    }
    remainingCapacity-=cpLength;
    ถ้า(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } มิฉะนั้น {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    คืน true;
}

UBool ReorderingBuffer::appendZeroCC(const char16_t *s, const char16_t *sLimit, UErrorCode &errorCode) {
    ถ้า(s==sLimit) {
        คืน true;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    ถ้า(remainingCapacity<length && !resize(length, errorCode)) {
        คืน false;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    คืน true;
}

void ReorderingBuffer::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

void ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    ถ้า(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } มิฉะนั้น {
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
    ถ้า(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    ถ้า(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    ถ้า(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        คืน false;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    คืน true;
}

void ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    ถ้า(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    ถ้า(reorderStart>=codePointStart) {
        คืน 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    ถ้า(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    คืน impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
void ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    สำหรับ(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    do {
        *--r=*--q;
    } ขณะ(codePointLimit!=q);
    writeCodePoint(q, c);
    ถ้า(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

sa_nracanaa CanonIterData : public UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    void addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

void
Normalizer2Impl::init(const int32_t *inIndexes, const UCPTrie *inTrie,
                      const uint16_t *inExtraData, const uint8_t *inSmallFCD) {
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

static uint32_t U_CALLCONV
segmentStarterMapper(const void * /*context*/, uint32_t value) {
    คืน value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

void
Normalizer2Impl::addLcccChars(UnicodeSet &set) const {
    UChar32 start = 0, end;
    uint32_t norm16;
    ขณะ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        ถ้า (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } มิฉะนั้น ถ้า (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            ถ้า (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

void
Normalizer2Impl::addPropertyStarts(const USetAdder *sa, UErrorCode & /*errorCode*/) const {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    ขณะ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        ถ้า (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            ขณะ (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                ถ้า (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    สำหรับ(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_COUNT) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

void
Normalizer2Impl::addCanonIterPropertyStarts(const USetAdder *sa, UErrorCode &errorCode) const {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    ถ้า (!ensureCanonIterData(errorCode)) { คืน; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    ขณะ ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

const char16_t *
Normalizer2Impl::copyLowPrefixFromNulTerminated(const char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) const {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    const char16_t *prevSrc=src;
    char16_t c;
    ขณะ((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    ถ้า(--src!=prevSrc) {
        ถ้า(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    คืน src;
}

UnicodeString &
Normalizer2Impl::decompose(const UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) const {
    ถ้า(U_FAILURE(errorCode)) {
        dest.setToBogus();
        คืน dest;
    }
    const char16_t *sArray=src.getBuffer();
    ถ้า(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        คืน dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    คืน dest;
}

void
Normalizer2Impl::decompose(const char16_t *src, const char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) const {
    ถ้า(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    ReorderingBuffer buffer(*this, dest);
    ถ้า(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
const char16_t *
Normalizer2Impl::decompose(const char16_t *src, const char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) const {
    ถ้า(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        คืน src;
    }
    UChar32 minNoCP=minDecompNoCP;
    ถ้า(limit==nullptr) {
        src=copyLowPrefixFromNulTerminated(src, minNoCP, buffer, errorCode);
        ถ้า(U_FAILURE(errorCode)) {
            คืน src;
        }
        limit=u_strchr(src, 0);
    }

    const char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    const char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    สำหรับ(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        สำหรับ(prevSrc=src; src!=limit;) {
            ถ้า( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } มิฉะนั้น ถ้า(!U16_IS_LEAD(c)) {
                bha_nga;
            } มิฉะนั้น {
                char16_t c2;
                ถ้า((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    ถ้า(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } มิฉะนั้น {
                        bha_nga;
                    }
                } มิฉะนั้น {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        ถ้า(src!=prevSrc) {
            ถ้า(buffer!=nullptr) {
                ถ้า(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    bha_nga;
                }
            } มิฉะนั้น {
                prevCC=0;
                prevBoundary=src;
            }
        }
        ถ้า(src==limit) {
            bha_nga;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        ถ้า(buffer!=nullptr) {
            ถ้า(!decompose(c, norm16, *buffer, errorCode)) {
                bha_nga;
            }
        } มิฉะนั้น {
            ถ้า(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                ถ้า(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    ถ้า(cc<=1) {
                        prevBoundary=src;
                    }
                    continue;
                }
            }
            คืน prevBoundary;  // "no" or cc out of order
        }
    }
    คืน src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
const char16_t *
Normalizer2Impl::decomposeShort(const char16_t *src, const char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) const {
    ถ้า (U_FAILURE(errorCode)) {
        คืน nullptr;
    }
    ขณะ(src<limit) {
        ถ้า (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            คืน src;
        }
        const char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        ถ้า (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            คืน prevSrc;
        }
        ถ้า(!decompose(c, norm16, buffer, errorCode)) {
            คืน nullptr;
        }
        ถ้า (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            คืน src;
        }
    }
    คืน src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) const {
    // get the decomposition and the lead and trail cc's
    ถ้า (norm16 >= limitNoNo) {
        ถ้า (isMaybeYesOrNonZeroCC(norm16)) {
            คืน buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } มิฉะนั้น ถ้า (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ถ้า (norm16 < minYesNo) {
        // c does not decompose
        คืน buffer.append(c, 0, errorCode);
    } มิฉะนั้น ถ้า(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        คืน buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    const uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    ถ้า(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } มิฉะนั้น {
        leadCC=0;
    }
    คืน buffer.append(reinterpret_cast<const char16_t*>(mapping) + 1, length, true, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
const uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               const uint8_t *src, const uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) const {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    const uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    สำหรับ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        const uint8_t *fastStart = src;
        const uint8_t *prevSrc;
        uint16_t norm16 = 0;

        สำหรับ (;;) {
            ถ้า (src == limit) {
                ถ้า (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                คืน src;
            }
            ถ้า (*src < minNoLead) {
                ++src;
            } มิฉะนั้น {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                ถ้า (!isMostDecompYesAndZeroCC(norm16)) {
                    bha_nga;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        ถ้า (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            ถ้า (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                bha_nga;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        ถ้า (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            ถ้า (prevCC <= cc || cc == 0) {
                prevCC = cc;
                ถ้า (cc <= 1) {
                    ถ้า (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        bha_nga;
                    }
                    prevBoundary = src;
                }
                continue;
            }
        }
        ถ้า (sink == nullptr) {
            คืน prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        ถ้า (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            ถ้า (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                bha_nga;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*this, s16, errorCode);
        ถ้า (U_FAILURE(errorCode)) {
            bha_nga;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, false /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        ถ้า (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, false /* onlyContiguous */,
                                 buffer, errorCode);
        }
        ถ้า (U_FAILURE(errorCode)) {
            bha_nga;
        }
        ถ้า ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            bha_nga;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        ถ้า (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            ถ้า (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                bha_nga;
            }
        } มิฉะนั้น {
            ถ้า (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                bha_nga;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    คืน src;
}

const uint8_t *
Normalizer2Impl::decomposeShort(const uint8_t *src, const uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) const {
    ถ้า (U_FAILURE(errorCode)) {
        คืน nullptr;
    }
    ขณะ (src < limit) {
        const uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        ถ้า (norm16 >= limitNoNo) {
            ถ้า (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                ถ้า (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    คืน prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                ถ้า (!buffer.append(c, cc, errorCode)) {
                    คืน nullptr;
                }
                ถ้า (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    คืน src;
                }
                continue;
            } มิฉะนั้น ถ้า (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                ถ้า (stopAt != STOP_AT_LIMIT) {
                    คืน prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } มิฉะนั้น ถ้า (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            คืน prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        ถ้า (norm16 < minYesNo) {
            ถ้า (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            ถ้า (!buffer.append(c, 0, errorCode)) {
                คืน nullptr;
            }
        } มิฉะนั้น ถ้า (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            ถ้า (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            ถ้า (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                คืน nullptr;
            }
        } มิฉะนั้น {
            // The character decomposes, get everything from the variable-length extra data.
            const uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            ถ้า (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } มิฉะนั้น {
                leadCC = 0;
            }
            ถ้า (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                คืน prevSrc;
            }
            ถ้า (!buffer.append(reinterpret_cast<const char16_t*>(mapping) + 1, length, true, leadCC, trailCC, errorCode)) {
                คืน nullptr;
            }
        }
        ถ้า ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            คืน src;
        }
    }
    คืน src;
}

const char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) const {
    uint16_t norm16;
    ถ้า(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        คืน nullptr;
    }
    const char16_t *decomp = nullptr;
    ถ้า(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    ถ้า (norm16 < minYesNo) {
        คืน decomp;
    } มิฉะนั้น ถ้า(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        คืน buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    const uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    คืน reinterpret_cast<const char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
const char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) const {
    uint16_t norm16;
    ถ้า(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        คืน nullptr;
    } มิฉะนั้น ถ้า(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        คืน buffer;
    } มิฉะนั้น ถ้า(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        คืน buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    const uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    ถ้า(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        const uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        ถ้า(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            คืน reinterpret_cast<const char16_t*>(rawMapping) - rm0;
        } มิฉะนั้น {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<const char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            คืน buffer;
        }
    } มิฉะนั้น {
        length=mLength;
        คืน reinterpret_cast<const char16_t*>(mapping) + 1;
    }
}

void Normalizer2Impl::decomposeAndAppend(const char16_t *src, const char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safeMiddle,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) const {
    buffer.copyReorderableSuffixTo(safeMiddle);
    ถ้า(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        คืน;
    }
    // Just merge the strings at the boundary.
    bool isFirst = true;
    uint8_t firstCC = 0, prevCC = 0, cc;
    const char16_t *p = src;
    ขณะ (p != limit) {
        const char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ถ้า ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            bha_nga;
        }
        ถ้า (isFirst) {
            firstCC = cc;
            isFirst = false;
        }
        prevCC = cc;
    }
    ถ้า(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    ถ้า (buffer.append(src, static_cast<int32_t>(p - src), false, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) const {
    คืน c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) const {
    ถ้า (norm16 < minNoNoCompNoMaybeCC) {
        คืน true;
    }
    ถ้า (norm16 >= limitNoNo) {
        คืน norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    const uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    คืน (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) const {
    ถ้า (c < minDecompNoCP) {
        คืน true;
    }
    ถ้า (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        คืน true;
    }
    คืน norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) const {
    ถ้า(norm16 <= minYesNo || isHangulLVT(norm16)) {
        คืน true;
    }
    ถ้า (norm16 >= limitNoNo) {
        ถ้า (isMaybeYesOrNonZeroCC(norm16)) {
            คืน norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } มิฉะนั้น ถ้า (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            คืน (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    const uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    ถ้า(firstUnit>0x1ff) {
        คืน false;  // trailCC>1
    }
    ถ้า(firstUnit<=0xff) {
        คืน true;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    คืน (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(const uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    ถ้า(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        ขณะ(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        ถ้า(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            ถ้า(firstUnit&COMP_1_TRIPLE) {
                คืน (static_cast<int32_t>(list[1]) << 16) | list[2];
            } มิฉะนั้น {
                คืน list[1];
            }
        }
    } มิฉะนั้น {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        สำหรับ(;;) {
            ถ้า(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } มิฉะนั้น ถ้า(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                ถ้า(key2>(secondUnit=list[1])) {
                    ถ้า(firstUnit&COMP_1_LAST_TUPLE) {
                        bha_nga;
                    } มิฉะนั้น {
                        list+=3;
                    }
                } มิฉะนั้น ถ้า(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    คืน (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } มิฉะนั้น {
                    bha_nga;
                }
            } มิฉะนั้น {
                bha_nga;
            }
        }
    }
    คืน -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
void Normalizer2Impl::addComposites(const uint16_t *list, UnicodeSet &set) const {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    do {
        firstUnit=*list;
        ถ้า((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } มิฉะนั้น {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        ถ้า((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } ขณะ((firstUnit&COMP_1_LAST_TUPLE)==0);
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
void Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) const {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    ถ้า(p==limit) {
        คืน;
    }

    char16_t *starter, *pRemove, *q, *r;
    const uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=false;
    prevCC=0;

    สำหรับ(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        ถ้า( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            ถ้า(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                ถ้า(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    ถ้า(prev<Hangul::JAMO_L_COUNT) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_COUNT+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_COUNT);
                        char16_t t;
                        ถ้า (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_COUNT) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        ขณะ(r<limit) {
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
                ถ้า(p==limit) {
                    bha_nga;
                }
                compositionsList=nullptr;
                continue;
            } มิฉะนั้น ถ้า((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                ถ้า(starterIsSupplementary) {
                    ถ้า(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } มิฉะนั้น {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=false;
                        q=starter+1;
                        r=q+1;
                        ขณะ(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } มิฉะนั้น ถ้า(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=true;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    ขณะ(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } มิฉะนั้น {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                ถ้า(pRemove<p) {
                    q=pRemove;
                    r=p;
                    ขณะ(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                ถ้า(p==limit) {
                    bha_nga;
                }
                // Is the composite a starter that combines forward?
                ถ้า(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } มิฉะนั้น {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                continue;
            }
        }

        // no combination this time
        prevCC=cc;
        ถ้า(p==limit) {
            bha_nga;
        }

        // If c did not combine, then check if it is a starter.
        ถ้า(cc==0) {
            // Found a new starter.
            ถ้า((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                ถ้า(U_IS_BMP(c)) {
                    starterIsSupplementary=false;
                    starter=p-1;
                } มิฉะนั้น {
                    starterIsSupplementary=true;
                    starter=p-2;
                }
            }
        } มิฉะนั้น ถ้า(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) const {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    const uint16_t *list;
    ถ้า(isInert(norm16)) {
        คืน U_SENTINEL;
    } มิฉะนั้น ถ้า(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        ถ้า(isJamoL(norm16)) {
            ถ้า (b < Hangul::JAMO_V_BASE) {
                คืน U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            ถ้า(b<Hangul::JAMO_V_COUNT) {
                คืน
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_COUNT+b)*
                     Hangul::JAMO_T_COUNT);
            } มิฉะนั้น {
                คืน U_SENTINEL;
            }
        } มิฉะนั้น ถ้า(isHangulLV(norm16)) {
            ถ้า (b <= Hangul::JAMO_T_BASE) {
               คืน U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            ถ้า(b<Hangul::JAMO_T_COUNT) {  // not b==0!
                คืน a+b;
            } มิฉะนั้น {
                คืน U_SENTINEL;
            }
        } มิฉะนั้น {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            ถ้า(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } มิฉะนั้น ถ้า(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        คืน U_SENTINEL;
    } มิฉะนั้น {
        list=getDataForMaybe(norm16);
        ถ้า(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    ถ้า(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        คืน U_SENTINEL;
    }
#ถ้า U_SIGNED_RIGHT_SHIFT_IS_ARITHMETIC
    คืน combine(list, b)>>1;
#มิฉะนั้น
    int32_t compositeAndFwd=combine(list, b);
    คืน compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(const char16_t *src, const char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) const {
    const char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ถ้า(limit==nullptr) {
        src=copyLowPrefixFromNulTerminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        ถ้า(U_FAILURE(errorCode)) {
            คืน false;
        }
        limit=u_strchr(src, 0);
        ถ้า (prevBoundary != src) {
            ถ้า (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } มิฉะนั้น {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    สำหรับ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        const char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        สำหรับ (;;) {
            ถ้า (src == limit) {
                ถ้า (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                คืน true;
            }
            ถ้า( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } มิฉะนั้น {
                prevSrc = src++;
                ถ้า(!U16_IS_LEAD(c)) {
                    bha_nga;
                } มิฉะนั้น {
                    char16_t c2;
                    ถ้า(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ถ้า(!isCompYesAndZeroCC(norm16)) {
                            bha_nga;
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
        ถ้า (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ถ้า (!doCompose) {
                คืน false;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ถ้า (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ถ้า (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ถ้า (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        bha_nga;
                    }
                    ถ้า(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        bha_nga;
                    }
                    prevBoundary = src;
                    continue;
                }
            } มิฉะนั้น ถ้า (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ถ้า (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ถ้า (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        bha_nga;
                    }
                    const char16_t *mapping = reinterpret_cast<const char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    ถ้า(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        bha_nga;
                    }
                    prevBoundary = src;
                    continue;
                }
            } มิฉะนั้น ถ้า (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ถ้า (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ถ้า (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        bha_nga;
                    }
                    prevBoundary = src;
                    continue;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } มิฉะนั้น ถ้า (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            ถ้า(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                ถ้า(l<Hangul::JAMO_L_COUNT) {
                    ถ้า (!doCompose) {
                        คืน false;
                    }
                    int32_t t;
                    ถ้า (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_COUNT) {
                        // The next character is a Jamo T.
                        ++src;
                    } มิฉะนั้น ถ้า (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } มิฉะนั้น {
                        t = -1;
                    }
                    ถ้า (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_COUNT + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_COUNT + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        ถ้า (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            bha_nga;
                        }
                        ถ้า (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            bha_nga;
                        }
                        prevBoundary = src;
                        continue;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } มิฉะนั้น ถ้า (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ถ้า (!doCompose) {
                    คืน false;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                ถ้า (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    bha_nga;
                }
                ถ้า (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    bha_nga;
                }
                prevBoundary = src;
                continue;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } มิฉะนั้น ถ้า (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            ถ้า (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ถ้า (!doCompose) {
                    คืน false;
                }
            } มิฉะนั้น {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                const char16_t *nextSrc;
                uint16_t n16;
                สำหรับ (;;) {
                    ถ้า (src == limit) {
                        ถ้า (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        คืน true;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    ถ้า (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        ถ้า (prevCC > cc) {
                            ถ้า (!doCompose) {
                                คืน false;
                            }
                            bha_nga;
                        }
                    } มิฉะนั้น {
                        bha_nga;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ถ้า (norm16HasCompBoundaryBefore(n16)) {
                    ถ้า (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    continue;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ถ้า (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            const char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            ถ้า (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ถ้า (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            bha_nga;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, false /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, true /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        ถ้า (U_FAILURE(errorCode)) {
            bha_nga;
        }
        ถ้า ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            คืน true;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        ถ้า(!doCompose) {
            ถ้า(!buffer.equals(prevSrc, src)) {
                คืน false;
            }
            buffer.remove();
        }
        prevBoundary=src;
    }
    คืน true;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
const char16_t *
Normalizer2Impl::composeQuickCheck(const char16_t *src, const char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) const {
    const char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ถ้า(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copyLowPrefixFromNulTerminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        ถ้า (prevBoundary != src) {
            ถ้า (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } มิฉะนั้น {
                prevBoundary = --src;
            }
        }
    }

    สำหรับ(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        const char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        สำหรับ (;;) {
            ถ้า(src==limit) {
                คืน src;
            }
            ถ้า( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } มิฉะนั้น {
                prevSrc = src++;
                ถ้า(!U16_IS_LEAD(c)) {
                    bha_nga;
                } มิฉะนั้น {
                    char16_t c2;
                    ถ้า(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ถ้า(!isCompYesAndZeroCC(norm16)) {
                            bha_nga;
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
        ถ้า (prevBoundary != prevSrc) {
            ถ้า (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } มิฉะนั้น {
                const char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                ถ้า (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } มิฉะนั้น {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        ถ้า (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            ถ้า (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } มิฉะนั้น {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                const char16_t *nextSrc;
                สำหรับ (;;) {
                    ถ้า (norm16 < MIN_YES_YES_WITH_CC) {
                        ถ้า (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } มิฉะนั้น {
                            คืน prevBoundary;
                        }
                    }
                    ถ้า (src == limit) {
                        คืน src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    ถ้า (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        ถ้า (!(prevCC <= cc || cc == 0)) {
                            bha_nga;
                        }
                    } มิฉะนั้น {
                        bha_nga;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                ถ้า (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    continue;
                }
            }
        }
        ถ้า(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        คืน prevBoundary;
    }
}

void Normalizer2Impl::composeAndAppend(const char16_t *src, const char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safeMiddle,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) const {
    ถ้า(!buffer.isEmpty()) {
        const char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        ถ้า(src!=firstStarterInSrc) {
            const char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString middle(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safeMiddle=middle;
            middle.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            const char16_t *middleStart=middle.getBuffer();
            compose(middleStart, middleStart+middle.length(), onlyContiguous,
                    true, buffer, errorCode);
            ถ้า(U_FAILURE(errorCode)) {
                คืน;
            }
            src=firstStarterInSrc;
        }
    }
    ถ้า(doCompose) {
        compose(src, limit, onlyContiguous, true, buffer, errorCode);
    } มิฉะนั้น {
        ถ้า(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             const uint8_t *src, const uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) const {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    const uint8_t *prevBoundary = src;

    สำหรับ (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        const uint8_t *prevSrc;
        uint16_t norm16 = 0;
        สำหรับ (;;) {
            ถ้า (src == limit) {
                ถ้า (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                คืน true;
            }
            ถ้า (*src < minNoMaybeLead) {
                ++src;
            } มิฉะนั้น {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                ถ้า (!isCompYesAndZeroCC(norm16)) {
                    bha_nga;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        ถ้า (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ถ้า (sink == nullptr) {
                คืน false;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ถ้า (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ถ้า (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ถ้า (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        bha_nga;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    continue;
                }
            } มิฉะนั้น ถ้า (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ถ้า (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ถ้า (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        bha_nga;
                    }
                    const uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    ถ้า (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<const char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        bha_nga;
                    }
                    prevBoundary = src;
                    continue;
                }
            } มิฉะนั้น ถ้า (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ถ้า (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ถ้า (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        bha_nga;
                    }
                    ถ้า (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    continue;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } มิฉะนั้น ถ้า (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            ถ้า (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                ถ้า (static_cast<uint32_t>(l) < Hangul::JAMO_L_COUNT) {
                    ถ้า (sink == nullptr) {
                        คืน false;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    ถ้า (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } มิฉะนั้น ถ้า (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    ถ้า (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_COUNT + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_COUNT + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        ถ้า (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            bha_nga;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        continue;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } มิฉะนั้น ถ้า (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ถ้า (sink == nullptr) {
                    คืน false;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                ถ้า (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    bha_nga;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                continue;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } มิฉะนั้น ถ้า (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            ถ้า (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ถ้า (sink == nullptr) {
                    คืน false;
                }
            } มิฉะนั้น {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                const uint8_t *nextSrc;
                uint16_t n16;
                สำหรับ (;;) {
                    ถ้า (src == limit) {
                        ถ้า (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        คืน true;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    ถ้า (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        ถ้า (prevCC > cc) {
                            ถ้า (sink == nullptr) {
                                คืน false;
                            }
                            bha_nga;
                        }
                    } มิฉะนั้น {
                        bha_nga;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ถ้า (norm16HasCompBoundaryBefore(n16)) {
                    ถ้า (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    continue;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ถ้า (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            const uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            ถ้า (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*this, s16, errorCode);
        ถ้า (U_FAILURE(errorCode)) {
            bha_nga;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        ถ้า (U_FAILURE(errorCode)) {
            bha_nga;
        }
        ถ้า ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            คืน true;
        }
        recompose(buffer, 0, onlyContiguous);
        ถ้า (!buffer.equals(prevSrc, src)) {
            ถ้า (sink == nullptr) {
                คืน false;
            }
            ถ้า (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                bha_nga;
            }
            ถ้า (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                bha_nga;
            }
            prevBoundary = src;
        }
    }
    คืน true;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(const char16_t *src, const char16_t *limit) const {
    ถ้า (src == limit || *src < minCompNoMaybeCP) {
        คืน true;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    คืน norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(const uint8_t *src, const uint8_t *limit) const {
    ถ้า (src == limit) {
        คืน true;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    คืน norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(const char16_t *start, const char16_t *p,
                                            UBool onlyContiguous) const {
    ถ้า (start == p) {
        คืน true;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    คืน norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(const uint8_t *start, const uint8_t *p,
                                            UBool onlyContiguous) const {
    ถ้า (start == p) {
        คืน true;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    คืน norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

const char16_t *Normalizer2Impl::findPreviousCompBoundary(const char16_t *start, const char16_t *p,
                                                       UBool onlyContiguous) const {
    ขณะ (p != start) {
        const char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        ถ้า (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            คืน codePointLimit;
        }
        ถ้า (hasCompBoundaryBefore(c, norm16)) {
            คืน p;
        }
    }
    คืน p;
}

const char16_t *Normalizer2Impl::findNextCompBoundary(const char16_t *p, const char16_t *limit,
                                                   UBool onlyContiguous) const {
    ขณะ (p != limit) {
        const char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ถ้า (hasCompBoundaryBefore(c, norm16)) {
            คืน codePointStart;
        }
        ถ้า (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            คืน p;
        }
    }
    คืน p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(const char16_t *start, const char16_t *p) const {
    ถ้า (start == p) {
        คืน 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    คืน static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(const uint8_t *start, const uint8_t *p) const {
    ถ้า (start == p) {
        คืน 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    คืน static_cast<uint8_t>(getFCD16(c));
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
#ถ้า (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) const {
    uint16_t norm16=getNorm16(c);
    ถ้า (norm16 >= limitNoNo) {
        ถ้า(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            คืน norm16|(norm16<<8);
        } มิฉะนั้น ถ้า(norm16>=minMaybeYes) {
            คืน 0;
        } มิฉะนั้น ถ้า(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            ถ้า (deltaTrailCC <= DELTA_TCCC_1) {
                คืน deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ถ้า(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        คืน 0;
    }
    // c decomposes, get everything from the variable-length extra data
    const uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    ถ้า(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    คืน norm16;
}
#ถ้า (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) const {
    U_ASSERT(norm16 >= minMaybeNo);
    ถ้า (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        คืน norm16 | (norm16<<8);
    } มิฉะนั้น ถ้า (norm16 >= minMaybeYes) {
        คืน 0;
    }
    // c decomposes, get everything from the variable-length extra data
    const uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    คืน firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
const char16_t *
Normalizer2Impl::makeFCD(const char16_t *src, const char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) const {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    const char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    ถ้า(limit==nullptr) {
        src=copyLowPrefixFromNulTerminated(src, minLcccCP, buffer, errorCode);
        ถ้า(U_FAILURE(errorCode)) {
            คืน src;
        }
        ถ้า(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            ถ้า(prevFCD16>1) {
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

    const char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    สำหรับ(;;) {
        // count code units with lccc==0
        สำหรับ(prevSrc=src; src!=limit;) {
            ถ้า((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } มิฉะนั้น ถ้า(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } มิฉะนั้น {
                ถ้า(U16_IS_LEAD(c)) {
                    char16_t c2;
                    ถ้า((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                ถ้า((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } มิฉะนั้น {
                    bha_nga;
                }
            }
        }
        // copy these code units all at once
        ถ้า(src!=prevSrc) {
            ถ้า(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                bha_nga;
            }
            ถ้า(src==limit) {
                bha_nga;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            ถ้า(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                ถ้า(prev<minDecompNoCP) {
                    prevFCD16=0;
                } มิฉะนั้น {
                    prevFCD16=getFCD16FromNormData(prev);
                    ถ้า(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } มิฉะนั้น {
                const char16_t *p=src-1;
                ถ้า(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                ถ้า(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } มิฉะนั้น ถ้า(src==limit) {
            bha_nga;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        ถ้า((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            ถ้า((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            ถ้า(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                bha_nga;
            }
            prevFCD16=fcd16;
            continue;
        } มิฉะนั้น ถ้า(buffer==nullptr) {
            คืน prevBoundary;  // quick check "no"
        } มิฉะนั้น {
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
            decomposeShort(prevBoundary, src, false, false, *buffer, errorCode);
            ถ้า (U_FAILURE(errorCode)) {
                bha_nga;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    คืน src;
}

void Normalizer2Impl::makeFCDAndAppend(const char16_t *src, const char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safeMiddle,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) const {
    ถ้า(!buffer.isEmpty()) {
        const char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        ถ้า(src!=firstBoundaryInSrc) {
            const char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString middle(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safeMiddle=middle;
            middle.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            const char16_t *middleStart=middle.getBuffer();
            makeFCD(middleStart, middleStart+middle.length(), &buffer, errorCode);
            ถ้า(U_FAILURE(errorCode)) {
                คืน;
            }
            src=firstBoundaryInSrc;
        }
    }
    ถ้า(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } มิฉะนั้น {
        ถ้า(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

const char16_t *Normalizer2Impl::findPreviousFCDBoundary(const char16_t *start, const char16_t *p) const {
    ขณะ(start<p) {
        const char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        ถ้า (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            คืน codePointLimit;
        }
        ถ้า (norm16HasDecompBoundaryBefore(norm16)) {
            คืน p;
        }
    }
    คืน p;
}

const char16_t *Normalizer2Impl::findNextFCDBoundary(const char16_t *p, const char16_t *limit) const {
    ขณะ(p<limit) {
        const char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ถ้า (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            คืน codePointStart;
        }
        ถ้า (norm16HasDecompBoundaryAfter(norm16)) {
            คืน p;
        }
    }
    คืน p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

void CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canonValue = umutablecptrie_get(mutableTrie, decompLead);
    ถ้า((canonValue&(CANON_HAS_SET|CANON_VALUE_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canonValue|origin, &errorCode);
    } มิฉะนั้น {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        ถ้า((canonValue&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(new UnicodeSet, errorCode);
            set=lpSet.getAlias();
            ถ้า(U_FAILURE(errorCode)) {
                คืน;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canonValue & CANON_VALUE_MASK);
            canonValue = (canonValue & ~CANON_VALUE_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canonValue, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            ถ้า (U_FAILURE(errorCode)) {
                คืน;
            }
            ถ้า(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } มิฉะนั้น {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canonValue & CANON_VALUE_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
class InitCanonIterData {
public:
    static void doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
static void U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

void InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = new CanonIterData(errorCode);
    ถ้า (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ถ้า (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        ขณะ ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            ถ้า (value != Normalizer2Impl::INERT) {
                impl->makeCanonIterDataFromNorm16(start, end, value, *impl->fCanonIterData, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->fCanonIterData->mutableTrie, "CanonIterData");
#endif
        impl->fCanonIterData->trie = umutablecptrie_buildImmutable(
            impl->fCanonIterData->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_VALUE_BITS_32, &errorCode);
        umutablecptrie_close(impl->fCanonIterData->mutableTrie);
        impl->fCanonIterData->mutableTrie = nullptr;
    }
    ถ้า (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

void Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, const uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) const {
    ถ้า(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        คืน;
    }
    สำหรับ(UChar32 c=start; c<=end; ++c) {
        uint32_t oldValue = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t newValue=oldValue;
        ถ้า(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            newValue|=CANON_NOT_SEGMENT_STARTER;
            ถ้า(norm16<MIN_NORMAL_MAYBE_YES) {
                newValue|=CANON_HAS_COMPOSITIONS;
            }
        } มิฉะนั้น ถ้า(norm16<minYesNo) {
            newValue|=CANON_HAS_COMPOSITIONS;
        } มิฉะนั้น {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            ถ้า (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            ถ้า (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                const uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                ถ้า((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    ถ้า(c==c2 && (*(mapping-1)&0xff)!=0) {
                        newValue|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                ถ้า(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    ถ้า(norm16_2>=minNoNo) {
                        ขณะ(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2Value = umutablecptrie_get(newData.mutableTrie, c2);
                            ถ้า((c2Value&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2Value|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } มิฉะนั้น {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        ถ้า(newValue!=oldValue) {
            umutablecptrie_set(newData.mutableTrie, c, newValue, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) const {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(this);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    คืน U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::getCanonValue(UChar32 c) const {
    คืน static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

const UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) const {
    คืน *static_cast<const UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) const {
    คืน getCanonValue(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) const {
    int32_t canonValue=getCanonValue(c)&~CANON_NOT_SEGMENT_STARTER;
    ถ้า(canonValue==0) {
        คืน false;
    }
    set.clear();
    int32_t value=canonValue&CANON_VALUE_MASK;
    ถ้า((canonValue&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } มิฉะนั้น ถ้า(value!=0) {
        set.add(value);
    }
    ถ้า((canonValue&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        ถ้า(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_COUNT);
            set.add(syllable, syllable+Hangul::JAMO_VT_COUNT-1);
        } มิฉะนั้น {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    คืน true;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(const UDataSwapper *ds,
            const void *inData, int32_t length, void *outData,
            UErrorCode *pErrorCode) {
    const UDataInfo *pInfo;
    int32_t headerSize;

    const uint8_t *inBytes;
    uint8_t *outBytes;

    const int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ถ้า(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        คืน 0;
    }

    /* check data format and format version */
    pInfo=(const UDataInfo *)((const var_na *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    ถ้า(!(
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
        คืน 0;
    }

    inBytes=(const uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(const int32_t *)inBytes;
    int32_t minIndexesLength;
    ถ้า(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } มิฉะนั้น ถ้า(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } มิฉะนั้น ถ้า(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } มิฉะนั้น {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    ถ้า(length>=0) {
        length-=headerSize;
        ถ้า(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            คืน 0;
        }
    }

    /* read the first few indexes */
    สำหรับ(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    ถ้า(length>=0) {
        ถ้า(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            คืน 0;
        }

        /* copy the data for inaccessible bytes */
        ถ้า(inBytes!=outBytes) {
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

    คืน headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
