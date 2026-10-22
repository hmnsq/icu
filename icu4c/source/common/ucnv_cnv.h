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

#यदि_अपरिभाषितम् UCNV_CNV_H
#परिभाषय UCNV_CNV_H

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "uset_imp.h"

U_CDECL_BEGIN

/* this is used in fromUnicode DBCS tables as an "unassigned" marker */
#परिभाषय missingCharMarker 0xFFFF

/*
 * #define missingUCharMarker 0xfffe
 *
 * commented out because there are actually two values used in toUnicode tables:
 * U+fffe "unassigned"
 * U+ffff "illegal"
 */

/** Forward declaration, see ucnv_bld.h */
संरचना UConverter_Shared_दत्त;
प्रकारनिर्देश संरचना UConverter_Shared_दत्त UConverter_Shared_दत्त;

/* function types for UConverterImpl ---------------------------------------- */

/* struct with arguments for UConverterLoad and ucnv_load() */
प्रकारनिर्देश संरचना {
    int32_t आकार_2;               /* sizeof(UConverterLoadArgs) */
    int32_t nestedLoads;        /* count nested ucnv_load() calls */
    UBool onlyTestIsLoadable;   /* input: don't actually load */
    UBool reserved0;            /* reserved - for good alignment of the pointers */
    int16_t reserved;           /* reserved - for good alignment of the pointers */
    uint32_t options;
    ध्रुव वर्ण *pkg, *name, *locale;
} UConverter_आरोपय_तर्काः;

#परिभाषय UCNV_आरोपय_तर्काः_INITIALIZER \
    { (int32_t)परिमाण(UConverter_आरोपय_तर्काः), 0, false, false, 0, 0, रिक्त, रिक्त, रिक्त }

प्रकारनिर्देश शून्य (*UConverter_आरोपय) (UConverter_Shared_दत्त *shared_दत्त,
                                UConverter_आरोपय_तर्काः *p_तर्काः,
                                ध्रुव uint8_t *raw, UError_संहिता *p_Error_संहिता);
प्रकारनिर्देश शून्य (*UConverterUnload) (UConverter_Shared_दत्त *shared_दत्त);

प्रकारनिर्देश शून्य (*UConverterOpen) (UConverter *cnv, UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *p_Error_संहिता);
प्रकारनिर्देश शून्य (*UConverterClose) (UConverter *cnv);

प्रकारनिर्देश गणना UConverterResetChoice {
    UCNV_RESET_BOTH,
    UCNV_RESET_गन्तव्यम्_UNICODE,
    UCNV_RESET_FROM_UNICODE
} UConverterResetChoice;

प्रकारनिर्देश शून्य (*UConverterReset) (UConverter *cnv, UConverterResetChoice choice);

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
प्रकारनिर्देश शून्य (*UConverter_गन्तव्यम्_Unicode) (UConverter_गन्तव्यम्_Unicode_तर्काः *, UError_संहिता *);

/*
 * Same rules as for UConverterToUnicode.
 * A lead surrogate is kept in fromUChar32 across buffers, and if an error
 * occurs, then the offending input code point must be put into fromUChar32
 * as well.
 */
प्रकारनिर्देश शून्य (*UConverterFromUnicode) (UConverter_From_Unicode_तर्काः *, UError_संहिता *);

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
प्रकारनिर्देश शून्य (*UConverterConvert) (UConverter_From_Unicode_तर्काः *pFromUArgs,
                                   UConverter_गन्तव्यम्_Unicode_तर्काः *p_गन्तव्यम्_UArgs,
                                   UError_संहिता *p_Error_संहिता);

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
प्रकारनिर्देश UChar32 (*UConverter_Get_अग्र_UChar) (UConverter_गन्तव्यम्_Unicode_तर्काः *, UError_संहिता *);

प्रकारनिर्देश शून्य (*UConverterGetStarters)(ध्रुव UConverter* converter,
                                      UBool starters[256],
                                      UError_संहिता *p_Error_संहिता);

/* If this function pointer is null or if the function returns null
 * the name field in static data struct should be returned by 
 * ucnv_getName() API function
 */
प्रकारनिर्देश ध्रुव वर्ण * (*UConverterGetName) (ध्रुव UConverter *cnv);

/**
 * Write the codepage substitution character.
 * If this function is not set, then ucnv_cbFromUWriteSub() writes
 * the substitution character from UConverter.
 * For stateful converters, it is typically necessary to handle this
 * specifically for the converter in order to properly maintain the state.
 */
प्रकारनिर्देश शून्य (*UConverterWriteSub) (UConverter_From_Unicode_तर्काः *p_तर्काः, int32_t विस्थापनम्_सूचकांक, UError_संहिता *p_Error_संहिता);

/**
 * For converter-specific safeClone processing
 * If this function is not set, then ucnv_safeClone assumes that the converter has no private data that changes
 * after the converter is done opening.
 * If this function is set, then it is called just after a memcpy() of
 * converter data to the new, empty converter, and is expected to set up
 * the initial state of the converter.  It is not expected to increment the
 * reference counts of the standard data types such as the shared data.
 */
प्रकारनिर्देश UConverter * (*UConverterSafeClone) (ध्रुव UConverter   *cnv, 
                                             शून्य               *राशिः_बफर,
                                             int32_t            *p_बफर_आकार, 
                                             UError_संहिता         *स्थितिः);

