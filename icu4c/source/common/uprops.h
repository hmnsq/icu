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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UPROPS_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UPROPS_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"

/* indexes[] entries */
ಎಣಿಕೆ {
    UPROPS_PROPS32_ಸೂಚ್ಯಂಕ,
    UPROPS_EXCEPTIONS_ಸೂಚ್ಯಂಕ,
    UPROPS_EXCEPTIONS_ತುದಿ_ಸೂಚ್ಯಂಕ,

    UPROPS_ADDITIONAL_TRIE_ಸೂಚ್ಯಂಕ,
    UPROPS_ADDITIONAL_VECTORS_ಸೂಚ್ಯಂಕ,
    UPROPS_ADDITIONAL_VECTORS_COLUMNS_ಸೂಚ್ಯಂಕ,

    UPROPS_SCRIPT_EXTENSIONS_ಸೂಚ್ಯಂಕ,

    UPROPS_ಖಂಡ_TRIE_ಸೂಚ್ಯಂಕ,
    UPROPS_RESERVED_ಸೂಚ್ಯಂಕ_8,

    /** size of the data file (number of 32-bit units after the header) */
    UPROPS_ದತ್ತಾಂಶ_ತುದಿ_ಸೂಚ್ಯಂಕ,

    /** maximum values for code values in vector word 0 */
    UPROPS_MAX_ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ=10,
    /** maximum values for code values in vector word 2 */
    UPROPS_MAX_ಮೌಲ್ಯಗಳು_2_ಸೂಚ್ಯಂಕ,
    /** maximum values for other code values */
    UPROPS_MAX_ಮೌಲ್ಯಗಳು_OTHER_ಸೂಚ್ಯಂಕ,

    UPROPS_ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ=16
};

/* definitions for the main properties words */
ಎಣಿಕೆ {
    /* general category shift==0                                0 (5 bits) */
    /* reserved                                                 5 (1 bit) */
    UPROPS_NUMERIC_TYPE_ಮೌಲ್ಯ_ಸರಿಸು=6                       /*  6 (10 bits) */
};

#ವ್ಯಾಖ್ಯಾನಿಸು GET_CATEGORY(props) ((props)&0x1f)
#ವ್ಯಾಖ್ಯಾನಿಸು CAT_ಮುಖವಾಡ(props) U_ಮುಖವಾಡ(GET_CATEGORY(props))

#ವ್ಯಾಖ್ಯಾನಿಸು GET_NUMERIC_TYPE_ಮೌಲ್ಯ(props) ((props)>>UPROPS_NUMERIC_TYPE_ಮೌಲ್ಯ_ಸರಿಸು)

/* constants for the storage form of numeric types and values */
ಎಣಿಕೆ {
    /** No numeric value. */
    UPROPS_NTV_NONE=0,
    /** Decimal digits: nv=0..9 */
    UPROPS_NTV_DECIMAL_ಆರಂಭ=1,
    /** Other digits: nv=0..9 */
    UPROPS_NTV_DIGIT_ಆರಂಭ=11,
    /** Small integers: nv=0..154 */
    UPROPS_NTV_NUMERIC_ಆರಂಭ=21,
    /** Fractions: ((ntv>>4)-12) / ((ntv&0xf)+1) = -1..17 / 1..16 */
    UPROPS_NTV_FRACTION_ಆರಂಭ=0xb0,
    /**
     * Large integers:
     * ((ntv>>5)-14) * 10^((ntv&0x1f)+2) = (1..9)*(10^2..10^33)
     * (only one significant decimal digit)
     */
    UPROPS_NTV_LARGE_ಆರಂಭ=0x1e0,
    /**
     * Sexagesimal numbers:
     * ((ntv>>2)-0xbf) * 60^((ntv&3)+1) = (1..9)*(60^1..60^4)
     */
    UPROPS_NTV_BASE60_ಆರಂಭ=0x300,
    /**
     * Fraction-20 values:
     * frac20 = ntv-0x324 = 0..0x17 -> 1|3|5|7 / 20|40|80|160|320|640
     * numerator: num = 2*(frac20&3)+1
     * denominator: den = 20<<(frac20>>2)
     */
    UPROPS_NTV_FRACTION20_ಆರಂಭ=UPROPS_NTV_BASE60_ಆರಂಭ+36,  // 0x300+9*4=0x324
    /**
     * Fraction-32 values:
     * frac32 = ntv-0x34c = 0..15 -> 1|3|5|7 / 32|64|128|256
     * numerator: num = 2*(frac32&3)+1
     * denominator: den = 32<<(frac32>>2)
     */
    UPROPS_NTV_FRACTION32_ಆರಂಭ=UPROPS_NTV_FRACTION20_ಆರಂಭ+24,  // 0x324+6*4=0x34c
    /** No numeric value (yet). */
    UPROPS_NTV_RESERVED_ಆರಂಭ=UPROPS_NTV_FRACTION32_ಆರಂಭ+16,  // 0x34c+4*4=0x35c

    UPROPS_NTV_MAX_SMALL_INT=UPROPS_NTV_FRACTION_ಆರಂಭ-UPROPS_NTV_NUMERIC_ಆರಂಭ-1
};

