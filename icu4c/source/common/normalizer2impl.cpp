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

#ܐܢ !UCONFIG_NO_NORMALIZATION

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
    ܐܢ (c <= 0x7f) {
        ܦܢܐ static_cast<uint8_t>(c);
    } ܐܚܪܢܐ ܐܢ (c <= 0x7ff) {
        ܦܢܐ static_cast<uint8_t>(0xc0 + (c >> 6));
    } ܐܚܪܢܐ {
        // Should not occur because ccc(U+0300)!=0.
        ܦܢܐ 0xe0;
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
UChar32 codePointFromValidUTF8(ثابت uint8_t *cpStart, ثابت uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    بدل(cpLimit-cpStart) {
    حالة 1:
        ܦܢܐ c;
    حالة 2:
        ܦܢܐ ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    حالة 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        ܦܢܐ static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    حالة 4:
        ܦܢܐ ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    افتراضي:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(ثابت uint8_t *start, ثابت uint8_t *p) {
    ܐܢ ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        ܐܢ (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            ܦܢܐ ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    ܦܢܐ U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(ثابت uint8_t *src, ثابت uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    ܐܢ ((limit - src) >= 3 && *src == 0xe1) {
        ܐܢ (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            ܐܢ (0xa8 <= t && t <= 0xbf) {
                ܦܢܐ t - 0xa7;
            }
        } ܐܚܪܢܐ ܐܢ (src[1] == 0x87) {
            uint8_t t = src[2];
            ܐܢ (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                ܦܢܐ t - (0xa7 - 0x40);
            }
        }
    }
    ܦܢܐ -1;
}

فراغ
appendCodePointDelta(ثابت uint8_t *cpStart, ثابت uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    حرف buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    ܐܢ (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } ܐܚܪܢܐ {
        int32_t trail = *(cpLimit-1) + delta;
        ܐܢ (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            ܥܒܕ { buffer[length++] = *cpStart++; } ܟܡܐ (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } ܐܚܪܢܐ {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    ܐܢ (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(ثابت Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    ܐܢ (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    ܐܢ(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    ܐܢ(start==limit) {
        lastCC=0;
    } ܐܚܪܢܐ {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        ܐܢ(lastCC>1) {
            ܟܡܐ(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    ܦܢܐ ܫܪܪܐ;
}

UBool ReorderingBuffer::equals(ثابت char16_t *otherStart, ثابت char16_t *otherLimit) ثابت {
    int32_t length = static_cast<int32_t>(limit - start);
    ܦܢܐ
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(ثابت uint8_t *otherStart, ثابت uint8_t *otherLimit) ثابت {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    ܐܢ (otherLength < length || (otherLength / 3) > length) {
        ܦܢܐ ܕܓܠܐ;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    لكل (int32_t i = 0, j = 0;;) {
        ܐܢ (i >= length) {
            ܦܢܐ j >= otherLength;
        } ܐܚܪܢܐ ܐܢ (j >= otherLength) {
            ܦܢܐ ܕܓܠܐ;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        ܐܢ (c != other) {
            ܦܢܐ ܕܓܠܐ;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    ܐܢ(remainingCapacity<2 && !resize(2, errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        ܐܢ(cc<=1) {
            reorderStart=limit;
        }
    } ܐܚܪܢܐ {
        insert(c, cc);
    }
    remainingCapacity-=2;
    ܦܢܐ ܫܪܪܐ;
}

UBool ReorderingBuffer::append(ثابت char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    ܐܢ(length==0) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܐܢ(remainingCapacity<length && !resize(length, errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    remainingCapacity-=length;
    ܐܢ(lastCC<=leadCC || leadCC==0) {
        ܐܢ(trailCC<=1) {
            reorderStart=limit+length;
        } ܐܚܪܢܐ ܐܢ(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        ثابت char16_t *sLimit=s+length;
        ܥܒܕ { *limit++=*s++; } ܟܡܐ(s!=sLimit);
        lastCC=trailCC;
    } ܐܚܪܢܐ {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        ܟܡܐ(i<length) {
            U16_NEXT(s, i, length, c);
            ܐܢ(i<length) {
                ܐܢ (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } ܐܚܪܢܐ {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } ܐܚܪܢܐ {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    ܦܢܐ ܫܪܪܐ;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    ܐܢ(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    remainingCapacity-=cpLength;
    ܐܢ(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } ܐܚܪܢܐ {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    ܦܢܐ ܫܪܪܐ;
}

UBool ReorderingBuffer::appendZeroCC(ثابت char16_t *s, ثابت char16_t *sLimit, UErrorCode &errorCode) {
    ܐܢ(s==sLimit) {
        ܦܢܐ ܫܪܪܐ;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    ܐܢ(remainingCapacity<length && !resize(length, errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    ܦܢܐ ܫܪܪܐ;
}

فراغ ReorderingBuffer::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

فراغ ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    ܐܢ(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } ܐܚܪܢܐ {
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
    ܐܢ(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    ܐܢ(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    ܐܢ(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    ܦܢܐ ܫܪܪܐ;
}

فراغ ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    ܐܢ(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    ܐܢ(reorderStart>=codePointStart) {
        ܦܢܐ 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    ܐܢ(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    ܦܢܐ impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
فراغ ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    لكل(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    ܥܒܕ {
        *--r=*--q;
    } ܟܡܐ(codePointLimit!=q);
    writeCodePoint(q, c);
    ܐܢ(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

بنية CanonIterData : عام UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    فراغ addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

فراغ
Normalizer2Impl::init(ثابت int32_t *inIndexes, ثابت UCPTrie *inTrie,
                      ثابت uint16_t *inExtraData, ثابت uint8_t *inSmallFCD) {
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

ساكن uint32_t U_CALLCONV
segmentStarterMapper(ثابت فراغ * /*context*/, uint32_t value) {
    ܦܢܐ value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

فراغ
Normalizer2Impl::addLcccChars(UnicodeSet &set) ثابت {
    UChar32 start = 0, end;
    uint32_t norm16;
    ܟܡܐ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        ܐܢ (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } ܐܚܪܢܐ ܐܢ (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            ܐܢ (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

فراغ
Normalizer2Impl::addPropertyStarts(ثابت USetAdder *sa, UErrorCode & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    ܟܡܐ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        ܐܢ (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            ܟܡܐ (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                ܐܢ (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    لكل(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_COUNT) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

فراغ
Normalizer2Impl::addCanonIterPropertyStarts(ثابت USetAdder *sa, UErrorCode &errorCode) ثابت {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    ܐܢ (!ensureCanonIterData(errorCode)) { ܦܢܐ; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    ܟܡܐ ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

ثابت char16_t *
Normalizer2Impl::copyLowPrefixFromNulTerminated(ثابت char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) ثابت {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ثابت char16_t *prevSrc=src;
    char16_t c;
    ܟܡܐ((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    ܐܢ(--src!=prevSrc) {
        ܐܢ(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    ܦܢܐ src;
}

UnicodeString &
Normalizer2Impl::decompose(ثابت UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) ثابت {
    ܐܢ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ܦܢܐ dest;
    }
    ثابت char16_t *sArray=src.getBuffer();
    ܐܢ(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        ܦܢܐ dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    ܦܢܐ dest;
}

فراغ
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) ثابت {
    ܐܢ(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    ReorderingBuffer buffer(*هذا, dest);
    ܐܢ(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ثابت char16_t *
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) ثابت {
    ܐܢ(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ src;
    }
    UChar32 minNoCP=minDecompNoCP;
    ܐܢ(limit==nullptr) {
        src=copyLowPrefixFromNulTerminated(src, minNoCP, buffer, errorCode);
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ src;
        }
        limit=u_strchr(src, 0);
    }

    ثابت char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ثابت char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    لكل(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        لكل(prevSrc=src; src!=limit;) {
            ܐܢ( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ܐܚܪܢܐ ܐܢ(!U16_IS_LEAD(c)) {
                اكسر;
            } ܐܚܪܢܐ {
                char16_t c2;
                ܐܢ((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    ܐܢ(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } ܐܚܪܢܐ {
                        اكسر;
                    }
                } ܐܚܪܢܐ {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        ܐܢ(src!=prevSrc) {
            ܐܢ(buffer!=nullptr) {
                ܐܢ(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    اكسر;
                }
            } ܐܚܪܢܐ {
                prevCC=0;
                prevBoundary=src;
            }
        }
        ܐܢ(src==limit) {
            اكسر;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        ܐܢ(buffer!=nullptr) {
            ܐܢ(!decompose(c, norm16, *buffer, errorCode)) {
                اكسر;
            }
        } ܐܚܪܢܐ {
            ܐܢ(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                ܐܢ(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    ܐܢ(cc<=1) {
                        prevBoundary=src;
                    }
                    استمر;
                }
            }
            ܦܢܐ prevBoundary;  // "no" or cc out of order
        }
    }
    ܦܢܐ src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ثابت char16_t *
Normalizer2Impl::decomposeShort(ثابت char16_t *src, ثابت char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ nullptr;
    }
    ܟܡܐ(src<limit) {
        ܐܢ (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            ܦܢܐ src;
        }
        ثابت char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        ܐܢ (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            ܦܢܐ prevSrc;
        }
        ܐܢ(!decompose(c, norm16, buffer, errorCode)) {
            ܦܢܐ nullptr;
        }
        ܐܢ (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ܦܢܐ src;
        }
    }
    ܦܢܐ src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) ثابت {
    // get the decomposition and the lead and trail cc's
    ܐܢ (norm16 >= limitNoNo) {
        ܐܢ (isMaybeYesOrNonZeroCC(norm16)) {
            ܦܢܐ buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } ܐܚܪܢܐ ܐܢ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ܐܢ (norm16 < minYesNo) {
        // c does not decompose
        ܦܢܐ buffer.append(c, 0, errorCode);
    } ܐܚܪܢܐ ܐܢ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        ܦܢܐ buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    ܐܢ(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } ܐܚܪܢܐ {
        leadCC=0;
    }
    ܦܢܐ buffer.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, ܫܪܪܐ, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ثابت uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ثابت uint8_t *src, ثابت uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) ثابت {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ثابت uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    لكل (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ثابت uint8_t *fastStart = src;
        ثابت uint8_t *prevSrc;
        uint16_t norm16 = 0;

        لكل (;;) {
            ܐܢ (src == limit) {
                ܐܢ (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                ܦܢܐ src;
            }
            ܐܢ (*src < minNoLead) {
                ++src;
            } ܐܚܪܢܐ {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                ܐܢ (!isMostDecompYesAndZeroCC(norm16)) {
                    اكسر;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        ܐܢ (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            ܐܢ (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                اكسر;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        ܐܢ (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            ܐܢ (prevCC <= cc || cc == 0) {
                prevCC = cc;
                ܐܢ (cc <= 1) {
                    ܐܢ (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        اكسر;
                    }
                    prevBoundary = src;
                }
                استمر;
            }
        }
        ܐܢ (sink == nullptr) {
            ܦܢܐ prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        ܐܢ (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            ܐܢ (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                اكسر;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*هذا, s16, errorCode);
        ܐܢ (U_FAILURE(errorCode)) {
            اكسر;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, ܕܓܠܐ /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        ܐܢ (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, ܕܓܠܐ /* onlyContiguous */,
                                 buffer, errorCode);
        }
        ܐܢ (U_FAILURE(errorCode)) {
            اكسر;
        }
        ܐܢ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            اكسر;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        ܐܢ (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            ܐܢ (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                اكسر;
            }
        } ܐܚܪܢܐ {
            ܐܢ (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                اكسر;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    ܦܢܐ src;
}

ثابت uint8_t *
Normalizer2Impl::decomposeShort(ثابت uint8_t *src, ثابت uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ nullptr;
    }
    ܟܡܐ (src < limit) {
        ثابت uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        ܐܢ (norm16 >= limitNoNo) {
            ܐܢ (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                ܐܢ (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    ܦܢܐ prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                ܐܢ (!buffer.append(c, cc, errorCode)) {
                    ܦܢܐ nullptr;
                }
                ܐܢ (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    ܦܢܐ src;
                }
                استمر;
            } ܐܚܪܢܐ ܐܢ (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                ܐܢ (stopAt != STOP_AT_LIMIT) {
                    ܦܢܐ prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } ܐܚܪܢܐ ܐܢ (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            ܦܢܐ prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        ܐܢ (norm16 < minYesNo) {
            ܐܢ (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            ܐܢ (!buffer.append(c, 0, errorCode)) {
                ܦܢܐ nullptr;
            }
        } ܐܚܪܢܐ ܐܢ (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            ܐܢ (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            ܐܢ (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                ܦܢܐ nullptr;
            }
        } ܐܚܪܢܐ {
            // The character decomposes, get everything from the variable-length extra data.
            ثابت uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            ܐܢ (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } ܐܚܪܢܐ {
                leadCC = 0;
            }
            ܐܢ (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                ܦܢܐ prevSrc;
            }
            ܐܢ (!buffer.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, ܫܪܪܐ, leadCC, trailCC, errorCode)) {
                ܦܢܐ nullptr;
            }
        }
        ܐܢ ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            ܦܢܐ src;
        }
    }
    ܦܢܐ src;
}

ثابت char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) ثابت {
    uint16_t norm16;
    ܐܢ(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        ܦܢܐ nullptr;
    }
    ثابت char16_t *decomp = nullptr;
    ܐܢ(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    ܐܢ (norm16 < minYesNo) {
        ܦܢܐ decomp;
    } ܐܚܪܢܐ ܐܢ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        ܦܢܐ buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    ܦܢܐ reinterpret_cast<ثابت char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ثابت char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) ثابت {
    uint16_t norm16;
    ܐܢ(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        ܦܢܐ nullptr;
    } ܐܚܪܢܐ ܐܢ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        ܦܢܐ buffer;
    } ܐܚܪܢܐ ܐܢ(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        ܦܢܐ buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    ܐܢ(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ثابت uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        ܐܢ(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            ܦܢܐ reinterpret_cast<ثابت char16_t*>(rawMapping) - rm0;
        } ܐܚܪܢܐ {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<ثابت char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            ܦܢܐ buffer;
        }
    } ܐܚܪܢܐ {
        length=mLength;
        ܦܢܐ reinterpret_cast<ثابت char16_t*>(mapping) + 1;
    }
}

فراغ Normalizer2Impl::decomposeAndAppend(ثابت char16_t *src, ثابت char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safeMiddle,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) ثابت {
    buffer.copyReorderableSuffixTo(safeMiddle);
    ܐܢ(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        ܦܢܐ;
    }
    // Just merge the strings at the boundary.
    منطقي isFirst = ܫܪܪܐ;
    uint8_t firstCC = 0, prevCC = 0, cc;
    ثابت char16_t *p = src;
    ܟܡܐ (p != limit) {
        ثابت char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ܐܢ ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            اكسر;
        }
        ܐܢ (isFirst) {
            firstCC = cc;
            isFirst = ܕܓܠܐ;
        }
        prevCC = cc;
    }
    ܐܢ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    ܐܢ (buffer.append(src, static_cast<int32_t>(p - src), ܕܓܠܐ, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ثابت {
    ܦܢܐ c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ثابت {
    ܐܢ (norm16 < minNoNoCompNoMaybeCC) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܐܢ (norm16 >= limitNoNo) {
        ܦܢܐ norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ܦܢܐ (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ثابت {
    ܐܢ (c < minDecompNoCP) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܐܢ (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܦܢܐ norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ثابت {
    ܐܢ(norm16 <= minYesNo || isHangulLVT(norm16)) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܐܢ (norm16 >= limitNoNo) {
        ܐܢ (isMaybeYesOrNonZeroCC(norm16)) {
            ܦܢܐ norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } ܐܚܪܢܐ ܐܢ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            ܦܢܐ (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    ܐܢ(firstUnit>0x1ff) {
        ܦܢܐ ܕܓܠܐ;  // trailCC>1
    }
    ܐܢ(firstUnit<=0xff) {
        ܦܢܐ ܫܪܪܐ;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    ܦܢܐ (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
    uint16_t key1, firstUnit;
    ܐܢ(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        ܟܡܐ(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        ܐܢ(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            ܐܢ(firstUnit&COMP_1_TRIPLE) {
                ܦܢܐ (static_cast<int32_t>(list[1]) << 16) | list[2];
            } ܐܚܪܢܐ {
                ܦܢܐ list[1];
            }
        }
    } ܐܚܪܢܐ {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        لكل(;;) {
            ܐܢ(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } ܐܚܪܢܐ ܐܢ(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                ܐܢ(key2>(secondUnit=list[1])) {
                    ܐܢ(firstUnit&COMP_1_LAST_TUPLE) {
                        اكسر;
                    } ܐܚܪܢܐ {
                        list+=3;
                    }
                } ܐܚܪܢܐ ܐܢ(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    ܦܢܐ (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } ܐܚܪܢܐ {
                    اكسر;
                }
            } ܐܚܪܢܐ {
                اكسر;
            }
        }
    }
    ܦܢܐ -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
فراغ Normalizer2Impl::addComposites(ثابت uint16_t *list, UnicodeSet &set) ثابت {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    ܥܒܕ {
        firstUnit=*list;
        ܐܢ((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } ܐܚܪܢܐ {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        ܐܢ((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } ܟܡܐ((firstUnit&COMP_1_LAST_TUPLE)==0);
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
فراغ Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) ثابت {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    ܐܢ(p==limit) {
        ܦܢܐ;
    }

    char16_t *starter, *pRemove, *q, *r;
    ثابت uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=ܕܓܠܐ;
    prevCC=0;

    لكل(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        ܐܢ( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            ܐܢ(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                ܐܢ(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    ܐܢ(prev<Hangul::JAMO_L_COUNT) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_COUNT+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_COUNT);
                        char16_t t;
                        ܐܢ (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_COUNT) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        ܟܡܐ(r<limit) {
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
                ܐܢ(p==limit) {
                    اكسر;
                }
                compositionsList=nullptr;
                استمر;
            } ܐܚܪܢܐ ܐܢ((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                ܐܢ(starterIsSupplementary) {
                    ܐܢ(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } ܐܚܪܢܐ {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=ܕܓܠܐ;
                        q=starter+1;
                        r=q+1;
                        ܟܡܐ(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } ܐܚܪܢܐ ܐܢ(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=ܫܪܪܐ;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    ܟܡܐ(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } ܐܚܪܢܐ {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                ܐܢ(pRemove<p) {
                    q=pRemove;
                    r=p;
                    ܟܡܐ(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                ܐܢ(p==limit) {
                    اكسر;
                }
                // Is the composite a starter that combines forward?
                ܐܢ(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } ܐܚܪܢܐ {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                استمر;
            }
        }

        // no combination this time
        prevCC=cc;
        ܐܢ(p==limit) {
            اكسر;
        }

        // If c did not combine, then check if it is a starter.
        ܐܢ(cc==0) {
            // Found a new starter.
            ܐܢ((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                ܐܢ(U_IS_BMP(c)) {
                    starterIsSupplementary=ܕܓܠܐ;
                    starter=p-1;
                } ܐܚܪܢܐ {
                    starterIsSupplementary=ܫܪܪܐ;
                    starter=p-2;
                }
            }
        } ܐܚܪܢܐ ܐܢ(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) ثابت {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    ثابت uint16_t *list;
    ܐܢ(isInert(norm16)) {
        ܦܢܐ U_SENTINEL;
    } ܐܚܪܢܐ ܐܢ(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        ܐܢ(isJamoL(norm16)) {
            ܐܢ (b < Hangul::JAMO_V_BASE) {
                ܦܢܐ U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            ܐܢ(b<Hangul::JAMO_V_COUNT) {
                ܦܢܐ
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_COUNT+b)*
                     Hangul::JAMO_T_COUNT);
            } ܐܚܪܢܐ {
                ܦܢܐ U_SENTINEL;
            }
        } ܐܚܪܢܐ ܐܢ(isHangulLV(norm16)) {
            ܐܢ (b <= Hangul::JAMO_T_BASE) {
               ܦܢܐ U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            ܐܢ(b<Hangul::JAMO_T_COUNT) {  // not b==0!
                ܦܢܐ a+b;
            } ܐܚܪܢܐ {
                ܦܢܐ U_SENTINEL;
            }
        } ܐܚܪܢܐ {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            ܐܢ(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } ܐܚܪܢܐ ܐܢ(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        ܦܢܐ U_SENTINEL;
    } ܐܚܪܢܐ {
        list=getDataForMaybe(norm16);
        ܐܢ(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    ܐܢ(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        ܦܢܐ U_SENTINEL;
    }
#ܐܢ U_SIGNED_RIGHT_SHIFT_IS_ARITHMETIC
    ܦܢܐ combine(list, b)>>1;
#ܐܚܪܢܐ
    int32_t compositeAndFwd=combine(list, b);
    ܦܢܐ compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ثابت char16_t *src, ثابت char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) ثابت {
    ثابت char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ܐܢ(limit==nullptr) {
        src=copyLowPrefixFromNulTerminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ ܕܓܠܐ;
        }
        limit=u_strchr(src, 0);
        ܐܢ (prevBoundary != src) {
            ܐܢ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ܐܚܪܢܐ {
                buffer.removeSuffix(1);
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
            ܐܢ (src == limit) {
                ܐܢ (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                ܦܢܐ ܫܪܪܐ;
            }
            ܐܢ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ܐܚܪܢܐ {
                prevSrc = src++;
                ܐܢ(!U16_IS_LEAD(c)) {
                    اكسر;
                } ܐܚܪܢܐ {
                    char16_t c2;
                    ܐܢ(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ܐܢ(!isCompYesAndZeroCC(norm16)) {
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
        ܐܢ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ܐܢ (!doCompose) {
                ܦܢܐ ܕܓܠܐ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ܐܢ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ܐܢ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ܐܢ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        اكسر;
                    }
                    ܐܢ(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } ܐܚܪܢܐ ܐܢ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ܐܢ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ܐܢ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        اكسر;
                    }
                    ثابت char16_t *mapping = reinterpret_cast<ثابت char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    ܐܢ(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } ܐܚܪܢܐ ܐܢ (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ܐܢ (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ܐܢ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ܐܚܪܢܐ ܐܢ (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            ܐܢ(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                ܐܢ(l<Hangul::JAMO_L_COUNT) {
                    ܐܢ (!doCompose) {
                        ܦܢܐ ܕܓܠܐ;
                    }
                    int32_t t;
                    ܐܢ (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_COUNT) {
                        // The next character is a Jamo T.
                        ++src;
                    } ܐܚܪܢܐ ܐܢ (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } ܐܚܪܢܐ {
                        t = -1;
                    }
                    ܐܢ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_COUNT + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_COUNT + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        ܐܢ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            اكسر;
                        }
                        ܐܢ (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
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
            } ܐܚܪܢܐ ܐܢ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ܐܢ (!doCompose) {
                    ܦܢܐ ܕܓܠܐ;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                ܐܢ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    اكسر;
                }
                ܐܢ (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    اكسر;
                }
                prevBoundary = src;
                استمر;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ܐܚܪܢܐ ܐܢ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            ܐܢ (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ܐܢ (!doCompose) {
                    ܦܢܐ ܕܓܠܐ;
                }
            } ܐܚܪܢܐ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *nextSrc;
                uint16_t n16;
                لكل (;;) {
                    ܐܢ (src == limit) {
                        ܐܢ (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        ܦܢܐ ܫܪܪܐ;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    ܐܢ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        ܐܢ (prevCC > cc) {
                            ܐܢ (!doCompose) {
                                ܦܢܐ ܕܓܠܐ;
                            }
                            اكسر;
                        }
                    } ܐܚܪܢܐ {
                        اكسر;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ܐܢ (norm16HasCompBoundaryBefore(n16)) {
                    ܐܢ (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    استمر;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ܐܢ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ثابت char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            ܐܢ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ܐܢ (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            اكسر;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, ܕܓܠܐ /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, ܫܪܪܐ /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        ܐܢ (U_FAILURE(errorCode)) {
            اكسر;
        }
        ܐܢ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ ܫܪܪܐ;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        ܐܢ(!doCompose) {
            ܐܢ(!buffer.equals(prevSrc, src)) {
                ܦܢܐ ܕܓܠܐ;
            }
            buffer.remove();
        }
        prevBoundary=src;
    }
    ܦܢܐ ܫܪܪܐ;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ثابت char16_t *
Normalizer2Impl::composeQuickCheck(ثابت char16_t *src, ثابت char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) ثابت {
    ثابت char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    ܐܢ(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copyLowPrefixFromNulTerminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        ܐܢ (prevBoundary != src) {
            ܐܢ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } ܐܚܪܢܐ {
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
            ܐܢ(src==limit) {
                ܦܢܐ src;
            }
            ܐܢ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } ܐܚܪܢܐ {
                prevSrc = src++;
                ܐܢ(!U16_IS_LEAD(c)) {
                    اكسر;
                } ܐܚܪܢܐ {
                    char16_t c2;
                    ܐܢ(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        ܐܢ(!isCompYesAndZeroCC(norm16)) {
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
        ܐܢ (prevBoundary != prevSrc) {
            ܐܢ (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } ܐܚܪܢܐ {
                ثابت char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                ܐܢ (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } ܐܚܪܢܐ {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        ܐܢ (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            ܐܢ (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } ܐܚܪܢܐ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *nextSrc;
                لكل (;;) {
                    ܐܢ (norm16 < MIN_YES_YES_WITH_CC) {
                        ܐܢ (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } ܐܚܪܢܐ {
                            ܦܢܐ prevBoundary;
                        }
                    }
                    ܐܢ (src == limit) {
                        ܦܢܐ src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    ܐܢ (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        ܐܢ (!(prevCC <= cc || cc == 0)) {
                            اكسر;
                        }
                    } ܐܚܪܢܐ {
                        اكسر;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                ܐܢ (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    استمر;
                }
            }
        }
        ܐܢ(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        ܦܢܐ prevBoundary;
    }
}

فراغ Normalizer2Impl::composeAndAppend(ثابت char16_t *src, ثابت char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safeMiddle,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ثابت {
    ܐܢ(!buffer.isEmpty()) {
        ثابت char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        ܐܢ(src!=firstStarterInSrc) {
            ثابت char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString middle(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safeMiddle=middle;
            middle.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            ثابت char16_t *middleStart=middle.getBuffer();
            compose(middleStart, middleStart+middle.length(), onlyContiguous,
                    ܫܪܪܐ, buffer, errorCode);
            ܐܢ(U_FAILURE(errorCode)) {
                ܦܢܐ;
            }
            src=firstStarterInSrc;
        }
    }
    ܐܢ(doCompose) {
        compose(src, limit, onlyContiguous, ܫܪܪܐ, buffer, errorCode);
    } ܐܚܪܢܐ {
        ܐܢ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ثابت uint8_t *src, ثابت uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) ثابت {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ثابت uint8_t *prevBoundary = src;

    لكل (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت uint8_t *prevSrc;
        uint16_t norm16 = 0;
        لكل (;;) {
            ܐܢ (src == limit) {
                ܐܢ (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                ܦܢܐ ܫܪܪܐ;
            }
            ܐܢ (*src < minNoMaybeLead) {
                ++src;
            } ܐܚܪܢܐ {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                ܐܢ (!isCompYesAndZeroCC(norm16)) {
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
        ܐܢ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            ܐܢ (sink == nullptr) {
                ܦܢܐ ܕܓܠܐ;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            ܐܢ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                ܐܢ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ܐܢ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        اكسر;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    استمر;
                }
            } ܐܚܪܢܐ ܐܢ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                ܐܢ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    ܐܢ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        اكسر;
                    }
                    ثابت uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    ܐܢ (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ثابت char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        اكسر;
                    }
                    prevBoundary = src;
                    استمر;
                }
            } ܐܚܪܢܐ ܐܢ (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                ܐܢ (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    ܐܢ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        اكسر;
                    }
                    ܐܢ (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    استمر;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } ܐܚܪܢܐ ܐܢ (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            ܐܢ (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                ܐܢ (static_cast<uint32_t>(l) < Hangul::JAMO_L_COUNT) {
                    ܐܢ (sink == nullptr) {
                        ܦܢܐ ܕܓܠܐ;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    ܐܢ (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } ܐܚܪܢܐ ܐܢ (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    ܐܢ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_COUNT + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_COUNT + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        ܐܢ (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            اكسر;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
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
            } ܐܚܪܢܐ ܐܢ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                ܐܢ (sink == nullptr) {
                    ܦܢܐ ܕܓܠܐ;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                ܐܢ (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    اكسر;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                استمر;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } ܐܚܪܢܐ ܐܢ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            ܐܢ (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                ܐܢ (sink == nullptr) {
                    ܦܢܐ ܕܓܠܐ;
                }
            } ܐܚܪܢܐ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت uint8_t *nextSrc;
                uint16_t n16;
                لكل (;;) {
                    ܐܢ (src == limit) {
                        ܐܢ (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        ܦܢܐ ܫܪܪܐ;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    ܐܢ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        ܐܢ (prevCC > cc) {
                            ܐܢ (sink == nullptr) {
                                ܦܢܐ ܕܓܠܐ;
                            }
                            اكسر;
                        }
                    } ܐܚܪܢܐ {
                        اكسر;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                ܐܢ (norm16HasCompBoundaryBefore(n16)) {
                    ܐܢ (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    استمر;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        ܐܢ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ثابت uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            ܐܢ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*هذا, s16, errorCode);
        ܐܢ (U_FAILURE(errorCode)) {
            اكسر;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        ܐܢ (U_FAILURE(errorCode)) {
            اكسر;
        }
        ܐܢ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ ܫܪܪܐ;
        }
        recompose(buffer, 0, onlyContiguous);
        ܐܢ (!buffer.equals(prevSrc, src)) {
            ܐܢ (sink == nullptr) {
                ܦܢܐ ܕܓܠܐ;
            }
            ܐܢ (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                اكسر;
            }
            ܐܢ (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                اكسر;
            }
            prevBoundary = src;
        }
    }
    ܦܢܐ ܫܪܪܐ;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت char16_t *src, ثابت char16_t *limit) ثابت {
    ܐܢ (src == limit || *src < minCompNoMaybeCP) {
        ܦܢܐ ܫܪܪܐ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    ܦܢܐ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت uint8_t *src, ثابت uint8_t *limit) ثابت {
    ܐܢ (src == limit) {
        ܦܢܐ ܫܪܪܐ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    ܦܢܐ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت char16_t *start, ثابت char16_t *p,
                                            UBool onlyContiguous) ثابت {
    ܐܢ (start == p) {
        ܦܢܐ ܫܪܪܐ;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    ܦܢܐ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت uint8_t *start, ثابت uint8_t *p,
                                            UBool onlyContiguous) ثابت {
    ܐܢ (start == p) {
        ܦܢܐ ܫܪܪܐ;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    ܦܢܐ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ثابت char16_t *Normalizer2Impl::findPreviousCompBoundary(ثابت char16_t *start, ثابت char16_t *p,
                                                       UBool onlyContiguous) ثابت {
    ܟܡܐ (p != start) {
        ثابت char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        ܐܢ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ܦܢܐ codePointLimit;
        }
        ܐܢ (hasCompBoundaryBefore(c, norm16)) {
            ܦܢܐ p;
        }
    }
    ܦܢܐ p;
}

ثابت char16_t *Normalizer2Impl::findNextCompBoundary(ثابت char16_t *p, ثابت char16_t *limit,
                                                   UBool onlyContiguous) ثابت {
    ܟܡܐ (p != limit) {
        ثابت char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ܐܢ (hasCompBoundaryBefore(c, norm16)) {
            ܦܢܐ codePointStart;
        }
        ܐܢ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            ܦܢܐ p;
        }
    }
    ܦܢܐ p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ثابت char16_t *start, ثابت char16_t *p) ثابت {
    ܐܢ (start == p) {
        ܦܢܐ 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    ܦܢܐ static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ثابت uint8_t *start, ثابت uint8_t *p) ثابت {
    ܐܢ (start == p) {
        ܦܢܐ 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    ܦܢܐ static_cast<uint8_t>(getFCD16(c));
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
#ܐܢ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) ثابت {
    uint16_t norm16=getNorm16(c);
    ܐܢ (norm16 >= limitNoNo) {
        ܐܢ(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            ܦܢܐ norm16|(norm16<<8);
        } ܐܚܪܢܐ ܐܢ(norm16>=minMaybeYes) {
            ܦܢܐ 0;
        } ܐܚܪܢܐ ܐܢ(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            ܐܢ (deltaTrailCC <= DELTA_TCCC_1) {
                ܦܢܐ deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    ܐܢ(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        ܦܢܐ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    ܐܢ(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    ܦܢܐ norm16;
}
#ܐܢ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ثابت {
    U_ASSERT(norm16 >= minMaybeNo);
    ܐܢ (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        ܦܢܐ norm16 | (norm16<<8);
    } ܐܚܪܢܐ ܐܢ (norm16 >= minMaybeYes) {
        ܦܢܐ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    ܦܢܐ firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ثابت char16_t *
Normalizer2Impl::makeFCD(ثابت char16_t *src, ثابت char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) ثابت {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ثابت char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    ܐܢ(limit==nullptr) {
        src=copyLowPrefixFromNulTerminated(src, minLcccCP, buffer, errorCode);
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ src;
        }
        ܐܢ(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            ܐܢ(prevFCD16>1) {
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

    ثابت char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    لكل(;;) {
        // count code units with lccc==0
        لكل(prevSrc=src; src!=limit;) {
            ܐܢ((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } ܐܚܪܢܐ ܐܢ(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } ܐܚܪܢܐ {
                ܐܢ(U16_IS_LEAD(c)) {
                    char16_t c2;
                    ܐܢ((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                ܐܢ((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } ܐܚܪܢܐ {
                    اكسر;
                }
            }
        }
        // copy these code units all at once
        ܐܢ(src!=prevSrc) {
            ܐܢ(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                اكسر;
            }
            ܐܢ(src==limit) {
                اكسر;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            ܐܢ(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                ܐܢ(prev<minDecompNoCP) {
                    prevFCD16=0;
                } ܐܚܪܢܐ {
                    prevFCD16=getFCD16FromNormData(prev);
                    ܐܢ(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } ܐܚܪܢܐ {
                ثابت char16_t *p=src-1;
                ܐܢ(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                ܐܢ(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } ܐܚܪܢܐ ܐܢ(src==limit) {
            اكسر;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        ܐܢ((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            ܐܢ((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            ܐܢ(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                اكسر;
            }
            prevFCD16=fcd16;
            استمر;
        } ܐܚܪܢܐ ܐܢ(buffer==nullptr) {
            ܦܢܐ prevBoundary;  // quick check "no"
        } ܐܚܪܢܐ {
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
            decomposeShort(prevBoundary, src, ܕܓܠܐ, ܕܓܠܐ, *buffer, errorCode);
            ܐܢ (U_FAILURE(errorCode)) {
                اكسر;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    ܦܢܐ src;
}

فراغ Normalizer2Impl::makeFCDAndAppend(ثابت char16_t *src, ثابت char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safeMiddle,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ثابت {
    ܐܢ(!buffer.isEmpty()) {
        ثابت char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        ܐܢ(src!=firstBoundaryInSrc) {
            ثابت char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString middle(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safeMiddle=middle;
            middle.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            ثابت char16_t *middleStart=middle.getBuffer();
            makeFCD(middleStart, middleStart+middle.length(), &buffer, errorCode);
            ܐܢ(U_FAILURE(errorCode)) {
                ܦܢܐ;
            }
            src=firstBoundaryInSrc;
        }
    }
    ܐܢ(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } ܐܚܪܢܐ {
        ܐܢ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

ثابت char16_t *Normalizer2Impl::findPreviousFCDBoundary(ثابت char16_t *start, ثابت char16_t *p) ثابت {
    ܟܡܐ(start<p) {
        ثابت char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        ܐܢ (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            ܦܢܐ codePointLimit;
        }
        ܐܢ (norm16HasDecompBoundaryBefore(norm16)) {
            ܦܢܐ p;
        }
    }
    ܦܢܐ p;
}

ثابت char16_t *Normalizer2Impl::findNextFCDBoundary(ثابت char16_t *p, ثابت char16_t *limit) ثابت {
    ܟܡܐ(p<limit) {
        ثابت char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        ܐܢ (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            ܦܢܐ codePointStart;
        }
        ܐܢ (norm16HasDecompBoundaryAfter(norm16)) {
            ܦܢܐ p;
        }
    }
    ܦܢܐ p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

فراغ CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canonValue = umutablecptrie_get(mutableTrie, decompLead);
    ܐܢ((canonValue&(CANON_HAS_SET|CANON_VALUE_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canonValue|origin, &errorCode);
    } ܐܚܪܢܐ {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        ܐܢ((canonValue&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(جديد UnicodeSet, errorCode);
            set=lpSet.getAlias();
            ܐܢ(U_FAILURE(errorCode)) {
                ܦܢܐ;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canonValue & CANON_VALUE_MASK);
            canonValue = (canonValue & ~CANON_VALUE_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canonValue, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            ܐܢ (U_FAILURE(errorCode)) {
                ܦܢܐ;
            }
            ܐܢ(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } ܐܚܪܢܐ {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canonValue & CANON_VALUE_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
صنف InitCanonIterData {
عام:
    ساكن فراغ doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ساكن فراغ U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

فراغ InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = جديد CanonIterData(errorCode);
    ܐܢ (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ܐܢ (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        ܟܡܐ ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            ܐܢ (value != Normalizer2Impl::INERT) {
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
    ܐܢ (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

فراغ Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, ثابت uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) ثابت {
    ܐܢ(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        ܦܢܐ;
    }
    لكل(UChar32 c=start; c<=end; ++c) {
        uint32_t oldValue = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t newValue=oldValue;
        ܐܢ(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            newValue|=CANON_NOT_SEGMENT_STARTER;
            ܐܢ(norm16<MIN_NORMAL_MAYBE_YES) {
                newValue|=CANON_HAS_COMPOSITIONS;
            }
        } ܐܚܪܢܐ ܐܢ(norm16<minYesNo) {
            newValue|=CANON_HAS_COMPOSITIONS;
        } ܐܚܪܢܐ {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            ܐܢ (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            ܐܢ (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ثابت uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                ܐܢ((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    ܐܢ(c==c2 && (*(mapping-1)&0xff)!=0) {
                        newValue|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                ܐܢ(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    ܐܢ(norm16_2>=minNoNo) {
                        ܟܡܐ(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2Value = umutablecptrie_get(newData.mutableTrie, c2);
                            ܐܢ((c2Value&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2Value|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } ܐܚܪܢܐ {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        ܐܢ(newValue!=oldValue) {
            umutablecptrie_set(newData.mutableTrie, c, newValue, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) ثابت {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(هذا);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    ܦܢܐ U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::getCanonValue(UChar32 c) ثابت {
    ܦܢܐ static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

ثابت UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) ثابت {
    ܦܢܐ *static_cast<ثابت UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ثابت {
    ܦܢܐ getCanonValue(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) ثابت {
    int32_t canonValue=getCanonValue(c)&~CANON_NOT_SEGMENT_STARTER;
    ܐܢ(canonValue==0) {
        ܦܢܐ ܕܓܠܐ;
    }
    set.clear();
    int32_t value=canonValue&CANON_VALUE_MASK;
    ܐܢ((canonValue&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } ܐܚܪܢܐ ܐܢ(value!=0) {
        set.add(value);
    }
    ܐܢ((canonValue&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        ܐܢ(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_COUNT);
            set.add(syllable, syllable+Hangul::JAMO_VT_COUNT-1);
        } ܐܚܪܢܐ {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    ܦܢܐ ܫܪܪܐ;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(ثابت UDataSwapper *ds,
            ثابت فراغ *inData, int32_t length, فراغ *outData,
            UErrorCode *pErrorCode) {
    ثابت UDataInfo *pInfo;
    int32_t headerSize;

    ثابت uint8_t *inBytes;
    uint8_t *outBytes;

    ثابت int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ܐܢ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ܦܢܐ 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    ܐܢ(!(
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
        ܦܢܐ 0;
    }

    inBytes=(ثابت uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(ثابت int32_t *)inBytes;
    int32_t minIndexesLength;
    ܐܢ(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } ܐܚܪܢܐ ܐܢ(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } ܐܚܪܢܐ ܐܢ(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } ܐܚܪܢܐ {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    ܐܢ(length>=0) {
        length-=headerSize;
        ܐܢ(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ 0;
        }
    }

    /* read the first few indexes */
    لكل(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    ܐܢ(length>=0) {
        ܐܢ(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ 0;
        }

        /* copy the data for inaccessible bytes */
        ܐܢ(inBytes!=outBytes) {
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

    ܦܢܐ headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
