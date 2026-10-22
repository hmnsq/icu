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

#जर !UCONFIG_NO_NORMALIZATION

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
ओळीत uint8_t leadByteForCP(UChar32 c) {
    जर (c <= 0x7f) {
        परत static_cast<uint8_t>(c);
    } अन्यथा जर (c <= 0x7ff) {
        परत static_cast<uint8_t>(0xc0 + (c >> 6));
    } अन्यथा {
        // Should not occur because ccc(U+0300)!=0.
        परत 0xe0;
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
UChar32 codePointFromValidUTF8(स्थिरांक uint8_t *cpStart, स्थिरांक uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    निवड(cpLimit-cpStart) {
    स्थिती 1:
        परत c;
    स्थिती 2:
        परत ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    स्थिती 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        परत static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    स्थिती 4:
        परत ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    पूर्वनिर्धारित:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(स्थिरांक uint8_t *start, स्थिरांक uint8_t *p) {
    जर ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        जर (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            परत ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    परत U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(स्थिरांक uint8_t *src, स्थिरांक uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    जर ((limit - src) >= 3 && *src == 0xe1) {
        जर (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            जर (0xa8 <= t && t <= 0xbf) {
                परत t - 0xa7;
            }
        } अन्यथा जर (src[1] == 0x87) {
            uint8_t t = src[2];
            जर (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                परत t - (0xa7 - 0x40);
            }
        }
    }
    परत -1;
}

शून्य
appendCodePointDelta(स्थिरांक uint8_t *cpStart, स्थिरांक uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    अक्षर buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    जर (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } अन्यथा {
        int32_t trail = *(cpLimit-1) + delta;
        जर (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            करा { buffer[length++] = *cpStart++; } पर्यंत (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } अन्यथा {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    जर (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(स्थिरांक Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    जर (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    जर(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत असत्य;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    जर(start==limit) {
        lastCC=0;
    } अन्यथा {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        जर(lastCC>1) {
            पर्यंत(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    परत सत्य;
}

UBool ReorderingBuffer::equals(स्थिरांक char16_t *otherStart, स्थिरांक char16_t *otherLimit) स्थिरांक {
    int32_t length = static_cast<int32_t>(limit - start);
    परत
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(स्थिरांक uint8_t *otherStart, स्थिरांक uint8_t *otherLimit) स्थिरांक {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    जर (otherLength < length || (otherLength / 3) > length) {
        परत असत्य;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    साठी (int32_t i = 0, j = 0;;) {
        जर (i >= length) {
            परत j >= otherLength;
        } अन्यथा जर (j >= otherLength) {
            परत असत्य;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        जर (c != other) {
            परत असत्य;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    जर(remainingCapacity<2 && !resize(2, errorCode)) {
        परत असत्य;
    }
    जर(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        जर(cc<=1) {
            reorderStart=limit;
        }
    } अन्यथा {
        insert(c, cc);
    }
    remainingCapacity-=2;
    परत सत्य;
}

UBool ReorderingBuffer::append(स्थिरांक char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    जर(length==0) {
        परत सत्य;
    }
    जर(remainingCapacity<length && !resize(length, errorCode)) {
        परत असत्य;
    }
    remainingCapacity-=length;
    जर(lastCC<=leadCC || leadCC==0) {
        जर(trailCC<=1) {
            reorderStart=limit+length;
        } अन्यथा जर(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        स्थिरांक char16_t *sLimit=s+length;
        करा { *limit++=*s++; } पर्यंत(s!=sLimit);
        lastCC=trailCC;
    } अन्यथा {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        पर्यंत(i<length) {
            U16_NEXT(s, i, length, c);
            जर(i<length) {
                जर (isNFD) {
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
    परत सत्य;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    जर(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        परत असत्य;
    }
    remainingCapacity-=cpLength;
    जर(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } अन्यथा {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    परत सत्य;
}

UBool ReorderingBuffer::appendZeroCC(स्थिरांक char16_t *s, स्थिरांक char16_t *sLimit, UErrorCode &errorCode) {
    जर(s==sLimit) {
        परत सत्य;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    जर(remainingCapacity<length && !resize(length, errorCode)) {
        परत असत्य;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    परत सत्य;
}

शून्य ReorderingBuffer::हटाओ() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

शून्य ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    जर(suffixLength<(limit-start)) {
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
    जर(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    जर(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    जर(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत असत्य;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    परत सत्य;
}

शून्य ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    जर(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    जर(reorderStart>=codePointStart) {
        परत 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    जर(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    परत impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
शून्य ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    साठी(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    करा {
        *--r=*--q;
    } पर्यंत(codePointLimit!=q);
    writeCodePoint(q, c);
    जर(cc<=1) {
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
Normalizer2Impl::init(स्थिरांक int32_t *inIndexes, स्थिरांक UCPTrie *inTrie,
                      स्थिरांक uint16_t *inExtraData, स्थिरांक uint8_t *inSmallFCD) {
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
segmentStarterMapper(स्थिरांक शून्य * /*context*/, uint32_t value) {
    परत value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

शून्य
Normalizer2Impl::addLcccChars(UnicodeSet &set) स्थिरांक {
    UChar32 start = 0, end;
    uint32_t norm16;
    पर्यंत ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        जर (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } अन्यथा जर (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            जर (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

शून्य
Normalizer2Impl::addPropertyStarts(स्थिरांक USetAdder *sa, UErrorCode & /*errorCode*/) स्थिरांक {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    पर्यंत ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        जर (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            पर्यंत (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                जर (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    साठी(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_मोजणी) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

शून्य
Normalizer2Impl::addCanonIterPropertyStarts(स्थिरांक USetAdder *sa, UErrorCode &errorCode) स्थिरांक {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    जर (!ensureCanonIterData(errorCode)) { परत; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    पर्यंत ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

स्थिरांक char16_t *
Normalizer2Impl::copy_खालचा_Prefix_From_Nul_Terminated(स्थिरांक char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) स्थिरांक {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    स्थिरांक char16_t *prevSrc=src;
    char16_t c;
    पर्यंत((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    जर(--src!=prevSrc) {
        जर(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    परत src;
}

UnicodeString &
Normalizer2Impl::decompose(स्थिरांक UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) स्थिरांक {
    जर(U_FAILURE(errorCode)) {
        dest.setToBogus();
        परत dest;
    }
    स्थिरांक char16_t *sArray=src.getBuffer();
    जर(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        परत dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    परत dest;
}

शून्य
Normalizer2Impl::decompose(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) स्थिरांक {
    जर(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.हटाओ();
    ReorderingBuffer buffer(*हे, dest);
    जर(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
स्थिरांक char16_t *
Normalizer2Impl::decompose(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) स्थिरांक {
    जर(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत src;
    }
    UChar32 minNoCP=minDecompNoCP;
    जर(limit==nullptr) {
        src=copy_खालचा_Prefix_From_Nul_Terminated(src, minNoCP, buffer, errorCode);
        जर(U_FAILURE(errorCode)) {
            परत src;
        }
        limit=u_strchr(src, 0);
    }

    स्थिरांक char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    स्थिरांक char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    साठी(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        साठी(prevSrc=src; src!=limit;) {
            जर( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा जर(!U16_IS_LEAD(c)) {
                थांबा;
            } अन्यथा {
                char16_t c2;
                जर((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    जर(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } अन्यथा {
                        थांबा;
                    }
                } अन्यथा {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        जर(src!=prevSrc) {
            जर(buffer!=nullptr) {
                जर(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    थांबा;
                }
            } अन्यथा {
                prevCC=0;
                prevBoundary=src;
            }
        }
        जर(src==limit) {
            थांबा;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        जर(buffer!=nullptr) {
            जर(!decompose(c, norm16, *buffer, errorCode)) {
                थांबा;
            }
        } अन्यथा {
            जर(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                जर(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    जर(cc<=1) {
                        prevBoundary=src;
                    }
                    सुरूठेवा;
                }
            }
            परत prevBoundary;  // "no" or cc out of order
        }
    }
    परत src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
स्थिरांक char16_t *
Normalizer2Impl::decomposeShort(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) {
        परत nullptr;
    }
    पर्यंत(src<limit) {
        जर (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            परत src;
        }
        स्थिरांक char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        जर (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            परत prevSrc;
        }
        जर(!decompose(c, norm16, buffer, errorCode)) {
            परत nullptr;
        }
        जर (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            परत src;
        }
    }
    परत src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) स्थिरांक {
    // get the decomposition and the lead and trail cc's
    जर (norm16 >= limitNoNo) {
        जर (isMaybeYesOrNonZeroCC(norm16)) {
            परत buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } अन्यथा जर (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    जर (norm16 < minYesNo) {
        // c does not decompose
        परत buffer.append(c, 0, errorCode);
    } अन्यथा जर(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        परत buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिरांक uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    जर(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } अन्यथा {
        leadCC=0;
    }
    परत buffer.append(reinterpret_cast<स्थिरांक char16_t*>(mapping) + 1, length, सत्य, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
स्थिरांक uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               स्थिरांक uint8_t *src, स्थिरांक uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) स्थिरांक {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    स्थिरांक uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    साठी (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        स्थिरांक uint8_t *fastStart = src;
        स्थिरांक uint8_t *prevSrc;
        uint16_t norm16 = 0;

        साठी (;;) {
            जर (src == limit) {
                जर (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                परत src;
            }
            जर (*src < minNoLead) {
                ++src;
            } अन्यथा {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                जर (!isMostDecompYesAndZeroCC(norm16)) {
                    थांबा;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        जर (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            जर (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                थांबा;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        जर (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            जर (prevCC <= cc || cc == 0) {
                prevCC = cc;
                जर (cc <= 1) {
                    जर (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        थांबा;
                    }
                    prevBoundary = src;
                }
                सुरूठेवा;
            }
        }
        जर (sink == nullptr) {
            परत prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        जर (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            जर (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                थांबा;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*हे, s16, errorCode);
        जर (U_FAILURE(errorCode)) {
            थांबा;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, असत्य /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        जर (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, असत्य /* onlyContiguous */,
                                 buffer, errorCode);
        }
        जर (U_FAILURE(errorCode)) {
            थांबा;
        }
        जर ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            थांबा;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        जर (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            जर (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                थांबा;
            }
        } अन्यथा {
            जर (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                थांबा;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    परत src;
}

स्थिरांक uint8_t *
Normalizer2Impl::decomposeShort(स्थिरांक uint8_t *src, स्थिरांक uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) {
        परत nullptr;
    }
    पर्यंत (src < limit) {
        स्थिरांक uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        जर (norm16 >= limitNoNo) {
            जर (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                जर (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    परत prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                जर (!buffer.append(c, cc, errorCode)) {
                    परत nullptr;
                }
                जर (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    परत src;
                }
                सुरूठेवा;
            } अन्यथा जर (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                जर (stopAt != STOP_AT_LIMIT) {
                    परत prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } अन्यथा जर (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            परत prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        जर (norm16 < minYesNo) {
            जर (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            जर (!buffer.append(c, 0, errorCode)) {
                परत nullptr;
            }
        } अन्यथा जर (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            जर (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            जर (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                परत nullptr;
            }
        } अन्यथा {
            // The character decomposes, get everything from the variable-length extra data.
            स्थिरांक uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            जर (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } अन्यथा {
                leadCC = 0;
            }
            जर (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                परत prevSrc;
            }
            जर (!buffer.append(reinterpret_cast<स्थिरांक char16_t*>(mapping) + 1, length, सत्य, leadCC, trailCC, errorCode)) {
                परत nullptr;
            }
        }
        जर ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            परत src;
        }
    }
    परत src;
}

स्थिरांक char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) स्थिरांक {
    uint16_t norm16;
    जर(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        परत nullptr;
    }
    स्थिरांक char16_t *decomp = nullptr;
    जर(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    जर (norm16 < minYesNo) {
        परत decomp;
    } अन्यथा जर(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        परत buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिरांक uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    परत reinterpret_cast<स्थिरांक char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
स्थिरांक char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) स्थिरांक {
    uint16_t norm16;
    जर(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        परत nullptr;
    } अन्यथा जर(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        परत buffer;
    } अन्यथा जर(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        परत buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिरांक uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    जर(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        स्थिरांक uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        जर(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            परत reinterpret_cast<स्थिरांक char16_t*>(rawMapping) - rm0;
        } अन्यथा {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<स्थिरांक char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            परत buffer;
        }
    } अन्यथा {
        length=mLength;
        परत reinterpret_cast<स्थिरांक char16_t*>(mapping) + 1;
    }
}

शून्य Normalizer2Impl::decomposeAndAppend(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_मधला,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) स्थिरांक {
    buffer.copyReorderableSuffixTo(safe_मधला);
    जर(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        परत;
    }
    // Just merge the strings at the boundary.
    तार्किक isFirst = सत्य;
    uint8_t firstCC = 0, prevCC = 0, cc;
    स्थिरांक char16_t *p = src;
    पर्यंत (p != limit) {
        स्थिरांक char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        जर ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            थांबा;
        }
        जर (isFirst) {
            firstCC = cc;
            isFirst = असत्य;
        }
        prevCC = cc;
    }
    जर(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    जर (buffer.append(src, static_cast<int32_t>(p - src), असत्य, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) स्थिरांक {
    परत c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) स्थिरांक {
    जर (norm16 < minNoNoCompNoMaybeCC) {
        परत सत्य;
    }
    जर (norm16 >= limitNoNo) {
        परत norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिरांक uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    परत (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) स्थिरांक {
    जर (c < minDecompNoCP) {
        परत सत्य;
    }
    जर (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        परत सत्य;
    }
    परत norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) स्थिरांक {
    जर(norm16 <= minYesNo || isHangulLVT(norm16)) {
        परत सत्य;
    }
    जर (norm16 >= limitNoNo) {
        जर (isMaybeYesOrNonZeroCC(norm16)) {
            परत norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } अन्यथा जर (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            परत (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिरांक uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    जर(firstUnit>0x1ff) {
        परत असत्य;  // trailCC>1
    }
    जर(firstUnit<=0xff) {
        परत सत्य;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    परत (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(स्थिरांक uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    जर(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        पर्यंत(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        जर(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            जर(firstUnit&COMP_1_TRIPLE) {
                परत (static_cast<int32_t>(list[1]) << 16) | list[2];
            } अन्यथा {
                परत list[1];
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
        साठी(;;) {
            जर(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } अन्यथा जर(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                जर(key2>(secondUnit=list[1])) {
                    जर(firstUnit&COMP_1_LAST_TUPLE) {
                        थांबा;
                    } अन्यथा {
                        list+=3;
                    }
                } अन्यथा जर(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    परत (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } अन्यथा {
                    थांबा;
                }
            } अन्यथा {
                थांबा;
            }
        }
    }
    परत -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
शून्य Normalizer2Impl::addComposites(स्थिरांक uint16_t *list, UnicodeSet &set) स्थिरांक {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    करा {
        firstUnit=*list;
        जर((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } अन्यथा {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        जर((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } पर्यंत((firstUnit&COMP_1_LAST_TUPLE)==0);
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
                                UBool onlyContiguous) स्थिरांक {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    जर(p==limit) {
        परत;
    }

    char16_t *starter, *pRemove, *q, *r;
    स्थिरांक uint16_t *compositionsList;
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

    साठी(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        जर( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            जर(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                जर(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    जर(prev<Hangul::JAMO_L_मोजणी) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_मोजणी+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_मोजणी);
                        char16_t t;
                        जर (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_मोजणी) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        पर्यंत(r<limit) {
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
                जर(p==limit) {
                    थांबा;
                }
                compositionsList=nullptr;
                सुरूठेवा;
            } अन्यथा जर((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                जर(starterIsSupplementary) {
                    जर(U_IS_SUPPLEMENTARY(composite)) {
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
                        पर्यंत(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } अन्यथा जर(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=सत्य;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    पर्यंत(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } अन्यथा {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                जर(pRemove<p) {
                    q=pRemove;
                    r=p;
                    पर्यंत(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                जर(p==limit) {
                    थांबा;
                }
                // Is the composite a starter that combines forward?
                जर(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } अन्यथा {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                सुरूठेवा;
            }
        }

        // no combination this time
        prevCC=cc;
        जर(p==limit) {
            थांबा;
        }

        // If c did not combine, then check if it is a starter.
        जर(cc==0) {
            // Found a new starter.
            जर((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                जर(U_IS_BMP(c)) {
                    starterIsSupplementary=असत्य;
                    starter=p-1;
                } अन्यथा {
                    starterIsSupplementary=सत्य;
                    starter=p-2;
                }
            }
        } अन्यथा जर(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) स्थिरांक {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    स्थिरांक uint16_t *list;
    जर(isInert(norm16)) {
        परत U_SENTINEL;
    } अन्यथा जर(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        जर(isJamoL(norm16)) {
            जर (b < Hangul::JAMO_V_BASE) {
                परत U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            जर(b<Hangul::JAMO_V_मोजणी) {
                परत
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_मोजणी+b)*
                     Hangul::JAMO_T_मोजणी);
            } अन्यथा {
                परत U_SENTINEL;
            }
        } अन्यथा जर(isHangulLV(norm16)) {
            जर (b <= Hangul::JAMO_T_BASE) {
               परत U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            जर(b<Hangul::JAMO_T_मोजणी) {  // not b==0!
                परत a+b;
            } अन्यथा {
                परत U_SENTINEL;
            }
        } अन्यथा {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            जर(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } अन्यथा जर(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        परत U_SENTINEL;
    } अन्यथा {
        list=getDataForMaybe(norm16);
        जर(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    जर(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        परत U_SENTINEL;
    }
#जर U_SIGNED_उजवा_SHIFT_IS_ARITHMETIC
    परत combine(list, b)>>1;
#अन्यथा
    int32_t compositeAndFwd=combine(list, b);
    परत compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) स्थिरांक {
    स्थिरांक char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    जर(limit==nullptr) {
        src=copy_खालचा_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        जर(U_FAILURE(errorCode)) {
            परत असत्य;
        }
        limit=u_strchr(src, 0);
        जर (prevBoundary != src) {
            जर (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } अन्यथा {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    साठी (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        स्थिरांक char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        साठी (;;) {
            जर (src == limit) {
                जर (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                परत सत्य;
            }
            जर( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा {
                prevSrc = src++;
                जर(!U16_IS_LEAD(c)) {
                    थांबा;
                } अन्यथा {
                    char16_t c2;
                    जर(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        जर(!isCompYesAndZeroCC(norm16)) {
                            थांबा;
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
        जर (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            जर (!doCompose) {
                परत असत्य;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            जर (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                जर (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    जर (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        थांबा;
                    }
                    जर(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        थांबा;
                    }
                    prevBoundary = src;
                    सुरूठेवा;
                }
            } अन्यथा जर (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                जर (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    जर (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        थांबा;
                    }
                    स्थिरांक char16_t *mapping = reinterpret_cast<स्थिरांक char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    जर(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        थांबा;
                    }
                    prevBoundary = src;
                    सुरूठेवा;
                }
            } अन्यथा जर (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                जर (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    जर (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        थांबा;
                    }
                    prevBoundary = src;
                    सुरूठेवा;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } अन्यथा जर (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            जर(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                जर(l<Hangul::JAMO_L_मोजणी) {
                    जर (!doCompose) {
                        परत असत्य;
                    }
                    int32_t t;
                    जर (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_मोजणी) {
                        // The next character is a Jamo T.
                        ++src;
                    } अन्यथा जर (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } अन्यथा {
                        t = -1;
                    }
                    जर (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_मोजणी + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_मोजणी + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        जर (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            थांबा;
                        }
                        जर (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            थांबा;
                        }
                        prevBoundary = src;
                        सुरूठेवा;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } अन्यथा जर (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                जर (!doCompose) {
                    परत असत्य;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                जर (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    थांबा;
                }
                जर (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    थांबा;
                }
                prevBoundary = src;
                सुरूठेवा;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } अन्यथा जर (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            जर (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                जर (!doCompose) {
                    परत असत्य;
                }
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                स्थिरांक char16_t *nextSrc;
                uint16_t n16;
                साठी (;;) {
                    जर (src == limit) {
                        जर (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        परत सत्य;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    जर (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        जर (prevCC > cc) {
                            जर (!doCompose) {
                                परत असत्य;
                            }
                            थांबा;
                        }
                    } अन्यथा {
                        थांबा;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                जर (norm16HasCompBoundaryBefore(n16)) {
                    जर (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    सुरूठेवा;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        जर (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            स्थिरांक char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            जर (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        जर (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            थांबा;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, असत्य /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, सत्य /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        जर (U_FAILURE(errorCode)) {
            थांबा;
        }
        जर ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            परत सत्य;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        जर(!doCompose) {
            जर(!buffer.equals(prevSrc, src)) {
                परत असत्य;
            }
            buffer.हटाओ();
        }
        prevBoundary=src;
    }
    परत सत्य;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
स्थिरांक char16_t *
Normalizer2Impl::composeQuickCheck(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) स्थिरांक {
    स्थिरांक char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    जर(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_खालचा_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        जर (prevBoundary != src) {
            जर (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } अन्यथा {
                prevBoundary = --src;
            }
        }
    }

    साठी(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        स्थिरांक char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        साठी (;;) {
            जर(src==limit) {
                परत src;
            }
            जर( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा {
                prevSrc = src++;
                जर(!U16_IS_LEAD(c)) {
                    थांबा;
                } अन्यथा {
                    char16_t c2;
                    जर(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        जर(!isCompYesAndZeroCC(norm16)) {
                            थांबा;
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
        जर (prevBoundary != prevSrc) {
            जर (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } अन्यथा {
                स्थिरांक char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                जर (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } अन्यथा {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        जर (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            जर (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                स्थिरांक char16_t *nextSrc;
                साठी (;;) {
                    जर (norm16 < MIN_YES_YES_WITH_CC) {
                        जर (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } अन्यथा {
                            परत prevBoundary;
                        }
                    }
                    जर (src == limit) {
                        परत src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    जर (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        जर (!(prevCC <= cc || cc == 0)) {
                            थांबा;
                        }
                    } अन्यथा {
                        थांबा;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                जर (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    सुरूठेवा;
                }
            }
        }
        जर(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        परत prevBoundary;
    }
}

शून्य Normalizer2Impl::composeAndAppend(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_मधला,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) स्थिरांक {
    जर(!buffer.isEmpty()) {
        स्थिरांक char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        जर(src!=firstStarterInSrc) {
            स्थिरांक char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString मधला(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_मधला=मधला;
            मधला.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            स्थिरांक char16_t *मधला_Start=मधला.getBuffer();
            compose(मधला_Start, मधला_Start+मधला.length(), onlyContiguous,
                    सत्य, buffer, errorCode);
            जर(U_FAILURE(errorCode)) {
                परत;
            }
            src=firstStarterInSrc;
        }
    }
    जर(doCompose) {
        compose(src, limit, onlyContiguous, सत्य, buffer, errorCode);
    } अन्यथा {
        जर(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             स्थिरांक uint8_t *src, स्थिरांक uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) स्थिरांक {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    स्थिरांक uint8_t *prevBoundary = src;

    साठी (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        स्थिरांक uint8_t *prevSrc;
        uint16_t norm16 = 0;
        साठी (;;) {
            जर (src == limit) {
                जर (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                परत सत्य;
            }
            जर (*src < minNoMaybeLead) {
                ++src;
            } अन्यथा {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                जर (!isCompYesAndZeroCC(norm16)) {
                    थांबा;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        जर (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            जर (sink == nullptr) {
                परत असत्य;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            जर (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                जर (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    जर (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        थांबा;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    सुरूठेवा;
                }
            } अन्यथा जर (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                जर (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    जर (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        थांबा;
                    }
                    स्थिरांक uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    जर (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<स्थिरांक char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        थांबा;
                    }
                    prevBoundary = src;
                    सुरूठेवा;
                }
            } अन्यथा जर (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                जर (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    जर (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        थांबा;
                    }
                    जर (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    सुरूठेवा;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } अन्यथा जर (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            जर (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                जर (static_cast<uint32_t>(l) < Hangul::JAMO_L_मोजणी) {
                    जर (sink == nullptr) {
                        परत असत्य;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    जर (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } अन्यथा जर (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    जर (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_मोजणी + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_मोजणी + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        जर (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            थांबा;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        सुरूठेवा;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } अन्यथा जर (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                जर (sink == nullptr) {
                    परत असत्य;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                जर (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    थांबा;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                सुरूठेवा;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } अन्यथा जर (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            जर (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                जर (sink == nullptr) {
                    परत असत्य;
                }
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                स्थिरांक uint8_t *nextSrc;
                uint16_t n16;
                साठी (;;) {
                    जर (src == limit) {
                        जर (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        परत सत्य;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    जर (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        जर (prevCC > cc) {
                            जर (sink == nullptr) {
                                परत असत्य;
                            }
                            थांबा;
                        }
                    } अन्यथा {
                        थांबा;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                जर (norm16HasCompBoundaryBefore(n16)) {
                    जर (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    सुरूठेवा;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        जर (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            स्थिरांक uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            जर (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*हे, s16, errorCode);
        जर (U_FAILURE(errorCode)) {
            थांबा;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        जर (U_FAILURE(errorCode)) {
            थांबा;
        }
        जर ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            परत सत्य;
        }
        recompose(buffer, 0, onlyContiguous);
        जर (!buffer.equals(prevSrc, src)) {
            जर (sink == nullptr) {
                परत असत्य;
            }
            जर (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                थांबा;
            }
            जर (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                थांबा;
            }
            prevBoundary = src;
        }
    }
    परत सत्य;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(स्थिरांक char16_t *src, स्थिरांक char16_t *limit) स्थिरांक {
    जर (src == limit || *src < minCompNoMaybeCP) {
        परत सत्य;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    परत norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(स्थिरांक uint8_t *src, स्थिरांक uint8_t *limit) स्थिरांक {
    जर (src == limit) {
        परत सत्य;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    परत norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(स्थिरांक char16_t *start, स्थिरांक char16_t *p,
                                            UBool onlyContiguous) स्थिरांक {
    जर (start == p) {
        परत सत्य;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    परत norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(स्थिरांक uint8_t *start, स्थिरांक uint8_t *p,
                                            UBool onlyContiguous) स्थिरांक {
    जर (start == p) {
        परत सत्य;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    परत norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

स्थिरांक char16_t *Normalizer2Impl::findPreviousCompBoundary(स्थिरांक char16_t *start, स्थिरांक char16_t *p,
                                                       UBool onlyContiguous) स्थिरांक {
    पर्यंत (p != start) {
        स्थिरांक char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        जर (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            परत codePointLimit;
        }
        जर (hasCompBoundaryBefore(c, norm16)) {
            परत p;
        }
    }
    परत p;
}

स्थिरांक char16_t *Normalizer2Impl::findNextCompBoundary(स्थिरांक char16_t *p, स्थिरांक char16_t *limit,
                                                   UBool onlyContiguous) स्थिरांक {
    पर्यंत (p != limit) {
        स्थिरांक char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        जर (hasCompBoundaryBefore(c, norm16)) {
            परत codePointStart;
        }
        जर (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            परत p;
        }
    }
    परत p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(स्थिरांक char16_t *start, स्थिरांक char16_t *p) स्थिरांक {
    जर (start == p) {
        परत 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    परत static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(स्थिरांक uint8_t *start, स्थिरांक uint8_t *p) स्थिरांक {
    जर (start == p) {
        परत 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    परत static_cast<uint8_t>(getFCD16(c));
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
#जर (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) स्थिरांक {
    uint16_t norm16=getNorm16(c);
    जर (norm16 >= limitNoNo) {
        जर(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            परत norm16|(norm16<<8);
        } अन्यथा जर(norm16>=minMaybeYes) {
            परत 0;
        } अन्यथा जर(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            जर (deltaTrailCC <= DELTA_TCCC_1) {
                परत deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    जर(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        परत 0;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिरांक uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    जर(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    परत norm16;
}
#जर (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) स्थिरांक {
    U_ASSERT(norm16 >= minMaybeNo);
    जर (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        परत norm16 | (norm16<<8);
    } अन्यथा जर (norm16 >= minMaybeYes) {
        परत 0;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिरांक uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    परत firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
स्थिरांक char16_t *
Normalizer2Impl::makeFCD(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) स्थिरांक {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    स्थिरांक char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    जर(limit==nullptr) {
        src=copy_खालचा_Prefix_From_Nul_Terminated(src, minLcccCP, buffer, errorCode);
        जर(U_FAILURE(errorCode)) {
            परत src;
        }
        जर(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            जर(prevFCD16>1) {
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

    स्थिरांक char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    साठी(;;) {
        // count code units with lccc==0
        साठी(prevSrc=src; src!=limit;) {
            जर((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } अन्यथा जर(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } अन्यथा {
                जर(U16_IS_LEAD(c)) {
                    char16_t c2;
                    जर((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                जर((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } अन्यथा {
                    थांबा;
                }
            }
        }
        // copy these code units all at once
        जर(src!=prevSrc) {
            जर(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                थांबा;
            }
            जर(src==limit) {
                थांबा;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            जर(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                जर(prev<minDecompNoCP) {
                    prevFCD16=0;
                } अन्यथा {
                    prevFCD16=getFCD16FromNormData(prev);
                    जर(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } अन्यथा {
                स्थिरांक char16_t *p=src-1;
                जर(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                जर(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } अन्यथा जर(src==limit) {
            थांबा;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        जर((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            जर((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            जर(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                थांबा;
            }
            prevFCD16=fcd16;
            सुरूठेवा;
        } अन्यथा जर(buffer==nullptr) {
            परत prevBoundary;  // quick check "no"
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
            जर (U_FAILURE(errorCode)) {
                थांबा;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    परत src;
}

शून्य Normalizer2Impl::makeFCDAndAppend(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_मधला,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) स्थिरांक {
    जर(!buffer.isEmpty()) {
        स्थिरांक char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        जर(src!=firstBoundaryInSrc) {
            स्थिरांक char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString मधला(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_मधला=मधला;
            मधला.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            स्थिरांक char16_t *मधला_Start=मधला.getBuffer();
            makeFCD(मधला_Start, मधला_Start+मधला.length(), &buffer, errorCode);
            जर(U_FAILURE(errorCode)) {
                परत;
            }
            src=firstBoundaryInSrc;
        }
    }
    जर(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } अन्यथा {
        जर(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

स्थिरांक char16_t *Normalizer2Impl::findPreviousFCDBoundary(स्थिरांक char16_t *start, स्थिरांक char16_t *p) स्थिरांक {
    पर्यंत(start<p) {
        स्थिरांक char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        जर (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            परत codePointLimit;
        }
        जर (norm16HasDecompBoundaryBefore(norm16)) {
            परत p;
        }
    }
    परत p;
}

स्थिरांक char16_t *Normalizer2Impl::findNextFCDBoundary(स्थिरांक char16_t *p, स्थिरांक char16_t *limit) स्थिरांक {
    पर्यंत(p<limit) {
        स्थिरांक char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        जर (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            परत codePointStart;
        }
        जर (norm16HasDecompBoundaryAfter(norm16)) {
            परत p;
        }
    }
    परत p;
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
    जर((canon_मूल्य&(CANON_HAS_SET|CANON_मूल्य_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_मूल्य|origin, &errorCode);
    } अन्यथा {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        जर((canon_मूल्य&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(नवीन UnicodeSet, errorCode);
            set=lpSet.getAlias();
            जर(U_FAILURE(errorCode)) {
                परत;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_मूल्य & CANON_मूल्य_MASK);
            canon_मूल्य = (canon_मूल्य & ~CANON_मूल्य_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_मूल्य, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            जर (U_FAILURE(errorCode)) {
                परत;
            }
            जर(firstOrigin!=0) {
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
    impl->fCanonIterData = नवीन CanonIterData(errorCode);
    जर (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    जर (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        पर्यंत ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            जर (value != Normalizer2Impl::INERT) {
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
    जर (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

शून्य Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, स्थिरांक uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) स्थिरांक {
    जर(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        परत;
    }
    साठी(UChar32 c=start; c<=end; ++c) {
        uint32_t old_मूल्य = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t new_मूल्य=old_मूल्य;
        जर(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_मूल्य|=CANON_NOT_SEGMENT_STARTER;
            जर(norm16<MIN_NORMAL_MAYBE_YES) {
                new_मूल्य|=CANON_HAS_COMPOSITIONS;
            }
        } अन्यथा जर(norm16<minYesNo) {
            new_मूल्य|=CANON_HAS_COMPOSITIONS;
        } अन्यथा {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            जर (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            जर (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                स्थिरांक uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                जर((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    जर(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_मूल्य|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                जर(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    जर(norm16_2>=minNoNo) {
                        पर्यंत(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2_मूल्य = umutablecptrie_get(newData.mutableTrie, c2);
                            जर((c2_मूल्य&CANON_NOT_SEGMENT_STARTER)==0) {
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
        जर(new_मूल्य!=old_मूल्य) {
            umutablecptrie_set(newData.mutableTrie, c, new_मूल्य, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) स्थिरांक {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(हे);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    परत U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_मूल्य(UChar32 c) स्थिरांक {
    परत static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

स्थिरांक UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) स्थिरांक {
    परत *static_cast<स्थिरांक UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) स्थिरांक {
    परत get_Canon_मूल्य(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) स्थिरांक {
    int32_t canon_मूल्य=get_Canon_मूल्य(c)&~CANON_NOT_SEGMENT_STARTER;
    जर(canon_मूल्य==0) {
        परत असत्य;
    }
    set.clear();
    int32_t value=canon_मूल्य&CANON_मूल्य_MASK;
    जर((canon_मूल्य&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } अन्यथा जर(value!=0) {
        set.add(value);
    }
    जर((canon_मूल्य&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        जर(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_मोजणी);
            set.add(syllable, syllable+Hangul::JAMO_VT_मोजणी-1);
        } अन्यथा {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    परत सत्य;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(स्थिरांक UDataSwapper *ds,
            स्थिरांक शून्य *inData, int32_t length, शून्य *outData,
            UErrorCode *pErrorCode) {
    स्थिरांक UDataInfo *pInfo;
    int32_t headerSize;

    स्थिरांक uint8_t *inBytes;
    uint8_t *outBytes;

    स्थिरांक int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    जर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        परत 0;
    }

    /* check data format and format version */
    pInfo=(स्थिरांक UDataInfo *)((स्थिरांक अक्षर *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    जर(!(
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
        परत 0;
    }

    inBytes=(स्थिरांक uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(स्थिरांक int32_t *)inBytes;
    int32_t minIndexesLength;
    जर(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } अन्यथा जर(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } अन्यथा जर(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } अन्यथा {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    जर(length>=0) {
        length-=headerSize;
        जर(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }
    }

    /* read the first few indexes */
    साठी(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    जर(length>=0) {
        जर(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }

        /* copy the data for inaccessible bytes */
        जर(inBytes!=outBytes) {
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

    परत headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
