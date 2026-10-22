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

#ifndef __UTRIE2_H__
#define __UTRIE2_H__

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "putilimp.h"

U_CDECL_BEGIN

কাঠামো UTrie;  /* forward declaration */
#ifndef __UTRIE_H__
প্রকারলেখ কাঠামো UTrie UTrie;
#endif

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
কাঠামো UTrie2;
প্রকারলেখ কাঠামো UTrie2 UTrie2;

/* Public UTrie2 API functions: read-only access ---------------------------- */

/**
 * Selectors for the width of a UTrie2 data value.
 */
গণনা UTrie2_মান_Bits {
    /** 16 bits per UTrie2 data value. */
    UTRIE2_16_মান_BITS,
    /** 32 bits per UTrie2 data value. */
    UTRIE2_32_মান_BITS,
    /** Number of selectors for the width of UTrie2 data values. */
    UTRIE2_গুনতি_মান_BITS
};
প্রকারলেখ গণনা UTrie2_মান_Bits UTrie2_মান_Bits;

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
utrie2_openFromSerialized(UTrie2_মান_Bits মান_Bits,
                          ধ্রুবক শূন্য *তথ্য, int32_t দৈর্ঘ্য, int32_t *p_Actual_দৈর্ঘ্য,
                          UErrorCode *pErrorCode);

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
utrie2_openDummy(UTrie2_মান_Bits মান_Bits,
                 uint32_t initial_মান, uint32_t error_মান,
                 UErrorCode *pErrorCode);

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
utrie2_get32(ধ্রুবক UTrie2 *trie, UChar32 c);

/* enumeration callback types */

/**
 * Callback from utrie2_enum(), extracts a uint32_t value from a
 * trie value. This value will be passed on to the UTrie2EnumRange function.
 *
 * @param context an opaque pointer, as passed into utrie2_enum()
 * @param value a value from the trie
 * @return the value that is to be passed on to the UTrie2EnumRange function
 */
প্রকারলেখ uint32_t U_CALLCONV
UTrie2_Enum_মান(ধ্রুবক শূন্য *context, uint32_t মান_2);

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
প্রকারলেখ UBool U_CALLCONV
UTrie2EnumRange(ধ্রুবক শূন্য *context, UChar32 start, UChar32 end, uint32_t মান_2);

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
U_CAPI শূন্য U_EXPORT2
utrie2_enum(ধ্রুবক UTrie2 *trie,
            UTrie2_Enum_মান *enum_মান, UTrie2EnumRange *enumRange, ধ্রুবক শূন্য *context);

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
utrie2_open(uint32_t initial_মান, uint32_t error_মান, UErrorCode *pErrorCode);

/**
 * Clone a trie.
 * You must utrie2_close() the clone once you are done using it.
 *
 * @param other the trie to clone
 * @param pErrorCode an in/out ICU UErrorCode
 * @return a pointer to the new trie clone
 */
U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ধ্রুবক UTrie2 *other, UErrorCode *pErrorCode);

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
utrie2_cloneAsThawed(ধ্রুবক UTrie2 *other, UErrorCode *pErrorCode);

/**
 * Close a trie and release associated memory.
 *
 * @param trie the trie
 */
U_CAPI শূন্য U_EXPORT2
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
U_CAPI শূন্য U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t মান_2, UErrorCode *pErrorCode);

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
U_CAPI শূন্য U_EXPORT2
utrie2_setRange32(UTrie2 *trie,
                  UChar32 start, UChar32 end,
                  uint32_t মান_2, UBool overwrite,
                  UErrorCode *pErrorCode);

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
U_CAPI শূন্য U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_মান_Bits মান_Bits, UErrorCode *pErrorCode);

/**
 * Test if the trie is frozen. (See utrie2_freeze().)
 *
 * @param trie the trie
 * @return true if the trie is frozen, that is, immutable, ready for serialization
 *         and for use with fast macros
 */
