// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  udataswp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003jun05
*   created by: Markus W. Scherer
*
*   Definitions for ICU data transformations for different platforms,
*   changing between big- and little-endian data and/or between
*   charset families (ASCII<->EBCDIC).
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UDATASWP_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UDATASWP_H__

#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

/* forward declaration */

U_CDECL_BEGIN

ਬਣਤਰ UDataSwapper;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UDataSwapper UDataSwapper;

/**
 * Function type for data transformation.
 * Transforms data, or just returns the length of the data if
 * the input length is -1.
 * Swap functions assume that their data pointers are aligned properly.
 *
 * Quick implementation outline:
 * (best to copy and adapt and existing swapper implementation)
 * check that the data looks like the expected format
 * if(length<0) {
 *   preflight:
 *   never dereference outData
 *   read inData and determine the data size
 *   assume that inData is long enough for this
 * } else {
 *   outData can be NULL if length==0
 *   inData==outData (in-place swapping) possible but not required!
 *   verify that length>=(actual size)
 *   if there is a chance that not every byte up to size is reached
 *     due to padding etc.:
 *   if(inData!=outData) {
 *     memcpy(outData, inData, actual size);
 *   }
 *   swap contents
 * }
 * return actual size
 *
 * Further implementation notes:
 * - read integers from inData before swapping them
 *   because in-place swapping can make them unreadable
 * - compareInvChars compares a local Unicode string with already-swapped
 *   output charset strings
 *
 * @param ds Pointer to UDataSwapper containing global data about the
 *           transformation and function pointers for handling primitive
 *           types.
 * @param inData Pointer to the input data to be transformed or examined.
 * @param length Length of the data, counting bytes. May be -1 for preflighting.
 *               If length>=0, then transform the data.
 *               If length==-1, then only determine the length of the data.
 *               The length cannot be determined from the data itself for all
 *               types of data (e.g., not for simple arrays of integers).
 * @param outData Pointer to the output data buffer.
 *                If length>=0 (transformation), then the output buffer must
 *                have a capacity of at least length.
 *                If length==-1, then outData will not be used and can be NULL.
 * @param pErrorCode ICU UErrorCode parameter, must not be NULL and must
 *                   fulfill U_SUCCESS on input.
 * @return The actual length of the data.
 *
 * @see UDataSwapper
 * @internal ICU 2.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ int32_t U_CALLCONV
UData_ਅਦਲਾ_ਬਦਲੀ_Fn(ਅਚਲ UDataSwapper *ds,
            ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
            UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Convert one uint16_t from input to platform endianness.
 * @internal ICU 2.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint16_t U_CALLCONV
UDataReadUInt16(uint16_t x);

/**
 * Convert one uint32_t from input to platform endianness.
 * @internal ICU 2.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t U_CALLCONV
UDataReadUInt32(uint32_t x);

/**
 * Convert one uint16_t from platform to input endianness.
 * @internal ICU 2.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
UDataWriteUInt16(uint16_t *p, uint16_t x);

/**
 * Convert one uint32_t from platform to input endianness.
 * @internal ICU 2.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
UDataWriteUInt32(uint32_t *p, uint32_t x);

/**
 * Compare invariant-character strings, one in the output data and the
 * other one caller-provided in Unicode.
 * An output data string is compared because strings are usually swapped
 * before the rest of the data, to allow for sorting of string tables
 * according to the output charset.
 * You can use -1 for the length parameters of NUL-terminated strings as usual.
 * Returns Unicode code point order for invariant characters.
 * @internal ICU 2.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ int32_t U_CALLCONV
UDataCompareInvChars(ਅਚਲ UDataSwapper *ds,
                     ਅਚਲ ਅੱਖਰ *ਬਾਹਰਲਾ_String, int32_t ਬਾਹਰਲਾ_ਲੰਮਾਈ,
                     ਅਚਲ UChar *localString, int32_t local_ਲੰਮਾਈ);

/**
 * Function for message output when an error occurs during data swapping.
 * A format string and variable number of arguments are passed
 * like for vprintf().
 *
 * @param context A function-specific context pointer.
 * @param fmt The format string.
 * @param args The arguments for format string inserts.
 *
 * @internal ICU 2.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
UDataPrintError(ਖਾਲੀ *context, ਅਚਲ ਅੱਖਰ *fmt, va_list ਦਲੀਲਾਂ_2);

ਬਣਤਰ UDataSwapper {
    /** Input endianness. @internal ICU 2.8 */
    UBool ਵਿੱਚ_Is_Big_Endian;
    /** Input charset family. @see U_CHARSET_FAMILY @internal ICU 2.8 */
    uint8_t ਵਿੱਚ_Charset;
    /** Output endianness. @internal ICU 2.8 */
    UBool ਬਾਹਰਲਾ_Is_Big_Endian;
    /** Output charset family. @see U_CHARSET_FAMILY @internal ICU 2.8 */
    uint8_t ਬਾਹਰਲਾ_Charset;

    /* basic functions for reading data values */

    /** Convert one uint16_t from input to platform endianness. @internal ICU 2.8 */
    UDataReadUInt16 *readUInt16;
    /** Convert one uint32_t from input to platform endianness. @internal ICU 2.8 */
    UDataReadUInt32 *readUInt32;
    /** Compare an invariant-character output string with a local one. @internal ICU 2.8 */
    UDataCompareInvChars *compareInvChars;

    /* basic functions for writing data values */

    /** Convert one uint16_t from platform to input endianness. @internal ICU 2.8 */
    UDataWriteUInt16 *writeUInt16;
    /** Convert one uint32_t from platform to input endianness. @internal ICU 2.8 */
    UDataWriteUInt32 *writeUInt32;

    /* basic functions for data transformations */

    /** Transform an array of 16-bit integers. @internal ICU 2.8 */
    UData_ਅਦਲਾ_ਬਦਲੀ_Fn *ਅਦਲਾ_ਬਦਲੀ_Array16;
    /** Transform an array of 32-bit integers. @internal ICU 2.8 */
    UData_ਅਦਲਾ_ਬਦਲੀ_Fn *ਅਦਲਾ_ਬਦਲੀ_Array32;
    /** Transform an array of 64-bit integers. @internal ICU 53 */
    UData_ਅਦਲਾ_ਬਦਲੀ_Fn *ਅਦਲਾ_ਬਦਲੀ_Array64;
    /** Transform an invariant-character string. @internal ICU 2.8 */
    UData_ਅਦਲਾ_ਬਦਲੀ_Fn *ਅਦਲਾ_ਬਦਲੀ_Inv_Chars;

    /**
     * Function for message output when an error occurs during data swapping.
     * Can be NULL.
     * @internal ICU 2.8
     */
    UDataPrintError *printError;
    /** Context pointer for printError. @internal ICU 2.8 */
    ਖਾਲੀ *printErrorContext;
};

