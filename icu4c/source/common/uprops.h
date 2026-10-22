// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uprops.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002feb24
*   created by: Markus W. Scherer
*
*   Constants for mostly non-core Unicode character properties
*   stored in uprops.icu.
*/

#جيڪڏهن_وصف_نه_ٿيل __UPROPS_H__
#وصف_ڪريو __UPROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"
#ضمن "uset_imp.h"
#ضمن "udataswp.h"

/* indexes[] entries */
تعداد {
    UPROPS_PROPS32_اشاريو,
    UPROPS_EXCEPTIONS_اشاريو,
    UPROPS_EXCEPTIONS_چوٽي_اشاريو,

    UPROPS_ADDITIONAL_TRIE_اشاريو,
    UPROPS_ADDITIONAL_VECTORS_اشاريو,
    UPROPS_ADDITIONAL_VECTORS_COLUMNS_اشاريو,

    UPROPS_SCRIPT_EXTENSIONS_اشاريو,

    UPROPS_بلاڪ_TRIE_اشاريو,
    UPROPS_RESERVED_اشاريو_8,

    /** size of the data file (number of 32-bit units after the header) */
    UPROPS_معلومات_چوٽي_اشاريو,

    /** maximum values for code values in vector word 0 */
    UPROPS_MAX_قدرون_اشاريو=10,
    /** maximum values for code values in vector word 2 */
    UPROPS_MAX_قدرون_2_اشاريو,
    /** maximum values for other code values */
    UPROPS_MAX_قدرون_OTHER_اشاريو,

    UPROPS_اشاريو_ڳڻپ=16
};

/* definitions for the main properties words */
تعداد {
    /* general category shift==0                                0 (5 bits) */
    /* reserved                                                 5 (1 bit) */
    UPROPS_NUMERIC_TYPE_قدر_سرڪايو=6                       /*  6 (10 bits) */
};

#وصف_ڪريو GET_CATEGORY(props) ((props)&0x1f)
#وصف_ڪريو CAT_نقاب(props) U_نقاب(GET_CATEGORY(props))

#وصف_ڪريو GET_NUMERIC_TYPE_قدر(props) ((props)>>UPROPS_NUMERIC_TYPE_قدر_سرڪايو)

/* constants for the storage form of numeric types and values */
تعداد {
    /** No numeric value. */
    UPROPS_NTV_NONE=0,
    /** Decimal digits: nv=0..9 */
    UPROPS_NTV_DECIMAL_شروعات=1,
    /** Other digits: nv=0..9 */
    UPROPS_NTV_DIGIT_شروعات=11,
    /** Small integers: nv=0..154 */
    UPROPS_NTV_NUMERIC_شروعات=21,
    /** Fractions: ((ntv>>4)-12) / ((ntv&0xf)+1) = -1..17 / 1..16 */
    UPROPS_NTV_FRACTION_شروعات=0xb0,
    /**
     * Large integers:
     * ((ntv>>5)-14) * 10^((ntv&0x1f)+2) = (1..9)*(10^2..10^33)
     * (only one significant decimal digit)
     */
    UPROPS_NTV_LARGE_شروعات=0x1e0,
    /**
     * Sexagesimal numbers:
     * ((ntv>>2)-0xbf) * 60^((ntv&3)+1) = (1..9)*(60^1..60^4)
     */
    UPROPS_NTV_BASE60_شروعات=0x300,
    /**
     * Fraction-20 values:
     * frac20 = ntv-0x324 = 0..0x17 -> 1|3|5|7 / 20|40|80|160|320|640
     * numerator: num = 2*(frac20&3)+1
     * denominator: den = 20<<(frac20>>2)
     */
    UPROPS_NTV_FRACTION20_شروعات=UPROPS_NTV_BASE60_شروعات+36,  // 0x300+9*4=0x324
    /**
     * Fraction-32 values:
     * frac32 = ntv-0x34c = 0..15 -> 1|3|5|7 / 32|64|128|256
     * numerator: num = 2*(frac32&3)+1
     * denominator: den = 32<<(frac32>>2)
     */
    UPROPS_NTV_FRACTION32_شروعات=UPROPS_NTV_FRACTION20_شروعات+24,  // 0x324+6*4=0x34c
    /** No numeric value (yet). */
    UPROPS_NTV_RESERVED_شروعات=UPROPS_NTV_FRACTION32_شروعات+16,  // 0x34c+4*4=0x35c

    UPROPS_NTV_MAX_SMALL_INT=UPROPS_NTV_FRACTION_شروعات-UPROPS_NTV_NUMERIC_شروعات-1
};