U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ধ্রুবক UTrie2 *trie);

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
utrie2_serialize(ধ্রুবক UTrie2 *trie,
                 শূন্য *তথ্য, int32_t capacity,
                 UErrorCode *pErrorCode);

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
utrie2_fromUTrie(ধ্রুবক UTrie *trie1, uint32_t error_মান, UErrorCode *pErrorCode);

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
#define UTRIE2_GET16(trie, c) _UTRIE2_GET((trie), সূচক, (trie)->সূচক_দৈর্ঘ্য, (c))

/**
 * Return a 32-bit trie value from a code point, with range checking.
 * Returns trie->errorValue if c is not in the range 0..U+10ffff.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code point
 * @return (uint32_t) The code point's trie value.
 */
#define UTRIE2_GET32(trie, c) _UTRIE2_GET((trie), data32, 0, (c))

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
#define UTRIE2_U16_NEXT16(trie, src, limit, c, ফলাফল) UTRIE2_U16_পরবর্তী(trie, সূচক, src, limit, c, ফলাফল)

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
#define UTRIE2_U16_NEXT32(trie, src, limit, c, ফলাফল) UTRIE2_U16_পরবর্তী(trie, data32, src, limit, c, ফলাফল)

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
#define UTRIE2_U16_PREV16(trie, start, src, c, ফলাফল) UTRIE2_U16_পূর্ব(trie, সূচক, start, src, c, ফলাফল)

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
#define UTRIE2_U16_PREV32(trie, start, src, c, ফলাফল) UTRIE2_U16_পূর্ব(trie, data32, start, src, c, ফলাফল)

/**
 * UTF-8: Post-increment src and get a 16-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param src (const char *, in/out) the source text pointer
 * @param limit (const char *, in) the limit pointer for the text (must not be NULL)
 * @param result (uint16_t, out) uint16_t variable for the trie lookup result
 */
#define UTRIE2_U8_NEXT16(trie, src, limit, ফলাফল)\
    UTRIE2_U8_পরবর্তী(trie, data16, সূচক, src, limit, ফলাফল)

/**
 * UTF-8: Post-increment src and get a 32-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param src (const char *, in/out) the source text pointer
 * @param limit (const char *, in) the limit pointer for the text (must not be NULL)
 * @param result (uint16_t, out) uint32_t variable for the trie lookup result
 */
#define UTRIE2_U8_NEXT32(trie, src, limit, ফলাফল) \
    UTRIE2_U8_পরবর্তী(trie, data32, data32, src, limit, ফলাফল)

/**
 * UTF-8: Pre-decrement src and get a 16-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param start (const char *, in) the start pointer for the text
 * @param src (const char *, in/out) the source text pointer
 * @param result (uint16_t, out) uint16_t variable for the trie lookup result
 */
#define UTRIE2_U8_PREV16(trie, start, src, ফলাফল) \
    UTRIE2_U8_পূর্ব(trie, data16, সূচক, start, src, ফলাফল)

/**
 * UTF-8: Pre-decrement src and get a 32-bit value from the trie.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param start (const char *, in) the start pointer for the text
 * @param src (const char *, in/out) the source text pointer
 * @param result (uint16_t, out) uint32_t variable for the trie lookup result
 */
#define UTRIE2_U8_PREV32(trie, start, src, ফলাফল) \
    UTRIE2_U8_পূর্ব(trie, data32, data32, start, src, ফলাফল)

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
utrie2_get32FromLeadSurrogateCodeUnit(ধ্রুবক UTrie2 *trie, UChar32 c);

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
U_CAPI শূন্য U_EXPORT2
utrie2_enumForLeadSurrogate(ধ্রুবক UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_মান *enum_মান, UTrie2EnumRange *enumRange,
                            ধ্রুবক শূন্য *context);

/**
 * Set a value for a lead surrogate code unit.
 *
 * @param trie the unfrozen trie
 * @param lead the lead surrogate code unit (U+D800..U+DBFF)
 * @param value the value
 * @param pErrorCode an in/out ICU UErrorCode; among other possible error codes:
 * - U_NO_WRITE_PERMISSION if the trie is frozen
 */
