// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2001-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  utrie2.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2008aug16 (starting from a copy of utrie.h)
*   created by: Markus W. Scherer
*/

#اگر_تعریف_نشده __UTRIE2_H__
#تعریف_کن __UTRIE2_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/utf8.h"
#ضمن "putilimp.h"

U_CDECL_BEGIN

بنية UTrie;  /* forward declaration */
#اگر_تعریف_نشده __UTRIE_H__
تعريفنوع بنية UTrie UTrie;
#پایان_اگر

/**
 * \file
 *
 * This is a common implementation of a Unicode trie.
 * It is a kind of compressed, serializable table of 16- or 32-bit values associated with
 * Unicode code points (0..0x10ffff). (A map from code points to integers.)
 *
 * This is the second common version of a Unicode trie (hence the name UTrie2).
 * Compared with UTrie version 1:
 * - Still splitting BMP code points 11:5 bits for index and data table lookups.
 * - Still separate data for lead surrogate code _units_ vs. code _points_,
 *   but the lead surrogate code unit values are not required any more
 *   for data lookup for supplementary code points.
 * - The "folding" mechanism is removed. In UTrie version 1, this somewhat
 *   hard-to-explain mechanism was meant to be used for optimized UTF-16
 *   processing, with application-specific encoding of indexing bits
 *   in the lead surrogate data for the associated supplementary code points.
 * - For the last single-value code point range (ending with U+10ffff),
 *   the starting code point ("highStart") and the value are stored.
 * - For supplementary code points U+10000..highStart-1 a three-table lookup
 *   (two index tables and one data table) is used. The first index
 *   is truncated, omitting both the BMP portion and the high range.
 * - There is a special small index for 2-byte UTF-8, and the initial data
 *   entries are designed for fast 1/2-byte UTF-8 lookup.
 *   Starting with ICU 60, C0 and C1 are not recognized as UTF-8 lead bytes any more at all,
 *   and the associated 2-byte indexes are unused.
 */

/**
 * Trie structure.
 * Use only with public API macros and functions.
 */
بنية UTrie2;
تعريفنوع بنية UTrie2 UTrie2;

/* Public UTrie2 API functions: read-only access ---------------------------- */

/**
 * Selectors for the width of a UTrie2 data value.
 */
تعداد UTrie2_مقدار_بیتها {
    /** 16 bits per UTrie2 data value. */
    UTRIE2_16_مقدار_بیتها,
    /** 32 bits per UTrie2 data value. */
    UTRIE2_32_مقدار_بیتها,
    /** Number of selectors for the width of UTrie2 data values. */
    UTRIE2_شمار_مقدار_بیتها
};
تعريفنوع تعداد UTrie2_مقدار_بیتها UTrie2_مقدار_بیتها;

/**
 * Open a frozen trie from its serialized from, stored in 32-bit-aligned memory.
 * Inverse of utrie2_serialize().
 * The memory must remain valid and unchanged as long as the trie is used.
 * You must utrie2_close() the trie once you are done using it.
 *
 * @param valueBits selects the data entry size; results in an
 *                  U_INVALID_FORMAT_ERROR if it does not match the serialized form
 * @param data a pointer to 32-bit-aligned memory containing the serialized form of a UTrie2
 * @param length the number of bytes available at data;
 *               can be more than necessary
 * @param pActualLength receives the actual number of bytes at data taken up by the trie data;
 *                      can be NULL
 * @param pErrorCode an in/out ICU UErrorCode
 * @return the unserialized trie
 *
 * @see utrie2_open
 * @see utrie2_serialize
 */
U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_مقدار_بیتها مقدار_بیتها,
                          ثابت فراغ *داده, int32_t درازی, int32_t *p_Actual_درازی,
                          UError_کد *p_Error_کد);

/**
 * Open a frozen, empty "dummy" trie.
 * A dummy trie is an empty trie, used when a real data trie cannot
 * be loaded. Equivalent to calling utrie2_open() and utrie2_freeze(),
 * but without internally creating and compacting/serializing the
 * builder data structure.
 *
 * The trie always returns the initialValue,
 * or the errorValue for out-of-range code points and illegal UTF-8.
 *
 * You must utrie2_close() the trie once you are done using it.
 *
 * @param valueBits selects the data entry size
 * @param initialValue the initial value that is set for all code points
 * @param errorValue the value for out-of-range code points and illegal UTF-8
 * @param pErrorCode an in/out ICU UErrorCode
 * @return the dummy trie
 *
 * @see utrie2_openFromSerialized
 * @see utrie2_open
 */
U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_مقدار_بیتها مقدار_بیتها,
                 uint32_t initial_مقدار, uint32_t error_مقدار,
                 UError_کد *p_Error_کد);

/**
 * Get a value from a code point as stored in the trie.
 * Easier to use than UTRIE2_GET16() and UTRIE2_GET32() but slower.
 * Easier to use because, unlike the macros, this function works on all UTrie2
 * objects, frozen or not, holding 16-bit or 32-bit data values.
 *
 * @param trie the trie
 * @param c the code point
 * @return the value
 */
U_CAPI uint32_t U_EXPORT2
utrie2_get32(ثابت UTrie2 *trie, UChar32 c);

/* enumeration callback types */

