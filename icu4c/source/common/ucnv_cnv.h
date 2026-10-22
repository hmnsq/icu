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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UCNV_CNV_H
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_CNV_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"

U_CDECL_BEGIN

/* this is used in fromUnicode DBCS tables as an "unassigned" marker */
#ವ್ಯಾಖ್ಯಾನಿಸು missingCharMarker 0xFFFF

/*
 * #define missingUCharMarker 0xfffe
 *
 * commented out because there are actually two values used in toUnicode tables:
 * U+fffe "unassigned"
 * U+ffff "illegal"
 */

/** Forward declaration, see ucnv_bld.h */
ರಚನೆ UConverter_Shared_ದತ್ತಾಂಶ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UConverter_Shared_ದತ್ತಾಂಶ UConverter_Shared_ದತ್ತಾಂಶ;

/* function types for UConverterImpl ---------------------------------------- */

/* struct with arguments for UConverterLoad and ucnv_load() */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    int32_t ಗಾತ್ರ_3;               /* sizeof(UConverterLoadArgs) */
    int32_t nestedLoads;        /* count nested ucnv_load() calls */
    UBool onlyTestIsLoadable;   /* input: don't actually load */
    UBool reserved0;            /* reserved - for good alignment of the pointers */
    int16_t reserved;           /* reserved - for good alignment of the pointers */
    uint32_t options;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pkg, *name, *locale;
} UConverter_ತುಂಬು_ವಾದಗಳು;

#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER \
    { (int32_t)ಗಾತ್ರ(UConverter_ತುಂಬು_ವಾದಗಳು), 0, false, false, 0, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ }

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverter_ತುಂಬು) (UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                                UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
                                ಸ್ಥಿರಾಂಕ uint8_t *raw, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverterUnload) (UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverterOpen) (UConverter *cnv, UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverterClose) (UConverter *cnv);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UConverterResetChoice {
    UCNV_RESET_BOTH,
    UCNV_RESET_ಗಮ್ಯ_UNICODE,
    UCNV_RESET_FROM_UNICODE
} UConverterResetChoice;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverterReset) (UConverter *cnv, UConverterResetChoice choice);

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverter_ಗಮ್ಯ_Unicode) (UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *, UError_ಸಂಕೇತ *);

/*
 * Same rules as for UConverterToUnicode.
 * A lead surrogate is kept in fromUChar32 across buffers, and if an error
 * occurs, then the offending input code point must be put into fromUChar32
 * as well.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverterFromUnicode) (UConverter_From_Unicode_ವಾದಗಳು *, UError_ಸಂಕೇತ *);

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverterConvert) (UConverter_From_Unicode_ವಾದಗಳು *pFromUArgs,
                                   UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ಗಮ್ಯ_UArgs,
                                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ UChar32 (*UConverter_Get_ಮುಂದಿನ_UChar) (UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *, UError_ಸಂಕೇತ *);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverterGetStarters)(ಸ್ಥಿರಾಂಕ UConverter* converter,
                                      UBool starters[256],
                                      UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/* If this function pointer is null or if the function returns null
 * the name field in static data struct should be returned by 
 * ucnv_getName() API function
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * (*UConverterGetName) (ಸ್ಥಿರಾಂಕ UConverter *cnv);

/**
 * Write the codepage substitution character.
 * If this function is not set, then ucnv_cbFromUWriteSub() writes
 * the substitution character from UConverter.
 * For stateful converters, it is typically necessary to handle this
 * specifically for the converter in order to properly maintain the state.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverterWriteSub) (UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು, int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * For converter-specific safeClone processing
 * If this function is not set, then ucnv_safeClone assumes that the converter has no private data that changes
 * after the converter is done opening.
 * If this function is set, then it is called just after a memcpy() of
 * converter data to the new, empty converter, and is expected to set up
 * the initial state of the converter.  It is not expected to increment the
 * reference counts of the standard data types such as the shared data.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ UConverter * (*UConverter_ಸುರಕ್ಷಿತ_Clone) (ಸ್ಥಿರಾಂಕ UConverter   *cnv, 
                                             ಶೂನ್ಯ               *ರಾಶಿ_ಬಫರ್,
                                             int32_t            *p_ಬಫರ್_ಗಾತ್ರ, 
                                             UError_ಸಂಕೇತ         *ಸ್ಥಿತಿ_2);

/**
 * Filters for some ucnv_getUnicodeSet() implementation code.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UConverter_ಗಣ_Filter {
    UCNV_ಗಣ_FILTER_NONE,
    UCNV_ಗಣ_FILTER_DBCS_ONLY,
    UCNV_ಗಣ_FILTER_2022_CN,
    UCNV_ಗಣ_FILTER_SJIS,
    UCNV_ಗಣ_FILTER_GR94_DBCS,
    UCNV_ಗಣ_FILTER_HZ,
    UCNV_ಗಣ_FILTER_ಎಣಿಕೆ
} UConverter_ಗಣ_Filter;

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*UConverter_Get_Unicode_ಗಣ) (ಸ್ಥಿರಾಂಕ UConverter *cnv,
                                         ಸ್ಥಿರಾಂಕ USetAdder *sa,
                                         UConverter_Unicode_ಗಣ which,
                                         UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

UBool CONVERSION_U_SUCCESS (UError_ಸಂಕೇತ err);

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
ರಚನೆ UConverterImpl {
    UConverterType type;

    UConverter_ತುಂಬು ತುಂಬು;
    UConverterUnload unload;

    UConverterOpen open;
    UConverterClose close;
    UConverterReset reset;

    UConverter_ಗಮ್ಯ_Unicode ಗಮ್ಯ_Unicode;
    UConverter_ಗಮ್ಯ_Unicode ಗಮ್ಯ_Unicode_With_Offsets;
    UConverterFromUnicode fromUnicode;
    UConverterFromUnicode fromUnicodeWithOffsets;
    UConverter_Get_ಮುಂದಿನ_UChar get_ಮುಂದಿನ_UChar;

    UConverterGetStarters getStarters;
    UConverterGetName getName;
    UConverterWriteSub writeSub;
    UConverter_ಸುರಕ್ಷಿತ_Clone ಸುರಕ್ಷಿತ_Clone;
    UConverter_Get_Unicode_ಗಣ get_Unicode_ಗಣ;

    UConverterConvert ಗಮ್ಯ_UTF8;
    UConverterConvert fromUTF8;
};

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ
    _MBCSData, Latin1_ದತ್ತಾಂಶ,
    UTF8_ದತ್ತಾಂಶ, _UTF16BEData, _UTF16LEData, _UTF32BEData, _UTF32LEData,
    ISO2022_ದತ್ತಾಂಶ, 
    _LMBCSData1,_LMBCSData2, _LMBCSData3, _LMBCSData4, _LMBCSData5, _LMBCSData6,
    _LMBCSData8,_LMBCSData11,_LMBCSData16,_LMBCSData17,_LMBCSData18,_LMBCSData19,
    _HZData,_ISCIIData, _SCSUData, _ASCIIData,
    UTF7_ದತ್ತಾಂಶ, Bocu1_ದತ್ತಾಂಶ, UTF16_ದತ್ತಾಂಶ, UTF32_ದತ್ತಾಂಶ, CESU8_ದತ್ತಾಂಶ, _IMAPData, Compound_ಪಠ್ಯ_ದತ್ತಾಂಶ;

U_CDECL_ಕೊನೆ

/** Always use fallbacks from codepage to Unicode */
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_U_USE_FALLBACK(useFallback) true
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) true

