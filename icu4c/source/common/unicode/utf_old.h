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

#جيڪڏهن_وصف_نه_ٿيل __UTF_OLD_H__
#وصف_ڪريو __UTF_OLD_H__

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
#جيڪڏهن_وصف_نه_ٿيل U_HIDE_OBSOLETE_UTF_OLD_H
#   define U_HIDE_OBSOLETE_UTF_OLD_H 0
#جيڪڏهن_پورو

#إذا !defined(U_HIDE_DEPRECATED_API) && !U_HIDE_OBSOLETE_UTF_OLD_H

/* Formerly utf.h, part 1 --------------------------------------------------- */

#جيڪڏهن_وصف_ٿيل U_USE_UTF_DEPRECATES
/**
 * Unicode string and array offset and index type.
 * ICU always counts Unicode code units (UChars) for
 * string offsets, indexes, and lengths, not Unicode code points.
 *
 * @obsolete ICU 2.6. Use int32_t directly instead since this API will be removed in that release.
 */
تعريفنوع int32_t UText_وٿي;
#جيڪڏهن_پورو

/** Number of bits in a Unicode string code unit - ICU uses 16-bit Unicode. @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF_سائيز 16

/**
 * The default choice for general Unicode string macros is to use the ..._SAFE macro implementations
 * with strict=false.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#وصف_ڪريو UTF_محفوظ
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_رد UTF_UNSAFE
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_رد UTF_STRICT

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
#وصف_ڪريو UTF8_ERROR_قدر_1 0x15

/**
 * See documentation on UTF8_ERROR_VALUE_1 for details.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#وصف_ڪريو UTF8_ERROR_قدر_2 0x9f

/**
 * Error value for all UTFs. This code point value will be set by macros with error
 * checking if an error is detected.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#وصف_ڪريو UTF_ERROR_قدر 0xffff

/**
 * Is a given 32-bit code an error value
 * as returned by one of the macros for any UTF?
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#وصف_ڪريو UTF_IS_ERROR(c) \
    (((c)&0xfffe)==0xfffe || (c)==UTF8_ERROR_قدر_1 || (c)==UTF8_ERROR_قدر_2)

/**
 * This is a combined macro: Is c a valid Unicode value _and_ not an error code?
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#وصف_ڪريو UTF_IS_VALID(c) \
    (UTF_IS_UNICODE_CHAR(c) && \
     (c)!=UTF8_ERROR_قدر_1 && (c)!=UTF8_ERROR_قدر_2)

/**
 * Is this code unit or code point a surrogate (U+d800..U+dfff)?
 * @deprecated ICU 2.4. Renamed to U_IS_SURROGATE and U16_IS_SURROGATE, see utf_old.h.
 */
#وصف_ڪريو UTF_IS_SURROGATE(uchar) (((uchar)&0xfffff800)==0xd800)

/**
 * Is a given 32-bit code point a Unicode noncharacter?
 *
 * @deprecated ICU 2.4. Renamed to U_IS_UNICODE_NONCHAR, see utf_old.h.
 */
#وصف_ڪريو UTF_IS_UNICODE_NONCHAR(c) \
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
#وصف_ڪريو UTF_IS_UNICODE_CHAR(c) \
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
#جيڪڏهن_وصف_ٿيل U_UTF8_IMPL
// No forward declaration if compiling utf_impl.cpp, which defines utf8_countTrailBytes.
#وإلاإذا defined(U_STATIC_IMPLEMENTATION) || defined(U_COMMON_IMPLEMENTATION)
U_CAPI ثابت uint8_t utf8_ڳڻپ_Trail_بائيٽون[];
#وإلا
U_CFUNC U_IMPORT ثابت uint8_t utf8_ڳڻپ_Trail_بائيٽون[];
#جيڪڏهن_پورو 

/**
 * Count the trail bytes for a UTF-8 lead byte.
 * @deprecated ICU 2.4. Renamed to U8_COUNT_TRAIL_BYTES, see utf_old.h.
 */
#وصف_ڪريو UTF8_ڳڻپ_TRAIL_بائيٽون(leadByte) (utf8_ڳڻپ_Trail_بائيٽون[(uint8_t)leadByte])

/**
 * Mask a UTF-8 lead byte, leave only the lower bits that form part of the code point value.
 * @deprecated ICU 2.4. Renamed to U8_MASK_LEAD_BYTE, see utf_old.h.
 */
#وصف_ڪريو UTF8_نقاب_LEAD_BYTE(leadByte, ڳڻپ_Trail_بائيٽون) ((leadByte)&=(1<<(6-(ڳڻپ_Trail_بائيٽون)))-1)