/**
 * Callback from utrie2_enum(), extracts a uint32_t value from a
 * trie value. This value will be passed on to the UTrie2EnumRange function.
 *
 * @param context an opaque pointer, as passed into utrie2_enum()
 * @param value a value from the trie
 * @return the value that is to be passed on to the UTrie2EnumRange function
 */
تعريفنوع uint32_t U_CALLCONV
UTrie2_Enum_مقدار(ثابت فراغ *context, uint32_t مقدار_2);

/**
 * Callback from utrie2_enum(), is called for each contiguous range
 * of code points with the same value as retrieved from the trie and
 * transformed by the UTrie2EnumValue function.
 *
 * The callback function can stop the enumeration by returning false.
 *
 * @param context an opaque pointer, as passed into utrie2_enum()
 * @param start the first code point in a contiguous range with value
 * @param end the last code point in a contiguous range with value (inclusive)
 * @param value the value that is set for all code points in [start..end]
 * @return false to stop the enumeration
 */
تعريفنوع UBool U_CALLCONV
UTrie2EnumRange(ثابت فراغ *context, UChar32 شروع, UChar32 انتها, uint32_t مقدار_2);

/**
 * Enumerate efficiently all values in a trie.
 * Do not modify the trie during the enumeration.
 *
 * For each entry in the trie, the value to be delivered is passed through
 * the UTrie2EnumValue function.
 * The value is unchanged if that function pointer is NULL.
 *
 * For each contiguous range of code points with a given (transformed) value,
 * the UTrie2EnumRange function is called.
 *
 * @param trie a pointer to the trie
 * @param enumValue a pointer to a function that may transform the trie entry value,
 *                  or NULL if the values from the trie are to be used directly
 * @param enumRange a pointer to a function that is called for each contiguous range
 *                  of code points with the same (transformed) value
 * @param context an opaque pointer that is passed on to the callback functions
 */
U_CAPI فراغ U_EXPORT2
utrie2_enum(ثابت UTrie2 *trie,
            UTrie2_Enum_مقدار *enum_مقدار, UTrie2EnumRange *enumRange, ثابت فراغ *context);

/* Building a trie ---------------------------------------------------------- */

/**
 * Open an empty, writable trie. At build time, 32-bit data values are used.
 * utrie2_freeze() takes a valueBits parameter
 * which determines the data value width in the serialized and frozen forms.
 * You must utrie2_close() the trie once you are done using it.
 *
 * @param initialValue the initial value that is set for all code points
 * @param errorValue the value for out-of-range code points and illegal UTF-8
 * @param pErrorCode an in/out ICU UErrorCode
 * @return a pointer to the allocated and initialized new trie
 */
U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_مقدار, uint32_t error_مقدار, UError_کد *p_Error_کد);

/**
 * Clone a trie.
 * You must utrie2_close() the clone once you are done using it.
 *
 * @param other the trie to clone
 * @param pErrorCode an in/out ICU UErrorCode
 * @return a pointer to the new trie clone
 */
U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ثابت UTrie2 *other, UError_کد *p_Error_کد);

/**
 * Clone a trie. The clone will be mutable/writable even if the other trie
 * is frozen. (See utrie2_freeze().)
 * You must utrie2_close() the clone once you are done using it.
 *
 * @param other the trie to clone
 * @param pErrorCode an in/out ICU UErrorCode
 * @return a pointer to the new trie clone
 */
U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ثابت UTrie2 *other, UError_کد *p_Error_کد);

/**
 * Close a trie and release associated memory.
 *
 * @param trie the trie
 */
U_CAPI فراغ U_EXPORT2
utrie2_close(UTrie2 *trie);

/**
 * Set a value for a code point.
 *
 * @param trie the unfrozen trie
 * @param c the code point
 * @param value the value
 * @param pErrorCode an in/out ICU UErrorCode; among other possible error codes:
 * - U_NO_WRITE_PERMISSION if the trie is frozen
 */
U_CAPI فراغ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t مقدار_2, UError_کد *p_Error_کد);

/**
 * Set a value in a range of code points [start..end].
 * All code points c with start<=c<=end will get the value if
 * overwrite is true or if the old value is the initial value.
 *
 * @param trie the unfrozen trie
 * @param start the first code point to get the value
 * @param end the last code point to get the value (inclusive)
 * @param value the value
 * @param overwrite flag for whether old non-initial values are to be overwritten
 * @param pErrorCode an in/out ICU UErrorCode; among other possible error codes:
 * - U_NO_WRITE_PERMISSION if the trie is frozen
 */
U_CAPI فراغ U_EXPORT2
utrie2_مجموعه_Range32(UTrie2 *trie,
                  UChar32 شروع, UChar32 انتها,
                  uint32_t مقدار_2, UBool overwrite,
                  UError_کد *p_Error_کد);

/**
 * Freeze a trie. Make it immutable (read-only) and compact it,
 * ready for serialization and for use with fast macros.
 * Functions to set values will fail after serializing.
 *
 * A trie can be frozen only once. If this function is called again with different
 * valueBits then it will set a U_ILLEGAL_ARGUMENT_ERROR.
 *
 * @param trie the trie
 * @param valueBits selects the data entry size; if smaller than 32 bits, then
 *                  the values stored in the trie will be truncated
 * @param pErrorCode an in/out ICU UErrorCode; among other possible error codes:
 * - U_INDEX_OUTOFBOUNDS_ERROR if the compacted index or data arrays are too long
 *                             for serialization
 *                             (the trie will be immutable and usable,
 *                             but not frozen and not usable with the fast macros)
 *
 * @see utrie2_cloneAsThawed
 */
