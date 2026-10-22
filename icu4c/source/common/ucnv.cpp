// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1998-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  ucnv.c:
*  Implements APIs for the ICU's codeset conversion library;
*  mostly calls through internal functions;
*  created by Bertrand A. Damiba
*
* Modification History:
*
*   Date        Name        Description
*   04/04/99    helena      Fixed internal header inclusion.
*   05/09/00    helena      Added implementation to handle fallback mappings.
*   06/20/2000  helena      OS/400 port changes; mostly typecast.
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ <memory>

#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ucnv_err.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "utracimp.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "ucnv_imp.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#నిర్వచించు CHUNK_కొలత 1024

రకనిర్వచనం నిర్మాణం UAmbiguousConverter {
    స్థిరరాశి అక్షరం *name;
    స్థిరరాశి char16_t variant5c;
} UAmbiguousConverter;

స్థిర స్థిరరాశి UAmbiguousConverter ambiguousConverters[]={
    { "ibm-897_P100-1995", 0xa5 },
    { "ibm-942_P120-1999", 0xa5 },
    { "ibm-943_P130-1999", 0xa5 },
    { "ibm-946_P100-1995", 0xa5 },
    { "ibm-33722_P120-1999", 0xa5 },
    { "ibm-1041_P100-1995", 0xa5 },
    /*{ "ibm-54191_P100-2006", 0xa5 },*/
    /*{ "ibm-62383_P100-2007", 0xa5 },*/
    /*{ "ibm-891_P100-1995", 0x20a9 },*/
    { "ibm-944_P100-1995", 0x20a9 },
    { "ibm-949_P110-1999", 0x20a9 },
    { "ibm-1363_P110-1997", 0x20a9 },
    { "ISO_2022,locale=ko,version=0", 0x20a9 },
    { "ibm-1088_P100-1995", 0x20a9 }
};