U_CAPI শূন্য U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 lead, uint32_t মান_2,
                                     UErrorCode *pErrorCode);

/**
 * Return a 16-bit trie value from a UTF-16 single/lead code unit (<=U+ffff).
 * Same as UTRIE2_GET16() if c is a BMP code point except for lead surrogates,
 * but smaller and faster.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code unit, must be 0<=c<=U+ffff
 * @return (uint16_t) The code unit's trie value.
 */
#define UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c) _UTRIE2_GET_FROM_U16_SINGLE_LEAD((trie), সূচক, c)

/**
 * Return a 32-bit trie value from a UTF-16 single/lead code unit (<=U+ffff).
 * Same as UTRIE2_GET32() if c is a BMP code point except for lead surrogates,
 * but smaller and faster.
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code unit, must be 0<=c<=U+ffff
 * @return (uint32_t) The code unit's trie value.
 */
#define UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c) _UTRIE2_GET_FROM_U16_SINGLE_LEAD((trie), data32, c)

/**
 * Return a 16-bit trie value from a supplementary code point (U+10000..U+10ffff).
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code point, must be U+10000<=c<=U+10ffff
 * @return (uint16_t) The code point's trie value.
 */
#define UTRIE2_GET16_FROM_SUPP(trie, c) _UTRIE2_GET_FROM_SUPP((trie), সূচক, c)

/**
 * Return a 32-bit trie value from a supplementary code point (U+10000..U+10ffff).
 *
 * @param trie (const UTrie2 *, in) a frozen trie
 * @param c (UChar32, in) the input code point, must be U+10000<=c<=U+10ffff
 * @return (uint32_t) The code point's trie value.
 */
#define UTRIE2_GET32_FROM_SUPP(trie, c) _UTRIE2_GET_FROM_SUPP((trie), data32, c)

U_CDECL_END

/* C++ convenience wrappers ------------------------------------------------- */

#ifdef __cplusplus

#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/utf.h"

U_NAMESPACE_BEGIN

// Use the Forward/Backward subclasses below.
class UTrie2StringIterator : সর্বজনীন UMemory {
সর্বজনীন:
    UTrie2StringIterator(ধ্রুবক UTrie2 *t, ধ্রুবক char16_t *p) :
        trie(t), codePointStart(p), codePointLimit(p), codePoint(U_SENTINEL) {}

    ধ্রুবক UTrie2 *trie;
    ধ্রুবক char16_t *codePointStart, *codePointLimit;
    UChar32 codePoint;
};

class BackwardUTrie2StringIterator : সর্বজনীন UTrie2StringIterator {
সর্বজনীন:
    BackwardUTrie2StringIterator(ধ্রুবক UTrie2 *t, ধ্রুবক char16_t *s, ধ্রুবক char16_t *p) :
        UTrie2StringIterator(t, p), start(s) {}

    uint16_t previous16();

    ধ্রুবক char16_t *start;
};

class ForwardUTrie2StringIterator : সর্বজনীন UTrie2StringIterator {
সর্বজনীন:
    // Iteration limit l can be nullptr.
    // In that case, the caller must detect c==0 and stop.
    ForwardUTrie2StringIterator(ধ্রুবক UTrie2 *t, ধ্রুবক char16_t *p, ধ্রুবক char16_t *l) :
        UTrie2StringIterator(t, p), limit(l) {}

    uint16_t next16();

    ধ্রুবক char16_t *limit;
};

U_NAMESPACE_END

#endif

/* Internal definitions ----------------------------------------------------- */

U_CDECL_BEGIN

/** Build-time trie structure. */
কাঠামো UNewTrie2;
প্রকারলেখ কাঠামো UNewTrie2 UNewTrie2;

/*
 * Trie structure definition.
 *
 * Either the data table is 16 bits wide and accessed via the index
 * pointer, with each index item increased by indexLength;
 * in this case, data32==NULL, and data16 is used for direct ASCII access.
 *
 * Or the data table is 32 bits wide and accessed via the data32 pointer.
 */
