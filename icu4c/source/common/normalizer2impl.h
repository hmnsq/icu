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

#यदि_अपरिभाषितम् __NORMALIZER2IMPL_H__
#परिभाषय __NORMALIZER2IMPL_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/unorm.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "uset_imp.h"

// When the nfc.nrm data is *not* hardcoded into the common library
// (with this constant set to 0),
// then it needs to be built into the data package:
// Add nfc.nrm to icu4c/source/data/Makefile.in DAT_FILES_SHORT
#परिभाषय NORM2_HARDCODE_NFC_दत्त 1

U_NAMESPACE_BEGIN

संरचना Canon_Iter_दत्त;

class ByteSink;
class Edits;
class Init_Canon_Iter_दत्त;
class LcccContext;

class U_COMMON_API Hangul {
सार्वजनिक:
    /* Korean Hangul and Jamo constants */
    गणना {
        JAMO_L_आधारः=0x1100,     /* "lead" jamo */
        JAMO_L_अन्तः=0x1112,
        JAMO_V_आधारः=0x1161,     /* "vowel" jamo */
        JAMO_V_अन्तः=0x1175,
        JAMO_T_आधारः=0x11a7,     /* "trail" jamo */
        JAMO_T_अन्तः=0x11c2,

        HANGUL_आधारः=0xac00,
        HANGUL_अन्तः=0xd7a3,

        JAMO_L_गणनम्=19,
        JAMO_V_गणनम्=21,
        JAMO_T_गणनम्=28,

        JAMO_VT_गणनम्=JAMO_V_गणनम्*JAMO_T_गणनम्,

        HANGUL_गणनम्=JAMO_L_गणनम्*JAMO_V_गणनम्*JAMO_T_गणनम्,
        HANGUL_सीमा=HANGUL_आधारः+HANGUL_गणनम्
    };

    स्थिर अन्तःस्थ UBool isHangul(UChar32 c) {
        प्रत्यागम HANGUL_आधारः<=c && c<HANGUL_सीमा;
    }
    स्थिर अन्तःस्थ UBool
    isHangulLV(UChar32 c) {
        c-=HANGUL_आधारः;
        प्रत्यागम 0<=c && c<HANGUL_गणनम् && c%JAMO_T_गणनम्==0;
    }
    स्थिर अन्तःस्थ UBool isJamoL(UChar32 c) {
        प्रत्यागम static_cast<uint32_t>(c - JAMO_L_आधारः) < JAMO_L_गणनम्;
    }
    स्थिर अन्तःस्थ UBool isJamoV(UChar32 c) {
        प्रत्यागम static_cast<uint32_t>(c - JAMO_V_आधारः) < JAMO_V_गणनम्;
    }
    स्थिर अन्तःस्थ UBool isJamoT(UChar32 c) {
        int32_t t=c-JAMO_T_आधारः;
        प्रत्यागम 0<t && t<JAMO_T_गणनम्;  // not JAMO_T_BASE itself
    }
    स्थिर UBool isJamo(UChar32 c) {
        प्रत्यागम JAMO_L_आधारः<=c && c<=JAMO_T_अन्तः &&
            (c<=JAMO_L_अन्तः || (JAMO_V_आधारः<=c && c<=JAMO_V_अन्तः) || JAMO_T_आधारः<c);
    }

    /**
     * Decomposes c, which must be a Hangul syllable, into buffer
     * and returns the length of the decomposition (2 or 3).
     */
    स्थिर अन्तःस्थ int32_t decompose(UChar32 c, char16_t बफर[3]) {
        c-=HANGUL_आधारः;
        UChar32 c2=c%JAMO_T_गणनम्;
        c/=JAMO_T_गणनम्;
        बफर[0] = static_cast<char16_t>(JAMO_L_आधारः + c / JAMO_V_गणनम्);
        बफर[1] = static_cast<char16_t>(JAMO_V_आधारः + c % JAMO_V_गणनम्);
        यदि(c2==0) {
            प्रत्यागम 2;
        } अन्यथा {
            बफर[2] = static_cast<char16_t>(JAMO_T_आधारः + c2);
            प्रत्यागम 3;
        }
    }

    /**
     * Decomposes c, which must be a Hangul syllable, into buffer.
     * This is the raw, not recursive, decomposition. Its length is always 2.
     */
    स्थिर अन्तःस्थ शून्य getRawDecomposition(UChar32 c, char16_t बफर[2]) {
        UChar32 orig=c;
        c-=HANGUL_आधारः;
        UChar32 c2=c%JAMO_T_गणनम्;
        यदि(c2==0) {
            c/=JAMO_T_गणनम्;
            बफर[0] = static_cast<char16_t>(JAMO_L_आधारः + c / JAMO_V_गणनम्);
            बफर[1] = static_cast<char16_t>(JAMO_V_आधारः + c % JAMO_V_गणनम्);
        } अन्यथा {
            बफर[0] = static_cast<char16_t>(orig - c2); // LV syllable
            बफर[1] = static_cast<char16_t>(JAMO_T_आधारः + c2);
        }
    }
गुप्त:
    Hangul() = delete;  // no instantiation
};

class Normalizer2Impl;