/*Calls through createConverter */
U_CAPI UConverter* U_EXPORT2
ucnv_open (స్థిరరాశి అక్షరం *name,
                       UError_సంకేతం * err)
{
    UConverter *r;

    ఒకవేళ (err == శూన్యసూచిక || U_FAILURE (*err)) {
        తిరిగి శూన్యసూచిక;
    }

    r =  ucnv_createConverter(శూన్యసూచిక, name, err);
    తిరిగి r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (స్థిరరాశి అక్షరం *packageName, స్థిరరాశి అక్షరం *converterName, UError_సంకేతం * err)
{
    తిరిగి ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (స్థిరరాశి char16_t * name,
                         UError_సంకేతం * err)
{
    అక్షరం asciiName[UCNV_MAX_CONVERTER_NAME_నిడివి];

    ఒకవేళ (err == శూన్యసూచిక || U_FAILURE(*err))
        తిరిగి శూన్యసూచిక;
    ఒకవేళ (name == శూన్యసూచిక)
        తిరిగి ucnv_open (శూన్యసూచిక, err);
    ఒకవేళ (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_నిడివి)
    {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
స్థిర int32_t
ucnv_copyPlatformString(అక్షరం *platformString, UConverterPlatform pltfrm)
{
    ఎంపిక (pltfrm)
    {
    స్థితి UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        తిరిగి 4;
    స్థితి UCNV_UNKNOWN:
        ఆపు;
    }

    /* default to empty string */
    *platformString = 0;
    తిరిగి 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UError_సంకేతం * err)
{
    అక్షరం myName[UCNV_MAX_CONVERTER_NAME_నిడివి];
    int32_t myNameLen;

    ఒకవేళ (err == శూన్యసూచిక || U_FAILURE (*err))
        తిరిగి శూన్యసూచిక;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_గమ్యం_String(myName + myNameLen, codepage, 10);

    తిరిగి ucnv_createConverter(శూన్యసూచిక, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_సురక్షిత_Clone(స్థిరరాశి UConverter* cnv,
               శూన్య* దొంతర_బఫరు U_LIFETIME_BOUND,
               int32_t* p_బఫరు_కొలత,
               UError_సంకేతం* స్థితి_2)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t దొంతర_బఫరు_కొలత;
    int32_t బఫరు_కొలత_Needed;
    UError_సంకేతం cbErr;
    UConverter_గమ్యం_Unicode_వాదనలు గమ్యం_UArgs = {
        పరిమాణం(UConverter_గమ్యం_Unicode_వాదనలు),
            నిజం,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక
    };
    UConverter_From_Unicode_వాదనలు fromUArgs = {
        పరిమాణం(UConverter_From_Unicode_వాదనలు),
            నిజం,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక
    };

    UTRACE_నమోదు_OC(UTRACE_UCNV_CLONE);

    ఒకవేళ (స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        UTRACE_EXIT_స్థితి(స్థితి_2? *స్థితి_2: U_ILLEGAL_వాదం_ERROR);
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (cnv == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        UTRACE_EXIT_స్థితి(*స్థితి_2);
        తిరిగి శూన్యసూచిక;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, స్థితి_2), cnv, దొంతర_బఫరు);

    ఒకవేళ (cnv->shared_దత్తాంశం->impl->సురక్షిత_Clone != శూన్యసూచిక) {
        /* call the custom safeClone function for sizing */
        బఫరు_కొలత_Needed = 0;
        cnv->shared_దత్తాంశం->impl->సురక్షిత_Clone(cnv, శూన్యసూచిక, &బఫరు_కొలత_Needed, స్థితి_2);
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            UTRACE_EXIT_స్థితి(*స్థితి_2);
            తిరిగి శూన్యసూచిక;
        }
    }
    లేకపోతే
    {
        /* inherent sizing */
        బఫరు_కొలత_Needed = పరిమాణం(UConverter);
    }

    ఒకవేళ (p_బఫరు_కొలత == శూన్యసూచిక) {
        దొంతర_బఫరు_కొలత = 1;
        p_బఫరు_కొలత = &దొంతర_బఫరు_కొలత;
    } లేకపోతే {
        దొంతర_బఫరు_కొలత = *p_బఫరు_కొలత;
        ఒకవేళ (దొంతర_బఫరు_కొలత <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_బఫరు_కొలత = బఫరు_కొలత_Needed;
            UTRACE_EXIT_విలువ(బఫరు_కొలత_Needed);
            తిరిగి శూన్యసూచిక;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    ఒకవేళ (దొంతర_బఫరు) {
        uintptr_t p = reinterpret_cast<uintptr_t>(దొంతర_బఫరు);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t సూచిక_Adjustment = aligned_p - p;
        ఒకవేళ (బఫరు_కొలత_Needed + సూచిక_Adjustment <= దొంతర_బఫరు_కొలత) {
            దొంతర_బఫరు = reinterpret_cast<శూన్య *>(aligned_p);
            దొంతర_బఫరు_కొలత -= static_cast<int32_t>(సూచిక_Adjustment);
        } లేకపోతే {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            దొంతర_బఫరు_కొలత = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    ఒకవేళ (దొంతర_బఫరు_కొలత < బఫరు_కొలత_Needed || దొంతర_బఫరు == శూన్యసూచిక)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (బఫరు_కొలత_Needed);

        ఒకవేళ(localConverter == శూన్యసూచిక) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            UTRACE_EXIT_స్థితి(*స్థితి_2);
            తిరిగి శూన్యసూచిక;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        ఒకవేళ (p_బఫరు_కొలత != &దొంతర_బఫరు_కొలత) {
            *స్థితి_2 = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_బఫరు_కొలత = బఫరు_కొలత_Needed;
    } లేకపోతే {
        /* just use the stack buffer */
        localConverter = (UConverter*) దొంతర_బఫరు;
        allocatedConverter = శూన్యసూచిక;
    }

    uprv_memset(localConverter, 0, బఫరు_కొలత_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, పరిమాణం(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = అబద్ధం;

    /* copy the substitution string */
    ఒకవేళ (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } లేకపోతే {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_బఫరు_నిడివి * U_SIZEOF_UCHAR);
        ఒకవేళ (localConverter->subChars == శూన్యసూచిక) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_స్థితి(*స్థితి_2);
            తిరిగి శూన్యసూచిక;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_బఫరు_నిడివి * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    ఒకవేళ (cnv->shared_దత్తాంశం->impl->సురక్షిత_Clone != శూన్యసూచిక) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_దత్తాంశం->impl->సురక్షిత_Clone(cnv, localConverter, p_బఫరు_కొలత, స్థితి_2);
    }

    ఒకవేళ(localConverter==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        ఒకవేళ (allocatedConverter != శూన్యసూచిక && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_స్థితి(*స్థితి_2);
        తిరిగి శూన్యసూచిక;
    }

    /* increment refcount of shared data if needed */
    ఒకవేళ (cnv->shared_దత్తాంశం->isReferenceCounted) {
        ucnv_increment_Ref_లెక్క(cnv->shared_దత్తాంశం);
    }

    ఒకవేళ(localConverter == (UConverter*)దొంతర_బఫరు) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = నిజం;
    }

    /* allow callback functions to handle any memory allocation */
    గమ్యం_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->గమ్యం_UContext, &గమ్యం_UArgs, శూన్యసూచిక, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, శూన్యసూచిక, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_స్థితి(localConverter, *స్థితి_2);
    తిరిగి localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(స్థిరరాశి UConverter* cnv, UError_సంకేతం *స్థితి_2)
{
    తిరిగి ucnv_సురక్షిత_Clone(cnv, శూన్యసూచిక, శూన్యసూచిక, స్థితి_2);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI శూన్య  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;

    UTRACE_నమోదు_OC(UTRACE_UCNV_CLOSE);

    ఒకవేళ (converter == శూన్యసూచిక)
    {
        UTRACE_EXIT();
        తిరిగి;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_సంకేతం), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    ఒకవేళ (converter->fromCharErrorBehaviour != UCNV_గమ్యం_U_DEFAULT_CALLBACK) {
        UConverter_గమ్యం_Unicode_వాదనలు గమ్యం_UArgs = {
            పరిమాణం(UConverter_గమ్యం_Unicode_వాదనలు),
                నిజం,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక
        };

        గమ్యం_UArgs.converter = converter;
        error_సంకేతం = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->గమ్యం_UContext, &గమ్యం_UArgs, శూన్యసూచిక, 0, UCNV_CLOSE, &error_సంకేతం);
    }
    ఒకవేళ (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_వాదనలు fromUArgs = {
            పరిమాణం(UConverter_From_Unicode_వాదనలు),
                నిజం,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక
        };
        fromUArgs.converter = converter;
        error_సంకేతం = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, శూన్యసూచిక, 0, 0, UCNV_CLOSE, &error_సంకేతం);
    }

    ఒకవేళ (converter->shared_దత్తాంశం->impl->close != శూన్యసూచిక) {
        converter->shared_దత్తాంశం->impl->close(converter);
    }

    ఒకవేళ (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    ఒకవేళ (converter->shared_దత్తాంశం->isReferenceCounted) {
        ucnv_unload_Shared_దత్తాంశం_If_Ready(converter->shared_దత్తాంశం);
    }

    ఒకవేళ(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI స్థిరరాశి అక్షరం*   U_EXPORT2
ucnv_get_అందుబాటు_Name (int32_t n)
{
    ఒకవేళ (0 <= n && n <= 0xffff) {
        UError_సంకేతం err = U_ZERO_ERROR;
        స్థిరరాశి అక్షరం *name = ucnv_bld_get_అందుబాటు_Converter((uint16_t)n, &err);
        ఒకవేళ (U_SUCCESS(err)) {
            తిరిగి name;
        }
    }
    తిరిగి శూన్యసూచిక;
}

U_CAPI int32_t   U_EXPORT2
ucnv_లెక్క_అందుబాటు ()
{
    UError_సంకేతం err = U_ZERO_ERROR;
    తిరిగి ucnv_bld_లెక్క_అందుబాటు_Converters(&err);
}

U_CAPI శూన్య    U_EXPORT2
ucnv_getSubstChars (స్థిరరాశి UConverter * converter,
                    అక్షరం *mySubChar,
                    int8_t * len,
                    UError_సంకేతం * err)
{
    ఒకవేళ (U_FAILURE (*err))
        తిరిగి;

    ఒకవేళ (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        తిరిగి;
    }

    ఒకవేళ (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI శూన్య    U_EXPORT2
ucnv_సమితి_Subst_Chars (UConverter * converter,
                    స్థిరరాశి అక్షరం *mySubChar,
                    int8_t len,
                    UError_సంకేతం * err)
{
    ఒకవేళ (U_FAILURE (*err))
        తిరిగి;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    ఒకవేళ ((len > converter->shared_దత్తాంశం->static_దత్తాంశం->max_బైట్లు_చొప్పున_Char)
     || (len < converter->shared_దత్తాంశం->static_దత్తాంశం->min_బైట్లు_చొప్పున_Char))
    {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    
    uprv_memcpy (converter->subChars, mySubChar, len); /*copies the subchars */
    converter->subCharLen = len;  /*sets the new len */

    /*
    * There is currently (2001Feb) no separate API to set/get subChar1.
    * In order to always have subChar written after it is explicitly set,
    * we set subChar1 to 0.
    */
    converter->subChar1 = 0;
}

U_CAPI శూన్య U_EXPORT2
ucnv_సమితి_Subst_String(UConverter *cnv,
                    స్థిరరాశి char16_t *s,
                    int32_t length,
                    UError_సంకేతం *err) {
    alignas(UConverter) అక్షరం clone_బఫరు[U_CNV_SAFECLONE_BUFFERSIZE];
    అక్షరం chars[UCNV_ERROR_బఫరు_నిడివి];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_కొలత, length8;

    /* Let the following functions check all arguments. */
    clone_కొలత = పరిమాణం(clone_బఫరు);
    clone = ucnv_సురక్షిత_Clone(cnv, clone_బఫరు, &clone_కొలత, err);
    ucnv_సమితి_From_UCall_వెనుక(clone, UCNV_FROM_U_CALLBACK_STOP, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)పరిమాణం(chars), s, length, err);
    ucnv_close(clone);
    ఒకవేళ (U_FAILURE(*err)) {
        తిరిగి;
    }

    ఒకవేళ (cnv->shared_దత్తాంశం->impl->writeSub == శూన్యసూచిక
#ఒకవేళ !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_దత్తాంశం->static_దత్తాంశం->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#అయితే_ముగింపు
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } లేకపోతే {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        ఒకవేళ (length > UCNV_ERROR_బఫరు_నిడివి) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి;
        }
        subChars = (uint8_t *)s;
        ఒకవేళ (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    ఒకవేళ (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        ఒకవేళ (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_బఫరు_నిడివి * U_SIZEOF_UCHAR);
            ఒకవేళ (cnv->subChars == శూన్యసూచిక) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_బఫరు_నిడివి * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    ఒకవేళ (length8 == 0) {
        cnv->subCharLen = 0;
    } లేకపోతే {
        uprv_memcpy(cnv->subChars, subChars, length8);
        ఒకవేళ (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } లేకపోతే /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
స్థిర శూన్య _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    ఒకవేళ(converter == శూన్యసూచిక) {
        తిరిగి;
    }

    ఒకవేళ(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_సంకేతం error_సంకేతం;

        ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE && converter->fromCharErrorBehaviour != UCNV_గమ్యం_U_DEFAULT_CALLBACK) {
            UConverter_గమ్యం_Unicode_వాదనలు గమ్యం_UArgs = {
                పరిమాణం(UConverter_గమ్యం_Unicode_వాదనలు),
                నిజం,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక
            };
            గమ్యం_UArgs.converter = converter;
            error_సంకేతం = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->గమ్యం_UContext, &గమ్యం_UArgs, శూన్యసూచిక, 0, UCNV_RESET, &error_సంకేతం);
        }
        ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_వాదనలు fromUArgs = {
                పరిమాణం(UConverter_From_Unicode_వాదనలు),
                నిజం,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక,
                శూన్యసూచిక
            };
            fromUArgs.converter = converter;
            error_సంకేతం = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, శూన్యసూచిక, 0, 0, UCNV_RESET, &error_సంకేతం);
        }
    }

    /* now reset the converter itself */
    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        converter->గమ్యం_Unicode_స్థితి = converter->shared_దత్తాంశం->గమ్యం_Unicode_స్థితి;
        converter->బహుళకం = 0;
        converter->గమ్యం_ULength = 0;
        converter->invalid_Char_నిడివి = converter->UChar_Error_బఫరు_నిడివి = 0;
        converter->pre_గమ్యం_ULength = 0;
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
        converter->from_Unicode_స్థితి = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_నిడివి = converter->char_Error_బఫరు_నిడివి = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    ఒకవేళ (converter->shared_దత్తాంశం->impl->reset != శూన్యసూచిక) {
        /* call the custom reset function */
        converter->shared_దత్తాంశం->impl->reset(converter, choice);
    }
}

U_CAPI శూన్య  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, నిజం);
}

U_CAPI శూన్య  U_EXPORT2
ucnv_reset_గమ్యం_Unicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_గమ్యం_UNICODE, నిజం);
}

U_CAPI శూన్య  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, నిజం);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_కొలత (స్థిరరాశి UConverter * converter)
{
    తిరిగి converter->max_బైట్లు_చొప్పున_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_కొలత (స్థిరరాశి UConverter * converter)
{
    తిరిగి converter->shared_దత్తాంశం->static_దత్తాంశం->min_బైట్లు_చొప్పున_Char;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ucnv_getName(స్థిరరాశి UConverter* converter U_LIFETIME_BOUND, UError_సంకేతం* err)
{
    ఒకవేళ (U_FAILURE (*err))
        తిరిగి శూన్యసూచిక;
    ఒకవేళ(converter->shared_దత్తాంశం->impl->getName){
        స్థిరరాశి అక్షరం* తాత్కాలిక= converter->shared_దత్తాంశం->impl->getName(converter);
        ఒకవేళ(తాత్కాలిక)
            తిరిగి తాత్కాలిక;
    }
    తిరిగి converter->shared_దత్తాంశం->static_దత్తాంశం->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(స్థిరరాశి UConverter * converter,
              UError_సంకేతం * err)
{
    int32_t ccsid;
    ఒకవేళ (U_FAILURE (*err))
        తిరిగి -1;

    ccsid = converter->shared_దత్తాంశం->static_దత్తాంశం->codepage;
    ఒకవేళ (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        స్థిరరాశి అక్షరం *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        ఒకవేళ (U_SUCCESS(*err) && standardName) {
            స్థిరరాశి అక్షరం *ccsidStr = uprv_strchr(standardName, '-');
            ఒకవేళ (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    తిరిగి ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (స్థిరరాశి UConverter * converter,
                                      UError_సంకేతం * err)
{
    ఒకవేళ (U_FAILURE (*err))
        తిరిగి UCNV_UNKNOWN;

    తిరిగి (UConverterPlatform)converter->shared_దత్తాంశం->static_దత్తాంశం->platform;
}

U_CAPI శూన్య U_EXPORT2
    ucnv_get_గమ్యం_UCall_వెనుక (స్థిరరాశి UConverter * converter,
                         UConverter_గమ్యం_UCallback *action,
                         స్థిరరాశి శూన్య **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->గమ్యం_UContext;
}

U_CAPI శూన్య U_EXPORT2
    ucnv_get_From_UCall_వెనుక (స్థిరరాశి UConverter * converter,
                           UConverterFromUCallback *action,
                           స్థిరరాశి శూన్య **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI శూన్య    U_EXPORT2
ucnv_సమితి_గమ్యం_UCall_వెనుక (UConverter * converter,
                            UConverter_గమ్యం_UCallback newAction,
                            స్థిరరాశి శూన్య* newContext,
                            UConverter_గమ్యం_UCallback *oldAction,
                            స్థిరరాశి శూన్య** oldContext,
                            UError_సంకేతం * err)
{
    ఒకవేళ (U_FAILURE (*err))
        తిరిగి;
    ఒకవేళ (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    ఒకవేళ (oldContext) *oldContext = converter->గమ్యం_UContext;
    converter->గమ్యం_UContext = newContext;
}

U_CAPI శూన్య  U_EXPORT2
ucnv_సమితి_From_UCall_వెనుక (UConverter * converter,
                            UConverterFromUCallback newAction,
                            స్థిరరాశి శూన్య* newContext,
                            UConverterFromUCallback *oldAction,
                            స్థిరరాశి శూన్య** oldContext,
                            UError_సంకేతం * err)
{
    ఒకవేళ (U_FAILURE (*err))
        తిరిగి;
    ఒకవేళ (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    ఒకవేళ (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

స్థిర శూన్య
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t ఆధారం_సూచీ, int32_t error_ఆదానం_నిడివి) {
    int32_t *హద్దు;
    int32_t delta, స్థానభ్రంశం;

    ఒకవేళ(ఆధారం_సూచీ>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=ఆధారం_సూచీ-error_ఆదానం_నిడివి;
    } లేకపోతే {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    హద్దు=offsets+length;
    ఒకవేళ(delta==0) {
        /* most common case, nothing to do */
    } లేకపోతే ఒకవేళ(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        అయితే(offsets<హద్దు) {
            స్థానభ్రంశం=*offsets;
            ఒకవేళ(స్థానభ్రంశం>=0) {
                *offsets=స్థానభ్రంశం+delta;
            }
            ++offsets;
        }
    } లేకపోతే /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        అయితే(offsets<హద్దు) {
            *offsets++=-1;
        }
    }
}

/* ucnv_fromUnicode --------------------------------------------------------- */

/*
 * Implementation note for m:n conversions
 *
 * While collecting source units to find the longest match for m:n conversion,
 * some source units may need to be stored for a partial match.
 * When a second buffer does not yield a match on all of the previously stored
 * source units, then they must be "replayed", i.e., fed back into the converter.
 *
 * The code relies on the fact that replaying will not nest -
 * converting a replay buffer will not result in a replay.
 * This is because a replay is necessary only after the _continuation_ of a
 * partial match failed, but a replay buffer is converted as a whole.
 * It may result in some of its units being stored again for a partial match,
 * but there will not be a continuation _during_ the replay which could fail.
 *
 * It is conceivable that a callback function could call the converter
 * recursively in a way that causes another replay to be stored, but that
 * would be an error in the callback function.
 * Such violations will cause assertion failures in a debug build,
 * and wrong output, but they will not cause a crash.
 */

స్థిర శూన్య
_fromUnicodeWithCallback(UConverter_From_Unicode_వాదనలు *p_వాదనలు, UError_సంకేతం *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    స్థిరరాశి char16_t *s;
    అక్షరం *t;
    int32_t *offsets;
    int32_t ఆధారం_సూచీ;
    int32_t error_ఆదానం_నిడివి;
    UBool converter_Saw_చివర_యొక్క_ఆదానం, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    స్థిరరాశి char16_t *real_ఆధారం, *real_ఆధారం_హద్దు;
    int32_t real_ఆధారం_సూచీ;
    UBool realFlush;

    cnv=p_వాదనలు->converter;
    s=p_వాదనలు->ఆధారం;
    t=p_వాదనలు->లక్ష్యం;
    offsets=p_వాదనలు->offsets;

    /* get the converter implementation function */
    ఆధారం_సూచీ=0;
    ఒకవేళ(offsets==శూన్యసూచిక) {
        fromUnicode=cnv->shared_దత్తాంశం->impl->fromUnicode;
    } లేకపోతే {
        fromUnicode=cnv->shared_దత్తాంశం->impl->fromUnicodeWithOffsets;
        ఒకవేళ(fromUnicode==శూన్యసూచిక) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_దత్తాంశం->impl->fromUnicode;
            /* we will write -1 for each offset */
            ఆధారం_సూచీ=-1;
        }
    }

    ఒకవేళ(cnv->preFromULength>=0) {
        /* normal mode */
        real_ఆధారం=శూన్యసూచిక;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ఆధారం_హద్దు=శూన్యసూచిక;
        realFlush=అబద్ధం;
        real_ఆధారం_సూచీ=0;
    } లేకపోతే {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        ఒకవేళ (!p_వాదనలు->ఆధారం) {
            *err=U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
        real_ఆధారం=p_వాదనలు->ఆధారం;
        real_ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
        realFlush=p_వాదనలు->flush;
        real_ఆధారం_సూచీ=ఆధారం_సూచీ;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_వాదనలు->ఆధారం=replay;
        p_వాదనలు->ఆధారం_హద్దు=replay-cnv->preFromULength;
        p_వాదనలు->flush=అబద్ధం;
        ఆధారం_సూచీ=-1;

        cnv->preFromULength=0;
    }

    /*
     * loop for conversion and error handling
     *
     * loop {
     *   convert
     *   loop {
     *     update offsets
     *     handle end of input
     *     handle errors/call callback
     *   }
     * }
     */
    కొరకు(;;) {
        ఒకవేళ(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(p_వాదనలు, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_చివర_యొక్క_ఆదానం=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_వాదనలు->flush && p_వాదనలు->ఆధారం==p_వాదనలు->ఆధారం_హద్దు &&
                        cnv->fromUChar32==0);
        } లేకపోతే {
            /* handle error from ucnv_convertEx() */
            converter_Saw_చివర_యొక్క_ఆదానం=అబద్ధం;
        }

        /* no callback called yet for this iteration */
        calledCallback=అబద్ధం;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_ఆదానం_నిడివి=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        కొరకు(;;) {
            /* update offsets if we write any */
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                int32_t length = static_cast<int32_t>(p_వాదనలు->లక్ష్యం - t);
                ఒకవేళ(length>0) {
                    _updateOffsets(offsets, length, ఆధారం_సూచీ, error_ఆదానం_నిడివి);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_వాదనలు->offsets=offsets+=length;
                }

                ఒకవేళ(ఆధారం_సూచీ>=0) {
                    ఆధారం_సూచీ += static_cast<int32_t>(p_వాదనలు->ఆధారం - s);
                }
            }

            ఒకవేళ(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                ఒకవేళ(real_ఆధారం==శూన్యసూచిక) {
                    real_ఆధారం=p_వాదనలు->ఆధారం;
                    real_ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
                    realFlush=p_వాదనలు->flush;
                    real_ఆధారం_సూచీ=ఆధారం_సూచీ;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_వాదనలు->ఆధారం=replay;
                    p_వాదనలు->ఆధారం_హద్దు=replay-cnv->preFromULength;
                    p_వాదనలు->flush=అబద్ధం;
                    ఒకవేళ((ఆధారం_సూచీ+=cnv->preFromULength)<0) {
                        ఆధారం_సూచీ=-1;
                    }

                    cnv->preFromULength=0;
                } లేకపోతే {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ఆధారం==శూన్యసూచిక);
                    *err=U_అంతర్గత_కార్యక్రమం_ERROR;
                }
            }

            /* update pointers */
            s=p_వాదనలు->ఆధారం;
            t=p_వాదనలు->లక్ష్యం;

            ఒకవేళ(U_SUCCESS(*err)) {
                ఒకవేళ(s<p_వాదనలు->ఆధారం_హద్దు) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    ఆపు;
                } లేకపోతే ఒకవేళ(real_ఆధారం!=శూన్యసూచిక) {
                    /* switch back from replaying to the real source and continue */
                    p_వాదనలు->ఆధారం=real_ఆధారం;
                    p_వాదనలు->ఆధారం_హద్దు=real_ఆధారం_హద్దు;
                    p_వాదనలు->flush=realFlush;
                    ఆధారం_సూచీ=real_ఆధారం_సూచీ;

                    real_ఆధారం=శూన్యసూచిక;
                    ఆపు;
                } లేకపోతే ఒకవేళ(p_వాదనలు->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_దొరికింది;
                    calledCallback=అబద్ధం; /* new error condition */
                } లేకపోతే {
                    /* input consumed */
                    ఒకవేళ(p_వాదనలు->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        ఒకవేళ(!converter_Saw_చివర_యొక్క_ఆదానం) {
                            ఆపు;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, అబద్ధం);
                    }

                    /* done successfully */
                    తిరిగి;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_సంకేతం e;

                ఒకవేళ( calledCallback ||
                    (e=*err)==U_బఫరు_అధికప్రవాహం_ERROR ||
                    (e!=U_INVALID_CHAR_దొరికింది &&
                     e!=U_ILLEGAL_CHAR_దొరికింది &&
                     e!=U_TRUNCATED_CHAR_దొరికింది)
                ) {
                    /*
                     * the callback did not or cannot resolve the error:
                     * set output pointers and return
                     *
                     * the check for buffer overflow is redundant but it is
                     * a high-runner case and hopefully documents the intent
                     * well
                     *
                     * if we were replaying, then the replay buffer must be
                     * copied back into the UConverter
                     * and the real arguments must be restored
                     */
                    ఒకవేళ(real_ఆధారం!=శూన్యసూచిక) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_వాదనలు->ఆధారం_హద్దు - p_వాదనలు->ఆధారం);
                        ఒకవేళ(length>0) {
                            u_memcpy(cnv->preFromU, p_వాదనలు->ఆధారం, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_వాదనలు->ఆధారం=real_ఆధారం;
                        p_వాదనలు->ఆధారం_హద్దు=real_ఆధారం_హద్దు;
                        p_వాదనలు->flush=realFlush;
                    }

                    తిరిగి;
                }
            }

            /* callback handling */
            {
                UChar32 సంకేతం_బిందువు;

                /* get and write the code point */
                సంకేతం_బిందువు=cnv->fromUChar32;
                error_ఆదానం_నిడివి=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_బఫరు, error_ఆదానం_నిడివి, సంకేతం_బిందువు);
                cnv->invalid_UChar_నిడివి = static_cast<int8_t>(error_ఆదానం_నిడివి);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_వాదనలు,
                    cnv->invalid_UChar_బఫరు, error_ఆదానం_నిడివి, సంకేతం_బిందువు,
                    *err==U_INVALID_CHAR_దొరికింది ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
                    err);
            }

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=నిజం;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
స్థిర UBool
ucnv_output_అధికప్రవాహం_From_Unicode(UConverter *cnv,
                               అక్షరం **లక్ష్యం, స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు,
                               int32_t **pOffsets,
                               UError_సంకేతం *err) {
    int32_t *offsets;
    అక్షరం *అధికప్రవాహం, *t;
    int32_t i, length;

    t=*లక్ష్యం;
    ఒకవేళ(pOffsets!=శూన్యసూచిక) {
        offsets=*pOffsets;
    } లేకపోతే {
        offsets=శూన్యసూచిక;
    }

    అధికప్రవాహం = reinterpret_cast<అక్షరం*>(cnv->char_Error_బఫరు);
    length=cnv->char_Error_బఫరు_నిడివి;
    i=0;
    అయితే(i<length) {
        ఒకవేళ(t==లక్ష్యం_హద్దు) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            చేయి {
                అధికప్రవాహం[j++]=అధికప్రవాహం[i++];
            } అయితే(i<length);

            cnv->char_Error_బఫరు_నిడివి = static_cast<int8_t>(j);
            *లక్ష్యం=t;
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *pOffsets=offsets;
            }
            *err=U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి నిజం;
        }

        /* copy the overflow contents to the target */
        *t++=అధికప్రవాహం[i++];
        ఒకవేళ(offsets!=శూన్యసూచిక) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_బఫరు_నిడివి=0;
    *లక్ష్యం=t;
    ఒకవేళ(offsets!=శూన్యసూచిక) {
        *pOffsets=offsets;
    }
    తిరిగి అబద్ధం;
}

U_CAPI శూన్య U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 అక్షరం **లక్ష్యం, స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు,
                 స్థిరరాశి char16_t **ఆధారం, స్థిరరాశి char16_t *ఆధారం_హద్దు,
                 int32_t *offsets,
                 UBool flush,
                 UError_సంకేతం *err) {
    UConverter_From_Unicode_వాదనలు వాదనలు_2;
    స్థిరరాశి char16_t *s;
    అక్షరం *t;

    /* check parameters */
    ఒకవేళ(err==శూన్యసూచిక || U_FAILURE(*err)) {
        తిరిగి;
    }

    ఒకవేళ(cnv==శూన్యసూచిక || లక్ష్యం==శూన్యసూచిక || ఆధారం==శూన్యసూచిక) {
        *err=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    s=*ఆధారం;
    t=*లక్ష్యం;

    ఒకవేళ ((స్థిరరాశి శూన్య *)U_MAX_PTR(ఆధారం_హద్దు) == (స్థిరరాశి శూన్య *)ఆధారం_హద్దు) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        ఆధారం_హద్దు = (స్థిరరాశి char16_t *)(((స్థిరరాశి అక్షరం *)ఆధారం_హద్దు) - 1);
    }

    /*
     * All these conditions should never happen.
     *
     * 1) Make sure that the limits are >= to the address source or target
     *
     * 2) Make sure that the buffer sizes do not exceed the number range for
     * int32_t because some functions use the size (in units or bytes)
     * rather than comparing pointers, and because offsets are int32_t values.
     *
     * size_t is guaranteed to be unsigned and large enough for the job.
     *
     * Return with an error instead of adjusting the limits because we would
     * not be able to maintain the semantics that either the source must be
     * consumed or the target filled (unless an error occurs).
     * An adjustment would be targetLimit=t+0x7fffffff; for example.
     *
     * 3) Make sure that the user didn't incorrectly cast a char16_t * pointer
     * to a char * pointer and provide an incomplete char16_t code unit.
     */
    ఒకవేళ (ఆధారం_హద్దు<s || లక్ష్యం_హద్దు<t ||
        ((size_t)(ఆధారం_హద్దు-s)>(size_t)0x3fffffff && ఆధారం_హద్దు>s) ||
        ((size_t)(లక్ష్యం_హద్దు-t)>(size_t)0x7fffffff && లక్ష్యం_హద్దు>t) ||
        (((స్థిరరాశి అక్షరం *)ఆధారం_హద్దు-(స్థిరరాశి అక్షరం *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    
    /* output the target overflow buffer */
    ఒకవేళ( cnv->char_Error_బఫరు_నిడివి>0 &&
        ucnv_output_అధికప్రవాహం_From_Unicode(cnv, లక్ష్యం, లక్ష్యం_హద్దు, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        తిరిగి;
    }
    /* *target may have moved, therefore stop using t */

    ఒకవేళ(!flush && s==ఆధారం_హద్దు && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        తిరిగి;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    వాదనలు_2.converter=cnv;
    వాదనలు_2.flush=flush;
    వాదనలు_2.offsets=offsets;
    వాదనలు_2.ఆధారం=s;
    వాదనలు_2.ఆధారం_హద్దు=ఆధారం_హద్దు;
    వాదనలు_2.లక్ష్యం=*లక్ష్యం;
    వాదనలు_2.లక్ష్యం_హద్దు=లక్ష్యం_హద్దు;
    వాదనలు_2.size=పరిమాణం(వాదనలు_2);

    _fromUnicodeWithCallback(&వాదనలు_2, err);

    *ఆధారం=వాదనలు_2.ఆధారం;
    *లక్ష్యం=వాదనలు_2.లక్ష్యం;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

స్థిర శూన్య
గమ్యం_Unicode_With_Callback(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు, UError_సంకేతం *err) {
    UConverter_గమ్యం_Unicode గమ్యం_Unicode;
    UConverter *cnv;
    స్థిరరాశి అక్షరం *s;
    char16_t *t;
    int32_t *offsets;
    int32_t ఆధారం_సూచీ;
    int32_t error_ఆదానం_నిడివి;
    UBool converter_Saw_చివర_యొక్క_ఆదానం, calledCallback;

    /* variables for m:n conversion */
    అక్షరం replay[UCNV_EXT_MAX_బైట్లు];
    స్థిరరాశి అక్షరం *real_ఆధారం, *real_ఆధారం_హద్దు;
    int32_t real_ఆధారం_సూచీ;
    UBool realFlush;

    cnv=p_వాదనలు->converter;
    s=p_వాదనలు->ఆధారం;
    t=p_వాదనలు->లక్ష్యం;
    offsets=p_వాదనలు->offsets;

    /* get the converter implementation function */
    ఆధారం_సూచీ=0;
    ఒకవేళ(offsets==శూన్యసూచిక) {
        గమ్యం_Unicode=cnv->shared_దత్తాంశం->impl->గమ్యం_Unicode;
    } లేకపోతే {
        గమ్యం_Unicode=cnv->shared_దత్తాంశం->impl->గమ్యం_Unicode_With_Offsets;
        ఒకవేళ(గమ్యం_Unicode==శూన్యసూచిక) {
            /* there is no WithOffsets implementation */
            గమ్యం_Unicode=cnv->shared_దత్తాంశం->impl->గమ్యం_Unicode;
            /* we will write -1 for each offset */
            ఆధారం_సూచీ=-1;
        }
    }

    ఒకవేళ(cnv->pre_గమ్యం_ULength>=0) {
        /* normal mode */
        real_ఆధారం=శూన్యసూచిక;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ఆధారం_హద్దు=శూన్యసూచిక;
        realFlush=అబద్ధం;
        real_ఆధారం_సూచీ=0;
    } లేకపోతే {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        ఒకవేళ (!p_వాదనలు->ఆధారం) {
            *err=U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
        real_ఆధారం=p_వాదనలు->ఆధారం;
        real_ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
        realFlush=p_వాదనలు->flush;
        real_ఆధారం_సూచీ=ఆధారం_సూచీ;

        uprv_memcpy(replay, cnv->pre_గమ్యం_U, -cnv->pre_గమ్యం_ULength);
        p_వాదనలు->ఆధారం=replay;
        p_వాదనలు->ఆధారం_హద్దు=replay-cnv->pre_గమ్యం_ULength;
        p_వాదనలు->flush=అబద్ధం;
        ఆధారం_సూచీ=-1;

        cnv->pre_గమ్యం_ULength=0;
    }

    /*
     * loop for conversion and error handling
     *
     * loop {
     *   convert
     *   loop {
     *     update offsets
     *     handle end of input
     *     handle errors/call callback
     *   }
     * }
     */
    కొరకు(;;) {
        ఒకవేళ(U_SUCCESS(*err)) {
            /* convert */
            గమ్యం_Unicode(p_వాదనలు, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_చివర_యొక్క_ఆదానం=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_వాదనలు->flush && p_వాదనలు->ఆధారం==p_వాదనలు->ఆధారం_హద్దు &&
                        cnv->గమ్యం_ULength==0);
        } లేకపోతే {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_చివర_యొక్క_ఆదానం=అబద్ధం;
        }

        /* no callback called yet for this iteration */
        calledCallback=అబద్ధం;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_ఆదానం_నిడివి=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        కొరకు(;;) {
            /* update offsets if we write any */
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                int32_t length = static_cast<int32_t>(p_వాదనలు->లక్ష్యం - t);
                ఒకవేళ(length>0) {
                    _updateOffsets(offsets, length, ఆధారం_సూచీ, error_ఆదానం_నిడివి);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_వాదనలు->offsets=offsets+=length;
                }

                ఒకవేళ(ఆధారం_సూచీ>=0) {
                    ఆధారం_సూచీ += static_cast<int32_t>(p_వాదనలు->ఆధారం - s);
                }
            }

            ఒకవేళ(cnv->pre_గమ్యం_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                ఒకవేళ(real_ఆధారం==శూన్యసూచిక) {
                    real_ఆధారం=p_వాదనలు->ఆధారం;
                    real_ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
                    realFlush=p_వాదనలు->flush;
                    real_ఆధారం_సూచీ=ఆధారం_సూచీ;

                    uprv_memcpy(replay, cnv->pre_గమ్యం_U, -cnv->pre_గమ్యం_ULength);
                    p_వాదనలు->ఆధారం=replay;
                    p_వాదనలు->ఆధారం_హద్దు=replay-cnv->pre_గమ్యం_ULength;
                    p_వాదనలు->flush=అబద్ధం;
                    ఒకవేళ((ఆధారం_సూచీ+=cnv->pre_గమ్యం_ULength)<0) {
                        ఆధారం_సూచీ=-1;
                    }

                    cnv->pre_గమ్యం_ULength=0;
                } లేకపోతే {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ఆధారం==శూన్యసూచిక);
                    *err=U_అంతర్గత_కార్యక్రమం_ERROR;
                }
            }

            /* update pointers */
            s=p_వాదనలు->ఆధారం;
            t=p_వాదనలు->లక్ష్యం;

            ఒకవేళ(U_SUCCESS(*err)) {
                ఒకవేళ(s<p_వాదనలు->ఆధారం_హద్దు) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    ఆపు;
                } లేకపోతే ఒకవేళ(real_ఆధారం!=శూన్యసూచిక) {
                    /* switch back from replaying to the real source and continue */
                    p_వాదనలు->ఆధారం=real_ఆధారం;
                    p_వాదనలు->ఆధారం_హద్దు=real_ఆధారం_హద్దు;
                    p_వాదనలు->flush=realFlush;
                    ఆధారం_సూచీ=real_ఆధారం_సూచీ;

                    real_ఆధారం=శూన్యసూచిక;
                    ఆపు;
                } లేకపోతే ఒకవేళ(p_వాదనలు->flush && cnv->గమ్యం_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_దొరికింది;
                    calledCallback=అబద్ధం; /* new error condition */
                } లేకపోతే {
                    /* input consumed */
                    ఒకవేళ(p_వాదనలు->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        ఒకవేళ(!converter_Saw_చివర_యొక్క_ఆదానం) {
                            ఆపు;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_గమ్యం_UNICODE, అబద్ధం);
                    }

                    /* done successfully */
                    తిరిగి;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_సంకేతం e;

                ఒకవేళ( calledCallback ||
                    (e=*err)==U_బఫరు_అధికప్రవాహం_ERROR ||
                    (e!=U_INVALID_CHAR_దొరికింది &&
                     e!=U_ILLEGAL_CHAR_దొరికింది &&
                     e!=U_TRUNCATED_CHAR_దొరికింది &&
                     e!=U_ILLEGAL_ESCAPE_SEQUENCE &&
                     e!=U_UNSUPPORTED_ESCAPE_SEQUENCE)
                ) {
                    /*
                     * the callback did not or cannot resolve the error:
                     * set output pointers and return
                     *
                     * the check for buffer overflow is redundant but it is
                     * a high-runner case and hopefully documents the intent
                     * well
                     *
                     * if we were replaying, then the replay buffer must be
                     * copied back into the UConverter
                     * and the real arguments must be restored
                     */
                    ఒకవేళ(real_ఆధారం!=శూన్యసూచిక) {
                        int32_t length;

                        U_ASSERT(cnv->pre_గమ్యం_ULength==0);

                        length = static_cast<int32_t>(p_వాదనలు->ఆధారం_హద్దు - p_వాదనలు->ఆధారం);
                        ఒకవేళ(length>0) {
                            uprv_memcpy(cnv->pre_గమ్యం_U, p_వాదనలు->ఆధారం, length);
                            cnv->pre_గమ్యం_ULength = static_cast<int8_t>(-length);
                        }

                        p_వాదనలు->ఆధారం=real_ఆధారం;
                        p_వాదనలు->ఆధారం_హద్దు=real_ఆధారం_హద్దు;
                        p_వాదనలు->flush=realFlush;
                    }

                    తిరిగి;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_ఆదానం_నిడివి=cnv->invalid_Char_నిడివి=cnv->గమ్యం_ULength;
            ఒకవేళ(error_ఆదానం_నిడివి>0) {
                uprv_memcpy(cnv->invalid_Char_బఫరు, cnv->గమ్యం_UBytes, error_ఆదానం_నిడివి);
            }

            /* set the converter state to deal with the next character */
            cnv->గమ్యం_ULength=0;

            /* call the callback function */
            ఒకవేళ(cnv->గమ్యం_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_దొరికింది) {
                cnv->గమ్యం_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->గమ్యం_UContext, p_వాదనలు,
                cnv->invalid_Char_బఫరు, error_ఆదానం_నిడివి,
                cnv->గమ్యం_UCallback_Reason,
                err);
            cnv->గమ్యం_UCallback_Reason = UCNV_ILLEGAL; /* reset to default value */

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=నిజం;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
స్థిర UBool
ucnv_output_అధికప్రవాహం_గమ్యం_Unicode(UConverter *cnv,
                             char16_t **లక్ష్యం, స్థిరరాశి char16_t *లక్ష్యం_హద్దు,
                             int32_t **pOffsets,
                             UError_సంకేతం *err) {
    int32_t *offsets;
    char16_t *అధికప్రవాహం, *t;
    int32_t i, length;

    t=*లక్ష్యం;
    ఒకవేళ(pOffsets!=శూన్యసూచిక) {
        offsets=*pOffsets;
    } లేకపోతే {
        offsets=శూన్యసూచిక;
    }

    అధికప్రవాహం=cnv->UChar_Error_బఫరు;
    length=cnv->UChar_Error_బఫరు_నిడివి;
    i=0;
    అయితే(i<length) {
        ఒకవేళ(t==లక్ష్యం_హద్దు) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            చేయి {
                అధికప్రవాహం[j++]=అధికప్రవాహం[i++];
            } అయితే(i<length);

            cnv->UChar_Error_బఫరు_నిడివి = static_cast<int8_t>(j);
            *లక్ష్యం=t;
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *pOffsets=offsets;
            }
            *err=U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి నిజం;
        }

        /* copy the overflow contents to the target */
        *t++=అధికప్రవాహం[i++];
        ఒకవేళ(offsets!=శూన్యసూచిక) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_బఫరు_నిడివి=0;
    *లక్ష్యం=t;
    ఒకవేళ(offsets!=శూన్యసూచిక) {
        *pOffsets=offsets;
    }
    తిరిగి అబద్ధం;
}

U_CAPI శూన్య U_EXPORT2
ucnv_గమ్యం_Unicode(UConverter *cnv,
               char16_t **లక్ష్యం, స్థిరరాశి char16_t *లక్ష్యం_హద్దు,
               స్థిరరాశి అక్షరం **ఆధారం, స్థిరరాశి అక్షరం *ఆధారం_హద్దు,
               int32_t *offsets,
               UBool flush,
               UError_సంకేతం *err) {
    UConverter_గమ్యం_Unicode_వాదనలు వాదనలు_2;
    స్థిరరాశి అక్షరం *s;
    char16_t *t;

    /* check parameters */
    ఒకవేళ(err==శూన్యసూచిక || U_FAILURE(*err)) {
        తిరిగి;
    }

    ఒకవేళ(cnv==శూన్యసూచిక || లక్ష్యం==శూన్యసూచిక || ఆధారం==శూన్యసూచిక) {
        *err=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    s=*ఆధారం;
    t=*లక్ష్యం;

    ఒకవేళ ((స్థిరరాశి శూన్య *)U_MAX_PTR(లక్ష్యం_హద్దు) == (స్థిరరాశి శూన్య *)లక్ష్యం_హద్దు) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        లక్ష్యం_హద్దు = (స్థిరరాశి char16_t *)(((స్థిరరాశి అక్షరం *)లక్ష్యం_హద్దు) - 1);
    }

    /*
     * All these conditions should never happen.
     *
     * 1) Make sure that the limits are >= to the address source or target
     *
     * 2) Make sure that the buffer sizes do not exceed the number range for
     * int32_t because some functions use the size (in units or bytes)
     * rather than comparing pointers, and because offsets are int32_t values.
     *
     * size_t is guaranteed to be unsigned and large enough for the job.
     *
     * Return with an error instead of adjusting the limits because we would
     * not be able to maintain the semantics that either the source must be
     * consumed or the target filled (unless an error occurs).
     * An adjustment would be sourceLimit=t+0x7fffffff; for example.
     *
     * 3) Make sure that the user didn't incorrectly cast a char16_t * pointer
     * to a char * pointer and provide an incomplete char16_t code unit.
     */
    ఒకవేళ (ఆధారం_హద్దు<s || లక్ష్యం_హద్దు<t ||
        ((size_t)(ఆధారం_హద్దు-s)>(size_t)0x7fffffff && ఆధారం_హద్దు>s) ||
        ((size_t)(లక్ష్యం_హద్దు-t)>(size_t)0x3fffffff && లక్ష్యం_హద్దు>t) ||
        (((స్థిరరాశి అక్షరం *)లక్ష్యం_హద్దు-(స్థిరరాశి అక్షరం *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    
    /* output the target overflow buffer */
    ఒకవేళ( cnv->UChar_Error_బఫరు_నిడివి>0 &&
        ucnv_output_అధికప్రవాహం_గమ్యం_Unicode(cnv, లక్ష్యం, లక్ష్యం_హద్దు, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        తిరిగి;
    }
    /* *target may have moved, therefore stop using t */

    ఒకవేళ(!flush && s==ఆధారం_హద్దు && cnv->pre_గమ్యం_ULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        తిరిగి;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    వాదనలు_2.converter=cnv;
    వాదనలు_2.flush=flush;
    వాదనలు_2.offsets=offsets;
    వాదనలు_2.ఆధారం=s;
    వాదనలు_2.ఆధారం_హద్దు=ఆధారం_హద్దు;
    వాదనలు_2.లక్ష్యం=*లక్ష్యం;
    వాదనలు_2.లక్ష్యం_హద్దు=లక్ష్యం_హద్దు;
    వాదనలు_2.size=పరిమాణం(వాదనలు_2);

    గమ్యం_Unicode_With_Callback(&వాదనలు_2, err);

    *ఆధారం=వాదనలు_2.ఆధారం;
    *లక్ష్యం=వాదనలు_2.లక్ష్యం;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                అక్షరం *dest, int32_t dest_సామర్థ్యం,
                స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి char16_t *src_హద్దు;
    అక్షరం *originalDest, *dest_హద్దు;
    int32_t dest_నిడివి;

    /* check arguments */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ( cnv==శూన్యసూచిక ||
        dest_సామర్థ్యం<0 || (dest_సామర్థ్యం>0 && dest==శూన్యసూచిక) ||
        src_నిడివి<-1 || (src_నిడివి!=0 && src==శూన్యసూచిక)
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    ఒకవేళ(src_నిడివి==-1) {
        src_నిడివి=u_strlen(src);
    }
    ఒకవేళ(src_నిడివి>0) {
        src_హద్దు=src+src_నిడివి;
        dest_సామర్థ్యం=pin_సామర్థ్యం(dest, dest_సామర్థ్యం);
        dest_హద్దు=dest+dest_సామర్థ్యం;

        /* perform the conversion */
        UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_హద్దు, &src, src_హద్దు, శూన్యసూచిక, నిజం, &బఫరు_స్థితి);
        dest_నిడివి=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        ఒకవేళ(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR) {
            అక్షరం బఫరు[1024];

            dest_హద్దు=బఫరు+పరిమాణం(బఫరు);
            చేయి {
                dest=బఫరు;
                బఫరు_స్థితి=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_హద్దు, &src, src_హద్దు, శూన్యసూచిక, నిజం, &బఫరు_స్థితి);
                dest_నిడివి+=(int32_t)(dest-బఫరు);
            } అయితే(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR);
        }
        ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
            *p_Error_సంకేతం = బఫరు_స్థితి;
        }
    } లేకపోతే {
        dest_నిడివి=0;
    }

    తిరిగి u_terminateChars(originalDest, dest_సామర్థ్యం, dest_నిడివి, p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
ucnv_గమ్యం_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_సామర్థ్యం,
              స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
              UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి అక్షరం *src_హద్దు;
    char16_t *originalDest, *dest_హద్దు;
    int32_t dest_నిడివి;

    /* check arguments */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ( cnv==శూన్యసూచిక ||
        dest_సామర్థ్యం<0 || (dest_సామర్థ్యం>0 && dest==శూన్యసూచిక) ||
        src_నిడివి<-1 || (src_నిడివి!=0 && src==శూన్యసూచిక))
    {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* initialize */
    ucnv_reset_గమ్యం_Unicode(cnv);
    originalDest=dest;
    ఒకవేళ(src_నిడివి==-1) {
        src_నిడివి=(int32_t)uprv_strlen(src);
    }
    ఒకవేళ(src_నిడివి>0) {
        src_హద్దు=src+src_నిడివి;
        dest_సామర్థ్యం=pin_సామర్థ్యం(dest, dest_సామర్థ్యం);
        dest_హద్దు=dest+dest_సామర్థ్యం;

        /* perform the conversion */
        UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
        ucnv_గమ్యం_Unicode(cnv, &dest, dest_హద్దు, &src, src_హద్దు, శూన్యసూచిక, నిజం, &బఫరు_స్థితి);
        dest_నిడివి=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        ఒకవేళ(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR)
        {
            char16_t బఫరు[1024];

            dest_హద్దు=బఫరు+UPRV_LENGTHOF(బఫరు);
            చేయి {
                dest=బఫరు;
                బఫరు_స్థితి=U_ZERO_ERROR;
                ucnv_గమ్యం_Unicode(cnv, &dest, dest_హద్దు, &src, src_హద్దు, శూన్యసూచిక, నిజం, &బఫరు_స్థితి);
                dest_నిడివి+=(int32_t)(dest-బఫరు);
            }
            అయితే(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR);
        }
        ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
            *p_Error_సంకేతం = బఫరు_స్థితి;
        }
    } లేకపోతే {
        dest_నిడివి=0;
    }

    తిరిగి u_terminateUChars(originalDest, dest_సామర్థ్యం, dest_నిడివి, p_Error_సంకేతం);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_తదుపరి_UChar(UConverter *cnv,
                  స్థిరరాశి అక్షరం **ఆధారం, స్థిరరాశి అక్షరం *ఆధారం_హద్దు,
                  UError_సంకేతం *err) {
    UConverter_గమ్యం_Unicode_వాదనలు వాదనలు_2;
    char16_t బఫరు[U16_MAX_నిడివి];
    స్థిరరాశి అక్షరం *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    ఒకవేళ(err==శూన్యసూచిక || U_FAILURE(*err)) {
        తిరిగి 0xffff;
    }

    ఒకవేళ(cnv==శూన్యసూచిక || ఆధారం==శూన్యసూచిక) {
        *err=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0xffff;
    }

    s=*ఆధారం;
    ఒకవేళ(ఆధారం_హద్దు<s) {
        *err=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0xffff;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t because some functions use the size (in units or bytes)
     * rather than comparing pointers, and because offsets are int32_t values.
     *
     * size_t is guaranteed to be unsigned and large enough for the job.
     *
     * Return with an error instead of adjusting the limits because we would
     * not be able to maintain the semantics that either the source must be
     * consumed or the target filled (unless an error occurs).
     * An adjustment would be sourceLimit=t+0x7fffffff; for example.
     */
    ఒకవేళ(((size_t)(ఆధారం_హద్దు-s)>(size_t)0x7fffffff && ఆధారం_హద్దు>s)) {
        *err=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    ఒకవేళ(cnv->UChar_Error_బఫరు_నిడివి>0) {
        char16_t *అధికప్రవాహం;

        అధికప్రవాహం=cnv->UChar_Error_బఫరు;
        i=0;
        length=cnv->UChar_Error_బఫరు_నిడివి;
        U16_తదుపరి(అధికప్రవాహం, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        ఒకవేళ((cnv->UChar_Error_బఫరు_నిడివి=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_బఫరు, cnv->UChar_Error_బఫరు+i,
                         cnv->UChar_Error_బఫరు_నిడివి*U_SIZEOF_UCHAR);
        }

        ఒకవేళ(!U16_IS_LEAD(c) || i<length) {
            తిరిగి c;
        }
        /*
         * Continue if the overflow buffer contained only a lead surrogate,
         * in case the converter outputs single surrogates from complete
         * input sequences.
         */
    }

    /*
     * flush==true is implied for ucnv_getNextUChar()
     *
     * do not simply return even if s==sourceLimit because the converter may
     * not have seen flush==true before
     */

    /* prepare the converter arguments */
    వాదనలు_2.converter=cnv;
    వాదనలు_2.flush=నిజం;
    వాదనలు_2.offsets=శూన్యసూచిక;
    వాదనలు_2.ఆధారం=s;
    వాదనలు_2.ఆధారం_హద్దు=ఆధారం_హద్దు;
    వాదనలు_2.లక్ష్యం=బఫరు;
    వాదనలు_2.లక్ష్యం_హద్దు=బఫరు+1;
    వాదనలు_2.size=పరిమాణం(వాదనలు_2);

    ఒకవేళ(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        ఒకవేళ(cnv->గమ్యం_ULength==0 && cnv->shared_దత్తాంశం->impl->get_తదుపరి_UChar!=శూన్యసూచిక) {
            c=cnv->shared_దత్తాంశం->impl->get_తదుపరి_UChar(&వాదనలు_2, err);
            *ఆధారం=s=వాదనలు_2.ఆధారం;
            ఒకవేళ(*err==U_సూచీ_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_గమ్యం_UNICODE, అబద్ధం);
                తిరిగి 0xffff; /* no output */
            } లేకపోతే ఒకవేళ(U_SUCCESS(*err) && c>=0) {
                తిరిగి c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        గమ్యం_Unicode_With_Callback(&వాదనలు_2, err);

        ఒకవేళ(*err==U_బఫరు_అధికప్రవాహం_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(వాదనలు_2.లక్ష్యం-బఫరు);
    } లేకపోతే {
        /* write the lead surrogate from the overflow buffer */
        బఫరు[0]=(char16_t)c;
        వాదనలు_2.లక్ష్యం=బఫరు+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    ఒకవేళ(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } లేకపోతే ఒకవేళ(length==0) {
        /* no input or only state changes */
        *err=U_సూచీ_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } లేకపోతే {
        c=బఫరు[0];
        i=1;
        ఒకవేళ(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } లేకపోతే {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            ఒకవేళ(cnv->UChar_Error_బఫరు_నిడివి>0) {
                /* got overflow output from the conversion */
                ఒకవేళ(U16_IS_TRAIL(c2=cnv->UChar_Error_బఫరు[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    ఒకవేళ((--cnv->UChar_Error_బఫరు_నిడివి)>0) {
                        uprv_memmove(cnv->UChar_Error_బఫరు, cnv->UChar_Error_బఫరు+1,
                                     cnv->UChar_Error_బఫరు_నిడివి*U_SIZEOF_UCHAR);
                    }
                } లేకపోతే {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } లేకపోతే ఒకవేళ(వాదనలు_2.ఆధారం<ఆధారం_హద్దు) {
                /* convert once more, to buffer[1] */
                వాదనలు_2.లక్ష్యం_హద్దు=బఫరు+2;
                గమ్యం_Unicode_With_Callback(&వాదనలు_2, err);
                ఒకవేళ(*err==U_బఫరు_అధికప్రవాహం_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(వాదనలు_2.లక్ష్యం-బఫరు);
                ఒకవేళ(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=బఫరు[1])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    i=2;
                }
            }
        }
    }

    /*
     * move leftover output from buffer[i..length[
     * into the beginning of the overflow buffer
     */
    ఒకవేళ(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        ఒకవేళ((length=cnv->UChar_Error_బఫరు_నిడివి)>0) {
            uprv_memmove(cnv->UChar_Error_బఫరు+delta, cnv->UChar_Error_బఫరు,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_బఫరు_నిడివి=(int8_t)(length+delta);

        cnv->UChar_Error_బఫరు[0]=బఫరు[i++];
        ఒకవేళ(delta>1) {
            cnv->UChar_Error_బఫరు[1]=బఫరు[i];
        }
    }

    *ఆధారం=వాదనలు_2.ఆధారం;
    తిరిగి c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI శూన్య U_EXPORT2
ucnv_convertEx(UConverter *లక్ష్యం_Cnv, UConverter *ఆధారం_Cnv,
               అక్షరం **లక్ష్యం, స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు,
               స్థిరరాశి అక్షరం **ఆధారం, స్థిరరాశి అక్షరం *ఆధారం_హద్దు,
               char16_t *ఇరుసు_ప్రారంభం, char16_t **ఇరుసు_ఆధారం,
               char16_t **ఇరుసు_లక్ష్యం, స్థిరరాశి char16_t *ఇరుసు_హద్దు,
               UBool reset, UBool flush,
               UError_సంకేతం *p_Error_సంకేతం) {
    char16_t ఇరుసు_బఫరు[CHUNK_కొలత];
    స్థిరరాశి char16_t *my_ఇరుసు_ఆధారం;
    char16_t *my_ఇరుసు_లక్ష్యం;
    స్థిరరాశి అక్షరం *s;
    అక్షరం *t;

    UConverter_గమ్యం_Unicode_వాదనలు గమ్యం_UArgs;
    UConverter_From_Unicode_వాదనలు fromUArgs;
    UConverterConvert convert;

    /* error checking */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }

    ఒకవేళ( లక్ష్యం_Cnv==శూన్యసూచిక || ఆధారం_Cnv==శూన్యసూచిక ||
        ఆధారం==శూన్యసూచిక || *ఆధారం==శూన్యసూచిక ||
        లక్ష్యం==శూన్యసూచిక || *లక్ష్యం==శూన్యసూచిక || లక్ష్యం_హద్దు==శూన్యసూచిక
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    s=*ఆధారం;
    t=*లక్ష్యం;
    ఒకవేళ((ఆధారం_హద్దు!=శూన్యసూచిక && ఆధారం_హద్దు<s) || లక్ష్యం_హద్దు<t) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    ఒకవేళ(
        (ఆధారం_హద్దు!=శూన్యసూచిక && ((size_t)(ఆధారం_హద్దు-s)>(size_t)0x7fffffff && ఆధారం_హద్దు>s)) ||
        ((size_t)(లక్ష్యం_హద్దు-t)>(size_t)0x7fffffff && లక్ష్యం_హద్దు>t)
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    
    ఒకవేళ(ఇరుసు_ప్రారంభం==శూన్యసూచిక) {
        ఒకవేళ(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }

        /* use the stack pivot buffer */
        my_ఇరుసు_ఆధారం=my_ఇరుసు_లక్ష్యం=ఇరుసు_ప్రారంభం=ఇరుసు_బఫరు;
        ఇరుసు_ఆధారం=(char16_t **)&my_ఇరుసు_ఆధారం;
        ఇరుసు_లక్ష్యం=&my_ఇరుసు_లక్ష్యం;
        ఇరుసు_హద్దు=ఇరుసు_బఫరు+CHUNK_కొలత;
    } లేకపోతే ఒకవేళ(  ఇరుసు_ప్రారంభం>=ఇరుసు_హద్దు ||
                ఇరుసు_ఆధారం==శూన్యసూచిక || *ఇరుసు_ఆధారం==శూన్యసూచిక ||
                ఇరుసు_లక్ష్యం==శూన్యసూచిక || *ఇరుసు_లక్ష్యం==శూన్యసూచిక ||
                ఇరుసు_హద్దు==శూన్యసూచిక
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ఒకవేళ(ఆధారం_హద్దు==శూన్యసూచిక) {
        /* get limit of single-byte-NUL-terminated source string */
        ఆధారం_హద్దు=uprv_strchr(*ఆధారం, 0);
    }

    ఒకవేళ(reset) {
        ucnv_reset_గమ్యం_Unicode(ఆధారం_Cnv);
        ucnv_resetFromUnicode(లక్ష్యం_Cnv);
        *ఇరుసు_ఆధారం=*ఇరుసు_లక్ష్యం=ఇరుసు_ప్రారంభం;
    } లేకపోతే ఒకవేళ(లక్ష్యం_Cnv->char_Error_బఫరు_నిడివి>0) {
        /* output the targetCnv overflow buffer */
        ఒకవేళ(ucnv_output_అధికప్రవాహం_From_Unicode(లక్ష్యం_Cnv, లక్ష్యం, లక్ష్యం_హద్దు, శూన్యసూచిక, p_Error_సంకేతం)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            తిరిగి;
        }
        /* *target has moved, therefore stop using t */

        ఒకవేళ( !flush &&
            లక్ష్యం_Cnv->preFromULength>=0 && *ఇరుసు_ఆధారం==*ఇరుసు_లక్ష్యం &&
            ఆధారం_Cnv->UChar_Error_బఫరు_నిడివి==0 && ఆధారం_Cnv->pre_గమ్యం_ULength>=0 && s==ఆధారం_హద్దు
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            తిరిగి;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    ఒకవేళ( ఆధారం_Cnv->shared_దత్తాంశం->static_దత్తాంశం->conversionType==UCNV_UTF8 &&
        లక్ష్యం_Cnv->shared_దత్తాంశం->impl->fromUTF8!=శూన్యసూచిక
    ) {
        convert=లక్ష్యం_Cnv->shared_దత్తాంశం->impl->fromUTF8;
    } లేకపోతే ఒకవేళ( లక్ష్యం_Cnv->shared_దత్తాంశం->static_దత్తాంశం->conversionType==UCNV_UTF8 &&
               ఆధారం_Cnv->shared_దత్తాంశం->impl->గమ్యం_UTF8!=శూన్యసూచిక
    ) {
        convert=ఆధారం_Cnv->shared_దత్తాంశం->impl->గమ్యం_UTF8;
    } లేకపోతే {
        convert=శూన్యసూచిక;
    }

    /*
     * If direct-UTF-8 conversion is available, then we use a smaller
     * pivot buffer for error handling and partial matches
     * so that we quickly return to direct conversion.
     *
     * 32 is large enough for UCNV_EXT_MAX_UCHARS and UCNV_ERROR_BUFFER_LENGTH.
     *
     * We could reduce the pivot buffer size further, at the cost of
     * buffer overflows from callbacks.
     * The pivot buffer should not be smaller than the maximum number of
     * fromUnicode extension table input UChars
     * (for m:n conversion, see
     * targetCnv->sharedData->mbcs.extIndexes[UCNV_EXT_COUNT_UCHARS])
     * or 2 for surrogate pairs.
     *
     * Too small a buffer can cause thrashing between pivoting and direct
     * conversion, with function call overhead outweighing the benefits
     * of direct conversion.
     */
    ఒకవేళ(convert!=శూన్యసూచిక && (ఇరుసు_హద్దు-ఇరుసు_ప్రారంభం)>32) {
        ఇరుసు_హద్దు=ఇరుసు_ప్రారంభం+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=లక్ష్యం_Cnv;
    fromUArgs.flush=అబద్ధం;
    fromUArgs.offsets=శూన్యసూచిక;
    fromUArgs.లక్ష్యం=*లక్ష్యం;
    fromUArgs.లక్ష్యం_హద్దు=లక్ష్యం_హద్దు;
    fromUArgs.size=పరిమాణం(fromUArgs);

    గమ్యం_UArgs.converter=ఆధారం_Cnv;
    గమ్యం_UArgs.flush=flush;
    గమ్యం_UArgs.offsets=శూన్యసూచిక;
    గమ్యం_UArgs.ఆధారం=s;
    గమ్యం_UArgs.ఆధారం_హద్దు=ఆధారం_హద్దు;
    గమ్యం_UArgs.లక్ష్యం_హద్దు=ఇరుసు_హద్దు;
    గమ్యం_UArgs.size=పరిమాణం(గమ్యం_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=శూన్యసూచిక;

    /*
     * conversion loop
     *
     * The sequence of steps in the loop may appear backward,
     * but the principle is simple:
     * In the chain of
     *   source - sourceCnv overflow - pivot - targetCnv overflow - target
     * empty out later buffers before refilling them from earlier ones.
     *
     * The targetCnv overflow buffer is flushed out only once before the loop.
     */
    కొరకు(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        ఒకవేళ( *ఇరుసు_ఆధారం<*ఇరుసు_లక్ష్యం ||
            U_FAILURE(*p_Error_సంకేతం) ||
            లక్ష్యం_Cnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.ఆధారం=*ఇరుసు_ఆధారం;
            fromUArgs.ఆధారం_హద్దు=*ఇరుసు_లక్ష్యం;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_సంకేతం);
            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                /* target overflow, or conversion error */
                *ఇరుసు_ఆధారం=(char16_t *)fromUArgs.ఆధారం;
                ఆపు;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *ఇరుసు_ఆధారం=*ఇరుసు_లక్ష్యం=ఇరుసు_ప్రారంభం;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        ఒకవేళ(ఆధారం_Cnv->UChar_Error_బఫరు_నిడివి>0) {
            ఒకవేళ(ucnv_output_అధికప్రవాహం_గమ్యం_Unicode(ఆధారం_Cnv, ఇరుసు_లక్ష్యం, ఇరుసు_హద్దు, శూన్యసూచిక, p_Error_సంకేతం)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_సంకేతం=U_ZERO_ERROR;
            }
            కొనసాగు;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        ఒకవేళ( గమ్యం_UArgs.ఆధారం==ఆధారం_హద్దు &&
            ఆధారం_Cnv->pre_గమ్యం_ULength>=0 && ఆధారం_Cnv->గమ్యం_ULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            ఆపు;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        ఒకవేళ(convert!=శూన్యసూచిక && లక్ష్యం_Cnv->preFromUFirstCP<0 && ఆధారం_Cnv->pre_గమ్యం_ULength==0) {
            ఒకవేళ(*p_Error_సంకేతం==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_సంకేతం=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &గమ్యం_UArgs, p_Error_సంకేతం);
            ఒకవేళ(*p_Error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
                ఆపు;
            } లేకపోతే ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                ఒకవేళ(ఆధారం_Cnv->గమ్యం_ULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } లేకపోతే {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *ఇరుసు_ఆధారం=*ఇరుసు_లక్ష్యం=ఇరుసు_ప్రారంభం+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ(*p_Error_సంకేతం==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_సంకేతం=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } లేకపోతే ఒకవేళ(flush && ఆధారం_Cnv->గమ్యం_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_సంకేతం=U_TRUNCATED_CHAR_దొరికింది;
            } లేకపోతే {
                /* input consumed */
                ఒకవేళ(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(ఆధారం_Cnv, UCNV_RESET_గమ్యం_UNICODE, అబద్ధం);
                    _reset(లక్ష్యం_Cnv, UCNV_RESET_FROM_UNICODE, అబద్ధం);
                }

                /* done successfully */
                ఆపు;
            }
        }
        
        /*
         * toUnicode(source -> pivot);
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling, continuing partial matches
         * and flushing the replay buffer.
         *
         * The pivot buffer is empty and reset.
         */
        గమ్యం_UArgs.లక్ష్యం=ఇరుసు_ప్రారంభం; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        గమ్యం_Unicode_With_Callback(&గమ్యం_UArgs, p_Error_సంకేతం);
        *ఇరుసు_లక్ష్యం=గమ్యం_UArgs.లక్ష్యం;
        ఒకవేళ(*p_Error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_సంకేతం=U_ZERO_ERROR;
        } లేకపోతే ఒకవేళ(U_FAILURE(*p_Error_సంకేతం) || (!flush && *ఇరుసు_లక్ష్యం==ఇరుసు_ప్రారంభం)) {
            /* conversion error, or there was nothing left to convert */
            ఆపు;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        ఒకవేళ( flush && గమ్యం_UArgs.ఆధారం==ఆధారం_హద్దు &&
            ఆధారం_Cnv->pre_గమ్యం_ULength>=0 &&
            ఆధారం_Cnv->UChar_Error_బఫరు_నిడివి==0
        ) {
            fromUArgs.flush=నిజం;
        }
    }

    /*
     * The conversion loop is exited when one of the following is true:
     * - the entire source text has been converted successfully to the target buffer
     * - a target buffer overflow occurred
     * - a conversion error occurred
     */

    *ఆధారం=గమ్యం_UArgs.ఆధారం;
    *లక్ష్యం=fromUArgs.లక్ష్యం;

    /* terminate the target buffer if possible */
    ఒకవేళ(flush && U_SUCCESS(*p_Error_సంకేతం)) {
        ఒకవేళ(*లక్ష్యం!=లక్ష్యం_హద్దు) {
            **లక్ష్యం=0;
            ఒకవేళ(*p_Error_సంకేతం==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_సంకేతం=U_ZERO_ERROR;
            }
        } లేకపోతే {
            *p_Error_సంకేతం=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
స్థిర int32_t
ucnv_అంతర్గత_Convert(UConverter *బహిర్గతం_Converter, UConverter *లోపల_Converter,
                     అక్షరం *లక్ష్యం, int32_t లక్ష్యం_సామర్థ్యం,
                     స్థిరరాశి అక్షరం *ఆధారం, int32_t ఆధారం_నిడివి,
                     UError_సంకేతం *p_Error_సంకేతం) {
    char16_t ఇరుసు_బఫరు[CHUNK_కొలత];
    char16_t *ఇరుసు, *pivot2;

    అక్షరం *my_లక్ష్యం;
    స్థిరరాశి అక్షరం *ఆధారం_హద్దు;
    స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు;
    int32_t లక్ష్యం_నిడివి=0;

    /* set up */
    ఒకవేళ(ఆధారం_నిడివి<0) {
        ఆధారం_హద్దు=uprv_strchr(ఆధారం, 0);
    } లేకపోతే {
        ఆధారం_హద్దు=ఆధారం+ఆధారం_నిడివి;
    }

    /* if there is no input data, we're done */
    ఒకవేళ(ఆధారం==ఆధారం_హద్దు) {
        తిరిగి u_terminateChars(లక్ష్యం, లక్ష్యం_సామర్థ్యం, 0, p_Error_సంకేతం);
    }

    ఇరుసు=pivot2=ఇరుసు_బఫరు;
    my_లక్ష్యం=లక్ష్యం;
    లక్ష్యం_నిడివి=0;

    ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
        /* perform real conversion */
        లక్ష్యం_హద్దు=లక్ష్యం+లక్ష్యం_సామర్థ్యం;
        ucnv_convertEx(బహిర్గతం_Converter, లోపల_Converter,
                       &my_లక్ష్యం, లక్ష్యం_హద్దు,
                       &ఆధారం, ఆధారం_హద్దు,
                       ఇరుసు_బఫరు, &ఇరుసు, &pivot2, ఇరుసు_బఫరు+CHUNK_కొలత,
                       అబద్ధం,
                       నిజం,
                       p_Error_సంకేతం);
        లక్ష్యం_నిడివి = static_cast<int32_t>(my_లక్ష్యం - లక్ష్యం);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    ఒకవేళ(*p_Error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR || లక్ష్యం_సామర్థ్యం==0)
    {
        అక్షరం లక్ష్యం_బఫరు[CHUNK_కొలత];

        లక్ష్యం_హద్దు=లక్ష్యం_బఫరు+CHUNK_కొలత;
        చేయి {
            *p_Error_సంకేతం=U_ZERO_ERROR;
            my_లక్ష్యం=లక్ష్యం_బఫరు;
            ucnv_convertEx(బహిర్గతం_Converter, లోపల_Converter,
                           &my_లక్ష్యం, లక్ష్యం_హద్దు,
                           &ఆధారం, ఆధారం_హద్దు,
                           ఇరుసు_బఫరు, &ఇరుసు, &pivot2, ఇరుసు_బఫరు+CHUNK_కొలత,
                           అబద్ధం,
                           నిజం,
                           p_Error_సంకేతం);
            లక్ష్యం_నిడివి += static_cast<int32_t>(my_లక్ష్యం - లక్ష్యం_బఫరు);
        } అయితే(*p_Error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        తిరిగి u_terminateChars(లక్ష్యం, లక్ష్యం_సామర్థ్యం, లక్ష్యం_నిడివి, p_Error_సంకేతం);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    తిరిగి లక్ష్యం_నిడివి;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(స్థిరరాశి అక్షరం *గమ్యం_Converter_Name, స్థిరరాశి అక్షరం *fromConverterName,
             అక్షరం *లక్ష్యం, int32_t లక్ష్యం_సామర్థ్యం,
             స్థిరరాశి అక్షరం *ఆధారం, int32_t ఆధారం_నిడివి,
             UError_సంకేతం *p_Error_సంకేతం) {
    UConverter in, out; /* stack-allocated */
    UConverter *లోపల_Converter, *బహిర్గతం_Converter;
    int32_t లక్ష్యం_నిడివి;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ( ఆధారం==శూన్యసూచిక || ఆధారం_నిడివి<-1 ||
        లక్ష్యం_సామర్థ్యం<0 || (లక్ష్యం_సామర్థ్యం>0 && లక్ష్యం==శూన్యసూచిక)
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* if there is no input data, we're done */
    ఒకవేళ(ఆధారం_నిడివి==0 || (ఆధారం_నిడివి<0 && *ఆధారం==0)) {
        తిరిగి u_terminateChars(లక్ష్యం, లక్ష్యం_సామర్థ్యం, 0, p_Error_సంకేతం);
    }

    /* create the converters */
    లోపల_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    బహిర్గతం_Converter=ucnv_createConverter(&out, గమ్యం_Converter_Name, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        ucnv_close(లోపల_Converter);
        తిరిగి 0;
    }

    లక్ష్యం_నిడివి=ucnv_అంతర్గత_Convert(బహిర్గతం_Converter, లోపల_Converter,
                                      లక్ష్యం, లక్ష్యం_సామర్థ్యం,
                                      ఆధారం, ఆధారం_నిడివి,
                                      p_Error_సంకేతం);

    ucnv_close(లోపల_Converter);
    ucnv_close(బహిర్గతం_Converter);

    తిరిగి లక్ష్యం_నిడివి;
}

/* @internal */
స్థిర int32_t
ucnv_convertAlgorithmic(UBool convert_గమ్యం_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        అక్షరం *లక్ష్యం, int32_t లక్ష్యం_సామర్థ్యం,
                        స్థిరరాశి అక్షరం *ఆధారం, int32_t ఆధారం_నిడివి,
                        UError_సంకేతం *p_Error_సంకేతం) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *గమ్యం, *from;
    int32_t లక్ష్యం_నిడివి;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ( cnv==శూన్యసూచిక || ఆధారం==శూన్యసూచిక || ఆధారం_నిడివి<-1 ||
        లక్ష్యం_సామర్థ్యం<0 || (లక్ష్యం_సామర్థ్యం>0 && లక్ష్యం==శూన్యసూచిక)
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* if there is no input data, we're done */
    ఒకవేళ(ఆధారం_నిడివి==0 || (ఆధారం_నిడివి<0 && *ఆధారం==0)) {
        తిరిగి u_terminateChars(లక్ష్యం, లక్ష్యం_సామర్థ్యం, 0, p_Error_సంకేతం);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* reset the other converter */
    ఒకవేళ(convert_గమ్యం_Algorithmic) {
        /* cnv->Unicode->algo */
        ucnv_reset_గమ్యం_Unicode(cnv);
        గమ్యం=algoConverter;
        from=cnv;
    } లేకపోతే {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        గమ్యం=cnv;
    }

    లక్ష్యం_నిడివి=ucnv_అంతర్గత_Convert(గమ్యం, from,
                                      లక్ష్యం, లక్ష్యం_సామర్థ్యం,
                                      ఆధారం, ఆధారం_నిడివి,
                                      p_Error_సంకేతం);

    ucnv_close(algoConverter);

    తిరిగి లక్ష్యం_నిడివి;
}

U_CAPI int32_t U_EXPORT2
ucnv_గమ్యం_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   అక్షరం *లక్ష్యం, int32_t లక్ష్యం_సామర్థ్యం,
                   స్థిరరాశి అక్షరం *ఆధారం, int32_t ఆధారం_నిడివి,
                   UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి ucnv_convertAlgorithmic(నిజం, algorithmicType, cnv,
                                   లక్ష్యం, లక్ష్యం_సామర్థ్యం,
                                   ఆధారం, ఆధారం_నిడివి,
                                   p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     అక్షరం *లక్ష్యం, int32_t లక్ష్యం_సామర్థ్యం,
                     స్థిరరాశి అక్షరం *ఆధారం, int32_t ఆధారం_నిడివి,
                     UError_సంకేతం *p_Error_సంకేతం) UPRV_NO_SANITIZE_UNDEFINED {

    ఒకవేళ(algorithmicType<0 || UCNV_సంఖ్య_యొక్క_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    తిరిగి ucnv_convertAlgorithmic(అబద్ధం, algorithmicType, cnv,
                                   లక్ష్యం, లక్ష్యం_సామర్థ్యం,
                                   ఆధారం, ఆధారం_నిడివి,
                                   p_Error_సంకేతం);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(స్థిరరాశి UConverter* converter)
{
    int8_t type = converter->shared_దత్తాంశం->static_దత్తాంశం->conversionType;
#ఒకవేళ !UCONFIG_NO_LEGACY_CONVERSION
    ఒకవేళ(type == UCNV_MBCS) {
        తిరిగి ucnv_MBCSGetType(converter);
    }
#అయితే_ముగింపు
    తిరిగి (UConverterType)type;
}

U_CAPI శూన్య  U_EXPORT2
ucnv_getStarters(స్థిరరాశి UConverter* converter, 
                 UBool starters[256],
                 UError_సంకేతం* err)
{
    ఒకవేళ (err == శూన్యసూచిక || U_FAILURE(*err)) {
        తిరిగి;
    }

    ఒకవేళ(converter->shared_దత్తాంశం->impl->getStarters != శూన్యసూచిక) {
        converter->shared_దత్తాంశం->impl->getStarters(converter, starters, err);
    } లేకపోతే {
        *err = U_ILLEGAL_వాదం_ERROR;
    }
}

స్థిర స్థిరరాశి UAmbiguousConverter *ucnv_getAmbiguous(స్థిరరాశి UConverter *cnv)
{
    UError_సంకేతం error_సంకేతం;
    స్థిరరాశి అక్షరం *name;
    int32_t i;

    ఒకవేళ(cnv==శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }

    error_సంకేతం=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    కొరకు(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        ఒకవేళ(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            తిరిగి ambiguousConverters+i;
        }
    }

    తిరిగి శూన్యసూచిక;
}

U_CAPI శూన్య  U_EXPORT2
ucnv_fixFileSeparator(స్థిరరాశి UConverter *cnv, 
                      char16_t* ఆధారం,
                      int32_t ఆధారం_నిడివి) {
    స్థిరరాశి UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    ఒకవేళ(cnv==శూన్యసూచిక || ఆధారం==శూన్యసూచిక || ఆధారం_నిడివి<=0 || (a=ucnv_getAmbiguous(cnv))==శూన్యసూచిక)
    {
        తిరిగి;
    }

    variant5c=a->variant5c;
    కొరకు(i=0; i<ఆధారం_నిడివి; ++i) {
        ఒకవేళ(ఆధారం[i]==variant5c) {
            ఆధారం[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(స్థిరరాశి UConverter *cnv) {
    తిరిగి ucnv_getAmbiguous(cnv)!=శూన్యసూచిక;
}

U_CAPI శూన్య  U_EXPORT2
ucnv_సమితి_Fallback(UConverter *cnv, UBool ఉపయోగిస్తుంది_Fallback)
{
    cnv->useFallback = ఉపయోగిస్తుంది_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_ఉపయోగిస్తుంది_Fallback(స్థిరరాశి UConverter *cnv)
{
    తిరిగి cnv->useFallback;
}

U_CAPI శూన్య  U_EXPORT2
ucnv_getInvalidChars (స్థిరరాశి UConverter * converter,
                      అక్షరం *err_బైట్లు,
                      int8_t * len,
                      UError_సంకేతం * err)
{
    ఒకవేళ (err == శూన్యసూచిక || U_FAILURE(*err))
    {
        తిరిగి;
    }
    ఒకవేళ (len == శూన్యసూచిక || err_బైట్లు == శూన్యసూచిక || converter == శూన్యసూచిక)
    {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ (*len < converter->invalid_Char_నిడివి)
    {
        *err = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }
    ఒకవేళ ((*len = converter->invalid_Char_నిడివి) > 0)
    {
        uprv_memcpy (err_బైట్లు, converter->invalid_Char_బఫరు, *len);
    }
}

U_CAPI శూన్య  U_EXPORT2
ucnv_getInvalidUChars (స్థిరరాశి UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_సంకేతం * err)
{
    ఒకవేళ (err == శూన్యసూచిక || U_FAILURE(*err))
    {
        తిరిగి;
    }
    ఒకవేళ (len == శూన్యసూచిక || errChars == శూన్యసూచిక || converter == శూన్యసూచిక)
    {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ (*len < converter->invalid_UChar_నిడివి)
    {
        *err = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }
    ఒకవేళ ((*len = converter->invalid_UChar_నిడివి) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_బఫరు, *len);
    }
}

#నిర్వచించు SIG_MAX_LEN 5

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ucnv_detectUnicodeSignature( స్థిరరాశి అక్షరం* ఆధారం,
                             int32_t ఆధారం_నిడివి,
                             int32_t* signature_నిడివి,
                             UError_సంకేతం* p_Error_సంకేతం) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    అక్షరం ప్రారంభం[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    పూర్ణాంకం i = 0;

    ఒకవేళ((p_Error_సంకేతం==శూన్యసూచిక) || U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }
    
    ఒకవేళ(ఆధారం == శూన్యసూచిక || ఆధారం_నిడివి < -1){
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(signature_నిడివి == శూన్యసూచిక) {
        signature_నిడివి = &dummy;
    }

    ఒకవేళ(ఆధారం_నిడివి==-1){
        ఆధారం_నిడివి=(int32_t)uprv_strlen(ఆధారం);
    }

    
    అయితే(i<ఆధారం_నిడివి&& i<SIG_MAX_LEN){
        ప్రారంభం[i]=ఆధారం[i];
        i++;
    }

    ఒకవేళ(ప్రారంభం[0] == '\xFE' && ప్రారంభం[1] == '\xFF') {
        *signature_నిడివి=2;
        తిరిగి  "UTF-16BE";
    } లేకపోతే ఒకవేళ(ప్రారంభం[0] == '\xFF' && ప్రారంభం[1] == '\xFE') {
        ఒకవేళ(ప్రారంభం[2] == '\x00' && ప్రారంభం[3] =='\x00') {
            *signature_నిడివి=4;
            తిరిగి "UTF-32LE";
        } లేకపోతే {
            *signature_నిడివి=2;
            తిరిగి  "UTF-16LE";
        }
    } లేకపోతే ఒకవేళ(ప్రారంభం[0] == '\xEF' && ప్రారంభం[1] == '\xBB' && ప్రారంభం[2] == '\xBF') {
        *signature_నిడివి=3;
        తిరిగి  "UTF-8";
    } లేకపోతే ఒకవేళ(ప్రారంభం[0] == '\x00' && ప్రారంభం[1] == '\x00' && 
              ప్రారంభం[2] == '\xFE' && ప్రారంభం[3]=='\xFF') {
        *signature_నిడివి=4;
        తిరిగి  "UTF-32BE";
    } లేకపోతే ఒకవేళ(ప్రారంభం[0] == '\x0E' && ప్రారంభం[1] == '\xFE' && ప్రారంభం[2] == '\xFF') {
        *signature_నిడివి=3;
        తిరిగి "SCSU";
    } లేకపోతే ఒకవేళ(ప్రారంభం[0] == '\xFB' && ప్రారంభం[1] == '\xEE' && ప్రారంభం[2] == '\x28') {
        *signature_నిడివి=3;
        తిరిగి "BOCU-1";
    } లేకపోతే ఒకవేళ(ప్రారంభం[0] == '\x2B' && ప్రారంభం[1] == '\x2F' && ప్రారంభం[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        ఒకవేళ(ప్రారంభం[3] == '\x38' && ప్రారంభం[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_నిడివి=5;
            తిరిగి "UTF-7";
        } లేకపోతే ఒకవేళ(ప్రారంభం[3] == '\x38' || ప్రారంభం[3] == '\x39' || ప్రారంభం[3] == '\x2B' || ప్రారంభం[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_నిడివి=4;
            తిరిగి "UTF-7";
        }
    }లేకపోతే ఒకవేళ(ప్రారంభం[0]=='\xDD' && ప్రారంభం[1]== '\x73'&& ప్రారంభం[2]=='\x66' && ప్రారంభం[3]=='\x73'){
        *signature_నిడివి=4;
        తిరిగి "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_నిడివి=0;
    తిరిగి శూన్యసూచిక;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(స్థిరరాశి UConverter* cnv, UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి -1;
    }
    ఒకవేళ(cnv == శూన్యసూచిక){
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి -1;
    }

    ఒకవేళ(cnv->preFromUFirstCP >= 0){
        తిరిగి U16_నిడివి(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }లేకపోతే ఒకవేళ(cnv->preFromULength < 0){
        తిరిగి -cnv->preFromULength ;
    }లేకపోతే ఒకవేళ(cnv->fromUChar32 > 0){
        తిరిగి 1;
    }
    తిరిగి 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_గమ్యం_UCount_Pending(స్థిరరాశి UConverter* cnv, UError_సంకేతం* స్థితి_2){

    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి -1;
    }
    ఒకవేళ(cnv == శూన్యసూచిక){
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి -1;
    }

    ఒకవేళ(cnv->pre_గమ్యం_ULength > 0){
        తిరిగి cnv->pre_గమ్యం_ULength ;
    }లేకపోతే ఒకవేళ(cnv->pre_గమ్యం_ULength < 0){
        తిరిగి -cnv->pre_గమ్యం_ULength;
    }లేకపోతే ఒకవేళ(cnv->గమ్యం_ULength > 0){
        తిరిగి cnv->గమ్యం_ULength;
    }
    తిరిగి 0;
}

U_CAPI UBool U_EXPORT2
ucnv_is_Fixed_వెడల్పు(UConverter *cnv, UError_సంకేతం *స్థితి_2){
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి అబద్ధం;
    }

    ఒకవేళ (cnv == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }

    ఎంపిక (ucnv_getType(cnv)) {
        స్థితి UCNV_SBCS:
        స్థితి UCNV_DBCS:
        స్థితి UCNV_UTF32_BigEndian:
        స్థితి UCNV_UTF32_LittleEndian:
        స్థితి UCNV_UTF32:
        స్థితి UCNV_US_ASCII:
            తిరిగి నిజం;
        అప్రమేయ:
            తిరిగి అబద్ధం;
    }
}
#అయితే_ముగింపు

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
