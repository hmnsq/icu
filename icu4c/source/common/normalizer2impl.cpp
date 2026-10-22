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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_NORMALIZATION

#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/edits.h"
#அந்தர்பாவ "unicode/normalizer2.h"
#அந்தர்பாவ "unicode/stringoptions.h"
#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/umutablecptrie.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "normalizer2impl.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucptrie_impl.h"
#அந்தர்பாவ "uset_imp.h"
#அந்தர்பாவ "uvector.h"

U_NAMESPACE_BEGIN

நாமாகாஶ {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
உள்வரி uint8_t leadByteForCP(UChar32 c) {
    எனில் (c <= 0x7f) {
        திரும்பு static_cast<uint8_t>(c);
    } இல்லையெனில் எனில் (c <= 0x7ff) {
        திரும்பு static_cast<uint8_t>(0xc0 + (c >> 6));
    } இல்லையெனில் {
        // Should not occur because ccc(U+0300)!=0.
        திரும்பு 0xe0;
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
UChar32 codePointFromValidUTF8(மாறிலி uint8_t *cpStart, மாறிலி uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    தேர்வு(cpLimit-cpStart) {
    நிலை 1:
        திரும்பு c;
    நிலை 2:
        திரும்பு ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    நிலை 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        திரும்பு static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    நிலை 4:
        திரும்பு ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    இயல்பு:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(மாறிலி uint8_t *start, மாறிலி uint8_t *p) {
    எனில் ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        எனில் (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            திரும்பு ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    திரும்பு U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(மாறிலி uint8_t *src, மாறிலி uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    எனில் ((limit - src) >= 3 && *src == 0xe1) {
        எனில் (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            எனில் (0xa8 <= t && t <= 0xbf) {
                திரும்பு t - 0xa7;
            }
        } இல்லையெனில் எனில் (src[1] == 0x87) {
            uint8_t t = src[2];
            எனில் (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                திரும்பு t - (0xa7 - 0x40);
            }
        }
    }
    திரும்பு -1;
}

வெறுமை
appendCodePointDelta(மாறிலி uint8_t *cpStart, மாறிலி uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    எழுத்து buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    எனில் (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } இல்லையெனில் {
        int32_t trail = *(cpLimit-1) + delta;
        எனில் (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            செய் { buffer[length++] = *cpStart++; } போது (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } இல்லையெனில் {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    எனில் (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(மாறிலி Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    எனில் (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    எனில்(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு பொய்;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    எனில்(start==limit) {
        lastCC=0;
    } இல்லையெனில் {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        எனில்(lastCC>1) {
            போது(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    திரும்பு உண்மை;
}

UBool ReorderingBuffer::equals(மாறிலி char16_t *otherStart, மாறிலி char16_t *otherLimit) மாறிலி {
    int32_t length = static_cast<int32_t>(limit - start);
    திரும்பு
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(மாறிலி uint8_t *otherStart, மாறிலி uint8_t *otherLimit) மாறிலி {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    எனில் (otherLength < length || (otherLength / 3) > length) {
        திரும்பு பொய்;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    க்கு (int32_t i = 0, j = 0;;) {
        எனில் (i >= length) {
            திரும்பு j >= otherLength;
        } இல்லையெனில் எனில் (j >= otherLength) {
            திரும்பு பொய்;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        எனில் (c != other) {
            திரும்பு பொய்;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    எனில்(remainingCapacity<2 && !resize(2, errorCode)) {
        திரும்பு பொய்;
    }
    எனில்(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        எனில்(cc<=1) {
            reorderStart=limit;
        }
    } இல்லையெனில் {
        insert(c, cc);
    }
    remainingCapacity-=2;
    திரும்பு உண்மை;
}

UBool ReorderingBuffer::append(மாறிலி char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    எனில்(length==0) {
        திரும்பு உண்மை;
    }
    எனில்(remainingCapacity<length && !resize(length, errorCode)) {
        திரும்பு பொய்;
    }
    remainingCapacity-=length;
    எனில்(lastCC<=leadCC || leadCC==0) {
        எனில்(trailCC<=1) {
            reorderStart=limit+length;
        } இல்லையெனில் எனில்(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        மாறிலி char16_t *sLimit=s+length;
        செய் { *limit++=*s++; } போது(s!=sLimit);
        lastCC=trailCC;
    } இல்லையெனில் {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        போது(i<length) {
            U16_NEXT(s, i, length, c);
            எனில்(i<length) {
                எனில் (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } இல்லையெனில் {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } இல்லையெனில் {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    திரும்பு உண்மை;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    எனில்(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        திரும்பு பொய்;
    }
    remainingCapacity-=cpLength;
    எனில்(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } இல்லையெனில் {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    திரும்பு உண்மை;
}

UBool ReorderingBuffer::appendZeroCC(மாறிலி char16_t *s, மாறிலி char16_t *sLimit, UErrorCode &errorCode) {
    எனில்(s==sLimit) {
        திரும்பு உண்மை;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    எனில்(remainingCapacity<length && !resize(length, errorCode)) {
        திரும்பு பொய்;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    திரும்பு உண்மை;
}

வெறுமை ReorderingBuffer::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

வெறுமை ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    எனில்(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } இல்லையெனில் {
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
    எனில்(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    எனில்(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    எனில்(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு பொய்;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    திரும்பு உண்மை;
}

வெறுமை ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    எனில்(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    எனில்(reorderStart>=codePointStart) {
        திரும்பு 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    எனில்(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    திரும்பு impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
வெறுமை ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    க்கு(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    செய் {
        *--r=*--q;
    } போது(codePointLimit!=q);
    writeCodePoint(q, c);
    எனில்(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

கட்டமைப்பு CanonIterData : பொது UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    வெறுமை addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

வெறுமை
Normalizer2Impl::init(மாறிலி int32_t *inIndexes, மாறிலி UCPTrie *inTrie,
                      மாறிலி uint16_t *inExtraData, மாறிலி uint8_t *inSmallFCD) {
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

நிலையான uint32_t U_CALLCONV
segmentStarterMapper(மாறிலி வெறுமை * /*context*/, uint32_t value) {
    திரும்பு value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

வெறுமை
Normalizer2Impl::addLcccChars(UnicodeSet &set) மாறிலி {
    UChar32 start = 0, end;
    uint32_t norm16;
    போது ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        எனில் (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } இல்லையெனில் எனில் (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            எனில் (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

வெறுமை
Normalizer2Impl::addPropertyStarts(மாறிலி USetAdder *sa, UErrorCode & /*errorCode*/) மாறிலி {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    போது ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        எனில் (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            போது (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                எனில் (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    க்கு(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_மொத்தம்) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

வெறுமை
Normalizer2Impl::addCanonIterPropertyStarts(மாறிலி USetAdder *sa, UErrorCode &errorCode) மாறிலி {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    எனில் (!ensureCanonIterData(errorCode)) { திரும்பு; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    போது ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

மாறிலி char16_t *
Normalizer2Impl::copy_கீழ்_Prefix_From_Nul_Terminated(மாறிலி char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) மாறிலி {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    மாறிலி char16_t *prevSrc=src;
    char16_t c;
    போது((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    எனில்(--src!=prevSrc) {
        எனில்(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    திரும்பு src;
}

UnicodeString &
Normalizer2Impl::decompose(மாறிலி UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        dest.setToBogus();
        திரும்பு dest;
    }
    மாறிலி char16_t *sArray=src.getBuffer();
    எனில்(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        திரும்பு dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    திரும்பு dest;
}

வெறுமை
Normalizer2Impl::decompose(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) மாறிலி {
    எனில்(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    ReorderingBuffer buffer(*இது, dest);
    எனில்(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
மாறிலி char16_t *
Normalizer2Impl::decompose(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) மாறிலி {
    எனில்(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு src;
    }
    UChar32 minNoCP=minDecompNoCP;
    எனில்(limit==nullptr) {
        src=copy_கீழ்_Prefix_From_Nul_Terminated(src, minNoCP, buffer, errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு src;
        }
        limit=u_strchr(src, 0);
    }

    மாறிலி char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    மாறிலி char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    க்கு(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        க்கு(prevSrc=src; src!=limit;) {
            எனில்( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } இல்லையெனில் எனில்(!U16_IS_LEAD(c)) {
                நிறுத்து;
            } இல்லையெனில் {
                char16_t c2;
                எனில்((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    எனில்(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } இல்லையெனில் {
                        நிறுத்து;
                    }
                } இல்லையெனில் {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        எனில்(src!=prevSrc) {
            எனில்(buffer!=nullptr) {
                எனில்(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    நிறுத்து;
                }
            } இல்லையெனில் {
                prevCC=0;
                prevBoundary=src;
            }
        }
        எனில்(src==limit) {
            நிறுத்து;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        எனில்(buffer!=nullptr) {
            எனில்(!decompose(c, norm16, *buffer, errorCode)) {
                நிறுத்து;
            }
        } இல்லையெனில் {
            எனில்(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                எனில்(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    எனில்(cc<=1) {
                        prevBoundary=src;
                    }
                    தொடர்;
                }
            }
            திரும்பு prevBoundary;  // "no" or cc out of order
        }
    }
    திரும்பு src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
மாறிலி char16_t *
Normalizer2Impl::decomposeShort(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    போது(src<limit) {
        எனில் (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            திரும்பு src;
        }
        மாறிலி char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        எனில் (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            திரும்பு prevSrc;
        }
        எனில்(!decompose(c, norm16, buffer, errorCode)) {
            திரும்பு nullptr;
        }
        எனில் (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            திரும்பு src;
        }
    }
    திரும்பு src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) மாறிலி {
    // get the decomposition and the lead and trail cc's
    எனில் (norm16 >= limitNoNo) {
        எனில் (isMaybeYesOrNonZeroCC(norm16)) {
            திரும்பு buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } இல்லையெனில் எனில் (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    எனில் (norm16 < minYesNo) {
        // c does not decompose
        திரும்பு buffer.append(c, 0, errorCode);
    } இல்லையெனில் எனில்(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        திரும்பு buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    மாறிலி uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    எனில்(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } இல்லையெனில் {
        leadCC=0;
    }
    திரும்பு buffer.append(reinterpret_cast<மாறிலி char16_t*>(mapping) + 1, length, உண்மை, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
மாறிலி uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               மாறிலி uint8_t *src, மாறிலி uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) மாறிலி {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    மாறிலி uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    க்கு (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        மாறிலி uint8_t *fastStart = src;
        மாறிலி uint8_t *prevSrc;
        uint16_t norm16 = 0;

        க்கு (;;) {
            எனில் (src == limit) {
                எனில் (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                திரும்பு src;
            }
            எனில் (*src < minNoLead) {
                ++src;
            } இல்லையெனில் {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                எனில் (!isMostDecompYesAndZeroCC(norm16)) {
                    நிறுத்து;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        எனில் (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            எனில் (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                நிறுத்து;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        எனில் (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            எனில் (prevCC <= cc || cc == 0) {
                prevCC = cc;
                எனில் (cc <= 1) {
                    எனில் (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        நிறுத்து;
                    }
                    prevBoundary = src;
                }
                தொடர்;
            }
        }
        எனில் (sink == nullptr) {
            திரும்பு prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        எனில் (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            எனில் (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                நிறுத்து;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*இது, s16, errorCode);
        எனில் (U_FAILURE(errorCode)) {
            நிறுத்து;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, பொய் /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        எனில் (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, பொய் /* onlyContiguous */,
                                 buffer, errorCode);
        }
        எனில் (U_FAILURE(errorCode)) {
            நிறுத்து;
        }
        எனில் ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            நிறுத்து;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        எனில் (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            எனில் (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                நிறுத்து;
            }
        } இல்லையெனில் {
            எனில் (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                நிறுத்து;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    திரும்பு src;
}

மாறிலி uint8_t *
Normalizer2Impl::decomposeShort(மாறிலி uint8_t *src, மாறிலி uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    போது (src < limit) {
        மாறிலி uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        எனில் (norm16 >= limitNoNo) {
            எனில் (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                எனில் (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    திரும்பு prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                எனில் (!buffer.append(c, cc, errorCode)) {
                    திரும்பு nullptr;
                }
                எனில் (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    திரும்பு src;
                }
                தொடர்;
            } இல்லையெனில் எனில் (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                எனில் (stopAt != STOP_AT_LIMIT) {
                    திரும்பு prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } இல்லையெனில் எனில் (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            திரும்பு prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        எனில் (norm16 < minYesNo) {
            எனில் (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            எனில் (!buffer.append(c, 0, errorCode)) {
                திரும்பு nullptr;
            }
        } இல்லையெனில் எனில் (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            எனில் (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            எனில் (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                திரும்பு nullptr;
            }
        } இல்லையெனில் {
            // The character decomposes, get everything from the variable-length extra data.
            மாறிலி uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            எனில் (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } இல்லையெனில் {
                leadCC = 0;
            }
            எனில் (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                திரும்பு prevSrc;
            }
            எனில் (!buffer.append(reinterpret_cast<மாறிலி char16_t*>(mapping) + 1, length, உண்மை, leadCC, trailCC, errorCode)) {
                திரும்பு nullptr;
            }
        }
        எனில் ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            திரும்பு src;
        }
    }
    திரும்பு src;
}

மாறிலி char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) மாறிலி {
    uint16_t norm16;
    எனில்(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        திரும்பு nullptr;
    }
    மாறிலி char16_t *decomp = nullptr;
    எனில்(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    எனில் (norm16 < minYesNo) {
        திரும்பு decomp;
    } இல்லையெனில் எனில்(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        திரும்பு buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    மாறிலி uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    திரும்பு reinterpret_cast<மாறிலி char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
மாறிலி char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) மாறிலி {
    uint16_t norm16;
    எனில்(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        திரும்பு nullptr;
    } இல்லையெனில் எனில்(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        திரும்பு buffer;
    } இல்லையெனில் எனில்(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        திரும்பு buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    மாறிலி uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    எனில்(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        மாறிலி uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        எனில்(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            திரும்பு reinterpret_cast<மாறிலி char16_t*>(rawMapping) - rm0;
        } இல்லையெனில் {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<மாறிலி char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            திரும்பு buffer;
        }
    } இல்லையெனில் {
        length=mLength;
        திரும்பு reinterpret_cast<மாறிலி char16_t*>(mapping) + 1;
    }
}

வெறுமை Normalizer2Impl::decomposeAndAppend(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_நடு,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) மாறிலி {
    buffer.copyReorderableSuffixTo(safe_நடு);
    எனில்(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        திரும்பு;
    }
    // Just merge the strings at the boundary.
    தருக்க isFirst = உண்மை;
    uint8_t firstCC = 0, prevCC = 0, cc;
    மாறிலி char16_t *p = src;
    போது (p != limit) {
        மாறிலி char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        எனில் ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            நிறுத்து;
        }
        எனில் (isFirst) {
            firstCC = cc;
            isFirst = பொய்;
        }
        prevCC = cc;
    }
    எனில்(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    எனில் (buffer.append(src, static_cast<int32_t>(p - src), பொய், firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) மாறிலி {
    திரும்பு c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) மாறிலி {
    எனில் (norm16 < minNoNoCompNoMaybeCC) {
        திரும்பு உண்மை;
    }
    எனில் (norm16 >= limitNoNo) {
        திரும்பு norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    மாறிலி uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    திரும்பு (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) மாறிலி {
    எனில் (c < minDecompNoCP) {
        திரும்பு உண்மை;
    }
    எனில் (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        திரும்பு உண்மை;
    }
    திரும்பு norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) மாறிலி {
    எனில்(norm16 <= minYesNo || isHangulLVT(norm16)) {
        திரும்பு உண்மை;
    }
    எனில் (norm16 >= limitNoNo) {
        எனில் (isMaybeYesOrNonZeroCC(norm16)) {
            திரும்பு norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } இல்லையெனில் எனில் (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            திரும்பு (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    மாறிலி uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    எனில்(firstUnit>0x1ff) {
        திரும்பு பொய்;  // trailCC>1
    }
    எனில்(firstUnit<=0xff) {
        திரும்பு உண்மை;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    திரும்பு (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(மாறிலி uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    எனில்(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        போது(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        எனில்(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            எனில்(firstUnit&COMP_1_TRIPLE) {
                திரும்பு (static_cast<int32_t>(list[1]) << 16) | list[2];
            } இல்லையெனில் {
                திரும்பு list[1];
            }
        }
    } இல்லையெனில் {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        க்கு(;;) {
            எனில்(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } இல்லையெனில் எனில்(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                எனில்(key2>(secondUnit=list[1])) {
                    எனில்(firstUnit&COMP_1_LAST_TUPLE) {
                        நிறுத்து;
                    } இல்லையெனில் {
                        list+=3;
                    }
                } இல்லையெனில் எனில்(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    திரும்பு (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } இல்லையெனில் {
                    நிறுத்து;
                }
            } இல்லையெனில் {
                நிறுத்து;
            }
        }
    }
    திரும்பு -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
வெறுமை Normalizer2Impl::addComposites(மாறிலி uint16_t *list, UnicodeSet &set) மாறிலி {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    செய் {
        firstUnit=*list;
        எனில்((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } இல்லையெனில் {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        எனில்((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } போது((firstUnit&COMP_1_LAST_TUPLE)==0);
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
வெறுமை Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) மாறிலி {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    எனில்(p==limit) {
        திரும்பு;
    }

    char16_t *starter, *pRemove, *q, *r;
    மாறிலி uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=பொய்;
    prevCC=0;

    க்கு(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        எனில்( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            எனில்(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                எனில்(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    எனில்(prev<Hangul::JAMO_L_மொத்தம்) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_மொத்தம்+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_மொத்தம்);
                        char16_t t;
                        எனில் (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_மொத்தம்) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        போது(r<limit) {
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
                எனில்(p==limit) {
                    நிறுத்து;
                }
                compositionsList=nullptr;
                தொடர்;
            } இல்லையெனில் எனில்((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                எனில்(starterIsSupplementary) {
                    எனில்(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } இல்லையெனில் {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=பொய்;
                        q=starter+1;
                        r=q+1;
                        போது(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } இல்லையெனில் எனில்(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=உண்மை;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    போது(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } இல்லையெனில் {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                எனில்(pRemove<p) {
                    q=pRemove;
                    r=p;
                    போது(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                எனில்(p==limit) {
                    நிறுத்து;
                }
                // Is the composite a starter that combines forward?
                எனில்(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } இல்லையெனில் {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                தொடர்;
            }
        }

        // no combination this time
        prevCC=cc;
        எனில்(p==limit) {
            நிறுத்து;
        }

        // If c did not combine, then check if it is a starter.
        எனில்(cc==0) {
            // Found a new starter.
            எனில்((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                எனில்(U_IS_BMP(c)) {
                    starterIsSupplementary=பொய்;
                    starter=p-1;
                } இல்லையெனில் {
                    starterIsSupplementary=உண்மை;
                    starter=p-2;
                }
            }
        } இல்லையெனில் எனில்(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) மாறிலி {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    மாறிலி uint16_t *list;
    எனில்(isInert(norm16)) {
        திரும்பு U_SENTINEL;
    } இல்லையெனில் எனில்(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        எனில்(isJamoL(norm16)) {
            எனில் (b < Hangul::JAMO_V_BASE) {
                திரும்பு U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            எனில்(b<Hangul::JAMO_V_மொத்தம்) {
                திரும்பு
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_மொத்தம்+b)*
                     Hangul::JAMO_T_மொத்தம்);
            } இல்லையெனில் {
                திரும்பு U_SENTINEL;
            }
        } இல்லையெனில் எனில்(isHangulLV(norm16)) {
            எனில் (b <= Hangul::JAMO_T_BASE) {
               திரும்பு U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            எனில்(b<Hangul::JAMO_T_மொத்தம்) {  // not b==0!
                திரும்பு a+b;
            } இல்லையெனில் {
                திரும்பு U_SENTINEL;
            }
        } இல்லையெனில் {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            எனில்(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } இல்லையெனில் எனில்(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        திரும்பு U_SENTINEL;
    } இல்லையெனில் {
        list=getDataForMaybe(norm16);
        எனில்(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    எனில்(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        திரும்பு U_SENTINEL;
    }
#எனில் U_SIGNED_வலது_SHIFT_IS_ARITHMETIC
    திரும்பு combine(list, b)>>1;
#இல்லையெனில்
    int32_t compositeAndFwd=combine(list, b);
    திரும்பு compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) மாறிலி {
    மாறிலி char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    எனில்(limit==nullptr) {
        src=copy_கீழ்_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு பொய்;
        }
        limit=u_strchr(src, 0);
        எனில் (prevBoundary != src) {
            எனில் (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } இல்லையெனில் {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    க்கு (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        மாறிலி char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        க்கு (;;) {
            எனில் (src == limit) {
                எனில் (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                திரும்பு உண்மை;
            }
            எனில்( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } இல்லையெனில் {
                prevSrc = src++;
                எனில்(!U16_IS_LEAD(c)) {
                    நிறுத்து;
                } இல்லையெனில் {
                    char16_t c2;
                    எனில்(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        எனில்(!isCompYesAndZeroCC(norm16)) {
                            நிறுத்து;
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
        எனில் (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            எனில் (!doCompose) {
                திரும்பு பொய்;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            எனில் (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                எனில் (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    எனில் (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        நிறுத்து;
                    }
                    எனில்(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        நிறுத்து;
                    }
                    prevBoundary = src;
                    தொடர்;
                }
            } இல்லையெனில் எனில் (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                எனில் (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    எனில் (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        நிறுத்து;
                    }
                    மாறிலி char16_t *mapping = reinterpret_cast<மாறிலி char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    எனில்(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        நிறுத்து;
                    }
                    prevBoundary = src;
                    தொடர்;
                }
            } இல்லையெனில் எனில் (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                எனில் (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    எனில் (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        நிறுத்து;
                    }
                    prevBoundary = src;
                    தொடர்;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } இல்லையெனில் எனில் (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            எனில்(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                எனில்(l<Hangul::JAMO_L_மொத்தம்) {
                    எனில் (!doCompose) {
                        திரும்பு பொய்;
                    }
                    int32_t t;
                    எனில் (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_மொத்தம்) {
                        // The next character is a Jamo T.
                        ++src;
                    } இல்லையெனில் எனில் (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } இல்லையெனில் {
                        t = -1;
                    }
                    எனில் (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_மொத்தம் + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_மொத்தம் + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        எனில் (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            நிறுத்து;
                        }
                        எனில் (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            நிறுத்து;
                        }
                        prevBoundary = src;
                        தொடர்;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } இல்லையெனில் எனில் (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                எனில் (!doCompose) {
                    திரும்பு பொய்;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                எனில் (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    நிறுத்து;
                }
                எனில் (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    நிறுத்து;
                }
                prevBoundary = src;
                தொடர்;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } இல்லையெனில் எனில் (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            எனில் (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                எனில் (!doCompose) {
                    திரும்பு பொய்;
                }
            } இல்லையெனில் {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                மாறிலி char16_t *nextSrc;
                uint16_t n16;
                க்கு (;;) {
                    எனில் (src == limit) {
                        எனில் (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        திரும்பு உண்மை;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    எனில் (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        எனில் (prevCC > cc) {
                            எனில் (!doCompose) {
                                திரும்பு பொய்;
                            }
                            நிறுத்து;
                        }
                    } இல்லையெனில் {
                        நிறுத்து;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                எனில் (norm16HasCompBoundaryBefore(n16)) {
                    எனில் (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    தொடர்;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        எனில் (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            மாறிலி char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            எனில் (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        எனில் (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            நிறுத்து;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, பொய் /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, உண்மை /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        எனில் (U_FAILURE(errorCode)) {
            நிறுத்து;
        }
        எனில் ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு உண்மை;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        எனில்(!doCompose) {
            எனில்(!buffer.equals(prevSrc, src)) {
                திரும்பு பொய்;
            }
            buffer.remove();
        }
        prevBoundary=src;
    }
    திரும்பு உண்மை;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
மாறிலி char16_t *
Normalizer2Impl::composeQuickCheck(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) மாறிலி {
    மாறிலி char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    எனில்(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_கீழ்_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        எனில் (prevBoundary != src) {
            எனில் (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } இல்லையெனில் {
                prevBoundary = --src;
            }
        }
    }

    க்கு(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        மாறிலி char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        க்கு (;;) {
            எனில்(src==limit) {
                திரும்பு src;
            }
            எனில்( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } இல்லையெனில் {
                prevSrc = src++;
                எனில்(!U16_IS_LEAD(c)) {
                    நிறுத்து;
                } இல்லையெனில் {
                    char16_t c2;
                    எனில்(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        எனில்(!isCompYesAndZeroCC(norm16)) {
                            நிறுத்து;
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
        எனில் (prevBoundary != prevSrc) {
            எனில் (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } இல்லையெனில் {
                மாறிலி char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                எனில் (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } இல்லையெனில் {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        எனில் (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            எனில் (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } இல்லையெனில் {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                மாறிலி char16_t *nextSrc;
                க்கு (;;) {
                    எனில் (norm16 < MIN_YES_YES_WITH_CC) {
                        எனில் (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } இல்லையெனில் {
                            திரும்பு prevBoundary;
                        }
                    }
                    எனில் (src == limit) {
                        திரும்பு src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    எனில் (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        எனில் (!(prevCC <= cc || cc == 0)) {
                            நிறுத்து;
                        }
                    } இல்லையெனில் {
                        நிறுத்து;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                எனில் (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    தொடர்;
                }
            }
        }
        எனில்(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        திரும்பு prevBoundary;
    }
}

வெறுமை Normalizer2Impl::composeAndAppend(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_நடு,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) மாறிலி {
    எனில்(!buffer.isEmpty()) {
        மாறிலி char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        எனில்(src!=firstStarterInSrc) {
            மாறிலி char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString நடு(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_நடு=நடு;
            நடு.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            மாறிலி char16_t *நடு_Start=நடு.getBuffer();
            compose(நடு_Start, நடு_Start+நடு.length(), onlyContiguous,
                    உண்மை, buffer, errorCode);
            எனில்(U_FAILURE(errorCode)) {
                திரும்பு;
            }
            src=firstStarterInSrc;
        }
    }
    எனில்(doCompose) {
        compose(src, limit, onlyContiguous, உண்மை, buffer, errorCode);
    } இல்லையெனில் {
        எனில்(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             மாறிலி uint8_t *src, மாறிலி uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) மாறிலி {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    மாறிலி uint8_t *prevBoundary = src;

    க்கு (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        மாறிலி uint8_t *prevSrc;
        uint16_t norm16 = 0;
        க்கு (;;) {
            எனில் (src == limit) {
                எனில் (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                திரும்பு உண்மை;
            }
            எனில் (*src < minNoMaybeLead) {
                ++src;
            } இல்லையெனில் {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                எனில் (!isCompYesAndZeroCC(norm16)) {
                    நிறுத்து;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        எனில் (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            எனில் (sink == nullptr) {
                திரும்பு பொய்;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            எனில் (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                எனில் (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    எனில் (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        நிறுத்து;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    தொடர்;
                }
            } இல்லையெனில் எனில் (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                எனில் (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    எனில் (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        நிறுத்து;
                    }
                    மாறிலி uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    எனில் (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<மாறிலி char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        நிறுத்து;
                    }
                    prevBoundary = src;
                    தொடர்;
                }
            } இல்லையெனில் எனில் (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                எனில் (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    எனில் (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        நிறுத்து;
                    }
                    எனில் (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    தொடர்;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } இல்லையெனில் எனில் (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            எனில் (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                எனில் (static_cast<uint32_t>(l) < Hangul::JAMO_L_மொத்தம்) {
                    எனில் (sink == nullptr) {
                        திரும்பு பொய்;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    எனில் (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } இல்லையெனில் எனில் (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    எனில் (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_மொத்தம் + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_மொத்தம் + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        எனில் (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            நிறுத்து;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        தொடர்;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } இல்லையெனில் எனில் (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                எனில் (sink == nullptr) {
                    திரும்பு பொய்;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                எனில் (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    நிறுத்து;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                தொடர்;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } இல்லையெனில் எனில் (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            எனில் (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                எனில் (sink == nullptr) {
                    திரும்பு பொய்;
                }
            } இல்லையெனில் {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                மாறிலி uint8_t *nextSrc;
                uint16_t n16;
                க்கு (;;) {
                    எனில் (src == limit) {
                        எனில் (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        திரும்பு உண்மை;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    எனில் (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        எனில் (prevCC > cc) {
                            எனில் (sink == nullptr) {
                                திரும்பு பொய்;
                            }
                            நிறுத்து;
                        }
                    } இல்லையெனில் {
                        நிறுத்து;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                எனில் (norm16HasCompBoundaryBefore(n16)) {
                    எனில் (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    தொடர்;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        எனில் (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            மாறிலி uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            எனில் (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*இது, s16, errorCode);
        எனில் (U_FAILURE(errorCode)) {
            நிறுத்து;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        எனில் (U_FAILURE(errorCode)) {
            நிறுத்து;
        }
        எனில் ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு உண்மை;
        }
        recompose(buffer, 0, onlyContiguous);
        எனில் (!buffer.equals(prevSrc, src)) {
            எனில் (sink == nullptr) {
                திரும்பு பொய்;
            }
            எனில் (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                நிறுத்து;
            }
            எனில் (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                நிறுத்து;
            }
            prevBoundary = src;
        }
    }
    திரும்பு உண்மை;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(மாறிலி char16_t *src, மாறிலி char16_t *limit) மாறிலி {
    எனில் (src == limit || *src < minCompNoMaybeCP) {
        திரும்பு உண்மை;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    திரும்பு norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(மாறிலி uint8_t *src, மாறிலி uint8_t *limit) மாறிலி {
    எனில் (src == limit) {
        திரும்பு உண்மை;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    திரும்பு norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(மாறிலி char16_t *start, மாறிலி char16_t *p,
                                            UBool onlyContiguous) மாறிலி {
    எனில் (start == p) {
        திரும்பு உண்மை;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    திரும்பு norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(மாறிலி uint8_t *start, மாறிலி uint8_t *p,
                                            UBool onlyContiguous) மாறிலி {
    எனில் (start == p) {
        திரும்பு உண்மை;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    திரும்பு norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

மாறிலி char16_t *Normalizer2Impl::findPreviousCompBoundary(மாறிலி char16_t *start, மாறிலி char16_t *p,
                                                       UBool onlyContiguous) மாறிலி {
    போது (p != start) {
        மாறிலி char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        எனில் (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            திரும்பு codePointLimit;
        }
        எனில் (hasCompBoundaryBefore(c, norm16)) {
            திரும்பு p;
        }
    }
    திரும்பு p;
}

மாறிலி char16_t *Normalizer2Impl::findNextCompBoundary(மாறிலி char16_t *p, மாறிலி char16_t *limit,
                                                   UBool onlyContiguous) மாறிலி {
    போது (p != limit) {
        மாறிலி char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        எனில் (hasCompBoundaryBefore(c, norm16)) {
            திரும்பு codePointStart;
        }
        எனில் (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            திரும்பு p;
        }
    }
    திரும்பு p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(மாறிலி char16_t *start, மாறிலி char16_t *p) மாறிலி {
    எனில் (start == p) {
        திரும்பு 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    திரும்பு static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(மாறிலி uint8_t *start, மாறிலி uint8_t *p) மாறிலி {
    எனில் (start == p) {
        திரும்பு 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    திரும்பு static_cast<uint8_t>(getFCD16(c));
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
#எனில் (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) மாறிலி {
    uint16_t norm16=getNorm16(c);
    எனில் (norm16 >= limitNoNo) {
        எனில்(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            திரும்பு norm16|(norm16<<8);
        } இல்லையெனில் எனில்(norm16>=minMaybeYes) {
            திரும்பு 0;
        } இல்லையெனில் எனில்(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            எனில் (deltaTrailCC <= DELTA_TCCC_1) {
                திரும்பு deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    எனில்(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        திரும்பு 0;
    }
    // c decomposes, get everything from the variable-length extra data
    மாறிலி uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    எனில்(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    திரும்பு norm16;
}
#எனில் (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) மாறிலி {
    U_ASSERT(norm16 >= minMaybeNo);
    எனில் (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        திரும்பு norm16 | (norm16<<8);
    } இல்லையெனில் எனில் (norm16 >= minMaybeYes) {
        திரும்பு 0;
    }
    // c decomposes, get everything from the variable-length extra data
    மாறிலி uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    திரும்பு firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
மாறிலி char16_t *
Normalizer2Impl::makeFCD(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) மாறிலி {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    மாறிலி char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    எனில்(limit==nullptr) {
        src=copy_கீழ்_Prefix_From_Nul_Terminated(src, minLcccCP, buffer, errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு src;
        }
        எனில்(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            எனில்(prevFCD16>1) {
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

    மாறிலி char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    க்கு(;;) {
        // count code units with lccc==0
        க்கு(prevSrc=src; src!=limit;) {
            எனில்((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } இல்லையெனில் எனில்(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } இல்லையெனில் {
                எனில்(U16_IS_LEAD(c)) {
                    char16_t c2;
                    எனில்((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                எனில்((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } இல்லையெனில் {
                    நிறுத்து;
                }
            }
        }
        // copy these code units all at once
        எனில்(src!=prevSrc) {
            எனில்(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                நிறுத்து;
            }
            எனில்(src==limit) {
                நிறுத்து;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            எனில்(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                எனில்(prev<minDecompNoCP) {
                    prevFCD16=0;
                } இல்லையெனில் {
                    prevFCD16=getFCD16FromNormData(prev);
                    எனில்(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } இல்லையெனில் {
                மாறிலி char16_t *p=src-1;
                எனில்(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                எனில்(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } இல்லையெனில் எனில்(src==limit) {
            நிறுத்து;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        எனில்((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            எனில்((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            எனில்(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                நிறுத்து;
            }
            prevFCD16=fcd16;
            தொடர்;
        } இல்லையெனில் எனில்(buffer==nullptr) {
            திரும்பு prevBoundary;  // quick check "no"
        } இல்லையெனில் {
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
            decomposeShort(prevBoundary, src, பொய், பொய், *buffer, errorCode);
            எனில் (U_FAILURE(errorCode)) {
                நிறுத்து;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    திரும்பு src;
}

வெறுமை Normalizer2Impl::makeFCDAndAppend(மாறிலி char16_t *src, மாறிலி char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_நடு,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) மாறிலி {
    எனில்(!buffer.isEmpty()) {
        மாறிலி char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        எனில்(src!=firstBoundaryInSrc) {
            மாறிலி char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString நடு(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_நடு=நடு;
            நடு.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            மாறிலி char16_t *நடு_Start=நடு.getBuffer();
            makeFCD(நடு_Start, நடு_Start+நடு.length(), &buffer, errorCode);
            எனில்(U_FAILURE(errorCode)) {
                திரும்பு;
            }
            src=firstBoundaryInSrc;
        }
    }
    எனில்(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } இல்லையெனில் {
        எனில்(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

மாறிலி char16_t *Normalizer2Impl::findPreviousFCDBoundary(மாறிலி char16_t *start, மாறிலி char16_t *p) மாறிலி {
    போது(start<p) {
        மாறிலி char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        எனில் (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            திரும்பு codePointLimit;
        }
        எனில் (norm16HasDecompBoundaryBefore(norm16)) {
            திரும்பு p;
        }
    }
    திரும்பு p;
}

மாறிலி char16_t *Normalizer2Impl::findNextFCDBoundary(மாறிலி char16_t *p, மாறிலி char16_t *limit) மாறிலி {
    போது(p<limit) {
        மாறிலி char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        எனில் (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            திரும்பு codePointStart;
        }
        எனில் (norm16HasDecompBoundaryAfter(norm16)) {
            திரும்பு p;
        }
    }
    திரும்பு p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

வெறுமை CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_மதிப்பு = umutablecptrie_get(mutableTrie, decompLead);
    எனில்((canon_மதிப்பு&(CANON_HAS_SET|CANON_மதிப்பு_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_மதிப்பு|origin, &errorCode);
    } இல்லையெனில் {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        எனில்((canon_மதிப்பு&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(புதிய UnicodeSet, errorCode);
            set=lpSet.getAlias();
            எனில்(U_FAILURE(errorCode)) {
                திரும்பு;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_மதிப்பு & CANON_மதிப்பு_MASK);
            canon_மதிப்பு = (canon_மதிப்பு & ~CANON_மதிப்பு_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_மதிப்பு, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            எனில் (U_FAILURE(errorCode)) {
                திரும்பு;
            }
            எனில்(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } இல்லையெனில் {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_மதிப்பு & CANON_மதிப்பு_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
வகுப்பு InitCanonIterData {
பொது:
    நிலையான வெறுமை doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
நிலையான வெறுமை U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

வெறுமை InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = புதிய CanonIterData(errorCode);
    எனில் (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        போது ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            எனில் (value != Normalizer2Impl::INERT) {
                impl->makeCanonIterDataFromNorm16(start, end, value, *impl->fCanonIterData, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->fCanonIterData->mutableTrie, "CanonIterData");
#endif
        impl->fCanonIterData->trie = umutablecptrie_buildImmutable(
            impl->fCanonIterData->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_மதிப்பு_BITS_32, &errorCode);
        umutablecptrie_close(impl->fCanonIterData->mutableTrie);
        impl->fCanonIterData->mutableTrie = nullptr;
    }
    எனில் (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

வெறுமை Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, மாறிலி uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) மாறிலி {
    எனில்(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        திரும்பு;
    }
    க்கு(UChar32 c=start; c<=end; ++c) {
        uint32_t old_மதிப்பு = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t new_மதிப்பு=old_மதிப்பு;
        எனில்(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_மதிப்பு|=CANON_NOT_SEGMENT_STARTER;
            எனில்(norm16<MIN_NORMAL_MAYBE_YES) {
                new_மதிப்பு|=CANON_HAS_COMPOSITIONS;
            }
        } இல்லையெனில் எனில்(norm16<minYesNo) {
            new_மதிப்பு|=CANON_HAS_COMPOSITIONS;
        } இல்லையெனில் {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            எனில் (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            எனில் (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                மாறிலி uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                எனில்((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    எனில்(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_மதிப்பு|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                எனில்(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    எனில்(norm16_2>=minNoNo) {
                        போது(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2_மதிப்பு = umutablecptrie_get(newData.mutableTrie, c2);
                            எனில்((c2_மதிப்பு&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2_மதிப்பு|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } இல்லையெனில் {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        எனில்(new_மதிப்பு!=old_மதிப்பு) {
            umutablecptrie_set(newData.mutableTrie, c, new_மதிப்பு, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) மாறிலி {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(இது);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    திரும்பு U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_மதிப்பு(UChar32 c) மாறிலி {
    திரும்பு static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

மாறிலி UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) மாறிலி {
    திரும்பு *static_cast<மாறிலி UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) மாறிலி {
    திரும்பு get_Canon_மதிப்பு(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) மாறிலி {
    int32_t canon_மதிப்பு=get_Canon_மதிப்பு(c)&~CANON_NOT_SEGMENT_STARTER;
    எனில்(canon_மதிப்பு==0) {
        திரும்பு பொய்;
    }
    set.clear();
    int32_t value=canon_மதிப்பு&CANON_மதிப்பு_MASK;
    எனில்((canon_மதிப்பு&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } இல்லையெனில் எனில்(value!=0) {
        set.add(value);
    }
    எனில்((canon_மதிப்பு&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        எனில்(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_மொத்தம்);
            set.add(syllable, syllable+Hangul::JAMO_VT_மொத்தம்-1);
        } இல்லையெனில் {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    திரும்பு உண்மை;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(மாறிலி UDataSwapper *ds,
            மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
            UErrorCode *pErrorCode) {
    மாறிலி UDataInfo *pInfo;
    int32_t headerSize;

    மாறிலி uint8_t *inBytes;
    uint8_t *outBytes;

    மாறிலி int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    /* check data format and format version */
    pInfo=(மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    எனில்(!(
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
        திரும்பு 0;
    }

    inBytes=(மாறிலி uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(மாறிலி int32_t *)inBytes;
    int32_t minIndexesLength;
    எனில்(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } இல்லையெனில் எனில்(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } இல்லையெனில் எனில்(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } இல்லையெனில் {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    எனில்(length>=0) {
        length-=headerSize;
        எனில்(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
    }

    /* read the first few indexes */
    க்கு(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    எனில்(length>=0) {
        எனில்(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }

        /* copy the data for inaccessible bytes */
        எனில்(inBytes!=outBytes) {
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

    திரும்பு headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
