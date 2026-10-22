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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل <memory>

#شامل "unicode/ustring.h"
#شامل "unicode/ucnv.h"
#شامل "unicode/ucnv_err.h"
#شامل "unicode/uset.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf16.h"
#شامل "putilimp.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "uassert.h"
#شامل "utracimp.h"
#شامل "ustr_imp.h"
#شامل "ucnv_imp.h"
#شامل "ucnv_cnv.h"
#شامل "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#define CHUNK_SIZE 1024

تعریفنوع ساختار UAmbiguousConverter {
    ثابت نویسه *name;
    ثابت char16_t variant5c;
} UAmbiguousConverter;

ایستا ثابت UAmbiguousConverter ambiguousConverters[]={
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
ucnv_open (ثابت نویسه *name,
                       UErrorCode * err)
{
    UConverter *r;

    اگر (err == nullptr || U_FAILURE (*err)) {
        بازگشت nullptr;
    }

    r =  ucnv_createConverter(nullptr, name, err);
    بازگشت r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ثابت نویسه *packageName, ثابت نویسه *converterName, UErrorCode * err)
{
    بازگشت ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ثابت char16_t * name,
                         UErrorCode * err)
{
    نویسه asciiName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    اگر (err == nullptr || U_FAILURE(*err))
        بازگشت nullptr;
    اگر (name == nullptr)
        بازگشت ucnv_open (nullptr, err);
    اگر (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_LENGTH)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    بازگشت ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
ایستا int32_t
ucnv_copyPlatformString(نویسه *platformString, UConverterPlatform pltfrm)
{
    انتخاب (pltfrm)
    {
    حالت UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        بازگشت 4;
    حالت UCNV_UNKNOWN:
        بشکن;
    }

    /* default to empty string */
    *platformString = 0;
    بازگشت 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UErrorCode * err)
{
    نویسه myName[UCNV_MAX_CONVERTER_NAME_LENGTH];
    int32_t myNameLen;

    اگر (err == nullptr || U_FAILURE (*err))
        بازگشت nullptr;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integerToString(myName + myNameLen, codepage, 10);

    بازگشت ucnv_createConverter(nullptr, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_safeClone(ثابت UConverter* cnv,
               تهی* پشته_Buffer U_LIFETIME_BOUND,
               int32_t* pBufferSize,
               UErrorCode* status)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t پشته_Buffer_Size;
    int32_t bufferSizeNeeded;
    UErrorCode cbErr;
    UConverterToUnicodeArgs toUArgs = {
        اندازه(UConverterToUnicodeArgs),
            درست,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };
    UConverterFromUnicodeArgs fromUArgs = {
        اندازه(UConverterFromUnicodeArgs),
            درست,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLONE);

    اگر (status == nullptr || U_FAILURE(*status)){
        UTRACE_EXIT_STATUS(status? *status: U_ILLEGAL_ARGUMENT_ERROR);
        بازگشت nullptr;
    }

    اگر (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(*status);
        بازگشت nullptr;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, status), cnv, پشته_Buffer);

    اگر (cnv->sharedData->impl->safeClone != nullptr) {
        /* call the custom safeClone function for sizing */
        bufferSizeNeeded = 0;
        cnv->sharedData->impl->safeClone(cnv, nullptr, &bufferSizeNeeded, status);
        اگر (U_FAILURE(*status)) {
            UTRACE_EXIT_STATUS(*status);
            بازگشت nullptr;
        }
    }
    وگرنه
    {
        /* inherent sizing */
        bufferSizeNeeded = اندازه(UConverter);
    }

    اگر (pBufferSize == nullptr) {
        پشته_Buffer_Size = 1;
        pBufferSize = &پشته_Buffer_Size;
    } وگرنه {
        پشته_Buffer_Size = *pBufferSize;
        اگر (پشته_Buffer_Size <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *pBufferSize = bufferSizeNeeded;
            UTRACE_EXIT_مقدار(bufferSizeNeeded);
            بازگشت nullptr;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    اگر (پشته_Buffer) {
        uintptr_t p = reinterpret_cast<uintptr_t>(پشته_Buffer);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t pointerAdjustment = aligned_p - p;
        اگر (bufferSizeNeeded + pointerAdjustment <= پشته_Buffer_Size) {
            پشته_Buffer = reinterpret_cast<تهی *>(aligned_p);
            پشته_Buffer_Size -= static_cast<int32_t>(pointerAdjustment);
        } وگرنه {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            پشته_Buffer_Size = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    اگر (پشته_Buffer_Size < bufferSizeNeeded || پشته_Buffer == nullptr)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (bufferSizeNeeded);

        اگر(localConverter == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            UTRACE_EXIT_STATUS(*status);
            بازگشت nullptr;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        اگر (pBufferSize != &پشته_Buffer_Size) {
            *status = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *pBufferSize = bufferSizeNeeded;
    } وگرنه {
        /* just use the stack buffer */
        localConverter = (UConverter*) پشته_Buffer;
        allocatedConverter = nullptr;
    }

    uprv_memset(localConverter, 0, bufferSizeNeeded);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, اندازه(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = نادرست;

    /* copy the substitution string */
    اگر (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } وگرنه {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
        اگر (localConverter->subChars == nullptr) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_STATUS(*status);
            بازگشت nullptr;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    اگر (cnv->sharedData->impl->safeClone != nullptr) {
        /* call the custom safeClone function */
        localConverter = cnv->sharedData->impl->safeClone(cnv, localConverter, pBufferSize, status);
    }

    اگر(localConverter==nullptr || U_FAILURE(*status)) {
        اگر (allocatedConverter != nullptr && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_STATUS(*status);
        بازگشت nullptr;
    }

    /* increment refcount of shared data if needed */
    اگر (cnv->sharedData->isReferenceCounted) {
        ucnv_increment_Ref_شمار(cnv->sharedData);
    }

    اگر(localConverter == (UConverter*)پشته_Buffer) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = درست;
    }

    /* allow callback functions to handle any memory allocation */
    toUArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->toUContext, &toUArgs, nullptr, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, nullptr, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_STATUS(localConverter, *status);
    بازگشت localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ثابت UConverter* cnv, UErrorCode *status)
{
    بازگشت ucnv_safeClone(cnv, nullptr, nullptr, status);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI تهی  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLOSE);

    اگر (converter == nullptr)
    {
        UTRACE_EXIT();
        بازگشت;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &errorCode), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    اگر (converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
        UConverterToUnicodeArgs toUArgs = {
            اندازه(UConverterToUnicodeArgs),
                درست,
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
    اگر (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverterFromUnicodeArgs fromUArgs = {
            اندازه(UConverterFromUnicodeArgs),
                درست,
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

    اگر (converter->sharedData->impl->close != nullptr) {
        converter->sharedData->impl->close(converter);
    }

    اگر (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    اگر (converter->sharedData->isReferenceCounted) {
        ucnv_unloadSharedDataIfReady(converter->sharedData);
    }

    اگر(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ثابت نویسه*   U_EXPORT2
ucnv_getAvailableName (int32_t n)
{
    اگر (0 <= n && n <= 0xffff) {
        UErrorCode err = U_ZERO_ERROR;
        ثابت نویسه *name = ucnv_bld_getAvailableConverter((uint16_t)n, &err);
        اگر (U_SUCCESS(err)) {
            بازگشت name;
        }
    }
    بازگشت nullptr;
}

U_CAPI int32_t   U_EXPORT2
ucnv_شمار_Available ()
{
    UErrorCode err = U_ZERO_ERROR;
    بازگشت ucnv_bld_شمار_Available_Converters(&err);
}

U_CAPI تهی    U_EXPORT2
ucnv_getSubstChars (ثابت UConverter * converter,
                    نویسه *mySubChar,
                    int8_t * len,
                    UErrorCode * err)
{
    اگر (U_FAILURE (*err))
        بازگشت;

    اگر (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        بازگشت;
    }

    اگر (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI تهی    U_EXPORT2
ucnv_setSubstChars (UConverter * converter,
                    ثابت نویسه *mySubChar,
                    int8_t len,
                    UErrorCode * err)
{
    اگر (U_FAILURE (*err))
        بازگشت;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    اگر ((len > converter->sharedData->staticData->maxBytesPerChar)
     || (len < converter->sharedData->staticData->minBytesPerChar))
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
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

U_CAPI تهی U_EXPORT2
ucnv_setSubstString(UConverter *cnv,
                    ثابت char16_t *s,
                    int32_t length,
                    UErrorCode *err) {
    alignas(UConverter) نویسه cloneBuffer[U_CNV_SAFECLONE_BUFFERSIZE];
    نویسه chars[UCNV_ERROR_BUFFER_LENGTH];

    UConverter *clone;
    uint8_t *subChars;
    int32_t cloneSize, length8;

    /* Let the following functions check all arguments. */
    cloneSize = اندازه(cloneBuffer);
    clone = ucnv_safeClone(cnv, cloneBuffer, &cloneSize, err);
    ucnv_setFromUCallBack(clone, UCNV_FROM_U_CALLBACK_STOP, nullptr, nullptr, nullptr, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)اندازه(chars), s, length, err);
    ucnv_close(clone);
    اگر (U_FAILURE(*err)) {
        بازگشت;
    }

    اگر (cnv->sharedData->impl->writeSub == nullptr
#اگر !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->sharedData->staticData->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#endif
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } وگرنه {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        اگر (length > UCNV_ERROR_BUFFER_LENGTH) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_BUFFER_OVERFLOW_ERROR;
            بازگشت;
        }
        subChars = (uint8_t *)s;
        اگر (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    اگر (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        اگر (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
            اگر (cnv->subChars == nullptr) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    اگر (length8 == 0) {
        cnv->subCharLen = 0;
    } وگرنه {
        uprv_memcpy(cnv->subChars, subChars, length8);
        اگر (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } وگرنه /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
ایستا تهی _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    اگر(converter == nullptr) {
        بازگشت;
    }

    اگر(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UErrorCode errorCode;

        اگر(choice<=UCNV_RESET_TO_UNICODE && converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
            UConverterToUnicodeArgs toUArgs = {
                اندازه(UConverterToUnicodeArgs),
                درست,
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
        اگر(choice!=UCNV_RESET_TO_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverterFromUnicodeArgs fromUArgs = {
                اندازه(UConverterFromUnicodeArgs),
                درست,
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
    اگر(choice<=UCNV_RESET_TO_UNICODE) {
        converter->toUnicodeStatus = converter->sharedData->toUnicodeStatus;
        converter->mode = 0;
        converter->toULength = 0;
        converter->invalidCharLength = converter->UCharErrorBufferLength = 0;
        converter->preToULength = 0;
    }
    اگر(choice!=UCNV_RESET_TO_UNICODE) {
        converter->fromUnicodeStatus = 0;
        converter->fromUChar32 = 0;
        converter->invalidUCharLength = converter->charErrorBufferLength = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    اگر (converter->sharedData->impl->reset != nullptr) {
        /* call the custom reset function */
        converter->sharedData->impl->reset(converter, choice);
    }
}

U_CAPI تهی  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, درست);
}

U_CAPI تهی  U_EXPORT2
ucnv_resetToUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_TO_UNICODE, درست);
}

U_CAPI تهی  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, درست);
}

U_CAPI int8_t   U_EXPORT2
ucnv_getMaxCharSize (ثابت UConverter * converter)
{
    بازگشت converter->maxBytesPerUChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_getMinCharSize (ثابت UConverter * converter)
{
    بازگشت converter->sharedData->staticData->minBytesPerChar;
}

U_CAPI ثابت نویسه* U_EXPORT2
ucnv_getName(ثابت UConverter* converter U_LIFETIME_BOUND, UErrorCode* err)
{
    اگر (U_FAILURE (*err))
        بازگشت nullptr;
    اگر(converter->sharedData->impl->getName){
        ثابت نویسه* temp= converter->sharedData->impl->getName(converter);
        اگر(temp)
            بازگشت temp;
    }
    بازگشت converter->sharedData->staticData->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ثابت UConverter * converter,
              UErrorCode * err)
{
    int32_t ccsid;
    اگر (U_FAILURE (*err))
        بازگشت -1;

    ccsid = converter->sharedData->staticData->codepage;
    اگر (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        ثابت نویسه *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        اگر (U_SUCCESS(*err) && standardName) {
            ثابت نویسه *ccsidStr = uprv_strchr(standardName, '-');
            اگر (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    بازگشت ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (ثابت UConverter * converter,
                                      UErrorCode * err)
{
    اگر (U_FAILURE (*err))
        بازگشت UCNV_UNKNOWN;

    بازگشت (UConverterPlatform)converter->sharedData->staticData->platform;
}

U_CAPI تهی U_EXPORT2
    ucnv_getToUCallBack (ثابت UConverter * converter,
                         UConverterToUCallback *action,
                         ثابت تهی **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->toUContext;
}

U_CAPI تهی U_EXPORT2
    ucnv_getFromUCallBack (ثابت UConverter * converter,
                           UConverterFromUCallback *action,
                           ثابت تهی **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI تهی    U_EXPORT2
ucnv_setToUCallBack (UConverter * converter,
                            UConverterToUCallback newAction,
                            ثابت تهی* newContext,
                            UConverterToUCallback *oldAction,
                            ثابت تهی** oldContext,
                            UErrorCode * err)
{
    اگر (U_FAILURE (*err))
        بازگشت;
    اگر (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    اگر (oldContext) *oldContext = converter->toUContext;
    converter->toUContext = newContext;
}

U_CAPI تهی  U_EXPORT2
ucnv_setFromUCallBack (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ثابت تهی* newContext,
                            UConverterFromUCallback *oldAction,
                            ثابت تهی** oldContext,
                            UErrorCode * err)
{
    اگر (U_FAILURE (*err))
        بازگشت;
    اگر (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    اگر (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

ایستا تهی
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t sourceIndex, int32_t error_درون_داد_Length) {
    int32_t *limit;
    int32_t delta, offset;

    اگر(sourceIndex>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=sourceIndex-error_درون_داد_Length;
    } وگرنه {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    limit=offsets+length;
    اگر(delta==0) {
        /* most common case, nothing to do */
    } وگرنه اگر(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        وقتی(offsets<limit) {
            offset=*offsets;
            اگر(offset>=0) {
                *offsets=offset+delta;
            }
            ++offsets;
        }
    } وگرنه /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        وقتی(offsets<limit) {
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

ایستا تهی
_fromUnicodeWithCallback(UConverterFromUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ثابت char16_t *s;
    نویسه *t;
    int32_t *offsets;
    int32_t sourceIndex;
    int32_t error_درون_داد_Length;
    UBool converter_Saw_End_Of_درون_داد, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ثابت char16_t *realSource, *realSourceLimit;
    int32_t realSourceIndex;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    sourceIndex=0;
    اگر(offsets==nullptr) {
        fromUnicode=cnv->sharedData->impl->fromUnicode;
    } وگرنه {
        fromUnicode=cnv->sharedData->impl->fromUnicodeWithOffsets;
        اگر(fromUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->sharedData->impl->fromUnicode;
            /* we will write -1 for each offset */
            sourceIndex=-1;
        }
    }

    اگر(cnv->preFromULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=نادرست;
        realSourceIndex=0;
    } وگرنه {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        اگر (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        realSourceIndex=sourceIndex;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preFromULength;
        pArgs->flush=نادرست;
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
    برای(;;) {
        اگر(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_End_Of_درون_داد=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->fromUChar32==0);
        } وگرنه {
            /* handle error from ucnv_convertEx() */
            converter_Saw_End_Of_درون_داد=نادرست;
        }

        /* no callback called yet for this iteration */
        calledCallback=نادرست;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_درون_داد_Length=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        برای(;;) {
            /* update offsets if we write any */
            اگر(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                اگر(length>0) {
                    _updateOffsets(offsets, length, sourceIndex, error_درون_داد_Length);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                اگر(sourceIndex>=0) {
                    sourceIndex += static_cast<int32_t>(pArgs->source - s);
                }
            }

            اگر(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                اگر(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    realSourceIndex=sourceIndex;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preFromULength;
                    pArgs->flush=نادرست;
                    اگر((sourceIndex+=cnv->preFromULength)<0) {
                        sourceIndex=-1;
                    }

                    cnv->preFromULength=0;
                } وگرنه {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            اگر(U_SUCCESS(*err)) {
                اگر(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    بشکن;
                } وگرنه اگر(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    sourceIndex=realSourceIndex;

                    realSource=nullptr;
                    بشکن;
                } وگرنه اگر(pArgs->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=نادرست; /* new error condition */
                } وگرنه {
                    /* input consumed */
                    اگر(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        اگر(!converter_Saw_End_Of_درون_داد) {
                            بشکن;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, نادرست);
                    }

                    /* done successfully */
                    بازگشت;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                اگر( calledCallback ||
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
                    اگر(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        اگر(length>0) {
                            u_memcpy(cnv->preFromU, pArgs->source, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    بازگشت;
                }
            }

            /* callback handling */
            {
                UChar32 codePoint;

                /* get and write the code point */
                codePoint=cnv->fromUChar32;
                error_درون_داد_Length=0;
                U16_APPEND_UNSAFE(cnv->invalidUCharBuffer, error_درون_داد_Length, codePoint);
                cnv->invalidUCharLength = static_cast<int8_t>(error_درون_داد_Length);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, pArgs,
                    cnv->invalidUCharBuffer, error_درون_داد_Length, codePoint,
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
            calledCallback=درست;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
ایستا UBool
ucnv_outputOverflowFromUnicode(UConverter *cnv,
                               نویسه **target, ثابت نویسه *targetLimit,
                               int32_t **pOffsets,
                               UErrorCode *err) {
    int32_t *offsets;
    نویسه *overflow, *t;
    int32_t i, length;

    t=*target;
    اگر(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } وگرنه {
        offsets=nullptr;
    }

    overflow = reinterpret_cast<نویسه*>(cnv->charErrorBuffer);
    length=cnv->charErrorBufferLength;
    i=0;
    وقتی(i<length) {
        اگر(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            انجامبده {
                overflow[j++]=overflow[i++];
            } وقتی(i<length);

            cnv->charErrorBufferLength = static_cast<int8_t>(j);
            *target=t;
            اگر(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_BUFFER_OVERFLOW_ERROR;
            بازگشت درست;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        اگر(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->charErrorBufferLength=0;
    *target=t;
    اگر(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    بازگشت نادرست;
}

U_CAPI تهی U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 نویسه **target, ثابت نویسه *targetLimit,
                 ثابت char16_t **source, ثابت char16_t *sourceLimit,
                 int32_t *offsets,
                 UBool flush,
                 UErrorCode *err) {
    UConverterFromUnicodeArgs args;
    ثابت char16_t *s;
    نویسه *t;

    /* check parameters */
    اگر(err==nullptr || U_FAILURE(*err)) {
        بازگشت;
    }

    اگر(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    s=*source;
    t=*target;

    اگر ((ثابت تهی *)U_MAX_PTR(sourceLimit) == (ثابت تهی *)sourceLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        sourceLimit = (ثابت char16_t *)(((ثابت نویسه *)sourceLimit) - 1);
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
    اگر (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x3fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t) ||
        (((ثابت نویسه *)sourceLimit-(ثابت نویسه *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    
    /* output the target overflow buffer */
    اگر( cnv->charErrorBufferLength>0 &&
        ucnv_outputOverflowFromUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        بازگشت;
    }
    /* *target may have moved, therefore stop using t */

    اگر(!flush && s==sourceLimit && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        بازگشت;
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
    args.size=اندازه(args);

    _fromUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

ایستا تهی
_toUnicodeWithCallback(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterToUnicode toUnicode;
    UConverter *cnv;
    ثابت نویسه *s;
    char16_t *t;
    int32_t *offsets;
    int32_t sourceIndex;
    int32_t error_درون_داد_Length;
    UBool converter_Saw_End_Of_درون_داد, calledCallback;

    /* variables for m:n conversion */
    نویسه replay[UCNV_EXT_MAX_BYTES];
    ثابت نویسه *realSource, *realSourceLimit;
    int32_t realSourceIndex;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    sourceIndex=0;
    اگر(offsets==nullptr) {
        toUnicode=cnv->sharedData->impl->toUnicode;
    } وگرنه {
        toUnicode=cnv->sharedData->impl->toUnicodeWithOffsets;
        اگر(toUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            toUnicode=cnv->sharedData->impl->toUnicode;
            /* we will write -1 for each offset */
            sourceIndex=-1;
        }
    }

    اگر(cnv->preToULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=نادرست;
        realSourceIndex=0;
    } وگرنه {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        اگر (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        realSourceIndex=sourceIndex;

        uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preToULength;
        pArgs->flush=نادرست;
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
    برای(;;) {
        اگر(U_SUCCESS(*err)) {
            /* convert */
            toUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_End_Of_درون_داد=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->toULength==0);
        } وگرنه {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_End_Of_درون_داد=نادرست;
        }

        /* no callback called yet for this iteration */
        calledCallback=نادرست;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_درون_داد_Length=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        برای(;;) {
            /* update offsets if we write any */
            اگر(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                اگر(length>0) {
                    _updateOffsets(offsets, length, sourceIndex, error_درون_داد_Length);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                اگر(sourceIndex>=0) {
                    sourceIndex += static_cast<int32_t>(pArgs->source - s);
                }
            }

            اگر(cnv->preToULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                اگر(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    realSourceIndex=sourceIndex;

                    uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preToULength;
                    pArgs->flush=نادرست;
                    اگر((sourceIndex+=cnv->preToULength)<0) {
                        sourceIndex=-1;
                    }

                    cnv->preToULength=0;
                } وگرنه {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            اگر(U_SUCCESS(*err)) {
                اگر(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    بشکن;
                } وگرنه اگر(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    sourceIndex=realSourceIndex;

                    realSource=nullptr;
                    بشکن;
                } وگرنه اگر(pArgs->flush && cnv->toULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=نادرست; /* new error condition */
                } وگرنه {
                    /* input consumed */
                    اگر(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        اگر(!converter_Saw_End_Of_درون_داد) {
                            بشکن;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_TO_UNICODE, نادرست);
                    }

                    /* done successfully */
                    بازگشت;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                اگر( calledCallback ||
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
                    اگر(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preToULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        اگر(length>0) {
                            uprv_memcpy(cnv->preToU, pArgs->source, length);
                            cnv->preToULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    بازگشت;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_درون_داد_Length=cnv->invalidCharLength=cnv->toULength;
            اگر(error_درون_داد_Length>0) {
                uprv_memcpy(cnv->invalidCharBuffer, cnv->toUBytes, error_درون_داد_Length);
            }

            /* set the converter state to deal with the next character */
            cnv->toULength=0;

            /* call the callback function */
            اگر(cnv->toUCallbackReason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_FOUND) {
                cnv->toUCallbackReason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->toUContext, pArgs,
                cnv->invalidCharBuffer, error_درون_داد_Length,
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
            calledCallback=درست;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
ایستا UBool
ucnv_outputOverflowToUnicode(UConverter *cnv,
                             char16_t **target, ثابت char16_t *targetLimit,
                             int32_t **pOffsets,
                             UErrorCode *err) {
    int32_t *offsets;
    char16_t *overflow, *t;
    int32_t i, length;

    t=*target;
    اگر(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } وگرنه {
        offsets=nullptr;
    }

    overflow=cnv->UCharErrorBuffer;
    length=cnv->UCharErrorBufferLength;
    i=0;
    وقتی(i<length) {
        اگر(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            انجامبده {
                overflow[j++]=overflow[i++];
            } وقتی(i<length);

            cnv->UCharErrorBufferLength = static_cast<int8_t>(j);
            *target=t;
            اگر(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_BUFFER_OVERFLOW_ERROR;
            بازگشت درست;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        اگر(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UCharErrorBufferLength=0;
    *target=t;
    اگر(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    بازگشت نادرست;
}

U_CAPI تهی U_EXPORT2
ucnv_toUnicode(UConverter *cnv,
               char16_t **target, ثابت char16_t *targetLimit,
               ثابت نویسه **source, ثابت نویسه *sourceLimit,
               int32_t *offsets,
               UBool flush,
               UErrorCode *err) {
    UConverterToUnicodeArgs args;
    ثابت نویسه *s;
    char16_t *t;

    /* check parameters */
    اگر(err==nullptr || U_FAILURE(*err)) {
        بازگشت;
    }

    اگر(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    s=*source;
    t=*target;

    اگر ((ثابت تهی *)U_MAX_PTR(targetLimit) == (ثابت تهی *)targetLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        targetLimit = (ثابت char16_t *)(((ثابت نویسه *)targetLimit) - 1);
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
    اگر (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x3fffffff && targetLimit>t) ||
        (((ثابت نویسه *)targetLimit-(ثابت نویسه *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    
    /* output the target overflow buffer */
    اگر( cnv->UCharErrorBufferLength>0 &&
        ucnv_outputOverflowToUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        بازگشت;
    }
    /* *target may have moved, therefore stop using t */

    اگر(!flush && s==sourceLimit && cnv->preToULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        بازگشت;
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
    args.size=اندازه(args);

    _toUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                نویسه *dest, int32_t destCapacity,
                ثابت char16_t *src, int32_t srcLength,
                UErrorCode *pErrorCode) {
    ثابت char16_t *srcLimit;
    نویسه *originalDest, *destLimit;
    int32_t destLength;

    /* check arguments */
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    اگر( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        srcLength<-1 || (srcLength!=0 && src==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    اگر(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    اگر(srcLength>0) {
        srcLimit=src+srcLength;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, درست, &bufferStatus);
        destLength=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        اگر(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
            نویسه buffer[1024];

            destLimit=buffer+اندازه(buffer);
            انجامبده {
                dest=buffer;
                bufferStatus=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, درست, &bufferStatus);
                destLength+=(int32_t)(dest-buffer);
            } وقتی(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
        }
        اگر (U_FAILURE(bufferStatus)) {
            *pErrorCode = bufferStatus;
        }
    } وگرنه {
        destLength=0;
    }

    بازگشت u_terminateChars(originalDest, destCapacity, destLength, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_toUChars(UConverter *cnv,
              char16_t *dest, int32_t destCapacity,
              ثابت نویسه *src, int32_t srcLength,
              UErrorCode *pErrorCode) {
    ثابت نویسه *srcLimit;
    char16_t *originalDest, *destLimit;
    int32_t destLength;

    /* check arguments */
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    اگر( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        srcLength<-1 || (srcLength!=0 && src==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    /* initialize */
    ucnv_resetToUnicode(cnv);
    originalDest=dest;
    اگر(srcLength==-1) {
        srcLength=(int32_t)uprv_strlen(src);
    }
    اگر(srcLength>0) {
        srcLimit=src+srcLength;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, درست, &bufferStatus);
        destLength=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        اگر(bufferStatus==U_BUFFER_OVERFLOW_ERROR)
        {
            char16_t buffer[1024];

            destLimit=buffer+UPRV_LENGTHOF(buffer);
            انجامبده {
                dest=buffer;
                bufferStatus=U_ZERO_ERROR;
                ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, درست, &bufferStatus);
                destLength+=(int32_t)(dest-buffer);
            }
            وقتی(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
        }
        اگر (U_FAILURE(bufferStatus)) {
            *pErrorCode = bufferStatus;
        }
    } وگرنه {
        destLength=0;
    }

    بازگشت u_terminateUChars(originalDest, destCapacity, destLength, pErrorCode);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_getNextUChar(UConverter *cnv,
                  ثابت نویسه **source, ثابت نویسه *sourceLimit,
                  UErrorCode *err) {
    UConverterToUnicodeArgs args;
    char16_t buffer[U16_MAX_LENGTH];
    ثابت نویسه *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    اگر(err==nullptr || U_FAILURE(*err)) {
        بازگشت 0xffff;
    }

    اگر(cnv==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0xffff;
    }

    s=*source;
    اگر(sourceLimit<s) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0xffff;
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
    اگر(((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    اگر(cnv->UCharErrorBufferLength>0) {
        char16_t *overflow;

        overflow=cnv->UCharErrorBuffer;
        i=0;
        length=cnv->UCharErrorBufferLength;
        U16_NEXT(overflow, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        اگر((cnv->UCharErrorBufferLength=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UCharErrorBuffer, cnv->UCharErrorBuffer+i,
                         cnv->UCharErrorBufferLength*U_SIZEOF_UCHAR);
        }

        اگر(!U16_IS_LEAD(c) || i<length) {
            بازگشت c;
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
    args.flush=درست;
    args.offsets=nullptr;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=buffer;
    args.targetLimit=buffer+1;
    args.size=اندازه(args);

    اگر(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        اگر(cnv->toULength==0 && cnv->sharedData->impl->getNextUChar!=nullptr) {
            c=cnv->sharedData->impl->getNextUChar(&args, err);
            *source=s=args.source;
            اگر(*err==U_INDEX_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_TO_UNICODE, نادرست);
                بازگشت 0xffff; /* no output */
            } وگرنه اگر(U_SUCCESS(*err) && c>=0) {
                بازگشت c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        _toUnicodeWithCallback(&args, err);

        اگر(*err==U_BUFFER_OVERFLOW_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(args.target-buffer);
    } وگرنه {
        /* write the lead surrogate from the overflow buffer */
        buffer[0]=(char16_t)c;
        args.target=buffer+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    اگر(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } وگرنه اگر(length==0) {
        /* no input or only state changes */
        *err=U_INDEX_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } وگرنه {
        c=buffer[0];
        i=1;
        اگر(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } وگرنه {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            اگر(cnv->UCharErrorBufferLength>0) {
                /* got overflow output from the conversion */
                اگر(U16_IS_TRAIL(c2=cnv->UCharErrorBuffer[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    اگر((--cnv->UCharErrorBufferLength)>0) {
                        uprv_memmove(cnv->UCharErrorBuffer, cnv->UCharErrorBuffer+1,
                                     cnv->UCharErrorBufferLength*U_SIZEOF_UCHAR);
                    }
                } وگرنه {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } وگرنه اگر(args.source<sourceLimit) {
                /* convert once more, to buffer[1] */
                args.targetLimit=buffer+2;
                _toUnicodeWithCallback(&args, err);
                اگر(*err==U_BUFFER_OVERFLOW_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(args.target-buffer);
                اگر(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=buffer[1])) {
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
    اگر(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        اگر((length=cnv->UCharErrorBufferLength)>0) {
            uprv_memmove(cnv->UCharErrorBuffer+delta, cnv->UCharErrorBuffer,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UCharErrorBufferLength=(int8_t)(length+delta);

        cnv->UCharErrorBuffer[0]=buffer[i++];
        اگر(delta>1) {
            cnv->UCharErrorBuffer[1]=buffer[i];
        }
    }

    *source=args.source;
    بازگشت c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI تهی U_EXPORT2
ucnv_convertEx(UConverter *targetCnv, UConverter *sourceCnv,
               نویسه **target, ثابت نویسه *targetLimit,
               ثابت نویسه **source, ثابت نویسه *sourceLimit,
               char16_t *pivotStart, char16_t **pivotSource,
               char16_t **pivotTarget, ثابت char16_t *pivotLimit,
               UBool reset, UBool flush,
               UErrorCode *pErrorCode) {
    char16_t pivotBuffer[CHUNK_SIZE];
    ثابت char16_t *myPivotSource;
    char16_t *myPivotTarget;
    ثابت نویسه *s;
    نویسه *t;

    UConverterToUnicodeArgs toUArgs;
    UConverterFromUnicodeArgs fromUArgs;
    UConverterConvert convert;

    /* error checking */
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت;
    }

    اگر( targetCnv==nullptr || sourceCnv==nullptr ||
        source==nullptr || *source==nullptr ||
        target==nullptr || *target==nullptr || targetLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    s=*source;
    t=*target;
    اگر((sourceLimit!=nullptr && sourceLimit<s) || targetLimit<t) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    اگر(
        (sourceLimit!=nullptr && ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    
    اگر(pivotStart==nullptr) {
        اگر(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }

        /* use the stack pivot buffer */
        myPivotSource=myPivotTarget=pivotStart=pivotBuffer;
        pivotSource=(char16_t **)&myPivotSource;
        pivotTarget=&myPivotTarget;
        pivotLimit=pivotBuffer+CHUNK_SIZE;
    } وگرنه اگر(  pivotStart>=pivotLimit ||
                pivotSource==nullptr || *pivotSource==nullptr ||
                pivotTarget==nullptr || *pivotTarget==nullptr ||
                pivotLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    اگر(sourceLimit==nullptr) {
        /* get limit of single-byte-NUL-terminated source string */
        sourceLimit=uprv_strchr(*source, 0);
    }

    اگر(reset) {
        ucnv_resetToUnicode(sourceCnv);
        ucnv_resetFromUnicode(targetCnv);
        *pivotSource=*pivotTarget=pivotStart;
    } وگرنه اگر(targetCnv->charErrorBufferLength>0) {
        /* output the targetCnv overflow buffer */
        اگر(ucnv_outputOverflowFromUnicode(targetCnv, target, targetLimit, nullptr, pErrorCode)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            بازگشت;
        }
        /* *target has moved, therefore stop using t */

        اگر( !flush &&
            targetCnv->preFromULength>=0 && *pivotSource==*pivotTarget &&
            sourceCnv->UCharErrorBufferLength==0 && sourceCnv->preToULength>=0 && s==sourceLimit
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            بازگشت;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    اگر( sourceCnv->sharedData->staticData->conversionType==UCNV_UTF8 &&
        targetCnv->sharedData->impl->fromUTF8!=nullptr
    ) {
        convert=targetCnv->sharedData->impl->fromUTF8;
    } وگرنه اگر( targetCnv->sharedData->staticData->conversionType==UCNV_UTF8 &&
               sourceCnv->sharedData->impl->toUTF8!=nullptr
    ) {
        convert=sourceCnv->sharedData->impl->toUTF8;
    } وگرنه {
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
    اگر(convert!=nullptr && (pivotLimit-pivotStart)>32) {
        pivotLimit=pivotStart+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=targetCnv;
    fromUArgs.flush=نادرست;
    fromUArgs.offsets=nullptr;
    fromUArgs.target=*target;
    fromUArgs.targetLimit=targetLimit;
    fromUArgs.size=اندازه(fromUArgs);

    toUArgs.converter=sourceCnv;
    toUArgs.flush=flush;
    toUArgs.offsets=nullptr;
    toUArgs.source=s;
    toUArgs.sourceLimit=sourceLimit;
    toUArgs.targetLimit=pivotLimit;
    toUArgs.size=اندازه(toUArgs);

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
    برای(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        اگر( *pivotSource<*pivotTarget ||
            U_FAILURE(*pErrorCode) ||
            targetCnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.source=*pivotSource;
            fromUArgs.sourceLimit=*pivotTarget;
            _fromUnicodeWithCallback(&fromUArgs, pErrorCode);
            اگر(U_FAILURE(*pErrorCode)) {
                /* target overflow, or conversion error */
                *pivotSource=(char16_t *)fromUArgs.source;
                بشکن;
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
        اگر(sourceCnv->UCharErrorBufferLength>0) {
            اگر(ucnv_outputOverflowToUnicode(sourceCnv, pivotTarget, pivotLimit, nullptr, pErrorCode)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *pErrorCode=U_ZERO_ERROR;
            }
            ادامه;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        اگر( toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 && sourceCnv->toULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            بشکن;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        اگر(convert!=nullptr && targetCnv->preFromUFirstCP<0 && sourceCnv->preToULength==0) {
            اگر(*pErrorCode==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *pErrorCode=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &toUArgs, pErrorCode);
            اگر(*pErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                بشکن;
            } وگرنه اگر(U_FAILURE(*pErrorCode)) {
                اگر(sourceCnv->toULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } وگرنه {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *pivotSource=*pivotTarget=pivotStart+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    ادامه;
                }
            } وگرنه اگر(*pErrorCode==U_USING_DEFAULT_WARNING) {
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
            } وگرنه اگر(flush && sourceCnv->toULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *pErrorCode=U_TRUNCATED_CHAR_FOUND;
            } وگرنه {
                /* input consumed */
                اگر(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(sourceCnv, UCNV_RESET_TO_UNICODE, نادرست);
                    _reset(targetCnv, UCNV_RESET_FROM_UNICODE, نادرست);
                }

                /* done successfully */
                بشکن;
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
        اگر(*pErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *pErrorCode=U_ZERO_ERROR;
        } وگرنه اگر(U_FAILURE(*pErrorCode) || (!flush && *pivotTarget==pivotStart)) {
            /* conversion error, or there was nothing left to convert */
            بشکن;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        اگر( flush && toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 &&
            sourceCnv->UCharErrorBufferLength==0
        ) {
            fromUArgs.flush=درست;
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
    اگر(flush && U_SUCCESS(*pErrorCode)) {
        اگر(*target!=targetLimit) {
            **target=0;
            اگر(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {
                *pErrorCode=U_ZERO_ERROR;
            }
        } وگرنه {
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
ایستا int32_t
ucnv_internalConvert(UConverter *outConverter, UConverter *inConverter,
                     نویسه *target, int32_t targetCapacity,
                     ثابت نویسه *source, int32_t sourceLength,
                     UErrorCode *pErrorCode) {
    char16_t pivotBuffer[CHUNK_SIZE];
    char16_t *pivot, *pivot2;

    نویسه *myTarget;
    ثابت نویسه *sourceLimit;
    ثابت نویسه *targetLimit;
    int32_t targetLength=0;

    /* set up */
    اگر(sourceLength<0) {
        sourceLimit=uprv_strchr(source, 0);
    } وگرنه {
        sourceLimit=source+sourceLength;
    }

    /* if there is no input data, we're done */
    اگر(source==sourceLimit) {
        بازگشت u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    pivot=pivot2=pivotBuffer;
    myTarget=target;
    targetLength=0;

    اگر(targetCapacity>0) {
        /* perform real conversion */
        targetLimit=target+targetCapacity;
        ucnv_convertEx(outConverter, inConverter,
                       &myTarget, targetLimit,
                       &source, sourceLimit,
                       pivotBuffer, &pivot, &pivot2, pivotBuffer+CHUNK_SIZE,
                       نادرست,
                       درست,
                       pErrorCode);
        targetLength = static_cast<int32_t>(myTarget - target);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    اگر(*pErrorCode==U_BUFFER_OVERFLOW_ERROR || targetCapacity==0)
    {
        نویسه targetBuffer[CHUNK_SIZE];

        targetLimit=targetBuffer+CHUNK_SIZE;
        انجامبده {
            *pErrorCode=U_ZERO_ERROR;
            myTarget=targetBuffer;
            ucnv_convertEx(outConverter, inConverter,
                           &myTarget, targetLimit,
                           &source, sourceLimit,
                           pivotBuffer, &pivot, &pivot2, pivotBuffer+CHUNK_SIZE,
                           نادرست,
                           درست,
                           pErrorCode);
            targetLength += static_cast<int32_t>(myTarget - targetBuffer);
        } وقتی(*pErrorCode==U_BUFFER_OVERFLOW_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        بازگشت u_terminateChars(target, targetCapacity, targetLength, pErrorCode);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    بازگشت targetLength;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ثابت نویسه *toConverterName, ثابت نویسه *fromConverterName,
             نویسه *target, int32_t targetCapacity,
             ثابت نویسه *source, int32_t sourceLength,
             UErrorCode *pErrorCode) {
    UConverter in, out; /* stack-allocated */
    UConverter *inConverter, *outConverter;
    int32_t targetLength;

    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    اگر( source==nullptr || sourceLength<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    /* if there is no input data, we're done */
    اگر(sourceLength==0 || (sourceLength<0 && *source==0)) {
        بازگشت u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the converters */
    inConverter=ucnv_createConverter(&in, fromConverterName, pErrorCode);
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    outConverter=ucnv_createConverter(&out, toConverterName, pErrorCode);
    اگر(U_FAILURE(*pErrorCode)) {
        ucnv_close(inConverter);
        بازگشت 0;
    }

    targetLength=ucnv_internalConvert(outConverter, inConverter,
                                      target, targetCapacity,
                                      source, sourceLength,
                                      pErrorCode);

    ucnv_close(inConverter);
    ucnv_close(outConverter);

    بازگشت targetLength;
}

/* @internal */
ایستا int32_t
ucnv_convertAlgorithmic(UBool convertToAlgorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        نویسه *target, int32_t targetCapacity,
                        ثابت نویسه *source, int32_t sourceLength,
                        UErrorCode *pErrorCode) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *to, *from;
    int32_t targetLength;

    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    اگر( cnv==nullptr || source==nullptr || sourceLength<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    /* if there is no input data, we're done */
    اگر(sourceLength==0 || (sourceLength<0 && *source==0)) {
        بازگشت u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, pErrorCode);
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    /* reset the other converter */
    اگر(convertToAlgorithmic) {
        /* cnv->Unicode->algo */
        ucnv_resetToUnicode(cnv);
        to=algoConverter;
        from=cnv;
    } وگرنه {
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

    بازگشت targetLength;
}

U_CAPI int32_t U_EXPORT2
ucnv_toAlgorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   نویسه *target, int32_t targetCapacity,
                   ثابت نویسه *source, int32_t sourceLength,
                   UErrorCode *pErrorCode) {
    بازگشت ucnv_convertAlgorithmic(درست, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, sourceLength,
                                   pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     نویسه *target, int32_t targetCapacity,
                     ثابت نویسه *source, int32_t sourceLength,
                     UErrorCode *pErrorCode) UPRV_NO_SANITIZE_UNDEFINED {

    اگر(algorithmicType<0 || UCNV_عدد_OF_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    بازگشت ucnv_convertAlgorithmic(نادرست, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, sourceLength,
                                   pErrorCode);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ثابت UConverter* converter)
{
    int8_t type = converter->sharedData->staticData->conversionType;
#اگر !UCONFIG_NO_LEGACY_CONVERSION
    اگر(type == UCNV_MBCS) {
        بازگشت ucnv_MBCSGetType(converter);
    }
#endif
    بازگشت (UConverterType)type;
}

U_CAPI تهی  U_EXPORT2
ucnv_getStarters(ثابت UConverter* converter, 
                 UBool starters[256],
                 UErrorCode* err)
{
    اگر (err == nullptr || U_FAILURE(*err)) {
        بازگشت;
    }

    اگر(converter->sharedData->impl->getStarters != nullptr) {
        converter->sharedData->impl->getStarters(converter, starters, err);
    } وگرنه {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ایستا ثابت UAmbiguousConverter *ucnv_getAmbiguous(ثابت UConverter *cnv)
{
    UErrorCode errorCode;
    ثابت نویسه *name;
    int32_t i;

    اگر(cnv==nullptr) {
        بازگشت nullptr;
    }

    errorCode=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }

    برای(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        اگر(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            بازگشت ambiguousConverters+i;
        }
    }

    بازگشت nullptr;
}

U_CAPI تهی  U_EXPORT2
ucnv_fixFileSeparator(ثابت UConverter *cnv, 
                      char16_t* source,
                      int32_t sourceLength) {
    ثابت UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    اگر(cnv==nullptr || source==nullptr || sourceLength<=0 || (a=ucnv_getAmbiguous(cnv))==nullptr)
    {
        بازگشت;
    }

    variant5c=a->variant5c;
    برای(i=0; i<sourceLength; ++i) {
        اگر(source[i]==variant5c) {
            source[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ثابت UConverter *cnv) {
    بازگشت ucnv_getAmbiguous(cnv)!=nullptr;
}

U_CAPI تهی  U_EXPORT2
ucnv_setFallback(UConverter *cnv, UBool usesFallback)
{
    cnv->useFallback = usesFallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_usesFallback(ثابت UConverter *cnv)
{
    بازگشت cnv->useFallback;
}

U_CAPI تهی  U_EXPORT2
ucnv_getInvalidChars (ثابت UConverter * converter,
                      نویسه *errBytes,
                      int8_t * len,
                      UErrorCode * err)
{
    اگر (err == nullptr || U_FAILURE(*err))
    {
        بازگشت;
    }
    اگر (len == nullptr || errBytes == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    اگر (*len < converter->invalidCharLength)
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت;
    }
    اگر ((*len = converter->invalidCharLength) > 0)
    {
        uprv_memcpy (errBytes, converter->invalidCharBuffer, *len);
    }
}

U_CAPI تهی  U_EXPORT2
ucnv_getInvalidUChars (ثابت UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UErrorCode * err)
{
    اگر (err == nullptr || U_FAILURE(*err))
    {
        بازگشت;
    }
    اگر (len == nullptr || errChars == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    اگر (*len < converter->invalidUCharLength)
    {
        *err = U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت;
    }
    اگر ((*len = converter->invalidUCharLength) > 0)
    {
        u_memcpy (errChars, converter->invalidUCharBuffer, *len);
    }
}

#define SIG_MAX_LEN 5

U_CAPI ثابت نویسه* U_EXPORT2
ucnv_detectUnicodeSignature( ثابت نویسه* source,
                             int32_t sourceLength,
                             int32_t* signatureLength,
                             UErrorCode* pErrorCode) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    نویسه start[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    صحیح i = 0;

    اگر((pErrorCode==nullptr) || U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }
    
    اگر(source == nullptr || sourceLength < -1){
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    اگر(signatureLength == nullptr) {
        signatureLength = &dummy;
    }

    اگر(sourceLength==-1){
        sourceLength=(int32_t)uprv_strlen(source);
    }

    
    وقتی(i<sourceLength&& i<SIG_MAX_LEN){
        start[i]=source[i];
        i++;
    }

    اگر(start[0] == '\xFE' && start[1] == '\xFF') {
        *signatureLength=2;
        بازگشت  "UTF-16BE";
    } وگرنه اگر(start[0] == '\xFF' && start[1] == '\xFE') {
        اگر(start[2] == '\x00' && start[3] =='\x00') {
            *signatureLength=4;
            بازگشت "UTF-32LE";
        } وگرنه {
            *signatureLength=2;
            بازگشت  "UTF-16LE";
        }
    } وگرنه اگر(start[0] == '\xEF' && start[1] == '\xBB' && start[2] == '\xBF') {
        *signatureLength=3;
        بازگشت  "UTF-8";
    } وگرنه اگر(start[0] == '\x00' && start[1] == '\x00' && 
              start[2] == '\xFE' && start[3]=='\xFF') {
        *signatureLength=4;
        بازگشت  "UTF-32BE";
    } وگرنه اگر(start[0] == '\x0E' && start[1] == '\xFE' && start[2] == '\xFF') {
        *signatureLength=3;
        بازگشت "SCSU";
    } وگرنه اگر(start[0] == '\xFB' && start[1] == '\xEE' && start[2] == '\x28') {
        *signatureLength=3;
        بازگشت "BOCU-1";
    } وگرنه اگر(start[0] == '\x2B' && start[1] == '\x2F' && start[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        اگر(start[3] == '\x38' && start[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signatureLength=5;
            بازگشت "UTF-7";
        } وگرنه اگر(start[3] == '\x38' || start[3] == '\x39' || start[3] == '\x2B' || start[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signatureLength=4;
            بازگشت "UTF-7";
        }
    }وگرنه اگر(start[0]=='\xDD' && start[1]== '\x73'&& start[2]=='\x66' && start[3]=='\x73'){
        *signatureLength=4;
        بازگشت "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signatureLength=0;
    بازگشت nullptr;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ثابت UConverter* cnv, UErrorCode* status)
{
    اگر(status == nullptr || U_FAILURE(*status)){
        بازگشت -1;
    }
    اگر(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت -1;
    }

    اگر(cnv->preFromUFirstCP >= 0){
        بازگشت U16_LENGTH(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }وگرنه اگر(cnv->preFromULength < 0){
        بازگشت -cnv->preFromULength ;
    }وگرنه اگر(cnv->fromUChar32 > 0){
        بازگشت 1;
    }
    بازگشت 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_toUCountPending(ثابت UConverter* cnv, UErrorCode* status){

    اگر(status == nullptr || U_FAILURE(*status)){
        بازگشت -1;
    }
    اگر(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت -1;
    }

    اگر(cnv->preToULength > 0){
        بازگشت cnv->preToULength ;
    }وگرنه اگر(cnv->preToULength < 0){
        بازگشت -cnv->preToULength;
    }وگرنه اگر(cnv->toULength > 0){
        بازگشت cnv->toULength;
    }
    بازگشت 0;
}

U_CAPI UBool U_EXPORT2
ucnv_isFixedWidth(UConverter *cnv, UErrorCode *status){
    اگر (U_FAILURE(*status)) {
        بازگشت نادرست;
    }

    اگر (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت نادرست;
    }

    انتخاب (ucnv_getType(cnv)) {
        حالت UCNV_SBCS:
        حالت UCNV_DBCS:
        حالت UCNV_UTF32_BigEndian:
        حالت UCNV_UTF32_LittleEndian:
        حالت UCNV_UTF32:
        حالت UCNV_US_ASCII:
            بازگشت درست;
        پیشفرض:
            بازگشت نادرست;
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
