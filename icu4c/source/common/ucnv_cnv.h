// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
*   ucnv_cnv.h:
*   Definitions for converter implementations.
*
* Modification History:
*
*   Date        Name        Description
*   05/09/00    helena      Added implementation to handle fallback mappings.
*   06/29/2000  helena      Major rewrite of the callback APIs.
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UCNV_CNV_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_CNV_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_err.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uset_imp.h"

U_CDECL_BEGIN

/* this is used in fromUnicode DBCS tables as an "unassigned" marker */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ missingCharMarker 0xFFFF

/*
 * #define missingUCharMarker 0xfffe
 *
 * commented out because there are actually two values used in toUnicode tables:
 * U+fffe "unassigned"
 * U+ffff "illegal"
 */

/** Forward declaration, see ucnv_bld.h */
ਬਣਤਰ UConverter_Shared_ਅੰਕੜੇ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UConverter_Shared_ਅੰਕੜੇ UConverter_Shared_ਅੰਕੜੇ;

/* function types for UConverterImpl ---------------------------------------- */

/* struct with arguments for UConverterLoad and ucnv_load() */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    int32_t ਮਾਪ_2;               /* sizeof(UConverterLoadArgs) */
    int32_t nestedLoads;        /* count nested ucnv_load() calls */
    UBool onlyTestIsLoadable;   /* input: don't actually load */
    UBool reserved0;            /* reserved - for good alignment of the pointers */
    int16_t reserved;           /* reserved - for good alignment of the pointers */
    uint32_t options;
    ਅਚਲ ਅੱਖਰ *pkg, *name, *locale;
} UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER \
    { (int32_t)ਆਕਾਰ(UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ), 0, false, false, 0, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ }

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverter_ਲੱਦੋ) (UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                                UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                                ਅਚਲ uint8_t *raw, UError_ਕੋਡ *p_Error_ਕੋਡ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverterUnload) (UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverterOpen) (UConverter *cnv, UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *p_Error_ਕੋਡ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverterClose) (UConverter *cnv);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UConverterResetChoice {
    UCNV_RESET_BOTH,
    UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE,
    UCNV_RESET_FROM_UNICODE
} UConverterResetChoice;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverterReset) (UConverter *cnv, UConverterResetChoice choice);