#وصف_ڪريو UPROPS_NTV_GET_TYPE(ntv) \
    ((ntv==UPROPS_NTV_NONE) ? U_NT_NONE : \
    (ntv<UPROPS_NTV_DIGIT_شروعات) ?  U_NT_DECIMAL : \
    (ntv<UPROPS_NTV_NUMERIC_شروعات) ? U_NT_DIGIT : \
    U_NT_NUMERIC)

/* number of properties vector words */
#وصف_ڪريو UPROPS_ويڪٽر_لفظ     3

#جيڪڏهن_وصف_ٿيل __cplusplus

namespace {

// Properties in vector word 0
//بٽ
// 31..26   Age major version (major=0..63)
// 25..24   Age minor version (minor=0..3)
// 23..17   reserved
// 16..15   Indic Conjunct Break
// 14..12   East Asian Width
// 11..10   3..1: Bits 9..0 = Script_Extensions index
//             3: Script value from Script_Extensions
//             2: Script=Inherited
//             1: Script=Common
//             0: Script=bits 9..0
//  9.. 0   UScriptCode, or index to Script_Extensions

// *Note*: If we need more than the available bits for new properties,
// then we could move the Age property out of the properties vectors.
// For example, we could store the Age property in its own trie.
// In a small, 8-bit-value-width CodePointTrie, it would be larger than
// the amount of data that we would save in the properties vectors and their trie,
// but the size increase would be a small percentage of the total uprops.icu size.
// It would certainly be a much smaller increase than widening the properties vectors.
// The savings in the properties vectors+trie from pulling out the Age property
// are partly from mediocre correlation between Age and other property values.
// (Adding new characters to existing scripts tends to split property vectors where
// new characters are similar to old ones.)
// See https://github.com/unicode-org/icu/pull/3025 for details.

مضمن constexpr uint32_t UPROPS_AGE_نقاب = 0xff000000;
مضمن constexpr int32_t UPROPS_AGE_سرڪايو = 24;

مضمن constexpr uint8_t UPROPS_AGE_MAJOR_MAX = 63;
مضمن constexpr uint8_t UPROPS_AGE_MINOR_MAX = 3;

مضمن constexpr uint32_t UPROPS_EA_نقاب = 0x00007000;
مضمن constexpr int32_t UPROPS_EA_سرڪايو = 12;

مضمن constexpr uint32_t UPROPS_INCB_نقاب = 0x00018000;
مضمن constexpr int32_t UPROPS_INCB_سرڪايو = 15;

/** Script_Extensions: mask includes Script */
مضمن constexpr uint32_t UPROPS_SCRIPT_X_نقاب = 0x00000fff;

// UPROPS_SCRIPT_X_WITH_COMMON must be the lowest value that involves Script_Extensions.
مضمن constexpr uint32_t UPROPS_SCRIPT_X_WITH_OTHER = 0xc00;
مضمن constexpr uint32_t UPROPS_SCRIPT_X_WITH_INHERITED = 0x800;
مضمن constexpr uint32_t UPROPS_SCRIPT_X_WITH_COMMON = 0x400;
مضمن constexpr int32_t UPROPS_MAX_SCRIPT = 0x3ff;

/*
 * Properties in vector word 1
 * Each bit encodes one binary property.
 * The following constants represent the bit number, use 1<<UPROPS_XYZ.
 * UPROPS_BINARY_1_TOP<=32!
 *
 * Keep this list of property enums in sync with
 * propListNames[] in icu/source/tools/genprops/props2.c!
 *
 * ICU 2.6/uprops format version 3.2 stores full properties instead of "Other_".
 */
تعداد {
    UPROPS_WHITE_SPACE,
    UPROPS_DASH,
    UPROPS_HYPHEN,
    UPROPS_QUOTATION_MARK,
    UPROPS_TERMINAL_PUNCTUATION,
    UPROPS_MATH,
    UPROPS_HEX_DIGIT,
    UPROPS_ASCII_HEX_DIGIT,
    UPROPS_ALPHABETIC,
    UPROPS_IDEOGRAPHIC,
    UPROPS_DIACRITIC,
    UPROPS_EXTENDER,
    UPROPS_NONCHARACTER_ڪوڊ_نقطو,
    UPROPS_GRAPHEME_EXTEND,
    UPROPS_GRAPHEME_LINK,
    UPROPS_IDS_ٻٽو_OPERATOR,
    UPROPS_IDS_TRINARY_OPERATOR,
    UPROPS_RADICAL,
    UPROPS_UNIFIED_IDEOGRAPH,
    UPROPS_DEFAULT_IGNORABLE_ڪوڊ_نقطو,
    UPROPS_DEPRECATED,
    UPROPS_LOGICAL_ترتيب_EXCEPTION,
    UPROPS_XID_شروعات,
    UPROPS_XID_CONTINUE,
    UPROPS_ID_شروعات,                            /* ICU 2.6, uprops format version 3.2 */
    UPROPS_ID_CONTINUE,
    UPROPS_GRAPHEME_بنياد,
    UPROPS_S_رڪن,                              /* new in ICU 3.0 and Unicode 4.0.1 */
    UPROPS_VARIATION_SELECTOR,
    UPROPS_نمونو_SYNTAX,                      /* new in ICU 3.4 and Unicode 4.1 */
    UPROPS_نمونو_WHITE_SPACE,
    UPROPS_PREPENDED_CONCATENATION_MARK,        // new in ICU 60 and Unicode 10
    UPROPS_ٻٽو_1_چوٽي                         /* ==32 - full! */
};

/*
 * Properties in vector word 2
 * Bits
 * 31..26   ICU 75: Identifier_Type bit set
 *          ICU 70..74: unused
 *          ICU 57..69: emoji properties; moved to uemoji.icu in ICU 70
 * 25..20   Line Break
 * 19..15   Sentence Break
 * 14..10   Word Break
 *  9.. 5   Grapheme Cluster Break
 *  4.. 0   Decomposition Type
 */

// https://www.unicode.org/reports/tr39/#Identifier_Status_and_Type
// The Identifier_Type maps each code point to a *set* of one or more values.
// Some can be combined with others, some can only occur alone.
// Exclusion & Limited_Use are combinable bits, but cannot occur together.
// We use this forbidden combination for enumerated values.
// We use 6 bits for all possible combinations.
// If more combinable values are added, then we need to use more bits.
//
// We do not store separate data for Identifier_Status:
// We can derive that from the encoded Identifier_Type via a simple range check.

مضمن constexpr uint32_t UPROPS_2_ID_TYPE_نقاب = 0xfc000000;
مضمن constexpr int32_t UPROPS_2_ID_TYPE_سرڪايو = 26;

تعداد {
    // A high bit for use in idTypeToEncoded[] but not used in the data
    UPROPS_ID_TYPE_ٻٽو_انگ = 0x80,

    // Combinable bits
    UPROPS_ID_TYPE_EXCLUSION = 0x20,
    UPROPS_ID_TYPE_LIMITED_USE = 0x10,
    UPROPS_ID_TYPE_UNCOMMON_USE = 8,
    UPROPS_ID_TYPE_TECHNICAL = 4,
    UPROPS_ID_TYPE_OBSOLETE = 2,
    UPROPS_ID_TYPE_NOT_XID = 1,

    // Exclusive values
    UPROPS_ID_TYPE_NOT_CHARACTER = 0,

    // Forbidden bit combination used for enumerating other exclusive values
    UPROPS_ID_TYPE_FORBIDDEN = UPROPS_ID_TYPE_EXCLUSION | UPROPS_ID_TYPE_LIMITED_USE, // 0x30
    UPROPS_ID_TYPE_DEPRECATED = UPROPS_ID_TYPE_FORBIDDEN, // 0x30
    UPROPS_ID_TYPE_DEFAULT_IGNORABLE, // 0x31
    UPROPS_ID_TYPE_NOT_NFKC, // 0x32

    UPROPS_ID_TYPE_ALLOWED_MIN = UPROPS_ID_TYPE_FORBIDDEN + 0xc, // 0x3c
    UPROPS_ID_TYPE_INCLUSION = UPROPS_ID_TYPE_FORBIDDEN + 0xe, // 0x3e
    UPROPS_ID_TYPE_RECOMMENDED = UPROPS_ID_TYPE_FORBIDDEN + 0xf, // 0x3f
};

/**
 * Maps UIdentifierType to encoded bits.
 * When UPROPS_ID_TYPE_BIT is set, then use "&" to test whether the value bit is set.
 * When UPROPS_ID_TYPE_BIT is not set, then compare ("==") the array value with the data value.
 */
مضمن constexpr uint8_t uprops_id_Type_منزل_Encoded[] = {
    UPROPS_ID_TYPE_NOT_CHARACTER,
    UPROPS_ID_TYPE_DEPRECATED,
    UPROPS_ID_TYPE_DEFAULT_IGNORABLE,
    UPROPS_ID_TYPE_NOT_NFKC,
    UPROPS_ID_TYPE_ٻٽو_انگ | UPROPS_ID_TYPE_NOT_XID,
    UPROPS_ID_TYPE_ٻٽو_انگ | UPROPS_ID_TYPE_EXCLUSION,
    UPROPS_ID_TYPE_ٻٽو_انگ | UPROPS_ID_TYPE_OBSOLETE,
    UPROPS_ID_TYPE_ٻٽو_انگ | UPROPS_ID_TYPE_TECHNICAL,
    UPROPS_ID_TYPE_ٻٽو_انگ | UPROPS_ID_TYPE_UNCOMMON_USE,
    UPROPS_ID_TYPE_ٻٽو_انگ | UPROPS_ID_TYPE_LIMITED_USE,
    UPROPS_ID_TYPE_INCLUSION,
    UPROPS_ID_TYPE_RECOMMENDED
};

}  // namespace

