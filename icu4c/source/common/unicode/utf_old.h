// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  utf_old.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002sep21
*   created by: Markus W. Scherer
*/

/**
 * \file
 * \brief C API: Deprecated macros for Unicode string handling
 *
 * The macros in utf_old.h are all deprecated and their use discouraged.
 * Some of the design principles behind the set of UTF macros
 * have changed or proved impractical.
 * Almost all of the old "UTF macros" are at least renamed.
 * If you are looking for a new equivalent to an old macro, please see the
 * comment at the old one.
 *
 * Brief summary of reasons for deprecation:
 * - Switch on UTF_SIZE (selection of UTF-8/16/32 default string processing)
 *   was impractical.
 * - Switch on UTF_SAFE etc. (selection of unsafe/safe/strict default string processing)
 *   was of little use and impractical.
 * - Whole classes of macros became obsolete outside of the UTF_SIZE/UTF_SAFE
 *   selection framework: UTF32_ macros (all trivial)
 *   and UTF_ default and intermediate macros (all aliases).
 * - The selection framework also caused many macro aliases.
 * - Change in Unicode standard: "irregular" sequences (3.0) became illegal (3.2).
 * - Change of language in Unicode standard:
 *   Growing distinction between internal x-bit Unicode strings and external UTF-x
 *   forms, with the former more lenient.
 *   Suggests renaming of UTF16_ macros to U16_.
 * - The prefix "UTF_" without a width number confused some users.
 * - "Safe" append macros needed the addition of an error indicator output.
 * - "Safe" UTF-8 macros used legitimate (if rarely used) code point values
 *   to indicate error conditions.
 * - The use of the "_CHAR" infix for code point operations confused some users.
 *
 * More details:
 *
 * Until ICU 2.2, utf.h theoretically allowed to choose among UTF-8/16/32
 * for string processing, and among unsafe/safe/strict default macros for that.
 *
 * It proved nearly impossible to write non-trivial, high-performance code
 * that is UTF-generic.
 * Unsafe default macros would be dangerous for default string processing,
 * and the main reason for the "strict" versions disappeared:
 * Between Unicode 3.0 and 3.2 all "irregular" UTF-8 sequences became illegal.
 * The only other conditions that "strict" checked for were non-characters,
 * which are valid during processing. Only during text input/output should they
 * be checked, and at that time other well-formedness checks may be
 * necessary or useful as well.
 * This can still be done by using U16_NEXT and U_IS_UNICODE_NONCHAR
 * or U_IS_UNICODE_CHAR.
 *
 * The old UTF8_..._SAFE macros also used some normal Unicode code points
 * to indicate malformed sequences.
 * The new UTF8_ macros without suffix use negative values instead.
 *
 * The entire contents of utf32.h was moved here without replacement
 * because all those macros were trivial and
 * were meaningful only in the framework of choosing the UTF size.
 *
 * See Jitterbug 2150 and its discussion on the ICU mailing list
 * in September 2002.
 *
 * <hr>
 *
 * <em>Obsolete part</em> of pre-ICU 2.4 utf.h file documentation:
 *
 * <p>The original concept for these files was for ICU to allow
 * in principle to set which UTF (UTF-8/16/32) is used internally
 * by defining UTF_SIZE to either 8, 16, or 32. utf.h would then define the UChar type
 * accordingly. UTF-16 was the default.</p>
 *
 * <p>This concept has been abandoned.
 * A lot of the ICU source code assumes UChar strings are in UTF-16.
 * This is especially true for low-level code like
 * conversion, normalization, and collation.
 * The utf.h header enforces the default of UTF-16.
 * The UTF-8 and UTF-32 macros remain for now for completeness and backward compatibility.</p>
 *
 * <p>Accordingly, utf.h defines UChar to be an unsigned 16-bit integer. If this matches wchar_t, then
 * UChar is defined to be exactly wchar_t, otherwise uint16_t.</p>
 *
 * <p>UChar32 is defined to be a signed 32-bit integer (int32_t), large enough for a 21-bit
 * Unicode code point (Unicode scalar value, 0..0x10ffff).
 * Before ICU 2.4, the definition of UChar32 was similarly platform-dependent as
 * the definition of UChar. For details see the documentation for UChar32 itself.</p>
 *
 * <p>utf.h also defines a number of C macros for handling single Unicode code points and
 * for using UTF Unicode strings. It includes utf8.h, utf16.h, and utf32.h for the actual
 * implementations of those macros and then aliases one set of them (for UTF-16) for general use.
 * The UTF-specific macros have the UTF size in the macro name prefixes (UTF16_...), while
 * the general alias macros always begin with UTF_...</p>
 *
 * <p>Many string operations can be done with or without error checking.
 * Where such a distinction is useful, there are two versions of the macros, "unsafe" and "safe"
 * ones with ..._UNSAFE and ..._SAFE suffixes. The unsafe macros are fast but may cause
 * program failures if the strings are not well-formed. The safe macros have an additional, boolean
 * parameter "strict". If strict is false, then only illegal sequences are detected.
 * Otherwise, irregular sequences and non-characters are detected as well (like single surrogates).
 * Safe macros return special error code points for illegal/irregular sequences:
 * Typically, U+ffff, or values that would result in a code unit sequence of the same length
 * as the erroneous input sequence.<br>
 * Note that _UNSAFE macros have fewer parameters: They do not have the strictness parameter, and
 * they do not have start/length parameters for boundary checking.</p>
 *
 * <p>Here, the macros are aliased in two steps:
 * In the first step, the UTF-specific macros with UTF16_ prefix and _UNSAFE and _SAFE suffixes are
 * aliased according to the UTF_SIZE to macros with UTF_ prefix and the same suffixes and signatures.
 * Then, in a second step, the default, general alias macros are set to use either the unsafe or
 * the safe/not strict (default) or the safe/strict macro;
 * these general macros do not have a strictness parameter.</p>
 *
 * <p>It is possible to change the default choice for the general alias macros to be unsafe, safe/not strict or safe/strict.
 * The default is safe/not strict. It is not recommended to select the unsafe macros as the basis for
 * Unicode string handling in ICU! To select this, define UTF_SAFE, UTF_STRICT, or UTF_UNSAFE.</p>
 *
 * <p>For general use, one should use the default, general macros with UTF_ prefix and no _SAFE/_UNSAFE suffix.
 * Only in some cases it may be necessary to control the choice of macro directly and use a less generic alias.
 * For example, if it can be assumed that a string is well-formed and the index will stay within the bounds,
 * then the _UNSAFE version may be used.
 * If a UTF-8 string is to be processed, then the macros with UTF8_ prefixes need to be used.</p>
 *
 * <hr>
 *
 * Deprecated ICU 2.4. Use the macros in utf.h, utf16.h, utf8.h instead.
 */