/*
 * Converter implementation function(s) for ucnv_toUnicode().
 * If the toUnicodeWithOffsets function pointer is NULL,
 * then the toUnicode function will be used and the offsets will be set to -1.
 *
 * Must maintain state across buffers. Use toUBytes[toULength] for partial input
 * sequences; it will be checked in ucnv.c at the end of the input stream
 * to detect truncated input.
 * Some converters may need additional detection and may then set U_TRUNCATED_CHAR_FOUND.
 *
 * The toUnicodeWithOffsets must write exactly as many offset values as target
 * units. Write offset values of -1 for when the source index corresponding to
 * the output unit is not known (e.g., the character started in an earlier buffer).
 * The pArgs->offsets pointer need not be moved forward.
 *
 * At function return, either one of the following conditions must be true:
 * - U_BUFFER_OVERFLOW_ERROR and the target is full: target==targetLimit
 * - another error code with toUBytes[toULength] set to the offending input
 * - no error, and the source is consumed: source==sourceLimit
 *
 * The ucnv.c code will handle the end of the input (reset)
 * (reset, and truncation detection) and callbacks.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverter_ਮੰਜ਼ਿਲ_Unicode) (UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *, UError_ਕੋਡ *);

/*
 * Same rules as for UConverterToUnicode.
 * A lead surrogate is kept in fromUChar32 across buffers, and if an error
 * occurs, then the offending input code point must be put into fromUChar32
 * as well.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverterFromUnicode) (UConverter_From_Unicode_ਦਲੀਲਾਂ *, UError_ਕੋਡ *);

/*
 * Converter implementation function for ucnv_convertEx(), for direct conversion
 * between two charsets without pivoting through UTF-16.
 * The rules are the same as for UConverterToUnicode and UConverterFromUnicode.
 * In addition,
 * - The toUnicode side must behave and keep state exactly like the
 *   UConverterToUnicode implementation for the same source charset.
 * - A U_USING_DEFAULT_WARNING can be set to request to temporarily fall back
 *   to pivoting. When this function is called, the conversion framework makes
 *   sure that this warning is not set on input.
 * - Continuing a partial match and flushing the toUnicode replay buffer
 *   are handled by pivoting, using the toUnicode and fromUnicode functions.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverterConvert) (UConverter_From_Unicode_ਦਲੀਲਾਂ *pFromUArgs,
                                   UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਮੰਜ਼ਿਲ_UArgs,
                                   UError_ਕੋਡ *p_Error_ਕੋਡ);

/*
 * Converter implementation function for ucnv_getNextUChar().
 * If the function pointer is NULL, then the toUnicode function will be used.
 *
 * Will be called at a character boundary (toULength==0).
 * May return with
 * - U_INDEX_OUTOFBOUNDS_ERROR if there was no output for the input
 *   (the return value will be ignored)
 * - U_TRUNCATED_CHAR_FOUND or another error code (never U_BUFFER_OVERFLOW_ERROR!)
 *   with toUBytes[toULength] set to the offending input
 *   (the return value will be ignored)
 * - return UCNV_GET_NEXT_UCHAR_USE_TO_U, without moving the source pointer,
 *   to indicate that the ucnv.c code shall call the toUnicode function instead
 * - return a real code point result
 *
 * Unless UCNV_GET_NEXT_UCHAR_USE_TO_U is returned, the source bytes must be consumed.
 *
 * The ucnv.c code will handle the end of the input (reset)
 * (except for truncation detection!) and callbacks.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ UChar32 (*UConverter_Get_ਅਗਲਾ_UChar) (UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *, UError_ਕੋਡ *);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverterGetStarters)(ਅਚਲ UConverter* converter,
                                      UBool starters[256],
                                      UError_ਕੋਡ *p_Error_ਕੋਡ);

/* If this function pointer is null or if the function returns null
 * the name field in static data struct should be returned by 
 * ucnv_getName() API function
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਚਲ ਅੱਖਰ * (*UConverterGetName) (ਅਚਲ UConverter *cnv);

/**
 * Write the codepage substitution character.
 * If this function is not set, then ucnv_cbFromUWriteSub() writes
 * the substitution character from UConverter.
 * For stateful converters, it is typically necessary to handle this
 * specifically for the converter in order to properly maintain the state.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverterWriteSub) (UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ, UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * For converter-specific safeClone processing
 * If this function is not set, then ucnv_safeClone assumes that the converter has no private data that changes
 * after the converter is done opening.
 * If this function is set, then it is called just after a memcpy() of
 * converter data to the new, empty converter, and is expected to set up
 * the initial state of the converter.  It is not expected to increment the
 * reference counts of the standard data types such as the shared data.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ UConverter * (*UConverter_ਸੁਰੱਖਿਅਤ_Clone) (ਅਚਲ UConverter   *cnv, 
                                             ਖਾਲੀ               *ਢੇਰ_ਬਫਰ,
                                             int32_t            *p_ਬਫਰ_ਮਾਪ, 
                                             UError_ਕੋਡ         *ਸਥਿਤੀ);

/**
 * Filters for some ucnv_getUnicodeSet() implementation code.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UConverter_ਸੈੱਟ_Filter {
    UCNV_ਸੈੱਟ_FILTER_NONE,
    UCNV_ਸੈੱਟ_FILTER_DBCS_ONLY,
    UCNV_ਸੈੱਟ_FILTER_2022_CN,
    UCNV_ਸੈੱਟ_FILTER_SJIS,
    UCNV_ਸੈੱਟ_FILTER_GR94_DBCS,
    UCNV_ਸੈੱਟ_FILTER_HZ,
    UCNV_ਸੈੱਟ_FILTER_ਗਿਣਤੀ
} UConverter_ਸੈੱਟ_Filter;

/**
 * Fills the set of Unicode code points that can be converted by an ICU converter.
 * The API function ucnv_getUnicodeSet() clears the USet before calling
 * the converter's getUnicodeSet() implementation; the converter should only
 * add the appropriate code points to allow recursive use.
 * For example, the ISO-2022-JP converter will call each subconverter's
 * getUnicodeSet() implementation to consecutively add code points to
 * the same USet, which will result in a union of the sets of all subconverters.
 *
 * For more documentation, see ucnv_getUnicodeSet() in ucnv.h.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*UConverter_Get_Unicode_ਸੈੱਟ) (ਅਚਲ UConverter *cnv,
                                         ਅਚਲ USetAdder *sa,
                                         UConverter_Unicode_ਸੈੱਟ which,
                                         UError_ਕੋਡ *p_Error_ਕੋਡ);

UBool CONVERSION_U_SUCCESS (UError_ਕੋਡ err);

/**
 * UConverterImpl contains all the data and functions for a converter type.
 * Its function pointers work much like a C++ vtable.
 * Many converter types need to define only a subset of the functions;
 * when a function pointer is NULL, then a default action will be performed.
 *
 * Every converter type must implement toUnicode, fromUnicode, and getNextUChar,
 * otherwise the converter may crash.
 * Every converter type that has variable-length codepage sequences should
 * also implement toUnicodeWithOffsets and fromUnicodeWithOffsets for
 * correct offset handling.
 * All other functions may or may not be implemented - it depends only on
 * whether the converter type needs them.
 *
 * When open() fails, then close() will be called, if present.
 */
