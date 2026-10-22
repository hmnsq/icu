// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  normalizer2impl.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009nov22
*   created by: Markus W. Scherer
*/

#अगर_अघोषित __NORMALIZER2IMPL_H__
#घोषणा __NORMALIZER2IMPL_H__

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/normalizer2.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/unorm.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf16.h"
#समावेश "mutex.h"
#समावेश "udataswp.h"
#समावेश "uset_imp.h"

// When the nfc.nrm data is *not* hardcoded into the common library
// (with this constant set to 0),
// then it needs to be built into the data package:
// Add nfc.nrm to icu4c/source/data/Makefile.in DAT_FILES_SHORT
#घोषणा NORM2_HARDCODE_NFC_तथ्य 1

U_NAMESPACE_BEGIN

काष्ठा Canon_Iter_तथ्य;

class ByteSink;
class Edits;
class Init_Canon_Iter_तथ्य;
class LcccContext;

class U_COMMON_API Hangul {
खुला:
    /* Korean Hangul and Jamo constants */
    क्रमागत {
        JAMO_L_BASE=0x1100,     /* "lead" jamo */
        JAMO_L_END=0x1112,
        JAMO_V_BASE=0x1161,     /* "vowel" jamo */
        JAMO_V_END=0x1175,
        JAMO_T_BASE=0x11a7,     /* "trail" jamo */
        JAMO_T_END=0x11c2,

        HANGUL_BASE=0xac00,
        HANGUL_END=0xd7a3,

        JAMO_L_गिनती=19,
        JAMO_V_गिनती=21,
        JAMO_T_गिनती=28,

        JAMO_VT_गिनती=JAMO_V_गिनती*JAMO_T_गिनती,

        HANGUL_गिनती=JAMO_L_गिनती*JAMO_V_गिनती*JAMO_T_गिनती,
        HANGUL_LIMIT=HANGUL_BASE+HANGUL_गिनती
    };

    अटल अंतरभूत UBool isHangul(UChar32 c) {
        वापस HANGUL_BASE<=c && c<HANGUL_LIMIT;
    }
    अटल अंतरभूत UBool
    isHangulLV(UChar32 c) {
        c-=HANGUL_BASE;
        वापस 0<=c && c<HANGUL_गिनती && c%JAMO_T_गिनती==0;
    }
    अटल अंतरभूत UBool isJamoL(UChar32 c) {
        वापस static_cast<uint32_t>(c - JAMO_L_BASE) < JAMO_L_गिनती;
    }
    अटल अंतरभूत UBool isJamoV(UChar32 c) {
        वापस static_cast<uint32_t>(c - JAMO_V_BASE) < JAMO_V_गिनती;
    }
    अटल अंतरभूत UBool isJamoT(UChar32 c) {
        int32_t t=c-JAMO_T_BASE;
        वापस 0<t && t<JAMO_T_गिनती;  // not JAMO_T_BASE itself
    }
    अटल UBool isJamo(UChar32 c) {
        वापस JAMO_L_BASE<=c && c<=JAMO_T_END &&
            (c<=JAMO_L_END || (JAMO_V_BASE<=c && c<=JAMO_V_END) || JAMO_T_BASE<c);
    }

    /**
     * Decomposes c, which must be a Hangul syllable, into buffer
     * and returns the length of the decomposition (2 or 3).
     */
    अटल अंतरभूत int32_t decompose(UChar32 c, char16_t बफर[3]) {
        c-=HANGUL_BASE;
        UChar32 c2=c%JAMO_T_गिनती;
        c/=JAMO_T_गिनती;
        बफर[0] = static_cast<char16_t>(JAMO_L_BASE + c / JAMO_V_गिनती);
        बफर[1] = static_cast<char16_t>(JAMO_V_BASE + c % JAMO_V_गिनती);
        अगर(c2==0) {
            वापस 2;
        } अन्यथा {
            बफर[2] = static_cast<char16_t>(JAMO_T_BASE + c2);
            वापस 3;
        }
    }

    /**
     * Decomposes c, which must be a Hangul syllable, into buffer.
     * This is the raw, not recursive, decomposition. Its length is always 2.
     */
    अटल अंतरभूत व्योम getRawDecomposition(UChar32 c, char16_t बफर[2]) {
        UChar32 orig=c;
        c-=HANGUL_BASE;
        UChar32 c2=c%JAMO_T_गिनती;
        अगर(c2==0) {
            c/=JAMO_T_गिनती;
            बफर[0] = static_cast<char16_t>(JAMO_L_BASE + c / JAMO_V_गिनती);
            बफर[1] = static_cast<char16_t>(JAMO_V_BASE + c % JAMO_V_गिनती);
        } अन्यथा {
            बफर[0] = static_cast<char16_t>(orig - c2); // LV syllable
            बफर[1] = static_cast<char16_t>(JAMO_T_BASE + c2);
        }
    }
निजी:
    Hangul() = delete;  // no instantiation
};

class Normalizer2Impl;

class U_COMMON_API Reordering_बफर : खुला UMemory {
खुला:
    /** Constructs only; init() should be called. */
    Reordering_बफर(स्थिर Normalizer2Impl &ni, UnicodeString &dest) :
        impl(ni), str(dest),
        start(nullptr), reorderStart(nullptr), limit(nullptr),
        remainingCapacity(0), lastCC(0) {}
    /** Constructs, removes the string contents, and initializes for a small initial capacity. */
    Reordering_बफर(स्थिर Normalizer2Impl &ni, UnicodeString &dest, UErrorCode &errorCode);
    ~Reordering_बफर() {
        अगर (start != nullptr) {
            str.release_बफर(static_cast<int32_t>(limit - start));
        }
    }
    UBool init(int32_t destCapacity, UErrorCode &errorCode);

    UBool isEmpty() स्थिर { वापस start==limit; }
    int32_t लंबाई() स्थिर { वापस static_cast<int32_t>(limit - start); }
    char16_t *getStart() { वापस start; }
    char16_t *getLimit() { वापस limit; }
    uint8_t getLastCC() स्थिर { वापस lastCC; }

    UBool equals(स्थिर char16_t *start, स्थिर char16_t *limit) स्थिर;
    UBool equals(स्थिर uint8_t *otherStart, स्थिर uint8_t *otherLimit) स्थिर;

