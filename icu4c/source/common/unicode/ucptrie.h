// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.h (modified from utrie2.h)
// created: 2017dec29 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UCPTRIE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UCPTRIE_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucpmap.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಆದರೆ_ಅಂತ್ಯ   // U_SHOW_CPLUSPLUS_API

U_CDECL_BEGIN

/**
 * \file
 * \brief C API: This file defines an immutable Unicode code point trie.
 *
 * @see UCPTrie
 * @see UMutableCPTrie
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
/** @internal */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ UCPTrie_ದತ್ತಾಂಶ {
    /** @internal */
    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ptr0;
    /** @internal */
    ಸ್ಥಿರಾಂಕ uint16_t *ptr16;
    /** @internal */
    ಸ್ಥಿರಾಂಕ uint32_t *ptr32;
    /** @internal */
    ಸ್ಥಿರಾಂಕ uint8_t *ptr8;
} UCPTrie_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Immutable Unicode code point trie structure.
 * Fast, reasonably compact, map from Unicode code points (U+0000..U+10FFFF) to integer values.
 * For details see https://icu.unicode.org/design/struct/utrie
 *
 * Do not access UCPTrie fields directly; use public functions and macros.
 * Functions are easy to use: They support all trie types and value widths.
 *
 * When performance is really important, macros provide faster access.
 * Most macros are specific to either "fast" or "small" tries, see UCPTrieType.
 * There are "fast" macros for special optimized use cases.
 *
 * The macros will return bogus values, or may crash, if used on the wrong type or value width.
 *
 * @see UMutableCPTrie
 * @stable ICU 63
 */
ರಚನೆ UCPTrie {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
    /** @internal */
    ಸ್ಥಿರಾಂಕ uint16_t *ಸೂಚ್ಯಂಕ;
    /** @internal */
    UCPTrie_ದತ್ತಾಂಶ ದತ್ತಾಂಶ;

    /** @internal */
    int32_t ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    /** @internal */
    int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    /** Start of the last range which ends at U+10FFFF. @internal */
    UChar32 ಮೇಲಿನ_ಆರಂಭ;
    /** highStart>>12 @internal */
    uint16_t shifted12_ಮೇಲಿನ_ಆರಂಭ;

    /** @internal */
    int8_t type;  // UCPTrieType
    /** @internal */
    int8_t ಮೌಲ್ಯ_ಅಗಲ;  // UCPTrieValueWidth

    /** padding/reserved @internal */
    uint32_t reserved32;
    /** padding/reserved @internal */
    uint16_t reserved16;

    /**
     * Internal index-3 null block offset.
     * Set to an impossibly high value (e.g., 0xffff) if there is no dedicated index-3 null block.
     * @internal
     */
    uint16_t index3_Null_ಸ್ಥಾನಾಂತರ;
    /**
     * Internal data null block offset, not shifted.
     * Set to an impossibly high value (e.g., 0xfffff) if there is no dedicated data null block.
     * @internal
     */
    int32_t ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    /** @internal */
    uint32_t null_ಮೌಲ್ಯ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    /** @internal */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
};
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UCPTrie UCPTrie;
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Selectors for the type of a UCPTrie.
 * Different trade-offs for size vs. speed.
 *
 * @see umutablecptrie_buildImmutable
 * @see ucptrie_openFromBinary
 * @see ucptrie_getType
 * @stable ICU 63
 */
ಎಣಿಕೆ UCPTrieType {
    /**
     * For ucptrie_openFromBinary() to accept any type.
     * ucptrie_getType() will return the actual type.
     * @stable ICU 63
     */
    UCPTRIE_TYPE_ANY = -1,
    /**
     * Fast/simple/larger BMP data structure. Use functions and "fast" macros.
     * @stable ICU 63
     */
    UCPTRIE_TYPE_FAST,
    /**
     * Small/slower BMP data structure. Use functions and "small" macros.
     * @stable ICU 63
     */
    UCPTRIE_TYPE_SMALL
};
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UCPTrieType UCPTrieType;
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Selectors for the number of bits in a UCPTrie data value.
 *
 * @see umutablecptrie_buildImmutable
 * @see ucptrie_openFromBinary
 * @see ucptrie_getValueWidth
 * @stable ICU 63
 */
