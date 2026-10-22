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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION

#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "unicode/normalizer2.h"
#അന്തര്ഭാവ "unicode/stringoptions.h"
#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/umutablecptrie.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "normalizer2impl.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucptrie_impl.h"
#അന്തര്ഭാവ "uset_imp.h"
#അന്തര്ഭാവ "uvector.h"

U_NAMESPACE_BEGIN

നാമാകാശ {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
വരിയിൽ uint8_t leadByteForCP(UChar32 c) {
    എങ്കിൽ (c <= 0x7f) {
        തിരികെ static_cast<uint8_t>(c);
    } അല്ലെങ്കിൽ എങ്കിൽ (c <= 0x7ff) {
        തിരികെ static_cast<uint8_t>(0xc0 + (c >> 6));
    } അല്ലെങ്കിൽ {
        // Should not occur because ccc(U+0300)!=0.
        തിരികെ 0xe0;
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
UChar32 codePointFromValidUTF8(സ്ഥിരം uint8_t *cpStart, സ്ഥിരം uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    തിരഞ്ഞെടുപ്പ്(cpLimit-cpStart) {
    അവസ്ഥ 1:
        തിരികെ c;
    അവസ്ഥ 2:
        തിരികെ ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    അവസ്ഥ 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        തിരികെ static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    അവസ്ഥ 4:
        തിരികെ ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    സ്ഥിരസ്ഥിതി:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(സ്ഥിരം uint8_t *start, സ്ഥിരം uint8_t *p) {
    എങ്കിൽ ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        എങ്കിൽ (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            തിരികെ ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    തിരികെ U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(സ്ഥിരം uint8_t *src, സ്ഥിരം uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    എങ്കിൽ ((limit - src) >= 3 && *src == 0xe1) {
        എങ്കിൽ (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            എങ്കിൽ (0xa8 <= t && t <= 0xbf) {
                തിരികെ t - 0xa7;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (src[1] == 0x87) {
            uint8_t t = src[2];
            എങ്കിൽ (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                തിരികെ t - (0xa7 - 0x40);
            }
        }
    }
    തിരികെ -1;
}

ശൂന്യം
appendCodePointDelta(സ്ഥിരം uint8_t *cpStart, സ്ഥിരം uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    അക്ഷരം buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    എങ്കിൽ (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } അല്ലെങ്കിൽ {
        int32_t trail = *(cpLimit-1) + delta;
        എങ്കിൽ (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            ചെയ്യുക { buffer[length++] = *cpStart++; } ആകുമ്പോൾ (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } അല്ലെങ്കിൽ {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    എങ്കിൽ (edits != nullptr) {
        edits->addReplace(cpLength, length);
    }
    sink.Append(buffer, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

ReorderingBuffer::ReorderingBuffer(സ്ഥിരം Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.getBuffer(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    എങ്കിൽ (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    എങ്കിൽ(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ അസത്യം;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    എങ്കിൽ(start==limit) {
        lastCC=0;
    } അല്ലെങ്കിൽ {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        എങ്കിൽ(lastCC>1) {
            ആകുമ്പോൾ(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    തിരികെ സത്യം;
}

UBool ReorderingBuffer::equals(സ്ഥിരം char16_t *otherStart, സ്ഥിരം char16_t *otherLimit) സ്ഥിരം {
    int32_t length = static_cast<int32_t>(limit - start);
    തിരികെ
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(സ്ഥിരം uint8_t *otherStart, സ്ഥിരം uint8_t *otherLimit) സ്ഥിരം {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    എങ്കിൽ (otherLength < length || (otherLength / 3) > length) {
        തിരികെ അസത്യം;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    വേണ്ടി (int32_t i = 0, j = 0;;) {
        എങ്കിൽ (i >= length) {
            തിരികെ j >= otherLength;
        } അല്ലെങ്കിൽ എങ്കിൽ (j >= otherLength) {
            തിരികെ അസത്യം;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        എങ്കിൽ (c != other) {
            തിരികെ അസത്യം;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    എങ്കിൽ(remainingCapacity<2 && !resize(2, errorCode)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        എങ്കിൽ(cc<=1) {
            reorderStart=limit;
        }
    } അല്ലെങ്കിൽ {
        insert(c, cc);
    }
    remainingCapacity-=2;
    തിരികെ സത്യം;
}

UBool ReorderingBuffer::append(സ്ഥിരം char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    എങ്കിൽ(length==0) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(remainingCapacity<length && !resize(length, errorCode)) {
        തിരികെ അസത്യം;
    }
    remainingCapacity-=length;
    എങ്കിൽ(lastCC<=leadCC || leadCC==0) {
        എങ്കിൽ(trailCC<=1) {
            reorderStart=limit+length;
        } അല്ലെങ്കിൽ എങ്കിൽ(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        സ്ഥിരം char16_t *sLimit=s+length;
        ചെയ്യുക { *limit++=*s++; } ആകുമ്പോൾ(s!=sLimit);
        lastCC=trailCC;
    } അല്ലെങ്കിൽ {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        ആകുമ്പോൾ(i<length) {
            U16_NEXT(s, i, length, c);
            എങ്കിൽ(i<length) {
                എങ്കിൽ (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } അല്ലെങ്കിൽ {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } അല്ലെങ്കിൽ {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    തിരികെ സത്യം;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    എങ്കിൽ(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        തിരികെ അസത്യം;
    }
    remainingCapacity-=cpLength;
    എങ്കിൽ(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } അല്ലെങ്കിൽ {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    തിരികെ സത്യം;
}

UBool ReorderingBuffer::appendZeroCC(സ്ഥിരം char16_t *s, സ്ഥിരം char16_t *sLimit, UErrorCode &errorCode) {
    എങ്കിൽ(s==sLimit) {
        തിരികെ സത്യം;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    എങ്കിൽ(remainingCapacity<length && !resize(length, errorCode)) {
        തിരികെ അസത്യം;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    തിരികെ സത്യം;
}

ശൂന്യം ReorderingBuffer::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

ശൂന്യം ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    എങ്കിൽ(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } അല്ലെങ്കിൽ {
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
    എങ്കിൽ(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    എങ്കിൽ(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    എങ്കിൽ(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ അസത്യം;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    തിരികെ സത്യം;
}

ശൂന്യം ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    എങ്കിൽ(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    എങ്കിൽ(reorderStart>=codePointStart) {
        തിരികെ 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    എങ്കിൽ(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    തിരികെ impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
ശൂന്യം ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    വേണ്ടി(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    ചെയ്യുക {
        *--r=*--q;
    } ആകുമ്പോൾ(codePointLimit!=q);
    writeCodePoint(q, c);
    എങ്കിൽ(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

ഘടന CanonIterData : പൊതു UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    ശൂന്യം addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

ശൂന്യം
Normalizer2Impl::init(സ്ഥിരം int32_t *inIndexes, സ്ഥിരം UCPTrie *inTrie,
                      സ്ഥിരം uint16_t *inExtraData, സ്ഥിരം uint8_t *inSmallFCD) {
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

സ്ഥിതം uint32_t U_CALLCONV
segmentStarterMapper(സ്ഥിരം ശൂന്യം * /*context*/, uint32_t value) {
    തിരികെ value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

ശൂന്യം
Normalizer2Impl::addLcccChars(UnicodeSet &set) സ്ഥിരം {
    UChar32 start = 0, end;
    uint32_t norm16;
    ആകുമ്പോൾ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        എങ്കിൽ (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } അല്ലെങ്കിൽ എങ്കിൽ (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            എങ്കിൽ (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

ശൂന്യം
Normalizer2Impl::addPropertyStarts(സ്ഥിരം USetAdder *sa, UErrorCode & /*errorCode*/) സ്ഥിരം {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    ആകുമ്പോൾ ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        എങ്കിൽ (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            ആകുമ്പോൾ (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                എങ്കിൽ (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    വേണ്ടി(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_എണ്ണം) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

ശൂന്യം
Normalizer2Impl::addCanonIterPropertyStarts(സ്ഥിരം USetAdder *sa, UErrorCode &errorCode) സ്ഥിരം {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    എങ്കിൽ (!ensureCanonIterData(errorCode)) { തിരികെ; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    ആകുമ്പോൾ ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

സ്ഥിരം char16_t *
Normalizer2Impl::copy_താഴെയുള്ള_Prefix_From_Nul_Terminated(സ്ഥിരം char16_t *src,
                                                UChar32 minNeedDataCP,
                                                ReorderingBuffer *buffer,
                                                UErrorCode &errorCode) സ്ഥിരം {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    സ്ഥിരം char16_t *prevSrc=src;
    char16_t c;
    ആകുമ്പോൾ((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    എങ്കിൽ(--src!=prevSrc) {
        എങ്കിൽ(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    തിരികെ src;
}

UnicodeString &
Normalizer2Impl::decompose(സ്ഥിരം UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        തിരികെ dest;
    }
    സ്ഥിരം char16_t *sArray=src.getBuffer();
    എങ്കിൽ(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        തിരികെ dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    തിരികെ dest;
}

ശൂന്യം
Normalizer2Impl::decompose(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    ReorderingBuffer buffer(*ഇത്, dest);
    എങ്കിൽ(buffer.init(destLengthEstimate, errorCode)) {
        decompose(src, limit, &buffer, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
സ്ഥിരം char16_t *
Normalizer2Impl::decompose(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                           ReorderingBuffer *buffer,
                           UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ src;
    }
    UChar32 minNoCP=minDecompNoCP;
    എങ്കിൽ(limit==nullptr) {
        src=copy_താഴെയുള്ള_Prefix_From_Nul_Terminated(src, minNoCP, buffer, errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ src;
        }
        limit=u_strchr(src, 0);
    }

    സ്ഥിരം char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    സ്ഥിരം char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    വേണ്ടി(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        വേണ്ടി(prevSrc=src; src!=limit;) {
            എങ്കിൽ( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } അല്ലെങ്കിൽ എങ്കിൽ(!U16_IS_LEAD(c)) {
                നിർത്തുക;
            } അല്ലെങ്കിൽ {
                char16_t c2;
                എങ്കിൽ((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    എങ്കിൽ(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } അല്ലെങ്കിൽ {
                        നിർത്തുക;
                    }
                } അല്ലെങ്കിൽ {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        എങ്കിൽ(src!=prevSrc) {
            എങ്കിൽ(buffer!=nullptr) {
                എങ്കിൽ(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    നിർത്തുക;
                }
            } അല്ലെങ്കിൽ {
                prevCC=0;
                prevBoundary=src;
            }
        }
        എങ്കിൽ(src==limit) {
            നിർത്തുക;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        എങ്കിൽ(buffer!=nullptr) {
            എങ്കിൽ(!decompose(c, norm16, *buffer, errorCode)) {
                നിർത്തുക;
            }
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                എങ്കിൽ(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    എങ്കിൽ(cc<=1) {
                        prevBoundary=src;
                    }
                    തുടരുക;
                }
            }
            തിരികെ prevBoundary;  // "no" or cc out of order
        }
    }
    തിരികെ src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
സ്ഥിരം char16_t *
Normalizer2Impl::decomposeShort(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    ആകുമ്പോൾ(src<limit) {
        എങ്കിൽ (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            തിരികെ src;
        }
        സ്ഥിരം char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        എങ്കിൽ (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            തിരികെ prevSrc;
        }
        എങ്കിൽ(!decompose(c, norm16, buffer, errorCode)) {
            തിരികെ nullptr;
        }
        എങ്കിൽ (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            തിരികെ src;
        }
    }
    തിരികെ src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) സ്ഥിരം {
    // get the decomposition and the lead and trail cc's
    എങ്കിൽ (norm16 >= limitNoNo) {
        എങ്കിൽ (isMaybeYesOrNonZeroCC(norm16)) {
            തിരികെ buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } അല്ലെങ്കിൽ എങ്കിൽ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    എങ്കിൽ (norm16 < minYesNo) {
        // c does not decompose
        തിരികെ buffer.append(c, 0, errorCode);
    } അല്ലെങ്കിൽ എങ്കിൽ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        തിരികെ buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    സ്ഥിരം uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    എങ്കിൽ(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } അല്ലെങ്കിൽ {
        leadCC=0;
    }
    തിരികെ buffer.append(reinterpret_cast<സ്ഥിരം char16_t*>(mapping) + 1, length, സത്യം, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
സ്ഥിരം uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               സ്ഥിരം uint8_t *src, സ്ഥിരം uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) സ്ഥിരം {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    സ്ഥിരം uint8_t *prevBoundary = src;
    // only for quick check
    uint8_t prevCC = 0;

    വേണ്ടി (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        സ്ഥിരം uint8_t *fastStart = src;
        സ്ഥിരം uint8_t *prevSrc;
        uint16_t norm16 = 0;

        വേണ്ടി (;;) {
            എങ്കിൽ (src == limit) {
                എങ്കിൽ (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                തിരികെ src;
            }
            എങ്കിൽ (*src < minNoLead) {
                ++src;
            } അല്ലെങ്കിൽ {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                എങ്കിൽ (!isMostDecompYesAndZeroCC(norm16)) {
                    നിർത്തുക;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        എങ്കിൽ (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            എങ്കിൽ (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                നിർത്തുക;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        എങ്കിൽ (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            എങ്കിൽ (prevCC <= cc || cc == 0) {
                prevCC = cc;
                എങ്കിൽ (cc <= 1) {
                    എങ്കിൽ (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        നിർത്തുക;
                    }
                    prevBoundary = src;
                }
                തുടരുക;
            }
        }
        എങ്കിൽ (sink == nullptr) {
            തിരികെ prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        എങ്കിൽ (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            എങ്കിൽ (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                നിർത്തുക;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*ഇത്, s16, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) {
            നിർത്തുക;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, അസത്യം /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        എങ്കിൽ (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, അസത്യം /* onlyContiguous */,
                                 buffer, errorCode);
        }
        എങ്കിൽ (U_FAILURE(errorCode)) {
            നിർത്തുക;
        }
        എങ്കിൽ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            നിർത്തുക;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        എങ്കിൽ (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            എങ്കിൽ (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                നിർത്തുക;
            }
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                നിർത്തുക;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    തിരികെ src;
}

സ്ഥിരം uint8_t *
Normalizer2Impl::decomposeShort(സ്ഥിരം uint8_t *src, സ്ഥിരം uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    ആകുമ്പോൾ (src < limit) {
        സ്ഥിരം uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        എങ്കിൽ (norm16 >= limitNoNo) {
            എങ്കിൽ (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                എങ്കിൽ (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    തിരികെ prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                എങ്കിൽ (!buffer.append(c, cc, errorCode)) {
                    തിരികെ nullptr;
                }
                എങ്കിൽ (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    തിരികെ src;
                }
                തുടരുക;
            } അല്ലെങ്കിൽ എങ്കിൽ (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                എങ്കിൽ (stopAt != STOP_AT_LIMIT) {
                    തിരികെ prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            തിരികെ prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        എങ്കിൽ (norm16 < minYesNo) {
            എങ്കിൽ (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            എങ്കിൽ (!buffer.append(c, 0, errorCode)) {
                തിരികെ nullptr;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            എങ്കിൽ (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            എങ്കിൽ (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                തിരികെ nullptr;
            }
        } അല്ലെങ്കിൽ {
            // The character decomposes, get everything from the variable-length extra data.
            സ്ഥിരം uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            എങ്കിൽ (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } അല്ലെങ്കിൽ {
                leadCC = 0;
            }
            എങ്കിൽ (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                തിരികെ prevSrc;
            }
            എങ്കിൽ (!buffer.append(reinterpret_cast<സ്ഥിരം char16_t*>(mapping) + 1, length, സത്യം, leadCC, trailCC, errorCode)) {
                തിരികെ nullptr;
            }
        }
        എങ്കിൽ ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            തിരികെ src;
        }
    }
    തിരികെ src;
}

സ്ഥിരം char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) സ്ഥിരം {
    uint16_t norm16;
    എങ്കിൽ(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        തിരികെ nullptr;
    }
    സ്ഥിരം char16_t *decomp = nullptr;
    എങ്കിൽ(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    എങ്കിൽ (norm16 < minYesNo) {
        തിരികെ decomp;
    } അല്ലെങ്കിൽ എങ്കിൽ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        തിരികെ buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    സ്ഥിരം uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    തിരികെ reinterpret_cast<സ്ഥിരം char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
സ്ഥിരം char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) സ്ഥിരം {
    uint16_t norm16;
    എങ്കിൽ(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        തിരികെ buffer;
    } അല്ലെങ്കിൽ എങ്കിൽ(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        തിരികെ buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    സ്ഥിരം uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    എങ്കിൽ(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        സ്ഥിരം uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        എങ്കിൽ(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            തിരികെ reinterpret_cast<സ്ഥിരം char16_t*>(rawMapping) - rm0;
        } അല്ലെങ്കിൽ {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<സ്ഥിരം char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            തിരികെ buffer;
        }
    } അല്ലെങ്കിൽ {
        length=mLength;
        തിരികെ reinterpret_cast<സ്ഥിരം char16_t*>(mapping) + 1;
    }
}

ശൂന്യം Normalizer2Impl::decomposeAndAppend(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_നടുവ്,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) സ്ഥിരം {
    buffer.copyReorderableSuffixTo(safe_നടുവ്);
    എങ്കിൽ(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        തിരികെ;
    }
    // Just merge the strings at the boundary.
    തർക്കം isFirst = സത്യം;
    uint8_t firstCC = 0, prevCC = 0, cc;
    സ്ഥിരം char16_t *p = src;
    ആകുമ്പോൾ (p != limit) {
        സ്ഥിരം char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        എങ്കിൽ ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            നിർത്തുക;
        }
        എങ്കിൽ (isFirst) {
            firstCC = cc;
            isFirst = അസത്യം;
        }
        prevCC = cc;
    }
    എങ്കിൽ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    എങ്കിൽ (buffer.append(src, static_cast<int32_t>(p - src), അസത്യം, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) സ്ഥിരം {
    തിരികെ c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) സ്ഥിരം {
    എങ്കിൽ (norm16 < minNoNoCompNoMaybeCC) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (norm16 >= limitNoNo) {
        തിരികെ norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    സ്ഥിരം uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    തിരികെ (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) സ്ഥിരം {
    എങ്കിൽ (c < minDecompNoCP) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        തിരികെ സത്യം;
    }
    തിരികെ norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) സ്ഥിരം {
    എങ്കിൽ(norm16 <= minYesNo || isHangulLVT(norm16)) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (norm16 >= limitNoNo) {
        എങ്കിൽ (isMaybeYesOrNonZeroCC(norm16)) {
            തിരികെ norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } അല്ലെങ്കിൽ എങ്കിൽ (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            തിരികെ (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    സ്ഥിരം uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    എങ്കിൽ(firstUnit>0x1ff) {
        തിരികെ അസത്യം;  // trailCC>1
    }
    എങ്കിൽ(firstUnit<=0xff) {
        തിരികെ സത്യം;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    തിരികെ (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(സ്ഥിരം uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    എങ്കിൽ(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        ആകുമ്പോൾ(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        എങ്കിൽ(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            എങ്കിൽ(firstUnit&COMP_1_TRIPLE) {
                തിരികെ (static_cast<int32_t>(list[1]) << 16) | list[2];
            } അല്ലെങ്കിൽ {
                തിരികെ list[1];
            }
        }
    } അല്ലെങ്കിൽ {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        വേണ്ടി(;;) {
            എങ്കിൽ(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } അല്ലെങ്കിൽ എങ്കിൽ(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                എങ്കിൽ(key2>(secondUnit=list[1])) {
                    എങ്കിൽ(firstUnit&COMP_1_LAST_TUPLE) {
                        നിർത്തുക;
                    } അല്ലെങ്കിൽ {
                        list+=3;
                    }
                } അല്ലെങ്കിൽ എങ്കിൽ(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    തിരികെ (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } അല്ലെങ്കിൽ {
                    നിർത്തുക;
                }
            } അല്ലെങ്കിൽ {
                നിർത്തുക;
            }
        }
    }
    തിരികെ -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
ശൂന്യം Normalizer2Impl::addComposites(സ്ഥിരം uint16_t *list, UnicodeSet &set) സ്ഥിരം {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    ചെയ്യുക {
        firstUnit=*list;
        എങ്കിൽ((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } അല്ലെങ്കിൽ {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        എങ്കിൽ((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } ആകുമ്പോൾ((firstUnit&COMP_1_LAST_TUPLE)==0);
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
ശൂന്യം Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) സ്ഥിരം {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    എങ്കിൽ(p==limit) {
        തിരികെ;
    }

    char16_t *starter, *pRemove, *q, *r;
    സ്ഥിരം uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, prevCC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=അസത്യം;
    prevCC=0;

    വേണ്ടി(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        എങ്കിൽ( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            എങ്കിൽ(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                എങ്കിൽ(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    എങ്കിൽ(prev<Hangul::JAMO_L_എണ്ണം) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_എണ്ണം+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_എണ്ണം);
                        char16_t t;
                        എങ്കിൽ (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_എണ്ണം) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        ആകുമ്പോൾ(r<limit) {
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
                എങ്കിൽ(p==limit) {
                    നിർത്തുക;
                }
                compositionsList=nullptr;
                തുടരുക;
            } അല്ലെങ്കിൽ എങ്കിൽ((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                എങ്കിൽ(starterIsSupplementary) {
                    എങ്കിൽ(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } അല്ലെങ്കിൽ {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=അസത്യം;
                        q=starter+1;
                        r=q+1;
                        ആകുമ്പോൾ(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } അല്ലെങ്കിൽ എങ്കിൽ(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=സത്യം;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    ആകുമ്പോൾ(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } അല്ലെങ്കിൽ {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                എങ്കിൽ(pRemove<p) {
                    q=pRemove;
                    r=p;
                    ആകുമ്പോൾ(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                എങ്കിൽ(p==limit) {
                    നിർത്തുക;
                }
                // Is the composite a starter that combines forward?
                എങ്കിൽ(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } അല്ലെങ്കിൽ {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                തുടരുക;
            }
        }

        // no combination this time
        prevCC=cc;
        എങ്കിൽ(p==limit) {
            നിർത്തുക;
        }

        // If c did not combine, then check if it is a starter.
        എങ്കിൽ(cc==0) {
            // Found a new starter.
            എങ്കിൽ((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                എങ്കിൽ(U_IS_BMP(c)) {
                    starterIsSupplementary=അസത്യം;
                    starter=p-1;
                } അല്ലെങ്കിൽ {
                    starterIsSupplementary=സത്യം;
                    starter=p-2;
                }
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    buffer.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) സ്ഥിരം {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    സ്ഥിരം uint16_t *list;
    എങ്കിൽ(isInert(norm16)) {
        തിരികെ U_SENTINEL;
    } അല്ലെങ്കിൽ എങ്കിൽ(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        എങ്കിൽ(isJamoL(norm16)) {
            എങ്കിൽ (b < Hangul::JAMO_V_BASE) {
                തിരികെ U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            എങ്കിൽ(b<Hangul::JAMO_V_എണ്ണം) {
                തിരികെ
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_എണ്ണം+b)*
                     Hangul::JAMO_T_എണ്ണം);
            } അല്ലെങ്കിൽ {
                തിരികെ U_SENTINEL;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(isHangulLV(norm16)) {
            എങ്കിൽ (b <= Hangul::JAMO_T_BASE) {
               തിരികെ U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            എങ്കിൽ(b<Hangul::JAMO_T_എണ്ണം) {  // not b==0!
                തിരികെ a+b;
            } അല്ലെങ്കിൽ {
                തിരികെ U_SENTINEL;
            }
        } അല്ലെങ്കിൽ {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            എങ്കിൽ(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        തിരികെ U_SENTINEL;
    } അല്ലെങ്കിൽ {
        list=getDataForMaybe(norm16);
        എങ്കിൽ(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    എങ്കിൽ(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        തിരികെ U_SENTINEL;
    }
#എങ്കിൽ U_SIGNED_വലത്_SHIFT_IS_ARITHMETIC
    തിരികെ combine(list, b)>>1;
#അല്ലെങ്കിൽ
    int32_t compositeAndFwd=combine(list, b);
    തിരികെ compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#endif
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         ReorderingBuffer &buffer,
                         UErrorCode &errorCode) സ്ഥിരം {
    സ്ഥിരം char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    എങ്കിൽ(limit==nullptr) {
        src=copy_താഴെയുള്ള_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ അസത്യം;
        }
        limit=u_strchr(src, 0);
        എങ്കിൽ (prevBoundary != src) {
            എങ്കിൽ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } അല്ലെങ്കിൽ {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    വേണ്ടി (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        സ്ഥിരം char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        വേണ്ടി (;;) {
            എങ്കിൽ (src == limit) {
                എങ്കിൽ (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                തിരികെ സത്യം;
            }
            എങ്കിൽ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } അല്ലെങ്കിൽ {
                prevSrc = src++;
                എങ്കിൽ(!U16_IS_LEAD(c)) {
                    നിർത്തുക;
                } അല്ലെങ്കിൽ {
                    char16_t c2;
                    എങ്കിൽ(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        എങ്കിൽ(!isCompYesAndZeroCC(norm16)) {
                            നിർത്തുക;
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
        എങ്കിൽ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            എങ്കിൽ (!doCompose) {
                തിരികെ അസത്യം;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            എങ്കിൽ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                എങ്കിൽ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    എങ്കിൽ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        നിർത്തുക;
                    }
                    എങ്കിൽ(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        നിർത്തുക;
                    }
                    prevBoundary = src;
                    തുടരുക;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                എങ്കിൽ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    എങ്കിൽ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        നിർത്തുക;
                    }
                    സ്ഥിരം char16_t *mapping = reinterpret_cast<സ്ഥിരം char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    എങ്കിൽ(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        നിർത്തുക;
                    }
                    prevBoundary = src;
                    തുടരുക;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                എങ്കിൽ (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    എങ്കിൽ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        നിർത്തുക;
                    }
                    prevBoundary = src;
                    തുടരുക;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } അല്ലെങ്കിൽ എങ്കിൽ (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            എങ്കിൽ(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                എങ്കിൽ(l<Hangul::JAMO_L_എണ്ണം) {
                    എങ്കിൽ (!doCompose) {
                        തിരികെ അസത്യം;
                    }
                    int32_t t;
                    എങ്കിൽ (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_എണ്ണം) {
                        // The next character is a Jamo T.
                        ++src;
                    } അല്ലെങ്കിൽ എങ്കിൽ (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } അല്ലെങ്കിൽ {
                        t = -1;
                    }
                    എങ്കിൽ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_എണ്ണം + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_എണ്ണം + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        എങ്കിൽ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            നിർത്തുക;
                        }
                        എങ്കിൽ (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            നിർത്തുക;
                        }
                        prevBoundary = src;
                        തുടരുക;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                എങ്കിൽ (!doCompose) {
                    തിരികെ അസത്യം;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                എങ്കിൽ (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    നിർത്തുക;
                }
                എങ്കിൽ (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    നിർത്തുക;
                }
                prevBoundary = src;
                തുടരുക;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } അല്ലെങ്കിൽ എങ്കിൽ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            എങ്കിൽ (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                എങ്കിൽ (!doCompose) {
                    തിരികെ അസത്യം;
                }
            } അല്ലെങ്കിൽ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                സ്ഥിരം char16_t *nextSrc;
                uint16_t n16;
                വേണ്ടി (;;) {
                    എങ്കിൽ (src == limit) {
                        എങ്കിൽ (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        തിരികെ സത്യം;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    എങ്കിൽ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        എങ്കിൽ (prevCC > cc) {
                            എങ്കിൽ (!doCompose) {
                                തിരികെ അസത്യം;
                            }
                            നിർത്തുക;
                        }
                    } അല്ലെങ്കിൽ {
                        നിർത്തുക;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                എങ്കിൽ (norm16HasCompBoundaryBefore(n16)) {
                    എങ്കിൽ (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    തുടരുക;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        എങ്കിൽ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            സ്ഥിരം char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            എങ്കിൽ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        എങ്കിൽ (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            നിർത്തുക;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, അസത്യം /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, സത്യം /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) {
            നിർത്തുക;
        }
        എങ്കിൽ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ സത്യം;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        എങ്കിൽ(!doCompose) {
            എങ്കിൽ(!buffer.equals(prevSrc, src)) {
                തിരികെ അസത്യം;
            }
            buffer.remove();
        }
        prevBoundary=src;
    }
    തിരികെ സത്യം;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
സ്ഥിരം char16_t *
Normalizer2Impl::composeQuickCheck(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalizationCheckResult *pQCResult) സ്ഥിരം {
    സ്ഥിരം char16_t *prevBoundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    എങ്കിൽ(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_താഴെയുള്ള_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        എങ്കിൽ (prevBoundary != src) {
            എങ്കിൽ (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } അല്ലെങ്കിൽ {
                prevBoundary = --src;
            }
        }
    }

    വേണ്ടി(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        സ്ഥിരം char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        വേണ്ടി (;;) {
            എങ്കിൽ(src==limit) {
                തിരികെ src;
            }
            എങ്കിൽ( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } അല്ലെങ്കിൽ {
                prevSrc = src++;
                എങ്കിൽ(!U16_IS_LEAD(c)) {
                    നിർത്തുക;
                } അല്ലെങ്കിൽ {
                    char16_t c2;
                    എങ്കിൽ(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        എങ്കിൽ(!isCompYesAndZeroCC(norm16)) {
                            നിർത്തുക;
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
        എങ്കിൽ (prevBoundary != prevSrc) {
            എങ്കിൽ (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } അല്ലെങ്കിൽ {
                സ്ഥിരം char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                എങ്കിൽ (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } അല്ലെങ്കിൽ {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        എങ്കിൽ (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            എങ്കിൽ (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } അല്ലെങ്കിൽ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                സ്ഥിരം char16_t *nextSrc;
                വേണ്ടി (;;) {
                    എങ്കിൽ (norm16 < MIN_YES_YES_WITH_CC) {
                        എങ്കിൽ (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } അല്ലെങ്കിൽ {
                            തിരികെ prevBoundary;
                        }
                    }
                    എങ്കിൽ (src == limit) {
                        തിരികെ src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    എങ്കിൽ (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        എങ്കിൽ (!(prevCC <= cc || cc == 0)) {
                            നിർത്തുക;
                        }
                    } അല്ലെങ്കിൽ {
                        നിർത്തുക;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                എങ്കിൽ (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    തുടരുക;
                }
            }
        }
        എങ്കിൽ(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        തിരികെ prevBoundary;
    }
}

ശൂന്യം Normalizer2Impl::composeAndAppend(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_നടുവ്,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(!buffer.isEmpty()) {
        സ്ഥിരം char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        എങ്കിൽ(src!=firstStarterInSrc) {
            സ്ഥിരം char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString നടുവ്(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_നടുവ്=നടുവ്;
            നടുവ്.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            സ്ഥിരം char16_t *നടുവ്_Start=നടുവ്.getBuffer();
            compose(നടുവ്_Start, നടുവ്_Start+നടുവ്.length(), onlyContiguous,
                    സത്യം, buffer, errorCode);
            എങ്കിൽ(U_FAILURE(errorCode)) {
                തിരികെ;
            }
            src=firstStarterInSrc;
        }
    }
    എങ്കിൽ(doCompose) {
        compose(src, limit, onlyContiguous, സത്യം, buffer, errorCode);
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             സ്ഥിരം uint8_t *src, സ്ഥിരം uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) സ്ഥിരം {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    സ്ഥിരം uint8_t *prevBoundary = src;

    വേണ്ടി (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        സ്ഥിരം uint8_t *prevSrc;
        uint16_t norm16 = 0;
        വേണ്ടി (;;) {
            എങ്കിൽ (src == limit) {
                എങ്കിൽ (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                തിരികെ സത്യം;
            }
            എങ്കിൽ (*src < minNoMaybeLead) {
                ++src;
            } അല്ലെങ്കിൽ {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                എങ്കിൽ (!isCompYesAndZeroCC(norm16)) {
                    നിർത്തുക;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        എങ്കിൽ (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            എങ്കിൽ (sink == nullptr) {
                തിരികെ അസത്യം;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            എങ്കിൽ (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                എങ്കിൽ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    എങ്കിൽ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        നിർത്തുക;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    തുടരുക;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                എങ്കിൽ (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    എങ്കിൽ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        നിർത്തുക;
                    }
                    സ്ഥിരം uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    എങ്കിൽ (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<സ്ഥിരം char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        നിർത്തുക;
                    }
                    prevBoundary = src;
                    തുടരുക;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                എങ്കിൽ (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    എങ്കിൽ (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        നിർത്തുക;
                    }
                    എങ്കിൽ (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    തുടരുക;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } അല്ലെങ്കിൽ എങ്കിൽ (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            എങ്കിൽ (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                എങ്കിൽ (static_cast<uint32_t>(l) < Hangul::JAMO_L_എണ്ണം) {
                    എങ്കിൽ (sink == nullptr) {
                        തിരികെ അസത്യം;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    എങ്കിൽ (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } അല്ലെങ്കിൽ എങ്കിൽ (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    എങ്കിൽ (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_എണ്ണം + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_എണ്ണം + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        എങ്കിൽ (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            നിർത്തുക;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        തുടരുക;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                എങ്കിൽ (sink == nullptr) {
                    തിരികെ അസത്യം;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                എങ്കിൽ (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    നിർത്തുക;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                തുടരുക;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } അല്ലെങ്കിൽ എങ്കിൽ (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            എങ്കിൽ (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                എങ്കിൽ (sink == nullptr) {
                    തിരികെ അസത്യം;
                }
            } അല്ലെങ്കിൽ {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                സ്ഥിരം uint8_t *nextSrc;
                uint16_t n16;
                വേണ്ടി (;;) {
                    എങ്കിൽ (src == limit) {
                        എങ്കിൽ (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        തിരികെ സത്യം;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    എങ്കിൽ (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        എങ്കിൽ (prevCC > cc) {
                            എങ്കിൽ (sink == nullptr) {
                                തിരികെ അസത്യം;
                            }
                            നിർത്തുക;
                        }
                    } അല്ലെങ്കിൽ {
                        നിർത്തുക;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                എങ്കിൽ (norm16HasCompBoundaryBefore(n16)) {
                    എങ്കിൽ (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    തുടരുക;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        എങ്കിൽ (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            സ്ഥിരം uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            എങ്കിൽ (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*ഇത്, s16, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) {
            നിർത്തുക;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) {
            നിർത്തുക;
        }
        എങ്കിൽ ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ സത്യം;
        }
        recompose(buffer, 0, onlyContiguous);
        എങ്കിൽ (!buffer.equals(prevSrc, src)) {
            എങ്കിൽ (sink == nullptr) {
                തിരികെ അസത്യം;
            }
            എങ്കിൽ (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                നിർത്തുക;
            }
            എങ്കിൽ (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                നിർത്തുക;
            }
            prevBoundary = src;
        }
    }
    തിരികെ സത്യം;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit) സ്ഥിരം {
    എങ്കിൽ (src == limit || *src < minCompNoMaybeCP) {
        തിരികെ സത്യം;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    തിരികെ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(സ്ഥിരം uint8_t *src, സ്ഥിരം uint8_t *limit) സ്ഥിരം {
    എങ്കിൽ (src == limit) {
        തിരികെ സത്യം;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    തിരികെ norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(സ്ഥിരം char16_t *start, സ്ഥിരം char16_t *p,
                                            UBool onlyContiguous) സ്ഥിരം {
    എങ്കിൽ (start == p) {
        തിരികെ സത്യം;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    തിരികെ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(സ്ഥിരം uint8_t *start, സ്ഥിരം uint8_t *p,
                                            UBool onlyContiguous) സ്ഥിരം {
    എങ്കിൽ (start == p) {
        തിരികെ സത്യം;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    തിരികെ norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

സ്ഥിരം char16_t *Normalizer2Impl::findPreviousCompBoundary(സ്ഥിരം char16_t *start, സ്ഥിരം char16_t *p,
                                                       UBool onlyContiguous) സ്ഥിരം {
    ആകുമ്പോൾ (p != start) {
        സ്ഥിരം char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        എങ്കിൽ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            തിരികെ codePointLimit;
        }
        എങ്കിൽ (hasCompBoundaryBefore(c, norm16)) {
            തിരികെ p;
        }
    }
    തിരികെ p;
}

സ്ഥിരം char16_t *Normalizer2Impl::findNextCompBoundary(സ്ഥിരം char16_t *p, സ്ഥിരം char16_t *limit,
                                                   UBool onlyContiguous) സ്ഥിരം {
    ആകുമ്പോൾ (p != limit) {
        സ്ഥിരം char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        എങ്കിൽ (hasCompBoundaryBefore(c, norm16)) {
            തിരികെ codePointStart;
        }
        എങ്കിൽ (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            തിരികെ p;
        }
    }
    തിരികെ p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(സ്ഥിരം char16_t *start, സ്ഥിരം char16_t *p) സ്ഥിരം {
    എങ്കിൽ (start == p) {
        തിരികെ 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    തിരികെ static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(സ്ഥിരം uint8_t *start, സ്ഥിരം uint8_t *p) സ്ഥിരം {
    എങ്കിൽ (start == p) {
        തിരികെ 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    തിരികെ static_cast<uint8_t>(getFCD16(c));
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
#എങ്കിൽ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) സ്ഥിരം {
    uint16_t norm16=getNorm16(c);
    എങ്കിൽ (norm16 >= limitNoNo) {
        എങ്കിൽ(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            തിരികെ norm16|(norm16<<8);
        } അല്ലെങ്കിൽ എങ്കിൽ(norm16>=minMaybeYes) {
            തിരികെ 0;
        } അല്ലെങ്കിൽ എങ്കിൽ(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            എങ്കിൽ (deltaTrailCC <= DELTA_TCCC_1) {
                തിരികെ deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    എങ്കിൽ(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        തിരികെ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    സ്ഥിരം uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    എങ്കിൽ(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    തിരികെ norm16;
}
#എങ്കിൽ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) സ്ഥിരം {
    U_ASSERT(norm16 >= minMaybeNo);
    എങ്കിൽ (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        തിരികെ norm16 | (norm16<<8);
    } അല്ലെങ്കിൽ എങ്കിൽ (norm16 >= minMaybeYes) {
        തിരികെ 0;
    }
    // c decomposes, get everything from the variable-length extra data
    സ്ഥിരം uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    തിരികെ firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
സ്ഥിരം char16_t *
Normalizer2Impl::makeFCD(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                         ReorderingBuffer *buffer,
                         UErrorCode &errorCode) സ്ഥിരം {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    സ്ഥിരം char16_t *prevBoundary=src;
    int32_t prevFCD16=0;
    എങ്കിൽ(limit==nullptr) {
        src=copy_താഴെയുള്ള_Prefix_From_Nul_Terminated(src, minLcccCP, buffer, errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ src;
        }
        എങ്കിൽ(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            എങ്കിൽ(prevFCD16>1) {
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

    സ്ഥിരം char16_t *prevSrc;
    UChar32 c=0;
    uint16_t fcd16=0;

    വേണ്ടി(;;) {
        // count code units with lccc==0
        വേണ്ടി(prevSrc=src; src!=limit;) {
            എങ്കിൽ((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } അല്ലെങ്കിൽ എങ്കിൽ(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } അല്ലെങ്കിൽ {
                എങ്കിൽ(U16_IS_LEAD(c)) {
                    char16_t c2;
                    എങ്കിൽ((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                എങ്കിൽ((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } അല്ലെങ്കിൽ {
                    നിർത്തുക;
                }
            }
        }
        // copy these code units all at once
        എങ്കിൽ(src!=prevSrc) {
            എങ്കിൽ(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                നിർത്തുക;
            }
            എങ്കിൽ(src==limit) {
                നിർത്തുക;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            എങ്കിൽ(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                എങ്കിൽ(prev<minDecompNoCP) {
                    prevFCD16=0;
                } അല്ലെങ്കിൽ {
                    prevFCD16=getFCD16FromNormData(prev);
                    എങ്കിൽ(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } അല്ലെങ്കിൽ {
                സ്ഥിരം char16_t *p=src-1;
                എങ്കിൽ(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                എങ്കിൽ(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } അല്ലെങ്കിൽ എങ്കിൽ(src==limit) {
            നിർത്തുക;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        എങ്കിൽ((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            എങ്കിൽ((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            എങ്കിൽ(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                നിർത്തുക;
            }
            prevFCD16=fcd16;
            തുടരുക;
        } അല്ലെങ്കിൽ എങ്കിൽ(buffer==nullptr) {
            തിരികെ prevBoundary;  // quick check "no"
        } അല്ലെങ്കിൽ {
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
            decomposeShort(prevBoundary, src, അസത്യം, അസത്യം, *buffer, errorCode);
            എങ്കിൽ (U_FAILURE(errorCode)) {
                നിർത്തുക;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    തിരികെ src;
}

ശൂന്യം Normalizer2Impl::makeFCDAndAppend(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_നടുവ്,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(!buffer.isEmpty()) {
        സ്ഥിരം char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        എങ്കിൽ(src!=firstBoundaryInSrc) {
            സ്ഥിരം char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString നടുവ്(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_നടുവ്=നടുവ്;
            നടുവ്.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            സ്ഥിരം char16_t *നടുവ്_Start=നടുവ്.getBuffer();
            makeFCD(നടുവ്_Start, നടുവ്_Start+നടുവ്.length(), &buffer, errorCode);
            എങ്കിൽ(U_FAILURE(errorCode)) {
                തിരികെ;
            }
            src=firstBoundaryInSrc;
        }
    }
    എങ്കിൽ(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

സ്ഥിരം char16_t *Normalizer2Impl::findPreviousFCDBoundary(സ്ഥിരം char16_t *start, സ്ഥിരം char16_t *p) സ്ഥിരം {
    ആകുമ്പോൾ(start<p) {
        സ്ഥിരം char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        എങ്കിൽ (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            തിരികെ codePointLimit;
        }
        എങ്കിൽ (norm16HasDecompBoundaryBefore(norm16)) {
            തിരികെ p;
        }
    }
    തിരികെ p;
}

സ്ഥിരം char16_t *Normalizer2Impl::findNextFCDBoundary(സ്ഥിരം char16_t *p, സ്ഥിരം char16_t *limit) സ്ഥിരം {
    ആകുമ്പോൾ(p<limit) {
        സ്ഥിരം char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        എങ്കിൽ (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            തിരികെ codePointStart;
        }
        എങ്കിൽ (norm16HasDecompBoundaryAfter(norm16)) {
            തിരികെ p;
        }
    }
    തിരികെ p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

ശൂന്യം CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_മൂല്യം = umutablecptrie_get(mutableTrie, decompLead);
    എങ്കിൽ((canon_മൂല്യം&(CANON_HAS_SET|CANON_മൂല്യം_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_മൂല്യം|origin, &errorCode);
    } അല്ലെങ്കിൽ {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        എങ്കിൽ((canon_മൂല്യം&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(പുതിയ UnicodeSet, errorCode);
            set=lpSet.getAlias();
            എങ്കിൽ(U_FAILURE(errorCode)) {
                തിരികെ;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_മൂല്യം & CANON_മൂല്യം_MASK);
            canon_മൂല്യം = (canon_മൂല്യം & ~CANON_മൂല്യം_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_മൂല്യം, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            എങ്കിൽ (U_FAILURE(errorCode)) {
                തിരികെ;
            }
            എങ്കിൽ(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } അല്ലെങ്കിൽ {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_മൂല്യം & CANON_മൂല്യം_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
വർഗ്ഗം InitCanonIterData {
പൊതു:
    സ്ഥിതം ശൂന്യം doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
സ്ഥിതം ശൂന്യം U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

ശൂന്യം InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = പുതിയ CanonIterData(errorCode);
    എങ്കിൽ (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        ആകുമ്പോൾ ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            എങ്കിൽ (value != Normalizer2Impl::INERT) {
                impl->makeCanonIterDataFromNorm16(start, end, value, *impl->fCanonIterData, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->fCanonIterData->mutableTrie, "CanonIterData");
#endif
        impl->fCanonIterData->trie = umutablecptrie_buildImmutable(
            impl->fCanonIterData->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_മൂല്യം_BITS_32, &errorCode);
        umutablecptrie_close(impl->fCanonIterData->mutableTrie);
        impl->fCanonIterData->mutableTrie = nullptr;
    }
    എങ്കിൽ (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

ശൂന്യം Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, സ്ഥിരം uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        തിരികെ;
    }
    വേണ്ടി(UChar32 c=start; c<=end; ++c) {
        uint32_t old_മൂല്യം = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t new_മൂല്യം=old_മൂല്യം;
        എങ്കിൽ(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_മൂല്യം|=CANON_NOT_SEGMENT_STARTER;
            എങ്കിൽ(norm16<MIN_NORMAL_MAYBE_YES) {
                new_മൂല്യം|=CANON_HAS_COMPOSITIONS;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(norm16<minYesNo) {
            new_മൂല്യം|=CANON_HAS_COMPOSITIONS;
        } അല്ലെങ്കിൽ {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            എങ്കിൽ (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            എങ്കിൽ (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                സ്ഥിരം uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                എങ്കിൽ((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    എങ്കിൽ(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_മൂല്യം|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                എങ്കിൽ(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    എങ്കിൽ(norm16_2>=minNoNo) {
                        ആകുമ്പോൾ(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2_മൂല്യം = umutablecptrie_get(newData.mutableTrie, c2);
                            എങ്കിൽ((c2_മൂല്യം&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2_മൂല്യം|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } അല്ലെങ്കിൽ {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        എങ്കിൽ(new_മൂല്യം!=old_മൂല്യം) {
            umutablecptrie_set(newData.mutableTrie, c, new_മൂല്യം, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) സ്ഥിരം {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(ഇത്);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    തിരികെ U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_മൂല്യം(UChar32 c) സ്ഥിരം {
    തിരികെ static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

സ്ഥിരം UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) സ്ഥിരം {
    തിരികെ *static_cast<സ്ഥിരം UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) സ്ഥിരം {
    തിരികെ get_Canon_മൂല്യം(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) സ്ഥിരം {
    int32_t canon_മൂല്യം=get_Canon_മൂല്യം(c)&~CANON_NOT_SEGMENT_STARTER;
    എങ്കിൽ(canon_മൂല്യം==0) {
        തിരികെ അസത്യം;
    }
    set.clear();
    int32_t value=canon_മൂല്യം&CANON_മൂല്യം_MASK;
    എങ്കിൽ((canon_മൂല്യം&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } അല്ലെങ്കിൽ എങ്കിൽ(value!=0) {
        set.add(value);
    }
    എങ്കിൽ((canon_മൂല്യം&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        എങ്കിൽ(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_എണ്ണം);
            set.add(syllable, syllable+Hangul::JAMO_VT_എണ്ണം-1);
        } അല്ലെങ്കിൽ {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    തിരികെ സത്യം;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(സ്ഥിരം UDataSwapper *ds,
            സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
            UErrorCode *pErrorCode) {
    സ്ഥിരം UDataInfo *pInfo;
    int32_t headerSize;

    സ്ഥിരം uint8_t *inBytes;
    uint8_t *outBytes;

    സ്ഥിരം int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_TOTAL_SIZE+1];

    int32_t i, offset, nextOffset, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    /* check data format and format version */
    pInfo=(സ്ഥിരം UDataInfo *)((സ്ഥിരം അക്ഷരം *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    എങ്കിൽ(!(
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
        തിരികെ 0;
    }

    inBytes=(സ്ഥിരം uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(സ്ഥിരം int32_t *)inBytes;
    int32_t minIndexesLength;
    എങ്കിൽ(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } അല്ലെങ്കിൽ എങ്കിൽ(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } അല്ലെങ്കിൽ എങ്കിൽ(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } അല്ലെങ്കിൽ {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    എങ്കിൽ(length>=0) {
        length-=headerSize;
        എങ്കിൽ(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
    }

    /* read the first few indexes */
    വേണ്ടി(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    എങ്കിൽ(length>=0) {
        എങ്കിൽ(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }

        /* copy the data for inaccessible bytes */
        എങ്കിൽ(inBytes!=outBytes) {
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

    തിരികെ headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