U_CAPI فراغ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_مقدار_بیتها مقدار_بیتها, UError_کد *p_Error_کد);

/**
 * Test if the trie is frozen. (See utrie2_freeze().)
 *
 * @param trie the trie
 * @return true if the trie is frozen, that is, immutable, ready for serialization
 *         and for use with fast macros
 */
U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ثابت UTrie2 *trie);

/**
 * Serialize a frozen trie into 32-bit aligned memory.
 * If the trie is not frozen, then the function returns with a U_ILLEGAL_ARGUMENT_ERROR.
 * A trie can be serialized multiple times.
 *
 * @param trie the frozen trie
 * @param data a pointer to 32-bit-aligned memory to be filled with the trie data,
 *             can be NULL if capacity==0
 * @param capacity the number of bytes available at data,
 *                 or 0 for preflighting
 * @param pErrorCode an in/out ICU UErrorCode; among other possible error codes:
 * - U_BUFFER_OVERFLOW_ERROR if the data storage block is too small for serialization
 * - U_ILLEGAL_ARGUMENT_ERROR if the trie is not frozen or the data and capacity
 *                            parameters are bad
 * @return the number of bytes written or needed for the trie
 *
 * @see utrie2_openFromSerialized()
 */
U_CAPI int32_t U_EXPORT2
utrie2_serialize(ثابت UTrie2 *trie,
                 فراغ *داده, int32_t ظرفیت,
                 UError_کد *p_Error_کد);

/* Public UTrie2 API: miscellaneous functions ------------------------------- */

/**
 * Build a UTrie2 (version 2) from a UTrie (version 1).
 * Enumerates all values in the UTrie and builds a UTrie2 with the same values.
 * The resulting UTrie2 will be frozen.
 *
 * @param trie1 the runtime UTrie structure to be enumerated
 * @param errorValue the value for out-of-range code points and illegal UTF-8
 * @param pErrorCode an in/out ICU UErrorCode
 * @return The frozen UTrie2 with the same values as the UTrie.
 */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ثابت UTrie *trie1, uint32_t error_مقدار, UError_کد *p_Error_کد);

/* Public UTrie2 API macros ------------------------------------------------- */

/*
 * These macros provide fast data lookup from a frozen trie.
 * They will crash when used on an unfrozen trie.
 */

/**
 * Return a 16-bit trie value from a code point, with range checking.
 * Returns trie->errorValue if c is not in the range 0..U+10ffff.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code point
 * @return (uint16_t) The code point's trie value.
 */
#تعریف_کن UTRIE2_GET16(trie, c) _UTRIE2_GET((trie), شاخص, (trie)->شاخص_درازی, (c))

/**
 * Return a 32-bit trie value from a code point, with range checking.
 * Returns trie->errorValue if c is not in the range 0..U+10ffff.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code point
 * @return (uint32_t) The code point's trie value.
 */
#تعریف_کن UTRIE2_GET32(trie, c) _UTRIE2_GET((trie), data32, 0, (c))

/**
 * UTF-16: Get the next code point (UChar32 c, out), post-increment src,
 * and get a 16-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param src (const UChar *, in/out) the source text pointer
 * @param limit (const UChar *, in) the limit pointer for the text, or NULL if NUL-terminated
 * @param c (UChar32, out) variable for the code point
 * @param result (uint16_t, out) uint16_t variable for the trie lookup result
 */
#تعریف_کن UTRIE2_U16_NEXT16(trie, src, حد, c, نتیجه) UTRIE2_U16_بعدی(trie, شاخص, src, حد, c, نتیجه)

/**
 * UTF-16: Get the next code point (UChar32 c, out), post-increment src,
 * and get a 32-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param src (const UChar *, in/out) the source text pointer
 * @param limit (const UChar *, in) the limit pointer for the text, or NULL if NUL-terminated
 * @param c (UChar32, out) variable for the code point
 * @param result (uint32_t, out) uint32_t variable for the trie lookup result
 */
#تعریف_کن UTRIE2_U16_NEXT32(trie, src, حد, c, نتیجه) UTRIE2_U16_بعدی(trie, data32, src, حد, c, نتیجه)

/**
 * UTF-16: Get the previous code point (UChar32 c, out), pre-decrement src,
 * and get a 16-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param start (const UChar *, in) the start pointer for the text
 * @param src (const UChar *, in/out) the source text pointer
 * @param c (UChar32, out) variable for the code point
 * @param result (uint16_t, out) uint16_t variable for the trie lookup result
 */
#تعریف_کن UTRIE2_U16_PREV16(trie, شروع, src, c, نتیجه) _UTRIE2_U16_PREV(trie, شاخص, شروع, src, c, نتیجه)