ಎಣಿಕೆ UCPTrie_ಮೌಲ್ಯ_ಅಗಲ {
    /**
     * For ucptrie_openFromBinary() to accept any data value width.
     * ucptrie_getValueWidth() will return the actual data value width.
     * @stable ICU 63
     */
    UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ANY = -1,
    /**
     * The trie stores 16 bits per data value.
     * It returns them as unsigned values 0..0xffff=65535.
     * @stable ICU 63
     */
    UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16,
    /**
     * The trie stores 32 bits per data value.
     * @stable ICU 63
     */
    UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32,
    /**
     * The trie stores 8 bits per data value.
     * It returns them as unsigned values 0..0xff=255.
     * @stable ICU 63
     */
    UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8
};
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UCPTrie_ಮೌಲ್ಯ_ಅಗಲ UCPTrie_ಮೌಲ್ಯ_ಅಗಲ;
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Opens a trie from its binary form, stored in 32-bit-aligned memory.
 * Inverse of ucptrie_toBinary().
 *
 * The memory must remain valid and unchanged as long as the trie is used.
 * You must ucptrie_close() the trie once you are done using it.
 *
 * @param type selects the trie type; results in an
 *             U_INVALID_FORMAT_ERROR if it does not match the binary data;
 *             use UCPTRIE_TYPE_ANY to accept any type
 * @param valueWidth selects the number of bits in a data value; results in an
 *                  U_INVALID_FORMAT_ERROR if it does not match the binary data;
 *                  use UCPTRIE_VALUE_BITS_ANY to accept any data value width
 * @param data a pointer to 32-bit-aligned memory containing the binary data of a UCPTrie
 * @param length the number of bytes available at data;
 *               can be more than necessary
 * @param pActualLength receives the actual number of bytes at data taken up by the trie data;
 *                      can be NULL
 * @param pErrorCode an in/out ICU UErrorCode
 * @return the trie
 *
 * @see umutablecptrie_open
 * @see umutablecptrie_buildImmutable
 * @see ucptrie_toBinary
 * @stable ICU 63
 */
U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ದ್ವಿಮಾನ(UCPTrieType type,
                       UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ,
                       ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ದತ್ತಾಂಶ U_LIFETIME_BOUND,
                       int32_t ಉದ್ದಳತೆ,
                       int32_t* p_Actual_ಉದ್ದಳತೆ,
                       UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ);

/**
 * Closes a trie and releases associated memory.
 *
 * @param trie the trie
 * @stable ICU 63
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
ucptrie_close(UCPTrie *trie);

/**
 * Returns the trie type.
 *
 * @param trie the trie
 * @return the trie type
 * @see ucptrie_openFromBinary
 * @see UCPTRIE_TYPE_ANY
 * @stable ICU 63
 */
U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ಸ್ಥಿರಾಂಕ UCPTrie *trie);

/**
 * Returns the number of bits in a trie data value.
 *
 * @param trie the trie
 * @return the number of bits in a trie data value
 * @see ucptrie_openFromBinary
 * @see UCPTRIE_VALUE_BITS_ANY
 * @stable ICU 63
 */
U_CAPI UCPTrie_ಮೌಲ್ಯ_ಅಗಲ U_EXPORT2
ucptrie_get_ಮೌಲ್ಯ_ಅಗಲ(ಸ್ಥಿರಾಂಕ UCPTrie *trie);

/**
 * Returns the value for a code point as stored in the trie, with range checking.
 * Returns the trie error value if c is not in the range 0..U+10FFFF.
 *
 * Easier to use than UCPTRIE_FAST_GET() and similar macros but slower.
 * Easier to use because, unlike the macros, this function works on all UCPTrie
 * objects, for all types and value widths.
 *
 * @param trie the trie
 * @param c the code point
 * @return the trie value,
 *         or the trie error value if the code point is not in the range 0..U+10FFFF
 * @stable ICU 63
 */
U_CAPI uint32_t U_EXPORT2
ucptrie_get(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 c);