/** Use fallbacks from Unicode to codepage when cnv->useFallback or for private-use code points */
#ವ್ಯಾಖ್ಯಾನಿಸು IS_ಖಾಸಗಿ_USE(c) ((uint32_t)((c)-0xe000)<0x1900 || (uint32_t)((c)-0xf0000)<0x20000)
#ವ್ಯಾಖ್ಯಾನಿಸು FROM_U_USE_FALLBACK(useFallback, c) ((useFallback) || IS_ಖಾಸಗಿ_USE(c))
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_FROM_U_USE_FALLBACK(cnv, c) FROM_U_USE_FALLBACK((cnv)->useFallback, c)

/**
 * Magic number for ucnv_getNextUChar(), returned by a
 * getNextUChar() implementation to indicate to use the converter's toUnicode()
 * instead of the native function.
 * @internal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U -9

U_CFUNC ಶೂನ್ಯ
ucnv_get_Complete_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                   ಸ್ಥಿರಾಂಕ USetAdder *sa,
                   UConverter_Unicode_ಗಣ which,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CFUNC ಶೂನ್ಯ
ucnv_get_Non_Surrogate_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                               ಸ್ಥಿರಾಂಕ USetAdder *sa,
                               UConverter_Unicode_ಗಣ which,
                               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CFUNC ಶೂನ್ಯ
ucnv_from_UWrite_ಬೈಟುಗಳು(UConverter *cnv,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, int32_t ಉದ್ದಳತೆ,
                     ಅಕ್ಷರ **ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ,
                     int32_t **offsets,
                     int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);
U_CFUNC ಶೂನ್ಯ
ucnv_ಗಮ್ಯ_UWrite_UChars(UConverter *cnv,
                    ಸ್ಥಿರಾಂಕ UChar *uchars, int32_t ಉದ್ದಳತೆ,
                    UChar **ಗುರಿ, ಸ್ಥಿರಾಂಕ UChar *ಗುರಿ_ಎಲ್ಲೆ,
                    int32_t **offsets,
                    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CFUNC ಶೂನ್ಯ
ucnv_ಗಮ್ಯ_UWrite_ಸಂಕೇತ_ಬಿಂದು(UConverter *cnv,
                       UChar32 c,
                       UChar **ಗುರಿ, ಸ್ಥಿರಾಂಕ UChar *ಗುರಿ_ಎಲ್ಲೆ,
                       int32_t **offsets,
                       int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* UCNV_CNV */