#جيڪڏهن_پورو  // __cplusplus

#وصف_ڪريو UPROPS_LB_نقاب          0x03f00000
#وصف_ڪريو UPROPS_LB_سرڪايو         20

#وصف_ڪريو UPROPS_SB_نقاب          0x000f8000
#وصف_ڪريو UPROPS_SB_سرڪايو         15

#وصف_ڪريو UPROPS_WB_نقاب          0x00007c00
#وصف_ڪريو UPROPS_WB_سرڪايو         10

#وصف_ڪريو UPROPS_GCB_نقاب         0x000003e0
#وصف_ڪريو UPROPS_GCB_سرڪايو        5

#وصف_ڪريو UPROPS_DT_نقاب          0x0000001f

#جيڪڏهن_وصف_ٿيل __cplusplus

namespace {

// Bits 9..0 in UPROPS_MAX_VALUES_OTHER_INDEX
مضمن constexpr uint32_t UPROPS_MAX_بلاڪ = 0x3ff;

}  // namespace

#جيڪڏهن_پورو  // __cplusplus

/**
 * Gets the main properties value for a code point.
 * Implemented in uchar.c for uprops.cpp.
 */
U_CFUNC uint32_t
u_getMainProperties(UChar32 c);

/**
 * Get a properties vector word for a code point.
 * Implemented in uchar.c for uprops.cpp.
 * @return 0 if no data or illegal argument
 */
