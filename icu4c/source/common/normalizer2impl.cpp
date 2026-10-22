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
UChar32 संहिता_Point_From_Valid_UTF8(ध्रुव uint8_t *cpStart, ध्रुव uint8_t *cp_सीमा) {
    // Similar to U8_NEXT_UNSAFE(s, i, c).
    U_ASSERT(cpStart < cp_सीमा);
    uint8_t c = *cpStart;
    विकल्प(cp_सीमा-cpStart) {
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
int32_t get_Jamo_TMinus_आधारः(ध्रुव uint8_t *src, ध्रुव uint8_t *सीमा) {
    // Jamo T: E1 86 A8..E1 87 82
    यदि ((सीमा - src) >= 3 && *src == 0xe1) {
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
append_संहिता_Point_Delta(ध्रुव uint8_t *cpStart, ध्रुव uint8_t *cp_सीमा, int32_t delta,
                     ByteSink &sink, Edits *edits) {
    वर्ण बफर[U8_MAX_दीर्घता];
    int32_t length;
    int32_t cp_दीर्घता = static_cast<int32_t>(cp_सीमा - cpStart);
    यदि (cp_दीर्घता == 1) {
        // The builder makes ASCII map to ASCII.
        बफर[0] = static_cast<uint8_t>(*cpStart + delta);
        length = 1;
    } अन्यथा {
        int32_t trail = *(cp_सीमा-1) + delta;
        यदि (0x80 <= trail && trail <= 0xbf) {
            // The delta only changes the last trail byte.
            --cp_सीमा;
            length = 0;
            कुरु { बफर[length++] = *cpStart++; } यावत् (cpStart < cp_सीमा);
            बफर[length++] = static_cast<uint8_t>(trail);
        } अन्यथा {
            // Decode the code point, add the delta, re-encode.
            UChar32 c = संहिता_Point_From_Valid_UTF8(cpStart, cp_सीमा) + delta;
            length = 0;
            U8_APPEND_UNSAFE(बफर, length, c);
        }
    }
    यदि (edits != शून्यसूचकः) {
        edits->सङ्कलय_Replace(cp_दीर्घता, length);
    }
    sink.Append(बफर, length);
}

}  // namespace

// ReorderingBuffer -------------------------------------------------------- ***

Reordering_बफर::Reordering_बफर(ध्रुव Normalizer2Impl &ni, UnicodeString &dest,
                                   UError_संहिता &error_संहिता) :
        impl(ni), str(dest),
        start(str.get_बफर(8)), reorderStart(start), सीमा(start),
        remaining_क्षमता(str.get_क्षमता()), चरमः_CC(0) {
    यदि (start == शून्यसूचकः && U_SUCCESS(error_संहिता)) {
        // getBuffer() already did str.setToBogus()
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
    }
}

UBool Reordering_बफर::init(int32_t dest_क्षमता, UError_संहिता &error_संहिता) {
    int32_t length=str.length();
    start=str.get_बफर(dest_क्षमता);
    यदि(start==शून्यसूचकः) {
        // getBuffer() already did str.setToBogus()
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम असत्य;
    }
    सीमा=start+length;
    remaining_क्षमता=str.get_क्षमता()-length;
    reorderStart=start;
    यदि(start==सीमा) {
        चरमः_CC=0;
    } अन्यथा {
        समुच्चयः_Iterator();
        चरमः_CC=previousCC();
        // Set reorderStart after the last code point with cc<=1 if there is one.
        यदि(चरमः_CC>1) {
            यावत्(previousCC()>1) {}
        }
        reorderStart=संहिता_Point_सीमा;
    }
    प्रत्यागम सत्य;
}

UBool Reordering_बफर::equals(ध्रुव char16_t *otherStart, ध्रुव char16_t *other_सीमा) ध्रुव {
    int32_t length = static_cast<int32_t>(सीमा - start);
    प्रत्यागम
        length == static_cast<int32_t>(other_सीमा - otherStart) &&
        0==u_memcmp(start, otherStart, length);
}

UBool Reordering_बफर::equals(ध्रुव uint8_t *otherStart, ध्रुव uint8_t *other_सीमा) ध्रुव {
    U_ASSERT((other_सीमा - otherStart) <= INT32_MAX);  // ensured by caller
    int32_t length = static_cast<int32_t>(सीमा - start);
    int32_t other_दीर्घता = static_cast<int32_t>(other_सीमा - otherStart);
    // For equal strings, UTF-8 is at least as long as UTF-16, and at most three times as long.
    यदि (other_दीर्घता < length || (other_दीर्घता / 3) > length) {
        प्रत्यागम असत्य;
    }
    // Compare valid strings from between normalization boundaries.
    // (Invalid sequences are normalization-inert.)
    प्रति (int32_t i = 0, j = 0;;) {
        यदि (i >= length) {
            प्रत्यागम j >= other_दीर्घता;
        } अन्यथा यदि (j >= other_दीर्घता) {
            प्रत्यागम असत्य;
        }
        // Not at the end of either string yet.
        UChar32 c, other;
        U16_अग्र_UNSAFE(start, i, c);
        U8_अग्र_UNSAFE(otherStart, j, other);
        यदि (c != other) {
            प्रत्यागम असत्य;
        }
    }
}

UBool Reordering_बफर::appendSupplementary(UChar32 c, uint8_t cc, UError_संहिता &error_संहिता) {
    यदि(remaining_क्षमता<2 && !resize(2, error_संहिता)) {
        प्रत्यागम असत्य;
    }
    यदि(चरमः_CC<=cc || cc==0) {
        सीमा[0]=U16_LEAD(c);
        सीमा[1]=U16_TRAIL(c);
        सीमा+=2;
        चरमः_CC=cc;
        यदि(cc<=1) {
            reorderStart=सीमा;
        }
    } अन्यथा {
        insert(c, cc);
    }
    remaining_क्षमता-=2;
    प्रत्यागम सत्य;
}

UBool Reordering_बफर::append(ध्रुव char16_t *s, int32_t length, UBool isNFD,
                               uint8_t leadCC, uint8_t trailCC,
                               UError_संहिता &error_संहिता) {
    यदि(length==0) {
        प्रत्यागम सत्य;
    }
    यदि(remaining_क्षमता<length && !resize(length, error_संहिता)) {
        प्रत्यागम असत्य;
    }
    remaining_क्षमता-=length;
    यदि(चरमः_CC<=leadCC || leadCC==0) {
        यदि(trailCC<=1) {
            reorderStart=सीमा+length;
        } अन्यथा यदि(leadCC<=1) {
            reorderStart=सीमा+1;  // Ok if not a code point boundary.
        }
        ध्रुव char16_t *s_सीमा=s+length;
        कुरु { *सीमा++=*s++; } यावत्(s!=s_सीमा);
        चरमः_CC=trailCC;
    } अन्यथा {
        int32_t i=0;
        UChar32 c;
        U16_अग्र(s, i, length, c);
        insert(c, leadCC);  // insert first code point
        यावत्(i<length) {
            U16_अग्र(s, i, length, c);
            यदि(i<length) {
                यदि (isNFD) {
                    leadCC = Normalizer2Impl::getCCFromYesOrMaybeYes(impl.getRawNorm16(c));
                } अन्यथा {
                    leadCC = impl.getCC(impl.getNorm16(c));
                }
            } अन्यथा {
                leadCC=trailCC;
            }
            append(c, leadCC, error_संहिता);
        }
    }
    प्रत्यागम सत्य;
}

UBool Reordering_बफर::appendZeroCC(UChar32 c, UError_संहिता &error_संहिता) {
    int32_t cp_दीर्घता=U16_दीर्घता(c);
    यदि(remaining_क्षमता<cp_दीर्घता && !resize(cp_दीर्घता, error_संहिता)) {
        प्रत्यागम असत्य;
    }
    remaining_क्षमता-=cp_दीर्घता;
    यदि(cp_दीर्घता==1) {
        *सीमा++ = static_cast<char16_t>(c);
    } अन्यथा {
        सीमा[0]=U16_LEAD(c);
        सीमा[1]=U16_TRAIL(c);
        सीमा+=2;
    }
    चरमः_CC=0;
    reorderStart=सीमा;
    प्रत्यागम सत्य;
}

UBool Reordering_बफर::appendZeroCC(ध्रुव char16_t *s, ध्रुव char16_t *s_सीमा, UError_संहिता &error_संहिता) {
    यदि(s==s_सीमा) {
        प्रत्यागम सत्य;
    }
    int32_t length = static_cast<int32_t>(s_सीमा - s);
    यदि(remaining_क्षमता<length && !resize(length, error_संहिता)) {
        प्रत्यागम असत्य;
    }
    u_memcpy(सीमा, s, length);
    सीमा+=length;
    remaining_क्षमता-=length;
    चरमः_CC=0;
    reorderStart=सीमा;
    प्रत्यागम सत्य;
}

शून्य Reordering_बफर::remove() {
    reorderStart=सीमा=start;
    remaining_क्षमता=str.get_क्षमता();
    चरमः_CC=0;
}

शून्य Reordering_बफर::removeSuffix(int32_t suffix_दीर्घता) {
    यदि(suffix_दीर्घता<(सीमा-start)) {
        सीमा-=suffix_दीर्घता;
        remaining_क्षमता+=suffix_दीर्घता;
    } अन्यथा {
        सीमा=start;
        remaining_क्षमता=str.get_क्षमता();
    }
    चरमः_CC=0;
    reorderStart=सीमा;
}

UBool Reordering_बफर::resize(int32_t append_दीर्घता, UError_संहिता &error_संहिता) {
    int32_t reorder_Start_सूचकांक = static_cast<int32_t>(reorderStart - start);
    int32_t length = static_cast<int32_t>(सीमा - start);
    str.release_बफर(length);
    int32_t new_क्षमता=length+append_दीर्घता;
    int32_t double_क्षमता=2*str.get_क्षमता();
    यदि(new_क्षमता<double_क्षमता) {
        new_क्षमता=double_क्षमता;
    }
    यदि(new_क्षमता<256) {
        new_क्षमता=256;
    }
    start=str.get_बफर(new_क्षमता);
    यदि(start==शून्यसूचकः) {
        // getBuffer() already did str.setToBogus()
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम असत्य;
    }
    reorderStart=start+reorder_Start_सूचकांक;
    सीमा=start+length;
    remaining_क्षमता=str.get_क्षमता()-length;
    प्रत्यागम सत्य;
}

शून्य Reordering_बफर::skipPrevious() {
    संहिता_Point_सीमा=संहिता_Point_Start;
    char16_t c=*--संहिता_Point_Start;
    यदि(U16_IS_TRAIL(c) && start<संहिता_Point_Start && U16_IS_LEAD(*(संहिता_Point_Start-1))) {
        --संहिता_Point_Start;
    }
}

uint8_t Reordering_बफर::previousCC() {
    संहिता_Point_सीमा=संहिता_Point_Start;
    यदि(reorderStart>=संहिता_Point_Start) {
        प्रत्यागम 0;
    }
    UChar32 c=*--संहिता_Point_Start;
    char16_t c2;
    यदि(U16_IS_TRAIL(c) && start<संहिता_Point_Start && U16_IS_LEAD(c2=*(संहिता_Point_Start-1))) {
        --संहिता_Point_Start;
        c=U16_GET_SUPPLEMENTARY(c2, c);
    }
    प्रत्यागम impl.getCCFromYesOrMaybeYesCP(c);
}

// Inserts c somewhere before the last character.
// Requires 0<cc<lastCC which implies reorderStart<limit.
शून्य Reordering_बफर::insert(UChar32 c, uint8_t cc) {
    प्रति(समुच्चयः_Iterator(), skipPrevious(); previousCC()>cc;) {}
    // insert c at codePointLimit, after the character with prevCC<=cc
    char16_t *q=सीमा;
    char16_t *r=सीमा+=U16_दीर्घता(c);
    कुरु {
        *--r=*--q;
    } यावत्(संहिता_Point_सीमा!=q);
    write_संहिता_Point(q, c);
    यदि(cc<=1) {
        reorderStart=r;
    }
}

// Normalizer2Impl --------------------------------------------------------- ***

संरचना Canon_Iter_दत्त : सार्वजनिक UMemory {
    Canon_Iter_दत्त(UError_संहिता &error_संहिता);
    ~Canon_Iter_दत्त();
    शून्य सङ्कलय_गन्तव्यम्_Start_समुच्चयः(UChar32 origin, UChar32 decompLead, UError_संहिता &error_संहिता);
    UMutableCPTrie *mutableTrie;
    UCPTrie *trie;
    UVector canonStartSets;  // contains UnicodeSet *
};

Normalizer2Impl::~Normalizer2Impl() {
    विलोपय f_Canon_Iter_दत्त;
}

शून्य
Normalizer2Impl::init(ध्रुव int32_t *अन्तर्_Indexes, ध्रुव UCPTrie *अन्तर्_Trie,
                      ध्रुव uint16_t *अन्तर्_Extra_दत्त, ध्रुव uint8_t *अन्तर्_Small_FCD) {
    minDecompNoCP = static_cast<char16_t>(अन्तर्_Indexes[IX_MIN_DECOMP_NO_CP]);
    minCompNoMaybeCP = static_cast<char16_t>(अन्तर्_Indexes[IX_MIN_COMP_NO_MAYBE_CP]);
    minLcccCP = static_cast<char16_t>(अन्तर्_Indexes[IX_MIN_LCCC_CP]);

    minYesNo = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_YES_NO]);
    minYesNoMappingsOnly = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_YES_NO_MAPPINGS_ONLY]);
    minNoNo = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_NO_NO]);
    minNoNoCompBoundaryBefore = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE]);
    minNoNoCompNoMaybeCC = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC]);
    min_No_No_रिक्तानि = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_NO_NO_रिक्तानि]);
    सीमा_No_No = static_cast<uint16_t>(अन्तर्_Indexes[IX_सीमा_NO_NO]);
    minMaybeNo = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_MAYBE_NO]);
    minMaybeNoCombinesFwd = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_MAYBE_NO_COMBINES_FWD]);
    minMaybeYes = static_cast<uint16_t>(अन्तर्_Indexes[IX_MIN_MAYBE_YES]);
    U_ASSERT((minMaybeNo & 7) == 0);  // 8-aligned for noNoDelta bit fields
    centerNoNoDelta = (minMaybeNo >> DELTA_सरणम्) - MAX_DELTA - 1;

    normTrie=अन्तर्_Trie;
    extra_दत्त=अन्तर्_Extra_दत्त;
    smallFCD=अन्तर्_Small_FCD;
}