class U_COMMON_API Reordering_बफर : सार्वजनिक UMemory {
सार्वजनिक:
    /** Constructs only; init() should be called. */
    Reordering_बफर(ध्रुव Normalizer2Impl &ni, UnicodeString &dest) :
        impl(ni), str(dest),
        start(nullptr), reorderStart(nullptr), सीमा(nullptr),
        remaining_क्षमता(0), चरमः_CC(0) {}
    /** Constructs, removes the string contents, and initializes for a small initial capacity. */
    Reordering_बफर(ध्रुव Normalizer2Impl &ni, UnicodeString &dest, UError_संहिता &error_संहिता);
    ~Reordering_बफर() {
        यदि (start != nullptr) {
            str.release_बफर(static_cast<int32_t>(सीमा - start));
        }
    }
    UBool init(int32_t dest_क्षमता, UError_संहिता &error_संहिता);

    UBool is_रिक्तानि() ध्रुव { प्रत्यागम start==सीमा; }
    int32_t दीर्घता() ध्रुव { प्रत्यागम static_cast<int32_t>(सीमा - start); }
    char16_t *getStart() { प्रत्यागम start; }
    char16_t *get_सीमा() { प्रत्यागम सीमा; }
    uint8_t get_चरमः_CC() ध्रुव { प्रत्यागम चरमः_CC; }

    UBool equals(ध्रुव char16_t *start, ध्रुव char16_t *सीमा) ध्रुव;
    UBool equals(ध्रुव uint8_t *otherStart, ध्रुव uint8_t *other_सीमा) ध्रुव;

    UBool append(UChar32 c, uint8_t cc, UError_संहिता &error_संहिता) {
        प्रत्यागम (c<=0xffff) ?
            appendBMP(static_cast<char16_t>(c), cc, error_संहिता) :
            appendSupplementary(c, cc, error_संहिता);
    }
    UBool append(ध्रुव char16_t *s, int32_t दीर्घता, UBool isNFD,
                 uint8_t leadCC, uint8_t trailCC,
                 UError_संहिता &error_संहिता);
    UBool appendBMP(char16_t c, uint8_t cc, UError_संहिता &error_संहिता) {
        यदि(remaining_क्षमता==0 && !resize(1, error_संहिता)) {
            प्रत्यागम false;
        }
        यदि(चरमः_CC<=cc || cc==0) {
            *सीमा++=c;
            चरमः_CC=cc;
            यदि(cc<=1) {
                reorderStart=सीमा;
            }
        } अन्यथा {
            निवेशय(c, cc);
        }
        --remaining_क्षमता;
        प्रत्यागम true;
    }
    UBool appendZeroCC(UChar32 c, UError_संहिता &error_संहिता);
    UBool appendZeroCC(ध्रुव char16_t *s, ध्रुव char16_t *s_सीमा, UError_संहिता &error_संहिता);
    शून्य remove();
    शून्य removeSuffix(int32_t suffix_दीर्घता);
    शून्य समुच्चयः_Reordering_सीमा(char16_t *new_सीमा) {
        remaining_क्षमता += static_cast<int32_t>(सीमा - new_सीमा);
        reorderStart=सीमा=new_सीमा;
        चरमः_CC=0;
    }
    शून्य copy_Reorderable_Suffix_गन्तव्यम्(UnicodeString &s) ध्रुव {
        s.समुच्चयः_गन्तव्यम्(ConstChar16Ptr(reorderStart), static_cast<int32_t>(सीमा - reorderStart));
    }
गुप्त:
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

    UBool appendSupplementary(UChar32 c, uint8_t cc, UError_संहिता &error_संहिता);
    शून्य निवेशय(UChar32 c, uint8_t cc);
    स्थिर शून्य write_संहिता_Point(char16_t *p, UChar32 c) {
        यदि(c<=0xffff) {
            *p = static_cast<char16_t>(c);
        } अन्यथा {
            p[0]=U16_LEAD(c);
            p[1]=U16_TRAIL(c);
        }
    }
    UBool resize(int32_t append_दीर्घता, UError_संहिता &error_संहिता);

    ध्रुव Normalizer2Impl &impl;
    UnicodeString &str;
    char16_t *start, *reorderStart, *सीमा;
    int32_t remaining_क्षमता;
    uint8_t चरमः_CC;

    // private backward iterator
    शून्य समुच्चयः_Iterator() { संहिता_Point_Start=सीमा; }
    शून्य skipPrevious();  // Requires start<codePointStart.
    uint8_t previousCC();  // Returns 0 if there is no previous character.

    char16_t *संहिता_Point_Start, *संहिता_Point_सीमा;
};

/**
 * Low-level implementation of the Unicode Normalization Algorithm.
 * For the data structure and details see the documentation at the end of
 * this normalizer2impl.h and in the design doc at
 * https://unicode-org.github.io/icu/design/normalization/custom.html
 */
class U_COMMON_API_CLASS Normalizer2Impl : सार्वजनिक UObject {
सार्वजनिक:
    U_COMMON_API Normalizer2Impl() : normTrie(nullptr), f_Canon_Iter_दत्त(nullptr) {}
    U_COMMON_API आभास ~Normalizer2Impl();

    U_COMMON_API शून्य init(ध्रुव int32_t* अन्तर्_Indexes,
                           ध्रुव UCPTrie* अन्तर्_Trie,
                           ध्रुव uint16_t* अन्तर्_Extra_दत्त,
                           ध्रुव uint8_t* अन्तर्_Small_FCD);