/**
 * UTF-16: Get the previous code point (UChar32 c, out), pre-decrement src,
 * and get a 32-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param start (const UChar *, in) the start pointer for the text
 * @param src (const UChar *, in/out) the source text pointer
 * @param c (UChar32, out) variable for the code point
 * @param result (uint32_t, out) uint32_t variable for the trie lookup result
 */
#تعریف_کن UTRIE2_U16_PREV32(trie, شروع, src, c, نتیجه) _UTRIE2_U16_PREV(trie, data32, شروع, src, c, نتیجه)

/**
 * UTF-8: Post-increment src and get a 16-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param src (const char *, in/out) the source text pointer
 * @param limit (const char *, in) the limit pointer for the text (must not be NULL)
 * @param result (uint16_t, out) uint16_t variable for the trie lookup result
 */
#تعریف_کن UTRIE2_U8_NEXT16(trie, src, حد, نتیجه)\
    UTRIE2_U8_بعدی(trie, data16, شاخص, src, حد, نتیجه)

/**
 * UTF-8: Post-increment src and get a 32-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param src (const char *, in/out) the source text pointer
 * @param limit (const char *, in) the limit pointer for the text (must not be NULL)
 * @param result (uint16_t, out) uint32_t variable for the trie lookup result
 */
#تعریف_کن UTRIE2_U8_NEXT32(trie, src, حد, نتیجه) \
    UTRIE2_U8_بعدی(trie, data32, data32, src, حد, نتیجه)

/**
 * UTF-8: Pre-decrement src and get a 16-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param start (const char *, in) the start pointer for the text
 * @param src (const char *, in/out) the source text pointer
 * @param result (uint16_t, out) uint16_t variable for the trie lookup result
 */
#تعریف_کن UTRIE2_U8_PREV16(trie, شروع, src, نتیجه) \
    _UTRIE2_U8_PREV(trie, data16, شاخص, شروع, src, نتیجه)

/**
 * UTF-8: Pre-decrement src and get a 32-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param start (const char *, in) the start pointer for the text
 * @param src (const char *, in/out) the source text pointer
 * @param result (uint16_t, out) uint32_t variable for the trie lookup result
 */
#تعریف_کن UTRIE2_U8_PREV32(trie, شروع, src, نتیجه) \
    _UTRIE2_U8_PREV(trie, data32, data32, شروع, src, نتیجه)

/* Public UTrie2 API: optimized UTF-16 access ------------------------------- */

/*
 * The following functions and macros are used for highly optimized UTF-16
 * text processing. The UTRIE2_U16_NEXTxy() macros do not depend on these.
 *
 * A UTrie2 stores separate values for lead surrogate code _units_ vs. code _points_.
 * UTF-16 text processing can be optimized by detecting surrogate pairs and
 * assembling supplementary code points only when there is non-trivial data
 * available.
 *
 * At build-time, use utrie2_enumForLeadSurrogate() to see if there
 * is non-trivial (non-initialValue) data for any of the supplementary
 * code points associated with a lead surrogate.
 * If so, then set a special (application-specific) value for the
 * lead surrogate code _unit_, with utrie2_set32ForLeadSurrogateCodeUnit().
 *
 * At runtime, use UTRIE2_GET16_FROM_U16_SINGLE_LEAD() or
 * UTRIE2_GET32_FROM_U16_SINGLE_LEAD() per code unit. If there is non-trivial
 * data and the code unit is a lead surrogate, then check if a trail surrogate
 * follows. If so, assemble the supplementary code point with
 * U16_GET_SUPPLEMENTARY() and look up its value with UTRIE2_GET16_FROM_SUPP()
 * or UTRIE2_GET32_FROM_SUPP(); otherwise reset the lead
 * surrogate's value or do a code point lookup for it.
 *
 * If there is only trivial data for lead and trail surrogates, then processing
 * can often skip them. For example, in normalization or case mapping
 * all characters that do not have any mappings are simply copied as is.
 */

/**
 * Get a value from a lead surrogate code unit as stored in the trie.
 *
 * @param trie the trie
 * @param c the code unit (U+D800..U+DBFF)
 * @return the value
 */
U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_کد_Unit(ثابت UTrie2 *trie, UChar32 c);

/**
 * Enumerate the trie values for the 1024=0x400 code points
 * corresponding to a given lead surrogate.
 * For example, for the lead surrogate U+D87E it will enumerate the values
 * for [U+2F800..U+2FC00[.
 * Used by data builder code that sets special lead surrogate code unit values
 * for optimized UTF-16 string processing.
 *
 * Do not modify the trie during the enumeration.
 *
 * Except for the limited code point range, this functions just like utrie2_enum():
 * For each entry in the trie, the value to be delivered is passed through
 * the UTrie2EnumValue function.
 * The value is unchanged if that function pointer is NULL.
 *
 * For each contiguous range of code points with a given (transformed) value,
 * the UTrie2EnumRange function is called.
 *
 * @param trie a pointer to the trie
 * @param enumValue a pointer to a function that may transform the trie entry value,
 *                  or NULL if the values from the trie are to be used directly
 * @param enumRange a pointer to a function that is called for each contiguous range
 *                  of code points with the same (transformed) value
 * @param context an opaque pointer that is passed on to the callback functions
 */
