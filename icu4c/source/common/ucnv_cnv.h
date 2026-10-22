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

#جيڪڏهن_وصف_نه_ٿيل UCNV_CNV_H
#وصف_ڪريو UCNV_CNV_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/ucnv_err.h"
#ضمن "unicode/uset.h"
#ضمن "uset_imp.h"

U_CDECL_BEGIN

/* this is used in fromUnicode DBCS tables as an "unassigned" marker */
#وصف_ڪريو missingCharMarker 0xFFFF

/*
 * #define missingUCharMarker 0xfffe
 *
 * commented out because there are actually two values used in toUnicode tables:
 * U+fffe "unassigned"
 * U+ffff "illegal"
 */

/** Forward declaration, see ucnv_bld.h */
بنية UConverter_Shared_معلومات;
تعريفنوع بنية UConverter_Shared_معلومات UConverter_Shared_معلومات;

/* function types for UConverterImpl ---------------------------------------- */

/* struct with arguments for UConverterLoad and ucnv_load() */
تعريفنوع بنية {
    int32_t سائيز_2;               /* sizeof(UConverterLoadArgs) */
    int32_t nestedLoads;        /* count nested ucnv_load() calls */
    UBool onlyTestIsLoadable;   /* input: don't actually load */
    UBool reserved0;            /* reserved - for good alignment of the pointers */
    int16_t reserved;           /* reserved - for good alignment of the pointers */
    uint32_t options;
    ثابت حرف *pkg, *name, *locale;
} UConverter_ڀريو_دليلون;

#وصف_ڪريو UCNV_ڀريو_دليلون_INITIALIZER \
    { (int32_t)حجم(UConverter_ڀريو_دليلون), 0, false, false, 0, 0, عدم, عدم, عدم }

تعريفنوع فراغ (*UConverter_ڀريو) (UConverter_Shared_معلومات *shared_معلومات,
                                UConverter_ڀريو_دليلون *p_دليلون,
                                ثابت uint8_t *raw, UError_ڪوڊ *p_Error_ڪوڊ);
تعريفنوع فراغ (*UConverterUnload) (UConverter_Shared_معلومات *shared_معلومات);

تعريفنوع فراغ (*UConverterOpen) (UConverter *cnv, UConverter_ڀريو_دليلون *p_دليلون, UError_ڪوڊ *p_Error_ڪوڊ);
تعريفنوع فراغ (*UConverterClose) (UConverter *cnv);

تعريفنوع تعداد UConverterResetChoice {
    UCNV_RESET_BOTH,
    UCNV_RESET_منزل_UNICODE,
    UCNV_RESET_FROM_UNICODE
} UConverterResetChoice;

تعريفنوع فراغ (*UConverterReset) (UConverter *cnv, UConverterResetChoice choice);

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
تعريفنوع فراغ (*UConverter_منزل_Unicode) (UConverter_منزل_Unicode_دليلون *, UError_ڪوڊ *);

/*
 * Same rules as for UConverterToUnicode.
 * A lead surrogate is kept in fromUChar32 across buffers, and if an error
 * occurs, then the offending input code point must be put into fromUChar32
 * as well.
 */
تعريفنوع فراغ (*UConverterFromUnicode) (UConverter_From_Unicode_دليلون *, UError_ڪوڊ *);

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
تعريفنوع فراغ (*UConverterConvert) (UConverter_From_Unicode_دليلون *pFromUArgs,
                                   UConverter_منزل_Unicode_دليلون *p_منزل_UArgs,
                                   UError_ڪوڊ *p_Error_ڪوڊ);

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
تعريفنوع UChar32 (*UConverter_Get_ايندڙ_UChar) (UConverter_منزل_Unicode_دليلون *, UError_ڪوڊ *);

تعريفنوع فراغ (*UConverterGetStarters)(ثابت UConverter* converter,
                                      UBool starters[256],
                                      UError_ڪوڊ *p_Error_ڪوڊ);

/* If this function pointer is null or if the function returns null
 * the name field in static data struct should be returned by 
 * ucnv_getName() API function
 */
تعريفنوع ثابت حرف * (*UConverterGetName) (ثابت UConverter *cnv);

/**
 * Write the codepage substitution character.
 * If this function is not set, then ucnv_cbFromUWriteSub() writes
 * the substitution character from UConverter.
 * For stateful converters, it is typically necessary to handle this
 * specifically for the converter in order to properly maintain the state.
 */
تعريفنوع فراغ (*UConverterWriteSub) (UConverter_From_Unicode_دليلون *p_دليلون, int32_t وٿي_اشاريو, UError_ڪوڊ *p_Error_ڪوڊ);

/**
 * For converter-specific safeClone processing
 * If this function is not set, then ucnv_safeClone assumes that the converter has no private data that changes
 * after the converter is done opening.
 * If this function is set, then it is called just after a memcpy() of
 * converter data to the new, empty converter, and is expected to set up
 * the initial state of the converter.  It is not expected to increment the
 * reference counts of the standard data types such as the shared data.
 */
تعريفنوع UConverter * (*UConverter_محفوظ_Clone) (ثابت UConverter   *cnv, 
                                             فراغ               *ڍير_بفر,
                                             int32_t            *p_بفر_سائيز, 
                                             UError_ڪوڊ         *حالت);

/**
 * Filters for some ucnv_getUnicodeSet() implementation code.
 */