    U_COMMON_API शून्य सङ्कलय_Lccc_Chars(Unicode_समुच्चयः& समुच्चयः) ध्रुव;
    U_COMMON_API शून्य सङ्कलय_Property_Starts(ध्रुव USetAdder* sa, UError_संहिता& error_संहिता) ध्रुव;
    U_COMMON_API शून्य सङ्कलय_Canon_Iter_Property_Starts(ध्रुव USetAdder* sa, UError_संहिता& error_संहिता) ध्रुव;

    // low-level properties ------------------------------------------------ ***

    U_COMMON_API UBool ensure_Canon_Iter_दत्त(UError_संहिता& error_संहिता) ध्रुव;

    // The trie stores values for lead surrogate code *units*.
    // Surrogate code *points* are inert.
    U_COMMON_API uint16_t getNorm16(UChar32 c) ध्रुव {
        प्रत्यागम U_IS_LEAD(c) ?
            static_cast<uint16_t>(INERT) :
            UCPTRIE_FAST_GET(normTrie, UCPTRIE_16, c);
    }
    U_COMMON_API uint16_t getRawNorm16(UChar32 c) ध्रुव {
        प्रत्यागम UCPTRIE_FAST_GET(normTrie, UCPTRIE_16, c);
    }

    U_COMMON_API UNormalization_Check_परिणाम get_Comp_शीघ्र_Check(uint16_t norm16) ध्रुव {
        यदि(norm16<minNoNo || MIN_YES_YES_WITH_CC<=norm16) {
            प्रत्यागम UNORM_YES;
        } अन्यथा यदि(minMaybeNo<=norm16) {
            प्रत्यागम UNORM_MAYBE;
        } अन्यथा {
            प्रत्यागम UNORM_NO;
        }
    }
    U_COMMON_API UBool isAlgorithmicNoNo(uint16_t norm16) ध्रुव {
        प्रत्यागम सीमा_No_No <= norm16 && norm16 < minMaybeNo;
    }
    U_COMMON_API UBool isCompNo(uint16_t norm16) ध्रुव {
        प्रत्यागम minNoNo <= norm16 && norm16 < minMaybeNo;
    }
    U_COMMON_API UBool isDecompYes(uint16_t norm16) ध्रुव {
        प्रत्यागम norm16 < minYesNo || minMaybeYes <= norm16;
    }

    U_COMMON_API uint8_t getCC(uint16_t norm16) ध्रुव {
        यदि(norm16>=MIN_प्रसामान्यम्_MAYBE_YES) {
            प्रत्यागम get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(norm16);
        }
        यदि(norm16<minNoNo || सीमा_No_No<=norm16) {
            प्रत्यागम 0;
        }
        प्रत्यागम getCCFromNoNo(norm16);
    }
    U_COMMON_API स्थिर uint8_t get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(uint16_t norm16) {
        प्रत्यागम static_cast<uint8_t>(norm16 >> विस्थापनम्_सरणम्);
    }
    U_COMMON_API स्थिर uint8_t getCCFromYesOrMaybeYes(uint16_t norm16) {
        प्रत्यागम norm16>=MIN_प्रसामान्यम्_MAYBE_YES ? get_CCFrom_प्रसामान्यम्_Yes_Or_Maybe(norm16) : 0;
    }
    U_COMMON_API uint8_t getCCFromYesOrMaybeYesCP(UChar32 c) ध्रुव {
        यदि (c < minCompNoMaybeCP) { प्रत्यागम 0; }
        प्रत्यागम getCCFromYesOrMaybeYes(getNorm16(c));
    }

