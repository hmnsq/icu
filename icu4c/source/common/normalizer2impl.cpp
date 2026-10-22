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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/umutablecptrie.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucptrie_impl.h"
#अन्तर्भाव "uset_imp.h"
#अन्तर्भाव "uvector.h"

U_NAMESPACE_BEGIN

नामाकाश {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
अन्तःस्थ uint8_t leadByteForCP(UChar32 c) {
    यदि (c <= 0x7f) {
        प्रत्यागम static_cast<uint8_t>(c);
    } अन्यथा यदि (c <= 0x7ff) {
        प्रत्यागम static_cast<uint8_t>(0xc0 + (c >> 6));
    } अन्यथा {
        // Should not occur because ccc(U+0300)!=0.
        प्रत्यागम 0xe0;
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
UChar32 codePointFromValidUTF8(ध्रुव uint8_t *cpStart, ध्रुव uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    विकल्प(cpLimit-cpStart) {
    पक्ष 1:
        प्रत्यागम c;
    पक्ष 2:
        प्रत्यागम ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    पक्ष 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        प्रत्यागम static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    पक्ष 4:
        प्रत्यागम ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    स्वभाव:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(ध्रुव uint8_t *start, ध्रुव uint8_t *p) {
    यदि ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        यदि (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            प्रत्यागम ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    प्रत्यागम U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(ध्रुव uint8_t *src, ध्रुव uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    यदि ((limit - src) >= 3 && *src == 0xe1) {
        यदि (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            यदि (0xa8 <= t && t <= 0xbf) {
                प्रत्यागम t - 0xa7;
            }
        } अन्यथा यदि (src[1] == 0x87) {
            uint8_t t = src[2];
            यदि (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                प्रत्यागम t - (0xa7 - 0x40);
            }
        }
    }
    प्रत्यागम -1;
}

शून्य
appendCodePointDelta(ध्रुव uint8_t *cpStart, ध्रुव uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    वर्ण buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    यदि (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } अन्यथा {
        int32_t trail = *(cpLimit-1) + delta;
        यदि (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            कुरु { buffer[length++] = *cpStart++; } यावत् (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } अन्यथा {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    यदि (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(ध्रुव Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    यदि (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    यदि(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम असत्य;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    यदि(start==limit) {
        lastCC=0;
    } अन्यथा {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        यदि(lastCC>1) {
            यावत्(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    प्रत्यागम सत्य;
}

UBool ReorderingBuffer::equals(ध्रुव char16_t *otherStart, ध्रुव char16_t *otherLimit) ध्रुव {
    int32_t length = static_cast<int32_t>(limit - start);
    प्रत्यागम
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(ध्रुव uint8_t *otherStart, ध्रुव uint8_t *otherLimit) ध्रुव {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    यदि (otherLength < length || (otherLength / 3) > length) {
        प्रत्यागम असत्य;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    प्रति (int32_t i = 0, j = 0;;) {
        यदि (i >= length) {
            प्रत्यागम j >= otherLength;
        } अन्यथा यदि (j >= otherLength) {
            प्रत्यागम असत्य;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        यदि (c != other) {
            प्रत्यागम असत्य;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    यदि(remainingCapacity<2 && !resize(2, errorCode)) {
        प्रत्यागम असत्य;
    }
    यदि(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        यदि(cc<=1) {
            reorderStart=limit;
        }
    } अन्यथा {
        insert(c, cc);
    }
    remainingCapacity-=2;
    प्रत्यागम सत्य;
}

UBool ReorderingBuffer::append(ध्रुव char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    यदि(length==0) {
        प्रत्यागम सत्य;
    }
    यदि(remainingCapacity<length && !resize(length, errorCode)) {
        प्रत्यागम असत्य;
    }
    remainingCapacity-=length;
    यदि(lastCC<=leadCC || leadCC==0) {
        यदि(trailCC<=1) {
            reorderStart=limit+length;
        } अन्यथा यदि(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        ध्रुव char16_t *sLimit=s+length;
        कुरु { *limit++=*s++; } यावत्(s!=sLimit);
        lastCC=trailCC;
    } अन्यथा {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        यावत्(i<length) {
            U16_NEXT(s, i, length, c);
            यदि(i<length) {
                यदि (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } अन्यथा {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } अन्यथा {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    प्रत्यागम सत्य;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    यदि(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        प्रत्यागम असत्य;
    }
    remainingCapacity-=cpLength;
    यदि(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } अन्यथा {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    प्रत्यागम सत्य;
}

UBool ReorderingBuffer::appendZeroCC(ध्रुव char16_t *s, ध्रुव char16_t *sLimit, UErrorCode &errorCode) {
    यदि(s==sLimit) {
        प्रत्यागम सत्य;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    यदि(remainingCapacity<length && !resize(length, errorCode)) {
        प्रत्यागम असत्य;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    प्रत्यागम सत्य;
}

शून्य ReorderingBuffer::हटाओ() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

शून्य ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    यदि(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } अन्यथा {
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
    यदि(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    यदि(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    यदि(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम असत्य;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    प्रत्यागम सत्य;
}

शून्य ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    यदि(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    यदि(reorderStart>=codePointStart) {
        प्रत्यागम 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    यदि(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    प्रत्यागम impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
शून्य ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    प्रति(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    कुरु {
        *--r=*--q;
    } यावत्(codePointLimit!=q);
    writeCodePoint(q, c);
    यदि(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

संरचना CanonIterData : सार्वजनिक UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    शून्य addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

शून्य
Normalizer2Impl::init(ध्रुव int32_t *inIndexes, ध्रुव UCPTrie *inTrie,
                      ध्रुव uint16_t *inExtraData, ध्रुव uint8_t *inSmallFCD) {
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

स्थिर uint32_t U_CALLCONV
segmentStarterMapper(ध्रुव शून्य * /*context*/, uint32_t value) {
    प्रत्यागम value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

शून्य
Normalizer2Impl::addLcccChars(UnicodeSet &set) ध्रुव {
    UChar32 start = 0, end;
    uint32_t norm16;
    यावत् ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        यदि (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } अन्यथा यदि (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            यदि (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

शून्य
Normalizer2Impl::addPropertyStarts(ध्रुव USetAdder *sa, UErrorCode & /*errorCode*/) ध्रुव {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    यावत् ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        यदि (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            यावत् (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                यदि (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    प्रति(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_गिनती) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

शून्य
Normalizer2Impl::addCanonIterPropertyStarts(ध्रुव USetAdder *sa, UErrorCode &errorCode) ध्रुव {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    यदि (!ensureCanonIterData(errorCode)) { प्रत्यागम; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    यावत् ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

ध्रुव char16_t *
Normalizer2Impl::copy_निचला_Prefix_From_Nul_Terminated(ध्रुव char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) ध्रुव {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ध्रुव char16_t *prevSrc=src;
    char16_t c;
    यावत्((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    यदि(--src!=prevSrc) {
        यदि(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    प्रत्यागम src;
}

UnicodeString &
Normalizer2Impl::decompose(ध्रुव UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        dest.setToBogus();
        प्रत्यागम dest;
    }
    ध्रुव char16_t *sArray=src.getBuffer();
    यदि(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        प्रत्यागम dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    प्रत्यागम dest;
}

शून्य
Normalizer2Impl::decompose(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) ध्रुव {
    यदि(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.हटाओ();
    ReorderingBuffer buffer(*एतत्, dest);
    यदि(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ध्रुव char16_t *
Normalizer2Impl::decompose(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) ध्रुव {
    यदि(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम src;
    }
    UChar32 minNoCP=minDecompNoCP;
    यदि(limit==nullptr) {
        src=copy_निचला_Prefix_From_Nul_Terminated(src, minNoCP, buffer, errorCode);
        यदि(U_FAILURE(errorCode)) {
            प्रत्यागम src;
        }
        limit=u_strchr(src, 0);
    }

    ध्रुव char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ध्रुव char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    प्रति(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        प्रति(prevSrc=src; src!=limit;) {
            यदि( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा यदि(!U16_IS_LEAD(c)) {
                भङ्ग;
            } अन्यथा {
                char16_t c2;
                यदि((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    यदि(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } अन्यथा {
                        भङ्ग;
                    }
                } अन्यथा {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        यदि(src!=prevSrc) {
            यदि(buffer!=nullptr) {
                यदि(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    भङ्ग;
                }
            } अन्यथा {
                prevCC=0;
                prevBoundary=src;
            }
        }
        यदि(src==limit) {
            भङ्ग;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        यदि(buffer!=nullptr) {
            यदि(!decompose(c, norm16, *buffer, errorCode)) {
                भङ्ग;
            }
        } अन्यथा {
            यदि(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                यदि(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    यदि(cc<=1) {
                        prevBoundary=src;
                    }
                    अनुवर्त;
                }
            }
            प्रत्यागम prevBoundary;  // "no" or cc out of order
        }
    }
    प्रत्यागम src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ध्रुव char16_t *
Normalizer2Impl::decomposeShort(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    यावत्(src<limit) {
        यदि (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            प्रत्यागम src;
        }
        ध्रुव char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        यदि (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            प्रत्यागम prevSrc;
        }
        यदि(!decompose(c, norm16, buffer, errorCode)) {
            प्रत्यागम nullptr;
        }
        यदि (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            प्रत्यागम src;
        }
    }
    प्रत्यागम src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) ध्रुव {
    // get the decomposition and the lead and trail cc's
    यदि (norm16 >= limitNoNo) {
        यदि (isMaybeYesOrNonZeroCC(norm16)) {
            प्रत्यागम buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } अन्यथा यदि (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    यदि (norm16 < minYesNo) {
        // c does not decompose
        प्रत्यागम buffer.append(c, 0, errorCode);
    } अन्यथा यदि(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        प्रत्यागम buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    यदि(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } अन्यथा {
        leadCC=0;
    }
    प्रत्यागम buffer.append(reinterpret_cast<ध्रुव char16_t*>(mapping) + 1, length, सत्य, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ध्रुव uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ध्रुव uint8_t *src, ध्रुव uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) ध्रुव {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ध्रुव uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    प्रति (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ध्रुव uint8_t *fastStart = src;
        ध्रुव uint8_t *prevSrc;
        uint16_t norm16 = 0;

        प्रति (;;) {
            यदि (src == limit) {
                यदि (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                प्रत्यागम src;
            }
            यदि (*src < minNoLead) {
                ++src;
            } अन्यथा {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                यदि (!isMostDecompYesAndZeroCC(norm16)) {
                    भङ्ग;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        यदि (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            यदि (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                भङ्ग;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        यदि (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            यदि (prevCC <= cc || cc == 0) {
                prevCC = cc;
                यदि (cc <= 1) {
                    यदि (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        भङ्ग;
                    }
                    prevBoundary = src;
                }
                अनुवर्त;
            }
        }
        यदि (sink == nullptr) {
            प्रत्यागम prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        यदि (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            यदि (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                भङ्ग;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*एतत्, s16, errorCode);
        यदि (U_FAILURE(errorCode)) {
            भङ्ग;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, असत्य /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        यदि (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, असत्य /* onlyContiguous */,
                                 buffer, errorCode);
        }
        यदि (U_FAILURE(errorCode)) {
            भङ्ग;
        }
        यदि ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            भङ्ग;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        यदि (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            यदि (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                भङ्ग;
            }
        } अन्यथा {
            यदि (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                भङ्ग;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    प्रत्यागम src;
}

ध्रुव uint8_t *
Normalizer2Impl::decomposeShort(ध्रुव uint8_t *src, ध्रुव uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    यावत् (src < limit) {
        ध्रुव uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        यदि (norm16 >= limitNoNo) {
            यदि (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                यदि (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    प्रत्यागम prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                यदि (!buffer.append(c, cc, errorCode)) {
                    प्रत्यागम nullptr;
                }
                यदि (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    प्रत्यागम src;
                }
                अनुवर्त;
            } अन्यथा यदि (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                यदि (stopAt != STOP_AT_LIMIT) {
                    प्रत्यागम prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } अन्यथा यदि (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            प्रत्यागम prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        यदि (norm16 < minYesNo) {
            यदि (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            यदि (!buffer.append(c, 0, errorCode)) {
                प्रत्यागम nullptr;
            }
        } अन्यथा यदि (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            यदि (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            यदि (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                प्रत्यागम nullptr;
            }
        } अन्यथा {
            // The character decomposes, get everything from the variable-length extra data.
            ध्रुव uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            यदि (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } अन्यथा {
                leadCC = 0;
            }
            यदि (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                प्रत्यागम prevSrc;
            }
            यदि (!buffer.append(reinterpret_cast<ध्रुव char16_t*>(mapping) + 1, length, सत्य, leadCC, trailCC, errorCode)) {
                प्रत्यागम nullptr;
            }
        }
        यदि ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            प्रत्यागम src;
        }
    }
    प्रत्यागम src;
}

ध्रुव char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) ध्रुव {
    uint16_t norm16;
    यदि(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        प्रत्यागम nullptr;
    }
    ध्रुव char16_t *decomp = nullptr;
    यदि(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    यदि (norm16 < minYesNo) {
        प्रत्यागम decomp;
    } अन्यथा यदि(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        प्रत्यागम buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    प्रत्यागम reinterpret_cast<ध्रुव char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ध्रुव char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) ध्रुव {
    uint16_t norm16;
    यदि(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        प्रत्यागम nullptr;
    } अन्यथा यदि(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        प्रत्यागम buffer;
    } अन्यथा यदि(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        प्रत्यागम buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    यदि(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ध्रुव uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        यदि(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            प्रत्यागम reinterpret_cast<ध्रुव char16_t*>(rawMapping) - rm0;
        } अन्यथा {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<ध्रुव char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            प्रत्यागम buffer;
        }
    } अन्यथा {
        length=mLength;
        प्रत्यागम reinterpret_cast<ध्रुव char16_t*>(mapping) + 1;
    }
}

शून्य Normalizer2Impl::decomposeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_मध्य,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) ध्रुव {
    buffer.copyReorderableSuffixTo(safe_मध्य);
    यदि(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        प्रत्यागम;
    }
    // Just merge the strings at the boundary.
    तार्किक isFirst = सत्य;
    uint8_t firstCC = 0, prevCC = 0, cc;
    ध्रुव char16_t *p = src;
    यावत् (p != limit) {
        ध्रुव char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        यदि ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            भङ्ग;
        }
        यदि (isFirst) {
            firstCC = cc;
            isFirst = असत्य;
        }
        prevCC = cc;
    }
    यदि(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    यदि (buffer.append(src, static_cast<int32_t>(p - src), असत्य, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ध्रुव {
    प्रत्यागम c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ध्रुव {
    यदि (norm16 < minNoNoCompNoMaybeCC) {
        प्रत्यागम सत्य;
    }
    यदि (norm16 >= limitNoNo) {
        प्रत्यागम norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    प्रत्यागम (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ध्रुव {
    यदि (c < minDecompNoCP) {
        प्रत्यागम सत्य;
    }
    यदि (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        प्रत्यागम सत्य;
    }
    प्रत्यागम norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ध्रुव {
    यदि(norm16 <= minYesNo || isHangulLVT(norm16)) {
        प्रत्यागम सत्य;
    }
    यदि (norm16 >= limitNoNo) {
        यदि (isMaybeYesOrNonZeroCC(norm16)) {
            प्रत्यागम norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } अन्यथा यदि (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            प्रत्यागम (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    यदि(firstUnit>0x1ff) {
        प्रत्यागम असत्य;  // trailCC>1
    }
    यदि(firstUnit<=0xff) {
        प्रत्यागम सत्य;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    प्रत्यागम (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(ध्रुव uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    यदि(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        यावत्(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        यदि(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            यदि(firstUnit&COMP_1_TRIPLE) {
                प्रत्यागम (static_cast<int32_t>(list[1]) << 16) | list[2];
            } अन्यथा {
                प्रत्यागम list[1];
            }
        }
    } अन्यथा {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        प्रति(;;) {
            यदि(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } अन्यथा यदि(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                यदि(key2>(secondUnit=list[1])) {
                    यदि(firstUnit&COMP_1_LAST_TUPLE) {
                        भङ्ग;
                    } अन्यथा {
                        list+=3;
                    }
                } अन्यथा यदि(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    प्रत्यागम (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } अन्यथा {
                    भङ्ग;
                }
            } अन्यथा {
                भङ्ग;
            }
        }
    }
    प्रत्यागम -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
शून्य Normalizer2Impl::addComposites(ध्रुव uint16_t *list, UnicodeSet &set) ध्रुव {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    कुरु {
        firstUnit=*list;
        यदि((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } अन्यथा {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        यदि((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } यावत्((firstUnit&COMP_1_LAST_TUPLE)==0);
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
शून्य Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) ध्रुव {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    यदि(p==limit) {
        प्रत्यागम;
    }

    char16_t *starter, *pRemove, *q, *r;
    ध्रुव uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=असत्य;
    prevCC=0;

    प्रति(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        यदि( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            यदि(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                यदि(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    यदि(prev<Hangul::JAMO_L_गिनती) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_गिनती+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_गिनती);
                        char16_t t;
                        यदि (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_गिनती) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        यावत्(r<limit) {
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
                यदि(p==limit) {
                    भङ्ग;
                }
                compositionsList=nullptr;
                अनुवर्त;
            } अन्यथा यदि((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                यदि(starterIsSupplementary) {
                    यदि(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } अन्यथा {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=असत्य;
                        q=starter+1;
                        r=q+1;
                        यावत्(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } अन्यथा यदि(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=सत्य;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    यावत्(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } अन्यथा {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                यदि(pRemove<p) {
                    q=pRemove;
                    r=p;
                    यावत्(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                यदि(p==limit) {
                    भङ्ग;
                }
                // Is the composite a starter that combines forward?
                यदि(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } अन्यथा {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                अनुवर्त;
            }
        }

        // no combination this time
        prevCC=cc;
        यदि(p==limit) {
            भङ्ग;
        }

        // If c did not combine, then check if it is a starter.
        यदि(cc==0) {
            // Found a new starter.
            यदि((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                यदि(U_IS_BMP(c)) {
                    starterIsSupplementary=असत्य;
                    starter=p-1;
                } अन्यथा {
                    starterIsSupplementary=सत्य;
                    starter=p-2;
                }
            }
        } अन्यथा यदि(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ध्रुव {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ध्रुव uint16_t *list;
    यदि(isInert(norm16)) {
        प्रत्यागम U_SENTINEL;
    } अन्यथा यदि(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        यदि(isJamoL(norm16)) {
            यदि (b < Hangul::JAMO_V_BASE) {
                प्रत्यागम U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            यदि(b<Hangul::JAMO_V_गिनती) {
                प्रत्यागम
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_गिनती+b)*
                     Hangul::JAMO_T_गिनती);
            } अन्यथा {
                प्रत्यागम U_SENTINEL;
            }
        } अन्यथा यदि(isHangulLV(norm16)) {
            यदि (b <= Hangul::JAMO_T_BASE) {
               प्रत्यागम U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            यदि(b<Hangul::JAMO_T_गिनती) {  // not b==0!
                प्रत्यागम a+b;
            } अन्यथा {
                प्रत्यागम U_SENTINEL;
            }
        } अन्यथा {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            यदि(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } अन्यथा यदि(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        प्रत्यागम U_SENTINEL;
    } अन्यथा {
        list=getDataForMaybe(norm16);
        यदि(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    यदि(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        प्रत्यागम U_SENTINEL;
    }
#यदि U_SIGNED_दायाँ_SHIFT_IS_ARITHMETIC
    प्रत्यागम combine(list, b)>>1;
#अन्यथा
    int32_t compositeAndFwd=combine(list, b);
    प्रत्यागम compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) ध्रुव {
    ध्रुव char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    यदि(limit==nullptr) {
        src=copy_निचला_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        यदि(U_FAILURE(errorCode)) {
            प्रत्यागम असत्य;
        }
        limit=u_strchr(src, 0);
        यदि (prevBoundary != src) {
            यदि (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } अन्यथा {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    प्रति (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ध्रुव char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        प्रति (;;) {
            यदि (src == limit) {
                यदि (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                प्रत्यागम सत्य;
            }
            यदि( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा {
                prevSrc = src++;
                यदि(!U16_IS_LEAD(c)) {
                    भङ्ग;
                } अन्यथा {
                    char16_t c2;
                    यदि(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        यदि(!isCompYesAndZeroCC(norm16)) {
                            भङ्ग;
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
        यदि (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            यदि (!doCompose) {
                प्रत्यागम असत्य;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            यदि (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    यदि (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        भङ्ग;
                    }
                    यदि(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        भङ्ग;
                    }
                    prevBoundary = src;
                    अनुवर्त;
                }
            } अन्यथा यदि (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    यदि (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        भङ्ग;
                    }
                    ध्रुव char16_t *mapping = reinterpret_cast<ध्रुव char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    यदि(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        भङ्ग;
                    }
                    prevBoundary = src;
                    अनुवर्त;
                }
            } अन्यथा यदि (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                यदि (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    यदि (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        भङ्ग;
                    }
                    prevBoundary = src;
                    अनुवर्त;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } अन्यथा यदि (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            यदि(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                यदि(l<Hangul::JAMO_L_गिनती) {
                    यदि (!doCompose) {
                        प्रत्यागम असत्य;
                    }
                    int32_t t;
                    यदि (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_गिनती) {
                        // The next character is a Jamo T.
                        ++src;
                    } अन्यथा यदि (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } अन्यथा {
                        t = -1;
                    }
                    यदि (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_गिनती + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_गिनती + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        यदि (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            भङ्ग;
                        }
                        यदि (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            भङ्ग;
                        }
                        prevBoundary = src;
                        अनुवर्त;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } अन्यथा यदि (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                यदि (!doCompose) {
                    प्रत्यागम असत्य;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                यदि (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    भङ्ग;
                }
                यदि (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    भङ्ग;
                }
                prevBoundary = src;
                अनुवर्त;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } अन्यथा यदि (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            यदि (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                यदि (!doCompose) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ध्रुव char16_t *nextSrc;
                uint16_t n16;
                प्रति (;;) {
                    यदि (src == limit) {
                        यदि (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        प्रत्यागम सत्य;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    यदि (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        यदि (prevCC > cc) {
                            यदि (!doCompose) {
                                प्रत्यागम असत्य;
                            }
                            भङ्ग;
                        }
                    } अन्यथा {
                        भङ्ग;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                यदि (norm16HasCompBoundaryBefore(n16)) {
                    यदि (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    अनुवर्त;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        यदि (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ध्रुव char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            यदि (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        यदि (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            भङ्ग;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, असत्य /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, सत्य /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        यदि (U_FAILURE(errorCode)) {
            भङ्ग;
        }
        यदि ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम सत्य;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        यदि(!doCompose) {
            यदि(!buffer.equals(prevSrc, src)) {
                प्रत्यागम असत्य;
            }
            buffer.हटाओ();
        }
        prevBoundary=src;
    }
    प्रत्यागम सत्य;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ध्रुव char16_t *
Normalizer2Impl::composeQuickCheck(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) ध्रुव {
    ध्रुव char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    यदि(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_निचला_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        यदि (prevBoundary != src) {
            यदि (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } अन्यथा {
                prevBoundary = --src;
            }
        }
    }

    प्रति(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ध्रुव char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        प्रति (;;) {
            यदि(src==limit) {
                प्रत्यागम src;
            }
            यदि( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा {
                prevSrc = src++;
                यदि(!U16_IS_LEAD(c)) {
                    भङ्ग;
                } अन्यथा {
                    char16_t c2;
                    यदि(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        यदि(!isCompYesAndZeroCC(norm16)) {
                            भङ्ग;
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
        यदि (prevBoundary != prevSrc) {
            यदि (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } अन्यथा {
                ध्रुव char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                यदि (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } अन्यथा {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        यदि (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            यदि (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ध्रुव char16_t *nextSrc;
                प्रति (;;) {
                    यदि (norm16 < MIN_YES_YES_WITH_CC) {
                        यदि (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } अन्यथा {
                            प्रत्यागम prevBoundary;
                        }
                    }
                    यदि (src == limit) {
                        प्रत्यागम src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    यदि (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        यदि (!(prevCC <= cc || cc == 0)) {
                            भङ्ग;
                        }
                    } अन्यथा {
                        भङ्ग;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                यदि (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    अनुवर्त;
                }
            }
        }
        यदि(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        प्रत्यागम prevBoundary;
    }
}

शून्य Normalizer2Impl::composeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_मध्य,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ध्रुव {
    यदि(!buffer.isEmpty()) {
        ध्रुव char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        यदि(src!=firstStarterInSrc) {
            ध्रुव char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString मध्य(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_मध्य=मध्य;
            मध्य.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            ध्रुव char16_t *मध्य_Start=मध्य.getBuffer();
            compose(मध्य_Start, मध्य_Start+मध्य.length(), onlyContiguous,
                    सत्य, buffer, errorCode);
            यदि(U_FAILURE(errorCode)) {
                प्रत्यागम;
            }
            src=firstStarterInSrc;
        }
    }
    यदि(doCompose) {
        compose(src, limit, onlyContiguous, सत्य, buffer, errorCode);
    } अन्यथा {
        यदि(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ध्रुव uint8_t *src, ध्रुव uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) ध्रुव {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ध्रुव uint8_t *prevBoundary = src;

    प्रति (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ध्रुव uint8_t *prevSrc;
        uint16_t norm16 = 0;
        प्रति (;;) {
            यदि (src == limit) {
                यदि (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                प्रत्यागम सत्य;
            }
            यदि (*src < minNoMaybeLead) {
                ++src;
            } अन्यथा {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                यदि (!isCompYesAndZeroCC(norm16)) {
                    भङ्ग;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        यदि (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            यदि (sink == nullptr) {
                प्रत्यागम असत्य;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            यदि (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    यदि (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        भङ्ग;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    अनुवर्त;
                }
            } अन्यथा यदि (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    यदि (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        भङ्ग;
                    }
                    ध्रुव uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    यदि (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ध्रुव char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        भङ्ग;
                    }
                    prevBoundary = src;
                    अनुवर्त;
                }
            } अन्यथा यदि (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                यदि (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    यदि (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        भङ्ग;
                    }
                    यदि (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    अनुवर्त;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } अन्यथा यदि (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            यदि (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                यदि (static_cast<uint32_t>(l) < Hangul::JAMO_L_गिनती) {
                    यदि (sink == nullptr) {
                        प्रत्यागम असत्य;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    यदि (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } अन्यथा यदि (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    यदि (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_गिनती + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_गिनती + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        यदि (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            भङ्ग;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        अनुवर्त;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } अन्यथा यदि (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                यदि (sink == nullptr) {
                    प्रत्यागम असत्य;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                यदि (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    भङ्ग;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                अनुवर्त;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } अन्यथा यदि (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            यदि (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                यदि (sink == nullptr) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ध्रुव uint8_t *nextSrc;
                uint16_t n16;
                प्रति (;;) {
                    यदि (src == limit) {
                        यदि (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        प्रत्यागम सत्य;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    यदि (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        यदि (prevCC > cc) {
                            यदि (sink == nullptr) {
                                प्रत्यागम असत्य;
                            }
                            भङ्ग;
                        }
                    } अन्यथा {
                        भङ्ग;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                यदि (norm16HasCompBoundaryBefore(n16)) {
                    यदि (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    अनुवर्त;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        यदि (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ध्रुव uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            यदि (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*एतत्, s16, errorCode);
        यदि (U_FAILURE(errorCode)) {
            भङ्ग;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        यदि (U_FAILURE(errorCode)) {
            भङ्ग;
        }
        यदि ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम सत्य;
        }
        recompose(buffer, 0, onlyContiguous);
        यदि (!buffer.equals(prevSrc, src)) {
            यदि (sink == nullptr) {
                प्रत्यागम असत्य;
            }
            यदि (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                भङ्ग;
            }
            यदि (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                भङ्ग;
            }
            prevBoundary = src;
        }
    }
    प्रत्यागम सत्य;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ध्रुव char16_t *src, ध्रुव char16_t *limit) ध्रुव {
    यदि (src == limit || *src < minCompNoMaybeCP) {
        प्रत्यागम सत्य;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    प्रत्यागम norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ध्रुव uint8_t *src, ध्रुव uint8_t *limit) ध्रुव {
    यदि (src == limit) {
        प्रत्यागम सत्य;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    प्रत्यागम norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ध्रुव char16_t *start, ध्रुव char16_t *p,
                                            UBool onlyContiguous) ध्रुव {
    यदि (start == p) {
        प्रत्यागम सत्य;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    प्रत्यागम norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ध्रुव uint8_t *start, ध्रुव uint8_t *p,
                                            UBool onlyContiguous) ध्रुव {
    यदि (start == p) {
        प्रत्यागम सत्य;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    प्रत्यागम norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ध्रुव char16_t *Normalizer2Impl::findPreviousCompBoundary(ध्रुव char16_t *start, ध्रुव char16_t *p,
                                                       UBool onlyContiguous) ध्रुव {
    यावत् (p != start) {
        ध्रुव char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            प्रत्यागम codePointLimit;
        }
        यदि (hasCompBoundaryBefore(c, norm16)) {
            प्रत्यागम p;
        }
    }
    प्रत्यागम p;
}

ध्रुव char16_t *Normalizer2Impl::findNextCompBoundary(ध्रुव char16_t *p, ध्रुव char16_t *limit,
                                                   UBool onlyContiguous) ध्रुव {
    यावत् (p != limit) {
        ध्रुव char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        यदि (hasCompBoundaryBefore(c, norm16)) {
            प्रत्यागम codePointStart;
        }
        यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            प्रत्यागम p;
        }
    }
    प्रत्यागम p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ध्रुव char16_t *start, ध्रुव char16_t *p) ध्रुव {
    यदि (start == p) {
        प्रत्यागम 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    प्रत्यागम static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ध्रुव uint8_t *start, ध्रुव uint8_t *p) ध्रुव {
    यदि (start == p) {
        प्रत्यागम 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    प्रत्यागम static_cast<uint8_t>(getFCD16(c));
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
#यदि (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) ध्रुव {
    uint16_t norm16=getNorm16(c);
    यदि (norm16 >= limitNoNo) {
        यदि(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            प्रत्यागम norm16|(norm16<<8);
        } अन्यथा यदि(norm16>=minMaybeYes) {
            प्रत्यागम 0;
        } अन्यथा यदि(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            यदि (deltaTrailCC <= DELTA_TCCC_1) {
                प्रत्यागम deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    यदि(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        प्रत्यागम 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    यदि(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    प्रत्यागम norm16;
}
#यदि (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ध्रुव {
    U_ASSERT(norm16 >= minMaybeNo);
    यदि (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        प्रत्यागम norm16 | (norm16<<8);
    } अन्यथा यदि (norm16 >= minMaybeYes) {
        प्रत्यागम 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    प्रत्यागम firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ध्रुव char16_t *
Normalizer2Impl::makeFCD(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) ध्रुव {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ध्रुव char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    यदि(limit==nullptr) {
        src=copy_निचला_Prefix_From_Nul_Terminated(src, minLcccCP, buffer, errorCode);
        यदि(U_FAILURE(errorCode)) {
            प्रत्यागम src;
        }
        यदि(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            यदि(prevFCD16>1) {
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

    ध्रुव char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    प्रति(;;) {
        // count code units with lccc==0
        प्रति(prevSrc=src; src!=limit;) {
            यदि((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } अन्यथा यदि(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } अन्यथा {
                यदि(U16_IS_LEAD(c)) {
                    char16_t c2;
                    यदि((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                यदि((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } अन्यथा {
                    भङ्ग;
                }
            }
        }
        // copy these code units all at once
        यदि(src!=prevSrc) {
            यदि(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                भङ्ग;
            }
            यदि(src==limit) {
                भङ्ग;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            यदि(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                यदि(prev<minDecompNoCP) {
                    prevFCD16=0;
                } अन्यथा {
                    prevFCD16=getFCD16FromNormData(prev);
                    यदि(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } अन्यथा {
                ध्रुव char16_t *p=src-1;
                यदि(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                यदि(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } अन्यथा यदि(src==limit) {
            भङ्ग;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        यदि((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            यदि((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            यदि(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                भङ्ग;
            }
            prevFCD16=fcd16;
            अनुवर्त;
        } अन्यथा यदि(buffer==nullptr) {
            प्रत्यागम prevBoundary;  // quick check "no"
        } अन्यथा {
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
            decomposeShort(prevBoundary, src, असत्य, असत्य, *buffer, errorCode);
            यदि (U_FAILURE(errorCode)) {
                भङ्ग;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    प्रत्यागम src;
}

शून्य Normalizer2Impl::makeFCDAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_मध्य,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ध्रुव {
    यदि(!buffer.isEmpty()) {
        ध्रुव char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        यदि(src!=firstBoundaryInSrc) {
            ध्रुव char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString मध्य(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_मध्य=मध्य;
            मध्य.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            ध्रुव char16_t *मध्य_Start=मध्य.getBuffer();
            makeFCD(मध्य_Start, मध्य_Start+मध्य.length(), &buffer, errorCode);
            यदि(U_FAILURE(errorCode)) {
                प्रत्यागम;
            }
            src=firstBoundaryInSrc;
        }
    }
    यदि(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } अन्यथा {
        यदि(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

ध्रुव char16_t *Normalizer2Impl::findPreviousFCDBoundary(ध्रुव char16_t *start, ध्रुव char16_t *p) ध्रुव {
    यावत्(start<p) {
        ध्रुव char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        यदि (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            प्रत्यागम codePointLimit;
        }
        यदि (norm16HasDecompBoundaryBefore(norm16)) {
            प्रत्यागम p;
        }
    }
    प्रत्यागम p;
}

ध्रुव char16_t *Normalizer2Impl::findNextFCDBoundary(ध्रुव char16_t *p, ध्रुव char16_t *limit) ध्रुव {
    यावत्(p<limit) {
        ध्रुव char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        यदि (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            प्रत्यागम codePointStart;
        }
        यदि (norm16HasDecompBoundaryAfter(norm16)) {
            प्रत्यागम p;
        }
    }
    प्रत्यागम p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

शून्य CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_मूल्य = umutablecptrie_get(mutableTrie, decompLead);
    यदि((canon_मूल्य&(CANON_HAS_SET|CANON_मूल्य_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_मूल्य|origin, &errorCode);
    } अन्यथा {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        यदि((canon_मूल्य&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(नूतन UnicodeSet, errorCode);
            set=lpSet.getAlias();
            यदि(U_FAILURE(errorCode)) {
                प्रत्यागम;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_मूल्य & CANON_मूल्य_MASK);
            canon_मूल्य = (canon_मूल्य & ~CANON_मूल्य_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_मूल्य, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            यदि (U_FAILURE(errorCode)) {
                प्रत्यागम;
            }
            यदि(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } अन्यथा {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_मूल्य & CANON_मूल्य_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
वर्ग InitCanonIterData {
सार्वजनिक:
    स्थिर शून्य doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
स्थिर शून्य U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

शून्य InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = नूतन CanonIterData(errorCode);
    यदि (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        यावत् ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            यदि (value != Normalizer2Impl::INERT) {
                impl->makeCanonIterDataFromNorm16(start, end, value, *impl->fCanonIterData, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->fCanonIterData->mutableTrie, "CanonIterData");
#endif
        impl->fCanonIterData->trie = umutablecptrie_buildImmutable(
            impl->fCanonIterData->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_मूल्य_BITS_32, &errorCode);
        umutablecptrie_close(impl->fCanonIterData->mutableTrie);
        impl->fCanonIterData->mutableTrie = nullptr;
    }
    यदि (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

शून्य Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, ध्रुव uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) ध्रुव {
    यदि(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        प्रत्यागम;
    }
    प्रति(UChar32 c=start; c<=end; ++c) {
        uint32_t old_मूल्य = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t new_मूल्य=old_मूल्य;
        यदि(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_मूल्य|=CANON_NOT_SEGMENT_STARTER;
            यदि(norm16<MIN_NORMAL_MAYBE_YES) {
                new_मूल्य|=CANON_HAS_COMPOSITIONS;
            }
        } अन्यथा यदि(norm16<minYesNo) {
            new_मूल्य|=CANON_HAS_COMPOSITIONS;
        } अन्यथा {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            यदि (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            यदि (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ध्रुव uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                यदि((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    यदि(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_मूल्य|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                यदि(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    यदि(norm16_2>=minNoNo) {
                        यावत्(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2_मूल्य = umutablecptrie_get(newData.mutableTrie, c2);
                            यदि((c2_मूल्य&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2_मूल्य|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } अन्यथा {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        यदि(new_मूल्य!=old_मूल्य) {
            umutablecptrie_set(newData.mutableTrie, c, new_मूल्य, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) ध्रुव {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(एतत्);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    प्रत्यागम U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_मूल्य(UChar32 c) ध्रुव {
    प्रत्यागम static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

ध्रुव UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) ध्रुव {
    प्रत्यागम *static_cast<ध्रुव UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ध्रुव {
    प्रत्यागम get_Canon_मूल्य(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) ध्रुव {
    int32_t canon_मूल्य=get_Canon_मूल्य(c)&~CANON_NOT_SEGMENT_STARTER;
    यदि(canon_मूल्य==0) {
        प्रत्यागम असत्य;
    }
    set.clear();
    int32_t value=canon_मूल्य&CANON_मूल्य_MASK;
    यदि((canon_मूल्य&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } अन्यथा यदि(value!=0) {
        set.add(value);
    }
    यदि((canon_मूल्य&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        यदि(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_गिनती);
            set.add(syllable, syllable+Hangul::JAMO_VT_गिनती-1);
        } अन्यथा {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    प्रत्यागम सत्य;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *inData, int32_t length, शून्य *outData,
            UErrorCode *pErrorCode) {
    ध्रुव UDataInfo *pInfo;
    int32_t headerSize;

    ध्रुव uint8_t *inBytes;
    uint8_t *outBytes;

    ध्रुव int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    यदि(!(
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
        प्रत्यागम 0;
    }

    inBytes=(ध्रुव uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(ध्रुव int32_t *)inBytes;
    int32_t minIndexesLength;
    यदि(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } अन्यथा यदि(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } अन्यथा यदि(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } अन्यथा {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    यदि(length>=0) {
        length-=headerSize;
        यदि(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    /* read the first few indexes */
    प्रति(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    यदि(length>=0) {
        यदि(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        /* copy the data for inaccessible bytes */
        यदि(inBytes!=outBytes) {
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

    प्रत्यागम headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
