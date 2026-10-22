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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UCASE_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UCASE_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uset_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
U_NAMESPACE_BEGIN

class UnicodeString;

U_NAMESPACE_ਸਿਰਾ
#ਜੇ_ਅੰਤ

/* library API -------------------------------------------------------------- */

U_CFUNC ਖਾਲੀ U_EXPORT2
ucase_ਜੋੜੋ_Property_Starts(ਅਚਲ USetAdder *sa, UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Requires non-NULL locale ID but otherwise does the equivalent of
 * checking for language codes as if uloc_getLanguage() were called:
 * Accepts both 2- and 3-letter codes and accepts case variants.
 */
U_CFUNC int32_t
ucase_getCaseLocale(ਅਚਲ ਅੱਖਰ *locale);

/* Casing locale types for ucase_getCaseLocale */
ਗਿਣਤੀ {
    UCASE_LOC_UNKNOWN,
    UCASE_LOC_ਜੜ੍ਹ,
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRCASECMP_OPTIONS_ਨਕਾਬ 0xffff

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOLD_CASE_OPTIONS_ਨਕਾਬ 7

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
U_CFUNC ਖਾਲੀ U_EXPORT2
ucase_ਜੋੜੋ_Case_ਘੇਰਾ(UChar32 c, ਅਚਲ USetAdder *sa);

/** Case closure with only scf=Simple_Case_Folding. */
U_CFUNC ਖਾਲੀ U_EXPORT2
ucase_ਜੋੜੋ_Simple_Case_ਘੇਰਾ(UChar32 c, ਅਚਲ USetAdder *sa);

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
ucase_ਜੋੜੋ_String_Case_ਘੇਰਾ(ਅਚਲ UChar *s, int32_t ਲੰਮਾਈ, ਅਚਲ USetAdder *sa);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
U_NAMESPACE_BEGIN

/**
 * Iterator over characters with more than one code point in the full default Case_Folding.
 */
class U_COMMON_API ਭਰਿਆ_Case_Folding_Iterator {
ਜਨਤਕ:
    /** Constructor. */
    ਭਰਿਆ_Case_Folding_Iterator();
    /**
     * Returns the next (cp, full) pair where "full" is cp's full default Case_Folding.
     * Returns a negative cp value at the end of the iteration.
     */
    UChar32 ਅਗਲਾ(UnicodeString &ਭਰਿਆ);
ਨਿਜੀ:
    ਭਰਿਆ_Case_Folding_Iterator(ਅਚਲ ਭਰਿਆ_Case_Folding_Iterator &) = delete;  // no copy
    ਭਰਿਆ_Case_Folding_Iterator &ਸੰਚਾਲਕ=(ਅਚਲ ਭਰਿਆ_Case_Folding_Iterator &) = delete;  // no assignment

    ਅਚਲ char16_t *unfold;
    int32_t unfold_ਪੰਕਤੀਆਂ;
    int32_t unfold_ਪੰਕਤੀ_ਚੌੜਾਈ;
    int32_t unfold_String_ਚੌੜਾਈ;
    int32_t current_ਪੰਕਤੀ;
    int32_t ਪੰਕਤੀ_Cp_ਸੂਚਕਾਂਕ;
};

/**
 * Fast case mapping data for ASCII/Latin.
 * Linear arrays of delta bytes: 0=no mapping; EXC=exception.
 * Deltas must not cross the ASCII boundary, or else they cannot be easily used
 * in simple UTF-8 code.
 */
namespace LatinCase {

/** Case mapping/folding data for code points up to U+017F. */
constexpr char16_t ਹੱਦਬੰਦੀ_2 = 0x180;
/** U+017F case-folds and uppercases crossing the ASCII boundary. */
constexpr char16_t LONG_S = 0x17f;
/** Exception: Complex mapping, or too-large delta. */
constexpr int8_t EXC = -0x80;

/** Deltas for lowercasing for most locales, and default case folding. */
ਬਾਹਰੀ ਅਚਲ int8_t ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_ਸਾਧਾਰਨ[ਹੱਦਬੰਦੀ_2];
/** Deltas for lowercasing for tr/az/lt, and Turkic case folding. */
ਬਾਹਰੀ ਅਚਲ int8_t ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_TR_LT[ਹੱਦਬੰਦੀ_2];

/** Deltas for uppercasing for most locales. */
ਬਾਹਰੀ ਅਚਲ int8_t ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_ਸਾਧਾਰਨ[ਹੱਦਬੰਦੀ_2];
/** Deltas for uppercasing for tr/az. */
ਬਾਹਰੀ ਅਚਲ int8_t ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_TR[ਹੱਦਬੰਦੀ_2];

}  // namespace LatinCase

U_NAMESPACE_ਸਿਰਾ
#ਜੇ_ਅੰਤ

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ UChar32 U_CALLCONV
UCaseContextIterator(ਖਾਲੀ *context, int8_t dir);

/**
 * Sample struct which may be used by some implementations of
 * UCaseContextIterator.
 */
ਬਣਤਰ UCaseContext {
    ਖਾਲੀ *p;
    int32_t ਸ਼ੁਰੂ, ਸੂਚਕਾਂਕ, ਹੱਦਬੰਦੀ;
    int32_t cp_ਸ਼ੁਰੂ, cp_ਹੱਦਬੰਦੀ;
    int8_t dir;
    int8_t b1, b2, b3;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UCaseContext UCaseContext;

U_CDECL_ਸਿਰਾ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASECONTEXT_INITIALIZER { ਨਿਰਾ,  0, 0, 0,  0, 0,  0,  0, 0, 0 }

ਗਿਣਤੀ {
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
    UCASE_MAX_STRING_ਲੰਮਾਈ=0x1f
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
ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਹੇਠਲਾ(UChar32 c,
                  UCaseContextIterator *iter, ਖਾਲੀ *context,
                  ਅਚਲ UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਉੱਤਲਾ(UChar32 c,
                  UCaseContextIterator *iter, ਖਾਲੀ *context,
                  ਅਚਲ UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Title(UChar32 c,
                  UCaseContextIterator *iter, ਖਾਲੀ *context,
                  ਅਚਲ UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Folding(UChar32 c,
                    ਅਚਲ UChar **pString,
                    uint32_t options);

U_CFUNC int32_t U_EXPORT2
ucase_has_ਦੁਆਧਾਰੀ_Property(UChar32 c, UProperty which);


U_CDECL_BEGIN

/**
 * @internal
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ int32_t U_CALLCONV
UCase_Map_ਭਰਿਆ(UChar32 c,
             UCaseContextIterator *iter, ਖਾਲੀ *context,
             ਅਚਲ UChar **pString,
             int32_t caseLocale);

U_CDECL_ਸਿਰਾ

/* for icuexportdata -------------------------------------------------------- */

ਬਣਤਰ UCaseProps {
    ਖਾਲੀ *mem;  // TODO: was unused, and type UDataMemory -- remove
    ਅਚਲ int32_t *indexes;
    ਅਚਲ uint16_t *exceptions;
    ਅਚਲ uint16_t *unfold;

    UTrie2 trie;
    uint8_t format_ਸੰਸਕਰਣ[4];
};

U_CAPI ਅਚਲ ਬਣਤਰ UCaseProps * U_EXPORT2
ucase_getSingleton(int32_t *p_Exceptions_ਲੰਮਾਈ, int32_t *p_Unfold_ਲੰਮਾਈ);

/* file definitions --------------------------------------------------------- */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_ਅੰਕੜੇ_NAME "ucase"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_ਅੰਕੜੇ_TYPE "icu"

/* format "cAsE" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_FMT_0 0x63
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_FMT_1 0x41
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_FMT_2 0x53
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_FMT_3 0x45

/* indexes into indexes[] */
ਗਿਣਤੀ {
    UCASE_IX_ਸੂਚਕਾਂਕ_ਸਿਖਰ,
    UCASE_IX_ਲੰਮਾਈ,
    UCASE_IX_TRIE_ਮਾਪ,
    UCASE_IX_EXC_ਲੰਮਾਈ,
    UCASE_IX_UNFOLD_ਲੰਮਾਈ,

    UCASE_IX_MAX_ਭਰਿਆ_ਲੰਮਾਈ=15,
    UCASE_IX_ਸਿਖਰ=16
};

/* definitions for 16-bit case properties word ------------------------------ */

U_CFUNC ਅਚਲ UTrie2 * U_EXPORT2
ucase_getTrie(ਖਾਲੀ);

/* 2-bit constants for types of cased characters */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_TYPE_ਨਕਾਬ     3
ਗਿਣਤੀ {
    UCASE_NONE,
    UCASE_ਹੇਠਲਾ,
    UCASE_ਉੱਤਲਾ,
    UCASE_TITLE
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_GET_TYPE(props) ((props)&UCASE_TYPE_ਨਕਾਬ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_GET_TYPE_AND_IGNORABLE(props) ((props)&7)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_IS_ਉੱਤਲਾ_OR_TITLE(props) ((props)&2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_IGNORABLE         4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_EXCEPTION         8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_SENSITIVE         0x10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_HAS_EXCEPTION(props) ((props)&UCASE_EXCEPTION)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_DOT_ਨਕਾਬ      0x60
ਗਿਣਤੀ {
    UCASE_NO_DOT=0,         /* normal characters with cc=0 */
    UCASE_SOFT_DOTTED=0x20, /* soft-dotted characters with cc=0 */
    UCASE_ABOVE=0x40,       /* "above" accents with cc=230 */
    UCASE_OTHER_ACCENT=0x60 /* other accent character (0<cc!=230) */
};

/* no exception: bits 15..7 are a 9-bit signed case mapping delta */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_DELTA_ਖਿਸਕਾਓ   7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_DELTA_ਨਕਾਬ    0xff80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_MAX_DELTA     0xff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_MIN_DELTA     (-UCASE_MAX_DELTA-1)

#ਜੇ U_SIGNED_ਸੱਜਾ_ਖਿਸਕਾਓ_IS_ARITHMETIC
#   define UCASE_GET_DELTA(props) ((int16_t)(props)>>UCASE_DELTA_ਖਿਸਕਾਓ)
#ਨਹੀਂਤਾਂ
#   define UCASE_GET_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UCASE_DELTA_ਖਿਸਕਾਓ)|0xfe00) : ((uint16_t)(props)>>UCASE_DELTA_ਖਿਸਕਾਓ))
#ਜੇ_ਅੰਤ

/* exception: bits 15..4 are an unsigned 12-bit index into the exceptions array */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_EXC_ਖਿਸਕਾਓ     4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_EXC_ਨਕਾਬ      0xfff0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_MAX_EXCEPTIONS ((UCASE_EXC_ਨਕਾਬ>>UCASE_EXC_ਖਿਸਕਾਓ)+1)

/* definitions for 16-bit main exceptions word ------------------------------ */

/* first 8 bits indicate values in optional slots */
ਗਿਣਤੀ {
    UCASE_EXC_ਹੇਠਲਾ,
    UCASE_EXC_FOLD,
    UCASE_EXC_ਉੱਤਲਾ,
    UCASE_EXC_TITLE,
    UCASE_EXC_DELTA,
    UCASE_EXC_5,            /* reserved */
    UCASE_EXC_ਘੇਰਾ,
    UCASE_EXC_ਭਰਿਆ_MAPPINGS,
    UCASE_EXC_ALL_SLOTS     /* one past the last slot */
};

/* each slot is 2 uint16_t instead of 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_EXC_DOUBLE_SLOTS      0x100

ਗਿਣਤੀ {
    UCASE_EXC_NO_SIMPLE_CASE_FOLDING=0x200,
    UCASE_EXC_DELTA_IS_NEGATIVE=0x400,
    UCASE_EXC_SENSITIVE=0x800
};

/* UCASE_EXC_DOT_MASK=UCASE_DOT_MASK<<UCASE_EXC_DOT_SHIFT */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_EXC_DOT_ਖਿਸਕਾਓ     7

/* normally stored in the main word, but pushed out for larger exception indexes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_EXC_DOT_ਨਕਾਬ      0x3000
ਗਿਣਤੀ {
    UCASE_EXC_NO_DOT=0,
    UCASE_EXC_SOFT_DOTTED=0x1000,
    UCASE_EXC_ABOVE=0x2000,         /* "above" accents with cc=230 */
    UCASE_EXC_OTHER_ACCENT=0x3000   /* other character (0<cc!=230) */
};

/* complex/conditional mappings */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_EXC_CONDITIONAL_SPECIAL   0x4000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_EXC_CONDITIONAL_FOLD      0x8000

/* definitions for lengths word for full case mappings */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_ਭਰਿਆ_ਹੇਠਲਾ    0xf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_ਭਰਿਆ_FOLDING  0xf0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_ਭਰਿਆ_ਉੱਤਲਾ    0xf00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_ਭਰਿਆ_TITLE    0xf000

/* maximum lengths */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_ਭਰਿਆ_MAPPINGS_MAX_ਲੰਮਾਈ (4*0xf)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCASE_ਘੇਰਾ_MAX_ਲੰਮਾਈ 0xf

/* constants for reverse case folding ("unfold") data */
ਗਿਣਤੀ {
    UCASE_UNFOLD_ਪੰਕਤੀਆਂ,
    UCASE_UNFOLD_ਪੰਕਤੀ_ਚੌੜਾਈ,
    UCASE_UNFOLD_STRING_ਚੌੜਾਈ
};

#ਜੇ_ਅੰਤ
