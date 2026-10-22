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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن <memory>

#ضمن "unicode/ustring.h"
#ضمن "unicode/ucnv.h"
#ضمن "unicode/ucnv_err.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf16.h"
#ضمن "putilimp.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uassert.h"
#ضمن "utracimp.h"
#ضمن "ustr_imp.h"
#ضمن "ucnv_imp.h"
#ضمن "ucnv_cnv.h"
#ضمن "ucnv_bld.h"

/* size of intermediate and preflighting buffers in ucnv_convert() */
#تعریف_کن CHUNK_اندازه 1024

تعريفنوع بنية UAmbiguousConverter {
    ثابت حرف *name;
    ثابت char16_t variant5c;
} UAmbiguousConverter;

ساكن ثابت UAmbiguousConverter ambiguousConverters[]={
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
ucnv_open (ثابت حرف *name,
                       UError_کد * err)
{
    UConverter *r;

    إذا (err == اشاره_تهی || U_FAILURE (*err)) {
        أرجع اشاره_تهی;
    }

    r =  ucnv_createConverter(اشاره_تهی, name, err);
    أرجع r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ثابت حرف *packageName, ثابت حرف *converterName, UError_کد * err)
{
    أرجع ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ثابت char16_t * name,
                         UError_کد * err)
{
    حرف asciiName[UCNV_MAX_CONVERTER_NAME_درازی];

    إذا (err == اشاره_تهی || U_FAILURE(*err))
        أرجع اشاره_تهی;
    إذا (name == اشاره_تهی)
        أرجع ucnv_open (اشاره_تهی, err);
    إذا (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_درازی)
    {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    أرجع ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
ساكن int32_t
ucnv_copyPlatformString(حرف *platformString, UConverterPlatform pltfrm)
{
    بدل (pltfrm)
    {
    حالة UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        أرجع 4;
    حالة UCNV_UNKNOWN:
        اكسر;
    }

    /* default to empty string */
    *platformString = 0;
    أرجع 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UError_کد * err)
{
    حرف myName[UCNV_MAX_CONVERTER_NAME_درازی];
    int32_t myNameLen;

    إذا (err == اشاره_تهی || U_FAILURE (*err))
        أرجع اشاره_تهی;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_مقصد_String(myName + myNameLen, codepage, 10);

    أرجع ucnv_createConverter(اشاره_تهی, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_امن_Clone(ثابت UConverter* cnv,
               فراغ* پشته_میانگیر U_LIFETIME_BOUND,
               int32_t* p_میانگیر_اندازه,
               UError_کد* وضعیت)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t پشته_میانگیر_اندازه;
    int32_t میانگیر_اندازه_Needed;
    UError_کد cbErr;
    UConverter_مقصد_Unicode_آرگومانها مقصد_UArgs = {
        حجم(UConverter_مقصد_Unicode_آرگومانها),
            درست,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی
    };
    UConverter_From_Unicode_آرگومانها fromUArgs = {
        حجم(UConverter_From_Unicode_آرگومانها),
            درست,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی,
            اشاره_تهی
    };

    UTRACE_مدخل_OC(UTRACE_UCNV_CLONE);

    إذا (وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)){
        UTRACE_EXIT_وضعیت(وضعیت? *وضعیت: U_ILLEGAL_آرگومان_ERROR);
        أرجع اشاره_تهی;
    }

    إذا (cnv == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        UTRACE_EXIT_وضعیت(*وضعیت);
        أرجع اشاره_تهی;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, وضعیت), cnv, پشته_میانگیر);

    إذا (cnv->shared_داده->impl->امن_Clone != اشاره_تهی) {
        /* call the custom safeClone function for sizing */
        میانگیر_اندازه_Needed = 0;
        cnv->shared_داده->impl->امن_Clone(cnv, اشاره_تهی, &میانگیر_اندازه_Needed, وضعیت);
        إذا (U_FAILURE(*وضعیت)) {
            UTRACE_EXIT_وضعیت(*وضعیت);
            أرجع اشاره_تهی;
        }
    }
    وإلا
    {
        /* inherent sizing */
        میانگیر_اندازه_Needed = حجم(UConverter);
    }

    إذا (p_میانگیر_اندازه == اشاره_تهی) {
        پشته_میانگیر_اندازه = 1;
        p_میانگیر_اندازه = &پشته_میانگیر_اندازه;
    } وإلا {
        پشته_میانگیر_اندازه = *p_میانگیر_اندازه;
        إذا (پشته_میانگیر_اندازه <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_میانگیر_اندازه = میانگیر_اندازه_Needed;
            UTRACE_EXIT_مقدار(میانگیر_اندازه_Needed);
            أرجع اشاره_تهی;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    إذا (پشته_میانگیر) {
        uintptr_t p = reinterpret_cast<uintptr_t>(پشته_میانگیر);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t اشاره_گر_Adjustment = aligned_p - p;
        إذا (میانگیر_اندازه_Needed + اشاره_گر_Adjustment <= پشته_میانگیر_اندازه) {
            پشته_میانگیر = reinterpret_cast<فراغ *>(aligned_p);
            پشته_میانگیر_اندازه -= static_cast<int32_t>(اشاره_گر_Adjustment);
        } وإلا {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            پشته_میانگیر_اندازه = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    إذا (پشته_میانگیر_اندازه < میانگیر_اندازه_Needed || پشته_میانگیر == اشاره_تهی)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (میانگیر_اندازه_Needed);

        إذا(localConverter == اشاره_تهی) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
            UTRACE_EXIT_وضعیت(*وضعیت);
            أرجع اشاره_تهی;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        إذا (p_میانگیر_اندازه != &پشته_میانگیر_اندازه) {
            *وضعیت = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_میانگیر_اندازه = میانگیر_اندازه_Needed;
    } وإلا {
        /* just use the stack buffer */
        localConverter = (UConverter*) پشته_میانگیر;
        allocatedConverter = اشاره_تهی;
    }

    uprv_memset(localConverter, 0, میانگیر_اندازه_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, حجم(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = خطأ;

    /* copy the substitution string */
    إذا (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } وإلا {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_میانگیر_درازی * U_SIZEOF_UCHAR);
        إذا (localConverter->subChars == اشاره_تهی) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_وضعیت(*وضعیت);
            أرجع اشاره_تهی;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_میانگیر_درازی * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    إذا (cnv->shared_داده->impl->امن_Clone != اشاره_تهی) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_داده->impl->امن_Clone(cnv, localConverter, p_میانگیر_اندازه, وضعیت);
    }

    إذا(localConverter==اشاره_تهی || U_FAILURE(*وضعیت)) {
        إذا (allocatedConverter != اشاره_تهی && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_وضعیت(*وضعیت);
        أرجع اشاره_تهی;
    }

    /* increment refcount of shared data if needed */
    إذا (cnv->shared_داده->isReferenceCounted) {
        ucnv_increment_Ref_شمار(cnv->shared_داده);
    }

    إذا(localConverter == (UConverter*)پشته_میانگیر) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = درست;
    }

    /* allow callback functions to handle any memory allocation */
    مقصد_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->مقصد_UContext, &مقصد_UArgs, اشاره_تهی, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, اشاره_تهی, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_وضعیت(localConverter, *وضعیت);
    أرجع localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ثابت UConverter* cnv, UError_کد *وضعیت)
{
    أرجع ucnv_امن_Clone(cnv, اشاره_تهی, اشاره_تهی, وضعیت);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI فراغ  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_کد error_کد = U_ZERO_ERROR;

    UTRACE_مدخل_OC(UTRACE_UCNV_CLOSE);

    إذا (converter == اشاره_تهی)
    {
        UTRACE_EXIT();
        أرجع;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_کد), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    إذا (converter->fromCharErrorBehaviour != UCNV_مقصد_U_DEFAULT_CALLBACK) {
        UConverter_مقصد_Unicode_آرگومانها مقصد_UArgs = {
            حجم(UConverter_مقصد_Unicode_آرگومانها),
                درست,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی
        };

        مقصد_UArgs.converter = converter;
        error_کد = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->مقصد_UContext, &مقصد_UArgs, اشاره_تهی, 0, UCNV_CLOSE, &error_کد);
    }
    إذا (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_آرگومانها fromUArgs = {
            حجم(UConverter_From_Unicode_آرگومانها),
                درست,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی
        };
        fromUArgs.converter = converter;
        error_کد = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, اشاره_تهی, 0, 0, UCNV_CLOSE, &error_کد);
    }

    إذا (converter->shared_داده->impl->close != اشاره_تهی) {
        converter->shared_داده->impl->close(converter);
    }

    إذا (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    إذا (converter->shared_داده->isReferenceCounted) {
        ucnv_unload_Shared_داده_If_Ready(converter->shared_داده);
    }

    إذا(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ثابت حرف*   U_EXPORT2
ucnv_get_موجود_Name (int32_t n)
{
    إذا (0 <= n && n <= 0xffff) {
        UError_کد err = U_ZERO_ERROR;
        ثابت حرف *name = ucnv_bld_get_موجود_Converter((uint16_t)n, &err);
        إذا (U_SUCCESS(err)) {
            أرجع name;
        }
    }
    أرجع اشاره_تهی;
}

U_CAPI int32_t   U_EXPORT2
ucnv_شمار_موجود ()
{
    UError_کد err = U_ZERO_ERROR;
    أرجع ucnv_bld_شمار_موجود_Converters(&err);
}

U_CAPI فراغ    U_EXPORT2
ucnv_getSubstChars (ثابت UConverter * converter,
                    حرف *mySubChar,
                    int8_t * len,
                    UError_کد * err)
{
    إذا (U_FAILURE (*err))
        أرجع;

    إذا (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        أرجع;
    }

    إذا (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI فراغ    U_EXPORT2
ucnv_مجموعه_Subst_Chars (UConverter * converter,
                    ثابت حرف *mySubChar,
                    int8_t len,
                    UError_کد * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    إذا ((len > converter->shared_داده->static_داده->max_بایتها_در_هر_Char)
     || (len < converter->shared_داده->static_داده->min_بایتها_در_هر_Char))
    {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
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

U_CAPI فراغ U_EXPORT2
ucnv_مجموعه_Subst_String(UConverter *cnv,
                    ثابت char16_t *s,
                    int32_t length,
                    UError_کد *err) {
    alignas(UConverter) حرف clone_میانگیر[U_CNV_SAFECLONE_BUFFERSIZE];
    حرف chars[UCNV_ERROR_میانگیر_درازی];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_اندازه, length8;

    /* Let the following functions check all arguments. */
    clone_اندازه = حجم(clone_میانگیر);
    clone = ucnv_امن_Clone(cnv, clone_میانگیر, &clone_اندازه, err);
    ucnv_مجموعه_From_UCall_پشت(clone, UCNV_FROM_U_CALLBACK_STOP, اشاره_تهی, اشاره_تهی, اشاره_تهی, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)حجم(chars), s, length, err);
    ucnv_close(clone);
    إذا (U_FAILURE(*err)) {
        أرجع;
    }

    إذا (cnv->shared_داده->impl->writeSub == اشاره_تهی
#إذا !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_داده->static_داده->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#پایان_اگر
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } وإلا {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        إذا (length > UCNV_ERROR_میانگیر_درازی) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_میانگیر_سرریز_ERROR;
            أرجع;
        }
        subChars = (uint8_t *)s;
        إذا (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    إذا (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        إذا (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_میانگیر_درازی * U_SIZEOF_UCHAR);
            إذا (cnv->subChars == اشاره_تهی) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_میانگیر_درازی * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    إذا (length8 == 0) {
        cnv->subCharLen = 0;
    } وإلا {
        uprv_memcpy(cnv->subChars, subChars, length8);
        إذا (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } وإلا /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
ساكن فراغ _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    إذا(converter == اشاره_تهی) {
        أرجع;
    }

    إذا(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_کد error_کد;

        إذا(choice<=UCNV_RESET_مقصد_UNICODE && converter->fromCharErrorBehaviour != UCNV_مقصد_U_DEFAULT_CALLBACK) {
            UConverter_مقصد_Unicode_آرگومانها مقصد_UArgs = {
                حجم(UConverter_مقصد_Unicode_آرگومانها),
                درست,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی
            };
            مقصد_UArgs.converter = converter;
            error_کد = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->مقصد_UContext, &مقصد_UArgs, اشاره_تهی, 0, UCNV_RESET, &error_کد);
        }
        إذا(choice!=UCNV_RESET_مقصد_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_آرگومانها fromUArgs = {
                حجم(UConverter_From_Unicode_آرگومانها),
                درست,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی,
                اشاره_تهی
            };
            fromUArgs.converter = converter;
            error_کد = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, اشاره_تهی, 0, 0, UCNV_RESET, &error_کد);
        }
    }

    /* now reset the converter itself */
    إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
        converter->مقصد_Unicode_وضعیت = converter->shared_داده->مقصد_Unicode_وضعیت;
        converter->نما = 0;
        converter->مقصد_ULength = 0;
        converter->invalid_Char_درازی = converter->UChar_Error_میانگیر_درازی = 0;
        converter->pre_مقصد_ULength = 0;
    }
    إذا(choice!=UCNV_RESET_مقصد_UNICODE) {
        converter->from_Unicode_وضعیت = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_درازی = converter->char_Error_میانگیر_درازی = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    إذا (converter->shared_داده->impl->reset != اشاره_تهی) {
        /* call the custom reset function */
        converter->shared_داده->impl->reset(converter, choice);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, درست);
}

U_CAPI فراغ  U_EXPORT2
ucnv_reset_مقصد_Unicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_مقصد_UNICODE, درست);
}

U_CAPI فراغ  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, درست);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_اندازه (ثابت UConverter * converter)
{
    أرجع converter->max_بایتها_در_هر_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_اندازه (ثابت UConverter * converter)
{
    أرجع converter->shared_داده->static_داده->min_بایتها_در_هر_Char;
}

U_CAPI ثابت حرف* U_EXPORT2
ucnv_getName(ثابت UConverter* converter U_LIFETIME_BOUND, UError_کد* err)
{
    إذا (U_FAILURE (*err))
        أرجع اشاره_تهی;
    إذا(converter->shared_داده->impl->getName){
        ثابت حرف* موقت= converter->shared_داده->impl->getName(converter);
        إذا(موقت)
            أرجع موقت;
    }
    أرجع converter->shared_داده->static_داده->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ثابت UConverter * converter,
              UError_کد * err)
{
    int32_t ccsid;
    إذا (U_FAILURE (*err))
        أرجع -1;

    ccsid = converter->shared_داده->static_داده->codepage;
    إذا (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        ثابت حرف *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        إذا (U_SUCCESS(*err) && standardName) {
            ثابت حرف *ccsidStr = uprv_strchr(standardName, '-');
            إذا (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    أرجع ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (ثابت UConverter * converter,
                                      UError_کد * err)
{
    إذا (U_FAILURE (*err))
        أرجع UCNV_UNKNOWN;

    أرجع (UConverterPlatform)converter->shared_داده->static_داده->platform;
}

U_CAPI فراغ U_EXPORT2
    ucnv_get_مقصد_UCall_پشت (ثابت UConverter * converter,
                         UConverter_مقصد_UCallback *action,
                         ثابت فراغ **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->مقصد_UContext;
}

U_CAPI فراغ U_EXPORT2
    ucnv_get_From_UCall_پشت (ثابت UConverter * converter,
                           UConverterFromUCallback *action,
                           ثابت فراغ **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI فراغ    U_EXPORT2
ucnv_مجموعه_مقصد_UCall_پشت (UConverter * converter,
                            UConverter_مقصد_UCallback newAction,
                            ثابت فراغ* newContext,
                            UConverter_مقصد_UCallback *oldAction,
                            ثابت فراغ** oldContext,
                            UError_کد * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    إذا (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    إذا (oldContext) *oldContext = converter->مقصد_UContext;
    converter->مقصد_UContext = newContext;
}

U_CAPI فراغ  U_EXPORT2
ucnv_مجموعه_From_UCall_پشت (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ثابت فراغ* newContext,
                            UConverterFromUCallback *oldAction,
                            ثابت فراغ** oldContext,
                            UError_کد * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    إذا (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    إذا (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

ساكن فراغ
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t منبع_شاخص, int32_t error_درون_داد_درازی) {
    int32_t *حد;
    int32_t delta, جابه_جایی;

    إذا(منبع_شاخص>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=منبع_شاخص-error_درون_داد_درازی;
    } وإلا {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    حد=offsets+length;
    إذا(delta==0) {
        /* most common case, nothing to do */
    } وإلا إذا(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        طالما(offsets<حد) {
            جابه_جایی=*offsets;
            إذا(جابه_جایی>=0) {
                *offsets=جابه_جایی+delta;
            }
            ++offsets;
        }
    } وإلا /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        طالما(offsets<حد) {
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

ساكن فراغ
_fromUnicodeWithCallback(UConverter_From_Unicode_آرگومانها *p_آرگومانها, UError_کد *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ثابت char16_t *s;
    حرف *t;
    int32_t *offsets;
    int32_t منبع_شاخص;
    int32_t error_درون_داد_درازی;
    UBool converter_Saw_انتها_از_درون_داد, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ثابت char16_t *real_منبع, *real_منبع_حد;
    int32_t real_منبع_شاخص;
    UBool realFlush;

    cnv=p_آرگومانها->converter;
    s=p_آرگومانها->منبع;
    t=p_آرگومانها->هدف;
    offsets=p_آرگومانها->offsets;

    /* get the converter implementation function */
    منبع_شاخص=0;
    إذا(offsets==اشاره_تهی) {
        fromUnicode=cnv->shared_داده->impl->fromUnicode;
    } وإلا {
        fromUnicode=cnv->shared_داده->impl->fromUnicodeWithOffsets;
        إذا(fromUnicode==اشاره_تهی) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_داده->impl->fromUnicode;
            /* we will write -1 for each offset */
            منبع_شاخص=-1;
        }
    }

    إذا(cnv->preFromULength>=0) {
        /* normal mode */
        real_منبع=اشاره_تهی;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_منبع_حد=اشاره_تهی;
        realFlush=خطأ;
        real_منبع_شاخص=0;
    } وإلا {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        إذا (!p_آرگومانها->منبع) {
            *err=U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }
        real_منبع=p_آرگومانها->منبع;
        real_منبع_حد=p_آرگومانها->منبع_حد;
        realFlush=p_آرگومانها->flush;
        real_منبع_شاخص=منبع_شاخص;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_آرگومانها->منبع=replay;
        p_آرگومانها->منبع_حد=replay-cnv->preFromULength;
        p_آرگومانها->flush=خطأ;
        منبع_شاخص=-1;

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
    لكل(;;) {
        إذا(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(p_آرگومانها, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_انتها_از_درون_داد=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_آرگومانها->flush && p_آرگومانها->منبع==p_آرگومانها->منبع_حد &&
                        cnv->fromUChar32==0);
        } وإلا {
            /* handle error from ucnv_convertEx() */
            converter_Saw_انتها_از_درون_داد=خطأ;
        }

        /* no callback called yet for this iteration */
        calledCallback=خطأ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_درون_داد_درازی=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        لكل(;;) {
            /* update offsets if we write any */
            إذا(offsets!=اشاره_تهی) {
                int32_t length = static_cast<int32_t>(p_آرگومانها->هدف - t);
                إذا(length>0) {
                    _updateOffsets(offsets, length, منبع_شاخص, error_درون_داد_درازی);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_آرگومانها->offsets=offsets+=length;
                }

                إذا(منبع_شاخص>=0) {
                    منبع_شاخص += static_cast<int32_t>(p_آرگومانها->منبع - s);
                }
            }

            إذا(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                إذا(real_منبع==اشاره_تهی) {
                    real_منبع=p_آرگومانها->منبع;
                    real_منبع_حد=p_آرگومانها->منبع_حد;
                    realFlush=p_آرگومانها->flush;
                    real_منبع_شاخص=منبع_شاخص;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_آرگومانها->منبع=replay;
                    p_آرگومانها->منبع_حد=replay-cnv->preFromULength;
                    p_آرگومانها->flush=خطأ;
                    إذا((منبع_شاخص+=cnv->preFromULength)<0) {
                        منبع_شاخص=-1;
                    }

                    cnv->preFromULength=0;
                } وإلا {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_منبع==اشاره_تهی);
                    *err=U_داخلی_برنامه_ERROR;
                }
            }

            /* update pointers */
            s=p_آرگومانها->منبع;
            t=p_آرگومانها->هدف;

            إذا(U_SUCCESS(*err)) {
                إذا(s<p_آرگومانها->منبع_حد) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    اكسر;
                } وإلا إذا(real_منبع!=اشاره_تهی) {
                    /* switch back from replaying to the real source and continue */
                    p_آرگومانها->منبع=real_منبع;
                    p_آرگومانها->منبع_حد=real_منبع_حد;
                    p_آرگومانها->flush=realFlush;
                    منبع_شاخص=real_منبع_شاخص;

                    real_منبع=اشاره_تهی;
                    اكسر;
                } وإلا إذا(p_آرگومانها->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_یافت;
                    calledCallback=خطأ; /* new error condition */
                } وإلا {
                    /* input consumed */
                    إذا(p_آرگومانها->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        إذا(!converter_Saw_انتها_از_درون_داد) {
                            اكسر;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, خطأ);
                    }

                    /* done successfully */
                    أرجع;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_کد e;

                إذا( calledCallback ||
                    (e=*err)==U_میانگیر_سرریز_ERROR ||
                    (e!=U_INVALID_CHAR_یافت &&
                     e!=U_ILLEGAL_CHAR_یافت &&
                     e!=U_TRUNCATED_CHAR_یافت)
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
                    إذا(real_منبع!=اشاره_تهی) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_آرگومانها->منبع_حد - p_آرگومانها->منبع);
                        إذا(length>0) {
                            u_memcpy(cnv->preFromU, p_آرگومانها->منبع, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_آرگومانها->منبع=real_منبع;
                        p_آرگومانها->منبع_حد=real_منبع_حد;
                        p_آرگومانها->flush=realFlush;
                    }

                    أرجع;
                }
            }

            /* callback handling */
            {
                UChar32 کد_نقطه;

                /* get and write the code point */
                کد_نقطه=cnv->fromUChar32;
                error_درون_داد_درازی=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_میانگیر, error_درون_داد_درازی, کد_نقطه);
                cnv->invalid_UChar_درازی = static_cast<int8_t>(error_درون_داد_درازی);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_آرگومانها,
                    cnv->invalid_UChar_میانگیر, error_درون_داد_درازی, کد_نقطه,
                    *err==U_INVALID_CHAR_یافت ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
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
ساكن UBool
ucnv_output_سرریز_From_Unicode(UConverter *cnv,
                               حرف **هدف, ثابت حرف *هدف_حد,
                               int32_t **pOffsets,
                               UError_کد *err) {
    int32_t *offsets;
    حرف *سرریز, *t;
    int32_t i, length;

    t=*هدف;
    إذا(pOffsets!=اشاره_تهی) {
        offsets=*pOffsets;
    } وإلا {
        offsets=اشاره_تهی;
    }

    سرریز = reinterpret_cast<حرف*>(cnv->char_Error_میانگیر);
    length=cnv->char_Error_میانگیر_درازی;
    i=0;
    طالما(i<length) {
        إذا(t==هدف_حد) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            افعل {
                سرریز[j++]=سرریز[i++];
            } طالما(i<length);

            cnv->char_Error_میانگیر_درازی = static_cast<int8_t>(j);
            *هدف=t;
            إذا(offsets!=اشاره_تهی) {
                *pOffsets=offsets;
            }
            *err=U_میانگیر_سرریز_ERROR;
            أرجع درست;
        }

        /* copy the overflow contents to the target */
        *t++=سرریز[i++];
        إذا(offsets!=اشاره_تهی) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_میانگیر_درازی=0;
    *هدف=t;
    إذا(offsets!=اشاره_تهی) {
        *pOffsets=offsets;
    }
    أرجع خطأ;
}

U_CAPI فراغ U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 حرف **هدف, ثابت حرف *هدف_حد,
                 ثابت char16_t **منبع, ثابت char16_t *منبع_حد,
                 int32_t *offsets,
                 UBool flush,
                 UError_کد *err) {
    UConverter_From_Unicode_آرگومانها آرگومانها_2;
    ثابت char16_t *s;
    حرف *t;

    /* check parameters */
    إذا(err==اشاره_تهی || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(cnv==اشاره_تهی || هدف==اشاره_تهی || منبع==اشاره_تهی) {
        *err=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    s=*منبع;
    t=*هدف;

    إذا ((ثابت فراغ *)U_MAX_PTR(منبع_حد) == (ثابت فراغ *)منبع_حد) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        منبع_حد = (ثابت char16_t *)(((ثابت حرف *)منبع_حد) - 1);
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
    إذا (منبع_حد<s || هدف_حد<t ||
        ((size_t)(منبع_حد-s)>(size_t)0x3fffffff && منبع_حد>s) ||
        ((size_t)(هدف_حد-t)>(size_t)0x7fffffff && هدف_حد>t) ||
        (((ثابت حرف *)منبع_حد-(ثابت حرف *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    
    /* output the target overflow buffer */
    إذا( cnv->char_Error_میانگیر_درازی>0 &&
        ucnv_output_سرریز_From_Unicode(cnv, هدف, هدف_حد, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        أرجع;
    }
    /* *target may have moved, therefore stop using t */

    إذا(!flush && s==منبع_حد && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        أرجع;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    آرگومانها_2.converter=cnv;
    آرگومانها_2.flush=flush;
    آرگومانها_2.offsets=offsets;
    آرگومانها_2.منبع=s;
    آرگومانها_2.منبع_حد=منبع_حد;
    آرگومانها_2.هدف=*هدف;
    آرگومانها_2.هدف_حد=هدف_حد;
    آرگومانها_2.size=حجم(آرگومانها_2);

    _fromUnicodeWithCallback(&آرگومانها_2, err);

    *منبع=آرگومانها_2.منبع;
    *هدف=آرگومانها_2.هدف;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

ساكن فراغ
مقصد_Unicode_With_Callback(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها, UError_کد *err) {
    UConverter_مقصد_Unicode مقصد_Unicode;
    UConverter *cnv;
    ثابت حرف *s;
    char16_t *t;
    int32_t *offsets;
    int32_t منبع_شاخص;
    int32_t error_درون_داد_درازی;
    UBool converter_Saw_انتها_از_درون_داد, calledCallback;

    /* variables for m:n conversion */
    حرف replay[UCNV_EXT_MAX_بایتها];
    ثابت حرف *real_منبع, *real_منبع_حد;
    int32_t real_منبع_شاخص;
    UBool realFlush;

    cnv=p_آرگومانها->converter;
    s=p_آرگومانها->منبع;
    t=p_آرگومانها->هدف;
    offsets=p_آرگومانها->offsets;

    /* get the converter implementation function */
    منبع_شاخص=0;
    إذا(offsets==اشاره_تهی) {
        مقصد_Unicode=cnv->shared_داده->impl->مقصد_Unicode;
    } وإلا {
        مقصد_Unicode=cnv->shared_داده->impl->مقصد_Unicode_With_Offsets;
        إذا(مقصد_Unicode==اشاره_تهی) {
            /* there is no WithOffsets implementation */
            مقصد_Unicode=cnv->shared_داده->impl->مقصد_Unicode;
            /* we will write -1 for each offset */
            منبع_شاخص=-1;
        }
    }

    إذا(cnv->pre_مقصد_ULength>=0) {
        /* normal mode */
        real_منبع=اشاره_تهی;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_منبع_حد=اشاره_تهی;
        realFlush=خطأ;
        real_منبع_شاخص=0;
    } وإلا {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        إذا (!p_آرگومانها->منبع) {
            *err=U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }
        real_منبع=p_آرگومانها->منبع;
        real_منبع_حد=p_آرگومانها->منبع_حد;
        realFlush=p_آرگومانها->flush;
        real_منبع_شاخص=منبع_شاخص;

        uprv_memcpy(replay, cnv->pre_مقصد_U, -cnv->pre_مقصد_ULength);
        p_آرگومانها->منبع=replay;
        p_آرگومانها->منبع_حد=replay-cnv->pre_مقصد_ULength;
        p_آرگومانها->flush=خطأ;
        منبع_شاخص=-1;

        cnv->pre_مقصد_ULength=0;
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
    لكل(;;) {
        إذا(U_SUCCESS(*err)) {
            /* convert */
            مقصد_Unicode(p_آرگومانها, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_انتها_از_درون_داد=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_آرگومانها->flush && p_آرگومانها->منبع==p_آرگومانها->منبع_حد &&
                        cnv->مقصد_ULength==0);
        } وإلا {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_انتها_از_درون_داد=خطأ;
        }

        /* no callback called yet for this iteration */
        calledCallback=خطأ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_درون_داد_درازی=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        لكل(;;) {
            /* update offsets if we write any */
            إذا(offsets!=اشاره_تهی) {
                int32_t length = static_cast<int32_t>(p_آرگومانها->هدف - t);
                إذا(length>0) {
                    _updateOffsets(offsets, length, منبع_شاخص, error_درون_داد_درازی);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_آرگومانها->offsets=offsets+=length;
                }

                إذا(منبع_شاخص>=0) {
                    منبع_شاخص += static_cast<int32_t>(p_آرگومانها->منبع - s);
                }
            }

            إذا(cnv->pre_مقصد_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                إذا(real_منبع==اشاره_تهی) {
                    real_منبع=p_آرگومانها->منبع;
                    real_منبع_حد=p_آرگومانها->منبع_حد;
                    realFlush=p_آرگومانها->flush;
                    real_منبع_شاخص=منبع_شاخص;

                    uprv_memcpy(replay, cnv->pre_مقصد_U, -cnv->pre_مقصد_ULength);
                    p_آرگومانها->منبع=replay;
                    p_آرگومانها->منبع_حد=replay-cnv->pre_مقصد_ULength;
                    p_آرگومانها->flush=خطأ;
                    إذا((منبع_شاخص+=cnv->pre_مقصد_ULength)<0) {
                        منبع_شاخص=-1;
                    }

                    cnv->pre_مقصد_ULength=0;
                } وإلا {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_منبع==اشاره_تهی);
                    *err=U_داخلی_برنامه_ERROR;
                }
            }

            /* update pointers */
            s=p_آرگومانها->منبع;
            t=p_آرگومانها->هدف;

            إذا(U_SUCCESS(*err)) {
                إذا(s<p_آرگومانها->منبع_حد) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    اكسر;
                } وإلا إذا(real_منبع!=اشاره_تهی) {
                    /* switch back from replaying to the real source and continue */
                    p_آرگومانها->منبع=real_منبع;
                    p_آرگومانها->منبع_حد=real_منبع_حد;
                    p_آرگومانها->flush=realFlush;
                    منبع_شاخص=real_منبع_شاخص;

                    real_منبع=اشاره_تهی;
                    اكسر;
                } وإلا إذا(p_آرگومانها->flush && cnv->مقصد_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_یافت;
                    calledCallback=خطأ; /* new error condition */
                } وإلا {
                    /* input consumed */
                    إذا(p_آرگومانها->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        إذا(!converter_Saw_انتها_از_درون_داد) {
                            اكسر;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_مقصد_UNICODE, خطأ);
                    }

                    /* done successfully */
                    أرجع;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_کد e;

                إذا( calledCallback ||
                    (e=*err)==U_میانگیر_سرریز_ERROR ||
                    (e!=U_INVALID_CHAR_یافت &&
                     e!=U_ILLEGAL_CHAR_یافت &&
                     e!=U_TRUNCATED_CHAR_یافت &&
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
                    إذا(real_منبع!=اشاره_تهی) {
                        int32_t length;

                        U_ASSERT(cnv->pre_مقصد_ULength==0);

                        length = static_cast<int32_t>(p_آرگومانها->منبع_حد - p_آرگومانها->منبع);
                        إذا(length>0) {
                            uprv_memcpy(cnv->pre_مقصد_U, p_آرگومانها->منبع, length);
                            cnv->pre_مقصد_ULength = static_cast<int8_t>(-length);
                        }

                        p_آرگومانها->منبع=real_منبع;
                        p_آرگومانها->منبع_حد=real_منبع_حد;
                        p_آرگومانها->flush=realFlush;
                    }

                    أرجع;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_درون_داد_درازی=cnv->invalid_Char_درازی=cnv->مقصد_ULength;
            إذا(error_درون_داد_درازی>0) {
                uprv_memcpy(cnv->invalid_Char_میانگیر, cnv->مقصد_UBytes, error_درون_داد_درازی);
            }

            /* set the converter state to deal with the next character */
            cnv->مقصد_ULength=0;

            /* call the callback function */
            إذا(cnv->مقصد_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_یافت) {
                cnv->مقصد_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->مقصد_UContext, p_آرگومانها,
                cnv->invalid_Char_میانگیر, error_درون_داد_درازی,
                cnv->مقصد_UCallback_Reason,
                err);
            cnv->مقصد_UCallback_Reason = UCNV_ILLEGAL; /* reset to default value */

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
ساكن UBool
ucnv_output_سرریز_مقصد_Unicode(UConverter *cnv,
                             char16_t **هدف, ثابت char16_t *هدف_حد,
                             int32_t **pOffsets,
                             UError_کد *err) {
    int32_t *offsets;
    char16_t *سرریز, *t;
    int32_t i, length;

    t=*هدف;
    إذا(pOffsets!=اشاره_تهی) {
        offsets=*pOffsets;
    } وإلا {
        offsets=اشاره_تهی;
    }

    سرریز=cnv->UChar_Error_میانگیر;
    length=cnv->UChar_Error_میانگیر_درازی;
    i=0;
    طالما(i<length) {
        إذا(t==هدف_حد) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            افعل {
                سرریز[j++]=سرریز[i++];
            } طالما(i<length);

            cnv->UChar_Error_میانگیر_درازی = static_cast<int8_t>(j);
            *هدف=t;
            إذا(offsets!=اشاره_تهی) {
                *pOffsets=offsets;
            }
            *err=U_میانگیر_سرریز_ERROR;
            أرجع درست;
        }

        /* copy the overflow contents to the target */
        *t++=سرریز[i++];
        إذا(offsets!=اشاره_تهی) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_میانگیر_درازی=0;
    *هدف=t;
    إذا(offsets!=اشاره_تهی) {
        *pOffsets=offsets;
    }
    أرجع خطأ;
}

U_CAPI فراغ U_EXPORT2
ucnv_مقصد_Unicode(UConverter *cnv,
               char16_t **هدف, ثابت char16_t *هدف_حد,
               ثابت حرف **منبع, ثابت حرف *منبع_حد,
               int32_t *offsets,
               UBool flush,
               UError_کد *err) {
    UConverter_مقصد_Unicode_آرگومانها آرگومانها_2;
    ثابت حرف *s;
    char16_t *t;

    /* check parameters */
    إذا(err==اشاره_تهی || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(cnv==اشاره_تهی || هدف==اشاره_تهی || منبع==اشاره_تهی) {
        *err=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    s=*منبع;
    t=*هدف;

    إذا ((ثابت فراغ *)U_MAX_PTR(هدف_حد) == (ثابت فراغ *)هدف_حد) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        هدف_حد = (ثابت char16_t *)(((ثابت حرف *)هدف_حد) - 1);
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
    إذا (منبع_حد<s || هدف_حد<t ||
        ((size_t)(منبع_حد-s)>(size_t)0x7fffffff && منبع_حد>s) ||
        ((size_t)(هدف_حد-t)>(size_t)0x3fffffff && هدف_حد>t) ||
        (((ثابت حرف *)هدف_حد-(ثابت حرف *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    
    /* output the target overflow buffer */
    إذا( cnv->UChar_Error_میانگیر_درازی>0 &&
        ucnv_output_سرریز_مقصد_Unicode(cnv, هدف, هدف_حد, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        أرجع;
    }
    /* *target may have moved, therefore stop using t */

    إذا(!flush && s==منبع_حد && cnv->pre_مقصد_ULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        أرجع;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    آرگومانها_2.converter=cnv;
    آرگومانها_2.flush=flush;
    آرگومانها_2.offsets=offsets;
    آرگومانها_2.منبع=s;
    آرگومانها_2.منبع_حد=منبع_حد;
    آرگومانها_2.هدف=*هدف;
    آرگومانها_2.هدف_حد=هدف_حد;
    آرگومانها_2.size=حجم(آرگومانها_2);

    مقصد_Unicode_With_Callback(&آرگومانها_2, err);

    *منبع=آرگومانها_2.منبع;
    *هدف=آرگومانها_2.هدف;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                حرف *dest, int32_t dest_ظرفیت,
                ثابت char16_t *src, int32_t src_درازی,
                UError_کد *p_Error_کد) {
    ثابت char16_t *src_حد;
    حرف *originalDest, *dest_حد;
    int32_t dest_درازی;

    /* check arguments */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    إذا( cnv==اشاره_تهی ||
        dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==اشاره_تهی) ||
        src_درازی<-1 || (src_درازی!=0 && src==اشاره_تهی)
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    إذا(src_درازی==-1) {
        src_درازی=u_strlen(src);
    }
    إذا(src_درازی>0) {
        src_حد=src+src_درازی;
        dest_ظرفیت=pin_ظرفیت(dest, dest_ظرفیت);
        dest_حد=dest+dest_ظرفیت;

        /* perform the conversion */
        UError_کد میانگیر_وضعیت = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_حد, &src, src_حد, اشاره_تهی, درست, &میانگیر_وضعیت);
        dest_درازی=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        إذا(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR) {
            حرف میانگیر[1024];

            dest_حد=میانگیر+حجم(میانگیر);
            افعل {
                dest=میانگیر;
                میانگیر_وضعیت=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_حد, &src, src_حد, اشاره_تهی, درست, &میانگیر_وضعیت);
                dest_درازی+=(int32_t)(dest-میانگیر);
            } طالما(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR);
        }
        إذا (U_FAILURE(میانگیر_وضعیت)) {
            *p_Error_کد = میانگیر_وضعیت;
        }
    } وإلا {
        dest_درازی=0;
    }

    أرجع u_terminateChars(originalDest, dest_ظرفیت, dest_درازی, p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
ucnv_مقصد_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_ظرفیت,
              ثابت حرف *src, int32_t src_درازی,
              UError_کد *p_Error_کد) {
    ثابت حرف *src_حد;
    char16_t *originalDest, *dest_حد;
    int32_t dest_درازی;

    /* check arguments */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    إذا( cnv==اشاره_تهی ||
        dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==اشاره_تهی) ||
        src_درازی<-1 || (src_درازی!=0 && src==اشاره_تهی))
    {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* initialize */
    ucnv_reset_مقصد_Unicode(cnv);
    originalDest=dest;
    إذا(src_درازی==-1) {
        src_درازی=(int32_t)uprv_strlen(src);
    }
    إذا(src_درازی>0) {
        src_حد=src+src_درازی;
        dest_ظرفیت=pin_ظرفیت(dest, dest_ظرفیت);
        dest_حد=dest+dest_ظرفیت;

        /* perform the conversion */
        UError_کد میانگیر_وضعیت = U_ZERO_ERROR;
        ucnv_مقصد_Unicode(cnv, &dest, dest_حد, &src, src_حد, اشاره_تهی, درست, &میانگیر_وضعیت);
        dest_درازی=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        إذا(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR)
        {
            char16_t میانگیر[1024];

            dest_حد=میانگیر+UPRV_LENGTHOF(میانگیر);
            افعل {
                dest=میانگیر;
                میانگیر_وضعیت=U_ZERO_ERROR;
                ucnv_مقصد_Unicode(cnv, &dest, dest_حد, &src, src_حد, اشاره_تهی, درست, &میانگیر_وضعیت);
                dest_درازی+=(int32_t)(dest-میانگیر);
            }
            طالما(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR);
        }
        إذا (U_FAILURE(میانگیر_وضعیت)) {
            *p_Error_کد = میانگیر_وضعیت;
        }
    } وإلا {
        dest_درازی=0;
    }

    أرجع u_terminateUChars(originalDest, dest_ظرفیت, dest_درازی, p_Error_کد);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_بعدی_UChar(UConverter *cnv,
                  ثابت حرف **منبع, ثابت حرف *منبع_حد,
                  UError_کد *err) {
    UConverter_مقصد_Unicode_آرگومانها آرگومانها_2;
    char16_t میانگیر[U16_MAX_درازی];
    ثابت حرف *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    إذا(err==اشاره_تهی || U_FAILURE(*err)) {
        أرجع 0xffff;
    }

    إذا(cnv==اشاره_تهی || منبع==اشاره_تهی) {
        *err=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0xffff;
    }

    s=*منبع;
    إذا(منبع_حد<s) {
        *err=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0xffff;
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
    إذا(((size_t)(منبع_حد-s)>(size_t)0x7fffffff && منبع_حد>s)) {
        *err=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    إذا(cnv->UChar_Error_میانگیر_درازی>0) {
        char16_t *سرریز;

        سرریز=cnv->UChar_Error_میانگیر;
        i=0;
        length=cnv->UChar_Error_میانگیر_درازی;
        U16_بعدی(سرریز, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        إذا((cnv->UChar_Error_میانگیر_درازی=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_میانگیر, cnv->UChar_Error_میانگیر+i,
                         cnv->UChar_Error_میانگیر_درازی*U_SIZEOF_UCHAR);
        }

        إذا(!U16_IS_LEAD(c) || i<length) {
            أرجع c;
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
    آرگومانها_2.converter=cnv;
    آرگومانها_2.flush=درست;
    آرگومانها_2.offsets=اشاره_تهی;
    آرگومانها_2.منبع=s;
    آرگومانها_2.منبع_حد=منبع_حد;
    آرگومانها_2.هدف=میانگیر;
    آرگومانها_2.هدف_حد=میانگیر+1;
    آرگومانها_2.size=حجم(آرگومانها_2);

    إذا(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        إذا(cnv->مقصد_ULength==0 && cnv->shared_داده->impl->get_بعدی_UChar!=اشاره_تهی) {
            c=cnv->shared_داده->impl->get_بعدی_UChar(&آرگومانها_2, err);
            *منبع=s=آرگومانها_2.منبع;
            إذا(*err==U_شاخص_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_مقصد_UNICODE, خطأ);
                أرجع 0xffff; /* no output */
            } وإلا إذا(U_SUCCESS(*err) && c>=0) {
                أرجع c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        مقصد_Unicode_With_Callback(&آرگومانها_2, err);

        إذا(*err==U_میانگیر_سرریز_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(آرگومانها_2.هدف-میانگیر);
    } وإلا {
        /* write the lead surrogate from the overflow buffer */
        میانگیر[0]=(char16_t)c;
        آرگومانها_2.هدف=میانگیر+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    إذا(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } وإلا إذا(length==0) {
        /* no input or only state changes */
        *err=U_شاخص_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } وإلا {
        c=میانگیر[0];
        i=1;
        إذا(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } وإلا {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            إذا(cnv->UChar_Error_میانگیر_درازی>0) {
                /* got overflow output from the conversion */
                إذا(U16_IS_TRAIL(c2=cnv->UChar_Error_میانگیر[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    إذا((--cnv->UChar_Error_میانگیر_درازی)>0) {
                        uprv_memmove(cnv->UChar_Error_میانگیر, cnv->UChar_Error_میانگیر+1,
                                     cnv->UChar_Error_میانگیر_درازی*U_SIZEOF_UCHAR);
                    }
                } وإلا {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } وإلا إذا(آرگومانها_2.منبع<منبع_حد) {
                /* convert once more, to buffer[1] */
                آرگومانها_2.هدف_حد=میانگیر+2;
                مقصد_Unicode_With_Callback(&آرگومانها_2, err);
                إذا(*err==U_میانگیر_سرریز_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(آرگومانها_2.هدف-میانگیر);
                إذا(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=میانگیر[1])) {
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
    إذا(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        إذا((length=cnv->UChar_Error_میانگیر_درازی)>0) {
            uprv_memmove(cnv->UChar_Error_میانگیر+delta, cnv->UChar_Error_میانگیر,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_میانگیر_درازی=(int8_t)(length+delta);

        cnv->UChar_Error_میانگیر[0]=میانگیر[i++];
        إذا(delta>1) {
            cnv->UChar_Error_میانگیر[1]=میانگیر[i];
        }
    }

    *منبع=آرگومانها_2.منبع;
    أرجع c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI فراغ U_EXPORT2
ucnv_convertEx(UConverter *هدف_Cnv, UConverter *منبع_Cnv,
               حرف **هدف, ثابت حرف *هدف_حد,
               ثابت حرف **منبع, ثابت حرف *منبع_حد,
               char16_t *محور_شروع, char16_t **محور_منبع,
               char16_t **محور_هدف, ثابت char16_t *محور_حد,
               UBool reset, UBool flush,
               UError_کد *p_Error_کد) {
    char16_t محور_میانگیر[CHUNK_اندازه];
    ثابت char16_t *my_محور_منبع;
    char16_t *my_محور_هدف;
    ثابت حرف *s;
    حرف *t;

    UConverter_مقصد_Unicode_آرگومانها مقصد_UArgs;
    UConverter_From_Unicode_آرگومانها fromUArgs;
    UConverterConvert convert;

    /* error checking */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع;
    }

    إذا( هدف_Cnv==اشاره_تهی || منبع_Cnv==اشاره_تهی ||
        منبع==اشاره_تهی || *منبع==اشاره_تهی ||
        هدف==اشاره_تهی || *هدف==اشاره_تهی || هدف_حد==اشاره_تهی
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    s=*منبع;
    t=*هدف;
    إذا((منبع_حد!=اشاره_تهی && منبع_حد<s) || هدف_حد<t) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    إذا(
        (منبع_حد!=اشاره_تهی && ((size_t)(منبع_حد-s)>(size_t)0x7fffffff && منبع_حد>s)) ||
        ((size_t)(هدف_حد-t)>(size_t)0x7fffffff && هدف_حد>t)
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    
    إذا(محور_شروع==اشاره_تهی) {
        إذا(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }

        /* use the stack pivot buffer */
        my_محور_منبع=my_محور_هدف=محور_شروع=محور_میانگیر;
        محور_منبع=(char16_t **)&my_محور_منبع;
        محور_هدف=&my_محور_هدف;
        محور_حد=محور_میانگیر+CHUNK_اندازه;
    } وإلا إذا(  محور_شروع>=محور_حد ||
                محور_منبع==اشاره_تهی || *محور_منبع==اشاره_تهی ||
                محور_هدف==اشاره_تهی || *محور_هدف==اشاره_تهی ||
                محور_حد==اشاره_تهی
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    إذا(منبع_حد==اشاره_تهی) {
        /* get limit of single-byte-NUL-terminated source string */
        منبع_حد=uprv_strchr(*منبع, 0);
    }

    إذا(reset) {
        ucnv_reset_مقصد_Unicode(منبع_Cnv);
        ucnv_resetFromUnicode(هدف_Cnv);
        *محور_منبع=*محور_هدف=محور_شروع;
    } وإلا إذا(هدف_Cnv->char_Error_میانگیر_درازی>0) {
        /* output the targetCnv overflow buffer */
        إذا(ucnv_output_سرریز_From_Unicode(هدف_Cnv, هدف, هدف_حد, اشاره_تهی, p_Error_کد)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            أرجع;
        }
        /* *target has moved, therefore stop using t */

        إذا( !flush &&
            هدف_Cnv->preFromULength>=0 && *محور_منبع==*محور_هدف &&
            منبع_Cnv->UChar_Error_میانگیر_درازی==0 && منبع_Cnv->pre_مقصد_ULength>=0 && s==منبع_حد
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            أرجع;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    إذا( منبع_Cnv->shared_داده->static_داده->conversionType==UCNV_UTF8 &&
        هدف_Cnv->shared_داده->impl->fromUTF8!=اشاره_تهی
    ) {
        convert=هدف_Cnv->shared_داده->impl->fromUTF8;
    } وإلا إذا( هدف_Cnv->shared_داده->static_داده->conversionType==UCNV_UTF8 &&
               منبع_Cnv->shared_داده->impl->مقصد_UTF8!=اشاره_تهی
    ) {
        convert=منبع_Cnv->shared_داده->impl->مقصد_UTF8;
    } وإلا {
        convert=اشاره_تهی;
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
    إذا(convert!=اشاره_تهی && (محور_حد-محور_شروع)>32) {
        محور_حد=محور_شروع+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=هدف_Cnv;
    fromUArgs.flush=خطأ;
    fromUArgs.offsets=اشاره_تهی;
    fromUArgs.هدف=*هدف;
    fromUArgs.هدف_حد=هدف_حد;
    fromUArgs.size=حجم(fromUArgs);

    مقصد_UArgs.converter=منبع_Cnv;
    مقصد_UArgs.flush=flush;
    مقصد_UArgs.offsets=اشاره_تهی;
    مقصد_UArgs.منبع=s;
    مقصد_UArgs.منبع_حد=منبع_حد;
    مقصد_UArgs.هدف_حد=محور_حد;
    مقصد_UArgs.size=حجم(مقصد_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=اشاره_تهی;

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
    لكل(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        إذا( *محور_منبع<*محور_هدف ||
            U_FAILURE(*p_Error_کد) ||
            هدف_Cnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.منبع=*محور_منبع;
            fromUArgs.منبع_حد=*محور_هدف;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_کد);
            إذا(U_FAILURE(*p_Error_کد)) {
                /* target overflow, or conversion error */
                *محور_منبع=(char16_t *)fromUArgs.منبع;
                اكسر;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *محور_منبع=*محور_هدف=محور_شروع;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        إذا(منبع_Cnv->UChar_Error_میانگیر_درازی>0) {
            إذا(ucnv_output_سرریز_مقصد_Unicode(منبع_Cnv, محور_هدف, محور_حد, اشاره_تهی, p_Error_کد)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_کد=U_ZERO_ERROR;
            }
            استمر;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        إذا( مقصد_UArgs.منبع==منبع_حد &&
            منبع_Cnv->pre_مقصد_ULength>=0 && منبع_Cnv->مقصد_ULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            اكسر;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        إذا(convert!=اشاره_تهی && هدف_Cnv->preFromUFirstCP<0 && منبع_Cnv->pre_مقصد_ULength==0) {
            إذا(*p_Error_کد==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_کد=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &مقصد_UArgs, p_Error_کد);
            إذا(*p_Error_کد==U_میانگیر_سرریز_ERROR) {
                اكسر;
            } وإلا إذا(U_FAILURE(*p_Error_کد)) {
                إذا(منبع_Cnv->مقصد_ULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } وإلا {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *محور_منبع=*محور_هدف=محور_شروع+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    استمر;
                }
            } وإلا إذا(*p_Error_کد==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_کد=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } وإلا إذا(flush && منبع_Cnv->مقصد_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_کد=U_TRUNCATED_CHAR_یافت;
            } وإلا {
                /* input consumed */
                إذا(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(منبع_Cnv, UCNV_RESET_مقصد_UNICODE, خطأ);
                    _reset(هدف_Cnv, UCNV_RESET_FROM_UNICODE, خطأ);
                }

                /* done successfully */
                اكسر;
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
        مقصد_UArgs.هدف=محور_شروع; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        مقصد_Unicode_With_Callback(&مقصد_UArgs, p_Error_کد);
        *محور_هدف=مقصد_UArgs.هدف;
        إذا(*p_Error_کد==U_میانگیر_سرریز_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_کد=U_ZERO_ERROR;
        } وإلا إذا(U_FAILURE(*p_Error_کد) || (!flush && *محور_هدف==محور_شروع)) {
            /* conversion error, or there was nothing left to convert */
            اكسر;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        إذا( flush && مقصد_UArgs.منبع==منبع_حد &&
            منبع_Cnv->pre_مقصد_ULength>=0 &&
            منبع_Cnv->UChar_Error_میانگیر_درازی==0
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

    *منبع=مقصد_UArgs.منبع;
    *هدف=fromUArgs.هدف;

    /* terminate the target buffer if possible */
    إذا(flush && U_SUCCESS(*p_Error_کد)) {
        إذا(*هدف!=هدف_حد) {
            **هدف=0;
            إذا(*p_Error_کد==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_کد=U_ZERO_ERROR;
            }
        } وإلا {
            *p_Error_کد=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
ساكن int32_t
ucnv_داخلی_Convert(UConverter *بیرون_Converter, UConverter *درون_Converter,
                     حرف *هدف, int32_t هدف_ظرفیت,
                     ثابت حرف *منبع, int32_t منبع_درازی,
                     UError_کد *p_Error_کد) {
    char16_t محور_میانگیر[CHUNK_اندازه];
    char16_t *محور, *pivot2;

    حرف *my_هدف;
    ثابت حرف *منبع_حد;
    ثابت حرف *هدف_حد;
    int32_t هدف_درازی=0;

    /* set up */
    إذا(منبع_درازی<0) {
        منبع_حد=uprv_strchr(منبع, 0);
    } وإلا {
        منبع_حد=منبع+منبع_درازی;
    }

    /* if there is no input data, we're done */
    إذا(منبع==منبع_حد) {
        أرجع u_terminateChars(هدف, هدف_ظرفیت, 0, p_Error_کد);
    }

    محور=pivot2=محور_میانگیر;
    my_هدف=هدف;
    هدف_درازی=0;

    إذا(هدف_ظرفیت>0) {
        /* perform real conversion */
        هدف_حد=هدف+هدف_ظرفیت;
        ucnv_convertEx(بیرون_Converter, درون_Converter,
                       &my_هدف, هدف_حد,
                       &منبع, منبع_حد,
                       محور_میانگیر, &محور, &pivot2, محور_میانگیر+CHUNK_اندازه,
                       خطأ,
                       درست,
                       p_Error_کد);
        هدف_درازی = static_cast<int32_t>(my_هدف - هدف);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    إذا(*p_Error_کد==U_میانگیر_سرریز_ERROR || هدف_ظرفیت==0)
    {
        حرف هدف_میانگیر[CHUNK_اندازه];

        هدف_حد=هدف_میانگیر+CHUNK_اندازه;
        افعل {
            *p_Error_کد=U_ZERO_ERROR;
            my_هدف=هدف_میانگیر;
            ucnv_convertEx(بیرون_Converter, درون_Converter,
                           &my_هدف, هدف_حد,
                           &منبع, منبع_حد,
                           محور_میانگیر, &محور, &pivot2, محور_میانگیر+CHUNK_اندازه,
                           خطأ,
                           درست,
                           p_Error_کد);
            هدف_درازی += static_cast<int32_t>(my_هدف - هدف_میانگیر);
        } طالما(*p_Error_کد==U_میانگیر_سرریز_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        أرجع u_terminateChars(هدف, هدف_ظرفیت, هدف_درازی, p_Error_کد);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    أرجع هدف_درازی;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ثابت حرف *مقصد_Converter_Name, ثابت حرف *fromConverterName,
             حرف *هدف, int32_t هدف_ظرفیت,
             ثابت حرف *منبع, int32_t منبع_درازی,
             UError_کد *p_Error_کد) {
    UConverter in, out; /* stack-allocated */
    UConverter *درون_Converter, *بیرون_Converter;
    int32_t هدف_درازی;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    إذا( منبع==اشاره_تهی || منبع_درازی<-1 ||
        هدف_ظرفیت<0 || (هدف_ظرفیت>0 && هدف==اشاره_تهی)
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* if there is no input data, we're done */
    إذا(منبع_درازی==0 || (منبع_درازی<0 && *منبع==0)) {
        أرجع u_terminateChars(هدف, هدف_ظرفیت, 0, p_Error_کد);
    }

    /* create the converters */
    درون_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    بیرون_Converter=ucnv_createConverter(&out, مقصد_Converter_Name, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        ucnv_close(درون_Converter);
        أرجع 0;
    }

    هدف_درازی=ucnv_داخلی_Convert(بیرون_Converter, درون_Converter,
                                      هدف, هدف_ظرفیت,
                                      منبع, منبع_درازی,
                                      p_Error_کد);

    ucnv_close(درون_Converter);
    ucnv_close(بیرون_Converter);

    أرجع هدف_درازی;
}

/* @internal */
ساكن int32_t
ucnv_convertAlgorithmic(UBool convert_مقصد_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        حرف *هدف, int32_t هدف_ظرفیت,
                        ثابت حرف *منبع, int32_t منبع_درازی,
                        UError_کد *p_Error_کد) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *مقصد, *from;
    int32_t هدف_درازی;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    إذا( cnv==اشاره_تهی || منبع==اشاره_تهی || منبع_درازی<-1 ||
        هدف_ظرفیت<0 || (هدف_ظرفیت>0 && هدف==اشاره_تهی)
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* if there is no input data, we're done */
    إذا(منبع_درازی==0 || (منبع_درازی<0 && *منبع==0)) {
        أرجع u_terminateChars(هدف, هدف_ظرفیت, 0, p_Error_کد);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    /* reset the other converter */
    إذا(convert_مقصد_Algorithmic) {
        /* cnv->Unicode->algo */
        ucnv_reset_مقصد_Unicode(cnv);
        مقصد=algoConverter;
        from=cnv;
    } وإلا {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        مقصد=cnv;
    }

    هدف_درازی=ucnv_داخلی_Convert(مقصد, from,
                                      هدف, هدف_ظرفیت,
                                      منبع, منبع_درازی,
                                      p_Error_کد);

    ucnv_close(algoConverter);

    أرجع هدف_درازی;
}

U_CAPI int32_t U_EXPORT2
ucnv_مقصد_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   حرف *هدف, int32_t هدف_ظرفیت,
                   ثابت حرف *منبع, int32_t منبع_درازی,
                   UError_کد *p_Error_کد) {
    أرجع ucnv_convertAlgorithmic(درست, algorithmicType, cnv,
                                   هدف, هدف_ظرفیت,
                                   منبع, منبع_درازی,
                                   p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     حرف *هدف, int32_t هدف_ظرفیت,
                     ثابت حرف *منبع, int32_t منبع_درازی,
                     UError_کد *p_Error_کد) UPRV_NO_SANITIZE_UNDEFINED {

    إذا(algorithmicType<0 || UCNV_عدد_از_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    أرجع ucnv_convertAlgorithmic(خطأ, algorithmicType, cnv,
                                   هدف, هدف_ظرفیت,
                                   منبع, منبع_درازی,
                                   p_Error_کد);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ثابت UConverter* converter)
{
    int8_t type = converter->shared_داده->static_داده->conversionType;
#إذا !UCONFIG_NO_LEGACY_CONVERSION
    إذا(type == UCNV_MBCS) {
        أرجع ucnv_MBCSGetType(converter);
    }
#پایان_اگر
    أرجع (UConverterType)type;
}

U_CAPI فراغ  U_EXPORT2
ucnv_getStarters(ثابت UConverter* converter, 
                 UBool starters[256],
                 UError_کد* err)
{
    إذا (err == اشاره_تهی || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(converter->shared_داده->impl->getStarters != اشاره_تهی) {
        converter->shared_داده->impl->getStarters(converter, starters, err);
    } وإلا {
        *err = U_ILLEGAL_آرگومان_ERROR;
    }
}

ساكن ثابت UAmbiguousConverter *ucnv_getAmbiguous(ثابت UConverter *cnv)
{
    UError_کد error_کد;
    ثابت حرف *name;
    int32_t i;

    إذا(cnv==اشاره_تهی) {
        أرجع اشاره_تهی;
    }

    error_کد=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }

    لكل(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        إذا(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            أرجع ambiguousConverters+i;
        }
    }

    أرجع اشاره_تهی;
}

U_CAPI فراغ  U_EXPORT2
ucnv_fixFileSeparator(ثابت UConverter *cnv, 
                      char16_t* منبع,
                      int32_t منبع_درازی) {
    ثابت UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    إذا(cnv==اشاره_تهی || منبع==اشاره_تهی || منبع_درازی<=0 || (a=ucnv_getAmbiguous(cnv))==اشاره_تهی)
    {
        أرجع;
    }

    variant5c=a->variant5c;
    لكل(i=0; i<منبع_درازی; ++i) {
        إذا(منبع[i]==variant5c) {
            منبع[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ثابت UConverter *cnv) {
    أرجع ucnv_getAmbiguous(cnv)!=اشاره_تهی;
}

U_CAPI فراغ  U_EXPORT2
ucnv_مجموعه_Fallback(UConverter *cnv, UBool استفاده_میکند_Fallback)
{
    cnv->useFallback = استفاده_میکند_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_استفاده_میکند_Fallback(ثابت UConverter *cnv)
{
    أرجع cnv->useFallback;
}

U_CAPI فراغ  U_EXPORT2
ucnv_getInvalidChars (ثابت UConverter * converter,
                      حرف *err_بایتها,
                      int8_t * len,
                      UError_کد * err)
{
    إذا (err == اشاره_تهی || U_FAILURE(*err))
    {
        أرجع;
    }
    إذا (len == اشاره_تهی || err_بایتها == اشاره_تهی || converter == اشاره_تهی)
    {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    إذا (*len < converter->invalid_Char_درازی)
    {
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    إذا ((*len = converter->invalid_Char_درازی) > 0)
    {
        uprv_memcpy (err_بایتها, converter->invalid_Char_میانگیر, *len);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_getInvalidUChars (ثابت UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_کد * err)
{
    إذا (err == اشاره_تهی || U_FAILURE(*err))
    {
        أرجع;
    }
    إذا (len == اشاره_تهی || errChars == اشاره_تهی || converter == اشاره_تهی)
    {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    إذا (*len < converter->invalid_UChar_درازی)
    {
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    إذا ((*len = converter->invalid_UChar_درازی) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_میانگیر, *len);
    }
}

#تعریف_کن SIG_MAX_LEN 5

U_CAPI ثابت حرف* U_EXPORT2
ucnv_detectUnicodeSignature( ثابت حرف* منبع,
                             int32_t منبع_درازی,
                             int32_t* signature_درازی,
                             UError_کد* p_Error_کد) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    حرف شروع[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    صحيح i = 0;

    إذا((p_Error_کد==اشاره_تهی) || U_FAILURE(*p_Error_کد)){
        أرجع اشاره_تهی;
    }
    
    إذا(منبع == اشاره_تهی || منبع_درازی < -1){
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    إذا(signature_درازی == اشاره_تهی) {
        signature_درازی = &dummy;
    }

    إذا(منبع_درازی==-1){
        منبع_درازی=(int32_t)uprv_strlen(منبع);
    }

    
    طالما(i<منبع_درازی&& i<SIG_MAX_LEN){
        شروع[i]=منبع[i];
        i++;
    }

    إذا(شروع[0] == '\xFE' && شروع[1] == '\xFF') {
        *signature_درازی=2;
        أرجع  "UTF-16BE";
    } وإلا إذا(شروع[0] == '\xFF' && شروع[1] == '\xFE') {
        إذا(شروع[2] == '\x00' && شروع[3] =='\x00') {
            *signature_درازی=4;
            أرجع "UTF-32LE";
        } وإلا {
            *signature_درازی=2;
            أرجع  "UTF-16LE";
        }
    } وإلا إذا(شروع[0] == '\xEF' && شروع[1] == '\xBB' && شروع[2] == '\xBF') {
        *signature_درازی=3;
        أرجع  "UTF-8";
    } وإلا إذا(شروع[0] == '\x00' && شروع[1] == '\x00' && 
              شروع[2] == '\xFE' && شروع[3]=='\xFF') {
        *signature_درازی=4;
        أرجع  "UTF-32BE";
    } وإلا إذا(شروع[0] == '\x0E' && شروع[1] == '\xFE' && شروع[2] == '\xFF') {
        *signature_درازی=3;
        أرجع "SCSU";
    } وإلا إذا(شروع[0] == '\xFB' && شروع[1] == '\xEE' && شروع[2] == '\x28') {
        *signature_درازی=3;
        أرجع "BOCU-1";
    } وإلا إذا(شروع[0] == '\x2B' && شروع[1] == '\x2F' && شروع[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        إذا(شروع[3] == '\x38' && شروع[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_درازی=5;
            أرجع "UTF-7";
        } وإلا إذا(شروع[3] == '\x38' || شروع[3] == '\x39' || شروع[3] == '\x2B' || شروع[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_درازی=4;
            أرجع "UTF-7";
        }
    }وإلا إذا(شروع[0]=='\xDD' && شروع[1]== '\x73'&& شروع[2]=='\x66' && شروع[3]=='\x73'){
        *signature_درازی=4;
        أرجع "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_درازی=0;
    أرجع اشاره_تهی;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ثابت UConverter* cnv, UError_کد* وضعیت)
{
    إذا(وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)){
        أرجع -1;
    }
    إذا(cnv == اشاره_تهی){
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع -1;
    }

    إذا(cnv->preFromUFirstCP >= 0){
        أرجع U16_درازی(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }وإلا إذا(cnv->preFromULength < 0){
        أرجع -cnv->preFromULength ;
    }وإلا إذا(cnv->fromUChar32 > 0){
        أرجع 1;
    }
    أرجع 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_مقصد_UCount_Pending(ثابت UConverter* cnv, UError_کد* وضعیت){

    إذا(وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)){
        أرجع -1;
    }
    إذا(cnv == اشاره_تهی){
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع -1;
    }

    إذا(cnv->pre_مقصد_ULength > 0){
        أرجع cnv->pre_مقصد_ULength ;
    }وإلا إذا(cnv->pre_مقصد_ULength < 0){
        أرجع -cnv->pre_مقصد_ULength;
    }وإلا إذا(cnv->مقصد_ULength > 0){
        أرجع cnv->مقصد_ULength;
    }
    أرجع 0;
}

U_CAPI UBool U_EXPORT2
ucnv_is_Fixed_عرض(UConverter *cnv, UError_کد *وضعیت){
    إذا (U_FAILURE(*وضعیت)) {
        أرجع خطأ;
    }

    إذا (cnv == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع خطأ;
    }

    بدل (ucnv_getType(cnv)) {
        حالة UCNV_SBCS:
        حالة UCNV_DBCS:
        حالة UCNV_UTF32_BigEndian:
        حالة UCNV_UTF32_LittleEndian:
        حالة UCNV_UTF32:
        حالة UCNV_US_ASCII:
            أرجع درست;
        افتراضي:
            أرجع خطأ;
    }
}
#پایان_اگر

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
