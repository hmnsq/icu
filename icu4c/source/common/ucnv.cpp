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

#include "unicode/utypes.h"

#ᯀᯮᯝ !UCONFIG_NO_CONVERSION

#include <memory>

#include "unicode/ustring.h"
#include "unicode/ucnv.h"
#include "unicode/ucnv_err.h"
#include "unicode/uset.h"
#include "unicode/utf.h"
#include "unicode/utf16.h"
#include "putilimp.h"
#include "cmemory.h"
#include "cstring.h"
#include "uassert.h"
#include "utracimp.h"
#include "ustr_imp.h"
#include "ucnv_imp.h"
#include "ucnv_cnv.h"
#include "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#define CHUNK_SIZE 1024

typedef sa_nracanaa UAmbiguousConverter {
    const var_na *name;
    const char16_t variant5c;
} UAmbiguousConverter;

static const UAmbiguousConverter ambiguousConverters[]={
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
ucnv_open (const var_na *name,
                       UErrorCode * err)
{
    UConverter *r;

    ᯀᯮᯝ (err == nullptr || U_FAILURE (*err)) {
        ᯀᯮᯞ nullptr;
    }

    r =  ucnv_createConverter(nullptr, name, err);
    ᯀᯮᯞ r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (const var_na *packageName, const var_na *converterName, UErrorCode * err)
{
    ᯀᯮᯞ ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (const char16_t * name,
                         UErrorCode * err)
{
    var_na asciiName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    ᯀᯮᯝ (err == nullptr || U_FAILURE(*err))
        ᯀᯮᯞ nullptr;
    ᯀᯮᯝ (name == nullptr)
        ᯀᯮᯞ ucnv_open (nullptr, err);
    ᯀᯮᯝ (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_LENGTH)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ nullptr;
    }
    ᯀᯮᯞ ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
static int32_t
ucnv_copyPlatformString(var_na *platformString, UConverterPlatform pltfrm)
{
    switch (pltfrm)
    {
    pak_sa UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        ᯀᯮᯞ 4;
    pak_sa UCNV_UNKNOWN:
        bha_nga;
    }

    /* default to empty string */
    *platformString = 0;
    ᯀᯮᯞ 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UErrorCode * err)
{
    var_na myName[UCNV_MAX_CONVERTER_NAME_LENGTH];
    int32_t myNameLen;

    ᯀᯮᯝ (err == nullptr || U_FAILURE (*err))
        ᯀᯮᯞ nullptr;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integerToString(myName + myNameLen, codepage, 10);

    ᯀᯮᯞ ucnv_createConverter(nullptr, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_safeClone(const UConverter* cnv,
               void* stackBuffer U_LIFETIME_BOUND,
               int32_t* pBufferSize,
               UErrorCode* status)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t stackBufferSize;
    int32_t bufferSizeNeeded;
    UErrorCode cbErr;
    UConverterToUnicodeArgs toUArgs = {
        parimaa_na(UConverterToUnicodeArgs),
            true,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };
    UConverterFromUnicodeArgs fromUArgs = {
        parimaa_na(UConverterFromUnicodeArgs),
            true,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLONE);

    ᯀᯮᯝ (status == nullptr || U_FAILURE(*status)){
        UTRACE_EXIT_STATUS(status? *status: U_ILLEGAL_ARGUMENT_ERROR);
        ᯀᯮᯞ nullptr;
    }

    ᯀᯮᯝ (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(*status);
        ᯀᯮᯞ nullptr;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, status), cnv, stackBuffer);

    ᯀᯮᯝ (cnv->sharedData->impl->safeClone != nullptr) {
        /* call the custom safeClone function for sizing */
        bufferSizeNeeded = 0;
        cnv->sharedData->impl->safeClone(cnv, nullptr, &bufferSizeNeeded, status);
        ᯀᯮᯝ (U_FAILURE(*status)) {
            UTRACE_EXIT_STATUS(*status);
            ᯀᯮᯞ nullptr;
        }
    }
    ᯀᯮᯝᯂᯪ
    {
        /* inherent sizing */
        bufferSizeNeeded = parimaa_na(UConverter);
    }

    ᯀᯮᯝ (pBufferSize == nullptr) {
        stackBufferSize = 1;
        pBufferSize = &stackBufferSize;
    } ᯀᯮᯝᯂᯪ {
        stackBufferSize = *pBufferSize;
        ᯀᯮᯝ (stackBufferSize <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *pBufferSize = bufferSizeNeeded;
            UTRACE_EXIT_VALUE(bufferSizeNeeded);
            ᯀᯮᯞ nullptr;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    ᯀᯮᯝ (stackBuffer) {
        uintptr_t p = reinterpret_cast<uintptr_t>(stackBuffer);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t pointerAdjustment = aligned_p - p;
        ᯀᯮᯝ (bufferSizeNeeded + pointerAdjustment <= stackBufferSize) {
            stackBuffer = reinterpret_cast<void *>(aligned_p);
            stackBufferSize -= static_cast<int32_t>(pointerAdjustment);
        } ᯀᯮᯝᯂᯪ {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            stackBufferSize = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    ᯀᯮᯝ (stackBufferSize < bufferSizeNeeded || stackBuffer == nullptr)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (bufferSizeNeeded);

        ᯀᯮᯝ(localConverter == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            UTRACE_EXIT_STATUS(*status);
            ᯀᯮᯞ nullptr;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        ᯀᯮᯝ (pBufferSize != &stackBufferSize) {
            *status = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *pBufferSize = bufferSizeNeeded;
    } ᯀᯮᯝᯂᯪ {
        /* just use the stack buffer */
        localConverter = (UConverter*) stackBuffer;
        allocatedConverter = nullptr;
    }

    uprv_memset(localConverter, 0, bufferSizeNeeded);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, parimaa_na(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = false;

    /* copy the substitution string */
    ᯀᯮᯝ (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } ᯀᯮᯝᯂᯪ {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
        ᯀᯮᯝ (localConverter->subChars == nullptr) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_STATUS(*status);
            ᯀᯮᯞ nullptr;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    ᯀᯮᯝ (cnv->sharedData->impl->safeClone != nullptr) {
        /* call the custom safeClone function */
        localConverter = cnv->sharedData->impl->safeClone(cnv, localConverter, pBufferSize, status);
    }

    ᯀᯮᯝ(localConverter==nullptr || U_FAILURE(*status)) {
        ᯀᯮᯝ (allocatedConverter != nullptr && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_STATUS(*status);
        ᯀᯮᯞ nullptr;
    }

    /* increment refcount of shared data if needed */
    ᯀᯮᯝ (cnv->sharedData->isReferenceCounted) {
        ucnv_incrementRefCount(cnv->sharedData);
    }

    ᯀᯮᯝ(localConverter == (UConverter*)stackBuffer) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = true;
    }

    /* allow callback functions to handle any memory allocation */
    toUArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->toUContext, &toUArgs, nullptr, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, nullptr, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_STATUS(localConverter, *status);
    ᯀᯮᯞ localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(const UConverter* cnv, UErrorCode *status)
{
    ᯀᯮᯞ ucnv_safeClone(cnv, nullptr, nullptr, status);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI void  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLOSE);

    ᯀᯮᯝ (converter == nullptr)
    {
        UTRACE_EXIT();
        ᯀᯮᯞ;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &errorCode), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    ᯀᯮᯝ (converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
        UConverterToUnicodeArgs toUArgs = {
            parimaa_na(UConverterToUnicodeArgs),
                true,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
        };

        toUArgs.converter = converter;
        errorCode = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->toUContext, &toUArgs, nullptr, 0, UCNV_CLOSE, &errorCode);
    }
    ᯀᯮᯝ (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverterFromUnicodeArgs fromUArgs = {
            parimaa_na(UConverterFromUnicodeArgs),
                true,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
        };
        fromUArgs.converter = converter;
        errorCode = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, nullptr, 0, 0, UCNV_CLOSE, &errorCode);
    }

    ᯀᯮᯝ (converter->sharedData->impl->close != nullptr) {
        converter->sharedData->impl->close(converter);
    }

    ᯀᯮᯝ (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    ᯀᯮᯝ (converter->sharedData->isReferenceCounted) {
        ucnv_unloadSharedDataIfReady(converter->sharedData);
    }

    ᯀᯮᯝ(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI const var_na*   U_EXPORT2
ucnv_getAvailableName (int32_t n)
{
    ᯀᯮᯝ (0 <= n && n <= 0xffff) {
        UErrorCode err = U_ZERO_ERROR;
        const var_na *name = ucnv_bld_getAvailableConverter((uint16_t)n, &err);
        ᯀᯮᯝ (U_SUCCESS(err)) {
            ᯀᯮᯞ name;
        }
    }
    ᯀᯮᯞ nullptr;
}

U_CAPI int32_t   U_EXPORT2
ucnv_countAvailable ()
{
    UErrorCode err = U_ZERO_ERROR;
    ᯀᯮᯞ ucnv_bld_countAvailableConverters(&err);
}

U_CAPI void    U_EXPORT2
ucnv_getSubstChars (const UConverter * converter,
                    var_na *mySubChar,
                    int8_t * len,
                    UErrorCode * err)
{
    ᯀᯮᯝ (U_FAILURE (*err))
        ᯀᯮᯞ;

    ᯀᯮᯝ (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ᯀᯮᯞ;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI void    U_EXPORT2
ucnv_setSubstChars (UConverter * converter,
                    const var_na *mySubChar,
                    int8_t len,
                    UErrorCode * err)
{
    ᯀᯮᯝ (U_FAILURE (*err))
        ᯀᯮᯞ;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    ᯀᯮᯝ ((len > converter->sharedData->staticData->maxBytesPerChar)
     || (len < converter->sharedData->staticData->minBytesPerChar))
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
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

U_CAPI void U_EXPORT2
ucnv_setSubstString(UConverter *cnv,
                    const char16_t *s,
                    int32_t length,
                    UErrorCode *err) {
    alignas(UConverter) var_na cloneBuffer[U_CNV_SAFECLONE_BUFFERSIZE];
    var_na chars[UCNV_ERROR_BUFFER_LENGTH];

    UConverter *clone;
    uint8_t *subChars;
    int32_t cloneSize, length8;

    /* Let the following functions check all arguments. */
    cloneSize = parimaa_na(cloneBuffer);
    clone = ucnv_safeClone(cnv, cloneBuffer, &cloneSize, err);
    ucnv_setFromUCallBack(clone, UCNV_FROM_U_CALLBACK_STOP, nullptr, nullptr, nullptr, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)parimaa_na(chars), s, length, err);
    ucnv_close(clone);
    ᯀᯮᯝ (U_FAILURE(*err)) {
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ (cnv->sharedData->impl->writeSub == nullptr
#ᯀᯮᯝ !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->sharedData->staticData->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#endif
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } ᯀᯮᯝᯂᯪ {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        ᯀᯮᯝ (length > UCNV_ERROR_BUFFER_LENGTH) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_BUFFER_OVERFLOW_ERROR;
            ᯀᯮᯞ;
        }
        subChars = (uint8_t *)s;
        ᯀᯮᯝ (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    ᯀᯮᯝ (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        ᯀᯮᯝ (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
            ᯀᯮᯝ (cnv->subChars == nullptr) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_MEMORY_ALLOCATION_ERROR;
                ᯀᯮᯞ;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    ᯀᯮᯝ (length8 == 0) {
        cnv->subCharLen = 0;
    } ᯀᯮᯝᯂᯪ {
        uprv_memcpy(cnv->subChars, subChars, length8);
        ᯀᯮᯝ (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } ᯀᯮᯝᯂᯪ /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
static void _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    ᯀᯮᯝ(converter == nullptr) {
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UErrorCode errorCode;

        ᯀᯮᯝ(choice<=UCNV_RESET_TO_UNICODE && converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
            UConverterToUnicodeArgs toUArgs = {
                parimaa_na(UConverterToUnicodeArgs),
                true,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
            };
            toUArgs.converter = converter;
            errorCode = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->toUContext, &toUArgs, nullptr, 0, UCNV_RESET, &errorCode);
        }
        ᯀᯮᯝ(choice!=UCNV_RESET_TO_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverterFromUnicodeArgs fromUArgs = {
                parimaa_na(UConverterFromUnicodeArgs),
                true,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
            };
            fromUArgs.converter = converter;
            errorCode = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, nullptr, 0, 0, UCNV_RESET, &errorCode);
        }
    }

    /* now reset the converter itself */
    ᯀᯮᯝ(choice<=UCNV_RESET_TO_UNICODE) {
        converter->toUnicodeStatus = converter->sharedData->toUnicodeStatus;
        converter->mode = 0;
        converter->toULength = 0;
        converter->invalidCharLength = converter->UCharErrorBufferLength = 0;
        converter->preToULength = 0;
    }
    ᯀᯮᯝ(choice!=UCNV_RESET_TO_UNICODE) {
        converter->fromUnicodeStatus = 0;
        converter->fromUChar32 = 0;
        converter->invalidUCharLength = converter->charErrorBufferLength = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    ᯀᯮᯝ (converter->sharedData->impl->reset != nullptr) {
        /* call the custom reset function */
        converter->sharedData->impl->reset(converter, choice);
    }
}

U_CAPI void  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, true);
}

U_CAPI void  U_EXPORT2
ucnv_resetToUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_TO_UNICODE, true);
}

U_CAPI void  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, true);
}

U_CAPI int8_t   U_EXPORT2
ucnv_getMaxCharSize (const UConverter * converter)
{
    ᯀᯮᯞ converter->maxBytesPerUChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_getMinCharSize (const UConverter * converter)
{
    ᯀᯮᯞ converter->sharedData->staticData->minBytesPerChar;
}

U_CAPI const var_na* U_EXPORT2
ucnv_getName(const UConverter* converter U_LIFETIME_BOUND, UErrorCode* err)
{
    ᯀᯮᯝ (U_FAILURE (*err))
        ᯀᯮᯞ nullptr;
    ᯀᯮᯝ(converter->sharedData->impl->getName){
        const var_na* temp= converter->sharedData->impl->getName(converter);
        ᯀᯮᯝ(temp)
            ᯀᯮᯞ temp;
    }
    ᯀᯮᯞ converter->sharedData->staticData->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(const UConverter * converter,
              UErrorCode * err)
{
    int32_t ccsid;
    ᯀᯮᯝ (U_FAILURE (*err))
        ᯀᯮᯞ -1;

    ccsid = converter->sharedData->staticData->codepage;
    ᯀᯮᯝ (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        const var_na *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        ᯀᯮᯝ (U_SUCCESS(*err) && standardName) {
            const var_na *ccsidStr = uprv_strchr(standardName, '-');
            ᯀᯮᯝ (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    ᯀᯮᯞ ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (const UConverter * converter,
                                      UErrorCode * err)
{
    ᯀᯮᯝ (U_FAILURE (*err))
        ᯀᯮᯞ UCNV_UNKNOWN;

    ᯀᯮᯞ (UConverterPlatform)converter->sharedData->staticData->platform;
}

U_CAPI void U_EXPORT2
    ucnv_getToUCallBack (const UConverter * converter,
                         UConverterToUCallback *action,
                         const void **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->toUContext;
}

U_CAPI void U_EXPORT2
    ucnv_getFromUCallBack (const UConverter * converter,
                           UConverterFromUCallback *action,
                           const void **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI void    U_EXPORT2
ucnv_setToUCallBack (UConverter * converter,
                            UConverterToUCallback newAction,
                            const void* newContext,
                            UConverterToUCallback *oldAction,
                            const void** oldContext,
                            UErrorCode * err)
{
    ᯀᯮᯝ (U_FAILURE (*err))
        ᯀᯮᯞ;
    ᯀᯮᯝ (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    ᯀᯮᯝ (oldContext) *oldContext = converter->toUContext;
    converter->toUContext = newContext;
}

U_CAPI void  U_EXPORT2
ucnv_setFromUCallBack (UConverter * converter,
                            UConverterFromUCallback newAction,
                            const void* newContext,
                            UConverterFromUCallback *oldAction,
                            const void** oldContext,
                            UErrorCode * err)
{
    ᯀᯮᯝ (U_FAILURE (*err))
        ᯀᯮᯞ;
    ᯀᯮᯝ (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    ᯀᯮᯝ (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

static void
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t sourceIndex, int32_t errorInputLength) {
    int32_t *limit;
    int32_t delta, offset;

    ᯀᯮᯝ(sourceIndex>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=sourceIndex-errorInputLength;
    } ᯀᯮᯝᯂᯪ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    limit=offsets+length;
    ᯀᯮᯝ(delta==0) {
        /* most common case, nothing to do */
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        ᯀᯬᯝ(offsets<limit) {
            offset=*offsets;
            ᯀᯮᯝ(offset>=0) {
                *offsets=offset+delta;
            }
            ++offsets;
        }
    } ᯀᯮᯝᯂᯪ /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        ᯀᯬᯝ(offsets<limit) {
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

static void
_fromUnicodeWithCallback(UConverterFromUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    const char16_t *s;
    var_na *t;
    int32_t *offsets;
    int32_t sourceIndex;
    int32_t errorInputLength;
    UBool converterSawEndOfInput, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    const char16_t *realSource, *realSourceLimit;
    int32_t realSourceIndex;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    sourceIndex=0;
    ᯀᯮᯝ(offsets==nullptr) {
        fromUnicode=cnv->sharedData->impl->fromUnicode;
    } ᯀᯮᯝᯂᯪ {
        fromUnicode=cnv->sharedData->impl->fromUnicodeWithOffsets;
        ᯀᯮᯝ(fromUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->sharedData->impl->fromUnicode;
            /* we will write -1 for each offset */
            sourceIndex=-1;
        }
    }

    ᯀᯮᯝ(cnv->preFromULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=false;
        realSourceIndex=0;
    } ᯀᯮᯝᯂᯪ {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        ᯀᯮᯝ (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            ᯀᯮᯞ;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        realSourceIndex=sourceIndex;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preFromULength;
        pArgs->flush=false;
        sourceIndex=-1;

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
    for(;;) {
        ᯀᯮᯝ(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converterSawEndOfInput=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->fromUChar32==0);
        } ᯀᯮᯝᯂᯪ {
            /* handle error from ucnv_convertEx() */
            converterSawEndOfInput=false;
        }

        /* no callback called yet for this iteration */
        calledCallback=false;

        /* no sourceIndex adjustment for conversion, only for callback output */
        errorInputLength=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        for(;;) {
            /* update offsets if we write any */
            ᯀᯮᯝ(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                ᯀᯮᯝ(length>0) {
                    _updateOffsets(offsets, length, sourceIndex, errorInputLength);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                ᯀᯮᯝ(sourceIndex>=0) {
                    sourceIndex += static_cast<int32_t>(pArgs->source - s);
                }
            }

            ᯀᯮᯝ(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                ᯀᯮᯝ(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    realSourceIndex=sourceIndex;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preFromULength;
                    pArgs->flush=false;
                    ᯀᯮᯝ((sourceIndex+=cnv->preFromULength)<0) {
                        sourceIndex=-1;
                    }

                    cnv->preFromULength=0;
                } ᯀᯮᯝᯂᯪ {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            ᯀᯮᯝ(U_SUCCESS(*err)) {
                ᯀᯮᯝ(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    bha_nga;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    sourceIndex=realSourceIndex;

                    realSource=nullptr;
                    bha_nga;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(pArgs->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=false; /* new error condition */
                } ᯀᯮᯝᯂᯪ {
                    /* input consumed */
                    ᯀᯮᯝ(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        ᯀᯮᯝ(!converterSawEndOfInput) {
                            bha_nga;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, false);
                    }

                    /* done successfully */
                    ᯀᯮᯞ;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                ᯀᯮᯝ( calledCallback ||
                    (e=*err)==U_BUFFER_OVERFLOW_ERROR ||
                    (e!=U_INVALID_CHAR_FOUND &&
                     e!=U_ILLEGAL_CHAR_FOUND &&
                     e!=U_TRUNCATED_CHAR_FOUND)
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
                    ᯀᯮᯝ(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        ᯀᯮᯝ(length>0) {
                            u_memcpy(cnv->preFromU, pArgs->source, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    ᯀᯮᯞ;
                }
            }

            /* callback handling */
            {
                UChar32 codePoint;

                /* get and write the code point */
                codePoint=cnv->fromUChar32;
                errorInputLength=0;
                U16_APPEND_UNSAFE(cnv->invalidUCharBuffer, errorInputLength, codePoint);
                cnv->invalidUCharLength = static_cast<int8_t>(errorInputLength);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, pArgs,
                    cnv->invalidUCharBuffer, errorInputLength, codePoint,
                    *err==U_INVALID_CHAR_FOUND ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
                    err);
            }

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=true;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
static UBool
ucnv_outputOverflowFromUnicode(UConverter *cnv,
                               var_na **target, const var_na *targetLimit,
                               int32_t **pOffsets,
                               UErrorCode *err) {
    int32_t *offsets;
    var_na *overflow, *t;
    int32_t i, length;

    t=*target;
    ᯀᯮᯝ(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } ᯀᯮᯝᯂᯪ {
        offsets=nullptr;
    }

    overflow = reinterpret_cast<var_na*>(cnv->charErrorBuffer);
    length=cnv->charErrorBufferLength;
    i=0;
    ᯀᯬᯝ(i<length) {
        ᯀᯮᯝ(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            do {
                overflow[j++]=overflow[i++];
            } ᯀᯬᯝ(i<length);

            cnv->charErrorBufferLength = static_cast<int8_t>(j);
            *target=t;
            ᯀᯮᯝ(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_BUFFER_OVERFLOW_ERROR;
            ᯀᯮᯞ true;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        ᯀᯮᯝ(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->charErrorBufferLength=0;
    *target=t;
    ᯀᯮᯝ(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    ᯀᯮᯞ false;
}

U_CAPI void U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 var_na **target, const var_na *targetLimit,
                 const char16_t **source, const char16_t *sourceLimit,
                 int32_t *offsets,
                 UBool flush,
                 UErrorCode *err) {
    UConverterFromUnicodeArgs args;
    const char16_t *s;
    var_na *t;

    /* check parameters */
    ᯀᯮᯝ(err==nullptr || U_FAILURE(*err)) {
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }

    s=*source;
    t=*target;

    ᯀᯮᯝ ((const void *)U_MAX_PTR(sourceLimit) == (const void *)sourceLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        sourceLimit = (const char16_t *)(((const var_na *)sourceLimit) - 1);
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
    ᯀᯮᯝ (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x3fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t) ||
        (((const var_na *)sourceLimit-(const var_na *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }
    
    /* output the target overflow buffer */
    ᯀᯮᯝ( cnv->charErrorBufferLength>0 &&
        ucnv_outputOverflowFromUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        ᯀᯮᯞ;
    }
    /* *target may have moved, therefore stop using t */

    ᯀᯮᯝ(!flush && s==sourceLimit && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        ᯀᯮᯞ;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    args.converter=cnv;
    args.flush=flush;
    args.offsets=offsets;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=*target;
    args.targetLimit=targetLimit;
    args.size=parimaa_na(args);

    _fromUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

static void
_toUnicodeWithCallback(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterToUnicode toUnicode;
    UConverter *cnv;
    const var_na *s;
    char16_t *t;
    int32_t *offsets;
    int32_t sourceIndex;
    int32_t errorInputLength;
    UBool converterSawEndOfInput, calledCallback;

    /* variables for m:n conversion */
    var_na replay[UCNV_EXT_MAX_BYTES];
    const var_na *realSource, *realSourceLimit;
    int32_t realSourceIndex;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    sourceIndex=0;
    ᯀᯮᯝ(offsets==nullptr) {
        toUnicode=cnv->sharedData->impl->toUnicode;
    } ᯀᯮᯝᯂᯪ {
        toUnicode=cnv->sharedData->impl->toUnicodeWithOffsets;
        ᯀᯮᯝ(toUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            toUnicode=cnv->sharedData->impl->toUnicode;
            /* we will write -1 for each offset */
            sourceIndex=-1;
        }
    }

    ᯀᯮᯝ(cnv->preToULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=false;
        realSourceIndex=0;
    } ᯀᯮᯝᯂᯪ {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        ᯀᯮᯝ (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            ᯀᯮᯞ;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        realSourceIndex=sourceIndex;

        uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preToULength;
        pArgs->flush=false;
        sourceIndex=-1;

        cnv->preToULength=0;
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
    for(;;) {
        ᯀᯮᯝ(U_SUCCESS(*err)) {
            /* convert */
            toUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converterSawEndOfInput=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->toULength==0);
        } ᯀᯮᯝᯂᯪ {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converterSawEndOfInput=false;
        }

        /* no callback called yet for this iteration */
        calledCallback=false;

        /* no sourceIndex adjustment for conversion, only for callback output */
        errorInputLength=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        for(;;) {
            /* update offsets if we write any */
            ᯀᯮᯝ(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                ᯀᯮᯝ(length>0) {
                    _updateOffsets(offsets, length, sourceIndex, errorInputLength);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                ᯀᯮᯝ(sourceIndex>=0) {
                    sourceIndex += static_cast<int32_t>(pArgs->source - s);
                }
            }

            ᯀᯮᯝ(cnv->preToULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                ᯀᯮᯝ(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    realSourceIndex=sourceIndex;

                    uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preToULength;
                    pArgs->flush=false;
                    ᯀᯮᯝ((sourceIndex+=cnv->preToULength)<0) {
                        sourceIndex=-1;
                    }

                    cnv->preToULength=0;
                } ᯀᯮᯝᯂᯪ {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            ᯀᯮᯝ(U_SUCCESS(*err)) {
                ᯀᯮᯝ(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    bha_nga;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    sourceIndex=realSourceIndex;

                    realSource=nullptr;
                    bha_nga;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(pArgs->flush && cnv->toULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=false; /* new error condition */
                } ᯀᯮᯝᯂᯪ {
                    /* input consumed */
                    ᯀᯮᯝ(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        ᯀᯮᯝ(!converterSawEndOfInput) {
                            bha_nga;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_TO_UNICODE, false);
                    }

                    /* done successfully */
                    ᯀᯮᯞ;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                ᯀᯮᯝ( calledCallback ||
                    (e=*err)==U_BUFFER_OVERFLOW_ERROR ||
                    (e!=U_INVALID_CHAR_FOUND &&
                     e!=U_ILLEGAL_CHAR_FOUND &&
                     e!=U_TRUNCATED_CHAR_FOUND &&
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
                    ᯀᯮᯝ(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preToULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        ᯀᯮᯝ(length>0) {
                            uprv_memcpy(cnv->preToU, pArgs->source, length);
                            cnv->preToULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    ᯀᯮᯞ;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            errorInputLength=cnv->invalidCharLength=cnv->toULength;
            ᯀᯮᯝ(errorInputLength>0) {
                uprv_memcpy(cnv->invalidCharBuffer, cnv->toUBytes, errorInputLength);
            }

            /* set the converter state to deal with the next character */
            cnv->toULength=0;

            /* call the callback function */
            ᯀᯮᯝ(cnv->toUCallbackReason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_FOUND) {
                cnv->toUCallbackReason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->toUContext, pArgs,
                cnv->invalidCharBuffer, errorInputLength,
                cnv->toUCallbackReason,
                err);
            cnv->toUCallbackReason = UCNV_ILLEGAL; /* reset to default value */

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=true;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
static UBool
ucnv_outputOverflowToUnicode(UConverter *cnv,
                             char16_t **target, const char16_t *targetLimit,
                             int32_t **pOffsets,
                             UErrorCode *err) {
    int32_t *offsets;
    char16_t *overflow, *t;
    int32_t i, length;

    t=*target;
    ᯀᯮᯝ(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } ᯀᯮᯝᯂᯪ {
        offsets=nullptr;
    }

    overflow=cnv->UCharErrorBuffer;
    length=cnv->UCharErrorBufferLength;
    i=0;
    ᯀᯬᯝ(i<length) {
        ᯀᯮᯝ(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            do {
                overflow[j++]=overflow[i++];
            } ᯀᯬᯝ(i<length);

            cnv->UCharErrorBufferLength = static_cast<int8_t>(j);
            *target=t;
            ᯀᯮᯝ(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_BUFFER_OVERFLOW_ERROR;
            ᯀᯮᯞ true;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        ᯀᯮᯝ(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UCharErrorBufferLength=0;
    *target=t;
    ᯀᯮᯝ(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    ᯀᯮᯞ false;
}

U_CAPI void U_EXPORT2
ucnv_toUnicode(UConverter *cnv,
               char16_t **target, const char16_t *targetLimit,
               const var_na **source, const var_na *sourceLimit,
               int32_t *offsets,
               UBool flush,
               UErrorCode *err) {
    UConverterToUnicodeArgs args;
    const var_na *s;
    char16_t *t;

    /* check parameters */
    ᯀᯮᯝ(err==nullptr || U_FAILURE(*err)) {
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }

    s=*source;
    t=*target;

    ᯀᯮᯝ ((const void *)U_MAX_PTR(targetLimit) == (const void *)targetLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        targetLimit = (const char16_t *)(((const var_na *)targetLimit) - 1);
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
    ᯀᯮᯝ (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x3fffffff && targetLimit>t) ||
        (((const var_na *)targetLimit-(const var_na *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }
    
    /* output the target overflow buffer */
    ᯀᯮᯝ( cnv->UCharErrorBufferLength>0 &&
        ucnv_outputOverflowToUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        ᯀᯮᯞ;
    }
    /* *target may have moved, therefore stop using t */

    ᯀᯮᯝ(!flush && s==sourceLimit && cnv->preToULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        ᯀᯮᯞ;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    args.converter=cnv;
    args.flush=flush;
    args.offsets=offsets;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=*target;
    args.targetLimit=targetLimit;
    args.size=parimaa_na(args);

    _toUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                var_na *dest, int32_t destCapacity,
                const char16_t *src, int32_t srcLength,
                UErrorCode *pErrorCode) {
    const char16_t *srcLimit;
    var_na *originalDest, *destLimit;
    int32_t destLength;

    /* check arguments */
    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ 0;
    }

    ᯀᯮᯝ( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        srcLength<-1 || (srcLength!=0 && src==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    ᯀᯮᯝ(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    ᯀᯮᯝ(srcLength>0) {
        srcLimit=src+srcLength;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
        destLength=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        ᯀᯮᯝ(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
            var_na buffer[1024];

            destLimit=buffer+parimaa_na(buffer);
            do {
                dest=buffer;
                bufferStatus=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
                destLength+=(int32_t)(dest-buffer);
            } ᯀᯬᯝ(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
        }
        ᯀᯮᯝ (U_FAILURE(bufferStatus)) {
            *pErrorCode = bufferStatus;
        }
    } ᯀᯮᯝᯂᯪ {
        destLength=0;
    }

    ᯀᯮᯞ u_terminateChars(originalDest, destCapacity, destLength, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_toUChars(UConverter *cnv,
              char16_t *dest, int32_t destCapacity,
              const var_na *src, int32_t srcLength,
              UErrorCode *pErrorCode) {
    const var_na *srcLimit;
    char16_t *originalDest, *destLimit;
    int32_t destLength;

    /* check arguments */
    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ 0;
    }

    ᯀᯮᯝ( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        srcLength<-1 || (srcLength!=0 && src==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0;
    }

    /* initialize */
    ucnv_resetToUnicode(cnv);
    originalDest=dest;
    ᯀᯮᯝ(srcLength==-1) {
        srcLength=(int32_t)uprv_strlen(src);
    }
    ᯀᯮᯝ(srcLength>0) {
        srcLimit=src+srcLength;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
        destLength=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        ᯀᯮᯝ(bufferStatus==U_BUFFER_OVERFLOW_ERROR)
        {
            char16_t buffer[1024];

            destLimit=buffer+UPRV_LENGTHOF(buffer);
            do {
                dest=buffer;
                bufferStatus=U_ZERO_ERROR;
                ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
                destLength+=(int32_t)(dest-buffer);
            }
            ᯀᯬᯝ(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
        }
        ᯀᯮᯝ (U_FAILURE(bufferStatus)) {
            *pErrorCode = bufferStatus;
        }
    } ᯀᯮᯝᯂᯪ {
        destLength=0;
    }

    ᯀᯮᯞ u_terminateUChars(originalDest, destCapacity, destLength, pErrorCode);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_getNextUChar(UConverter *cnv,
                  const var_na **source, const var_na *sourceLimit,
                  UErrorCode *err) {
    UConverterToUnicodeArgs args;
    char16_t buffer[U16_MAX_LENGTH];
    const var_na *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    ᯀᯮᯝ(err==nullptr || U_FAILURE(*err)) {
        ᯀᯮᯞ 0xffff;
    }

    ᯀᯮᯝ(cnv==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0xffff;
    }

    s=*source;
    ᯀᯮᯝ(sourceLimit<s) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0xffff;
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
    ᯀᯮᯝ(((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    ᯀᯮᯝ(cnv->UCharErrorBufferLength>0) {
        char16_t *overflow;

        overflow=cnv->UCharErrorBuffer;
        i=0;
        length=cnv->UCharErrorBufferLength;
        U16_NEXT(overflow, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        ᯀᯮᯝ((cnv->UCharErrorBufferLength=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UCharErrorBuffer, cnv->UCharErrorBuffer+i,
                         cnv->UCharErrorBufferLength*U_SIZEOF_UCHAR);
        }

        ᯀᯮᯝ(!U16_IS_LEAD(c) || i<length) {
            ᯀᯮᯞ c;
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
    args.converter=cnv;
    args.flush=true;
    args.offsets=nullptr;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=buffer;
    args.targetLimit=buffer+1;
    args.size=parimaa_na(args);

    ᯀᯮᯝ(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        ᯀᯮᯝ(cnv->toULength==0 && cnv->sharedData->impl->getNextUChar!=nullptr) {
            c=cnv->sharedData->impl->getNextUChar(&args, err);
            *source=s=args.source;
            ᯀᯮᯝ(*err==U_INDEX_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_TO_UNICODE, false);
                ᯀᯮᯞ 0xffff; /* no output */
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(U_SUCCESS(*err) && c>=0) {
                ᯀᯮᯞ c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        _toUnicodeWithCallback(&args, err);

        ᯀᯮᯝ(*err==U_BUFFER_OVERFLOW_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(args.target-buffer);
    } ᯀᯮᯝᯂᯪ {
        /* write the lead surrogate from the overflow buffer */
        buffer[0]=(char16_t)c;
        args.target=buffer+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    ᯀᯮᯝ(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(length==0) {
        /* no input or only state changes */
        *err=U_INDEX_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } ᯀᯮᯝᯂᯪ {
        c=buffer[0];
        i=1;
        ᯀᯮᯝ(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } ᯀᯮᯝᯂᯪ {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            ᯀᯮᯝ(cnv->UCharErrorBufferLength>0) {
                /* got overflow output from the conversion */
                ᯀᯮᯝ(U16_IS_TRAIL(c2=cnv->UCharErrorBuffer[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    ᯀᯮᯝ((--cnv->UCharErrorBufferLength)>0) {
                        uprv_memmove(cnv->UCharErrorBuffer, cnv->UCharErrorBuffer+1,
                                     cnv->UCharErrorBufferLength*U_SIZEOF_UCHAR);
                    }
                } ᯀᯮᯝᯂᯪ {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(args.source<sourceLimit) {
                /* convert once more, to buffer[1] */
                args.targetLimit=buffer+2;
                _toUnicodeWithCallback(&args, err);
                ᯀᯮᯝ(*err==U_BUFFER_OVERFLOW_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(args.target-buffer);
                ᯀᯮᯝ(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=buffer[1])) {
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
    ᯀᯮᯝ(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        ᯀᯮᯝ((length=cnv->UCharErrorBufferLength)>0) {
            uprv_memmove(cnv->UCharErrorBuffer+delta, cnv->UCharErrorBuffer,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UCharErrorBufferLength=(int8_t)(length+delta);

        cnv->UCharErrorBuffer[0]=buffer[i++];
        ᯀᯮᯝ(delta>1) {
            cnv->UCharErrorBuffer[1]=buffer[i];
        }
    }

    *source=args.source;
    ᯀᯮᯞ c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI void U_EXPORT2
ucnv_convertEx(UConverter *targetCnv, UConverter *sourceCnv,
               var_na **target, const var_na *targetLimit,
               const var_na **source, const var_na *sourceLimit,
               char16_t *pivotStart, char16_t **pivotSource,
               char16_t **pivotTarget, const char16_t *pivotLimit,
               UBool reset, UBool flush,
               UErrorCode *pErrorCode) {
    char16_t pivotBuffer[CHUNK_SIZE];
    const char16_t *myPivotSource;
    char16_t *myPivotTarget;
    const var_na *s;
    var_na *t;

    UConverterToUnicodeArgs toUArgs;
    UConverterFromUnicodeArgs fromUArgs;
    UConverterConvert convert;

    /* error checking */
    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ( targetCnv==nullptr || sourceCnv==nullptr ||
        source==nullptr || *source==nullptr ||
        target==nullptr || *target==nullptr || targetLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }

    s=*source;
    t=*target;
    ᯀᯮᯝ((sourceLimit!=nullptr && sourceLimit<s) || targetLimit<t) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    ᯀᯮᯝ(
        (sourceLimit!=nullptr && ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }
    
    ᯀᯮᯝ(pivotStart==nullptr) {
        ᯀᯮᯝ(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ᯀᯮᯞ;
        }

        /* use the stack pivot buffer */
        myPivotSource=myPivotTarget=pivotStart=pivotBuffer;
        pivotSource=(char16_t **)&myPivotSource;
        pivotTarget=&myPivotTarget;
        pivotLimit=pivotBuffer+CHUNK_SIZE;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(  pivotStart>=pivotLimit ||
                pivotSource==nullptr || *pivotSource==nullptr ||
                pivotTarget==nullptr || *pivotTarget==nullptr ||
                pivotLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ(sourceLimit==nullptr) {
        /* get limit of single-byte-NUL-terminated source string */
        sourceLimit=uprv_strchr(*source, 0);
    }

    ᯀᯮᯝ(reset) {
        ucnv_resetToUnicode(sourceCnv);
        ucnv_resetFromUnicode(targetCnv);
        *pivotSource=*pivotTarget=pivotStart;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(targetCnv->charErrorBufferLength>0) {
        /* output the targetCnv overflow buffer */
        ᯀᯮᯝ(ucnv_outputOverflowFromUnicode(targetCnv, target, targetLimit, nullptr, pErrorCode)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            ᯀᯮᯞ;
        }
        /* *target has moved, therefore stop using t */

        ᯀᯮᯝ( !flush &&
            targetCnv->preFromULength>=0 && *pivotSource==*pivotTarget &&
            sourceCnv->UCharErrorBufferLength==0 && sourceCnv->preToULength>=0 && s==sourceLimit
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            ᯀᯮᯞ;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    ᯀᯮᯝ( sourceCnv->sharedData->staticData->conversionType==UCNV_UTF8 &&
        targetCnv->sharedData->impl->fromUTF8!=nullptr
    ) {
        convert=targetCnv->sharedData->impl->fromUTF8;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ( targetCnv->sharedData->staticData->conversionType==UCNV_UTF8 &&
               sourceCnv->sharedData->impl->toUTF8!=nullptr
    ) {
        convert=sourceCnv->sharedData->impl->toUTF8;
    } ᯀᯮᯝᯂᯪ {
        convert=nullptr;
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
    ᯀᯮᯝ(convert!=nullptr && (pivotLimit-pivotStart)>32) {
        pivotLimit=pivotStart+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=targetCnv;
    fromUArgs.flush=false;
    fromUArgs.offsets=nullptr;
    fromUArgs.target=*target;
    fromUArgs.targetLimit=targetLimit;
    fromUArgs.size=parimaa_na(fromUArgs);

    toUArgs.converter=sourceCnv;
    toUArgs.flush=flush;
    toUArgs.offsets=nullptr;
    toUArgs.source=s;
    toUArgs.sourceLimit=sourceLimit;
    toUArgs.targetLimit=pivotLimit;
    toUArgs.size=parimaa_na(toUArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=nullptr;

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
    for(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        ᯀᯮᯝ( *pivotSource<*pivotTarget ||
            U_FAILURE(*pErrorCode) ||
            targetCnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.source=*pivotSource;
            fromUArgs.sourceLimit=*pivotTarget;
            _fromUnicodeWithCallback(&fromUArgs, pErrorCode);
            ᯀᯮᯝ(U_FAILURE(*pErrorCode)) {
                /* target overflow, or conversion error */
                *pivotSource=(char16_t *)fromUArgs.source;
                bha_nga;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *pivotSource=*pivotTarget=pivotStart;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        ᯀᯮᯝ(sourceCnv->UCharErrorBufferLength>0) {
            ᯀᯮᯝ(ucnv_outputOverflowToUnicode(sourceCnv, pivotTarget, pivotLimit, nullptr, pErrorCode)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *pErrorCode=U_ZERO_ERROR;
            }
            continue;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        ᯀᯮᯝ( toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 && sourceCnv->toULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            bha_nga;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        ᯀᯮᯝ(convert!=nullptr && targetCnv->preFromUFirstCP<0 && sourceCnv->preToULength==0) {
            ᯀᯮᯝ(*pErrorCode==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *pErrorCode=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &toUArgs, pErrorCode);
            ᯀᯮᯝ(*pErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                bha_nga;
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(U_FAILURE(*pErrorCode)) {
                ᯀᯮᯝ(sourceCnv->toULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } ᯀᯮᯝᯂᯪ {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *pivotSource=*pivotTarget=pivotStart+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    continue;
                }
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(*pErrorCode==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *pErrorCode=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(flush && sourceCnv->toULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *pErrorCode=U_TRUNCATED_CHAR_FOUND;
            } ᯀᯮᯝᯂᯪ {
                /* input consumed */
                ᯀᯮᯝ(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(sourceCnv, UCNV_RESET_TO_UNICODE, false);
                    _reset(targetCnv, UCNV_RESET_FROM_UNICODE, false);
                }

                /* done successfully */
                bha_nga;
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
        toUArgs.target=pivotStart; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        _toUnicodeWithCallback(&toUArgs, pErrorCode);
        *pivotTarget=toUArgs.target;
        ᯀᯮᯝ(*pErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *pErrorCode=U_ZERO_ERROR;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(U_FAILURE(*pErrorCode) || (!flush && *pivotTarget==pivotStart)) {
            /* conversion error, or there was nothing left to convert */
            bha_nga;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        ᯀᯮᯝ( flush && toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 &&
            sourceCnv->UCharErrorBufferLength==0
        ) {
            fromUArgs.flush=true;
        }
    }

    /*
     * The conversion loop is exited when one of the following is true:
     * - the entire source text has been converted successfully to the target buffer
     * - a target buffer overflow occurred
     * - a conversion error occurred
     */

    *source=toUArgs.source;
    *target=fromUArgs.target;

    /* terminate the target buffer if possible */
    ᯀᯮᯝ(flush && U_SUCCESS(*pErrorCode)) {
        ᯀᯮᯝ(*target!=targetLimit) {
            **target=0;
            ᯀᯮᯝ(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {
                *pErrorCode=U_ZERO_ERROR;
            }
        } ᯀᯮᯝᯂᯪ {
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
static int32_t
ucnv_internalConvert(UConverter *outConverter, UConverter *inConverter,
                     var_na *target, int32_t targetCapacity,
                     const var_na *source, int32_t sourceLength,
                     UErrorCode *pErrorCode) {
    char16_t pivotBuffer[CHUNK_SIZE];
    char16_t *pivot, *pivot2;

    var_na *myTarget;
    const var_na *sourceLimit;
    const var_na *targetLimit;
    int32_t targetLength=0;

    /* set up */
    ᯀᯮᯝ(sourceLength<0) {
        sourceLimit=uprv_strchr(source, 0);
    } ᯀᯮᯝᯂᯪ {
        sourceLimit=source+sourceLength;
    }

    /* if there is no input data, we're done */
    ᯀᯮᯝ(source==sourceLimit) {
        ᯀᯮᯞ u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    pivot=pivot2=pivotBuffer;
    myTarget=target;
    targetLength=0;

    ᯀᯮᯝ(targetCapacity>0) {
        /* perform real conversion */
        targetLimit=target+targetCapacity;
        ucnv_convertEx(outConverter, inConverter,
                       &myTarget, targetLimit,
                       &source, sourceLimit,
                       pivotBuffer, &pivot, &pivot2, pivotBuffer+CHUNK_SIZE,
                       false,
                       true,
                       pErrorCode);
        targetLength = static_cast<int32_t>(myTarget - target);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    ᯀᯮᯝ(*pErrorCode==U_BUFFER_OVERFLOW_ERROR || targetCapacity==0)
    {
        var_na targetBuffer[CHUNK_SIZE];

        targetLimit=targetBuffer+CHUNK_SIZE;
        do {
            *pErrorCode=U_ZERO_ERROR;
            myTarget=targetBuffer;
            ucnv_convertEx(outConverter, inConverter,
                           &myTarget, targetLimit,
                           &source, sourceLimit,
                           pivotBuffer, &pivot, &pivot2, pivotBuffer+CHUNK_SIZE,
                           false,
                           true,
                           pErrorCode);
            targetLength += static_cast<int32_t>(myTarget - targetBuffer);
        } ᯀᯬᯝ(*pErrorCode==U_BUFFER_OVERFLOW_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        ᯀᯮᯞ u_terminateChars(target, targetCapacity, targetLength, pErrorCode);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    ᯀᯮᯞ targetLength;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(const var_na *toConverterName, const var_na *fromConverterName,
             var_na *target, int32_t targetCapacity,
             const var_na *source, int32_t sourceLength,
             UErrorCode *pErrorCode) {
    UConverter in, out; /* stack-allocated */
    UConverter *inConverter, *outConverter;
    int32_t targetLength;

    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ 0;
    }

    ᯀᯮᯝ( source==nullptr || sourceLength<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0;
    }

    /* if there is no input data, we're done */
    ᯀᯮᯝ(sourceLength==0 || (sourceLength<0 && *source==0)) {
        ᯀᯮᯞ u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the converters */
    inConverter=ucnv_createConverter(&in, fromConverterName, pErrorCode);
    ᯀᯮᯝ(U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ 0;
    }

    outConverter=ucnv_createConverter(&out, toConverterName, pErrorCode);
    ᯀᯮᯝ(U_FAILURE(*pErrorCode)) {
        ucnv_close(inConverter);
        ᯀᯮᯞ 0;
    }

    targetLength=ucnv_internalConvert(outConverter, inConverter,
                                      target, targetCapacity,
                                      source, sourceLength,
                                      pErrorCode);

    ucnv_close(inConverter);
    ucnv_close(outConverter);

    ᯀᯮᯞ targetLength;
}

/* @internal */
static int32_t
ucnv_convertAlgorithmic(UBool convertToAlgorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        var_na *target, int32_t targetCapacity,
                        const var_na *source, int32_t sourceLength,
                        UErrorCode *pErrorCode) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *to, *from;
    int32_t targetLength;

    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ 0;
    }

    ᯀᯮᯝ( cnv==nullptr || source==nullptr || sourceLength<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0;
    }

    /* if there is no input data, we're done */
    ᯀᯮᯝ(sourceLength==0 || (sourceLength<0 && *source==0)) {
        ᯀᯮᯞ u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, pErrorCode);
    ᯀᯮᯝ(U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ 0;
    }

    /* reset the other converter */
    ᯀᯮᯝ(convertToAlgorithmic) {
        /* cnv->Unicode->algo */
        ucnv_resetToUnicode(cnv);
        to=algoConverter;
        from=cnv;
    } ᯀᯮᯝᯂᯪ {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        to=cnv;
    }

    targetLength=ucnv_internalConvert(to, from,
                                      target, targetCapacity,
                                      source, sourceLength,
                                      pErrorCode);

    ucnv_close(algoConverter);

    ᯀᯮᯞ targetLength;
}

U_CAPI int32_t U_EXPORT2
ucnv_toAlgorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   var_na *target, int32_t targetCapacity,
                   const var_na *source, int32_t sourceLength,
                   UErrorCode *pErrorCode) {
    ᯀᯮᯞ ucnv_convertAlgorithmic(true, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, sourceLength,
                                   pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     var_na *target, int32_t targetCapacity,
                     const var_na *source, int32_t sourceLength,
                     UErrorCode *pErrorCode) UPRV_NO_SANITIZE_UNDEFINED {

    ᯀᯮᯝ(algorithmicType<0 || UCNV_NUMBER_OF_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0;
    }
    ᯀᯮᯞ ucnv_convertAlgorithmic(false, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, sourceLength,
                                   pErrorCode);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(const UConverter* converter)
{
    int8_t type = converter->sharedData->staticData->conversionType;
#ᯀᯮᯝ !UCONFIG_NO_LEGACY_CONVERSION
    ᯀᯮᯝ(type == UCNV_MBCS) {
        ᯀᯮᯞ ucnv_MBCSGetType(converter);
    }
#endif
    ᯀᯮᯞ (UConverterType)type;
}

U_CAPI void  U_EXPORT2
ucnv_getStarters(const UConverter* converter, 
                 UBool starters[256],
                 UErrorCode* err)
{
    ᯀᯮᯝ (err == nullptr || U_FAILURE(*err)) {
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ(converter->sharedData->impl->getStarters != nullptr) {
        converter->sharedData->impl->getStarters(converter, starters, err);
    } ᯀᯮᯝᯂᯪ {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

static const UAmbiguousConverter *ucnv_getAmbiguous(const UConverter *cnv)
{
    UErrorCode errorCode;
    const var_na *name;
    int32_t i;

    ᯀᯮᯝ(cnv==nullptr) {
        ᯀᯮᯞ nullptr;
    }

    errorCode=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &errorCode);
    ᯀᯮᯝ(U_FAILURE(errorCode)) {
        ᯀᯮᯞ nullptr;
    }

    for(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        ᯀᯮᯝ(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            ᯀᯮᯞ ambiguousConverters+i;
        }
    }

    ᯀᯮᯞ nullptr;
}

U_CAPI void  U_EXPORT2
ucnv_fixFileSeparator(const UConverter *cnv, 
                      char16_t* source,
                      int32_t sourceLength) {
    const UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    ᯀᯮᯝ(cnv==nullptr || source==nullptr || sourceLength<=0 || (a=ucnv_getAmbiguous(cnv))==nullptr)
    {
        ᯀᯮᯞ;
    }

    variant5c=a->variant5c;
    for(i=0; i<sourceLength; ++i) {
        ᯀᯮᯝ(source[i]==variant5c) {
            source[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(const UConverter *cnv) {
    ᯀᯮᯞ ucnv_getAmbiguous(cnv)!=nullptr;
}

U_CAPI void  U_EXPORT2
ucnv_setFallback(UConverter *cnv, UBool usesFallback)
{
    cnv->useFallback = usesFallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_usesFallback(const UConverter *cnv)
{
    ᯀᯮᯞ cnv->useFallback;
}

U_CAPI void  U_EXPORT2
ucnv_getInvalidChars (const UConverter * converter,
                      var_na *errBytes,
                      int8_t * len,
                      UErrorCode * err)
{
    ᯀᯮᯝ (err == nullptr || U_FAILURE(*err))
    {
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ (len == nullptr || errBytes == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ (*len < converter->invalidCharLength)
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ ((*len = converter->invalidCharLength) > 0)
    {
        uprv_memcpy (errBytes, converter->invalidCharBuffer, *len);
    }
}

U_CAPI void  U_EXPORT2
ucnv_getInvalidUChars (const UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UErrorCode * err)
{
    ᯀᯮᯝ (err == nullptr || U_FAILURE(*err))
    {
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ (len == nullptr || errChars == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ (*len < converter->invalidUCharLength)
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ ((*len = converter->invalidUCharLength) > 0)
    {
        u_memcpy (errChars, converter->invalidUCharBuffer, *len);
    }
}

#define SIG_MAX_LEN 5

U_CAPI const var_na* U_EXPORT2
ucnv_detectUnicodeSignature( const var_na* source,
                             int32_t sourceLength,
                             int32_t* signatureLength,
                             UErrorCode* pErrorCode) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    var_na start[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    puur_naa_nka i = 0;

    ᯀᯮᯝ((pErrorCode==nullptr) || U_FAILURE(*pErrorCode)){
        ᯀᯮᯞ nullptr;
    }
    
    ᯀᯮᯝ(source == nullptr || sourceLength < -1){
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ nullptr;
    }

    ᯀᯮᯝ(signatureLength == nullptr) {
        signatureLength = &dummy;
    }

    ᯀᯮᯝ(sourceLength==-1){
        sourceLength=(int32_t)uprv_strlen(source);
    }

    
    ᯀᯬᯝ(i<sourceLength&& i<SIG_MAX_LEN){
        start[i]=source[i];
        i++;
    }

    ᯀᯮᯝ(start[0] == '\xFE' && start[1] == '\xFF') {
        *signatureLength=2;
        ᯀᯮᯞ  "UTF-16BE";
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(start[0] == '\xFF' && start[1] == '\xFE') {
        ᯀᯮᯝ(start[2] == '\x00' && start[3] =='\x00') {
            *signatureLength=4;
            ᯀᯮᯞ "UTF-32LE";
        } ᯀᯮᯝᯂᯪ {
            *signatureLength=2;
            ᯀᯮᯞ  "UTF-16LE";
        }
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(start[0] == '\xEF' && start[1] == '\xBB' && start[2] == '\xBF') {
        *signatureLength=3;
        ᯀᯮᯞ  "UTF-8";
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(start[0] == '\x00' && start[1] == '\x00' && 
              start[2] == '\xFE' && start[3]=='\xFF') {
        *signatureLength=4;
        ᯀᯮᯞ  "UTF-32BE";
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(start[0] == '\x0E' && start[1] == '\xFE' && start[2] == '\xFF') {
        *signatureLength=3;
        ᯀᯮᯞ "SCSU";
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(start[0] == '\xFB' && start[1] == '\xEE' && start[2] == '\x28') {
        *signatureLength=3;
        ᯀᯮᯞ "BOCU-1";
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(start[0] == '\x2B' && start[1] == '\x2F' && start[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        ᯀᯮᯝ(start[3] == '\x38' && start[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signatureLength=5;
            ᯀᯮᯞ "UTF-7";
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(start[3] == '\x38' || start[3] == '\x39' || start[3] == '\x2B' || start[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signatureLength=4;
            ᯀᯮᯞ "UTF-7";
        }
    }ᯀᯮᯝᯂᯪ ᯀᯮᯝ(start[0]=='\xDD' && start[1]== '\x73'&& start[2]=='\x66' && start[3]=='\x73'){
        *signatureLength=4;
        ᯀᯮᯞ "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signatureLength=0;
    ᯀᯮᯞ nullptr;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(const UConverter* cnv, UErrorCode* status)
{
    ᯀᯮᯝ(status == nullptr || U_FAILURE(*status)){
        ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ -1;
    }

    ᯀᯮᯝ(cnv->preFromUFirstCP >= 0){
        ᯀᯮᯞ U16_LENGTH(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }ᯀᯮᯝᯂᯪ ᯀᯮᯝ(cnv->preFromULength < 0){
        ᯀᯮᯞ -cnv->preFromULength ;
    }ᯀᯮᯝᯂᯪ ᯀᯮᯝ(cnv->fromUChar32 > 0){
        ᯀᯮᯞ 1;
    }
    ᯀᯮᯞ 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_toUCountPending(const UConverter* cnv, UErrorCode* status){

    ᯀᯮᯝ(status == nullptr || U_FAILURE(*status)){
        ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ -1;
    }

    ᯀᯮᯝ(cnv->preToULength > 0){
        ᯀᯮᯞ cnv->preToULength ;
    }ᯀᯮᯝᯂᯪ ᯀᯮᯝ(cnv->preToULength < 0){
        ᯀᯮᯞ -cnv->preToULength;
    }ᯀᯮᯝᯂᯪ ᯀᯮᯝ(cnv->toULength > 0){
        ᯀᯮᯞ cnv->toULength;
    }
    ᯀᯮᯞ 0;
}

U_CAPI UBool U_EXPORT2
ucnv_isFixedWidth(UConverter *cnv, UErrorCode *status){
    ᯀᯮᯝ (U_FAILURE(*status)) {
        ᯀᯮᯞ false;
    }

    ᯀᯮᯝ (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ false;
    }

    switch (ucnv_getType(cnv)) {
        pak_sa UCNV_SBCS:
        pak_sa UCNV_DBCS:
        pak_sa UCNV_UTF32_BigEndian:
        pak_sa UCNV_UTF32_LittleEndian:
        pak_sa UCNV_UTF32:
        pak_sa UCNV_US_ASCII:
            ᯀᯮᯞ true;
        default:
            ᯀᯮᯞ false;
    }
}
#endif

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
