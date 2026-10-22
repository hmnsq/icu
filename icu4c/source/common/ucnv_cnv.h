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

#నిర్వచితం_కాకపోతే UCNV_CNV_H
#నిర్వచించు UCNV_CNV_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ucnv_err.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "uset_imp.h"

U_CDECL_BEGIN

/* this is used in fromUnicode DBCS tables as an "unassigned" marker */
#నిర్వచించు missingCharMarker 0xFFFF

/*
 * #define missingUCharMarker 0xfffe
 *
 * commented out because there are actually two values used in toUnicode tables:
 * U+fffe "unassigned"
 * U+ffff "illegal"
 */

/** Forward declaration, see ucnv_bld.h */
నిర్మాణం UConverter_Shared_దత్తాంశం;
రకనిర్వచనం నిర్మాణం UConverter_Shared_దత్తాంశం UConverter_Shared_దత్తాంశం;

/* function types for UConverterImpl ---------------------------------------- */

/* struct with arguments for UConverterLoad and ucnv_load() */
రకనిర్వచనం నిర్మాణం {
    int32_t కొలత_2;               /* sizeof(UConverterLoadArgs) */
    int32_t nestedLoads;        /* count nested ucnv_load() calls */
    UBool onlyTestIsLoadable;   /* input: don't actually load */
    UBool reserved0;            /* reserved - for good alignment of the pointers */
    int16_t reserved;           /* reserved - for good alignment of the pointers */
    uint32_t options;
    స్థిరరాశి అక్షరం *pkg, *name, *locale;
} UConverter_ఎక్కించు_వాదనలు;

#నిర్వచించు UCNV_ఎక్కించు_వాదనలు_INITIALIZER \
    { (int32_t)పరిమాణం(UConverter_ఎక్కించు_వాదనలు), 0, false, false, 0, 0, ఖాళీ, ఖాళీ, ఖాళీ }

రకనిర్వచనం శూన్య (*UConverter_ఎక్కించు) (UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                                UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
                                స్థిరరాశి uint8_t *raw, UError_సంకేతం *p_Error_సంకేతం);
రకనిర్వచనం శూన్య (*UConverterUnload) (UConverter_Shared_దత్తాంశం *shared_దత్తాంశం);

రకనిర్వచనం శూన్య (*UConverterOpen) (UConverter *cnv, UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *p_Error_సంకేతం);
రకనిర్వచనం శూన్య (*UConverterClose) (UConverter *cnv);

రకనిర్వచనం గణన UConverterResetChoice {
    UCNV_RESET_BOTH,
    UCNV_RESET_గమ్యం_UNICODE,
    UCNV_RESET_FROM_UNICODE
} UConverterResetChoice;

రకనిర్వచనం శూన్య (*UConverterReset) (UConverter *cnv, UConverterResetChoice choice);

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
రకనిర్వచనం శూన్య (*UConverter_గమ్యం_Unicode) (UConverter_గమ్యం_Unicode_వాదనలు *, UError_సంకేతం *);

/*
 * Same rules as for UConverterToUnicode.
 * A lead surrogate is kept in fromUChar32 across buffers, and if an error
 * occurs, then the offending input code point must be put into fromUChar32
 * as well.
 */
రకనిర్వచనం శూన్య (*UConverterFromUnicode) (UConverter_From_Unicode_వాదనలు *, UError_సంకేతం *);

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
రకనిర్వచనం శూన్య (*UConverterConvert) (UConverter_From_Unicode_వాదనలు *pFromUArgs,
                                   UConverter_గమ్యం_Unicode_వాదనలు *p_గమ్యం_UArgs,
                                   UError_సంకేతం *p_Error_సంకేతం);

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
రకనిర్వచనం UChar32 (*UConverter_Get_తదుపరి_UChar) (UConverter_గమ్యం_Unicode_వాదనలు *, UError_సంకేతం *);

రకనిర్వచనం శూన్య (*UConverterGetStarters)(స్థిరరాశి UConverter* converter,
                                      UBool starters[256],
                                      UError_సంకేతం *p_Error_సంకేతం);

/* If this function pointer is null or if the function returns null
 * the name field in static data struct should be returned by 
 * ucnv_getName() API function
 */
రకనిర్వచనం స్థిరరాశి అక్షరం * (*UConverterGetName) (స్థిరరాశి UConverter *cnv);

/**
 * Write the codepage substitution character.
 * If this function is not set, then ucnv_cbFromUWriteSub() writes
 * the substitution character from UConverter.
 * For stateful converters, it is typically necessary to handle this
 * specifically for the converter in order to properly maintain the state.
 */
రకనిర్వచనం శూన్య (*UConverterWriteSub) (UConverter_From_Unicode_వాదనలు *p_వాదనలు, int32_t స్థానభ్రంశం_సూచీ, UError_సంకేతం *p_Error_సంకేతం);

/**
 * For converter-specific safeClone processing
 * If this function is not set, then ucnv_safeClone assumes that the converter has no private data that changes
 * after the converter is done opening.
 * If this function is set, then it is called just after a memcpy() of
 * converter data to the new, empty converter, and is expected to set up
 * the initial state of the converter.  It is not expected to increment the
 * reference counts of the standard data types such as the shared data.
 */
రకనిర్వచనం UConverter * (*UConverter_సురక్షిత_Clone) (స్థిరరాశి UConverter   *cnv, 
                                             శూన్య               *దొంతర_బఫరు,
                                             int32_t            *p_బఫరు_కొలత, 
                                             UError_సంకేతం         *స్థితి_2);

/**
 * Filters for some ucnv_getUnicodeSet() implementation code.
 */