U_CFUNC uint32_t
u_getUnicodeProperties(UChar32 c, int32_t ڪالم);

/**
 * Get the maximum values for some enum/int properties.
 * Use the same column numbers as for u_getUnicodeProperties().
 * The returned value will contain maximum values stored in the same bit fields
 * as where the enum values are stored in the u_getUnicodeProperties()
 * return values for the same columns.
 *
 * Valid columns are those for properties words that contain enumerated values.
 * (ICU 2.6: columns 0 and 2)
 * For other column numbers, this function will return 0.
 *
 * @internal
 */
U_CFUNC int32_t
uprv_get_Max_قدرون(int32_t ڪالم);

/**
 * Checks if c is alphabetic, or a decimal digit; implements UCHAR_POSIX_ALNUM.
 * @internal
 */
U_CFUNC UBool
u_isalnumPOSIX(UChar32 c);

/**
 * Checks if c is in
 * [^\p{space}\p{gc=Control}\p{gc=Surrogate}\p{gc=Unassigned}]
 * with space=\p{Whitespace} and Control=Cc.
 * Implements UCHAR_POSIX_GRAPH.
 * @internal
 */
U_CFUNC UBool
u_isgraphPOSIX(UChar32 c);

/**
 * Checks if c is in \p{graph}\p{blank} - \p{cntrl}.
 * Implements UCHAR_POSIX_PRINT.
 * @internal
 */
