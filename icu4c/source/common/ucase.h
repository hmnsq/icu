// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2004-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucase.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug30
*   created by: Markus W. Scherer
*
*   Low-level Unicode character/string case mapping code.
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UCASE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UCASE_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "utrie2.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
U_NAMESPACE_BEGIN

class UnicodeString;

U_NAMESPACE_ಕೊನೆ
#ಆದರೆ_ಅಂತ್ಯ

/* library API -------------------------------------------------------------- */

U_CFUNC ಶೂನ್ಯ U_EXPORT2
ucase_ಕೂಡಿಸು_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * Requires non-NULL locale ID but otherwise does the equivalent of
 * checking for language codes as if uloc_getLanguage() were called:
 * Accepts both 2- and 3-letter codes and accepts case variants.
 */
U_CFUNC int32_t
ucase_getCaseLocale(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale);

/* Casing locale types for ucase_getCaseLocale */
ಎಣಿಕೆ {
    UCASE_LOC_UNKNOWN,
    UCASE_LOC_ಬೇರು,
    UCASE_LOC_TURKISH,
    UCASE_LOC_LITHUANIAN,
    UCASE_LOC_GREEK,
    UCASE_LOC_DUTCH,
    UCASE_LOC_ARMENIAN
};

/**
 * Bit mask for getting just the options from a string compare options word
 * that are relevant for case-insensitive string comparison.
 * See stringoptions.h. Also include _STRNCMP_STYLE and U_COMPARE_CODE_POINT_ORDER.
 * @internal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು STRCASECMP_OPTIONS_ಮುಖವಾಡ 0xffff

/**
 * Bit mask for getting just the options from a string compare options word
 * that are relevant for case folding (of a single string or code point).
 *
 * Currently only bit 0 for U_FOLD_CASE_EXCLUDE_SPECIAL_I.
 * It is conceivable that at some point we might use one more bit for using uppercase sharp s.
 * It is conceivable that at some point we might want the option to use only simple case foldings
 * when operating on strings.
 *
 * See stringoptions.h.
 * @internal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು FOLD_CASE_OPTIONS_ಮುಖವಾಡ 7

/* single-code point functions */

U_CAPI UChar32 U_EXPORT2
ucase_tolower(UChar32 c);

U_CAPI UChar32 U_EXPORT2
ucase_toupper(UChar32 c);

U_CAPI UChar32 U_EXPORT2
ucase_totitle(UChar32 c);

U_CAPI UChar32 U_EXPORT2
ucase_fold(UChar32 c, uint32_t options);

/**
 * Adds all simple case mappings and the full case folding for c to sa,
 * and also adds special case closure mappings.
 * c itself is not added.
 * For example, the mappings
 * - for s include long s
 * - for sharp s include ss
 * - for k include the Kelvin sign
 */
U_CFUNC ಶೂನ್ಯ U_EXPORT2
ucase_ಕೂಡಿಸು_Case_ಆವರಣ(UChar32 c, ಸ್ಥಿರಾಂಕ USetAdder *sa);

/** Case closure with only scf=Simple_Case_Folding. */
U_CFUNC ಶೂನ್ಯ U_EXPORT2
ucase_ಕೂಡಿಸು_Simple_Case_ಆವರಣ(UChar32 c, ಸ್ಥಿರಾಂಕ USetAdder *sa);

/**
 * Maps the string to single code points and adds the associated case closure
 * mappings.
 * The string is mapped to code points if it is their full case folding string.
 * In other words, this performs a reverse full case folding and then
 * adds the case closure items of the resulting code points.
 * If the string is found and its closure applied, then
 * the string itself is added as well as part of its code points' closure.
 * It must be length>=0.
 *
 * @return true if the string was found
 */
U_CFUNC UBool U_EXPORT2
ucase_ಕೂಡಿಸು_String_Case_ಆವರಣ(ಸ್ಥಿರಾಂಕ UChar *s, int32_t ಉದ್ದಳತೆ, ಸ್ಥಿರಾಂಕ USetAdder *sa);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
U_NAMESPACE_BEGIN

/**
 * Iterator over characters with more than one code point in the full default Case_Folding.
 */
