// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.h (modified from utrie2.h)
// created: 2017dec29 Markus W. Scherer

#यदि_अपरिभाषितम् __UCPTRIE_H__
#परिभाषय __UCPTRIE_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucpmap.h"
#अन्तर्भाव "unicode/utf8.h"

#यदि U_SHOW_CPLUSPLUS_API
#अन्तर्भाव "unicode/localpointer.h"
#यदि_अन्तः   // U_SHOW_CPLUSPLUS_API

U_CDECL_BEGIN

/**
 * \file
 * \brief C API: This file defines an immutable Unicode code point trie.
 *
 * @see UCPTrie
 * @see UMutableCPTrie
 */

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
/** @internal */
प्रकारनिर्देश सङ्घ UCPTrie_दत्त {
    /** @internal */
    ध्रुव शून्य *ptr0;
    /** @internal */
    ध्रुव uint16_t *ptr16;
    /** @internal */
    ध्रुव uint32_t *ptr32;
    /** @internal */
    ध्रुव uint8_t *ptr8;
} UCPTrie_दत्त;
#यदि_अन्तः

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
संरचना UCPTrie {
#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
    /** @internal */
    ध्रुव uint16_t *सूचकांक;
    /** @internal */
    UCPTrie_दत्त दत्त;

    /** @internal */
    int32_t सूचकांक_दीर्घता;
    /** @internal */
    int32_t दत्त_दीर्घता;
    /** Start of the last range which ends at U+10FFFF. @internal */
    UChar32 उच्चः_Start;
    /** highStart>>12 @internal */
    uint16_t shifted12_उच्चः_Start;

    /** @internal */
    int8_t type;  // UCPTrieType
    /** @internal */
    int8_t मानम्_विस्तारः;  // UCPTrieValueWidth

    /** padding/reserved @internal */
    uint32_t reserved32;
    /** padding/reserved @internal */
    uint16_t reserved16;

    /**
     * Internal index-3 null block offset.
     * Set to an impossibly high value (e.g., 0xffff) if there is no dedicated index-3 null block.
     * @internal
     */
    uint16_t index3_Null_विस्थापनम्;
    /**
     * Internal data null block offset, not shifted.
     * Set to an impossibly high value (e.g., 0xfffff) if there is no dedicated data null block.
     * @internal
     */
    int32_t दत्त_Null_विस्थापनम्;
    /** @internal */
    uint32_t null_मानम्;

#यदि_परिभाषितम् UCPTRIE_DEBUG
    /** @internal */
    ध्रुव वर्ण *name;
#यदि_अन्तः
#यदि_अन्तः
};
#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
प्रकारनिर्देश संरचना UCPTrie UCPTrie;
#यदि_अन्तः

/**
 * Selectors for the type of a UCPTrie.
 * Different trade-offs for size vs. speed.
 *
 * @see umutablecptrie_buildImmutable
 * @see ucptrie_openFromBinary
 * @see ucptrie_getType
 * @stable ICU 63
 */