/**
 * Returns the last code point such that all those from start to there have the same value.
 * Can be used to efficiently iterate over all same-value ranges in a trie.
 * (This is normally faster than iterating over code points and get()ting each value,
 * but much slower than a data structure that stores ranges directly.)
 *
 * If the UCPMapValueFilter function pointer is not NULL, then
 * the value to be delivered is passed through that function, and the return value is the end
 * of the range where all values are modified to the same actual value.
 * The value is unchanged if that function pointer is NULL.
 *
 * Example:
 * \code
 * UChar32 start = 0, end;
 * uint32_t value;
 * while ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
 *                                NULL, NULL, &value)) >= 0) {
 *     // Work with the range start..end and its value.
 *     start = end + 1;
 * }
 * \endcode
 *
 * @param trie the trie
 * @param start range start
 * @param option defines whether surrogates are treated normally,
 *               or as having the surrogateValue; usually UCPMAP_RANGE_NORMAL
 * @param surrogateValue value for surrogates; ignored if option==UCPMAP_RANGE_NORMAL
 * @param filter a pointer to a function that may modify the trie data value,
 *     or NULL if the values from the trie are to be used unmodified
 * @param context an opaque pointer that is passed on to the filter function
 * @param pValue if not NULL, receives the value that every code point start..end has;
 *     may have been modified by filter(context, trie value)
 *     if that function pointer is not NULL
 * @return the range end code point, or -1 if start is not a valid code point
 * @stable ICU 63
 */
U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 ಆರಂಭ,
                 UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ);

/**
 * Writes a memory-mappable form of the trie into 32-bit aligned memory.
 * Inverse of ucptrie_openFromBinary().
 *
 * @param trie the trie
 * @param data a pointer to 32-bit-aligned memory to be filled with the trie data;
 *             can be NULL if capacity==0
 * @param capacity the number of bytes available at data, or 0 for pure preflighting
 * @param pErrorCode an in/out ICU UErrorCode;
 *                   U_BUFFER_OVERFLOW_ERROR if the capacity is too small
 * @return the number of bytes written or (if buffer overflow) needed for the trie
 *
 * @see ucptrie_openFromBinary()
 * @stable ICU 63
 */
U_CAPI int32_t U_EXPORT2
ucptrie_ಗಮ್ಯ_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ UCPTrie *trie, ಶೂನ್ಯ *ದತ್ತಾಂಶ, int32_t ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * Macro parameter value for a trie with 16-bit data values.
 * Use the name of this macro as a "dataAccess" parameter in other macros.
 * Do not use this macro in any other way.
 *
 * @see UCPTRIE_VALUE_BITS_16
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_16(trie, i) ((trie)->ದತ್ತಾಂಶ.ptr16[i])