কাঠামো UTrie2 {
    /* protected: used by macros and functions for reading values */
    ধ্রুবক uint16_t *সূচক;
    ধ্রুবক uint16_t *data16;     /* for fast UTF-8 ASCII access, if 16b data */
    ধ্রুবক uint32_t *data32;     /* NULL if 16b data is used via index */

    int32_t সূচক_দৈর্ঘ্য, তথ্য_দৈর্ঘ্য;
    uint16_t index2NullOffset;  /* 0xffff if there is no dedicated index-2 null block */
    uint16_t তথ্য_Null_Offset;
    uint32_t initial_মান;
    /** Value returned for out-of-range code points and illegal UTF-8. */
    uint32_t error_মান;

    /* Start of the last range which ends at U+10ffff, and its value. */
    UChar32 উপরের_Start;
    int32_t উপরের_মান_সূচক;

    /* private: used by builder and unserialization functions */
    শূন্য *memory;           /* serialized bytes; NULL if not frozen yet */
    int32_t দৈর্ঘ্য;         /* number of serialized bytes at memory; 0 if not frozen yet */
    UBool isMemoryOwned;    /* true if the trie owns the memory */
    UBool padding1;
    int16_t padding2;
    UNewTrie2 *newTrie;     /* builder object; NULL when frozen */

#ifdef UTRIE2_DEBUG
    ধ্রুবক অক্ষর *name;
#endif
};

/**
 * Trie constants, defining shift widths, index array lengths, etc.
 *
 * These are needed for the runtime macros but users can treat these as
 * implementation details and skip to the actual public API further below.
 */
গণনা {
    /** Shift size for getting the index-1 table offset. */
    UTRIE2_SHIFT_1=6+5,

    /** Shift size for getting the index-2 table offset. */
    UTRIE2_SHIFT_2=5,

    /**
     * Difference between the two shift sizes,
     * for getting an index-1 offset from an index-2 offset. 6=11-5
     */
    UTRIE2_SHIFT_1_2=UTRIE2_SHIFT_1-UTRIE2_SHIFT_2,

    /**
     * Number of index-1 entries for the BMP. 32=0x20
     * This part of the index-1 table is omitted from the serialized form.
     */
    UTRIE2_OMITTED_BMP_সূচক_1_দৈর্ঘ্য=0x10000>>UTRIE2_SHIFT_1,

    /** Number of code points per index-1 table entry. 2048=0x800 */
    UTRIE2_CP_PER_সূচক_1_ENTRY=1<<UTRIE2_SHIFT_1,

    /** Number of entries in an index-2 block. 64=0x40 */
    UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য=1<<UTRIE2_SHIFT_1_2,

    /** Mask for getting the lower bits for the in-index-2-block offset. */
    UTRIE2_সূচক_2_MASK=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য-1,

    /** Number of entries in a data block. 32=0x20 */
    UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য=1<<UTRIE2_SHIFT_2,

    /** Mask for getting the lower bits for the in-data-block offset. */
    UTRIE2_তথ্য_MASK=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য-1,

    /**
     * Shift size for shifting left the index array values.
     * Increases possible data size with 16-bit index values at the cost
     * of compactability.
     * This requires data blocks to be aligned by UTRIE2_DATA_GRANULARITY.
     */
    UTRIE2_সূচক_SHIFT=2,

    /** The alignment size of a data block. Also the granularity for compaction. */
    UTRIE2_তথ্য_GRANULARITY=1<<UTRIE2_সূচক_SHIFT,

    /* Fixed layout of the first part of the index array. ------------------- */

    /**
     * The BMP part of the index-2 table is fixed and linear and starts at offset 0.
     * Length=2048=0x800=0x10000>>UTRIE2_SHIFT_2.
     */
    UTRIE2_সূচক_2_OFFSET=0,

    /**
     * The part of the index-2 table for U+D800..U+DBFF stores values for
     * lead surrogate code _units_ not code _points_.
     * Values for lead surrogate code _points_ are indexed with this portion of the table.
     * Length=32=0x20=0x400>>UTRIE2_SHIFT_2. (There are 1024=0x400 lead surrogates.)
     */
    UTRIE2_LSCP_সূচক_2_OFFSET=0x10000>>UTRIE2_SHIFT_2,
    UTRIE2_LSCP_সূচক_2_দৈর্ঘ্য=0x400>>UTRIE2_SHIFT_2,

    /** Count the lengths of both BMP pieces. 2080=0x820 */
    UTRIE2_সূচক_2_BMP_দৈর্ঘ্য=UTRIE2_LSCP_সূচক_2_OFFSET+UTRIE2_LSCP_সূচক_2_দৈর্ঘ্য,

    /**
     * The 2-byte UTF-8 version of the index-2 table follows at offset 2080=0x820.
     * Length 32=0x20 for lead bytes C0..DF, regardless of UTRIE2_SHIFT_2.
     */
    UTRIE2_UTF8_2_B_সূচক_2_OFFSET=UTRIE2_সূচক_2_BMP_দৈর্ঘ্য,
    UTRIE2_UTF8_2_B_সূচক_2_দৈর্ঘ্য=0x800>>6,  /* U+0800 is the first code point after 2-byte UTF-8 */

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
    UTRIE2_সূচক_1_OFFSET=UTRIE2_UTF8_2_B_সূচক_2_OFFSET+UTRIE2_UTF8_2_B_সূচক_2_দৈর্ঘ্য,
    UTRIE2_MAX_সূচক_1_দৈর্ঘ্য=0x100000>>UTRIE2_SHIFT_1,

    /*
     * Fixed layout of the first part of the data array. -----------------------
     * Starts with 4 blocks (128=0x80 entries) for ASCII.
     */

    /**
     * The illegal-UTF-8 data block follows the ASCII block, at offset 128=0x80.
     * Used with linear access for single bytes 0..0xbf for simple error handling.
     * Length 64=0x40, not UTRIE2_DATA_BLOCK_LENGTH.
     */
    UTRIE2_BAD_UTF8_তথ্য_OFFSET=0x80,

    /** The start of non-linear-ASCII data blocks, at offset 192=0xc0. */
    UTRIE2_তথ্য_START_OFFSET=0xc0
};