    UBool append(UChar32 c, uint8_t cc, UErrorCode &errorCode) {
        वापस (c<=0xffff) ?
            appendBMP(static_cast<char16_t>(c), cc, errorCode) :
            appendSupplementary(c, cc, errorCode);
    }
    UBool append(स्थिर char16_t *s, int32_t लंबाई, UBool isNFD,
                 uint8_t leadCC, uint8_t trailCC,
                 UErrorCode &errorCode);
    UBool appendBMP(char16_t c, uint8_t cc, UErrorCode &errorCode) {
        अगर(remainingCapacity==0 && !resize(1, errorCode)) {
            वापस false;
        }
        अगर(lastCC<=cc || cc==0) {
            *limit++=c;
            lastCC=cc;
            अगर(cc<=1) {
                reorderStart=limit;
            }
        } अन्यथा {
            जोड़ो(c, cc);
        }
        --remainingCapacity;
        वापस true;
    }
    UBool appendZeroCC(UChar32 c, UErrorCode &errorCode);
    UBool appendZeroCC(स्थिर char16_t *s, स्थिर char16_t *sLimit, UErrorCode &errorCode);
    व्योम हटाओ();
    व्योम removeSuffix(int32_t suffix_लंबाई);
    व्योम setReorderingLimit(char16_t *newLimit) {
        remainingCapacity += static_cast<int32_t>(limit - newLimit);
        reorderStart=limit=newLimit;
        lastCC=0;
    }
    व्योम copyReorderableSuffixTo(UnicodeString &s) स्थिर {
        s.setTo(ConstChar16Ptr(reorderStart), static_cast<int32_t>(limit - reorderStart));
    }
निजी:
    /*
     * TODO: Revisit whether it makes sense to track reorderStart.
     * It is set to after the last known character with cc<=1,
     * which stops previousCC() before it reads that character and looks up its cc.
     * previousCC() is normally only called from insert().
     * In other words, reorderStart speeds up the insertion of a combining mark
     * into a multi-combining mark sequence where it does not belong at the end.
     * This might not be worth the trouble.
     * On the other hand, it's not a huge amount of trouble.
     *
     * We probably need it for UNORM_SIMPLE_APPEND.
     */

    UBool appendSupplementary(UChar32 c, uint8_t cc, UErrorCode &errorCode);
    व्योम जोड़ो(UChar32 c, uint8_t cc);
    अटल व्योम writeCodePoint(char16_t *p, UChar32 c) {
        अगर(c<=0xffff) {
            *p = static_cast<char16_t>(c);
        } अन्यथा {
            p[0]=U16_LEAD(c);
            p[1]=U16_TRAIL(c);
        }
    }
    UBool resize(int32_t append_लंबाई, UErrorCode &errorCode);

    स्थिर Normalizer2Impl &impl;
    UnicodeString &str;
    char16_t *start, *reorderStart, *limit;
    int32_t remainingCapacity;
    uint8_t lastCC;

    // private backward iterator
    व्योम setIterator() { codePointStart=limit; }
    व्योम skipPrevious();  // Requires start<codePointStart.
    uint8_t previousCC();  // Returns 0 if there is no previous character.

    char16_t *codePointStart, *codePointLimit;
};

/**
 * Low-level implementation of the Unicode Normalization Algorithm.
 * For the data structure and details see the documentation at the end of
 * this normalizer2impl.h and in the design doc at
 * https://unicode-org.github.io/icu/design/normalization/custom.html
 */
class U_COMMON_API_CLASS Normalizer2Impl : खुला UObject {
खुला:
    U_COMMON_API Normalizer2Impl() : normTrie(nullptr), f_Canon_Iter_तथ्य(nullptr) {}
    U_COMMON_API भव ~Normalizer2Impl();

    U_COMMON_API व्योम init(स्थिर int32_t* inIndexes,
                           स्थिर UCPTrie* inTrie,
                           स्थिर uint16_t* in_Extra_तथ्य,
                           स्थिर uint8_t* inSmallFCD);

    U_COMMON_API व्योम addLcccChars(UnicodeSet& set) स्थिर;
    U_COMMON_API व्योम addPropertyStarts(स्थिर USetAdder* sa, UErrorCode& errorCode) स्थिर;
    U_COMMON_API व्योम addCanonIterPropertyStarts(स्थिर USetAdder* sa, UErrorCode& errorCode) स्थिर;

    // low-level properties ------------------------------------------------ ***

    U_COMMON_API UBool ensure_Canon_Iter_तथ्य(UErrorCode& errorCode) स्थिर;

    // The trie stores values for lead surrogate code *units*.
    // Surrogate code *points* are inert.
    U_COMMON_API uint16_t getNorm16(UChar32 c) स्थिर {
        वापस U_IS_LEAD(c) ?
            static_cast<uint16_t>(INERT) :
            UCPTRIE_FAST_GET(normTrie, UCPTRIE_16, c);
    }
    U_COMMON_API uint16_t getRawNorm16(UChar32 c) स्थिर {
        वापस UCPTRIE_FAST_GET(normTrie, UCPTRIE_16, c);
    }

    U_COMMON_API UNormalization_Check_परिणाम getCompQuickCheck(uint16_t norm16) स्थिर {
        अगर(norm16<minNoNo || MIN_YES_YES_WITH_CC<=norm16) {
            वापस UNORM_YES;
        } अन्यथा अगर(minMaybeNo<=norm16) {
            वापस UNORM_MAYBE;
        } अन्यथा {
            वापस UNORM_NO;
        }
    }
    U_COMMON_API UBool isAlgorithmicNoNo(uint16_t norm16) स्थिर {
        वापस limitNoNo <= norm16 && norm16 < minMaybeNo;
    }
    U_COMMON_API UBool isCompNo(uint16_t norm16) स्थिर {
        वापस minNoNo <= norm16 && norm16 < minMaybeNo;
    }
    U_COMMON_API UBool isDecompYes(uint16_t norm16) स्थिर {
        वापस norm16 < minYesNo || minMaybeYes <= norm16;
    }

    U_COMMON_API uint8_t getCC(uint16_t norm16) स्थिर {
        अगर(norm16>=MIN_NORMAL_MAYBE_YES) {
            वापस getCCFromNormalYesOrMaybe(norm16);
        }
        अगर(norm16<minNoNo || limitNoNo<=norm16) {
            वापस 0;
        }
        वापस getCCFromNoNo(norm16);
    }
    U_COMMON_API अटल uint8_t getCCFromNormalYesOrMaybe(uint16_t norm16) {
        वापस static_cast<uint8_t>(norm16 >> OFFSET_SHIFT);
    }
    U_COMMON_API अटल uint8_t getCCFromYesOrMaybeYes(uint16_t norm16) {
        वापस norm16>=MIN_NORMAL_MAYBE_YES ? getCCFromNormalYesOrMaybe(norm16) : 0;
    }
    U_COMMON_API uint8_t getCCFromYesOrMaybeYesCP(UChar32 c) स्थिर {
        अगर (c < minCompNoMaybeCP) { वापस 0; }
        वापस getCCFromYesOrMaybeYes(getNorm16(c));
    }