/** Is this this code point a single code unit (byte)? @deprecated ICU 2.4. Renamed to U8_IS_SINGLE, see utf_old.h. */
#وصف_ڪريو UTF8_IS_SINGLE(uchar) (((uchar)&0x80)==0)
/** Is this this code unit the lead code unit (byte) of a code point? @deprecated ICU 2.4. Renamed to U8_IS_LEAD, see utf_old.h. */
#وصف_ڪريو UTF8_IS_LEAD(uchar) ((uint8_t)((uchar)-0xc0)<0x3e)
/** Is this this code unit a trailing code unit (byte) of a code point? @deprecated ICU 2.4. Renamed to U8_IS_TRAIL, see utf_old.h. */
#وصف_ڪريو UTF8_IS_TRAIL(uchar) (((uchar)&0xc0)==0x80)

/** Does this scalar Unicode value need multiple code units for storage? @deprecated ICU 2.4. Use U8_LENGTH or test ((uint32_t)(c)>0x7f) instead, see utf_old.h. */
#وصف_ڪريو UTF8_NEED_گڻو_UCHAR(c) ((uint32_t)(c)>0x7f)

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
#   define UTF8_CHAR_ڊيگهائي(c) \
        ((uint32_t)(c)<=0x7f ? 1 : \
            ((uint32_t)(c)<=0x7ff ? 2 : \
                ((uint32_t)((c)-0x10000)>0xfffff ? 3 : 4) \
            ) \
        )
#وإلا
#   define UTF8_CHAR_ڊيگهائي(c) \
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
#جيڪڏهن_پورو

/** The maximum number of bytes per code point. @deprecated ICU 2.4. Renamed to U8_MAX_LENGTH, see utf_old.h. */
#وصف_ڪريو UTF8_MAX_CHAR_ڊيگهائي 4

/** Average number of code units compared to UTF-16. @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF8_ARRAY_سائيز(سائيز_2) ((5*(سائيز_2))/2)

/** @deprecated ICU 2.4. Renamed to U8_GET_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_GET_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t utf8_get_char_unsafe_اشاريو=(int32_t)(i); \
    UTF8_سيٽ_CHAR_شروعات_UNSAFE(s, utf8_get_char_unsafe_اشاريو); \
    UTF8_ايندڙ_CHAR_UNSAFE(s, utf8_get_char_unsafe_اشاريو, c); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Use U8_GET instead, see utf_old.h. */
