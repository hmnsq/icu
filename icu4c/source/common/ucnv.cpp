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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব <memory>

#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/ucnv_err.h"
#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "utracimp.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "ucnv_imp.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#define CHUNK_আকার 1024

প্রকারলেখ কাঠামো UAmbiguousConverter {
    ধ্রুবক অক্ষর *name;
    ধ্রুবক char16_t variant5c;
} UAmbiguousConverter;

স্থির ধ্রুবক UAmbiguousConverter ambiguousConverters[]={
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
ucnv_open (ধ্রুবক অক্ষর *name,
                       UErrorCode * err)
{
    UConverter *r;

    যদি (err == nullptr || U_FAILURE (*err)) {
        ফেরত nullptr;
    }

    r =  ucnv_createConverter(nullptr, name, err);
    ফেরত r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ধ্রুবক অক্ষর *packageName, ধ্রুবক অক্ষর *converterName, UErrorCode * err)
{
    ফেরত ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ধ্রুবক char16_t * name,
                         UErrorCode * err)
{
    অক্ষর asciiName[UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য];

    যদি (err == nullptr || U_FAILURE(*err))
        ফেরত nullptr;
    যদি (name == nullptr)
        ফেরত ucnv_open (nullptr, err);
    যদি (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    ফেরত ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
স্থির int32_t
ucnv_copyPlatformString(অক্ষর *platformString, UConverterPlatform pltfrm)
{
    নির্বাচন (pltfrm)
    {
    ক্ষেত্র UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        ফেরত 4;
    ক্ষেত্র UCNV_UNKNOWN:
        বিরতি;
    }

    /* default to empty string */
    *platformString = 0;
    ফেরত 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UErrorCode * err)
{
    অক্ষর myName[UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য];
    int32_t myNameLen;

    যদি (err == nullptr || U_FAILURE (*err))
        ফেরত nullptr;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integerToString(myName + myNameLen, codepage, 10);

    ফেরত ucnv_createConverter(nullptr, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_safeClone(ধ্রুবক UConverter* cnv,
               শূন্য* স্তূপ_বাফার U_LIFETIME_BOUND,
               int32_t* p_বাফার_আকার,
               UErrorCode* status)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t স্তূপ_বাফার_আকার;
    int32_t বাফার_আকার_Needed;
    UErrorCode cbErr;
    UConverterToUnicodeArgs toUArgs = {
        আকার(UConverterToUnicodeArgs),
            সত্য,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };
    UConverterFromUnicodeArgs fromUArgs = {
        আকার(UConverterFromUnicodeArgs),
            সত্য,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    };

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLONE);

    যদি (status == nullptr || U_FAILURE(*status)){
        UTRACE_EXIT_STATUS(status? *status: U_ILLEGAL_ARGUMENT_ERROR);
        ফেরত nullptr;
    }

    যদি (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(*status);
        ফেরত nullptr;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, status), cnv, স্তূপ_বাফার);

    যদি (cnv->shared_তথ্য->impl->safeClone != nullptr) {
        /* call the custom safeClone function for sizing */
        বাফার_আকার_Needed = 0;
        cnv->shared_তথ্য->impl->safeClone(cnv, nullptr, &বাফার_আকার_Needed, status);
        যদি (U_FAILURE(*status)) {
            UTRACE_EXIT_STATUS(*status);
            ফেরত nullptr;
        }
    }
    অন্যথা
    {
        /* inherent sizing */
        বাফার_আকার_Needed = আকার(UConverter);
    }

    যদি (p_বাফার_আকার == nullptr) {
        স্তূপ_বাফার_আকার = 1;
        p_বাফার_আকার = &স্তূপ_বাফার_আকার;
    } অন্যথা {
        স্তূপ_বাফার_আকার = *p_বাফার_আকার;
        যদি (স্তূপ_বাফার_আকার <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_বাফার_আকার = বাফার_আকার_Needed;
            UTRACE_EXIT_মান(বাফার_আকার_Needed);
            ফেরত nullptr;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    যদি (স্তূপ_বাফার) {
        uintptr_t p = reinterpret_cast<uintptr_t>(স্তূপ_বাফার);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t pointerAdjustment = aligned_p - p;
        যদি (বাফার_আকার_Needed + pointerAdjustment <= স্তূপ_বাফার_আকার) {
            স্তূপ_বাফার = reinterpret_cast<শূন্য *>(aligned_p);
            স্তূপ_বাফার_আকার -= static_cast<int32_t>(pointerAdjustment);
        } অন্যথা {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            স্তূপ_বাফার_আকার = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    যদি (স্তূপ_বাফার_আকার < বাফার_আকার_Needed || স্তূপ_বাফার == nullptr)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (বাফার_আকার_Needed);

        যদি(localConverter == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            UTRACE_EXIT_STATUS(*status);
            ফেরত nullptr;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        যদি (p_বাফার_আকার != &স্তূপ_বাফার_আকার) {
            *status = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_বাফার_আকার = বাফার_আকার_Needed;
    } অন্যথা {
        /* just use the stack buffer */
        localConverter = (UConverter*) স্তূপ_বাফার;
        allocatedConverter = nullptr;
    }

    uprv_memset(localConverter, 0, বাফার_আকার_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, আকার(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = মিথ্যা;

    /* copy the substitution string */
    যদি (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } অন্যথা {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_বাফার_দৈর্ঘ্য * U_SIZEOF_UCHAR);
        যদি (localConverter->subChars == nullptr) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_STATUS(*status);
            ফেরত nullptr;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_বাফার_দৈর্ঘ্য * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    যদি (cnv->shared_তথ্য->impl->safeClone != nullptr) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_তথ্য->impl->safeClone(cnv, localConverter, p_বাফার_আকার, status);
    }

    যদি(localConverter==nullptr || U_FAILURE(*status)) {
        যদি (allocatedConverter != nullptr && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_STATUS(*status);
        ফেরত nullptr;
    }

    /* increment refcount of shared data if needed */
    যদি (cnv->shared_তথ্য->isReferenceCounted) {
        ucnv_increment_Ref_গুনতি(cnv->shared_তথ্য);
    }

    যদি(localConverter == (UConverter*)স্তূপ_বাফার) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = সত্য;
    }

    /* allow callback functions to handle any memory allocation */
    toUArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->toUContext, &toUArgs, nullptr, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, nullptr, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_STATUS(localConverter, *status);
    ফেরত localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ধ্রুবক UConverter* cnv, UErrorCode *status)
{
    ফেরত ucnv_safeClone(cnv, nullptr, nullptr, status);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI শূন্য  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UTRACE_ENTRY_OC(UTRACE_UCNV_CLOSE);

    যদি (converter == nullptr)
    {
        UTRACE_EXIT();
        ফেরত;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &errorCode), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    যদি (converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
        UConverterToUnicodeArgs toUArgs = {
            আকার(UConverterToUnicodeArgs),
                সত্য,
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
    যদি (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverterFromUnicodeArgs fromUArgs = {
            আকার(UConverterFromUnicodeArgs),
                সত্য,
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

    যদি (converter->shared_তথ্য->impl->close != nullptr) {
        converter->shared_তথ্য->impl->close(converter);
    }

    যদি (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    যদি (converter->shared_তথ্য->isReferenceCounted) {
        ucnv_unload_Shared_তথ্য_If_Ready(converter->shared_তথ্য);
    }

    যদি(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ধ্রুবক অক্ষর*   U_EXPORT2
ucnv_getAvailableName (int32_t n)
{
    যদি (0 <= n && n <= 0xffff) {
        UErrorCode err = U_ZERO_ERROR;
        ধ্রুবক অক্ষর *name = ucnv_bld_getAvailableConverter((uint16_t)n, &err);
        যদি (U_SUCCESS(err)) {
            ফেরত name;
        }
    }
    ফেরত nullptr;
}

U_CAPI int32_t   U_EXPORT2
ucnv_গুনতি_Available ()
{
    UErrorCode err = U_ZERO_ERROR;
    ফেরত ucnv_bld_গুনতি_Available_Converters(&err);
}

U_CAPI শূন্য    U_EXPORT2
ucnv_getSubstChars (ধ্রুবক UConverter * converter,
                    অক্ষর *mySubChar,
                    int8_t * len,
                    UErrorCode * err)
{
    যদি (U_FAILURE (*err))
        ফেরত;

    যদি (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        ফেরত;
    }

    যদি (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI শূন্য    U_EXPORT2
ucnv_setSubstChars (UConverter * converter,
                    ধ্রুবক অক্ষর *mySubChar,
                    int8_t len,
                    UErrorCode * err)
{
    যদি (U_FAILURE (*err))
        ফেরত;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    যদি ((len > converter->shared_তথ্য->static_তথ্য->maxBytesPerChar)
     || (len < converter->shared_তথ্য->static_তথ্য->minBytesPerChar))
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
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

U_CAPI শূন্য U_EXPORT2
ucnv_setSubstString(UConverter *cnv,
                    ধ্রুবক char16_t *s,
                    int32_t length,
                    UErrorCode *err) {
    alignas(UConverter) অক্ষর clone_বাফার[U_CNV_SAFECLONE_BUFFERSIZE];
    অক্ষর chars[UCNV_ERROR_বাফার_দৈর্ঘ্য];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_আকার, length8;

    /* Let the following functions check all arguments. */
    clone_আকার = আকার(clone_বাফার);
    clone = ucnv_safeClone(cnv, clone_বাফার, &clone_আকার, err);
    ucnv_setFromUCallBack(clone, UCNV_FROM_U_CALLBACK_STOP, nullptr, nullptr, nullptr, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)আকার(chars), s, length, err);
    ucnv_close(clone);
    যদি (U_FAILURE(*err)) {
        ফেরত;
    }

    যদি (cnv->shared_তথ্য->impl->writeSub == nullptr
#যদি !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_তথ্য->static_তথ্য->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#endif
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } অন্যথা {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        যদি (length > UCNV_ERROR_বাফার_দৈর্ঘ্য) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_বাফার_OVERFLOW_ERROR;
            ফেরত;
        }
        subChars = (uint8_t *)s;
        যদি (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    যদি (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        যদি (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_বাফার_দৈর্ঘ্য * U_SIZEOF_UCHAR);
            যদি (cnv->subChars == nullptr) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_বাফার_দৈর্ঘ্য * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    যদি (length8 == 0) {
        cnv->subCharLen = 0;
    } অন্যথা {
        uprv_memcpy(cnv->subChars, subChars, length8);
        যদি (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } অন্যথা /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
স্থির শূন্য _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    যদি(converter == nullptr) {
        ফেরত;
    }

    যদি(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UErrorCode errorCode;

        যদি(choice<=UCNV_RESET_TO_UNICODE && converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
            UConverterToUnicodeArgs toUArgs = {
                আকার(UConverterToUnicodeArgs),
                সত্য,
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
        যদি(choice!=UCNV_RESET_TO_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverterFromUnicodeArgs fromUArgs = {
                আকার(UConverterFromUnicodeArgs),
                সত্য,
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
    যদি(choice<=UCNV_RESET_TO_UNICODE) {
        converter->toUnicodeStatus = converter->shared_তথ্য->toUnicodeStatus;
        converter->mode = 0;
        converter->toULength = 0;
        converter->invalid_Char_দৈর্ঘ্য = converter->UChar_Error_বাফার_দৈর্ঘ্য = 0;
        converter->preToULength = 0;
    }
    যদি(choice!=UCNV_RESET_TO_UNICODE) {
        converter->fromUnicodeStatus = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_দৈর্ঘ্য = converter->char_Error_বাফার_দৈর্ঘ্য = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    যদি (converter->shared_তথ্য->impl->reset != nullptr) {
        /* call the custom reset function */
        converter->shared_তথ্য->impl->reset(converter, choice);
    }
}

U_CAPI শূন্য  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, সত্য);
}

U_CAPI শূন্য  U_EXPORT2
ucnv_resetToUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_TO_UNICODE, সত্য);
}

U_CAPI শূন্য  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, সত্য);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_আকার (ধ্রুবক UConverter * converter)
{
    ফেরত converter->maxBytesPerUChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_আকার (ধ্রুবক UConverter * converter)
{
    ফেরত converter->shared_তথ্য->static_তথ্য->minBytesPerChar;
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ucnv_getName(ধ্রুবক UConverter* converter U_LIFETIME_BOUND, UErrorCode* err)
{
    যদি (U_FAILURE (*err))
        ফেরত nullptr;
    যদি(converter->shared_তথ্য->impl->getName){
        ধ্রুবক অক্ষর* অস্থায়ী= converter->shared_তথ্য->impl->getName(converter);
        যদি(অস্থায়ী)
            ফেরত অস্থায়ী;
    }
    ফেরত converter->shared_তথ্য->static_তথ্য->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ধ্রুবক UConverter * converter,
              UErrorCode * err)
{
    int32_t ccsid;
    যদি (U_FAILURE (*err))
        ফেরত -1;

    ccsid = converter->shared_তথ্য->static_তথ্য->codepage;
    যদি (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        ধ্রুবক অক্ষর *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        যদি (U_SUCCESS(*err) && standardName) {
            ধ্রুবক অক্ষর *ccsidStr = uprv_strchr(standardName, '-');
            যদি (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    ফেরত ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (ধ্রুবক UConverter * converter,
                                      UErrorCode * err)
{
    যদি (U_FAILURE (*err))
        ফেরত UCNV_UNKNOWN;

    ফেরত (UConverterPlatform)converter->shared_তথ্য->static_তথ্য->platform;
}

U_CAPI শূন্য U_EXPORT2
    ucnv_getToUCallBack (ধ্রুবক UConverter * converter,
                         UConverterToUCallback *action,
                         ধ্রুবক শূন্য **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->toUContext;
}

U_CAPI শূন্য U_EXPORT2
    ucnv_getFromUCallBack (ধ্রুবক UConverter * converter,
                           UConverterFromUCallback *action,
                           ধ্রুবক শূন্য **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI শূন্য    U_EXPORT2
ucnv_setToUCallBack (UConverter * converter,
                            UConverterToUCallback newAction,
                            ধ্রুবক শূন্য* newContext,
                            UConverterToUCallback *oldAction,
                            ধ্রুবক শূন্য** oldContext,
                            UErrorCode * err)
{
    যদি (U_FAILURE (*err))
        ফেরত;
    যদি (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    যদি (oldContext) *oldContext = converter->toUContext;
    converter->toUContext = newContext;
}

U_CAPI শূন্য  U_EXPORT2
ucnv_setFromUCallBack (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ধ্রুবক শূন্য* newContext,
                            UConverterFromUCallback *oldAction,
                            ধ্রুবক শূন্য** oldContext,
                            UErrorCode * err)
{
    যদি (U_FAILURE (*err))
        ফেরত;
    যদি (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    যদি (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

স্থির শূন্য
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t source_সূচক, int32_t error_প্রবেশ_দৈর্ঘ্য) {
    int32_t *limit;
    int32_t delta, offset;

    যদি(source_সূচক>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=source_সূচক-error_প্রবেশ_দৈর্ঘ্য;
    } অন্যথা {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    limit=offsets+length;
    যদি(delta==0) {
        /* most common case, nothing to do */
    } অন্যথা যদি(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        যখন(offsets<limit) {
            offset=*offsets;
            যদি(offset>=0) {
                *offsets=offset+delta;
            }
            ++offsets;
        }
    } অন্যথা /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        যখন(offsets<limit) {
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

স্থির শূন্য
_fromUnicodeWithCallback(UConverterFromUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ধ্রুবক char16_t *s;
    অক্ষর *t;
    int32_t *offsets;
    int32_t source_সূচক;
    int32_t error_প্রবেশ_দৈর্ঘ্য;
    UBool converter_Saw_End_Of_প্রবেশ, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ধ্রুবক char16_t *realSource, *realSourceLimit;
    int32_t real_Source_সূচক;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    source_সূচক=0;
    যদি(offsets==nullptr) {
        fromUnicode=cnv->shared_তথ্য->impl->fromUnicode;
    } অন্যথা {
        fromUnicode=cnv->shared_তথ্য->impl->fromUnicodeWithOffsets;
        যদি(fromUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_তথ্য->impl->fromUnicode;
            /* we will write -1 for each offset */
            source_সূচক=-1;
        }
    }

    যদি(cnv->preFromULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=মিথ্যা;
        real_Source_সূচক=0;
    } অন্যথা {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        যদি (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        real_Source_সূচক=source_সূচক;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preFromULength;
        pArgs->flush=মিথ্যা;
        source_সূচক=-1;

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
    জন্য(;;) {
        যদি(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_End_Of_প্রবেশ=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->fromUChar32==0);
        } অন্যথা {
            /* handle error from ucnv_convertEx() */
            converter_Saw_End_Of_প্রবেশ=মিথ্যা;
        }

        /* no callback called yet for this iteration */
        calledCallback=মিথ্যা;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_প্রবেশ_দৈর্ঘ্য=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        জন্য(;;) {
            /* update offsets if we write any */
            যদি(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                যদি(length>0) {
                    _updateOffsets(offsets, length, source_সূচক, error_প্রবেশ_দৈর্ঘ্য);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                যদি(source_সূচক>=0) {
                    source_সূচক += static_cast<int32_t>(pArgs->source - s);
                }
            }

            যদি(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                যদি(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    real_Source_সূচক=source_সূচক;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preFromULength;
                    pArgs->flush=মিথ্যা;
                    যদি((source_সূচক+=cnv->preFromULength)<0) {
                        source_সূচক=-1;
                    }

                    cnv->preFromULength=0;
                } অন্যথা {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            যদি(U_SUCCESS(*err)) {
                যদি(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    বিরতি;
                } অন্যথা যদি(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    source_সূচক=real_Source_সূচক;

                    realSource=nullptr;
                    বিরতি;
                } অন্যথা যদি(pArgs->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=মিথ্যা; /* new error condition */
                } অন্যথা {
                    /* input consumed */
                    যদি(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        যদি(!converter_Saw_End_Of_প্রবেশ) {
                            বিরতি;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, মিথ্যা);
                    }

                    /* done successfully */
                    ফেরত;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                যদি( calledCallback ||
                    (e=*err)==U_বাফার_OVERFLOW_ERROR ||
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
                    যদি(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        যদি(length>0) {
                            u_memcpy(cnv->preFromU, pArgs->source, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    ফেরত;
                }
            }

            /* callback handling */
            {
                UChar32 codePoint;

                /* get and write the code point */
                codePoint=cnv->fromUChar32;
                error_প্রবেশ_দৈর্ঘ্য=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_বাফার, error_প্রবেশ_দৈর্ঘ্য, codePoint);
                cnv->invalid_UChar_দৈর্ঘ্য = static_cast<int8_t>(error_প্রবেশ_দৈর্ঘ্য);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, pArgs,
                    cnv->invalid_UChar_বাফার, error_প্রবেশ_দৈর্ঘ্য, codePoint,
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
            calledCallback=সত্য;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
স্থির UBool
ucnv_outputOverflowFromUnicode(UConverter *cnv,
                               অক্ষর **target, ধ্রুবক অক্ষর *targetLimit,
                               int32_t **pOffsets,
                               UErrorCode *err) {
    int32_t *offsets;
    অক্ষর *overflow, *t;
    int32_t i, length;

    t=*target;
    যদি(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } অন্যথা {
        offsets=nullptr;
    }

    overflow = reinterpret_cast<অক্ষর*>(cnv->char_Error_বাফার);
    length=cnv->char_Error_বাফার_দৈর্ঘ্য;
    i=0;
    যখন(i<length) {
        যদি(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            করো {
                overflow[j++]=overflow[i++];
            } যখন(i<length);

            cnv->char_Error_বাফার_দৈর্ঘ্য = static_cast<int8_t>(j);
            *target=t;
            যদি(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_বাফার_OVERFLOW_ERROR;
            ফেরত সত্য;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        যদি(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_বাফার_দৈর্ঘ্য=0;
    *target=t;
    যদি(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    ফেরত মিথ্যা;
}

U_CAPI শূন্য U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 অক্ষর **target, ধ্রুবক অক্ষর *targetLimit,
                 ধ্রুবক char16_t **source, ধ্রুবক char16_t *sourceLimit,
                 int32_t *offsets,
                 UBool flush,
                 UErrorCode *err) {
    UConverterFromUnicodeArgs args;
    ধ্রুবক char16_t *s;
    অক্ষর *t;

    /* check parameters */
    যদি(err==nullptr || U_FAILURE(*err)) {
        ফেরত;
    }

    যদি(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    s=*source;
    t=*target;

    যদি ((ধ্রুবক শূন্য *)U_MAX_PTR(sourceLimit) == (ধ্রুবক শূন্য *)sourceLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        sourceLimit = (ধ্রুবক char16_t *)(((ধ্রুবক অক্ষর *)sourceLimit) - 1);
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
    যদি (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x3fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t) ||
        (((ধ্রুবক অক্ষর *)sourceLimit-(ধ্রুবক অক্ষর *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    
    /* output the target overflow buffer */
    যদি( cnv->char_Error_বাফার_দৈর্ঘ্য>0 &&
        ucnv_outputOverflowFromUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        ফেরত;
    }
    /* *target may have moved, therefore stop using t */

    যদি(!flush && s==sourceLimit && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        ফেরত;
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
    args.size=আকার(args);

    _fromUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

স্থির শূন্য
_toUnicodeWithCallback(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterToUnicode toUnicode;
    UConverter *cnv;
    ধ্রুবক অক্ষর *s;
    char16_t *t;
    int32_t *offsets;
    int32_t source_সূচক;
    int32_t error_প্রবেশ_দৈর্ঘ্য;
    UBool converter_Saw_End_Of_প্রবেশ, calledCallback;

    /* variables for m:n conversion */
    অক্ষর replay[UCNV_EXT_MAX_BYTES];
    ধ্রুবক অক্ষর *realSource, *realSourceLimit;
    int32_t real_Source_সূচক;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    source_সূচক=0;
    যদি(offsets==nullptr) {
        toUnicode=cnv->shared_তথ্য->impl->toUnicode;
    } অন্যথা {
        toUnicode=cnv->shared_তথ্য->impl->toUnicodeWithOffsets;
        যদি(toUnicode==nullptr) {
            /* there is no WithOffsets implementation */
            toUnicode=cnv->shared_তথ্য->impl->toUnicode;
            /* we will write -1 for each offset */
            source_সূচক=-1;
        }
    }

    যদি(cnv->preToULength>=0) {
        /* normal mode */
        realSource=nullptr;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=nullptr;
        realFlush=মিথ্যা;
        real_Source_সূচক=0;
    } অন্যথা {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        যদি (!pArgs->source) {
            *err=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        real_Source_সূচক=source_সূচক;

        uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preToULength;
        pArgs->flush=মিথ্যা;
        source_সূচক=-1;

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
    জন্য(;;) {
        যদি(U_SUCCESS(*err)) {
            /* convert */
            toUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_End_Of_প্রবেশ=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->toULength==0);
        } অন্যথা {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_End_Of_প্রবেশ=মিথ্যা;
        }

        /* no callback called yet for this iteration */
        calledCallback=মিথ্যা;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_প্রবেশ_দৈর্ঘ্য=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        জন্য(;;) {
            /* update offsets if we write any */
            যদি(offsets!=nullptr) {
                int32_t length = static_cast<int32_t>(pArgs->target - t);
                যদি(length>0) {
                    _updateOffsets(offsets, length, source_সূচক, error_প্রবেশ_দৈর্ঘ্য);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                যদি(source_সূচক>=0) {
                    source_সূচক += static_cast<int32_t>(pArgs->source - s);
                }
            }

            যদি(cnv->preToULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                যদি(realSource==nullptr) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    real_Source_সূচক=source_সূচক;

                    uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preToULength;
                    pArgs->flush=মিথ্যা;
                    যদি((source_সূচক+=cnv->preToULength)<0) {
                        source_সূচক=-1;
                    }

                    cnv->preToULength=0;
                } অন্যথা {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==nullptr);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            যদি(U_SUCCESS(*err)) {
                যদি(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    বিরতি;
                } অন্যথা যদি(realSource!=nullptr) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    source_সূচক=real_Source_সূচক;

                    realSource=nullptr;
                    বিরতি;
                } অন্যথা যদি(pArgs->flush && cnv->toULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=মিথ্যা; /* new error condition */
                } অন্যথা {
                    /* input consumed */
                    যদি(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        যদি(!converter_Saw_End_Of_প্রবেশ) {
                            বিরতি;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_TO_UNICODE, মিথ্যা);
                    }

                    /* done successfully */
                    ফেরত;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                যদি( calledCallback ||
                    (e=*err)==U_বাফার_OVERFLOW_ERROR ||
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
                    যদি(realSource!=nullptr) {
                        int32_t length;

                        U_ASSERT(cnv->preToULength==0);

                        length = static_cast<int32_t>(pArgs->sourceLimit - pArgs->source);
                        যদি(length>0) {
                            uprv_memcpy(cnv->preToU, pArgs->source, length);
                            cnv->preToULength = static_cast<int8_t>(-length);
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    ফেরত;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_প্রবেশ_দৈর্ঘ্য=cnv->invalid_Char_দৈর্ঘ্য=cnv->toULength;
            যদি(error_প্রবেশ_দৈর্ঘ্য>0) {
                uprv_memcpy(cnv->invalid_Char_বাফার, cnv->toUBytes, error_প্রবেশ_দৈর্ঘ্য);
            }

            /* set the converter state to deal with the next character */
            cnv->toULength=0;

            /* call the callback function */
            যদি(cnv->toUCallbackReason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_FOUND) {
                cnv->toUCallbackReason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->toUContext, pArgs,
                cnv->invalid_Char_বাফার, error_প্রবেশ_দৈর্ঘ্য,
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
            calledCallback=সত্য;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
স্থির UBool
ucnv_outputOverflowToUnicode(UConverter *cnv,
                             char16_t **target, ধ্রুবক char16_t *targetLimit,
                             int32_t **pOffsets,
                             UErrorCode *err) {
    int32_t *offsets;
    char16_t *overflow, *t;
    int32_t i, length;

    t=*target;
    যদি(pOffsets!=nullptr) {
        offsets=*pOffsets;
    } অন্যথা {
        offsets=nullptr;
    }

    overflow=cnv->UChar_Error_বাফার;
    length=cnv->UChar_Error_বাফার_দৈর্ঘ্য;
    i=0;
    যখন(i<length) {
        যদি(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            করো {
                overflow[j++]=overflow[i++];
            } যখন(i<length);

            cnv->UChar_Error_বাফার_দৈর্ঘ্য = static_cast<int8_t>(j);
            *target=t;
            যদি(offsets!=nullptr) {
                *pOffsets=offsets;
            }
            *err=U_বাফার_OVERFLOW_ERROR;
            ফেরত সত্য;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        যদি(offsets!=nullptr) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_বাফার_দৈর্ঘ্য=0;
    *target=t;
    যদি(offsets!=nullptr) {
        *pOffsets=offsets;
    }
    ফেরত মিথ্যা;
}

U_CAPI শূন্য U_EXPORT2
ucnv_toUnicode(UConverter *cnv,
               char16_t **target, ধ্রুবক char16_t *targetLimit,
               ধ্রুবক অক্ষর **source, ধ্রুবক অক্ষর *sourceLimit,
               int32_t *offsets,
               UBool flush,
               UErrorCode *err) {
    UConverterToUnicodeArgs args;
    ধ্রুবক অক্ষর *s;
    char16_t *t;

    /* check parameters */
    যদি(err==nullptr || U_FAILURE(*err)) {
        ফেরত;
    }

    যদি(cnv==nullptr || target==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    s=*source;
    t=*target;

    যদি ((ধ্রুবক শূন্য *)U_MAX_PTR(targetLimit) == (ধ্রুবক শূন্য *)targetLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        targetLimit = (ধ্রুবক char16_t *)(((ধ্রুবক অক্ষর *)targetLimit) - 1);
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
    যদি (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x3fffffff && targetLimit>t) ||
        (((ধ্রুবক অক্ষর *)targetLimit-(ধ্রুবক অক্ষর *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    
    /* output the target overflow buffer */
    যদি( cnv->UChar_Error_বাফার_দৈর্ঘ্য>0 &&
        ucnv_outputOverflowToUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        ফেরত;
    }
    /* *target may have moved, therefore stop using t */

    যদি(!flush && s==sourceLimit && cnv->preToULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        ফেরত;
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
    args.size=আকার(args);

    _toUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                অক্ষর *dest, int32_t destCapacity,
                ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                UErrorCode *pErrorCode) {
    ধ্রুবক char16_t *srcLimit;
    অক্ষর *originalDest, *destLimit;
    int32_t dest_দৈর্ঘ্য;

    /* check arguments */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        src_দৈর্ঘ্য<-1 || (src_দৈর্ঘ্য!=0 && src==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    যদি(src_দৈর্ঘ্য==-1) {
        src_দৈর্ঘ্য=u_strlen(src);
    }
    যদি(src_দৈর্ঘ্য>0) {
        srcLimit=src+src_দৈর্ঘ্য;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode বাফার_Status = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, সত্য, &বাফার_Status);
        dest_দৈর্ঘ্য=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        যদি(বাফার_Status==U_বাফার_OVERFLOW_ERROR) {
            অক্ষর বাফার[1024];

            destLimit=বাফার+আকার(বাফার);
            করো {
                dest=বাফার;
                বাফার_Status=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, সত্য, &বাফার_Status);
                dest_দৈর্ঘ্য+=(int32_t)(dest-বাফার);
            } যখন(বাফার_Status==U_বাফার_OVERFLOW_ERROR);
        }
        যদি (U_FAILURE(বাফার_Status)) {
            *pErrorCode = বাফার_Status;
        }
    } অন্যথা {
        dest_দৈর্ঘ্য=0;
    }

    ফেরত u_terminateChars(originalDest, destCapacity, dest_দৈর্ঘ্য, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_toUChars(UConverter *cnv,
              char16_t *dest, int32_t destCapacity,
              ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
              UErrorCode *pErrorCode) {
    ধ্রুবক অক্ষর *srcLimit;
    char16_t *originalDest, *destLimit;
    int32_t dest_দৈর্ঘ্য;

    /* check arguments */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি( cnv==nullptr ||
        destCapacity<0 || (destCapacity>0 && dest==nullptr) ||
        src_দৈর্ঘ্য<-1 || (src_দৈর্ঘ্য!=0 && src==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* initialize */
    ucnv_resetToUnicode(cnv);
    originalDest=dest;
    যদি(src_দৈর্ঘ্য==-1) {
        src_দৈর্ঘ্য=(int32_t)uprv_strlen(src);
    }
    যদি(src_দৈর্ঘ্য>0) {
        srcLimit=src+src_দৈর্ঘ্য;
        destCapacity=pinCapacity(dest, destCapacity);
        destLimit=dest+destCapacity;

        /* perform the conversion */
        UErrorCode বাফার_Status = U_ZERO_ERROR;
        ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, সত্য, &বাফার_Status);
        dest_দৈর্ঘ্য=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        যদি(বাফার_Status==U_বাফার_OVERFLOW_ERROR)
        {
            char16_t বাফার[1024];

            destLimit=বাফার+UPRV_LENGTHOF(বাফার);
            করো {
                dest=বাফার;
                বাফার_Status=U_ZERO_ERROR;
                ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, সত্য, &বাফার_Status);
                dest_দৈর্ঘ্য+=(int32_t)(dest-বাফার);
            }
            যখন(বাফার_Status==U_বাফার_OVERFLOW_ERROR);
        }
        যদি (U_FAILURE(বাফার_Status)) {
            *pErrorCode = বাফার_Status;
        }
    } অন্যথা {
        dest_দৈর্ঘ্য=0;
    }

    ফেরত u_terminateUChars(originalDest, destCapacity, dest_দৈর্ঘ্য, pErrorCode);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_পরবর্তী_UChar(UConverter *cnv,
                  ধ্রুবক অক্ষর **source, ধ্রুবক অক্ষর *sourceLimit,
                  UErrorCode *err) {
    UConverterToUnicodeArgs args;
    char16_t বাফার[U16_MAX_দৈর্ঘ্য];
    ধ্রুবক অক্ষর *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    যদি(err==nullptr || U_FAILURE(*err)) {
        ফেরত 0xffff;
    }

    যদি(cnv==nullptr || source==nullptr) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0xffff;
    }

    s=*source;
    যদি(sourceLimit<s) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0xffff;
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
    যদি(((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    যদি(cnv->UChar_Error_বাফার_দৈর্ঘ্য>0) {
        char16_t *overflow;

        overflow=cnv->UChar_Error_বাফার;
        i=0;
        length=cnv->UChar_Error_বাফার_দৈর্ঘ্য;
        U16_পরবর্তী(overflow, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        যদি((cnv->UChar_Error_বাফার_দৈর্ঘ্য=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_বাফার, cnv->UChar_Error_বাফার+i,
                         cnv->UChar_Error_বাফার_দৈর্ঘ্য*U_SIZEOF_UCHAR);
        }

        যদি(!U16_IS_LEAD(c) || i<length) {
            ফেরত c;
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
    args.flush=সত্য;
    args.offsets=nullptr;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=বাফার;
    args.targetLimit=বাফার+1;
    args.size=আকার(args);

    যদি(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        যদি(cnv->toULength==0 && cnv->shared_তথ্য->impl->get_পরবর্তী_UChar!=nullptr) {
            c=cnv->shared_তথ্য->impl->get_পরবর্তী_UChar(&args, err);
            *source=s=args.source;
            যদি(*err==U_সূচক_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_TO_UNICODE, মিথ্যা);
                ফেরত 0xffff; /* no output */
            } অন্যথা যদি(U_SUCCESS(*err) && c>=0) {
                ফেরত c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        _toUnicodeWithCallback(&args, err);

        যদি(*err==U_বাফার_OVERFLOW_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(args.target-বাফার);
    } অন্যথা {
        /* write the lead surrogate from the overflow buffer */
        বাফার[0]=(char16_t)c;
        args.target=বাফার+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    যদি(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } অন্যথা যদি(length==0) {
        /* no input or only state changes */
        *err=U_সূচক_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } অন্যথা {
        c=বাফার[0];
        i=1;
        যদি(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } অন্যথা {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            যদি(cnv->UChar_Error_বাফার_দৈর্ঘ্য>0) {
                /* got overflow output from the conversion */
                যদি(U16_IS_TRAIL(c2=cnv->UChar_Error_বাফার[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    যদি((--cnv->UChar_Error_বাফার_দৈর্ঘ্য)>0) {
                        uprv_memmove(cnv->UChar_Error_বাফার, cnv->UChar_Error_বাফার+1,
                                     cnv->UChar_Error_বাফার_দৈর্ঘ্য*U_SIZEOF_UCHAR);
                    }
                } অন্যথা {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } অন্যথা যদি(args.source<sourceLimit) {
                /* convert once more, to buffer[1] */
                args.targetLimit=বাফার+2;
                _toUnicodeWithCallback(&args, err);
                যদি(*err==U_বাফার_OVERFLOW_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(args.target-বাফার);
                যদি(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=বাফার[1])) {
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
    যদি(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        যদি((length=cnv->UChar_Error_বাফার_দৈর্ঘ্য)>0) {
            uprv_memmove(cnv->UChar_Error_বাফার+delta, cnv->UChar_Error_বাফার,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_বাফার_দৈর্ঘ্য=(int8_t)(length+delta);

        cnv->UChar_Error_বাফার[0]=বাফার[i++];
        যদি(delta>1) {
            cnv->UChar_Error_বাফার[1]=বাফার[i];
        }
    }

    *source=args.source;
    ফেরত c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI শূন্য U_EXPORT2
ucnv_convertEx(UConverter *targetCnv, UConverter *sourceCnv,
               অক্ষর **target, ধ্রুবক অক্ষর *targetLimit,
               ধ্রুবক অক্ষর **source, ধ্রুবক অক্ষর *sourceLimit,
               char16_t *pivotStart, char16_t **pivotSource,
               char16_t **pivotTarget, ধ্রুবক char16_t *pivotLimit,
               UBool reset, UBool flush,
               UErrorCode *pErrorCode) {
    char16_t pivot_বাফার[CHUNK_আকার];
    ধ্রুবক char16_t *myPivotSource;
    char16_t *myPivotTarget;
    ধ্রুবক অক্ষর *s;
    অক্ষর *t;

    UConverterToUnicodeArgs toUArgs;
    UConverterFromUnicodeArgs fromUArgs;
    UConverterConvert convert;

    /* error checking */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত;
    }

    যদি( targetCnv==nullptr || sourceCnv==nullptr ||
        source==nullptr || *source==nullptr ||
        target==nullptr || *target==nullptr || targetLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    s=*source;
    t=*target;
    যদি((sourceLimit!=nullptr && sourceLimit<s) || targetLimit<t) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    যদি(
        (sourceLimit!=nullptr && ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    
    যদি(pivotStart==nullptr) {
        যদি(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }

        /* use the stack pivot buffer */
        myPivotSource=myPivotTarget=pivotStart=pivot_বাফার;
        pivotSource=(char16_t **)&myPivotSource;
        pivotTarget=&myPivotTarget;
        pivotLimit=pivot_বাফার+CHUNK_আকার;
    } অন্যথা যদি(  pivotStart>=pivotLimit ||
                pivotSource==nullptr || *pivotSource==nullptr ||
                pivotTarget==nullptr || *pivotTarget==nullptr ||
                pivotLimit==nullptr
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    যদি(sourceLimit==nullptr) {
        /* get limit of single-byte-NUL-terminated source string */
        sourceLimit=uprv_strchr(*source, 0);
    }

    যদি(reset) {
        ucnv_resetToUnicode(sourceCnv);
        ucnv_resetFromUnicode(targetCnv);
        *pivotSource=*pivotTarget=pivotStart;
    } অন্যথা যদি(targetCnv->char_Error_বাফার_দৈর্ঘ্য>0) {
        /* output the targetCnv overflow buffer */
        যদি(ucnv_outputOverflowFromUnicode(targetCnv, target, targetLimit, nullptr, pErrorCode)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            ফেরত;
        }
        /* *target has moved, therefore stop using t */

        যদি( !flush &&
            targetCnv->preFromULength>=0 && *pivotSource==*pivotTarget &&
            sourceCnv->UChar_Error_বাফার_দৈর্ঘ্য==0 && sourceCnv->preToULength>=0 && s==sourceLimit
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            ফেরত;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    যদি( sourceCnv->shared_তথ্য->static_তথ্য->conversionType==UCNV_UTF8 &&
        targetCnv->shared_তথ্য->impl->fromUTF8!=nullptr
    ) {
        convert=targetCnv->shared_তথ্য->impl->fromUTF8;
    } অন্যথা যদি( targetCnv->shared_তথ্য->static_তথ্য->conversionType==UCNV_UTF8 &&
               sourceCnv->shared_তথ্য->impl->toUTF8!=nullptr
    ) {
        convert=sourceCnv->shared_তথ্য->impl->toUTF8;
    } অন্যথা {
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
    যদি(convert!=nullptr && (pivotLimit-pivotStart)>32) {
        pivotLimit=pivotStart+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=targetCnv;
    fromUArgs.flush=মিথ্যা;
    fromUArgs.offsets=nullptr;
    fromUArgs.target=*target;
    fromUArgs.targetLimit=targetLimit;
    fromUArgs.size=আকার(fromUArgs);

    toUArgs.converter=sourceCnv;
    toUArgs.flush=flush;
    toUArgs.offsets=nullptr;
    toUArgs.source=s;
    toUArgs.sourceLimit=sourceLimit;
    toUArgs.targetLimit=pivotLimit;
    toUArgs.size=আকার(toUArgs);

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
    জন্য(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        যদি( *pivotSource<*pivotTarget ||
            U_FAILURE(*pErrorCode) ||
            targetCnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.source=*pivotSource;
            fromUArgs.sourceLimit=*pivotTarget;
            _fromUnicodeWithCallback(&fromUArgs, pErrorCode);
            যদি(U_FAILURE(*pErrorCode)) {
                /* target overflow, or conversion error */
                *pivotSource=(char16_t *)fromUArgs.source;
                বিরতি;
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
        যদি(sourceCnv->UChar_Error_বাফার_দৈর্ঘ্য>0) {
            যদি(ucnv_outputOverflowToUnicode(sourceCnv, pivotTarget, pivotLimit, nullptr, pErrorCode)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *pErrorCode=U_ZERO_ERROR;
            }
            চালু;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        যদি( toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 && sourceCnv->toULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            বিরতি;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        যদি(convert!=nullptr && targetCnv->preFromUFirstCP<0 && sourceCnv->preToULength==0) {
            যদি(*pErrorCode==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *pErrorCode=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &toUArgs, pErrorCode);
            যদি(*pErrorCode==U_বাফার_OVERFLOW_ERROR) {
                বিরতি;
            } অন্যথা যদি(U_FAILURE(*pErrorCode)) {
                যদি(sourceCnv->toULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } অন্যথা {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *pivotSource=*pivotTarget=pivotStart+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    চালু;
                }
            } অন্যথা যদি(*pErrorCode==U_USING_DEFAULT_WARNING) {
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
            } অন্যথা যদি(flush && sourceCnv->toULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *pErrorCode=U_TRUNCATED_CHAR_FOUND;
            } অন্যথা {
                /* input consumed */
                যদি(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(sourceCnv, UCNV_RESET_TO_UNICODE, মিথ্যা);
                    _reset(targetCnv, UCNV_RESET_FROM_UNICODE, মিথ্যা);
                }

                /* done successfully */
                বিরতি;
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
        যদি(*pErrorCode==U_বাফার_OVERFLOW_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *pErrorCode=U_ZERO_ERROR;
        } অন্যথা যদি(U_FAILURE(*pErrorCode) || (!flush && *pivotTarget==pivotStart)) {
            /* conversion error, or there was nothing left to convert */
            বিরতি;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        যদি( flush && toUArgs.source==sourceLimit &&
            sourceCnv->preToULength>=0 &&
            sourceCnv->UChar_Error_বাফার_দৈর্ঘ্য==0
        ) {
            fromUArgs.flush=সত্য;
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
    যদি(flush && U_SUCCESS(*pErrorCode)) {
        যদি(*target!=targetLimit) {
            **target=0;
            যদি(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {
                *pErrorCode=U_ZERO_ERROR;
            }
        } অন্যথা {
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
স্থির int32_t
ucnv_internalConvert(UConverter *outConverter, UConverter *inConverter,
                     অক্ষর *target, int32_t targetCapacity,
                     ধ্রুবক অক্ষর *source, int32_t source_দৈর্ঘ্য,
                     UErrorCode *pErrorCode) {
    char16_t pivot_বাফার[CHUNK_আকার];
    char16_t *pivot, *pivot2;

    অক্ষর *myTarget;
    ধ্রুবক অক্ষর *sourceLimit;
    ধ্রুবক অক্ষর *targetLimit;
    int32_t target_দৈর্ঘ্য=0;

    /* set up */
    যদি(source_দৈর্ঘ্য<0) {
        sourceLimit=uprv_strchr(source, 0);
    } অন্যথা {
        sourceLimit=source+source_দৈর্ঘ্য;
    }

    /* if there is no input data, we're done */
    যদি(source==sourceLimit) {
        ফেরত u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    pivot=pivot2=pivot_বাফার;
    myTarget=target;
    target_দৈর্ঘ্য=0;

    যদি(targetCapacity>0) {
        /* perform real conversion */
        targetLimit=target+targetCapacity;
        ucnv_convertEx(outConverter, inConverter,
                       &myTarget, targetLimit,
                       &source, sourceLimit,
                       pivot_বাফার, &pivot, &pivot2, pivot_বাফার+CHUNK_আকার,
                       মিথ্যা,
                       সত্য,
                       pErrorCode);
        target_দৈর্ঘ্য = static_cast<int32_t>(myTarget - target);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    যদি(*pErrorCode==U_বাফার_OVERFLOW_ERROR || targetCapacity==0)
    {
        অক্ষর target_বাফার[CHUNK_আকার];

        targetLimit=target_বাফার+CHUNK_আকার;
        করো {
            *pErrorCode=U_ZERO_ERROR;
            myTarget=target_বাফার;
            ucnv_convertEx(outConverter, inConverter,
                           &myTarget, targetLimit,
                           &source, sourceLimit,
                           pivot_বাফার, &pivot, &pivot2, pivot_বাফার+CHUNK_আকার,
                           মিথ্যা,
                           সত্য,
                           pErrorCode);
            target_দৈর্ঘ্য += static_cast<int32_t>(myTarget - target_বাফার);
        } যখন(*pErrorCode==U_বাফার_OVERFLOW_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        ফেরত u_terminateChars(target, targetCapacity, target_দৈর্ঘ্য, pErrorCode);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    ফেরত target_দৈর্ঘ্য;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ধ্রুবক অক্ষর *toConverterName, ধ্রুবক অক্ষর *fromConverterName,
             অক্ষর *target, int32_t targetCapacity,
             ধ্রুবক অক্ষর *source, int32_t source_দৈর্ঘ্য,
             UErrorCode *pErrorCode) {
    UConverter in, out; /* stack-allocated */
    UConverter *inConverter, *outConverter;
    int32_t target_দৈর্ঘ্য;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি( source==nullptr || source_দৈর্ঘ্য<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* if there is no input data, we're done */
    যদি(source_দৈর্ঘ্য==0 || (source_দৈর্ঘ্য<0 && *source==0)) {
        ফেরত u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the converters */
    inConverter=ucnv_createConverter(&in, fromConverterName, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    outConverter=ucnv_createConverter(&out, toConverterName, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ucnv_close(inConverter);
        ফেরত 0;
    }

    target_দৈর্ঘ্য=ucnv_internalConvert(outConverter, inConverter,
                                      target, targetCapacity,
                                      source, source_দৈর্ঘ্য,
                                      pErrorCode);

    ucnv_close(inConverter);
    ucnv_close(outConverter);

    ফেরত target_দৈর্ঘ্য;
}

/* @internal */
স্থির int32_t
ucnv_convertAlgorithmic(UBool convertToAlgorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        অক্ষর *target, int32_t targetCapacity,
                        ধ্রুবক অক্ষর *source, int32_t source_দৈর্ঘ্য,
                        UErrorCode *pErrorCode) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *to, *from;
    int32_t target_দৈর্ঘ্য;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি( cnv==nullptr || source==nullptr || source_দৈর্ঘ্য<-1 ||
        targetCapacity<0 || (targetCapacity>0 && target==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* if there is no input data, we're done */
    যদি(source_দৈর্ঘ্য==0 || (source_দৈর্ঘ্য<0 && *source==0)) {
        ফেরত u_terminateChars(target, targetCapacity, 0, pErrorCode);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* reset the other converter */
    যদি(convertToAlgorithmic) {
        /* cnv->Unicode->algo */
        ucnv_resetToUnicode(cnv);
        to=algoConverter;
        from=cnv;
    } অন্যথা {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        to=cnv;
    }

    target_দৈর্ঘ্য=ucnv_internalConvert(to, from,
                                      target, targetCapacity,
                                      source, source_দৈর্ঘ্য,
                                      pErrorCode);

    ucnv_close(algoConverter);

    ফেরত target_দৈর্ঘ্য;
}

U_CAPI int32_t U_EXPORT2
ucnv_toAlgorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   অক্ষর *target, int32_t targetCapacity,
                   ধ্রুবক অক্ষর *source, int32_t source_দৈর্ঘ্য,
                   UErrorCode *pErrorCode) {
    ফেরত ucnv_convertAlgorithmic(সত্য, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, source_দৈর্ঘ্য,
                                   pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     অক্ষর *target, int32_t targetCapacity,
                     ধ্রুবক অক্ষর *source, int32_t source_দৈর্ঘ্য,
                     UErrorCode *pErrorCode) UPRV_NO_SANITIZE_UNDEFINED {

    যদি(algorithmicType<0 || UCNV_সংখ্যা_OF_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    ফেরত ucnv_convertAlgorithmic(মিথ্যা, algorithmicType, cnv,
                                   target, targetCapacity,
                                   source, source_দৈর্ঘ্য,
                                   pErrorCode);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ধ্রুবক UConverter* converter)
{
    int8_t type = converter->shared_তথ্য->static_তথ্য->conversionType;
#যদি !UCONFIG_NO_LEGACY_CONVERSION
    যদি(type == UCNV_MBCS) {
        ফেরত ucnv_MBCSGetType(converter);
    }
#endif
    ফেরত (UConverterType)type;
}

U_CAPI শূন্য  U_EXPORT2
ucnv_getStarters(ধ্রুবক UConverter* converter, 
                 UBool starters[256],
                 UErrorCode* err)
{
    যদি (err == nullptr || U_FAILURE(*err)) {
        ফেরত;
    }

    যদি(converter->shared_তথ্য->impl->getStarters != nullptr) {
        converter->shared_তথ্য->impl->getStarters(converter, starters, err);
    } অন্যথা {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

স্থির ধ্রুবক UAmbiguousConverter *ucnv_getAmbiguous(ধ্রুবক UConverter *cnv)
{
    UErrorCode errorCode;
    ধ্রুবক অক্ষর *name;
    int32_t i;

    যদি(cnv==nullptr) {
        ফেরত nullptr;
    }

    errorCode=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }

    জন্য(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        যদি(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            ফেরত ambiguousConverters+i;
        }
    }

    ফেরত nullptr;
}

U_CAPI শূন্য  U_EXPORT2
ucnv_fixFileSeparator(ধ্রুবক UConverter *cnv, 
                      char16_t* source,
                      int32_t source_দৈর্ঘ্য) {
    ধ্রুবক UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    যদি(cnv==nullptr || source==nullptr || source_দৈর্ঘ্য<=0 || (a=ucnv_getAmbiguous(cnv))==nullptr)
    {
        ফেরত;
    }

    variant5c=a->variant5c;
    জন্য(i=0; i<source_দৈর্ঘ্য; ++i) {
        যদি(source[i]==variant5c) {
            source[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ধ্রুবক UConverter *cnv) {
    ফেরত ucnv_getAmbiguous(cnv)!=nullptr;
}

U_CAPI শূন্য  U_EXPORT2
ucnv_setFallback(UConverter *cnv, UBool usesFallback)
{
    cnv->useFallback = usesFallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_usesFallback(ধ্রুবক UConverter *cnv)
{
    ফেরত cnv->useFallback;
}

U_CAPI শূন্য  U_EXPORT2
ucnv_getInvalidChars (ধ্রুবক UConverter * converter,
                      অক্ষর *errBytes,
                      int8_t * len,
                      UErrorCode * err)
{
    যদি (err == nullptr || U_FAILURE(*err))
    {
        ফেরত;
    }
    যদি (len == nullptr || errBytes == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    যদি (*len < converter->invalid_Char_দৈর্ঘ্য)
    {
        *err = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }
    যদি ((*len = converter->invalid_Char_দৈর্ঘ্য) > 0)
    {
        uprv_memcpy (errBytes, converter->invalid_Char_বাফার, *len);
    }
}

U_CAPI শূন্য  U_EXPORT2
ucnv_getInvalidUChars (ধ্রুবক UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UErrorCode * err)
{
    যদি (err == nullptr || U_FAILURE(*err))
    {
        ফেরত;
    }
    যদি (len == nullptr || errChars == nullptr || converter == nullptr)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    যদি (*len < converter->invalid_UChar_দৈর্ঘ্য)
    {
        *err = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }
    যদি ((*len = converter->invalid_UChar_দৈর্ঘ্য) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_বাফার, *len);
    }
}

#define SIG_MAX_LEN 5

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ucnv_detectUnicodeSignature( ধ্রুবক অক্ষর* source,
                             int32_t source_দৈর্ঘ্য,
                             int32_t* signature_দৈর্ঘ্য,
                             UErrorCode* pErrorCode) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    অক্ষর start[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    পূর্ণসংখ্যা i = 0;

    যদি((pErrorCode==nullptr) || U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }
    
    যদি(source == nullptr || source_দৈর্ঘ্য < -1){
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    যদি(signature_দৈর্ঘ্য == nullptr) {
        signature_দৈর্ঘ্য = &dummy;
    }

    যদি(source_দৈর্ঘ্য==-1){
        source_দৈর্ঘ্য=(int32_t)uprv_strlen(source);
    }

    
    যখন(i<source_দৈর্ঘ্য&& i<SIG_MAX_LEN){
        start[i]=source[i];
        i++;
    }

    যদি(start[0] == '\xFE' && start[1] == '\xFF') {
        *signature_দৈর্ঘ্য=2;
        ফেরত  "UTF-16BE";
    } অন্যথা যদি(start[0] == '\xFF' && start[1] == '\xFE') {
        যদি(start[2] == '\x00' && start[3] =='\x00') {
            *signature_দৈর্ঘ্য=4;
            ফেরত "UTF-32LE";
        } অন্যথা {
            *signature_দৈর্ঘ্য=2;
            ফেরত  "UTF-16LE";
        }
    } অন্যথা যদি(start[0] == '\xEF' && start[1] == '\xBB' && start[2] == '\xBF') {
        *signature_দৈর্ঘ্য=3;
        ফেরত  "UTF-8";
    } অন্যথা যদি(start[0] == '\x00' && start[1] == '\x00' && 
              start[2] == '\xFE' && start[3]=='\xFF') {
        *signature_দৈর্ঘ্য=4;
        ফেরত  "UTF-32BE";
    } অন্যথা যদি(start[0] == '\x0E' && start[1] == '\xFE' && start[2] == '\xFF') {
        *signature_দৈর্ঘ্য=3;
        ফেরত "SCSU";
    } অন্যথা যদি(start[0] == '\xFB' && start[1] == '\xEE' && start[2] == '\x28') {
        *signature_দৈর্ঘ্য=3;
        ফেরত "BOCU-1";
    } অন্যথা যদি(start[0] == '\x2B' && start[1] == '\x2F' && start[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        যদি(start[3] == '\x38' && start[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_দৈর্ঘ্য=5;
            ফেরত "UTF-7";
        } অন্যথা যদি(start[3] == '\x38' || start[3] == '\x39' || start[3] == '\x2B' || start[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_দৈর্ঘ্য=4;
            ফেরত "UTF-7";
        }
    }অন্যথা যদি(start[0]=='\xDD' && start[1]== '\x73'&& start[2]=='\x66' && start[3]=='\x73'){
        *signature_দৈর্ঘ্য=4;
        ফেরত "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_দৈর্ঘ্য=0;
    ফেরত nullptr;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ধ্রুবক UConverter* cnv, UErrorCode* status)
{
    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত -1;
    }
    যদি(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত -1;
    }

    যদি(cnv->preFromUFirstCP >= 0){
        ফেরত U16_দৈর্ঘ্য(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }অন্যথা যদি(cnv->preFromULength < 0){
        ফেরত -cnv->preFromULength ;
    }অন্যথা যদি(cnv->fromUChar32 > 0){
        ফেরত 1;
    }
    ফেরত 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_toUCountPending(ধ্রুবক UConverter* cnv, UErrorCode* status){

    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত -1;
    }
    যদি(cnv == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত -1;
    }

    যদি(cnv->preToULength > 0){
        ফেরত cnv->preToULength ;
    }অন্যথা যদি(cnv->preToULength < 0){
        ফেরত -cnv->preToULength;
    }অন্যথা যদি(cnv->toULength > 0){
        ফেরত cnv->toULength;
    }
    ফেরত 0;
}

U_CAPI UBool U_EXPORT2
ucnv_isFixedWidth(UConverter *cnv, UErrorCode *status){
    যদি (U_FAILURE(*status)) {
        ফেরত মিথ্যা;
    }

    যদি (cnv == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত মিথ্যা;
    }

    নির্বাচন (ucnv_getType(cnv)) {
        ক্ষেত্র UCNV_SBCS:
        ক্ষেত্র UCNV_DBCS:
        ক্ষেত্র UCNV_UTF32_BigEndian:
        ক্ষেত্র UCNV_UTF32_LittleEndian:
        ক্ষেত্র UCNV_UTF32:
        ক্ষেত্র UCNV_US_ASCII:
            ফেরত সত্য;
        পূর্বনির্ধারিত:
            ফেরত মিথ্যা;
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