U_CAPI فراغ U_EXPORT2
utrie2_enumForLeadSurrogate(ثابت UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_مقدار *enum_مقدار, UTrie2EnumRange *enumRange,
                            ثابت فراغ *context);

/**
 * Set a value for a lead surrogate code unit.
 *
 * @param trie the unfrozen trie
 * @param lead the lead surrogate code unit (U+D800..U+DBFF)
 * @param value the value
 * @param pErrorCode an in/out ICU UErrorCode; among other possible error codes:
 * - U_NO_WRITE_PERMISSION if the trie is frozen
 */
U_CAPI فراغ U_EXPORT2
utrie2_set32_For_Lead_Surrogate_کد_Unit(UTrie2 *trie,
                                     UChar32 lead, uint32_t مقدار_2,
                                     UError_کد *p_Error_کد);

/**
 * Return a 16-bit trie value from a UTF-16 single/lead code unit (<=U+ffff).
 * Same as UTRIE2_GET16() if c is a BMP code point except for lead surrogates,
 * but smaller and faster.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code unit, must be 0<=c<=U+ffff
 * @return (uint16_t) The code unit's trie value.
 */
#تعریف_کن UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c) _UTRIE2_GET_FROM_U16_SINGLE_LEAD((trie), شاخص, c)

/**
 * Return a 32-bit trie value from a UTF-16 single/lead code unit (<=U+ffff).
 * Same as UTRIE2_GET32() if c is a BMP code point except for lead surrogates,
 * but smaller and faster.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code unit, must be 0<=c<=U+ffff
 * @return (uint32_t) The code unit's trie value.
 */
#تعریف_کن UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c) _UTRIE2_GET_FROM_U16_SINGLE_LEAD((trie), data32, c)

/**
 * Return a 16-bit trie value from a supplementary code point (U+10000..U+10ffff).
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code point, must be U+10000<=c<=U+10ffff
 * @return (uint16_t) The code point's trie value.
 */
#تعریف_کن UTRIE2_GET16_FROM_SUPP(trie, c) _UTRIE2_GET_FROM_SUPP((trie), شاخص, c)

/**
 * Return a 32-bit trie value from a supplementary code point (U+10000..U+10ffff).
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code point, must be U+10000<=c<=U+10ffff
 * @return (uint32_t) The code point's trie value.
 */
#تعریف_کن UTRIE2_GET32_FROM_SUPP(trie, c) _UTRIE2_GET_FROM_SUPP((trie), data32, c)

U_CDECL_انتها

/* C++ convenience wrappers ------------------------------------------------- */

#اگر_تعریف_شده __cplusplus

#ضمن "unicode/uobject.h"
#ضمن "unicode/utf.h"

U_NAMESPACE_BEGIN

// Use the Forward/Backward subclasses below.
class UTrie2StringIterator : عام UMemory {
عام:
    UTrie2StringIterator(ثابت UTrie2 *t, ثابت char16_t *p) :
        trie(t), کد_نقطه_شروع(p), کد_نقطه_حد(p), کد_نقطه(U_SENTINEL) {}

    ثابت UTrie2 *trie;
    ثابت char16_t *کد_نقطه_شروع, *کد_نقطه_حد;
    UChar32 کد_نقطه;
};

class BackwardUTrie2StringIterator : عام UTrie2StringIterator {
عام:
    BackwardUTrie2StringIterator(ثابت UTrie2 *t, ثابت char16_t *s, ثابت char16_t *p) :
        UTrie2StringIterator(t, p), شروع(s) {}

    uint16_t previous16();

    ثابت char16_t *شروع;
};

class ForwardUTrie2StringIterator : عام UTrie2StringIterator {
عام:
    // Iteration limit l can be nullptr.
    // In that case, the caller must detect c==0 and stop.
    ForwardUTrie2StringIterator(ثابت UTrie2 *t, ثابت char16_t *p, ثابت char16_t *l) :
        UTrie2StringIterator(t, p), حد(l) {}

    uint16_t next16();

    ثابت char16_t *حد;
};

U_NAMESPACE_انتها

#پایان_اگر

/* Internal definitions ----------------------------------------------------- */

U_CDECL_BEGIN

/** Build-time trie structure. */
بنية UNewTrie2;
تعريفنوع بنية UNewTrie2 UNewTrie2;

/*
 * Trie structure definition.
 *
 * Either the data table is 16 bits wide and accessed via the index
 * pointer, with each index item increased by indexLength;
 * in this case, data32==NULL, and data16 is used for direct ASCII access.
 *
 * Or the data table is 32 bits wide and accessed via the data32 pointer.
 */
بنية UTrie2 {
    /* protected: used by macros and functions for reading values */
    ثابت uint16_t *شاخص;
    ثابت uint16_t *data16;     /* for fast UTF-8 ASCII access, if 16b data */
    ثابت uint32_t *data32;     /* NULL if 16b data is used via index */

    int32_t شاخص_درازی, داده_درازی;
    uint16_t index2_Null_جابه_جایی;  /* 0xffff if there is no dedicated index-2 null block */
    uint16_t داده_Null_جابه_جایی;
    uint32_t initial_مقدار;
    /** Value returned for out-of-range code points and illegal UTF-8. */
    uint32_t error_مقدار;

    /* Start of the last range which ends at U+10ffff, and its value. */
    UChar32 بالا_شروع;
    int32_t بالا_مقدار_شاخص;

    /* private: used by builder and unserialization functions */
    فراغ *حافظه;           /* serialized bytes; NULL if not frozen yet */
    int32_t درازی;         /* number of serialized bytes at memory; 0 if not frozen yet */
    UBool is_حافظه_Owned;    /* true if the trie owns the memory */
    UBool padding1;
    int16_t padding2;
    UNewTrie2 *newTrie;     /* builder object; NULL when frozen */

#اگر_تعریف_شده UTRIE2_DEBUG
    ثابت حرف *name;
#پایان_اگر
};