    /**
     * Returns the FCD data for code point c.
     * @param c A Unicode code point.
     * @return The lccc(c) in bits 15..8 and tccc(c) in bits 7..0.
     */
    U_COMMON_API uint16_t getFCD16(UChar32 c) ध्रुव {
        यदि(c<minDecompNoCP) {
            प्रत्यागम 0;
        } अन्यथा यदि(c<=0xffff) {
            यदि(!singleLeadMightHaveNonZeroFCD16(c)) { प्रत्यागम 0; }
        }
        प्रत्यागम get_FCD16_From_Norm_दत्त(c);
    }
    /**
     * Returns the FCD data for the next code point (post-increment).
     * Might skip only a lead surrogate rather than the whole surrogate pair if none of
     * the supplementary code points associated with the lead surrogate have non-zero FCD data.
     * @param s A valid pointer into a string. Requires s!=limit.
     * @param limit The end of the string, or NULL.
     * @return The lccc(c) in bits 15..8 and tccc(c) in bits 7..0.
     */
    U_COMMON_API uint16_t अग्र_FCD16(ध्रुव char16_t*& s, ध्रुव char16_t* सीमा) ध्रुव {
        UChar32 c=*s++;
        यदि(c<minDecompNoCP || !singleLeadMightHaveNonZeroFCD16(c)) {
            प्रत्यागम 0;
        }
        char16_t c2;
        यदि(U16_IS_LEAD(c) && s!=सीमा && U16_IS_TRAIL(c2=*s)) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
            ++s;
        }
        प्रत्यागम get_FCD16_From_Norm_दत्त(c);
    }
    /**
     * Returns the FCD data for the previous code point (pre-decrement).
     * @param start The start of the string.
     * @param s A valid pointer into a string. Requires start<s.
     * @return The lccc(c) in bits 15..8 and tccc(c) in bits 7..0.
     */
    U_COMMON_API uint16_t previousFCD16(ध्रुव char16_t* start, ध्रुव char16_t*& s) ध्रुव {
        UChar32 c=*--s;
        यदि(c<minDecompNoCP) {
            प्रत्यागम 0;
        }
        यदि(!U16_IS_TRAIL(c)) {
            यदि(!singleLeadMightHaveNonZeroFCD16(c)) {
                प्रत्यागम 0;
            }
        } अन्यथा {
            char16_t c2;
            यदि(start<s && U16_IS_LEAD(c2=*(s-1))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
                --s;
            }
        }
        प्रत्यागम get_FCD16_From_Norm_दत्त(c);
    }

    /** Returns true if the single-or-lead code unit c might have non-zero FCD data. */
    U_COMMON_API UBool singleLeadMightHaveNonZeroFCD16(UChar32 lead) ध्रुव {
        // 0<=lead<=0xffff
        uint8_t द्वयङ्काः=smallFCD[lead>>8];
        यदि(द्वयङ्काः==0) { प्रत्यागम false; }
        प्रत्यागम (द्वयङ्काः >> ((lead >> 5) & 7)) & 1;
    }
    /** Returns the FCD value from the regular normalization data. */
    U_COMMON_API uint16_t get_FCD16_From_Norm_दत्त(UChar32 c) ध्रुव;

    U_COMMON_API uint16_t getFCD16FromMaybeOrNonZeroCC(uint16_t norm16) ध्रुव;

    /**
     * Gets the decomposition for one code point.
     * @param c code point
     * @param buffer out-only buffer for algorithmic decompositions
     * @param length out-only, takes the length of the decomposition, if any
     * @return pointer to the decomposition, or NULL if none
     */
    U_COMMON_API ध्रुव char16_t* getDecomposition(UChar32 c, char16_t बफर[4], int32_t& दीर्घता) ध्रुव;

    /**
     * Gets the raw decomposition for one code point.
     * @param c code point
     * @param buffer out-only buffer for algorithmic decompositions
     * @param length out-only, takes the length of the decomposition, if any
     * @return pointer to the decomposition, or NULL if none
     */
    U_COMMON_API ध्रुव char16_t* getRawDecomposition(UChar32 c,
                                                     char16_t बफर[30],
                                                     int32_t& दीर्घता) ध्रुव;

    U_COMMON_API UChar32 composePair(UChar32 a, UChar32 b) ध्रुव;

    U_COMMON_API UBool isCanonSegmentStarter(UChar32 c) ध्रुव;
    U_COMMON_API UBool get_Canon_Start_समुच्चयः(UChar32 c, Unicode_समुच्चयः& समुच्चयः) ध्रुव;

    गणना {
        // Fixed norm16 values.
        MIN_YES_YES_WITH_CC=0xfe02,
        JAMO_VT=0xfe00,
        MIN_प्रसामान्यम्_MAYBE_YES=0xfc00,
        JAMO_L=2,  // offset=1 hasCompBoundaryAfter=false
        INERT=1,  // offset=0 hasCompBoundaryAfter=true

        // norm16 bit 0 is comp-boundary-after.
        HAS_COMP_BOUNDARY_AFTER=1,
        विस्थापनम्_सरणम्=1,

        // For algorithmic one-way mappings, norm16 bits 2..1 indicate the
        // tccc (0, 1, >1) for quick FCC boundary-after tests.
        DELTA_TCCC_0=0,
        DELTA_TCCC_1=2,
        DELTA_TCCC_GT_1=4,
        DELTA_TCCC_आवरणम्=6,
        DELTA_सरणम्=3,

        MAX_DELTA=0x40
    };

    गणना {
        // Byte offsets from the start of the data, after the generic header.
        IX_NORM_TRIE_विस्थापनम्,
        IX_EXTRA_दत्त_विस्थापनम्,
        IX_SMALL_FCD_विस्थापनम्,
        IX_RESERVED3_विस्थापनम्,
        IX_RESERVED4_विस्थापनम्,
        IX_RESERVED5_विस्थापनम्,
        IX_RESERVED6_विस्थापनम्,
        IX_योग_आकार,

        // Code point thresholds for quick check codes.
        IX_MIN_DECOMP_NO_CP,
        IX_MIN_COMP_NO_MAYBE_CP,

        // Norm16 value thresholds for quick check combinations and types of extra data.

        /** Mappings & compositions in [minYesNo..minYesNoMappingsOnly[. */
        IX_MIN_YES_NO,
        /** Mappings are comp-normalized. */
        IX_MIN_NO_NO,
        IX_सीमा_NO_NO,
        IX_MIN_MAYBE_YES,

        /** Mappings only in [minYesNoMappingsOnly..minNoNo[. */
        IX_MIN_YES_NO_MAPPINGS_ONLY,
        /** Mappings are not comp-normalized but have a comp boundary before. */
        IX_MIN_NO_NO_COMP_BOUNDARY_BEFORE,
        /** Mappings do not have a comp boundary before. */
        IX_MIN_NO_NO_COMP_NO_MAYBE_CC,
        /** Mappings to the empty string. */
        IX_MIN_NO_NO_रिक्तानि,

        IX_MIN_LCCC_CP,
        IX_RESERVED19,

        /** Two-way mappings; each starts with a character that combines backward. */
        IX_MIN_MAYBE_NO,  // 20
        /** Two-way mappings & compositions. */
        IX_MIN_MAYBE_NO_COMBINES_FWD,

        IX_गणनम्  // 22
    };

    गणना {
        MAPPING_HAS_CCC_LCCC_WORD=0x80,
        MAPPING_HAS_RAW_MAPPING=0x40,
        // unused bit 0x20,
        MAPPING_दीर्घता_आवरणम्=0x1f
    };

    गणना {
        COMP_1_चरमः_TUPLE=0x8000,
        COMP_1_TRIPLE=1,
        COMP_1_TRAIL_सीमा=0x3400,
        COMP_1_TRAIL_आवरणम्=0x7ffe,
        COMP_1_TRAIL_सरणम्=9,  // 10-1 for the "triple" bit
        COMP_2_TRAIL_सरणम्=6,
        COMP_2_TRAIL_आवरणम्=0xffc0
    };

    // higher-level functionality ------------------------------------------ ***

    // NFD without an NFD Normalizer2 instance.
    U_COMMON_API UnicodeString& decompose(ध्रुव UnicodeString& src,
                                          UnicodeString& dest,
                                          UError_संहिता& error_संहिता) ध्रुव;
    /**
     * Decomposes [src, limit[ and writes the result to dest.
     * limit can be NULL if src is NUL-terminated.
     * destLengthEstimate is the initial dest buffer capacity and can be -1.
     */
    U_COMMON_API शून्य decompose(ध्रुव char16_t* src,
                                ध्रुव char16_t* सीमा,
                                UnicodeString& dest,
                                int32_t dest_दीर्घता_Estimate,
                                UError_संहिता& error_संहिता) ध्रुव;

    U_COMMON_API ध्रुव char16_t* decompose(ध्रुव char16_t* src,
                                           ध्रुव char16_t* सीमा,
                                           Reordering_बफर* बफर,
                                           UError_संहिता& error_संहिता) ध्रुव;
    U_COMMON_API शून्य decomposeAndAppend(ध्रुव char16_t* src,
                                         ध्रुव char16_t* सीमा,
                                         UBool doDecompose,
                                         UnicodeString& safe_मध्यः,
                                         Reordering_बफर& बफर,
                                         UError_संहिता& error_संहिता) ध्रुव;

    /** sink==nullptr: isNormalized()/spanQuickCheckYes() */
    U_COMMON_API ध्रुव uint8_t* decomposeUTF8(uint32_t options,
                                              ध्रुव uint8_t* src,
                                              ध्रुव uint8_t* सीमा,
                                              ByteSink* sink,
                                              Edits* edits,
                                              UError_संहिता& error_संहिता) ध्रुव;

    U_COMMON_API UBool compose(ध्रुव char16_t* src,
                               ध्रुव char16_t* सीमा,
                               UBool onlyContiguous,
                               UBool doCompose,
                               Reordering_बफर& बफर,
                               UError_संहिता& error_संहिता) ध्रुव;
    U_COMMON_API ध्रुव char16_t* compose_शीघ्र_Check(ध्रुव char16_t* src,
                                                   ध्रुव char16_t* सीमा,
                                                   UBool onlyContiguous,
                                                   UNormalization_Check_परिणाम* pQCResult) ध्रुव;
    U_COMMON_API शून्य composeAndAppend(ध्रुव char16_t* src,
                                       ध्रुव char16_t* सीमा,
                                       UBool doCompose,
                                       UBool onlyContiguous,
                                       UnicodeString& safe_मध्यः,
                                       Reordering_बफर& बफर,
                                       UError_संहिता& error_संहिता) ध्रुव;

    /** sink==nullptr: isNormalized() */
    U_COMMON_API UBool composeUTF8(uint32_t options,
                                   UBool onlyContiguous,
                                   ध्रुव uint8_t* src,
                                   ध्रुव uint8_t* सीमा,
                                   ByteSink* sink,
                                   icu::Edits* edits,
                                   UError_संहिता& error_संहिता) ध्रुव;

    U_COMMON_API ध्रुव char16_t* makeFCD(ध्रुव char16_t* src,
                                         ध्रुव char16_t* सीमा,
                                         Reordering_बफर* बफर,
                                         UError_संहिता& error_संहिता) ध्रुव;
    U_COMMON_API शून्य makeFCDAndAppend(ध्रुव char16_t* src,
                                       ध्रुव char16_t* सीमा,
                                       UBool doMakeFCD,
                                       UnicodeString& safe_मध्यः,
                                       Reordering_बफर& बफर,
                                       UError_संहिता& error_संहिता) ध्रुव;

    U_COMMON_API UBool hasDecompBoundaryBefore(UChar32 c) ध्रुव;
    U_COMMON_API UBool norm16HasDecompBoundaryBefore(uint16_t norm16) ध्रुव;
    U_COMMON_API UBool hasDecompBoundaryAfter(UChar32 c) ध्रुव;
    U_COMMON_API UBool norm16HasDecompBoundaryAfter(uint16_t norm16) ध्रुव;
    U_COMMON_API UBool isDecompInert(UChar32 c) ध्रुव { प्रत्यागम isDecompYesAndZeroCC(getNorm16(c)); }

    U_COMMON_API UBool hasCompBoundaryBefore(UChar32 c) ध्रुव {
        प्रत्यागम c<minCompNoMaybeCP || norm16HasCompBoundaryBefore(getNorm16(c));
    }
    U_COMMON_API UBool hasCompBoundaryAfter(UChar32 c, UBool onlyContiguous) ध्रुव {
        प्रत्यागम norm16HasCompBoundaryAfter(getNorm16(c), onlyContiguous);
    }
    U_COMMON_API UBool isCompInert(UChar32 c, UBool onlyContiguous) ध्रुव {
        uint16_t norm16=getNorm16(c);
        प्रत्यागम isCompYesAndZeroCC(norm16) &&
            (norm16 & HAS_COMP_BOUNDARY_AFTER) != 0 &&
            (!onlyContiguous || isInert(norm16) || *get_दत्त_For_Yes_Or_No(norm16) <= 0x1ff);
            // The last check fetches the mapping's first unit and checks tccc<=1.
    }

    U_COMMON_API UBool hasFCDBoundaryBefore(UChar32 c) ध्रुव { प्रत्यागम hasDecompBoundaryBefore(c); }
    U_COMMON_API UBool hasFCDBoundaryAfter(UChar32 c) ध्रुव { प्रत्यागम hasDecompBoundaryAfter(c); }
    U_COMMON_API UBool isFCDInert(UChar32 c) ध्रुव { प्रत्यागम getFCD16(c) <= 1; }

  गुप्त:
    मित्रम् class Init_Canon_Iter_दत्त;
    मित्रम् class LcccContext;

    UBool isMaybe(uint16_t norm16) ध्रुव { प्रत्यागम minMaybeNo<=norm16 && norm16<=JAMO_VT; }
    UBool isMaybeYesOrNonZeroCC(uint16_t norm16) ध्रुव { प्रत्यागम norm16>=minMaybeYes; }
    स्थिर UBool isInert(uint16_t norm16) { प्रत्यागम norm16==INERT; }
    स्थिर UBool isJamoL(uint16_t norm16) { प्रत्यागम norm16==JAMO_L; }
    स्थिर UBool isJamoVT(uint16_t norm16) { प्रत्यागम norm16==JAMO_VT; }
    uint16_t hangulLVT() ध्रुव { प्रत्यागम minYesNoMappingsOnly|HAS_COMP_BOUNDARY_AFTER; }
    UBool isHangulLV(uint16_t norm16) ध्रुव { प्रत्यागम norm16==minYesNo; }
    UBool isHangulLVT(uint16_t norm16) ध्रुव {
        प्रत्यागम norm16==hangulLVT();
    }
    UBool isCompYesAndZeroCC(uint16_t norm16) ध्रुव { प्रत्यागम norm16<minNoNo; }
    // UBool isCompYes(uint16_t norm16) const {
    //     return norm16>=MIN_YES_YES_WITH_CC || norm16<minNoNo;
    // }
    // UBool isCompYesOrMaybe(uint16_t norm16) const {
    //     return norm16<minNoNo || minMaybeNo<=norm16;
    // }
    // UBool hasZeroCCFromDecompYes(uint16_t norm16) const {
    //     return norm16<=MIN_NORMAL_MAYBE_YES || norm16==JAMO_VT;
    // }
    UBool isDecompYesAndZeroCC(uint16_t norm16) ध्रुव {
        प्रत्यागम norm16<minYesNo ||
               norm16==JAMO_VT ||
               (minMaybeYes<=norm16 && norm16<=MIN_प्रसामान्यम्_MAYBE_YES);
    }
    /**
     * A little faster and simpler than isDecompYesAndZeroCC() but does not include
     * the MaybeYes which combine-forward and have ccc=0.
     */
    UBool isMostDecompYesAndZeroCC(uint16_t norm16) ध्रुव {
        प्रत्यागम norm16<minYesNo || norm16==MIN_प्रसामान्यम्_MAYBE_YES || norm16==JAMO_VT;
    }
    /** Since formatVersion 5: same as isAlgorithmicNoNo() */
    UBool isDecompNoAlgorithmic(uint16_t norm16) ध्रुव { प्रत्यागम सीमा_No_No<=norm16 && norm16<minMaybeNo; }

    // For use with isCompYes().
    // Perhaps the compiler can combine the two tests for MIN_YES_YES_WITH_CC.
    // static uint8_t getCCFromYes(uint16_t norm16) {
    //     return norm16>=MIN_YES_YES_WITH_CC ? getCCFromNormalYesOrMaybe(norm16) : 0;
    // }
    uint8_t getCCFromNoNo(uint16_t norm16) ध्रुव {
        ध्रुव uint16_t *mapping=get_दत्त_For_Yes_Or_No(norm16);
        यदि(*mapping&MAPPING_HAS_CCC_LCCC_WORD) {
            प्रत्यागम static_cast<uint8_t>(*(mapping - 1));
        } अन्यथा {
            प्रत्यागम 0;
        }
    }
    // requires that the [cpStart..cpLimit[ character passes isCompYesAndZeroCC()
    uint8_t getTrailCCFromCompYesAndZeroCC(uint16_t norm16) ध्रुव {
        यदि(norm16<=minYesNo) {
            प्रत्यागम 0;  // yesYes and Hangul LV have ccc=tccc=0
        } अन्यथा {
            // For Hangul LVT we harmlessly fetch a firstUnit with tccc=0 here.
            प्रत्यागम static_cast<uint8_t>(*get_दत्त_For_Yes_Or_No(norm16) >> 8); // tccc from yesNo
        }
    }
    uint8_t getPreviousTrailCC(ध्रुव char16_t *start, ध्रुव char16_t *p) ध्रुव;
    uint8_t getPreviousTrailCC(ध्रुव uint8_t *start, ध्रुव uint8_t *p) ध्रुव;

    // Requires algorithmic-NoNo.
    UChar32 mapAlgorithmic(UChar32 c, uint16_t norm16) ध्रुव {
        प्रत्यागम c+(norm16>>DELTA_सरणम्)-centerNoNoDelta;
    }
    UChar32 getAlgorithmicDelta(uint16_t norm16) ध्रुव {
        प्रत्यागम (norm16>>DELTA_सरणम्)-centerNoNoDelta;
    }

    ध्रुव uint16_t *get_दत्त_For_Yes_Or_No(uint16_t norm16) ध्रुव {
        प्रत्यागम extra_दत्त+(norm16>>विस्थापनम्_सरणम्);
    }
    ध्रुव uint16_t *get_दत्त_For_Maybe(uint16_t norm16) ध्रुव {
        प्रत्यागम extra_दत्त+((norm16-minMaybeNo+सीमा_No_No)>>विस्थापनम्_सरणम्);
    }
    ध्रुव uint16_t *get_दत्त(uint16_t norm16) ध्रुव {
        यदि(norm16>=minMaybeNo) {
            norm16=norm16-minMaybeNo+सीमा_No_No;
        }
        प्रत्यागम extra_दत्त+(norm16>>विस्थापनम्_सरणम्);
    }
    ध्रुव uint16_t *getCompositionsListForDecompYes(uint16_t norm16) ध्रुव {
        यदि(norm16<JAMO_L || MIN_प्रसामान्यम्_MAYBE_YES<=norm16) {
            प्रत्यागम nullptr;
        } अन्यथा {
            // if yesYes: if Jamo L: harmless empty list
            प्रत्यागम get_दत्त(norm16);
        }
    }
    ध्रुव uint16_t *get_Compositions_List_For_संयुक्त(uint16_t norm16) ध्रुव {
        // A composite has both mapping & compositions list.
        ध्रुव uint16_t *list=get_दत्त(norm16);
        प्रत्यागम list+  // mapping pointer
            1+  // +1 to skip the first unit with the mapping length
            (*list&MAPPING_दीर्घता_आवरणम्);  // + mapping length
    }
    /**
     * @param c code point must have compositions
     * @return compositions list pointer
     */
    ध्रुव uint16_t *getCompositionsList(uint16_t norm16) ध्रुव {
        प्रत्यागम isDecompYes(norm16) ?
                getCompositionsListForDecompYes(norm16) :
                get_Compositions_List_For_संयुक्त(norm16);
    }

    ध्रुव char16_t *copy_नीचः_उपसर्गः_From_Nul_Terminated(ध्रुव char16_t *src,
                                                UChar32 min_Need_दत्त_CP,
                                                Reordering_बफर *बफर,
                                                UError_संहिता &error_संहिता) ध्रुव;

    गणना StopAt { STOP_AT_सीमा, STOP_AT_DECOMP_BOUNDARY, STOP_AT_COMP_BOUNDARY };

    ध्रुव char16_t *decomposeShort(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
                                UBool stopAtCompBoundary, UBool onlyContiguous,
                                Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव;
    UBool decompose(UChar32 c, uint16_t norm16,
                    Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव;

    ध्रुव uint8_t *decomposeShort(ध्रुव uint8_t *src, ध्रुव uint8_t *सीमा,
                                  StopAt stopAt, UBool onlyContiguous,
                                  Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव;

    स्थिर int32_t combine(ध्रुव uint16_t *list, UChar32 trail);
    शून्य सङ्कलय_Composites(ध्रुव uint16_t *list, Unicode_समुच्चयः &समुच्चयः) ध्रुव;
    शून्य recompose(Reordering_बफर &बफर, int32_t recompose_Start_सूचकांक,
                   UBool onlyContiguous) ध्रुव;

    UBool hasCompBoundaryBefore(UChar32 c, uint16_t norm16) ध्रुव {
        प्रत्यागम c<minCompNoMaybeCP || norm16HasCompBoundaryBefore(norm16);
    }
    UBool norm16HasCompBoundaryBefore(uint16_t norm16) ध्रुव  {
        प्रत्यागम norm16 < minNoNoCompNoMaybeCC || isAlgorithmicNoNo(norm16);
    }
    UBool hasCompBoundaryBefore(ध्रुव char16_t *src, ध्रुव char16_t *सीमा) ध्रुव;
    UBool hasCompBoundaryBefore(ध्रुव uint8_t *src, ध्रुव uint8_t *सीमा) ध्रुव;
    UBool hasCompBoundaryAfter(ध्रुव char16_t *start, ध्रुव char16_t *p,
                               UBool onlyContiguous) ध्रुव;
    UBool hasCompBoundaryAfter(ध्रुव uint8_t *start, ध्रुव uint8_t *p,
                               UBool onlyContiguous) ध्रुव;
    UBool norm16HasCompBoundaryAfter(uint16_t norm16, UBool onlyContiguous) ध्रुव {
        प्रत्यागम (norm16 & HAS_COMP_BOUNDARY_AFTER) != 0 &&
            (!onlyContiguous || isTrailCC01ForCompBoundaryAfter(norm16));
    }
    /** For FCC: Given norm16 HAS_COMP_BOUNDARY_AFTER, does it have tccc<=1? */
    UBool isTrailCC01ForCompBoundaryAfter(uint16_t norm16) ध्रुव {
        प्रत्यागम isInert(norm16) || (isDecompNoAlgorithmic(norm16) ?
            (norm16 & DELTA_TCCC_आवरणम्) <= DELTA_TCCC_1 : *get_दत्त_For_Yes_Or_No(norm16) <= 0x1ff);
    }

    ध्रुव char16_t *findPreviousCompBoundary(ध्रुव char16_t *start, ध्रुव char16_t *p,
                                             UBool onlyContiguous) ध्रुव;
    ध्रुव char16_t *find_अग्र_Comp_Boundary(ध्रुव char16_t *p, ध्रुव char16_t *सीमा,
                                         UBool onlyContiguous) ध्रुव;

    ध्रुव char16_t *findPreviousFCDBoundary(ध्रुव char16_t *start, ध्रुव char16_t *p) ध्रुव;
    ध्रुव char16_t *find_अग्र_FCDBoundary(ध्रुव char16_t *p, ध्रुव char16_t *सीमा) ध्रुव;

    शून्य make_Canon_Iter_दत्त_From_Norm16(UChar32 start, UChar32 अन्तः, ध्रुव uint16_t norm16,
                                     Canon_Iter_दत्त &new_दत्त, UError_संहिता &error_संहिता) ध्रुव;

    int32_t get_Canon_मानम्(UChar32 c) ध्रुव;
    ध्रुव Unicode_समुच्चयः &get_Canon_Start_समुच्चयः(int32_t n) ध्रुव;

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
    uint16_t min_No_No_रिक्तानि;
    uint16_t सीमा_No_No;
    uint16_t centerNoNoDelta;
    uint16_t minMaybeNo;
    uint16_t minMaybeNoCombinesFwd;
    uint16_t minMaybeYes;

    ध्रुव UCPTrie *normTrie;
    ध्रुव uint16_t *extra_दत्त;  // mappings and/or compositions
    ध्रुव uint8_t *smallFCD;  // [0x100] one bit per 32 BMP code points, set if any FCD!=0

    UInitOnce       f_Canon_Iter_दत्त_Init_Once {};
    Canon_Iter_दत्त  *f_Canon_Iter_दत्त;
};

// bits in canonIterData
#परिभाषय CANON_NOT_SEGMENT_STARTER 0x80000000
#परिभाषय CANON_HAS_COMPOSITIONS 0x40000000
#परिभाषय CANON_HAS_समुच्चयः 0x200000
#परिभाषय CANON_मानम्_आवरणम् 0x1fffff

/**
 * ICU-internal shortcut for quick access to standard Unicode normalization.
 */
class U_COMMON_API Normalizer2Factory {
सार्वजनिक:
    स्थिर ध्रुव Normalizer2 *getFCDInstance(UError_संहिता &error_संहिता);
    स्थिर ध्रुव Normalizer2 *getFCCInstance(UError_संहिता &error_संहिता);
    स्थिर ध्रुव Normalizer2 *getNoopInstance(UError_संहिता &error_संहिता);

    स्थिर ध्रुव Normalizer2 *getInstance(UNormalization_बहुलकम् बहुलकम्, UError_संहिता &error_संहिता);

    स्थिर ध्रुव Normalizer2Impl *getNFCImpl(UError_संहिता &error_संहिता);
    स्थिर ध्रुव Normalizer2Impl *getNFKCImpl(UError_संहिता &error_संहिता);
    स्थिर ध्रुव Normalizer2Impl *getNFKC_CFImpl(UError_संहिता &error_संहिता);

    // Get the Impl instance of the Normalizer2.
    // Must be used only when it is known that norm2 is a Normalizer2WithImpl instance.
    स्थिर ध्रुव Normalizer2Impl* getImpl(ध्रुव Normalizer2* norm2 U_LIFETIME_BOUND);
गुप्त:
    Normalizer2Factory() = delete;  // No instantiation.
};

U_NAMESPACE_अन्तः

U_CAPI int32_t U_EXPORT2
unorm2_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *अन्तर्_दत्त, int32_t दीर्घता, शून्य *बहिर्गमनम्_दत्त,
            UError_संहिता *p_Error_संहिता);

/**
 * Get the NF*_QC property for a code point, for u_getIntPropertyValue().
 * @internal
 */
U_CFUNC UNormalization_Check_परिणाम
unorm_get_शीघ्र_Check(UChar32 c, UNormalization_बहुलकम् बहुलकम्);

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

#यदि_अन्तः  /* !UCONFIG_NO_NORMALIZATION */
#यदि_अन्तः  /* __NORMALIZER2IMPL_H__ */