U_CDECL_BEGIN

स्थिर uint32_t U_CALLCONV
segmentStarterMapper(ध्रुव शून्य * /*context*/, uint32_t value) {
    प्रत्यागम value&CANON_NOT_SEGMENT_STARTER;
}

U_CDECL_अन्तः

शून्य
Normalizer2Impl::सङ्कलय_Lccc_Chars(Unicode_समुच्चयः &समुच्चयः) ध्रुव {
    UChar32 start = 0, अन्तः;
    uint32_t norm16;
    यावत् ((अन्तः = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   शून्यसूचकः, शून्यसूचकः, &norm16)) >= 0) {
        यदि (norm16 > Normalizer2Impl::MIN_प्रसामान्यम्_MAYBE_YES &&
                norm16 != Normalizer2Impl::JAMO_VT) {
            समुच्चयः.सङ्कलय(start, अन्तः);
        } अन्यथा यदि (minNoNoCompNoMaybeCC <= norm16 && norm16 < सीमा_No_No) {
            uint16_t fcd16 = getFCD16(start);
            यदि (fcd16 > 0xff) { समुच्चयः.सङ्कलय(start, अन्तः); }
        }
        start = अन्तः + 1;
    }
}

शून्य
Normalizer2Impl::सङ्कलय_Property_Starts(ध्रुव USetAdder *sa, UError_संहिता & /*errorCode*/) ध्रुव {
    // Add the start code point of each same-value range of the trie.
    UChar32 start = 0, अन्तः;
    uint32_t value;
    यावत् ((अन्तः = ucptrie_getRange(normTrie, start, UCPMAP_RANGE_FIXED_LEAD_SURROGATES, INERT,
                                   शून्यसूचकः, शून्यसूचकः, &value)) >= 0) {
        sa->सङ्कलय(sa->समुच्चयः, start);
        यदि (start != अन्तः && isAlgorithmicNoNo(static_cast<uint16_t>(value)) &&
                (value & Normalizer2Impl::DELTA_TCCC_आवरणम्) > Normalizer2Impl::DELTA_TCCC_1) {
            // Range of code points with same-norm16-value algorithmic decompositions.
            // They might have different non-zero FCD16 values.
            uint16_t पूर्व_FCD16 = getFCD16(start);
            यावत् (++start <= अन्तः) {
                uint16_t fcd16 = getFCD16(start);
                यदि (fcd16 != पूर्व_FCD16) {
                    sa->सङ्कलय(sa->समुच्चयः, start);
                    पूर्व_FCD16 = fcd16;
                }
            }
        }
        start = अन्तः + 1;
    }

    /* add Hangul LV syllables and LV+1 because of skippables */
    प्रति(char16_t c=Hangul::HANGUL_आधारः; c<Hangul::HANGUL_सीमा; c+=Hangul::JAMO_T_गणनम्) {
        sa->सङ्कलय(sa->समुच्चयः, c);
        sa->सङ्कलय(sa->समुच्चयः, c+1);
    }
    sa->सङ्कलय(sa->समुच्चयः, Hangul::HANGUL_सीमा); /* add Hangul+1 to continue with other properties */
}

शून्य
Normalizer2Impl::सङ्कलय_Canon_Iter_Property_Starts(ध्रुव USetAdder *sa, UError_संहिता &error_संहिता) ध्रुव {
    // Add the start code point of each same-value range of the canonical iterator data trie.
    यदि (!ensure_Canon_Iter_दत्त(error_संहिता)) { प्रत्यागम; }
    // Currently only used for the SEGMENT_STARTER property.
    UChar32 start = 0, अन्तः;
    uint32_t value;
    यावत् ((अन्तः = ucptrie_getRange(f_Canon_Iter_दत्त->trie, start, UCPMAP_RANGE_प्रसामान्यम्, 0,
                                   segmentStarterMapper, शून्यसूचकः, &value)) >= 0) {
        sa->सङ्कलय(sa->समुच्चयः, start);
        start = अन्तः + 1;
    }
}

ध्रुव char16_t *
Normalizer2Impl::copy_नीचः_उपसर्गः_From_Nul_Terminated(ध्रुव char16_t *src,
                                                UChar32 min_Need_दत्त_CP,
                                                Reordering_बफर *बफर,
                                                UError_संहिता &error_संहिता) ध्रुव {
    // Make some effort to support NUL-terminated strings reasonably.
    // Take the part of the fast quick check loop that does not look up
    // data and check the first part of the string.
    // After this prefix, determine the string length to simplify the rest
    // of the code.
    ध्रुव char16_t *पूर्व_Src=src;
    char16_t c;
    यावत्((c=*src++)<min_Need_दत्त_CP && c!=0) {}
    // Back out the last character for full processing.
    // Copy this prefix.
    यदि(--src!=पूर्व_Src) {
        यदि(बफर!=शून्यसूचकः) {
            बफर->appendZeroCC(पूर्व_Src, src, error_संहिता);
        }
    }
    प्रत्यागम src;
}

UnicodeString &
Normalizer2Impl::decompose(ध्रुव UnicodeString &src, UnicodeString &dest,
                           UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        dest.समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम dest;
    }
    ध्रुव char16_t *sArray=src.get_बफर();
    यदि(&dest==&src || sArray==शून्यसूचकः) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        dest.समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम dest;
    }
    decompose(sArray, sArray+src.length(), dest, src.length(), error_संहिता);
    प्रत्यागम dest;
}

शून्य
Normalizer2Impl::decompose(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                           UnicodeString &dest,
                           int32_t dest_दीर्घता_Estimate,
                           UError_संहिता &error_संहिता) ध्रुव {
    यदि(dest_दीर्घता_Estimate<0 && सीमा!=शून्यसूचकः) {
        dest_दीर्घता_Estimate = static_cast<int32_t>(सीमा - src);
    }
    dest.remove();
    Reordering_बफर बफर(*एतत्, dest);
    यदि(बफर.init(dest_दीर्घता_Estimate, error_संहिता)) {
        decompose(src, सीमा, &बफर, error_संहिता);
    }
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/spanQuickCheckYes
ध्रुव char16_t *
Normalizer2Impl::decompose(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                           Reordering_बफर *बफर,
                           UError_संहिता &error_संहिता) ध्रुव {
    यदि(src==शून्यसूचकः) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम src;
    }
    UChar32 minNoCP=minDecompNoCP;
    यदि(सीमा==शून्यसूचकः) {
        src=copy_नीचः_उपसर्गः_From_Nul_Terminated(src, minNoCP, बफर, error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम src;
        }
        सीमा=u_strchr(src, 0);
    }

    ध्रुव char16_t *पूर्व_Src;
    UChar32 c=0;
    uint16_t norm16=0;

    // only for quick check
    ध्रुव char16_t *पूर्व_Boundary=src;
    uint8_t पूर्व_CC=0;

    प्रति(;;) {
        // count code units below the minimum or with irrelevant data for the quick check
        प्रति(पूर्व_Src=src; src!=सीमा;) {
            यदि( (c=*src)<minNoCP ||
                isMostDecompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा यदि(!U16_IS_LEAD(c)) {
                भङ्ग;
            } अन्यथा {
                char16_t c2;
                यदि((src+1)!=सीमा && U16_IS_TRAIL(c2=src[1])) {
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
        यदि(src!=पूर्व_Src) {
            यदि(बफर!=शून्यसूचकः) {
                यदि(!बफर->appendZeroCC(पूर्व_Src, src, error_संहिता)) {
                    भङ्ग;
                }
            } अन्यथा {
                पूर्व_CC=0;
                पूर्व_Boundary=src;
            }
        }
        यदि(src==सीमा) {
            भङ्ग;
        }

        // Check one above-minimum, relevant code point.
        src+=U16_दीर्घता(c);
        यदि(बफर!=शून्यसूचकः) {
            यदि(!decompose(c, norm16, *बफर, error_संहिता)) {
                भङ्ग;
            }
        } अन्यथा {
            यदि(isDecompYes(norm16)) {
                uint8_t cc=getCCFromYesOrMaybeYes(norm16);
                यदि(पूर्व_CC<=cc || cc==0) {
                    पूर्व_CC=cc;
                    यदि(cc<=1) {
                        पूर्व_Boundary=src;
                    }
                    अनुवर्त;
                }
            }
            प्रत्यागम पूर्व_Boundary;  // "no" or cc out of order
        }
    }
    प्रत्यागम src;
}

// Decompose a short piece of text which is likely to contain characters that
// fail the quick check loop and/or where the quick check loop's overhead
// is unlikely to be amortized.
// Called by the compose() and makeFCD() implementations.
ध्रुव char16_t *
Normalizer2Impl::decomposeShort(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यावत्(src<सीमा) {
        यदि (stopAtCompBoundary && *src < minCompNoMaybeCP) {
            प्रत्यागम src;
        }
        ध्रुव char16_t *पूर्व_Src = src;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, src, सीमा, c, norm16);
        यदि (stopAtCompBoundary && norm16HasCompBoundaryBefore(norm16)) {
            प्रत्यागम पूर्व_Src;
        }
        यदि(!decompose(c, norm16, बफर, error_संहिता)) {
            प्रत्यागम शून्यसूचकः;
        }
        यदि (stopAtCompBoundary && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            प्रत्यागम src;
        }
    }
    प्रत्यागम src;
}

UBool Normalizer2Impl::decompose(UChar32 c, uint16_t norm16,
                                 Reordering_बफर &बफर,
                                 UError_संहिता &error_संहिता) ध्रुव {
    // get the decomposition and the lead and trail cc's
    यदि (norm16 >= सीमा_No_No) {
        यदि (isMaybeYesOrNonZeroCC(norm16)) {
            प्रत्यागम बफर.append(c, getCCFromYesOrMaybeYes(norm16), error_संहिता);
        } अन्यथा यदि (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            c=mapAlgorithmic(c, norm16);
            norm16=getRawNorm16(c);
        }
    }
    यदि (norm16 < minYesNo) {
        // c does not decompose
        प्रत्यागम बफर.append(c, 0, error_संहिता);
    } अन्यथा यदि(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        char16_t jamos[3];
        प्रत्यागम बफर.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_संहिता);
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=get_दत्त(norm16);
    uint16_t firstUnit=*mapping;
    int32_t length=firstUnit&MAPPING_दीर्घता_आवरणम्;
    uint8_t leadCC, trailCC;
    trailCC = static_cast<uint8_t>(firstUnit >> 8);
    यदि(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
    } अन्यथा {
        leadCC=0;
    }
    प्रत्यागम बफर.append(reinterpret_cast<ध्रुव char16_t*>(mapping) + 1, length, सत्य, leadCC, trailCC, error_संहिता);
}

// Dual functionality:
// sink != nullptr: normalize
// sink == nullptr: isNormalized/spanQuickCheckYes
ध्रुव uint8_t *
Normalizer2Impl::decomposeUTF8(uint32_t options,
                               ध्रुव uint8_t *src, ध्रुव uint8_t *सीमा,
                               ByteSink *sink, Edits *edits, UError_संहिता &error_संहिता) ध्रुव {
    U_ASSERT(सीमा != शून्यसूचकः);
    UnicodeString s16;
    uint8_t minNoLead = leadByteForCP(minDecompNoCP);

    ध्रुव uint8_t *पूर्व_Boundary = src;
    // only for quick check
    uint8_t पूर्व_CC = 0;

    प्रति (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no" code point,
        // or with (decompYes && ccc==0) properties.
        ध्रुव uint8_t *fastStart = src;
        ध्रुव uint8_t *पूर्व_Src;
        uint16_t norm16 = 0;

        प्रति (;;) {
            यदि (src == सीमा) {
                यदि (पूर्व_Boundary != सीमा && sink != शून्यसूचकः) {
                    ByteSinkUtil::appendUnchanged(पूर्व_Boundary, सीमा,
                                                  *sink, options, edits, error_संहिता);
                }
                प्रत्यागम src;
            }
            यदि (*src < minNoLead) {
                ++src;
            } अन्यथा {
                पूर्व_Src = src;
                UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, src, सीमा, norm16);
                यदि (!isMostDecompYesAndZeroCC(norm16)) {
                    भङ्ग;
                }
            }
        }
        // isMostDecompYesAndZeroCC(norm16) is false, that is, norm16>=minYesNo,
        // and the current character at [prevSrc..src[ is not a common case with cc=0
        // (MIN_NORMAL_MAYBE_YES or JAMO_VT).
        // It could still be a maybeYes with cc=0.
        यदि (पूर्व_Src != fastStart) {
            // The fast path looped over yes/0 characters before the current one.
            यदि (sink != शून्यसूचकः &&
                    !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                   *sink, options, edits, error_संहिता)) {
                भङ्ग;
            }
            पूर्व_Boundary = पूर्व_Src;
            पूर्व_CC = 0;
        }

        // Medium-fast path: Quick check.
        यदि (isMaybeYesOrNonZeroCC(norm16)) {
            // Does not decompose.
            uint8_t cc = getCCFromYesOrMaybeYes(norm16);
            यदि (पूर्व_CC <= cc || cc == 0) {
                पूर्व_CC = cc;
                यदि (cc <= 1) {
                    यदि (sink != शून्यसूचकः &&
                            !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, src,
                                                           *sink, options, edits, error_संहिता)) {
                        भङ्ग;
                    }
                    पूर्व_Boundary = src;
                }
                अनुवर्त;
            }
        }
        यदि (sink == शून्यसूचकः) {
            प्रत्यागम पूर्व_Boundary;  // quick check: "no" or cc out of order
        }

        // Slow path
        // Decompose up to and including the current character.
        यदि (पूर्व_Boundary != पूर्व_Src && norm16HasDecompBoundaryBefore(norm16)) {
            यदि (!ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                               *sink, options, edits, error_संहिता)) {
                भङ्ग;
            }
            पूर्व_Boundary = पूर्व_Src;
        }
        Reordering_बफर बफर(*एतत्, s16, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) {
            भङ्ग;
        }
        decomposeShort(पूर्व_Boundary, src, STOP_AT_सीमा, असत्य /* onlyContiguous */,
                       बफर, error_संहिता);
        // Decompose until the next boundary.
        यदि (बफर.get_चरमः_CC() > 1) {
            src = decomposeShort(src, सीमा, STOP_AT_DECOMP_BOUNDARY, असत्य /* onlyContiguous */,
                                 बफर, error_संहिता);
        }
        यदि (U_FAILURE(error_संहिता)) {
            भङ्ग;
        }
        यदि ((src - पूर्व_Src) > INT32_MAX) {  // guard before buffer.equals()
            error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
            भङ्ग;
        }
        // We already know there was a change if the original character decomposed;
        // otherwise compare.
        यदि (isMaybeYesOrNonZeroCC(norm16) && बफर.equals(पूर्व_Boundary, src)) {
            यदि (!ByteSinkUtil::appendUnchanged(पूर्व_Boundary, src,
                                               *sink, options, edits, error_संहिता)) {
                भङ्ग;
            }
        } अन्यथा {
            यदि (!ByteSinkUtil::appendChange(पूर्व_Boundary, src, बफर.getStart(), बफर.length(),
                                            *sink, edits, error_संहिता)) {
                भङ्ग;
            }
        }
        पूर्व_Boundary = src;
        पूर्व_CC = 0;
    }
    प्रत्यागम src;
}