/**
 * Trie constants, defining shift widths, index array lengths, etc.
 *
 * These are needed for the runtime macros but users can treat these as
 * implementation details and skip to the actual public API further below.
 */
تعداد {
    /** Shift size for getting the index-1 table offset. */
    UTRIE2_لغزش_1=6+5,

    /** Shift size for getting the index-2 table offset. */
    UTRIE2_لغزش_2=5,

    /**
     * Difference between the two shift sizes,
     * for getting an index-1 offset from an index-2 offset. 6=11-5
     */
    UTRIE2_لغزش_1_2=UTRIE2_لغزش_1-UTRIE2_لغزش_2,

    /**
     * Number of index-1 entries for the BMP. 32=0x20
     * This part of the index-1 table is omitted from the serialized form.
     */
    UTRIE2_OMITTED_BMP_شاخص_1_درازی=0x10000>>UTRIE2_لغزش_1,

    /** Number of code points per index-1 table entry. 2048=0x800 */
    UTRIE2_CP_در_هر_شاخص_1_مدخل=1<<UTRIE2_لغزش_1,

    /** Number of entries in an index-2 block. 64=0x40 */
    UTRIE2_شاخص_2_بلوک_درازی=1<<UTRIE2_لغزش_1_2,

    /** Mask for getting the lower bits for the in-index-2-block offset. */
    UTRIE2_شاخص_2_ماسک=UTRIE2_شاخص_2_بلوک_درازی-1,

    /** Number of entries in a data block. 32=0x20 */
    UTRIE2_داده_بلوک_درازی=1<<UTRIE2_لغزش_2,

    /** Mask for getting the lower bits for the in-data-block offset. */
    UTRIE2_داده_ماسک=UTRIE2_داده_بلوک_درازی-1,

    /**
     * Shift size for shifting left the index array values.
     * Increases possible data size with 16-bit index values at the cost
     * of compactability.
     * This requires data blocks to be aligned by UTRIE2_DATA_GRANULARITY.
     */
    UTRIE2_شاخص_لغزش=2,

    /** The alignment size of a data block. Also the granularity for compaction. */
    UTRIE2_داده_GRANULARITY=1<<UTRIE2_شاخص_لغزش,

    /* Fixed layout of the first part of the index array. ------------------- */

    /**
     * The BMP part of the index-2 table is fixed and linear and starts at offset 0.
     * Length=2048=0x800=0x10000>>UTRIE2_SHIFT_2.
     */
    UTRIE2_شاخص_2_جابه_جایی=0,

    /**
     * The part of the index-2 table for U+D800..U+DBFF stores values for
     * lead surrogate code _units_ not code _points_.
     * Values for lead surrogate code _points_ are indexed with this portion of the table.
     * Length=32=0x20=0x400>>UTRIE2_SHIFT_2. (There are 1024=0x400 lead surrogates.)
     */
    UTRIE2_LSCP_شاخص_2_جابه_جایی=0x10000>>UTRIE2_لغزش_2,
    UTRIE2_LSCP_شاخص_2_درازی=0x400>>UTRIE2_لغزش_2,

    /** Count the lengths of both BMP pieces. 2080=0x820 */
    UTRIE2_شاخص_2_BMP_درازی=UTRIE2_LSCP_شاخص_2_جابه_جایی+UTRIE2_LSCP_شاخص_2_درازی,

    /**
     * The 2-byte UTF-8 version of the index-2 table follows at offset 2080=0x820.
     * Length 32=0x20 for lead bytes C0..DF, regardless of UTRIE2_SHIFT_2.
     */
    UTRIE2_UTF8_2_B_شاخص_2_جابه_جایی=UTRIE2_شاخص_2_BMP_درازی,
    UTRIE2_UTF8_2_B_شاخص_2_درازی=0x800>>6,  /* U+0800 is the first code point after 2-byte UTF-8 */

    /**
     * The index-1 table, only used for supplementary code points, at offset 2112=0x840.
     * Variable length, for code points up to highStart, where the last single-value range starts.
     * Maximum length 512=0x200=0x100000>>UTRIE2_SHIFT_1.
     * (For 0x100000 supplementary code points U+10000..U+10ffff.)
     *
     * The part of the index-2 table for supplementary code points starts
     * after this index-1 table.
     *
     * Both the index-1 table and the following part of the index-2 table
     * are omitted completely if there is only BMP data.
     */
    UTRIE2_شاخص_1_جابه_جایی=UTRIE2_UTF8_2_B_شاخص_2_جابه_جایی+UTRIE2_UTF8_2_B_شاخص_2_درازی,
    UTRIE2_MAX_شاخص_1_درازی=0x100000>>UTRIE2_لغزش_1,

    /*
     * Fixed layout of the first part of the data array. -----------------------
     * Starts with 4 blocks (128=0x80 entries) for ASCII.
     */

    /**
     * The illegal-UTF-8 data block follows the ASCII block, at offset 128=0x80.
     * Used with linear access for single bytes 0..0xbf for simple error handling.
     * Length 64=0x40, not UTRIE2_DATA_BLOCK_LENGTH.
     */
    UTRIE2_BAD_UTF8_داده_جابه_جایی=0x80,

    /** The start of non-linear-ASCII data blocks, at offset 192=0xc0. */
    UTRIE2_داده_شروع_جابه_جایی=0xc0
};

