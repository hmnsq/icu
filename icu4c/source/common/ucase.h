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

#अगर_अघोषित __UCASE_H__
#घोषणा __UCASE_H__

#समावेश "unicode/utypes.h"
#समावेश "unicode/uset.h"
#समावेश "putilimp.h"
#समावेश "uset_imp.h"
#समावेश "udataswp.h"
#समावेश "utrie2.h"

#अगर_घोषित __cplusplus
U_NAMESPACE_BEGIN

class UnicodeString;

U_NAMESPACE_END
#पूर्ण_अगर

/* library API -------------------------------------------------------------- */

U_CFUNC व्योम U_EXPORT2
ucase_addPropertyStarts(स्थिर USetAdder *sa, UErrorCode *pErrorCode);

/**
 * Requires non-NULL locale ID but otherwise does the equivalent of
 * checking for language codes as if uloc_getLanguage() were called:
 * Accepts both 2- and 3-letter codes and accepts case variants.
 */
U_CFUNC int32_t
ucase_getCaseLocale(स्थिर अक्षर *locale);

/* Casing locale types for ucase_getCaseLocale */
क्रमागत {
    UCASE_LOC_UNKNOWN,
    UCASE_LOC_मूल,
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
#घोषणा _STRCASECMP_OPTIONS_MASK 0xffff

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
#घोषणा _FOLD_CASE_OPTIONS_MASK 7

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
U_CFUNC व्योम U_EXPORT2
ucase_addCaseClosure(UChar32 c, स्थिर USetAdder *sa);

/** Case closure with only scf=Simple_Case_Folding. */
U_CFUNC व्योम U_EXPORT2
ucase_addSimpleCaseClosure(UChar32 c, स्थिर USetAdder *sa);

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
ucase_addStringCaseClosure(स्थिर UChar *s, int32_t लंबाई, स्थिर USetAdder *sa);

#अगर_घोषित __cplusplus
U_NAMESPACE_BEGIN

/**
 * Iterator over characters with more than one code point in the full default Case_Folding.
 */
class U_COMMON_API FullCaseFoldingIterator {
खुला:
    /** Constructor. */
    FullCaseFoldingIterator();
    /**
     * Returns the next (cp, full) pair where "full" is cp's full default Case_Folding.
     * Returns a negative cp value at the end of the iteration.
     */
    UChar32 अग्र(UnicodeString &full);
निजी:
    FullCaseFoldingIterator(स्थिर FullCaseFoldingIterator &) = delete;  // no copy
    FullCaseFoldingIterator &चालक=(स्थिर FullCaseFoldingIterator &) = delete;  // no assignment

    स्थिर char16_t *unfold;
    int32_t unfoldRows;
    int32_t unfoldRowWidth;
    int32_t unfoldStringWidth;
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
constexpr char16_t LIMIT = 0x180;
/** U+017F case-folds and uppercases crossing the ASCII boundary. */
constexpr char16_t LONG_S = 0x17f;
/** Exception: Complex mapping, or too-large delta. */
constexpr int8_t EXC = -0x80;

/** Deltas for lowercasing for most locales, and default case folding. */
बाह्य स्थिर int8_t TO_LOWER_NORMAL[LIMIT];
/** Deltas for lowercasing for tr/az/lt, and Turkic case folding. */
बाह्य स्थिर int8_t TO_LOWER_TR_LT[LIMIT];

/** Deltas for uppercasing for most locales. */
बाह्य स्थिर int8_t TO_UPPER_NORMAL[LIMIT];
/** Deltas for uppercasing for tr/az. */
बाह्य स्थिर int8_t TO_UPPER_TR[LIMIT];

}  // namespace LatinCase

U_NAMESPACE_END
#पूर्ण_अगर

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
प्रकार UChar32 U_CALLCONV
UCaseContextIterator(व्योम *context, int8_t dir);

/**
 * Sample struct which may be used by some implementations of
 * UCaseContextIterator.
 */
काष्ठा UCaseContext {
    व्योम *p;
    int32_t start, सूचकांक, limit;
    int32_t cpStart, cpLimit;
    int8_t dir;
    int8_t b1, b2, b3;
};
प्रकार काष्ठा UCaseContext UCaseContext;

U_CDECL_END

#घोषणा UCASECONTEXT_INITIALIZER { शून्य,  0, 0, 0,  0, 0,  0,  0, 0, 0 }

क्रमागत {
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
    UCASE_MAX_STRING_लंबाई=0x1f
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
ucase_toFullLower(UChar32 c,
                  UCaseContextIterator *iter, व्योम *context,
                  स्थिर UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_toFullUpper(UChar32 c,
                  UCaseContextIterator *iter, व्योम *context,
                  स्थिर UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_toFullTitle(UChar32 c,
                  UCaseContextIterator *iter, व्योम *context,
                  स्थिर UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_toFullFolding(UChar32 c,
                    स्थिर UChar **pString,
                    uint32_t options);

U_CFUNC int32_t U_EXPORT2
ucase_has_द्विआधारी_Property(UChar32 c, UProperty which);


U_CDECL_BEGIN

/**
 * @internal
 */
प्रकार int32_t U_CALLCONV
UCaseMapFull(UChar32 c,
             UCaseContextIterator *iter, व्योम *context,
             स्थिर UChar **pString,
             int32_t caseLocale);

U_CDECL_END

/* for icuexportdata -------------------------------------------------------- */

काष्ठा UCaseProps {
    व्योम *mem;  // TODO: was unused, and type UDataMemory -- remove
    स्थिर int32_t *indexes;
    स्थिर uint16_t *exceptions;
    स्थिर uint16_t *unfold;

    UTrie2 trie;
    uint8_t formatVersion[4];
};

U_CAPI स्थिर काष्ठा UCaseProps * U_EXPORT2
ucase_getSingleton(int32_t *p_Exceptions_लंबाई, int32_t *p_Unfold_लंबाई);

/* file definitions --------------------------------------------------------- */

#घोषणा UCASE_तथ्य_NAME "ucase"
#घोषणा UCASE_तथ्य_TYPE "icu"

/* format "cAsE" */
#घोषणा UCASE_FMT_0 0x63
#घोषणा UCASE_FMT_1 0x41
#घोषणा UCASE_FMT_2 0x53
#घोषणा UCASE_FMT_3 0x45

/* indexes into indexes[] */
क्रमागत {
    UCASE_IX_सूचकांक_शीर्ष,
    UCASE_IX_लंबाई,
    UCASE_IX_TRIE_आकार,
    UCASE_IX_EXC_लंबाई,
    UCASE_IX_UNFOLD_लंबाई,

    UCASE_IX_MAX_FULL_लंबाई=15,
    UCASE_IX_शीर्ष=16
};

/* definitions for 16-bit case properties word ------------------------------ */

U_CFUNC स्थिर UTrie2 * U_EXPORT2
ucase_getTrie(व्योम);

/* 2-bit constants for types of cased characters */
#घोषणा UCASE_TYPE_MASK     3
क्रमागत {
    UCASE_NONE,
    UCASE_LOWER,
    UCASE_UPPER,
    UCASE_TITLE
};

#घोषणा UCASE_GET_TYPE(props) ((props)&UCASE_TYPE_MASK)
#घोषणा UCASE_GET_TYPE_AND_IGNORABLE(props) ((props)&7)

#घोषणा UCASE_IS_UPPER_OR_TITLE(props) ((props)&2)

#घोषणा UCASE_IGNORABLE         4
#घोषणा UCASE_EXCEPTION         8
#घोषणा UCASE_SENSITIVE         0x10

#घोषणा UCASE_HAS_EXCEPTION(props) ((props)&UCASE_EXCEPTION)

#घोषणा UCASE_DOT_MASK      0x60
क्रमागत {
    UCASE_NO_DOT=0,         /* normal characters with cc=0 */
    UCASE_SOFT_DOTTED=0x20, /* soft-dotted characters with cc=0 */
    UCASE_ABOVE=0x40,       /* "above" accents with cc=230 */
    UCASE_OTHER_ACCENT=0x60 /* other accent character (0<cc!=230) */
};

/* no exception: bits 15..7 are a 9-bit signed case mapping delta */
#घोषणा UCASE_DELTA_SHIFT   7
#घोषणा UCASE_DELTA_MASK    0xff80
#घोषणा UCASE_MAX_DELTA     0xff
#घोषणा UCASE_MIN_DELTA     (-UCASE_MAX_DELTA-1)

#अगर U_SIGNED_दायाँ_SHIFT_IS_ARITHMETIC
#   define UCASE_GET_DELTA(props) ((int16_t)(props)>>UCASE_DELTA_SHIFT)
#अन्यथा
#   define UCASE_GET_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UCASE_DELTA_SHIFT)|0xfe00) : ((uint16_t)(props)>>UCASE_DELTA_SHIFT))
#पूर्ण_अगर

/* exception: bits 15..4 are an unsigned 12-bit index into the exceptions array */
#घोषणा UCASE_EXC_SHIFT     4
#घोषणा UCASE_EXC_MASK      0xfff0
#घोषणा UCASE_MAX_EXCEPTIONS ((UCASE_EXC_MASK>>UCASE_EXC_SHIFT)+1)

/* definitions for 16-bit main exceptions word ------------------------------ */

/* first 8 bits indicate values in optional slots */
क्रमागत {
    UCASE_EXC_LOWER,
    UCASE_EXC_FOLD,
    UCASE_EXC_UPPER,
    UCASE_EXC_TITLE,
    UCASE_EXC_DELTA,
    UCASE_EXC_5,            /* reserved */
    UCASE_EXC_CLOSURE,
    UCASE_EXC_FULL_MAPPINGS,
    UCASE_EXC_ALL_SLOTS     /* one past the last slot */
};

/* each slot is 2 uint16_t instead of 1 */
#घोषणा UCASE_EXC_DOUBLE_SLOTS      0x100

क्रमागत {
    UCASE_EXC_NO_SIMPLE_CASE_FOLDING=0x200,
    UCASE_EXC_DELTA_IS_NEGATIVE=0x400,
    UCASE_EXC_SENSITIVE=0x800
};

/* UCASE_EXC_DOT_MASK=UCASE_DOT_MASK<<UCASE_EXC_DOT_SHIFT */
#घोषणा UCASE_EXC_DOT_SHIFT     7

/* normally stored in the main word, but pushed out for larger exception indexes */
#घोषणा UCASE_EXC_DOT_MASK      0x3000
क्रमागत {
    UCASE_EXC_NO_DOT=0,
    UCASE_EXC_SOFT_DOTTED=0x1000,
    UCASE_EXC_ABOVE=0x2000,         /* "above" accents with cc=230 */
    UCASE_EXC_OTHER_ACCENT=0x3000   /* other character (0<cc!=230) */
};

/* complex/conditional mappings */
#घोषणा UCASE_EXC_CONDITIONAL_SPECIAL   0x4000
#घोषणा UCASE_EXC_CONDITIONAL_FOLD      0x8000

/* definitions for lengths word for full case mappings */
#घोषणा UCASE_FULL_LOWER    0xf
#घोषणा UCASE_FULL_FOLDING  0xf0
#घोषणा UCASE_FULL_UPPER    0xf00
#घोषणा UCASE_FULL_TITLE    0xf000

/* maximum lengths */
#घोषणा UCASE_FULL_MAPPINGS_MAX_लंबाई (4*0xf)
#घोषणा UCASE_CLOSURE_MAX_लंबाई 0xf

/* constants for reverse case folding ("unfold") data */
क्रमागत {
    UCASE_UNFOLD_ROWS,
    UCASE_UNFOLD_ROW_WIDTH,
    UCASE_UNFOLD_STRING_WIDTH
};

#पूर्ण_अगर