class U_COMMON_API ತುಂಬಿದ_Case_Folding_Iterator {
ಸಾರ್ವಜನಿಕ:
    /** Constructor. */
    ತುಂಬಿದ_Case_Folding_Iterator();
    /**
     * Returns the next (cp, full) pair where "full" is cp's full default Case_Folding.
     * Returns a negative cp value at the end of the iteration.
     */
    UChar32 ಮುಂದಿನ(UnicodeString &ತುಂಬಿದ);
ಖಾಸಗಿ:
    ತುಂಬಿದ_Case_Folding_Iterator(ಸ್ಥಿರಾಂಕ ತುಂಬಿದ_Case_Folding_Iterator &) = delete;  // no copy
    ತುಂಬಿದ_Case_Folding_Iterator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ತುಂಬಿದ_Case_Folding_Iterator &) = delete;  // no assignment

    ಸ್ಥಿರಾಂಕ char16_t *unfold;
    int32_t unfold_ಅಡ್ಡಸಾಲುಗಳು;
    int32_t unfold_ಅಡ್ಡಸಾಲು_ಅಗಲ;
    int32_t unfold_String_ಅಗಲ;
    int32_t current_ಅಡ್ಡಸಾಲು;
    int32_t ಅಡ್ಡಸಾಲು_Cp_ಸೂಚ್ಯಂಕ;
};

/**
 * Fast case mapping data for ASCII/Latin.
 * Linear arrays of delta bytes: 0=no mapping; EXC=exception.
 * Deltas must not cross the ASCII boundary, or else they cannot be easily used
 * in simple UTF-8 code.
 */
namespace LatinCase {

/** Case mapping/folding data for code points up to U+017F. */
constexpr char16_t ಎಲ್ಲೆ_2 = 0x180;
/** U+017F case-folds and uppercases crossing the ASCII boundary. */
constexpr char16_t LONG_S = 0x17f;
/** Exception: Complex mapping, or too-large delta. */
constexpr int8_t EXC = -0x80;

/** Deltas for lowercasing for most locales, and default case folding. */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ int8_t ಗಮ್ಯ_ಅಧೋ_ಸಾಮಾನ್ಯ[ಎಲ್ಲೆ_2];
/** Deltas for lowercasing for tr/az/lt, and Turkic case folding. */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ int8_t ಗಮ್ಯ_ಅಧೋ_TR_LT[ಎಲ್ಲೆ_2];

/** Deltas for uppercasing for most locales. */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ int8_t ಗಮ್ಯ_ಊರ್ಧ್ವ_ಸಾಮಾನ್ಯ[ಎಲ್ಲೆ_2];
/** Deltas for uppercasing for tr/az. */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ int8_t ಗಮ್ಯ_ಊರ್ಧ್ವ_TR[ಎಲ್ಲೆ_2];

}  // namespace LatinCase

U_NAMESPACE_ಕೊನೆ
#ಆದರೆ_ಅಂತ್ಯ

/** @return UCASE_NONE, UCASE_LOWER, UCASE_UPPER, UCASE_TITLE */
U_CAPI int32_t U_EXPORT2
ucase_getType(UChar32 c);

/** @return like ucase_getType() but also sets UCASE_IGNORABLE if c is case-ignorable */
U_CAPI int32_t U_EXPORT2
ucase_getTypeOrIgnorable(UChar32 c);

U_CAPI UBool U_EXPORT2
ucase_isSoftDotted(UChar32 c);

U_CAPI UBool U_EXPORT2
ucase_isCaseSensitive(UChar32 c);

/* string case mapping functions */

U_CDECL_BEGIN

/**
 * Iterator function for string case mappings, which need to look at the
 * context (surrounding text) of a given character for conditional mappings.
 *
 * The iterator only needs to go backward or forward away from the
 * character in question. It does not use any indexes on this interface.
 * It does not support random access or an arbitrary change of
 * iteration direction.
 *
 * The code point being case-mapped itself is never returned by
 * this iterator.
 *
 * @param context A pointer to the iterator's working data.
 * @param dir If <0 then start iterating backward from the character;
 *            if >0 then start iterating forward from the character;
 *            if 0 then continue iterating in the current direction.
 * @return Next code point, or <0 when the iteration is done.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ UChar32 U_CALLCONV
UCaseContextIterator(ಶೂನ್ಯ *context, int8_t dir);

/**
 * Sample struct which may be used by some implementations of
 * UCaseContextIterator.
 */