#اگر_تعریف_نشده __UTF_OLD_H__
#تعریف_کن __UTF_OLD_H__

#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"

/**
 * \def U_HIDE_OBSOLETE_UTF_OLD_H
 *
 * Hides the obsolete definitions in unicode/utf_old.h.
 * Recommended to be set to 1 at compile time to make sure
 * the long-deprecated macros are no longer used.
 *
 * For reasons for the deprecation see the utf_old.h file comments.
 *
 * @internal
 */
#اگر_تعریف_نشده U_HIDE_OBSOLETE_UTF_OLD_H
#   define U_HIDE_OBSOLETE_UTF_OLD_H 0
#پایان_اگر

#إذا !defined(U_HIDE_DEPRECATED_API) && !U_HIDE_OBSOLETE_UTF_OLD_H

/* Formerly utf.h, part 1 --------------------------------------------------- */

#اگر_تعریف_شده U_USE_UTF_DEPRECATES
/**
 * Unicode string and array offset and index type.
 * ICU always counts Unicode code units (UChars) for
 * string offsets, indexes, and lengths, not Unicode code points.
 *
 * @obsolete ICU 2.6. Use int32_t directly instead since this API will be removed in that release.
 */
تعريفنوع int32_t UText_جابه_جایی;
#پایان_اگر

/** Number of bits in a Unicode string code unit - ICU uses 16-bit Unicode. @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF_اندازه 16

/**
 * The default choice for general Unicode string macros is to use the ..._SAFE macro implementations
 * with strict=false.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#تعریف_کن UTF_امن
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#لغو_تعریف UTF_UNSAFE
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#لغو_تعریف UTF_STRICT

/**
 * UTF8_ERROR_VALUE_1 and UTF8_ERROR_VALUE_2 are special error values for UTF-8,
 * which need 1 or 2 bytes in UTF-8:
 * \code
 * U+0015 = NAK = Negative Acknowledge, C0 control character
 * U+009f = highest C1 control character
 * \endcode
 *
 * These are used by UTF8_..._SAFE macros so that they can return an error value
 * that needs the same number of code units (bytes) as were seen by
 * a macro. They should be tested with UTF_IS_ERROR() or UTF_IS_VALID().
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#تعریف_کن UTF8_ERROR_مقدار_1 0x15

/**
 * See documentation on UTF8_ERROR_VALUE_1 for details.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#تعریف_کن UTF8_ERROR_مقدار_2 0x9f

/**
 * Error value for all UTFs. This code point value will be set by macros with error
 * checking if an error is detected.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#تعریف_کن UTF_ERROR_مقدار 0xffff

/**
 * Is a given 32-bit code an error value
 * as returned by one of the macros for any UTF?
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#تعریف_کن UTF_IS_ERROR(c) \
    (((c)&0xfffe)==0xfffe || (c)==UTF8_ERROR_مقدار_1 || (c)==UTF8_ERROR_مقدار_2)

/**
 * This is a combined macro: Is c a valid Unicode value _and_ not an error code?
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#تعریف_کن UTF_IS_VALID(c) \
    (UTF_IS_UNICODE_CHAR(c) && \
     (c)!=UTF8_ERROR_مقدار_1 && (c)!=UTF8_ERROR_مقدار_2)

/**
 * Is this code unit or code point a surrogate (U+d800..U+dfff)?
 * @deprecated ICU 2.4. Renamed to U_IS_SURROGATE and U16_IS_SURROGATE, see utf_old.h.
 */
#تعریف_کن UTF_IS_SURROGATE(uchar) (((uchar)&0xfffff800)==0xd800)

/**
 * Is a given 32-bit code point a Unicode noncharacter?
 *
 * @deprecated ICU 2.4. Renamed to U_IS_UNICODE_NONCHAR, see utf_old.h.
 */
#تعریف_کن UTF_IS_UNICODE_NONCHAR(c) \
    ((c)>=0xfdd0 && \
     ((uint32_t)(c)<=0xfdef || ((c)&0xfffe)==0xfffe) && \
     (uint32_t)(c)<=0x10ffff)

/**
 * Is a given 32-bit value a Unicode code point value (0..U+10ffff)
 * that can be assigned a character?
 *
 * Code points that are not characters include:
 * - single surrogate code points (U+d800..U+dfff, 2048 code points)
 * - the last two code points on each plane (U+__fffe and U+__ffff, 34 code points)
 * - U+fdd0..U+fdef (new with Unicode 3.1, 32 code points)
 * - the highest Unicode code point value is U+10ffff
 *
 * This means that all code points below U+d800 are character code points,
 * and that boundary is tested first for performance.
 *
 * @deprecated ICU 2.4. Renamed to U_IS_UNICODE_CHAR, see utf_old.h.
 */
#تعریف_کن UTF_IS_UNICODE_CHAR(c) \
    ((uint32_t)(c)<0xd800 || \
        ((uint32_t)(c)>0xdfff && \
         (uint32_t)(c)<=0x10ffff && \
         !UTF_IS_UNICODE_NONCHAR(c)))

/* Formerly utf8.h ---------------------------------------------------------- */

/**
* \var utf8_countTrailBytes
* Internal array with numbers of trail bytes for any given byte used in
* lead byte position.
*
* This is internal since it is not meant to be called directly by external clients;
* however it is called by public macros in this file and thus must remain stable,
* and should not be hidden when other internal functions are hidden (otherwise
* public macros would fail to compile).
* @internal
*/
#اگر_تعریف_شده U_UTF8_IMPL
// No forward declaration if compiling utf_impl.cpp, which defines utf8_countTrailBytes.
#وإلاإذا defined(U_STATIC_IMPLEMENTATION) || defined(U_COMMON_IMPLEMENTATION)
U_CAPI ثابت uint8_t utf8_شمار_Trail_بایتها[];
#وإلا
U_CFUNC U_IMPORT ثابت uint8_t utf8_شمار_Trail_بایتها[];
#پایان_اگر 

/**
 * Count the trail bytes for a UTF-8 lead byte.
 * @deprecated ICU 2.4. Renamed to U8_COUNT_TRAIL_BYTES, see utf_old.h.
 */