/* Internal functions and macros -------------------------------------------- */

/**
 * Internal function for part of the UTRIE2_U8_NEXTxx() macro implementations.
 * Do not call directly.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utrie2_internal_U8_পরবর্তী_সূচক(ধ্রুবক UTrie2 *trie, UChar32 c,
                           ধ্রুবক uint8_t *src, ধ্রুবক uint8_t *limit);

/**
 * Internal function for part of the UTRIE2_U8_PREVxx() macro implementations.
 * Do not call directly.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utrie2_internal_U8_পূর্ব_সূচক(ধ্রুবক UTrie2 *trie, UChar32 c,
                           ধ্রুবক uint8_t *start, ধ্রুবক uint8_t *src);


/** Internal low-level trie getter. Returns a data index. */
#define UTRIE2_সূচক_RAW(offset, trie_সূচক, c) \
    (((int32_t)((trie_সূচক)[(offset)+((c)>>UTRIE2_SHIFT_2)]) \
    <<UTRIE2_সূচক_SHIFT)+ \
    ((c)&UTRIE2_তথ্য_MASK))

/** Internal trie getter from a UTF-16 single/lead code unit. Returns the data index. */
#define UTRIE2_সূচক_FROM_U16_SINGLE_LEAD(trie_সূচক, c) UTRIE2_সূচক_RAW(0, trie_সূচক, c)

/** Internal trie getter from a lead surrogate code point (D800..DBFF). Returns the data index. */
#define UTRIE2_সূচক_FROM_LSCP(trie_সূচক, c) \
    UTRIE2_সূচক_RAW(UTRIE2_LSCP_সূচক_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2), trie_সূচক, c)