تعريفنوع تعداد UConverter_سيٽ_Filter {
    UCNV_سيٽ_FILTER_NONE,
    UCNV_سيٽ_FILTER_DBCS_ONLY,
    UCNV_سيٽ_FILTER_2022_CN,
    UCNV_سيٽ_FILTER_SJIS,
    UCNV_سيٽ_FILTER_GR94_DBCS,
    UCNV_سيٽ_FILTER_HZ,
    UCNV_سيٽ_FILTER_ڳڻپ
} UConverter_سيٽ_Filter;

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
تعريفنوع فراغ (*UConverter_Get_Unicode_سيٽ) (ثابت UConverter *cnv,
                                         ثابت USetAdder *sa,
                                         UConverter_Unicode_سيٽ which,
                                         UError_ڪوڊ *p_Error_ڪوڊ);

UBool CONVERSION_U_SUCCESS (UError_ڪوڊ err);

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
بنية UConverterImpl {
    UConverterType type;

    UConverter_ڀريو ڀريو;
    UConverterUnload unload;

    UConverterOpen open;
    UConverterClose close;
    UConverterReset reset;

    UConverter_منزل_Unicode منزل_Unicode;
    UConverter_منزل_Unicode منزل_Unicode_With_Offsets;
    UConverterFromUnicode fromUnicode;
    UConverterFromUnicode fromUnicodeWithOffsets;
    UConverter_Get_ايندڙ_UChar get_ايندڙ_UChar;

    UConverterGetStarters getStarters;
    UConverterGetName getName;
    UConverterWriteSub writeSub;
    UConverter_محفوظ_Clone محفوظ_Clone;
    UConverter_Get_Unicode_سيٽ get_Unicode_سيٽ;

    UConverterConvert منزل_UTF8;
    UConverterConvert fromUTF8;
};

خارجي ثابت UConverter_Shared_معلومات
    _MBCSData, Latin1_معلومات,
    UTF8_معلومات, _UTF16BEData, _UTF16LEData, _UTF32BEData, _UTF32LEData,
    ISO2022_معلومات, 
    _LMBCSData1,_LMBCSData2, _LMBCSData3, _LMBCSData4, _LMBCSData5, _LMBCSData6,
    _LMBCSData8,_LMBCSData11,_LMBCSData16,_LMBCSData17,_LMBCSData18,_LMBCSData19,
    _HZData,_ISCIIData, _SCSUData, _ASCIIData,
    UTF7_معلومات, Bocu1_معلومات, UTF16_معلومات, UTF32_معلومات, CESU8_معلومات, _IMAPData, Compound_متن_معلومات;

U_CDECL_پڇاڙي

/** Always use fallbacks from codepage to Unicode */
#وصف_ڪريو منزل_U_USE_FALLBACK(useFallback) true
#وصف_ڪريو UCNV_منزل_U_USE_FALLBACK(cnv) true

/** Use fallbacks from Unicode to codepage when cnv->useFallback or for private-use code points */
#وصف_ڪريو IS_ذاتي_USE(c) ((uint32_t)((c)-0xe000)<0x1900 || (uint32_t)((c)-0xf0000)<0x20000)
#وصف_ڪريو FROM_U_USE_FALLBACK(useFallback, c) ((useFallback) || IS_ذاتي_USE(c))
#وصف_ڪريو UCNV_FROM_U_USE_FALLBACK(cnv, c) FROM_U_USE_FALLBACK((cnv)->useFallback, c)

/**
 * Magic number for ucnv_getNextUChar(), returned by a
 * getNextUChar() implementation to indicate to use the converter's toUnicode()
 * instead of the native function.
 * @internal
 */
#وصف_ڪريو UCNV_GET_ايندڙ_UCHAR_USE_منزل_U -9

U_CFUNC فراغ
ucnv_get_Complete_Unicode_سيٽ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سيٽ which,
                   UError_ڪوڊ *p_Error_ڪوڊ);

U_CFUNC فراغ
ucnv_get_Non_Surrogate_Unicode_سيٽ(ثابت UConverter *cnv,
                               ثابت USetAdder *sa,
                               UConverter_Unicode_سيٽ which,
                               UError_ڪوڊ *p_Error_ڪوڊ);

U_CFUNC فراغ
ucnv_from_UWrite_بائيٽون(UConverter *cnv,
                     ثابت حرف *بائيٽون, int32_t ڊيگهائي,
                     حرف **هدف, ثابت حرف *هدف_حد_بندي,
                     int32_t **offsets,
                     int32_t ذريعو_اشاريو,
                     UError_ڪوڊ *p_Error_ڪوڊ);
U_CFUNC فراغ
ucnv_منزل_UWrite_UChars(UConverter *cnv,
                    ثابت UChar *uchars, int32_t ڊيگهائي,
                    UChar **هدف, ثابت UChar *هدف_حد_بندي,
                    int32_t **offsets,
                    int32_t ذريعو_اشاريو,
                    UError_ڪوڊ *p_Error_ڪوڊ);

U_CFUNC فراغ
ucnv_منزل_UWrite_ڪوڊ_نقطو(UConverter *cnv,
                       UChar32 c,
                       UChar **هدف, ثابت UChar *هدف_حد_بندي,
                       int32_t **offsets,
                       int32_t ذريعو_اشاريو,
                       UError_ڪوڊ *p_Error_ڪوڊ);

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* UCNV_CNV */