/* Internal functions and macros -------------------------------------------- */

/**
 * Internal function for part of the UTRIE2_U8_NEXTxx() macro implementations.
 * Do not call directly.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utrie2_داخلی_U8_بعدی_شاخص(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *src, ثابت uint8_t *حد);

/**
 * Internal function for part of the UTRIE2_U8_PREVxx() macro implementations.
 * Do not call directly.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utrie2_داخلی_U8_Prev_شاخص(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *شروع, ثابت uint8_t *src);


/** Internal low-level trie getter. Returns a data index. */
#تعریف_کن UTRIE2_شاخص_RAW(جابه_جایی, trie_شاخص, c) \
    (((int32_t)((trie_شاخص)[(جابه_جایی)+((c)>>UTRIE2_لغزش_2)]) \
    <<UTRIE2_شاخص_لغزش)+ \
    ((c)&UTRIE2_داده_ماسک))

/** Internal trie getter from a UTF-16 single/lead code unit. Returns the data index. */
#تعریف_کن UTRIE2_شاخص_FROM_U16_SINGLE_LEAD(trie_شاخص, c) UTRIE2_شاخص_RAW(0, trie_شاخص, c)

/** Internal trie getter from a lead surrogate code point (D800..DBFF). Returns the data index. */
#تعریف_کن UTRIE2_شاخص_FROM_LSCP(trie_شاخص, c) \
    UTRIE2_شاخص_RAW(UTRIE2_LSCP_شاخص_2_جابه_جایی-(0xd800>>UTRIE2_لغزش_2), trie_شاخص, c)

/** Internal trie getter from a BMP code point. Returns the data index. */
#تعریف_کن UTRIE2_شاخص_FROM_BMP(trie_شاخص, c) \
    UTRIE2_شاخص_RAW(U_IS_LEAD(c) ? UTRIE2_LSCP_شاخص_2_جابه_جایی-(0xd800>>UTRIE2_لغزش_2) : 0, \
                      trie_شاخص, c)

/** Internal trie getter from a supplementary code point below highStart. Returns the data index. */
#تعریف_کن UTRIE2_شاخص_FROM_SUPP(trie_شاخص, c) \
    (((int32_t)((trie_شاخص)[ \
        (trie_شاخص)[(UTRIE2_شاخص_1_جابه_جایی-UTRIE2_OMITTED_BMP_شاخص_1_درازی)+ \
                      ((c)>>UTRIE2_لغزش_1)]+ \
        (((c)>>UTRIE2_لغزش_2)&UTRIE2_شاخص_2_ماسک)]) \
    <<UTRIE2_شاخص_لغزش)+ \
    ((c)&UTRIE2_داده_ماسک))

/**
 * Internal trie getter from a code point, with checking that c is in 0..10FFFF.
 * Returns the data index.
 */
#تعریف_کن UTRIE2_شاخص_FROM_CP(trie, ascii_جابه_جایی, c) \
    ((uint32_t)(c)<0xd800 ? \
        UTRIE2_شاخص_RAW(0, (trie)->شاخص, c) : \
        (uint32_t)(c)<=0xffff ? \
            UTRIE2_شاخص_RAW( \
                (c)<=0xdbff ? UTRIE2_LSCP_شاخص_2_جابه_جایی-(0xd800>>UTRIE2_لغزش_2) : 0, \
                (trie)->شاخص, c) : \
            (uint32_t)(c)>0x10ffff ? \
                (ascii_جابه_جایی)+UTRIE2_BAD_UTF8_داده_جابه_جایی : \
                (c)>=(trie)->بالا_شروع ? \
                    (trie)->بالا_مقدار_شاخص : \
                    UTRIE2_شاخص_FROM_SUPP((trie)->شاخص, c))

/** Internal trie getter from a UTF-16 single/lead code unit. Returns the data. */
#تعریف_کن _UTRIE2_GET_FROM_U16_SINGLE_LEAD(trie, داده, c) \
    (trie)->داده[UTRIE2_شاخص_FROM_U16_SINGLE_LEAD((trie)->شاخص, c)]

/** Internal trie getter from a supplementary code point. Returns the data. */
#تعریف_کن _UTRIE2_GET_FROM_SUPP(trie, داده, c) \
    (trie)->داده[(c)>=(trie)->بالا_شروع ? (trie)->بالا_مقدار_شاخص : \
                 UTRIE2_شاخص_FROM_SUPP((trie)->شاخص, c)]

/**
 * Internal trie getter from a code point, with checking that c is in 0..10FFFF.
 * Returns the data.
 */
