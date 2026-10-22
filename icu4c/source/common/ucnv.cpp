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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ <memory>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_err.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHUNK_ਮਾਪ 1024

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UAmbiguousConverter {
    ਅਚਲ ਅੱਖਰ *name;
    ਅਚਲ char16_t variant5c;
} UAmbiguousConverter;

ਸਥਿਰ ਅਚਲ UAmbiguousConverter ambiguousConverters[]={
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
ucnv_open (ਅਚਲ ਅੱਖਰ *name,
                       UError_ਕੋਡ * err)
{
    UConverter *r;

    ਜੇ (err == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE (*err)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    r =  ucnv_createConverter(ਖਾਲੀ_ਸੂਚਕ, name, err);
    ਵਾਪਸ r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ਅਚਲ ਅੱਖਰ *packageName, ਅਚਲ ਅੱਖਰ *converterName, UError_ਕੋਡ * err)
{
    ਵਾਪਸ ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ਅਚਲ char16_t * name,
                         UError_ਕੋਡ * err)
{
    ਅੱਖਰ asciiName[UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ];

    ਜੇ (err == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err))
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (name == ਖਾਲੀ_ਸੂਚਕ)
        ਵਾਪਸ ucnv_open (ਖਾਲੀ_ਸੂਚਕ, err);
    ਜੇ (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ)
    {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
ਸਥਿਰ int32_t
ucnv_copyPlatformString(ਅੱਖਰ *platformString, UConverterPlatform pltfrm)
{
    ਚੋਣ (pltfrm)
    {
    ਹਾਲਤ UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        ਵਾਪਸ 4;
    ਹਾਲਤ UCNV_UNKNOWN:
        ਰੋਕੋ;
    }

    /* default to empty string */
    *platformString = 0;
    ਵਾਪਸ 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UError_ਕੋਡ * err)
{
    ਅੱਖਰ myName[UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ];
    int32_t myNameLen;

    ਜੇ (err == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE (*err))
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_ਮੰਜ਼ਿਲ_String(myName + myNameLen, codepage, 10);

    ਵਾਪਸ ucnv_createConverter(ਖਾਲੀ_ਸੂਚਕ, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_ਸੁਰੱਖਿਅਤ_Clone(ਅਚਲ UConverter* cnv,
               ਖਾਲੀ* ਢੇਰ_ਬਫਰ U_LIFETIME_BOUND,
               int32_t* p_ਬਫਰ_ਮਾਪ,
               UError_ਕੋਡ* ਸਥਿਤੀ)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t ਢੇਰ_ਬਫਰ_ਮਾਪ;
    int32_t ਬਫਰ_ਮਾਪ_Needed;
    UError_ਕੋਡ cbErr;
    UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ ਮੰਜ਼ਿਲ_UArgs = {
        ਆਕਾਰ(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ),
            ਸੱਚ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ
    };
    UConverter_From_Unicode_ਦਲੀਲਾਂ fromUArgs = {
        ਆਕਾਰ(UConverter_From_Unicode_ਦਲੀਲਾਂ),
            ਸੱਚ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ
    };

    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_CLONE);

    ਜੇ (ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ? *ਸਥਿਤੀ: U_ILLEGAL_ਦਲੀਲ_ERROR);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (cnv == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        UTRACE_EXIT_ਸਥਿਤੀ(*ਸਥਿਤੀ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, ਸਥਿਤੀ), cnv, ਢੇਰ_ਬਫਰ);

    ਜੇ (cnv->shared_ਅੰਕੜੇ->impl->ਸੁਰੱਖਿਅਤ_Clone != ਖਾਲੀ_ਸੂਚਕ) {
        /* call the custom safeClone function for sizing */
        ਬਫਰ_ਮਾਪ_Needed = 0;
        cnv->shared_ਅੰਕੜੇ->impl->ਸੁਰੱਖਿਅਤ_Clone(cnv, ਖਾਲੀ_ਸੂਚਕ, &ਬਫਰ_ਮਾਪ_Needed, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            UTRACE_EXIT_ਸਥਿਤੀ(*ਸਥਿਤੀ);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }
    ਨਹੀਂਤਾਂ
    {
        /* inherent sizing */
        ਬਫਰ_ਮਾਪ_Needed = ਆਕਾਰ(UConverter);
    }

    ਜੇ (p_ਬਫਰ_ਮਾਪ == ਖਾਲੀ_ਸੂਚਕ) {
        ਢੇਰ_ਬਫਰ_ਮਾਪ = 1;
        p_ਬਫਰ_ਮਾਪ = &ਢੇਰ_ਬਫਰ_ਮਾਪ;
    } ਨਹੀਂਤਾਂ {
        ਢੇਰ_ਬਫਰ_ਮਾਪ = *p_ਬਫਰ_ਮਾਪ;
        ਜੇ (ਢੇਰ_ਬਫਰ_ਮਾਪ <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_ਬਫਰ_ਮਾਪ = ਬਫਰ_ਮਾਪ_Needed;
            UTRACE_EXIT_ਮੁੱਲ(ਬਫਰ_ਮਾਪ_Needed);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    ਜੇ (ਢੇਰ_ਬਫਰ) {
        uintptr_t p = reinterpret_cast<uintptr_t>(ਢੇਰ_ਬਫਰ);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t ਸੂਚਕ_Adjustment = aligned_p - p;
        ਜੇ (ਬਫਰ_ਮਾਪ_Needed + ਸੂਚਕ_Adjustment <= ਢੇਰ_ਬਫਰ_ਮਾਪ) {
            ਢੇਰ_ਬਫਰ = reinterpret_cast<ਖਾਲੀ *>(aligned_p);
            ਢੇਰ_ਬਫਰ_ਮਾਪ -= static_cast<int32_t>(ਸੂਚਕ_Adjustment);
        } ਨਹੀਂਤਾਂ {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            ਢੇਰ_ਬਫਰ_ਮਾਪ = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    ਜੇ (ਢੇਰ_ਬਫਰ_ਮਾਪ < ਬਫਰ_ਮਾਪ_Needed || ਢੇਰ_ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (ਬਫਰ_ਮਾਪ_Needed);

        ਜੇ(localConverter == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            UTRACE_EXIT_ਸਥਿਤੀ(*ਸਥਿਤੀ);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        ਜੇ (p_ਬਫਰ_ਮਾਪ != &ਢੇਰ_ਬਫਰ_ਮਾਪ) {
            *ਸਥਿਤੀ = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_ਬਫਰ_ਮਾਪ = ਬਫਰ_ਮਾਪ_Needed;
    } ਨਹੀਂਤਾਂ {
        /* just use the stack buffer */
        localConverter = (UConverter*) ਢੇਰ_ਬਫਰ;
        allocatedConverter = ਖਾਲੀ_ਸੂਚਕ;
    }

    uprv_memset(localConverter, 0, ਬਫਰ_ਮਾਪ_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, ਆਕਾਰ(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = ਝੂਠ;

    /* copy the substitution string */
    ਜੇ (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } ਨਹੀਂਤਾਂ {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ * U_SIZEOF_UCHAR);
        ਜੇ (localConverter->subChars == ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_ਸਥਿਤੀ(*ਸਥਿਤੀ);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    ਜੇ (cnv->shared_ਅੰਕੜੇ->impl->ਸੁਰੱਖਿਅਤ_Clone != ਖਾਲੀ_ਸੂਚਕ) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_ਅੰਕੜੇ->impl->ਸੁਰੱਖਿਅਤ_Clone(cnv, localConverter, p_ਬਫਰ_ਮਾਪ, ਸਥਿਤੀ);
    }

    ਜੇ(localConverter==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਜੇ (allocatedConverter != ਖਾਲੀ_ਸੂਚਕ && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_ਸਥਿਤੀ(*ਸਥਿਤੀ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* increment refcount of shared data if needed */
    ਜੇ (cnv->shared_ਅੰਕੜੇ->isReferenceCounted) {
        ucnv_increment_Ref_ਗਿਣਤੀ(cnv->shared_ਅੰਕੜੇ);
    }

    ਜੇ(localConverter == (UConverter*)ਢੇਰ_ਬਫਰ) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = ਸੱਚ;
    }

    /* allow callback functions to handle any memory allocation */
    ਮੰਜ਼ਿਲ_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->ਮੰਜ਼ਿਲ_UContext, &ਮੰਜ਼ਿਲ_UArgs, ਖਾਲੀ_ਸੂਚਕ, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, ਖਾਲੀ_ਸੂਚਕ, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_ਸਥਿਤੀ(localConverter, *ਸਥਿਤੀ);
    ਵਾਪਸ localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ਅਚਲ UConverter* cnv, UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਵਾਪਸ ucnv_ਸੁਰੱਖਿਅਤ_Clone(cnv, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;

    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_CLOSE);

    ਜੇ (converter == ਖਾਲੀ_ਸੂਚਕ)
    {
        UTRACE_EXIT();
        ਵਾਪਸ;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_ਕੋਡ), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    ਜੇ (converter->fromCharErrorBehaviour != UCNV_ਮੰਜ਼ਿਲ_U_DEFAULT_CALLBACK) {
        UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ ਮੰਜ਼ਿਲ_UArgs = {
            ਆਕਾਰ(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ),
                ਸੱਚ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ
        };

        ਮੰਜ਼ਿਲ_UArgs.converter = converter;
        error_ਕੋਡ = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->ਮੰਜ਼ਿਲ_UContext, &ਮੰਜ਼ਿਲ_UArgs, ਖਾਲੀ_ਸੂਚਕ, 0, UCNV_CLOSE, &error_ਕੋਡ);
    }
    ਜੇ (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_ਦਲੀਲਾਂ fromUArgs = {
            ਆਕਾਰ(UConverter_From_Unicode_ਦਲੀਲਾਂ),
                ਸੱਚ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ
        };
        fromUArgs.converter = converter;
        error_ਕੋਡ = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, ਖਾਲੀ_ਸੂਚਕ, 0, 0, UCNV_CLOSE, &error_ਕੋਡ);
    }

    ਜੇ (converter->shared_ਅੰਕੜੇ->impl->close != ਖਾਲੀ_ਸੂਚਕ) {
        converter->shared_ਅੰਕੜੇ->impl->close(converter);
    }

    ਜੇ (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    ਜੇ (converter->shared_ਅੰਕੜੇ->isReferenceCounted) {
        ucnv_unload_Shared_ਅੰਕੜੇ_If_Ready(converter->shared_ਅੰਕੜੇ);
    }

    ਜੇ(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ਅਚਲ ਅੱਖਰ*   U_EXPORT2
ucnv_get_ਉਪਲਬਧ_Name (int32_t n)
{
    ਜੇ (0 <= n && n <= 0xffff) {
        UError_ਕੋਡ err = U_ZERO_ERROR;
        ਅਚਲ ਅੱਖਰ *name = ucnv_bld_get_ਉਪਲਬਧ_Converter((uint16_t)n, &err);
        ਜੇ (U_SUCCESS(err)) {
            ਵਾਪਸ name;
        }
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI int32_t   U_EXPORT2
ucnv_ਗਿਣਤੀ_ਉਪਲਬਧ ()
{
    UError_ਕੋਡ err = U_ZERO_ERROR;
    ਵਾਪਸ ucnv_bld_ਗਿਣਤੀ_ਉਪਲਬਧ_Converters(&err);
}

U_CAPI ਖਾਲੀ    U_EXPORT2
ucnv_getSubstChars (ਅਚਲ UConverter * converter,
                    ਅੱਖਰ *mySubChar,
                    int8_t * len,
                    UError_ਕੋਡ * err)
{
    ਜੇ (U_FAILURE (*err))
        ਵਾਪਸ;

    ਜੇ (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        ਵਾਪਸ;
    }

    ਜੇ (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI ਖਾਲੀ    U_EXPORT2
ucnv_ਸੈੱਟ_Subst_Chars (UConverter * converter,
                    ਅਚਲ ਅੱਖਰ *mySubChar,
                    int8_t len,
                    UError_ਕੋਡ * err)
{
    ਜੇ (U_FAILURE (*err))
        ਵਾਪਸ;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    ਜੇ ((len > converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char)
     || (len < converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->min_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char))
    {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
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

U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_ਸੈੱਟ_Subst_String(UConverter *cnv,
                    ਅਚਲ char16_t *s,
                    int32_t length,
                    UError_ਕੋਡ *err) {
    alignas(UConverter) ਅੱਖਰ clone_ਬਫਰ[U_CNV_SAFECLONE_BUFFERSIZE];
    ਅੱਖਰ chars[UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_ਮਾਪ, length8;

    /* Let the following functions check all arguments. */
    clone_ਮਾਪ = ਆਕਾਰ(clone_ਬਫਰ);
    clone = ucnv_ਸੁਰੱਖਿਅਤ_Clone(cnv, clone_ਬਫਰ, &clone_ਮਾਪ, err);
    ucnv_ਸੈੱਟ_From_UCall_ਪਿੱਠ(clone, UCNV_FROM_U_CALLBACK_STOP, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)ਆਕਾਰ(chars), s, length, err);
    ucnv_close(clone);
    ਜੇ (U_FAILURE(*err)) {
        ਵਾਪਸ;
    }

    ਜੇ (cnv->shared_ਅੰਕੜੇ->impl->writeSub == ਖਾਲੀ_ਸੂਚਕ
#ਜੇ !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#ਜੇ_ਅੰਤ
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } ਨਹੀਂਤਾਂ {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        ਜੇ (length > UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ;
        }
        subChars = (uint8_t *)s;
        ਜੇ (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    ਜੇ (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        ਜੇ (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ * U_SIZEOF_UCHAR);
            ਜੇ (cnv->subChars == ਖਾਲੀ_ਸੂਚਕ) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    ਜੇ (length8 == 0) {
        cnv->subCharLen = 0;
    } ਨਹੀਂਤਾਂ {
        uprv_memcpy(cnv->subChars, subChars, length8);
        ਜੇ (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } ਨਹੀਂਤਾਂ /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
ਸਥਿਰ ਖਾਲੀ _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    ਜੇ(converter == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    ਜੇ(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_ਕੋਡ error_ਕੋਡ;

        ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE && converter->fromCharErrorBehaviour != UCNV_ਮੰਜ਼ਿਲ_U_DEFAULT_CALLBACK) {
            UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ ਮੰਜ਼ਿਲ_UArgs = {
                ਆਕਾਰ(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ),
                ਸੱਚ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ
            };
            ਮੰਜ਼ਿਲ_UArgs.converter = converter;
            error_ਕੋਡ = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->ਮੰਜ਼ਿਲ_UContext, &ਮੰਜ਼ਿਲ_UArgs, ਖਾਲੀ_ਸੂਚਕ, 0, UCNV_RESET, &error_ਕੋਡ);
        }
        ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_ਦਲੀਲਾਂ fromUArgs = {
                ਆਕਾਰ(UConverter_From_Unicode_ਦਲੀਲਾਂ),
                ਸੱਚ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ,
                ਖਾਲੀ_ਸੂਚਕ
            };
            fromUArgs.converter = converter;
            error_ਕੋਡ = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, ਖਾਲੀ_ਸੂਚਕ, 0, 0, UCNV_RESET, &error_ਕੋਡ);
        }
    }

    /* now reset the converter itself */
    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = converter->shared_ਅੰਕੜੇ->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
        converter->ਬਹੁਲਕ = 0;
        converter->ਮੰਜ਼ਿਲ_ULength = 0;
        converter->invalid_Char_ਲੰਮਾਈ = converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 0;
        converter->pre_ਮੰਜ਼ਿਲ_ULength = 0;
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        converter->from_Unicode_ਸਥਿਤੀ = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_ਲੰਮਾਈ = converter->char_Error_ਬਫਰ_ਲੰਮਾਈ = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    ਜੇ (converter->shared_ਅੰਕੜੇ->impl->reset != ਖਾਲੀ_ਸੂਚਕ) {
        /* call the custom reset function */
        converter->shared_ਅੰਕੜੇ->impl->reset(converter, choice);
    }
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, ਸੱਚ);
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_reset_ਮੰਜ਼ਿਲ_Unicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE, ਸੱਚ);
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, ਸੱਚ);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_ਮਾਪ (ਅਚਲ UConverter * converter)
{
    ਵਾਪਸ converter->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_ਮਾਪ (ਅਚਲ UConverter * converter)
{
    ਵਾਪਸ converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->min_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ucnv_getName(ਅਚਲ UConverter* converter U_LIFETIME_BOUND, UError_ਕੋਡ* err)
{
    ਜੇ (U_FAILURE (*err))
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ(converter->shared_ਅੰਕੜੇ->impl->getName){
        ਅਚਲ ਅੱਖਰ* ਆਰਜ਼ੀ= converter->shared_ਅੰਕੜੇ->impl->getName(converter);
        ਜੇ(ਆਰਜ਼ੀ)
            ਵਾਪਸ ਆਰਜ਼ੀ;
    }
    ਵਾਪਸ converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ਅਚਲ UConverter * converter,
              UError_ਕੋਡ * err)
{
    int32_t ccsid;
    ਜੇ (U_FAILURE (*err))
        ਵਾਪਸ -1;

    ccsid = converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->codepage;
    ਜੇ (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        ਅਚਲ ਅੱਖਰ *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        ਜੇ (U_SUCCESS(*err) && standardName) {
            ਅਚਲ ਅੱਖਰ *ccsidStr = uprv_strchr(standardName, '-');
            ਜੇ (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    ਵਾਪਸ ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (ਅਚਲ UConverter * converter,
                                      UError_ਕੋਡ * err)
{
    ਜੇ (U_FAILURE (*err))
        ਵਾਪਸ UCNV_UNKNOWN;

    ਵਾਪਸ (UConverterPlatform)converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->platform;
}

U_CAPI ਖਾਲੀ U_EXPORT2
    ucnv_get_ਮੰਜ਼ਿਲ_UCall_ਪਿੱਠ (ਅਚਲ UConverter * converter,
                         UConverter_ਮੰਜ਼ਿਲ_UCallback *action,
                         ਅਚਲ ਖਾਲੀ **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->ਮੰਜ਼ਿਲ_UContext;
}

U_CAPI ਖਾਲੀ U_EXPORT2
    ucnv_get_From_UCall_ਪਿੱਠ (ਅਚਲ UConverter * converter,
                           UConverterFromUCallback *action,
                           ਅਚਲ ਖਾਲੀ **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI ਖਾਲੀ    U_EXPORT2
ucnv_ਸੈੱਟ_ਮੰਜ਼ਿਲ_UCall_ਪਿੱਠ (UConverter * converter,
                            UConverter_ਮੰਜ਼ਿਲ_UCallback newAction,
                            ਅਚਲ ਖਾਲੀ* newContext,
                            UConverter_ਮੰਜ਼ਿਲ_UCallback *oldAction,
                            ਅਚਲ ਖਾਲੀ** oldContext,
                            UError_ਕੋਡ * err)
{
    ਜੇ (U_FAILURE (*err))
        ਵਾਪਸ;
    ਜੇ (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    ਜੇ (oldContext) *oldContext = converter->ਮੰਜ਼ਿਲ_UContext;
    converter->ਮੰਜ਼ਿਲ_UContext = newContext;
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_ਸੈੱਟ_From_UCall_ਪਿੱਠ (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ਅਚਲ ਖਾਲੀ* newContext,
                            UConverterFromUCallback *oldAction,
                            ਅਚਲ ਖਾਲੀ** oldContext,
                            UError_ਕੋਡ * err)
{
    ਜੇ (U_FAILURE (*err))
        ਵਾਪਸ;
    ਜੇ (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    ਜੇ (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

ਸਥਿਰ ਖਾਲੀ
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, int32_t error_ਨਿਵੇਸ਼_ਲੰਮਾਈ) {
    int32_t *ਹੱਦਬੰਦੀ;
    int32_t delta, ਵਿੱਥ;

    ਜੇ(ਸਰੋਤ_ਸੂਚਕਾਂਕ>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=ਸਰੋਤ_ਸੂਚਕਾਂਕ-error_ਨਿਵੇਸ਼_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    ਹੱਦਬੰਦੀ=offsets+length;
    ਜੇ(delta==0) {
        /* most common case, nothing to do */
    } ਨਹੀਂਤਾਂ ਜੇ(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        ਜਦੋਂ(offsets<ਹੱਦਬੰਦੀ) {
            ਵਿੱਥ=*offsets;
            ਜੇ(ਵਿੱਥ>=0) {
                *offsets=ਵਿੱਥ+delta;
            }
            ++offsets;
        }
    } ਨਹੀਂਤਾਂ /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        ਜਦੋਂ(offsets<ਹੱਦਬੰਦੀ) {
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

ਸਥਿਰ ਖਾਲੀ
_fromUnicodeWithCallback(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ਅਚਲ char16_t *s;
    ਅੱਖਰ *t;
    int32_t *offsets;
    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    int32_t error_ਨਿਵੇਸ਼_ਲੰਮਾਈ;
    UBool converter_Saw_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ਅਚਲ char16_t *real_ਸਰੋਤ, *real_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    int32_t real_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    UBool realFlush;

    cnv=p_ਦਲੀਲਾਂ->converter;
    s=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    t=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* get the converter implementation function */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;
    ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
        fromUnicode=cnv->shared_ਅੰਕੜੇ->impl->fromUnicode;
    } ਨਹੀਂਤਾਂ {
        fromUnicode=cnv->shared_ਅੰਕੜੇ->impl->fromUnicodeWithOffsets;
        ਜੇ(fromUnicode==ਖਾਲੀ_ਸੂਚਕ) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_ਅੰਕੜੇ->impl->fromUnicode;
            /* we will write -1 for each offset */
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;
        }
    }

    ਜੇ(cnv->preFromULength>=0) {
        /* normal mode */
        real_ਸਰੋਤ=ਖਾਲੀ_ਸੂਚਕ;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਖਾਲੀ_ਸੂਚਕ;
        realFlush=ਝੂਠ;
        real_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;
    } ਨਹੀਂਤਾਂ {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        ਜੇ (!p_ਦਲੀਲਾਂ->ਸਰੋਤ) {
            *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
        real_ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
        real_ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
        realFlush=p_ਦਲੀਲਾਂ->flush;
        real_ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਸਰੋਤ_ਸੂਚਕਾਂਕ;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_ਦਲੀਲਾਂ->ਸਰੋਤ=replay;
        p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=replay-cnv->preFromULength;
        p_ਦਲੀਲਾਂ->flush=ਝੂਠ;
        ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;

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
    ਲਈ(;;) {
        ਜੇ(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(p_ਦਲੀਲਾਂ, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_ਦਲੀਲਾਂ->flush && p_ਦਲੀਲਾਂ->ਸਰੋਤ==p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ &&
                        cnv->fromUChar32==0);
        } ਨਹੀਂਤਾਂ {
            /* handle error from ucnv_convertEx() */
            converter_Saw_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼=ਝੂਠ;
        }

        /* no callback called yet for this iteration */
        calledCallback=ਝੂਠ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_ਨਿਵੇਸ਼_ਲੰਮਾਈ=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        ਲਈ(;;) {
            /* update offsets if we write any */
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                int32_t length = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ - t);
                ਜੇ(length>0) {
                    _updateOffsets(offsets, length, ਸਰੋਤ_ਸੂਚਕਾਂਕ, error_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_ਦਲੀਲਾਂ->offsets=offsets+=length;
                }

                ਜੇ(ਸਰੋਤ_ਸੂਚਕਾਂਕ>=0) {
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ += static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਸਰੋਤ - s);
                }
            }

            ਜੇ(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                ਜੇ(real_ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ) {
                    real_ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
                    real_ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
                    realFlush=p_ਦਲੀਲਾਂ->flush;
                    real_ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਸਰੋਤ_ਸੂਚਕਾਂਕ;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=replay;
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=replay-cnv->preFromULength;
                    p_ਦਲੀਲਾਂ->flush=ਝੂਠ;
                    ਜੇ((ਸਰੋਤ_ਸੂਚਕਾਂਕ+=cnv->preFromULength)<0) {
                        ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;
                    }

                    cnv->preFromULength=0;
                } ਨਹੀਂਤਾਂ {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ);
                    *err=U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
                }
            }

            /* update pointers */
            s=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
            t=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;

            ਜੇ(U_SUCCESS(*err)) {
                ਜੇ(s<p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(real_ਸਰੋਤ!=ਖਾਲੀ_ਸੂਚਕ) {
                    /* switch back from replaying to the real source and continue */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=real_ਸਰੋਤ;
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=real_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
                    p_ਦਲੀਲਾਂ->flush=realFlush;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=real_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

                    real_ਸਰੋਤ=ਖਾਲੀ_ਸੂਚਕ;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(p_ਦਲੀਲਾਂ->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_ਮਿਲਿਆ;
                    calledCallback=ਝੂਠ; /* new error condition */
                } ਨਹੀਂਤਾਂ {
                    /* input consumed */
                    ਜੇ(p_ਦਲੀਲਾਂ->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        ਜੇ(!converter_Saw_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼) {
                            ਰੋਕੋ;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, ਝੂਠ);
                    }

                    /* done successfully */
                    ਵਾਪਸ;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_ਕੋਡ e;

                ਜੇ( calledCallback ||
                    (e=*err)==U_ਬਫਰ_ਉਛਾਲ_ERROR ||
                    (e!=U_INVALID_CHAR_ਮਿਲਿਆ &&
                     e!=U_ILLEGAL_CHAR_ਮਿਲਿਆ &&
                     e!=U_TRUNCATED_CHAR_ਮਿਲਿਆ)
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
                    ਜੇ(real_ਸਰੋਤ!=ਖਾਲੀ_ਸੂਚਕ) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ - p_ਦਲੀਲਾਂ->ਸਰੋਤ);
                        ਜੇ(length>0) {
                            u_memcpy(cnv->preFromU, p_ਦਲੀਲਾਂ->ਸਰੋਤ, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_ਦਲੀਲਾਂ->ਸਰੋਤ=real_ਸਰੋਤ;
                        p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=real_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
                        p_ਦਲੀਲਾਂ->flush=realFlush;
                    }

                    ਵਾਪਸ;
                }
            }

            /* callback handling */
            {
                UChar32 ਕੋਡ_ਬਿੰਦੂ;

                /* get and write the code point */
                ਕੋਡ_ਬਿੰਦੂ=cnv->fromUChar32;
                error_ਨਿਵੇਸ਼_ਲੰਮਾਈ=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_ਬਫਰ, error_ਨਿਵੇਸ਼_ਲੰਮਾਈ, ਕੋਡ_ਬਿੰਦੂ);
                cnv->invalid_UChar_ਲੰਮਾਈ = static_cast<int8_t>(error_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_ਦਲੀਲਾਂ,
                    cnv->invalid_UChar_ਬਫਰ, error_ਨਿਵੇਸ਼_ਲੰਮਾਈ, ਕੋਡ_ਬਿੰਦੂ,
                    *err==U_INVALID_CHAR_ਮਿਲਿਆ ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
                    err);
            }

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=ਸੱਚ;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
ਸਥਿਰ UBool
ucnv_output_ਉਛਾਲ_From_Unicode(UConverter *cnv,
                               ਅੱਖਰ **ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                               int32_t **pOffsets,
                               UError_ਕੋਡ *err) {
    int32_t *offsets;
    ਅੱਖਰ *ਉਛਾਲ, *t;
    int32_t i, length;

    t=*ਨਿਸ਼ਾਨਾ;
    ਜੇ(pOffsets!=ਖਾਲੀ_ਸੂਚਕ) {
        offsets=*pOffsets;
    } ਨਹੀਂਤਾਂ {
        offsets=ਖਾਲੀ_ਸੂਚਕ;
    }

    ਉਛਾਲ = reinterpret_cast<ਅੱਖਰ*>(cnv->char_Error_ਬਫਰ);
    length=cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ;
    i=0;
    ਜਦੋਂ(i<length) {
        ਜੇ(t==ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            ਕਰੋ {
                ਉਛਾਲ[j++]=ਉਛਾਲ[i++];
            } ਜਦੋਂ(i<length);

            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ = static_cast<int8_t>(j);
            *ਨਿਸ਼ਾਨਾ=t;
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *pOffsets=offsets;
            }
            *err=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ ਸੱਚ;
        }

        /* copy the overflow contents to the target */
        *t++=ਉਛਾਲ[i++];
        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=0;
    *ਨਿਸ਼ਾਨਾ=t;
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
        *pOffsets=offsets;
    }
    ਵਾਪਸ ਝੂਠ;
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 ਅੱਖਰ **ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                 ਅਚਲ char16_t **ਸਰੋਤ, ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                 int32_t *offsets,
                 UBool flush,
                 UError_ਕੋਡ *err) {
    UConverter_From_Unicode_ਦਲੀਲਾਂ ਦਲੀਲਾਂ_2;
    ਅਚਲ char16_t *s;
    ਅੱਖਰ *t;

    /* check parameters */
    ਜੇ(err==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err)) {
        ਵਾਪਸ;
    }

    ਜੇ(cnv==ਖਾਲੀ_ਸੂਚਕ || ਨਿਸ਼ਾਨਾ==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ) {
        *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    s=*ਸਰੋਤ;
    t=*ਨਿਸ਼ਾਨਾ;

    ਜੇ ((ਅਚਲ ਖਾਲੀ *)U_MAX_PTR(ਸਰੋਤ_ਹੱਦਬੰਦੀ) == (ਅਚਲ ਖਾਲੀ *)ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        ਸਰੋਤ_ਹੱਦਬੰਦੀ = (ਅਚਲ char16_t *)(((ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ_ਹੱਦਬੰਦੀ) - 1);
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
    ਜੇ (ਸਰੋਤ_ਹੱਦਬੰਦੀ<s || ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ<t ||
        ((size_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-s)>(size_t)0x3fffffff && ਸਰੋਤ_ਹੱਦਬੰਦੀ>s) ||
        ((size_t)(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-t)>(size_t)0x7fffffff && ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ>t) ||
        (((ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ_ਹੱਦਬੰਦੀ-(ਅਚਲ ਅੱਖਰ *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    
    /* output the target overflow buffer */
    ਜੇ( cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ>0 &&
        ucnv_output_ਉਛਾਲ_From_Unicode(cnv, ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        ਵਾਪਸ;
    }
    /* *target may have moved, therefore stop using t */

    ਜੇ(!flush && s==ਸਰੋਤ_ਹੱਦਬੰਦੀ && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        ਵਾਪਸ;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    ਦਲੀਲਾਂ_2.converter=cnv;
    ਦਲੀਲਾਂ_2.flush=flush;
    ਦਲੀਲਾਂ_2.offsets=offsets;
    ਦਲੀਲਾਂ_2.ਸਰੋਤ=s;
    ਦਲੀਲਾਂ_2.ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ=*ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਦਲੀਲਾਂ_2.size=ਆਕਾਰ(ਦਲੀਲਾਂ_2);

    _fromUnicodeWithCallback(&ਦਲੀਲਾਂ_2, err);

    *ਸਰੋਤ=ਦਲੀਲਾਂ_2.ਸਰੋਤ;
    *ਨਿਸ਼ਾਨਾ=ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

ਸਥਿਰ ਖਾਲੀ
ਮੰਜ਼ਿਲ_Unicode_With_Callback(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *err) {
    UConverter_ਮੰਜ਼ਿਲ_Unicode ਮੰਜ਼ਿਲ_Unicode;
    UConverter *cnv;
    ਅਚਲ ਅੱਖਰ *s;
    char16_t *t;
    int32_t *offsets;
    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    int32_t error_ਨਿਵੇਸ਼_ਲੰਮਾਈ;
    UBool converter_Saw_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼, calledCallback;

    /* variables for m:n conversion */
    ਅੱਖਰ replay[UCNV_EXT_MAX_ਬਾਈਟਾਂ];
    ਅਚਲ ਅੱਖਰ *real_ਸਰੋਤ, *real_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    int32_t real_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    UBool realFlush;

    cnv=p_ਦਲੀਲਾਂ->converter;
    s=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    t=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* get the converter implementation function */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;
    ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
        ਮੰਜ਼ਿਲ_Unicode=cnv->shared_ਅੰਕੜੇ->impl->ਮੰਜ਼ਿਲ_Unicode;
    } ਨਹੀਂਤਾਂ {
        ਮੰਜ਼ਿਲ_Unicode=cnv->shared_ਅੰਕੜੇ->impl->ਮੰਜ਼ਿਲ_Unicode_With_Offsets;
        ਜੇ(ਮੰਜ਼ਿਲ_Unicode==ਖਾਲੀ_ਸੂਚਕ) {
            /* there is no WithOffsets implementation */
            ਮੰਜ਼ਿਲ_Unicode=cnv->shared_ਅੰਕੜੇ->impl->ਮੰਜ਼ਿਲ_Unicode;
            /* we will write -1 for each offset */
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;
        }
    }

    ਜੇ(cnv->pre_ਮੰਜ਼ਿਲ_ULength>=0) {
        /* normal mode */
        real_ਸਰੋਤ=ਖਾਲੀ_ਸੂਚਕ;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਖਾਲੀ_ਸੂਚਕ;
        realFlush=ਝੂਠ;
        real_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;
    } ਨਹੀਂਤਾਂ {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        ਜੇ (!p_ਦਲੀਲਾਂ->ਸਰੋਤ) {
            *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
        real_ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
        real_ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
        realFlush=p_ਦਲੀਲਾਂ->flush;
        real_ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਸਰੋਤ_ਸੂਚਕਾਂਕ;

        uprv_memcpy(replay, cnv->pre_ਮੰਜ਼ਿਲ_U, -cnv->pre_ਮੰਜ਼ਿਲ_ULength);
        p_ਦਲੀਲਾਂ->ਸਰੋਤ=replay;
        p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=replay-cnv->pre_ਮੰਜ਼ਿਲ_ULength;
        p_ਦਲੀਲਾਂ->flush=ਝੂਠ;
        ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;

        cnv->pre_ਮੰਜ਼ਿਲ_ULength=0;
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
    ਲਈ(;;) {
        ਜੇ(U_SUCCESS(*err)) {
            /* convert */
            ਮੰਜ਼ਿਲ_Unicode(p_ਦਲੀਲਾਂ, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_ਦਲੀਲਾਂ->flush && p_ਦਲੀਲਾਂ->ਸਰੋਤ==p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ &&
                        cnv->ਮੰਜ਼ਿਲ_ULength==0);
        } ਨਹੀਂਤਾਂ {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼=ਝੂਠ;
        }

        /* no callback called yet for this iteration */
        calledCallback=ਝੂਠ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_ਨਿਵੇਸ਼_ਲੰਮਾਈ=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        ਲਈ(;;) {
            /* update offsets if we write any */
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                int32_t length = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ - t);
                ਜੇ(length>0) {
                    _updateOffsets(offsets, length, ਸਰੋਤ_ਸੂਚਕਾਂਕ, error_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_ਦਲੀਲਾਂ->offsets=offsets+=length;
                }

                ਜੇ(ਸਰੋਤ_ਸੂਚਕਾਂਕ>=0) {
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ += static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਸਰੋਤ - s);
                }
            }

            ਜੇ(cnv->pre_ਮੰਜ਼ਿਲ_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                ਜੇ(real_ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ) {
                    real_ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
                    real_ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
                    realFlush=p_ਦਲੀਲਾਂ->flush;
                    real_ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਸਰੋਤ_ਸੂਚਕਾਂਕ;

                    uprv_memcpy(replay, cnv->pre_ਮੰਜ਼ਿਲ_U, -cnv->pre_ਮੰਜ਼ਿਲ_ULength);
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=replay;
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=replay-cnv->pre_ਮੰਜ਼ਿਲ_ULength;
                    p_ਦਲੀਲਾਂ->flush=ਝੂਠ;
                    ਜੇ((ਸਰੋਤ_ਸੂਚਕਾਂਕ+=cnv->pre_ਮੰਜ਼ਿਲ_ULength)<0) {
                        ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;
                    }

                    cnv->pre_ਮੰਜ਼ਿਲ_ULength=0;
                } ਨਹੀਂਤਾਂ {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ);
                    *err=U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
                }
            }

            /* update pointers */
            s=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
            t=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;

            ਜੇ(U_SUCCESS(*err)) {
                ਜੇ(s<p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(real_ਸਰੋਤ!=ਖਾਲੀ_ਸੂਚਕ) {
                    /* switch back from replaying to the real source and continue */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=real_ਸਰੋਤ;
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=real_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
                    p_ਦਲੀਲਾਂ->flush=realFlush;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=real_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

                    real_ਸਰੋਤ=ਖਾਲੀ_ਸੂਚਕ;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(p_ਦਲੀਲਾਂ->flush && cnv->ਮੰਜ਼ਿਲ_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_ਮਿਲਿਆ;
                    calledCallback=ਝੂਠ; /* new error condition */
                } ਨਹੀਂਤਾਂ {
                    /* input consumed */
                    ਜੇ(p_ਦਲੀਲਾਂ->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        ਜੇ(!converter_Saw_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼) {
                            ਰੋਕੋ;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE, ਝੂਠ);
                    }

                    /* done successfully */
                    ਵਾਪਸ;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_ਕੋਡ e;

                ਜੇ( calledCallback ||
                    (e=*err)==U_ਬਫਰ_ਉਛਾਲ_ERROR ||
                    (e!=U_INVALID_CHAR_ਮਿਲਿਆ &&
                     e!=U_ILLEGAL_CHAR_ਮਿਲਿਆ &&
                     e!=U_TRUNCATED_CHAR_ਮਿਲਿਆ &&
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
                    ਜੇ(real_ਸਰੋਤ!=ਖਾਲੀ_ਸੂਚਕ) {
                        int32_t length;

                        U_ASSERT(cnv->pre_ਮੰਜ਼ਿਲ_ULength==0);

                        length = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ - p_ਦਲੀਲਾਂ->ਸਰੋਤ);
                        ਜੇ(length>0) {
                            uprv_memcpy(cnv->pre_ਮੰਜ਼ਿਲ_U, p_ਦਲੀਲਾਂ->ਸਰੋਤ, length);
                            cnv->pre_ਮੰਜ਼ਿਲ_ULength = static_cast<int8_t>(-length);
                        }

                        p_ਦਲੀਲਾਂ->ਸਰੋਤ=real_ਸਰੋਤ;
                        p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ=real_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
                        p_ਦਲੀਲਾਂ->flush=realFlush;
                    }

                    ਵਾਪਸ;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_ਨਿਵੇਸ਼_ਲੰਮਾਈ=cnv->invalid_Char_ਲੰਮਾਈ=cnv->ਮੰਜ਼ਿਲ_ULength;
            ਜੇ(error_ਨਿਵੇਸ਼_ਲੰਮਾਈ>0) {
                uprv_memcpy(cnv->invalid_Char_ਬਫਰ, cnv->ਮੰਜ਼ਿਲ_UBytes, error_ਨਿਵੇਸ਼_ਲੰਮਾਈ);
            }

            /* set the converter state to deal with the next character */
            cnv->ਮੰਜ਼ਿਲ_ULength=0;

            /* call the callback function */
            ਜੇ(cnv->ਮੰਜ਼ਿਲ_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_ਮਿਲਿਆ) {
                cnv->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->ਮੰਜ਼ਿਲ_UContext, p_ਦਲੀਲਾਂ,
                cnv->invalid_Char_ਬਫਰ, error_ਨਿਵੇਸ਼_ਲੰਮਾਈ,
                cnv->ਮੰਜ਼ਿਲ_UCallback_Reason,
                err);
            cnv->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_ILLEGAL; /* reset to default value */

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=ਸੱਚ;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
ਸਥਿਰ UBool
ucnv_output_ਉਛਾਲ_ਮੰਜ਼ਿਲ_Unicode(UConverter *cnv,
                             char16_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             int32_t **pOffsets,
                             UError_ਕੋਡ *err) {
    int32_t *offsets;
    char16_t *ਉਛਾਲ, *t;
    int32_t i, length;

    t=*ਨਿਸ਼ਾਨਾ;
    ਜੇ(pOffsets!=ਖਾਲੀ_ਸੂਚਕ) {
        offsets=*pOffsets;
    } ਨਹੀਂਤਾਂ {
        offsets=ਖਾਲੀ_ਸੂਚਕ;
    }

    ਉਛਾਲ=cnv->UChar_Error_ਬਫਰ;
    length=cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ;
    i=0;
    ਜਦੋਂ(i<length) {
        ਜੇ(t==ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            ਕਰੋ {
                ਉਛਾਲ[j++]=ਉਛਾਲ[i++];
            } ਜਦੋਂ(i<length);

            cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = static_cast<int8_t>(j);
            *ਨਿਸ਼ਾਨਾ=t;
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *pOffsets=offsets;
            }
            *err=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ ਸੱਚ;
        }

        /* copy the overflow contents to the target */
        *t++=ਉਛਾਲ[i++];
        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=0;
    *ਨਿਸ਼ਾਨਾ=t;
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
        *pOffsets=offsets;
    }
    ਵਾਪਸ ਝੂਠ;
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_ਮੰਜ਼ਿਲ_Unicode(UConverter *cnv,
               char16_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
               ਅਚਲ ਅੱਖਰ **ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ,
               int32_t *offsets,
               UBool flush,
               UError_ਕੋਡ *err) {
    UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ ਦਲੀਲਾਂ_2;
    ਅਚਲ ਅੱਖਰ *s;
    char16_t *t;

    /* check parameters */
    ਜੇ(err==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err)) {
        ਵਾਪਸ;
    }

    ਜੇ(cnv==ਖਾਲੀ_ਸੂਚਕ || ਨਿਸ਼ਾਨਾ==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ) {
        *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    s=*ਸਰੋਤ;
    t=*ਨਿਸ਼ਾਨਾ;

    ਜੇ ((ਅਚਲ ਖਾਲੀ *)U_MAX_PTR(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) == (ਅਚਲ ਖਾਲੀ *)ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (ਅਚਲ char16_t *)(((ਅਚਲ ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) - 1);
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
    ਜੇ (ਸਰੋਤ_ਹੱਦਬੰਦੀ<s || ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ<t ||
        ((size_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-s)>(size_t)0x7fffffff && ਸਰੋਤ_ਹੱਦਬੰਦੀ>s) ||
        ((size_t)(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-t)>(size_t)0x3fffffff && ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ>t) ||
        (((ਅਚਲ ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-(ਅਚਲ ਅੱਖਰ *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    
    /* output the target overflow buffer */
    ਜੇ( cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ>0 &&
        ucnv_output_ਉਛਾਲ_ਮੰਜ਼ਿਲ_Unicode(cnv, ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        ਵਾਪਸ;
    }
    /* *target may have moved, therefore stop using t */

    ਜੇ(!flush && s==ਸਰੋਤ_ਹੱਦਬੰਦੀ && cnv->pre_ਮੰਜ਼ਿਲ_ULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        ਵਾਪਸ;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    ਦਲੀਲਾਂ_2.converter=cnv;
    ਦਲੀਲਾਂ_2.flush=flush;
    ਦਲੀਲਾਂ_2.offsets=offsets;
    ਦਲੀਲਾਂ_2.ਸਰੋਤ=s;
    ਦਲੀਲਾਂ_2.ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ=*ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    ਦਲੀਲਾਂ_2.size=ਆਕਾਰ(ਦਲੀਲਾਂ_2);

    ਮੰਜ਼ਿਲ_Unicode_With_Callback(&ਦਲੀਲਾਂ_2, err);

    *ਸਰੋਤ=ਦਲੀਲਾਂ_2.ਸਰੋਤ;
    *ਨਿਸ਼ਾਨਾ=ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ char16_t *src_ਹੱਦਬੰਦੀ;
    ਅੱਖਰ *originalDest, *dest_ਹੱਦਬੰਦੀ;
    int32_t dest_ਲੰਮਾਈ;

    /* check arguments */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ( cnv==ਖਾਲੀ_ਸੂਚਕ ||
        dest_ਸਮਰੱਥਾ<0 || (dest_ਸਮਰੱਥਾ>0 && dest==ਖਾਲੀ_ਸੂਚਕ) ||
        src_ਲੰਮਾਈ<-1 || (src_ਲੰਮਾਈ!=0 && src==ਖਾਲੀ_ਸੂਚਕ)
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    ਜੇ(src_ਲੰਮਾਈ==-1) {
        src_ਲੰਮਾਈ=u_strlen(src);
    }
    ਜੇ(src_ਲੰਮਾਈ>0) {
        src_ਹੱਦਬੰਦੀ=src+src_ਲੰਮਾਈ;
        dest_ਸਮਰੱਥਾ=pin_ਸਮਰੱਥਾ(dest, dest_ਸਮਰੱਥਾ);
        dest_ਹੱਦਬੰਦੀ=dest+dest_ਸਮਰੱਥਾ;

        /* perform the conversion */
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_ਹੱਦਬੰਦੀ, &src, src_ਹੱਦਬੰਦੀ, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ, &ਬਫਰ_ਸਥਿਤੀ);
        dest_ਲੰਮਾਈ=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        ਜੇ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
            ਅੱਖਰ ਬਫਰ[1024];

            dest_ਹੱਦਬੰਦੀ=ਬਫਰ+ਆਕਾਰ(ਬਫਰ);
            ਕਰੋ {
                dest=ਬਫਰ;
                ਬਫਰ_ਸਥਿਤੀ=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_ਹੱਦਬੰਦੀ, &src, src_ਹੱਦਬੰਦੀ, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ, &ਬਫਰ_ਸਥਿਤੀ);
                dest_ਲੰਮਾਈ+=(int32_t)(dest-ਬਫਰ);
            } ਜਦੋਂ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR);
        }
        ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
            *p_Error_ਕੋਡ = ਬਫਰ_ਸਥਿਤੀ;
        }
    } ਨਹੀਂਤਾਂ {
        dest_ਲੰਮਾਈ=0;
    }

    ਵਾਪਸ u_terminateChars(originalDest, dest_ਸਮਰੱਥਾ, dest_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
ucnv_ਮੰਜ਼ਿਲ_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
              ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ ਅੱਖਰ *src_ਹੱਦਬੰਦੀ;
    char16_t *originalDest, *dest_ਹੱਦਬੰਦੀ;
    int32_t dest_ਲੰਮਾਈ;

    /* check arguments */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ( cnv==ਖਾਲੀ_ਸੂਚਕ ||
        dest_ਸਮਰੱਥਾ<0 || (dest_ਸਮਰੱਥਾ>0 && dest==ਖਾਲੀ_ਸੂਚਕ) ||
        src_ਲੰਮਾਈ<-1 || (src_ਲੰਮਾਈ!=0 && src==ਖਾਲੀ_ਸੂਚਕ))
    {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* initialize */
    ucnv_reset_ਮੰਜ਼ਿਲ_Unicode(cnv);
    originalDest=dest;
    ਜੇ(src_ਲੰਮਾਈ==-1) {
        src_ਲੰਮਾਈ=(int32_t)uprv_strlen(src);
    }
    ਜੇ(src_ਲੰਮਾਈ>0) {
        src_ਹੱਦਬੰਦੀ=src+src_ਲੰਮਾਈ;
        dest_ਸਮਰੱਥਾ=pin_ਸਮਰੱਥਾ(dest, dest_ਸਮਰੱਥਾ);
        dest_ਹੱਦਬੰਦੀ=dest+dest_ਸਮਰੱਥਾ;

        /* perform the conversion */
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
        ucnv_ਮੰਜ਼ਿਲ_Unicode(cnv, &dest, dest_ਹੱਦਬੰਦੀ, &src, src_ਹੱਦਬੰਦੀ, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ, &ਬਫਰ_ਸਥਿਤੀ);
        dest_ਲੰਮਾਈ=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        ਜੇ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR)
        {
            char16_t ਬਫਰ[1024];

            dest_ਹੱਦਬੰਦੀ=ਬਫਰ+UPRV_LENGTHOF(ਬਫਰ);
            ਕਰੋ {
                dest=ਬਫਰ;
                ਬਫਰ_ਸਥਿਤੀ=U_ZERO_ERROR;
                ucnv_ਮੰਜ਼ਿਲ_Unicode(cnv, &dest, dest_ਹੱਦਬੰਦੀ, &src, src_ਹੱਦਬੰਦੀ, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ, &ਬਫਰ_ਸਥਿਤੀ);
                dest_ਲੰਮਾਈ+=(int32_t)(dest-ਬਫਰ);
            }
            ਜਦੋਂ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR);
        }
        ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
            *p_Error_ਕੋਡ = ਬਫਰ_ਸਥਿਤੀ;
        }
    } ਨਹੀਂਤਾਂ {
        dest_ਲੰਮਾਈ=0;
    }

    ਵਾਪਸ u_terminateUChars(originalDest, dest_ਸਮਰੱਥਾ, dest_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_ਅਗਲਾ_UChar(UConverter *cnv,
                  ਅਚਲ ਅੱਖਰ **ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                  UError_ਕੋਡ *err) {
    UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ ਦਲੀਲਾਂ_2;
    char16_t ਬਫਰ[U16_MAX_ਲੰਮਾਈ];
    ਅਚਲ ਅੱਖਰ *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    ਜੇ(err==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err)) {
        ਵਾਪਸ 0xffff;
    }

    ਜੇ(cnv==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ) {
        *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0xffff;
    }

    s=*ਸਰੋਤ;
    ਜੇ(ਸਰੋਤ_ਹੱਦਬੰਦੀ<s) {
        *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0xffff;
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
    ਜੇ(((size_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-s)>(size_t)0x7fffffff && ਸਰੋਤ_ਹੱਦਬੰਦੀ>s)) {
        *err=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    ਜੇ(cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ>0) {
        char16_t *ਉਛਾਲ;

        ਉਛਾਲ=cnv->UChar_Error_ਬਫਰ;
        i=0;
        length=cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ;
        U16_ਅਗਲਾ(ਉਛਾਲ, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        ਜੇ((cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_ਬਫਰ, cnv->UChar_Error_ਬਫਰ+i,
                         cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ*U_SIZEOF_UCHAR);
        }

        ਜੇ(!U16_IS_LEAD(c) || i<length) {
            ਵਾਪਸ c;
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
    ਦਲੀਲਾਂ_2.converter=cnv;
    ਦਲੀਲਾਂ_2.flush=ਸੱਚ;
    ਦਲੀਲਾਂ_2.offsets=ਖਾਲੀ_ਸੂਚਕ;
    ਦਲੀਲਾਂ_2.ਸਰੋਤ=s;
    ਦਲੀਲਾਂ_2.ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ=ਬਫਰ;
    ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=ਬਫਰ+1;
    ਦਲੀਲਾਂ_2.size=ਆਕਾਰ(ਦਲੀਲਾਂ_2);

    ਜੇ(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        ਜੇ(cnv->ਮੰਜ਼ਿਲ_ULength==0 && cnv->shared_ਅੰਕੜੇ->impl->get_ਅਗਲਾ_UChar!=ਖਾਲੀ_ਸੂਚਕ) {
            c=cnv->shared_ਅੰਕੜੇ->impl->get_ਅਗਲਾ_UChar(&ਦਲੀਲਾਂ_2, err);
            *ਸਰੋਤ=s=ਦਲੀਲਾਂ_2.ਸਰੋਤ;
            ਜੇ(*err==U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE, ਝੂਠ);
                ਵਾਪਸ 0xffff; /* no output */
            } ਨਹੀਂਤਾਂ ਜੇ(U_SUCCESS(*err) && c>=0) {
                ਵਾਪਸ c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        ਮੰਜ਼ਿਲ_Unicode_With_Callback(&ਦਲੀਲਾਂ_2, err);

        ਜੇ(*err==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ-ਬਫਰ);
    } ਨਹੀਂਤਾਂ {
        /* write the lead surrogate from the overflow buffer */
        ਬਫਰ[0]=(char16_t)c;
        ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ=ਬਫਰ+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    ਜੇ(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } ਨਹੀਂਤਾਂ ਜੇ(length==0) {
        /* no input or only state changes */
        *err=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } ਨਹੀਂਤਾਂ {
        c=ਬਫਰ[0];
        i=1;
        ਜੇ(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } ਨਹੀਂਤਾਂ {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            ਜੇ(cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ>0) {
                /* got overflow output from the conversion */
                ਜੇ(U16_IS_TRAIL(c2=cnv->UChar_Error_ਬਫਰ[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    ਜੇ((--cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ)>0) {
                        uprv_memmove(cnv->UChar_Error_ਬਫਰ, cnv->UChar_Error_ਬਫਰ+1,
                                     cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ*U_SIZEOF_UCHAR);
                    }
                } ਨਹੀਂਤਾਂ {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ਦਲੀਲਾਂ_2.ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                /* convert once more, to buffer[1] */
                ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=ਬਫਰ+2;
                ਮੰਜ਼ਿਲ_Unicode_With_Callback(&ਦਲੀਲਾਂ_2, err);
                ਜੇ(*err==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(ਦਲੀਲਾਂ_2.ਨਿਸ਼ਾਨਾ-ਬਫਰ);
                ਜੇ(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=ਬਫਰ[1])) {
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
    ਜੇ(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        ਜੇ((length=cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ)>0) {
            uprv_memmove(cnv->UChar_Error_ਬਫਰ+delta, cnv->UChar_Error_ਬਫਰ,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=(int8_t)(length+delta);

        cnv->UChar_Error_ਬਫਰ[0]=ਬਫਰ[i++];
        ਜੇ(delta>1) {
            cnv->UChar_Error_ਬਫਰ[1]=ਬਫਰ[i];
        }
    }

    *ਸਰੋਤ=ਦਲੀਲਾਂ_2.ਸਰੋਤ;
    ਵਾਪਸ c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_convertEx(UConverter *ਨਿਸ਼ਾਨਾ_Cnv, UConverter *ਸਰੋਤ_Cnv,
               ਅੱਖਰ **ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
               ਅਚਲ ਅੱਖਰ **ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ,
               char16_t *ਧੁਰਾ_ਸ਼ੁਰੂ, char16_t **ਧੁਰਾ_ਸਰੋਤ,
               char16_t **ਧੁਰਾ_ਨਿਸ਼ਾਨਾ, ਅਚਲ char16_t *ਧੁਰਾ_ਹੱਦਬੰਦੀ,
               UBool reset, UBool flush,
               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    char16_t ਧੁਰਾ_ਬਫਰ[CHUNK_ਮਾਪ];
    ਅਚਲ char16_t *my_ਧੁਰਾ_ਸਰੋਤ;
    char16_t *my_ਧੁਰਾ_ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਅੱਖਰ *s;
    ਅੱਖਰ *t;

    UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ ਮੰਜ਼ਿਲ_UArgs;
    UConverter_From_Unicode_ਦਲੀਲਾਂ fromUArgs;
    UConverterConvert convert;

    /* error checking */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    ਜੇ( ਨਿਸ਼ਾਨਾ_Cnv==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ_Cnv==ਖਾਲੀ_ਸੂਚਕ ||
        ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ || *ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ ||
        ਨਿਸ਼ਾਨਾ==ਖਾਲੀ_ਸੂਚਕ || *ਨਿਸ਼ਾਨਾ==ਖਾਲੀ_ਸੂਚਕ || ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    s=*ਸਰੋਤ;
    t=*ਨਿਸ਼ਾਨਾ;
    ਜੇ((ਸਰੋਤ_ਹੱਦਬੰਦੀ!=ਖਾਲੀ_ਸੂਚਕ && ਸਰੋਤ_ਹੱਦਬੰਦੀ<s) || ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ<t) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    ਜੇ(
        (ਸਰੋਤ_ਹੱਦਬੰਦੀ!=ਖਾਲੀ_ਸੂਚਕ && ((size_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-s)>(size_t)0x7fffffff && ਸਰੋਤ_ਹੱਦਬੰਦੀ>s)) ||
        ((size_t)(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-t)>(size_t)0x7fffffff && ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ>t)
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    
    ਜੇ(ਧੁਰਾ_ਸ਼ੁਰੂ==ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }

        /* use the stack pivot buffer */
        my_ਧੁਰਾ_ਸਰੋਤ=my_ਧੁਰਾ_ਨਿਸ਼ਾਨਾ=ਧੁਰਾ_ਸ਼ੁਰੂ=ਧੁਰਾ_ਬਫਰ;
        ਧੁਰਾ_ਸਰੋਤ=(char16_t **)&my_ਧੁਰਾ_ਸਰੋਤ;
        ਧੁਰਾ_ਨਿਸ਼ਾਨਾ=&my_ਧੁਰਾ_ਨਿਸ਼ਾਨਾ;
        ਧੁਰਾ_ਹੱਦਬੰਦੀ=ਧੁਰਾ_ਬਫਰ+CHUNK_ਮਾਪ;
    } ਨਹੀਂਤਾਂ ਜੇ(  ਧੁਰਾ_ਸ਼ੁਰੂ>=ਧੁਰਾ_ਹੱਦਬੰਦੀ ||
                ਧੁਰਾ_ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ || *ਧੁਰਾ_ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ ||
                ਧੁਰਾ_ਨਿਸ਼ਾਨਾ==ਖਾਲੀ_ਸੂਚਕ || *ਧੁਰਾ_ਨਿਸ਼ਾਨਾ==ਖਾਲੀ_ਸੂਚਕ ||
                ਧੁਰਾ_ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ(ਸਰੋਤ_ਹੱਦਬੰਦੀ==ਖਾਲੀ_ਸੂਚਕ) {
        /* get limit of single-byte-NUL-terminated source string */
        ਸਰੋਤ_ਹੱਦਬੰਦੀ=uprv_strchr(*ਸਰੋਤ, 0);
    }

    ਜੇ(reset) {
        ucnv_reset_ਮੰਜ਼ਿਲ_Unicode(ਸਰੋਤ_Cnv);
        ucnv_resetFromUnicode(ਨਿਸ਼ਾਨਾ_Cnv);
        *ਧੁਰਾ_ਸਰੋਤ=*ਧੁਰਾ_ਨਿਸ਼ਾਨਾ=ਧੁਰਾ_ਸ਼ੁਰੂ;
    } ਨਹੀਂਤਾਂ ਜੇ(ਨਿਸ਼ਾਨਾ_Cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ>0) {
        /* output the targetCnv overflow buffer */
        ਜੇ(ucnv_output_ਉਛਾਲ_From_Unicode(ਨਿਸ਼ਾਨਾ_Cnv, ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            ਵਾਪਸ;
        }
        /* *target has moved, therefore stop using t */

        ਜੇ( !flush &&
            ਨਿਸ਼ਾਨਾ_Cnv->preFromULength>=0 && *ਧੁਰਾ_ਸਰੋਤ==*ਧੁਰਾ_ਨਿਸ਼ਾਨਾ &&
            ਸਰੋਤ_Cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ==0 && ਸਰੋਤ_Cnv->pre_ਮੰਜ਼ਿਲ_ULength>=0 && s==ਸਰੋਤ_ਹੱਦਬੰਦੀ
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            ਵਾਪਸ;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    ਜੇ( ਸਰੋਤ_Cnv->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->conversionType==UCNV_UTF8 &&
        ਨਿਸ਼ਾਨਾ_Cnv->shared_ਅੰਕੜੇ->impl->fromUTF8!=ਖਾਲੀ_ਸੂਚਕ
    ) {
        convert=ਨਿਸ਼ਾਨਾ_Cnv->shared_ਅੰਕੜੇ->impl->fromUTF8;
    } ਨਹੀਂਤਾਂ ਜੇ( ਨਿਸ਼ਾਨਾ_Cnv->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->conversionType==UCNV_UTF8 &&
               ਸਰੋਤ_Cnv->shared_ਅੰਕੜੇ->impl->ਮੰਜ਼ਿਲ_UTF8!=ਖਾਲੀ_ਸੂਚਕ
    ) {
        convert=ਸਰੋਤ_Cnv->shared_ਅੰਕੜੇ->impl->ਮੰਜ਼ਿਲ_UTF8;
    } ਨਹੀਂਤਾਂ {
        convert=ਖਾਲੀ_ਸੂਚਕ;
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
    ਜੇ(convert!=ਖਾਲੀ_ਸੂਚਕ && (ਧੁਰਾ_ਹੱਦਬੰਦੀ-ਧੁਰਾ_ਸ਼ੁਰੂ)>32) {
        ਧੁਰਾ_ਹੱਦਬੰਦੀ=ਧੁਰਾ_ਸ਼ੁਰੂ+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=ਨਿਸ਼ਾਨਾ_Cnv;
    fromUArgs.flush=ਝੂਠ;
    fromUArgs.offsets=ਖਾਲੀ_ਸੂਚਕ;
    fromUArgs.ਨਿਸ਼ਾਨਾ=*ਨਿਸ਼ਾਨਾ;
    fromUArgs.ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    fromUArgs.size=ਆਕਾਰ(fromUArgs);

    ਮੰਜ਼ਿਲ_UArgs.converter=ਸਰੋਤ_Cnv;
    ਮੰਜ਼ਿਲ_UArgs.flush=flush;
    ਮੰਜ਼ਿਲ_UArgs.offsets=ਖਾਲੀ_ਸੂਚਕ;
    ਮੰਜ਼ਿਲ_UArgs.ਸਰੋਤ=s;
    ਮੰਜ਼ਿਲ_UArgs.ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਮੰਜ਼ਿਲ_UArgs.ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=ਧੁਰਾ_ਹੱਦਬੰਦੀ;
    ਮੰਜ਼ਿਲ_UArgs.size=ਆਕਾਰ(ਮੰਜ਼ਿਲ_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=ਖਾਲੀ_ਸੂਚਕ;

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
    ਲਈ(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        ਜੇ( *ਧੁਰਾ_ਸਰੋਤ<*ਧੁਰਾ_ਨਿਸ਼ਾਨਾ ||
            U_FAILURE(*p_Error_ਕੋਡ) ||
            ਨਿਸ਼ਾਨਾ_Cnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.ਸਰੋਤ=*ਧੁਰਾ_ਸਰੋਤ;
            fromUArgs.ਸਰੋਤ_ਹੱਦਬੰਦੀ=*ਧੁਰਾ_ਨਿਸ਼ਾਨਾ;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_ਕੋਡ);
            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                /* target overflow, or conversion error */
                *ਧੁਰਾ_ਸਰੋਤ=(char16_t *)fromUArgs.ਸਰੋਤ;
                ਰੋਕੋ;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *ਧੁਰਾ_ਸਰੋਤ=*ਧੁਰਾ_ਨਿਸ਼ਾਨਾ=ਧੁਰਾ_ਸ਼ੁਰੂ;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        ਜੇ(ਸਰੋਤ_Cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ>0) {
            ਜੇ(ucnv_output_ਉਛਾਲ_ਮੰਜ਼ਿਲ_Unicode(ਸਰੋਤ_Cnv, ਧੁਰਾ_ਨਿਸ਼ਾਨਾ, ਧੁਰਾ_ਹੱਦਬੰਦੀ, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_ਕੋਡ=U_ZERO_ERROR;
            }
            ਜਾਰੀ;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        ਜੇ( ਮੰਜ਼ਿਲ_UArgs.ਸਰੋਤ==ਸਰੋਤ_ਹੱਦਬੰਦੀ &&
            ਸਰੋਤ_Cnv->pre_ਮੰਜ਼ਿਲ_ULength>=0 && ਸਰੋਤ_Cnv->ਮੰਜ਼ਿਲ_ULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            ਰੋਕੋ;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        ਜੇ(convert!=ਖਾਲੀ_ਸੂਚਕ && ਨਿਸ਼ਾਨਾ_Cnv->preFromUFirstCP<0 && ਸਰੋਤ_Cnv->pre_ਮੰਜ਼ਿਲ_ULength==0) {
            ਜੇ(*p_Error_ਕੋਡ==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_ਕੋਡ=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &ਮੰਜ਼ਿਲ_UArgs, p_Error_ਕੋਡ);
            ਜੇ(*p_Error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                ਜੇ(ਸਰੋਤ_Cnv->ਮੰਜ਼ਿਲ_ULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } ਨਹੀਂਤਾਂ {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *ਧੁਰਾ_ਸਰੋਤ=*ਧੁਰਾ_ਨਿਸ਼ਾਨਾ=ਧੁਰਾ_ਸ਼ੁਰੂ+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(*p_Error_ਕੋਡ==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_ਕੋਡ=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } ਨਹੀਂਤਾਂ ਜੇ(flush && ਸਰੋਤ_Cnv->ਮੰਜ਼ਿਲ_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_ਕੋਡ=U_TRUNCATED_CHAR_ਮਿਲਿਆ;
            } ਨਹੀਂਤਾਂ {
                /* input consumed */
                ਜੇ(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(ਸਰੋਤ_Cnv, UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE, ਝੂਠ);
                    _reset(ਨਿਸ਼ਾਨਾ_Cnv, UCNV_RESET_FROM_UNICODE, ਝੂਠ);
                }

                /* done successfully */
                ਰੋਕੋ;
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
        ਮੰਜ਼ਿਲ_UArgs.ਨਿਸ਼ਾਨਾ=ਧੁਰਾ_ਸ਼ੁਰੂ; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        ਮੰਜ਼ਿਲ_Unicode_With_Callback(&ਮੰਜ਼ਿਲ_UArgs, p_Error_ਕੋਡ);
        *ਧੁਰਾ_ਨਿਸ਼ਾਨਾ=ਮੰਜ਼ਿਲ_UArgs.ਨਿਸ਼ਾਨਾ;
        ਜੇ(*p_Error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_ਕੋਡ=U_ZERO_ERROR;
        } ਨਹੀਂਤਾਂ ਜੇ(U_FAILURE(*p_Error_ਕੋਡ) || (!flush && *ਧੁਰਾ_ਨਿਸ਼ਾਨਾ==ਧੁਰਾ_ਸ਼ੁਰੂ)) {
            /* conversion error, or there was nothing left to convert */
            ਰੋਕੋ;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        ਜੇ( flush && ਮੰਜ਼ਿਲ_UArgs.ਸਰੋਤ==ਸਰੋਤ_ਹੱਦਬੰਦੀ &&
            ਸਰੋਤ_Cnv->pre_ਮੰਜ਼ਿਲ_ULength>=0 &&
            ਸਰੋਤ_Cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ==0
        ) {
            fromUArgs.flush=ਸੱਚ;
        }
    }

    /*
     * The conversion loop is exited when one of the following is true:
     * - the entire source text has been converted successfully to the target buffer
     * - a target buffer overflow occurred
     * - a conversion error occurred
     */

    *ਸਰੋਤ=ਮੰਜ਼ਿਲ_UArgs.ਸਰੋਤ;
    *ਨਿਸ਼ਾਨਾ=fromUArgs.ਨਿਸ਼ਾਨਾ;

    /* terminate the target buffer if possible */
    ਜੇ(flush && U_SUCCESS(*p_Error_ਕੋਡ)) {
        ਜੇ(*ਨਿਸ਼ਾਨਾ!=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            **ਨਿਸ਼ਾਨਾ=0;
            ਜੇ(*p_Error_ਕੋਡ==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_ਕੋਡ=U_ZERO_ERROR;
            }
        } ਨਹੀਂਤਾਂ {
            *p_Error_ਕੋਡ=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
ਸਥਿਰ int32_t
ucnv_ਅੰਦਰੂਨੀ_Convert(UConverter *ਬਾਹਰਲਾ_Converter, UConverter *ਵਿੱਚ_Converter,
                     ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                     ਅਚਲ ਅੱਖਰ *ਸਰੋਤ, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    char16_t ਧੁਰਾ_ਬਫਰ[CHUNK_ਮਾਪ];
    char16_t *ਧੁਰਾ, *pivot2;

    ਅੱਖਰ *my_ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ=0;

    /* set up */
    ਜੇ(ਸਰੋਤ_ਲੰਮਾਈ<0) {
        ਸਰੋਤ_ਹੱਦਬੰਦੀ=uprv_strchr(ਸਰੋਤ, 0);
    } ਨਹੀਂਤਾਂ {
        ਸਰੋਤ_ਹੱਦਬੰਦੀ=ਸਰੋਤ+ਸਰੋਤ_ਲੰਮਾਈ;
    }

    /* if there is no input data, we're done */
    ਜੇ(ਸਰੋਤ==ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ u_terminateChars(ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, 0, p_Error_ਕੋਡ);
    }

    ਧੁਰਾ=pivot2=ਧੁਰਾ_ਬਫਰ;
    my_ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ=0;

    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        /* perform real conversion */
        ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=ਨਿਸ਼ਾਨਾ+ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        ucnv_convertEx(ਬਾਹਰਲਾ_Converter, ਵਿੱਚ_Converter,
                       &my_ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                       &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                       ਧੁਰਾ_ਬਫਰ, &ਧੁਰਾ, &pivot2, ਧੁਰਾ_ਬਫਰ+CHUNK_ਮਾਪ,
                       ਝੂਠ,
                       ਸੱਚ,
                       p_Error_ਕੋਡ);
        ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ = static_cast<int32_t>(my_ਨਿਸ਼ਾਨਾ - ਨਿਸ਼ਾਨਾ);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    ਜੇ(*p_Error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR || ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ==0)
    {
        ਅੱਖਰ ਨਿਸ਼ਾਨਾ_ਬਫਰ[CHUNK_ਮਾਪ];

        ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=ਨਿਸ਼ਾਨਾ_ਬਫਰ+CHUNK_ਮਾਪ;
        ਕਰੋ {
            *p_Error_ਕੋਡ=U_ZERO_ERROR;
            my_ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ_ਬਫਰ;
            ucnv_convertEx(ਬਾਹਰਲਾ_Converter, ਵਿੱਚ_Converter,
                           &my_ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                           &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                           ਧੁਰਾ_ਬਫਰ, &ਧੁਰਾ, &pivot2, ਧੁਰਾ_ਬਫਰ+CHUNK_ਮਾਪ,
                           ਝੂਠ,
                           ਸੱਚ,
                           p_Error_ਕੋਡ);
            ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ += static_cast<int32_t>(my_ਨਿਸ਼ਾਨਾ - ਨਿਸ਼ਾਨਾ_ਬਫਰ);
        } ਜਦੋਂ(*p_Error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        ਵਾਪਸ u_terminateChars(ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    ਵਾਪਸ ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ਅਚਲ ਅੱਖਰ *ਮੰਜ਼ਿਲ_Converter_Name, ਅਚਲ ਅੱਖਰ *fromConverterName,
             ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
             ਅਚਲ ਅੱਖਰ *ਸਰੋਤ, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter in, out; /* stack-allocated */
    UConverter *ਵਿੱਚ_Converter, *ਬਾਹਰਲਾ_Converter;
    int32_t ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ( ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ_ਲੰਮਾਈ<-1 ||
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ<0 || (ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0 && ਨਿਸ਼ਾਨਾ==ਖਾਲੀ_ਸੂਚਕ)
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* if there is no input data, we're done */
    ਜੇ(ਸਰੋਤ_ਲੰਮਾਈ==0 || (ਸਰੋਤ_ਲੰਮਾਈ<0 && *ਸਰੋਤ==0)) {
        ਵਾਪਸ u_terminateChars(ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, 0, p_Error_ਕੋਡ);
    }

    /* create the converters */
    ਵਿੱਚ_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਬਾਹਰਲਾ_Converter=ucnv_createConverter(&out, ਮੰਜ਼ਿਲ_Converter_Name, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ucnv_close(ਵਿੱਚ_Converter);
        ਵਾਪਸ 0;
    }

    ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ=ucnv_ਅੰਦਰੂਨੀ_Convert(ਬਾਹਰਲਾ_Converter, ਵਿੱਚ_Converter,
                                      ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                                      ਸਰੋਤ, ਸਰੋਤ_ਲੰਮਾਈ,
                                      p_Error_ਕੋਡ);

    ucnv_close(ਵਿੱਚ_Converter);
    ucnv_close(ਬਾਹਰਲਾ_Converter);

    ਵਾਪਸ ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ;
}

/* @internal */
ਸਥਿਰ int32_t
ucnv_convertAlgorithmic(UBool convert_ਮੰਜ਼ਿਲ_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                        ਅਚਲ ਅੱਖਰ *ਸਰੋਤ, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
                        UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *ਮੰਜ਼ਿਲ, *from;
    int32_t ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ( cnv==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ_ਲੰਮਾਈ<-1 ||
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ<0 || (ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0 && ਨਿਸ਼ਾਨਾ==ਖਾਲੀ_ਸੂਚਕ)
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* if there is no input data, we're done */
    ਜੇ(ਸਰੋਤ_ਲੰਮਾਈ==0 || (ਸਰੋਤ_ਲੰਮਾਈ<0 && *ਸਰੋਤ==0)) {
        ਵਾਪਸ u_terminateChars(ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, 0, p_Error_ਕੋਡ);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* reset the other converter */
    ਜੇ(convert_ਮੰਜ਼ਿਲ_Algorithmic) {
        /* cnv->Unicode->algo */
        ucnv_reset_ਮੰਜ਼ਿਲ_Unicode(cnv);
        ਮੰਜ਼ਿਲ=algoConverter;
        from=cnv;
    } ਨਹੀਂਤਾਂ {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        ਮੰਜ਼ਿਲ=cnv;
    }

    ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ=ucnv_ਅੰਦਰੂਨੀ_Convert(ਮੰਜ਼ਿਲ, from,
                                      ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                                      ਸਰੋਤ, ਸਰੋਤ_ਲੰਮਾਈ,
                                      p_Error_ਕੋਡ);

    ucnv_close(algoConverter);

    ਵਾਪਸ ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ;
}

U_CAPI int32_t U_EXPORT2
ucnv_ਮੰਜ਼ਿਲ_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                   ਅਚਲ ਅੱਖਰ *ਸਰੋਤ, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ ucnv_convertAlgorithmic(ਸੱਚ, algorithmicType, cnv,
                                   ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                                   ਸਰੋਤ, ਸਰੋਤ_ਲੰਮਾਈ,
                                   p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                     ਅਚਲ ਅੱਖਰ *ਸਰੋਤ, int32_t ਸਰੋਤ_ਲੰਮਾਈ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) UPRV_NO_SANITIZE_UNDEFINED {

    ਜੇ(algorithmicType<0 || UCNV_ਸੰਖਿਆ_ਦਾ_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ ucnv_convertAlgorithmic(ਝੂਠ, algorithmicType, cnv,
                                   ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                                   ਸਰੋਤ, ਸਰੋਤ_ਲੰਮਾਈ,
                                   p_Error_ਕੋਡ);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ਅਚਲ UConverter* converter)
{
    int8_t type = converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->conversionType;
#ਜੇ !UCONFIG_NO_LEGACY_CONVERSION
    ਜੇ(type == UCNV_MBCS) {
        ਵਾਪਸ ucnv_MBCSGetType(converter);
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ (UConverterType)type;
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_getStarters(ਅਚਲ UConverter* converter, 
                 UBool starters[256],
                 UError_ਕੋਡ* err)
{
    ਜੇ (err == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err)) {
        ਵਾਪਸ;
    }

    ਜੇ(converter->shared_ਅੰਕੜੇ->impl->getStarters != ਖਾਲੀ_ਸੂਚਕ) {
        converter->shared_ਅੰਕੜੇ->impl->getStarters(converter, starters, err);
    } ਨਹੀਂਤਾਂ {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

ਸਥਿਰ ਅਚਲ UAmbiguousConverter *ucnv_getAmbiguous(ਅਚਲ UConverter *cnv)
{
    UError_ਕੋਡ error_ਕੋਡ;
    ਅਚਲ ਅੱਖਰ *name;
    int32_t i;

    ਜੇ(cnv==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    error_ਕੋਡ=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਲਈ(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        ਜੇ(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            ਵਾਪਸ ambiguousConverters+i;
        }
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_fixFileSeparator(ਅਚਲ UConverter *cnv, 
                      char16_t* ਸਰੋਤ,
                      int32_t ਸਰੋਤ_ਲੰਮਾਈ) {
    ਅਚਲ UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    ਜੇ(cnv==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ==ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ_ਲੰਮਾਈ<=0 || (a=ucnv_getAmbiguous(cnv))==ਖਾਲੀ_ਸੂਚਕ)
    {
        ਵਾਪਸ;
    }

    variant5c=a->variant5c;
    ਲਈ(i=0; i<ਸਰੋਤ_ਲੰਮਾਈ; ++i) {
        ਜੇ(ਸਰੋਤ[i]==variant5c) {
            ਸਰੋਤ[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ਅਚਲ UConverter *cnv) {
    ਵਾਪਸ ucnv_getAmbiguous(cnv)!=ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_ਸੈੱਟ_Fallback(UConverter *cnv, UBool ਵਰਤਦਾ_Fallback)
{
    cnv->useFallback = ਵਰਤਦਾ_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_ਵਰਤਦਾ_Fallback(ਅਚਲ UConverter *cnv)
{
    ਵਾਪਸ cnv->useFallback;
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_getInvalidChars (ਅਚਲ UConverter * converter,
                      ਅੱਖਰ *err_ਬਾਈਟਾਂ,
                      int8_t * len,
                      UError_ਕੋਡ * err)
{
    ਜੇ (err == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err))
    {
        ਵਾਪਸ;
    }
    ਜੇ (len == ਖਾਲੀ_ਸੂਚਕ || err_ਬਾਈਟਾਂ == ਖਾਲੀ_ਸੂਚਕ || converter == ਖਾਲੀ_ਸੂਚਕ)
    {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (*len < converter->invalid_Char_ਲੰਮਾਈ)
    {
        *err = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ ((*len = converter->invalid_Char_ਲੰਮਾਈ) > 0)
    {
        uprv_memcpy (err_ਬਾਈਟਾਂ, converter->invalid_Char_ਬਫਰ, *len);
    }
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_getInvalidUChars (ਅਚਲ UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_ਕੋਡ * err)
{
    ਜੇ (err == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err))
    {
        ਵਾਪਸ;
    }
    ਜੇ (len == ਖਾਲੀ_ਸੂਚਕ || errChars == ਖਾਲੀ_ਸੂਚਕ || converter == ਖਾਲੀ_ਸੂਚਕ)
    {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (*len < converter->invalid_UChar_ਲੰਮਾਈ)
    {
        *err = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ ((*len = converter->invalid_UChar_ਲੰਮਾਈ) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_ਬਫਰ, *len);
    }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIG_MAX_LEN 5

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ucnv_detectUnicodeSignature( ਅਚਲ ਅੱਖਰ* ਸਰੋਤ,
                             int32_t ਸਰੋਤ_ਲੰਮਾਈ,
                             int32_t* signature_ਲੰਮਾਈ,
                             UError_ਕੋਡ* p_Error_ਕੋਡ) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    ਅੱਖਰ ਸ਼ੁਰੂ[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    ਪੂਰਨ i = 0;

    ਜੇ((p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ) || U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    
    ਜੇ(ਸਰੋਤ == ਖਾਲੀ_ਸੂਚਕ || ਸਰੋਤ_ਲੰਮਾਈ < -1){
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(signature_ਲੰਮਾਈ == ਖਾਲੀ_ਸੂਚਕ) {
        signature_ਲੰਮਾਈ = &dummy;
    }

    ਜੇ(ਸਰੋਤ_ਲੰਮਾਈ==-1){
        ਸਰੋਤ_ਲੰਮਾਈ=(int32_t)uprv_strlen(ਸਰੋਤ);
    }

    
    ਜਦੋਂ(i<ਸਰੋਤ_ਲੰਮਾਈ&& i<SIG_MAX_LEN){
        ਸ਼ੁਰੂ[i]=ਸਰੋਤ[i];
        i++;
    }

    ਜੇ(ਸ਼ੁਰੂ[0] == '\xFE' && ਸ਼ੁਰੂ[1] == '\xFF') {
        *signature_ਲੰਮਾਈ=2;
        ਵਾਪਸ  "UTF-16BE";
    } ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ[0] == '\xFF' && ਸ਼ੁਰੂ[1] == '\xFE') {
        ਜੇ(ਸ਼ੁਰੂ[2] == '\x00' && ਸ਼ੁਰੂ[3] =='\x00') {
            *signature_ਲੰਮਾਈ=4;
            ਵਾਪਸ "UTF-32LE";
        } ਨਹੀਂਤਾਂ {
            *signature_ਲੰਮਾਈ=2;
            ਵਾਪਸ  "UTF-16LE";
        }
    } ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ[0] == '\xEF' && ਸ਼ੁਰੂ[1] == '\xBB' && ਸ਼ੁਰੂ[2] == '\xBF') {
        *signature_ਲੰਮਾਈ=3;
        ਵਾਪਸ  "UTF-8";
    } ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ[0] == '\x00' && ਸ਼ੁਰੂ[1] == '\x00' && 
              ਸ਼ੁਰੂ[2] == '\xFE' && ਸ਼ੁਰੂ[3]=='\xFF') {
        *signature_ਲੰਮਾਈ=4;
        ਵਾਪਸ  "UTF-32BE";
    } ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ[0] == '\x0E' && ਸ਼ੁਰੂ[1] == '\xFE' && ਸ਼ੁਰੂ[2] == '\xFF') {
        *signature_ਲੰਮਾਈ=3;
        ਵਾਪਸ "SCSU";
    } ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ[0] == '\xFB' && ਸ਼ੁਰੂ[1] == '\xEE' && ਸ਼ੁਰੂ[2] == '\x28') {
        *signature_ਲੰਮਾਈ=3;
        ਵਾਪਸ "BOCU-1";
    } ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ[0] == '\x2B' && ਸ਼ੁਰੂ[1] == '\x2F' && ਸ਼ੁਰੂ[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        ਜੇ(ਸ਼ੁਰੂ[3] == '\x38' && ਸ਼ੁਰੂ[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_ਲੰਮਾਈ=5;
            ਵਾਪਸ "UTF-7";
        } ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ[3] == '\x38' || ਸ਼ੁਰੂ[3] == '\x39' || ਸ਼ੁਰੂ[3] == '\x2B' || ਸ਼ੁਰੂ[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_ਲੰਮਾਈ=4;
            ਵਾਪਸ "UTF-7";
        }
    }ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ[0]=='\xDD' && ਸ਼ੁਰੂ[1]== '\x73'&& ਸ਼ੁਰੂ[2]=='\x66' && ਸ਼ੁਰੂ[3]=='\x73'){
        *signature_ਲੰਮਾਈ=4;
        ਵਾਪਸ "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_ਲੰਮਾਈ=0;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ਅਚਲ UConverter* cnv, UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ -1;
    }
    ਜੇ(cnv == ਖਾਲੀ_ਸੂਚਕ){
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ -1;
    }

    ਜੇ(cnv->preFromUFirstCP >= 0){
        ਵਾਪਸ U16_ਲੰਮਾਈ(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }ਨਹੀਂਤਾਂ ਜੇ(cnv->preFromULength < 0){
        ਵਾਪਸ -cnv->preFromULength ;
    }ਨਹੀਂਤਾਂ ਜੇ(cnv->fromUChar32 > 0){
        ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_ਮੰਜ਼ਿਲ_UCount_Pending(ਅਚਲ UConverter* cnv, UError_ਕੋਡ* ਸਥਿਤੀ){

    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ -1;
    }
    ਜੇ(cnv == ਖਾਲੀ_ਸੂਚਕ){
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ -1;
    }

    ਜੇ(cnv->pre_ਮੰਜ਼ਿਲ_ULength > 0){
        ਵਾਪਸ cnv->pre_ਮੰਜ਼ਿਲ_ULength ;
    }ਨਹੀਂਤਾਂ ਜੇ(cnv->pre_ਮੰਜ਼ਿਲ_ULength < 0){
        ਵਾਪਸ -cnv->pre_ਮੰਜ਼ਿਲ_ULength;
    }ਨਹੀਂਤਾਂ ਜੇ(cnv->ਮੰਜ਼ਿਲ_ULength > 0){
        ਵਾਪਸ cnv->ਮੰਜ਼ਿਲ_ULength;
    }
    ਵਾਪਸ 0;
}

U_CAPI UBool U_EXPORT2
ucnv_is_Fixed_ਚੌੜਾਈ(UConverter *cnv, UError_ਕੋਡ *ਸਥਿਤੀ){
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ (cnv == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }

    ਚੋਣ (ucnv_getType(cnv)) {
        ਹਾਲਤ UCNV_SBCS:
        ਹਾਲਤ UCNV_DBCS:
        ਹਾਲਤ UCNV_UTF32_BigEndian:
        ਹਾਲਤ UCNV_UTF32_LittleEndian:
        ਹਾਲਤ UCNV_UTF32:
        ਹਾਲਤ UCNV_US_ASCII:
            ਵਾਪਸ ਸੱਚ;
        ਮੂਲ:
            ਵਾਪਸ ਝੂਠ;
    }
}
#ਜੇ_ਅੰਤ

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