U_CDECL_ਸਿਰਾ

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool ਵਿੱਚ_Is_Big_Endian, uint8_t ਵਿੱਚ_Charset,
                  UBool ਬਾਹਰਲਾ_Is_Big_Endian, uint8_t ਬਾਹਰਲਾ_Charset,
                  UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Open a UDataSwapper for the given input data and the specified output
 * characteristics.
 * Values of -1 for any of the characteristics mean the local platform's
 * characteristics.
 *
 * @see udata_swap
 * @internal ICU 2.8
 */
U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_ਨਿਵੇਸ਼_ਅੰਕੜੇ(ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, int32_t ਲੰਮਾਈ,
                              UBool ਬਾਹਰਲਾ_Is_Big_Endian, uint8_t ਬਾਹਰਲਾ_Charset,
                              UError_ਕੋਡ *p_Error_ਕੋਡ);

U_CAPI ਖਾਲੀ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds);

/**
 * Read the beginning of an ICU data piece, recognize magic bytes,
 * swap the structure.
 * Set a U_UNSUPPORTED_ERROR if it does not look like an ICU data piece.
 *
 * @return The size of the data header, in bytes.
 *
 * @internal ICU 2.8
 */
U_CAPI int32_t U_EXPORT2
udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ਅਚਲ UDataSwapper *ds,
                     ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Convert one int16_t from input to platform endianness.
 * @internal ICU 2.8
 */
U_CAPI int16_t U_EXPORT2
udata_readInt16(ਅਚਲ UDataSwapper *ds, int16_t x);

/**
 * Convert one int32_t from input to platform endianness.
 * @internal ICU 2.8
 */