రకనిర్వచనం గణన UConverter_సమితి_Filter {
    UCNV_సమితి_FILTER_NONE,
    UCNV_సమితి_FILTER_DBCS_ONLY,
    UCNV_సమితి_FILTER_2022_CN,
    UCNV_సమితి_FILTER_SJIS,
    UCNV_సమితి_FILTER_GR94_DBCS,
    UCNV_సమితి_FILTER_HZ,
    UCNV_సమితి_FILTER_లెక్క
} UConverter_సమితి_Filter;

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
రకనిర్వచనం శూన్య (*UConverter_Get_Unicode_సమితి) (స్థిరరాశి UConverter *cnv,
                                         స్థిరరాశి USetAdder *sa,
                                         UConverter_Unicode_సమితి which,
                                         UError_సంకేతం *p_Error_సంకేతం);

UBool CONVERSION_U_SUCCESS (UError_సంకేతం err);

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
నిర్మాణం UConverterImpl {
    UConverterType type;

    UConverter_ఎక్కించు ఎక్కించు;
    UConverterUnload unload;

    UConverterOpen open;
    UConverterClose close;
    UConverterReset reset;

    UConverter_గమ్యం_Unicode గమ్యం_Unicode;
    UConverter_గమ్యం_Unicode గమ్యం_Unicode_With_Offsets;
    UConverterFromUnicode fromUnicode;
    UConverterFromUnicode fromUnicodeWithOffsets;
    UConverter_Get_తదుపరి_UChar get_తదుపరి_UChar;

    UConverterGetStarters getStarters;
    UConverterGetName getName;
    UConverterWriteSub writeSub;
    UConverter_సురక్షిత_Clone సురక్షిత_Clone;
    UConverter_Get_Unicode_సమితి get_Unicode_సమితి;

    UConverterConvert గమ్యం_UTF8;
    UConverterConvert fromUTF8;
};

బాహ్య స్థిరరాశి UConverter_Shared_దత్తాంశం
    _MBCSData, Latin1_దత్తాంశం,
    UTF8_దత్తాంశం, _UTF16BEData, _UTF16LEData, _UTF32BEData, _UTF32LEData,
    ISO2022_దత్తాంశం, 
    _LMBCSData1,_LMBCSData2, _LMBCSData3, _LMBCSData4, _LMBCSData5, _LMBCSData6,
    _LMBCSData8,_LMBCSData11,_LMBCSData16,_LMBCSData17,_LMBCSData18,_LMBCSData19,
    _HZData,_ISCIIData, _SCSUData, _ASCIIData,
    UTF7_దత్తాంశం, Bocu1_దత్తాంశం, UTF16_దత్తాంశం, UTF32_దత్తాంశం, CESU8_దత్తాంశం, _IMAPData, Compound_పాఠ్యం_దత్తాంశం;

U_CDECL_చివర

/** Always use fallbacks from codepage to Unicode */
#నిర్వచించు గమ్యం_U_USE_FALLBACK(useFallback) true
#నిర్వచించు UCNV_గమ్యం_U_USE_FALLBACK(cnv) true

/** Use fallbacks from Unicode to codepage when cnv->useFallback or for private-use code points */
#నిర్వచించు IS_వ్యక్తిగత_USE(c) ((uint32_t)((c)-0xe000)<0x1900 || (uint32_t)((c)-0xf0000)<0x20000)
#నిర్వచించు FROM_U_USE_FALLBACK(useFallback, c) ((useFallback) || IS_వ్యక్తిగత_USE(c))
#నిర్వచించు UCNV_FROM_U_USE_FALLBACK(cnv, c) FROM_U_USE_FALLBACK((cnv)->useFallback, c)

/**
 * Magic number for ucnv_getNextUChar(), returned by a
 * getNextUChar() implementation to indicate to use the converter's toUnicode()
 * instead of the native function.
 * @internal
 */
#నిర్వచించు UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U -9

U_CFUNC శూన్య
ucnv_get_Complete_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                   స్థిరరాశి USetAdder *sa,
                   UConverter_Unicode_సమితి which,
                   UError_సంకేతం *p_Error_సంకేతం);

U_CFUNC శూన్య
ucnv_get_Non_Surrogate_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                               స్థిరరాశి USetAdder *sa,
                               UConverter_Unicode_సమితి which,
                               UError_సంకేతం *p_Error_సంకేతం);

U_CFUNC శూన్య
ucnv_from_UWrite_బైట్లు(UConverter *cnv,
                     స్థిరరాశి అక్షరం *బైట్లు, int32_t నిడివి,
                     అక్షరం **లక్ష్యం, స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు,
                     int32_t **offsets,
                     int32_t ఆధారం_సూచీ,
                     UError_సంకేతం *p_Error_సంకేతం);
U_CFUNC శూన్య
ucnv_గమ్యం_UWrite_UChars(UConverter *cnv,
                    స్థిరరాశి UChar *uchars, int32_t నిడివి,
                    UChar **లక్ష్యం, స్థిరరాశి UChar *లక్ష్యం_హద్దు,
                    int32_t **offsets,
                    int32_t ఆధారం_సూచీ,
                    UError_సంకేతం *p_Error_సంకేతం);

U_CFUNC శూన్య
ucnv_గమ్యం_UWrite_సంకేతం_బిందువు(UConverter *cnv,
                       UChar32 c,
                       UChar **లక్ష్యం, స్థిరరాశి UChar *లక్ష్యం_హద్దు,
                       int32_t **offsets,
                       int32_t ఆధారం_సూచీ,
                       UError_సంకేతం *p_Error_సంకేతం);

#అయితే_ముగింపు

#అయితే_ముగింపు /* UCNV_CNV */