ਬਣਤਰ UConverterImpl {
    UConverterType type;

    UConverter_ਲੱਦੋ ਲੱਦੋ;
    UConverterUnload unload;

    UConverterOpen open;
    UConverterClose close;
    UConverterReset reset;

    UConverter_ਮੰਜ਼ਿਲ_Unicode ਮੰਜ਼ਿਲ_Unicode;
    UConverter_ਮੰਜ਼ਿਲ_Unicode ਮੰਜ਼ਿਲ_Unicode_With_Offsets;
    UConverterFromUnicode fromUnicode;
    UConverterFromUnicode fromUnicodeWithOffsets;
    UConverter_Get_ਅਗਲਾ_UChar get_ਅਗਲਾ_UChar;

    UConverterGetStarters getStarters;
    UConverterGetName getName;
    UConverterWriteSub writeSub;
    UConverter_ਸੁਰੱਖਿਅਤ_Clone ਸੁਰੱਖਿਅਤ_Clone;
    UConverter_Get_Unicode_ਸੈੱਟ get_Unicode_ਸੈੱਟ;

    UConverterConvert ਮੰਜ਼ਿਲ_UTF8;
    UConverterConvert fromUTF8;
};

ਬਾਹਰੀ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ
    _MBCSData, Latin1_ਅੰਕੜੇ,
    UTF8_ਅੰਕੜੇ, _UTF16BEData, _UTF16LEData, _UTF32BEData, _UTF32LEData,
    ISO2022_ਅੰਕੜੇ, 
    _LMBCSData1,_LMBCSData2, _LMBCSData3, _LMBCSData4, _LMBCSData5, _LMBCSData6,
    _LMBCSData8,_LMBCSData11,_LMBCSData16,_LMBCSData17,_LMBCSData18,_LMBCSData19,
    _HZData,_ISCIIData, _SCSUData, _ASCIIData,
    UTF7_ਅੰਕੜੇ, Bocu1_ਅੰਕੜੇ, UTF16_ਅੰਕੜੇ, UTF32_ਅੰਕੜੇ, CESU8_ਅੰਕੜੇ, _IMAPData, Compound_ਪਾਠ_ਅੰਕੜੇ;

U_CDECL_ਸਿਰਾ

/** Always use fallbacks from codepage to Unicode */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_U_USE_FALLBACK(useFallback) true
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) true

/** Use fallbacks from Unicode to codepage when cnv->useFallback or for private-use code points */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_ਨਿੱਜੀ_USE(c) ((uint32_t)((c)-0xe000)<0x1900 || (uint32_t)((c)-0xf0000)<0x20000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FROM_U_USE_FALLBACK(useFallback, c) ((useFallback) || IS_ਨਿੱਜੀ_USE(c))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_FROM_U_USE_FALLBACK(cnv, c) FROM_U_USE_FALLBACK((cnv)->useFallback, c)

/**
 * Magic number for ucnv_getNextUChar(), returned by a
 * getNextUChar() implementation to indicate to use the converter's toUnicode()
 * instead of the native function.
 * @internal
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U -9

U_CFUNC ਖਾਲੀ
ucnv_get_Complete_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                   ਅਚਲ USetAdder *sa,
                   UConverter_Unicode_ਸੈੱਟ which,
                   UError_ਕੋਡ *p_Error_ਕੋਡ);

U_CFUNC ਖਾਲੀ
ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                               ਅਚਲ USetAdder *sa,
                               UConverter_Unicode_ਸੈੱਟ which,
                               UError_ਕੋਡ *p_Error_ਕੋਡ);

U_CFUNC ਖਾਲੀ
ucnv_from_UWrite_ਬਾਈਟਾਂ(UConverter *cnv,
                     ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, int32_t ਲੰਮਾਈ,
                     ਅੱਖਰ **ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                     int32_t **offsets,
                     int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ);
U_CFUNC ਖਾਲੀ
ucnv_ਮੰਜ਼ਿਲ_UWrite_UChars(UConverter *cnv,
                    ਅਚਲ UChar *uchars, int32_t ਲੰਮਾਈ,
                    UChar **ਨਿਸ਼ਾਨਾ, ਅਚਲ UChar *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                    int32_t **offsets,
                    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ);

U_CFUNC ਖਾਲੀ
ucnv_ਮੰਜ਼ਿਲ_UWrite_ਕੋਡ_ਬਿੰਦੂ(UConverter *cnv,
                       UChar32 c,
                       UChar **ਨਿਸ਼ਾਨਾ, ਅਚਲ UChar *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                       int32_t **offsets,
                       int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* UCNV_CNV */