गणना UCPTrieType {
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
#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
प्रकारनिर्देश गणना UCPTrieType UCPTrieType;
#यदि_अन्तः

/**
 * Selectors for the number of bits in a UCPTrie data value.
 *
 * @see umutablecptrie_buildImmutable
 * @see ucptrie_openFromBinary
 * @see ucptrie_getValueWidth
 * @stable ICU 63
 */
गणना UCPTrie_मानम्_विस्तारः {
    /**
     * For ucptrie_openFromBinary() to accept any data value width.
     * ucptrie_getValueWidth() will return the actual data value width.
     * @stable ICU 63
     */
    UCPTRIE_मानम्_द्वयङ्काः_ANY = -1,
    /**
     * The trie stores 16 bits per data value.
     * It returns them as unsigned values 0..0xffff=65535.
     * @stable ICU 63
     */
    UCPTRIE_मानम्_द्वयङ्काः_16,
    /**
     * The trie stores 32 bits per data value.
     * @stable ICU 63
     */
    UCPTRIE_मानम्_द्वयङ्काः_32,
    /**
     * The trie stores 8 bits per data value.
     * It returns them as unsigned values 0..0xff=255.
     * @stable ICU 63
     */
    UCPTRIE_मानम्_द्वयङ्काः_8
};
#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
प्रकारनिर्देश गणना UCPTrie_मानम्_विस्तारः UCPTrie_मानम्_विस्तारः;
#यदि_अन्तः

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
ucptrie_open_From_द्विमान(UCPTrieType type,
                       UCPTrie_मानम्_विस्तारः मानम्_विस्तारः,
                       ध्रुव शून्य* दत्त U_LIFETIME_BOUND,
                       int32_t दीर्घता,
                       int32_t* p_Actual_दीर्घता,
                       UError_संहिता* p_Error_संहिता);

/**
 * Closes a trie and releases associated memory.
 *
 * @param trie the trie
 * @stable ICU 63
 */
U_CAPI शून्य U_EXPORT2
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
ucptrie_getType(ध्रुव UCPTrie *trie);

/**
 * Returns the number of bits in a trie data value.
 *
 * @param trie the trie
 * @return the number of bits in a trie data value
 * @see ucptrie_openFromBinary
 * @see UCPTRIE_VALUE_BITS_ANY
 * @stable ICU 63
 */
U_CAPI UCPTrie_मानम्_विस्तारः U_EXPORT2
ucptrie_get_मानम्_विस्तारः(ध्रुव UCPTrie *trie);

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
ucptrie_get(ध्रुव UCPTrie *trie, UChar32 c);

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
ucptrie_getRange(ध्रुव UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_मानम्,
                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्);

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
ucptrie_गन्तव्यम्_द्विमान(ध्रुव UCPTrie *trie, शून्य *दत्त, int32_t क्षमता, UError_संहिता *p_Error_संहिता);

/**
 * Macro parameter value for a trie with 16-bit data values.
 * Use the name of this macro as a "dataAccess" parameter in other macros.
 * Do not use this macro in any other way.
 *
 * @see UCPTRIE_VALUE_BITS_16
 * @stable ICU 63
 */
#परिभाषय UCPTRIE_16(trie, i) ((trie)->दत्त.ptr16[i])

/**
 * Macro parameter value for a trie with 32-bit data values.
 * Use the name of this macro as a "dataAccess" parameter in other macros.
 * Do not use this macro in any other way.
 *
 * @see UCPTRIE_VALUE_BITS_32
 * @stable ICU 63
 */
#परिभाषय UCPTRIE_32(trie, i) ((trie)->दत्त.ptr32[i])

/**
 * Macro parameter value for a trie with 8-bit data values.
 * Use the name of this macro as a "dataAccess" parameter in other macros.
 * Do not use this macro in any other way.
 *
 * @see UCPTRIE_VALUE_BITS_8
 * @stable ICU 63
 */
#परिभाषय UCPTRIE_8(trie, i) ((trie)->दत्त.ptr8[i])

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
#परिभाषय UCPTRIE_FAST_GET(trie, दत्त_Access, c) दत्त_Access(trie, UCPTRIE_CP_सूचकांक(trie, 0xffff, c))

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
#परिभाषय UCPTRIE_SMALL_GET(trie, दत्त_Access, c) \
    दत्त_Access(trie, UCPTRIE_CP_सूचकांक(trie, UCPTRIE_SMALL_MAX, c))

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
#परिभाषय UCPTRIE_FAST_U16_अग्र(trie, दत्त_Access, src, सीमा, c, परिणाम) UPRV_खण्डम्_MACRO_BEGIN { \
    (c) = *(src)++; \
    int32_t __index; \
    यदि (!U16_IS_SURROGATE(c)) { \
        __index = UCPTRIE_FAST_सूचकांक(trie, c); \
    } अन्यथा { \
        uint16_t __c2; \
        यदि (U16_IS_SURROGATE_LEAD(c) && (src) != (सीमा) && U16_IS_TRAIL(__c2 = *(src))) { \
            ++(src); \
            (c) = U16_GET_SUPPLEMENTARY((c), __c2); \
            __index = UCPTRIE_SMALL_सूचकांक(trie, c); \
        } अन्यथा { \
            __index = (trie)->दत्त_दीर्घता - UCPTRIE_ERROR_मानम्_NEG_दत्त_विस्थापनम्; \
        } \
    } \
    (परिणाम) = दत्त_Access(trie, __index); \
} UPRV_खण्डम्_MACRO_अन्तः

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
#परिभाषय UCPTRIE_FAST_U16_पूर्व(trie, दत्त_Access, start, src, c, परिणाम) UPRV_खण्डम्_MACRO_BEGIN { \
    (c) = *--(src); \
    int32_t __index; \
    यदि (!U16_IS_SURROGATE(c)) { \
        __index = UCPTRIE_FAST_सूचकांक(trie, c); \
    } अन्यथा { \
        uint16_t __c2; \
        यदि (U16_IS_SURROGATE_TRAIL(c) && (src) != (start) && U16_IS_LEAD(__c2 = *((src) - 1))) { \
            --(src); \
            (c) = U16_GET_SUPPLEMENTARY(__c2, (c)); \
            __index = UCPTRIE_SMALL_सूचकांक(trie, c); \
        } अन्यथा { \
            __index = (trie)->दत्त_दीर्घता - UCPTRIE_ERROR_मानम्_NEG_दत्त_विस्थापनम्; \
        } \
    } \
    (परिणाम) = दत्त_Access(trie, __index); \
} UPRV_खण्डम्_MACRO_अन्तः

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
#परिभाषय UCPTRIE_FAST_U8_अग्र(trie, दत्त_Access, src, सीमा, परिणाम) UPRV_खण्डम्_MACRO_BEGIN { \
    int32_t __lead = (uint8_t)*(src)++; \
    यदि (!U8_IS_SINGLE(__lead)) { \
        uint8_t __t1, __t2, __t3; \
        यदि ((src) != (सीमा) && \
            (__lead >= 0xe0 ? \
                __lead < 0xf0 ?  /* U+0800..U+FFFF except surrogates */ \
                    U8_LEAD3_T1_द्वयङ्काः[__lead &= 0xf] & (1 << ((__t1 = *(src)) >> 5)) && \
                    ++(src) != (सीमा) && (__t2 = *(src) - 0x80) <= 0x3f && \
                    (__lead = ((int32_t)(trie)->सूचकांक[(__lead << 6) + (__t1 & 0x3f)]) + __t2, 1) \
                :  /* U+10000..U+10FFFF */ \
                    (__lead -= 0xf0) <= 4 && \
                    U8_LEAD4_T1_द्वयङ्काः[(__t1 = *(src)) >> 4] & (1 << __lead) && \
                    (__lead = (__lead << 6) | (__t1 & 0x3f), ++(src) != (सीमा)) && \
                    (__t2 = *(src) - 0x80) <= 0x3f && \
                    ++(src) != (सीमा) && (__t3 = *(src) - 0x80) <= 0x3f && \
                    (__lead = __lead >= (trie)->shifted12_उच्चः_Start ? \
                        (trie)->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम् : \
                        ucptrie_आन्तरिकम्_Small_U8_सूचकांक((trie), __lead, __t2, __t3), 1) \
            :  /* U+0080..U+07FF */ \
                __lead >= 0xc2 && (__t1 = *(src) - 0x80) <= 0x3f && \
                (__lead = (int32_t)(trie)->सूचकांक[__lead & 0x1f] + __t1, 1))) { \
            ++(src); \
        } अन्यथा { \
            __lead = (trie)->दत्त_दीर्घता - UCPTRIE_ERROR_मानम्_NEG_दत्त_विस्थापनम्;  /* ill-formed*/ \
        } \
    } \
    (परिणाम) = दत्त_Access(trie, __lead); \
} UPRV_खण्डम्_MACRO_अन्तः

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
#परिभाषय UCPTRIE_FAST_U8_पूर्व(trie, दत्त_Access, start, src, परिणाम) UPRV_खण्डम्_MACRO_BEGIN { \
    int32_t __index = (uint8_t)*--(src); \
    यदि (!U8_IS_SINGLE(__index)) { \
        __index = ucptrie_आन्तरिकम्_U8_पूर्व_सूचकांक((trie), __index, (ध्रुव uint8_t *)(start), \
                                              (ध्रुव uint8_t *)(src)); \
        (src) -= __index & 7; \
        __index >>= 3; \
    } \
    (परिणाम) = दत्त_Access(trie, __index); \
} UPRV_खण्डम्_MACRO_अन्तः

