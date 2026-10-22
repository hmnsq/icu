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

#यदि_अपरिभाषितम् __UCASE_H__
#परिभाषय __UCASE_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uset_imp.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "utrie2.h"

#यदि_परिभाषितम् __cplusplus
U_NAMESPACE_BEGIN

class UnicodeString;

U_NAMESPACE_अन्तः
#यदि_अन्तः

/* library API -------------------------------------------------------------- */

U_CFUNC शून्य U_EXPORT2
ucase_सङ्कलय_Property_Starts(ध्रुव USetAdder *sa, UError_संहिता *p_Error_संहिता);

/**
 * Requires non-NULL locale ID but otherwise does the equivalent of
 * checking for language codes as if uloc_getLanguage() were called:
 * Accepts both 2- and 3-letter codes and accepts case variants.
 */
U_CFUNC int32_t
ucase_getCaseLocale(ध्रुव वर्ण *locale);

/* Casing locale types for ucase_getCaseLocale */
गणना {
    UCASE_LOC_UNKNOWN,
    UCASE_LOC_मूलम्,
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
#परिभाषय STRCASECMP_OPTIONS_आवरणम् 0xffff

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
#परिभाषय FOLD_CASE_OPTIONS_आवरणम् 7

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
U_CFUNC शून्य U_EXPORT2
ucase_सङ्कलय_Case_संवृतिः(UChar32 c, ध्रुव USetAdder *sa);

/** Case closure with only scf=Simple_Case_Folding. */
U_CFUNC शून्य U_EXPORT2
ucase_सङ्कलय_Simple_Case_संवृतिः(UChar32 c, ध्रुव USetAdder *sa);

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
ucase_सङ्कलय_String_Case_संवृतिः(ध्रुव UChar *s, int32_t दीर्घता, ध्रुव USetAdder *sa);

#यदि_परिभाषितम् __cplusplus
U_NAMESPACE_BEGIN

/**
 * Iterator over characters with more than one code point in the full default Case_Folding.
 */
class U_COMMON_API पूर्णम्_Case_Folding_Iterator {
सार्वजनिक:
    /** Constructor. */
    पूर्णम्_Case_Folding_Iterator();
    /**
     * Returns the next (cp, full) pair where "full" is cp's full default Case_Folding.
     * Returns a negative cp value at the end of the iteration.
     */
    UChar32 अग्र(UnicodeString &पूर्णम्);
गुप्त:
    पूर्णम्_Case_Folding_Iterator(ध्रुव पूर्णम्_Case_Folding_Iterator &) = delete;  // no copy
    पूर्णम्_Case_Folding_Iterator &संक्रियकः=(ध्रुव पूर्णम्_Case_Folding_Iterator &) = delete;  // no assignment

    ध्रुव char16_t *unfold;
    int32_t unfold_पङ्क्तिकाः;
    int32_t unfold_Row_विस्तारः;
    int32_t unfold_String_विस्तारः;
    int32_t currentRow;
    int32_t row_Cp_सूचकांक;
};

/**
 * Fast case mapping data for ASCII/Latin.
 * Linear arrays of delta bytes: 0=no mapping; EXC=exception.
 * Deltas must not cross the ASCII boundary, or else they cannot be easily used
 * in simple UTF-8 code.
 */
namespace LatinCase {

/** Case mapping/folding data for code points up to U+017F. */
constexpr char16_t सीमा_2 = 0x180;
/** U+017F case-folds and uppercases crossing the ASCII boundary. */
constexpr char16_t LONG_S = 0x17f;
/** Exception: Complex mapping, or too-large delta. */
constexpr int8_t EXC = -0x80;

/** Deltas for lowercasing for most locales, and default case folding. */
बाह्य ध्रुव int8_t गन्तव्यम्_अधस्तनम्_प्रसामान्यम्[सीमा_2];
/** Deltas for lowercasing for tr/az/lt, and Turkic case folding. */
बाह्य ध्रुव int8_t गन्तव्यम्_अधस्तनम्_TR_LT[सीमा_2];

/** Deltas for uppercasing for most locales. */
बाह्य ध्रुव int8_t गन्तव्यम्_ऊर्ध्वस्थम्_प्रसामान्यम्[सीमा_2];
/** Deltas for uppercasing for tr/az. */
बाह्य ध्रुव int8_t गन्तव्यम्_ऊर्ध्वस्थम्_TR[सीमा_2];

}  // namespace LatinCase

U_NAMESPACE_अन्तः
#यदि_अन्तः

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
प्रकारनिर्देश UChar32 U_CALLCONV
UCaseContextIterator(शून्य *context, int8_t dir);

/**
 * Sample struct which may be used by some implementations of
 * UCaseContextIterator.
 */
संरचना UCaseContext {
    शून्य *p;
    int32_t start, सूचकांक, सीमा;
    int32_t cpStart, cp_सीमा;
    int8_t dir;
    int8_t b1, b2, b3;
};
प्रकारनिर्देश संरचना UCaseContext UCaseContext;

U_CDECL_अन्तः

#परिभाषय UCASECONTEXT_INITIALIZER { रिक्त,  0, 0, 0,  0, 0,  0,  0, 0, 0 }

गणना {
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
    UCASE_MAX_STRING_दीर्घता=0x1f
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
ucase_गन्तव्यम्_पूर्णम्_अधस्तनम्(UChar32 c,
                  UCaseContextIterator *iter, शून्य *context,
                  ध्रुव UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_गन्तव्यम्_पूर्णम्_ऊर्ध्वस्थम्(UChar32 c,
                  UCaseContextIterator *iter, शून्य *context,
                  ध्रुव UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_गन्तव्यम्_पूर्णम्_Title(UChar32 c,
                  UCaseContextIterator *iter, शून्य *context,
                  ध्रुव UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_गन्तव्यम्_पूर्णम्_Folding(UChar32 c,
                    ध्रुव UChar **pString,
                    uint32_t options);

U_CFUNC int32_t U_EXPORT2
ucase_has_द्विमान_Property(UChar32 c, UProperty which);


U_CDECL_BEGIN

/**
 * @internal
 */
प्रकारनिर्देश int32_t U_CALLCONV
UCase_Map_पूर्णम्(UChar32 c,
             UCaseContextIterator *iter, शून्य *context,
             ध्रुव UChar **pString,
             int32_t caseLocale);

U_CDECL_अन्तः

/* for icuexportdata -------------------------------------------------------- */

संरचना UCaseProps {
    शून्य *mem;  // TODO: was unused, and type UDataMemory -- remove
    ध्रुव int32_t *indexes;
    ध्रुव uint16_t *exceptions;
    ध्रुव uint16_t *unfold;

    UTrie2 trie;
    uint8_t format_संस्करणम्[4];
};

U_CAPI ध्रुव संरचना UCaseProps * U_EXPORT2
ucase_getSingleton(int32_t *p_Exceptions_दीर्घता, int32_t *p_Unfold_दीर्घता);

/* file definitions --------------------------------------------------------- */

#परिभाषय UCASE_दत्त_NAME "ucase"
#परिभाषय UCASE_दत्त_TYPE "icu"

/* format "cAsE" */
#परिभाषय UCASE_FMT_0 0x63
#परिभाषय UCASE_FMT_1 0x41
#परिभाषय UCASE_FMT_2 0x53
#परिभाषय UCASE_FMT_3 0x45

/* indexes into indexes[] */
गणना {
    UCASE_IX_सूचकांक_शिखरम्,
    UCASE_IX_दीर्घता,
    UCASE_IX_TRIE_आकार,
    UCASE_IX_EXC_दीर्घता,
    UCASE_IX_UNFOLD_दीर्घता,

    UCASE_IX_MAX_पूर्णम्_दीर्घता=15,
    UCASE_IX_शिखरम्=16
};

/* definitions for 16-bit case properties word ------------------------------ */

U_CFUNC ध्रुव UTrie2 * U_EXPORT2
ucase_getTrie(शून्य);

/* 2-bit constants for types of cased characters */
#परिभाषय UCASE_TYPE_आवरणम्     3
गणना {
    UCASE_NONE,
    UCASE_अधस्तनम्,
    UCASE_ऊर्ध्वस्थम्,
    UCASE_TITLE
};

#परिभाषय UCASE_GET_TYPE(props) ((props)&UCASE_TYPE_आवरणम्)
#परिभाषय UCASE_GET_TYPE_AND_IGNORABLE(props) ((props)&7)

#परिभाषय UCASE_IS_ऊर्ध्वस्थम्_OR_TITLE(props) ((props)&2)

#परिभाषय UCASE_IGNORABLE         4
#परिभाषय UCASE_EXCEPTION         8
#परिभाषय UCASE_SENSITIVE         0x10

#परिभाषय UCASE_HAS_EXCEPTION(props) ((props)&UCASE_EXCEPTION)

#परिभाषय UCASE_DOT_आवरणम्      0x60
गणना {
    UCASE_NO_DOT=0,         /* normal characters with cc=0 */
    UCASE_SOFT_DOTTED=0x20, /* soft-dotted characters with cc=0 */
    UCASE_ABOVE=0x40,       /* "above" accents with cc=230 */
    UCASE_OTHER_ACCENT=0x60 /* other accent character (0<cc!=230) */
};

/* no exception: bits 15..7 are a 9-bit signed case mapping delta */
#परिभाषय UCASE_DELTA_सरणम्   7
#परिभाषय UCASE_DELTA_आवरणम्    0xff80
#परिभाषय UCASE_MAX_DELTA     0xff
#परिभाषय UCASE_MIN_DELTA     (-UCASE_MAX_DELTA-1)

#यदि U_SIGNED_दक्षिणः_सरणम्_IS_ARITHMETIC
#   define UCASE_GET_DELTA(props) ((int16_t)(props)>>UCASE_DELTA_सरणम्)
#अन्यथा
#   define UCASE_GET_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UCASE_DELTA_सरणम्)|0xfe00) : ((uint16_t)(props)>>UCASE_DELTA_सरणम्))
#यदि_अन्तः

/* exception: bits 15..4 are an unsigned 12-bit index into the exceptions array */
#परिभाषय UCASE_EXC_सरणम्     4
#परिभाषय UCASE_EXC_आवरणम्      0xfff0
#परिभाषय UCASE_MAX_EXCEPTIONS ((UCASE_EXC_आवरणम्>>UCASE_EXC_सरणम्)+1)

/* definitions for 16-bit main exceptions word ------------------------------ */

/* first 8 bits indicate values in optional slots */
गणना {
    UCASE_EXC_अधस्तनम्,
    UCASE_EXC_FOLD,
    UCASE_EXC_ऊर्ध्वस्थम्,
    UCASE_EXC_TITLE,
    UCASE_EXC_DELTA,
    UCASE_EXC_5,            /* reserved */
    UCASE_EXC_संवृतिः,
    UCASE_EXC_पूर्णम्_MAPPINGS,
    UCASE_EXC_ALL_SLOTS     /* one past the last slot */
};

/* each slot is 2 uint16_t instead of 1 */
#परिभाषय UCASE_EXC_DOUBLE_SLOTS      0x100

गणना {
    UCASE_EXC_NO_SIMPLE_CASE_FOLDING=0x200,
    UCASE_EXC_DELTA_IS_NEGATIVE=0x400,
    UCASE_EXC_SENSITIVE=0x800
};

/* UCASE_EXC_DOT_MASK=UCASE_DOT_MASK<<UCASE_EXC_DOT_SHIFT */
#परिभाषय UCASE_EXC_DOT_सरणम्     7

/* normally stored in the main word, but pushed out for larger exception indexes */
#परिभाषय UCASE_EXC_DOT_आवरणम्      0x3000
गणना {
    UCASE_EXC_NO_DOT=0,
    UCASE_EXC_SOFT_DOTTED=0x1000,
    UCASE_EXC_ABOVE=0x2000,         /* "above" accents with cc=230 */
    UCASE_EXC_OTHER_ACCENT=0x3000   /* other character (0<cc!=230) */
};

/* complex/conditional mappings */
#परिभाषय UCASE_EXC_CONDITIONAL_SPECIAL   0x4000
#परिभाषय UCASE_EXC_CONDITIONAL_FOLD      0x8000

/* definitions for lengths word for full case mappings */
#परिभाषय UCASE_पूर्णम्_अधस्तनम्    0xf
#परिभाषय UCASE_पूर्णम्_FOLDING  0xf0
#परिभाषय UCASE_पूर्णम्_ऊर्ध्वस्थम्    0xf00
#परिभाषय UCASE_पूर्णम्_TITLE    0xf000

/* maximum lengths */
#परिभाषय UCASE_पूर्णम्_MAPPINGS_MAX_दीर्घता (4*0xf)
#परिभाषय UCASE_संवृतिः_MAX_दीर्घता 0xf

/* constants for reverse case folding ("unfold") data */
गणना {
    UCASE_UNFOLD_पङ्क्तिकाः,
    UCASE_UNFOLD_ROW_विस्तारः,
    UCASE_UNFOLD_STRING_विस्तारः
};

#यदि_अन्तः
