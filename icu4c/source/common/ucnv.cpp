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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश <memory>

#समावेश "unicode/ustring.h"
#समावेश "unicode/ucnv.h"
#समावेश "unicode/ucnv_err.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf16.h"
#समावेश "putilimp.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "uassert.h"
#समावेश "utracimp.h"
#समावेश "ustr_imp.h"
#समावेश "ucnv_imp.h"
#समावेश "ucnv_cnv.h"
#समावेश "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#घोषणा CHUNK_आकार 1024

प्रकार काष्ठा UAmbiguousConverter {
    स्थिर अक्षर *name;
    स्थिर char16_t variant5c;
} UAmbiguousConverter;

अटल स्थिर UAmbiguousConverter ambiguousConverters[]={
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
ucnv_open (स्थिर अक्षर *name,
                       UErrorCode * err)
{
    UConverter *r;

    अगर (err == nullptr || U_FAILURE (*err)) {
        वापस nullptr;
    }

    r =  ucnv_createConverter(nullptr, name, err);
    वापस r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (स्थिर अक्षर *packageName, स्थिर अक्षर *converterName, UErrorCode * err)
{
    वापस ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (स्थिर char16_t * name,
                         UErrorCode * err)
{
    अक्षर asciiName[UCNV_MAX_CONVERTER_NAME_लंबाई];

    अगर (err == nullptr || U_FAILURE(*err))
        वापस nullptr;
    अगर (name == nullptr)
        वापस ucnv_open (nullptr, err);
    अगर (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_लंबाई)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    वापस ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
अटल int32_t
ucnv_copyPlatformString(अक्षर *platformString, UConverterPlatform pltfrm)
{
    चयन (pltfrm)
    {
    हाल UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        वापस 4;
    हाल UCNV_UNKNOWN:
        अवरोध;
    }

    /* default to empty string */
    *platformString = 0;
    वापस 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UErrorCode * err)
{
    अक्षर myName[UCNV_MAX_CONVERTER_NAME_लंबाई];
    int32_t myNameLen;

    अगर (err == nullptr || U_FAILURE (*err))
        वापस nullptr;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integerToString(myName + myNameLen, codepage, 10);

    वापस ucnv_createConverter(nullptr, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_safeClone(स्थिर UConverter* cnv,
               व्योम* ढेर_बफर U_LIFETIME_BOUND,
               int32_t* p_बफर_आकार,
               UErrorCode* status)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t ढेर_बफर_आकार;
    int32_t बफर_आकार_Needed;
    UErrorCode cbErr;
    UConverterToUnicodeArgs toUArgs = {
        माप(UConverterToUnicodeArgs),
            true,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };
    UConverterFromUnicodeArgs fromUArgs = {
        माप(UConverterFromUnicodeArgs),
            true,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLONE);

    अगर (status == nullptr || U_FAILURE(*status)){
        UTRACE_EXIT_STATUS(status? *status: U_ILLEGAL_ARGUMENT_ERROR);
        वापस nullptr;
    }

    अगर (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(*status);
        वापस nullptr;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, status), cnv, ढेर_बफर);

    अगर (cnv->shared_तथ्य->impl->safeClone != nullptr) {
        /* call the custom safeClone function for sizing */
        बफर_आकार_Needed = 0;
        cnv->shared_तथ्य->impl->safeClone(cnv, nullptr, &बफर_आकार_Needed, status);
        अगर (U_FAILURE(*status)) {
            UTRACE_EXIT_STATUS(*status);
            वापस nullptr;
        }
    }
    अन्यथा
    {
        /* inherent sizing */
        बफर_आकार_Needed = माप(UConverter);
    }

    अगर (p_बफर_आकार == nullptr) {
        ढेर_बफर_आकार = 1;
        p_बफर_आकार = &ढेर_बफर_आकार;
    } अन्यथा {
        ढेर_बफर_आकार = *p_बफर_आकार;
        अगर (ढेर_बफर_आकार <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_बफर_आकार = बफर_आकार_Needed;
            UTRACE_EXIT_मूल्य(बफर_आकार_Needed);
            वापस nullptr;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    अगर (ढेर_बफर) {
        uintptr_t p = reinterpret_cast<uintptr_t>(ढेर_बफर);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        सूचक_भेद_प्रकार pointerAdjustment = aligned_p - p;
        अगर (बफर_आकार_Needed + pointerAdjustment <= ढेर_बफर_आकार) {
            ढेर_बफर = reinterpret_cast<व्योम *>(aligned_p);
            ढेर_बफर_आकार -= static_cast<int32_t>(pointerAdjustment);
        } अन्यथा {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            ढेर_बफर_आकार = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    अगर (ढेर_बफर_आकार < बफर_आकार_Needed || ढेर_बफर == nullptr)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (बफर_आकार_Needed);

        अगर(localConverter == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            UTRACE_EXIT_STATUS(*status);
            वापस nullptr;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        अगर (p_बफर_आकार != &ढेर_बफर_आकार) {
            *status = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_बफर_आकार = बफर_आकार_Needed;
    } अन्यथा {
        /* just use the stack buffer */
        localConverter = (UConverter*) ढेर_बफर;
        allocatedConverter = nullptr;
    }

    uprv_memset(localConverter, 0, बफर_आकार_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, माप(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = false;

    /* copy the substitution string */
    अगर (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } अन्यथा {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_बफर_लंबाई * U_SIZEOF_UCHAR);
        अगर (localConverter->subChars == nullptr) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_STATUS(*status);
            वापस nullptr;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_बफर_लंबाई * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    अगर (cnv->shared_तथ्य->impl->safeClone != nullptr) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_तथ्य->impl->safeClone(cnv, localConverter, p_बफर_आकार, status);
    }

    अगर(localConverter==nullptr || U_FAILURE(*status)) {
        अगर (allocatedConverter != nullptr && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_STATUS(*status);
        वापस nullptr;
    }

    /* increment refcount of shared data if needed */
    अगर (cnv->shared_तथ्य->isReferenceCounted) {
        ucnv_increment_Ref_गिनती(cnv->shared_तथ्य);
    }

    अगर(localConverter == (UConverter*)ढेर_बफर) {
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
    वापस localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(स्थिर UConverter* cnv, UErrorCode *status)
{
    वापस ucnv_safeClone(cnv, nullptr, nullptr, status);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI व्योम  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLOSE);

    अगर (converter == nullptr)
    {
        UTRACE_EXIT();
        वापस;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &errorCode), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    अगर (converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
        UConverterToUnicodeArgs toUArgs = {
            माप(UConverterToUnicodeArgs),
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
    अगर (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverterFromUnicodeArgs fromUArgs = {
            माप(UConverterFromUnicodeArgs),
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

    अगर (converter->shared_तथ्य->impl->बंद != nullptr) {
        converter->shared_तथ्य->impl->बंद(converter);
    }

    अगर (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    अगर (converter->shared_तथ्य->isReferenceCounted) {
        ucnv_unload_Shared_तथ्य_If_Ready(converter->shared_तथ्य);
    }

    अगर(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI स्थिर अक्षर*   U_EXPORT2
ucnv_getAvailableName (int32_t n)
{
    अगर (0 <= n && n <= 0xffff) {
        UErrorCode err = U_ZERO_ERROR;
        स्थिर अक्षर *name = ucnv_bld_getAvailableConverter((uint16_t)n, &err);
        अगर (U_SUCCESS(err)) {
            वापस name;
        }
    }
    वापस nullptr;
}

U_CAPI int32_t   U_EXPORT2
ucnv_गिनती_Available ()
{
    UErrorCode err = U_ZERO_ERROR;
    वापस ucnv_bld_गिनती_Available_Converters(&err);
}

U_CAPI व्योम    U_EXPORT2
ucnv_getSubstChars (स्थिर UConverter * converter,
                    अक्षर *mySubChar,
                    int8_t * len,
                    UErrorCode * err)
{
    अगर (U_FAILURE (*err))
        वापस;

    अगर (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        वापस;
    }

    अगर (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI व्योम    U_EXPORT2
ucnv_setSubstChars (UConverter * converter,
                    स्थिर अक्षर *mySubChar,
                    int8_t len,
                    UErrorCode * err)
{
    अगर (U_FAILURE (*err))
        वापस;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    अगर ((len > converter->shared_तथ्य->static_तथ्य->maxBytesPerChar)
     || (len < converter->shared_तथ्य->static_तथ्य->minBytesPerChar))
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
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

U_CAPI व्योम U_EXPORT2
ucnv_setSubstString(UConverter *cnv,
                    स्थिर char16_t *s,
                    int32_t length,
                    UErrorCode *err) {
    alignas(UConverter) अक्षर clone_बफर[U_CNV_SAFECLONE_BUFFERSIZE];
    अक्षर chars[UCNV_ERROR_बफर_लंबाई];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_आकार, length8;

    /* Let the following functions check all arguments. */
    clone_आकार = माप(clone_बफर);
    clone = ucnv_safeClone(cnv, clone_बफर, &clone_आकार, err);
    ucnv_setFromUCallBack(clone, UCNV_FROM_U_CALLBACK_STOP, nullptr, nullptr, nullptr, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)माप(chars), s, length, err);
    ucnv_close(clone);
    अगर (U_FAILURE(*err)) {
        वापस;
    }

    अगर (cnv->shared_तथ्य->impl->writeSub == nullptr
#अगर !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_तथ्य->static_तथ्य->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#पूर्ण_अगर
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } अन्यथा {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        अगर (length > UCNV_ERROR_बफर_लंबाई) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_बफर_OVERFLOW_ERROR;
            वापस;
        }
        subChars = (uint8_t *)s;
        अगर (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    अगर (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        अगर (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_बफर_लंबाई * U_SIZEOF_UCHAR);
            अगर (cnv->subChars == nullptr) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_बफर_लंबाई * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    अगर (length8 == 0) {
        cnv->subCharLen = 0;
    } अन्यथा {
        uprv_memcpy(cnv->subChars, subChars, length8);
        अगर (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } अन्यथा /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
अटल व्योम _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    अगर(converter == nullptr) {
        वापस;
    }

    अगर(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UErrorCode errorCode;

        अगर(choice<=UCNV_RESET_TO_UNICODE && converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
            UConverterToUnicodeArgs toUArgs = {
                माप(UConverterToUnicodeArgs),
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
        अगर(choice!=UCNV_RESET_TO_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverterFromUnicodeArgs fromUArgs = {
                माप(UConverterFromUnicodeArgs),
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
    अगर(choice<=UCNV_RESET_TO_UNICODE) {
        converter->toUnicodeStatus = converter->shared_तथ्य->toUnicodeStatus;
        converter->mode = 0;
        converter->toULength = 0;
        converter->invalid_Char_लंबाई = converter->UChar_Error_बफर_लंबाई = 0;
        converter->preToULength = 0;
    }
    अगर(choice!=UCNV_RESET_TO_UNICODE) {
        converter->fromUnicodeStatus = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_लंबाई = converter->char_Error_बफर_लंबाई = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    अगर (converter->shared_तथ्य->impl->reset != nullptr) {
        /* call the custom reset function */
        converter->shared_तथ्य->impl->reset(converter, choice);
    }
}

U_CAPI व्योम  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, true);
}

U_CAPI व्योम  U_EXPORT2
ucnv_resetToUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_TO_UNICODE, true);
}

U_CAPI व्योम  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, true);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_आकार (स्थिर UConverter * converter)
{
    वापस converter->maxBytesPerUChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_आकार (स्थिर UConverter * converter)
{
    वापस converter->shared_तथ्य->static_तथ्य->minBytesPerChar;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ucnv_getName(स्थिर UConverter* converter U_LIFETIME_BOUND, UErrorCode* err)
{
    अगर (U_FAILURE (*err))
        वापस nullptr;
    अगर(converter->shared_तथ्य->impl->getName){
        स्थिर अक्षर* अस्थायी= converter->shared_तथ्य->impl->getName(converter);
        अगर(अस्थायी)
            वापस अस्थायी;
    }
    वापस converter->shared_तथ्य->static_तथ्य->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(स्थिर UConverter * converter,
              UErrorCode * err)
{
    int32_t ccsid;
    अगर (U_FAILURE (*err))
        वापस -1;

    ccsid = converter->shared_तथ्य->static_तथ्य->codepage;
    अगर (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        स्थिर अक्षर *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        अगर (U_SUCCESS(*err) && standardName) {
            स्थिर अक्षर *ccsidStr = uprv_strchr(standardName, '-');
            अगर (ccsidStr) {
                ccsid = (int32_t)म_से_द(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    वापस ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (स्थिर UConverter * converter,
                                      UErrorCode * err)
{
    अगर (U_FAILURE (*err))
        वापस UCNV_UNKNOWN;

    वापस (UConverterPlatform)converter->shared_तथ्य->static_तथ्य->platform;
}

U_CAPI व्योम U_EXPORT2
    ucnv_getToUCallBack (स्थिर UConverter * converter,
                         UConverterToUCallback *action,
                         स्थिर व्योम **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->toUContext;
}

U_CAPI व्योम U_EXPORT2
    ucnv_getFromUCallBack (स्थिर UConverter * converter,
                           UConverterFromUCallback *action,
                           स्थिर व्योम **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI व्योम    U_EXPORT2
ucnv_setToUCallBack (UConverter * converter,
                            UConverterToUCallback newAction,
                            स्थिर व्योम* newContext,
                            UConverterToUCallback *oldAction,
                            स्थिर व्योम** oldContext,
                            UErrorCode * err)
{
    अगर (U_FAILURE (*err))
        वापस;
    अगर (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    अगर (oldContext) *oldContext = converter->toUContext;
    converter->toUContext = newContext;
}

U_CAPI व्योम  U_EXPORT2
ucnv_setFromUCallBack (UConverter * converter,
                            UConverterFromUCallback newAction,
                            स्थिर व्योम* newContext,
                            UConverterFromUCallback *oldAction,
                            स्थिर व्योम** oldContext,
                            UErrorCode * err)
{
    अगर (U_FAILURE (*err))
        वापस;
    अगर (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    अगर (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

अटल व्योम
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t source_सूचकांक, int32_t error_निवेश_लंबाई) {
    int32_t *limit;
    int32_t delta, offset;

    अगर(source_सूचकांक>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=source_सूचकांक-error_निवेश_लंबाई;
    } अन्यथा {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    limit=offsets+length;
    अगर(delta==0) {
        /* most common case, nothing to do */
    } अन्यथा अगर(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        जबतक(offsets<limit) {
            offset=*offsets;
            अगर(offset>=0) {
                *offsets=offset+delta;
            }
            ++offsets;
        }
    } अन्यथा /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        जबतक(offsets<limit) {
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

अटल व्योम
_fromUnicodeWithCallback(UConverterFromUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    स्थिर char16_t *s;
    अक्षर *t;
    int32_t *offsets;
    int32_t source_सूचकांक;
    int32_t error_निवेश_लंबाई;
    UBool converter_Saw_End_Of_निवेश, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    स्थिर char16_t *realSource, *realSourceLimit;
    int32_t real_Source_सूचकांक;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    source_सूचकांक=0;
    अगर(offsets==nullptr) {
        fromUnicode=cnv->shared_तथ्य->impl->fromUnicode;
    } अन्यथा {
        fromUnicode=cnv->shared_तथ्य->impl->fromUnicodeWithOffsets;
        अगर(fromUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_तथ्य->impl->fromUnicode;
            /* we will write -1 for each offset */
            source_सूचकांक=-1;
        }
    }

    अगर(cnv->preFromULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=false;
        real_Source_सूचकांक=0;
    } अन्यथा {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        अगर (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->सफाई;
        real_Source_सूचकांक=source_सूचकांक;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preFromULength;
        pArgs->सफाई=false;
        source_सूचकांक=-1;

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
    क्रम(;;) {
        अगर(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_End_Of_निवेश=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->सफाई && pArgs->source==pArgs->sourceLimit &&
                        cnv->fromUChar32==0);
        } अन्यथा {
            /* handle error from ucnv_convertEx() */
            converter_Saw_End_Of_निवेश=false;
        }

        /* no callback called yet for this iteration */
        calledCallback=false;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_निवेश_लंबाई=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        क्रम(;;) {
            /* update offsets if we write any */
            अगर(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                अगर(length>0) {
                    _updateOffsets(offsets, length, source_सूचकांक, error_निवेश_लंबाई);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                अगर(source_सूचकांक>=0) {
                    source_सूचकांक += static_cast<int32_t>(pArgs->source - s);
                }
            }

            अगर(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                अगर(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->सफाई;
                    real_Source_सूचकांक=source_सूचकांक;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preFromULength;
                    pArgs->सफाई=false;
                    अगर((source_सूचकांक+=cnv->preFromULength)<0) {
                        source_सूचकांक=-1;
                    }

                    cnv->preFromULength=0;
                } अन्यथा {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            अगर(U_SUCCESS(*err)) {
                अगर(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    अवरोध;
                } अन्यथा अगर(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->सफाई=realFlush;
                    source_सूचकांक=real_Source_सूचकांक;

                    realSource=nullptr;
                    अवरोध;
                } अन्यथा अगर(pArgs->सफाई && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=false; /* new error condition */
                } अन्यथा {
                    /* input consumed */
                    अगर(pArgs->सफाई) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        अगर(!converter_Saw_End_Of_निवेश) {
                            अवरोध;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, false);
                    }

                    /* done successfully */
                    वापस;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                अगर( calledCallback ||
                    (e=*err)==U_बफर_OVERFLOW_ERROR ||
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
                    अगर(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        अगर(length>0) {
                            u_memcpy(cnv->preFromU, pArgs->source, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->सफाई=realFlush;
                    }

                    वापस;
                }
            }

            /* callback handling */
            {
                UChar32 codePoint;

                /* get and write the code point */
                codePoint=cnv->fromUChar32;
                error_निवेश_लंबाई=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_बफर, error_निवेश_लंबाई, codePoint);
                cnv->invalid_UChar_लंबाई = static_cast<int8_t>(error_निवेश_लंबाई);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, pArgs,
                    cnv->invalid_UChar_बफर, error_निवेश_लंबाई, codePoint,
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
अटल UBool
ucnv_outputOverflowFromUnicode(UConverter *cnv,
                               अक्षर **target, स्थिर अक्षर *targetLimit,
                               int32_t **pOffsets,
                               UErrorCode *err) {
    int32_t *offsets;
    अक्षर *overflow, *t;
    int32_t i, length;

    t=*target;
    अगर(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } अन्यथा {
        offsets=nullptr;
    }

    overflow = reinterpret_cast<अक्षर*>(cnv->char_Error_बफर);
    length=cnv->char_Error_बफर_लंबाई;
    i=0;
    जबतक(i<length) {
        अगर(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            करो {
                overflow[j++]=overflow[i++];
            } जबतक(i<length);

            cnv->char_Error_बफर_लंबाई = static_cast<int8_t>(j);
            *target=t;
            अगर(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_बफर_OVERFLOW_ERROR;
            वापस true;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        अगर(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_बफर_लंबाई=0;
    *target=t;
    अगर(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    वापस false;
}

U_CAPI व्योम U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 अक्षर **target, स्थिर अक्षर *targetLimit,
                 स्थिर char16_t **source, स्थिर char16_t *sourceLimit,
                 int32_t *offsets,
                 UBool सफाई,
                 UErrorCode *err) {
    UConverterFromUnicodeArgs args;
    स्थिर char16_t *s;
    अक्षर *t;

    /* check parameters */
    अगर(err==nullptr || U_FAILURE(*err)) {
        वापस;
    }

    अगर(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    s=*source;
    t=*target;

    अगर ((स्थिर व्योम *)U_MAX_PTR(sourceLimit) == (स्थिर व्योम *)sourceLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        sourceLimit = (स्थिर char16_t *)(((स्थिर अक्षर *)sourceLimit) - 1);
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
    अगर (sourceLimit<s || targetLimit<t ||
        ((माप_प्रकार)(sourceLimit-s)>(माप_प्रकार)0x3fffffff && sourceLimit>s) ||
        ((माप_प्रकार)(targetLimit-t)>(माप_प्रकार)0x7fffffff && targetLimit>t) ||
        (((स्थिर अक्षर *)sourceLimit-(स्थिर अक्षर *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    
    /* output the target overflow buffer */
    अगर( cnv->char_Error_बफर_लंबाई>0 &&
        ucnv_outputOverflowFromUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        वापस;
    }
    /* *target may have moved, therefore stop using t */

    अगर(!सफाई && s==sourceLimit && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        वापस;
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
    args.सफाई=सफाई;
    args.offsets=offsets;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=*target;
    args.targetLimit=targetLimit;
    args.size=माप(args);

    _fromUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

अटल व्योम
_toUnicodeWithCallback(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterToUnicode toUnicode;
    UConverter *cnv;
    स्थिर अक्षर *s;
    char16_t *t;
    int32_t *offsets;
    int32_t source_सूचकांक;
    int32_t error_निवेश_लंबाई;
    UBool converter_Saw_End_Of_निवेश, calledCallback;

    /* variables for m:n conversion */
    अक्षर replay[UCNV_EXT_MAX_BYTES];
    स्थिर अक्षर *realSource, *realSourceLimit;
    int32_t real_Source_सूचकांक;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    source_सूचकांक=0;
    अगर(offsets==nullptr) {
        toUnicode=cnv->shared_तथ्य->impl->toUnicode;
    } अन्यथा {
        toUnicode=cnv->shared_तथ्य->impl->toUnicodeWithOffsets;
        अगर(toUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            toUnicode=cnv->shared_तथ्य->impl->toUnicode;
            /* we will write -1 for each offset */
            source_सूचकांक=-1;
        }
    }

    अगर(cnv->preToULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=false;
        real_Source_सूचकांक=0;
    } अन्यथा {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        अगर (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->सफाई;
        real_Source_सूचकांक=source_सूचकांक;

        uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preToULength;
        pArgs->सफाई=false;
        source_सूचकांक=-1;

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
    क्रम(;;) {
        अगर(U_SUCCESS(*err)) {
            /* convert */
            toUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_End_Of_निवेश=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->सफाई && pArgs->source==pArgs->sourceLimit &&
                        cnv->toULength==0);
        } अन्यथा {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_End_Of_निवेश=false;
        }

        /* no callback called yet for this iteration */
        calledCallback=false;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_निवेश_लंबाई=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        क्रम(;;) {
            /* update offsets if we write any */
            अगर(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                अगर(length>0) {
                    _updateOffsets(offsets, length, source_सूचकांक, error_निवेश_लंबाई);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                अगर(source_सूचकांक>=0) {
                    source_सूचकांक += static_cast<int32_t>(pArgs->source - s);
                }
            }

            अगर(cnv->preToULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                अगर(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->सफाई;
                    real_Source_सूचकांक=source_सूचकांक;

                    uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preToULength;
                    pArgs->सफाई=false;
                    अगर((source_सूचकांक+=cnv->preToULength)<0) {
                        source_सूचकांक=-1;
                    }

                    cnv->preToULength=0;
                } अन्यथा {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            अगर(U_SUCCESS(*err)) {
                अगर(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    अवरोध;
                } अन्यथा अगर(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->सफाई=realFlush;
                    source_सूचकांक=real_Source_सूचकांक;

                    realSource=nullptr;
                    अवरोध;
                } अन्यथा अगर(pArgs->सफाई && cnv->toULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=false; /* new error condition */
                } अन्यथा {
                    /* input consumed */
                    अगर(pArgs->सफाई) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        अगर(!converter_Saw_End_Of_निवेश) {
                            अवरोध;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_TO_UNICODE, false);
                    }

                    /* done successfully */
                    वापस;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                अगर( calledCallback ||
                    (e=*err)==U_बफर_OVERFLOW_ERROR ||
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
                    अगर(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preToULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        अगर(length>0) {
                            uprv_memcpy(cnv->preToU, pArgs->source, length);
                            cnv->preToULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->सफाई=realFlush;
                    }

                    वापस;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_निवेश_लंबाई=cnv->invalid_Char_लंबाई=cnv->toULength;
            अगर(error_निवेश_लंबाई>0) {
                uprv_memcpy(cnv->invalid_Char_बफर, cnv->toUBytes, error_निवेश_लंबाई);
            }

            /* set the converter state to deal with the next character */
            cnv->toULength=0;

            /* call the callback function */
            अगर(cnv->toUCallbackReason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_FOUND) {
                cnv->toUCallbackReason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->toUContext, pArgs,
                cnv->invalid_Char_बफर, error_निवेश_लंबाई,
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
अटल UBool
ucnv_outputOverflowToUnicode(UConverter *cnv,
                             char16_t **target, स्थिर char16_t *targetLimit,
                             int32_t **pOffsets,
                             UErrorCode *err) {
    int32_t *offsets;
    char16_t *overflow, *t;
    int32_t i, length;

    t=*target;
    अगर(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } अन्यथा {
        offsets=nullptr;
    }

    overflow=cnv->UChar_Error_बफर;
    length=cnv->UChar_Error_बफर_लंबाई;
    i=0;
    जबतक(i<length) {
        अगर(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            करो {
                overflow[j++]=overflow[i++];
            } जबतक(i<length);

            cnv->UChar_Error_बफर_लंबाई = static_cast<int8_t>(j);
            *target=t;
            अगर(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_बफर_OVERFLOW_ERROR;
            वापस true;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        अगर(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_बफर_लंबाई=0;
    *target=t;
    अगर(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    वापस false;
}

U_CAPI व्योम U_EXPORT2
ucnv_toUnicode(UConverter *cnv,
               char16_t **target, स्थिर char16_t *targetLimit,
               स्थिर अक्षर **source, स्थिर अक्षर *sourceLimit,
               int32_t *offsets,
               UBool सफाई,
               UErrorCode *err) {
    UConverterToUnicodeArgs args;
    स्थिर अक्षर *s;
    char16_t *t;

    /* check parameters */
    अगर(err==nullptr || U_FAILURE(*err)) {
        वापस;
    }

    अगर(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    s=*source;
    t=*target;

    अगर ((स्थिर व्योम *)U_MAX_PTR(targetLimit) == (स्थिर व्योम *)targetLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        targetLimit = (स्थिर char16_t *)(((स्थिर अक्षर *)targetLimit) - 1);
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
    अगर (sourceLimit<s || targetLimit<t ||
        ((माप_प्रकार)(sourceLimit-s)>(माप_प्रकार)0x7fffffff && sourceLimit>s) ||
        ((माप_प्रकार)(targetLimit-t)>(माप_प्रकार)0x3fffffff && targetLimit>t) ||
        (((स्थिर अक्षर *)targetLimit-(स्थिर अक्षर *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    
    /* output the target overflow buffer */
    अगर( cnv->UChar_Error_बफर_लंबाई>0 &&
        ucnv_outputOverflowToUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        वापस;
    }
    /* *target may have moved, therefore stop using t */

    अगर(!सफाई && s==sourceLimit && cnv->preToULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        वापस;
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
    args.सफाई=सफाई;
    args.offsets=offsets;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=*target;
    args.targetLimit=targetLimit;
    args.size=माप(args);

    _toUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                अक्षर *dest, int32_t destCapacity,
                स्थिर char16_t *src, int32_t src_लंबाई,
                UErrorCode *pErrorCode) {
    स्थिर char16_t *srcLimit;
    अक्षर *originalDest, *destLimit;
    int32_t dest_लंबाई;

    /* check arguments */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        src_लंबाई<-1 || (src_लंबाई!=0 && src==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    अगर(src_लंबाई==-1) {
        src_लंबाई=u_strlen(src);
    }
    अगर(src_लंबाई>0) {
        srcLimit=src+src_लंबाई;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode बफर_Status = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &बफर_Status);
        dest_लंबाई=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        अगर(बफर_Status==U_बफर_OVERFLOW_ERROR) {
            अक्षर बफर[1024];

            destLimit=बफर+माप(बफर);
            करो {
                dest=बफर;
                बफर_Status=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &बफर_Status);
                dest_लंबाई+=(int32_t)(dest-बफर);
            } जबतक(बफर_Status==U_बफर_OVERFLOW_ERROR);
        }
        अगर (U_FAILURE(बफर_Status)) {
            *pErrorCode = बफर_Status;
        }
    } अन्यथा {
        dest_लंबाई=0;
    }

    वापस u_terminateChars(originalDest, destCapacity, dest_लंबाई, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_toUChars(UConverter *cnv,
              char16_t *dest, int32_t destCapacity,
              स्थिर अक्षर *src, int32_t src_लंबाई,
              UErrorCode *pErrorCode) {
    स्थिर अक्षर *srcLimit;
    char16_t *originalDest, *destLimit;
    int32_t dest_लंबाई;

    /* check arguments */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        src_लंबाई<-1 || (src_लंबाई!=0 && src==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* initialize */
    ucnv_resetToUnicode(cnv);
    originalDest=dest;
    अगर(src_लंबाई==-1) {
        src_लंबाई=(int32_t)uprv_strlen(src);
    }
    अगर(src_लंबाई>0) {
        srcLimit=src+src_लंबाई;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode बफर_Status = U_ZERO_ERROR;
        ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &बफर_Status);
        dest_लंबाई=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        अगर(बफर_Status==U_बफर_OVERFLOW_ERROR)
        {
            char16_t बफर[1024];

            destLimit=बफर+UPRV_LENGTHOF(बफर);
            करो {
                dest=बफर;
                बफर_Status=U_ZERO_ERROR;
                ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &बफर_Status);
                dest_लंबाई+=(int32_t)(dest-बफर);
            }
            जबतक(बफर_Status==U_बफर_OVERFLOW_ERROR);
        }
        अगर (U_FAILURE(बफर_Status)) {
            *pErrorCode = बफर_Status;
        }
    } अन्यथा {
        dest_लंबाई=0;
    }

    वापस u_terminateUChars(originalDest, destCapacity, dest_लंबाई, pErrorCode);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_अग्र_UChar(UConverter *cnv,
                  स्थिर अक्षर **source, स्थिर अक्षर *sourceLimit,
                  UErrorCode *err) {
    UConverterToUnicodeArgs args;
    char16_t बफर[U16_MAX_लंबाई];
    स्थिर अक्षर *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    अगर(err==nullptr || U_FAILURE(*err)) {
        वापस 0xffff;
    }

    अगर(cnv==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0xffff;
    }

    s=*source;
    अगर(sourceLimit<s) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0xffff;
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
    अगर(((माप_प्रकार)(sourceLimit-s)>(माप_प्रकार)0x7fffffff && sourceLimit>s)) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    अगर(cnv->UChar_Error_बफर_लंबाई>0) {
        char16_t *overflow;

        overflow=cnv->UChar_Error_बफर;
        i=0;
        length=cnv->UChar_Error_बफर_लंबाई;
        U16_अग्र(overflow, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        अगर((cnv->UChar_Error_बफर_लंबाई=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_बफर, cnv->UChar_Error_बफर+i,
                         cnv->UChar_Error_बफर_लंबाई*U_SIZEOF_UCHAR);
        }

        अगर(!U16_IS_LEAD(c) || i<length) {
            वापस c;
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
    args.सफाई=true;
    args.offsets=nullptr;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=बफर;
    args.targetLimit=बफर+1;
    args.size=माप(args);

    अगर(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        अगर(cnv->toULength==0 && cnv->shared_तथ्य->impl->get_अग्र_UChar!=nullptr) {
            c=cnv->shared_तथ्य->impl->get_अग्र_UChar(&args, err);
            *source=s=args.source;
            अगर(*err==U_सूचकांक_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_TO_UNICODE, false);
                वापस 0xffff; /* no output */
            } अन्यथा अगर(U_SUCCESS(*err) && c>=0) {
                वापस c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        _toUnicodeWithCallback(&args, err);

        अगर(*err==U_बफर_OVERFLOW_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(args.target-बफर);
    } अन्यथा {
        /* write the lead surrogate from the overflow buffer */
        बफर[0]=(char16_t)c;
        args.target=बफर+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    अगर(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } अन्यथा अगर(length==0) {
        /* no input or only state changes */
        *err=U_सूचकांक_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } अन्यथा {
        c=बफर[0];
        i=1;
        अगर(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } अन्यथा {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            अगर(cnv->UChar_Error_बफर_लंबाई>0) {
                /* got overflow output from the conversion */
                अगर(U16_IS_TRAIL(c2=cnv->UChar_Error_बफर[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    अगर((--cnv->UChar_Error_बफर_लंबाई)>0) {
                        uprv_memmove(cnv->UChar_Error_बफर, cnv->UChar_Error_बफर+1,
                                     cnv->UChar_Error_बफर_लंबाई*U_SIZEOF_UCHAR);
                    }
                } अन्यथा {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } अन्यथा अगर(args.source<sourceLimit) {
                /* convert once more, to buffer[1] */
                args.targetLimit=बफर+2;
                _toUnicodeWithCallback(&args, err);
                अगर(*err==U_बफर_OVERFLOW_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(args.target-बफर);
                अगर(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=बफर[1])) {
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
    अगर(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        अगर((length=cnv->UChar_Error_बफर_लंबाई)>0) {
            uprv_memmove(cnv->UChar_Error_बफर+delta, cnv->UChar_Error_बफर,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_बफर_लंबाई=(int8_t)(length+delta);

        cnv->UChar_Error_बफर[0]=बफर[i++];
        अगर(delta>1) {
            cnv->UChar_Error_बफर[1]=बफर[i];
        }
    }

    *source=args.source;
    वापस c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI व्योम U_EXPORT2
ucnv_convertEx(UConverter *targetCnv, UConverter *sourceCnv,
               अक्षर **target, स्थिर अक्षर *targetLimit,
               स्थिर अक्षर **source, स्थिर अक्षर *sourceLimit,
               char16_t *pivotStart, char16_t **pivotSource,
               char16_t **pivotTarget, स्थिर char16_t *pivotLimit,
               UBool reset, UBool सफाई,
               UErrorCode *pErrorCode) {
    char16_t pivot_बफर[CHUNK_आकार];
    स्थिर char16_t *myPivotSource;
    char16_t *myPivotTarget;
    स्थिर अक्षर *s;
    अक्षर *t;

    UConverterToUnicodeArgs toUArgs;
    UConverterFromUnicodeArgs fromUArgs;
    UConverterConvert convert;

    /* error checking */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस;
    }

    अगर( targetCnv==nullptr || sourceCnv==nullptr ||
        source==nullptr || *source==nullptr ||
        target==nullptr || *target==nullptr || targetLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    s=*source;
    t=*target;
    अगर((sourceLimit!=nullptr && sourceLimit<s) || targetLimit<t) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    अगर(
        (sourceLimit!=nullptr && ((माप_प्रकार)(sourceLimit-s)>(माप_प्रकार)0x7fffffff && sourceLimit>s)) ||
        ((माप_प्रकार)(targetLimit-t)>(माप_प्रकार)0x7fffffff && targetLimit>t)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    
    अगर(pivotStart==nullptr) {
        अगर(!सफाई) {
            /* streaming conversion requires an explicit pivot buffer */
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }

        /* use the stack pivot buffer */
        myPivotSource=myPivotTarget=pivotStart=pivot_बफर;
        pivotSource=(char16_t **)&myPivotSource;
        pivotTarget=&myPivotTarget;
        pivotLimit=pivot_बफर+CHUNK_आकार;
    } अन्यथा अगर(  pivotStart>=pivotLimit ||
                pivotSource==nullptr || *pivotSource==nullptr ||
                pivotTarget==nullptr || *pivotTarget==nullptr ||
                pivotLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    अगर(sourceLimit==nullptr) {
        /* get limit of single-byte-NUL-terminated source string */
        sourceLimit=uprv_strchr(*source, 0);
    }

    अगर(reset) {
        ucnv_resetToUnicode(sourceCnv);
        ucnv_resetFromUnicode(targetCnv);
        *pivotSource=*pivotTarget=pivotStart;
    } अन्यथा अगर(targetCnv->char_Error_बफर_लंबाई>0) {
        /* output the targetCnv overflow buffer */
        अगर(ucnv_outputOverflowFromUnicode(targetCnv, target, targetLimit, nullptr, pErrorCode)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            वापस;
        }
        /* *target has moved, therefore stop using t */

        अगर( !सफाई &&
            targetCnv->preFromULength>=0 && *pivotSource==*pivotTarget &&
            sourceCnv->UChar_Error_बफर_लंबाई==0 && sourceCnv->preToULength>=0 && s==sourceLimit
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            वापस;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    अगर( sourceCnv->shared_तथ्य->static_तथ्य->conversionType==UCNV_UTF8 &&
        targetCnv->shared_तथ्य->impl->fromUTF8!=nullptr
    ) {
        convert=targetCnv->shared_तथ्य->impl->fromUTF8;
    } अन्यथा अगर( targetCnv->shared_तथ्य->static_तथ्य->conversionType==UCNV_UTF8 &&
               sourceCnv->shared_तथ्य->impl->toUTF8!=nullptr
    ) {
        convert=sourceCnv->shared_तथ्य->impl->toUTF8;
    } अन्यथा {
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
    अगर(convert!=nullptr && (pivotLimit-pivotStart)>32) {
        pivotLimit=pivotStart+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=targetCnv;
    fromUArgs.सफाई=false;
    fromUArgs.offsets=nullptr;
    fromUArgs.target=*target;
    fromUArgs.targetLimit=targetLimit;
    fromUArgs.size=माप(fromUArgs);

    toUArgs.converter=sourceCnv;
    toUArgs.सफाई=सफाई;
    toUArgs.offsets=nullptr;
    toUArgs.source=s;
    toUArgs.sourceLimit=sourceLimit;
    toUArgs.targetLimit=pivotLimit;
    toUArgs.size=माप(toUArgs);

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
    क्रम(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        अगर( *pivotSource<*pivotTarget ||
            U_FAILURE(*pErrorCode) ||
            targetCnv->preFromULength<0 ||
            fromUArgs.सफाई
        ) {
            fromUArgs.source=*pivotSource;
            fromUArgs.sourceLimit=*pivotTarget;
            _fromUnicodeWithCallback(&fromUArgs, pErrorCode);
            अगर(U_FAILURE(*pErrorCode)) {
                /* target overflow, or conversion error */
                *pivotSource=(char16_t *)fromUArgs.source;
                अवरोध;
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
        अगर(sourceCnv->UChar_Error_बफर_लंबाई>0) {
            अगर(ucnv_outputOverflowToUnicode(sourceCnv, pivotTarget, pivotLimit, nullptr, pErrorCode)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *pErrorCode=U_ZERO_ERROR;
            }
            जारी;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        अगर( toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 && sourceCnv->toULength==0 &&
            (!सफाई || fromUArgs.सफाई)
        ) {
            /* done successfully */
            अवरोध;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        अगर(convert!=nullptr && targetCnv->preFromUFirstCP<0 && sourceCnv->preToULength==0) {
            अगर(*pErrorCode==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *pErrorCode=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &toUArgs, pErrorCode);
            अगर(*pErrorCode==U_बफर_OVERFLOW_ERROR) {
                अवरोध;
            } अन्यथा अगर(U_FAILURE(*pErrorCode)) {
                अगर(sourceCnv->toULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } अन्यथा {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *pivotSource=*pivotTarget=pivotStart+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    जारी;
                }
            } अन्यथा अगर(*pErrorCode==U_USING_DEFAULT_WARNING) {
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
            } अन्यथा अगर(सफाई && sourceCnv->toULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *pErrorCode=U_TRUNCATED_CHAR_FOUND;
            } अन्यथा {
                /* input consumed */
                अगर(सफाई) {
                    /* reset the converters without calling the callback functions */
                    _reset(sourceCnv, UCNV_RESET_TO_UNICODE, false);
                    _reset(targetCnv, UCNV_RESET_FROM_UNICODE, false);
                }

                /* done successfully */
                अवरोध;
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
        अगर(*pErrorCode==U_बफर_OVERFLOW_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *pErrorCode=U_ZERO_ERROR;
        } अन्यथा अगर(U_FAILURE(*pErrorCode) || (!सफाई && *pivotTarget==pivotStart)) {
            /* conversion error, or there was nothing left to convert */
            अवरोध;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        अगर( सफाई && toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 &&
            sourceCnv->UChar_Error_बफर_लंबाई==0
        ) {
            fromUArgs.सफाई=true;
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
    अगर(सफाई && U_SUCCESS(*pErrorCode)) {
        अगर(*target!=targetLimit) {
            **target=0;
            अगर(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {
                *pErrorCode=U_ZERO_ERROR;
            }
        } अन्यथा {
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
अटल int32_t
ucnv_internalConvert(UConverter *outConverter, UConverter *inConverter,
                     अक्षर *target, int32_t targetCapacity,
                     स्थिर अक्षर *source, int32_t source_लंबाई,
                     UErrorCode *pErrorCode) {
    char16_t pivot_बफर[CHUNK_आकार];
    char16_t *pivot, *pivot2;

    अक्षर *myTarget;
    स्थिर अक्षर *sourceLimit;
    स्थिर अक्षर *targetLimit;
    int32_t target_लंबाई=0;

    /* set up */
    अगर(source_लंबाई<0) {
        sourceLimit=uprv_strchr(source, 0);
    } अन्यथा {
        sourceLimit=source+source_लंबाई;
    }

    /* if there is no input data, we're done */
    अगर(source==sourceLimit) {
        वापस u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    pivot=pivot2=pivot_बफर;
    myTarget=target;
    target_लंबाई=0;

    अगर(targetCapacity>0) {
        /* perform real conversion */
        targetLimit=target+targetCapacity;
        ucnv_convertEx(outConverter, inConverter,
                       &myTarget, targetLimit,
                       &source, sourceLimit,
                       pivot_बफर, &pivot, &pivot2, pivot_बफर+CHUNK_आकार,
                       false,
                       true,
                       pErrorCode);
        target_लंबाई = static_cast<int32_t>(myTarget - target);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    अगर(*pErrorCode==U_बफर_OVERFLOW_ERROR || targetCapacity==0)
    {
        अक्षर target_बफर[CHUNK_आकार];

        targetLimit=target_बफर+CHUNK_आकार;
        करो {
            *pErrorCode=U_ZERO_ERROR;
            myTarget=target_बफर;
            ucnv_convertEx(outConverter, inConverter,
                           &myTarget, targetLimit,
                           &source, sourceLimit,
                           pivot_बफर, &pivot, &pivot2, pivot_बफर+CHUNK_आकार,
                           false,
                           true,
                           pErrorCode);
            target_लंबाई += static_cast<int32_t>(myTarget - target_बफर);
        } जबतक(*pErrorCode==U_बफर_OVERFLOW_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        वापस u_terminateChars(target, targetCapacity, target_लंबाई, pErrorCode);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    वापस target_लंबाई;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(स्थिर अक्षर *toConverterName, स्थिर अक्षर *fromConverterName,
             अक्षर *target, int32_t targetCapacity,
             स्थिर अक्षर *source, int32_t source_लंबाई,
             UErrorCode *pErrorCode) {
    UConverter in, out; /* stack-allocated */
    UConverter *inConverter, *outConverter;
    int32_t target_लंबाई;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर( source==nullptr || source_लंबाई<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* if there is no input data, we're done */
    अगर(source_लंबाई==0 || (source_लंबाई<0 && *source==0)) {
        वापस u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the converters */
    inConverter=ucnv_createConverter(&in, fromConverterName, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    outConverter=ucnv_createConverter(&out, toConverterName, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        ucnv_close(inConverter);
        वापस 0;
    }

    target_लंबाई=ucnv_internalConvert(outConverter, inConverter,
                                      target, targetCapacity,
                                      source, source_लंबाई,
                                      pErrorCode);

    ucnv_close(inConverter);
    ucnv_close(outConverter);

    वापस target_लंबाई;
}

/* @internal */
अटल int32_t
ucnv_convertAlgorithmic(UBool convertToAlgorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        अक्षर *target, int32_t targetCapacity,
                        स्थिर अक्षर *source, int32_t source_लंबाई,
                        UErrorCode *pErrorCode) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *to, *from;
    int32_t target_लंबाई;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर( cnv==nullptr || source==nullptr || source_लंबाई<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* if there is no input data, we're done */
    अगर(source_लंबाई==0 || (source_लंबाई<0 && *source==0)) {
        वापस u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* reset the other converter */
    अगर(convertToAlgorithmic) {
        /* cnv->Unicode->algo */
        ucnv_resetToUnicode(cnv);
        to=algoConverter;
        from=cnv;
    } अन्यथा {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        to=cnv;
    }

    target_लंबाई=ucnv_internalConvert(to, from,
                                      target, targetCapacity,
                                      source, source_लंबाई,
                                      pErrorCode);

    ucnv_close(algoConverter);

    वापस target_लंबाई;
}

U_CAPI int32_t U_EXPORT2
ucnv_toAlgorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   अक्षर *target, int32_t targetCapacity,
                   स्थिर अक्षर *source, int32_t source_लंबाई,
                   UErrorCode *pErrorCode) {
    वापस ucnv_convertAlgorithmic(true, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, source_लंबाई,
                                   pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     अक्षर *target, int32_t targetCapacity,
                     स्थिर अक्षर *source, int32_t source_लंबाई,
                     UErrorCode *pErrorCode) UPRV_NO_SANITIZE_UNDEFINED {

    अगर(algorithmicType<0 || UCNV_संख्या_OF_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    वापस ucnv_convertAlgorithmic(false, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, source_लंबाई,
                                   pErrorCode);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(स्थिर UConverter* converter)
{
    int8_t type = converter->shared_तथ्य->static_तथ्य->conversionType;
#अगर !UCONFIG_NO_LEGACY_CONVERSION
    अगर(type == UCNV_MBCS) {
        वापस ucnv_MBCSGetType(converter);
    }
#पूर्ण_अगर
    वापस (UConverterType)type;
}

U_CAPI व्योम  U_EXPORT2
ucnv_getStarters(स्थिर UConverter* converter, 
                 UBool starters[256],
                 UErrorCode* err)
{
    अगर (err == nullptr || U_FAILURE(*err)) {
        वापस;
    }

    अगर(converter->shared_तथ्य->impl->getStarters != nullptr) {
        converter->shared_तथ्य->impl->getStarters(converter, starters, err);
    } अन्यथा {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

अटल स्थिर UAmbiguousConverter *ucnv_getAmbiguous(स्थिर UConverter *cnv)
{
    UErrorCode errorCode;
    स्थिर अक्षर *name;
    int32_t i;

    अगर(cnv==nullptr) {
        वापस nullptr;
    }

    errorCode=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }

    क्रम(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        अगर(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            वापस ambiguousConverters+i;
        }
    }

    वापस nullptr;
}

U_CAPI व्योम  U_EXPORT2
ucnv_fixFileSeparator(स्थिर UConverter *cnv, 
                      char16_t* source,
                      int32_t source_लंबाई) {
    स्थिर UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    अगर(cnv==nullptr || source==nullptr || source_लंबाई<=0 || (a=ucnv_getAmbiguous(cnv))==nullptr)
    {
        वापस;
    }

    variant5c=a->variant5c;
    क्रम(i=0; i<source_लंबाई; ++i) {
        अगर(source[i]==variant5c) {
            source[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(स्थिर UConverter *cnv) {
    वापस ucnv_getAmbiguous(cnv)!=nullptr;
}

U_CAPI व्योम  U_EXPORT2
ucnv_setFallback(UConverter *cnv, UBool usesFallback)
{
    cnv->useFallback = usesFallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_usesFallback(स्थिर UConverter *cnv)
{
    वापस cnv->useFallback;
}

U_CAPI व्योम  U_EXPORT2
ucnv_getInvalidChars (स्थिर UConverter * converter,
                      अक्षर *errBytes,
                      int8_t * len,
                      UErrorCode * err)
{
    अगर (err == nullptr || U_FAILURE(*err))
    {
        वापस;
    }
    अगर (len == nullptr || errBytes == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर (*len < converter->invalid_Char_लंबाई)
    {
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }
    अगर ((*len = converter->invalid_Char_लंबाई) > 0)
    {
        uprv_memcpy (errBytes, converter->invalid_Char_बफर, *len);
    }
}

U_CAPI व्योम  U_EXPORT2
ucnv_getInvalidUChars (स्थिर UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UErrorCode * err)
{
    अगर (err == nullptr || U_FAILURE(*err))
    {
        वापस;
    }
    अगर (len == nullptr || errChars == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर (*len < converter->invalid_UChar_लंबाई)
    {
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }
    अगर ((*len = converter->invalid_UChar_लंबाई) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_बफर, *len);
    }
}

#घोषणा SIG_MAX_LEN 5

U_CAPI स्थिर अक्षर* U_EXPORT2
ucnv_detectUnicodeSignature( स्थिर अक्षर* source,
                             int32_t source_लंबाई,
                             int32_t* signature_लंबाई,
                             UErrorCode* pErrorCode) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    अक्षर start[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    पूर्णांक i = 0;

    अगर((pErrorCode==nullptr) || U_FAILURE(*pErrorCode)){
        वापस nullptr;
    }
    
    अगर(source == nullptr || source_लंबाई < -1){
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    अगर(signature_लंबाई == nullptr) {
        signature_लंबाई = &dummy;
    }

    अगर(source_लंबाई==-1){
        source_लंबाई=(int32_t)uprv_strlen(source);
    }

    
    जबतक(i<source_लंबाई&& i<SIG_MAX_LEN){
        start[i]=source[i];
        i++;
    }

    अगर(start[0] == '\xFE' && start[1] == '\xFF') {
        *signature_लंबाई=2;
        वापस  "UTF-16BE";
    } अन्यथा अगर(start[0] == '\xFF' && start[1] == '\xFE') {
        अगर(start[2] == '\x00' && start[3] =='\x00') {
            *signature_लंबाई=4;
            वापस "UTF-32LE";
        } अन्यथा {
            *signature_लंबाई=2;
            वापस  "UTF-16LE";
        }
    } अन्यथा अगर(start[0] == '\xEF' && start[1] == '\xBB' && start[2] == '\xBF') {
        *signature_लंबाई=3;
        वापस  "UTF-8";
    } अन्यथा अगर(start[0] == '\x00' && start[1] == '\x00' && 
              start[2] == '\xFE' && start[3]=='\xFF') {
        *signature_लंबाई=4;
        वापस  "UTF-32BE";
    } अन्यथा अगर(start[0] == '\x0E' && start[1] == '\xFE' && start[2] == '\xFF') {
        *signature_लंबाई=3;
        वापस "SCSU";
    } अन्यथा अगर(start[0] == '\xFB' && start[1] == '\xEE' && start[2] == '\x28') {
        *signature_लंबाई=3;
        वापस "BOCU-1";
    } अन्यथा अगर(start[0] == '\x2B' && start[1] == '\x2F' && start[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        अगर(start[3] == '\x38' && start[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_लंबाई=5;
            वापस "UTF-7";
        } अन्यथा अगर(start[3] == '\x38' || start[3] == '\x39' || start[3] == '\x2B' || start[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_लंबाई=4;
            वापस "UTF-7";
        }
    }अन्यथा अगर(start[0]=='\xDD' && start[1]== '\x73'&& start[2]=='\x66' && start[3]=='\x73'){
        *signature_लंबाई=4;
        वापस "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_लंबाई=0;
    वापस nullptr;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(स्थिर UConverter* cnv, UErrorCode* status)
{
    अगर(status == nullptr || U_FAILURE(*status)){
        वापस -1;
    }
    अगर(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस -1;
    }

    अगर(cnv->preFromUFirstCP >= 0){
        वापस U16_लंबाई(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }अन्यथा अगर(cnv->preFromULength < 0){
        वापस -cnv->preFromULength ;
    }अन्यथा अगर(cnv->fromUChar32 > 0){
        वापस 1;
    }
    वापस 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_toUCountPending(स्थिर UConverter* cnv, UErrorCode* status){

    अगर(status == nullptr || U_FAILURE(*status)){
        वापस -1;
    }
    अगर(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस -1;
    }

    अगर(cnv->preToULength > 0){
        वापस cnv->preToULength ;
    }अन्यथा अगर(cnv->preToULength < 0){
        वापस -cnv->preToULength;
    }अन्यथा अगर(cnv->toULength > 0){
        वापस cnv->toULength;
    }
    वापस 0;
}

U_CAPI UBool U_EXPORT2
ucnv_isFixedWidth(UConverter *cnv, UErrorCode *status){
    अगर (U_FAILURE(*status)) {
        वापस false;
    }

    अगर (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }

    चयन (ucnv_getType(cnv)) {
        हाल UCNV_SBCS:
        हाल UCNV_DBCS:
        हाल UCNV_UTF32_BigEndian:
        हाल UCNV_UTF32_LittleEndian:
        हाल UCNV_UTF32:
        हाल UCNV_US_ASCII:
            वापस true;
        शेष:
            वापस false;
    }
}
#पूर्ण_अगर

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