/**
 * Filters for some ucnv_getUnicodeSet() implementation code.
 */
प्रकारनिर्देश गणना UConverter_समुच्चयः_Filter {
    UCNV_समुच्चयः_FILTER_NONE,
    UCNV_समुच्चयः_FILTER_DBCS_ONLY,
    UCNV_समुच्चयः_FILTER_2022_CN,
    UCNV_समुच्चयः_FILTER_SJIS,
    UCNV_समुच्चयः_FILTER_GR94_DBCS,
    UCNV_समुच्चयः_FILTER_HZ,
    UCNV_समुच्चयः_FILTER_गणनम्
} UConverter_समुच्चयः_Filter;

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
प्रकारनिर्देश शून्य (*UConverter_Get_Unicode_समुच्चयः) (ध्रुव UConverter *cnv,
                                         ध्रुव USetAdder *sa,
                                         UConverter_Unicode_समुच्चयः which,
                                         UError_संहिता *p_Error_संहिता);

UBool CONVERSION_U_SUCCESS (UError_संहिता err);

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
संरचना UConverterImpl {
    UConverterType type;

    UConverter_आरोपय आरोपय;
    UConverterUnload unload;

    UConverterOpen उद्घाटय;
    UConverterClose close;
    UConverterReset reset;

    UConverter_गन्तव्यम्_Unicode गन्तव्यम्_Unicode;
    UConverter_गन्तव्यम्_Unicode गन्तव्यम्_Unicode_With_Offsets;
    UConverterFromUnicode fromUnicode;
    UConverterFromUnicode fromUnicodeWithOffsets;
    UConverter_Get_अग्र_UChar get_अग्र_UChar;

    UConverterGetStarters getStarters;
    UConverterGetName getName;
    UConverterWriteSub writeSub;
    UConverterSafeClone safeClone;
    UConverter_Get_Unicode_समुच्चयः get_Unicode_समुच्चयः;

    UConverterConvert गन्तव्यम्_UTF8;
    UConverterConvert fromUTF8;
};

बाह्य ध्रुव UConverter_Shared_दत्त
    _MBCSData, Latin1_दत्त,
    UTF8_दत्त, _UTF16BEData, _UTF16LEData, _UTF32BEData, _UTF32LEData,
    ISO2022_दत्त, 
    _LMBCSData1,_LMBCSData2, _LMBCSData3, _LMBCSData4, _LMBCSData5, _LMBCSData6,
    _LMBCSData8,_LMBCSData11,_LMBCSData16,_LMBCSData17,_LMBCSData18,_LMBCSData19,
    _HZData,_ISCIIData, _SCSUData, _ASCIIData,
    UTF7_दत्त, Bocu1_दत्त, UTF16_दत्त, UTF32_दत्त, CESU8_दत्त, _IMAPData, Compound_Text_दत्त;

U_CDECL_अन्तः

/** Always use fallbacks from codepage to Unicode */
#परिभाषय गन्तव्यम्_U_USE_FALLBACK(useFallback) true
#परिभाषय UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) true

/** Use fallbacks from Unicode to codepage when cnv->useFallback or for private-use code points */
#परिभाषय IS_वैयक्तिकम्_USE(c) ((uint32_t)((c)-0xe000)<0x1900 || (uint32_t)((c)-0xf0000)<0x20000)
#परिभाषय FROM_U_USE_FALLBACK(useFallback, c) ((useFallback) || IS_वैयक्तिकम्_USE(c))
#परिभाषय UCNV_FROM_U_USE_FALLBACK(cnv, c) FROM_U_USE_FALLBACK((cnv)->useFallback, c)

/**
 * Magic number for ucnv_getNextUChar(), returned by a
 * getNextUChar() implementation to indicate to use the converter's toUnicode()
 * instead of the native function.
 * @internal
 */
#परिभाषय UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U -9

U_CFUNC शून्य
ucnv_get_Complete_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                   ध्रुव USetAdder *sa,
                   UConverter_Unicode_समुच्चयः which,
                   UError_संहिता *p_Error_संहिता);

U_CFUNC शून्य
ucnv_get_Non_Surrogate_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                               ध्रुव USetAdder *sa,
                               UConverter_Unicode_समुच्चयः which,
                               UError_संहिता *p_Error_संहिता);

U_CFUNC शून्य
ucnv_from_UWrite_अष्टकानि(UConverter *cnv,
                     ध्रुव वर्ण *अष्टकानि, int32_t दीर्घता,
                     वर्ण **target, ध्रुव वर्ण *target_सीमा,
                     int32_t **offsets,
                     int32_t source_सूचकांक,
                     UError_संहिता *p_Error_संहिता);
U_CFUNC शून्य
ucnv_गन्तव्यम्_UWrite_UChars(UConverter *cnv,
                    ध्रुव UChar *uchars, int32_t दीर्घता,
                    UChar **target, ध्रुव UChar *target_सीमा,
                    int32_t **offsets,
                    int32_t source_सूचकांक,
                    UError_संहिता *p_Error_संहिता);

U_CFUNC शून्य
ucnv_गन्तव्यम्_UWrite_संहिता_Point(UConverter *cnv,
                       UChar32 c,
                       UChar **target, ध्रुव UChar *target_सीमा,
                       int32_t **offsets,
                       int32_t source_सूचकांक,
                       UError_संहिता *p_Error_संहिता);

#यदि_अन्तः

#यदि_अन्तः /* UCNV_CNV */
