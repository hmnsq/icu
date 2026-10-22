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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_NORMALIZATION

#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/edits.h"
#અન્તર્ભાવ "unicode/normalizer2.h"
#અન્તર્ભાવ "unicode/stringoptions.h"
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/umutablecptrie.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "unicode/utf8.h"
#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "normalizer2impl.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucptrie_impl.h"
#અન્તર્ભાવ "uset_imp.h"
#અન્તર્ભાવ "uvector.h"

U_NAMESPACE_BEGIN

નામાકાશ {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
અંતઃરેખ uint8_t leadByteForCP(UChar32 c) {
    જો (c <= 0x7f) {
        પાછા static_cast<uint8_t>(c);
    } નહીંતર જો (c <= 0x7ff) {
        પાછા static_cast<uint8_t>(0xc0 + (c >> 6));
    } નહીંતર {
        // Should not occur because ccc(U+0300)!=0.
        પાછા 0xe0;
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
UChar32 codePointFromValidUTF8(અચલ uint8_t *cpStart, અચલ uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    પસંદગી(cpLimit-cpStart) {
    સ્થિતિ 1:
        પાછા c;
    સ્થિતિ 2:
        પાછા ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    સ્થિતિ 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        પાછા static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    સ્થિતિ 4:
        પાછા ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    મૂળભૂત:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(અચલ uint8_t *start, અચલ uint8_t *p) {
    જો ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        જો (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            પાછા ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    પાછા U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(અચલ uint8_t *src, અચલ uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    જો ((limit - src) >= 3 && *src == 0xe1) {
        જો (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            જો (0xa8 <= t && t <= 0xbf) {
                પાછા t - 0xa7;
            }
        } નહીંતર જો (src[1] == 0x87) {
            uint8_t t = src[2];
            જો (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                પાછા t - (0xa7 - 0x40);
            }
        }
    }
    પાછા -1;
}

શૂન્ય
appendCodePointDelta(અચલ uint8_t *cpStart, અચલ uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    અક્ષર buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    જો (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } નહીંતર {
        int32_t trail = *(cpLimit-1) + delta;
        જો (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            કરો { buffer[length++] = *cpStart++; } જ્યારે (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } નહીંતર {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    જો (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(અચલ Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    જો (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    જો(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા ખોટું;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    જો(start==limit) {
        lastCC=0;
    } નહીંતર {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        જો(lastCC>1) {
            જ્યારે(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    પાછા સાચું;
}

UBool ReorderingBuffer::equals(અચલ char16_t *otherStart, અચલ char16_t *otherLimit) અચલ {
    int32_t length = static_cast<int32_t>(limit - start);
    પાછા
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(અચલ uint8_t *otherStart, અચલ uint8_t *otherLimit) અચલ {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    જો (otherLength < length || (otherLength / 3) > length) {
        પાછા ખોટું;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    માટે (int32_t i = 0, j = 0;;) {
        જો (i >= length) {
            પાછા j >= otherLength;
        } નહીંતર જો (j >= otherLength) {
            પાછા ખોટું;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        જો (c != other) {
            પાછા ખોટું;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    જો(remainingCapacity<2 && !resize(2, errorCode)) {
        પાછા ખોટું;
    }
    જો(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        જો(cc<=1) {
            reorderStart=limit;
        }
    } નહીંતર {
        insert(c, cc);
    }
    remainingCapacity-=2;
    પાછા સાચું;
}

UBool ReorderingBuffer::append(અચલ char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    જો(length==0) {
        પાછા સાચું;
    }
    જો(remainingCapacity<length && !resize(length, errorCode)) {
        પાછા ખોટું;
    }
    remainingCapacity-=length;
    જો(lastCC<=leadCC || leadCC==0) {
        જો(trailCC<=1) {
            reorderStart=limit+length;
        } નહીંતર જો(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        અચલ char16_t *sLimit=s+length;
        કરો { *limit++=*s++; } જ્યારે(s!=sLimit);
        lastCC=trailCC;
    } નહીંતર {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        જ્યારે(i<length) {
            U16_NEXT(s, i, length, c);
            જો(i<length) {
                જો (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } નહીંતર {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } નહીંતર {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    પાછા સાચું;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    જો(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        પાછા ખોટું;
    }
    remainingCapacity-=cpLength;
    જો(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } નહીંતર {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    પાછા સાચું;
}

UBool ReorderingBuffer::appendZeroCC(અચલ char16_t *s, અચલ char16_t *sLimit, UErrorCode &errorCode) {
    જો(s==sLimit) {
        પાછા સાચું;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    જો(remainingCapacity<length && !resize(length, errorCode)) {
        પાછા ખોટું;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    પાછા સાચું;
}

શૂન્ય ReorderingBuffer::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

શૂન્ય ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    જો(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } નહીંતર {
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
    જો(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    જો(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    જો(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા ખોટું;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    પાછા સાચું;
}

શૂન્ય ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    જો(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    જો(reorderStart>=codePointStart) {
        પાછા 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    જો(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    પાછા impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
શૂન્ય ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    માટે(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    કરો {
        *--r=*--q;
    } જ્યારે(codePointLimit!=q);
    writeCodePoint(q, c);
    જો(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

સંરચના CanonIterData : જાહેર UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    શૂન્ય addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

શૂન્ય
Normalizer2Impl::init(અચલ int32_t *inIndexes, અચલ UCPTrie *inTrie,
                      અચલ uint16_t *inExtraData, અચલ uint8_t *inSmallFCD) {
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

સ્થિર uint32_t U_CALLCONV
segmentStarterMapper(અચલ શૂન્ય * /*context*/, uint32_t value) {
    પાછા value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

શૂન્ય
Normalizer2Impl::addLcccChars(UnicodeSet &set) અચલ {
    UChar32 start = 0, end;
    uint32_t norm16;
    જ્યારે ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        જો (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } નહીંતર જો (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            જો (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

શૂન્ય
Normalizer2Impl::addPropertyStarts(અચલ USetAdder *sa, UErrorCode & /*errorCode*/) અચલ {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    જ્યારે ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        જો (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            જ્યારે (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                જો (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    માટે(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_ગણતરી) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

શૂન્ય
Normalizer2Impl::addCanonIterPropertyStarts(અચલ USetAdder *sa, UErrorCode &errorCode) અચલ {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    જો (!ensureCanonIterData(errorCode)) { પાછા; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    જ્યારે ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

અચલ char16_t *
Normalizer2Impl::copy_નીચલું_Prefix_From_Nul_Terminated(અચલ char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) અચલ {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    અચલ char16_t *prevSrc=src;
    char16_t c;
    જ્યારે((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    જો(--src!=prevSrc) {
        જો(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    પાછા src;
}

UnicodeString &
Normalizer2Impl::decompose(અચલ UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        dest.setToBogus();
        પાછા dest;
    }
    અચલ char16_t *sArray=src.getBuffer();
    જો(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        પાછા dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    પાછા dest;
}

શૂન્ય
Normalizer2Impl::decompose(અચલ char16_t *src, અચલ char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) અચલ {
    જો(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    ReorderingBuffer buffer(*આ, dest);
    જો(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
અચલ char16_t *
Normalizer2Impl::decompose(અચલ char16_t *src, અચલ char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) અચલ {
    જો(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા src;
    }
    UChar32 minNoCP=minDecompNoCP;
    જો(limit==nullptr) {
        src=copy_નીચલું_Prefix_From_Nul_Terminated(src, minNoCP, buffer, errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા src;
        }
        limit=u_strchr(src, 0);
    }

    અચલ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    અચલ char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    માટે(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        માટે(prevSrc=src; src!=limit;) {
            જો( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } નહીંતર જો(!U16_IS_LEAD(c)) {
                તોડો;
            } નહીંતર {
                char16_t c2;
                જો((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    જો(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } નહીંતર {
                        તોડો;
                    }
                } નહીંતર {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        જો(src!=prevSrc) {
            જો(buffer!=nullptr) {
                જો(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    તોડો;
                }
            } નહીંતર {
                prevCC=0;
                prevBoundary=src;
            }
        }
        જો(src==limit) {
            તોડો;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        જો(buffer!=nullptr) {
            જો(!decompose(c, norm16, *buffer, errorCode)) {
                તોડો;
            }
        } નહીંતર {
            જો(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                જો(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    જો(cc<=1) {
                        prevBoundary=src;
                    }
                    ચાલુ;
                }
            }
            પાછા prevBoundary;  // "no" or cc out of order
        }
    }
    પાછા src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
અચલ char16_t *
Normalizer2Impl::decomposeShort(અચલ char16_t *src, અચલ char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    જ્યારે(src<limit) {
        જો (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            પાછા src;
        }
        અચલ char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        જો (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            પાછા prevSrc;
        }
        જો(!decompose(c, norm16, buffer, errorCode)) {
            પાછા nullptr;
        }
        જો (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            પાછા src;
        }
    }
    પાછા src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) અચલ {
    // get the decomposition and the lead and trail cc's
    જો (norm16 >= limitNoNo) {
        જો (isMaybeYesOrNonZeroCC(norm16)) {
            પાછા buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } નહીંતર જો (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    જો (norm16 < minYesNo) {
        // c does not decompose
        પાછા buffer.append(c, 0, errorCode);
    } નહીંતર જો(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        પાછા buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    અચલ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    જો(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } નહીંતર {
        leadCC=0;
    }
    પાછા buffer.append(reinterpret_cast<અચલ char16_t*>(mapping) + 1, length, સાચું, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
અચલ uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               અચલ uint8_t *src, અચલ uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) અચલ {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    અચલ uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    માટે (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        અચલ uint8_t *fastStart = src;
        અચલ uint8_t *prevSrc;
        uint16_t norm16 = 0;

        માટે (;;) {
            જો (src == limit) {
                જો (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                પાછા src;
            }
            જો (*src < minNoLead) {
                ++src;
            } નહીંતર {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                જો (!isMostDecompYesAndZeroCC(norm16)) {
                    તોડો;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        જો (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            જો (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                તોડો;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        જો (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            જો (prevCC <= cc || cc == 0) {
                prevCC = cc;
                જો (cc <= 1) {
                    જો (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        તોડો;
                    }
                    prevBoundary = src;
                }
                ચાલુ;
            }
        }
        જો (sink == nullptr) {
            પાછા prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        જો (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            જો (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                તોડો;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*આ, s16, errorCode);
        જો (U_FAILURE(errorCode)) {
            તોડો;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, ખોટું /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        જો (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, ખોટું /* onlyContiguous */,
                                 buffer, errorCode);
        }
        જો (U_FAILURE(errorCode)) {
            તોડો;
        }
        જો ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            તોડો;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        જો (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            જો (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                તોડો;
            }
        } નહીંતર {
            જો (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                તોડો;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    પાછા src;
}

અચલ uint8_t *
Normalizer2Impl::decomposeShort(અચલ uint8_t *src, અચલ uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    જ્યારે (src < limit) {
        અચલ uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        જો (norm16 >= limitNoNo) {
            જો (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                જો (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    પાછા prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                જો (!buffer.append(c, cc, errorCode)) {
                    પાછા nullptr;
                }
                જો (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    પાછા src;
                }
                ચાલુ;
            } નહીંતર જો (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                જો (stopAt != STOP_AT_LIMIT) {
                    પાછા prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } નહીંતર જો (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            પાછા prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        જો (norm16 < minYesNo) {
            જો (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            જો (!buffer.append(c, 0, errorCode)) {
                પાછા nullptr;
            }
        } નહીંતર જો (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            જો (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            જો (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                પાછા nullptr;
            }
        } નહીંતર {
            // The character decomposes, get everything from the variable-length extra data.
            અચલ uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            જો (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } નહીંતર {
                leadCC = 0;
            }
            જો (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                પાછા prevSrc;
            }
            જો (!buffer.append(reinterpret_cast<અચલ char16_t*>(mapping) + 1, length, સાચું, leadCC, trailCC, errorCode)) {
                પાછા nullptr;
            }
        }
        જો ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            પાછા src;
        }
    }
    પાછા src;
}

અચલ char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) અચલ {
    uint16_t norm16;
    જો(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        પાછા nullptr;
    }
    અચલ char16_t *decomp = nullptr;
    જો(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    જો (norm16 < minYesNo) {
        પાછા decomp;
    } નહીંતર જો(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        પાછા buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    અચલ uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    પાછા reinterpret_cast<અચલ char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
અચલ char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) અચલ {
    uint16_t norm16;
    જો(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        પાછા nullptr;
    } નહીંતર જો(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        પાછા buffer;
    } નહીંતર જો(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        પાછા buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    અચલ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    જો(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        અચલ uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        જો(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            પાછા reinterpret_cast<અચલ char16_t*>(rawMapping) - rm0;
        } નહીંતર {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<અચલ char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            પાછા buffer;
        }
    } નહીંતર {
        length=mLength;
        પાછા reinterpret_cast<અચલ char16_t*>(mapping) + 1;
    }
}

શૂન્ય Normalizer2Impl::decomposeAndAppend(અચલ char16_t *src, અચલ char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_મધ્ય,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) અચલ {
    buffer.copyReorderableSuffixTo(safe_મધ્ય);
    જો(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        પાછા;
    }
    // Just merge the strings at the boundary.
    તાર્કિક isFirst = સાચું;
    uint8_t firstCC = 0, prevCC = 0, cc;
    અચલ char16_t *p = src;
    જ્યારે (p != limit) {
        અચલ char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        જો ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            તોડો;
        }
        જો (isFirst) {
            firstCC = cc;
            isFirst = ખોટું;
        }
        prevCC = cc;
    }
    જો(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    જો (buffer.append(src, static_cast<int32_t>(p - src), ખોટું, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) અચલ {
    પાછા c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) અચલ {
    જો (norm16 < minNoNoCompNoMaybeCC) {
        પાછા સાચું;
    }
    જો (norm16 >= limitNoNo) {
        પાછા norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    અચલ uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    પાછા (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) અચલ {
    જો (c < minDecompNoCP) {
        પાછા સાચું;
    }
    જો (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        પાછા સાચું;
    }
    પાછા norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) અચલ {
    જો(norm16 <= minYesNo || isHangulLVT(norm16)) {
        પાછા સાચું;
    }
    જો (norm16 >= limitNoNo) {
        જો (isMaybeYesOrNonZeroCC(norm16)) {
            પાછા norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } નહીંતર જો (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            પાછા (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    અચલ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    જો(firstUnit>0x1ff) {
        પાછા ખોટું;  // trailCC>1
    }
    જો(firstUnit<=0xff) {
        પાછા સાચું;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    પાછા (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(અચલ uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    જો(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        જ્યારે(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        જો(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            જો(firstUnit&COMP_1_TRIPLE) {
                પાછા (static_cast<int32_t>(list[1]) << 16) | list[2];
            } નહીંતર {
                પાછા list[1];
            }
        }
    } નહીંતર {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        માટે(;;) {
            જો(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } નહીંતર જો(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                જો(key2>(secondUnit=list[1])) {
                    જો(firstUnit&COMP_1_LAST_TUPLE) {
                        તોડો;
                    } નહીંતર {
                        list+=3;
                    }
                } નહીંતર જો(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    પાછા (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } નહીંતર {
                    તોડો;
                }
            } નહીંતર {
                તોડો;
            }
        }
    }
    પાછા -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
શૂન્ય Normalizer2Impl::addComposites(અચલ uint16_t *list, UnicodeSet &set) અચલ {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    કરો {
        firstUnit=*list;
        જો((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } નહીંતર {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        જો((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } જ્યારે((firstUnit&COMP_1_LAST_TUPLE)==0);
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
શૂન્ય Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) અચલ {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    જો(p==limit) {
        પાછા;
    }

    char16_t *starter, *pRemove, *q, *r;
    અચલ uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=ખોટું;
    prevCC=0;

    માટે(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        જો( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            જો(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                જો(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    જો(prev<Hangul::JAMO_L_ગણતરી) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_ગણતરી+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_ગણતરી);
                        char16_t t;
                        જો (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_ગણતરી) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        જ્યારે(r<limit) {
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
                જો(p==limit) {
                    તોડો;
                }
                compositionsList=nullptr;
                ચાલુ;
            } નહીંતર જો((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                જો(starterIsSupplementary) {
                    જો(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } નહીંતર {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=ખોટું;
                        q=starter+1;
                        r=q+1;
                        જ્યારે(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } નહીંતર જો(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=સાચું;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    જ્યારે(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } નહીંતર {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                જો(pRemove<p) {
                    q=pRemove;
                    r=p;
                    જ્યારે(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                જો(p==limit) {
                    તોડો;
                }
                // Is the composite a starter that combines forward?
                જો(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } નહીંતર {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                ચાલુ;
            }
        }

        // no combination this time
        prevCC=cc;
        જો(p==limit) {
            તોડો;
        }

        // If c did not combine, then check if it is a starter.
        જો(cc==0) {
            // Found a new starter.
            જો((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                જો(U_IS_BMP(c)) {
                    starterIsSupplementary=ખોટું;
                    starter=p-1;
                } નહીંતર {
                    starterIsSupplementary=સાચું;
                    starter=p-2;
                }
            }
        } નહીંતર જો(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) અચલ {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    અચલ uint16_t *list;
    જો(isInert(norm16)) {
        પાછા U_SENTINEL;
    } નહીંતર જો(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        જો(isJamoL(norm16)) {
            જો (b < Hangul::JAMO_V_BASE) {
                પાછા U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            જો(b<Hangul::JAMO_V_ગણતરી) {
                પાછા
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_ગણતરી+b)*
                     Hangul::JAMO_T_ગણતરી);
            } નહીંતર {
                પાછા U_SENTINEL;
            }
        } નહીંતર જો(isHangulLV(norm16)) {
            જો (b <= Hangul::JAMO_T_BASE) {
               પાછા U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            જો(b<Hangul::JAMO_T_ગણતરી) {  // not b==0!
                પાછા a+b;
            } નહીંતર {
                પાછા U_SENTINEL;
            }
        } નહીંતર {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            જો(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } નહીંતર જો(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        પાછા U_SENTINEL;
    } નહીંતર {
        list=getDataForMaybe(norm16);
        જો(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    જો(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        પાછા U_SENTINEL;
    }
#જો U_SIGNED_જમણું_SHIFT_IS_ARITHMETIC
    પાછા combine(list, b)>>1;
#નહીંતર
    int32_t compositeAndFwd=combine(list, b);
    પાછા compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(અચલ char16_t *src, અચલ char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) અચલ {
    અચલ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    જો(limit==nullptr) {
        src=copy_નીચલું_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા ખોટું;
        }
        limit=u_strchr(src, 0);
        જો (prevBoundary != src) {
            જો (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } નહીંતર {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    માટે (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        અચલ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        માટે (;;) {
            જો (src == limit) {
                જો (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                પાછા સાચું;
            }
            જો( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } નહીંતર {
                prevSrc = src++;
                જો(!U16_IS_LEAD(c)) {
                    તોડો;
                } નહીંતર {
                    char16_t c2;
                    જો(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        જો(!isCompYesAndZeroCC(norm16)) {
                            તોડો;
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
        જો (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            જો (!doCompose) {
                પાછા ખોટું;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            જો (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                જો (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    જો (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        તોડો;
                    }
                    જો(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        તોડો;
                    }
                    prevBoundary = src;
                    ચાલુ;
                }
            } નહીંતર જો (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                જો (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    જો (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        તોડો;
                    }
                    અચલ char16_t *mapping = reinterpret_cast<અચલ char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    જો(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        તોડો;
                    }
                    prevBoundary = src;
                    ચાલુ;
                }
            } નહીંતર જો (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                જો (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    જો (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        તોડો;
                    }
                    prevBoundary = src;
                    ચાલુ;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } નહીંતર જો (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            જો(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                જો(l<Hangul::JAMO_L_ગણતરી) {
                    જો (!doCompose) {
                        પાછા ખોટું;
                    }
                    int32_t t;
                    જો (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_ગણતરી) {
                        // The next character is a Jamo T.
                        ++src;
                    } નહીંતર જો (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } નહીંતર {
                        t = -1;
                    }
                    જો (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_ગણતરી + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_ગણતરી + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        જો (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            તોડો;
                        }
                        જો (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            તોડો;
                        }
                        prevBoundary = src;
                        ચાલુ;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } નહીંતર જો (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                જો (!doCompose) {
                    પાછા ખોટું;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                જો (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    તોડો;
                }
                જો (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    તોડો;
                }
                prevBoundary = src;
                ચાલુ;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } નહીંતર જો (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            જો (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                જો (!doCompose) {
                    પાછા ખોટું;
                }
            } નહીંતર {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                અચલ char16_t *nextSrc;
                uint16_t n16;
                માટે (;;) {
                    જો (src == limit) {
                        જો (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        પાછા સાચું;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    જો (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        જો (prevCC > cc) {
                            જો (!doCompose) {
                                પાછા ખોટું;
                            }
                            તોડો;
                        }
                    } નહીંતર {
                        તોડો;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                જો (norm16HasCompBoundaryBefore(n16)) {
                    જો (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    ચાલુ;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        જો (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            અચલ char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            જો (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        જો (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            તોડો;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, ખોટું /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, સાચું /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        જો (U_FAILURE(errorCode)) {
            તોડો;
        }
        જો ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા સાચું;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        જો(!doCompose) {
            જો(!buffer.equals(prevSrc, src)) {
                પાછા ખોટું;
            }
            buffer.remove();
        }
        prevBoundary=src;
    }
    પાછા સાચું;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
અચલ char16_t *
Normalizer2Impl::composeQuickCheck(અચલ char16_t *src, અચલ char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) અચલ {
    અચલ char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    જો(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_નીચલું_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        જો (prevBoundary != src) {
            જો (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } નહીંતર {
                prevBoundary = --src;
            }
        }
    }

    માટે(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        અચલ char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        માટે (;;) {
            જો(src==limit) {
                પાછા src;
            }
            જો( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } નહીંતર {
                prevSrc = src++;
                જો(!U16_IS_LEAD(c)) {
                    તોડો;
                } નહીંતર {
                    char16_t c2;
                    જો(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        જો(!isCompYesAndZeroCC(norm16)) {
                            તોડો;
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
        જો (prevBoundary != prevSrc) {
            જો (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } નહીંતર {
                અચલ char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                જો (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } નહીંતર {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        જો (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            જો (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } નહીંતર {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                અચલ char16_t *nextSrc;
                માટે (;;) {
                    જો (norm16 < MIN_YES_YES_WITH_CC) {
                        જો (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } નહીંતર {
                            પાછા prevBoundary;
                        }
                    }
                    જો (src == limit) {
                        પાછા src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    જો (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        જો (!(prevCC <= cc || cc == 0)) {
                            તોડો;
                        }
                    } નહીંતર {
                        તોડો;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                જો (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    ચાલુ;
                }
            }
        }
        જો(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        પાછા prevBoundary;
    }
}

શૂન્ય Normalizer2Impl::composeAndAppend(અચલ char16_t *src, અચલ char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_મધ્ય,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) અચલ {
    જો(!buffer.isEmpty()) {
        અચલ char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        જો(src!=firstStarterInSrc) {
            અચલ char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString મધ્ય(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_મધ્ય=મધ્ય;
            મધ્ય.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            અચલ char16_t *મધ્ય_Start=મધ્ય.getBuffer();
            compose(મધ્ય_Start, મધ્ય_Start+મધ્ય.length(), onlyContiguous,
                    સાચું, buffer, errorCode);
            જો(U_FAILURE(errorCode)) {
                પાછા;
            }
            src=firstStarterInSrc;
        }
    }
    જો(doCompose) {
        compose(src, limit, onlyContiguous, સાચું, buffer, errorCode);
    } નહીંતર {
        જો(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             અચલ uint8_t *src, અચલ uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) અચલ {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    અચલ uint8_t *prevBoundary = src;

    માટે (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        અચલ uint8_t *prevSrc;
        uint16_t norm16 = 0;
        માટે (;;) {
            જો (src == limit) {
                જો (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                પાછા સાચું;
            }
            જો (*src < minNoMaybeLead) {
                ++src;
            } નહીંતર {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                જો (!isCompYesAndZeroCC(norm16)) {
                    તોડો;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        જો (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            જો (sink == nullptr) {
                પાછા ખોટું;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            જો (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                જો (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    જો (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        તોડો;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    ચાલુ;
                }
            } નહીંતર જો (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                જો (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    જો (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        તોડો;
                    }
                    અચલ uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    જો (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<અચલ char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        તોડો;
                    }
                    prevBoundary = src;
                    ચાલુ;
                }
            } નહીંતર જો (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                જો (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    જો (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        તોડો;
                    }
                    જો (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    ચાલુ;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } નહીંતર જો (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            જો (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                જો (static_cast<uint32_t>(l) < Hangul::JAMO_L_ગણતરી) {
                    જો (sink == nullptr) {
                        પાછા ખોટું;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    જો (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } નહીંતર જો (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    જો (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_ગણતરી + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_ગણતરી + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        જો (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            તોડો;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        ચાલુ;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } નહીંતર જો (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                જો (sink == nullptr) {
                    પાછા ખોટું;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                જો (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    તોડો;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                ચાલુ;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } નહીંતર જો (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            જો (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                જો (sink == nullptr) {
                    પાછા ખોટું;
                }
            } નહીંતર {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                અચલ uint8_t *nextSrc;
                uint16_t n16;
                માટે (;;) {
                    જો (src == limit) {
                        જો (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        પાછા સાચું;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    જો (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        જો (prevCC > cc) {
                            જો (sink == nullptr) {
                                પાછા ખોટું;
                            }
                            તોડો;
                        }
                    } નહીંતર {
                        તોડો;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                જો (norm16HasCompBoundaryBefore(n16)) {
                    જો (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    ચાલુ;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        જો (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            અચલ uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            જો (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*આ, s16, errorCode);
        જો (U_FAILURE(errorCode)) {
            તોડો;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        જો (U_FAILURE(errorCode)) {
            તોડો;
        }
        જો ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા સાચું;
        }
        recompose(buffer, 0, onlyContiguous);
        જો (!buffer.equals(prevSrc, src)) {
            જો (sink == nullptr) {
                પાછા ખોટું;
            }
            જો (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                તોડો;
            }
            જો (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                તોડો;
            }
            prevBoundary = src;
        }
    }
    પાછા સાચું;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(અચલ char16_t *src, અચલ char16_t *limit) અચલ {
    જો (src == limit || *src < minCompNoMaybeCP) {
        પાછા સાચું;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    પાછા norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(અચલ uint8_t *src, અચલ uint8_t *limit) અચલ {
    જો (src == limit) {
        પાછા સાચું;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    પાછા norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(અચલ char16_t *start, અચલ char16_t *p,
                                            UBool onlyContiguous) અચલ {
    જો (start == p) {
        પાછા સાચું;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    પાછા norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(અચલ uint8_t *start, અચલ uint8_t *p,
                                            UBool onlyContiguous) અચલ {
    જો (start == p) {
        પાછા સાચું;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    પાછા norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

અચલ char16_t *Normalizer2Impl::findPreviousCompBoundary(અચલ char16_t *start, અચલ char16_t *p,
                                                       UBool onlyContiguous) અચલ {
    જ્યારે (p != start) {
        અચલ char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        જો (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            પાછા codePointLimit;
        }
        જો (hasCompBoundaryBefore(c, norm16)) {
            પાછા p;
        }
    }
    પાછા p;
}

અચલ char16_t *Normalizer2Impl::findNextCompBoundary(અચલ char16_t *p, અચલ char16_t *limit,
                                                   UBool onlyContiguous) અચલ {
    જ્યારે (p != limit) {
        અચલ char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        જો (hasCompBoundaryBefore(c, norm16)) {
            પાછા codePointStart;
        }
        જો (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            પાછા p;
        }
    }
    પાછા p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(અચલ char16_t *start, અચલ char16_t *p) અચલ {
    જો (start == p) {
        પાછા 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    પાછા static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(અચલ uint8_t *start, અચલ uint8_t *p) અચલ {
    જો (start == p) {
        પાછા 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    પાછા static_cast<uint8_t>(getFCD16(c));
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
#જો (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) અચલ {
    uint16_t norm16=getNorm16(c);
    જો (norm16 >= limitNoNo) {
        જો(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            પાછા norm16|(norm16<<8);
        } નહીંતર જો(norm16>=minMaybeYes) {
            પાછા 0;
        } નહીંતર જો(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            જો (deltaTrailCC <= DELTA_TCCC_1) {
                પાછા deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    જો(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        પાછા 0;
    }
    // c decomposes, get everything from the variable-length extra data
    અચલ uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    જો(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    પાછા norm16;
}
#જો (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) અચલ {
    U_ASSERT(norm16 >= minMaybeNo);
    જો (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        પાછા norm16 | (norm16<<8);
    } નહીંતર જો (norm16 >= minMaybeYes) {
        પાછા 0;
    }
    // c decomposes, get everything from the variable-length extra data
    અચલ uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    પાછા firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
અચલ char16_t *
Normalizer2Impl::makeFCD(અચલ char16_t *src, અચલ char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) અચલ {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    અચલ char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    જો(limit==nullptr) {
        src=copy_નીચલું_Prefix_From_Nul_Terminated(src, minLcccCP, buffer, errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા src;
        }
        જો(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            જો(prevFCD16>1) {
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

    અચલ char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    માટે(;;) {
        // count code units with lccc==0
        માટે(prevSrc=src; src!=limit;) {
            જો((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } નહીંતર જો(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } નહીંતર {
                જો(U16_IS_LEAD(c)) {
                    char16_t c2;
                    જો((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                જો((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } નહીંતર {
                    તોડો;
                }
            }
        }
        // copy these code units all at once
        જો(src!=prevSrc) {
            જો(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                તોડો;
            }
            જો(src==limit) {
                તોડો;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            જો(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                જો(prev<minDecompNoCP) {
                    prevFCD16=0;
                } નહીંતર {
                    prevFCD16=getFCD16FromNormData(prev);
                    જો(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } નહીંતર {
                અચલ char16_t *p=src-1;
                જો(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                જો(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } નહીંતર જો(src==limit) {
            તોડો;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        જો((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            જો((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            જો(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                તોડો;
            }
            prevFCD16=fcd16;
            ચાલુ;
        } નહીંતર જો(buffer==nullptr) {
            પાછા prevBoundary;  // quick check "no"
        } નહીંતર {
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
            decomposeShort(prevBoundary, src, ખોટું, ખોટું, *buffer, errorCode);
            જો (U_FAILURE(errorCode)) {
                તોડો;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    પાછા src;
}

શૂન્ય Normalizer2Impl::makeFCDAndAppend(અચલ char16_t *src, અચલ char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_મધ્ય,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) અચલ {
    જો(!buffer.isEmpty()) {
        અચલ char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        જો(src!=firstBoundaryInSrc) {
            અચલ char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString મધ્ય(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_મધ્ય=મધ્ય;
            મધ્ય.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            અચલ char16_t *મધ્ય_Start=મધ્ય.getBuffer();
            makeFCD(મધ્ય_Start, મધ્ય_Start+મધ્ય.length(), &buffer, errorCode);
            જો(U_FAILURE(errorCode)) {
                પાછા;
            }
            src=firstBoundaryInSrc;
        }
    }
    જો(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } નહીંતર {
        જો(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

અચલ char16_t *Normalizer2Impl::findPreviousFCDBoundary(અચલ char16_t *start, અચલ char16_t *p) અચલ {
    જ્યારે(start<p) {
        અચલ char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        જો (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            પાછા codePointLimit;
        }
        જો (norm16HasDecompBoundaryBefore(norm16)) {
            પાછા p;
        }
    }
    પાછા p;
}

અચલ char16_t *Normalizer2Impl::findNextFCDBoundary(અચલ char16_t *p, અચલ char16_t *limit) અચલ {
    જ્યારે(p<limit) {
        અચલ char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        જો (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            પાછા codePointStart;
        }
        જો (norm16HasDecompBoundaryAfter(norm16)) {
            પાછા p;
        }
    }
    પાછા p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

શૂન્ય CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_કિંમત = umutablecptrie_get(mutableTrie, decompLead);
    જો((canon_કિંમત&(CANON_HAS_SET|CANON_કિંમત_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_કિંમત|origin, &errorCode);
    } નહીંતર {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        જો((canon_કિંમત&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(નવું UnicodeSet, errorCode);
            set=lpSet.getAlias();
            જો(U_FAILURE(errorCode)) {
                પાછા;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_કિંમત & CANON_કિંમત_MASK);
            canon_કિંમત = (canon_કિંમત & ~CANON_કિંમત_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_કિંમત, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            જો (U_FAILURE(errorCode)) {
                પાછા;
            }
            જો(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } નહીંતર {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_કિંમત & CANON_કિંમત_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
વર્ગ InitCanonIterData {
જાહેર:
    સ્થિર શૂન્ય doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
સ્થિર શૂન્ય U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

શૂન્ય InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = નવું CanonIterData(errorCode);
    જો (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    જો (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        જ્યારે ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            જો (value != Normalizer2Impl::INERT) {
                impl->makeCanonIterDataFromNorm16(start, end, value, *impl->fCanonIterData, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->fCanonIterData->mutableTrie, "CanonIterData");
#endif
        impl->fCanonIterData->trie = umutablecptrie_buildImmutable(
            impl->fCanonIterData->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_કિંમત_BITS_32, &errorCode);
        umutablecptrie_close(impl->fCanonIterData->mutableTrie);
        impl->fCanonIterData->mutableTrie = nullptr;
    }
    જો (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

શૂન્ય Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, અચલ uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) અચલ {
    જો(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        પાછા;
    }
    માટે(UChar32 c=start; c<=end; ++c) {
        uint32_t old_કિંમત = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t new_કિંમત=old_કિંમત;
        જો(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_કિંમત|=CANON_NOT_SEGMENT_STARTER;
            જો(norm16<MIN_NORMAL_MAYBE_YES) {
                new_કિંમત|=CANON_HAS_COMPOSITIONS;
            }
        } નહીંતર જો(norm16<minYesNo) {
            new_કિંમત|=CANON_HAS_COMPOSITIONS;
        } નહીંતર {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            જો (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            જો (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                અચલ uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                જો((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    જો(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_કિંમત|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                જો(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    જો(norm16_2>=minNoNo) {
                        જ્યારે(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2_કિંમત = umutablecptrie_get(newData.mutableTrie, c2);
                            જો((c2_કિંમત&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2_કિંમત|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } નહીંતર {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        જો(new_કિંમત!=old_કિંમત) {
            umutablecptrie_set(newData.mutableTrie, c, new_કિંમત, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) અચલ {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(આ);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    પાછા U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_કિંમત(UChar32 c) અચલ {
    પાછા static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

અચલ UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) અચલ {
    પાછા *static_cast<અચલ UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) અચલ {
    પાછા get_Canon_કિંમત(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) અચલ {
    int32_t canon_કિંમત=get_Canon_કિંમત(c)&~CANON_NOT_SEGMENT_STARTER;
    જો(canon_કિંમત==0) {
        પાછા ખોટું;
    }
    set.clear();
    int32_t value=canon_કિંમત&CANON_કિંમત_MASK;
    જો((canon_કિંમત&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } નહીંતર જો(value!=0) {
        set.add(value);
    }
    જો((canon_કિંમત&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        જો(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_ગણતરી);
            set.add(syllable, syllable+Hangul::JAMO_VT_ગણતરી-1);
        } નહીંતર {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    પાછા સાચું;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(અચલ UDataSwapper *ds,
            અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
            UErrorCode *pErrorCode) {
    અચલ UDataInfo *pInfo;
    int32_t headerSize;

    અચલ uint8_t *inBytes;
    uint8_t *outBytes;

    અચલ int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }

    /* check data format and format version */
    pInfo=(અચલ UDataInfo *)((અચલ અક્ષર *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    જો(!(
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
        પાછા 0;
    }

    inBytes=(અચલ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(અચલ int32_t *)inBytes;
    int32_t minIndexesLength;
    જો(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } નહીંતર જો(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } નહીંતર જો(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } નહીંતર {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    જો(length>=0) {
        length-=headerSize;
        જો(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
    }

    /* read the first few indexes */
    માટે(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    જો(length>=0) {
        જો(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }

        /* copy the data for inaccessible bytes */
        જો(inBytes!=outBytes) {
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

    પાછા headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