ध्रुव uint8_t *
Normalizer2Impl::decomposeShort(ध्रुव uint8_t *src, ध्रुव uint8_t *सीमा,
                                StopAt stopAt, UBool onlyContiguous,
                                Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यावत् (src < सीमा) {
        ध्रुव uint8_t *पूर्व_Src = src;
        uint16_t norm16;
        UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, src, सीमा, norm16);
        // Get the decomposition and the lead and trail cc's.
        UChar32 c = U_SENTINEL;
        यदि (norm16 >= सीमा_No_No) {
            यदि (isMaybeYesOrNonZeroCC(norm16)) {
                // No comp boundaries around this character.
                uint8_t cc = getCCFromYesOrMaybeYes(norm16);
                यदि (cc == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                    प्रत्यागम पूर्व_Src;
                }
                c = संहिता_Point_From_Valid_UTF8(पूर्व_Src, src);
                यदि (!बफर.append(c, cc, error_संहिता)) {
                    प्रत्यागम शून्यसूचकः;
                }
                यदि (stopAt == STOP_AT_DECOMP_BOUNDARY && बफर.get_चरमः_CC() <= 1) {
                    प्रत्यागम src;
                }
                अनुवर्त;
            } अन्यथा यदि (norm16 < minMaybeNo) {
                // Maps to an isCompYesAndZeroCC.
                यदि (stopAt != STOP_AT_सीमा) {
                    प्रत्यागम पूर्व_Src;
                }
                c = संहिता_Point_From_Valid_UTF8(पूर्व_Src, src);
                c = mapAlgorithmic(c, norm16);
                norm16 = getRawNorm16(c);
            }
        } अन्यथा यदि (stopAt != STOP_AT_सीमा && norm16 < minNoNoCompNoMaybeCC) {
            प्रत्यागम पूर्व_Src;
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
                c = संहिता_Point_From_Valid_UTF8(पूर्व_Src, src);
            }
            // does not decompose
            यदि (!बफर.append(c, 0, error_संहिता)) {
                प्रत्यागम शून्यसूचकः;
            }
        } अन्यथा यदि (isHangulLV(norm16) || isHangulLVT(norm16)) {
            // Hangul syllable: decompose algorithmically
            यदि (c < 0) {
                c = संहिता_Point_From_Valid_UTF8(पूर्व_Src, src);
            }
            char16_t jamos[3];
            यदि (!बफर.appendZeroCC(jamos, jamos+Hangul::decompose(c, jamos), error_संहिता)) {
                प्रत्यागम शून्यसूचकः;
            }
        } अन्यथा {
            // The character decomposes, get everything from the variable-length extra data.
            ध्रुव uint16_t *mapping = get_दत्त(norm16);
            uint16_t firstUnit = *mapping;
            int32_t length = firstUnit & MAPPING_दीर्घता_आवरणम्;
            uint8_t trailCC = static_cast<uint8_t>(firstUnit >> 8);
            uint8_t leadCC;
            यदि (firstUnit & MAPPING_HAS_CCC_LCCC_WORD) {
                leadCC = static_cast<uint8_t>(*(mapping - 1) >> 8);
            } अन्यथा {
                leadCC = 0;
            }
            यदि (leadCC == 0 && stopAt == STOP_AT_DECOMP_BOUNDARY) {
                प्रत्यागम पूर्व_Src;
            }
            यदि (!बफर.append(reinterpret_cast<ध्रुव char16_t*>(mapping) + 1, length, सत्य, leadCC, trailCC, error_संहिता)) {
                प्रत्यागम शून्यसूचकः;
            }
        }
        यदि ((stopAt == STOP_AT_COMP_BOUNDARY && norm16HasCompBoundaryAfter(norm16, onlyContiguous)) ||
                (stopAt == STOP_AT_DECOMP_BOUNDARY && बफर.get_चरमः_CC() <= 1)) {
            प्रत्यागम src;
        }
    }
    प्रत्यागम src;
}

