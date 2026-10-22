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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ <memory>

#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "ucnv_imp.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#ವ್ಯಾಖ್ಯಾನಿಸು CHUNK_ಗಾತ್ರ 1024

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UAmbiguousConverter {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
    ಸ್ಥಿರಾಂಕ char16_t variant5c;
} UAmbiguousConverter;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UAmbiguousConverter ambiguousConverters[]={
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
ucnv_open (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                       UError_ಸಂಕೇತ * err)
{
    UConverter *r;

    ಆದರೆ (err == ಶೂನ್ಯಸೂಚಕ || U_FAILURE (*err)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    r =  ucnv_createConverter(ಶೂನ್ಯಸೂಚಕ, name, err);
    ಹಿಂತಿರುಗಿ r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName, UError_ಸಂಕೇತ * err)
{
    ಹಿಂತಿರುಗಿ ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ಸ್ಥಿರಾಂಕ char16_t * name,
                         UError_ಸಂಕೇತ * err)
{
    ಅಕ್ಷರ asciiName[UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ];

    ಆದರೆ (err == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err))
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (name == ಶೂನ್ಯಸೂಚಕ)
        ಹಿಂತಿರುಗಿ ucnv_open (ಶೂನ್ಯಸೂಚಕ, err);
    ಆದರೆ (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ)
    {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
ಸ್ಥಿರ int32_t
ucnv_copyPlatformString(ಅಕ್ಷರ *platformString, UConverterPlatform pltfrm)
{
    ಆಯ್ಕೆ (pltfrm)
    {
    ಸ್ಥಿತಿ UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        ಹಿಂತಿರುಗಿ 4;
    ಸ್ಥಿತಿ UCNV_UNKNOWN:
        ನಿಲ್ಲಿಸು;
    }

    /* default to empty string */
    *platformString = 0;
    ಹಿಂತಿರುಗಿ 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UError_ಸಂಕೇತ * err)
{
    ಅಕ್ಷರ myName[UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ];
    int32_t myNameLen;

    ಆದರೆ (err == ಶೂನ್ಯಸೂಚಕ || U_FAILURE (*err))
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_ಗಮ್ಯ_String(myName + myNameLen, codepage, 10);

    ಹಿಂತಿರುಗಿ ucnv_createConverter(ಶೂನ್ಯಸೂಚಕ, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_ಸುರಕ್ಷಿತ_Clone(ಸ್ಥಿರಾಂಕ UConverter* cnv,
               ಶೂನ್ಯ* ರಾಶಿ_ಬಫರ್ U_LIFETIME_BOUND,
               int32_t* p_ಬಫರ್_ಗಾತ್ರ,
               UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ;
    int32_t ಬಫರ್_ಗಾತ್ರ_Needed;
    UError_ಸಂಕೇತ cbErr;
    UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು ಗಮ್ಯ_UArgs = {
        ಗಾತ್ರ(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು),
            ನಿಜ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ
    };
    UConverter_From_Unicode_ವಾದಗಳು fromUArgs = {
        ಗಾತ್ರ(UConverter_From_Unicode_ವಾದಗಳು),
            ನಿಜ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ
    };

    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_CLONE);

    ಆದರೆ (ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2? *ಸ್ಥಿತಿ_2: U_ILLEGAL_ವಾದ_ERROR);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (cnv == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        UTRACE_EXIT_ಸ್ಥಿತಿ(*ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, ಸ್ಥಿತಿ_2), cnv, ರಾಶಿ_ಬಫರ್);

    ಆದರೆ (cnv->shared_ದತ್ತಾಂಶ->impl->ಸುರಕ್ಷಿತ_Clone != ಶೂನ್ಯಸೂಚಕ) {
        /* call the custom safeClone function for sizing */
        ಬಫರ್_ಗಾತ್ರ_Needed = 0;
        cnv->shared_ದತ್ತಾಂಶ->impl->ಸುರಕ್ಷಿತ_Clone(cnv, ಶೂನ್ಯಸೂಚಕ, &ಬಫರ್_ಗಾತ್ರ_Needed, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            UTRACE_EXIT_ಸ್ಥಿತಿ(*ಸ್ಥಿತಿ_2);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        /* inherent sizing */
        ಬಫರ್_ಗಾತ್ರ_Needed = ಗಾತ್ರ(UConverter);
    }

    ಆದರೆ (p_ಬಫರ್_ಗಾತ್ರ == ಶೂನ್ಯಸೂಚಕ) {
        ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ = 1;
        p_ಬಫರ್_ಗಾತ್ರ = &ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ = *p_ಬಫರ್_ಗಾತ್ರ;
        ಆದರೆ (ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_ಬಫರ್_ಗಾತ್ರ = ಬಫರ್_ಗಾತ್ರ_Needed;
            UTRACE_EXIT_ಮೌಲ್ಯ(ಬಫರ್_ಗಾತ್ರ_Needed);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    ಆದರೆ (ರಾಶಿ_ಬಫರ್) {
        uintptr_t p = reinterpret_cast<uintptr_t>(ರಾಶಿ_ಬಫರ್);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t ಸೂಚಕ_Adjustment = aligned_p - p;
        ಆದರೆ (ಬಫರ್_ಗಾತ್ರ_Needed + ಸೂಚಕ_Adjustment <= ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ) {
            ರಾಶಿ_ಬಫರ್ = reinterpret_cast<ಶೂನ್ಯ *>(aligned_p);
            ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ -= static_cast<int32_t>(ಸೂಚಕ_Adjustment);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    ಆದರೆ (ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ < ಬಫರ್_ಗಾತ್ರ_Needed || ರಾಶಿ_ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (ಬಫರ್_ಗಾತ್ರ_Needed);

        ಆದರೆ(localConverter == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            UTRACE_EXIT_ಸ್ಥಿತಿ(*ಸ್ಥಿತಿ_2);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        ಆದರೆ (p_ಬಫರ್_ಗಾತ್ರ != &ರಾಶಿ_ಬಫರ್_ಗಾತ್ರ) {
            *ಸ್ಥಿತಿ_2 = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_ಬಫರ್_ಗಾತ್ರ = ಬಫರ್_ಗಾತ್ರ_Needed;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* just use the stack buffer */
        localConverter = (UConverter*) ರಾಶಿ_ಬಫರ್;
        allocatedConverter = ಶೂನ್ಯಸೂಚಕ;
    }

    uprv_memset(localConverter, 0, ಬಫರ್_ಗಾತ್ರ_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, ಗಾತ್ರ(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = ಸುಳ್ಳು;

    /* copy the substitution string */
    ಆದರೆ (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } ಇಲ್ಲದಿದ್ದರೆ {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ * U_SIZEOF_UCHAR);
        ಆದರೆ (localConverter->subChars == ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_ಸ್ಥಿತಿ(*ಸ್ಥಿತಿ_2);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    ಆದರೆ (cnv->shared_ದತ್ತಾಂಶ->impl->ಸುರಕ್ಷಿತ_Clone != ಶೂನ್ಯಸೂಚಕ) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_ದತ್ತಾಂಶ->impl->ಸುರಕ್ಷಿತ_Clone(cnv, localConverter, p_ಬಫರ್_ಗಾತ್ರ, ಸ್ಥಿತಿ_2);
    }

    ಆದರೆ(localConverter==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (allocatedConverter != ಶೂನ್ಯಸೂಚಕ && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_ಸ್ಥಿತಿ(*ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* increment refcount of shared data if needed */
    ಆದರೆ (cnv->shared_ದತ್ತಾಂಶ->isReferenceCounted) {
        ucnv_increment_Ref_ಎಣಿಕೆ(cnv->shared_ದತ್ತಾಂಶ);
    }

    ಆದರೆ(localConverter == (UConverter*)ರಾಶಿ_ಬಫರ್) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = ನಿಜ;
    }

    /* allow callback functions to handle any memory allocation */
    ಗಮ್ಯ_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->ಗಮ್ಯ_UContext, &ಗಮ್ಯ_UArgs, ಶೂನ್ಯಸೂಚಕ, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, ಶೂನ್ಯಸೂಚಕ, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_ಸ್ಥಿತಿ(localConverter, *ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ಸ್ಥಿರಾಂಕ UConverter* cnv, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ ucnv_ಸುರಕ್ಷಿತ_Clone(cnv, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;

    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_CLOSE);

    ಆದರೆ (converter == ಶೂನ್ಯಸೂಚಕ)
    {
        UTRACE_EXIT();
        ಹಿಂತಿರುಗಿ;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_ಸಂಕೇತ), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    ಆದರೆ (converter->fromCharErrorBehaviour != UCNV_ಗಮ್ಯ_U_DEFAULT_CALLBACK) {
        UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು ಗಮ್ಯ_UArgs = {
            ಗಾತ್ರ(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು),
                ನಿಜ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ
        };

        ಗಮ್ಯ_UArgs.converter = converter;
        error_ಸಂಕೇತ = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->ಗಮ್ಯ_UContext, &ಗಮ್ಯ_UArgs, ಶೂನ್ಯಸೂಚಕ, 0, UCNV_CLOSE, &error_ಸಂಕೇತ);
    }
    ಆದರೆ (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_ವಾದಗಳು fromUArgs = {
            ಗಾತ್ರ(UConverter_From_Unicode_ವಾದಗಳು),
                ನಿಜ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ
        };
        fromUArgs.converter = converter;
        error_ಸಂಕೇತ = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, ಶೂನ್ಯಸೂಚಕ, 0, 0, UCNV_CLOSE, &error_ಸಂಕೇತ);
    }

    ಆದರೆ (converter->shared_ದತ್ತಾಂಶ->impl->close != ಶೂನ್ಯಸೂಚಕ) {
        converter->shared_ದತ್ತಾಂಶ->impl->close(converter);
    }

    ಆದರೆ (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    ಆದರೆ (converter->shared_ದತ್ತಾಂಶ->isReferenceCounted) {
        ucnv_unload_Shared_ದತ್ತಾಂಶ_If_Ready(converter->shared_ದತ್ತಾಂಶ);
    }

    ಆದರೆ(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*   U_EXPORT2
ucnv_get_ಲಭ್ಯ_Name (int32_t n)
{
    ಆದರೆ (0 <= n && n <= 0xffff) {
        UError_ಸಂಕೇತ err = U_ZERO_ERROR;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = ucnv_bld_get_ಲಭ್ಯ_Converter((uint16_t)n, &err);
        ಆದರೆ (U_SUCCESS(err)) {
            ಹಿಂತಿರುಗಿ name;
        }
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI int32_t   U_EXPORT2
ucnv_ಎಣಿಕೆ_ಲಭ್ಯ ()
{
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ ucnv_bld_ಎಣಿಕೆ_ಲಭ್ಯ_Converters(&err);
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
ucnv_getSubstChars (ಸ್ಥಿರಾಂಕ UConverter * converter,
                    ಅಕ್ಷರ *mySubChar,
                    int8_t * len,
                    UError_ಸಂಕೇತ * err)
{
    ಆದರೆ (U_FAILURE (*err))
        ಹಿಂತಿರುಗಿ;

    ಆದರೆ (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
ucnv_ಗಣ_Subst_Chars (UConverter * converter,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mySubChar,
                    int8_t len,
                    UError_ಸಂಕೇತ * err)
{
    ಆದರೆ (U_FAILURE (*err))
        ಹಿಂತಿರುಗಿ;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    ಆದರೆ ((len > converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->max_ಬೈಟುಗಳು_ಪ್ರತಿ_Char)
     || (len < converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->min_ಬೈಟುಗಳು_ಪ್ರತಿ_Char))
    {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
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

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_ಗಣ_Subst_String(UConverter *cnv,
                    ಸ್ಥಿರಾಂಕ char16_t *s,
                    int32_t length,
                    UError_ಸಂಕೇತ *err) {
    alignas(UConverter) ಅಕ್ಷರ clone_ಬಫರ್[U_CNV_SAFECLONE_BUFFERSIZE];
    ಅಕ್ಷರ chars[UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_ಗಾತ್ರ, length8;

    /* Let the following functions check all arguments. */
    clone_ಗಾತ್ರ = ಗಾತ್ರ(clone_ಬಫರ್);
    clone = ucnv_ಸುರಕ್ಷಿತ_Clone(cnv, clone_ಬಫರ್, &clone_ಗಾತ್ರ, err);
    ucnv_ಗಣ_From_UCall_ಹಿಂಭಾಗ(clone, UCNV_FROM_U_CALLBACK_STOP, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)ಗಾತ್ರ(chars), s, length, err);
    ucnv_close(clone);
    ಆದರೆ (U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (cnv->shared_ದತ್ತಾಂಶ->impl->writeSub == ಶೂನ್ಯಸೂಚಕ
#ಆದರೆ !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#ಆದರೆ_ಅಂತ್ಯ
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        ಆದರೆ (length > UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        subChars = (uint8_t *)s;
        ಆದರೆ (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    ಆದರೆ (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        ಆದರೆ (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ * U_SIZEOF_UCHAR);
            ಆದರೆ (cnv->subChars == ಶೂನ್ಯಸೂಚಕ) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    ಆದರೆ (length8 == 0) {
        cnv->subCharLen = 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        uprv_memcpy(cnv->subChars, subChars, length8);
        ಆದರೆ (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } ಇಲ್ಲದಿದ್ದರೆ /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
ಸ್ಥಿರ ಶೂನ್ಯ _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    ಆದರೆ(converter == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_ಸಂಕೇತ error_ಸಂಕೇತ;

        ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE && converter->fromCharErrorBehaviour != UCNV_ಗಮ್ಯ_U_DEFAULT_CALLBACK) {
            UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು ಗಮ್ಯ_UArgs = {
                ಗಾತ್ರ(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು),
                ನಿಜ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ
            };
            ಗಮ್ಯ_UArgs.converter = converter;
            error_ಸಂಕೇತ = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->ಗಮ್ಯ_UContext, &ಗಮ್ಯ_UArgs, ಶೂನ್ಯಸೂಚಕ, 0, UCNV_RESET, &error_ಸಂಕೇತ);
        }
        ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_ವಾದಗಳು fromUArgs = {
                ಗಾತ್ರ(UConverter_From_Unicode_ವಾದಗಳು),
                ನಿಜ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ,
                ಶೂನ್ಯಸೂಚಕ
            };
            fromUArgs.converter = converter;
            error_ಸಂಕೇತ = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, ಶೂನ್ಯಸೂಚಕ, 0, 0, UCNV_RESET, &error_ಸಂಕೇತ);
        }
    }

    /* now reset the converter itself */
    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = converter->shared_ದತ್ತಾಂಶ->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
        converter->ಬಹುಲಕ = 0;
        converter->ಗಮ್ಯ_ULength = 0;
        converter->invalid_Char_ಉದ್ದಳತೆ = converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 0;
        converter->pre_ಗಮ್ಯ_ULength = 0;
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        converter->from_Unicode_ಸ್ಥಿತಿ = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_ಉದ್ದಳತೆ = converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    ಆದರೆ (converter->shared_ದತ್ತಾಂಶ->impl->reset != ಶೂನ್ಯಸೂಚಕ) {
        /* call the custom reset function */
        converter->shared_ದತ್ತಾಂಶ->impl->reset(converter, choice);
    }
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, ನಿಜ);
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_reset_ಗಮ್ಯ_Unicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_ಗಮ್ಯ_UNICODE, ನಿಜ);
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, ನಿಜ);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_ಗಾತ್ರ (ಸ್ಥಿರಾಂಕ UConverter * converter)
{
    ಹಿಂತಿರುಗಿ converter->max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_ಗಾತ್ರ (ಸ್ಥಿರಾಂಕ UConverter * converter)
{
    ಹಿಂತಿರುಗಿ converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->min_ಬೈಟುಗಳು_ಪ್ರತಿ_Char;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ucnv_getName(ಸ್ಥಿರಾಂಕ UConverter* converter U_LIFETIME_BOUND, UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (U_FAILURE (*err))
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ(converter->shared_ದತ್ತಾಂಶ->impl->getName){
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ತಾತ್ಕಾಲಿಕ= converter->shared_ದತ್ತಾಂಶ->impl->getName(converter);
        ಆದರೆ(ತಾತ್ಕಾಲಿಕ)
            ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ;
    }
    ಹಿಂತಿರುಗಿ converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ಸ್ಥಿರಾಂಕ UConverter * converter,
              UError_ಸಂಕೇತ * err)
{
    int32_t ccsid;
    ಆದರೆ (U_FAILURE (*err))
        ಹಿಂತಿರುಗಿ -1;

    ccsid = converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->codepage;
    ಆದರೆ (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        ಆದರೆ (U_SUCCESS(*err) && standardName) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ccsidStr = uprv_strchr(standardName, '-');
            ಆದರೆ (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    ಹಿಂತಿರುಗಿ ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (ಸ್ಥಿರಾಂಕ UConverter * converter,
                                      UError_ಸಂಕೇತ * err)
{
    ಆದರೆ (U_FAILURE (*err))
        ಹಿಂತಿರುಗಿ UCNV_UNKNOWN;

    ಹಿಂತಿರುಗಿ (UConverterPlatform)converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->platform;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
    ucnv_get_ಗಮ್ಯ_UCall_ಹಿಂಭಾಗ (ಸ್ಥಿರಾಂಕ UConverter * converter,
                         UConverter_ಗಮ್ಯ_UCallback *action,
                         ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->ಗಮ್ಯ_UContext;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
    ucnv_get_From_UCall_ಹಿಂಭಾಗ (ಸ್ಥಿರಾಂಕ UConverter * converter,
                           UConverterFromUCallback *action,
                           ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI ಶೂನ್ಯ    U_EXPORT2
ucnv_ಗಣ_ಗಮ್ಯ_UCall_ಹಿಂಭಾಗ (UConverter * converter,
                            UConverter_ಗಮ್ಯ_UCallback newAction,
                            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* newContext,
                            UConverter_ಗಮ್ಯ_UCallback *oldAction,
                            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ** oldContext,
                            UError_ಸಂಕೇತ * err)
{
    ಆದರೆ (U_FAILURE (*err))
        ಹಿಂತಿರುಗಿ;
    ಆದರೆ (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    ಆದರೆ (oldContext) *oldContext = converter->ಗಮ್ಯ_UContext;
    converter->ಗಮ್ಯ_UContext = newContext;
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_ಗಣ_From_UCall_ಹಿಂಭಾಗ (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* newContext,
                            UConverterFromUCallback *oldAction,
                            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ** oldContext,
                            UError_ಸಂಕೇತ * err)
{
    ಆದರೆ (U_FAILURE (*err))
        ಹಿಂತಿರುಗಿ;
    ಆದರೆ (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    ಆದರೆ (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

ಸ್ಥಿರ ಶೂನ್ಯ
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, int32_t error_ಒಳಹರಿವು_ಉದ್ದಳತೆ) {
    int32_t *ಎಲ್ಲೆ;
    int32_t delta, ಸ್ಥಾನಾಂತರ;

    ಆದರೆ(ಮೂಲ_ಸೂಚ್ಯಂಕ>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=ಮೂಲ_ಸೂಚ್ಯಂಕ-error_ಒಳಹರಿವು_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    ಎಲ್ಲೆ=offsets+length;
    ಆದರೆ(delta==0) {
        /* most common case, nothing to do */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        ಆಗ(offsets<ಎಲ್ಲೆ) {
            ಸ್ಥಾನಾಂತರ=*offsets;
            ಆದರೆ(ಸ್ಥಾನಾಂತರ>=0) {
                *offsets=ಸ್ಥಾನಾಂತರ+delta;
            }
            ++offsets;
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        ಆಗ(offsets<ಎಲ್ಲೆ) {
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

ಸ್ಥಿರ ಶೂನ್ಯ
_fromUnicodeWithCallback(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *s;
    ಅಕ್ಷರ *t;
    int32_t *offsets;
    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;
    int32_t error_ಒಳಹರಿವು_ಉದ್ದಳತೆ;
    UBool converter_Saw_ಕೊನೆ_ದ_ಒಳಹರಿವು, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ಸ್ಥಿರಾಂಕ char16_t *real_ಮೂಲ, *real_ಮೂಲ_ಎಲ್ಲೆ;
    int32_t real_ಮೂಲ_ಸೂಚ್ಯಂಕ;
    UBool realFlush;

    cnv=p_ವಾದಗಳು->converter;
    s=p_ವಾದಗಳು->ಮೂಲ;
    t=p_ವಾದಗಳು->ಗುರಿ;
    offsets=p_ವಾದಗಳು->offsets;

    /* get the converter implementation function */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;
    ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
        fromUnicode=cnv->shared_ದತ್ತಾಂಶ->impl->fromUnicode;
    } ಇಲ್ಲದಿದ್ದರೆ {
        fromUnicode=cnv->shared_ದತ್ತಾಂಶ->impl->fromUnicodeWithOffsets;
        ಆದರೆ(fromUnicode==ಶೂನ್ಯಸೂಚಕ) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_ದತ್ತಾಂಶ->impl->fromUnicode;
            /* we will write -1 for each offset */
            ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;
        }
    }

    ಆದರೆ(cnv->preFromULength>=0) {
        /* normal mode */
        real_ಮೂಲ=ಶೂನ್ಯಸೂಚಕ;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ಮೂಲ_ಎಲ್ಲೆ=ಶೂನ್ಯಸೂಚಕ;
        realFlush=ಸುಳ್ಳು;
        real_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        ಆದರೆ (!p_ವಾದಗಳು->ಮೂಲ) {
            *err=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        real_ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
        real_ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
        realFlush=p_ವಾದಗಳು->flush;
        real_ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮೂಲ_ಸೂಚ್ಯಂಕ;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_ವಾದಗಳು->ಮೂಲ=replay;
        p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=replay-cnv->preFromULength;
        p_ವಾದಗಳು->flush=ಸುಳ್ಳು;
        ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;

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
    ಗಾಗಿ(;;) {
        ಆದರೆ(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(p_ವಾದಗಳು, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_ಕೊನೆ_ದ_ಒಳಹರಿವು=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_ವಾದಗಳು->flush && p_ವಾದಗಳು->ಮೂಲ==p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ &&
                        cnv->fromUChar32==0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* handle error from ucnv_convertEx() */
            converter_Saw_ಕೊನೆ_ದ_ಒಳಹರಿವು=ಸುಳ್ಳು;
        }

        /* no callback called yet for this iteration */
        calledCallback=ಸುಳ್ಳು;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_ಒಳಹರಿವು_ಉದ್ದಳತೆ=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        ಗಾಗಿ(;;) {
            /* update offsets if we write any */
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                int32_t length = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ - t);
                ಆದರೆ(length>0) {
                    _updateOffsets(offsets, length, ಮೂಲ_ಸೂಚ್ಯಂಕ, error_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_ವಾದಗಳು->offsets=offsets+=length;
                }

                ಆದರೆ(ಮೂಲ_ಸೂಚ್ಯಂಕ>=0) {
                    ಮೂಲ_ಸೂಚ್ಯಂಕ += static_cast<int32_t>(p_ವಾದಗಳು->ಮೂಲ - s);
                }
            }

            ಆದರೆ(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                ಆದರೆ(real_ಮೂಲ==ಶೂನ್ಯಸೂಚಕ) {
                    real_ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
                    real_ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
                    realFlush=p_ವಾದಗಳು->flush;
                    real_ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮೂಲ_ಸೂಚ್ಯಂಕ;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_ವಾದಗಳು->ಮೂಲ=replay;
                    p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=replay-cnv->preFromULength;
                    p_ವಾದಗಳು->flush=ಸುಳ್ಳು;
                    ಆದರೆ((ಮೂಲ_ಸೂಚ್ಯಂಕ+=cnv->preFromULength)<0) {
                        ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;
                    }

                    cnv->preFromULength=0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ಮೂಲ==ಶೂನ್ಯಸೂಚಕ);
                    *err=U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
                }
            }

            /* update pointers */
            s=p_ವಾದಗಳು->ಮೂಲ;
            t=p_ವಾದಗಳು->ಗುರಿ;

            ಆದರೆ(U_SUCCESS(*err)) {
                ಆದರೆ(s<p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(real_ಮೂಲ!=ಶೂನ್ಯಸೂಚಕ) {
                    /* switch back from replaying to the real source and continue */
                    p_ವಾದಗಳು->ಮೂಲ=real_ಮೂಲ;
                    p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=real_ಮೂಲ_ಎಲ್ಲೆ;
                    p_ವಾದಗಳು->flush=realFlush;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=real_ಮೂಲ_ಸೂಚ್ಯಂಕ;

                    real_ಮೂಲ=ಶೂನ್ಯಸೂಚಕ;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(p_ವಾದಗಳು->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
                    calledCallback=ಸುಳ್ಳು; /* new error condition */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* input consumed */
                    ಆದರೆ(p_ವಾದಗಳು->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        ಆದರೆ(!converter_Saw_ಕೊನೆ_ದ_ಒಳಹರಿವು) {
                            ನಿಲ್ಲಿಸು;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, ಸುಳ್ಳು);
                    }

                    /* done successfully */
                    ಹಿಂತಿರುಗಿ;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_ಸಂಕೇತ e;

                ಆದರೆ( calledCallback ||
                    (e=*err)==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR ||
                    (e!=U_INVALID_CHAR_ಸಿಕ್ಕಿತು &&
                     e!=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು &&
                     e!=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು)
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
                    ಆದರೆ(real_ಮೂಲ!=ಶೂನ್ಯಸೂಚಕ) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ - p_ವಾದಗಳು->ಮೂಲ);
                        ಆದರೆ(length>0) {
                            u_memcpy(cnv->preFromU, p_ವಾದಗಳು->ಮೂಲ, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_ವಾದಗಳು->ಮೂಲ=real_ಮೂಲ;
                        p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=real_ಮೂಲ_ಎಲ್ಲೆ;
                        p_ವಾದಗಳು->flush=realFlush;
                    }

                    ಹಿಂತಿರುಗಿ;
                }
            }

            /* callback handling */
            {
                UChar32 ಸಂಕೇತ_ಬಿಂದು;

                /* get and write the code point */
                ಸಂಕೇತ_ಬಿಂದು=cnv->fromUChar32;
                error_ಒಳಹರಿವು_ಉದ್ದಳತೆ=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_ಬಫರ್, error_ಒಳಹರಿವು_ಉದ್ದಳತೆ, ಸಂಕೇತ_ಬಿಂದು);
                cnv->invalid_UChar_ಉದ್ದಳತೆ = static_cast<int8_t>(error_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_ವಾದಗಳು,
                    cnv->invalid_UChar_ಬಫರ್, error_ಒಳಹರಿವು_ಉದ್ದಳತೆ, ಸಂಕೇತ_ಬಿಂದು,
                    *err==U_INVALID_CHAR_ಸಿಕ್ಕಿತು ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
                    err);
            }

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=ನಿಜ;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
ಸ್ಥಿರ UBool
ucnv_output_ಅತಿಪ್ರವಾಹ_From_Unicode(UConverter *cnv,
                               ಅಕ್ಷರ **ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ,
                               int32_t **pOffsets,
                               UError_ಸಂಕೇತ *err) {
    int32_t *offsets;
    ಅಕ್ಷರ *ಅತಿಪ್ರವಾಹ, *t;
    int32_t i, length;

    t=*ಗುರಿ;
    ಆದರೆ(pOffsets!=ಶೂನ್ಯಸೂಚಕ) {
        offsets=*pOffsets;
    } ಇಲ್ಲದಿದ್ದರೆ {
        offsets=ಶೂನ್ಯಸೂಚಕ;
    }

    ಅತಿಪ್ರವಾಹ = reinterpret_cast<ಅಕ್ಷರ*>(cnv->char_Error_ಬಫರ್);
    length=cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ;
    i=0;
    ಆಗ(i<length) {
        ಆದರೆ(t==ಗುರಿ_ಎಲ್ಲೆ) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            ಮಾಡು {
                ಅತಿಪ್ರವಾಹ[j++]=ಅತಿಪ್ರವಾಹ[i++];
            } ಆಗ(i<length);

            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = static_cast<int8_t>(j);
            *ಗುರಿ=t;
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *pOffsets=offsets;
            }
            *err=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }

        /* copy the overflow contents to the target */
        *t++=ಅತಿಪ್ರವಾಹ[i++];
        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=0;
    *ಗುರಿ=t;
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
        *pOffsets=offsets;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 ಅಕ್ಷರ **ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ,
                 ಸ್ಥಿರಾಂಕ char16_t **ಮೂಲ, ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ,
                 int32_t *offsets,
                 UBool flush,
                 UError_ಸಂಕೇತ *err) {
    UConverter_From_Unicode_ವಾದಗಳು ವಾದಗಳು_2;
    ಸ್ಥಿರಾಂಕ char16_t *s;
    ಅಕ್ಷರ *t;

    /* check parameters */
    ಆದರೆ(err==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(cnv==ಶೂನ್ಯಸೂಚಕ || ಗುರಿ==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ==ಶೂನ್ಯಸೂಚಕ) {
        *err=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    s=*ಮೂಲ;
    t=*ಗುರಿ;

    ಆದರೆ ((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)U_MAX_PTR(ಮೂಲ_ಎಲ್ಲೆ) == (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)ಮೂಲ_ಎಲ್ಲೆ) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        ಮೂಲ_ಎಲ್ಲೆ = (ಸ್ಥಿರಾಂಕ char16_t *)(((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ_ಎಲ್ಲೆ) - 1);
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
    ಆದರೆ (ಮೂಲ_ಎಲ್ಲೆ<s || ಗುರಿ_ಎಲ್ಲೆ<t ||
        ((size_t)(ಮೂಲ_ಎಲ್ಲೆ-s)>(size_t)0x3fffffff && ಮೂಲ_ಎಲ್ಲೆ>s) ||
        ((size_t)(ಗುರಿ_ಎಲ್ಲೆ-t)>(size_t)0x7fffffff && ಗುರಿ_ಎಲ್ಲೆ>t) ||
        (((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ_ಎಲ್ಲೆ-(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    
    /* output the target overflow buffer */
    ಆದರೆ( cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ>0 &&
        ucnv_output_ಅತಿಪ್ರವಾಹ_From_Unicode(cnv, ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        ಹಿಂತಿರುಗಿ;
    }
    /* *target may have moved, therefore stop using t */

    ಆದರೆ(!flush && s==ಮೂಲ_ಎಲ್ಲೆ && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        ಹಿಂತಿರುಗಿ;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    ವಾದಗಳು_2.converter=cnv;
    ವಾದಗಳು_2.flush=flush;
    ವಾದಗಳು_2.offsets=offsets;
    ವಾದಗಳು_2.ಮೂಲ=s;
    ವಾದಗಳು_2.ಮೂಲ_ಎಲ್ಲೆ=ಮೂಲ_ಎಲ್ಲೆ;
    ವಾದಗಳು_2.ಗುರಿ=*ಗುರಿ;
    ವಾದಗಳು_2.ಗುರಿ_ಎಲ್ಲೆ=ಗುರಿ_ಎಲ್ಲೆ;
    ವಾದಗಳು_2.size=ಗಾತ್ರ(ವಾದಗಳು_2);

    _fromUnicodeWithCallback(&ವಾದಗಳು_2, err);

    *ಮೂಲ=ವಾದಗಳು_2.ಮೂಲ;
    *ಗುರಿ=ವಾದಗಳು_2.ಗುರಿ;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

ಸ್ಥಿರ ಶೂನ್ಯ
ಗಮ್ಯ_Unicode_With_Callback(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *err) {
    UConverter_ಗಮ್ಯ_Unicode ಗಮ್ಯ_Unicode;
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
    char16_t *t;
    int32_t *offsets;
    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;
    int32_t error_ಒಳಹರಿವು_ಉದ್ದಳತೆ;
    UBool converter_Saw_ಕೊನೆ_ದ_ಒಳಹರಿವು, calledCallback;

    /* variables for m:n conversion */
    ಅಕ್ಷರ replay[UCNV_EXT_MAX_ಬೈಟುಗಳು];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *real_ಮೂಲ, *real_ಮೂಲ_ಎಲ್ಲೆ;
    int32_t real_ಮೂಲ_ಸೂಚ್ಯಂಕ;
    UBool realFlush;

    cnv=p_ವಾದಗಳು->converter;
    s=p_ವಾದಗಳು->ಮೂಲ;
    t=p_ವಾದಗಳು->ಗುರಿ;
    offsets=p_ವಾದಗಳು->offsets;

    /* get the converter implementation function */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;
    ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
        ಗಮ್ಯ_Unicode=cnv->shared_ದತ್ತಾಂಶ->impl->ಗಮ್ಯ_Unicode;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಮ್ಯ_Unicode=cnv->shared_ದತ್ತಾಂಶ->impl->ಗಮ್ಯ_Unicode_With_Offsets;
        ಆದರೆ(ಗಮ್ಯ_Unicode==ಶೂನ್ಯಸೂಚಕ) {
            /* there is no WithOffsets implementation */
            ಗಮ್ಯ_Unicode=cnv->shared_ದತ್ತಾಂಶ->impl->ಗಮ್ಯ_Unicode;
            /* we will write -1 for each offset */
            ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;
        }
    }

    ಆದರೆ(cnv->pre_ಗಮ್ಯ_ULength>=0) {
        /* normal mode */
        real_ಮೂಲ=ಶೂನ್ಯಸೂಚಕ;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ಮೂಲ_ಎಲ್ಲೆ=ಶೂನ್ಯಸೂಚಕ;
        realFlush=ಸುಳ್ಳು;
        real_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        ಆದರೆ (!p_ವಾದಗಳು->ಮೂಲ) {
            *err=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        real_ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
        real_ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
        realFlush=p_ವಾದಗಳು->flush;
        real_ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮೂಲ_ಸೂಚ್ಯಂಕ;

        uprv_memcpy(replay, cnv->pre_ಗಮ್ಯ_U, -cnv->pre_ಗಮ್ಯ_ULength);
        p_ವಾದಗಳು->ಮೂಲ=replay;
        p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=replay-cnv->pre_ಗಮ್ಯ_ULength;
        p_ವಾದಗಳು->flush=ಸುಳ್ಳು;
        ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;

        cnv->pre_ಗಮ್ಯ_ULength=0;
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
    ಗಾಗಿ(;;) {
        ಆದರೆ(U_SUCCESS(*err)) {
            /* convert */
            ಗಮ್ಯ_Unicode(p_ವಾದಗಳು, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_ಕೊನೆ_ದ_ಒಳಹರಿವು=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_ವಾದಗಳು->flush && p_ವಾದಗಳು->ಮೂಲ==p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ &&
                        cnv->ಗಮ್ಯ_ULength==0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_ಕೊನೆ_ದ_ಒಳಹರಿವು=ಸುಳ್ಳು;
        }

        /* no callback called yet for this iteration */
        calledCallback=ಸುಳ್ಳು;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_ಒಳಹರಿವು_ಉದ್ದಳತೆ=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        ಗಾಗಿ(;;) {
            /* update offsets if we write any */
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                int32_t length = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ - t);
                ಆದರೆ(length>0) {
                    _updateOffsets(offsets, length, ಮೂಲ_ಸೂಚ್ಯಂಕ, error_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_ವಾದಗಳು->offsets=offsets+=length;
                }

                ಆದರೆ(ಮೂಲ_ಸೂಚ್ಯಂಕ>=0) {
                    ಮೂಲ_ಸೂಚ್ಯಂಕ += static_cast<int32_t>(p_ವಾದಗಳು->ಮೂಲ - s);
                }
            }

            ಆದರೆ(cnv->pre_ಗಮ್ಯ_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                ಆದರೆ(real_ಮೂಲ==ಶೂನ್ಯಸೂಚಕ) {
                    real_ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
                    real_ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
                    realFlush=p_ವಾದಗಳು->flush;
                    real_ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮೂಲ_ಸೂಚ್ಯಂಕ;

                    uprv_memcpy(replay, cnv->pre_ಗಮ್ಯ_U, -cnv->pre_ಗಮ್ಯ_ULength);
                    p_ವಾದಗಳು->ಮೂಲ=replay;
                    p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=replay-cnv->pre_ಗಮ್ಯ_ULength;
                    p_ವಾದಗಳು->flush=ಸುಳ್ಳು;
                    ಆದರೆ((ಮೂಲ_ಸೂಚ್ಯಂಕ+=cnv->pre_ಗಮ್ಯ_ULength)<0) {
                        ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;
                    }

                    cnv->pre_ಗಮ್ಯ_ULength=0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ಮೂಲ==ಶೂನ್ಯಸೂಚಕ);
                    *err=U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
                }
            }

            /* update pointers */
            s=p_ವಾದಗಳು->ಮೂಲ;
            t=p_ವಾದಗಳು->ಗುರಿ;

            ಆದರೆ(U_SUCCESS(*err)) {
                ಆದರೆ(s<p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(real_ಮೂಲ!=ಶೂನ್ಯಸೂಚಕ) {
                    /* switch back from replaying to the real source and continue */
                    p_ವಾದಗಳು->ಮೂಲ=real_ಮೂಲ;
                    p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=real_ಮೂಲ_ಎಲ್ಲೆ;
                    p_ವಾದಗಳು->flush=realFlush;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=real_ಮೂಲ_ಸೂಚ್ಯಂಕ;

                    real_ಮೂಲ=ಶೂನ್ಯಸೂಚಕ;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(p_ವಾದಗಳು->flush && cnv->ಗಮ್ಯ_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
                    calledCallback=ಸುಳ್ಳು; /* new error condition */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* input consumed */
                    ಆದರೆ(p_ವಾದಗಳು->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        ಆದರೆ(!converter_Saw_ಕೊನೆ_ದ_ಒಳಹರಿವು) {
                            ನಿಲ್ಲಿಸು;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_ಗಮ್ಯ_UNICODE, ಸುಳ್ಳು);
                    }

                    /* done successfully */
                    ಹಿಂತಿರುಗಿ;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_ಸಂಕೇತ e;

                ಆದರೆ( calledCallback ||
                    (e=*err)==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR ||
                    (e!=U_INVALID_CHAR_ಸಿಕ್ಕಿತು &&
                     e!=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು &&
                     e!=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು &&
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
                    ಆದರೆ(real_ಮೂಲ!=ಶೂನ್ಯಸೂಚಕ) {
                        int32_t length;

                        U_ASSERT(cnv->pre_ಗಮ್ಯ_ULength==0);

                        length = static_cast<int32_t>(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ - p_ವಾದಗಳು->ಮೂಲ);
                        ಆದರೆ(length>0) {
                            uprv_memcpy(cnv->pre_ಗಮ್ಯ_U, p_ವಾದಗಳು->ಮೂಲ, length);
                            cnv->pre_ಗಮ್ಯ_ULength = static_cast<int8_t>(-length);
                        }

                        p_ವಾದಗಳು->ಮೂಲ=real_ಮೂಲ;
                        p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ=real_ಮೂಲ_ಎಲ್ಲೆ;
                        p_ವಾದಗಳು->flush=realFlush;
                    }

                    ಹಿಂತಿರುಗಿ;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_ಒಳಹರಿವು_ಉದ್ದಳತೆ=cnv->invalid_Char_ಉದ್ದಳತೆ=cnv->ಗಮ್ಯ_ULength;
            ಆದರೆ(error_ಒಳಹರಿವು_ಉದ್ದಳತೆ>0) {
                uprv_memcpy(cnv->invalid_Char_ಬಫರ್, cnv->ಗಮ್ಯ_UBytes, error_ಒಳಹರಿವು_ಉದ್ದಳತೆ);
            }

            /* set the converter state to deal with the next character */
            cnv->ಗಮ್ಯ_ULength=0;

            /* call the callback function */
            ಆದರೆ(cnv->ಗಮ್ಯ_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_ಸಿಕ್ಕಿತು) {
                cnv->ಗಮ್ಯ_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->ಗಮ್ಯ_UContext, p_ವಾದಗಳು,
                cnv->invalid_Char_ಬಫರ್, error_ಒಳಹರಿವು_ಉದ್ದಳತೆ,
                cnv->ಗಮ್ಯ_UCallback_Reason,
                err);
            cnv->ಗಮ್ಯ_UCallback_Reason = UCNV_ILLEGAL; /* reset to default value */

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=ನಿಜ;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
ಸ್ಥಿರ UBool
ucnv_output_ಅತಿಪ್ರವಾಹ_ಗಮ್ಯ_Unicode(UConverter *cnv,
                             char16_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ,
                             int32_t **pOffsets,
                             UError_ಸಂಕೇತ *err) {
    int32_t *offsets;
    char16_t *ಅತಿಪ್ರವಾಹ, *t;
    int32_t i, length;

    t=*ಗುರಿ;
    ಆದರೆ(pOffsets!=ಶೂನ್ಯಸೂಚಕ) {
        offsets=*pOffsets;
    } ಇಲ್ಲದಿದ್ದರೆ {
        offsets=ಶೂನ್ಯಸೂಚಕ;
    }

    ಅತಿಪ್ರವಾಹ=cnv->UChar_Error_ಬಫರ್;
    length=cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ;
    i=0;
    ಆಗ(i<length) {
        ಆದರೆ(t==ಗುರಿ_ಎಲ್ಲೆ) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            ಮಾಡು {
                ಅತಿಪ್ರವಾಹ[j++]=ಅತಿಪ್ರವಾಹ[i++];
            } ಆಗ(i<length);

            cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = static_cast<int8_t>(j);
            *ಗುರಿ=t;
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *pOffsets=offsets;
            }
            *err=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }

        /* copy the overflow contents to the target */
        *t++=ಅತಿಪ್ರವಾಹ[i++];
        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=0;
    *ಗುರಿ=t;
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
        *pOffsets=offsets;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_ಗಮ್ಯ_Unicode(UConverter *cnv,
               char16_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ,
               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ,
               int32_t *offsets,
               UBool flush,
               UError_ಸಂಕೇತ *err) {
    UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು ವಾದಗಳು_2;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
    char16_t *t;

    /* check parameters */
    ಆದರೆ(err==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(cnv==ಶೂನ್ಯಸೂಚಕ || ಗುರಿ==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ==ಶೂನ್ಯಸೂಚಕ) {
        *err=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    s=*ಮೂಲ;
    t=*ಗುರಿ;

    ಆದರೆ ((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)U_MAX_PTR(ಗುರಿ_ಎಲ್ಲೆ) == (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)ಗುರಿ_ಎಲ್ಲೆ) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        ಗುರಿ_ಎಲ್ಲೆ = (ಸ್ಥಿರಾಂಕ char16_t *)(((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಗುರಿ_ಎಲ್ಲೆ) - 1);
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
    ಆದರೆ (ಮೂಲ_ಎಲ್ಲೆ<s || ಗುರಿ_ಎಲ್ಲೆ<t ||
        ((size_t)(ಮೂಲ_ಎಲ್ಲೆ-s)>(size_t)0x7fffffff && ಮೂಲ_ಎಲ್ಲೆ>s) ||
        ((size_t)(ಗುರಿ_ಎಲ್ಲೆ-t)>(size_t)0x3fffffff && ಗುರಿ_ಎಲ್ಲೆ>t) ||
        (((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಗುರಿ_ಎಲ್ಲೆ-(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    
    /* output the target overflow buffer */
    ಆದರೆ( cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ>0 &&
        ucnv_output_ಅತಿಪ್ರವಾಹ_ಗಮ್ಯ_Unicode(cnv, ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        ಹಿಂತಿರುಗಿ;
    }
    /* *target may have moved, therefore stop using t */

    ಆದರೆ(!flush && s==ಮೂಲ_ಎಲ್ಲೆ && cnv->pre_ಗಮ್ಯ_ULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        ಹಿಂತಿರುಗಿ;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    ವಾದಗಳು_2.converter=cnv;
    ವಾದಗಳು_2.flush=flush;
    ವಾದಗಳು_2.offsets=offsets;
    ವಾದಗಳು_2.ಮೂಲ=s;
    ವಾದಗಳು_2.ಮೂಲ_ಎಲ್ಲೆ=ಮೂಲ_ಎಲ್ಲೆ;
    ವಾದಗಳು_2.ಗುರಿ=*ಗುರಿ;
    ವಾದಗಳು_2.ಗುರಿ_ಎಲ್ಲೆ=ಗುರಿ_ಎಲ್ಲೆ;
    ವಾದಗಳು_2.size=ಗಾತ್ರ(ವಾದಗಳು_2);

    ಗಮ್ಯ_Unicode_With_Callback(&ವಾದಗಳು_2, err);

    *ಮೂಲ=ವಾದಗಳು_2.ಮೂಲ;
    *ಗುರಿ=ವಾದಗಳು_2.ಗುರಿ;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ char16_t *src_ಎಲ್ಲೆ;
    ಅಕ್ಷರ *originalDest, *dest_ಎಲ್ಲೆ;
    int32_t dest_ಉದ್ದಳತೆ;

    /* check arguments */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ( cnv==ಶೂನ್ಯಸೂಚಕ ||
        dest_ಸಾಮರ್ಥ್ಯ<0 || (dest_ಸಾಮರ್ಥ್ಯ>0 && dest==ಶೂನ್ಯಸೂಚಕ) ||
        src_ಉದ್ದಳತೆ<-1 || (src_ಉದ್ದಳತೆ!=0 && src==ಶೂನ್ಯಸೂಚಕ)
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        src_ಉದ್ದಳತೆ=u_strlen(src);
    }
    ಆದರೆ(src_ಉದ್ದಳತೆ>0) {
        src_ಎಲ್ಲೆ=src+src_ಉದ್ದಳತೆ;
        dest_ಸಾಮರ್ಥ್ಯ=pin_ಸಾಮರ್ಥ್ಯ(dest, dest_ಸಾಮರ್ಥ್ಯ);
        dest_ಎಲ್ಲೆ=dest+dest_ಸಾಮರ್ಥ್ಯ;

        /* perform the conversion */
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_ಎಲ್ಲೆ, &src, src_ಎಲ್ಲೆ, ಶೂನ್ಯಸೂಚಕ, ನಿಜ, &ಬಫರ್_ಸ್ಥಿತಿ);
        dest_ಉದ್ದಳತೆ=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
            ಅಕ್ಷರ ಬಫರ್[1024];

            dest_ಎಲ್ಲೆ=ಬಫರ್+ಗಾತ್ರ(ಬಫರ್);
            ಮಾಡು {
                dest=ಬಫರ್;
                ಬಫರ್_ಸ್ಥಿತಿ=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_ಎಲ್ಲೆ, &src, src_ಎಲ್ಲೆ, ಶೂನ್ಯಸೂಚಕ, ನಿಜ, &ಬಫರ್_ಸ್ಥಿತಿ);
                dest_ಉದ್ದಳತೆ+=(int32_t)(dest-ಬಫರ್);
            } ಆಗ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR);
        }
        ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
            *p_Error_ಸಂಕೇತ = ಬಫರ್_ಸ್ಥಿತಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        dest_ಉದ್ದಳತೆ=0;
    }

    ಹಿಂತಿರುಗಿ u_terminateChars(originalDest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
ucnv_ಗಮ್ಯ_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src_ಎಲ್ಲೆ;
    char16_t *originalDest, *dest_ಎಲ್ಲೆ;
    int32_t dest_ಉದ್ದಳತೆ;

    /* check arguments */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ( cnv==ಶೂನ್ಯಸೂಚಕ ||
        dest_ಸಾಮರ್ಥ್ಯ<0 || (dest_ಸಾಮರ್ಥ್ಯ>0 && dest==ಶೂನ್ಯಸೂಚಕ) ||
        src_ಉದ್ದಳತೆ<-1 || (src_ಉದ್ದಳತೆ!=0 && src==ಶೂನ್ಯಸೂಚಕ))
    {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* initialize */
    ucnv_reset_ಗಮ್ಯ_Unicode(cnv);
    originalDest=dest;
    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        src_ಉದ್ದಳತೆ=(int32_t)uprv_strlen(src);
    }
    ಆದರೆ(src_ಉದ್ದಳತೆ>0) {
        src_ಎಲ್ಲೆ=src+src_ಉದ್ದಳತೆ;
        dest_ಸಾಮರ್ಥ್ಯ=pin_ಸಾಮರ್ಥ್ಯ(dest, dest_ಸಾಮರ್ಥ್ಯ);
        dest_ಎಲ್ಲೆ=dest+dest_ಸಾಮರ್ಥ್ಯ;

        /* perform the conversion */
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        ucnv_ಗಮ್ಯ_Unicode(cnv, &dest, dest_ಎಲ್ಲೆ, &src, src_ಎಲ್ಲೆ, ಶೂನ್ಯಸೂಚಕ, ನಿಜ, &ಬಫರ್_ಸ್ಥಿತಿ);
        dest_ಉದ್ದಳತೆ=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR)
        {
            char16_t ಬಫರ್[1024];

            dest_ಎಲ್ಲೆ=ಬಫರ್+UPRV_LENGTHOF(ಬಫರ್);
            ಮಾಡು {
                dest=ಬಫರ್;
                ಬಫರ್_ಸ್ಥಿತಿ=U_ZERO_ERROR;
                ucnv_ಗಮ್ಯ_Unicode(cnv, &dest, dest_ಎಲ್ಲೆ, &src, src_ಎಲ್ಲೆ, ಶೂನ್ಯಸೂಚಕ, ನಿಜ, &ಬಫರ್_ಸ್ಥಿತಿ);
                dest_ಉದ್ದಳತೆ+=(int32_t)(dest-ಬಫರ್);
            }
            ಆಗ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR);
        }
        ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
            *p_Error_ಸಂಕೇತ = ಬಫರ್_ಸ್ಥಿತಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        dest_ಉದ್ದಳತೆ=0;
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(originalDest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_ಮುಂದಿನ_UChar(UConverter *cnv,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ,
                  UError_ಸಂಕೇತ *err) {
    UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು ವಾದಗಳು_2;
    char16_t ಬಫರ್[U16_MAX_ಉದ್ದಳತೆ];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    ಆದರೆ(err==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    ಆದರೆ(cnv==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ==ಶೂನ್ಯಸೂಚಕ) {
        *err=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    s=*ಮೂಲ;
    ಆದರೆ(ಮೂಲ_ಎಲ್ಲೆ<s) {
        *err=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0xffff;
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
    ಆದರೆ(((size_t)(ಮೂಲ_ಎಲ್ಲೆ-s)>(size_t)0x7fffffff && ಮೂಲ_ಎಲ್ಲೆ>s)) {
        *err=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    ಆದರೆ(cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ>0) {
        char16_t *ಅತಿಪ್ರವಾಹ;

        ಅತಿಪ್ರವಾಹ=cnv->UChar_Error_ಬಫರ್;
        i=0;
        length=cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ;
        U16_ಮುಂದಿನ(ಅತಿಪ್ರವಾಹ, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        ಆದರೆ((cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_ಬಫರ್, cnv->UChar_Error_ಬಫರ್+i,
                         cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ*U_SIZEOF_UCHAR);
        }

        ಆದರೆ(!U16_IS_LEAD(c) || i<length) {
            ಹಿಂತಿರುಗಿ c;
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
    ವಾದಗಳು_2.converter=cnv;
    ವಾದಗಳು_2.flush=ನಿಜ;
    ವಾದಗಳು_2.offsets=ಶೂನ್ಯಸೂಚಕ;
    ವಾದಗಳು_2.ಮೂಲ=s;
    ವಾದಗಳು_2.ಮೂಲ_ಎಲ್ಲೆ=ಮೂಲ_ಎಲ್ಲೆ;
    ವಾದಗಳು_2.ಗುರಿ=ಬಫರ್;
    ವಾದಗಳು_2.ಗುರಿ_ಎಲ್ಲೆ=ಬಫರ್+1;
    ವಾದಗಳು_2.size=ಗಾತ್ರ(ವಾದಗಳು_2);

    ಆದರೆ(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        ಆದರೆ(cnv->ಗಮ್ಯ_ULength==0 && cnv->shared_ದತ್ತಾಂಶ->impl->get_ಮುಂದಿನ_UChar!=ಶೂನ್ಯಸೂಚಕ) {
            c=cnv->shared_ದತ್ತಾಂಶ->impl->get_ಮುಂದಿನ_UChar(&ವಾದಗಳು_2, err);
            *ಮೂಲ=s=ವಾದಗಳು_2.ಮೂಲ;
            ಆದರೆ(*err==U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_ಗಮ್ಯ_UNICODE, ಸುಳ್ಳು);
                ಹಿಂತಿರುಗಿ 0xffff; /* no output */
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_SUCCESS(*err) && c>=0) {
                ಹಿಂತಿರುಗಿ c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        ಗಮ್ಯ_Unicode_With_Callback(&ವಾದಗಳು_2, err);

        ಆದರೆ(*err==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(ವಾದಗಳು_2.ಗುರಿ-ಬಫರ್);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* write the lead surrogate from the overflow buffer */
        ಬಫರ್[0]=(char16_t)c;
        ವಾದಗಳು_2.ಗುರಿ=ಬಫರ್+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    ಆದರೆ(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length==0) {
        /* no input or only state changes */
        *err=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } ಇಲ್ಲದಿದ್ದರೆ {
        c=ಬಫರ್[0];
        i=1;
        ಆದರೆ(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            ಆದರೆ(cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ>0) {
                /* got overflow output from the conversion */
                ಆದರೆ(U16_IS_TRAIL(c2=cnv->UChar_Error_ಬಫರ್[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    ಆದರೆ((--cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ)>0) {
                        uprv_memmove(cnv->UChar_Error_ಬಫರ್, cnv->UChar_Error_ಬಫರ್+1,
                                     cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ*U_SIZEOF_UCHAR);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ವಾದಗಳು_2.ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                /* convert once more, to buffer[1] */
                ವಾದಗಳು_2.ಗುರಿ_ಎಲ್ಲೆ=ಬಫರ್+2;
                ಗಮ್ಯ_Unicode_With_Callback(&ವಾದಗಳು_2, err);
                ಆದರೆ(*err==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(ವಾದಗಳು_2.ಗುರಿ-ಬಫರ್);
                ಆದರೆ(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=ಬಫರ್[1])) {
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
    ಆದರೆ(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        ಆದರೆ((length=cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ)>0) {
            uprv_memmove(cnv->UChar_Error_ಬಫರ್+delta, cnv->UChar_Error_ಬಫರ್,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=(int8_t)(length+delta);

        cnv->UChar_Error_ಬಫರ್[0]=ಬಫರ್[i++];
        ಆದರೆ(delta>1) {
            cnv->UChar_Error_ಬಫರ್[1]=ಬಫರ್[i];
        }
    }

    *ಮೂಲ=ವಾದಗಳು_2.ಮೂಲ;
    ಹಿಂತಿರುಗಿ c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_convertEx(UConverter *ಗುರಿ_Cnv, UConverter *ಮೂಲ_Cnv,
               ಅಕ್ಷರ **ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ,
               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ,
               char16_t *ಅಚ್ಚು_ಆರಂಭ, char16_t **ಅಚ್ಚು_ಮೂಲ,
               char16_t **ಅಚ್ಚು_ಗುರಿ, ಸ್ಥಿರಾಂಕ char16_t *ಅಚ್ಚು_ಎಲ್ಲೆ,
               UBool reset, UBool flush,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    char16_t ಅಚ್ಚು_ಬಫರ್[CHUNK_ಗಾತ್ರ];
    ಸ್ಥಿರಾಂಕ char16_t *my_ಅಚ್ಚು_ಮೂಲ;
    char16_t *my_ಅಚ್ಚು_ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
    ಅಕ್ಷರ *t;

    UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು ಗಮ್ಯ_UArgs;
    UConverter_From_Unicode_ವಾದಗಳು fromUArgs;
    UConverterConvert convert;

    /* error checking */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ( ಗುರಿ_Cnv==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ_Cnv==ಶೂನ್ಯಸೂಚಕ ||
        ಮೂಲ==ಶೂನ್ಯಸೂಚಕ || *ಮೂಲ==ಶೂನ್ಯಸೂಚಕ ||
        ಗುರಿ==ಶೂನ್ಯಸೂಚಕ || *ಗುರಿ==ಶೂನ್ಯಸೂಚಕ || ಗುರಿ_ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    s=*ಮೂಲ;
    t=*ಗುರಿ;
    ಆದರೆ((ಮೂಲ_ಎಲ್ಲೆ!=ಶೂನ್ಯಸೂಚಕ && ಮೂಲ_ಎಲ್ಲೆ<s) || ಗುರಿ_ಎಲ್ಲೆ<t) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    ಆದರೆ(
        (ಮೂಲ_ಎಲ್ಲೆ!=ಶೂನ್ಯಸೂಚಕ && ((size_t)(ಮೂಲ_ಎಲ್ಲೆ-s)>(size_t)0x7fffffff && ಮೂಲ_ಎಲ್ಲೆ>s)) ||
        ((size_t)(ಗುರಿ_ಎಲ್ಲೆ-t)>(size_t)0x7fffffff && ಗುರಿ_ಎಲ್ಲೆ>t)
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    
    ಆದರೆ(ಅಚ್ಚು_ಆರಂಭ==ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        /* use the stack pivot buffer */
        my_ಅಚ್ಚು_ಮೂಲ=my_ಅಚ್ಚು_ಗುರಿ=ಅಚ್ಚು_ಆರಂಭ=ಅಚ್ಚು_ಬಫರ್;
        ಅಚ್ಚು_ಮೂಲ=(char16_t **)&my_ಅಚ್ಚು_ಮೂಲ;
        ಅಚ್ಚು_ಗುರಿ=&my_ಅಚ್ಚು_ಗುರಿ;
        ಅಚ್ಚು_ಎಲ್ಲೆ=ಅಚ್ಚು_ಬಫರ್+CHUNK_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(  ಅಚ್ಚು_ಆರಂಭ>=ಅಚ್ಚು_ಎಲ್ಲೆ ||
                ಅಚ್ಚು_ಮೂಲ==ಶೂನ್ಯಸೂಚಕ || *ಅಚ್ಚು_ಮೂಲ==ಶೂನ್ಯಸೂಚಕ ||
                ಅಚ್ಚು_ಗುರಿ==ಶೂನ್ಯಸೂಚಕ || *ಅಚ್ಚು_ಗುರಿ==ಶೂನ್ಯಸೂಚಕ ||
                ಅಚ್ಚು_ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(ಮೂಲ_ಎಲ್ಲೆ==ಶೂನ್ಯಸೂಚಕ) {
        /* get limit of single-byte-NUL-terminated source string */
        ಮೂಲ_ಎಲ್ಲೆ=uprv_strchr(*ಮೂಲ, 0);
    }

    ಆದರೆ(reset) {
        ucnv_reset_ಗಮ್ಯ_Unicode(ಮೂಲ_Cnv);
        ucnv_resetFromUnicode(ಗುರಿ_Cnv);
        *ಅಚ್ಚು_ಮೂಲ=*ಅಚ್ಚು_ಗುರಿ=ಅಚ್ಚು_ಆರಂಭ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗುರಿ_Cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ>0) {
        /* output the targetCnv overflow buffer */
        ಆದರೆ(ucnv_output_ಅತಿಪ್ರವಾಹ_From_Unicode(ಗುರಿ_Cnv, ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            ಹಿಂತಿರುಗಿ;
        }
        /* *target has moved, therefore stop using t */

        ಆದರೆ( !flush &&
            ಗುರಿ_Cnv->preFromULength>=0 && *ಅಚ್ಚು_ಮೂಲ==*ಅಚ್ಚು_ಗುರಿ &&
            ಮೂಲ_Cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ==0 && ಮೂಲ_Cnv->pre_ಗಮ್ಯ_ULength>=0 && s==ಮೂಲ_ಎಲ್ಲೆ
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            ಹಿಂತಿರುಗಿ;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    ಆದರೆ( ಮೂಲ_Cnv->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->conversionType==UCNV_UTF8 &&
        ಗುರಿ_Cnv->shared_ದತ್ತಾಂಶ->impl->fromUTF8!=ಶೂನ್ಯಸೂಚಕ
    ) {
        convert=ಗುರಿ_Cnv->shared_ದತ್ತಾಂಶ->impl->fromUTF8;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( ಗುರಿ_Cnv->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->conversionType==UCNV_UTF8 &&
               ಮೂಲ_Cnv->shared_ದತ್ತಾಂಶ->impl->ಗಮ್ಯ_UTF8!=ಶೂನ್ಯಸೂಚಕ
    ) {
        convert=ಮೂಲ_Cnv->shared_ದತ್ತಾಂಶ->impl->ಗಮ್ಯ_UTF8;
    } ಇಲ್ಲದಿದ್ದರೆ {
        convert=ಶೂನ್ಯಸೂಚಕ;
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
    ಆದರೆ(convert!=ಶೂನ್ಯಸೂಚಕ && (ಅಚ್ಚು_ಎಲ್ಲೆ-ಅಚ್ಚು_ಆರಂಭ)>32) {
        ಅಚ್ಚು_ಎಲ್ಲೆ=ಅಚ್ಚು_ಆರಂಭ+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=ಗುರಿ_Cnv;
    fromUArgs.flush=ಸುಳ್ಳು;
    fromUArgs.offsets=ಶೂನ್ಯಸೂಚಕ;
    fromUArgs.ಗುರಿ=*ಗುರಿ;
    fromUArgs.ಗುರಿ_ಎಲ್ಲೆ=ಗುರಿ_ಎಲ್ಲೆ;
    fromUArgs.size=ಗಾತ್ರ(fromUArgs);

    ಗಮ್ಯ_UArgs.converter=ಮೂಲ_Cnv;
    ಗಮ್ಯ_UArgs.flush=flush;
    ಗಮ್ಯ_UArgs.offsets=ಶೂನ್ಯಸೂಚಕ;
    ಗಮ್ಯ_UArgs.ಮೂಲ=s;
    ಗಮ್ಯ_UArgs.ಮೂಲ_ಎಲ್ಲೆ=ಮೂಲ_ಎಲ್ಲೆ;
    ಗಮ್ಯ_UArgs.ಗುರಿ_ಎಲ್ಲೆ=ಅಚ್ಚು_ಎಲ್ಲೆ;
    ಗಮ್ಯ_UArgs.size=ಗಾತ್ರ(ಗಮ್ಯ_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=ಶೂನ್ಯಸೂಚಕ;

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
    ಗಾಗಿ(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        ಆದರೆ( *ಅಚ್ಚು_ಮೂಲ<*ಅಚ್ಚು_ಗುರಿ ||
            U_FAILURE(*p_Error_ಸಂಕೇತ) ||
            ಗುರಿ_Cnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.ಮೂಲ=*ಅಚ್ಚು_ಮೂಲ;
            fromUArgs.ಮೂಲ_ಎಲ್ಲೆ=*ಅಚ್ಚು_ಗುರಿ;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                /* target overflow, or conversion error */
                *ಅಚ್ಚು_ಮೂಲ=(char16_t *)fromUArgs.ಮೂಲ;
                ನಿಲ್ಲಿಸು;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *ಅಚ್ಚು_ಮೂಲ=*ಅಚ್ಚು_ಗುರಿ=ಅಚ್ಚು_ಆರಂಭ;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        ಆದರೆ(ಮೂಲ_Cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ>0) {
            ಆದರೆ(ucnv_output_ಅತಿಪ್ರವಾಹ_ಗಮ್ಯ_Unicode(ಮೂಲ_Cnv, ಅಚ್ಚು_ಗುರಿ, ಅಚ್ಚು_ಎಲ್ಲೆ, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
            }
            ಮುಂದುವರಿ;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        ಆದರೆ( ಗಮ್ಯ_UArgs.ಮೂಲ==ಮೂಲ_ಎಲ್ಲೆ &&
            ಮೂಲ_Cnv->pre_ಗಮ್ಯ_ULength>=0 && ಮೂಲ_Cnv->ಗಮ್ಯ_ULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            ನಿಲ್ಲಿಸು;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        ಆದರೆ(convert!=ಶೂನ್ಯಸೂಚಕ && ಗುರಿ_Cnv->preFromUFirstCP<0 && ಮೂಲ_Cnv->pre_ಗಮ್ಯ_ULength==0) {
            ಆದರೆ(*p_Error_ಸಂಕೇತ==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &ಗಮ್ಯ_UArgs, p_Error_ಸಂಕೇತ);
            ಆದರೆ(*p_Error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                ಆದರೆ(ಮೂಲ_Cnv->ಗಮ್ಯ_ULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *ಅಚ್ಚು_ಮೂಲ=*ಅಚ್ಚು_ಗುರಿ=ಅಚ್ಚು_ಆರಂಭ+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*p_Error_ಸಂಕೇತ==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flush && ಮೂಲ_Cnv->ಗಮ್ಯ_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_ಸಂಕೇತ=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* input consumed */
                ಆದರೆ(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(ಮೂಲ_Cnv, UCNV_RESET_ಗಮ್ಯ_UNICODE, ಸುಳ್ಳು);
                    _reset(ಗುರಿ_Cnv, UCNV_RESET_FROM_UNICODE, ಸುಳ್ಳು);
                }

                /* done successfully */
                ನಿಲ್ಲಿಸು;
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
        ಗಮ್ಯ_UArgs.ಗುರಿ=ಅಚ್ಚು_ಆರಂಭ; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        ಗಮ್ಯ_Unicode_With_Callback(&ಗಮ್ಯ_UArgs, p_Error_ಸಂಕೇತ);
        *ಅಚ್ಚು_ಗುರಿ=ಗಮ್ಯ_UArgs.ಗುರಿ;
        ಆದರೆ(*p_Error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ) || (!flush && *ಅಚ್ಚು_ಗುರಿ==ಅಚ್ಚು_ಆರಂಭ)) {
            /* conversion error, or there was nothing left to convert */
            ನಿಲ್ಲಿಸು;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        ಆದರೆ( flush && ಗಮ್ಯ_UArgs.ಮೂಲ==ಮೂಲ_ಎಲ್ಲೆ &&
            ಮೂಲ_Cnv->pre_ಗಮ್ಯ_ULength>=0 &&
            ಮೂಲ_Cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ==0
        ) {
            fromUArgs.flush=ನಿಜ;
        }
    }

    /*
     * The conversion loop is exited when one of the following is true:
     * - the entire source text has been converted successfully to the target buffer
     * - a target buffer overflow occurred
     * - a conversion error occurred
     */

    *ಮೂಲ=ಗಮ್ಯ_UArgs.ಮೂಲ;
    *ಗುರಿ=fromUArgs.ಗುರಿ;

    /* terminate the target buffer if possible */
    ಆದರೆ(flush && U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        ಆದರೆ(*ಗುರಿ!=ಗುರಿ_ಎಲ್ಲೆ) {
            **ಗುರಿ=0;
            ಆದರೆ(*p_Error_ಸಂಕೇತ==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            *p_Error_ಸಂಕೇತ=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
ಸ್ಥಿರ int32_t
ucnv_ಆಂತರಿಕ_Convert(UConverter *ಹೊರಹೋಗು_Converter, UConverter *ಒಳ_Converter,
                     ಅಕ್ಷರ *ಗುರಿ, int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    char16_t ಅಚ್ಚು_ಬಫರ್[CHUNK_ಗಾತ್ರ];
    char16_t *ಅಚ್ಚು, *pivot2;

    ಅಕ್ಷರ *my_ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t ಗುರಿ_ಉದ್ದಳತೆ=0;

    /* set up */
    ಆದರೆ(ಮೂಲ_ಉದ್ದಳತೆ<0) {
        ಮೂಲ_ಎಲ್ಲೆ=uprv_strchr(ಮೂಲ, 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಮೂಲ_ಎಲ್ಲೆ=ಮೂಲ+ಮೂಲ_ಉದ್ದಳತೆ;
    }

    /* if there is no input data, we're done */
    ಆದರೆ(ಮೂಲ==ಮೂಲ_ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ u_terminateChars(ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ, 0, p_Error_ಸಂಕೇತ);
    }

    ಅಚ್ಚು=pivot2=ಅಚ್ಚು_ಬಫರ್;
    my_ಗುರಿ=ಗುರಿ;
    ಗುರಿ_ಉದ್ದಳತೆ=0;

    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        /* perform real conversion */
        ಗುರಿ_ಎಲ್ಲೆ=ಗುರಿ+ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        ucnv_convertEx(ಹೊರಹೋಗು_Converter, ಒಳ_Converter,
                       &my_ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
                       &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                       ಅಚ್ಚು_ಬಫರ್, &ಅಚ್ಚು, &pivot2, ಅಚ್ಚು_ಬಫರ್+CHUNK_ಗಾತ್ರ,
                       ಸುಳ್ಳು,
                       ನಿಜ,
                       p_Error_ಸಂಕೇತ);
        ಗುರಿ_ಉದ್ದಳತೆ = static_cast<int32_t>(my_ಗುರಿ - ಗುರಿ);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    ಆದರೆ(*p_Error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR || ಗುರಿ_ಸಾಮರ್ಥ್ಯ==0)
    {
        ಅಕ್ಷರ ಗುರಿ_ಬಫರ್[CHUNK_ಗಾತ್ರ];

        ಗುರಿ_ಎಲ್ಲೆ=ಗುರಿ_ಬಫರ್+CHUNK_ಗಾತ್ರ;
        ಮಾಡು {
            *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
            my_ಗುರಿ=ಗುರಿ_ಬಫರ್;
            ucnv_convertEx(ಹೊರಹೋಗು_Converter, ಒಳ_Converter,
                           &my_ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
                           &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                           ಅಚ್ಚು_ಬಫರ್, &ಅಚ್ಚು, &pivot2, ಅಚ್ಚು_ಬಫರ್+CHUNK_ಗಾತ್ರ,
                           ಸುಳ್ಳು,
                           ನಿಜ,
                           p_Error_ಸಂಕೇತ);
            ಗುರಿ_ಉದ್ದಳತೆ += static_cast<int32_t>(my_ಗುರಿ - ಗುರಿ_ಬಫರ್);
        } ಆಗ(*p_Error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        ಹಿಂತಿರುಗಿ u_terminateChars(ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ, ಗುರಿ_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    ಹಿಂತಿರುಗಿ ಗುರಿ_ಉದ್ದಳತೆ;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗಮ್ಯ_Converter_Name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fromConverterName,
             ಅಕ್ಷರ *ಗುರಿ, int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter in, out; /* stack-allocated */
    UConverter *ಒಳ_Converter, *ಹೊರಹೋಗು_Converter;
    int32_t ಗುರಿ_ಉದ್ದಳತೆ;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ( ಮೂಲ==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ_ಉದ್ದಳತೆ<-1 ||
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ<0 || (ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0 && ಗುರಿ==ಶೂನ್ಯಸೂಚಕ)
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* if there is no input data, we're done */
    ಆದರೆ(ಮೂಲ_ಉದ್ದಳತೆ==0 || (ಮೂಲ_ಉದ್ದಳತೆ<0 && *ಮೂಲ==0)) {
        ಹಿಂತಿರುಗಿ u_terminateChars(ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ, 0, p_Error_ಸಂಕೇತ);
    }

    /* create the converters */
    ಒಳ_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಹೊರಹೋಗು_Converter=ucnv_createConverter(&out, ಗಮ್ಯ_Converter_Name, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ucnv_close(ಒಳ_Converter);
        ಹಿಂತಿರುಗಿ 0;
    }

    ಗುರಿ_ಉದ್ದಳತೆ=ucnv_ಆಂತರಿಕ_Convert(ಹೊರಹೋಗು_Converter, ಒಳ_Converter,
                                      ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                                      ಮೂಲ, ಮೂಲ_ಉದ್ದಳತೆ,
                                      p_Error_ಸಂಕೇತ);

    ucnv_close(ಒಳ_Converter);
    ucnv_close(ಹೊರಹೋಗು_Converter);

    ಹಿಂತಿರುಗಿ ಗುರಿ_ಉದ್ದಳತೆ;
}

/* @internal */
ಸ್ಥಿರ int32_t
ucnv_convertAlgorithmic(UBool convert_ಗಮ್ಯ_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        ಅಕ್ಷರ *ಗುರಿ, int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
                        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *ಗಮ್ಯ, *from;
    int32_t ಗುರಿ_ಉದ್ದಳತೆ;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ( cnv==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ_ಉದ್ದಳತೆ<-1 ||
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ<0 || (ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0 && ಗುರಿ==ಶೂನ್ಯಸೂಚಕ)
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* if there is no input data, we're done */
    ಆದರೆ(ಮೂಲ_ಉದ್ದಳತೆ==0 || (ಮೂಲ_ಉದ್ದಳತೆ<0 && *ಮೂಲ==0)) {
        ಹಿಂತಿರುಗಿ u_terminateChars(ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ, 0, p_Error_ಸಂಕೇತ);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* reset the other converter */
    ಆದರೆ(convert_ಗಮ್ಯ_Algorithmic) {
        /* cnv->Unicode->algo */
        ucnv_reset_ಗಮ್ಯ_Unicode(cnv);
        ಗಮ್ಯ=algoConverter;
        from=cnv;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        ಗಮ್ಯ=cnv;
    }

    ಗುರಿ_ಉದ್ದಳತೆ=ucnv_ಆಂತರಿಕ_Convert(ಗಮ್ಯ, from,
                                      ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                                      ಮೂಲ, ಮೂಲ_ಉದ್ದಳತೆ,
                                      p_Error_ಸಂಕೇತ);

    ucnv_close(algoConverter);

    ಹಿಂತಿರುಗಿ ಗುರಿ_ಉದ್ದಳತೆ;
}

U_CAPI int32_t U_EXPORT2
ucnv_ಗಮ್ಯ_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   ಅಕ್ಷರ *ಗುರಿ, int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ucnv_convertAlgorithmic(ನಿಜ, algorithmicType, cnv,
                                   ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                                   ಮೂಲ, ಮೂಲ_ಉದ್ದಳತೆ,
                                   p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     ಅಕ್ಷರ *ಗುರಿ, int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ, int32_t ಮೂಲ_ಉದ್ದಳತೆ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) UPRV_NO_SANITIZE_UNDEFINED {

    ಆದರೆ(algorithmicType<0 || UCNV_ಸಂಖ್ಯೆ_ದ_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ ucnv_convertAlgorithmic(ಸುಳ್ಳು, algorithmicType, cnv,
                                   ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                                   ಮೂಲ, ಮೂಲ_ಉದ್ದಳತೆ,
                                   p_Error_ಸಂಕೇತ);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ಸ್ಥಿರಾಂಕ UConverter* converter)
{
    int8_t type = converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->conversionType;
#ಆದರೆ !UCONFIG_NO_LEGACY_CONVERSION
    ಆದರೆ(type == UCNV_MBCS) {
        ಹಿಂತಿರುಗಿ ucnv_MBCSGetType(converter);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ (UConverterType)type;
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_getStarters(ಸ್ಥಿರಾಂಕ UConverter* converter, 
                 UBool starters[256],
                 UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (err == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(converter->shared_ದತ್ತಾಂಶ->impl->getStarters != ಶೂನ್ಯಸೂಚಕ) {
        converter->shared_ದತ್ತಾಂಶ->impl->getStarters(converter, starters, err);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *err = U_ILLEGAL_ವಾದ_ERROR;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UAmbiguousConverter *ucnv_getAmbiguous(ಸ್ಥಿರಾಂಕ UConverter *cnv)
{
    UError_ಸಂಕೇತ error_ಸಂಕೇತ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
    int32_t i;

    ಆದರೆ(cnv==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    error_ಸಂಕೇತ=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಗಾಗಿ(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        ಆದರೆ(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            ಹಿಂತಿರುಗಿ ambiguousConverters+i;
        }
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_fixFileSeparator(ಸ್ಥಿರಾಂಕ UConverter *cnv, 
                      char16_t* ಮೂಲ,
                      int32_t ಮೂಲ_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    ಆದರೆ(cnv==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ==ಶೂನ್ಯಸೂಚಕ || ಮೂಲ_ಉದ್ದಳತೆ<=0 || (a=ucnv_getAmbiguous(cnv))==ಶೂನ್ಯಸೂಚಕ)
    {
        ಹಿಂತಿರುಗಿ;
    }

    variant5c=a->variant5c;
    ಗಾಗಿ(i=0; i<ಮೂಲ_ಉದ್ದಳತೆ; ++i) {
        ಆದರೆ(ಮೂಲ[i]==variant5c) {
            ಮೂಲ[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಹಿಂತಿರುಗಿ ucnv_getAmbiguous(cnv)!=ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_ಗಣ_Fallback(UConverter *cnv, UBool ಬಳಸುತ್ತದೆ_Fallback)
{
    cnv->useFallback = ಬಳಸುತ್ತದೆ_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_ಬಳಸುತ್ತದೆ_Fallback(ಸ್ಥಿರಾಂಕ UConverter *cnv)
{
    ಹಿಂತಿರುಗಿ cnv->useFallback;
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_getInvalidChars (ಸ್ಥಿರಾಂಕ UConverter * converter,
                      ಅಕ್ಷರ *err_ಬೈಟುಗಳು,
                      int8_t * len,
                      UError_ಸಂಕೇತ * err)
{
    ಆದರೆ (err == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err))
    {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (len == ಶೂನ್ಯಸೂಚಕ || err_ಬೈಟುಗಳು == ಶೂನ್ಯಸೂಚಕ || converter == ಶೂನ್ಯಸೂಚಕ)
    {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (*len < converter->invalid_Char_ಉದ್ದಳತೆ)
    {
        *err = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ ((*len = converter->invalid_Char_ಉದ್ದಳತೆ) > 0)
    {
        uprv_memcpy (err_ಬೈಟುಗಳು, converter->invalid_Char_ಬಫರ್, *len);
    }
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_getInvalidUChars (ಸ್ಥಿರಾಂಕ UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_ಸಂಕೇತ * err)
{
    ಆದರೆ (err == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err))
    {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (len == ಶೂನ್ಯಸೂಚಕ || errChars == ಶೂನ್ಯಸೂಚಕ || converter == ಶೂನ್ಯಸೂಚಕ)
    {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (*len < converter->invalid_UChar_ಉದ್ದಳತೆ)
    {
        *err = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ ((*len = converter->invalid_UChar_ಉದ್ದಳತೆ) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_ಬಫರ್, *len);
    }
}

#ವ್ಯಾಖ್ಯಾನಿಸು SIG_MAX_LEN 5

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ucnv_detectUnicodeSignature( ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ,
                             int32_t ಮೂಲ_ಉದ್ದಳತೆ,
                             int32_t* signature_ಉದ್ದಳತೆ,
                             UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    ಅಕ್ಷರ ಆರಂಭ[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    ಪೂರ್ಣಾಂಕ i = 0;

    ಆದರೆ((p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ) || U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    
    ಆದರೆ(ಮೂಲ == ಶೂನ್ಯಸೂಚಕ || ಮೂಲ_ಉದ್ದಳತೆ < -1){
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(signature_ಉದ್ದಳತೆ == ಶೂನ್ಯಸೂಚಕ) {
        signature_ಉದ್ದಳತೆ = &dummy;
    }

    ಆದರೆ(ಮೂಲ_ಉದ್ದಳತೆ==-1){
        ಮೂಲ_ಉದ್ದಳತೆ=(int32_t)uprv_strlen(ಮೂಲ);
    }

    
    ಆಗ(i<ಮೂಲ_ಉದ್ದಳತೆ&& i<SIG_MAX_LEN){
        ಆರಂಭ[i]=ಮೂಲ[i];
        i++;
    }

    ಆದರೆ(ಆರಂಭ[0] == '\xFE' && ಆರಂಭ[1] == '\xFF') {
        *signature_ಉದ್ದಳತೆ=2;
        ಹಿಂತಿರುಗಿ  "UTF-16BE";
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ[0] == '\xFF' && ಆರಂಭ[1] == '\xFE') {
        ಆದರೆ(ಆರಂಭ[2] == '\x00' && ಆರಂಭ[3] =='\x00') {
            *signature_ಉದ್ದಳತೆ=4;
            ಹಿಂತಿರುಗಿ "UTF-32LE";
        } ಇಲ್ಲದಿದ್ದರೆ {
            *signature_ಉದ್ದಳತೆ=2;
            ಹಿಂತಿರುಗಿ  "UTF-16LE";
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ[0] == '\xEF' && ಆರಂಭ[1] == '\xBB' && ಆರಂಭ[2] == '\xBF') {
        *signature_ಉದ್ದಳತೆ=3;
        ಹಿಂತಿರುಗಿ  "UTF-8";
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ[0] == '\x00' && ಆರಂಭ[1] == '\x00' && 
              ಆರಂಭ[2] == '\xFE' && ಆರಂಭ[3]=='\xFF') {
        *signature_ಉದ್ದಳತೆ=4;
        ಹಿಂತಿರುಗಿ  "UTF-32BE";
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ[0] == '\x0E' && ಆರಂಭ[1] == '\xFE' && ಆರಂಭ[2] == '\xFF') {
        *signature_ಉದ್ದಳತೆ=3;
        ಹಿಂತಿರುಗಿ "SCSU";
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ[0] == '\xFB' && ಆರಂಭ[1] == '\xEE' && ಆರಂಭ[2] == '\x28') {
        *signature_ಉದ್ದಳತೆ=3;
        ಹಿಂತಿರುಗಿ "BOCU-1";
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ[0] == '\x2B' && ಆರಂಭ[1] == '\x2F' && ಆರಂಭ[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        ಆದರೆ(ಆರಂಭ[3] == '\x38' && ಆರಂಭ[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_ಉದ್ದಳತೆ=5;
            ಹಿಂತಿರುಗಿ "UTF-7";
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ[3] == '\x38' || ಆರಂಭ[3] == '\x39' || ಆರಂಭ[3] == '\x2B' || ಆರಂಭ[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_ಉದ್ದಳತೆ=4;
            ಹಿಂತಿರುಗಿ "UTF-7";
        }
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ[0]=='\xDD' && ಆರಂಭ[1]== '\x73'&& ಆರಂಭ[2]=='\x66' && ಆರಂಭ[3]=='\x73'){
        *signature_ಉದ್ದಳತೆ=4;
        ಹಿಂತಿರುಗಿ "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_ಉದ್ದಳತೆ=0;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ಸ್ಥಿರಾಂಕ UConverter* cnv, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ(cnv == ಶೂನ್ಯಸೂಚಕ){
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ(cnv->preFromUFirstCP >= 0){
        ಹಿಂತಿರುಗಿ U16_ಉದ್ದಳತೆ(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cnv->preFromULength < 0){
        ಹಿಂತಿರುಗಿ -cnv->preFromULength ;
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cnv->fromUChar32 > 0){
        ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_ಗಮ್ಯ_UCount_Pending(ಸ್ಥಿರಾಂಕ UConverter* cnv, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){

    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ(cnv == ಶೂನ್ಯಸೂಚಕ){
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ(cnv->pre_ಗಮ್ಯ_ULength > 0){
        ಹಿಂತಿರುಗಿ cnv->pre_ಗಮ್ಯ_ULength ;
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cnv->pre_ಗಮ್ಯ_ULength < 0){
        ಹಿಂತಿರುಗಿ -cnv->pre_ಗಮ್ಯ_ULength;
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cnv->ಗಮ್ಯ_ULength > 0){
        ಹಿಂತಿರುಗಿ cnv->ಗಮ್ಯ_ULength;
    }
    ಹಿಂತಿರುಗಿ 0;
}

U_CAPI UBool U_EXPORT2
ucnv_is_Fixed_ಅಗಲ(UConverter *cnv, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2){
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ (cnv == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆಯ್ಕೆ (ucnv_getType(cnv)) {
        ಸ್ಥಿತಿ UCNV_SBCS:
        ಸ್ಥಿತಿ UCNV_DBCS:
        ಸ್ಥಿತಿ UCNV_UTF32_BigEndian:
        ಸ್ಥಿತಿ UCNV_UTF32_LittleEndian:
        ಸ್ಥಿತಿ UCNV_UTF32:
        ಸ್ಥಿತಿ UCNV_US_ASCII:
            ಹಿಂತಿರುಗಿ ನಿಜ;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
