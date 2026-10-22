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

#నిర్వచితం_కాకపోతే __UCASE_H__
#నిర్వచించు __UCASE_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uset_imp.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "utrie2.h"

#నిర్వచితమైతే __cplusplus
U_NAMESPACE_BEGIN

class UnicodeString;

U_NAMESPACE_చివర
#అయితే_ముగింపు

/* library API -------------------------------------------------------------- */

U_CFUNC శూన్య U_EXPORT2
ucase_కూడు_Property_Starts(స్థిరరాశి USetAdder *sa, UError_సంకేతం *p_Error_సంకేతం);

/**
 * Requires non-NULL locale ID but otherwise does the equivalent of
 * checking for language codes as if uloc_getLanguage() were called:
 * Accepts both 2- and 3-letter codes and accepts case variants.
 */
U_CFUNC int32_t
ucase_getCaseLocale(స్థిరరాశి అక్షరం *locale);

/* Casing locale types for ucase_getCaseLocale */
గణన {
    UCASE_LOC_UNKNOWN,
    UCASE_LOC_మూలం,
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
#నిర్వచించు STRCASECMP_OPTIONS_ముసుగు 0xffff

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
#నిర్వచించు FOLD_CASE_OPTIONS_ముసుగు 7

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
U_CFUNC శూన్య U_EXPORT2
ucase_కూడు_Case_సంవృతం(UChar32 c, స్థిరరాశి USetAdder *sa);

/** Case closure with only scf=Simple_Case_Folding. */
U_CFUNC శూన్య U_EXPORT2
ucase_కూడు_Simple_Case_సంవృతం(UChar32 c, స్థిరరాశి USetAdder *sa);

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
ucase_కూడు_String_Case_సంవృతం(స్థిరరాశి UChar *s, int32_t నిడివి, స్థిరరాశి USetAdder *sa);

#నిర్వచితమైతే __cplusplus
U_NAMESPACE_BEGIN

/**
 * Iterator over characters with more than one code point in the full default Case_Folding.
 */
class U_COMMON_API నిండిన_Case_Folding_Iterator {
బహిరంగ:
    /** Constructor. */
    నిండిన_Case_Folding_Iterator();
    /**
     * Returns the next (cp, full) pair where "full" is cp's full default Case_Folding.
     * Returns a negative cp value at the end of the iteration.
     */
    UChar32 తదుపరి(UnicodeString &నిండిన);
అంతరంగ:
    నిండిన_Case_Folding_Iterator(స్థిరరాశి నిండిన_Case_Folding_Iterator &) = delete;  // no copy
    నిండిన_Case_Folding_Iterator &క్రియాకారకం=(స్థిరరాశి నిండిన_Case_Folding_Iterator &) = delete;  // no assignment

    స్థిరరాశి char16_t *unfold;
    int32_t unfold_అడ్డువరుసలు;
    int32_t unfold_అడ్డువరుస_వెడల్పు;
    int32_t unfold_String_వెడల్పు;
    int32_t current_అడ్డువరుస;
    int32_t అడ్డువరుస_Cp_సూచీ;
};

/**
 * Fast case mapping data for ASCII/Latin.
 * Linear arrays of delta bytes: 0=no mapping; EXC=exception.
 * Deltas must not cross the ASCII boundary, or else they cannot be easily used
 * in simple UTF-8 code.
 */
namespace LatinCase {

/** Case mapping/folding data for code points up to U+017F. */
constexpr char16_t హద్దు_2 = 0x180;
/** U+017F case-folds and uppercases crossing the ASCII boundary. */
constexpr char16_t LONG_S = 0x17f;
/** Exception: Complex mapping, or too-large delta. */
constexpr int8_t EXC = -0x80;

/** Deltas for lowercasing for most locales, and default case folding. */
బాహ్య స్థిరరాశి int8_t గమ్యం_క్రింది_సాధారణ[హద్దు_2];
/** Deltas for lowercasing for tr/az/lt, and Turkic case folding. */
బాహ్య స్థిరరాశి int8_t గమ్యం_క్రింది_TR_LT[హద్దు_2];

/** Deltas for uppercasing for most locales. */
బాహ్య స్థిరరాశి int8_t గమ్యం_పై_సాధారణ[హద్దు_2];
/** Deltas for uppercasing for tr/az. */
బాహ్య స్థిరరాశి int8_t గమ్యం_పై_TR[హద్దు_2];

}  // namespace LatinCase

U_NAMESPACE_చివర
#అయితే_ముగింపు

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
రకనిర్వచనం UChar32 U_CALLCONV
UCaseContextIterator(శూన్య *context, int8_t dir);

/**
 * Sample struct which may be used by some implementations of
 * UCaseContextIterator.
 */
నిర్మాణం UCaseContext {
    శూన్య *p;
    int32_t ప్రారంభం, సూచీ, హద్దు;
    int32_t cp_ప్రారంభం, cp_హద్దు;
    int8_t dir;
    int8_t b1, b2, b3;
};
రకనిర్వచనం నిర్మాణం UCaseContext UCaseContext;

U_CDECL_చివర

#నిర్వచించు UCASECONTEXT_INITIALIZER { ఖాళీ,  0, 0, 0,  0, 0,  0,  0, 0, 0 }

గణన {
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
    UCASE_MAX_STRING_నిడివి=0x1f
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
ucase_గమ్యం_నిండిన_క్రింది(UChar32 c,
                  UCaseContextIterator *iter, శూన్య *context,
                  స్థిరరాశి UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_గమ్యం_నిండిన_పై(UChar32 c,
                  UCaseContextIterator *iter, శూన్య *context,
                  స్థిరరాశి UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_గమ్యం_నిండిన_Title(UChar32 c,
                  UCaseContextIterator *iter, శూన్య *context,
                  స్థిరరాశి UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_గమ్యం_నిండిన_Folding(UChar32 c,
                    స్థిరరాశి UChar **pString,
                    uint32_t options);

U_CFUNC int32_t U_EXPORT2
ucase_has_ద్విమాన_Property(UChar32 c, UProperty which);


U_CDECL_BEGIN

/**
 * @internal
 */
రకనిర్వచనం int32_t U_CALLCONV
UCase_Map_నిండిన(UChar32 c,
             UCaseContextIterator *iter, శూన్య *context,
             స్థిరరాశి UChar **pString,
             int32_t caseLocale);

U_CDECL_చివర

/* for icuexportdata -------------------------------------------------------- */

నిర్మాణం UCaseProps {
    శూన్య *mem;  // TODO: was unused, and type UDataMemory -- remove
    స్థిరరాశి int32_t *indexes;
    స్థిరరాశి uint16_t *exceptions;
    స్థిరరాశి uint16_t *unfold;

    UTrie2 trie;
    uint8_t format_సంస్కరణ[4];
};

U_CAPI స్థిరరాశి నిర్మాణం UCaseProps * U_EXPORT2
ucase_getSingleton(int32_t *p_Exceptions_నిడివి, int32_t *p_Unfold_నిడివి);

/* file definitions --------------------------------------------------------- */

#నిర్వచించు UCASE_దత్తాంశం_NAME "ucase"
#నిర్వచించు UCASE_దత్తాంశం_TYPE "icu"

/* format "cAsE" */
#నిర్వచించు UCASE_FMT_0 0x63
#నిర్వచించు UCASE_FMT_1 0x41
#నిర్వచించు UCASE_FMT_2 0x53
#నిర్వచించు UCASE_FMT_3 0x45

/* indexes into indexes[] */
గణన {
    UCASE_IX_సూచీ_శిఖరం,
    UCASE_IX_నిడివి,
    UCASE_IX_TRIE_కొలత,
    UCASE_IX_EXC_నిడివి,
    UCASE_IX_UNFOLD_నిడివి,

    UCASE_IX_MAX_నిండిన_నిడివి=15,
    UCASE_IX_శిఖరం=16
};

/* definitions for 16-bit case properties word ------------------------------ */

U_CFUNC స్థిరరాశి UTrie2 * U_EXPORT2
ucase_getTrie(శూన్య);

/* 2-bit constants for types of cased characters */
#నిర్వచించు UCASE_TYPE_ముసుగు     3
గణన {
    UCASE_NONE,
    UCASE_క్రింది,
    UCASE_పై,
    UCASE_TITLE
};

#నిర్వచించు UCASE_GET_TYPE(props) ((props)&UCASE_TYPE_ముసుగు)
#నిర్వచించు UCASE_GET_TYPE_AND_IGNORABLE(props) ((props)&7)

#నిర్వచించు UCASE_IS_పై_OR_TITLE(props) ((props)&2)

#నిర్వచించు UCASE_IGNORABLE         4
#నిర్వచించు UCASE_EXCEPTION         8
#నిర్వచించు UCASE_SENSITIVE         0x10

#నిర్వచించు UCASE_HAS_EXCEPTION(props) ((props)&UCASE_EXCEPTION)

#నిర్వచించు UCASE_DOT_ముసుగు      0x60
గణన {
    UCASE_NO_DOT=0,         /* normal characters with cc=0 */
    UCASE_SOFT_DOTTED=0x20, /* soft-dotted characters with cc=0 */
    UCASE_ABOVE=0x40,       /* "above" accents with cc=230 */
    UCASE_OTHER_ACCENT=0x60 /* other accent character (0<cc!=230) */
};

/* no exception: bits 15..7 are a 9-bit signed case mapping delta */
#నిర్వచించు UCASE_DELTA_జరుపు   7
#నిర్వచించు UCASE_DELTA_ముసుగు    0xff80
#నిర్వచించు UCASE_MAX_DELTA     0xff
#నిర్వచించు UCASE_MIN_DELTA     (-UCASE_MAX_DELTA-1)

#ఒకవేళ U_SIGNED_కుడి_జరుపు_IS_ARITHMETIC
#   define UCASE_GET_DELTA(props) ((int16_t)(props)>>UCASE_DELTA_జరుపు)
#లేకపోతే
#   define UCASE_GET_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UCASE_DELTA_జరుపు)|0xfe00) : ((uint16_t)(props)>>UCASE_DELTA_జరుపు))
#అయితే_ముగింపు

/* exception: bits 15..4 are an unsigned 12-bit index into the exceptions array */
#నిర్వచించు UCASE_EXC_జరుపు     4
#నిర్వచించు UCASE_EXC_ముసుగు      0xfff0
#నిర్వచించు UCASE_MAX_EXCEPTIONS ((UCASE_EXC_ముసుగు>>UCASE_EXC_జరుపు)+1)

/* definitions for 16-bit main exceptions word ------------------------------ */

/* first 8 bits indicate values in optional slots */
గణన {
    UCASE_EXC_క్రింది,
    UCASE_EXC_FOLD,
    UCASE_EXC_పై,
    UCASE_EXC_TITLE,
    UCASE_EXC_DELTA,
    UCASE_EXC_5,            /* reserved */
    UCASE_EXC_సంవృతం,
    UCASE_EXC_నిండిన_MAPPINGS,
    UCASE_EXC_ALL_SLOTS     /* one past the last slot */
};

/* each slot is 2 uint16_t instead of 1 */
#నిర్వచించు UCASE_EXC_DOUBLE_SLOTS      0x100

గణన {
    UCASE_EXC_NO_SIMPLE_CASE_FOLDING=0x200,
    UCASE_EXC_DELTA_IS_NEGATIVE=0x400,
    UCASE_EXC_SENSITIVE=0x800
};

/* UCASE_EXC_DOT_MASK=UCASE_DOT_MASK<<UCASE_EXC_DOT_SHIFT */
#నిర్వచించు UCASE_EXC_DOT_జరుపు     7

/* normally stored in the main word, but pushed out for larger exception indexes */
#నిర్వచించు UCASE_EXC_DOT_ముసుగు      0x3000
గణన {
    UCASE_EXC_NO_DOT=0,
    UCASE_EXC_SOFT_DOTTED=0x1000,
    UCASE_EXC_ABOVE=0x2000,         /* "above" accents with cc=230 */
    UCASE_EXC_OTHER_ACCENT=0x3000   /* other character (0<cc!=230) */
};

/* complex/conditional mappings */
#నిర్వచించు UCASE_EXC_CONDITIONAL_SPECIAL   0x4000
#నిర్వచించు UCASE_EXC_CONDITIONAL_FOLD      0x8000

/* definitions for lengths word for full case mappings */
#నిర్వచించు UCASE_నిండిన_క్రింది    0xf
#నిర్వచించు UCASE_నిండిన_FOLDING  0xf0
#నిర్వచించు UCASE_నిండిన_పై    0xf00
#నిర్వచించు UCASE_నిండిన_TITLE    0xf000

/* maximum lengths */
#నిర్వచించు UCASE_నిండిన_MAPPINGS_MAX_నిడివి (4*0xf)
#నిర్వచించు UCASE_సంవృతం_MAX_నిడివి 0xf

/* constants for reverse case folding ("unfold") data */
గణన {
    UCASE_UNFOLD_అడ్డువరుసలు,
    UCASE_UNFOLD_అడ్డువరుస_వెడల్పు,
    UCASE_UNFOLD_STRING_వెడల్పు
};

#అయితే_ముగింపు