#وصف_ڪريو UTF8_GET_CHAR_محفوظ(s, شروعات, i, ڊيگهائي, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t utf8_get_char_محفوظ_اشاريو=(int32_t)(i); \
    UTF8_سيٽ_CHAR_شروعات_محفوظ(s, شروعات, utf8_get_char_محفوظ_اشاريو); \
    UTF8_ايندڙ_CHAR_محفوظ(s, utf8_get_char_محفوظ_اشاريو, ڊيگهائي, c, strict); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_NEXT_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_ايندڙ_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا((uint8_t)((c)-0xc0)<0x35) { \
        uint8_t __count=UTF8_ڳڻپ_TRAIL_بائيٽون(c); \
        UTF8_نقاب_LEAD_BYTE(c, __count); \
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
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_APPEND_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_APPEND_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
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
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_FWD_1_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_FWD_1_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    (i)+=1+UTF8_ڳڻپ_TRAIL_بائيٽون((s)[i]); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_FWD_N_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_FWD_N_UNSAFE(s, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        UTF8_FWD_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_SET_CP_START_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_سيٽ_CHAR_شروعات_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    طالما(UTF8_IS_TRAIL((s)[i])) { --(i); } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Use U8_NEXT instead, see utf_old.h. */
#وصف_ڪريو UTF8_ايندڙ_CHAR_محفوظ(s, i, ڊيگهائي, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا((c)>=0x80) { \
        إذا(UTF8_IS_LEAD(c)) { \
            (c)=utf8_ايندڙ_Char_محفوظ_Body(s, &(i), (int32_t)(ڊيگهائي), c, strict); \
        } وإلا { \
            (c)=UTF8_ERROR_قدر_1; \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Use U8_APPEND instead, see utf_old.h. */
#وصف_ڪريو UTF8_APPEND_CHAR_محفوظ(s, i, ڊيگهائي, c)  UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0x7f) { \
        (s)[(i)++]=(uint8_t)(c); \
    } وإلا { \
        (i)=utf8_append_Char_محفوظ_Body(s, (int32_t)(i), (int32_t)(ڊيگهائي), c, عدم); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_FWD_1, see utf_old.h. */
#وصف_ڪريو UTF8_FWD_1_محفوظ(s, i, ڊيگهائي) U8_FWD_1(s, i, ڊيگهائي)

/** @deprecated ICU 2.4. Renamed to U8_FWD_N, see utf_old.h. */
#وصف_ڪريو UTF8_FWD_N_محفوظ(s, i, ڊيگهائي, n) U8_FWD_N(s, i, ڊيگهائي, n)

/** @deprecated ICU 2.4. Renamed to U8_SET_CP_START, see utf_old.h. */
#وصف_ڪريو UTF8_سيٽ_CHAR_شروعات_محفوظ(s, شروعات, i) U8_سيٽ_CP_شروعات(s, شروعات, i)

/** @deprecated ICU 2.4. Renamed to U8_PREV_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_PREV_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا(UTF8_IS_TRAIL(c)) { \
        uint8_t __b, __count=1, __shift=6; \
\
        /* c is a trail byte */ \
        (c)&=0x3f; \
        لكل(;;) { \
            __b=(s)[--(i)]; \
            إذا(__b>=0xc0) { \
                UTF8_نقاب_LEAD_BYTE(__b, __count); \
                (c)|=(UChar32)__b<<__shift; \
                اكسر; \
            } وإلا { \
                (c)|=(UChar32)(__b&0x3f)<<__shift; \
                ++__count; \
                __shift+=6; \
            } \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_BACK_1_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_پٺ_1_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    طالما(UTF8_IS_TRAIL((s)[--(i)])) {} \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_BACK_N_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_پٺ_N_UNSAFE(s, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        UTF8_پٺ_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_SET_CP_LIMIT_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF8_سيٽ_CHAR_حد_بندي_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    UTF8_پٺ_1_UNSAFE(s, i); \
    UTF8_FWD_1_UNSAFE(s, i); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Use U8_PREV instead, see utf_old.h. */
#وصف_ڪريو UTF8_PREV_CHAR_محفوظ(s, شروعات, i, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا((c)>=0x80) { \
        إذا((c)<=0xbf) { \
            (c)=utf8_prev_Char_محفوظ_Body(s, شروعات, &(i), c, strict); \
        } وإلا { \
            (c)=UTF8_ERROR_قدر_1; \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U8_BACK_1, see utf_old.h. */
#وصف_ڪريو UTF8_پٺ_1_محفوظ(s, شروعات, i) U8_پٺ_1(s, شروعات, i)

/** @deprecated ICU 2.4. Renamed to U8_BACK_N, see utf_old.h. */
#وصف_ڪريو UTF8_پٺ_N_محفوظ(s, شروعات, i, n) U8_پٺ_N(s, شروعات, i, n)

/** @deprecated ICU 2.4. Renamed to U8_SET_CP_LIMIT, see utf_old.h. */
#وصف_ڪريو UTF8_سيٽ_CHAR_حد_بندي_محفوظ(s, شروعات, i, ڊيگهائي) U8_سيٽ_CP_حد_بندي(s, شروعات, i, ڊيگهائي)

/* Formerly utf16.h --------------------------------------------------------- */

/** Is uchar a first/lead surrogate? @deprecated ICU 2.4. Renamed to U_IS_LEAD and U16_IS_LEAD, see utf_old.h. */
#وصف_ڪريو UTF_IS_پهريون_SURROGATE(uchar) (((uchar)&0xfffffc00)==0xd800)

/** Is uchar a second/trail surrogate? @deprecated ICU 2.4. Renamed to U_IS_TRAIL and U16_IS_TRAIL, see utf_old.h. */
#وصف_ڪريو UTF_IS_ٻيو_SURROGATE(uchar) (((uchar)&0xfffffc00)==0xdc00)

/** Assuming c is a surrogate, is it a first/lead surrogate? @deprecated ICU 2.4. Renamed to U_IS_SURROGATE_LEAD and U16_IS_SURROGATE_LEAD, see utf_old.h. */
#وصف_ڪريو UTF_IS_SURROGATE_پهريون(c) (((c)&0x400)==0)

/** Helper constant for UTF16_GET_PAIR_VALUE. @deprecated ICU 2.4. Renamed to U16_SURROGATE_OFFSET, see utf_old.h. */
#وصف_ڪريو UTF_SURROGATE_وٿي ((0xd800<<10UL)+0xdc00-0x10000)

/** Get the UTF-32 value from the surrogate code units. @deprecated ICU 2.4. Renamed to U16_GET_SUPPLEMENTARY, see utf_old.h. */
#وصف_ڪريو UTF16_GET_PAIR_قدر(پهريون, ٻيو) \
    (((پهريون)<<10UL)+(ٻيو)-UTF_SURROGATE_وٿي)

/** @deprecated ICU 2.4. Renamed to U16_LEAD, see utf_old.h. */
#وصف_ڪريو UTF_پهريون_SURROGATE(supplementary) (UChar)(((supplementary)>>10)+0xd7c0)

/** @deprecated ICU 2.4. Renamed to U16_TRAIL, see utf_old.h. */
#وصف_ڪريو UTF_ٻيو_SURROGATE(supplementary) (UChar)(((supplementary)&0x3ff)|0xdc00)

/** @deprecated ICU 2.4. Renamed to U16_LEAD, see utf_old.h. */
#وصف_ڪريو UTF16_LEAD(supplementary) UTF_پهريون_SURROGATE(supplementary)

/** @deprecated ICU 2.4. Renamed to U16_TRAIL, see utf_old.h. */
#وصف_ڪريو UTF16_TRAIL(supplementary) UTF_ٻيو_SURROGATE(supplementary)

/** @deprecated ICU 2.4. Renamed to U16_IS_SINGLE, see utf_old.h. */
#وصف_ڪريو UTF16_IS_SINGLE(uchar) !UTF_IS_SURROGATE(uchar)

/** @deprecated ICU 2.4. Renamed to U16_IS_LEAD, see utf_old.h. */
#وصف_ڪريو UTF16_IS_LEAD(uchar) UTF_IS_پهريون_SURROGATE(uchar)

/** @deprecated ICU 2.4. Renamed to U16_IS_TRAIL, see utf_old.h. */
#وصف_ڪريو UTF16_IS_TRAIL(uchar) UTF_IS_ٻيو_SURROGATE(uchar)

/** Does this scalar Unicode value need multiple code units for storage? @deprecated ICU 2.4. Use U16_LENGTH or test ((uint32_t)(c)>0xffff) instead, see utf_old.h. */
#وصف_ڪريو UTF16_NEED_گڻو_UCHAR(c) ((uint32_t)(c)>0xffff)

/** @deprecated ICU 2.4. Renamed to U16_LENGTH, see utf_old.h. */
#وصف_ڪريو UTF16_CHAR_ڊيگهائي(c) ((uint32_t)(c)<=0xffff ? 1 : 2)

/** @deprecated ICU 2.4. Renamed to U16_MAX_LENGTH, see utf_old.h. */
#وصف_ڪريو UTF16_MAX_CHAR_ڊيگهائي 2

/** Average number of code units compared to UTF-16. @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF16_ARRAY_سائيز(سائيز_2) (سائيز_2)

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
#وصف_ڪريو UTF16_GET_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[i]; \
    إذا(UTF_IS_SURROGATE(c)) { \
        إذا(UTF_IS_SURROGATE_پهريون(c)) { \
            (c)=UTF16_GET_PAIR_قدر((c), (s)[(i)+1]); \
        } وإلا { \
            (c)=UTF16_GET_PAIR_قدر((s)[(i)-1], (c)); \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Use U16_GET instead, see utf_old.h. */
#وصف_ڪريو UTF16_GET_CHAR_محفوظ(s, شروعات, i, ڊيگهائي, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[i]; \
    إذا(UTF_IS_SURROGATE(c)) { \
        uint16_t __c2; \
        إذا(UTF_IS_SURROGATE_پهريون(c)) { \
            إذا((i)+1<(ڊيگهائي) && UTF_IS_ٻيو_SURROGATE(__c2=(s)[(i)+1])) { \
                (c)=UTF16_GET_PAIR_قدر((c), __c2); \
                /* strict: ((c)&0xfffe)==0xfffe is caught by UTF_IS_ERROR() and UTF_IS_UNICODE_CHAR() */ \
            } وإلا إذا(strict) {\
                /* unmatched first surrogate */ \
                (c)=UTF_ERROR_قدر; \
            } \
        } وإلا { \
            إذا((i)-1>=(شروعات) && UTF_IS_پهريون_SURROGATE(__c2=(s)[(i)-1])) { \
                (c)=UTF16_GET_PAIR_قدر(__c2, (c)); \
                /* strict: ((c)&0xfffe)==0xfffe is caught by UTF_IS_ERROR() and UTF_IS_UNICODE_CHAR() */ \
            } وإلا إذا(strict) {\
                /* unmatched second surrogate */ \
                (c)=UTF_ERROR_قدر; \
            } \
        } \
    } وإلا إذا((strict) && !UTF_IS_UNICODE_CHAR(c)) { \
        (c)=UTF_ERROR_قدر; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_NEXT_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_ايندڙ_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا(UTF_IS_پهريون_SURROGATE(c)) { \
        (c)=UTF16_GET_PAIR_قدر((c), (s)[(i)++]); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_APPEND_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_APPEND_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0xffff) { \
        (s)[(i)++]=(uint16_t)(c); \
    } وإلا { \
        (s)[(i)++]=(uint16_t)(((c)>>10)+0xd7c0); \
        (s)[(i)++]=(uint16_t)(((c)&0x3ff)|0xdc00); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_FWD_1_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_FWD_1_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTF_IS_پهريون_SURROGATE((s)[(i)++])) { \
        ++(i); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_FWD_N_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_FWD_N_UNSAFE(s, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        UTF16_FWD_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_START_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_سيٽ_CHAR_شروعات_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTF_IS_ٻيو_SURROGATE((s)[i])) { \
        --(i); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Use U16_NEXT instead, see utf_old.h. */
#وصف_ڪريو UTF16_ايندڙ_CHAR_محفوظ(s, i, ڊيگهائي, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا(UTF_IS_پهريون_SURROGATE(c)) { \
        uint16_t __c2; \
        إذا((i)<(ڊيگهائي) && UTF_IS_ٻيو_SURROGATE(__c2=(s)[(i)])) { \
            ++(i); \
            (c)=UTF16_GET_PAIR_قدر((c), __c2); \
            /* strict: ((c)&0xfffe)==0xfffe is caught by UTF_IS_ERROR() and UTF_IS_UNICODE_CHAR() */ \
        } وإلا إذا(strict) {\
            /* unmatched first surrogate */ \
            (c)=UTF_ERROR_قدر; \
        } \
    } وإلا إذا((strict) && !UTF_IS_UNICODE_CHAR(c)) { \
        /* unmatched second surrogate or other non-character */ \
        (c)=UTF_ERROR_قدر; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Use U16_APPEND instead, see utf_old.h. */
#وصف_ڪريو UTF16_APPEND_CHAR_محفوظ(s, i, ڊيگهائي, c) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0xffff) { \
        (s)[(i)++]=(uint16_t)(c); \
    } وإلا إذا((uint32_t)(c)<=0x10ffff) { \
        إذا((i)+1<(ڊيگهائي)) { \
            (s)[(i)++]=(uint16_t)(((c)>>10)+0xd7c0); \
            (s)[(i)++]=(uint16_t)(((c)&0x3ff)|0xdc00); \
        } وإلا /* not enough space */ { \
            (s)[(i)++]=UTF_ERROR_قدر; \
        } \
    } وإلا /* c>0x10ffff, write error value */ { \
        (s)[(i)++]=UTF_ERROR_قدر; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_FWD_1, see utf_old.h. */
#وصف_ڪريو UTF16_FWD_1_محفوظ(s, i, ڊيگهائي) U16_FWD_1(s, i, ڊيگهائي)

/** @deprecated ICU 2.4. Renamed to U16_FWD_N, see utf_old.h. */
#وصف_ڪريو UTF16_FWD_N_محفوظ(s, i, ڊيگهائي, n) U16_FWD_N(s, i, ڊيگهائي, n)

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_START, see utf_old.h. */
#وصف_ڪريو UTF16_سيٽ_CHAR_شروعات_محفوظ(s, شروعات, i) U16_سيٽ_CP_شروعات(s, شروعات, i)

/** @deprecated ICU 2.4. Renamed to U16_PREV_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_PREV_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا(UTF_IS_ٻيو_SURROGATE(c)) { \
        (c)=UTF16_GET_PAIR_قدر((s)[--(i)], (c)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_BACK_1_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_پٺ_1_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTF_IS_ٻيو_SURROGATE((s)[--(i)])) { \
        --(i); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_BACK_N_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_پٺ_N_UNSAFE(s, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        UTF16_پٺ_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF16_سيٽ_CHAR_حد_بندي_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTF_IS_پهريون_SURROGATE((s)[(i)-1])) { \
        ++(i); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Use U16_PREV instead, see utf_old.h. */
#وصف_ڪريو UTF16_PREV_CHAR_محفوظ(s, شروعات, i, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا(UTF_IS_ٻيو_SURROGATE(c)) { \
        uint16_t __c2; \
        إذا((i)>(شروعات) && UTF_IS_پهريون_SURROGATE(__c2=(s)[(i)-1])) { \
            --(i); \
            (c)=UTF16_GET_PAIR_قدر(__c2, (c)); \
            /* strict: ((c)&0xfffe)==0xfffe is caught by UTF_IS_ERROR() and UTF_IS_UNICODE_CHAR() */ \
        } وإلا إذا(strict) {\
            /* unmatched second surrogate */ \
            (c)=UTF_ERROR_قدر; \
        } \
    } وإلا إذا((strict) && !UTF_IS_UNICODE_CHAR(c)) { \
        /* unmatched first surrogate or other non-character */ \
        (c)=UTF_ERROR_قدر; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Renamed to U16_BACK_1, see utf_old.h. */
#وصف_ڪريو UTF16_پٺ_1_محفوظ(s, شروعات, i) U16_پٺ_1(s, شروعات, i)

/** @deprecated ICU 2.4. Renamed to U16_BACK_N, see utf_old.h. */
#وصف_ڪريو UTF16_پٺ_N_محفوظ(s, شروعات, i, n) U16_پٺ_N(s, شروعات, i, n)

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT, see utf_old.h. */
#وصف_ڪريو UTF16_سيٽ_CHAR_حد_بندي_محفوظ(s, شروعات, i, ڊيگهائي) U16_سيٽ_CP_حد_بندي(s, شروعات, i, ڊيگهائي)

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
#وصف_ڪريو UTF32_IS_محفوظ(c, strict) \
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
#وصف_ڪريو UTF32_IS_SINGLE(uchar) 1
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_IS_LEAD(uchar) 0
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_IS_TRAIL(uchar) 0

/* number of code units per code point */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_NEED_گڻو_UCHAR(c) 0
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_CHAR_ڊيگهائي(c) 1
/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_MAX_CHAR_ڊيگهائي 1

/* average number of code units compared to UTF-16 */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_ARRAY_سائيز(سائيز_2) (سائيز_2)

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_GET_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[i]; \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_GET_CHAR_محفوظ(s, شروعات, i, ڊيگهائي, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[i]; \
    إذا(!UTF32_IS_محفوظ(c, strict)) { \
        (c)=UTF_ERROR_قدر; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/* definitions with forward iteration --------------------------------------- */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_ايندڙ_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_APPEND_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (s)[(i)++]=(c); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_FWD_1_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    ++(i); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_FWD_N_UNSAFE(s, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    (i)+=(n); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_سيٽ_CHAR_شروعات_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_ايندڙ_CHAR_محفوظ(s, i, ڊيگهائي, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[(i)++]; \
    إذا(!UTF32_IS_محفوظ(c, strict)) { \
        (c)=UTF_ERROR_قدر; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_APPEND_CHAR_محفوظ(s, i, ڊيگهائي, c) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((uint32_t)(c)<=0x10ffff) { \
        (s)[(i)++]=(c); \
    } وإلا /* c>0x10ffff, write 0xfffd */ { \
        (s)[(i)++]=0xfffd; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_FWD_1_محفوظ(s, i, ڊيگهائي) UPRV_بلاڪ_MACRO_BEGIN { \
    ++(i); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_FWD_N_محفوظ(s, i, ڊيگهائي, n) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(((i)+=(n))>(ڊيگهائي)) { \
        (i)=(ڊيگهائي); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_سيٽ_CHAR_شروعات_محفوظ(s, شروعات, i) UPRV_بلاڪ_MACRO_BEGIN { \
} UPRV_بلاڪ_MACRO_پڇاڙي

/* definitions with backward iteration -------------------------------------- */

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_PREV_CHAR_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_پٺ_1_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    --(i); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_پٺ_N_UNSAFE(s, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    (i)-=(n); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_سيٽ_CHAR_حد_بندي_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_PREV_CHAR_محفوظ(s, شروعات, i, c, strict) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(s)[--(i)]; \
    إذا(!UTF32_IS_محفوظ(c, strict)) { \
        (c)=UTF_ERROR_قدر; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_پٺ_1_محفوظ(s, شروعات, i) UPRV_بلاڪ_MACRO_BEGIN { \
    --(i); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_پٺ_N_محفوظ(s, شروعات, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    (i)-=(n); \
    إذا((i)<(شروعات)) { \
        (i)=(شروعات); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/** @deprecated ICU 2.4. Obsolete, see utf_old.h. */
#وصف_ڪريو UTF32_سيٽ_CHAR_حد_بندي_محفوظ(s, i, ڊيگهائي) UPRV_بلاڪ_MACRO_BEGIN { \
} UPRV_بلاڪ_MACRO_پڇاڙي

/* Formerly utf.h, part 2 --------------------------------------------------- */

/**
 * Estimate the number of code units for a string based on the number of UTF-16 code units.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#وصف_ڪريو UTF_ARRAY_سائيز(سائيز_2) UTF16_ARRAY_سائيز(سائيز_2)

/** @deprecated ICU 2.4. Renamed to U16_GET_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_GET_CHAR_UNSAFE(s, i, c)                 UTF16_GET_CHAR_UNSAFE(s, i, c)

/** @deprecated ICU 2.4. Use U16_GET instead, see utf_old.h. */
#وصف_ڪريو UTF_GET_CHAR_محفوظ(s, شروعات, i, ڊيگهائي, c, strict) UTF16_GET_CHAR_محفوظ(s, شروعات, i, ڊيگهائي, c, strict)


/** @deprecated ICU 2.4. Renamed to U16_NEXT_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_ايندڙ_CHAR_UNSAFE(s, i, c)                UTF16_ايندڙ_CHAR_UNSAFE(s, i, c)

/** @deprecated ICU 2.4. Use U16_NEXT instead, see utf_old.h. */
#وصف_ڪريو UTF_ايندڙ_CHAR_محفوظ(s, i, ڊيگهائي, c, strict)  UTF16_ايندڙ_CHAR_محفوظ(s, i, ڊيگهائي, c, strict)


/** @deprecated ICU 2.4. Renamed to U16_APPEND_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_APPEND_CHAR_UNSAFE(s, i, c)              UTF16_APPEND_CHAR_UNSAFE(s, i, c)

/** @deprecated ICU 2.4. Use U16_APPEND instead, see utf_old.h. */
#وصف_ڪريو UTF_APPEND_CHAR_محفوظ(s, i, ڊيگهائي, c)        UTF16_APPEND_CHAR_محفوظ(s, i, ڊيگهائي, c)


/** @deprecated ICU 2.4. Renamed to U16_FWD_1_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_FWD_1_UNSAFE(s, i)                       UTF16_FWD_1_UNSAFE(s, i)

/** @deprecated ICU 2.4. Renamed to U16_FWD_1, see utf_old.h. */
#وصف_ڪريو UTF_FWD_1_محفوظ(s, i, ڊيگهائي)                 UTF16_FWD_1_محفوظ(s, i, ڊيگهائي)


/** @deprecated ICU 2.4. Renamed to U16_FWD_N_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_FWD_N_UNSAFE(s, i, n)                    UTF16_FWD_N_UNSAFE(s, i, n)

/** @deprecated ICU 2.4. Renamed to U16_FWD_N, see utf_old.h. */
#وصف_ڪريو UTF_FWD_N_محفوظ(s, i, ڊيگهائي, n)              UTF16_FWD_N_محفوظ(s, i, ڊيگهائي, n)


/** @deprecated ICU 2.4. Renamed to U16_SET_CP_START_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_سيٽ_CHAR_شروعات_UNSAFE(s, i)              UTF16_سيٽ_CHAR_شروعات_UNSAFE(s, i)

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_START, see utf_old.h. */
#وصف_ڪريو UTF_سيٽ_CHAR_شروعات_محفوظ(s, شروعات, i)         UTF16_سيٽ_CHAR_شروعات_محفوظ(s, شروعات, i)


/** @deprecated ICU 2.4. Renamed to U16_PREV_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_PREV_CHAR_UNSAFE(s, i, c)                UTF16_PREV_CHAR_UNSAFE(s, i, c)

/** @deprecated ICU 2.4. Use U16_PREV instead, see utf_old.h. */
#وصف_ڪريو UTF_PREV_CHAR_محفوظ(s, شروعات, i, c, strict)   UTF16_PREV_CHAR_محفوظ(s, شروعات, i, c, strict)


/** @deprecated ICU 2.4. Renamed to U16_BACK_1_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_پٺ_1_UNSAFE(s, i)                      UTF16_پٺ_1_UNSAFE(s, i)

/** @deprecated ICU 2.4. Renamed to U16_BACK_1, see utf_old.h. */
#وصف_ڪريو UTF_پٺ_1_محفوظ(s, شروعات, i)                 UTF16_پٺ_1_محفوظ(s, شروعات, i)


/** @deprecated ICU 2.4. Renamed to U16_BACK_N_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_پٺ_N_UNSAFE(s, i, n)                   UTF16_پٺ_N_UNSAFE(s, i, n)

/** @deprecated ICU 2.4. Renamed to U16_BACK_N, see utf_old.h. */
#وصف_ڪريو UTF_پٺ_N_محفوظ(s, شروعات, i, n)              UTF16_پٺ_N_محفوظ(s, شروعات, i, n)


/** @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT_UNSAFE, see utf_old.h. */
#وصف_ڪريو UTF_سيٽ_CHAR_حد_بندي_UNSAFE(s, i)              UTF16_سيٽ_CHAR_حد_بندي_UNSAFE(s, i)

/** @deprecated ICU 2.4. Renamed to U16_SET_CP_LIMIT, see utf_old.h. */
#وصف_ڪريو UTF_سيٽ_CHAR_حد_بندي_محفوظ(s, شروعات, i, ڊيگهائي) UTF16_سيٽ_CHAR_حد_بندي_محفوظ(s, شروعات, i, ڊيگهائي)

/* Define default macros (UTF-16 "safe") ------------------------------------ */

/**
 * Does this code unit alone encode a code point (BMP, not a surrogate)?
 * Same as UTF16_IS_SINGLE.
 * @deprecated ICU 2.4. Renamed to U_IS_SINGLE and U16_IS_SINGLE, see utf_old.h.
 */
#وصف_ڪريو UTF_IS_SINGLE(uchar) U16_IS_SINGLE(uchar)

/**
 * Is this code unit the first one of several (a lead surrogate)?
 * Same as UTF16_IS_LEAD.
 * @deprecated ICU 2.4. Renamed to U_IS_LEAD and U16_IS_LEAD, see utf_old.h.
 */
#وصف_ڪريو UTF_IS_LEAD(uchar) U16_IS_LEAD(uchar)

/**
 * Is this code unit one of several but not the first one (a trail surrogate)?
 * Same as UTF16_IS_TRAIL.
 * @deprecated ICU 2.4. Renamed to U_IS_TRAIL and U16_IS_TRAIL, see utf_old.h.
 */
#وصف_ڪريو UTF_IS_TRAIL(uchar) U16_IS_TRAIL(uchar)

/**
 * Does this code point require multiple code units (is it a supplementary code point)?
 * Same as UTF16_NEED_MULTIPLE_UCHAR.
 * @deprecated ICU 2.4. Use U16_LENGTH or test ((uint32_t)(c)>0xffff) instead.
 */
#وصف_ڪريو UTF_NEED_گڻو_UCHAR(c) UTF16_NEED_گڻو_UCHAR(c)

/**
 * How many code units are used to encode this code point (1 or 2)?
 * Same as UTF16_CHAR_LENGTH.
 * @deprecated ICU 2.4. Renamed to U16_LENGTH, see utf_old.h.
 */
#وصف_ڪريو UTF_CHAR_ڊيگهائي(c) U16_ڊيگهائي(c)

/**
 * How many code units are used at most for any Unicode code point (2)?
 * Same as UTF16_MAX_CHAR_LENGTH.
 * @deprecated ICU 2.4. Renamed to U16_MAX_LENGTH, see utf_old.h.
 */
#وصف_ڪريو UTF_MAX_CHAR_ڊيگهائي U16_MAX_ڊيگهائي

/**
 * Set c to the code point that contains the code unit i.
 * i could point to the lead or the trail surrogate for the code point.
 * i is not modified.
 * Same as UTF16_GET_CHAR.
 * \pre 0<=i<length
 *
 * @deprecated ICU 2.4. Renamed to U16_GET, see utf_old.h.
 */
#وصف_ڪريو UTF_GET_CHAR(s, شروعات, i, ڊيگهائي, c) U16_GET(s, شروعات, i, ڊيگهائي, c)

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
#وصف_ڪريو UTF_ايندڙ_CHAR(s, i, ڊيگهائي, c) U16_ايندڙ(s, i, ڊيگهائي, c)

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
#وصف_ڪريو UTF_APPEND_CHAR(s, i, ڊيگهائي, c) UTF16_APPEND_CHAR_محفوظ(s, i, ڊيگهائي, c)

/**
 * Advance i to beyond the code units of the code point that begins at i.
 * I.e., advance i by one code point.
 * Same as UTF16_FWD_1.
 * \pre 0<=i<length
 * \post 0<i<=length
 *
 * @deprecated ICU 2.4. Renamed to U16_FWD_1, see utf_old.h.
 */
#وصف_ڪريو UTF_FWD_1(s, i, ڊيگهائي) U16_FWD_1(s, i, ڊيگهائي)

/**
 * Advance i to beyond the code units of the n code points where the first one begins at i.
 * I.e., advance i by n code points.
 * Same as UT16_FWD_N.
 * \pre 0<=i<length
 * \post 0<i<=length
 *
 * @deprecated ICU 2.4. Renamed to U16_FWD_N, see utf_old.h.
 */
#وصف_ڪريو UTF_FWD_N(s, i, ڊيگهائي, n) U16_FWD_N(s, i, ڊيگهائي, n)

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
#وصف_ڪريو UTF_سيٽ_CHAR_شروعات(s, شروعات, i) U16_سيٽ_CP_شروعات(s, شروعات, i)

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
#وصف_ڪريو UTF_PREV_CHAR(s, شروعات, i, c) U16_PREV(s, شروعات, i, c)

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
#وصف_ڪريو UTF_پٺ_1(s, شروعات, i) U16_پٺ_1(s, شروعات, i)

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
#وصف_ڪريو UTF_پٺ_N(s, شروعات, i, n) U16_پٺ_N(s, شروعات, i, n)

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
#وصف_ڪريو UTF_سيٽ_CHAR_حد_بندي(s, شروعات, i, ڊيگهائي) U16_سيٽ_CP_حد_بندي(s, شروعات, i, ڊيگهائي)

#جيڪڏهن_پورو  // !U_HIDE_DEPRECATED_API && !U_HIDE_OBSOLETE_UTF_OLD_H

#جيڪڏهن_پورو