/** Internal trie getter from a BMP code point. Returns the data index. */
#define UTRIE2_সূচক_FROM_BMP(trie_সূচক, c) \
    UTRIE2_সূচক_RAW(U_IS_LEAD(c) ? UTRIE2_LSCP_সূচক_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2) : 0, \
                      trie_সূচক, c)

/** Internal trie getter from a supplementary code point below highStart. Returns the data index. */
#define UTRIE2_সূচক_FROM_SUPP(trie_সূচক, c) \
    (((int32_t)((trie_সূচক)[ \
        (trie_সূচক)[(UTRIE2_সূচক_1_OFFSET-UTRIE2_OMITTED_BMP_সূচক_1_দৈর্ঘ্য)+ \
                      ((c)>>UTRIE2_SHIFT_1)]+ \
        (((c)>>UTRIE2_SHIFT_2)&UTRIE2_সূচক_2_MASK)]) \
    <<UTRIE2_সূচক_SHIFT)+ \
    ((c)&UTRIE2_তথ্য_MASK))

/**
 * Internal trie getter from a code point, with checking that c is in 0..10FFFF.
 * Returns the data index.
 */
#define UTRIE2_সূচক_FROM_CP(trie, asciiOffset, c) \
    ((uint32_t)(c)<0xd800 ? \
        UTRIE2_সূচক_RAW(0, (trie)->সূচক, c) : \
        (uint32_t)(c)<=0xffff ? \
            UTRIE2_সূচক_RAW( \
                (c)<=0xdbff ? UTRIE2_LSCP_সূচক_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2) : 0, \
                (trie)->সূচক, c) : \
            (uint32_t)(c)>0x10ffff ? \
                (asciiOffset)+UTRIE2_BAD_UTF8_তথ্য_OFFSET : \
                (c)>=(trie)->উপরের_Start ? \
                    (trie)->উপরের_মান_সূচক : \
                    UTRIE2_সূচক_FROM_SUPP((trie)->সূচক, c))

/** Internal trie getter from a UTF-16 single/lead code unit. Returns the data. */
#define _UTRIE2_GET_FROM_U16_SINGLE_LEAD(trie, তথ্য, c) \
    (trie)->তথ্য[UTRIE2_সূচক_FROM_U16_SINGLE_LEAD((trie)->সূচক, c)]

/** Internal trie getter from a supplementary code point. Returns the data. */
#define _UTRIE2_GET_FROM_SUPP(trie, তথ্য, c) \
    (trie)->তথ্য[(c)>=(trie)->উপরের_Start ? (trie)->উপরের_মান_সূচক : \
                 UTRIE2_সূচক_FROM_SUPP((trie)->সূচক, c)]

/**
 * Internal trie getter from a code point, with checking that c is in 0..10FFFF.
 * Returns the data.
 */
#define _UTRIE2_GET(trie, তথ্য, asciiOffset, c) \
    (trie)->তথ্য[UTRIE2_সূচক_FROM_CP(trie, asciiOffset, c)]

/** Internal next-post-increment: get the next code point (c) and its data. */
#define UTRIE2_U16_পরবর্তী(trie, তথ্য, src, limit, c, ফলাফল) UPRV_BLOCK_MACRO_BEGIN { \
    { \
        uint16_t __c2; \
        (c)=*(src)++; \
        যদি(!U16_IS_LEAD(c)) { \
            (ফলাফল)=_UTRIE2_GET_FROM_U16_SINGLE_LEAD(trie, তথ্য, c); \
        } অন্যথা যদি((src)==(limit) || !U16_IS_TRAIL(__c2=*(src))) { \
            (ফলাফল)=(trie)->তথ্য[UTRIE2_সূচক_FROM_LSCP((trie)->সূচক, c)]; \
        } অন্যথা { \
            ++(src); \
            (c)=U16_GET_SUPPLEMENTARY((c), __c2); \
            (ফলাফল)=_UTRIE2_GET_FROM_SUPP((trie), তথ্য, (c)); \
        } \
    } \
} UPRV_BLOCK_MACRO_END

