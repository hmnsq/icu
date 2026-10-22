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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/bytestream.h"
#شامل "unicode/edits.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/stringoptions.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/udata.h"
#شامل "unicode/umutablecptrie.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "unicode/utf8.h"
#شامل "bytesinkutil.h"
#شامل "cmemory.h"
#شامل "mutex.h"
#شامل "normalizer2impl.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل "ucptrie_impl.h"
#شامل "uset_imp.h"
#شامل "uvector.h"

U_NAMESPACE_BEGIN

فضاینام {

/**
 * UTF-8 lead byte for minNoMaybeCP.
 * Can be lower than the actual lead byte for c.
 * Typically U+0300 for NFC/NFD, U+00A0 for NFKC/NFKD, U+0041 for NFKC_Casefold.
 */
درونخطی uint8_t leadByteForCP(UChar32 c) {
    اگر (c <= 0x7f) {
        بازگشت static_cast<uint8_t>(c);
    } وگرنه اگر (c <= 0x7ff) {
        بازگشت static_cast<uint8_t>(0xc0 + (c >> 6));
    } وگرنه {
        // Should not occur because ccc(U+0300)!=0.
        بازگشت 0xe0;
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
    انتخاب(cpLimit-cpStart) {
    حالت 1:
        بازگشت c;
    حالت 2:
        بازگشت ((c&0x1f)<<6) | (cpStart[1]&0x3f);
    حالت 3:
        // no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (char16_t)
        بازگشت static_cast<char16_t>((c << 12) | ((cpStart[1] & 0x3f) << 6) | (cpStart[2] & 0x3f));
    حالت 4:
        بازگشت ((c&7)<<18) | ((cpStart[1]&0x3f)<<12) | ((cpStart[2]&0x3f)<<6) | (cpStart[3]&0x3f);
    پیشفرض:
        UPRV_UNREACHABLE_EXIT;  // Should not occur.
    }
}

/**
 * Returns the last code point in [start, p[ if it is valid and in U+1000..U+D7FF.
 * Otherwise returns a negative value.
 */
UChar32 previousHangulOrJamo(ثابت uint8_t *start, ثابت uint8_t *p) {
    اگر ((p - start) >= 3) {
        p -= 3;
        uint8_t l = *p;
        uint8_t t1, t2;
        اگر (0xe1 <= l && l <= 0xed &&
                (t1 = static_cast<uint8_t>(p[1] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(p[2] - 0x80)) <= 0x3f &&
                (l < 0xed || t1 <= 0x1f)) {
            بازگشت ((l & 0xf) << 12) | (t1 << 6) | t2;
        }
    }
    بازگشت U_SENTINEL;
}

/**
 * Returns the offset from the Jamo T base if [src, limit[ starts with a single Jamo T code point.
 * Otherwise returns a negative value.
 */
int32_t getJamoTMinusBase(ثابت uint8_t *src, ثابت uint8_t *limit) {
    // Jamo T: E1 86 A8..E1 87 82
    اگر ((limit - src) >= 3 && *src == 0xe1) {
        اگر (src[1] == 0x86) {
            uint8_t t = src[2];
            // The first Jamo T is U+11A8 but JAMO_T_BASE is 11A7.
            // Offset 0 does not correspond to any conjoining Jamo.
            اگر (0xa8 <= t && t <= 0xbf) {
                بازگشت t - 0xa7;
            }
        } وگرنه اگر (src[1] == 0x87) {
            uint8_t t = src[2];
            اگر (static_cast<int8_t>(t) <= static_cast<int8_t>(0x82u)) {
                بازگشت t - (0xa7 - 0x40);
            }
        }
    }
    بازگشت -1;
}

تهی
appendCodePointDelta(ثابت uint8_t *cpStart, ثابت uint8_t *cpLimit, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    نویسه buffer[U8_MAX_LENGTH];
    int32_t length;
    int32_t cpLength = static_cast<int32_t>(cpLimit - cpStart);
    اگر (cpLength == 1) {
        // The builder makes ASCII map to ASCII.
        buffer[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } وگرنه {
        int32_t trail = *(cpLimit-1) + delta;
        اگر (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cpLimit;
            length = 0;
            انجامبده { buffer[length++] = *cpStart++; } وقتی (cpStart < cpLimit);
            buffer[length++] = static_cast<uint8_t>(trail);
        } وگرنه {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = codePointFromValidUTF8(cpStart, cpLimit) + delta;
            length = 0;
            U8_APPEND_UNSAFE(buffer, length, c);
        }
    }
    اگر (edits != nullptr) {
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
    اگر (start == nullptr && U_SUCCESS(errorCode)) {
        // getBuffer() already did str.setToBogus()
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}

UBool ReorderingBuffer::init(int32_t destCapacity, UErrorCode &errorCode) {
    int32_t length=str.length();
    start=str.getBuffer(destCapacity);
    اگر(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت نادرست;
    }
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    reorderStart=start;
    اگر(start==limit) {
        lastCC=0;
    } وگرنه {
        setIterator();
        lastCC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        اگر(lastCC>1) {
            وقتی(previousCC()>1) {}
        }
        reorderStart=codePointLimit;
    }
    بازگشت درست;
}

UBool ReorderingBuffer::equals(ثابت char16_t *otherStart, ثابت char16_t *otherLimit) ثابت {
    int32_t length = static_cast<int32_t>(limit - start);
    بازگشت
        length == static_cast<int32_t>(otherLimit - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool ReorderingBuffer::equals(ثابت uint8_t *otherStart, ثابت uint8_t *otherLimit) ثابت {
    U_ASSERT((otherLimit - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(limit - start);
    int32_t otherLength = static_cast<int32_t>(otherLimit - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    اگر (otherLength < length || (otherLength / 3) > length) {
        بازگشت نادرست;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    برای (int32_t i = 0, j = 0;;) {
        اگر (i >= length) {
            بازگشت j >= otherLength;
        } وگرنه اگر (j >= otherLength) {
            بازگشت نادرست;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_NEXT_UNSAFE(start, i, c);
        U8_NEXT_UNSAFE(otherStart, j, other);
        اگر (c != other) {
            بازگشت نادرست;
        }
    }
}

UBool ReorderingBuffer::appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
    اگر(remainingCapacity<2 && !resize(2, errorCode)) {
        بازگشت نادرست;
    }
    اگر(lastCC<=cc || cc==0) {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
        lastCC=cc;
        اگر(cc<=1) {
            reorderStart=limit;
        }
    } وگرنه {
        insert(c, cc);
    }
    remainingCapacity-=2;
    بازگشت درست;
}

UBool ReorderingBuffer::append(ثابت char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UErrorCode &errorCode) {
    اگر(length==0) {
        بازگشت درست;
    }
    اگر(remainingCapacity<length && !resize(length, errorCode)) {
        بازگشت نادرست;
    }
    remainingCapacity-=length;
    اگر(lastCC<=leadCC || leadCC==0) {
        اگر(trailCC<=1) {
            reorderStart=limit+length;
        } وگرنه اگر(leadCC<=1) {
            reorderStart=limit+1;  // Ok if not a code point boundary.
        }
        ثابت char16_t *sLimit=s+length;
        انجامبده { *limit++=*s++; } وقتی(s!=sLimit);
        lastCC=trailCC;
    } وگرنه {
        int32_t i=0;
        UChar32 c;
        U16_NEXT(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        وقتی(i<length) {
            U16_NEXT(s, i, length, c);
            اگر(i<length) {
                اگر (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } وگرنه {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } وگرنه {
                leadCC=trailCC;
            }
            append(c, leadCC, errorCode);
        }
    }
    بازگشت درست;
}

UBool ReorderingBuffer::appendZeroCC(UChar32 c, UErrorCode &errorCode) {
    int32_t cpLength=U16_LENGTH(c);
    اگر(remainingCapacity<cpLength && !resize(cpLength, errorCode)) {
        بازگشت نادرست;
    }
    remainingCapacity-=cpLength;
    اگر(cpLength==1) {
        *limit++ = static_cast<char16_t>(c);
    } وگرنه {
        limit[0]=U16_LEAD(c);
        limit[1]=U16_TRAIL(c);
        limit+=2;
    }
    lastCC=0;
    reorderStart=limit;
    بازگشت درست;
}

UBool ReorderingBuffer::appendZeroCC(ثابت char16_t *s, ثابت char16_t *sLimit, UErrorCode &errorCode) {
    اگر(s==sLimit) {
        بازگشت درست;
    }
    int32_t length = static_cast<int32_t>(sLimit - s);
    اگر(remainingCapacity<length && !resize(length, errorCode)) {
        بازگشت نادرست;
    }
    u_memcpy(limit, s, length);
    limit+=length;
    remainingCapacity-=length;
    lastCC=0;
    reorderStart=limit;
    بازگشت درست;
}

تهی ReorderingBuffer::remove() {
    reorderStart=limit=start;
    remainingCapacity=str.getCapacity();
    lastCC=0;
}

تهی ReorderingBuffer::removeSuffix(int32_t suffixLength) {
    اگر(suffixLength<(limit-start)) {
        limit-=suffixLength;
        remainingCapacity+=suffixLength;
    } وگرنه {
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
    اگر(newCapacity<doubleCapacity) {
        newCapacity=doubleCapacity;
    }
    اگر(newCapacity<256) {
        newCapacity=256;
    }
    start=str.getBuffer(newCapacity);
    اگر(start==nullptr) {
        // getBuffer() already did str.setToBogus()
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت نادرست;
    }
    reorderStart=start+reorderStartIndex;
    limit=start+length;
    remainingCapacity=str.getCapacity()-length;
    بازگشت درست;
}

تهی ReorderingBuffer::skipPrevious() {
    codePointLimit=codePointStart;
    char16_t c=*--codePointStart;
    اگر(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(*(codePointStart-1))) {
        --codePointStart;
    }
}

uint8_t ReorderingBuffer::previousCC() {
    codePointLimit=codePointStart;
    اگر(reorderStart>=codePointStart) {
        بازگشت 0;
    }
    UChar32 c=*--codePointStart;
    char16_t c2;
    اگر(U16_IS_TRAIL(c) && start<codePointStart && U16_IS_LEAD(c2=*(codePointStart-1))) {
        --codePointStart;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    بازگشت impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
تهی ReorderingBuffer::insert(UChar32 c, uint8_t cc) {
    برای(setIterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=limit;
    char16_t *r=limit+=U16_LENGTH(c);
    انجامبده {
        *--r=*--q;
    } وقتی(codePointLimit!=q);
    writeCodePoint(q, c);
    اگر(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

ساختار CanonIterData : عمومی UMemory {
    CanonIterData(UErrorCode &errorCode);
    ~CanonIterData();
    تهی addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    delete fCanonIterData;
}

تهی
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

ایستا uint32_t U_CALLCONV
segmentStarterMapper(ثابت تهی * /*context*/, uint32_t value) {
    بازگشت value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_END

تهی
Normalizer2Impl::addLcccChars(UnicodeSet &set) ثابت {
    UChar32 start = 0, end;
    uint32_t norm16;
    وقتی ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &norm16)) >= 0) {
        اگر (norm16 > Normalizer2Impl::MIN_NORMAL_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            set.add(start, end);
        } وگرنه اگر (minNoNoCompNoMaybeCC <= norm16 && norm16 < limitNoNo) {
            uint16_t fcd16 = getFCD16(start);
            اگر (fcd16 > 0xff) { set.add(start, end); }
        }
        start = end + 1;
    }
}

تهی
Normalizer2Impl::addPropertyStarts(ثابت USetAdder *sa, UErrorCode & /*errorCode*/) ثابت {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, end;
    uint32_t value;
    وقتی ((end = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   nullptr, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        اگر (start != end && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_MASK) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t prevFCD16 = getFCD16(start);
            وقتی (++start <= end) {
                uint16_t fcd16 = getFCD16(start);
                اگر (fcd16 != prevFCD16) {
                    sa->add(sa->set, start);
                    prevFCD16 = fcd16;
                }
            }
        }
        start = end + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    برای(char16_t c=Hangul::HANGUL_BASE; c<Hangul::HANGUL_LIMIT; c+=Hangul::JAMO_T_شمار) {
        sa->add(sa->set, c);
        sa->add(sa->set, c+1);
    }
    sa->add(sa->set, Hangul::HANGUL_LIMIT); /* add Hangul+1 to continue with other properties */
}

تهی
Normalizer2Impl::addCanonIterPropertyStarts(ثابت USetAdder *sa, UErrorCode &errorCode) ثابت {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    اگر (!ensureCanonIterData(errorCode)) { بازگشت; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, end;
    uint32_t value;
    وقتی ((end = ucptrie_getRange(fCanonIterData->trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   segmentStarterMapper, nullptr, &value)) >= 0) {
        sa->add(sa->set, start);
        start = end + 1;
    }
}

ثابت char16_t *
Normalizer2Impl::copy_کم_Prefix_From_Nul_Terminated(ثابت char16_t *src,
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
    وقتی((c=*src++)<minNeedDataCP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    اگر(--src!=prevSrc) {
        اگر(buffer!=nullptr) {
            buffer->appendZeroCC(prevSrc, src, errorCode);
        }
    }
    بازگشت src;
}

UnicodeString &
Normalizer2Impl::decompose(ثابت UnicodeString &src, UnicodeString &dest,
                           UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        dest.setToBogus();
        بازگشت dest;
    }
    ثابت char16_t *sArray=src.getBuffer();
    اگر(&dest==&src || sArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        بازگشت dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), errorCode);
    بازگشت dest;
}

تهی
Normalizer2Impl::decompose(ثابت char16_t *src, ثابت char16_t *limit,
                           UnicodeString &dest,
                           int32_t destLengthEstimate,
                           UErrorCode &errorCode) ثابت {
    اگر(destLengthEstimate<0 && limit!=nullptr) {
        destLengthEstimate = static_cast<int32_t>(limit - src);
    }
    dest.remove();
    ReorderingBuffer buffer(*این, dest);
    اگر(buffer.init(destLengthEstimate, errorCode)) {
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
    اگر(src==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت src;
    }
    UChar32 minNoCP=minDecompNoCP;
    اگر(limit==nullptr) {
        src=copy_کم_Prefix_From_Nul_Terminated(src, minNoCP, buffer, errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت src;
        }
        limit=u_strchr(src, 0);
    }

    ثابت char16_t *prevSrc;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ثابت char16_t *prevBoundary=src;
    uint8_t prevCC=0;

    برای(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        برای(prevSrc=src; src!=limit;) {
            اگر( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } وگرنه اگر(!U16_IS_LEAD(c)) {
                بشکن;
            } وگرنه {
                char16_t c2;
                اگر((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                    اگر(isMostDecompYesAndZeroCC(norm16)) {
                        src+=2;
                    } وگرنه {
                        بشکن;
                    }
                } وگرنه {
                    ++src;  // unpaired lead surrogate: inert
                }
            }
        }
        // copy these code units all at once
        اگر(src!=prevSrc) {
            اگر(buffer!=nullptr) {
                اگر(!buffer->appendZeroCC(prevSrc, src, errorCode)) {
                    بشکن;
                }
            } وگرنه {
                prevCC=0;
                prevBoundary=src;
            }
        }
        اگر(src==limit) {
            بشکن;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_LENGTH(c);
        اگر(buffer!=nullptr) {
            اگر(!decompose(c, norm16, *buffer, errorCode)) {
                بشکن;
            }
        } وگرنه {
            اگر(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                اگر(prevCC<=cc || cc==0) {
                    prevCC=cc;
                    اگر(cc<=1) {
                        prevBoundary=src;
                    }
                    ادامه;
                }
            }
            بازگشت prevBoundary;  // "no" or cc out of order
        }
    }
    بازگشت src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ثابت char16_t *
Normalizer2Impl::decomposeShort(ثابت char16_t *src, ثابت char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    وقتی(src<limit) {
        اگر (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            بازگشت src;
        }
        ثابت char16_t *prevSrc = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
        اگر (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            بازگشت prevSrc;
        }
        اگر(!decompose(c, norm16, buffer, errorCode)) {
            بازگشت nullptr;
        }
        اگر (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            بازگشت src;
        }
    }
    بازگشت src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 ReorderingBuffer &buffer,
                                 UErrorCode &errorCode) ثابت {
    // get the decomposition and the lead and trail cc's
    اگر (norm16 >= limitNoNo) {
        اگر (isMaybeYesOrNonZeroCC(norm16)) {
            بازگشت buffer.append(c, getCCFromYesOrMaybeYes(norm16), errorCode);
        } وگرنه اگر (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    اگر (norm16 < minYesNo) {
        // c does not decompose
        بازگشت buffer.append(c, 0, errorCode);
    } وگرنه اگر(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        بازگشت buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode);
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_LENGTH_MASK;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    اگر(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } وگرنه {
        leadCC=0;
    }
    بازگشت buffer.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, درست, leadCC, trailCC, errorCode);
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

    برای (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ثابت uint8_t *fastStart = src;
        ثابت uint8_t *prevSrc;
        uint16_t norm16 = 0;

        برای (;;) {
            اگر (src == limit) {
                اگر (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                بازگشت src;
            }
            اگر (*src < minNoLead) {
                ++src;
            } وگرنه {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                اگر (!isMostDecompYesAndZeroCC(norm16)) {
                    بشکن;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        اگر (prevSrc != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            اگر (sink != nullptr &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                بشکن;
            }
            prevBoundary = prevSrc;
            prevCC = 0;
        }

        // Medium-fast path: Quick check.
        اگر (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            اگر (prevCC <= cc || cc == 0) {
                prevCC = cc;
                اگر (cc <= 1) {
                    اگر (sink != nullptr &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                                           *sink, options, edits, errorCode)) {
                        بشکن;
                    }
                    prevBoundary = src;
                }
                ادامه;
            }
        }
        اگر (sink == nullptr) {
            بازگشت prevBoundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        اگر (prevBoundary != prevSrc && norm16HasDecompBoundaryBefore(norm16)) {
            اگر (!ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                               *sink, options, edits, errorCode)) {
                بشکن;
            }
            prevBoundary = prevSrc;
        }
        ReorderingBuffer buffer(*این, s16, errorCode);
        اگر (U_FAILURE(errorCode)) {
            بشکن;
        }
        decomposeShort(prevBoundary, src, STOP_AT_LIMIT, نادرست /* onlyContiguous */,
                       buffer, errorCode);
        // Decompose until the next boundary.
        اگر (buffer.getLastCC() > 1) {
            src = decomposeShort(src, limit, STOP_AT_DECOMP_BOUNDARY, نادرست /* onlyContiguous */,
                                 buffer, errorCode);
        }
        اگر (U_FAILURE(errorCode)) {
            بشکن;
        }
        اگر ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            بشکن;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        اگر (isMaybeYesOrNonZeroCC(norm16) && buffer.equals(prevBoundary, src)) {
            اگر (!ByteSinkUtil::appendUnchanged(prevBoundary, src,
                                               *sink, options, edits, errorCode)) {
                بشکن;
            }
        } وگرنه {
            اگر (!ByteSinkUtil::appendChange(prevBoundary, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                بشکن;
            }
        }
        prevBoundary = src;
        prevCC = 0;
    }
    بازگشت src;
}

ثابت uint8_t *
Normalizer2Impl::decomposeShort(ثابت uint8_t *src, ثابت uint8_t *limit,
                                StopAt stopAt, UBool onlyContiguous,
                                ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    وقتی (src < limit) {
        ثابت uint8_t *prevSrc = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        اگر (norm16 >= limitNoNo) {
            اگر (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                اگر (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    بازگشت prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                اگر (!buffer.append(c, cc, errorCode)) {
                    بازگشت nullptr;
                }
                اگر (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1) {
                    بازگشت src;
                }
                ادامه;
            } وگرنه اگر (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                اگر (stopAt != STOP_AT_LIMIT) {
                    بازگشت prevSrc;
                }
                c = codePointFromValidUTF8(prevSrc, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } وگرنه اگر (stopAt != STOP_AT_LIMIT && norm16 < minNoNoCompNoMaybeCC) {
            بازگشت prevSrc;
        }
        // norm16!=INERT guarantees that [prevSrc, src[ is valid UTF-8.
        // We do not see invalid UTF-8 here because
        // its norm16==INERT is normalization-inert,
        // so it gets copied unchanged in the fast path,
        // and we stop the slow path where invalid UTF-8 begins.
        // c >= 0 is the result of an algorithmic mapping.
        U_ASSERT(c >= 0 || norm16 != INERT);
        اگر (norm16 < minYesNo) {
            اگر (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            // does not decompose
            اگر (!buffer.append(c, 0, errorCode)) {
                بازگشت nullptr;
            }
        } وگرنه اگر (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            اگر (c < 0) {
                c = codePointFromValidUTF8(prevSrc, src);
            }
            char16_t jamos[3];
            اگر (!buffer.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), errorCode)) {
                بازگشت nullptr;
            }
        } وگرنه {
            // The character decomposes, get everything from the variable-length extra data.
            ثابت uint16_t *mapping = getData(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_LENGTH_MASK;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            اگر (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } وگرنه {
                leadCC = 0;
            }
            اگر (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                بازگشت prevSrc;
            }
            اگر (!buffer.append(reinterpret_cast<ثابت char16_t*>(mapping) + 1, length, درست, leadCC, trailCC, errorCode)) {
                بازگشت nullptr;
            }
        }
        اگر ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && buffer.getLastCC() <= 1)) {
            بازگشت src;
        }
    }
    بازگشت src;
}

ثابت char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t buffer[4], int32_t &length) ثابت {
    uint16_t norm16;
    اگر(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        بازگشت nullptr;
    }
    ثابت char16_t *decomp = nullptr;
    اگر(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=buffer;
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    اگر (norm16 < minYesNo) {
        بازگشت decomp;
    } وگرنه اگر(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, buffer);
        بازگشت buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    length=*mapping&MAPPING_LENGTH_MASK;
    بازگشت reinterpret_cast<ثابت char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ثابت char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t buffer[30], int32_t &length) ثابت {
    uint16_t norm16;
    اگر(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        بازگشت nullptr;
    } وگرنه اگر(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, buffer);
        length=2;
        بازگشت buffer;
    } وگرنه اگر(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(buffer, length, c);
        بازگشت buffer;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    int32_t mLength=firstUnit&MAPPING_LENGTH_MASK;  // length of normal mapping
    اگر(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ثابت uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        اگر(rm0<=MAPPING_LENGTH_MASK) {
            length=rm0;
            بازگشت reinterpret_cast<ثابت char16_t*>(rawMapping) - rm0;
        } وگرنه {
            // Copy the normal mapping and replace its first two code units with rm0.
            buffer[0] = static_cast<char16_t>(rm0);
            u_memcpy(buffer + 1, reinterpret_cast<ثابت char16_t*>(mapping) + 1 + 2, mLength - 2);
            length=mLength-1;
            بازگشت buffer;
        }
    } وگرنه {
        length=mLength;
        بازگشت reinterpret_cast<ثابت char16_t*>(mapping) + 1;
    }
}

تهی Normalizer2Impl::decomposeAndAppend(ثابت char16_t *src, ثابت char16_t *limit,
                                         UBool doDecompose,
                                         UnicodeString &safe_میانه,
                                         ReorderingBuffer &buffer,
                                         UErrorCode &errorCode) ثابت {
    buffer.copyReorderableSuffixTo(safe_میانه);
    اگر(doDecompose) {
        decompose(src, limit, &buffer, errorCode);
        بازگشت;
    }
    // Just merge the strings at the boundary.
    منطقی isFirst = درست;
    uint8_t firstCC = 0, prevCC = 0, cc;
    ثابت char16_t *p = src;
    وقتی (p != limit) {
        ثابت char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        اگر ((cc = getCC(norm16)) == 0) {
            p = codePointStart;
            بشکن;
        }
        اگر (isFirst) {
            firstCC = cc;
            isFirst = نادرست;
        }
        prevCC = cc;
    }
    اگر(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
        limit=u_strchr(p, 0);
    }

    اگر (buffer.append(src, static_cast<int32_t>(p - src), نادرست, firstCC, prevCC, errorCode)) {
        buffer.appendZeroCC(p, limit, errorCode);
    }
}

UBool Normalizer2Impl::hasDecompBoundaryBefore(UChar32 c) ثابت {
    بازگشت c < minLcccCP || (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) ||
        norm16HasDecompBoundaryBefore(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryBefore(uint16_t norm16) ثابت {
    اگر (norm16 < minNoNoCompNoMaybeCC) {
        بازگشت درست;
    }
    اگر (norm16 >= limitNoNo) {
        بازگشت norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getDataForYesOrNo(norm16);
    uint16_t firstUnit=*mapping;
    // true if leadCC==0 (hasFCDBoundaryBefore())
    بازگشت (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
}

UBool Normalizer2Impl::hasDecompBoundaryAfter(UChar32 c) ثابت {
    اگر (c < minDecompNoCP) {
        بازگشت درست;
    }
    اگر (c <= 0xffff && !singleLeadMightHaveNonZeroFCD16(c)) {
        بازگشت درست;
    }
    بازگشت norm16HasDecompBoundaryAfter(getNorm16(c));
}

UBool Normalizer2Impl::norm16HasDecompBoundaryAfter(uint16_t norm16) ثابت {
    اگر(norm16 <= minYesNo || isHangulLVT(norm16)) {
        بازگشت درست;
    }
    اگر (norm16 >= limitNoNo) {
        اگر (isMaybeYesOrNonZeroCC(norm16)) {
            بازگشت norm16 <= MIN_NORMAL_MAYBE_YES || norm16 == JAMO_VT;
        } وگرنه اگر (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            بازگشت (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    // decomp after-boundary: same as hasFCDBoundaryAfter(),
    // fcd16<=1 || trailCC==0
    اگر(firstUnit>0x1ff) {
        بازگشت نادرست;  // trailCC>1
    }
    اگر(firstUnit<=0xff) {
        بازگشت درست;  // trailCC==0
    }
    // if(trailCC==1) test leadCC==0, same as checking for before-boundary
    // true if leadCC==0 (hasFCDBoundaryBefore())
    بازگشت (firstUnit&MAPPING_HAS_CCC_LCCC_WORD)==0 || (*(mapping-1)&0xff00)==0;
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
    اگر(trail<COMP_1_TRAIL_LIMIT) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        وقتی(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        اگر(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
            اگر(firstUnit&COMP_1_TRIPLE) {
                بازگشت (static_cast<int32_t>(list[1]) << 16) | list[2];
            } وگرنه {
                بازگشت list[1];
            }
        }
    } وگرنه {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_LIMIT +
                        (((trail>>COMP_1_TRAIL_SHIFT))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_SHIFT);
        uint16_t secondUnit;
        برای(;;) {
            اگر(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } وگرنه اگر(key1==(firstUnit&COMP_1_TRAIL_MASK)) {
                اگر(key2>(secondUnit=list[1])) {
                    اگر(firstUnit&COMP_1_LAST_TUPLE) {
                        بشکن;
                    } وگرنه {
                        list+=3;
                    }
                } وگرنه اگر(key2==(secondUnit&COMP_2_TRAIL_MASK)) {
                    بازگشت (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_MASK) << 16) | list[2];
                } وگرنه {
                    بشکن;
                }
            } وگرنه {
                بشکن;
            }
        }
    }
    بازگشت -1;
}

/**
  * @param list some character's compositions list
  * @param set recursively receives the composites from these compositions
  */
تهی Normalizer2Impl::addComposites(ثابت uint16_t *list, UnicodeSet &set) ثابت {
    uint16_t firstUnit;
    int32_t compositeAndFwd;
    انجامبده {
        firstUnit=*list;
        اگر((firstUnit&COMP_1_TRIPLE)==0) {
            compositeAndFwd=list[1];
            list+=2;
        } وگرنه {
            compositeAndFwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_MASK) << 16) | list[2];
            list+=3;
        }
        UChar32 composite=compositeAndFwd>>1;
        اگر((compositeAndFwd&1)!=0) {
            addComposites(getCompositionsListForComposite(getRawNorm16(composite)), set);
        }
        set.add(composite);
    } وقتی((firstUnit&COMP_1_LAST_TUPLE)==0);
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
تهی Normalizer2Impl::recompose(ReorderingBuffer &buffer, int32_t recomposeStartIndex,
                                UBool onlyContiguous) ثابت {
    char16_t *p=buffer.getStart()+recomposeStartIndex;
    char16_t *limit=buffer.getLimit();
    اگر(p==limit) {
        بازگشت;
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
    starterIsSupplementary=نادرست;
    prevCC=0;

    برای(;;) {
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        اگر( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=nullptr &&
            // the backward-combining character is not blocked
            (prevCC<cc || prevCC==0)
        ) {
            اگر(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                اگر(c<Hangul::JAMO_T_BASE) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t prev = static_cast<char16_t>(*starter - Hangul::JAMO_L_BASE);
                    اگر(prev<Hangul::JAMO_L_شمار) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_BASE +
                             (prev*Hangul::JAMO_V_شمار+(c-Hangul::JAMO_V_BASE))*
                             Hangul::JAMO_T_شمار);
                        char16_t t;
                        اگر (p != limit && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_BASE)) < Hangul::JAMO_T_شمار) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        وقتی(r<limit) {
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
                اگر(p==limit) {
                    بشکن;
                }
                compositionsList=nullptr;
                ادامه;
            } وگرنه اگر((compositeAndFwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 composite=compositeAndFwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_LENGTH(c);  // pRemove & p: start & limit of the combining mark
                اگر(starterIsSupplementary) {
                    اگر(U_IS_SUPPLEMENTARY(composite)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(composite);
                        starter[1]=U16_TRAIL(composite);
                    } وگرنه {
                        *starter = static_cast<char16_t>(composite);
                        // The composite is shorter than the starter,
                        // move the intermediate characters forward one.
                        starterIsSupplementary=نادرست;
                        q=starter+1;
                        r=q+1;
                        وقتی(r<pRemove) {
                            *q++=*r++;
                        }
                        --pRemove;
                    }
                } وگرنه اگر(U_IS_SUPPLEMENTARY(composite)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=درست;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    وقتی(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(composite);
                    *--starter=U16_LEAD(composite);  // undo the temporary increment
                } وگرنه {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(composite);
                }

                /* remove the combining mark by moving the following text over it */
                اگر(pRemove<p) {
                    q=pRemove;
                    r=p;
                    وقتی(r<limit) {
                        *q++=*r++;
                    }
                    limit=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                اگر(p==limit) {
                    بشکن;
                }
                // Is the composite a starter that combines forward?
                اگر(compositeAndFwd&1) {
                    compositionsList=
                        getCompositionsListForComposite(getRawNorm16(composite));
                } وگرنه {
                    compositionsList=nullptr;
                }

                // We combined; continue with looking for compositions.
                ادامه;
            }
        }

        // no combination this time
        prevCC=cc;
        اگر(p==limit) {
            بشکن;
        }

        // If c did not combine, then check if it is a starter.
        اگر(cc==0) {
            // Found a new starter.
            اگر((compositionsList=getCompositionsListForDecompYes(norm16))!=nullptr) {
                // It may combine with something, prepare for it.
                اگر(U_IS_BMP(c)) {
                    starterIsSupplementary=نادرست;
                    starter=p-1;
                } وگرنه {
                    starterIsSupplementary=درست;
                    starter=p-2;
                }
            }
        } وگرنه اگر(onlyContiguous) {
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
    اگر(isInert(norm16)) {
        بازگشت U_SENTINEL;
    } وگرنه اگر(norm16<minYesNoMappingsOnly) {
        // a combines forward.
        اگر(isJamoL(norm16)) {
            اگر (b < Hangul::JAMO_V_BASE) {
                بازگشت U_SENTINEL;
            }
            b-=Hangul::JAMO_V_BASE;
            اگر(b<Hangul::JAMO_V_شمار) {
                بازگشت
                    (Hangul::HANGUL_BASE+
                     ((a-Hangul::JAMO_L_BASE)*Hangul::JAMO_V_شمار+b)*
                     Hangul::JAMO_T_شمار);
            } وگرنه {
                بازگشت U_SENTINEL;
            }
        } وگرنه اگر(isHangulLV(norm16)) {
            اگر (b <= Hangul::JAMO_T_BASE) {
               بازگشت U_SENTINEL;
            }
            b-=Hangul::JAMO_T_BASE;
            اگر(b<Hangul::JAMO_T_شمار) {  // not b==0!
                بازگشت a+b;
            } وگرنه {
                بازگشت U_SENTINEL;
            }
        } وگرنه {
            // 'a' has a compositions list in extraData
            list=getDataForYesOrNo(norm16);
            اگر(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_LENGTH_MASK);  // + mapping length
            }
        }
    } وگرنه اگر(norm16<minMaybeNoCombinesFwd || MIN_NORMAL_MAYBE_YES<=norm16) {
        بازگشت U_SENTINEL;
    } وگرنه {
        list=getDataForMaybe(norm16);
        اگر(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_LENGTH_MASK);  // + mapping length
        }
    }
    اگر(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        بازگشت U_SENTINEL;
    }
#اگر U_SIGNED_راست_SHIFT_IS_ARITHMETIC
    بازگشت combine(list, b)>>1;
#وگرنه
    int32_t compositeAndFwd=combine(list, b);
    بازگشت compositeAndFwd>=0 ? compositeAndFwd>>1 : U_SENTINEL;
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
    اگر(limit==nullptr) {
        src=copy_کم_Prefix_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &buffer : nullptr,
                                           errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت نادرست;
        }
        limit=u_strchr(src, 0);
        اگر (prevBoundary != src) {
            اگر (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } وگرنه {
                buffer.removeSuffix(1);
                prevBoundary = --src;
            }
        }
    }

    برای (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        برای (;;) {
            اگر (src == limit) {
                اگر (prevBoundary != limit && doCompose) {
                    buffer.appendZeroCC(prevBoundary, limit, errorCode);
                }
                بازگشت درست;
            }
            اگر( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } وگرنه {
                prevSrc = src++;
                اگر(!U16_IS_LEAD(c)) {
                    بشکن;
                } وگرنه {
                    char16_t c2;
                    اگر(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        اگر(!isCompYesAndZeroCC(norm16)) {
                            بشکن;
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
        اگر (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            اگر (!doCompose) {
                بازگشت نادرست;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            اگر (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    اگر (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        بشکن;
                    }
                    اگر(!buffer.append(mapAlgorithmic(c, norm16), 0, errorCode)) {
                        بشکن;
                    }
                    prevBoundary = src;
                    ادامه;
                }
            } وگرنه اگر (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    اگر (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        بشکن;
                    }
                    ثابت char16_t *mapping = reinterpret_cast<ثابت char16_t *>(getDataForYesOrNo(norm16));
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    اگر(!buffer.appendZeroCC(mapping, mapping + length, errorCode)) {
                        بشکن;
                    }
                    prevBoundary = src;
                    ادامه;
                }
            } وگرنه اگر (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                اگر (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    اگر (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                        بشکن;
                    }
                    prevBoundary = src;
                    ادامه;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } وگرنه اگر (isJamoVT(norm16) && prevBoundary != prevSrc) {
            char16_t prev=*(prevSrc-1);
            اگر(c<Hangul::JAMO_T_BASE) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(prev - Hangul::JAMO_L_BASE);
                اگر(l<Hangul::JAMO_L_شمار) {
                    اگر (!doCompose) {
                        بازگشت نادرست;
                    }
                    int32_t t;
                    اگر (src != limit &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_BASE)) &&
                            t < Hangul::JAMO_T_شمار) {
                        // The next character is a Jamo T.
                        ++src;
                    } وگرنه اگر (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } وگرنه {
                        t = -1;
                    }
                    اگر (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_شمار + (c-Hangul::JAMO_V_BASE)) *
                            Hangul::JAMO_T_شمار + t;
                        --prevSrc;  // Replace the Jamo L as well.
                        اگر (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                            بشکن;
                        }
                        اگر (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                            بشکن;
                        }
                        prevBoundary = src;
                        ادامه;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } وگرنه اگر (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                اگر (!doCompose) {
                    بازگشت نادرست;
                }
                UChar32 syllable = prev + c - Hangul::JAMO_T_BASE;
                --prevSrc;  // Replace the Hangul LV as well.
                اگر (prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
                    بشکن;
                }
                اگر (!buffer.appendBMP(static_cast<char16_t>(syllable), 0, errorCode)) {
                    بشکن;
                }
                prevBoundary = src;
                ادامه;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } وگرنه اگر (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            اگر (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                اگر (!doCompose) {
                    بازگشت نادرست;
                }
            } وگرنه {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *nextSrc;
                uint16_t n16;
                برای (;;) {
                    اگر (src == limit) {
                        اگر (doCompose) {
                            buffer.appendZeroCC(prevBoundary, limit, errorCode);
                        }
                        بازگشت درست;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, n16);
                    اگر (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        اگر (prevCC > cc) {
                            اگر (!doCompose) {
                                بازگشت نادرست;
                            }
                            بشکن;
                        }
                    } وگرنه {
                        بشکن;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                اگر (norm16HasCompBoundaryBefore(n16)) {
                    اگر (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    ادامه;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        اگر (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ثابت char16_t *p = prevSrc;
            UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, norm16);
            اگر (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        اگر (doCompose && prevBoundary != prevSrc && !buffer.appendZeroCC(prevBoundary, prevSrc, errorCode)) {
            بشکن;
        }
        int32_t recomposeStartIndex=buffer.length();
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, نادرست /* !stopAtCompBoundary */, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, درست /* stopAtCompBoundary */, onlyContiguous,
                             buffer, errorCode);
        اگر (U_FAILURE(errorCode)) {
            بشکن;
        }
        اگر ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت درست;
        }
        recompose(buffer, recomposeStartIndex, onlyContiguous);
        اگر(!doCompose) {
            اگر(!buffer.equals(prevSrc, src)) {
                بازگشت نادرست;
            }
            buffer.remove();
        }
        prevBoundary=src;
    }
    بازگشت درست;
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
    اگر(limit==nullptr) {
        UErrorCode errorCode=U_ZERO_ERROR;
        src=copy_کم_Prefix_From_Nul_Terminated(src, minNoMaybeCP, nullptr, errorCode);
        limit=u_strchr(src, 0);
        اگر (prevBoundary != src) {
            اگر (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                prevBoundary = src;
            } وگرنه {
                prevBoundary = --src;
            }
        }
    }

    برای(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت char16_t *prevSrc;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        برای (;;) {
            اگر(src==limit) {
                بازگشت src;
            }
            اگر( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } وگرنه {
                prevSrc = src++;
                اگر(!U16_IS_LEAD(c)) {
                    بشکن;
                } وگرنه {
                    char16_t c2;
                    اگر(src!=limit && U16_IS_TRAIL(c2=*src)) {
                        ++src;
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                        norm16=UCPTRIE_FAST_SUPP_GET(normTrie, UCPTRIE_16, c);
                        اگر(!isCompYesAndZeroCC(norm16)) {
                            بشکن;
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
        اگر (prevBoundary != prevSrc) {
            اگر (norm16HasCompBoundaryBefore(norm16)) {
                prevBoundary = prevSrc;
            } وگرنه {
                ثابت char16_t *p = prevSrc;
                uint16_t n16;
                UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, prevBoundary, p, c, n16);
                اگر (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    prevBoundary = prevSrc;
                } وگرنه {
                    prevBoundary = p;
                    prevNorm16 = n16;
                }
            }
        }

        اگر (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            اگر (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(prevNorm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } وگرنه {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت char16_t *nextSrc;
                برای (;;) {
                    اگر (norm16 < MIN_YES_YES_WITH_CC) {
                        اگر (pQCResult != nullptr) {
                            *pQCResult = UNORM_MAYBE;
                        } وگرنه {
                            بازگشت prevBoundary;
                        }
                    }
                    اگر (src == limit) {
                        بازگشت src;
                    }
                    uint8_t prevCC = fcd16;
                    nextSrc = src;
                    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, c, norm16);
                    اگر (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        اگر (!(prevCC <= cc || cc == 0)) {
                            بشکن;
                        }
                    } وگرنه {
                        بشکن;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                اگر (isCompYesAndZeroCC(norm16)) {
                    prevBoundary = src;
                    src = nextSrc;
                    ادامه;
                }
            }
        }
        اگر(pQCResult!=nullptr) {
            *pQCResult=UNORM_NO;
        }
        بازگشت prevBoundary;
    }
}

تهی Normalizer2Impl::composeAndAppend(ثابت char16_t *src, ثابت char16_t *limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_میانه,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ثابت {
    اگر(!buffer.isEmpty()) {
        ثابت char16_t *firstStarterInSrc=findNextCompBoundary(src, limit, onlyContiguous);
        اگر(src!=firstStarterInSrc) {
            ثابت char16_t *lastStarterInDest=findPreviousCompBoundary(buffer.getStart(),
                                                                    buffer.getLimit(), onlyContiguous);
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastStarterInDest);
            UnicodeString میانه(lastStarterInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_میانه=میانه;
            میانه.append(src, static_cast<int32_t>(firstStarterInSrc - src));
            ثابت char16_t *میانه_Start=میانه.getBuffer();
            compose(میانه_Start, میانه_Start+میانه.length(), onlyContiguous,
                    درست, buffer, errorCode);
            اگر(U_FAILURE(errorCode)) {
                بازگشت;
            }
            src=firstStarterInSrc;
        }
    }
    اگر(doCompose) {
        compose(src, limit, onlyContiguous, درست, buffer, errorCode);
    } وگرنه {
        اگر(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
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

    برای (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ثابت uint8_t *prevSrc;
        uint16_t norm16 = 0;
        برای (;;) {
            اگر (src == limit) {
                اگر (prevBoundary != limit && sink != nullptr) {
                    ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                  *sink, options, edits, errorCode);
                }
                بازگشت درست;
            }
            اگر (*src < minNoMaybeLead) {
                ++src;
            } وگرنه {
                prevSrc = src;
                UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
                اگر (!isCompYesAndZeroCC(norm16)) {
                    بشکن;
                }
            }
        }
        // isCompYesAndZeroCC(norm16) is false, that is, norm16>=minNoNo.
        // The current character is either a "noNo" (has a mapping)
        // or a "maybeYes" / "maybeNo" (combines backward)
        // or a "yesYes" with ccc!=0.
        // It is not a Hangul syllable or Jamo L because those have "yes" properties.

        // Medium-fast path: Handle cases that do not require full decomposition and recomposition.
        اگر (norm16 < minMaybeNo) {  // minNoNo <= norm16 < minMaybeNo
            اگر (sink == nullptr) {
                بازگشت نادرست;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            اگر (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    اگر (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        بشکن;
                    }
                    appendCodePointDelta(prevSrc, src, getAlgorithmicDelta(norm16), *sink, edits);
                    prevBoundary = src;
                    ادامه;
                }
            } وگرنه اگر (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, limit)) {
                    اگر (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        بشکن;
                    }
                    ثابت uint16_t *mapping = getDataForYesOrNo(norm16);
                    int32_t length = *mapping++ & MAPPING_LENGTH_MASK;
                    اگر (!ByteSinkUtil::appendChange(prevSrc, src, reinterpret_cast<ثابت char16_t*>(mapping), length,
                                                    *sink, edits, errorCode)) {
                        بشکن;
                    }
                    prevBoundary = src;
                    ادامه;
                }
            } وگرنه اگر (norm16 >= minNoNoEmpty) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                اگر (hasCompBoundaryBefore(src, limit) ||
                        hasCompBoundaryAfter(prevBoundary, prevSrc, onlyContiguous)) {
                    اگر (prevBoundary != prevSrc &&
                            !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                           *sink, options, edits, errorCode)) {
                        بشکن;
                    }
                    اگر (edits != nullptr) {
                        edits->addReplace(static_cast<int32_t>(src - prevSrc), 0);
                    }
                    prevBoundary = src;
                    ادامه;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } وگرنه اگر (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - prevSrc) == 3 && *prevSrc == 0xe1);
            UChar32 prev = previousHangulOrJamo(prevBoundary, prevSrc);
            اگر (prevSrc[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = prev - Hangul::JAMO_L_BASE;
                اگر (static_cast<uint32_t>(l) < Hangul::JAMO_L_شمار) {
                    اگر (sink == nullptr) {
                        بازگشت نادرست;
                    }
                    int32_t t = getJamoTMinusBase(src, limit);
                    اگر (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } وگرنه اگر (hasCompBoundaryBefore(src, limit)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    اگر (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_BASE +
                            (l*Hangul::JAMO_V_شمار + (prevSrc[2]-0xa1)) *
                            Hangul::JAMO_T_شمار + t;
                        prevSrc -= 3;  // Replace the Jamo L as well.
                        اگر (prevBoundary != prevSrc &&
                                !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                               *sink, options, edits, errorCode)) {
                            بشکن;
                        }
                        ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                        prevBoundary = src;
                        ادامه;
                    }
                    // If we see L+V+x where x!=T then we drop to the slow path,
                    // decompose and recompose.
                    // This is to deal with NFKC finding normal L and V but a
                    // compatibility variant of a T.
                    // We need to either fully compose that combination here
                    // (which would complicate the code and may not work with strange custom data)
                    // or use the slow path.
                }
            } وگرنه اگر (Hangul::isHangulLV(prev)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                اگر (sink == nullptr) {
                    بازگشت نادرست;
                }
                UChar32 syllable = prev + getJamoTMinusBase(prevSrc, src);
                prevSrc -= 3;  // Replace the Hangul LV as well.
                اگر (prevBoundary != prevSrc &&
                        !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                       *sink, options, edits, errorCode)) {
                    بشکن;
                }
                ByteSinkUtil::appendCodePoint(prevSrc, src, syllable, *sink, edits);
                prevBoundary = src;
                ادامه;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } وگرنه اگر (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = getCCFromNormalYesOrMaybe(norm16);  // cc!=0
            اگر (onlyContiguous /* FCC */ && getPreviousTrailCC(prevBoundary, prevSrc) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                اگر (sink == nullptr) {
                    بازگشت نادرست;
                }
            } وگرنه {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ثابت uint8_t *nextSrc;
                uint16_t n16;
                برای (;;) {
                    اگر (src == limit) {
                        اگر (sink != nullptr) {
                            ByteSinkUtil::appendUnchanged(prevBoundary, limit,
                                                          *sink, options, edits, errorCode);
                        }
                        بازگشت درست;
                    }
                    uint8_t prevCC = cc;
                    nextSrc = src;
                    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, nextSrc, limit, n16);
                    اگر (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = getCCFromNormalYesOrMaybe(n16);
                        اگر (prevCC > cc) {
                            اگر (sink == nullptr) {
                                بازگشت نادرست;
                            }
                            بشکن;
                        }
                    } وگرنه {
                        بشکن;
                    }
                    src = nextSrc;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                اگر (norm16HasCompBoundaryBefore(n16)) {
                    اگر (isCompYesAndZeroCC(n16)) {
                        src = nextSrc;
                    }
                    ادامه;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        اگر (prevBoundary != prevSrc && !norm16HasCompBoundaryBefore(norm16)) {
            ثابت uint8_t *p = prevSrc;
            UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, prevBoundary, p, norm16);
            اگر (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                prevSrc = p;
            }
        }
        ReorderingBuffer buffer(*این, s16, errorCode);
        اگر (U_FAILURE(errorCode)) {
            بشکن;
        }
        // We know there is not a boundary here.
        decomposeShort(prevSrc, src, STOP_AT_LIMIT, onlyContiguous,
                       buffer, errorCode);
        // Decompose until the next boundary.
        src = decomposeShort(src, limit, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             buffer, errorCode);
        اگر (U_FAILURE(errorCode)) {
            بشکن;
        }
        اگر ((src - prevSrc) > INT32_MAX) {  // guard before buffer.equals()
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت درست;
        }
        recompose(buffer, 0, onlyContiguous);
        اگر (!buffer.equals(prevSrc, src)) {
            اگر (sink == nullptr) {
                بازگشت نادرست;
            }
            اگر (prevBoundary != prevSrc &&
                    !ByteSinkUtil::appendUnchanged(prevBoundary, prevSrc,
                                                   *sink, options, edits, errorCode)) {
                بشکن;
            }
            اگر (!ByteSinkUtil::appendChange(prevSrc, src, buffer.getStart(), buffer.length(),
                                            *sink, edits, errorCode)) {
                بشکن;
            }
            prevBoundary = src;
        }
    }
    بازگشت درست;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت char16_t *src, ثابت char16_t *limit) ثابت {
    اگر (src == limit || *src < minCompNoMaybeCP) {
        بازگشت درست;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, src, limit, c, norm16);
    بازگشت norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ثابت uint8_t *src, ثابت uint8_t *limit) ثابت {
    اگر (src == limit) {
        بازگشت درست;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_NEXT(normTrie, UCPTRIE_16, src, limit, norm16);
    بازگشت norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت char16_t *start, ثابت char16_t *p,
                                            UBool onlyContiguous) ثابت {
    اگر (start == p) {
        بازگشت درست;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
    بازگشت norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ثابت uint8_t *start, ثابت uint8_t *p,
                                            UBool onlyContiguous) ثابت {
    اگر (start == p) {
        بازگشت درست;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_PREV(normTrie, UCPTRIE_16, start, p, norm16);
    بازگشت norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ثابت char16_t *Normalizer2Impl::findPreviousCompBoundary(ثابت char16_t *start, ثابت char16_t *p,
                                                       UBool onlyContiguous) ثابت {
    وقتی (p != start) {
        ثابت char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            بازگشت codePointLimit;
        }
        اگر (hasCompBoundaryBefore(c, norm16)) {
            بازگشت p;
        }
    }
    بازگشت p;
}

ثابت char16_t *Normalizer2Impl::findNextCompBoundary(ثابت char16_t *p, ثابت char16_t *limit,
                                                   UBool onlyContiguous) ثابت {
    وقتی (p != limit) {
        ثابت char16_t *codePointStart = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        اگر (hasCompBoundaryBefore(c, norm16)) {
            بازگشت codePointStart;
        }
        اگر (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            بازگشت p;
        }
    }
    بازگشت p;
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ثابت char16_t *start, ثابت char16_t *p) ثابت {
    اگر (start == p) {
        بازگشت 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U16_PREV(start, 0, i, c);
    بازگشت static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ثابت uint8_t *start, ثابت uint8_t *p) ثابت {
    اگر (start == p) {
        بازگشت 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_PREV(start, 0, i, c);
    بازگشت static_cast<uint8_t>(getFCD16(c));
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
#اگر (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::getFCD16FromNormData(UChar32 c) ثابت {
    uint16_t norm16=getNorm16(c);
    اگر (norm16 >= limitNoNo) {
        اگر(norm16>=MIN_NORMAL_MAYBE_YES) {
            // combining mark
            norm16=getCCFromNormalYesOrMaybe(norm16);
            بازگشت norm16|(norm16<<8);
        } وگرنه اگر(norm16>=minMaybeYes) {
            بازگشت 0;
        } وگرنه اگر(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_MASK;
            اگر (deltaTrailCC <= DELTA_TCCC_1) {
                بازگشت deltaTrailCC >> OFFSET_SHIFT;
            }
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    اگر(norm16<=minYesNo || isHangulLVT(norm16)) {
        // no decomposition or Hangul syllable, all zeros
        بازگشت 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping=getData(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    اگر(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    بازگشت norm16;
}
#اگر (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ثابت {
    U_ASSERT(norm16 >= minMaybeNo);
    اگر (norm16 >= MIN_NORMAL_MAYBE_YES) {
        // combining mark
        norm16 = getCCFromNormalYesOrMaybe(norm16);
        بازگشت norm16 | (norm16<<8);
    } وگرنه اگر (norm16 >= minMaybeYes) {
        بازگشت 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ثابت uint16_t *mapping = getDataForMaybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    بازگشت firstUnit >> 8;  // tccc
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
    اگر(limit==nullptr) {
        src=copy_کم_Prefix_From_Nul_Terminated(src, minLcccCP, buffer, errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت src;
        }
        اگر(prevBoundary<src) {
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            prevFCD16=getFCD16(*(src-1));
            اگر(prevFCD16>1) {
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

    برای(;;) {
        // count code units with lccc==0
        برای(prevSrc=src; src!=limit;) {
            اگر((c=*src)<minLcccCP) {
                prevFCD16=~c;
                ++src;
            } وگرنه اگر(!singleLeadMightHaveNonZeroFCD16(c)) {
                prevFCD16=0;
                ++src;
            } وگرنه {
                اگر(U16_IS_LEAD(c)) {
                    char16_t c2;
                    اگر((src+1)!=limit && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                اگر((fcd16=getFCD16FromNormData(c))<=0xff) {
                    prevFCD16=fcd16;
                    src+=U16_LENGTH(c);
                } وگرنه {
                    بشکن;
                }
            }
        }
        // copy these code units all at once
        اگر(src!=prevSrc) {
            اگر(buffer!=nullptr && !buffer->appendZeroCC(prevSrc, src, errorCode)) {
                بشکن;
            }
            اگر(src==limit) {
                بشکن;
            }
            prevBoundary=src;
            // We know that the previous character's lccc==0.
            اگر(prevFCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 prev=~prevFCD16;
                اگر(prev<minDecompNoCP) {
                    prevFCD16=0;
                } وگرنه {
                    prevFCD16=getFCD16FromNormData(prev);
                    اگر(prevFCD16>1) {
                        --prevBoundary;
                    }
                }
            } وگرنه {
                ثابت char16_t *p=src-1;
                اگر(U16_IS_TRAIL(*p) && prevSrc<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    prevFCD16=getFCD16FromNormData(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                اگر(prevFCD16>1) {
                    prevBoundary=p;
                }
            }
            // The start of the current character (c).
            prevSrc=src;
        } وگرنه اگر(src==limit) {
            بشکن;
        }

        src+=U16_LENGTH(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        اگر((prevFCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            اگر((fcd16&0xff)<=1) {
                prevBoundary=src;
            }
            اگر(buffer!=nullptr && !buffer->appendZeroCC(c, errorCode)) {
                بشکن;
            }
            prevFCD16=fcd16;
            ادامه;
        } وگرنه اگر(buffer==nullptr) {
            بازگشت prevBoundary;  // quick check "no"
        } وگرنه {
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
            decomposeShort(prevBoundary, src, نادرست, نادرست, *buffer, errorCode);
            اگر (U_FAILURE(errorCode)) {
                بشکن;
            }
            prevBoundary=src;
            prevFCD16=0;
        }
    }
    بازگشت src;
}

تهی Normalizer2Impl::makeFCDAndAppend(ثابت char16_t *src, ثابت char16_t *limit,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_میانه,
                                       ReorderingBuffer &buffer,
                                       UErrorCode &errorCode) ثابت {
    اگر(!buffer.isEmpty()) {
        ثابت char16_t *firstBoundaryInSrc=findNextFCDBoundary(src, limit);
        اگر(src!=firstBoundaryInSrc) {
            ثابت char16_t *lastBoundaryInDest=findPreviousFCDBoundary(buffer.getStart(),
                                                                    buffer.getLimit());
            int32_t destSuffixLength = static_cast<int32_t>(buffer.getLimit() - lastBoundaryInDest);
            UnicodeString میانه(lastBoundaryInDest, destSuffixLength);
            buffer.removeSuffix(destSuffixLength);
            safe_میانه=میانه;
            میانه.append(src, static_cast<int32_t>(firstBoundaryInSrc - src));
            ثابت char16_t *میانه_Start=میانه.getBuffer();
            makeFCD(میانه_Start, میانه_Start+میانه.length(), &buffer, errorCode);
            اگر(U_FAILURE(errorCode)) {
                بازگشت;
            }
            src=firstBoundaryInSrc;
        }
    }
    اگر(doMakeFCD) {
        makeFCD(src, limit, &buffer, errorCode);
    } وگرنه {
        اگر(limit==nullptr) {  // appendZeroCC() needs limit!=nullptr
            limit=u_strchr(src, 0);
        }
        buffer.appendZeroCC(src, limit, errorCode);
    }
}

ثابت char16_t *Normalizer2Impl::findPreviousFCDBoundary(ثابت char16_t *start, ثابت char16_t *p) ثابت {
    وقتی(start<p) {
        ثابت char16_t *codePointLimit = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_PREV(normTrie, UCPTRIE_16, start, p, c, norm16);
        اگر (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            بازگشت codePointLimit;
        }
        اگر (norm16HasDecompBoundaryBefore(norm16)) {
            بازگشت p;
        }
    }
    بازگشت p;
}

ثابت char16_t *Normalizer2Impl::findNextFCDBoundary(ثابت char16_t *p, ثابت char16_t *limit) ثابت {
    وقتی(p<limit) {
        ثابت char16_t *codePointStart=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_NEXT(normTrie, UCPTRIE_16, p, limit, c, norm16);
        اگر (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            بازگشت codePointStart;
        }
        اگر (norm16HasDecompBoundaryAfter(norm16)) {
            بازگشت p;
        }
    }
    بازگشت p;
}

// CanonicalIterator data -------------------------------------------------- ***

CanonIterData::CanonIterData(UErrorCode &errorCode) :
        mutableTrie(umutablecptrie_open(0, 0, &errorCode)), trie(nullptr),
        canonStartSets(uprv_deleteUObject, nullptr, errorCode) {}

CanonIterData::~CanonIterData() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

تهی CanonIterData::addToStartSet(UChar32 origin, UChar32 decompLead, UErrorCode &errorCode) {
    uint32_t canon_مقدار = umutablecptrie_get(mutableTrie, decompLead);
    اگر((canon_مقدار&(CANON_HAS_SET|CANON_مقدار_MASK))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_set(mutableTrie, decompLead, canon_مقدار|origin, &errorCode);
    } وگرنه {
        // origin is not the first character, or it is U+0000.
        UnicodeSet *set;
        اگر((canon_مقدار&CANON_HAS_SET)==0) {
            LocalPointer<UnicodeSet> lpSet(جدید UnicodeSet, errorCode);
            set=lpSet.getAlias();
            اگر(U_FAILURE(errorCode)) {
                بازگشت;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_مقدار & CANON_مقدار_MASK);
            canon_مقدار = (canon_مقدار & ~CANON_مقدار_MASK) | CANON_HAS_SET | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_set(mutableTrie, decompLead, canon_مقدار, &errorCode);
            canonStartSets.adoptElement(lpSet.orphan(), errorCode);
            اگر (U_FAILURE(errorCode)) {
                بازگشت;
            }
            اگر(firstOrigin!=0) {
                set->add(firstOrigin);
            }
        } وگرنه {
            set = static_cast<UnicodeSet*>(canonStartSets[static_cast<int32_t>(canon_مقدار & CANON_مقدار_MASK)]);
        }
        set->add(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
رده InitCanonIterData {
عمومی:
    ایستا تهی doInit(Normalizer2Impl *impl, UErrorCode &errorCode);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
ایستا تهی U_CALLCONV
initCanonIterData(Normalizer2Impl *impl, UErrorCode &errorCode) {
    InitCanonIterData::doInit(impl, errorCode);
}

U_CDECL_END

تهی InitCanonIterData::doInit(Normalizer2Impl *impl, UErrorCode &errorCode) {
    U_ASSERT(impl->fCanonIterData == nullptr);
    impl->fCanonIterData = جدید CanonIterData(errorCode);
    اگر (impl->fCanonIterData == nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (U_SUCCESS(errorCode)) {
        UChar32 start = 0, end;
        uint32_t value;
        وقتی ((end = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       nullptr, nullptr, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            اگر (value != Normalizer2Impl::INERT) {
                impl->makeCanonIterDataFromNorm16(start, end, value, *impl->fCanonIterData, errorCode);
            }
            start = end + 1;
        }
#ifdef UCPTRIE_DEBUG
        umutablecptrie_setName(impl->fCanonIterData->mutableTrie, "CanonIterData");
#endif
        impl->fCanonIterData->trie = umutablecptrie_buildImmutable(
            impl->fCanonIterData->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_مقدار_BITS_32, &errorCode);
        umutablecptrie_close(impl->fCanonIterData->mutableTrie);
        impl->fCanonIterData->mutableTrie = nullptr;
    }
    اگر (U_FAILURE(errorCode)) {
        delete impl->fCanonIterData;
        impl->fCanonIterData = nullptr;
    }
}

تهی Normalizer2Impl::makeCanonIterDataFromNorm16(UChar32 start, UChar32 end, ثابت uint16_t norm16,
                                                  CanonIterData &newData,
                                                  UErrorCode &errorCode) ثابت {
    اگر(isInert(norm16) ||
            (minYesNo<=norm16 && norm16<minNoNo) ||
            (minMaybeNo<=norm16 && norm16<minMaybeYes)) {
        // Inert, or 2-way mapping (including Hangul syllable).
        // We do not write a canonStartSet for any yesNo/maybeNo character.
        // Composites from 2-way mappings are added at runtime from the
        // starter's compositions list, and the other characters in
        // 2-way mappings get CANON_NOT_SEGMENT_STARTER set because they are
        // "maybe" characters.
        بازگشت;
    }
    برای(UChar32 c=start; c<=end; ++c) {
        uint32_t old_مقدار = umutablecptrie_get(newData.mutableTrie, c);
        uint32_t new_مقدار=old_مقدار;
        اگر(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_مقدار|=CANON_NOT_SEGMENT_STARTER;
            اگر(norm16<MIN_NORMAL_MAYBE_YES) {
                new_مقدار|=CANON_HAS_COMPOSITIONS;
            }
        } وگرنه اگر(norm16<minYesNo) {
            new_مقدار|=CANON_HAS_COMPOSITIONS;
        } وگرنه {
            // c has a one-way decomposition
            UChar32 c2=c;
            // Do not modify the whole-range norm16 value.
            uint16_t norm16_2=norm16;
            اگر (isDecompNoAlgorithmic(norm16_2)) {
                // Maps to an isCompYesAndZeroCC.
                c2 = mapAlgorithmic(c2, norm16_2);
                norm16_2 = getRawNorm16(c2);
                // No compatibility mappings for the CanonicalIterator.
                U_ASSERT(!(isHangulLV(norm16_2) || isHangulLVT(norm16_2)));
            }
            اگر (norm16_2 > minYesNo) {
                // c decomposes, get everything from the variable-length extra data
                ثابت uint16_t *mapping=getDataForYesOrNo(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_LENGTH_MASK;
                اگر((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    اگر(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_مقدار|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                اگر(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_NEXT_UNSAFE(mapping, i, c2);
                    newData.addToStartSet(c, c2, errorCode);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    اگر(norm16_2>=minNoNo) {
                        وقتی(i<length) {
                            U16_NEXT_UNSAFE(mapping, i, c2);
                            uint32_t c2_مقدار = umutablecptrie_get(newData.mutableTrie, c2);
                            اگر((c2_مقدار&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_set(newData.mutableTrie, c2,
                                                   c2_مقدار|CANON_NOT_SEGMENT_STARTER, &errorCode);
                            }
                        }
                    }
                }
            } وگرنه {
                // c decomposed to c2 algorithmically; c has cc==0
                newData.addToStartSet(c, c2, errorCode);
            }
        }
        اگر(new_مقدار!=old_مقدار) {
            umutablecptrie_set(newData.mutableTrie, c, new_مقدار, &errorCode);
        }
    }
}

UBool Normalizer2Impl::ensureCanonIterData(UErrorCode &errorCode) ثابت {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(این);
    umtx_initOnce(me->fCanonIterDataInitOnce, &initCanonIterData, me, errorCode);
    بازگشت U_SUCCESS(errorCode);
}

int32_t Normalizer2Impl::get_Canon_مقدار(UChar32 c) ثابت {
    بازگشت static_cast<int32_t>(ucptrie_get(fCanonIterData->trie, c));
}

ثابت UnicodeSet &Normalizer2Impl::getCanonStartSet(int32_t n) ثابت {
    بازگشت *static_cast<ثابت UnicodeSet*>(fCanonIterData->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ثابت {
    بازگشت get_Canon_مقدار(c)>=0;
}

UBool Normalizer2Impl::getCanonStartSet(UChar32 c, UnicodeSet &set) ثابت {
    int32_t canon_مقدار=get_Canon_مقدار(c)&~CANON_NOT_SEGMENT_STARTER;
    اگر(canon_مقدار==0) {
        بازگشت نادرست;
    }
    set.clear();
    int32_t value=canon_مقدار&CANON_مقدار_MASK;
    اگر((canon_مقدار&CANON_HAS_SET)!=0) {
        set.addAll(getCanonStartSet(value));
    } وگرنه اگر(value!=0) {
        set.add(value);
    }
    اگر((canon_مقدار&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        اگر(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_BASE + (c - Hangul::JAMO_L_BASE) * Hangul::JAMO_VT_شمار);
            set.add(syllable, syllable+Hangul::JAMO_VT_شمار-1);
        } وگرنه {
            addComposites(getCompositionsList(norm16), set);
        }
    }
    بازگشت درست;
}

U_NAMESPACE_END

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(ثابت UDataSwapper *ds,
            ثابت تهی *inData, int32_t length, تهی *outData,
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
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت نویسه *)inData+4);
    uint8_t formatVersion0=pInfo->formatVersion[0];
    اگر(!(
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
        بازگشت 0;
    }

    inBytes=(ثابت uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(ثابت int32_t *)inBytes;
    int32_t minIndexesLength;
    اگر(formatVersion0==1) {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } وگرنه اگر(formatVersion0==2) {
        minIndexesLength=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } وگرنه اگر(formatVersion0<=4) {
        minIndexesLength=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } وگرنه {
        minIndexesLength=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    اگر(length>=0) {
        length-=headerSize;
        اگر(length<minIndexesLength*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }
    }

    /* read the first few indexes */
    برای(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_TOTAL_SIZE];

    اگر(length>=0) {
        اگر(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }

        /* copy the data for inaccessible bytes */
        اگر(inBytes!=outBytes) {
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

    بازگشت headerSize+size;
}

#endif  // !UCONFIG_NO_NORMALIZATION