#تعریف_کن UTF8_شمار_TRAIL_بایتها(leadByte) (utf8_شمار_Trail_بایتها[(uint8_t)leadByte])

/**
 * Mask a UTF-8 lead byte, leave only the lower bits that form part of the code point value.
 * @deprecated ICU 2.4. Renamed to U8_MASK_LEAD_BYTE, see utf_old.h.
 */
#تعریف_کن UTF8_ماسک_LEAD_BYTE(leadByte, شمار_Trail_بایتها) ((leadByte)&=(1<<(6-(شمار_Trail_بایتها)))-1)

/** Is this this code point a single code unit (byte)? @deprecated ICU 2.4. Renamed to U8_IS_SINGLE, see utf_old.h. */
#تعریف_کن UTF8_IS_SINGLE(uchar) (((uchar)&0x80)==0)
/** Is this this code unit the lead code unit (byte) of a code point? @deprecated ICU 2.4. Renamed to U8_IS_LEAD, see utf_old.h. */
#تعریف_کن UTF8_IS_LEAD(uchar) ((uint8_t)((uchar)-0xc0)<0x3e)
/** Is this this code unit a trailing code unit (byte) of a code point? @deprecated ICU 2.4. Renamed to U8_IS_TRAIL, see utf_old.h. */
#تعریف_کن UTF8_IS_TRAIL(uchar) (((uchar)&0xc0)==0x80)

/** Does this scalar Unicode value need multiple code units for storage? @deprecated ICU 2.4. Use U8_LENGTH or test ((uint32_t)(c)>0x7f) instead, see utf_old.h. */
#تعریف_کن UTF8_NEED_مضرب_UCHAR(c) ((uint32_t)(c)>0x7f)

/**
 * Given the lead character, how many bytes are taken by this code point.
 * ICU does not deal with code points >0x10ffff
 * unless necessary for advancing in the byte stream.
 *
 * These length macros take into account that for values >0x10ffff
 * the UTF8_APPEND_CHAR_SAFE macros would write the error code point 0xffff
 * with 3 bytes.
 * Code point comparisons need to be in uint32_t because UChar32
 * may be a signed type, and negative values must be recognized.
 *
 * @deprecated ICU 2.4. Use U8_LENGTH instead, see utf.h.
 */
#إذا 1
#   define UTF8_CHAR_درازی(c) \
        ((uint32_t)(c)<=0x7f ? 1 : \
            ((uint32_t)(c)<=0x7ff ? 2 : \
                ((uint32_t)((c)-0x10000)>0xfffff ? 3 : 4) \
            ) \
        )
#وإلا
#   define UTF8_CHAR_درازی(c) \
        ((uint32_t)(c)<=0x7f ? 1 : \
            ((uint32_t)(c)<=0x7ff ? 2 : \
                ((uint32_t)(c)<=0xffff ? 3 : \
                    ((uint32_t)(c)<=0x10ffff ? 4 : \
                        ((uint32_t)(c)<=0x3ffffff ? 5 : \
                            ((uint32_t)(c)<=0x7fffffff ? 6 : 3) \
                        ) \
                    ) \
                ) \
            ) \
        )
#پایان_اگر

/** The maximum number of bytes per code point. @deprecated ICU 2.4. Renamed to U8_MAX_LENGTH, see utf_old.h. */
#تعریف_کن UTF8_MAX_CHAR_درازی 4

/** Average number of code units compared to UTF-16. @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF8_ARRAY_اندازه(اندازه_2) ((5*(اندازه_2))/2)

/** @deprecated ICU 2.4. Renamed to U8_GET_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_GET_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    int32_t utf8_get_char_unsafe_شاخص=(int32_t)(i); \
    UTF8_مجموعه_CHAR_شروع_UNSAFE(s, utf8_get_char_unsafe_شاخص); \
    UTF8_بعدی_CHAR_UNSAFE(s, utf8_get_char_unsafe_شاخص, c); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Use U8_GET instead, see utf_old.h. */
