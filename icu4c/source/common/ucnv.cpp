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

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_CONVERSION

#अन्तर्भाव <memory>

#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "utracimp.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "ucnv_imp.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#define CHUNK_SIZE 1024

प्रकारनिर्देश संरचना UAmbiguousConverter {
    ध्रुव वर्ण *name;
    ध्रुव char16_t variant5c;
} UAmbiguousConverter;

स्थिर ध्रुव UAmbiguousConverter ambiguousConverters[]={
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
ucnv_open (ध्रुव वर्ण *name,
                       UErrorCode * err)
{
    UConverter *r;

    जइ (err == nullptr || U_FAILURE (*err)) {
        प्रत्यागम nullptr;
    }

    r =  ucnv_createConverter(nullptr, name, err);
    प्रत्यागम r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ध्रुव वर्ण *packageName, ध्रुव वर्ण *converterName, UErrorCode * err)
{
    प्रत्यागम ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ध्रुव char16_t * name,
                         UErrorCode * err)
{
    वर्ण asciiName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    जइ (err == nullptr || U_FAILURE(*err))
        प्रत्यागम nullptr;
    जइ (name == nullptr)
        प्रत्यागम ucnv_open (nullptr, err);
    जइ (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_LENGTH)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
स्थिर int32_t
ucnv_copyPlatformString(वर्ण *platformString, UConverterPlatform pltfrm)
{
    विकल्प (pltfrm)
    {
    पक्ष UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        प्रत्यागम 4;
    पक्ष UCNV_UNKNOWN:
        भङ्ग;
    }

    /* default to empty string */
    *platformString = 0;
    प्रत्यागम 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UErrorCode * err)
{
    वर्ण myName[UCNV_MAX_CONVERTER_NAME_LENGTH];
    int32_t myNameLen;

    जइ (err == nullptr || U_FAILURE (*err))
        प्रत्यागम nullptr;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integerToString(myName + myNameLen, codepage, 10);

    प्रत्यागम ucnv_createConverter(nullptr, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_safeClone(ध्रुव UConverter* cnv,
               शून्य* राशिः_Buffer U_LIFETIME_BOUND,
               int32_t* pBufferSize,
               UErrorCode* status)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t राशिः_Buffer_Size;
    int32_t bufferSizeNeeded;
    UErrorCode cbErr;
    UConverterToUnicodeArgs toUArgs = {
        परिमाण(UConverterToUnicodeArgs),
            सच्च,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };
    UConverterFromUnicodeArgs fromUArgs = {
        परिमाण(UConverterFromUnicodeArgs),
            सच्च,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLONE);

    जइ (status == nullptr || U_FAILURE(*status)){
        UTRACE_EXIT_STATUS(status? *status: U_ILLEGAL_ARGUMENT_ERROR);
        प्रत्यागम nullptr;
    }

    जइ (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(*status);
        प्रत्यागम nullptr;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, status), cnv, राशिः_Buffer);

    जइ (cnv->sharedData->impl->safeClone != nullptr) {
        /* call the custom safeClone function for sizing */
        bufferSizeNeeded = 0;
        cnv->sharedData->impl->safeClone(cnv, nullptr, &bufferSizeNeeded, status);
        जइ (U_FAILURE(*status)) {
            UTRACE_EXIT_STATUS(*status);
            प्रत्यागम nullptr;
        }
    }
    अण्णहा
    {
        /* inherent sizing */
        bufferSizeNeeded = परिमाण(UConverter);
    }

    जइ (pBufferSize == nullptr) {
        राशिः_Buffer_Size = 1;
        pBufferSize = &राशिः_Buffer_Size;
    } अण्णहा {
        राशिः_Buffer_Size = *pBufferSize;
        जइ (राशिः_Buffer_Size <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *pBufferSize = bufferSizeNeeded;
            UTRACE_EXIT_मानम्(bufferSizeNeeded);
            प्रत्यागम nullptr;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    जइ (राशिः_Buffer) {
        uintptr_t p = reinterpret_cast<uintptr_t>(राशिः_Buffer);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t pointerAdjustment = aligned_p - p;
        जइ (bufferSizeNeeded + pointerAdjustment <= राशिः_Buffer_Size) {
            राशिः_Buffer = reinterpret_cast<शून्य *>(aligned_p);
            राशिः_Buffer_Size -= static_cast<int32_t>(pointerAdjustment);
        } अण्णहा {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            राशिः_Buffer_Size = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    जइ (राशिः_Buffer_Size < bufferSizeNeeded || राशिः_Buffer == nullptr)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (bufferSizeNeeded);

        जइ(localConverter == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            UTRACE_EXIT_STATUS(*status);
            प्रत्यागम nullptr;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        जइ (pBufferSize != &राशिः_Buffer_Size) {
            *status = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *pBufferSize = bufferSizeNeeded;
    } अण्णहा {
        /* just use the stack buffer */
        localConverter = (UConverter*) राशिः_Buffer;
        allocatedConverter = nullptr;
    }

    uprv_memset(localConverter, 0, bufferSizeNeeded);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, परिमाण(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = मिच्छ;

    /* copy the substitution string */
    जइ (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } अण्णहा {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
        जइ (localConverter->subChars == nullptr) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_STATUS(*status);
            प्रत्यागम nullptr;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    जइ (cnv->sharedData->impl->safeClone != nullptr) {
        /* call the custom safeClone function */
        localConverter = cnv->sharedData->impl->safeClone(cnv, localConverter, pBufferSize, status);
    }

    जइ(localConverter==nullptr || U_FAILURE(*status)) {
        जइ (allocatedConverter != nullptr && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_STATUS(*status);
        प्रत्यागम nullptr;
    }

    /* increment refcount of shared data if needed */
    जइ (cnv->sharedData->isReferenceCounted) {
        ucnv_increment_Ref_गणनम्(cnv->sharedData);
    }

    जइ(localConverter == (UConverter*)राशिः_Buffer) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = सच्च;
    }

    /* allow callback functions to handle any memory allocation */
    toUArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->toUContext, &toUArgs, nullptr, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, nullptr, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_STATUS(localConverter, *status);
    प्रत्यागम localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ध्रुव UConverter* cnv, UErrorCode *status)
{
    प्रत्यागम ucnv_safeClone(cnv, nullptr, nullptr, status);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI शून्य  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLOSE);

    जइ (converter == nullptr)
    {
        UTRACE_EXIT();
        प्रत्यागम;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &errorCode), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    जइ (converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
        UConverterToUnicodeArgs toUArgs = {
            परिमाण(UConverterToUnicodeArgs),
                सच्च,
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
    जइ (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverterFromUnicodeArgs fromUArgs = {
            परिमाण(UConverterFromUnicodeArgs),
                सच्च,
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

    जइ (converter->sharedData->impl->close != nullptr) {
        converter->sharedData->impl->close(converter);
    }

    जइ (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    जइ (converter->sharedData->isReferenceCounted) {
        ucnv_unloadSharedDataIfReady(converter->sharedData);
    }

    जइ(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ध्रुव वर्ण*   U_EXPORT2
ucnv_getAvailableName (int32_t n)
{
    जइ (0 <= n && n <= 0xffff) {
        UErrorCode err = U_ZERO_ERROR;
        ध्रुव वर्ण *name = ucnv_bld_getAvailableConverter((uint16_t)n, &err);
        जइ (U_SUCCESS(err)) {
            प्रत्यागम name;
        }
    }
    प्रत्यागम nullptr;
}

U_CAPI int32_t   U_EXPORT2
ucnv_गणनम्_Available ()
{
    UErrorCode err = U_ZERO_ERROR;
    प्रत्यागम ucnv_bld_गणनम्_Available_Converters(&err);
}

U_CAPI शून्य    U_EXPORT2
ucnv_getSubstChars (ध्रुव UConverter * converter,
                    वर्ण *mySubChar,
                    int8_t * len,
                    UErrorCode * err)
{
    जइ (U_FAILURE (*err))
        प्रत्यागम;

    जइ (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        प्रत्यागम;
    }

    जइ (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI शून्य    U_EXPORT2
ucnv_setSubstChars (UConverter * converter,
                    ध्रुव वर्ण *mySubChar,
                    int8_t len,
                    UErrorCode * err)
{
    जइ (U_FAILURE (*err))
        प्रत्यागम;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    जइ ((len > converter->sharedData->staticData->maxBytesPerChar)
     || (len < converter->sharedData->staticData->minBytesPerChar))
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
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

U_CAPI शून्य U_EXPORT2
ucnv_setSubstString(UConverter *cnv,
                    ध्रुव char16_t *s,
                    int32_t length,
                    UErrorCode *err) {
    alignas(UConverter) वर्ण cloneBuffer[U_CNV_SAFECLONE_BUFFERSIZE];
    वर्ण chars[UCNV_ERROR_BUFFER_LENGTH];

    UConverter *clone;
    uint8_t *subChars;
    int32_t cloneSize, length8;

    /* Let the following functions check all arguments. */
    cloneSize = परिमाण(cloneBuffer);
    clone = ucnv_safeClone(cnv, cloneBuffer, &cloneSize, err);
    ucnv_setFromUCallBack(clone, UCNV_FROM_U_CALLBACK_STOP, nullptr, nullptr, nullptr, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)परिमाण(chars), s, length, err);
    ucnv_close(clone);
    जइ (U_FAILURE(*err)) {
        प्रत्यागम;
    }

    जइ (cnv->sharedData->impl->writeSub == nullptr
#जइ !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->sharedData->staticData->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#endif
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } अण्णहा {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        जइ (length > UCNV_ERROR_BUFFER_LENGTH) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम;
        }
        subChars = (uint8_t *)s;
        जइ (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    जइ (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        जइ (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
            जइ (cnv->subChars == nullptr) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    जइ (length8 == 0) {
        cnv->subCharLen = 0;
    } अण्णहा {
        uprv_memcpy(cnv->subChars, subChars, length8);
        जइ (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } अण्णहा /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
स्थिर शून्य _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    जइ(converter == nullptr) {
        प्रत्यागम;
    }

    जइ(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UErrorCode errorCode;

        जइ(choice<=UCNV_RESET_TO_UNICODE && converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
            UConverterToUnicodeArgs toUArgs = {
                परिमाण(UConverterToUnicodeArgs),
                सच्च,
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
        जइ(choice!=UCNV_RESET_TO_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverterFromUnicodeArgs fromUArgs = {
                परिमाण(UConverterFromUnicodeArgs),
                सच्च,
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
    जइ(choice<=UCNV_RESET_TO_UNICODE) {
        converter->toUnicodeStatus = converter->sharedData->toUnicodeStatus;
        converter->mode = 0;
        converter->toULength = 0;
        converter->invalidCharLength = converter->UCharErrorBufferLength = 0;
        converter->preToULength = 0;
    }
    जइ(choice!=UCNV_RESET_TO_UNICODE) {
        converter->fromUnicodeStatus = 0;
        converter->fromUChar32 = 0;
        converter->invalidUCharLength = converter->charErrorBufferLength = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    जइ (converter->sharedData->impl->reset != nullptr) {
        /* call the custom reset function */
        converter->sharedData->impl->reset(converter, choice);
    }
}

U_CAPI शून्य  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, सच्च);
}

U_CAPI शून्य  U_EXPORT2
ucnv_resetToUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_TO_UNICODE, सच्च);
}

U_CAPI शून्य  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, सच्च);
}

U_CAPI int8_t   U_EXPORT2
ucnv_getMaxCharSize (ध्रुव UConverter * converter)
{
    प्रत्यागम converter->maxBytesPerUChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_getMinCharSize (ध्रुव UConverter * converter)
{
    प्रत्यागम converter->sharedData->staticData->minBytesPerChar;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ucnv_getName(ध्रुव UConverter* converter U_LIFETIME_BOUND, UErrorCode* err)
{
    जइ (U_FAILURE (*err))
        प्रत्यागम nullptr;
    जइ(converter->sharedData->impl->getName){
        ध्रुव वर्ण* temp= converter->sharedData->impl->getName(converter);
        जइ(temp)
            प्रत्यागम temp;
    }
    प्रत्यागम converter->sharedData->staticData->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ध्रुव UConverter * converter,
              UErrorCode * err)
{
    int32_t ccsid;
    जइ (U_FAILURE (*err))
        प्रत्यागम -1;

    ccsid = converter->sharedData->staticData->codepage;
    जइ (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        ध्रुव वर्ण *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        जइ (U_SUCCESS(*err) && standardName) {
            ध्रुव वर्ण *ccsidStr = uprv_strchr(standardName, '-');
            जइ (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    प्रत्यागम ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (ध्रुव UConverter * converter,
                                      UErrorCode * err)
{
    जइ (U_FAILURE (*err))
        प्रत्यागम UCNV_UNKNOWN;

    प्रत्यागम (UConverterPlatform)converter->sharedData->staticData->platform;
}

U_CAPI शून्य U_EXPORT2
    ucnv_getToUCallBack (ध्रुव UConverter * converter,
                         UConverterToUCallback *action,
                         ध्रुव शून्य **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->toUContext;
}

U_CAPI शून्य U_EXPORT2
    ucnv_getFromUCallBack (ध्रुव UConverter * converter,
                           UConverterFromUCallback *action,
                           ध्रुव शून्य **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI शून्य    U_EXPORT2
ucnv_setToUCallBack (UConverter * converter,
                            UConverterToUCallback newAction,
                            ध्रुव शून्य* newContext,
                            UConverterToUCallback *oldAction,
                            ध्रुव शून्य** oldContext,
                            UErrorCode * err)
{
    जइ (U_FAILURE (*err))
        प्रत्यागम;
    जइ (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    जइ (oldContext) *oldContext = converter->toUContext;
    converter->toUContext = newContext;
}

U_CAPI शून्य  U_EXPORT2
ucnv_setFromUCallBack (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ध्रुव शून्य* newContext,
                            UConverterFromUCallback *oldAction,
                            ध्रुव शून्य** oldContext,
                            UErrorCode * err)
{
    जइ (U_FAILURE (*err))
        प्रत्यागम;
    जइ (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    जइ (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

स्थिर शून्य
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t sourceIndex, int32_t error_निवेशः_Length) {
    int32_t *limit;
    int32_t delta, offset;

    जइ(sourceIndex>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=sourceIndex-error_निवेशः_Length;
    } अण्णहा {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    limit=offsets+length;
    जइ(delta==0) {
        /* most common case, nothing to do */
    } अण्णहा जइ(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        जाव(offsets<limit) {
            offset=*offsets;
            जइ(offset>=0) {
                *offsets=offset+delta;
            }
            ++offsets;
        }
    } अण्णहा /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        जाव(offsets<limit) {
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

स्थिर शून्य
_fromUnicodeWithCallback(UConverterFromUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ध्रुव char16_t *s;
    वर्ण *t;
    int32_t *offsets;
    int32_t sourceIndex;
    int32_t error_निवेशः_Length;
    UBool converter_Saw_End_Of_निवेशः, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ध्रुव char16_t *realSource, *realSourceLimit;
    int32_t realSourceIndex;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    sourceIndex=0;
    जइ(offsets==nullptr) {
        fromUnicode=cnv->sharedData->impl->fromUnicode;
    } अण्णहा {
        fromUnicode=cnv->sharedData->impl->fromUnicodeWithOffsets;
        जइ(fromUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->sharedData->impl->fromUnicode;
            /* we will write -1 for each offset */
            sourceIndex=-1;
        }
    }

    जइ(cnv->preFromULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=मिच्छ;
        realSourceIndex=0;
    } अण्णहा {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        जइ (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        realSourceIndex=sourceIndex;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preFromULength;
        pArgs->flush=मिच्छ;
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
    प्रति(;;) {
        जइ(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_End_Of_निवेशः=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->fromUChar32==0);
        } अण्णहा {
            /* handle error from ucnv_convertEx() */
            converter_Saw_End_Of_निवेशः=मिच्छ;
        }

        /* no callback called yet for this iteration */
        calledCallback=मिच्छ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_निवेशः_Length=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        प्रति(;;) {
            /* update offsets if we write any */
            जइ(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                जइ(length>0) {
                    _updateOffsets(offsets, length, sourceIndex, error_निवेशः_Length);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                जइ(sourceIndex>=0) {
                    sourceIndex += static_cast<int32_t>(pArgs->source - s);
                }
            }

            जइ(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                जइ(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    realSourceIndex=sourceIndex;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preFromULength;
                    pArgs->flush=मिच्छ;
                    जइ((sourceIndex+=cnv->preFromULength)<0) {
                        sourceIndex=-1;
                    }

                    cnv->preFromULength=0;
                } अण्णहा {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            जइ(U_SUCCESS(*err)) {
                जइ(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    भङ्ग;
                } अण्णहा जइ(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    sourceIndex=realSourceIndex;

                    realSource=nullptr;
                    भङ्ग;
                } अण्णहा जइ(pArgs->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=मिच्छ; /* new error condition */
                } अण्णहा {
                    /* input consumed */
                    जइ(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        जइ(!converter_Saw_End_Of_निवेशः) {
                            भङ्ग;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, मिच्छ);
                    }

                    /* done successfully */
                    प्रत्यागम;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                जइ( calledCallback ||
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
                    जइ(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        जइ(length>0) {
                            u_memcpy(cnv->preFromU, pArgs->source, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    प्रत्यागम;
                }
            }

            /* callback handling */
            {
                UChar32 codePoint;

                /* get and write the code point */
                codePoint=cnv->fromUChar32;
                error_निवेशः_Length=0;
                U16_APPEND_UNSAFE(cnv->invalidUCharBuffer, error_निवेशः_Length, codePoint);
                cnv->invalidUCharLength = static_cast<int8_t>(error_निवेशः_Length);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, pArgs,
                    cnv->invalidUCharBuffer, error_निवेशः_Length, codePoint,
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
            calledCallback=सच्च;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
स्थिर UBool
ucnv_outputOverflowFromUnicode(UConverter *cnv,
                               वर्ण **target, ध्रुव वर्ण *targetLimit,
                               int32_t **pOffsets,
                               UErrorCode *err) {
    int32_t *offsets;
    वर्ण *overflow, *t;
    int32_t i, length;

    t=*target;
    जइ(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } अण्णहा {
        offsets=nullptr;
    }

    overflow = reinterpret_cast<वर्ण*>(cnv->charErrorBuffer);
    length=cnv->charErrorBufferLength;
    i=0;
    जाव(i<length) {
        जइ(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            करेहि {
                overflow[j++]=overflow[i++];
            } जाव(i<length);

            cnv->charErrorBufferLength = static_cast<int8_t>(j);
            *target=t;
            जइ(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम सच्च;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        जइ(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->charErrorBufferLength=0;
    *target=t;
    जइ(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    प्रत्यागम मिच्छ;
}

U_CAPI शून्य U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 वर्ण **target, ध्रुव वर्ण *targetLimit,
                 ध्रुव char16_t **source, ध्रुव char16_t *sourceLimit,
                 int32_t *offsets,
                 UBool flush,
                 UErrorCode *err) {
    UConverterFromUnicodeArgs args;
    ध्रुव char16_t *s;
    वर्ण *t;

    /* check parameters */
    जइ(err==nullptr || U_FAILURE(*err)) {
        प्रत्यागम;
    }

    जइ(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    s=*source;
    t=*target;

    जइ ((ध्रुव शून्य *)U_MAX_PTR(sourceLimit) == (ध्रुव शून्य *)sourceLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        sourceLimit = (ध्रुव char16_t *)(((ध्रुव वर्ण *)sourceLimit) - 1);
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
    जइ (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x3fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t) ||
        (((ध्रुव वर्ण *)sourceLimit-(ध्रुव वर्ण *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    
    /* output the target overflow buffer */
    जइ( cnv->charErrorBufferLength>0 &&
        ucnv_outputOverflowFromUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        प्रत्यागम;
    }
    /* *target may have moved, therefore stop using t */

    जइ(!flush && s==sourceLimit && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        प्रत्यागम;
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
    args.size=परिमाण(args);

    _fromUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

स्थिर शून्य
_toUnicodeWithCallback(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterToUnicode toUnicode;
    UConverter *cnv;
    ध्रुव वर्ण *s;
    char16_t *t;
    int32_t *offsets;
    int32_t sourceIndex;
    int32_t error_निवेशः_Length;
    UBool converter_Saw_End_Of_निवेशः, calledCallback;

    /* variables for m:n conversion */
    वर्ण replay[UCNV_EXT_MAX_BYTES];
    ध्रुव वर्ण *realSource, *realSourceLimit;
    int32_t realSourceIndex;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    sourceIndex=0;
    जइ(offsets==nullptr) {
        toUnicode=cnv->sharedData->impl->toUnicode;
    } अण्णहा {
        toUnicode=cnv->sharedData->impl->toUnicodeWithOffsets;
        जइ(toUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            toUnicode=cnv->sharedData->impl->toUnicode;
            /* we will write -1 for each offset */
            sourceIndex=-1;
        }
    }

    जइ(cnv->preToULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=मिच्छ;
        realSourceIndex=0;
    } अण्णहा {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        जइ (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        realSourceIndex=sourceIndex;

        uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preToULength;
        pArgs->flush=मिच्छ;
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
    प्रति(;;) {
        जइ(U_SUCCESS(*err)) {
            /* convert */
            toUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_End_Of_निवेशः=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->toULength==0);
        } अण्णहा {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_End_Of_निवेशः=मिच्छ;
        }

        /* no callback called yet for this iteration */
        calledCallback=मिच्छ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_निवेशः_Length=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        प्रति(;;) {
            /* update offsets if we write any */
            जइ(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                जइ(length>0) {
                    _updateOffsets(offsets, length, sourceIndex, error_निवेशः_Length);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                जइ(sourceIndex>=0) {
                    sourceIndex += static_cast<int32_t>(pArgs->source - s);
                }
            }

            जइ(cnv->preToULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                जइ(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    realSourceIndex=sourceIndex;

                    uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preToULength;
                    pArgs->flush=मिच्छ;
                    जइ((sourceIndex+=cnv->preToULength)<0) {
                        sourceIndex=-1;
                    }

                    cnv->preToULength=0;
                } अण्णहा {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            जइ(U_SUCCESS(*err)) {
                जइ(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    भङ्ग;
                } अण्णहा जइ(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    sourceIndex=realSourceIndex;

                    realSource=nullptr;
                    भङ्ग;
                } अण्णहा जइ(pArgs->flush && cnv->toULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=मिच्छ; /* new error condition */
                } अण्णहा {
                    /* input consumed */
                    जइ(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        जइ(!converter_Saw_End_Of_निवेशः) {
                            भङ्ग;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_TO_UNICODE, मिच्छ);
                    }

                    /* done successfully */
                    प्रत्यागम;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                जइ( calledCallback ||
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
                    जइ(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preToULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        जइ(length>0) {
                            uprv_memcpy(cnv->preToU, pArgs->source, length);
                            cnv->preToULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    प्रत्यागम;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_निवेशः_Length=cnv->invalidCharLength=cnv->toULength;
            जइ(error_निवेशः_Length>0) {
                uprv_memcpy(cnv->invalidCharBuffer, cnv->toUBytes, error_निवेशः_Length);
            }

            /* set the converter state to deal with the next character */
            cnv->toULength=0;

            /* call the callback function */
            जइ(cnv->toUCallbackReason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_FOUND) {
                cnv->toUCallbackReason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->toUContext, pArgs,
                cnv->invalidCharBuffer, error_निवेशः_Length,
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
            calledCallback=सच्च;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
स्थिर UBool
ucnv_outputOverflowToUnicode(UConverter *cnv,
                             char16_t **target, ध्रुव char16_t *targetLimit,
                             int32_t **pOffsets,
                             UErrorCode *err) {
    int32_t *offsets;
    char16_t *overflow, *t;
    int32_t i, length;

    t=*target;
    जइ(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } अण्णहा {
        offsets=nullptr;
    }

    overflow=cnv->UCharErrorBuffer;
    length=cnv->UCharErrorBufferLength;
    i=0;
    जाव(i<length) {
        जइ(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            करेहि {
                overflow[j++]=overflow[i++];
            } जाव(i<length);

            cnv->UCharErrorBufferLength = static_cast<int8_t>(j);
            *target=t;
            जइ(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम सच्च;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        जइ(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UCharErrorBufferLength=0;
    *target=t;
    जइ(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    प्रत्यागम मिच्छ;
}

U_CAPI शून्य U_EXPORT2
ucnv_toUnicode(UConverter *cnv,
               char16_t **target, ध्रुव char16_t *targetLimit,
               ध्रुव वर्ण **source, ध्रुव वर्ण *sourceLimit,
               int32_t *offsets,
               UBool flush,
               UErrorCode *err) {
    UConverterToUnicodeArgs args;
    ध्रुव वर्ण *s;
    char16_t *t;

    /* check parameters */
    जइ(err==nullptr || U_FAILURE(*err)) {
        प्रत्यागम;
    }

    जइ(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    s=*source;
    t=*target;

    जइ ((ध्रुव शून्य *)U_MAX_PTR(targetLimit) == (ध्रुव शून्य *)targetLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        targetLimit = (ध्रुव char16_t *)(((ध्रुव वर्ण *)targetLimit) - 1);
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
    जइ (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x3fffffff && targetLimit>t) ||
        (((ध्रुव वर्ण *)targetLimit-(ध्रुव वर्ण *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    
    /* output the target overflow buffer */
    जइ( cnv->UCharErrorBufferLength>0 &&
        ucnv_outputOverflowToUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        प्रत्यागम;
    }
    /* *target may have moved, therefore stop using t */

    जइ(!flush && s==sourceLimit && cnv->preToULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        प्रत्यागम;
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
    args.size=परिमाण(args);

    _toUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                वर्ण *dest, int32_t destCapacity,
                ध्रुव char16_t *src, int32_t srcLength,
                UErrorCode *pErrorCode) {
    ध्रुव char16_t *srcLimit;
    वर्ण *originalDest, *destLimit;
    int32_t destLength;

    /* check arguments */
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    जइ( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        srcLength<-1 || (srcLength!=0 && src==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    जइ(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    जइ(srcLength>0) {
        srcLimit=src+srcLength;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, सच्च, &bufferStatus);
        destLength=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        जइ(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
            वर्ण buffer[1024];

            destLimit=buffer+परिमाण(buffer);
            करेहि {
                dest=buffer;
                bufferStatus=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, सच्च, &bufferStatus);
                destLength+=(int32_t)(dest-buffer);
            } जाव(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
        }
        जइ (U_FAILURE(bufferStatus)) {
            *pErrorCode = bufferStatus;
        }
    } अण्णहा {
        destLength=0;
    }

    प्रत्यागम u_terminateChars(originalDest, destCapacity, destLength, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_toUChars(UConverter *cnv,
              char16_t *dest, int32_t destCapacity,
              ध्रुव वर्ण *src, int32_t srcLength,
              UErrorCode *pErrorCode) {
    ध्रुव वर्ण *srcLimit;
    char16_t *originalDest, *destLimit;
    int32_t destLength;

    /* check arguments */
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    जइ( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        srcLength<-1 || (srcLength!=0 && src==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* initialize */
    ucnv_resetToUnicode(cnv);
    originalDest=dest;
    जइ(srcLength==-1) {
        srcLength=(int32_t)uprv_strlen(src);
    }
    जइ(srcLength>0) {
        srcLimit=src+srcLength;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, सच्च, &bufferStatus);
        destLength=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        जइ(bufferStatus==U_BUFFER_OVERFLOW_ERROR)
        {
            char16_t buffer[1024];

            destLimit=buffer+UPRV_LENGTHOF(buffer);
            करेहि {
                dest=buffer;
                bufferStatus=U_ZERO_ERROR;
                ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, सच्च, &bufferStatus);
                destLength+=(int32_t)(dest-buffer);
            }
            जाव(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
        }
        जइ (U_FAILURE(bufferStatus)) {
            *pErrorCode = bufferStatus;
        }
    } अण्णहा {
        destLength=0;
    }

    प्रत्यागम u_terminateUChars(originalDest, destCapacity, destLength, pErrorCode);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_getNextUChar(UConverter *cnv,
                  ध्रुव वर्ण **source, ध्रुव वर्ण *sourceLimit,
                  UErrorCode *err) {
    UConverterToUnicodeArgs args;
    char16_t buffer[U16_MAX_LENGTH];
    ध्रुव वर्ण *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    जइ(err==nullptr || U_FAILURE(*err)) {
        प्रत्यागम 0xffff;
    }

    जइ(cnv==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0xffff;
    }

    s=*source;
    जइ(sourceLimit<s) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0xffff;
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
    जइ(((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    जइ(cnv->UCharErrorBufferLength>0) {
        char16_t *overflow;

        overflow=cnv->UCharErrorBuffer;
        i=0;
        length=cnv->UCharErrorBufferLength;
        U16_NEXT(overflow, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        जइ((cnv->UCharErrorBufferLength=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UCharErrorBuffer, cnv->UCharErrorBuffer+i,
                         cnv->UCharErrorBufferLength*U_SIZEOF_UCHAR);
        }

        जइ(!U16_IS_LEAD(c) || i<length) {
            प्रत्यागम c;
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
    args.flush=सच्च;
    args.offsets=nullptr;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=buffer;
    args.targetLimit=buffer+1;
    args.size=परिमाण(args);

    जइ(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        जइ(cnv->toULength==0 && cnv->sharedData->impl->getNextUChar!=nullptr) {
            c=cnv->sharedData->impl->getNextUChar(&args, err);
            *source=s=args.source;
            जइ(*err==U_INDEX_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_TO_UNICODE, मिच्छ);
                प्रत्यागम 0xffff; /* no output */
            } अण्णहा जइ(U_SUCCESS(*err) && c>=0) {
                प्रत्यागम c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        _toUnicodeWithCallback(&args, err);

        जइ(*err==U_BUFFER_OVERFLOW_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(args.target-buffer);
    } अण्णहा {
        /* write the lead surrogate from the overflow buffer */
        buffer[0]=(char16_t)c;
        args.target=buffer+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    जइ(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } अण्णहा जइ(length==0) {
        /* no input or only state changes */
        *err=U_INDEX_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } अण्णहा {
        c=buffer[0];
        i=1;
        जइ(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } अण्णहा {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            जइ(cnv->UCharErrorBufferLength>0) {
                /* got overflow output from the conversion */
                जइ(U16_IS_TRAIL(c2=cnv->UCharErrorBuffer[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    जइ((--cnv->UCharErrorBufferLength)>0) {
                        uprv_memmove(cnv->UCharErrorBuffer, cnv->UCharErrorBuffer+1,
                                     cnv->UCharErrorBufferLength*U_SIZEOF_UCHAR);
                    }
                } अण्णहा {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } अण्णहा जइ(args.source<sourceLimit) {
                /* convert once more, to buffer[1] */
                args.targetLimit=buffer+2;
                _toUnicodeWithCallback(&args, err);
                जइ(*err==U_BUFFER_OVERFLOW_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(args.target-buffer);
                जइ(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=buffer[1])) {
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
    जइ(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        जइ((length=cnv->UCharErrorBufferLength)>0) {
            uprv_memmove(cnv->UCharErrorBuffer+delta, cnv->UCharErrorBuffer,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UCharErrorBufferLength=(int8_t)(length+delta);

        cnv->UCharErrorBuffer[0]=buffer[i++];
        जइ(delta>1) {
            cnv->UCharErrorBuffer[1]=buffer[i];
        }
    }

    *source=args.source;
    प्रत्यागम c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI शून्य U_EXPORT2
ucnv_convertEx(UConverter *targetCnv, UConverter *sourceCnv,
               वर्ण **target, ध्रुव वर्ण *targetLimit,
               ध्रुव वर्ण **source, ध्रुव वर्ण *sourceLimit,
               char16_t *pivotStart, char16_t **pivotSource,
               char16_t **pivotTarget, ध्रुव char16_t *pivotLimit,
               UBool reset, UBool flush,
               UErrorCode *pErrorCode) {
    char16_t pivotBuffer[CHUNK_SIZE];
    ध्रुव char16_t *myPivotSource;
    char16_t *myPivotTarget;
    ध्रुव वर्ण *s;
    वर्ण *t;

    UConverterToUnicodeArgs toUArgs;
    UConverterFromUnicodeArgs fromUArgs;
    UConverterConvert convert;

    /* error checking */
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }

    जइ( targetCnv==nullptr || sourceCnv==nullptr ||
        source==nullptr || *source==nullptr ||
        target==nullptr || *target==nullptr || targetLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    s=*source;
    t=*target;
    जइ((sourceLimit!=nullptr && sourceLimit<s) || targetLimit<t) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    जइ(
        (sourceLimit!=nullptr && ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    
    जइ(pivotStart==nullptr) {
        जइ(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम;
        }

        /* use the stack pivot buffer */
        myPivotSource=myPivotTarget=pivotStart=pivotBuffer;
        pivotSource=(char16_t **)&myPivotSource;
        pivotTarget=&myPivotTarget;
        pivotLimit=pivotBuffer+CHUNK_SIZE;
    } अण्णहा जइ(  pivotStart>=pivotLimit ||
                pivotSource==nullptr || *pivotSource==nullptr ||
                pivotTarget==nullptr || *pivotTarget==nullptr ||
                pivotLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    जइ(sourceLimit==nullptr) {
        /* get limit of single-byte-NUL-terminated source string */
        sourceLimit=uprv_strchr(*source, 0);
    }

    जइ(reset) {
        ucnv_resetToUnicode(sourceCnv);
        ucnv_resetFromUnicode(targetCnv);
        *pivotSource=*pivotTarget=pivotStart;
    } अण्णहा जइ(targetCnv->charErrorBufferLength>0) {
        /* output the targetCnv overflow buffer */
        जइ(ucnv_outputOverflowFromUnicode(targetCnv, target, targetLimit, nullptr, pErrorCode)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            प्रत्यागम;
        }
        /* *target has moved, therefore stop using t */

        जइ( !flush &&
            targetCnv->preFromULength>=0 && *pivotSource==*pivotTarget &&
            sourceCnv->UCharErrorBufferLength==0 && sourceCnv->preToULength>=0 && s==sourceLimit
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            प्रत्यागम;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    जइ( sourceCnv->sharedData->staticData->conversionType==UCNV_UTF8 &&
        targetCnv->sharedData->impl->fromUTF8!=nullptr
    ) {
        convert=targetCnv->sharedData->impl->fromUTF8;
    } अण्णहा जइ( targetCnv->sharedData->staticData->conversionType==UCNV_UTF8 &&
               sourceCnv->sharedData->impl->toUTF8!=nullptr
    ) {
        convert=sourceCnv->sharedData->impl->toUTF8;
    } अण्णहा {
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
    जइ(convert!=nullptr && (pivotLimit-pivotStart)>32) {
        pivotLimit=pivotStart+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=targetCnv;
    fromUArgs.flush=मिच्छ;
    fromUArgs.offsets=nullptr;
    fromUArgs.target=*target;
    fromUArgs.targetLimit=targetLimit;
    fromUArgs.size=परिमाण(fromUArgs);

    toUArgs.converter=sourceCnv;
    toUArgs.flush=flush;
    toUArgs.offsets=nullptr;
    toUArgs.source=s;
    toUArgs.sourceLimit=sourceLimit;
    toUArgs.targetLimit=pivotLimit;
    toUArgs.size=परिमाण(toUArgs);

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
    प्रति(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        जइ( *pivotSource<*pivotTarget ||
            U_FAILURE(*pErrorCode) ||
            targetCnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.source=*pivotSource;
            fromUArgs.sourceLimit=*pivotTarget;
            _fromUnicodeWithCallback(&fromUArgs, pErrorCode);
            जइ(U_FAILURE(*pErrorCode)) {
                /* target overflow, or conversion error */
                *pivotSource=(char16_t *)fromUArgs.source;
                भङ्ग;
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
        जइ(sourceCnv->UCharErrorBufferLength>0) {
            जइ(ucnv_outputOverflowToUnicode(sourceCnv, pivotTarget, pivotLimit, nullptr, pErrorCode)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *pErrorCode=U_ZERO_ERROR;
            }
            अनुवर्त;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        जइ( toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 && sourceCnv->toULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            भङ्ग;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        जइ(convert!=nullptr && targetCnv->preFromUFirstCP<0 && sourceCnv->preToULength==0) {
            जइ(*pErrorCode==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *pErrorCode=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &toUArgs, pErrorCode);
            जइ(*pErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                भङ्ग;
            } अण्णहा जइ(U_FAILURE(*pErrorCode)) {
                जइ(sourceCnv->toULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } अण्णहा {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *pivotSource=*pivotTarget=pivotStart+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    अनुवर्त;
                }
            } अण्णहा जइ(*pErrorCode==U_USING_DEFAULT_WARNING) {
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
            } अण्णहा जइ(flush && sourceCnv->toULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *pErrorCode=U_TRUNCATED_CHAR_FOUND;
            } अण्णहा {
                /* input consumed */
                जइ(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(sourceCnv, UCNV_RESET_TO_UNICODE, मिच्छ);
                    _reset(targetCnv, UCNV_RESET_FROM_UNICODE, मिच्छ);
                }

                /* done successfully */
                भङ्ग;
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
        जइ(*pErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *pErrorCode=U_ZERO_ERROR;
        } अण्णहा जइ(U_FAILURE(*pErrorCode) || (!flush && *pivotTarget==pivotStart)) {
            /* conversion error, or there was nothing left to convert */
            भङ्ग;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        जइ( flush && toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 &&
            sourceCnv->UCharErrorBufferLength==0
        ) {
            fromUArgs.flush=सच्च;
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
    जइ(flush && U_SUCCESS(*pErrorCode)) {
        जइ(*target!=targetLimit) {
            **target=0;
            जइ(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {
                *pErrorCode=U_ZERO_ERROR;
            }
        } अण्णहा {
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
स्थिर int32_t
ucnv_internalConvert(UConverter *outConverter, UConverter *inConverter,
                     वर्ण *target, int32_t targetCapacity,
                     ध्रुव वर्ण *source, int32_t sourceLength,
                     UErrorCode *pErrorCode) {
    char16_t pivotBuffer[CHUNK_SIZE];
    char16_t *pivot, *pivot2;

    वर्ण *myTarget;
    ध्रुव वर्ण *sourceLimit;
    ध्रुव वर्ण *targetLimit;
    int32_t targetLength=0;

    /* set up */
    जइ(sourceLength<0) {
        sourceLimit=uprv_strchr(source, 0);
    } अण्णहा {
        sourceLimit=source+sourceLength;
    }

    /* if there is no input data, we're done */
    जइ(source==sourceLimit) {
        प्रत्यागम u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    pivot=pivot2=pivotBuffer;
    myTarget=target;
    targetLength=0;

    जइ(targetCapacity>0) {
        /* perform real conversion */
        targetLimit=target+targetCapacity;
        ucnv_convertEx(outConverter, inConverter,
                       &myTarget, targetLimit,
                       &source, sourceLimit,
                       pivotBuffer, &pivot, &pivot2, pivotBuffer+CHUNK_SIZE,
                       मिच्छ,
                       सच्च,
                       pErrorCode);
        targetLength = static_cast<int32_t>(myTarget - target);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    जइ(*pErrorCode==U_BUFFER_OVERFLOW_ERROR || targetCapacity==0)
    {
        वर्ण targetBuffer[CHUNK_SIZE];

        targetLimit=targetBuffer+CHUNK_SIZE;
        करेहि {
            *pErrorCode=U_ZERO_ERROR;
            myTarget=targetBuffer;
            ucnv_convertEx(outConverter, inConverter,
                           &myTarget, targetLimit,
                           &source, sourceLimit,
                           pivotBuffer, &pivot, &pivot2, pivotBuffer+CHUNK_SIZE,
                           मिच्छ,
                           सच्च,
                           pErrorCode);
            targetLength += static_cast<int32_t>(myTarget - targetBuffer);
        } जाव(*pErrorCode==U_BUFFER_OVERFLOW_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        प्रत्यागम u_terminateChars(target, targetCapacity, targetLength, pErrorCode);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    प्रत्यागम targetLength;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ध्रुव वर्ण *toConverterName, ध्रुव वर्ण *fromConverterName,
             वर्ण *target, int32_t targetCapacity,
             ध्रुव वर्ण *source, int32_t sourceLength,
             UErrorCode *pErrorCode) {
    UConverter in, out; /* stack-allocated */
    UConverter *inConverter, *outConverter;
    int32_t targetLength;

    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    जइ( source==nullptr || sourceLength<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* if there is no input data, we're done */
    जइ(sourceLength==0 || (sourceLength<0 && *source==0)) {
        प्रत्यागम u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the converters */
    inConverter=ucnv_createConverter(&in, fromConverterName, pErrorCode);
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    outConverter=ucnv_createConverter(&out, toConverterName, pErrorCode);
    जइ(U_FAILURE(*pErrorCode)) {
        ucnv_close(inConverter);
        प्रत्यागम 0;
    }

    targetLength=ucnv_internalConvert(outConverter, inConverter,
                                      target, targetCapacity,
                                      source, sourceLength,
                                      pErrorCode);

    ucnv_close(inConverter);
    ucnv_close(outConverter);

    प्रत्यागम targetLength;
}

/* @internal */
स्थिर int32_t
ucnv_convertAlgorithmic(UBool convertToAlgorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        वर्ण *target, int32_t targetCapacity,
                        ध्रुव वर्ण *source, int32_t sourceLength,
                        UErrorCode *pErrorCode) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *to, *from;
    int32_t targetLength;

    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    जइ( cnv==nullptr || source==nullptr || sourceLength<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* if there is no input data, we're done */
    जइ(sourceLength==0 || (sourceLength<0 && *source==0)) {
        प्रत्यागम u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, pErrorCode);
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* reset the other converter */
    जइ(convertToAlgorithmic) {
        /* cnv->Unicode->algo */
        ucnv_resetToUnicode(cnv);
        to=algoConverter;
        from=cnv;
    } अण्णहा {
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

    प्रत्यागम targetLength;
}

U_CAPI int32_t U_EXPORT2
ucnv_toAlgorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   वर्ण *target, int32_t targetCapacity,
                   ध्रुव वर्ण *source, int32_t sourceLength,
                   UErrorCode *pErrorCode) {
    प्रत्यागम ucnv_convertAlgorithmic(सच्च, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, sourceLength,
                                   pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     वर्ण *target, int32_t targetCapacity,
                     ध्रुव वर्ण *source, int32_t sourceLength,
                     UErrorCode *pErrorCode) UPRV_NO_SANITIZE_UNDEFINED {

    जइ(algorithmicType<0 || UCNV_सङ्ख्या_OF_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    प्रत्यागम ucnv_convertAlgorithmic(मिच्छ, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, sourceLength,
                                   pErrorCode);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ध्रुव UConverter* converter)
{
    int8_t type = converter->sharedData->staticData->conversionType;
#जइ !UCONFIG_NO_LEGACY_CONVERSION
    जइ(type == UCNV_MBCS) {
        प्रत्यागम ucnv_MBCSGetType(converter);
    }
#endif
    प्रत्यागम (UConverterType)type;
}

U_CAPI शून्य  U_EXPORT2
ucnv_getStarters(ध्रुव UConverter* converter, 
                 UBool starters[256],
                 UErrorCode* err)
{
    जइ (err == nullptr || U_FAILURE(*err)) {
        प्रत्यागम;
    }

    जइ(converter->sharedData->impl->getStarters != nullptr) {
        converter->sharedData->impl->getStarters(converter, starters, err);
    } अण्णहा {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

स्थिर ध्रुव UAmbiguousConverter *ucnv_getAmbiguous(ध्रुव UConverter *cnv)
{
    UErrorCode errorCode;
    ध्रुव वर्ण *name;
    int32_t i;

    जइ(cnv==nullptr) {
        प्रत्यागम nullptr;
    }

    errorCode=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &errorCode);
    जइ(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }

    प्रति(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        जइ(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            प्रत्यागम ambiguousConverters+i;
        }
    }

    प्रत्यागम nullptr;
}

U_CAPI शून्य  U_EXPORT2
ucnv_fixFileSeparator(ध्रुव UConverter *cnv, 
                      char16_t* source,
                      int32_t sourceLength) {
    ध्रुव UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    जइ(cnv==nullptr || source==nullptr || sourceLength<=0 || (a=ucnv_getAmbiguous(cnv))==nullptr)
    {
        प्रत्यागम;
    }

    variant5c=a->variant5c;
    प्रति(i=0; i<sourceLength; ++i) {
        जइ(source[i]==variant5c) {
            source[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ध्रुव UConverter *cnv) {
    प्रत्यागम ucnv_getAmbiguous(cnv)!=nullptr;
}

U_CAPI शून्य  U_EXPORT2
ucnv_setFallback(UConverter *cnv, UBool usesFallback)
{
    cnv->useFallback = usesFallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_usesFallback(ध्रुव UConverter *cnv)
{
    प्रत्यागम cnv->useFallback;
}

U_CAPI शून्य  U_EXPORT2
ucnv_getInvalidChars (ध्रुव UConverter * converter,
                      वर्ण *errBytes,
                      int8_t * len,
                      UErrorCode * err)
{
    जइ (err == nullptr || U_FAILURE(*err))
    {
        प्रत्यागम;
    }
    जइ (len == nullptr || errBytes == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    जइ (*len < converter->invalidCharLength)
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }
    जइ ((*len = converter->invalidCharLength) > 0)
    {
        uprv_memcpy (errBytes, converter->invalidCharBuffer, *len);
    }
}

U_CAPI शून्य  U_EXPORT2
ucnv_getInvalidUChars (ध्रुव UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UErrorCode * err)
{
    जइ (err == nullptr || U_FAILURE(*err))
    {
        प्रत्यागम;
    }
    जइ (len == nullptr || errChars == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    जइ (*len < converter->invalidUCharLength)
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }
    जइ ((*len = converter->invalidUCharLength) > 0)
    {
        u_memcpy (errChars, converter->invalidUCharBuffer, *len);
    }
}

#define SIG_MAX_LEN 5

U_CAPI ध्रुव वर्ण* U_EXPORT2
ucnv_detectUnicodeSignature( ध्रुव वर्ण* source,
                             int32_t sourceLength,
                             int32_t* signatureLength,
                             UErrorCode* pErrorCode) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    वर्ण start[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    पूर्णाङ्क i = 0;

    जइ((pErrorCode==nullptr) || U_FAILURE(*pErrorCode)){
        प्रत्यागम nullptr;
    }
    
    जइ(source == nullptr || sourceLength < -1){
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(signatureLength == nullptr) {
        signatureLength = &dummy;
    }

    जइ(sourceLength==-1){
        sourceLength=(int32_t)uprv_strlen(source);
    }

    
    जाव(i<sourceLength&& i<SIG_MAX_LEN){
        start[i]=source[i];
        i++;
    }

    जइ(start[0] == '\xFE' && start[1] == '\xFF') {
        *signatureLength=2;
        प्रत्यागम  "UTF-16BE";
    } अण्णहा जइ(start[0] == '\xFF' && start[1] == '\xFE') {
        जइ(start[2] == '\x00' && start[3] =='\x00') {
            *signatureLength=4;
            प्रत्यागम "UTF-32LE";
        } अण्णहा {
            *signatureLength=2;
            प्रत्यागम  "UTF-16LE";
        }
    } अण्णहा जइ(start[0] == '\xEF' && start[1] == '\xBB' && start[2] == '\xBF') {
        *signatureLength=3;
        प्रत्यागम  "UTF-8";
    } अण्णहा जइ(start[0] == '\x00' && start[1] == '\x00' && 
              start[2] == '\xFE' && start[3]=='\xFF') {
        *signatureLength=4;
        प्रत्यागम  "UTF-32BE";
    } अण्णहा जइ(start[0] == '\x0E' && start[1] == '\xFE' && start[2] == '\xFF') {
        *signatureLength=3;
        प्रत्यागम "SCSU";
    } अण्णहा जइ(start[0] == '\xFB' && start[1] == '\xEE' && start[2] == '\x28') {
        *signatureLength=3;
        प्रत्यागम "BOCU-1";
    } अण्णहा जइ(start[0] == '\x2B' && start[1] == '\x2F' && start[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        जइ(start[3] == '\x38' && start[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signatureLength=5;
            प्रत्यागम "UTF-7";
        } अण्णहा जइ(start[3] == '\x38' || start[3] == '\x39' || start[3] == '\x2B' || start[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signatureLength=4;
            प्रत्यागम "UTF-7";
        }
    }अण्णहा जइ(start[0]=='\xDD' && start[1]== '\x73'&& start[2]=='\x66' && start[3]=='\x73'){
        *signatureLength=4;
        प्रत्यागम "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signatureLength=0;
    प्रत्यागम nullptr;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ध्रुव UConverter* cnv, UErrorCode* status)
{
    जइ(status == nullptr || U_FAILURE(*status)){
        प्रत्यागम -1;
    }
    जइ(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम -1;
    }

    जइ(cnv->preFromUFirstCP >= 0){
        प्रत्यागम U16_LENGTH(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }अण्णहा जइ(cnv->preFromULength < 0){
        प्रत्यागम -cnv->preFromULength ;
    }अण्णहा जइ(cnv->fromUChar32 > 0){
        प्रत्यागम 1;
    }
    प्रत्यागम 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_toUCountPending(ध्रुव UConverter* cnv, UErrorCode* status){

    जइ(status == nullptr || U_FAILURE(*status)){
        प्रत्यागम -1;
    }
    जइ(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम -1;
    }

    जइ(cnv->preToULength > 0){
        प्रत्यागम cnv->preToULength ;
    }अण्णहा जइ(cnv->preToULength < 0){
        प्रत्यागम -cnv->preToULength;
    }अण्णहा जइ(cnv->toULength > 0){
        प्रत्यागम cnv->toULength;
    }
    प्रत्यागम 0;
}

U_CAPI UBool U_EXPORT2
ucnv_isFixedWidth(UConverter *cnv, UErrorCode *status){
    जइ (U_FAILURE(*status)) {
        प्रत्यागम मिच्छ;
    }

    जइ (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम मिच्छ;
    }

    विकल्प (ucnv_getType(cnv)) {
        पक्ष UCNV_SBCS:
        पक्ष UCNV_DBCS:
        पक्ष UCNV_UTF32_BigEndian:
        पक्ष UCNV_UTF32_LittleEndian:
        पक्ष UCNV_UTF32:
        पक्ष UCNV_US_ASCII:
            प्रत्यागम सच्च;
        स्वभाव:
            प्रत्यागम मिच्छ;
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