U_CFUNC UBool
u_isprintPOSIX(UChar32 c);

/** Some code points. @internal */
تعداد {
    TAB     =0x0009,
    LF      =0x000a,
    FF      =0x000c,
    CR      =0x000d,
    NBSP    =0x00a0,
    CGJ     =0x034f,
    FIGURESP=0x2007,
    HAIRSP  =0x200a,
    ZWNJ    =0x200c,
    ZWJ     =0x200d,
    RLM     =0x200f,
    NNBSP   =0x202f,
    ZWNBSP  =0xfeff
};

// TODO: Move these two functions into a different header file (new unames.h?) so that uprops.h
// need not be C-compatible any more.
/**
 * Get the maximum length of a (regular/1.0/extended) character name.
 * @return 0 if no character names available.
 */
U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_ڊيگهائي(فراغ);

/**
 * Fills set with characters that are used in Unicode character names.
 * Includes all characters that are used in regular/Unicode 1.0/extended names.
 * Just empties the set if no character names are available.
 * @param sa USetAdder to receive characters.
 */
U_CAPI فراغ U_EXPORT2
uprv_getCharNameCharacters(ثابت USetAdder *sa);

/**
 * Constants for which data and implementation files provide which properties.
 * Used by UnicodeSet for service-specific property enumeration.
 * @internal
 */
تعداد UProperty_ذريعو {
    /** No source, not a supported property. */
    UPROPS_SRC_NONE,
    /** From uchar.c/uprops.icu main trie */
    UPROPS_SRC_CHAR,
    /** From uchar.c/uprops.icu properties vectors trie */
    UPROPS_SRC_PROPSVEC,
    /** From unames.c/unames.icu */
    UPROPS_SRC_نالا,
    /** From ucase.c/ucase.icu */
    UPROPS_SRC_CASE,
    /** From ubidi_props.c/ubidi.icu */
    UPROPS_SRC_BIDI,
    /** From uchar.c/uprops.icu main trie as well as properties vectors trie */
    UPROPS_SRC_CHAR_AND_PROPSVEC,
    /** From ucase.c/ucase.icu as well as unorm.cpp/unorm.icu */
    UPROPS_SRC_CASE_AND_NORM,
    /** From normalizer2impl.cpp/nfc.nrm */
    UPROPS_SRC_NFC,
    /** From normalizer2impl.cpp/nfkc.nrm */
    UPROPS_SRC_NFKC,
    /** From normalizer2impl.cpp/nfkc_cf.nrm */
    UPROPS_SRC_NFKC_CF,
    /** From normalizer2impl.cpp/nfc.nrm canonical iterator data */
    UPROPS_SRC_NFC_CANON_ITER,
    // Text layout properties.
    UPROPS_SRC_INPC,
    UPROPS_SRC_INSC,
    UPROPS_SRC_VO,
    UPROPS_SRC_EMOJI,
    UPROPS_SRC_IDSU,
    UPROPS_SRC_ID_COMPAT_MATH,
    UPROPS_SRC_بلاڪ,
    UPROPS_SRC_MCM,
    /** One more than the highest UPropertySource (UPROPS_SRC_) constant. */
    UPROPS_SRC_ڳڻپ
};
تعريفنوع تعداد UProperty_ذريعو UProperty_ذريعو;