    /**
     * Returns the FCD data for code point c.
     * @param c A Unicode code point.
     * @return The lccc(c) in bits 15..8 and tccc(c) in bits 7..0.
     */
    U_COMMON_API uint16_t getFCD16(UChar32 c) स्थिर {
        अगर(c<minDecompNoCP) {
            वापस 0;
        } अन्यथा अगर(c<=0xffff) {
            अगर(!singleLeadMightHaveNonZeroFCD16(c)) { वापस 0; }
        }
        वापस get_FCD16_From_Norm_तथ्य(c);
    }
    /**
     * Returns the FCD data for the next code point (post-increment).
     * Might skip only a lead surrogate rather than the whole surrogate pair if none of
     * the supplementary code points associated with the lead surrogate have non-zero FCD data.
     * @param s A valid pointer into a string. Requires s!=limit.
     * @param limit The end of the string, or NULL.
     * @return The lccc(c) in bits 15..8 and tccc(c) in bits 7..0.
     */
    U_COMMON_API uint16_t अग्र_FCD16(स्थिर char16_t*& s, स्थिर char16_t* limit) स्थिर {
        UChar32 c=*s++;
        अगर(c<minDecompNoCP || !singleLeadMightHaveNonZeroFCD16(c)) {
            वापस 0;
        }
        char16_t c2;
        अगर(U16_IS_LEAD(c) && s!=limit && U16_IS_TRAIL(c2=*s)) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
            ++s;
        }
        वापस get_FCD16_From_Norm_तथ्य(c);
    }
    /**
     * Returns the FCD data for the previous code point (pre-decrement).
     * @param start The start of the string.
     * @param s A valid pointer into a string. Requires start<s.
     * @return The lccc(c) in bits 15..8 and tccc(c) in bits 7..0.
     */
    U_COMMON_API uint16_t previousFCD16(स्थिर char16_t* start, स्थिर char16_t*& s) स्थिर {
        UChar32 c=*--s;
        अगर(c<minDecompNoCP) {
            वापस 0;
        }
        अगर(!U16_IS_TRAIL(c)) {
            अगर(!singleLeadMightHaveNonZeroFCD16(c)) {
                वापस 0;
            }
        } अन्यथा {
            char16_t c2;
            अगर(start<s && U16_IS_LEAD(c2=*(s-1))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
                --s;
            }
        }
        वापस get_FCD16_From_Norm_तथ्य(c);
    }

    /** Returns true if the single-or-lead code unit c might have non-zero FCD data. */
    U_COMMON_API UBool singleLeadMightHaveNonZeroFCD16(UChar32 lead) स्थिर {
        // 0<=lead<=0xffff
        uint8_t bits=smallFCD[lead>>8];
        अगर(bits==0) { वापस false; }
        वापस (bits >> ((lead >> 5) & 7)) & 1;
    }
    /** Returns the FCD value from the regular normalization data. */
    U_COMMON_API uint16_t get_FCD16_From_Norm_तथ्य(UChar32 c) स्थिर;

    U_COMMON_API uint16_t getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) स्थिर;

    /**
     * Gets the decomposition for one code point.
     * @param c code point
     * @param buffer out-only buffer for algorithmic decompositions
     * @param length out-only, takes the length of the decomposition, if any
     * @return pointer to the decomposition, or NULL if none
     */
    U_COMMON_API स्थिर char16_t* getDecomposition(UChar32 c, char16_t बफर[4], int32_t& लंबाई) स्थिर;

    /**
     * Gets the raw decomposition for one code point.
     * @param c code point
     * @param buffer out-only buffer for algorithmic decompositions
     * @param length out-only, takes the length of the decomposition, if any
     * @return pointer to the decomposition, or NULL if none
     */
    U_COMMON_API स्थिर char16_t* getRawDecomposition(UChar32 c,
                                                     char16_t बफर[30],
                                                     int32_t& लंबाई) स्थिर;

    U_COMMON_API UChar32 composePair(UChar32 a, UChar32 b) स्थिर;

    U_COMMON_API UBool isCanonSegmentStarter(UChar32 c) स्थिर;
    U_COMMON_API UBool getCanonStartSet(UChar32 c, UnicodeSet& set) स्थिर;

    क्रमागत {
        // Fixed norm16 values.
        MIN_YES_YES_WITH_CC=0xfe02,
        JAMO_VT=0xfe00,
        MIN_NORMAL_MAYBE_YES=0xfc00,
        JAMO_L=2,  // offset=1 hasCompBoundaryAfter=false
        INERT=1,  // offset=0 hasCompBoundaryAfter=true

        // norm16 bit 0 is comp-boundary-after.
        HAS_COMP_BOUNDARY_AFTER=1,
        OFFSET_SHIFT=1,

        // For algorithmic one-way mappings, norm16 bits 2..1 indicate the
        // tccc (0, 1, >1) for quick FCC boundary-after tests.
        DELTA_TCCC_0=0,
        DELTA_TCCC_1=2,
        DELTA_TCCC_GT_1=4,
        DELTA_TCCC_MASK=6,
        DELTA_SHIFT=3,

        MAX_DELTA=0x40
    };

    क्रमागत {
        // Byte offsets from the start of the data, after the generic header.
        IX_NORM_TRIE_OFFSET,
        IX_EXTRA_तथ्य_OFFSET,
        IX_SMALL_FCD_OFFSET,
        IX_RESERVED3_OFFSET,
        IX_RESERVED4_OFFSET,
        IX_RESERVED5_OFFSET,
        IX_RESERVED6_OFFSET,
        IX_योग_आकार,

        // Code point thresholds for quick check codes.
        IX_MIN_DECOMP_NO_CP,
        IX_MIN_COMP_NO_MAYBE_CP,

        // Norm16 value thresholds for quick check combinations and types of extra data.

        /** Mappings & compositions in [minYesNo..minYesNoMappingsOnly[. */
        IX_MIN_YES_NO,
        /** Mappings are comp-normalized. */
        IX_MIN_NO_NO,
        IX_LIMIT_NO_NO,
        IX_MIN_MAYBE_YES,

        /** Mappings only in [minYesNoMappingsOnly..minNoNo[. */
        IX_MIN_YES_NO_MAPPINGS_ONLY,
        /** Mappings are not comp-normalized but have a comp boundary before. */
        IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE,
        /** Mappings do not have a comp boundary before. */
        IX_MIN_NO_NO_COMP_NO_MAYBE_CC,
        /** Mappings to the empty string. */
        IX_MIN_NO_NO_EMPTY,

        IX_MIN_LCCC_CP,
        IX_RESERVED19,

        /** Two-way mappings; each starts with a character that combines backward. */
        IX_MIN_MAYBE_NO,  // 20
        /** Two-way mappings & compositions. */
        IX_MIN_MAYBE_NO_COMBINES_FWD,

        IX_गिनती  // 22
    };

    क्रमागत {
        MAPPING_HAS_CCC_LCCC_WORD=0x80,
        MAPPING_HAS_RAW_MAPPING=0x40,
        // unused bit 0x20,
        MAPPING_लंबाई_MASK=0x1f
    };

    क्रमागत {
        COMP_1_LAST_TUPLE=0x8000,
        COMP_1_TRIPLE=1,
        COMP_1_TRAIL_LIMIT=0x3400,
        COMP_1_TRAIL_MASK=0x7ffe,
        COMP_1_TRAIL_SHIFT=9,  // 10-1 for the "triple" bit
        COMP_2_TRAIL_SHIFT=6,
        COMP_2_TRAIL_MASK=0xffc0
    };

    // higher-level functionality ------------------------------------------ ***

    // NFD without an NFD Normalizer2 instance.
    U_COMMON_API UnicodeString& decompose(स्थिर UnicodeString& src,
                                          UnicodeString& dest,
                                          UErrorCode& errorCode) स्थिर;
    /**
     * Decomposes [src, limit[ and writes the result to dest.
     * limit can be NULL if src is NUL-terminated.
     * destLengthEstimate is the initial dest buffer capacity and can be -1.
     */
    U_COMMON_API व्योम decompose(स्थिर char16_t* src,
                                स्थिर char16_t* limit,
                                UnicodeString& dest,
                                int32_t dest_लंबाई_Estimate,
                                UErrorCode& errorCode) स्थिर;

    U_COMMON_API स्थिर char16_t* decompose(स्थिर char16_t* src,
                                           स्थिर char16_t* limit,
                                           Reordering_बफर* बफर,
                                           UErrorCode& errorCode) स्थिर;
    U_COMMON_API व्योम decomposeAndAppend(स्थिर char16_t* src,
                                         स्थिर char16_t* limit,
                                         UBool doDecompose,
                                         UnicodeString& safe_मध्य,
                                         Reordering_बफर& बफर,
                                         UErrorCode& errorCode) स्थिर;

    /** sink==nullptr: isNormalized()/spanQuickCheckYes() */
    U_COMMON_API स्थिर uint8_t* decomposeUTF8(uint32_t options,
                                              स्थिर uint8_t* src,
                                              स्थिर uint8_t* limit,
                                              ByteSink* sink,
                                              Edits* edits,
                                              UErrorCode& errorCode) स्थिर;

    U_COMMON_API UBool compose(स्थिर char16_t* src,
                               स्थिर char16_t* limit,
                               UBool onlyContiguous,
                               UBool doCompose,
                               Reordering_बफर& बफर,
                               UErrorCode& errorCode) स्थिर;
    U_COMMON_API स्थिर char16_t* composeQuickCheck(स्थिर char16_t* src,
                                                   स्थिर char16_t* limit,
                                                   UBool onlyContiguous,
                                                   UNormalization_Check_परिणाम* pQCResult) स्थिर;
    U_COMMON_API व्योम composeAndAppend(स्थिर char16_t* src,
                                       स्थिर char16_t* limit,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString& safe_मध्य,
                                       Reordering_बफर& बफर,
                                       UErrorCode& errorCode) स्थिर;

    /** sink==nullptr: isNormalized() */
    U_COMMON_API UBool composeUTF8(uint32_t options,
                                   UBool onlyContiguous,
                                   स्थिर uint8_t* src,
                                   स्थिर uint8_t* limit,
                                   ByteSink* sink,
                                   icu::Edits* edits,
                                   UErrorCode& errorCode) स्थिर;

    U_COMMON_API स्थिर char16_t* makeFCD(स्थिर char16_t* src,
                                         स्थिर char16_t* limit,
                                         Reordering_बफर* बफर,
                                         UErrorCode& errorCode) स्थिर;
    U_COMMON_API व्योम makeFCDAndAppend(स्थिर char16_t* src,
                                       स्थिर char16_t* limit,
                                       UBool doMakeFCD,
                                       UnicodeString& safe_मध्य,
                                       Reordering_बफर& बफर,
                                       UErrorCode& errorCode) स्थिर;

    U_COMMON_API UBool hasDecompBoundaryBefore(UChar32 c) स्थिर;
    U_COMMON_API UBool norm16HasDecompBoundaryBefore(uint16_t norm16) स्थिर;
    U_COMMON_API UBool hasDecompBoundaryAfter(UChar32 c) स्थिर;
    U_COMMON_API UBool norm16HasDecompBoundaryAfter(uint16_t norm16) स्थिर;
    U_COMMON_API UBool isDecompInert(UChar32 c) स्थिर { वापस isDecompYesAndZeroCC(getNorm16(c)); }

    U_COMMON_API UBool hasCompBoundaryBefore(UChar32 c) स्थिर {
        वापस c<minCompNoMaybeCP || norm16HasCompBoundaryBefore(getNorm16(c));
    }
    U_COMMON_API UBool hasCompBoundaryAfter(UChar32 c, UBool onlyContiguous) स्थिर {
        वापस norm16HasCompBoundaryAfter(getNorm16(c), onlyContiguous);
    }
    U_COMMON_API UBool isCompInert(UChar32 c, UBool onlyContiguous) स्थिर {
        uint16_t norm16=getNorm16(c);
        वापस isCompYesAndZeroCC(norm16) &&
            (norm16 & HAS_COMP_BOUNDARY_AFTER) != 0 &&
            (!onlyContiguous || isInert(norm16) || *get_तथ्य_For_Yes_Or_No(norm16) <= 0x1ff);
            // The last check fetches the mapping's first unit and checks tccc<=1.
    }

    U_COMMON_API UBool hasFCDBoundaryBefore(UChar32 c) स्थिर { वापस hasDecompBoundaryBefore(c); }
    U_COMMON_API UBool hasFCDBoundaryAfter(UChar32 c) स्थिर { वापस hasDecompBoundaryAfter(c); }
    U_COMMON_API UBool isFCDInert(UChar32 c) स्थिर { वापस getFCD16(c) <= 1; }

  निजी:
    मित्र class Init_Canon_Iter_तथ्य;
    मित्र class LcccContext;

    UBool isMaybe(uint16_t norm16) स्थिर { वापस minMaybeNo<=norm16 && norm16<=JAMO_VT; }
    UBool isMaybeYesOrNonZeroCC(uint16_t norm16) स्थिर { वापस norm16>=minMaybeYes; }
    अटल UBool isInert(uint16_t norm16) { वापस norm16==INERT; }
    अटल UBool isJamoL(uint16_t norm16) { वापस norm16==JAMO_L; }
    अटल UBool isJamoVT(uint16_t norm16) { वापस norm16==JAMO_VT; }
    uint16_t hangulLVT() स्थिर { वापस minYesNoMappingsOnly|HAS_COMP_BOUNDARY_AFTER; }
    UBool isHangulLV(uint16_t norm16) स्थिर { वापस norm16==minYesNo; }
    UBool isHangulLVT(uint16_t norm16) स्थिर {
        वापस norm16==hangulLVT();
    }
    UBool isCompYesAndZeroCC(uint16_t norm16) स्थिर { वापस norm16<minNoNo; }
    // UBool isCompYes(uint16_t norm16) const {
    //     return norm16>=MIN_YES_YES_WITH_CC || norm16<minNoNo;
    // }
    // UBool isCompYesOrMaybe(uint16_t norm16) const {
    //     return norm16<minNoNo || minMaybeNo<=norm16;
    // }
    // UBool hasZeroCCFromDecompYes(uint16_t norm16) const {
    //     return norm16<=MIN_NORMAL_MAYBE_YES || norm16==JAMO_VT;
    // }
    UBool isDecompYesAndZeroCC(uint16_t norm16) स्थिर {
        वापस norm16<minYesNo ||
               norm16==JAMO_VT ||
               (minMaybeYes<=norm16 && norm16<=MIN_NORMAL_MAYBE_YES);
    }
    /**
     * A little faster and simpler than isDecompYesAndZeroCC() but does not include
     * the MaybeYes which combine-forward and have ccc=0.
     */
    UBool isMostDecompYesAndZeroCC(uint16_t norm16) स्थिर {
        वापस norm16<minYesNo || norm16==MIN_NORMAL_MAYBE_YES || norm16==JAMO_VT;
    }
    /** Since formatVersion 5: same as isAlgorithmicNoNo() */
    UBool isDecompNoAlgorithmic(uint16_t norm16) स्थिर { वापस limitNoNo<=norm16 && norm16<minMaybeNo; }

    // For use with isCompYes().
    // Perhaps the compiler can combine the two tests for MIN_YES_YES_WITH_CC.
    // static uint8_t getCCFromYes(uint16_t norm16) {
    //     return norm16>=MIN_YES_YES_WITH_CC ? getCCFromNormalYesOrMaybe(norm16) : 0;
    // }
    uint8_t getCCFromNoNo(uint16_t norm16) स्थिर {
        स्थिर uint16_t *mapping=get_तथ्य_For_Yes_Or_No(norm16);
        अगर(*mapping&MAPPING_HAS_CCC_LCCC_WORD) {
            वापस static_cast<uint8_t>(*(mapping - 1));
        } अन्यथा {
            वापस 0;
        }
    }
    // requires that the [cpStart..cpLimit[ character passes isCompYesAndZeroCC()
    uint8_t getTrailCCFromCompYesAndZeroCC(uint16_t norm16) स्थिर {
        अगर(norm16<=minYesNo) {
            वापस 0;  // yesYes and Hangul LV have ccc=tccc=0
        } अन्यथा {
            // For Hangul LVT we harmlessly fetch a firstUnit with tccc=0 here.
            वापस static_cast<uint8_t>(*get_तथ्य_For_Yes_Or_No(norm16) >> 8); // tccc from yesNo
        }
    }
    uint8_t getPreviousTrailCC(स्थिर char16_t *start, स्थिर char16_t *p) स्थिर;
    uint8_t getPreviousTrailCC(स्थिर uint8_t *start, स्थिर uint8_t *p) स्थिर;

    // Requires algorithmic-NoNo.
    UChar32 mapAlgorithmic(UChar32 c, uint16_t norm16) स्थिर {
        वापस c+(norm16>>DELTA_SHIFT)-centerNoNoDelta;
    }
    UChar32 getAlgorithmicDelta(uint16_t norm16) स्थिर {
        वापस (norm16>>DELTA_SHIFT)-centerNoNoDelta;
    }

    स्थिर uint16_t *get_तथ्य_For_Yes_Or_No(uint16_t norm16) स्थिर {
        वापस extra_तथ्य+(norm16>>OFFSET_SHIFT);
    }
    स्थिर uint16_t *get_तथ्य_For_Maybe(uint16_t norm16) स्थिर {
        वापस extra_तथ्य+((norm16-minMaybeNo+limitNoNo)>>OFFSET_SHIFT);
    }
    स्थिर uint16_t *get_तथ्य(uint16_t norm16) स्थिर {
        अगर(norm16>=minMaybeNo) {
            norm16=norm16-minMaybeNo+limitNoNo;
        }
        वापस extra_तथ्य+(norm16>>OFFSET_SHIFT);
    }
    स्थिर uint16_t *getCompositionsListForDecompYes(uint16_t norm16) स्थिर {
        अगर(norm16<JAMO_L || MIN_NORMAL_MAYBE_YES<=norm16) {
            वापस nullptr;
        } अन्यथा {
            // if yesYes: if Jamo L: harmless empty list
            वापस get_तथ्य(norm16);
        }
    }
    स्थिर uint16_t *getCompositionsListForComposite(uint16_t norm16) स्थिर {
        // A composite has both mapping & compositions list.
        स्थिर uint16_t *list=get_तथ्य(norm16);
        वापस list+  // mapping pointer
            1+  // +1 to skip the first unit with the mapping length
            (*list&MAPPING_लंबाई_MASK);  // + mapping length
    }
    /**
     * @param c code point must have compositions
     * @return compositions list pointer
     */
    स्थिर uint16_t *getCompositionsList(uint16_t norm16) स्थिर {
        वापस isDecompYes(norm16) ?
                getCompositionsListForDecompYes(norm16) :
                getCompositionsListForComposite(norm16);
    }

    स्थिर char16_t *copy_निचला_Prefix_From_Nul_Terminated(स्थिर char16_t *src,
                                                UChar32 min_Need_तथ्य_CP,
                                                Reordering_बफर *बफर,
                                                UErrorCode &errorCode) स्थिर;

    क्रमागत StopAt { STOP_AT_LIMIT, STOP_AT_DECOMP_BOUNDARY, STOP_AT_COMP_BOUNDARY };

    स्थिर char16_t *decomposeShort(स्थिर char16_t *src, स्थिर char16_t *limit,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर;
    UBool decompose(UChar32 c, uint16_t norm16,
                    Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर;

    स्थिर uint8_t *decomposeShort(स्थिर uint8_t *src, स्थिर uint8_t *limit,
                                  StopAt stopAt, UBool onlyContiguous,
                                  Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर;

    अटल int32_t combine(स्थिर uint16_t *list, UChar32 trail);
    व्योम addComposites(स्थिर uint16_t *list, UnicodeSet &set) स्थिर;
    व्योम recompose(Reordering_बफर &बफर, int32_t recompose_Start_सूचकांक,
                   UBool onlyContiguous) स्थिर;

    UBool hasCompBoundaryBefore(UChar32 c, uint16_t norm16) स्थिर {
        वापस c<minCompNoMaybeCP || norm16HasCompBoundaryBefore(norm16);
    }
    UBool norm16HasCompBoundaryBefore(uint16_t norm16) स्थिर  {
        वापस norm16 < minNoNoCompNoMaybeCC || isAlgorithmicNoNo(norm16);
    }
    UBool hasCompBoundaryBefore(स्थिर char16_t *src, स्थिर char16_t *limit) स्थिर;
    UBool hasCompBoundaryBefore(स्थिर uint8_t *src, स्थिर uint8_t *limit) स्थिर;
    UBool hasCompBoundaryAfter(स्थिर char16_t *start, स्थिर char16_t *p,
                               UBool onlyContiguous) स्थिर;
    UBool hasCompBoundaryAfter(स्थिर uint8_t *start, स्थिर uint8_t *p,
                               UBool onlyContiguous) स्थिर;
    UBool norm16HasCompBoundaryAfter(uint16_t norm16, UBool onlyContiguous) स्थिर {
        वापस (norm16 & HAS_COMP_BOUNDARY_AFTER) != 0 &&
            (!onlyContiguous || isTrailCC01ForCompBoundaryAfter(norm16));
    }
    /** For FCC: Given norm16 HAS_COMP_BOUNDARY_AFTER, does it have tccc<=1? */
    UBool isTrailCC01ForCompBoundaryAfter(uint16_t norm16) स्थिर {
        वापस isInert(norm16) || (isDecompNoAlgorithmic(norm16) ?
            (norm16 & DELTA_TCCC_MASK) <= DELTA_TCCC_1 : *get_तथ्य_For_Yes_Or_No(norm16) <= 0x1ff);
    }

    स्थिर char16_t *findPreviousCompBoundary(स्थिर char16_t *start, स्थिर char16_t *p,
                                             UBool onlyContiguous) स्थिर;
    स्थिर char16_t *find_अग्र_Comp_Boundary(स्थिर char16_t *p, स्थिर char16_t *limit,
                                         UBool onlyContiguous) स्थिर;

    स्थिर char16_t *findPreviousFCDBoundary(स्थिर char16_t *start, स्थिर char16_t *p) स्थिर;
    स्थिर char16_t *find_अग्र_FCDBoundary(स्थिर char16_t *p, स्थिर char16_t *limit) स्थिर;

    व्योम make_Canon_Iter_तथ्य_From_Norm16(UChar32 start, UChar32 end, स्थिर uint16_t norm16,
                                     Canon_Iter_तथ्य &new_तथ्य, UErrorCode &errorCode) स्थिर;

    int32_t get_Canon_मूल्य(UChar32 c) स्थिर;
    स्थिर UnicodeSet &getCanonStartSet(int32_t n) स्थिर;

    // UVersionInfo dataVersion;

    // BMP code point thresholds for quick check loops looking at single UTF-16 code units.
    char16_t minDecompNoCP;
    char16_t minCompNoMaybeCP;
    char16_t minLcccCP;

    // Norm16 value thresholds for quick check combinations and types of extra data.
    uint16_t minYesNo;
    uint16_t minYesNoMappingsOnly;
    uint16_t minNoNo;
    uint16_t minNoNoCompBoundaryBefore;
    uint16_t minNoNoCompNoMaybeCC;
    uint16_t minNoNoEmpty;
    uint16_t limitNoNo;
    uint16_t centerNoNoDelta;
    uint16_t minMaybeNo;
    uint16_t minMaybeNoCombinesFwd;
    uint16_t minMaybeYes;

    स्थिर UCPTrie *normTrie;
    स्थिर uint16_t *extra_तथ्य;  // mappings and/or compositions
    स्थिर uint8_t *smallFCD;  // [0x100] one bit per 32 BMP code points, set if any FCD!=0

    UInitOnce       f_Canon_Iter_तथ्य_Init_Once {};
    Canon_Iter_तथ्य  *f_Canon_Iter_तथ्य;
};

// bits in canonIterData
#घोषणा CANON_NOT_SEGMENT_STARTER 0x80000000
#घोषणा CANON_HAS_COMPOSITIONS 0x40000000
#घोषणा CANON_HAS_SET 0x200000
#घोषणा CANON_मूल्य_MASK 0x1fffff

/**
 * ICU-internal shortcut for quick access to standard Unicode normalization.
 */
class U_COMMON_API Normalizer2Factory {
खुला:
    अटल स्थिर Normalizer2 *getFCDInstance(UErrorCode &errorCode);
    अटल स्थिर Normalizer2 *getFCCInstance(UErrorCode &errorCode);
    अटल स्थिर Normalizer2 *getNoopInstance(UErrorCode &errorCode);

    अटल स्थिर Normalizer2 *getInstance(UNormalizationMode mode, UErrorCode &errorCode);

    अटल स्थिर Normalizer2Impl *getNFCImpl(UErrorCode &errorCode);
    अटल स्थिर Normalizer2Impl *getNFKCImpl(UErrorCode &errorCode);
    अटल स्थिर Normalizer2Impl *getNFKC_CFImpl(UErrorCode &errorCode);

    // Get the Impl instance of the Normalizer2.
    // Must be used only when it is known that norm2 is a Normalizer2WithImpl instance.
    अटल स्थिर Normalizer2Impl* getImpl(स्थिर Normalizer2* norm2 U_LIFETIME_BOUND);
निजी:
    Normalizer2Factory() = delete;  // No instantiation.
};

U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
unorm2_swap(स्थिर UDataSwapper *ds,
            स्थिर व्योम *in_तथ्य, int32_t लंबाई, व्योम *out_तथ्य,
            UErrorCode *pErrorCode);

/**
 * Get the NF*_QC property for a code point, for u_getIntPropertyValue().
 * @internal
 */
U_CFUNC UNormalization_Check_परिणाम
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode);

/**
 * Gets the 16-bit FCD value (lead & trail CCs) for a code point, for u_getIntPropertyValue().
 * @internal
 */
U_CFUNC uint16_t
unorm_getFCD16(UChar32 c);

/**
 * Format of Normalizer2 .nrm data files.
 * Format version 5.0.
 *
 * Normalizer2 .nrm data files provide data for the Unicode Normalization algorithms.
 * ICU ships with data files for standard Unicode Normalization Forms
 * NFC and NFD (nfc.nrm), NFKC and NFKD (nfkc.nrm),
 * NFKC_Casefold (nfkc_cf.nrm) and NFKC_Simple_Casefold (nfkc_scf.nrm).
 * Custom (application-specific) data can be built into additional .nrm files
 * with the gennorm2 build tool.
 * ICU ships with one such file, uts46.nrm, for the implementation of UTS #46.
 *
 * Normalizer2.getInstance() causes a .nrm file to be loaded, unless it has been
 * cached already. Internally, Normalizer2Impl.load() reads the .nrm file.
 *
 * A .nrm file begins with a standard ICU data file header
 * (DataHeader, see ucmndata.h and unicode/udata.h).
 * The UDataInfo.dataVersion field usually contains the Unicode version
 * for which the data was generated.
 *
 * After the header, the file contains the following parts.
 * Constants are defined as enum values of the Normalizer2Impl class.
 *
 * Many details of the data structures are described in the design doc
 * which is at https://unicode-org.github.io/icu/design/normalization/custom.html
 *
 * int32_t indexes[indexesLength]; -- indexesLength=indexes[IX_NORM_TRIE_OFFSET]/4;
 *
 *      The first eight indexes are byte offsets in ascending order.
 *      Each byte offset marks the start of the next part in the data file,
 *      and the end of the previous one.
 *      When two consecutive byte offsets are the same, then the corresponding part is empty.
 *      Byte offsets are offsets from after the header,
 *      that is, from the beginning of the indexes[].
 *      Each part starts at an offset with proper alignment for its data.
 *      If necessary, the previous part may include padding bytes to achieve this alignment.
 *
 *      minDecompNoCP=indexes[IX_MIN_DECOMP_NO_CP] is the lowest code point
 *      with a decomposition mapping, that is, with NF*D_QC=No.
 *      minCompNoMaybeCP=indexes[IX_MIN_COMP_NO_MAYBE_CP] is the lowest code point
 *      with NF*C_QC=No (has a one-way mapping) or Maybe (combines backward).
 *      minLcccCP=indexes[IX_MIN_LCCC_CP] (index 18, new in formatVersion 3)
 *      is the lowest code point with lccc!=0.
 *
 *      The next eight indexes are thresholds of 16-bit trie values for ranges of
 *      values indicating multiple normalization properties.
 *      Format version 5 adds the two minMaybeNo* threshold indexes.
 *      The thresholds are listed here in threshold order,
 *      not in the order they are stored in the indexes.
 *          minYesNo=indexes[IX_MIN_YES_NO];
 *          minYesNoMappingsOnly=indexes[IX_MIN_YES_NO_MAPPINGS_ONLY];
 *          minNoNo=indexes[IX_MIN_NO_NO];
 *          minNoNoCompBoundaryBefore=indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE];
 *          minNoNoCompNoMaybeCC=indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC];
 *          minNoNoEmpty=indexes[IX_MIN_NO_NO_EMPTY];
 *          limitNoNo=indexes[IX_LIMIT_NO_NO];
 *          minMaybeNo=indexes[IX_MIN_MAYBE_NO];
 *          minMaybeNoCombinesFwd=indexes[IX_MIN_MAYBE_NO_COMBINES_FWD];
 *          minMaybeYes=indexes[IX_MIN_MAYBE_YES];
 *      See the normTrie description below and the design doc for details.
 *
 * UCPTrie normTrie; -- see ucptrie_impl.h and ucptrie.h, same as Java CodePointTrie
 *
 *      The trie holds the main normalization data. Each code point is mapped to a 16-bit value.
 *      Rather than using independent bits in the value (which would require more than 16 bits),
 *      information is extracted primarily via range checks.
 *      Except, format version 3+ uses bit 0 for hasCompBoundaryAfter().
 *      For example, a 16-bit value norm16 in the range minYesNo<=norm16<minNoNo
 *      means that the character has NF*C_QC=Yes and NF*D_QC=No properties,
 *      which means it has a two-way (round-trip) decomposition mapping.
 *      Values in the ranges 2<=norm16<limitNoNo and minMaybeNo<=norm16<minMaybeYes
 *      are also directly indexes into the extraData
 *      pointing to mappings, compositions lists, or both.
 *      Value norm16==INERT (0 in versions 1 & 2, 1 in version 3+)
 *      means that the character is normalization-inert, that is,
 *      it does not have a mapping, does not participate in composition, has a zero
 *      canonical combining class, and forms a boundary where text before it and after it
 *      can be normalized independently.
 *      For details about how multiple properties are encoded in 16-bit values
 *      see the design doc.
 *      Note that the encoding cannot express all combinations of the properties involved;
 *      it only supports those combinations that are allowed by
 *      the Unicode Normalization algorithms. Details are in the design doc as well.
 *      The gennorm2 tool only builds .nrm files for data that conforms to the limitations.
 *
 *      The trie has a value for each lead surrogate code unit representing the "worst case"
 *      properties of the 1024 supplementary characters whose UTF-16 form starts with
 *      the lead surrogate. If all of the 1024 supplementary characters are normalization-inert,
 *      then their lead surrogate code unit has the trie value INERT.
 *      When the lead surrogate unit's value exceeds the quick check minimum during processing,
 *      the properties for the full supplementary code point need to be looked up.
 *
 * uint16_t extraData[];
 *
 *      The extraData array contains many per-character data sections.
 *      Each section contains mappings and/or composition lists.
 *      The norm16 value of each character that has such data is directly an index to
 *      a section of the extraData array.
 *
 *      In version 3+, the norm16 values must be shifted right by OFFSET_SHIFT
 *      for accessing extraData.
 *
 *      The data structures for compositions lists and mappings are described in the design doc.
 *
 *      In version 4 and below, the composition lists for MaybeYes characters were stored before
 *      the data for other characters.
 *      This sub-array had a length of MIN_NORMAL_MAYBE_YES-minMaybeYes.
 *      In version 3 & 4, the difference must be shifted right by OFFSET_SHIFT.
 *
 *      In version 5, the data for MaybeNo and MaybeYes characters is stored after
 *      the data for other characters.
 *
 *      If there are no MaybeNo and no MaybeYes characters,
 *      then minMaybeYes==minMaybeNo==MIN_NORMAL_MAYBE_YES.
 *      If there are such characters, then minMaybeNo is subtracted from their norm16 values
 *      to get the index into the extraData.
 *      In version 4 and below, the data index for Yes* and No* characters needs to be
 *      offset by the length of the MaybeYes data.
 *      In version 5, the data index for Maybe* characters needs to be offset by limitNoNo.
 *
 *      Version 5 is the first to support MaybeNo characters, and
 *      adds the minMaybeNo and minMaybeNoCombinesFwd thresholds and
 *      the corresponding sections of the extraData.
 *
 * uint8_t smallFCD[0x100]; -- new in format version 2
 *
 *      This is a bit set to help speed up FCD value lookups in the absence of a full
 *      UTrie2 or other large data structure with the full FCD value mapping.
 *
 *      Each smallFCD bit is set if any of the corresponding 32 BMP code points
 *      has a non-zero FCD value (lccc!=0 or tccc!=0).
 *      Bit 0 of smallFCD[0] is for U+0000..U+001F. Bit 7 of smallFCD[0xff] is for U+FFE0..U+FFFF.
 *      A bit for 32 lead surrogates is set if any of the 32k corresponding
 *      _supplementary_ code points has a non-zero FCD value.
 *
 *      This bit set is most useful for the large blocks of CJK characters with FCD=0.
 *
 * Changes from format version 1 to format version 2 ---------------------------
 *
 * - Addition of data for raw (not recursively decomposed) mappings.
 *   + The MAPPING_NO_COMP_BOUNDARY_AFTER bit in the extraData is now also set when
 *     the mapping is to an empty string or when the character combines-forward.
 *     This subsumes the one actual use of the MAPPING_PLUS_COMPOSITION_LIST bit which
 *     is then repurposed for the MAPPING_HAS_RAW_MAPPING bit.
 *   + For details see the design doc.
 * - Addition of indexes[IX_MIN_YES_NO_MAPPINGS_ONLY] and separation of the yesNo extraData into
 *   distinct ranges (combines-forward vs. not)
 *   so that a range check can be used to find out if there is a compositions list.
 *   This is fully equivalent with formatVersion 1's MAPPING_PLUS_COMPOSITION_LIST flag.
 *   It is needed for the new (in ICU 49) composePair(), not for other normalization.
 * - Addition of the smallFCD[] bit set.
 *
 * Changes from format version 2 to format version 3 (ICU 60) ------------------
 *
 * - norm16 bit 0 indicates hasCompBoundaryAfter(),
 *   except that for contiguous composition (FCC) the tccc must be checked as well.
 *   Data indexes and ccc values are shifted left by one (OFFSET_SHIFT).
 *   Thresholds like minNoNo are tested before shifting.
 *
 * - Algorithmic mapping deltas are shifted left by two more bits (total DELTA_SHIFT),
 *   to make room for two bits (three values) indicating whether the tccc is 0, 1, or greater.
 *   See DELTA_TCCC_MASK etc.
 *   This helps with fetching tccc/FCD values and FCC hasCompBoundaryAfter().
 *   minMaybeNo is 8-aligned so that the DELTA_TCCC_MASK bits can be tested directly.
 *
 * - Algorithmic mappings are only used for mapping to "comp yes and ccc=0" characters,
 *   and ASCII characters are mapped algorithmically only to other ASCII characters.
 *   This helps with hasCompBoundaryBefore() and compose() fast paths.
 *   It is never necessary any more to loop for algorithmic mappings.
 *
 * - Addition of indexes[IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE],
 *   indexes[IX_MIN_NO_NO_COMP_NO_MAYBE_CC], and indexes[IX_MIN_NO_NO_EMPTY],
 *   and separation of the noNo extraData into distinct ranges.
 *   With this, the noNo norm16 value indicates whether the mapping is
 *   compose-normalized, not normalized but hasCompBoundaryBefore(),
 *   not even that, or maps to an empty string.
 *   hasCompBoundaryBefore() can be determined solely from the norm16 value.
 *
 * - The norm16 value for Hangul LVT is now different from that for Hangul LV,
 *   so that hasCompBoundaryAfter() need not check for the syllable type.
 *   For Hangul LV, minYesNo continues to be used (no comp-boundary-after).
 *   For Hangul LVT, minYesNoMappingsOnly|HAS_COMP_BOUNDARY_AFTER is used.
 *   The extraData units at these indexes are set to firstUnit=2 and firstUnit=3, respectively,
 *   to simplify some code.
 *
 * - The extraData firstUnit bit 5 is no longer necessary
 *   (norm16 bit 0 used instead of firstUnit MAPPING_NO_COMP_BOUNDARY_AFTER),
 *   is reserved again, and always set to 0.
 *
 * - Addition of indexes[IX_MIN_LCCC_CP], the first code point where lccc!=0.
 *   This used to be hardcoded to U+0300, but in data like NFKC_Casefold it is lower:
 *   U+00AD Soft Hyphen maps to an empty string,
 *   which is artificially assigned "worst case" values lccc=1 and tccc=255.
 *
 * - A mapping to an empty string has explicit lccc=1 and tccc=255 values.
 *
 * Changes from format version 3 to format version 4 (ICU 63) ------------------
 *
 * Switched from UTrie2 to UCPTrie/CodePointTrie.
 *
 * The new trie no longer stores different values for surrogate code *units* vs.
 * surrogate code *points*.
 * Lead surrogates still have values for optimized UTF-16 string processing.
 * When looking up code point properties, the code now checks for lead surrogates and
 * treats them as inert.
 *
 * gennorm2 now has to reject mappings for surrogate code points.
 * UTS #46 maps unpaired surrogates to U+FFFD in code rather than via its
 * custom normalization data file.
 *
 * Changes from format version 4 to format version 5 (ICU 76) ------------------
 *
 * Unicode 16 adds the first MaybeYes characters which combine both backward and forward,
 * taking this formerly theoretical data structure into reality.
 *
 * Unicode 16 also adds the first characters that have two-way mappings whose first characters
 * combine backward. In order for normalization and the quick check to work properly,
 * these composite characters also must be marked as NFC_QC=Maybe,
 * corresponding to "combines back", although the composites themselves do not combine backward.
 * Format version 5 adds two new ranges between "algorithmic NoNo" and MaybeYes,
 * with thresholds minMaybeNo and minMaybeNoCombinesFwd,
 * and indexes[IX_MIN_MAYBE_NO] and indexes[IX_MIN_MAYBE_NO_COMBINES_FWD],
 * and corresponding mappings and composition lists in the extraData.
 *
 * Format version 5 moves the data for Maybe* characters from the start of the extraData array
 * to its end.
 */

#पूर्ण_अगर  /* !UCONFIG_NO_NORMALIZATION */
#पूर्ण_अगर  /* __NORMALIZER2IMPL_H__ */
