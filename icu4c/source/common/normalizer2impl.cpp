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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/bytestream.h"
#समावेश "unicode/edits.h"
#समावेश "unicode/normalizer2.h"
#समावेश "unicode/stringoptions.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/umutablecptrie.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf16.h"
#समावेश "unicode/utf8.h"
#समावेश "bytesinkutil.h"
#समावेश "cmemory.h"
#समावेश "mutex.h"
#समावेश "normalizer2impl.h"
#समावेश "putilimp.h"
#समावेश "uassert.h"
#समावेश "ucptrie_impl.h"
#समावेश "uset_imp.h"
#समावेश "uvector.h"

U_NAMESPACE_BEGIN

namespace {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
अंतरभूत uint8_t leadByteForCP(UChar32 c) {
    अगर (c <= 0x7f) {
        वापस static_cast<uint8_t>(c);
    } अन्यथा अगर (c <= 0x7ff) {
        वापस static_cast<uint8_t>(0xc0 + (c >> 6));
    } अन्यथा {
        // Should not occur because ccc(U+0300)!=0.
        वापस 0xe0;
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
UChar32 codePointFromValidUTF8(स्थिर uint8_t *cpStart, स्थिर uint8_t *cpLimit) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cpLimit);
    uint8_t c = *cpStart;
    चयन(cpLimit-cpStart) {
    हाल 1:
        वापस c;
    हाल 2:
        वापस ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    हाल 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        वापस static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    हाल 4:
        वापस ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    शेष:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(स्थिर uint8_t *start, स्थिर uint8_t *p) {
    अगर ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        अगर (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            वापस ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    वापस U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(स्थिर uint8_t *src, स्थिर uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    अगर ((limit - src) >= 3 && *src == 0xe1) {
        अगर (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            अगर (0xa8 <= t && t <= 0xbf) {
                वापस t - 0xa7;
            }
        } अन्यथा अगर (src[1] == 0x87) {
            uint8_t t = src[2];
            अगर (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                वापस t - (0xa7 - 0x40);
            }
        }
    }
    वापस -1;
}

व्योम
appendCodePointDelta(स्थिर uint8_t *cpStart, स्थिर uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    अक्षर बफर[U8_MAX_लंबाई];
    int32_t length;
    int32_t cp_लंबाई = static_cast<int32_t>(cpLimit - cpStart);
    अगर (cp_लंबाई == 1) {
        // The builder makes ASCII map to ASCII.
        बफर[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } अन्यथा {
        int32_t trail = *(cpLimit-1) + delta;
        अगर (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            करो { बफर[length++] = *cpStart++; } जबतक (cpStart < cpLimit);
            बफर[length++] = static_cast<uint8_t>(trail);
        } अन्यथा {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(बफर, length, c);
        }
    }
    अगर (edits != nullptr) {
        edits->addReplace(cp_लंबाई, length);
    }
    sink.Append(बफर, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_बफर::Reordering_बफर(स्थिर Normalizer2Impl &ni, UnicodeString &dest,
                                   UErrorCode &errorCode) :
        impl(ni), str(dest),
        start(str.get_बफर(8)), reorderStart(start), limit(start),
        remainingCapacity(str.getCapacity()), lastCC(0) {
    अगर (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool Reordering_बफर::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.get_बफर(destCapacity);
    अगर(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस false;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    अगर(start==limit) {
        lastCC=0;
    } अन्यथा {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        अगर(lastCC>1) {
            जबतक(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    वापस true;
}

UBool Reordering_बफर::equals(स्थिर char16_t *otherStart, स्थिर char16_t *otherLimit) स्थिर {
    int32_t length = static_cast<int32_t>(limit - start);
    वापस
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool Reordering_बफर::equals(स्थिर uint8_t *otherStart, स्थिर uint8_t *otherLimit) स्थिर {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t other_लंबाई = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    अगर (other_लंबाई < length || (other_लंबाई / 3) > length) {
        वापस false;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    क्रम (int32_t i = 0, j = 0;;) {
        अगर (i >= length) {
            वापस j >= other_लंबाई;
        } अन्यथा अगर (j >= other_लंबाई) {
            वापस false;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_अग्र_UNSAFE(start, i, c);
        U8_अग्र_UNSAFE(otherStart, j, other);
        अगर (c != other) {
            वापस false;
        }
    }
}

UBool Reordering_बफर::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    अगर(remainingCapacity<2 && !resize(2, errorCode)) {
        वापस false;
    }
    अगर(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        अगर(cc<=1) {
            reorderStart=limit;
        }
    } अन्यथा {
        insert(c, cc);
    }
    remainingCapacity-=2;
    वापस true;
}

UBool Reordering_बफर::append(स्थिर char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    अगर(length==0) {
        वापस true;
    }
    अगर(remainingCapacity<length && !resize(length, errorCode)) {
        वापस false;
    }
    remainingCapacity-=length;
    अगर(lastCC<=leadCC || leadCC==0) {
        अगर(trailCC<=1) {
            reorderStart=limit+length;
        } अन्यथा अगर(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        स्थिर char16_t *sLimit=s+length;
        करो { *limit++=*s++; } जबतक(s!=sLimit);
        lastCC=trailCC;
    } अन्यथा {
        int32_t i=0;
        UChar32 c;
        U16_अग्र(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        जबतक(i<length) {
            U16_अग्र(s, i, length, c);
            अगर(i<length) {
                अगर (isNFD) {
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
    वापस true;
}

UBool Reordering_बफर::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cp_लंबाई=U16_लंबाई(c);
    अगर(remainingCapacity<cp_लंबाई && !resize(cp_लंबाई, errorCode)) {
        वापस false;
    }
    remainingCapacity-=cp_लंबाई;
    अगर(cp_लंबाई==1) {
        *limit++ = static_cast<char16_t>(c);
    } अन्यथा {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    वापस true;
}

UBool Reordering_बफर::appendZeroCC(स्थिर char16_t *s, स्थिर char16_t *sLimit, UErrorCode &errorCode) {
    अगर(s==sLimit) {
        वापस true;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    अगर(remainingCapacity<length && !resize(length, errorCode)) {
        वापस false;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    वापस true;
}

व्योम Reordering_बफर::हटाओ() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

व्योम Reordering_बफर::removeSuffix(int32_t suffix_लंबाई) {
    अगर(suffix_लंबाई<(limit-start)) {
        limit-=suffix_लंबाई;
        remainingCapacity+=suffix_लंबाई;
    } अन्यथा {
        limit=start;
        remainingCapacity=str.getCapacity();
    }
    lastCC=0;
    reorderStart=limit;
}

UBool Reordering_बफर::resize(int32_t append_लंबाई, UErrorCode &errorCode) {
    int32_t reorder_Start_सूचकांक = static_cast<int32_t>(reorderStart - start);
    int32_t length = static_cast<int32_t>(limit - start);
    str.release_बफर(length);
    int32_t newCapacity=length+append_लंबाई;
    int32_t doubleCapacity=2*str.getCapacity();
    अगर(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    अगर(newCapacity<256) {
        newCapacity=256;
    }
    start=str.get_बफर(newCapacity);
    अगर(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस false;
    }
    reorderStart=start+reorder_Start_सूचकांक;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    वापस true;
}

व्योम Reordering_बफर::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    अगर(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t Reordering_बफर::previousCC() {
    codePointLimit=codePointStart;
    अगर(reorderStart>=codePointStart) {
        वापस 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    अगर(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    वापस impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
व्योम Reordering_बफर::insert(UChar32 c, uint8_t cc) {
    क्रम(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_लंबाई(c);
    करो {
        *--r=*--q;
    } जबतक(codePointLimit!=q);
    writeCodePoint(q, c);
    अगर(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

काष्ठा Canon_Iter_तथ्य : खुला UMemory {
    Canon_Iter_तथ्य(UErrorCode &errorCode);
    ~Canon_Iter_तथ्य();
    व्योम addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    मिटाओ f_Canon_Iter_तथ्य;
}

व्योम
Normalizer2Impl::init(स्थिर int32_t *inIndexes, स्थिर UCPTrie *inTrie,
                      स्थिर uint16_t *in_Extra_तथ्य, स्थिर uint8_t *inSmallFCD) {
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
    extra_तथ्य=in_Extra_तथ्य;
    smallFCD=inSmallFCD;
}

U_CDECL_BEGIN

अटल uint32_t U_CALLCONV
segmentStarterMapper(स्थिर व्योम * /*context*/, uint32_t value) {
    वापस value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

व्योम
Normalizer2Impl::addLcccChars(UnicodeSet &set) स्थिर {
    UChar32 start = 0, end;
    uint32_t norm16;
    जबतक ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        अगर (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } अन्यथा अगर (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            अगर (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

व्योम
Normalizer2Impl::addPropertyStarts(स्थिर USetAdder *sa, UErrorCode & /*errorCode*/) स्थिर {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    जबतक ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        अगर (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t पूर्व_FCD16 = getFCD16(start);
            जबतक (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                अगर (fcd16 != पूर्व_FCD16) {
                    sa->add(sa->set, start);
                    पूर्व_FCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    क्रम(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_गिनती) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

व्योम
Normalizer2Impl::addCanonIterPropertyStarts(स्थिर USetAdder *sa, UErrorCode &errorCode) स्थिर {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    अगर (!ensure_Canon_Iter_तथ्य(errorCode)) { वापस; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    जबतक ((end = ucptrie_getRange(f_Canon_Iter_तथ्य->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

स्थिर char16_t *
Normalizer2Impl::copy_निचला_Prefix_From_Nul_Terminated(स्थिर char16_t *src,
                                                UChar32 min_Need_तथ्य_CP,
                                                Reordering_बफर *बफर,
                                                UErrorCode &errorCode) स्थिर {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    स्थिर char16_t *पूर्व_Src=src;
    char16_t c;
    जबतक((c=*src++)<min_Need_तथ्य_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    अगर(--src!=पूर्व_Src) {
        अगर(बफर!=nullptr) {
            बफर->appendZeroCC(पूर्व_Src, src, errorCode);
        }
    }
    वापस src;
}

UnicodeString &
Normalizer2Impl::decompose(स्थिर UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        dest.setToBogus();
        वापस dest;
    }
    स्थिर char16_t *sArray=src.get_बफर();
    अगर(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        वापस dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    वापस dest;
}

व्योम
Normalizer2Impl::decompose(स्थिर char16_t *src, स्थिर char16_t *limit,
                           UnicodeString &dest,
                           int32_t dest_लंबाई_Estimate,
                           UErrorCode &errorCode) स्थिर {
    अगर(dest_लंबाई_Estimate<0 && limit!=nullptr) {
        dest_लंबाई_Estimate = static_cast<int32_t>(limit - src);
    }
    dest.हटाओ();
    Reordering_बफर बफर(*यह, dest);
    अगर(बफर.init(dest_लंबाई_Estimate, errorCode)) {
        decompose(src, limit, &बफर, errorCode);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
स्थिर char16_t *
Normalizer2Impl::decompose(स्थिर char16_t *src, स्थिर char16_t *limit,
                           Reordering_बफर *बफर,
                           UErrorCode &errorCode) स्थिर {
    अगर(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस src;
    }
    UChar32 minNoCP=minDecompNoCP;
    अगर(limit==nullptr) {
        src=copy_निचला_Prefix_From_Nul_Terminated(src, minNoCP, बफर, errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस src;
        }
        limit=u_strchr(src, 0);
    }

    स्थिर char16_t *पूर्व_Src;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    स्थिर char16_t *पूर्व_Boundary=src;
    uint8_t पूर्व_CC=0;

    क्रम(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        क्रम(पूर्व_Src=src; src!=limit;) {
            अगर( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा अगर(!U16_IS_LEAD(c)) {
                अवरोध;
            } अन्यथा {
                char16_t c2;
                अगर((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    अगर(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } अन्यथा {
                        अवरोध;
                    }
                } अन्यथा {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        अगर(src!=पूर्व_Src) {
            अगर(बफर!=nullptr) {
                अगर(!बफर->appendZeroCC(पूर्व_Src, src, errorCode)) {
                    अवरोध;
                }
            } अन्यथा {
                पूर्व_CC=0;
                पूर्व_Boundary=src;
            }
        }
        अगर(src==limit) {
            अवरोध;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_लंबाई(c);
        अगर(बफर!=nullptr) {
            अगर(!decompose(c, norm16, *बफर, errorCode)) {
                अवरोध;
            }
        } अन्यथा {
            अगर(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                अगर(पूर्व_CC<=cc || cc==0) {
                    पूर्व_CC=cc;
                    अगर(cc<=1) {
                        पूर्व_Boundary=src;
                    }
                    जारी;
                }
            }
            वापस पूर्व_Boundary;  // "no" or cc out of order
        }
    }
    वापस src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
स्थिर char16_t *
Normalizer2Impl::decomposeShort(स्थिर char16_t *src, स्थिर char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    जबतक(src<limit) {
        अगर (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            वापस src;
        }
        स्थिर char16_t *पूर्व_Src = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, src, limit, c, norm16);
        अगर (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            वापस पूर्व_Src;
        }
        अगर(!decompose(c, norm16, बफर, errorCode)) {
            वापस nullptr;
        }
        अगर (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            वापस src;
        }
    }
    वापस src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_बफर &बफर,
                                 UErrorCode &errorCode) स्थिर {
    // get the decomposition and the lead and trail cc's
    अगर (norm16 >= limitNoNo) {
        अगर (isMaybeYesOrNonZeroCC(norm16)) {
            वापस बफर.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } अन्यथा अगर (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    अगर (norm16 < minYesNo) {
        // c does not decompose
        वापस बफर.append(c, 0, errorCode);
    } अन्यथा अगर(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        वापस बफर.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिर uint16_t *mapping=get_तथ्य(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_लंबाई_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    अगर(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } अन्यथा {
        leadCC=0;
    }
    वापस बफर.append(reinterpret_cast<स्थिर char16_t*>(mapping) + 1, length, true, leadCC, trailCC, errorCode);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
स्थिर uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               स्थिर uint8_t *src, स्थिर uint8_t *limit,
                               ByteSink *sink, Edits *edits, UErrorCode &errorCode) स्थिर {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    स्थिर uint8_t *पूर्व_Boundary = src;
    // only for quick check
    uint8_t पूर्व_CC = 0;

    क्रम (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        स्थिर uint8_t *fastStart = src;
        स्थिर uint8_t *पूर्व_Src;
        uint16_t norm16 = 0;

        क्रम (;;) {
            अगर (src == limit) {
                अगर (पूर्व_Boundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(पूर्व_Boundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                वापस src;
            }
            अगर (*src < minNoLead) {
                ++src;
            } अन्यथा {
                पूर्व_Src = src;
                UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, src, limit, norm16);
                अगर (!isMostDecompYesAndZeroCC(norm16)) {
                    अवरोध;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        अगर (पूर्व_Src != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            अगर (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                   *sink, options, edits, errorCode)) {
                अवरोध;
            }
            पूर्व_Boundary = पूर्व_Src;
            पूर्व_CC = 0;
        }

        // Medium-fast path: Quick check.
        अगर (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            अगर (पूर्व_CC <= cc || cc == 0) {
                पूर्व_CC = cc;
                अगर (cc <= 1) {
                    अगर (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, src,
                                                           *sink, options, edits, errorCode)) {
                        अवरोध;
                    }
                    पूर्व_Boundary = src;
                }
                जारी;
            }
        }
        अगर (sink == nullptr) {
            वापस पूर्व_Boundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        अगर (पूर्व_Boundary != पूर्व_Src && norm16HasDecompBoundaryBefore(norm16)) {
            अगर (!ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                               *sink, options, edits, errorCode)) {
                अवरोध;
            }
            पूर्व_Boundary = पूर्व_Src;
        }
        Reordering_बफर बफर(*यह, s16, errorCode);
        अगर (U_FAILURE(errorCode)) {
            अवरोध;
        }
        decomposeShort(पूर्व_Boundary, src, STOP_AT_LIMIT, false /* onlyContiguous */,
                       बफर, errorCode);
        // Decompose until the next boundary.
        अगर (बफर.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, false /* onlyContiguous */,
                                 बफर, errorCode);
        }
        अगर (U_FAILURE(errorCode)) {
            अवरोध;
        }
        अगर ((src - पूर्व_Src) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
            अवरोध;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        अगर (isMaybeYesOrNonZeroCC(norm16) && बफर.equals(पूर्व_Boundary, src)) {
            अगर (!ByteSinkUtil::appendUnchanged(पूर्व_Boundary, src,
                                               *sink, options, edits, errorCode)) {
                अवरोध;
            }
        } अन्यथा {
            अगर (!ByteSinkUtil::appendChange(पूर्व_Boundary, src, बफर.getStart(), बफर.length(),
                                            *sink, edits, errorCode)) {
                अवरोध;
            }
        }
        पूर्व_Boundary = src;
        पूर्व_CC = 0;
    }
    वापस src;
}

स्थिर uint8_t *
Normalizer2Impl::decomposeShort(स्थिर uint8_t *src, स्थिर uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    जबतक (src < limit) {
        स्थिर uint8_t *पूर्व_Src = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        अगर (norm16 >= limitNoNo) {
            अगर (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                अगर (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    वापस पूर्व_Src;
                }
                c = codePointFromValidUTF8(पूर्व_Src, src);
                अगर (!बफर.append(c, cc, errorCode)) {
                    वापस nullptr;
                }
                अगर (stopAt == STOP_AT_DECOMP_BOUNDARY && बफर.getLastCC() <= 1) {
                    वापस src;
                }
                जारी;
            } अन्यथा अगर (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                अगर (stopAt != STOP_AT_LIMIT) {
                    वापस पूर्व_Src;
                }
                c = codePointFromValidUTF8(पूर्व_Src, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } अन्यथा अगर (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            वापस पूर्व_Src;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        अगर (norm16 < minYesNo) {
            अगर (c < 0) {
                c = codePointFromValidUTF8(पूर्व_Src, src);
            }
            // does not decompose
            अगर (!बफर.append(c, 0, errorCode)) {
                वापस nullptr;
            }
        } अन्यथा अगर (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            अगर (c < 0) {
                c = codePointFromValidUTF8(पूर्व_Src, src);
            }
            char16_t jamos[3];
            अगर (!बफर.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                वापस nullptr;
            }
        } अन्यथा {
            // The character decomposes, get everything from the variable-length extra data.
            स्थिर uint16_t *mapping = get_तथ्य(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_लंबाई_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            अगर (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } अन्यथा {
                leadCC = 0;
            }
            अगर (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                वापस पूर्व_Src;
            }
            अगर (!बफर.append(reinterpret_cast<स्थिर char16_t*>(mapping) + 1, length, true, leadCC, trailCC, errorCode)) {
                वापस nullptr;
            }
        }
        अगर ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && बफर.getLastCC() <= 1)) {
            वापस src;
        }
    }
    वापस src;
}

स्थिर char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t बफर[4], int32_t &length) स्थिर {
    uint16_t norm16;
    अगर(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        वापस nullptr;
    }
    स्थिर char16_t *decomp = nullptr;
    अगर(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=बफर;
        length=0;
        U16_APPEND_UNSAFE(बफर, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    अगर (norm16 < minYesNo) {
        वापस decomp;
    } अन्यथा अगर(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, बफर);
        वापस बफर;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिर uint16_t *mapping=get_तथ्य(norm16);
    length=*mapping&MAPPING_लंबाई_MASK;
    वापस reinterpret_cast<स्थिर char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
स्थिर char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t बफर[30], int32_t &length) स्थिर {
    uint16_t norm16;
    अगर(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        वापस nullptr;
    } अन्यथा अगर(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, बफर);
        length=2;
        वापस बफर;
    } अन्यथा अगर(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(बफर, length, c);
        वापस बफर;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिर uint16_t *mapping=get_तथ्य(norm16);
    uint16_t firstUnit=*mapping;
    int32_t m_लंबाई=firstUnit&MAPPING_लंबाई_MASK;  // length of normal mapping
    अगर(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        स्थिर uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        अगर(rm0<=MAPPING_लंबाई_MASK) {
            length=rm0;
            वापस reinterpret_cast<स्थिर char16_t*>(rawMapping) - rm0;
        } अन्यथा {
            // Copy the normal mapping and replace its first two code units with rm0.
            बफर[0] = static_cast<char16_t>(rm0);
            u_memcpy(बफर + 1, reinterpret_cast<स्थिर char16_t*>(mapping) + 1 + 2, m_लंबाई - 2);
            length=m_लंबाई-1;
            वापस बफर;
        }
    } अन्यथा {
        length=m_लंबाई;
        वापस reinterpret_cast<स्थिर char16_t*>(mapping) + 1;
    }
}

व्योम Normalizer2Impl::decomposeAndAppend(स्थिर char16_t *src, स्थिर char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_मध्य,
                                         Reordering_बफर &बफर,
                                         UErrorCode &errorCode) स्थिर {
    बफर.copyReorderableSuffixTo(safe_मध्य);
    अगर(doDecompose) {
        decompose(src, limit, &बफर, errorCode);
        वापस;
    }
    // Just merge the strings at the boundary.
    bool isFirst = true;
    uint8_t firstCC = 0, पूर्व_CC = 0, cc;
    स्थिर char16_t *p = src;
    जबतक (p != limit) {
        स्थिर char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, p, limit, c, norm16);
        अगर ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            अवरोध;
        }
        अगर (isFirst) {
            firstCC = cc;
            isFirst = false;
        }
        पूर्व_CC = cc;
    }
    अगर(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    अगर (बफर.append(src, static_cast<int32_t>(p - src), false, firstCC, पूर्व_CC, errorCode)) {
        बफर.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) स्थिर {
    वापस c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) स्थिर {
    अगर (norm16 < minNoNoCompNoMaybeCC) {
        वापस true;
    }
    अगर (norm16 >= limitNoNo) {
        वापस norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिर uint16_t *mapping=get_तथ्य_For_Yes_Or_No(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    वापस (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) स्थिर {
    अगर (c < minDecompNoCP) {
        वापस true;
    }
    अगर (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        वापस true;
    }
    वापस norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) स्थिर {
    अगर(norm16 <= minYesNo || isHangulLVT(norm16)) {
        वापस true;
    }
    अगर (norm16 >= limitNoNo) {
        अगर (isMaybeYesOrNonZeroCC(norm16)) {
            वापस norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } अन्यथा अगर (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            वापस (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिर uint16_t *mapping=get_तथ्य(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    अगर(firstUnit>0x1ff) {
        वापस false;  // trailCC>1
    }
    अगर(firstUnit<=0xff) {
        वापस true;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    वापस (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
int32_t Normalizer2Impl::combine(स्थिर uint16_t *list, UChar32 trail) {
    uint16_t key1, firstUnit;
    अगर(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        जबतक(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        अगर(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            अगर(firstUnit&COMP_1_TRIPLE) {
                वापस (static_cast<int32_t>(list[1]) << 16) | list[2];
            } अन्यथा {
                वापस list[1];
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
        क्रम(;;) {
            अगर(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } अन्यथा अगर(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                अगर(key2>(secondUnit=list[1])) {
                    अगर(firstUnit&COMP_1_LAST_TUPLE) {
                        अवरोध;
                    } अन्यथा {
                        list+=3;
                    }
                } अन्यथा अगर(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    वापस (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } अन्यथा {
                    अवरोध;
                }
            } अन्यथा {
                अवरोध;
            }
        }
    }
    वापस -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
व्योम Normalizer2Impl::addComposites(स्थिर uint16_t *list, UnicodeSet &set) स्थिर {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    करो {
        firstUnit=*list;
        अगर((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } अन्यथा {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        अगर((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } जबतक((firstUnit&COMP_1_LAST_TUPLE)==0);
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
व्योम Normalizer2Impl::recompose(Reordering_बफर &बफर, int32_t recompose_Start_सूचकांक,
                                UBool onlyContiguous) स्थिर {
    char16_t *p=बफर.getStart()+recompose_Start_सूचकांक;
    char16_t *limit=बफर.getLimit();
    अगर(p==limit) {
        वापस;
    }

    char16_t *starter, *pRemove, *q, *r;
    स्थिर uint16_t *compositionsList;
    UChar32 c, compositeAndFwd;
    uint16_t norm16;
    uint8_t cc, पूर्व_CC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=nullptr;  // used as indicator for whether we have a forward-combining starter
    starter=nullptr;
    starterIsSupplementary=false;
    पूर्व_CC=0;

    क्रम(;;) {
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        अगर( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (पूर्व_CC<cc || पूर्व_CC==0)
        ) {
            अगर(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                अगर(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t पूर्व = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    अगर(पूर्व<Hangul::JAMO_L_गिनती) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (पूर्व*Hangul::JAMO_V_गिनती+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_गिनती);
                        char16_t t;
                        अगर (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_गिनती) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        जबतक(r<limit) {
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
                अगर(p==limit) {
                    अवरोध;
                }
                compositionsList=nullptr;
                जारी;
            } अन्यथा अगर((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_लंबाई(c);  // pRemove & p: start & limit of the combining mark
                अगर(starterIsSupplementary) {
                    अगर(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } अन्यथा {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=false;
                        q=starter+1;
                        r=q+1;
                        जबतक(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } अन्यथा अगर(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=true;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    जबतक(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } अन्यथा {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                अगर(pRemove<p) {
                    q=pRemove;
                    r=p;
                    जबतक(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                अगर(p==limit) {
                    अवरोध;
                }
                // Is the composite a starter that combines forward?
                अगर(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } अन्यथा {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                जारी;
            }
        }

        // no combination this time
        पूर्व_CC=cc;
        अगर(p==limit) {
            अवरोध;
        }

        // If c did not combine, then check if it is a starter.
        अगर(cc==0) {
            // Found a new starter.
            अगर((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                अगर(U_IS_BMP(c)) {
                    starterIsSupplementary=false;
                    starter=p-1;
                } अन्यथा {
                    starterIsSupplementary=true;
                    starter=p-2;
                }
            }
        } अन्यथा अगर(onlyContiguous) {
            // FCC: no discontiguous compositions; any intervening character blocks.
            compositionsList=nullptr;
        }
    }
    बफर.setReorderingLimit(limit);
}

UChar32
Normalizer2Impl::composePair(UChar32 a, UChar32 b) स्थिर {
    uint16_t norm16=getNorm16(a);  // maps an out-of-range 'a' to inert norm16
    स्थिर uint16_t *list;
    अगर(isInert(norm16)) {
        वापस U_SENTINEL;
    } अन्यथा अगर(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        अगर(isJamoL(norm16)) {
            अगर (b < Hangul::JAMO_V_BASE) {
                वापस U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            अगर(b<Hangul::JAMO_V_गिनती) {
                वापस
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_गिनती+b)*
                     Hangul::JAMO_T_गिनती);
            } अन्यथा {
                वापस U_SENTINEL;
            }
        } अन्यथा अगर(isHangulLV(norm16)) {
            अगर (b <= Hangul::JAMO_T_BASE) {
               वापस U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            अगर(b<Hangul::JAMO_T_गिनती) {  // not b==0!
                वापस a+b;
            } अन्यथा {
                वापस U_SENTINEL;
            }
        } अन्यथा {
            // 'a' has a compositions list in extraData
            list=get_तथ्य_For_Yes_Or_No(norm16);
            अगर(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_लंबाई_MASK);  // + mapping length
            }
        }
    } अन्यथा अगर(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        वापस U_SENTINEL;
    } अन्यथा {
        list=get_तथ्य_For_Maybe(norm16);
        अगर(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_लंबाई_MASK);  // + mapping length
        }
    }
    अगर(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        वापस U_SENTINEL;
    }
#अगर U_SIGNED_दायाँ_SHIFT_IS_ARITHMETIC
    वापस combine(list, b)>>1;
#अन्यथा
    int32_t compositeAndFwd=combine(list, b);
    वापस compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
#पूर्ण_अगर
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(स्थिर char16_t *src, स्थिर char16_t *limit,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_बफर &बफर,
                         UErrorCode &errorCode) स्थिर {
    स्थिर char16_t *पूर्व_Boundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    अगर(limit==nullptr) {
        src=copy_निचला_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &बफर : nullptr,
                                           errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस false;
        }
        limit=u_strchr(src, 0);
        अगर (पूर्व_Boundary != src) {
            अगर (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                पूर्व_Boundary = src;
            } अन्यथा {
                बफर.removeSuffix(1);
                पूर्व_Boundary = --src;
            }
        }
    }

    क्रम (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        स्थिर char16_t *पूर्व_Src;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        क्रम (;;) {
            अगर (src == limit) {
                अगर (पूर्व_Boundary != limit && doCompose) {
                    बफर.appendZeroCC(पूर्व_Boundary, limit, errorCode);
                }
                वापस true;
            }
            अगर( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा {
                पूर्व_Src = src++;
                अगर(!U16_IS_LEAD(c)) {
                    अवरोध;
                } अन्यथा {
                    char16_t c2;
                    अगर(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        अगर(!isCompYesAndZeroCC(norm16)) {
                            अवरोध;
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
        अगर (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            अगर (!doCompose) {
                वापस false;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            अगर (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                अगर (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    अगर (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, errorCode)) {
                        अवरोध;
                    }
                    अगर(!बफर.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        अवरोध;
                    }
                    पूर्व_Boundary = src;
                    जारी;
                }
            } अन्यथा अगर (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                अगर (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    अगर (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, errorCode)) {
                        अवरोध;
                    }
                    स्थिर char16_t *mapping = reinterpret_cast<स्थिर char16_t *>(get_तथ्य_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_लंबाई_MASK;
                    अगर(!बफर.appendZeroCC(mapping, mapping + length, errorCode)) {
                        अवरोध;
                    }
                    पूर्व_Boundary = src;
                    जारी;
                }
            } अन्यथा अगर (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                अगर (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(पूर्व_Boundary, पूर्व_Src, onlyContiguous)) {
                    अगर (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, errorCode)) {
                        अवरोध;
                    }
                    पूर्व_Boundary = src;
                    जारी;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } अन्यथा अगर (isJamoVT(norm16) && पूर्व_Boundary != पूर्व_Src) {
            char16_t पूर्व=*(पूर्व_Src-1);
            अगर(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(पूर्व - Hangul::JAMO_L_BASE);
                अगर(l<Hangul::JAMO_L_गिनती) {
                    अगर (!doCompose) {
                        वापस false;
                    }
                    int32_t t;
                    अगर (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_गिनती) {
                        // The next character is a Jamo T.
                        ++src;
                    } अन्यथा अगर (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } अन्यथा {
                        t = -1;
                    }
                    अगर (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_गिनती + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_गिनती + t;
                        --पूर्व_Src;  // Replace the Jamo L as well.
                        अगर (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, errorCode)) {
                            अवरोध;
                        }
                        अगर (!बफर.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            अवरोध;
                        }
                        पूर्व_Boundary = src;
                        जारी;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } अन्यथा अगर (Hangul::isHangulLV(पूर्व)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                अगर (!doCompose) {
                    वापस false;
                }
                UChar32 syllable = पूर्व + c - Hangul::JAMO_T_BASE;
                --पूर्व_Src;  // Replace the Hangul LV as well.
                अगर (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, errorCode)) {
                    अवरोध;
                }
                अगर (!बफर.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    अवरोध;
                }
                पूर्व_Boundary = src;
                जारी;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } अन्यथा अगर (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            अगर (onlyContiguous /* FCC */ && getPreviousTrailCC(पूर्व_Boundary, पूर्व_Src) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                अगर (!doCompose) {
                    वापस false;
                }
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                स्थिर char16_t *अग्र_Src;
                uint16_t n16;
                क्रम (;;) {
                    अगर (src == limit) {
                        अगर (doCompose) {
                            बफर.appendZeroCC(पूर्व_Boundary, limit, errorCode);
                        }
                        वापस true;
                    }
                    uint8_t पूर्व_CC = cc;
                    अग्र_Src = src;
                    UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, अग्र_Src, limit, c, n16);
                    अगर (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        अगर (पूर्व_CC > cc) {
                            अगर (!doCompose) {
                                वापस false;
                            }
                            अवरोध;
                        }
                    } अन्यथा {
                        अवरोध;
                    }
                    src = अग्र_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                अगर (norm16HasCompBoundaryBefore(n16)) {
                    अगर (isCompYesAndZeroCC(n16)) {
                        src = अग्र_Src;
                    }
                    जारी;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        अगर (पूर्व_Boundary != पूर्व_Src && !norm16HasCompBoundaryBefore(norm16)) {
            स्थिर char16_t *p = पूर्व_Src;
            UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, पूर्व_Boundary, p, c, norm16);
            अगर (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                पूर्व_Src = p;
            }
        }
        अगर (doCompose && पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, errorCode)) {
            अवरोध;
        }
        int32_t recompose_Start_सूचकांक=बफर.length();
        // We know there is not a boundary here.
        decomposeShort(पूर्व_Src, src, false /* !stopAtCompBoundary */, onlyContiguous,
                       बफर, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, true /* stopAtCompBoundary */, onlyContiguous,
                             बफर, errorCode);
        अगर (U_FAILURE(errorCode)) {
            अवरोध;
        }
        अगर ((src - पूर्व_Src) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस true;
        }
        recompose(बफर, recompose_Start_सूचकांक, onlyContiguous);
        अगर(!doCompose) {
            अगर(!बफर.equals(पूर्व_Src, src)) {
                वापस false;
            }
            बफर.हटाओ();
        }
        पूर्व_Boundary=src;
    }
    वापस true;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
स्थिर char16_t *
Normalizer2Impl::composeQuickCheck(स्थिर char16_t *src, स्थिर char16_t *limit,
                                   UBool onlyContiguous,
                                   UNormalization_Check_परिणाम *pQCResult) स्थिर {
    स्थिर char16_t *पूर्व_Boundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    अगर(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_निचला_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        अगर (पूर्व_Boundary != src) {
            अगर (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                पूर्व_Boundary = src;
            } अन्यथा {
                पूर्व_Boundary = --src;
            }
        }
    }

    क्रम(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        स्थिर char16_t *पूर्व_Src;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        क्रम (;;) {
            अगर(src==limit) {
                वापस src;
            }
            अगर( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा {
                पूर्व_Src = src++;
                अगर(!U16_IS_LEAD(c)) {
                    अवरोध;
                } अन्यथा {
                    char16_t c2;
                    अगर(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        अगर(!isCompYesAndZeroCC(norm16)) {
                            अवरोध;
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

        uint16_t पूर्व_Norm16 = INERT;
        अगर (पूर्व_Boundary != पूर्व_Src) {
            अगर (norm16HasCompBoundaryBefore(norm16)) {
                पूर्व_Boundary = पूर्व_Src;
            } अन्यथा {
                स्थिर char16_t *p = पूर्व_Src;
                uint16_t n16;
                UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, पूर्व_Boundary, p, c, n16);
                अगर (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    पूर्व_Boundary = पूर्व_Src;
                } अन्यथा {
                    पूर्व_Boundary = p;
                    पूर्व_Norm16 = n16;
                }
            }
        }

        अगर (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            अगर (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(पूर्व_Norm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                स्थिर char16_t *अग्र_Src;
                क्रम (;;) {
                    अगर (norm16 < MIN_YES_YES_WITH_CC) {
                        अगर (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } अन्यथा {
                            वापस पूर्व_Boundary;
                        }
                    }
                    अगर (src == limit) {
                        वापस src;
                    }
                    uint8_t पूर्व_CC = fcd16;
                    अग्र_Src = src;
                    UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, अग्र_Src, limit, c, norm16);
                    अगर (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        अगर (!(पूर्व_CC <= cc || cc == 0)) {
                            अवरोध;
                        }
                    } अन्यथा {
                        अवरोध;
                    }
                    src = अग्र_Src;
                }
                // src is after the last in-order combining mark.
                अगर (isCompYesAndZeroCC(norm16)) {
                    पूर्व_Boundary = src;
                    src = अग्र_Src;
                    जारी;
                }
            }
        }
        अगर(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        वापस पूर्व_Boundary;
    }
}

व्योम Normalizer2Impl::composeAndAppend(स्थिर char16_t *src, स्थिर char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_मध्य,
                                       Reordering_बफर &बफर,
                                       UErrorCode &errorCode) स्थिर {
    अगर(!बफर.isEmpty()) {
        स्थिर char16_t *firstStarterInSrc=find_अग्र_Comp_Boundary(src, limit, onlyContiguous);
        अगर(src!=firstStarterInSrc) {
            स्थिर char16_t *lastStarterInDest=findPreviousCompBoundary(बफर.getStart(),
                                                                    बफर.getLimit(), onlyContiguous);
            int32_t dest_Suffix_लंबाई = static_cast<int32_t>(बफर.getLimit() - lastStarterInDest);
            UnicodeString मध्य(lastStarterInDest, dest_Suffix_लंबाई);
            बफर.removeSuffix(dest_Suffix_लंबाई);
            safe_मध्य=मध्य;
            मध्य.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            स्थिर char16_t *मध्य_Start=मध्य.get_बफर();
            compose(मध्य_Start, मध्य_Start+मध्य.length(), onlyContiguous,
                    true, बफर, errorCode);
            अगर(U_FAILURE(errorCode)) {
                वापस;
            }
            src=firstStarterInSrc;
        }
    }
    अगर(doCompose) {
        compose(src, limit, onlyContiguous, true, बफर, errorCode);
    } अन्यथा {
        अगर(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        बफर.appendZeroCC(src, limit, errorCode);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             स्थिर uint8_t *src, स्थिर uint8_t *limit,
                             ByteSink *sink, Edits *edits, UErrorCode &errorCode) स्थिर {
    U_ASSERT(limit != nullptr);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    स्थिर uint8_t *पूर्व_Boundary = src;

    क्रम (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        स्थिर uint8_t *पूर्व_Src;
        uint16_t norm16 = 0;
        क्रम (;;) {
            अगर (src == limit) {
                अगर (पूर्व_Boundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(पूर्व_Boundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                वापस true;
            }
            अगर (*src < minNoMaybeLead) {
                ++src;
            } अन्यथा {
                पूर्व_Src = src;
                UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, src, limit, norm16);
                अगर (!isCompYesAndZeroCC(norm16)) {
                    अवरोध;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        अगर (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            अगर (sink == nullptr) {
                वापस false;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            अगर (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                अगर (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    अगर (पूर्व_Boundary != पूर्व_Src &&
                            !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                           *sink, options, edits, errorCode)) {
                        अवरोध;
                    }
                    appendCodePointDelta(पूर्व_Src, src, getAlgorithmicDelta(norm16), *sink, edits);
                    पूर्व_Boundary = src;
                    जारी;
                }
            } अन्यथा अगर (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                अगर (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    अगर (पूर्व_Boundary != पूर्व_Src &&
                            !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                           *sink, options, edits, errorCode)) {
                        अवरोध;
                    }
                    स्थिर uint16_t *mapping = get_तथ्य_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_लंबाई_MASK;
                    अगर (!ByteSinkUtil::appendChange(पूर्व_Src, src, reinterpret_cast<स्थिर char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        अवरोध;
                    }
                    पूर्व_Boundary = src;
                    जारी;
                }
            } अन्यथा अगर (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                अगर (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(पूर्व_Boundary, पूर्व_Src, onlyContiguous)) {
                    अगर (पूर्व_Boundary != पूर्व_Src &&
                            !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                           *sink, options, edits, errorCode)) {
                        अवरोध;
                    }
                    अगर (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - पूर्व_Src), 0);
                    }
                    पूर्व_Boundary = src;
                    जारी;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } अन्यथा अगर (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - पूर्व_Src) == 3 && *पूर्व_Src == 0xe1);
            UChar32 पूर्व = previousHangulOrJamo(पूर्व_Boundary, पूर्व_Src);
            अगर (पूर्व_Src[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = पूर्व - Hangul::JAMO_L_BASE;
                अगर (static_cast<uint32_t>(l) < Hangul::JAMO_L_गिनती) {
                    अगर (sink == nullptr) {
                        वापस false;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    अगर (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } अन्यथा अगर (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    अगर (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_गिनती + (पूर्व_Src[2]-0xa1)) *
                            Hangul::JAMO_T_गिनती + t;
                        पूर्व_Src -= 3;  // Replace the Jamo L as well.
                        अगर (पूर्व_Boundary != पूर्व_Src &&
                                !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                               *sink, options, edits, errorCode)) {
                            अवरोध;
                        }
                        ByteSinkUtil::appendCodePoint(पूर्व_Src, src, syllable, *sink, edits);
                        पूर्व_Boundary = src;
                        जारी;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } अन्यथा अगर (Hangul::isHangulLV(पूर्व)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                अगर (sink == nullptr) {
                    वापस false;
                }
                UChar32 syllable = पूर्व + getJamoTMinusBase(पूर्व_Src, src);
                पूर्व_Src -= 3;  // Replace the Hangul LV as well.
                अगर (पूर्व_Boundary != पूर्व_Src &&
                        !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                       *sink, options, edits, errorCode)) {
                    अवरोध;
                }
                ByteSinkUtil::appendCodePoint(पूर्व_Src, src, syllable, *sink, edits);
                पूर्व_Boundary = src;
                जारी;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } अन्यथा अगर (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            अगर (onlyContiguous /* FCC */ && getPreviousTrailCC(पूर्व_Boundary, पूर्व_Src) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                अगर (sink == nullptr) {
                    वापस false;
                }
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                स्थिर uint8_t *अग्र_Src;
                uint16_t n16;
                क्रम (;;) {
                    अगर (src == limit) {
                        अगर (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(पूर्व_Boundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        वापस true;
                    }
                    uint8_t पूर्व_CC = cc;
                    अग्र_Src = src;
                    UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, अग्र_Src, limit, n16);
                    अगर (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        अगर (पूर्व_CC > cc) {
                            अगर (sink == nullptr) {
                                वापस false;
                            }
                            अवरोध;
                        }
                    } अन्यथा {
                        अवरोध;
                    }
                    src = अग्र_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                अगर (norm16HasCompBoundaryBefore(n16)) {
                    अगर (isCompYesAndZeroCC(n16)) {
                        src = अग्र_Src;
                    }
                    जारी;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        अगर (पूर्व_Boundary != पूर्व_Src && !norm16HasCompBoundaryBefore(norm16)) {
            स्थिर uint8_t *p = पूर्व_Src;
            UCPTRIE_FAST_U8_पूर्व(normTrie, UCPTRIE_16, पूर्व_Boundary, p, norm16);
            अगर (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                पूर्व_Src = p;
            }
        }
        Reordering_बफर बफर(*यह, s16, errorCode);
        अगर (U_FAILURE(errorCode)) {
            अवरोध;
        }
        // We know there is not a boundary here.
        decomposeShort(पूर्व_Src, src, STOP_AT_LIMIT, onlyContiguous,
                       बफर, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             बफर, errorCode);
        अगर (U_FAILURE(errorCode)) {
            अवरोध;
        }
        अगर ((src - पूर्व_Src) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस true;
        }
        recompose(बफर, 0, onlyContiguous);
        अगर (!बफर.equals(पूर्व_Src, src)) {
            अगर (sink == nullptr) {
                वापस false;
            }
            अगर (पूर्व_Boundary != पूर्व_Src &&
                    !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                   *sink, options, edits, errorCode)) {
                अवरोध;
            }
            अगर (!ByteSinkUtil::appendChange(पूर्व_Src, src, बफर.getStart(), बफर.length(),
                                            *sink, edits, errorCode)) {
                अवरोध;
            }
            पूर्व_Boundary = src;
        }
    }
    वापस true;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(स्थिर char16_t *src, स्थिर char16_t *limit) स्थिर {
    अगर (src == limit || *src < minCompNoMaybeCP) {
        वापस true;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, src, limit, c, norm16);
    वापस norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(स्थिर uint8_t *src, स्थिर uint8_t *limit) स्थिर {
    अगर (src == limit) {
        वापस true;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, src, limit, norm16);
    वापस norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(स्थिर char16_t *start, स्थिर char16_t *p,
                                            UBool onlyContiguous) स्थिर {
    अगर (start == p) {
        वापस true;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, start, p, c, norm16);
    वापस norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(स्थिर uint8_t *start, स्थिर uint8_t *p,
                                            UBool onlyContiguous) स्थिर {
    अगर (start == p) {
        वापस true;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_पूर्व(normTrie, UCPTRIE_16, start, p, norm16);
    वापस norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

स्थिर char16_t *Normalizer2Impl::findPreviousCompBoundary(स्थिर char16_t *start, स्थिर char16_t *p,
                                                       UBool onlyContiguous) स्थिर {
    जबतक (p != start) {
        स्थिर char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, start, p, c, norm16);
        अगर (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            वापस codePointLimit;
        }
        अगर (hasCompBoundaryBefore(c, norm16)) {
            वापस p;
        }
    }
    वापस p;
}

स्थिर char16_t *Normalizer2Impl::find_अग्र_Comp_Boundary(स्थिर char16_t *p, स्थिर char16_t *limit,
                                                   UBool onlyContiguous) स्थिर {
    जबतक (p != limit) {
        स्थिर char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, p, limit, c, norm16);
        अगर (hasCompBoundaryBefore(c, norm16)) {
            वापस codePointStart;
        }
        अगर (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            वापस p;
        }
    }
    वापस p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(स्थिर char16_t *start, स्थिर char16_t *p) स्थिर {
    अगर (start == p) {
        वापस 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_पूर्व(start, 0, i, c);
    वापस static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(स्थिर uint8_t *start, स्थिर uint8_t *p) स्थिर {
    अगर (start == p) {
        वापस 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_पूर्व(start, 0, i, c);
    वापस static_cast<uint8_t>(getFCD16(c));
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
#अगर (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#आशय optimize( "", off )
#पूर्ण_अगर
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_तथ्य(UChar32 c) स्थिर {
    uint16_t norm16=getNorm16(c);
    अगर (norm16 >= limitNoNo) {
        अगर(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            वापस norm16|(norm16<<8);
        } अन्यथा अगर(norm16>=minMaybeYes) {
            वापस 0;
        } अन्यथा अगर(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            अगर (deltaTrailCC <= DELTA_TCCC_1) {
                वापस deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    अगर(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        वापस 0;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिर uint16_t *mapping=get_तथ्य(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    अगर(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    वापस norm16;
}
#अगर (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#आशय optimize( "", on )
#पूर्ण_अगर

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) स्थिर {
    U_ASSERT(norm16 >= minMaybeNo);
    अगर (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        वापस norm16 | (norm16<<8);
    } अन्यथा अगर (norm16 >= minMaybeYes) {
        वापस 0;
    }
    // c decomposes, get everything from the variable-length extra data
    स्थिर uint16_t *mapping = get_तथ्य_For_Maybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    वापस firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
स्थिर char16_t *
Normalizer2Impl::makeFCD(स्थिर char16_t *src, स्थिर char16_t *limit,
                         Reordering_बफर *बफर,
                         UErrorCode &errorCode) स्थिर {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    स्थिर char16_t *पूर्व_Boundary=src;
    int32_t पूर्व_FCD16=0;
    अगर(limit==nullptr) {
        src=copy_निचला_Prefix_From_Nul_Terminated(src, minLcccCP, बफर, errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस src;
        }
        अगर(पूर्व_Boundary<src) {
            पूर्व_Boundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            पूर्व_FCD16=getFCD16(*(src-1));
            अगर(पूर्व_FCD16>1) {
                --पूर्व_Boundary;
            }
        }
        limit=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    स्थिर char16_t *पूर्व_Src;
    UChar32 c=0;
    uint16_t fcd16=0;

    क्रम(;;) {
        // count code units with lccc==0
        क्रम(पूर्व_Src=src; src!=limit;) {
            अगर((c=*src)<minLcccCP) {
                पूर्व_FCD16=~c;
                ++src;
            } अन्यथा अगर(!singleLeadMightHaveNonZeroFCD16(c)) {
                पूर्व_FCD16=0;
                ++src;
            } अन्यथा {
                अगर(U16_IS_LEAD(c)) {
                    char16_t c2;
                    अगर((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                अगर((fcd16=get_FCD16_From_Norm_तथ्य(c))<=0xff) {
                    पूर्व_FCD16=fcd16;
                    src+=U16_लंबाई(c);
                } अन्यथा {
                    अवरोध;
                }
            }
        }
        // copy these code units all at once
        अगर(src!=पूर्व_Src) {
            अगर(बफर!=nullptr && !बफर->appendZeroCC(पूर्व_Src, src, errorCode)) {
                अवरोध;
            }
            अगर(src==limit) {
                अवरोध;
            }
            पूर्व_Boundary=src;
            // We know that the previous character's lccc==0.
            अगर(पूर्व_FCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 पूर्व=~पूर्व_FCD16;
                अगर(पूर्व<minDecompNoCP) {
                    पूर्व_FCD16=0;
                } अन्यथा {
                    पूर्व_FCD16=get_FCD16_From_Norm_तथ्य(पूर्व);
                    अगर(पूर्व_FCD16>1) {
                        --पूर्व_Boundary;
                    }
                }
            } अन्यथा {
                स्थिर char16_t *p=src-1;
                अगर(U16_IS_TRAIL(*p) && पूर्व_Src<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    पूर्व_FCD16=get_FCD16_From_Norm_तथ्य(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                अगर(पूर्व_FCD16>1) {
                    पूर्व_Boundary=p;
                }
            }
            // The start of the current character (c).
            पूर्व_Src=src;
        } अन्यथा अगर(src==limit) {
            अवरोध;
        }

        src+=U16_लंबाई(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        अगर((पूर्व_FCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            अगर((fcd16&0xff)<=1) {
                पूर्व_Boundary=src;
            }
            अगर(बफर!=nullptr && !बफर->appendZeroCC(c, errorCode)) {
                अवरोध;
            }
            पूर्व_FCD16=fcd16;
            जारी;
        } अन्यथा अगर(बफर==nullptr) {
            वापस पूर्व_Boundary;  // quick check "no"
        } अन्यथा {
            /*
             * Back out the part of the source that we copied or appended
             * already but is now going to be decomposed.
             * prevSrc is set to after what was copied/appended.
             */
            बफर->removeSuffix(static_cast<int32_t>(पूर्व_Src - पूर्व_Boundary));
            /*
             * Find the part of the source that needs to be decomposed,
             * up to the next safe boundary.
             */
            src=find_अग्र_FCDBoundary(src, limit);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(पूर्व_Boundary, src, false, false, *बफर, errorCode);
            अगर (U_FAILURE(errorCode)) {
                अवरोध;
            }
            पूर्व_Boundary=src;
            पूर्व_FCD16=0;
        }
    }
    वापस src;
}

व्योम Normalizer2Impl::makeFCDAndAppend(स्थिर char16_t *src, स्थिर char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_मध्य,
                                       Reordering_बफर &बफर,
                                       UErrorCode &errorCode) स्थिर {
    अगर(!बफर.isEmpty()) {
        स्थिर char16_t *firstBoundaryInSrc=find_अग्र_FCDBoundary(src, limit);
        अगर(src!=firstBoundaryInSrc) {
            स्थिर char16_t *lastBoundaryInDest=findPreviousFCDBoundary(बफर.getStart(),
                                                                    बफर.getLimit());
            int32_t dest_Suffix_लंबाई = static_cast<int32_t>(बफर.getLimit() - lastBoundaryInDest);
            UnicodeString मध्य(lastBoundaryInDest, dest_Suffix_लंबाई);
            बफर.removeSuffix(dest_Suffix_लंबाई);
            safe_मध्य=मध्य;
            मध्य.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            स्थिर char16_t *मध्य_Start=मध्य.get_बफर();
            makeFCD(मध्य_Start, मध्य_Start+मध्य.length(), &बफर, errorCode);
            अगर(U_FAILURE(errorCode)) {
                वापस;
            }
            src=firstBoundaryInSrc;
        }
    }
    अगर(doMakeFCD) {
        makeFCD(src, limit, &बफर, errorCode);
    } अन्यथा {
        अगर(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        बफर.appendZeroCC(src, limit, errorCode);
    }
}

स्थिर char16_t *Normalizer2Impl::findPreviousFCDBoundary(स्थिर char16_t *start, स्थिर char16_t *p) स्थिर {
    जबतक(start<p) {
        स्थिर char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, start, p, c, norm16);
        अगर (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            वापस codePointLimit;
        }
        अगर (norm16HasDecompBoundaryBefore(norm16)) {
            वापस p;
        }
    }
    वापस p;
}

स्थिर char16_t *Normalizer2Impl::find_अग्र_FCDBoundary(स्थिर char16_t *p, स्थिर char16_t *limit) स्थिर {
    जबतक(p<limit) {
        स्थिर char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, p, limit, c, norm16);
        अगर (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            वापस codePointStart;
        }
        अगर (norm16HasDecompBoundaryAfter(norm16)) {
            वापस p;
        }
    }
    वापस p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_तथ्य::Canon_Iter_तथ्य(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

Canon_Iter_तथ्य::~Canon_Iter_तथ्य() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

व्योम Canon_Iter_तथ्य::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_मूल्य = umutablecptrie_get(mutableTrie, decompLead);
    अगर((canon_मूल्य&(CANON_HAS_SET|CANON_मूल्य_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_मूल्य|origin, &errorCode);
    } अन्यथा {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        अगर((canon_मूल्य&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(नया UnicodeSet, errorCode);
            set=lpSet.getAlias();
            अगर(U_FAILURE(errorCode)) {
                वापस;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_मूल्य & CANON_मूल्य_MASK);
            canon_मूल्य = (canon_मूल्य & ~CANON_मूल्य_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_मूल्य, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            अगर (U_FAILURE(errorCode)) {
                वापस;
            }
            अगर(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } अन्यथा {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_मूल्य & CANON_मूल्य_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
श्रेणी Init_Canon_Iter_तथ्य {
खुला:
    अटल व्योम doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
अटल व्योम U_CALLCONV
init_Canon_Iter_तथ्य(Normalizer2Impl *impl, UErrorCode &errorCode) {
    Init_Canon_Iter_तथ्य::doInit(impl, errorCode);
}

U_CDECL_END

व्योम Init_Canon_Iter_तथ्य::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->f_Canon_Iter_तथ्य == nullptr);
    impl->f_Canon_Iter_तथ्य = नया Canon_Iter_तथ्य(errorCode);
    अगर (impl->f_Canon_Iter_तथ्य == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        जबतक ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            अगर (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_तथ्य_From_Norm16(start, end, value, *impl->f_Canon_Iter_तथ्य, errorCode);
            }
            start = end + 1;
        }
#अगर_घोषित UCPTRIE_DEBUG
        umutablecptrie_setName(impl->f_Canon_Iter_तथ्य->mutableTrie, "CanonIterData");
#पूर्ण_अगर
        impl->f_Canon_Iter_तथ्य->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_तथ्य->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_मूल्य_BITS_32, &errorCode);
        umutablecptrie_close(impl->f_Canon_Iter_तथ्य->mutableTrie);
        impl->f_Canon_Iter_तथ्य->mutableTrie = nullptr;
    }
    अगर (U_FAILURE(errorCode)) {
        मिटाओ impl->f_Canon_Iter_तथ्य;
        impl->f_Canon_Iter_तथ्य = nullptr;
    }
}

व्योम Normalizer2Impl::make_Canon_Iter_तथ्य_From_Norm16(UChar32 start, UChar32 end, स्थिर uint16_t norm16,
                                                  Canon_Iter_तथ्य &new_तथ्य,
                                                  UErrorCode &errorCode) स्थिर {
    अगर(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        वापस;
    }
    क्रम(UChar32 c=start; c<=end; ++c) {
        uint32_t old_मूल्य = umutablecptrie_get(new_तथ्य.mutableTrie, c);
        uint32_t new_मूल्य=old_मूल्य;
        अगर(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_मूल्य|=CANON_NOT_SEGMENT_STARTER;
            अगर(norm16<MIN_NORMAL_MAYBE_YES) {
                new_मूल्य|=CANON_HAS_COMPOSITIONS;
            }
        } अन्यथा अगर(norm16<minYesNo) {
            new_मूल्य|=CANON_HAS_COMPOSITIONS;
        } अन्यथा {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            अगर (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            अगर (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                स्थिर uint16_t *mapping=get_तथ्य_For_Yes_Or_No(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_लंबाई_MASK;
                अगर((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    अगर(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_मूल्य|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                अगर(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_अग्र_UNSAFE(mapping, i, c2);
                    new_तथ्य.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    अगर(norm16_2>=minNoNo) {
                        जबतक(i<length) {
                            U16_अग्र_UNSAFE(mapping, i, c2);
                            uint32_t c2_मूल्य = umutablecptrie_get(new_तथ्य.mutableTrie, c2);
                            अगर((c2_मूल्य&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(new_तथ्य.mutableTrie, c2,
                                                   c2_मूल्य|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } अन्यथा {
                // c decomposed to c2 algorithmically; c has cc==0
                new_तथ्य.addToStartSet(c, c2, errorCode);
            }
        }
        अगर(new_मूल्य!=old_मूल्य) {
            umutablecptrie_set(new_तथ्य.mutableTrie, c, new_मूल्य, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_तथ्य(UErrorCode &errorCode) स्थिर {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(यह);
    umtx_initOnce(me->f_Canon_Iter_तथ्य_Init_Once, &init_Canon_Iter_तथ्य, me, errorCode);
    वापस U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_मूल्य(UChar32 c) स्थिर {
    वापस static_cast<int32_t>(ucptrie_get(f_Canon_Iter_तथ्य->trie, c));
}

स्थिर UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) स्थिर {
    वापस *static_cast<स्थिर UnicodeSet*>(f_Canon_Iter_तथ्य->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) स्थिर {
    वापस get_Canon_मूल्य(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) स्थिर {
    int32_t canon_मूल्य=get_Canon_मूल्य(c)&~CANON_NOT_SEGMENT_STARTER;
    अगर(canon_मूल्य==0) {
        वापस false;
    }
    set.सफा();
    int32_t value=canon_मूल्य&CANON_मूल्य_MASK;
    अगर((canon_मूल्य&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } अन्यथा अगर(value!=0) {
        set.add(value);
    }
    अगर((canon_मूल्य&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        अगर(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_गिनती);
            set.add(syllable, syllable+Hangul::JAMO_VT_गिनती-1);
        } अन्यथा {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    वापस true;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(स्थिर UDataSwapper *ds,
            स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
            UErrorCode *pErrorCode) {
    स्थिर UDataInfo *pInfo;
    int32_t header_आकार;

    स्थिर uint8_t *inBytes;
    uint8_t *outBytes;

    स्थिर int32_t *inIndexes;
    int32_t indexes[Normalizer2Impl::IX_योग_आकार+1];

    int32_t i, offset, अग्र_Offset, size;

    /* udata_swapDataHeader checks the arguments */
    header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* check data format and format version */
    pInfo=(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    अगर(!(
        pInfo->तथ्य_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->तथ्य_Format[1]==0x72 &&
        pInfo->तथ्य_Format[2]==0x6d &&
        pInfo->तथ्य_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                         pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    inBytes=(स्थिर uint8_t *)in_तथ्य+header_आकार;
    outBytes=(out_तथ्य == nullptr) ? nullptr : (uint8_t *)out_तथ्य+header_आकार;

    inIndexes=(स्थिर int32_t *)inBytes;
    int32_t min_Indexes_लंबाई;
    अगर(formatVersion0==1) {
        min_Indexes_लंबाई=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } अन्यथा अगर(formatVersion0==2) {
        min_Indexes_लंबाई=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } अन्यथा अगर(formatVersion0<=4) {
        min_Indexes_लंबाई=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } अन्यथा {
        min_Indexes_लंबाई=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    अगर(length>=0) {
        length-=header_आकार;
        अगर(length<min_Indexes_लंबाई*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
    }

    /* read the first few indexes */
    क्रम(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_योग_आकार];

    अगर(length>=0) {
        अगर(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }

        /* copy the data for inaccessible bytes */
        अगर(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset=0;

        /* swap the int32_t indexes[] */
        अग्र_Offset=indexes[Normalizer2Impl::IX_NORM_TRIE_OFFSET];
        ds->swapArray32(ds, inBytes, अग्र_Offset-offset, outBytes, pErrorCode);
        offset=अग्र_Offset;

        /* swap the trie */
        अग्र_Offset=indexes[Normalizer2Impl::IX_EXTRA_तथ्य_OFFSET];
        utrie_swapAnyVersion(ds, inBytes+offset, अग्र_Offset-offset, outBytes+offset, pErrorCode);
        offset=अग्र_Offset;

        /* swap the uint16_t extraData[] */
        अग्र_Offset=indexes[Normalizer2Impl::IX_SMALL_FCD_OFFSET];
        ds->swapArray16(ds, inBytes+offset, अग्र_Offset-offset, outBytes+offset, pErrorCode);
        offset=अग्र_Offset;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        अग्र_Offset=indexes[Normalizer2Impl::IX_SMALL_FCD_OFFSET+1];
        offset=अग्र_Offset;

        U_ASSERT(offset==size);
    }

    वापस header_आकार+size;
}

#पूर्ण_अगर  // !UCONFIG_NO_NORMALIZATION