#تعریف_کن _UTRIE2_GET(trie, داده, ascii_جابه_جایی, c) \
    (trie)->داده[UTRIE2_شاخص_FROM_CP(trie, ascii_جابه_جایی, c)]

/** Internal next-post-increment: get the next code point (c) and its data. */
#تعریف_کن UTRIE2_U16_بعدی(trie, داده, src, حد, c, نتیجه) UPRV_بلوک_MACRO_BEGIN { \
    { \
        uint16_t __c2; \
        (c)=*(src)++; \
        إذا(!U16_IS_LEAD(c)) { \
            (نتیجه)=_UTRIE2_GET_FROM_U16_SINGLE_LEAD(trie, داده, c); \
        } وإلا إذا((src)==(حد) || !U16_IS_TRAIL(__c2=*(src))) { \
            (نتیجه)=(trie)->داده[UTRIE2_شاخص_FROM_LSCP((trie)->شاخص, c)]; \
        } وإلا { \
            ++(src); \
            (c)=U16_GET_SUPPLEMENTARY((c), __c2); \
            (نتیجه)=_UTRIE2_GET_FROM_SUPP((trie), داده, (c)); \
        } \
    } \
} UPRV_بلوک_MACRO_انتها

/** Internal pre-decrement-previous: get the previous code point (c) and its data */
#تعریف_کن _UTRIE2_U16_PREV(trie, داده, شروع, src, c, نتیجه) UPRV_بلوک_MACRO_BEGIN { \
    { \
        uint16_t __c2; \
        (c)=*--(src); \
        إذا(!U16_IS_TRAIL(c) || (src)==(شروع) || !U16_IS_LEAD(__c2=*((src)-1))) { \
            (نتیجه)=(trie)->داده[UTRIE2_شاخص_FROM_BMP((trie)->شاخص, c)]; \
        } وإلا { \
            --(src); \
            (c)=U16_GET_SUPPLEMENTARY(__c2, (c)); \
            (نتیجه)=_UTRIE2_GET_FROM_SUPP((trie), داده, (c)); \
        } \
    } \
} UPRV_بلوک_MACRO_انتها

/** Internal UTF-8 next-post-increment: get the next code point's data. */
#تعریف_کن UTRIE2_U8_بعدی(trie, ascii, داده, src, حد, نتیجه) UPRV_بلوک_MACRO_BEGIN { \
    uint8_t __lead=(uint8_t)*(src)++; \
    إذا(U8_IS_SINGLE(__lead)) { \
        (نتیجه)=(trie)->ascii[__lead]; \
    } وإلا { \
        uint8_t __t1, __t2; \
        إذا( /* handle U+0800..U+FFFF inline */ \
            0xe0<=__lead && __lead<0xf0 && ((src)+1)<(حد) && \
            U8_IS_VALID_LEAD3_AND_T1(__lead, __t1=(uint8_t)*(src)) && \
            (__t2=(uint8_t)(*((src)+1)-0x80))<= 0x3f \
        ) { \
            (src)+=2; \
            (نتیجه)=(trie)->داده[ \
                ((int32_t)((trie)->شاخص[((__lead-0xe0)<<(12-UTRIE2_لغزش_2))+ \
                                         ((__t1&0x3f)<<(6-UTRIE2_لغزش_2))+(__t2>>UTRIE2_لغزش_2)]) \
                <<UTRIE2_شاخص_لغزش)+ \
                (__t2&UTRIE2_داده_ماسک)]; \
        } وإلا إذا( /* handle U+0080..U+07FF inline */ \
            __lead<0xe0 && __lead>=0xc2 && (src)<(حد) && \
            (__t1=(uint8_t)(*(src)-0x80))<=0x3f \
        ) { \
            ++(src); \
            (نتیجه)=(trie)->داده[ \
                (trie)->شاخص[(UTRIE2_UTF8_2_B_شاخص_2_جابه_جایی-0xc0)+__lead]+ \
                __t1]; \
        } وإلا { \
            int32_t __index=utrie2_داخلی_U8_بعدی_شاخص((trie), __lead, (ثابت uint8_t *)(src), \
                                                                       (ثابت uint8_t *)(حد)); \
            (src)+=__index&7; \
            (نتیجه)=(trie)->داده[__index>>3]; \
        } \
    } \
} UPRV_بلوک_MACRO_انتها

/** Internal UTF-8 pre-decrement-previous: get the previous code point's data. */
#تعریف_کن _UTRIE2_U8_PREV(trie, ascii, داده, شروع, src, نتیجه) UPRV_بلوک_MACRO_BEGIN { \
    uint8_t __b=(uint8_t)*--(src); \
    إذا(U8_IS_SINGLE(__b)) { \
        (نتیجه)=(trie)->ascii[__b]; \
    } وإلا { \
        int32_t __index=utrie2_داخلی_U8_Prev_شاخص((trie), __b, (ثابت uint8_t *)(شروع), \
                                                                (ثابت uint8_t *)(src)); \
        (src)-=__index&7; \
        (نتیجه)=(trie)->داده[__index>>3]; \
    } \
} UPRV_بلوک_MACRO_انتها

U_CDECL_انتها

#پایان_اگر