/**
 * Macro parameter value for a trie with 32-bit data values.
 * Use the name of this macro as a "dataAccess" parameter in other macros.
 * Do not use this macro in any other way.
 *
 * @see UCPTRIE_VALUE_BITS_32
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_32(trie, i) ((trie)->ದತ್ತಾಂಶ.ptr32[i])

/**
 * Macro parameter value for a trie with 8-bit data values.
 * Use the name of this macro as a "dataAccess" parameter in other macros.
 * Do not use this macro in any other way.
 *
 * @see UCPTRIE_VALUE_BITS_8
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_8(trie, i) ((trie)->ದತ್ತಾಂಶ.ptr8[i])

/**
 * Returns a trie value for a code point, with range checking.
 * Returns the trie error value if c is not in the range 0..U+10FFFF.
 *
 * @param trie (const UCPTrie *, in) the trie; must have type UCPTRIE_TYPE_FAST
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param c (UChar32, in) the input code point
 * @return The code point's trie value.
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_FAST_GET(trie, ದತ್ತಾಂಶ_Access, c) ದತ್ತಾಂಶ_Access(trie, UCPTRIE_CP_ಸೂಚ್ಯಂಕ(trie, 0xffff, c))

/**
 * Returns a 16-bit trie value for a code point, with range checking.
 * Returns the trie error value if c is not in the range U+0000..U+10FFFF.
 *
 * @param trie (const UCPTrie *, in) the trie; must have type UCPTRIE_TYPE_SMALL
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param c (UChar32, in) the input code point
 * @return The code point's trie value.
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_SMALL_GET(trie, ದತ್ತಾಂಶ_Access, c) \
    ದತ್ತಾಂಶ_Access(trie, UCPTRIE_CP_ಸೂಚ್ಯಂಕ(trie, UCPTRIE_SMALL_MAX, c))

/**
 * UTF-16: Reads the next code point (UChar32 c, out), post-increments src,
 * and gets a value from the trie.
 * Sets the trie error value if c is an unpaired surrogate.
 *
 * @param trie (const UCPTrie *, in) the trie; must have type UCPTRIE_TYPE_FAST
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param src (const UChar *, in/out) the source text pointer
 * @param limit (const UChar *, in) the limit pointer for the text, or NULL if NUL-terminated
 * @param c (UChar32, out) variable for the code point
 * @param result (out) variable for the trie lookup result
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_FAST_U16_ಮುಂದಿನ(trie, ದತ್ತಾಂಶ_Access, src, ಎಲ್ಲೆ, c, ಫಲಿತಾಂಶ) UPRV_ಖಂಡ_MACRO_BEGIN { \
    (c) = *(src)++; \
    int32_t __index; \
    ಆದರೆ (!U16_IS_SURROGATE(c)) { \
        __index = UCPTRIE_FAST_ಸೂಚ್ಯಂಕ(trie, c); \
    } ಇಲ್ಲದಿದ್ದರೆ { \
        uint16_t __c2; \
        ಆದರೆ (U16_IS_SURROGATE_LEAD(c) && (src) != (ಎಲ್ಲೆ) && U16_IS_TRAIL(__c2 = *(src))) { \
            ++(src); \
            (c) = U16_GET_SUPPLEMENTARY((c), __c2); \
            __index = UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ(trie, c); \
        } ಇಲ್ಲದಿದ್ದರೆ { \
            __index = (trie)->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ; \
        } \
    } \
    (ಫಲಿತಾಂಶ) = ದತ್ತಾಂಶ_Access(trie, __index); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/**
 * UTF-16: Reads the previous code point (UChar32 c, out), pre-decrements src,
 * and gets a value from the trie.
 * Sets the trie error value if c is an unpaired surrogate.
 *
 * @param trie (const UCPTrie *, in) the trie; must have type UCPTRIE_TYPE_FAST
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param start (const UChar *, in) the start pointer for the text
 * @param src (const UChar *, in/out) the source text pointer
 * @param c (UChar32, out) variable for the code point
 * @param result (out) variable for the trie lookup result
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_FAST_U16_PREV(trie, ದತ್ತಾಂಶ_Access, ಆರಂಭ, src, c, ಫಲಿತಾಂಶ) UPRV_ಖಂಡ_MACRO_BEGIN { \
    (c) = *--(src); \
    int32_t __index; \
    ಆದರೆ (!U16_IS_SURROGATE(c)) { \
        __index = UCPTRIE_FAST_ಸೂಚ್ಯಂಕ(trie, c); \
    } ಇಲ್ಲದಿದ್ದರೆ { \
        uint16_t __c2; \
        ಆದರೆ (U16_IS_SURROGATE_TRAIL(c) && (src) != (ಆರಂಭ) && U16_IS_LEAD(__c2 = *((src) - 1))) { \
            --(src); \
            (c) = U16_GET_SUPPLEMENTARY(__c2, (c)); \
            __index = UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ(trie, c); \
        } ಇಲ್ಲದಿದ್ದರೆ { \
            __index = (trie)->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ; \
        } \
    } \
    (ಫಲಿತಾಂಶ) = ದತ್ತಾಂಶ_Access(trie, __index); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/**
 * UTF-8: Post-increments src and gets a value from the trie.
 * Sets the trie error value for an ill-formed byte sequence.
 *
 * Unlike UCPTRIE_FAST_U16_NEXT() this UTF-8 macro does not provide the code point
 * because it would be more work to do so and is often not needed.
 * If the trie value differs from the error value, then the byte sequence is well-formed,
 * and the code point can be assembled without revalidation.
 *
 * @param trie (const UCPTrie *, in) the trie; must have type UCPTRIE_TYPE_FAST
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param src (const char *, in/out) the source text pointer
 * @param limit (const char *, in) the limit pointer for the text (must not be NULL)
 * @param result (out) variable for the trie lookup result
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_FAST_U8_ಮುಂದಿನ(trie, ದತ್ತಾಂಶ_Access, src, ಎಲ್ಲೆ, ಫಲಿತಾಂಶ) UPRV_ಖಂಡ_MACRO_BEGIN { \
    int32_t __lead = (uint8_t)*(src)++; \
    ಆದರೆ (!U8_IS_SINGLE(__lead)) { \
        uint8_t __t1, __t2, __t3; \
        ಆದರೆ ((src) != (ಎಲ್ಲೆ) && \
            (__lead >= 0xe0 ? \
                __lead < 0xf0 ?  /* U+0800..U+FFFF except surrogates */ \
                    U8_LEAD3_T1_ಬಿಟ್ಟುಗಳು[__lead &= 0xf] & (1 << ((__t1 = *(src)) >> 5)) && \
                    ++(src) != (ಎಲ್ಲೆ) && (__t2 = *(src) - 0x80) <= 0x3f && \
                    (__lead = ((int32_t)(trie)->ಸೂಚ್ಯಂಕ[(__lead << 6) + (__t1 & 0x3f)]) + __t2, 1) \
                :  /* U+10000..U+10FFFF */ \
                    (__lead -= 0xf0) <= 4 && \
                    U8_LEAD4_T1_ಬಿಟ್ಟುಗಳು[(__t1 = *(src)) >> 4] & (1 << __lead) && \
                    (__lead = (__lead << 6) | (__t1 & 0x3f), ++(src) != (ಎಲ್ಲೆ)) && \
                    (__t2 = *(src) - 0x80) <= 0x3f && \
                    ++(src) != (ಎಲ್ಲೆ) && (__t3 = *(src) - 0x80) <= 0x3f && \
                    (__lead = __lead >= (trie)->shifted12_ಮೇಲಿನ_ಆರಂಭ ? \
                        (trie)->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ : \
                        ucptrie_ಆಂತರಿಕ_Small_U8_ಸೂಚ್ಯಂಕ((trie), __lead, __t2, __t3), 1) \
            :  /* U+0080..U+07FF */ \
                __lead >= 0xc2 && (__t1 = *(src) - 0x80) <= 0x3f && \
                (__lead = (int32_t)(trie)->ಸೂಚ್ಯಂಕ[__lead & 0x1f] + __t1, 1))) { \
            ++(src); \
        } ಇಲ್ಲದಿದ್ದರೆ { \
            __lead = (trie)->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ;  /* ill-formed*/ \
        } \
    } \
    (ಫಲಿತಾಂಶ) = ದತ್ತಾಂಶ_Access(trie, __lead); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/**
 * UTF-8: Pre-decrements src and gets a value from the trie.
 * Sets the trie error value for an ill-formed byte sequence.
 *
 * Unlike UCPTRIE_FAST_U16_PREV() this UTF-8 macro does not provide the code point
 * because it would be more work to do so and is often not needed.
 * If the trie value differs from the error value, then the byte sequence is well-formed,
 * and the code point can be assembled without revalidation.
 *
 * @param trie (const UCPTrie *, in) the trie; must have type UCPTRIE_TYPE_FAST
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param start (const char *, in) the start pointer for the text
 * @param src (const char *, in/out) the source text pointer
 * @param result (out) variable for the trie lookup result
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_FAST_U8_PREV(trie, ದತ್ತಾಂಶ_Access, ಆರಂಭ, src, ಫಲಿತಾಂಶ) UPRV_ಖಂಡ_MACRO_BEGIN { \
    int32_t __index = (uint8_t)*--(src); \
    ಆದರೆ (!U8_IS_SINGLE(__index)) { \
        __index = ucptrie_ಆಂತರಿಕ_U8_Prev_ಸೂಚ್ಯಂಕ((trie), __index, (ಸ್ಥಿರಾಂಕ uint8_t *)(ಆರಂಭ), \
                                              (ಸ್ಥಿರಾಂಕ uint8_t *)(src)); \
        (src) -= __index & 7; \
        __index >>= 3; \
    } \
    (ಫಲಿತಾಂಶ) = ದತ್ತಾಂಶ_Access(trie, __index); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/**
 * Returns a trie value for an ASCII code point, without range checking.
 *
 * @param trie (const UCPTrie *, in) the trie (of either fast or small type)
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param c (UChar32, in) the input code point; must be U+0000..U+007F
 * @return The ASCII code point's trie value.
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_ASCII_GET(trie, ದತ್ತಾಂಶ_Access, c) ದತ್ತಾಂಶ_Access(trie, c)

/**
 * Returns a trie value for a BMP code point (U+0000..U+FFFF), without range checking.
 * Can be used to look up a value for a UTF-16 code unit if other parts of
 * the string processing check for surrogates.
 *
 * @param trie (const UCPTrie *, in) the trie; must have type UCPTRIE_TYPE_FAST
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param c (UChar32, in) the input code point, must be U+0000..U+FFFF
 * @return The BMP code point's trie value.
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_FAST_BMP_GET(trie, ದತ್ತಾಂಶ_Access, c) ದತ್ತಾಂಶ_Access(trie, UCPTRIE_FAST_ಸೂಚ್ಯಂಕ(trie, c))

/**
 * Returns a trie value for a supplementary code point (U+10000..U+10FFFF),
 * without range checking.
 *
 * @param trie (const UCPTrie *, in) the trie; must have type UCPTRIE_TYPE_FAST
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param c (UChar32, in) the input code point, must be U+10000..U+10FFFF
 * @return The supplementary code point's trie value.
 * @stable ICU 63
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_FAST_SUPP_GET(trie, ದತ್ತಾಂಶ_Access, c) ದತ್ತಾಂಶ_Access(trie, UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ(trie, c))

/* Internal definitions ----------------------------------------------------- */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN

/**
 * Internal implementation constants.
 * These are needed for the API macros, but users should not use these directly.
 * @internal
 */
ಎಣಿಕೆ {
    /** @internal */
    UCPTRIE_FAST_ಸರಿಸು = 6,

    /** Number of entries in a data block for code points below the fast limit. 64=0x40 @internal */
    UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ = 1 << UCPTRIE_FAST_ಸರಿಸು,

    /** Mask for getting the lower bits for the in-fast-data-block offset. @internal */
    UCPTRIE_FAST_ದತ್ತಾಂಶ_ಮುಖವಾಡ = UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ - 1,

    /** @internal */
    UCPTRIE_SMALL_MAX = 0xfff,

    /**
     * Offset from dataLength (to be subtracted) for fetching the
     * value returned for out-of-range code points and ill-formed UTF-8/16.
     * @internal
     */
    UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ = 1,
    /**
     * Offset from dataLength (to be subtracted) for fetching the
     * value returned for code points highStart..U+10FFFF.
     * @internal
     */
    UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ = 2
};

/* Internal functions and macros -------------------------------------------- */
// Do not conditionalize with #ifndef U_HIDE_INTERNAL_API, needed for public API

/** @internal */
U_CAPI int32_t U_EXPORT2
ucptrie_ಆಂತರಿಕ_Small_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 c);