ರಚನೆ UCaseContext {
    ಶೂನ್ಯ *p;
    int32_t ಆರಂಭ, ಸೂಚ್ಯಂಕ, ಎಲ್ಲೆ;
    int32_t cp_ಆರಂಭ, cp_ಎಲ್ಲೆ;
    int8_t dir;
    int8_t b1, b2, b3;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UCaseContext UCaseContext;

U_CDECL_ಕೊನೆ

#ವ್ಯಾಖ್ಯಾನಿಸು UCASECONTEXT_INITIALIZER { ಖಾಲಿ,  0, 0, 0,  0, 0,  0,  0, 0, 0 }

ಎಣಿಕೆ {
    /**
     * For string case mappings, a single character (a code point) is mapped
     * either to itself (in which case in-place mapping functions do nothing),
     * or to another single code point, or to a string.
     * Aside from the string contents, these are indicated with a single int32_t
     * value as follows:
     *
     * Mapping to self: Negative values (~self instead of -self to support U+0000)
     *
     * Mapping to another code point: Positive values >UCASE_MAX_STRING_LENGTH
     *
     * Mapping to a string: The string length (0..UCASE_MAX_STRING_LENGTH) is
     * returned. Note that the string result may indeed have zero length.
     */
    UCASE_MAX_STRING_ಉದ್ದಳತೆ=0x1f
};

/**
 * Get the full lowercase mapping for c.
 *
 * @param csp Case mapping properties.
 * @param c Character to be mapped.
 * @param iter Character iterator, used for context-sensitive mappings.
 *             See UCaseContextIterator for details.
 *             If iter==NULL then a context-independent result is returned.
 * @param context Pointer to be passed into iter.
 * @param pString If the mapping result is a string, then the pointer is
 *                written to *pString.
 * @param caseLocale Case locale value from ucase_getCaseLocale().
 * @return Output code point or string length, see UCASE_MAX_STRING_LENGTH.
 *
 * @see UCaseContextIterator
 * @see UCASE_MAX_STRING_LENGTH
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಅಧೋ(UChar32 c,
                  UCaseContextIterator *iter, ಶೂನ್ಯ *context,
                  ಸ್ಥಿರಾಂಕ UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಊರ್ಧ್ವ(UChar32 c,
                  UCaseContextIterator *iter, ಶೂನ್ಯ *context,
                  ಸ್ಥಿರಾಂಕ UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_ಗಮ್ಯ_ತುಂಬಿದ_Title(UChar32 c,
                  UCaseContextIterator *iter, ಶೂನ್ಯ *context,
                  ಸ್ಥಿರಾಂಕ UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(UChar32 c,
                    ಸ್ಥಿರಾಂಕ UChar **pString,
                    uint32_t options);

U_CFUNC int32_t U_EXPORT2
ucase_has_ದ್ವಿಮಾನ_Property(UChar32 c, UProperty which);


U_CDECL_BEGIN

/**
 * @internal
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ int32_t U_CALLCONV
UCase_Map_ತುಂಬಿದ(UChar32 c,
             UCaseContextIterator *iter, ಶೂನ್ಯ *context,
             ಸ್ಥಿರಾಂಕ UChar **pString,
             int32_t caseLocale);

U_CDECL_ಕೊನೆ

/* for icuexportdata -------------------------------------------------------- */

ರಚನೆ UCaseProps {
    ಶೂನ್ಯ *mem;  // TODO: was unused, and type UDataMemory -- remove
    ಸ್ಥಿರಾಂಕ int32_t *indexes;
    ಸ್ಥಿರಾಂಕ uint16_t *exceptions;
    ಸ್ಥಿರಾಂಕ uint16_t *unfold;

    UTrie2 trie;
    uint8_t format_ಆವೃತ್ತಿ[4];
};

U_CAPI ಸ್ಥಿರಾಂಕ ರಚನೆ UCaseProps * U_EXPORT2
ucase_getSingleton(int32_t *p_Exceptions_ಉದ್ದಳತೆ, int32_t *p_Unfold_ಉದ್ದಳತೆ);

/* file definitions --------------------------------------------------------- */

#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_ದತ್ತಾಂಶ_NAME "ucase"
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_ದತ್ತಾಂಶ_TYPE "icu"

/* format "cAsE" */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_FMT_0 0x63
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_FMT_1 0x41
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_FMT_2 0x53
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_FMT_3 0x45

/* indexes into indexes[] */
ಎಣಿಕೆ {
    UCASE_IX_ಸೂಚ್ಯಂಕ_ತುದಿ,
    UCASE_IX_ಉದ್ದಳತೆ,
    UCASE_IX_TRIE_ಗಾತ್ರ,
    UCASE_IX_EXC_ಉದ್ದಳತೆ,
    UCASE_IX_UNFOLD_ಉದ್ದಳತೆ,

    UCASE_IX_MAX_ತುಂಬಿದ_ಉದ್ದಳತೆ=15,
    UCASE_IX_ತುದಿ=16
};

/* definitions for 16-bit case properties word ------------------------------ */

U_CFUNC ಸ್ಥಿರಾಂಕ UTrie2 * U_EXPORT2
ucase_getTrie(ಶೂನ್ಯ);

/* 2-bit constants for types of cased characters */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_TYPE_ಮುಖವಾಡ     3
ಎಣಿಕೆ {
    UCASE_NONE,
    UCASE_ಅಧೋ,
    UCASE_ಊರ್ಧ್ವ,
    UCASE_TITLE
};

#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_GET_TYPE(props) ((props)&UCASE_TYPE_ಮುಖವಾಡ)
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_GET_TYPE_AND_IGNORABLE(props) ((props)&7)

#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_IS_ಊರ್ಧ್ವ_OR_TITLE(props) ((props)&2)

#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_IGNORABLE         4
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_EXCEPTION         8
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_SENSITIVE         0x10

#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_HAS_EXCEPTION(props) ((props)&UCASE_EXCEPTION)

#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_DOT_ಮುಖವಾಡ      0x60
ಎಣಿಕೆ {
    UCASE_NO_DOT=0,         /* normal characters with cc=0 */
    UCASE_SOFT_DOTTED=0x20, /* soft-dotted characters with cc=0 */
    UCASE_ABOVE=0x40,       /* "above" accents with cc=230 */
    UCASE_OTHER_ACCENT=0x60 /* other accent character (0<cc!=230) */
};

/* no exception: bits 15..7 are a 9-bit signed case mapping delta */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_DELTA_ಸರಿಸು   7
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_DELTA_ಮುಖವಾಡ    0xff80
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_MAX_DELTA     0xff
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_MIN_DELTA     (-UCASE_MAX_DELTA-1)

#ಆದರೆ U_SIGNED_ಬಲ_ಸರಿಸು_IS_ARITHMETIC
#   define UCASE_GET_DELTA(props) ((int16_t)(props)>>UCASE_DELTA_ಸರಿಸು)
#ಇಲ್ಲದಿದ್ದರೆ
#   define UCASE_GET_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UCASE_DELTA_ಸರಿಸು)|0xfe00) : ((uint16_t)(props)>>UCASE_DELTA_ಸರಿಸು))
#ಆದರೆ_ಅಂತ್ಯ

/* exception: bits 15..4 are an unsigned 12-bit index into the exceptions array */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_EXC_ಸರಿಸು     4
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_EXC_ಮುಖವಾಡ      0xfff0
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_MAX_EXCEPTIONS ((UCASE_EXC_ಮುಖವಾಡ>>UCASE_EXC_ಸರಿಸು)+1)

/* definitions for 16-bit main exceptions word ------------------------------ */

/* first 8 bits indicate values in optional slots */
ಎಣಿಕೆ {
    UCASE_EXC_ಅಧೋ,
    UCASE_EXC_FOLD,
    UCASE_EXC_ಊರ್ಧ್ವ,
    UCASE_EXC_TITLE,
    UCASE_EXC_DELTA,
    UCASE_EXC_5,            /* reserved */
    UCASE_EXC_ಆವರಣ,
    UCASE_EXC_ತುಂಬಿದ_MAPPINGS,
    UCASE_EXC_ALL_SLOTS     /* one past the last slot */
};

/* each slot is 2 uint16_t instead of 1 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_EXC_DOUBLE_SLOTS      0x100

ಎಣಿಕೆ {
    UCASE_EXC_NO_SIMPLE_CASE_FOLDING=0x200,
    UCASE_EXC_DELTA_IS_NEGATIVE=0x400,
    UCASE_EXC_SENSITIVE=0x800
};

/* UCASE_EXC_DOT_MASK=UCASE_DOT_MASK<<UCASE_EXC_DOT_SHIFT */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_EXC_DOT_ಸರಿಸು     7

/* normally stored in the main word, but pushed out for larger exception indexes */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_EXC_DOT_ಮುಖವಾಡ      0x3000
ಎಣಿಕೆ {
    UCASE_EXC_NO_DOT=0,
    UCASE_EXC_SOFT_DOTTED=0x1000,
    UCASE_EXC_ABOVE=0x2000,         /* "above" accents with cc=230 */
    UCASE_EXC_OTHER_ACCENT=0x3000   /* other character (0<cc!=230) */
};

/* complex/conditional mappings */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_EXC_CONDITIONAL_SPECIAL   0x4000
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_EXC_CONDITIONAL_FOLD      0x8000

/* definitions for lengths word for full case mappings */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_ತುಂಬಿದ_ಅಧೋ    0xf
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_ತುಂಬಿದ_FOLDING  0xf0
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_ತುಂಬಿದ_ಊರ್ಧ್ವ    0xf00
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_ತುಂಬಿದ_TITLE    0xf000

/* maximum lengths */
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_ತುಂಬಿದ_MAPPINGS_MAX_ಉದ್ದಳತೆ (4*0xf)
#ವ್ಯಾಖ್ಯಾನಿಸು UCASE_ಆವರಣ_MAX_ಉದ್ದಳತೆ 0xf

/* constants for reverse case folding ("unfold") data */
ಎಣಿಕೆ {
    UCASE_UNFOLD_ಅಡ್ಡಸಾಲುಗಳು,
    UCASE_UNFOLD_ಅಡ್ಡಸಾಲು_ಅಗಲ,
    UCASE_UNFOLD_STRING_ಅಗಲ
};

#ಆದರೆ_ಅಂತ್ಯ