ध्रुव char16_t *
Normalizer2Impl::getDecomposition(UChar32 c, char16_t बफर[4], int32_t &length) ध्रुव {
    uint16_t norm16;
    यदि(c<minDecompNoCP || isMaybeYesOrNonZeroCC(norm16=getNorm16(c))) {
        // c does not decompose
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव char16_t *decomp = शून्यसूचकः;
    यदि(isDecompNoAlgorithmic(norm16)) {
        // Maps to an isCompYesAndZeroCC.
        c=mapAlgorithmic(c, norm16);
        decomp=बफर;
        length=0;
        U16_APPEND_UNSAFE(बफर, length, c);
        // The mapping might decompose further.
        norm16 = getRawNorm16(c);
    }
    यदि (norm16 < minYesNo) {
        प्रत्यागम decomp;
    } अन्यथा यदि(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        length=Hangul::decompose(c, बफर);
        प्रत्यागम बफर;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=get_दत्त(norm16);
    length=*mapping&MAPPING_दीर्घता_आवरणम्;
    प्रत्यागम reinterpret_cast<ध्रुव char16_t*>(mapping) + 1;
}

// The capacity of the buffer must be 30=MAPPING_LENGTH_MASK-1
// so that a raw mapping fits that consists of one unit ("rm0")
// plus all but the first two code units of the normal mapping.
// The maximum length of a normal mapping is 31=MAPPING_LENGTH_MASK.
ध्रुव char16_t *
Normalizer2Impl::getRawDecomposition(UChar32 c, char16_t बफर[30], int32_t &length) ध्रुव {
    uint16_t norm16;
    यदि(c<minDecompNoCP || isDecompYes(norm16=getNorm16(c))) {
        // c does not decompose
        प्रत्यागम शून्यसूचकः;
    } अन्यथा यदि(isHangulLV(norm16) || isHangulLVT(norm16)) {
        // Hangul syllable: decompose algorithmically
        Hangul::getRawDecomposition(c, बफर);
        length=2;
        प्रत्यागम बफर;
    } अन्यथा यदि(isDecompNoAlgorithmic(norm16)) {
        c=mapAlgorithmic(c, norm16);
        length=0;
        U16_APPEND_UNSAFE(बफर, length, c);
        प्रत्यागम बफर;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=get_दत्त(norm16);
    uint16_t firstUnit=*mapping;
    int32_t m_दीर्घता=firstUnit&MAPPING_दीर्घता_आवरणम्;  // length of normal mapping
    यदि(firstUnit&MAPPING_HAS_RAW_MAPPING) {
        // Read the raw mapping from before the firstUnit and before the optional ccc/lccc word.
        // Bit 7=MAPPING_HAS_CCC_LCCC_WORD
        ध्रुव uint16_t *rawMapping=mapping-((firstUnit>>7)&1)-1;
        uint16_t rm0=*rawMapping;
        यदि(rm0<=MAPPING_दीर्घता_आवरणम्) {
            length=rm0;
            प्रत्यागम reinterpret_cast<ध्रुव char16_t*>(rawMapping) - rm0;
        } अन्यथा {
            // Copy the normal mapping and replace its first two code units with rm0.
            बफर[0] = static_cast<char16_t>(rm0);
            u_memcpy(बफर + 1, reinterpret_cast<ध्रुव char16_t*>(mapping) + 1 + 2, m_दीर्घता - 2);
            length=m_दीर्घता-1;
            प्रत्यागम बफर;
        }
    } अन्यथा {
        length=m_दीर्घता;
        प्रत्यागम reinterpret_cast<ध्रुव char16_t*>(mapping) + 1;
    }
}

शून्य Normalizer2Impl::decomposeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                                         UBool doDecompose,
                                         UnicodeString &safe_मध्यः,
                                         Reordering_बफर &बफर,
                                         UError_संहिता &error_संहिता) ध्रुव {
    बफर.copy_Reorderable_Suffix_गन्तव्यम्(safe_मध्यः);
    यदि(doDecompose) {
        decompose(src, सीमा, &बफर, error_संहिता);
        प्रत्यागम;
    }
    // Just merge the strings at the boundary.
    तार्किक isFirst = सत्य;
    uint8_t firstCC = 0, पूर्व_CC = 0, cc;
    ध्रुव char16_t *p = src;
    यावत् (p != सीमा) {
        ध्रुव char16_t *संहिता_Point_Start = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, p, सीमा, c, norm16);
        यदि ((cc = getCC(norm16)) == 0) {
            p = संहिता_Point_Start;
            भङ्ग;
        }
        यदि (isFirst) {
            firstCC = cc;
            isFirst = असत्य;
        }
        पूर्व_CC = cc;
    }
    यदि(सीमा==शून्यसूचकः) {  // appendZeroCC() needs limit!=nullptr
        सीमा=u_strchr(p, 0);
    }

    यदि (बफर.append(src, static_cast<int32_t>(p - src), असत्य, firstCC, पूर्व_CC, error_संहिता)) {
        बफर.appendZeroCC(p, सीमा, error_संहिता);
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
    यदि (norm16 >= सीमा_No_No) {
        प्रत्यागम norm16 <= MIN_प्रसामान्यम्_MAYBE_YES || norm16 == JAMO_VT;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=get_दत्त_For_Yes_Or_No(norm16);
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
    यदि (norm16 >= सीमा_No_No) {
        यदि (isMaybeYesOrNonZeroCC(norm16)) {
            प्रत्यागम norm16 <= MIN_प्रसामान्यम्_MAYBE_YES || norm16 == JAMO_VT;
        } अन्यथा यदि (norm16 < minMaybeNo) {
            // Maps to an isCompYesAndZeroCC.
            प्रत्यागम (norm16 & DELTA_TCCC_आवरणम्) <= DELTA_TCCC_1;
        }
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping=get_दत्त(norm16);
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
    यदि(trail<COMP_1_TRAIL_सीमा) {
        // trail character is 0..33FF
        // result entry may have 2 or 3 units
        key1 = static_cast<uint16_t>(trail << 1);
        यावत्(key1>(firstUnit=*list)) {
            list+=2+(firstUnit&COMP_1_TRIPLE);
        }
        यदि(key1==(firstUnit&COMP_1_TRAIL_आवरणम्)) {
            यदि(firstUnit&COMP_1_TRIPLE) {
                प्रत्यागम (static_cast<int32_t>(list[1]) << 16) | list[2];
            } अन्यथा {
                प्रत्यागम list[1];
            }
        }
    } अन्यथा {
        // trail character is 3400..10FFFF
        // result entry has 3 units
        key1 = static_cast<uint16_t>(COMP_1_TRAIL_सीमा +
                        (((trail>>COMP_1_TRAIL_सरणम्))&
                          ~COMP_1_TRIPLE));
        uint16_t key2 = static_cast<uint16_t>(trail << COMP_2_TRAIL_सरणम्);
        uint16_t secondUnit;
        प्रति(;;) {
            यदि(key1>(firstUnit=*list)) {
                list+=2+(firstUnit&COMP_1_TRIPLE);
            } अन्यथा यदि(key1==(firstUnit&COMP_1_TRAIL_आवरणम्)) {
                यदि(key2>(secondUnit=list[1])) {
                    यदि(firstUnit&COMP_1_चरमः_TUPLE) {
                        भङ्ग;
                    } अन्यथा {
                        list+=3;
                    }
                } अन्यथा यदि(key2==(secondUnit&COMP_2_TRAIL_आवरणम्)) {
                    प्रत्यागम (static_cast<int32_t>(secondUnit & ~COMP_2_TRAIL_आवरणम्) << 16) | list[2];
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
शून्य Normalizer2Impl::सङ्कलय_Composites(ध्रुव uint16_t *list, Unicode_समुच्चयः &समुच्चयः) ध्रुव {
    uint16_t firstUnit;
    int32_t संयुक्त_And_Fwd;
    कुरु {
        firstUnit=*list;
        यदि((firstUnit&COMP_1_TRIPLE)==0) {
            संयुक्त_And_Fwd=list[1];
            list+=2;
        } अन्यथा {
            संयुक्त_And_Fwd = ((static_cast<int32_t>(list[1]) & ~COMP_2_TRAIL_आवरणम्) << 16) | list[2];
            list+=3;
        }
        UChar32 संयुक्त=संयुक्त_And_Fwd>>1;
        यदि((संयुक्त_And_Fwd&1)!=0) {
            सङ्कलय_Composites(get_Compositions_List_For_संयुक्त(getRawNorm16(संयुक्त)), समुच्चयः);
        }
        समुच्चयः.सङ्कलय(संयुक्त);
    } यावत्((firstUnit&COMP_1_चरमः_TUPLE)==0);
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
शून्य Normalizer2Impl::recompose(Reordering_बफर &बफर, int32_t recompose_Start_सूचकांक,
                                UBool onlyContiguous) ध्रुव {
    char16_t *p=बफर.getStart()+recompose_Start_सूचकांक;
    char16_t *सीमा=बफर.get_सीमा();
    यदि(p==सीमा) {
        प्रत्यागम;
    }

    char16_t *starter, *pRemove, *q, *r;
    ध्रुव uint16_t *compositionsList;
    UChar32 c, संयुक्त_And_Fwd;
    uint16_t norm16;
    uint8_t cc, पूर्व_CC;
    UBool starterIsSupplementary;

    // Some of the following variables are not used until we have a forward-combining starter
    // and are only initialized now to avoid compiler warnings.
    compositionsList=शून्यसूचकः;  // used as indicator for whether we have a forward-combining starter
    starter=शून्यसूचकः;
    starterIsSupplementary=असत्य;
    पूर्व_CC=0;

    प्रति(;;) {
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, p, सीमा, c, norm16);
        cc=getCCFromYesOrMaybeYes(norm16);
        यदि( // this character combines backward and
            isMaybe(norm16) &&
            // we have seen a starter that combines forward and
            compositionsList!=शून्यसूचकः &&
            // the backward-combining character is not blocked
            (पूर्व_CC<cc || पूर्व_CC==0)
        ) {
            यदि(isJamoVT(norm16)) {
                // c is a Jamo V/T, see if we can compose it with the previous character.
                यदि(c<Hangul::JAMO_T_आधारः) {
                    // c is a Jamo Vowel, compose with previous Jamo L and following Jamo T.
                    char16_t पूर्व = static_cast<char16_t>(*starter - Hangul::JAMO_L_आधारः);
                    यदि(पूर्व<Hangul::JAMO_L_गणनम्) {
                        pRemove=p-1;
                        char16_t syllable = static_cast<char16_t>(
                            Hangul::HANGUL_आधारः +
                             (पूर्व*Hangul::JAMO_V_गणनम्+(c-Hangul::JAMO_V_आधारः))*
                             Hangul::JAMO_T_गणनम्);
                        char16_t t;
                        यदि (p != सीमा && (t = static_cast<char16_t>(*p - Hangul::JAMO_T_आधारः)) < Hangul::JAMO_T_गणनम्) {
                            ++p;
                            syllable+=t;  // The next character was a Jamo T.
                        }
                        *starter=syllable;
                        // remove the Jamo V/T
                        q=pRemove;
                        r=p;
                        यावत्(r<सीमा) {
                            *q++=*r++;
                        }
                        सीमा=q;
                        p=pRemove;
                    }
                }
                /*
                 * No "else" for Jamo T:
                 * Since the input is in NFD, there are no Hangul LV syllables that
                 * a Jamo T could combine with.
                 * All Jamo Ts are combined above when handling Jamo Vs.
                 */
                यदि(p==सीमा) {
                    भङ्ग;
                }
                compositionsList=शून्यसूचकः;
                अनुवर्त;
            } अन्यथा यदि((संयुक्त_And_Fwd=combine(compositionsList, c))>=0) {
                // The starter and the combining mark (c) do combine.
                UChar32 संयुक्त=संयुक्त_And_Fwd>>1;

                // Replace the starter with the composite, remove the combining mark.
                pRemove=p-U16_दीर्घता(c);  // pRemove & p: start & limit of the combining mark
                यदि(starterIsSupplementary) {
                    यदि(U_IS_SUPPLEMENTARY(संयुक्त)) {
                        // both are supplementary
                        starter[0]=U16_LEAD(संयुक्त);
                        starter[1]=U16_TRAIL(संयुक्त);
                    } अन्यथा {
                        *starter = static_cast<char16_t>(संयुक्त);
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
                } अन्यथा यदि(U_IS_SUPPLEMENTARY(संयुक्त)) {
                    // The composite is longer than the starter,
                    // move the intermediate characters back one.
                    starterIsSupplementary=सत्य;
                    ++starter;  // temporarily increment for the loop boundary
                    q=pRemove;
                    r=++pRemove;
                    यावत्(starter<q) {
                        *--r=*--q;
                    }
                    *starter=U16_TRAIL(संयुक्त);
                    *--starter=U16_LEAD(संयुक्त);  // undo the temporary increment
                } अन्यथा {
                    // both are on the BMP
                    *starter = static_cast<char16_t>(संयुक्त);
                }

                /* remove the combining mark by moving the following text over it */
                यदि(pRemove<p) {
                    q=pRemove;
                    r=p;
                    यावत्(r<सीमा) {
                        *q++=*r++;
                    }
                    सीमा=q;
                    p=pRemove;
                }
                // Keep prevCC because we removed the combining mark.

                यदि(p==सीमा) {
                    भङ्ग;
                }
                // Is the composite a starter that combines forward?
                यदि(संयुक्त_And_Fwd&1) {
                    compositionsList=
                        get_Compositions_List_For_संयुक्त(getRawNorm16(संयुक्त));
                } अन्यथा {
                    compositionsList=शून्यसूचकः;
                }

                // We combined; continue with looking for compositions.
                अनुवर्त;
            }
        }

        // no combination this time
        पूर्व_CC=cc;
        यदि(p==सीमा) {
            भङ्ग;
        }

        // If c did not combine, then check if it is a starter.
        यदि(cc==0) {
            // Found a new starter.
            यदि((compositionsList=getCompositionsListForDecompYes(norm16))!=शून्यसूचकः) {
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
            compositionsList=शून्यसूचकः;
        }
    }
    बफर.समुच्चयः_Reordering_सीमा(सीमा);
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
            यदि (b < Hangul::JAMO_V_आधारः) {
                प्रत्यागम U_SENTINEL;
            }
            b-=Hangul::JAMO_V_आधारः;
            यदि(b<Hangul::JAMO_V_गणनम्) {
                प्रत्यागम
                    (Hangul::HANGUL_आधारः+
                     ((a-Hangul::JAMO_L_आधारः)*Hangul::JAMO_V_गणनम्+b)*
                     Hangul::JAMO_T_गणनम्);
            } अन्यथा {
                प्रत्यागम U_SENTINEL;
            }
        } अन्यथा यदि(isHangulLV(norm16)) {
            यदि (b <= Hangul::JAMO_T_आधारः) {
               प्रत्यागम U_SENTINEL;
            }
            b-=Hangul::JAMO_T_आधारः;
            यदि(b<Hangul::JAMO_T_गणनम्) {  // not b==0!
                प्रत्यागम a+b;
            } अन्यथा {
                प्रत्यागम U_SENTINEL;
            }
        } अन्यथा {
            // 'a' has a compositions list in extraData
            list=get_दत्त_For_Yes_Or_No(norm16);
            यदि(norm16>minYesNo) {  // composite 'a' has both mapping & compositions list
                list+=  // mapping pointer
                    1+  // +1 to skip the first unit with the mapping length
                    (*list&MAPPING_दीर्घता_आवरणम्);  // + mapping length
            }
        }
    } अन्यथा यदि(norm16<minMaybeNoCombinesFwd || MIN_प्रसामान्यम्_MAYBE_YES<=norm16) {
        प्रत्यागम U_SENTINEL;
    } अन्यथा {
        list=get_दत्त_For_Maybe(norm16);
        यदि(norm16<minMaybeYes) {  // composite 'a' has both mapping & compositions list
            list+=  // mapping pointer
                1+  // +1 to skip the first unit with the mapping length
                (*list&MAPPING_दीर्घता_आवरणम्);  // + mapping length
        }
    }
    यदि(b<0 || 0x10ffff<b) {  // combine(list, b) requires a valid code point b
        प्रत्यागम U_SENTINEL;
    }
#यदि U_SIGNED_दक्षिणः_सरणम्_IS_ARITHMETIC
    प्रत्यागम combine(list, b)>>1;
#अन्यथा
    int32_t संयुक्त_And_Fwd=combine(list, b);
    प्रत्यागम संयुक्त_And_Fwd>=0 ? संयुक्त_And_Fwd>>1 : U_SENTINEL;
#यदि_अन्तः
}

// Very similar to composeQuickCheck(): Make the same changes in both places if relevant.
// doCompose: normalize
// !doCompose: isNormalized (buffer must be empty and initialized)
UBool
Normalizer2Impl::compose(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                         UBool onlyContiguous,
                         UBool doCompose,
                         Reordering_बफर &बफर,
                         UError_संहिता &error_संहिता) ध्रुव {
    ध्रुव char16_t *पूर्व_Boundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    यदि(सीमा==शून्यसूचकः) {
        src=copy_नीचः_उपसर्गः_From_Nul_Terminated(src, minNoMaybeCP,
                                           doCompose ? &बफर : शून्यसूचकः,
                                           error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम असत्य;
        }
        सीमा=u_strchr(src, 0);
        यदि (पूर्व_Boundary != src) {
            यदि (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                पूर्व_Boundary = src;
            } अन्यथा {
                बफर.removeSuffix(1);
                पूर्व_Boundary = --src;
            }
        }
    }

    प्रति (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ध्रुव char16_t *पूर्व_Src;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        प्रति (;;) {
            यदि (src == सीमा) {
                यदि (पूर्व_Boundary != सीमा && doCompose) {
                    बफर.appendZeroCC(पूर्व_Boundary, सीमा, error_संहिता);
                }
                प्रत्यागम सत्य;
            }
            यदि( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा {
                पूर्व_Src = src++;
                यदि(!U16_IS_LEAD(c)) {
                    भङ्ग;
                } अन्यथा {
                    char16_t c2;
                    यदि(src!=सीमा && U16_IS_TRAIL(c2=*src)) {
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
                        hasCompBoundaryBefore(src, सीमा)) {
                    यदि (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, error_संहिता)) {
                        भङ्ग;
                    }
                    यदि(!बफर.append(mapAlgorithmic(c, norm16), 0, error_संहिता)) {
                        भङ्ग;
                    }
                    पूर्व_Boundary = src;
                    अनुवर्त;
                }
            } अन्यथा यदि (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, सीमा)) {
                    यदि (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, error_संहिता)) {
                        भङ्ग;
                    }
                    ध्रुव char16_t *mapping = reinterpret_cast<ध्रुव char16_t *>(get_दत्त_For_Yes_Or_No(norm16));
                    int32_t length = *mapping++ & MAPPING_दीर्घता_आवरणम्;
                    यदि(!बफर.appendZeroCC(mapping, mapping + length, error_संहिता)) {
                        भङ्ग;
                    }
                    पूर्व_Boundary = src;
                    अनुवर्त;
                }
            } अन्यथा यदि (norm16 >= min_No_No_रिक्तानि) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                यदि (hasCompBoundaryBefore(src, सीमा) ||
                        hasCompBoundaryAfter(पूर्व_Boundary, पूर्व_Src, onlyContiguous)) {
                    यदि (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, error_संहिता)) {
                        भङ्ग;
                    }
                    पूर्व_Boundary = src;
                    अनुवर्त;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } अन्यथा यदि (isJamoVT(norm16) && पूर्व_Boundary != पूर्व_Src) {
            char16_t पूर्व=*(पूर्व_Src-1);
            यदि(c<Hangul::JAMO_T_आधारः) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                char16_t l = static_cast<char16_t>(पूर्व - Hangul::JAMO_L_आधारः);
                यदि(l<Hangul::JAMO_L_गणनम्) {
                    यदि (!doCompose) {
                        प्रत्यागम असत्य;
                    }
                    int32_t t;
                    यदि (src != सीमा &&
                            0 < (t = (static_cast<int32_t>(*src) - Hangul::JAMO_T_आधारः)) &&
                            t < Hangul::JAMO_T_गणनम्) {
                        // The next character is a Jamo T.
                        ++src;
                    } अन्यथा यदि (hasCompBoundaryBefore(src, सीमा)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    } अन्यथा {
                        t = -1;
                    }
                    यदि (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_आधारः +
                            (l*Hangul::JAMO_V_गणनम् + (c-Hangul::JAMO_V_आधारः)) *
                            Hangul::JAMO_T_गणनम् + t;
                        --पूर्व_Src;  // Replace the Jamo L as well.
                        यदि (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, error_संहिता)) {
                            भङ्ग;
                        }
                        यदि (!बफर.appendBMP(static_cast<char16_t>(syllable), 0, error_संहिता)) {
                            भङ्ग;
                        }
                        पूर्व_Boundary = src;
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
            } अन्यथा यदि (Hangul::isHangulLV(पूर्व)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                यदि (!doCompose) {
                    प्रत्यागम असत्य;
                }
                UChar32 syllable = पूर्व + c - Hangul::JAMO_T_आधारः;
                --पूर्व_Src;  // Replace the Hangul LV as well.
                यदि (पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, error_संहिता)) {
                    भङ्ग;
                }
                यदि (!बफर.appendBMP(static_cast<char16_t>(syllable), 0, error_संहिता)) {
                    भङ्ग;
                }
                पूर्व_Boundary = src;
                अनुवर्त;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } अन्यथा यदि (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(norm16);  // cc!=0
            यदि (onlyContiguous /* FCC */ && getPreviousTrailCC(पूर्व_Boundary, पूर्व_Src) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                यदि (!doCompose) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ध्रुव char16_t *अग्र_Src;
                uint16_t n16;
                प्रति (;;) {
                    यदि (src == सीमा) {
                        यदि (doCompose) {
                            बफर.appendZeroCC(पूर्व_Boundary, सीमा, error_संहिता);
                        }
                        प्रत्यागम सत्य;
                    }
                    uint8_t पूर्व_CC = cc;
                    अग्र_Src = src;
                    UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, अग्र_Src, सीमा, c, n16);
                    यदि (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(n16);
                        यदि (पूर्व_CC > cc) {
                            यदि (!doCompose) {
                                प्रत्यागम असत्य;
                            }
                            भङ्ग;
                        }
                    } अन्यथा {
                        भङ्ग;
                    }
                    src = अग्र_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                यदि (norm16HasCompBoundaryBefore(n16)) {
                    यदि (isCompYesAndZeroCC(n16)) {
                        src = अग्र_Src;
                    }
                    अनुवर्त;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        यदि (पूर्व_Boundary != पूर्व_Src && !norm16HasCompBoundaryBefore(norm16)) {
            ध्रुव char16_t *p = पूर्व_Src;
            UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, पूर्व_Boundary, p, c, norm16);
            यदि (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                पूर्व_Src = p;
            }
        }
        यदि (doCompose && पूर्व_Boundary != पूर्व_Src && !बफर.appendZeroCC(पूर्व_Boundary, पूर्व_Src, error_संहिता)) {
            भङ्ग;
        }
        int32_t recompose_Start_सूचकांक=बफर.length();
        // We know there is not a boundary here.
        decomposeShort(पूर्व_Src, src, असत्य /* !stopAtCompBoundary */, onlyContiguous,
                       बफर, error_संहिता);
        // Decompose until the next boundary.
        src = decomposeShort(src, सीमा, सत्य /* stopAtCompBoundary */, onlyContiguous,
                             बफर, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) {
            भङ्ग;
        }
        यदि ((src - पूर्व_Src) > INT32_MAX) {  // guard before buffer.equals()
            error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम सत्य;
        }
        recompose(बफर, recompose_Start_सूचकांक, onlyContiguous);
        यदि(!doCompose) {
            यदि(!बफर.equals(पूर्व_Src, src)) {
                प्रत्यागम असत्य;
            }
            बफर.remove();
        }
        पूर्व_Boundary=src;
    }
    प्रत्यागम सत्य;
}

// Very similar to compose(): Make the same changes in both places if relevant.
// pQCResult==nullptr: spanQuickCheckYes
// pQCResult!=nullptr: quickCheck (*pQCResult must be UNORM_YES)
ध्रुव char16_t *
Normalizer2Impl::compose_शीघ्र_Check(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                                   UBool onlyContiguous,
                                   UNormalization_Check_परिणाम *pQCResult) ध्रुव {
    ध्रुव char16_t *पूर्व_Boundary=src;
    UChar32 minNoMaybeCP=minCompNoMaybeCP;
    यदि(सीमा==शून्यसूचकः) {
        UError_संहिता error_संहिता=U_ZERO_ERROR;
        src=copy_नीचः_उपसर्गः_From_Nul_Terminated(src, minNoMaybeCP, शून्यसूचकः, error_संहिता);
        सीमा=u_strchr(src, 0);
        यदि (पूर्व_Boundary != src) {
            यदि (hasCompBoundaryAfter(*(src-1), onlyContiguous)) {
                पूर्व_Boundary = src;
            } अन्यथा {
                पूर्व_Boundary = --src;
            }
        }
    }

    प्रति(;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ध्रुव char16_t *पूर्व_Src;
        UChar32 c = 0;
        uint16_t norm16 = 0;
        प्रति (;;) {
            यदि(src==सीमा) {
                प्रत्यागम src;
            }
            यदि( (c=*src)<minNoMaybeCP ||
                isCompYesAndZeroCC(norm16=UCPTRIE_FAST_BMP_GET(normTrie, UCPTRIE_16, c))
            ) {
                ++src;
            } अन्यथा {
                पूर्व_Src = src++;
                यदि(!U16_IS_LEAD(c)) {
                    भङ्ग;
                } अन्यथा {
                    char16_t c2;
                    यदि(src!=सीमा && U16_IS_TRAIL(c2=*src)) {
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

        uint16_t पूर्व_Norm16 = INERT;
        यदि (पूर्व_Boundary != पूर्व_Src) {
            यदि (norm16HasCompBoundaryBefore(norm16)) {
                पूर्व_Boundary = पूर्व_Src;
            } अन्यथा {
                ध्रुव char16_t *p = पूर्व_Src;
                uint16_t n16;
                UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, पूर्व_Boundary, p, c, n16);
                यदि (norm16HasCompBoundaryAfter(n16, onlyContiguous)) {
                    पूर्व_Boundary = पूर्व_Src;
                } अन्यथा {
                    पूर्व_Boundary = p;
                    पूर्व_Norm16 = n16;
                }
            }
        }

        यदि (norm16 >= minMaybeNo) {
            uint16_t fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
            uint8_t cc = fcd16 >> 8;
            यदि (onlyContiguous /* FCC */ && cc != 0 &&
                    getTrailCCFromCompYesAndZeroCC(पूर्व_Norm16) > cc) {
                // The [prevBoundary..prevSrc[ character
                // passed the quick check "yes && ccc==0" test
                // but is out of canonical order with the current combining mark.
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ध्रुव char16_t *अग्र_Src;
                प्रति (;;) {
                    यदि (norm16 < MIN_YES_YES_WITH_CC) {
                        यदि (pQCResult != शून्यसूचकः) {
                            *pQCResult = UNORM_MAYBE;
                        } अन्यथा {
                            प्रत्यागम पूर्व_Boundary;
                        }
                    }
                    यदि (src == सीमा) {
                        प्रत्यागम src;
                    }
                    uint8_t पूर्व_CC = fcd16;
                    अग्र_Src = src;
                    UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, अग्र_Src, सीमा, c, norm16);
                    यदि (norm16 >= minMaybeNo) {
                        fcd16 = getFCD16FromMaybeOrNonZeroCC(norm16);
                        cc = fcd16 >> 8;
                        यदि (!(पूर्व_CC <= cc || cc == 0)) {
                            भङ्ग;
                        }
                    } अन्यथा {
                        भङ्ग;
                    }
                    src = अग्र_Src;
                }
                // src is after the last in-order combining mark.
                यदि (isCompYesAndZeroCC(norm16)) {
                    पूर्व_Boundary = src;
                    src = अग्र_Src;
                    अनुवर्त;
                }
            }
        }
        यदि(pQCResult!=शून्यसूचकः) {
            *pQCResult=UNORM_NO;
        }
        प्रत्यागम पूर्व_Boundary;
    }
}

शून्य Normalizer2Impl::composeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString &safe_मध्यः,
                                       Reordering_बफर &बफर,
                                       UError_संहिता &error_संहिता) ध्रुव {
    यदि(!बफर.is_रिक्तानि()) {
        ध्रुव char16_t *first_Starter_अन्तर्_Src=find_अग्र_Comp_Boundary(src, सीमा, onlyContiguous);
        यदि(src!=first_Starter_अन्तर्_Src) {
            ध्रुव char16_t *चरमः_Starter_अन्तर्_Dest=findPreviousCompBoundary(बफर.getStart(),
                                                                    बफर.get_सीमा(), onlyContiguous);
            int32_t dest_Suffix_दीर्घता = static_cast<int32_t>(बफर.get_सीमा() - चरमः_Starter_अन्तर्_Dest);
            UnicodeString मध्यः(चरमः_Starter_अन्तर्_Dest, dest_Suffix_दीर्घता);
            बफर.removeSuffix(dest_Suffix_दीर्घता);
            safe_मध्यः=मध्यः;
            मध्यः.append(src, static_cast<int32_t>(first_Starter_अन्तर्_Src - src));
            ध्रुव char16_t *मध्यः_Start=मध्यः.get_बफर();
            compose(मध्यः_Start, मध्यः_Start+मध्यः.length(), onlyContiguous,
                    सत्य, बफर, error_संहिता);
            यदि(U_FAILURE(error_संहिता)) {
                प्रत्यागम;
            }
            src=first_Starter_अन्तर्_Src;
        }
    }
    यदि(doCompose) {
        compose(src, सीमा, onlyContiguous, सत्य, बफर, error_संहिता);
    } अन्यथा {
        यदि(सीमा==शून्यसूचकः) {  // appendZeroCC() needs limit!=nullptr
            सीमा=u_strchr(src, 0);
        }
        बफर.appendZeroCC(src, सीमा, error_संहिता);
    }
}

UBool
Normalizer2Impl::composeUTF8(uint32_t options, UBool onlyContiguous,
                             ध्रुव uint8_t *src, ध्रुव uint8_t *सीमा,
                             ByteSink *sink, Edits *edits, UError_संहिता &error_संहिता) ध्रुव {
    U_ASSERT(सीमा != शून्यसूचकः);
    UnicodeString s16;
    uint8_t minNoMaybeLead = leadByteForCP(minCompNoMaybeCP);
    ध्रुव uint8_t *पूर्व_Boundary = src;

    प्रति (;;) {
        // Fast path: Scan over a sequence of characters below the minimum "no or maybe" code point,
        // or with (compYes && ccc==0) properties.
        ध्रुव uint8_t *पूर्व_Src;
        uint16_t norm16 = 0;
        प्रति (;;) {
            यदि (src == सीमा) {
                यदि (पूर्व_Boundary != सीमा && sink != शून्यसूचकः) {
                    ByteSinkUtil::appendUnchanged(पूर्व_Boundary, सीमा,
                                                  *sink, options, edits, error_संहिता);
                }
                प्रत्यागम सत्य;
            }
            यदि (*src < minNoMaybeLead) {
                ++src;
            } अन्यथा {
                पूर्व_Src = src;
                UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, src, सीमा, norm16);
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
            यदि (sink == शून्यसूचकः) {
                प्रत्यागम असत्य;
            }
            // Fast path for mapping a character that is immediately surrounded by boundaries.
            // In this case, we need not decompose around the current character.
            यदि (isDecompNoAlgorithmic(norm16)) {
                // Maps to a single isCompYesAndZeroCC character
                // which also implies hasCompBoundaryBefore.
                यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, सीमा)) {
                    यदि (पूर्व_Boundary != पूर्व_Src &&
                            !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                           *sink, options, edits, error_संहिता)) {
                        भङ्ग;
                    }
                    append_संहिता_Point_Delta(पूर्व_Src, src, getAlgorithmicDelta(norm16), *sink, edits);
                    पूर्व_Boundary = src;
                    अनुवर्त;
                }
            } अन्यथा यदि (norm16 < minNoNoCompBoundaryBefore) {
                // The mapping is comp-normalized which also implies hasCompBoundaryBefore.
                यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous) ||
                        hasCompBoundaryBefore(src, सीमा)) {
                    यदि (पूर्व_Boundary != पूर्व_Src &&
                            !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                           *sink, options, edits, error_संहिता)) {
                        भङ्ग;
                    }
                    ध्रुव uint16_t *mapping = get_दत्त_For_Yes_Or_No(norm16);
                    int32_t length = *mapping++ & MAPPING_दीर्घता_आवरणम्;
                    यदि (!ByteSinkUtil::appendChange(पूर्व_Src, src, reinterpret_cast<ध्रुव char16_t*>(mapping), length,
                                                    *sink, edits, error_संहिता)) {
                        भङ्ग;
                    }
                    पूर्व_Boundary = src;
                    अनुवर्त;
                }
            } अन्यथा यदि (norm16 >= min_No_No_रिक्तानि) {
                // The current character maps to nothing.
                // Simply omit it from the output if there is a boundary before _or_ after it.
                // The character itself implies no boundaries.
                यदि (hasCompBoundaryBefore(src, सीमा) ||
                        hasCompBoundaryAfter(पूर्व_Boundary, पूर्व_Src, onlyContiguous)) {
                    यदि (पूर्व_Boundary != पूर्व_Src &&
                            !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                           *sink, options, edits, error_संहिता)) {
                        भङ्ग;
                    }
                    यदि (edits != शून्यसूचकः) {
                        edits->सङ्कलय_Replace(static_cast<int32_t>(src - पूर्व_Src), 0);
                    }
                    पूर्व_Boundary = src;
                    अनुवर्त;
                }
            }
            // Other "noNo" type, or need to examine more text around this character:
            // Fall through to the slow path.
        } अन्यथा यदि (isJamoVT(norm16)) {
            // Jamo L: E1 84 80..92
            // Jamo V: E1 85 A1..B5
            // Jamo T: E1 86 A8..E1 87 82
            U_ASSERT((src - पूर्व_Src) == 3 && *पूर्व_Src == 0xe1);
            UChar32 पूर्व = previousHangulOrJamo(पूर्व_Boundary, पूर्व_Src);
            यदि (पूर्व_Src[1] == 0x85) {
                // The current character is a Jamo Vowel,
                // compose with previous Jamo L and following Jamo T.
                UChar32 l = पूर्व - Hangul::JAMO_L_आधारः;
                यदि (static_cast<uint32_t>(l) < Hangul::JAMO_L_गणनम्) {
                    यदि (sink == शून्यसूचकः) {
                        प्रत्यागम असत्य;
                    }
                    int32_t t = get_Jamo_TMinus_आधारः(src, सीमा);
                    यदि (t >= 0) {
                        // The next character is a Jamo T.
                        src += 3;
                    } अन्यथा यदि (hasCompBoundaryBefore(src, सीमा)) {
                        // No Jamo T follows, not even via decomposition.
                        t = 0;
                    }
                    यदि (t >= 0) {
                        UChar32 syllable = Hangul::HANGUL_आधारः +
                            (l*Hangul::JAMO_V_गणनम् + (पूर्व_Src[2]-0xa1)) *
                            Hangul::JAMO_T_गणनम् + t;
                        पूर्व_Src -= 3;  // Replace the Jamo L as well.
                        यदि (पूर्व_Boundary != पूर्व_Src &&
                                !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                               *sink, options, edits, error_संहिता)) {
                            भङ्ग;
                        }
                        ByteSinkUtil::append_संहिता_Point(पूर्व_Src, src, syllable, *sink, edits);
                        पूर्व_Boundary = src;
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
            } अन्यथा यदि (Hangul::isHangulLV(पूर्व)) {
                // The current character is a Jamo Trailing consonant,
                // compose with previous Hangul LV that does not contain a Jamo T.
                यदि (sink == शून्यसूचकः) {
                    प्रत्यागम असत्य;
                }
                UChar32 syllable = पूर्व + get_Jamo_TMinus_आधारः(पूर्व_Src, src);
                पूर्व_Src -= 3;  // Replace the Hangul LV as well.
                यदि (पूर्व_Boundary != पूर्व_Src &&
                        !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                       *sink, options, edits, error_संहिता)) {
                    भङ्ग;
                }
                ByteSinkUtil::append_संहिता_Point(पूर्व_Src, src, syllable, *sink, edits);
                पूर्व_Boundary = src;
                अनुवर्त;
            }
            // No matching context, or may need to decompose surrounding text first:
            // Fall through to the slow path.
        } अन्यथा यदि (norm16 > JAMO_VT) {  // norm16 >= MIN_YES_YES_WITH_CC
            // One or more combining marks that do not combine-back:
            // Check for canonical order, copy unchanged if ok and
            // if followed by a character with a boundary-before.
            uint8_t cc = get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(norm16);  // cc!=0
            यदि (onlyContiguous /* FCC */ && getPreviousTrailCC(पूर्व_Boundary, पूर्व_Src) > cc) {
                // Fails FCD test, need to decompose and contiguously recompose.
                यदि (sink == शून्यसूचकः) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                // If !onlyContiguous (not FCC), then we ignore the tccc of
                // the previous character which passed the quick check "yes && ccc==0" test.
                ध्रुव uint8_t *अग्र_Src;
                uint16_t n16;
                प्रति (;;) {
                    यदि (src == सीमा) {
                        यदि (sink != शून्यसूचकः) {
                            ByteSinkUtil::appendUnchanged(पूर्व_Boundary, सीमा,
                                                          *sink, options, edits, error_संहिता);
                        }
                        प्रत्यागम सत्य;
                    }
                    uint8_t पूर्व_CC = cc;
                    अग्र_Src = src;
                    UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, अग्र_Src, सीमा, n16);
                    यदि (n16 >= MIN_YES_YES_WITH_CC) {
                        cc = get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(n16);
                        यदि (पूर्व_CC > cc) {
                            यदि (sink == शून्यसूचकः) {
                                प्रत्यागम असत्य;
                            }
                            भङ्ग;
                        }
                    } अन्यथा {
                        भङ्ग;
                    }
                    src = अग्र_Src;
                }
                // src is after the last in-order combining mark.
                // If there is a boundary here, then we continue with no change.
                यदि (norm16HasCompBoundaryBefore(n16)) {
                    यदि (isCompYesAndZeroCC(n16)) {
                        src = अग्र_Src;
                    }
                    अनुवर्त;
                }
                // Use the slow path. There is no boundary in [prevSrc, src[.
            }
        }

        // Slow path: Find the nearest boundaries around the current character,
        // decompose and recompose.
        यदि (पूर्व_Boundary != पूर्व_Src && !norm16HasCompBoundaryBefore(norm16)) {
            ध्रुव uint8_t *p = पूर्व_Src;
            UCPTRIE_FAST_U8_पूर्व(normTrie, UCPTRIE_16, पूर्व_Boundary, p, norm16);
            यदि (!norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
                पूर्व_Src = p;
            }
        }
        Reordering_बफर बफर(*एतत्, s16, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) {
            भङ्ग;
        }
        // We know there is not a boundary here.
        decomposeShort(पूर्व_Src, src, STOP_AT_सीमा, onlyContiguous,
                       बफर, error_संहिता);
        // Decompose until the next boundary.
        src = decomposeShort(src, सीमा, STOP_AT_COMP_BOUNDARY, onlyContiguous,
                             बफर, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) {
            भङ्ग;
        }
        यदि ((src - पूर्व_Src) > INT32_MAX) {  // guard before buffer.equals()
            error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम सत्य;
        }
        recompose(बफर, 0, onlyContiguous);
        यदि (!बफर.equals(पूर्व_Src, src)) {
            यदि (sink == शून्यसूचकः) {
                प्रत्यागम असत्य;
            }
            यदि (पूर्व_Boundary != पूर्व_Src &&
                    !ByteSinkUtil::appendUnchanged(पूर्व_Boundary, पूर्व_Src,
                                                   *sink, options, edits, error_संहिता)) {
                भङ्ग;
            }
            यदि (!ByteSinkUtil::appendChange(पूर्व_Src, src, बफर.getStart(), बफर.length(),
                                            *sink, edits, error_संहिता)) {
                भङ्ग;
            }
            पूर्व_Boundary = src;
        }
    }
    प्रत्यागम सत्य;
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ध्रुव char16_t *src, ध्रुव char16_t *सीमा) ध्रुव {
    यदि (src == सीमा || *src < minCompNoMaybeCP) {
        प्रत्यागम सत्य;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, src, सीमा, c, norm16);
    प्रत्यागम norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryBefore(ध्रुव uint8_t *src, ध्रुव uint8_t *सीमा) ध्रुव {
    यदि (src == सीमा) {
        प्रत्यागम सत्य;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_अग्र(normTrie, UCPTRIE_16, src, सीमा, norm16);
    प्रत्यागम norm16HasCompBoundaryBefore(norm16);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ध्रुव char16_t *start, ध्रुव char16_t *p,
                                            UBool onlyContiguous) ध्रुव {
    यदि (start == p) {
        प्रत्यागम सत्य;
    }
    UChar32 c;
    uint16_t norm16;
    UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, start, p, c, norm16);
    प्रत्यागम norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

UBool Normalizer2Impl::hasCompBoundaryAfter(ध्रुव uint8_t *start, ध्रुव uint8_t *p,
                                            UBool onlyContiguous) ध्रुव {
    यदि (start == p) {
        प्रत्यागम सत्य;
    }
    uint16_t norm16;
    UCPTRIE_FAST_U8_पूर्व(normTrie, UCPTRIE_16, start, p, norm16);
    प्रत्यागम norm16HasCompBoundaryAfter(norm16, onlyContiguous);
}

ध्रुव char16_t *Normalizer2Impl::findPreviousCompBoundary(ध्रुव char16_t *start, ध्रुव char16_t *p,
                                                       UBool onlyContiguous) ध्रुव {
    यावत् (p != start) {
        ध्रुव char16_t *संहिता_Point_सीमा = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, start, p, c, norm16);
        यदि (norm16HasCompBoundaryAfter(norm16, onlyContiguous)) {
            प्रत्यागम संहिता_Point_सीमा;
        }
        यदि (hasCompBoundaryBefore(c, norm16)) {
            प्रत्यागम p;
        }
    }
    प्रत्यागम p;
}

ध्रुव char16_t *Normalizer2Impl::find_अग्र_Comp_Boundary(ध्रुव char16_t *p, ध्रुव char16_t *सीमा,
                                                   UBool onlyContiguous) ध्रुव {
    यावत् (p != सीमा) {
        ध्रुव char16_t *संहिता_Point_Start = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, p, सीमा, c, norm16);
        यदि (hasCompBoundaryBefore(c, norm16)) {
            प्रत्यागम संहिता_Point_Start;
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
    U16_पूर्व(start, 0, i, c);
    प्रत्यागम static_cast<uint8_t>(getFCD16(c));
}

uint8_t Normalizer2Impl::getPreviousTrailCC(ध्रुव uint8_t *start, ध्रुव uint8_t *p) ध्रुव {
    यदि (start == p) {
        प्रत्यागम 0;
    }
    int32_t i = static_cast<int32_t>(p - start);
    UChar32 c;
    U8_पूर्व(start, 0, i, c);
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
#निर्देशः optimize( "", off )
#यदि_अन्तः
// Gets the FCD value from the regular normalization data.
uint16_t Normalizer2Impl::get_FCD16_From_Norm_दत्त(UChar32 c) ध्रुव {
    uint16_t norm16=getNorm16(c);
    यदि (norm16 >= सीमा_No_No) {
        यदि(norm16>=MIN_प्रसामान्यम्_MAYBE_YES) {
            // combining mark
            norm16=get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(norm16);
            प्रत्यागम norm16|(norm16<<8);
        } अन्यथा यदि(norm16>=minMaybeYes) {
            प्रत्यागम 0;
        } अन्यथा यदि(norm16<minMaybeNo) {  // isDecompNoAlgorithmic(norm16)
            uint16_t deltaTrailCC = norm16 & DELTA_TCCC_आवरणम्;
            यदि (deltaTrailCC <= DELTA_TCCC_1) {
                प्रत्यागम deltaTrailCC >> विस्थापनम्_सरणम्;
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
    ध्रुव uint16_t *mapping=get_दत्त(norm16);
    uint16_t firstUnit=*mapping;
    norm16=firstUnit>>8;  // tccc
    यदि(firstUnit&MAPPING_HAS_CCC_LCCC_WORD) {
        norm16|=*(mapping-1)&0xff00;  // lccc
    }
    प्रत्यागम norm16;
}
#यदि (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#निर्देशः optimize( "", on )
#यदि_अन्तः

uint16_t Normalizer2Impl::getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ध्रुव {
    U_ASSERT(norm16 >= minMaybeNo);
    यदि (norm16 >= MIN_प्रसामान्यम्_MAYBE_YES) {
        // combining mark
        norm16 = get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(norm16);
        प्रत्यागम norm16 | (norm16<<8);
    } अन्यथा यदि (norm16 >= minMaybeYes) {
        प्रत्यागम 0;
    }
    // c decomposes, get everything from the variable-length extra data
    ध्रुव uint16_t *mapping = get_दत्त_For_Maybe(norm16);
    uint16_t firstUnit = *mapping;
    // maybeNo has lccc = 0
    U_ASSERT((firstUnit & MAPPING_HAS_CCC_LCCC_WORD) == 0 || (*(mapping - 1) & 0xff00) == 0);
    प्रत्यागम firstUnit >> 8;  // tccc
}

// Dual functionality:
// buffer!=nullptr: normalize
// buffer==nullptr: isNormalized/quickCheck/spanQuickCheckYes
ध्रुव char16_t *
Normalizer2Impl::makeFCD(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                         Reordering_बफर *बफर,
                         UError_संहिता &error_संहिता) ध्रुव {
    // Tracks the last FCD-safe boundary, before lccc=0 or after properly-ordered tccc<=1.
    // Similar to the prevBoundary in the compose() implementation.
    ध्रुव char16_t *पूर्व_Boundary=src;
    int32_t पूर्व_FCD16=0;
    यदि(सीमा==शून्यसूचकः) {
        src=copy_नीचः_उपसर्गः_From_Nul_Terminated(src, minLcccCP, बफर, error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम src;
        }
        यदि(पूर्व_Boundary<src) {
            पूर्व_Boundary=src;
            // We know that the previous character's lccc==0.
            // Fetching the fcd16 value was deferred for this below-U+0300 code point.
            पूर्व_FCD16=getFCD16(*(src-1));
            यदि(पूर्व_FCD16>1) {
                --पूर्व_Boundary;
            }
        }
        सीमा=u_strchr(src, 0);
    }

    // Note: In this function we use buffer->appendZeroCC() because we track
    // the lead and trail combining classes here, rather than leaving it to
    // the ReorderingBuffer.
    // The exception is the call to decomposeShort() which uses the buffer
    // in the normal way.

    ध्रुव char16_t *पूर्व_Src;
    UChar32 c=0;
    uint16_t fcd16=0;

    प्रति(;;) {
        // count code units with lccc==0
        प्रति(पूर्व_Src=src; src!=सीमा;) {
            यदि((c=*src)<minLcccCP) {
                पूर्व_FCD16=~c;
                ++src;
            } अन्यथा यदि(!singleLeadMightHaveNonZeroFCD16(c)) {
                पूर्व_FCD16=0;
                ++src;
            } अन्यथा {
                यदि(U16_IS_LEAD(c)) {
                    char16_t c2;
                    यदि((src+1)!=सीमा && U16_IS_TRAIL(c2=src[1])) {
                        c=U16_GET_SUPPLEMENTARY(c, c2);
                    }
                }
                यदि((fcd16=get_FCD16_From_Norm_दत्त(c))<=0xff) {
                    पूर्व_FCD16=fcd16;
                    src+=U16_दीर्घता(c);
                } अन्यथा {
                    भङ्ग;
                }
            }
        }
        // copy these code units all at once
        यदि(src!=पूर्व_Src) {
            यदि(बफर!=शून्यसूचकः && !बफर->appendZeroCC(पूर्व_Src, src, error_संहिता)) {
                भङ्ग;
            }
            यदि(src==सीमा) {
                भङ्ग;
            }
            पूर्व_Boundary=src;
            // We know that the previous character's lccc==0.
            यदि(पूर्व_FCD16<0) {
                // Fetching the fcd16 value was deferred for this below-minLcccCP code point.
                UChar32 पूर्व=~पूर्व_FCD16;
                यदि(पूर्व<minDecompNoCP) {
                    पूर्व_FCD16=0;
                } अन्यथा {
                    पूर्व_FCD16=get_FCD16_From_Norm_दत्त(पूर्व);
                    यदि(पूर्व_FCD16>1) {
                        --पूर्व_Boundary;
                    }
                }
            } अन्यथा {
                ध्रुव char16_t *p=src-1;
                यदि(U16_IS_TRAIL(*p) && पूर्व_Src<p && U16_IS_LEAD(*(p-1))) {
                    --p;
                    // Need to fetch the previous character's FCD value because
                    // prevFCD16 was just for the trail surrogate code point.
                    पूर्व_FCD16=get_FCD16_From_Norm_दत्त(U16_GET_SUPPLEMENTARY(p[0], p[1]));
                    // Still known to have lccc==0 because its lead surrogate unit had lccc==0.
                }
                यदि(पूर्व_FCD16>1) {
                    पूर्व_Boundary=p;
                }
            }
            // The start of the current character (c).
            पूर्व_Src=src;
        } अन्यथा यदि(src==सीमा) {
            भङ्ग;
        }

        src+=U16_दीर्घता(c);
        // The current character (c) at [prevSrc..src[ has a non-zero lead combining class.
        // Check for proper order, and decompose locally if necessary.
        यदि((पूर्व_FCD16&0xff)<=(fcd16>>8)) {
            // proper order: prev tccc <= current lccc
            यदि((fcd16&0xff)<=1) {
                पूर्व_Boundary=src;
            }
            यदि(बफर!=शून्यसूचकः && !बफर->appendZeroCC(c, error_संहिता)) {
                भङ्ग;
            }
            पूर्व_FCD16=fcd16;
            अनुवर्त;
        } अन्यथा यदि(बफर==शून्यसूचकः) {
            प्रत्यागम पूर्व_Boundary;  // quick check "no"
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
            src=find_अग्र_FCDBoundary(src, सीमा);
            /*
             * The source text does not fulfill the conditions for FCD.
             * Decompose and reorder a limited piece of the text.
             */
            decomposeShort(पूर्व_Boundary, src, असत्य, असत्य, *बफर, error_संहिता);
            यदि (U_FAILURE(error_संहिता)) {
                भङ्ग;
            }
            पूर्व_Boundary=src;
            पूर्व_FCD16=0;
        }
    }
    प्रत्यागम src;
}

शून्य Normalizer2Impl::makeFCDAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                                       UBool doMakeFCD,
                                       UnicodeString &safe_मध्यः,
                                       Reordering_बफर &बफर,
                                       UError_संहिता &error_संहिता) ध्रुव {
    यदि(!बफर.is_रिक्तानि()) {
        ध्रुव char16_t *first_Boundary_अन्तर्_Src=find_अग्र_FCDBoundary(src, सीमा);
        यदि(src!=first_Boundary_अन्तर्_Src) {
            ध्रुव char16_t *चरमः_Boundary_अन्तर्_Dest=findPreviousFCDBoundary(बफर.getStart(),
                                                                    बफर.get_सीमा());
            int32_t dest_Suffix_दीर्घता = static_cast<int32_t>(बफर.get_सीमा() - चरमः_Boundary_अन्तर्_Dest);
            UnicodeString मध्यः(चरमः_Boundary_अन्तर्_Dest, dest_Suffix_दीर्घता);
            बफर.removeSuffix(dest_Suffix_दीर्घता);
            safe_मध्यः=मध्यः;
            मध्यः.append(src, static_cast<int32_t>(first_Boundary_अन्तर्_Src - src));
            ध्रुव char16_t *मध्यः_Start=मध्यः.get_बफर();
            makeFCD(मध्यः_Start, मध्यः_Start+मध्यः.length(), &बफर, error_संहिता);
            यदि(U_FAILURE(error_संहिता)) {
                प्रत्यागम;
            }
            src=first_Boundary_अन्तर्_Src;
        }
    }
    यदि(doMakeFCD) {
        makeFCD(src, सीमा, &बफर, error_संहिता);
    } अन्यथा {
        यदि(सीमा==शून्यसूचकः) {  // appendZeroCC() needs limit!=nullptr
            सीमा=u_strchr(src, 0);
        }
        बफर.appendZeroCC(src, सीमा, error_संहिता);
    }
}

ध्रुव char16_t *Normalizer2Impl::findPreviousFCDBoundary(ध्रुव char16_t *start, ध्रुव char16_t *p) ध्रुव {
    यावत्(start<p) {
        ध्रुव char16_t *संहिता_Point_सीमा = p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_पूर्व(normTrie, UCPTRIE_16, start, p, c, norm16);
        यदि (c < minDecompNoCP || norm16HasDecompBoundaryAfter(norm16)) {
            प्रत्यागम संहिता_Point_सीमा;
        }
        यदि (norm16HasDecompBoundaryBefore(norm16)) {
            प्रत्यागम p;
        }
    }
    प्रत्यागम p;
}

ध्रुव char16_t *Normalizer2Impl::find_अग्र_FCDBoundary(ध्रुव char16_t *p, ध्रुव char16_t *सीमा) ध्रुव {
    यावत्(p<सीमा) {
        ध्रुव char16_t *संहिता_Point_Start=p;
        UChar32 c;
        uint16_t norm16;
        UCPTRIE_FAST_U16_अग्र(normTrie, UCPTRIE_16, p, सीमा, c, norm16);
        यदि (c < minLcccCP || norm16HasDecompBoundaryBefore(norm16)) {
            प्रत्यागम संहिता_Point_Start;
        }
        यदि (norm16HasDecompBoundaryAfter(norm16)) {
            प्रत्यागम p;
        }
    }
    प्रत्यागम p;
}

// CanonicalIterator data -------------------------------------------------- ***

Canon_Iter_दत्त::Canon_Iter_दत्त(UError_संहिता &error_संहिता) :
        mutableTrie(umutablecptrie_open(0, 0, &error_संहिता)), trie(शून्यसूचकः),
        canonStartSets(uprv_deleteUObject, शून्यसूचकः, error_संहिता) {}

Canon_Iter_दत्त::~Canon_Iter_दत्त() {
    umutablecptrie_close(mutableTrie);
    ucptrie_close(trie);
}

शून्य Canon_Iter_दत्त::सङ्कलय_गन्तव्यम्_Start_समुच्चयः(UChar32 origin, UChar32 decompLead, UError_संहिता &error_संहिता) {
    uint32_t canon_मानम् = umutablecptrie_get(mutableTrie, decompLead);
    यदि((canon_मानम्&(CANON_HAS_समुच्चयः|CANON_मानम्_आवरणम्))==0 && origin!=0) {
        // origin is the first character whose decomposition starts with
        // the character for which we are setting the value.
        umutablecptrie_समुच्चयः(mutableTrie, decompLead, canon_मानम्|origin, &error_संहिता);
    } अन्यथा {
        // origin is not the first character, or it is U+0000.
        Unicode_समुच्चयः *समुच्चयः;
        यदि((canon_मानम्&CANON_HAS_समुच्चयः)==0) {
            LocalPointer<Unicode_समुच्चयः> lp_समुच्चयः(नूतन Unicode_समुच्चयः, error_संहिता);
            समुच्चयः=lp_समुच्चयः.getAlias();
            यदि(U_FAILURE(error_संहिता)) {
                प्रत्यागम;
            }
            UChar32 firstOrigin = static_cast<UChar32>(canon_मानम् & CANON_मानम्_आवरणम्);
            canon_मानम् = (canon_मानम् & ~CANON_मानम्_आवरणम्) | CANON_HAS_समुच्चयः | static_cast<uint32_t>(canonStartSets.size());
            umutablecptrie_समुच्चयः(mutableTrie, decompLead, canon_मानम्, &error_संहिता);
            canonStartSets.adoptElement(lp_समुच्चयः.orphan(), error_संहिता);
            यदि (U_FAILURE(error_संहिता)) {
                प्रत्यागम;
            }
            यदि(firstOrigin!=0) {
                समुच्चयः->सङ्कलय(firstOrigin);
            }
        } अन्यथा {
            समुच्चयः = static_cast<Unicode_समुच्चयः*>(canonStartSets[static_cast<int32_t>(canon_मानम् & CANON_मानम्_आवरणम्)]);
        }
        समुच्चयः->सङ्कलय(origin);
    }
}

// C++ class for friend access to private Normalizer2Impl members.
वर्ग Init_Canon_Iter_दत्त {
सार्वजनिक:
    स्थिर शून्य doInit(Normalizer2Impl *impl, UError_संहिता &error_संहिता);
};

U_CDECL_BEGIN

// UInitOnce instantiation function for CanonIterData
स्थिर शून्य U_CALLCONV
init_Canon_Iter_दत्त(Normalizer2Impl *impl, UError_संहिता &error_संहिता) {
    Init_Canon_Iter_दत्त::doInit(impl, error_संहिता);
}

U_CDECL_अन्तः

शून्य Init_Canon_Iter_दत्त::doInit(Normalizer2Impl *impl, UError_संहिता &error_संहिता) {
    U_ASSERT(impl->f_Canon_Iter_दत्त == शून्यसूचकः);
    impl->f_Canon_Iter_दत्त = नूतन Canon_Iter_दत्त(error_संहिता);
    यदि (impl->f_Canon_Iter_दत्त == शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (U_SUCCESS(error_संहिता)) {
        UChar32 start = 0, अन्तः;
        uint32_t value;
        यावत् ((अन्तः = ucptrie_getRange(impl->normTrie, start,
                                       UCPMAP_RANGE_FIXED_LEAD_SURROGATES, Normalizer2Impl::INERT,
                                       शून्यसूचकः, शून्यसूचकः, &value)) >= 0) {
            // Call Normalizer2Impl::makeCanonIterDataFromNorm16() for a range of same-norm16 characters.
            यदि (value != Normalizer2Impl::INERT) {
                impl->make_Canon_Iter_दत्त_From_Norm16(start, अन्तः, value, *impl->f_Canon_Iter_दत्त, error_संहिता);
            }
            start = अन्तः + 1;
        }
#यदि_परिभाषितम् UCPTRIE_DEBUG
        umutablecptrie_समुच्चयः_Name(impl->f_Canon_Iter_दत्त->mutableTrie, "CanonIterData");
#यदि_अन्तः
        impl->f_Canon_Iter_दत्त->trie = umutablecptrie_buildImmutable(
            impl->f_Canon_Iter_दत्त->mutableTrie, UCPTRIE_TYPE_SMALL, UCPTRIE_मानम्_द्वयङ्काः_32, &error_संहिता);
        umutablecptrie_close(impl->f_Canon_Iter_दत्त->mutableTrie);
        impl->f_Canon_Iter_दत्त->mutableTrie = शून्यसूचकः;
    }
    यदि (U_FAILURE(error_संहिता)) {
        विलोपय impl->f_Canon_Iter_दत्त;
        impl->f_Canon_Iter_दत्त = शून्यसूचकः;
    }
}

शून्य Normalizer2Impl::make_Canon_Iter_दत्त_From_Norm16(UChar32 start, UChar32 अन्तः, ध्रुव uint16_t norm16,
                                                  Canon_Iter_दत्त &new_दत्त,
                                                  UError_संहिता &error_संहिता) ध्रुव {
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
    प्रति(UChar32 c=start; c<=अन्तः; ++c) {
        uint32_t old_मानम् = umutablecptrie_get(new_दत्त.mutableTrie, c);
        uint32_t new_मानम्=old_मानम्;
        यदि(isMaybeYesOrNonZeroCC(norm16)) {
            // not a segment starter if it occurs in a decomposition or has cc!=0
            new_मानम्|=CANON_NOT_SEGMENT_STARTER;
            यदि(norm16<MIN_प्रसामान्यम्_MAYBE_YES) {
                new_मानम्|=CANON_HAS_COMPOSITIONS;
            }
        } अन्यथा यदि(norm16<minYesNo) {
            new_मानम्|=CANON_HAS_COMPOSITIONS;
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
                ध्रुव uint16_t *mapping=get_दत्त_For_Yes_Or_No(norm16_2);
                uint16_t firstUnit=*mapping;
                int32_t length=firstUnit&MAPPING_दीर्घता_आवरणम्;
                यदि((firstUnit&MAPPING_HAS_CCC_LCCC_WORD)!=0) {
                    यदि(c==c2 && (*(mapping-1)&0xff)!=0) {
                        new_मानम्|=CANON_NOT_SEGMENT_STARTER;  // original c has cc!=0
                    }
                }
                // Skip empty mappings (no characters in the decomposition).
                यदि(length!=0) {
                    ++mapping;  // skip over the firstUnit
                    // add c to first code point's start set
                    int32_t i=0;
                    U16_अग्र_UNSAFE(mapping, i, c2);
                    new_दत्त.सङ्कलय_गन्तव्यम्_Start_समुच्चयः(c, c2, error_संहिता);
                    // Set CANON_NOT_SEGMENT_STARTER for each remaining code point of a
                    // one-way mapping. A 2-way mapping is possible here after
                    // intermediate algorithmic mapping.
                    यदि(norm16_2>=minNoNo) {
                        यावत्(i<length) {
                            U16_अग्र_UNSAFE(mapping, i, c2);
                            uint32_t c2_मानम् = umutablecptrie_get(new_दत्त.mutableTrie, c2);
                            यदि((c2_मानम्&CANON_NOT_SEGMENT_STARTER)==0) {
                                umutablecptrie_समुच्चयः(new_दत्त.mutableTrie, c2,
                                                   c2_मानम्|CANON_NOT_SEGMENT_STARTER, &error_संहिता);
                            }
                        }
                    }
                }
            } अन्यथा {
                // c decomposed to c2 algorithmically; c has cc==0
                new_दत्त.सङ्कलय_गन्तव्यम्_Start_समुच्चयः(c, c2, error_संहिता);
            }
        }
        यदि(new_मानम्!=old_मानम्) {
            umutablecptrie_समुच्चयः(new_दत्त.mutableTrie, c, new_मानम्, &error_संहिता);
        }
    }
}

UBool Normalizer2Impl::ensure_Canon_Iter_दत्त(UError_संहिता &error_संहिता) ध्रुव {
    // Logically const: Synchronized instantiation.
    Normalizer2Impl *me=const_cast<Normalizer2Impl *>(एतत्);
    umtx_initOnce(me->f_Canon_Iter_दत्त_Init_Once, &init_Canon_Iter_दत्त, me, error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता);
}

int32_t Normalizer2Impl::get_Canon_मानम्(UChar32 c) ध्रुव {
    प्रत्यागम static_cast<int32_t>(ucptrie_get(f_Canon_Iter_दत्त->trie, c));
}

ध्रुव Unicode_समुच्चयः &Normalizer2Impl::get_Canon_Start_समुच्चयः(int32_t n) ध्रुव {
    प्रत्यागम *static_cast<ध्रुव Unicode_समुच्चयः*>(f_Canon_Iter_दत्त->canonStartSets[n]);
}

UBool Normalizer2Impl::isCanonSegmentStarter(UChar32 c) ध्रुव {
    प्रत्यागम get_Canon_मानम्(c)>=0;
}

UBool Normalizer2Impl::get_Canon_Start_समुच्चयः(UChar32 c, Unicode_समुच्चयः &समुच्चयः) ध्रुव {
    int32_t canon_मानम्=get_Canon_मानम्(c)&~CANON_NOT_SEGMENT_STARTER;
    यदि(canon_मानम्==0) {
        प्रत्यागम असत्य;
    }
    समुच्चयः.clear();
    int32_t value=canon_मानम्&CANON_मानम्_आवरणम्;
    यदि((canon_मानम्&CANON_HAS_समुच्चयः)!=0) {
        समुच्चयः.सङ्कलय_All(get_Canon_Start_समुच्चयः(value));
    } अन्यथा यदि(value!=0) {
        समुच्चयः.सङ्कलय(value);
    }
    यदि((canon_मानम्&CANON_HAS_COMPOSITIONS)!=0) {
        uint16_t norm16=getRawNorm16(c);
        यदि(norm16==JAMO_L) {
            UChar32 syllable=
                static_cast<UChar32>(Hangul::HANGUL_आधारः + (c - Hangul::JAMO_L_आधारः) * Hangul::JAMO_VT_गणनम्);
            समुच्चयः.सङ्कलय(syllable, syllable+Hangul::JAMO_VT_गणनम्-1);
        } अन्यथा {
            सङ्कलय_Composites(getCompositionsList(norm16), समुच्चयः);
        }
    }
    प्रत्यागम सत्य;
}

U_NAMESPACE_अन्तः

// Normalizer2 data swapping ----------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
unorm2_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
            UError_संहिता *p_Error_संहिता) {
    ध्रुव UDataInfo *pInfo;
    int32_t शीर्षकम्_आकार;

    ध्रुव uint8_t *अन्तर्_अष्टकानि;
    uint8_t *बहिर्गमनम्_अष्टकानि;

    ध्रुव int32_t *अन्तर्_Indexes;
    int32_t indexes[Normalizer2Impl::IX_योग_आकार+1];

    int32_t i, विस्थापनम्, अग्र_विस्थापनम्, size;

    /* udata_swapDataHeader checks the arguments */
    शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    uint8_t formatVersion0=pInfo->format_संस्करणम्[0];
    यदि(!(
        pInfo->दत्त_Format[0]==0x4e &&   /* dataFormat="Nrm2" */
        pInfo->दत्त_Format[1]==0x72 &&
        pInfo->दत्त_Format[2]==0x6d &&
        pInfo->दत्त_Format[3]==0x32 &&
        (1<=formatVersion0 && formatVersion0<=5)
    )) {
        udata_printError(ds, "unorm2_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as Normalizer2 data\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                         pInfo->दत्त_Format[2], pInfo->दत्त_Format[3],
                         pInfo->format_संस्करणम्[0]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_अष्टकानि=(ध्रुव uint8_t *)अन्तर्_दत्त+शीर्षकम्_आकार;
    बहिर्गमनम्_अष्टकानि=(बहिर्गमनम्_दत्त == शून्यसूचकः) ? शून्यसूचकः : (uint8_t *)बहिर्गमनम्_दत्त+शीर्षकम्_आकार;

    अन्तर्_Indexes=(ध्रुव int32_t *)अन्तर्_अष्टकानि;
    int32_t min_Indexes_दीर्घता;
    यदि(formatVersion0==1) {
        min_Indexes_दीर्घता=Normalizer2Impl::IX_MIN_MAYBE_YES+1;
    } अन्यथा यदि(formatVersion0==2) {
        min_Indexes_दीर्घता=Normalizer2Impl::IX_MIN_YES_NO_MAPPINGS_ONLY+1;
    } अन्यथा यदि(formatVersion0<=4) {
        min_Indexes_दीर्घता=Normalizer2Impl::IX_MIN_LCCC_CP+1;
    } अन्यथा {
        min_Indexes_दीर्घता=Normalizer2Impl::IX_MIN_MAYBE_NO_COMBINES_FWD+1;
    }

    यदि(length>=0) {
        length-=शीर्षकम्_आकार;
        यदि(length<min_Indexes_दीर्घता*4) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for Normalizer2 data\n",
                             length);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    /* read the first few indexes */
    प्रति(i=0; i<UPRV_LENGTHOF(indexes); ++i) {
        indexes[i]=udata_readInt32(ds, अन्तर्_Indexes[i]);
    }

    /* get the total length of the data */
    size=indexes[Normalizer2Impl::IX_योग_आकार];

    यदि(length>=0) {
        यदि(length<size) {
            udata_printError(ds, "unorm2_swap(): too few bytes (%d after header) for all of Normalizer2 data\n",
                             length);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        /* copy the data for inaccessible bytes */
        यदि(अन्तर्_अष्टकानि!=बहिर्गमनम्_अष्टकानि) {
            uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, size);
        }

        विस्थापनम्=0;

        /* swap the int32_t indexes[] */
        अग्र_विस्थापनम्=indexes[Normalizer2Impl::IX_NORM_TRIE_विस्थापनम्];
        ds->swapArray32(ds, अन्तर्_अष्टकानि, अग्र_विस्थापनम्-विस्थापनम्, बहिर्गमनम्_अष्टकानि, p_Error_संहिता);
        विस्थापनम्=अग्र_विस्थापनम्;

        /* swap the trie */
        अग्र_विस्थापनम्=indexes[Normalizer2Impl::IX_EXTRA_दत्त_विस्थापनम्];
        utrie_swap_Any_संस्करणम्(ds, अन्तर्_अष्टकानि+विस्थापनम्, अग्र_विस्थापनम्-विस्थापनम्, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
        विस्थापनम्=अग्र_विस्थापनम्;

        /* swap the uint16_t extraData[] */
        अग्र_विस्थापनम्=indexes[Normalizer2Impl::IX_SMALL_FCD_विस्थापनम्];
        ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, अग्र_विस्थापनम्-विस्थापनम्, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
        विस्थापनम्=अग्र_विस्थापनम्;

        /* no need to swap the uint8_t smallFCD[] (new in formatVersion 2) */
        अग्र_विस्थापनम्=indexes[Normalizer2Impl::IX_SMALL_FCD_विस्थापनम्+1];
        विस्थापनम्=अग्र_विस्थापनम्;

        U_ASSERT(विस्थापनम्==size);
    }

    प्रत्यागम शीर्षकम्_आकार+size;
}

#यदि_अन्तः  // !UCONFIG_NO_NORMALIZATION