#تعریف_کن UTF8_GET_CHAR_امن(s, شروع, i, درازی, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    int32_t utf8_get_char_امن_شاخص=(int32_t)(i); \
    UTF8_مجموعه_CHAR_شروع_امن(s, شروع, utf8_get_char_امن_شاخص); \
    UTF8_بعدی_CHAR_امن(s, utf8_get_char_امن_شاخص, درازی, c, strict); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_NEXT_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_بعدی_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا((uint8_t)((c)-0xc0)<0x35) { \
        uint8_t __count=UTF8_شمار_TRAIL_بایتها(c); \
        UTF8_ماسک_LEAD_BYTE(c, __count); \
        بدل(__count) { \
        /* each following branch falls through to the next one */ \
        حالة 3: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
            U_FALLTHROUGH; \
        حالة 2: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
            U_FALLTHROUGH; \
        حالة 1: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
        /* no other branches to optimize switch() */ \
            اكسر; \
        } \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_APPEND_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_APPEND_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0x7f) { \
        (s)[(i)++]=(uint8_t)(c); \
    } وإلا { \
        إذا((uint32_t)(c)<=0x7ff) { \
            (s)[(i)++]=(uint8_t)(((c)>>6)|0xc0); \
        } وإلا { \
            إذا((uint32_t)(c)<=0xffff) { \
                (s)[(i)++]=(uint8_t)(((c)>>12)|0xe0); \
            } وإلا { \
                (s)[(i)++]=(uint8_t)(((c)>>18)|0xf0); \
                (s)[(i)++]=(uint8_t)((((c)>>12)&0x3f)|0x80); \
            } \
            (s)[(i)++]=(uint8_t)((((c)>>6)&0x3f)|0x80); \
        } \
        (s)[(i)++]=(uint8_t)(((c)&0x3f)|0x80); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_FWD_1_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_FWD_1_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    (i)+=1+UTF8_شمار_TRAIL_بایتها((s)[i]); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_FWD_N_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_FWD_N_UNSAFE(s, i, n) UPRV_بلوک_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        UTF8_FWD_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_SET_CP_START_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_مجموعه_CHAR_شروع_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    طالما(UTF8_IS_TRAIL((s)[i])) { --(i); } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Use U8_NEXT instead, see utf_old.h. */
#تعریف_کن UTF8_بعدی_CHAR_امن(s, i, درازی, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا((c)>=0x80) { \
        إذا(UTF8_IS_LEAD(c)) { \
            (c)=utf8_بعدی_Char_امن_Body(s, &(i), (int32_t)(درازی), c, strict); \
        } وإلا { \
            (c)=UTF8_ERROR_مقدار_1; \
        } \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Use U8_APPEND instead, see utf_old.h. */
#تعریف_کن UTF8_APPEND_CHAR_امن(s, i, درازی, c)  UPRV_بلوک_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0x7f) { \
        (s)[(i)++]=(uint8_t)(c); \
    } وإلا { \
        (i)=utf8_append_Char_امن_Body(s, (int32_t)(i), (int32_t)(درازی), c, عدم); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_FWD_1, see utf_old.h. */
#تعریف_کن UTF8_FWD_1_امن(s, i, درازی) U8_FWD_1(s, i, درازی)

/** @deprecated ICU 2.4. Renamed to U8_FWD_N, see utf_old.h. */
#تعریف_کن UTF8_FWD_N_امن(s, i, درازی, n) U8_FWD_N(s, i, درازی, n)

/** @deprecated ICU 2.4. Renamed to U8_SET_CP_START, see utf_old.h. */
#تعریف_کن UTF8_مجموعه_CHAR_شروع_امن(s, شروع, i) U8_مجموعه_CP_شروع(s, شروع, i)

/** @deprecated ICU 2.4. Renamed to U8_PREV_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_PREV_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا(UTF8_IS_TRAIL(c)) { \
        uint8_t __b, __count=1, __shift=6; \
\
        /* c is a trail byte */ \
        (c)&=0x3f; \
        لكل(;;) { \
            __b=(s)[--(i)]; \
            إذا(__b>=0xc0) { \
                UTF8_ماسک_LEAD_BYTE(__b, __count); \
                (c)|=(UChar32)__b<<__shift; \
                اكسر; \
            } وإلا { \
                (c)|=(UChar32)(__b&0x3f)<<__shift; \
                ++__count; \
                __shift+=6; \
            } \
        } \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_BACK_1_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_پشت_1_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    طالما(UTF8_IS_TRAIL((s)[--(i)])) {} \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_BACK_N_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_پشت_N_UNSAFE(s, i, n) UPRV_بلوک_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        UTF8_پشت_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_SET_CP_LIMIT_UNSAFE, see utf_old.h. */
#تعریف_کن UTF8_مجموعه_CHAR_حد_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    UTF8_پشت_1_UNSAFE(s, i); \
    UTF8_FWD_1_UNSAFE(s, i); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Use U8_PREV instead, see utf_old.h. */
#تعریف_کن UTF8_PREV_CHAR_امن(s, شروع, i, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا((c)>=0x80) { \
        إذا((c)<=0xbf) { \
            (c)=utf8_prev_Char_امن_Body(s, شروع, &(i), c, strict); \
        } وإلا { \
            (c)=UTF8_ERROR_مقدار_1; \
        } \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U8_BACK_1, see utf_old.h. */
#تعریف_کن UTF8_پشت_1_امن(s, شروع, i) U8_پشت_1(s, شروع, i)

/** @deprecated ICU 2.4. Renamed to U8_BACK_N, see utf_old.h. */
#تعریف_کن UTF8_پشت_N_امن(s, شروع, i, n) U8_پشت_N(s, شروع, i, n)

/** @deprecated ICU 2.4. Renamed to U8_SET_CP_LIMIT, see utf_old.h. */
#تعریف_کن UTF8_مجموعه_CHAR_حد_امن(s, شروع, i, درازی) U8_مجموعه_CP_حد(s, شروع, i, درازی)

/* Formerly utf16.h --------------------------------------------------------- */

/** Is uchar a first/lead surrogate? @deprecated ICU 2.4. Renamed to U_IS_LEAD and U16_IS_LEAD, see utf_old.h. */
#تعریف_کن UTF_IS_اولی_SURROGATE(uchar) (((uchar)&0xfffffc00)==0xd800)

/** Is uchar a second/trail surrogate? @deprecated ICU 2.4. Renamed to U_IS_TRAIL and U16_IS_TRAIL, see utf_old.h. */
#تعریف_کن UTF_IS_دومی_SURROGATE(uchar) (((uchar)&0xfffffc00)==0xdc00)

/** Assuming c is a surrogate, is it a first/lead surrogate? @deprecated ICU 2.4. Renamed to U_IS_SURROGATE_LEAD and U16_IS_SURROGATE_LEAD, see utf_old.h. */
#تعریف_کن UTF_IS_SURROGATE_اولی(c) (((c)&0x400)==0)

/** Helper constant for UTF16_GET_PAIR_VALUE. @deprecated ICU 2.4. Renamed to U16_SURROGATE_OFFSET, see utf_old.h. */
#تعریف_کن UTF_SURROGATE_جابه_جایی ((0xd800<<10UL)+0xdc00-0x10000)

/** Get the UTF-32 value from the surrogate code units. @deprecated ICU 2.4. Renamed to U16_GET_SUPPLEMENTARY, see utf_old.h. */
#تعریف_کن UTF16_GET_PAIR_مقدار(اولی, دومی) \
    (((اولی)<<10UL)+(دومی)-UTF_SURROGATE_جابه_جایی)

/** @deprecated ICU 2.4. Renamed to U16_LEAD, see utf_old.h. */
#تعریف_کن UTF_اولی_SURROGATE(supplementary) (UChar)(((supplementary)>>10)+0xd7c0)

/** @deprecated ICU 2.4. Renamed to U16_TRAIL, see utf_old.h. */
#تعریف_کن UTF_دومی_SURROGATE(supplementary) (UChar)(((supplementary)&0x3ff)|0xdc00)

/** @deprecated ICU 2.4. Renamed to U16_LEAD, see utf_old.h. */
#تعریف_کن UTF16_LEAD(supplementary) UTF_اولی_SURROGATE(supplementary)

/** @deprecated ICU 2.4. Renamed to U16_TRAIL, see utf_old.h. */
#تعریف_کن UTF16_TRAIL(supplementary) UTF_دومی_SURROGATE(supplementary)

/** @deprecated ICU 2.4. Renamed to U16_IS_SINGLE, see utf_old.h. */
#تعریف_کن UTF16_IS_SINGLE(uchar) !UTF_IS_SURROGATE(uchar)

/** @deprecated ICU 2.4. Renamed to U16_IS_LEAD, see utf_old.h. */
#تعریف_کن UTF16_IS_LEAD(uchar) UTF_IS_اولی_SURROGATE(uchar)

/** @deprecated ICU 2.4. Renamed to U16_IS_TRAIL, see utf_old.h. */
#تعریف_کن UTF16_IS_TRAIL(uchar) UTF_IS_دومی_SURROGATE(uchar)

/** Does this scalar Unicode value need multiple code units for storage? @deprecated ICU 2.4. Use U16_LENGTH or test ((uint32_t)(c)>0xffff) instead, see utf_old.h. */
#تعریف_کن UTF16_NEED_مضرب_UCHAR(c) ((uint32_t)(c)>0xffff)

/** @deprecated ICU 2.4. Renamed to U16_LENGTH, see utf_old.h. */
#تعریف_کن UTF16_CHAR_درازی(c) ((uint32_t)(c)<=0xffff ? 1 : 2)

/** @deprecated ICU 2.4. Renamed to U16_MAX_LENGTH, see utf_old.h. */
#تعریف_کن UTF16_MAX_CHAR_درازی 2

/** Average number of code units compared to UTF-16. @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF16_ARRAY_اندازه(اندازه_2) (اندازه_2)

/**
 * Get a single code point from an offset that points to any
 * of the code units that belong to that code point.
 * Assume 0<=i<length.
 *
 * This could be used for iteration together with
 * UTF16_CHAR_LENGTH() and UTF_IS_ERROR(),
 * but the use of UTF16_NEXT_CHAR[_UNSAFE]() and
 * UTF16_PREV_CHAR[_UNSAFE]() is more efficient for that.
 * @deprecated ICU 2.4. Renamed to U16_GET_UNSAFE, see utf_old.h.
 */
#تعریف_کن UTF16_GET_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[i]; \
    إذا(UTF_IS_SURROGATE(c)) { \
        إذا(UTF_IS_SURROGATE_اولی(c)) { \
            (c)=UTF16_GET_PAIR_مقدار((c), (s)[(i)+1]); \
        } وإلا { \
            (c)=UTF16_GET_PAIR_مقدار((s)[(i)-1], (c)); \
        } \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Use U16_GET instead, see utf_old.h. */
#تعریف_کن UTF16_GET_CHAR_امن(s, شروع, i, درازی, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[i]; \
    إذا(UTF_IS_SURROGATE(c)) { \
        uint16_t __c2; \
        إذا(UTF_IS_SURROGATE_اولی(c)) { \
            إذا((i)+1<(درازی) && UTF_IS_دومی_SURROGATE(__c2=(s)[(i)+1])) { \
                (c)=UTF16_GET_PAIR_مقدار((c), __c2); \
                /* strict: ((c)&0xfffe)==0xfffe is caught by UTF_IS_ERROR() and UTF_IS_UNICODE_CHAR() */ \
            } وإلا إذا(strict) {\
                /* unmatched first surrogate */ \
                (c)=UTF_ERROR_مقدار; \
            } \
        } وإلا { \
            إذا((i)-1>=(شروع) && UTF_IS_اولی_SURROGATE(__c2=(s)[(i)-1])) { \
                (c)=UTF16_GET_PAIR_مقدار(__c2, (c)); \
                /* strict: ((c)&0xfffe)==0xfffe is caught by UTF_IS_ERROR() and UTF_IS_UNICODE_CHAR() */ \
            } وإلا إذا(strict) {\
                /* unmatched second surrogate */ \
                (c)=UTF_ERROR_مقدار; \
            } \
        } \
    } وإلا إذا((strict) && !UTF_IS_UNICODE_CHAR(c)) { \
        (c)=UTF_ERROR_مقدار; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_NEXT_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_بعدی_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا(UTF_IS_اولی_SURROGATE(c)) { \
        (c)=UTF16_GET_PAIR_مقدار((c), (s)[(i)++]); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_APPEND_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_APPEND_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0xffff) { \
        (s)[(i)++]=(uint16_t)(c); \
    } وإلا { \
        (s)[(i)++]=(uint16_t)(((c)>>10)+0xd7c0); \
        (s)[(i)++]=(uint16_t)(((c)&0x3ff)|0xdc00); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_FWD_1_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_FWD_1_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    إذا(UTF_IS_اولی_SURROGATE((s)[(i)++])) { \
        ++(i); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_FWD_N_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_FWD_N_UNSAFE(s, i, n) UPRV_بلوک_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        UTF16_FWD_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_START_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_مجموعه_CHAR_شروع_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    إذا(UTF_IS_دومی_SURROGATE((s)[i])) { \
        --(i); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Use U16_NEXT instead, see utf_old.h. */
#تعریف_کن UTF16_بعدی_CHAR_امن(s, i, درازی, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا(UTF_IS_اولی_SURROGATE(c)) { \
        uint16_t __c2; \
        إذا((i)<(درازی) && UTF_IS_دومی_SURROGATE(__c2=(s)[(i)])) { \
            ++(i); \
            (c)=UTF16_GET_PAIR_مقدار((c), __c2); \
            /* strict: ((c)&0xfffe)==0xfffe is caught by UTF_IS_ERROR() and UTF_IS_UNICODE_CHAR() */ \
        } وإلا إذا(strict) {\
            /* unmatched first surrogate */ \
            (c)=UTF_ERROR_مقدار; \
        } \
    } وإلا إذا((strict) && !UTF_IS_UNICODE_CHAR(c)) { \
        /* unmatched second surrogate or other non-character */ \
        (c)=UTF_ERROR_مقدار; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Use U16_APPEND instead, see utf_old.h. */
#تعریف_کن UTF16_APPEND_CHAR_امن(s, i, درازی, c) UPRV_بلوک_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0xffff) { \
        (s)[(i)++]=(uint16_t)(c); \
    } وإلا إذا((uint32_t)(c)<=0x10ffff) { \
        إذا((i)+1<(درازی)) { \
            (s)[(i)++]=(uint16_t)(((c)>>10)+0xd7c0); \
            (s)[(i)++]=(uint16_t)(((c)&0x3ff)|0xdc00); \
        } وإلا /* not enough space */ { \
            (s)[(i)++]=UTF_ERROR_مقدار; \
        } \
    } وإلا /* c>0x10ffff, write error value */ { \
        (s)[(i)++]=UTF_ERROR_مقدار; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_FWD_1, see utf_old.h. */
#تعریف_کن UTF16_FWD_1_امن(s, i, درازی) U16_FWD_1(s, i, درازی)

/** @deprecated ICU 2.4. Renamed to U16_FWD_N, see utf_old.h. */
#تعریف_کن UTF16_FWD_N_امن(s, i, درازی, n) U16_FWD_N(s, i, درازی, n)

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_START, see utf_old.h. */
#تعریف_کن UTF16_مجموعه_CHAR_شروع_امن(s, شروع, i) U16_مجموعه_CP_شروع(s, شروع, i)

/** @deprecated ICU 2.4. Renamed to U16_PREV_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_PREV_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا(UTF_IS_دومی_SURROGATE(c)) { \
        (c)=UTF16_GET_PAIR_مقدار((s)[--(i)], (c)); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_BACK_1_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_پشت_1_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    إذا(UTF_IS_دومی_SURROGATE((s)[--(i)])) { \
        --(i); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_BACK_N_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_پشت_N_UNSAFE(s, i, n) UPRV_بلوک_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        UTF16_پشت_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT_UNSAFE, see utf_old.h. */
#تعریف_کن UTF16_مجموعه_CHAR_حد_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    إذا(UTF_IS_اولی_SURROGATE((s)[(i)-1])) { \
        ++(i); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Use U16_PREV instead, see utf_old.h. */
#تعریف_کن UTF16_PREV_CHAR_امن(s, شروع, i, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا(UTF_IS_دومی_SURROGATE(c)) { \
        uint16_t __c2; \
        إذا((i)>(شروع) && UTF_IS_اولی_SURROGATE(__c2=(s)[(i)-1])) { \
            --(i); \
            (c)=UTF16_GET_PAIR_مقدار(__c2, (c)); \
            /* strict: ((c)&0xfffe)==0xfffe is caught by UTF_IS_ERROR() and UTF_IS_UNICODE_CHAR() */ \
        } وإلا إذا(strict) {\
            /* unmatched second surrogate */ \
            (c)=UTF_ERROR_مقدار; \
        } \
    } وإلا إذا((strict) && !UTF_IS_UNICODE_CHAR(c)) { \
        /* unmatched first surrogate or other non-character */ \
        (c)=UTF_ERROR_مقدار; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Renamed to U16_BACK_1, see utf_old.h. */
#تعریف_کن UTF16_پشت_1_امن(s, شروع, i) U16_پشت_1(s, شروع, i)

/** @deprecated ICU 2.4. Renamed to U16_BACK_N, see utf_old.h. */
#تعریف_کن UTF16_پشت_N_امن(s, شروع, i, n) U16_پشت_N(s, شروع, i, n)

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT, see utf_old.h. */
#تعریف_کن UTF16_مجموعه_CHAR_حد_امن(s, شروع, i, درازی) U16_مجموعه_CP_حد(s, شروع, i, درازی)

/* Formerly utf32.h --------------------------------------------------------- */

/*
* Old documentation:
*
*   This file defines macros to deal with UTF-32 code units and code points.
*   Signatures and semantics are the same as for the similarly named macros
*   in utf16.h.
*   utf32.h is included by utf.h after unicode/umachine.h</p>
*   and some common definitions.
*   <p><b>Usage:</b>  ICU coding guidelines for if() statements should be followed when using these macros.
*                  Compound statements (curly braces {}) must be used  for if-else-while...
*                  bodies and all macro statements should be terminated with semicolon.</p>
*/

/* internal definitions ----------------------------------------------------- */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_IS_امن(c, strict) \
    (!(strict) ? \
        (uint32_t)(c)<=0x10ffff : \
        UTF_IS_UNICODE_CHAR(c))

/*
 * For the semantics of all of these macros, see utf16.h.
 * The UTF-32 versions are trivial because any code point is
 * encoded using exactly one code unit.
 */

/* single-code point definitions -------------------------------------------- */

/* classes of code unit values */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_IS_SINGLE(uchar) 1
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_IS_LEAD(uchar) 0
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_IS_TRAIL(uchar) 0

/* number of code units per code point */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_NEED_مضرب_UCHAR(c) 0
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_CHAR_درازی(c) 1
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_MAX_CHAR_درازی 1

/* average number of code units compared to UTF-16 */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_ARRAY_اندازه(اندازه_2) (اندازه_2)

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_GET_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[i]; \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_GET_CHAR_امن(s, شروع, i, درازی, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[i]; \
    إذا(!UTF32_IS_امن(c, strict)) { \
        (c)=UTF_ERROR_مقدار; \
    } \
} UPRV_بلوک_MACRO_انتها

/* definitions with forward iteration --------------------------------------- */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_بعدی_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_APPEND_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (s)[(i)++]=(c); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_FWD_1_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    ++(i); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_FWD_N_UNSAFE(s, i, n) UPRV_بلوک_MACRO_BEGIN { \
    (i)+=(n); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_مجموعه_CHAR_شروع_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_بعدی_CHAR_امن(s, i, درازی, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا(!UTF32_IS_امن(c, strict)) { \
        (c)=UTF_ERROR_مقدار; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_APPEND_CHAR_امن(s, i, درازی, c) UPRV_بلوک_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0x10ffff) { \
        (s)[(i)++]=(c); \
    } وإلا /* c>0x10ffff, write 0xfffd */ { \
        (s)[(i)++]=0xfffd; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_FWD_1_امن(s, i, درازی) UPRV_بلوک_MACRO_BEGIN { \
    ++(i); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_FWD_N_امن(s, i, درازی, n) UPRV_بلوک_MACRO_BEGIN { \
    إذا(((i)+=(n))>(درازی)) { \
        (i)=(درازی); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_مجموعه_CHAR_شروع_امن(s, شروع, i) UPRV_بلوک_MACRO_BEGIN { \
} UPRV_بلوک_MACRO_انتها

/* definitions with backward iteration -------------------------------------- */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_PREV_CHAR_UNSAFE(s, i, c) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_پشت_1_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
    --(i); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_پشت_N_UNSAFE(s, i, n) UPRV_بلوک_MACRO_BEGIN { \
    (i)-=(n); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_مجموعه_CHAR_حد_UNSAFE(s, i) UPRV_بلوک_MACRO_BEGIN { \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_PREV_CHAR_امن(s, شروع, i, c, strict) UPRV_بلوک_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا(!UTF32_IS_امن(c, strict)) { \
        (c)=UTF_ERROR_مقدار; \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_پشت_1_امن(s, شروع, i) UPRV_بلوک_MACRO_BEGIN { \
    --(i); \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_پشت_N_امن(s, شروع, i, n) UPRV_بلوک_MACRO_BEGIN { \
    (i)-=(n); \
    إذا((i)<(شروع)) { \
        (i)=(شروع); \
    } \
} UPRV_بلوک_MACRO_انتها

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#تعریف_کن UTF32_مجموعه_CHAR_حد_امن(s, i, درازی) UPRV_بلوک_MACRO_BEGIN { \
} UPRV_بلوک_MACRO_انتها

/* Formerly utf.h, part 2 --------------------------------------------------- */

/**
 * Estimate the number of code units for a string based on the number of UTF-16 code units.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#تعریف_کن UTF_ARRAY_اندازه(اندازه_2) UTF16_ARRAY_اندازه(اندازه_2)

/** @deprecated ICU 2.4. Renamed to U16_GET_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_GET_CHAR_UNSAFE(s, i, c)                 UTF16_GET_CHAR_UNSAFE(s, i, c)

/** @deprecated ICU 2.4. Use U16_GET instead, see utf_old.h. */
#تعریف_کن UTF_GET_CHAR_امن(s, شروع, i, درازی, c, strict) UTF16_GET_CHAR_امن(s, شروع, i, درازی, c, strict)


/** @deprecated ICU 2.4. Renamed to U16_NEXT_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_بعدی_CHAR_UNSAFE(s, i, c)                UTF16_بعدی_CHAR_UNSAFE(s, i, c)

/** @deprecated ICU 2.4. Use U16_NEXT instead, see utf_old.h. */
#تعریف_کن UTF_بعدی_CHAR_امن(s, i, درازی, c, strict)  UTF16_بعدی_CHAR_امن(s, i, درازی, c, strict)


/** @deprecated ICU 2.4. Renamed to U16_APPEND_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_APPEND_CHAR_UNSAFE(s, i, c)              UTF16_APPEND_CHAR_UNSAFE(s, i, c)

/** @deprecated ICU 2.4. Use U16_APPEND instead, see utf_old.h. */
#تعریف_کن UTF_APPEND_CHAR_امن(s, i, درازی, c)        UTF16_APPEND_CHAR_امن(s, i, درازی, c)


/** @deprecated ICU 2.4. Renamed to U16_FWD_1_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_FWD_1_UNSAFE(s, i)                       UTF16_FWD_1_UNSAFE(s, i)

/** @deprecated ICU 2.4. Renamed to U16_FWD_1, see utf_old.h. */
#تعریف_کن UTF_FWD_1_امن(s, i, درازی)                 UTF16_FWD_1_امن(s, i, درازی)


/** @deprecated ICU 2.4. Renamed to U16_FWD_N_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_FWD_N_UNSAFE(s, i, n)                    UTF16_FWD_N_UNSAFE(s, i, n)

/** @deprecated ICU 2.4. Renamed to U16_FWD_N, see utf_old.h. */
#تعریف_کن UTF_FWD_N_امن(s, i, درازی, n)              UTF16_FWD_N_امن(s, i, درازی, n)


/** @deprecated ICU 2.4. Renamed to U16_SET_CP_START_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_مجموعه_CHAR_شروع_UNSAFE(s, i)              UTF16_مجموعه_CHAR_شروع_UNSAFE(s, i)

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_START, see utf_old.h. */
#تعریف_کن UTF_مجموعه_CHAR_شروع_امن(s, شروع, i)         UTF16_مجموعه_CHAR_شروع_امن(s, شروع, i)


/** @deprecated ICU 2.4. Renamed to U16_PREV_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_PREV_CHAR_UNSAFE(s, i, c)                UTF16_PREV_CHAR_UNSAFE(s, i, c)

/** @deprecated ICU 2.4. Use U16_PREV instead, see utf_old.h. */
#تعریف_کن UTF_PREV_CHAR_امن(s, شروع, i, c, strict)   UTF16_PREV_CHAR_امن(s, شروع, i, c, strict)


/** @deprecated ICU 2.4. Renamed to U16_BACK_1_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_پشت_1_UNSAFE(s, i)                      UTF16_پشت_1_UNSAFE(s, i)

/** @deprecated ICU 2.4. Renamed to U16_BACK_1, see utf_old.h. */
#تعریف_کن UTF_پشت_1_امن(s, شروع, i)                 UTF16_پشت_1_امن(s, شروع, i)


/** @deprecated ICU 2.4. Renamed to U16_BACK_N_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_پشت_N_UNSAFE(s, i, n)                   UTF16_پشت_N_UNSAFE(s, i, n)

/** @deprecated ICU 2.4. Renamed to U16_BACK_N, see utf_old.h. */
#تعریف_کن UTF_پشت_N_امن(s, شروع, i, n)              UTF16_پشت_N_امن(s, شروع, i, n)


/** @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT_UNSAFE, see utf_old.h. */
#تعریف_کن UTF_مجموعه_CHAR_حد_UNSAFE(s, i)              UTF16_مجموعه_CHAR_حد_UNSAFE(s, i)

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT, see utf_old.h. */
#تعریف_کن UTF_مجموعه_CHAR_حد_امن(s, شروع, i, درازی) UTF16_مجموعه_CHAR_حد_امن(s, شروع, i, درازی)

/* Define default macros (UTF-16 "safe") ------------------------------------ */

/**
 * Does this code unit alone encode a code point (BMP, not a surrogate)?
 * Same as UTF16_IS_SINGLE.
 * @deprecated ICU 2.4. Renamed to U_IS_SINGLE and U16_IS_SINGLE, see utf_old.h.
 */
#تعریف_کن UTF_IS_SINGLE(uchar) U16_IS_SINGLE(uchar)

/**
 * Is this code unit the first one of several (a lead surrogate)?
 * Same as UTF16_IS_LEAD.
 * @deprecated ICU 2.4. Renamed to U_IS_LEAD and U16_IS_LEAD, see utf_old.h.
 */
#تعریف_کن UTF_IS_LEAD(uchar) U16_IS_LEAD(uchar)

/**
 * Is this code unit one of several but not the first one (a trail surrogate)?
 * Same as UTF16_IS_TRAIL.
 * @deprecated ICU 2.4. Renamed to U_IS_TRAIL and U16_IS_TRAIL, see utf_old.h.
 */
#تعریف_کن UTF_IS_TRAIL(uchar) U16_IS_TRAIL(uchar)

/**
 * Does this code point require multiple code units (is it a supplementary code point)?
 * Same as UTF16_NEED_MULTIPLE_UCHAR.
 * @deprecated ICU 2.4. Use U16_LENGTH or test ((uint32_t)(c)>0xffff) instead.
 */
#تعریف_کن UTF_NEED_مضرب_UCHAR(c) UTF16_NEED_مضرب_UCHAR(c)

/**
 * How many code units are used to encode this code point (1 or 2)?
 * Same as UTF16_CHAR_LENGTH.
 * @deprecated ICU 2.4. Renamed to U16_LENGTH, see utf_old.h.
 */
#تعریف_کن UTF_CHAR_درازی(c) U16_درازی(c)

/**
 * How many code units are used at most for any Unicode code point (2)?
 * Same as UTF16_MAX_CHAR_LENGTH.
 * @deprecated ICU 2.4. Renamed to U16_MAX_LENGTH, see utf_old.h.
 */
#تعریف_کن UTF_MAX_CHAR_درازی U16_MAX_درازی

/**
 * Set c to the code point that contains the code unit i.
 * i could point to the lead or the trail surrogate for the code point.
 * i is not modified.
 * Same as UTF16_GET_CHAR.
 * \pre 0<=i<length
 *
 * @deprecated ICU 2.4. Renamed to U16_GET, see utf_old.h.
 */
#تعریف_کن UTF_GET_CHAR(s, شروع, i, درازی, c) U16_GET(s, شروع, i, درازی, c)

/**
 * Set c to the code point that starts at code unit i
 * and advance i to beyond the code units of this code point (post-increment).
 * i must point to the first code unit of a code point.
 * Otherwise c is set to the trail unit (surrogate) itself.
 * Same as UTF16_NEXT_CHAR.
 * \pre 0<=i<length
 * \post 0<i<=length
 *
 * @deprecated ICU 2.4. Renamed to U16_NEXT, see utf_old.h.
 */
#تعریف_کن UTF_بعدی_CHAR(s, i, درازی, c) U16_بعدی(s, i, درازی, c)

/**
 * Append the code units of code point c to the string at index i
 * and advance i to beyond the new code units (post-increment).
 * The code units beginning at index i will be overwritten.
 * Same as UTF16_APPEND_CHAR.
 * \pre 0<=c<=0x10ffff
 * \pre 0<=i<length
 * \post 0<i<=length
 *
 * @deprecated ICU 2.4. Use U16_APPEND instead, see utf_old.h.
 */
#تعریف_کن UTF_APPEND_CHAR(s, i, درازی, c) UTF16_APPEND_CHAR_امن(s, i, درازی, c)

/**
 * Advance i to beyond the code units of the code point that begins at i.
 * I.e., advance i by one code point.
 * Same as UTF16_FWD_1.
 * \pre 0<=i<length
 * \post 0<i<=length
 *
 * @deprecated ICU 2.4. Renamed to U16_FWD_1, see utf_old.h.
 */
#تعریف_کن UTF_FWD_1(s, i, درازی) U16_FWD_1(s, i, درازی)

/**
 * Advance i to beyond the code units of the n code points where the first one begins at i.
 * I.e., advance i by n code points.
 * Same as UT16_FWD_N.
 * \pre 0<=i<length
 * \post 0<i<=length
 *
 * @deprecated ICU 2.4. Renamed to U16_FWD_N, see utf_old.h.
 */
#تعریف_کن UTF_FWD_N(s, i, درازی, n) U16_FWD_N(s, i, درازی, n)

/**
 * Take the random-access index i and adjust it so that it points to the beginning
 * of a code point.
 * The input index points to any code unit of a code point and is moved to point to
 * the first code unit of the same code point. i is never incremented.
 * In other words, if i points to a trail surrogate that is preceded by a matching
 * lead surrogate, then i is decremented. Otherwise it is not modified.
 * This can be used to start an iteration with UTF_NEXT_CHAR() from a random index.
 * Same as UTF16_SET_CHAR_START.
 * \pre start<=i<length
 * \post start<=i<length
 *
 * @deprecated ICU 2.4. Renamed to U16_SET_CP_START, see utf_old.h.
 */
#تعریف_کن UTF_مجموعه_CHAR_شروع(s, شروع, i) U16_مجموعه_CP_شروع(s, شروع, i)

/**
 * Set c to the code point that has code units before i
 * and move i backward (towards the beginning of the string)
 * to the first code unit of this code point (pre-increment).
 * i must point to the first code unit after the last unit of a code point (i==length is allowed).
 * Same as UTF16_PREV_CHAR.
 * \pre start<i<=length
 * \post start<=i<length
 *
 * @deprecated ICU 2.4. Renamed to U16_PREV, see utf_old.h.
 */
#تعریف_کن UTF_PREV_CHAR(s, شروع, i, c) U16_PREV(s, شروع, i, c)

/**
 * Move i backward (towards the beginning of the string)
 * to the first code unit of the code point that has code units before i.
 * I.e., move i backward by one code point.
 * i must point to the first code unit after the last unit of a code point (i==length is allowed).
 * Same as UTF16_BACK_1.
 * \pre start<i<=length
 * \post start<=i<length
 *
 * @deprecated ICU 2.4. Renamed to U16_BACK_1, see utf_old.h.
 */
#تعریف_کن UTF_پشت_1(s, شروع, i) U16_پشت_1(s, شروع, i)

/**
 * Move i backward (towards the beginning of the string)
 * to the first code unit of the n code points that have code units before i.
 * I.e., move i backward by n code points.
 * i must point to the first code unit after the last unit of a code point (i==length is allowed).
 * Same as UTF16_BACK_N.
 * \pre start<i<=length
 * \post start<=i<length
 *
 * @deprecated ICU 2.4. Renamed to U16_BACK_N, see utf_old.h.
 */
#تعریف_کن UTF_پشت_N(s, شروع, i, n) U16_پشت_N(s, شروع, i, n)

/**
 * Take the random-access index i and adjust it so that it points beyond
 * a code point. The input index points beyond any code unit
 * of a code point and is moved to point beyond the last code unit of the same
 * code point. i is never decremented.
 * In other words, if i points to a trail surrogate that is preceded by a matching
 * lead surrogate, then i is incremented. Otherwise it is not modified.
 * This can be used to start an iteration with UTF_PREV_CHAR() from a random index.
 * Same as UTF16_SET_CHAR_LIMIT.
 * \pre start<i<=length
 * \post start<i<=length
 *
 * @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT, see utf_old.h.
 */
#تعریف_کن UTF_مجموعه_CHAR_حد(s, شروع, i, درازی) U16_مجموعه_CP_حد(s, شروع, i, درازی)

#پایان_اگر  // !U_HIDE_DEPRECATED_API && !U_HIDE_OBSOLETE_UTF_OLD_H

#پایان_اگر
