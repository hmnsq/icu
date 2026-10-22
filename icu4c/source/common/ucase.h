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

#جيڪڏهن_وصف_نه_ٿيل __UCASE_H__
#وصف_ڪريو __UCASE_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"
#ضمن "putilimp.h"
#ضمن "uset_imp.h"
#ضمن "udataswp.h"
#ضمن "utrie2.h"

#جيڪڏهن_وصف_ٿيل __cplusplus
U_NAMESPACE_BEGIN

class UnicodeString;

U_NAMESPACE_پڇاڙي
#جيڪڏهن_پورو

/* library API -------------------------------------------------------------- */

U_CFUNC فراغ U_EXPORT2
ucase_جوڙيو_Property_Starts(ثابت USetAdder *sa, UError_ڪوڊ *p_Error_ڪوڊ);

/**
 * Requires non-NULL locale ID but otherwise does the equivalent of
 * checking for language codes as if uloc_getLanguage() were called:
 * Accepts both 2- and 3-letter codes and accepts case variants.
 */
U_CFUNC int32_t
ucase_getCaseLocale(ثابت حرف *locale);

/* Casing locale types for ucase_getCaseLocale */
تعداد {
    UCASE_LOC_UNKNOWN,
    UCASE_LOC_پاڙ,
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
#وصف_ڪريو STRCASECMP_OPTIONS_نقاب 0xffff

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
#وصف_ڪريو FOLD_CASE_OPTIONS_نقاب 7

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
U_CFUNC فراغ U_EXPORT2
ucase_جوڙيو_Case_گهيرو(UChar32 c, ثابت USetAdder *sa);

/** Case closure with only scf=Simple_Case_Folding. */
U_CFUNC فراغ U_EXPORT2
ucase_جوڙيو_Simple_Case_گهيرو(UChar32 c, ثابت USetAdder *sa);

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
ucase_جوڙيو_String_Case_گهيرو(ثابت UChar *s, int32_t ڊيگهائي, ثابت USetAdder *sa);

#جيڪڏهن_وصف_ٿيل __cplusplus
U_NAMESPACE_BEGIN

/**
 * Iterator over characters with more than one code point in the full default Case_Folding.
 */
class U_COMMON_API ڀريل_Case_Folding_Iterator {
عام:
    /** Constructor. */
    ڀريل_Case_Folding_Iterator();
    /**
     * Returns the next (cp, full) pair where "full" is cp's full default Case_Folding.
     * Returns a negative cp value at the end of the iteration.
     */
    UChar32 ايندڙ(UnicodeString &ڀريل);
خاص:
    ڀريل_Case_Folding_Iterator(ثابت ڀريل_Case_Folding_Iterator &) = delete;  // no copy
    ڀريل_Case_Folding_Iterator &عامل=(ثابت ڀريل_Case_Folding_Iterator &) = delete;  // no assignment

    ثابت char16_t *unfold;
    int32_t unfold_صفون;
    int32_t unfold_صف_ويڪر;
    int32_t unfold_String_ويڪر;
    int32_t current_صف;
    int32_t صف_Cp_اشاريو;
};

/**
 * Fast case mapping data for ASCII/Latin.
 * Linear arrays of delta bytes: 0=no mapping; EXC=exception.
 * Deltas must not cross the ASCII boundary, or else they cannot be easily used
 * in simple UTF-8 code.
 */
namespace LatinCase {

/** Case mapping/folding data for code points up to U+017F. */
constexpr char16_t حد_بندي_2 = 0x180;
/** U+017F case-folds and uppercases crossing the ASCII boundary. */
constexpr char16_t LONG_S = 0x17f;
/** Exception: Complex mapping, or too-large delta. */
constexpr int8_t EXC = -0x80;

/** Deltas for lowercasing for most locales, and default case folding. */
خارجي ثابت int8_t منزل_هيٺاهون_عام[حد_بندي_2];
/** Deltas for lowercasing for tr/az/lt, and Turkic case folding. */
خارجي ثابت int8_t منزل_هيٺاهون_TR_LT[حد_بندي_2];

/** Deltas for uppercasing for most locales. */
خارجي ثابت int8_t منزل_مٿانهون_عام[حد_بندي_2];
/** Deltas for uppercasing for tr/az. */
خارجي ثابت int8_t منزل_مٿانهون_TR[حد_بندي_2];

}  // namespace LatinCase

U_NAMESPACE_پڇاڙي
#جيڪڏهن_پورو

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
تعريفنوع UChar32 U_CALLCONV
UCaseContextIterator(فراغ *context, int8_t dir);

/**
 * Sample struct which may be used by some implementations of
 * UCaseContextIterator.
 */
بنية UCaseContext {
    فراغ *p;
    int32_t شروعات, اشاريو, حد_بندي;
    int32_t cp_شروعات, cp_حد_بندي;
    int8_t dir;
    int8_t b1, b2, b3;
};
تعريفنوع بنية UCaseContext UCaseContext;

U_CDECL_پڇاڙي

#وصف_ڪريو UCASECONTEXT_INITIALIZER { عدم,  0, 0, 0,  0, 0,  0,  0, 0, 0 }

تعداد {
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
    UCASE_MAX_STRING_ڊيگهائي=0x1f
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
ucase_منزل_ڀريل_هيٺاهون(UChar32 c,
                  UCaseContextIterator *iter, فراغ *context,
                  ثابت UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_منزل_ڀريل_مٿانهون(UChar32 c,
                  UCaseContextIterator *iter, فراغ *context,
                  ثابت UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_منزل_ڀريل_Title(UChar32 c,
                  UCaseContextIterator *iter, فراغ *context,
                  ثابت UChar **pString,
                  int32_t caseLocale);

U_CAPI int32_t U_EXPORT2
ucase_منزل_ڀريل_Folding(UChar32 c,
                    ثابت UChar **pString,
                    uint32_t options);

U_CFUNC int32_t U_EXPORT2
ucase_has_ٻٽو_Property(UChar32 c, UProperty which);


U_CDECL_BEGIN

/**
 * @internal
 */
تعريفنوع int32_t U_CALLCONV
UCase_Map_ڀريل(UChar32 c,
             UCaseContextIterator *iter, فراغ *context,
             ثابت UChar **pString,
             int32_t caseLocale);

U_CDECL_پڇاڙي

/* for icuexportdata -------------------------------------------------------- */

بنية UCaseProps {
    فراغ *mem;  // TODO: was unused, and type UDataMemory -- remove
    ثابت int32_t *indexes;
    ثابت uint16_t *exceptions;
    ثابت uint16_t *unfold;

    UTrie2 trie;
    uint8_t format_نسخو[4];
};

U_CAPI ثابت بنية UCaseProps * U_EXPORT2
ucase_getSingleton(int32_t *p_Exceptions_ڊيگهائي, int32_t *p_Unfold_ڊيگهائي);

/* file definitions --------------------------------------------------------- */

#وصف_ڪريو UCASE_معلومات_NAME "ucase"
#وصف_ڪريو UCASE_معلومات_TYPE "icu"

/* format "cAsE" */
#وصف_ڪريو UCASE_FMT_0 0x63
#وصف_ڪريو UCASE_FMT_1 0x41
#وصف_ڪريو UCASE_FMT_2 0x53
#وصف_ڪريو UCASE_FMT_3 0x45

/* indexes into indexes[] */
تعداد {
    UCASE_IX_اشاريو_چوٽي,
    UCASE_IX_ڊيگهائي,
    UCASE_IX_TRIE_سائيز,
    UCASE_IX_EXC_ڊيگهائي,
    UCASE_IX_UNFOLD_ڊيگهائي,

    UCASE_IX_MAX_ڀريل_ڊيگهائي=15,
    UCASE_IX_چوٽي=16
};

/* definitions for 16-bit case properties word ------------------------------ */

U_CFUNC ثابت UTrie2 * U_EXPORT2
ucase_getTrie(فراغ);

/* 2-bit constants for types of cased characters */
#وصف_ڪريو UCASE_TYPE_نقاب     3
تعداد {
    UCASE_NONE,
    UCASE_هيٺاهون,
    UCASE_مٿانهون,
    UCASE_TITLE
};

#وصف_ڪريو UCASE_GET_TYPE(props) ((props)&UCASE_TYPE_نقاب)
#وصف_ڪريو UCASE_GET_TYPE_AND_IGNORABLE(props) ((props)&7)

#وصف_ڪريو UCASE_IS_مٿانهون_OR_TITLE(props) ((props)&2)

#وصف_ڪريو UCASE_IGNORABLE         4
#وصف_ڪريو UCASE_EXCEPTION         8
#وصف_ڪريو UCASE_SENSITIVE         0x10

#وصف_ڪريو UCASE_HAS_EXCEPTION(props) ((props)&UCASE_EXCEPTION)

#وصف_ڪريو UCASE_DOT_نقاب      0x60
تعداد {
    UCASE_NO_DOT=0,         /* normal characters with cc=0 */
    UCASE_SOFT_DOTTED=0x20, /* soft-dotted characters with cc=0 */
    UCASE_ABOVE=0x40,       /* "above" accents with cc=230 */
    UCASE_OTHER_ACCENT=0x60 /* other accent character (0<cc!=230) */
};

/* no exception: bits 15..7 are a 9-bit signed case mapping delta */
#وصف_ڪريو UCASE_DELTA_سرڪايو   7
#وصف_ڪريو UCASE_DELTA_نقاب    0xff80
#وصف_ڪريو UCASE_MAX_DELTA     0xff
#وصف_ڪريو UCASE_MIN_DELTA     (-UCASE_MAX_DELTA-1)

#إذا U_SIGNED_ساڄو_سرڪايو_IS_ARITHMETIC
#   define UCASE_GET_DELTA(props) ((int16_t)(props)>>UCASE_DELTA_سرڪايو)
#وإلا
#   define UCASE_GET_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UCASE_DELTA_سرڪايو)|0xfe00) : ((uint16_t)(props)>>UCASE_DELTA_سرڪايو))
#جيڪڏهن_پورو

/* exception: bits 15..4 are an unsigned 12-bit index into the exceptions array */
#وصف_ڪريو UCASE_EXC_سرڪايو     4
#وصف_ڪريو UCASE_EXC_نقاب      0xfff0
#وصف_ڪريو UCASE_MAX_EXCEPTIONS ((UCASE_EXC_نقاب>>UCASE_EXC_سرڪايو)+1)

/* definitions for 16-bit main exceptions word ------------------------------ */

/* first 8 bits indicate values in optional slots */
تعداد {
    UCASE_EXC_هيٺاهون,
    UCASE_EXC_FOLD,
    UCASE_EXC_مٿانهون,
    UCASE_EXC_TITLE,
    UCASE_EXC_DELTA,
    UCASE_EXC_5,            /* reserved */
    UCASE_EXC_گهيرو,
    UCASE_EXC_ڀريل_MAPPINGS,
    UCASE_EXC_ALL_SLOTS     /* one past the last slot */
};

/* each slot is 2 uint16_t instead of 1 */
#وصف_ڪريو UCASE_EXC_DOUBLE_SLOTS      0x100

تعداد {
    UCASE_EXC_NO_SIMPLE_CASE_FOLDING=0x200,
    UCASE_EXC_DELTA_IS_NEGATIVE=0x400,
    UCASE_EXC_SENSITIVE=0x800
};

/* UCASE_EXC_DOT_MASK=UCASE_DOT_MASK<<UCASE_EXC_DOT_SHIFT */
#وصف_ڪريو UCASE_EXC_DOT_سرڪايو     7

/* normally stored in the main word, but pushed out for larger exception indexes */
#وصف_ڪريو UCASE_EXC_DOT_نقاب      0x3000
تعداد {
    UCASE_EXC_NO_DOT=0,
    UCASE_EXC_SOFT_DOTTED=0x1000,
    UCASE_EXC_ABOVE=0x2000,         /* "above" accents with cc=230 */
    UCASE_EXC_OTHER_ACCENT=0x3000   /* other character (0<cc!=230) */
};

/* complex/conditional mappings */
#وصف_ڪريو UCASE_EXC_CONDITIONAL_SPECIAL   0x4000
#وصف_ڪريو UCASE_EXC_CONDITIONAL_FOLD      0x8000

/* definitions for lengths word for full case mappings */
#وصف_ڪريو UCASE_ڀريل_هيٺاهون    0xf
#وصف_ڪريو UCASE_ڀريل_FOLDING  0xf0
#وصف_ڪريو UCASE_ڀريل_مٿانهون    0xf00
#وصف_ڪريو UCASE_ڀريل_TITLE    0xf000

/* maximum lengths */
#وصف_ڪريو UCASE_ڀريل_MAPPINGS_MAX_ڊيگهائي (4*0xf)
#وصف_ڪريو UCASE_گهيرو_MAX_ڊيگهائي 0xf

/* constants for reverse case folding ("unfold") data */
تعداد {
    UCASE_UNFOLD_صفون,
    UCASE_UNFOLD_صف_ويڪر,
    UCASE_UNFOLD_STRING_ويڪر
};

#جيڪڏهن_پورو