/** @internal */
U_CAPI int32_t U_EXPORT2
ucptrie_ಆಂತರಿಕ_Small_U8_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3);

/**
 * Internal function for part of the UCPTRIE_FAST_U8_PREVxx() macro implementations.
 * Do not call directly.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucptrie_ಆಂತರಿಕ_U8_Prev_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 c,
                            ಸ್ಥಿರಾಂಕ uint8_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *src);

/** Internal trie getter for a code point below the fast limit. Returns the data index. @internal */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_FAST_ಸೂಚ್ಯಂಕ(trie, c) \
    ((int32_t)(trie)->ಸೂಚ್ಯಂಕ[(c) >> UCPTRIE_FAST_ಸರಿಸು] + ((c) & UCPTRIE_FAST_ದತ್ತಾಂಶ_ಮುಖವಾಡ))

/** Internal trie getter for a code point at or above the fast limit. Returns the data index. @internal */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ(trie, c) \
    ((c) >= (trie)->ಮೇಲಿನ_ಆರಂಭ ? \
        (trie)->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ : \
        ucptrie_ಆಂತರಿಕ_Small_ಸೂಚ್ಯಂಕ(trie, c))

/**
 * Internal trie getter for a code point, with checking that c is in U+0000..10FFFF.
 * Returns the data index.
 * @internal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCPTRIE_CP_ಸೂಚ್ಯಂಕ(trie, fastMax, c) \
    ((uint32_t)(c) <= (uint32_t)(fastMax) ? \
        UCPTRIE_FAST_ಸೂಚ್ಯಂಕ(trie, c) : \
        (uint32_t)(c) <= 0x10ffff ? \
            UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ(trie, c) : \
            (trie)->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ)

U_CDECL_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // U_IN_DOXYGEN

#ಆದರೆ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalUCPTriePointer
 * "Smart pointer" class, closes a UCPTrie via ucptrie_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 63
 */
U_DEFINE_LOCAL_OPEN_ಸೂಚಕ(Local_UCPTrie_ಸೂಚಕ, UCPTrie, ucptrie_close);

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // U_SHOW_CPLUSPLUS_API

#ಆದರೆ_ಅಂತ್ಯ