#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_NTV_GET_TYPE(ntv) \
    ((ntv==UPROPS_NTV_NONE) ? U_NT_NONE : \
    (ntv<UPROPS_NTV_DIGIT_ಆರಂಭ) ?  U_NT_DECIMAL : \
    (ntv<UPROPS_NTV_NUMERIC_ಆರಂಭ) ? U_NT_DIGIT : \
    U_NT_NUMERIC)

/* number of properties vector words */
#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_ಸದಿಶ_ಪದಗಳು     3

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

namespace {

// Properties in vector word 0
//ಬಿಟ್ಟುಗಳು
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

ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_AGE_ಮುಖವಾಡ = 0xff000000;
ಸಾಲೊಳಗೆ constexpr int32_t UPROPS_AGE_ಸರಿಸು = 24;

ಸಾಲೊಳಗೆ constexpr uint8_t UPROPS_AGE_MAJOR_MAX = 63;
ಸಾಲೊಳಗೆ constexpr uint8_t UPROPS_AGE_MINOR_MAX = 3;

ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_EA_ಮುಖವಾಡ = 0x00007000;
ಸಾಲೊಳಗೆ constexpr int32_t UPROPS_EA_ಸರಿಸು = 12;

ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_INCB_ಮುಖವಾಡ = 0x00018000;
ಸಾಲೊಳಗೆ constexpr int32_t UPROPS_INCB_ಸರಿಸು = 15;

/** Script_Extensions: mask includes Script */
ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_SCRIPT_X_ಮುಖವಾಡ = 0x00000fff;

// UPROPS_SCRIPT_X_WITH_COMMON must be the lowest value that involves Script_Extensions.
ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_SCRIPT_X_WITH_OTHER = 0xc00;
ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_SCRIPT_X_WITH_INHERITED = 0x800;
ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_SCRIPT_X_WITH_COMMON = 0x400;
ಸಾಲೊಳಗೆ constexpr int32_t UPROPS_MAX_SCRIPT = 0x3ff;

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
ಎಣಿಕೆ {
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
    UPROPS_NONCHARACTER_ಸಂಕೇತ_ಬಿಂದು,
    UPROPS_GRAPHEME_EXTEND,
    UPROPS_GRAPHEME_LINK,
    UPROPS_IDS_ದ್ವಿಮಾನ_OPERATOR,
    UPROPS_IDS_TRINARY_OPERATOR,
    UPROPS_RADICAL,
    UPROPS_UNIFIED_IDEOGRAPH,
    UPROPS_DEFAULT_IGNORABLE_ಸಂಕೇತ_ಬಿಂದು,
    UPROPS_DEPRECATED,
    UPROPS_LOGICAL_ಕ್ರಮ_EXCEPTION,
    UPROPS_XID_ಆರಂಭ,
    UPROPS_XID_CONTINUE,
    UPROPS_ID_ಆರಂಭ,                            /* ICU 2.6, uprops format version 3.2 */
    UPROPS_ID_CONTINUE,
    UPROPS_GRAPHEME_ತಳ,
    UPROPS_S_ಪದ,                              /* new in ICU 3.0 and Unicode 4.0.1 */
    UPROPS_VARIATION_SELECTOR,
    UPROPS_ನಮೂನೆ_SYNTAX,                      /* new in ICU 3.4 and Unicode 4.1 */
    UPROPS_ನಮೂನೆ_WHITE_SPACE,
    UPROPS_PREPENDED_CONCATENATION_MARK,        // new in ICU 60 and Unicode 10
    UPROPS_ದ್ವಿಮಾನ_1_ತುದಿ                         /* ==32 - full! */
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

ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_2_ID_TYPE_ಮುಖವಾಡ = 0xfc000000;
ಸಾಲೊಳಗೆ constexpr int32_t UPROPS_2_ID_TYPE_ಸರಿಸು = 26;

ಎಣಿಕೆ {
    // A high bit for use in idTypeToEncoded[] but not used in the data
    UPROPS_ID_TYPE_ಬಿಟ್ = 0x80,

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
ಸಾಲೊಳಗೆ constexpr uint8_t uprops_id_Type_ಗಮ್ಯ_Encoded[] = {
    UPROPS_ID_TYPE_NOT_CHARACTER,
    UPROPS_ID_TYPE_DEPRECATED,
    UPROPS_ID_TYPE_DEFAULT_IGNORABLE,
    UPROPS_ID_TYPE_NOT_NFKC,
    UPROPS_ID_TYPE_ಬಿಟ್ | UPROPS_ID_TYPE_NOT_XID,
    UPROPS_ID_TYPE_ಬಿಟ್ | UPROPS_ID_TYPE_EXCLUSION,
    UPROPS_ID_TYPE_ಬಿಟ್ | UPROPS_ID_TYPE_OBSOLETE,
    UPROPS_ID_TYPE_ಬಿಟ್ | UPROPS_ID_TYPE_TECHNICAL,
    UPROPS_ID_TYPE_ಬಿಟ್ | UPROPS_ID_TYPE_UNCOMMON_USE,
    UPROPS_ID_TYPE_ಬಿಟ್ | UPROPS_ID_TYPE_LIMITED_USE,
    UPROPS_ID_TYPE_INCLUSION,
    UPROPS_ID_TYPE_RECOMMENDED
};

}  // namespace

#ಆದರೆ_ಅಂತ್ಯ  // __cplusplus

#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_LB_ಮುಖವಾಡ          0x03f00000
#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_LB_ಸರಿಸು         20

#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_SB_ಮುಖವಾಡ          0x000f8000
#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_SB_ಸರಿಸು         15

#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_WB_ಮುಖವಾಡ          0x00007c00
#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_WB_ಸರಿಸು         10

#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_GCB_ಮುಖವಾಡ         0x000003e0
#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_GCB_ಸರಿಸು        5

#ವ್ಯಾಖ್ಯಾನಿಸು UPROPS_DT_ಮುಖವಾಡ          0x0000001f

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

namespace {

// Bits 9..0 in UPROPS_MAX_VALUES_OTHER_INDEX
ಸಾಲೊಳಗೆ constexpr uint32_t UPROPS_MAX_ಖಂಡ = 0x3ff;

}  // namespace

#ಆದರೆ_ಅಂತ್ಯ  // __cplusplus

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
u_getUnicodeProperties(UChar32 c, int32_t ಸ್ತಂಭ);

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
uprv_get_Max_ಮೌಲ್ಯಗಳು(int32_t ಸ್ತಂಭ);

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
ಎಣಿಕೆ {
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
uprv_get_Max_Char_Name_ಉದ್ದಳತೆ(ಶೂನ್ಯ);

/**
 * Fills set with characters that are used in Unicode character names.
 * Includes all characters that are used in regular/Unicode 1.0/extended names.
 * Just empties the set if no character names are available.
 * @param sa USetAdder to receive characters.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_getCharNameCharacters(ಸ್ಥಿರಾಂಕ USetAdder *sa);

/**
 * Constants for which data and implementation files provide which properties.
 * Used by UnicodeSet for service-specific property enumeration.
 * @internal
 */
ಎಣಿಕೆ UProperty_ಮೂಲ {
    /** No source, not a supported property. */
    UPROPS_SRC_NONE,
    /** From uchar.c/uprops.icu main trie */
    UPROPS_SRC_CHAR,
    /** From uchar.c/uprops.icu properties vectors trie */
    UPROPS_SRC_PROPSVEC,
    /** From unames.c/unames.icu */
    UPROPS_SRC_ಹೆಸರುಗಳು,
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
    UPROPS_SRC_ಖಂಡ,
    UPROPS_SRC_MCM,
    /** One more than the highest UPropertySource (UPROPS_SRC_) constant. */
    UPROPS_SRC_ಎಣಿಕೆ
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UProperty_ಮೂಲ UProperty_ಮೂಲ;

/**
 * @see UPropertySource
 * @internal
 */
U_CFUNC UProperty_ಮೂಲ U_EXPORT2
uprops_get_ಮೂಲ(UProperty which);

/**
 * Enumerate uprops.icu's main data trie and add the
 * start of each range of same properties to the set.
 * @internal
 */
U_CFUNC ಶೂನ್ಯ U_EXPORT2
uchar_ಕೂಡಿಸು_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * Enumerate uprops.icu's properties vectors trie and add the
 * start of each range of same properties to the set.
 * @internal
 */
U_CFUNC ಶೂನ್ಯ U_EXPORT2
upropsvec_ಕೂಡಿಸು_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CFUNC ಶೂನ್ಯ U_EXPORT2
uprops_ಕೂಡಿಸು_Property_Starts(UProperty_ಮೂಲ src, ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

U_CFUNC ಶೂನ್ಯ U_EXPORT2
ublock_ಕೂಡಿಸು_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

#ಆದರೆ_ಅಂತ್ಯ  // __cplusplus

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
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_ಕೂಡಿಸು_Script_Extensions_ಸಂಕೇತ_Points(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

// TODO: Move this into a different header file (udataswp.h? new unames.h?) so that uprops.h
// need not be C-compatible any more.
/**
 * Swap the ICU Unicode character names file. See uchar.c.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uchar_ಅದಲುಬದಲು_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t ಉದ್ದಳತೆ, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

U_NAMESPACE_BEGIN

class Unicode_ಗಣ;

class CharacterProperties {
ಸಾರ್ವಜನಿಕ:
    CharacterProperties() = delete;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *getInclusionsForProperty(UProperty prop, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *get_ದ್ವಿಮಾನ_Property_ಗಣ(UProperty property, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
};

// implemented in uniset_props.cpp
U_CFUNC Unicode_ಗಣ *
uniset_getUnicode32Instance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