/**
 * @see UPropertySource
 * @internal
 */
U_CFUNC UProperty_ذريعو U_EXPORT2
uprops_get_ذريعو(UProperty which);

/**
 * Enumerate uprops.icu's main data trie and add the
 * start of each range of same properties to the set.
 * @internal
 */
U_CFUNC فراغ U_EXPORT2
uchar_جوڙيو_Property_Starts(ثابت USetAdder *sa, UError_ڪوڊ *p_Error_ڪوڊ);

/**
 * Enumerate uprops.icu's properties vectors trie and add the
 * start of each range of same properties to the set.
 * @internal
 */
U_CFUNC فراغ U_EXPORT2
upropsvec_جوڙيو_Property_Starts(ثابت USetAdder *sa, UError_ڪوڊ *p_Error_ڪوڊ);

U_CFUNC فراغ U_EXPORT2
uprops_جوڙيو_Property_Starts(UProperty_ذريعو src, ثابت USetAdder *sa, UError_ڪوڊ *p_Error_ڪوڊ);

#جيڪڏهن_وصف_ٿيل __cplusplus

U_CFUNC فراغ U_EXPORT2
ublock_جوڙيو_Property_Starts(ثابت USetAdder *sa, UError_ڪوڊ &error_ڪوڊ);

#جيڪڏهن_پورو  // __cplusplus

/**
 * Return a set of characters for property enumeration.
 * For each two consecutive characters (start, limit) in the set,
 * all of the properties for start..limit-1 are all the same.
 *
 * @param sa USetAdder to receive result. Existing contents are lost.
 * @internal
 */
/*U_CFUNC void U_EXPORT2
uprv_getInclusions(const USetAdder *sa, UErrorCode *pErrorCode);
*/

/** @internal for icuexportdata */
U_CAPI فراغ U_EXPORT2
uprv_جوڙيو_Script_Extensions_ڪوڊ_Points(ثابت USetAdder *sa, UError_ڪوڊ *p_Error_ڪوڊ);

// TODO: Move this into a different header file (udataswp.h? new unames.h?) so that uprops.h
// need not be C-compatible any more.
/**
 * Swap the ICU Unicode character names file. See uchar.c.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uchar_مٽا_سٽا_نالا(ثابت UDataSwapper *ds,
                ثابت فراغ *اندريون_معلومات, int32_t ڊيگهائي, فراغ *ٻاهريون_معلومات,
                UError_ڪوڊ *p_Error_ڪوڊ);

#جيڪڏهن_وصف_ٿيل __cplusplus

U_NAMESPACE_BEGIN

class Unicode_سيٽ;

class CharacterProperties {
عام:
    CharacterProperties() = delete;
    ساكن ثابت Unicode_سيٽ *getInclusionsForProperty(UProperty prop, UError_ڪوڊ &error_ڪوڊ);
    ساكن ثابت Unicode_سيٽ *get_ٻٽو_Property_سيٽ(UProperty property, UError_ڪوڊ &error_ڪوڊ);
};

// implemented in uniset_props.cpp
U_CFUNC Unicode_سيٽ *
uniset_getUnicode32Instance(UError_ڪوڊ &error_ڪوڊ);

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو

#جيڪڏهن_پورو