/**
 * Returns a trie value for an ASCII code point, without range checking.
 *
 * @param trie (const UCPTrie *, in) the trie (of either fast or small type)
 * @param dataAccess UCPTRIE_16, UCPTRIE_32, or UCPTRIE_8 according to the trie’s value width
 * @param c (UChar32, in) the input code point; must be U+0000..U+007F
 * @return The ASCII code point's trie value.
 * @stable ICU 63
 */
#परिभाषय UCPTRIE_ASCII_GET(trie, दत्त_Access, c) दत्त_Access(trie, c)

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
#परिभाषय UCPTRIE_FAST_BMP_GET(trie, दत्त_Access, c) दत्त_Access(trie, UCPTRIE_FAST_सूचकांक(trie, c))

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
#परिभाषय UCPTRIE_FAST_SUPP_GET(trie, दत्त_Access, c) दत्त_Access(trie, UCPTRIE_SMALL_सूचकांक(trie, c))

/* Internal definitions ----------------------------------------------------- */

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN

/**
 * Internal implementation constants.
 * These are needed for the API macros, but users should not use these directly.
 * @internal
 */
गणना {
    /** @internal */
    UCPTRIE_FAST_सरणम् = 6,

    /** Number of entries in a data block for code points below the fast limit. 64=0x40 @internal */
    UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता = 1 << UCPTRIE_FAST_सरणम्,

    /** Mask for getting the lower bits for the in-fast-data-block offset. @internal */
    UCPTRIE_FAST_दत्त_आवरणम् = UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता - 1,

    /** @internal */
    UCPTRIE_SMALL_MAX = 0xfff,

    /**
     * Offset from dataLength (to be subtracted) for fetching the
     * value returned for out-of-range code points and ill-formed UTF-8/16.
     * @internal
     */
    UCPTRIE_ERROR_मानम्_NEG_दत्त_विस्थापनम् = 1,
    /**
     * Offset from dataLength (to be subtracted) for fetching the
     * value returned for code points highStart..U+10FFFF.
     * @internal
     */
    UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम् = 2
};