U_CAPI int32_t U_EXPORT2
udata_readInt32(ਅਚਲ UDataSwapper *ds, int32_t x);

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_ਅਦਲਾ_ਬਦਲੀ_Inv_String_ਖੰਡ(ਅਚਲ UDataSwapper *ds,
                         ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                         UError_ਕੋਡ *p_Error_ਕੋਡ);

U_CAPI ਖਾਲੀ U_EXPORT2
udata_printError(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਅੱਖਰ *fmt,
                 ...);

/* internal exports from putil.c -------------------------------------------- */

/* declared here to keep them out of the public putil.h */

/**
 * Swap invariant char * strings ASCII->EBCDIC.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_ebcdicFromAscii(ਅਚਲ UDataSwapper *ds,
                     ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Copy invariant ASCII char * strings and verify they are invariant.
 * @internal
 */
U_CFUNC int32_t
uprv_copyAscii(ਅਚਲ UDataSwapper *ds,
               ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
               UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Swap invariant char * strings EBCDIC->ASCII.
 * @internal
 */
U_CFUNC int32_t
uprv_asciiFromEbcdic(ਅਚਲ UDataSwapper *ds,
                     ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Copy invariant EBCDIC char * strings and verify they are invariant.
 * @internal
 */
U_CFUNC int32_t
uprv_copyEbcdic(ਅਚਲ UDataSwapper *ds,
                ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Compare ASCII invariant char * with Unicode invariant UChar *
 * @internal
 */
U_CFUNC int32_t
uprv_compareInvAscii(ਅਚਲ UDataSwapper *ds,
                     ਅਚਲ ਅੱਖਰ *ਬਾਹਰਲਾ_String, int32_t ਬਾਹਰਲਾ_ਲੰਮਾਈ,
                     ਅਚਲ UChar *localString, int32_t local_ਲੰਮਾਈ);

/**
 * Compare EBCDIC invariant char * with Unicode invariant UChar *
 * @internal
 */
U_CFUNC int32_t
uprv_compareInvEbcdic(ਅਚਲ UDataSwapper *ds,
                      ਅਚਲ ਅੱਖਰ *ਬਾਹਰਲਾ_String, int32_t ਬਾਹਰਲਾ_ਲੰਮਾਈ,
                      ਅਚਲ UChar *localString, int32_t local_ਲੰਮਾਈ);

/**
 * \def uprv_compareInvWithUChar
 * Compare an invariant-character strings with a UChar string
 * @internal
 */
#ਜੇ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_compareInvWithUChar uprv_compareInvAscii
#ਜਾਂਜੇ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_compareInvWithUChar uprv_compareInvEbcdic
#ਨਹੀਂਤਾਂ
#   error Unknown charset family!
#ਜੇ_ਅੰਤ

// utrie_swap.cpp -----------------------------------------------------------***

/**
 * Swaps a serialized UTrie.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utrie_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
           ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
           UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Swaps a serialized UTrie2.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utrie2_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
            ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
            UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Swaps a serialized UCPTrie.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucptrie_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
             ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
             UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Swaps a serialized UTrie, UTrie2, or UCPTrie.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utrie_ਅਦਲਾ_ਬਦਲੀ_Any_ਸੰਸਕਰਣ(ਅਚਲ UDataSwapper *ds,
                     ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ);

/* material... -------------------------------------------------------------- */

#ਜੇ 0

/* udata.h */

/**
 * Public API function in udata.c
 *
 * Same as udata_openChoice() but automatically swaps the data.
 * isAcceptable, if not NULL, may accept data with endianness and charset family
 * different from the current platform's properties.
 * If the data is acceptable and the platform properties do not match, then
 * the swap function is called to swap an allocated version of the data.
 * Preflighting may or may not be performed depending on whether the size of
 * the loaded data item is known.
 *
 * @param isAcceptable Same as for udata_openChoice(). May be NULL.
 *
 * @internal ICU 2.8
 */
U_CAPI UData_ਸਮ੍ਰਿਤੀ * U_EXPORT2
udata_open_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *name,
               UData_ਸਮ੍ਰਿਤੀ_Is_Acceptable *isAcceptable, ਖਾਲੀ *isAcceptableContext,
               UData_ਅਦਲਾ_ਬਦਲੀ_Fn *ਅਦਲਾ_ਬਦਲੀ,
               UDataPrintError *printError, ਖਾਲੀ *printErrorContext,
               UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