/** Internal pre-decrement-previous: get the previous code point (c) and its data */
#define UTRIE2_U16_পূর্ব(trie, তথ্য, start, src, c, ফলাফল) UPRV_BLOCK_MACRO_BEGIN { \
    { \
        uint16_t __c2; \
        (c)=*--(src); \
        যদি(!U16_IS_TRAIL(c) || (src)==(start) || !U16_IS_LEAD(__c2=*((src)-1))) { \
            (ফলাফল)=(trie)->তথ্য[UTRIE2_সূচক_FROM_BMP((trie)->সূচক, c)]; \
        } অন্যথা { \
            --(src); \
            (c)=U16_GET_SUPPLEMENTARY(__c2, (c)); \
            (ফলাফল)=_UTRIE2_GET_FROM_SUPP((trie), তথ্য, (c)); \
        } \
    } \
} UPRV_BLOCK_MACRO_END

/** Internal UTF-8 next-post-increment: get the next code point's data. */
#define UTRIE2_U8_পরবর্তী(trie, ascii, তথ্য, src, limit, ফলাফল) UPRV_BLOCK_MACRO_BEGIN { \
    uint8_t __lead=(uint8_t)*(src)++; \
    যদি(U8_IS_SINGLE(__lead)) { \
        (ফলাফল)=(trie)->ascii[__lead]; \
    } অন্যথা { \
        uint8_t __t1, __t2; \
        যদি( /* handle U+0800..U+FFFF inline */ \
            0xe0<=__lead && __lead<0xf0 && ((src)+1)<(limit) && \
            U8_IS_VALID_LEAD3_AND_T1(__lead, __t1=(uint8_t)*(src)) && \
            (__t2=(uint8_t)(*((src)+1)-0x80))<= 0x3f \
        ) { \
            (src)+=2; \
            (ফলাফল)=(trie)->তথ্য[ \
                ((int32_t)((trie)->সূচক[((__lead-0xe0)<<(12-UTRIE2_SHIFT_2))+ \
                                         ((__t1&0x3f)<<(6-UTRIE2_SHIFT_2))+(__t2>>UTRIE2_SHIFT_2)]) \
                <<UTRIE2_সূচক_SHIFT)+ \
                (__t2&UTRIE2_তথ্য_MASK)]; \
        } অন্যথা যদি( /* handle U+0080..U+07FF inline */ \
            __lead<0xe0 && __lead>=0xc2 && (src)<(limit) && \
            (__t1=(uint8_t)(*(src)-0x80))<=0x3f \
        ) { \
            ++(src); \
            (ফলাফল)=(trie)->তথ্য[ \
                (trie)->সূচক[(UTRIE2_UTF8_2_B_সূচক_2_OFFSET-0xc0)+__lead]+ \
                __t1]; \
        } অন্যথা { \
            int32_t __index=utrie2_internal_U8_পরবর্তী_সূচক((trie), __lead, (ধ্রুবক uint8_t *)(src), \
                                                                       (ধ্রুবক uint8_t *)(limit)); \
            (src)+=__index&7; \
            (ফলাফল)=(trie)->তথ্য[__index>>3]; \
        } \
    } \
} UPRV_BLOCK_MACRO_END

/** Internal UTF-8 pre-decrement-previous: get the previous code point's data. */
#define UTRIE2_U8_পূর্ব(trie, ascii, তথ্য, start, src, ফলাফল) UPRV_BLOCK_MACRO_BEGIN { \
    uint8_t __b=(uint8_t)*--(src); \
    যদি(U8_IS_SINGLE(__b)) { \
        (ফলাফল)=(trie)->ascii[__b]; \
    } অন্যথা { \
        int32_t __index=utrie2_internal_U8_পূর্ব_সূচক((trie), __b, (ধ্রুবক uint8_t *)(start), \
                                                                (ধ্রুবক uint8_t *)(src)); \
        (src)-=__index&7; \
        (ফলাফল)=(trie)->তথ্য[__index>>3]; \
    } \
} UPRV_BLOCK_MACRO_END

U_CDECL_END

#endif