/* Internal functions and macros -------------------------------------------- */
// Do not conditionalize with #ifndef U_HIDE_INTERNAL_API, needed for public API

/** @internal */
U_CAPI int32_t U_EXPORT2
ucptrie_आन्तरिकम्_Small_सूचकांक(ध्रुव UCPTrie *trie, UChar32 c);

/** @internal */
U_CAPI int32_t U_EXPORT2
ucptrie_आन्तरिकम्_Small_U8_सूचकांक(ध्रुव UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3);

/**
 * Internal function for part of the UCPTRIE_FAST_U8_PREVxx() macro implementations.
 * Do not call directly.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucptrie_आन्तरिकम्_U8_पूर्व_सूचकांक(ध्रुव UCPTrie *trie, UChar32 c,
                            ध्रुव uint8_t *start, ध्रुव uint8_t *src);

/** Internal trie getter for a code point below the fast limit. Returns the data index. @internal */
#परिभाषय UCPTRIE_FAST_सूचकांक(trie, c) \
    ((int32_t)(trie)->सूचकांक[(c) >> UCPTRIE_FAST_सरणम्] + ((c) & UCPTRIE_FAST_दत्त_आवरणम्))

/** Internal trie getter for a code point at or above the fast limit. Returns the data index. @internal */
#परिभाषय UCPTRIE_SMALL_सूचकांक(trie, c) \
    ((c) >= (trie)->उच्चः_Start ? \
        (trie)->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम् : \
        ucptrie_आन्तरिकम्_Small_सूचकांक(trie, c))

/**
 * Internal trie getter for a code point, with checking that c is in U+0000..10FFFF.
 * Returns the data index.
 * @internal
 */
#परिभाषय UCPTRIE_CP_सूचकांक(trie, fastMax, c) \
    ((uint32_t)(c) <= (uint32_t)(fastMax) ? \
        UCPTRIE_FAST_सूचकांक(trie, c) : \
        (uint32_t)(c) <= 0x10ffff ? \
            UCPTRIE_SMALL_सूचकांक(trie, c) : \
            (trie)->दत्त_दीर्घता - UCPTRIE_ERROR_मानम्_NEG_दत्त_विस्थापनम्)

U_CDECL_अन्तः

#यदि_अन्तः  // U_IN_DOXYGEN

#यदि U_SHOW_CPLUSPLUS_API

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
U_DEFINE_LOCAL_OPEN_POINTER(LocalUCPTriePointer, UCPTrie, ucptrie_close);

U_NAMESPACE_अन्तः

#यदि_अन्तः  // U_SHOW_CPLUSPLUS_API

#यदि_अन्तः
