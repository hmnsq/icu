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
#تعریف_کړه CHUNK_اندازه 1024

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
                       UError_کوډ * err)
{
    UConverter *r;

    إذا (err == تش_اشاره || U_FAILURE (*err)) {
        أرجع تش_اشاره;
    }

    r =  ucnv_createConverter(تش_اشاره, name, err);
    أرجع r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ثابت حرف *packageName, ثابت حرف *converterName, UError_کوډ * err)
{
    أرجع ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ثابت char16_t * name,
                         UError_کوډ * err)
{
    حرف asciiName[UCNV_MAX_CONVERTER_NAME_اوږدتیا];

    إذا (err == تش_اشاره || U_FAILURE(*err))
        أرجع تش_اشاره;
    إذا (name == تش_اشاره)
        أرجع ucnv_open (تش_اشاره, err);
    إذا (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_اوږدتیا)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
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
                UError_کوډ * err)
{
    حرف myName[UCNV_MAX_CONVERTER_NAME_اوږدتیا];
    int32_t myNameLen;

    إذا (err == تش_اشاره || U_FAILURE (*err))
        أرجع تش_اشاره;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_منزل_String(myName + myNameLen, codepage, 10);

    أرجع ucnv_createConverter(تش_اشاره, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_خوندي_Clone(ثابت UConverter* cnv,
               فراغ* ډېری_بفر U_LIFETIME_BOUND,
               int32_t* p_بفر_اندازه,
               UError_کوډ* حالت)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t ډېری_بفر_اندازه;
    int32_t بفر_اندازه_Needed;
    UError_کوډ cbErr;
    UConverter_منزل_Unicode_دلیلونه منزل_UArgs = {
        حجم(UConverter_منزل_Unicode_دلیلونه),
            ریښتیا,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره
    };
    UConverter_From_Unicode_دلیلونه fromUArgs = {
        حجم(UConverter_From_Unicode_دلیلونه),
            ریښتیا,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره,
            تش_اشاره
    };

    UTRACE_ثبت_OC(UTRACE_UCNV_CLONE);

    إذا (حالت == تش_اشاره || U_FAILURE(*حالت)){
        UTRACE_EXIT_حالت(حالت? *حالت: U_ILLEGAL_دلیل_ERROR);
        أرجع تش_اشاره;
    }

    إذا (cnv == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(*حالت);
        أرجع تش_اشاره;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, حالت), cnv, ډېری_بفر);

    إذا (cnv->shared_معلومات->impl->خوندي_Clone != تش_اشاره) {
        /* call the custom safeClone function for sizing */
        بفر_اندازه_Needed = 0;
        cnv->shared_معلومات->impl->خوندي_Clone(cnv, تش_اشاره, &بفر_اندازه_Needed, حالت);
        إذا (U_FAILURE(*حالت)) {
            UTRACE_EXIT_حالت(*حالت);
            أرجع تش_اشاره;
        }
    }
    وإلا
    {
        /* inherent sizing */
        بفر_اندازه_Needed = حجم(UConverter);
    }

    إذا (p_بفر_اندازه == تش_اشاره) {
        ډېری_بفر_اندازه = 1;
        p_بفر_اندازه = &ډېری_بفر_اندازه;
    } وإلا {
        ډېری_بفر_اندازه = *p_بفر_اندازه;
        إذا (ډېری_بفر_اندازه <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_بفر_اندازه = بفر_اندازه_Needed;
            UTRACE_EXIT_ارزښت(بفر_اندازه_Needed);
            أرجع تش_اشاره;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    إذا (ډېری_بفر) {
        uintptr_t p = reinterpret_cast<uintptr_t>(ډېری_بفر);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t اشاره_ګر_Adjustment = aligned_p - p;
        إذا (بفر_اندازه_Needed + اشاره_ګر_Adjustment <= ډېری_بفر_اندازه) {
            ډېری_بفر = reinterpret_cast<فراغ *>(aligned_p);
            ډېری_بفر_اندازه -= static_cast<int32_t>(اشاره_ګر_Adjustment);
        } وإلا {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            ډېری_بفر_اندازه = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    إذا (ډېری_بفر_اندازه < بفر_اندازه_Needed || ډېری_بفر == تش_اشاره)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (بفر_اندازه_Needed);

        إذا(localConverter == تش_اشاره) {
            *حالت = U_حافظه_ویش_ERROR;
            UTRACE_EXIT_حالت(*حالت);
            أرجع تش_اشاره;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        إذا (p_بفر_اندازه != &ډېری_بفر_اندازه) {
            *حالت = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_بفر_اندازه = بفر_اندازه_Needed;
    } وإلا {
        /* just use the stack buffer */
        localConverter = (UConverter*) ډېری_بفر;
        allocatedConverter = تش_اشاره;
    }

    uprv_memset(localConverter, 0, بفر_اندازه_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, حجم(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = خطأ;

    /* copy the substitution string */
    إذا (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } وإلا {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_بفر_اوږدتیا * U_SIZEOF_UCHAR);
        إذا (localConverter->subChars == تش_اشاره) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_حالت(*حالت);
            أرجع تش_اشاره;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_بفر_اوږدتیا * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    إذا (cnv->shared_معلومات->impl->خوندي_Clone != تش_اشاره) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_معلومات->impl->خوندي_Clone(cnv, localConverter, p_بفر_اندازه, حالت);
    }

    إذا(localConverter==تش_اشاره || U_FAILURE(*حالت)) {
        إذا (allocatedConverter != تش_اشاره && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_حالت(*حالت);
        أرجع تش_اشاره;
    }

    /* increment refcount of shared data if needed */
    إذا (cnv->shared_معلومات->isReferenceCounted) {
        ucnv_increment_Ref_شمېر(cnv->shared_معلومات);
    }

    إذا(localConverter == (UConverter*)ډېری_بفر) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = ریښتیا;
    }

    /* allow callback functions to handle any memory allocation */
    منزل_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->منزل_UContext, &منزل_UArgs, تش_اشاره, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, تش_اشاره, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_حالت(localConverter, *حالت);
    أرجع localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ثابت UConverter* cnv, UError_کوډ *حالت)
{
    أرجع ucnv_خوندي_Clone(cnv, تش_اشاره, تش_اشاره, حالت);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI فراغ  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_کوډ error_کوډ = U_ZERO_ERROR;

    UTRACE_ثبت_OC(UTRACE_UCNV_CLOSE);

    إذا (converter == تش_اشاره)
    {
        UTRACE_EXIT();
        أرجع;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_کوډ), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    إذا (converter->fromCharErrorBehaviour != UCNV_منزل_U_DEFAULT_CALLBACK) {
        UConverter_منزل_Unicode_دلیلونه منزل_UArgs = {
            حجم(UConverter_منزل_Unicode_دلیلونه),
                ریښتیا,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره
        };

        منزل_UArgs.converter = converter;
        error_کوډ = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->منزل_UContext, &منزل_UArgs, تش_اشاره, 0, UCNV_CLOSE, &error_کوډ);
    }
    إذا (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_دلیلونه fromUArgs = {
            حجم(UConverter_From_Unicode_دلیلونه),
                ریښتیا,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره
        };
        fromUArgs.converter = converter;
        error_کوډ = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, تش_اشاره, 0, 0, UCNV_CLOSE, &error_کوډ);
    }

    إذا (converter->shared_معلومات->impl->close != تش_اشاره) {
        converter->shared_معلومات->impl->close(converter);
    }

    إذا (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    إذا (converter->shared_معلومات->isReferenceCounted) {
        ucnv_unload_Shared_معلومات_If_Ready(converter->shared_معلومات);
    }

    إذا(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ثابت حرف*   U_EXPORT2
ucnv_get_شته_Name (int32_t n)
{
    إذا (0 <= n && n <= 0xffff) {
        UError_کوډ err = U_ZERO_ERROR;
        ثابت حرف *name = ucnv_bld_get_شته_Converter((uint16_t)n, &err);
        إذا (U_SUCCESS(err)) {
            أرجع name;
        }
    }
    أرجع تش_اشاره;
}

U_CAPI int32_t   U_EXPORT2
ucnv_شمېر_شته ()
{
    UError_کوډ err = U_ZERO_ERROR;
    أرجع ucnv_bld_شمېر_شته_Converters(&err);
}

U_CAPI فراغ    U_EXPORT2
ucnv_getSubstChars (ثابت UConverter * converter,
                    حرف *mySubChar,
                    int8_t * len,
                    UError_کوډ * err)
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
ucnv_سېټ_Subst_Chars (UConverter * converter,
                    ثابت حرف *mySubChar,
                    int8_t len,
                    UError_کوډ * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    إذا ((len > converter->shared_معلومات->static_معلومات->max_بایټونه_فی_Char)
     || (len < converter->shared_معلومات->static_معلومات->min_بایټونه_فی_Char))
    {
        *err = U_ILLEGAL_دلیل_ERROR;
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
ucnv_سېټ_Subst_String(UConverter *cnv,
                    ثابت char16_t *s,
                    int32_t length,
                    UError_کوډ *err) {
    alignas(UConverter) حرف clone_بفر[U_CNV_SAFECLONE_BUFFERSIZE];
    حرف chars[UCNV_ERROR_بفر_اوږدتیا];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_اندازه, length8;

    /* Let the following functions check all arguments. */
    clone_اندازه = حجم(clone_بفر);
    clone = ucnv_خوندي_Clone(cnv, clone_بفر, &clone_اندازه, err);
    ucnv_سېټ_From_UCall_شا(clone, UCNV_FROM_U_CALLBACK_STOP, تش_اشاره, تش_اشاره, تش_اشاره, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)حجم(chars), s, length, err);
    ucnv_close(clone);
    إذا (U_FAILURE(*err)) {
        أرجع;
    }

    إذا (cnv->shared_معلومات->impl->writeSub == تش_اشاره
#إذا !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_معلومات->static_معلومات->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#د_که_پای
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
        إذا (length > UCNV_ERROR_بفر_اوږدتیا) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_بفر_ډکېدنه_ERROR;
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
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_بفر_اوږدتیا * U_SIZEOF_UCHAR);
            إذا (cnv->subChars == تش_اشاره) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_حافظه_ویش_ERROR;
                أرجع;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_بفر_اوږدتیا * U_SIZEOF_UCHAR);
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
    إذا(converter == تش_اشاره) {
        أرجع;
    }

    إذا(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_کوډ error_کوډ;

        إذا(choice<=UCNV_RESET_منزل_UNICODE && converter->fromCharErrorBehaviour != UCNV_منزل_U_DEFAULT_CALLBACK) {
            UConverter_منزل_Unicode_دلیلونه منزل_UArgs = {
                حجم(UConverter_منزل_Unicode_دلیلونه),
                ریښتیا,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره
            };
            منزل_UArgs.converter = converter;
            error_کوډ = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->منزل_UContext, &منزل_UArgs, تش_اشاره, 0, UCNV_RESET, &error_کوډ);
        }
        إذا(choice!=UCNV_RESET_منزل_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_دلیلونه fromUArgs = {
                حجم(UConverter_From_Unicode_دلیلونه),
                ریښتیا,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره,
                تش_اشاره
            };
            fromUArgs.converter = converter;
            error_کوډ = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, تش_اشاره, 0, 0, UCNV_RESET, &error_کوډ);
        }
    }

    /* now reset the converter itself */
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        converter->منزل_Unicode_حالت = converter->shared_معلومات->منزل_Unicode_حالت;
        converter->ډېرپېښ = 0;
        converter->منزل_ULength = 0;
        converter->invalid_Char_اوږدتیا = converter->UChar_Error_بفر_اوږدتیا = 0;
        converter->pre_منزل_ULength = 0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        converter->from_Unicode_حالت = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_اوږدتیا = converter->char_Error_بفر_اوږدتیا = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    إذا (converter->shared_معلومات->impl->reset != تش_اشاره) {
        /* call the custom reset function */
        converter->shared_معلومات->impl->reset(converter, choice);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, ریښتیا);
}

U_CAPI فراغ  U_EXPORT2
ucnv_reset_منزل_Unicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_منزل_UNICODE, ریښتیا);
}

U_CAPI فراغ  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, ریښتیا);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_اندازه (ثابت UConverter * converter)
{
    أرجع converter->max_بایټونه_فی_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_اندازه (ثابت UConverter * converter)
{
    أرجع converter->shared_معلومات->static_معلومات->min_بایټونه_فی_Char;
}

U_CAPI ثابت حرف* U_EXPORT2
ucnv_getName(ثابت UConverter* converter U_LIFETIME_BOUND, UError_کوډ* err)
{
    إذا (U_FAILURE (*err))
        أرجع تش_اشاره;
    إذا(converter->shared_معلومات->impl->getName){
        ثابت حرف* لنډمهاله= converter->shared_معلومات->impl->getName(converter);
        إذا(لنډمهاله)
            أرجع لنډمهاله;
    }
    أرجع converter->shared_معلومات->static_معلومات->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ثابت UConverter * converter,
              UError_کوډ * err)
{
    int32_t ccsid;
    إذا (U_FAILURE (*err))
        أرجع -1;

    ccsid = converter->shared_معلومات->static_معلومات->codepage;
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
                                      UError_کوډ * err)
{
    إذا (U_FAILURE (*err))
        أرجع UCNV_UNKNOWN;

    أرجع (UConverterPlatform)converter->shared_معلومات->static_معلومات->platform;
}

U_CAPI فراغ U_EXPORT2
    ucnv_get_منزل_UCall_شا (ثابت UConverter * converter,
                         UConverter_منزل_UCallback *action,
                         ثابت فراغ **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->منزل_UContext;
}

U_CAPI فراغ U_EXPORT2
    ucnv_get_From_UCall_شا (ثابت UConverter * converter,
                           UConverterFromUCallback *action,
                           ثابت فراغ **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI فراغ    U_EXPORT2
ucnv_سېټ_منزل_UCall_شا (UConverter * converter,
                            UConverter_منزل_UCallback newAction,
                            ثابت فراغ* newContext,
                            UConverter_منزل_UCallback *oldAction,
                            ثابت فراغ** oldContext,
                            UError_کوډ * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    إذا (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    إذا (oldContext) *oldContext = converter->منزل_UContext;
    converter->منزل_UContext = newContext;
}

U_CAPI فراغ  U_EXPORT2
ucnv_سېټ_From_UCall_شا (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ثابت فراغ* newContext,
                            UConverterFromUCallback *oldAction,
                            ثابت فراغ** oldContext,
                            UError_کوډ * err)
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
               int32_t سرچینه_شاخص, int32_t error_ننوت_اوږدتیا) {
    int32_t *حد;
    int32_t delta, دننه_واټن;

    إذا(سرچینه_شاخص>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=سرچینه_شاخص-error_ننوت_اوږدتیا;
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
            دننه_واټن=*offsets;
            إذا(دننه_واټن>=0) {
                *offsets=دننه_واټن+delta;
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
_fromUnicodeWithCallback(UConverter_From_Unicode_دلیلونه *p_دلیلونه, UError_کوډ *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ثابت char16_t *s;
    حرف *t;
    int32_t *offsets;
    int32_t سرچینه_شاخص;
    int32_t error_ننوت_اوږدتیا;
    UBool converter_Saw_پای_ټکی_د_ننوت, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ثابت char16_t *real_سرچینه, *real_سرچینه_حد;
    int32_t real_سرچینه_شاخص;
    UBool realFlush;

    cnv=p_دلیلونه->converter;
    s=p_دلیلونه->سرچینه;
    t=p_دلیلونه->موخه;
    offsets=p_دلیلونه->offsets;

    /* get the converter implementation function */
    سرچینه_شاخص=0;
    إذا(offsets==تش_اشاره) {
        fromUnicode=cnv->shared_معلومات->impl->fromUnicode;
    } وإلا {
        fromUnicode=cnv->shared_معلومات->impl->fromUnicodeWithOffsets;
        إذا(fromUnicode==تش_اشاره) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_معلومات->impl->fromUnicode;
            /* we will write -1 for each offset */
            سرچینه_شاخص=-1;
        }
    }

    إذا(cnv->preFromULength>=0) {
        /* normal mode */
        real_سرچینه=تش_اشاره;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_سرچینه_حد=تش_اشاره;
        realFlush=خطأ;
        real_سرچینه_شاخص=0;
    } وإلا {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        إذا (!p_دلیلونه->سرچینه) {
            *err=U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        real_سرچینه=p_دلیلونه->سرچینه;
        real_سرچینه_حد=p_دلیلونه->سرچینه_حد;
        realFlush=p_دلیلونه->flush;
        real_سرچینه_شاخص=سرچینه_شاخص;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_دلیلونه->سرچینه=replay;
        p_دلیلونه->سرچینه_حد=replay-cnv->preFromULength;
        p_دلیلونه->flush=خطأ;
        سرچینه_شاخص=-1;

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
            fromUnicode(p_دلیلونه, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_پای_ټکی_د_ننوت=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_دلیلونه->flush && p_دلیلونه->سرچینه==p_دلیلونه->سرچینه_حد &&
                        cnv->fromUChar32==0);
        } وإلا {
            /* handle error from ucnv_convertEx() */
            converter_Saw_پای_ټکی_د_ننوت=خطأ;
        }

        /* no callback called yet for this iteration */
        calledCallback=خطأ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_ننوت_اوږدتیا=0;

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
            إذا(offsets!=تش_اشاره) {
                int32_t length = static_cast<int32_t>(p_دلیلونه->موخه - t);
                إذا(length>0) {
                    _updateOffsets(offsets, length, سرچینه_شاخص, error_ننوت_اوږدتیا);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_دلیلونه->offsets=offsets+=length;
                }

                إذا(سرچینه_شاخص>=0) {
                    سرچینه_شاخص += static_cast<int32_t>(p_دلیلونه->سرچینه - s);
                }
            }

            إذا(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                إذا(real_سرچینه==تش_اشاره) {
                    real_سرچینه=p_دلیلونه->سرچینه;
                    real_سرچینه_حد=p_دلیلونه->سرچینه_حد;
                    realFlush=p_دلیلونه->flush;
                    real_سرچینه_شاخص=سرچینه_شاخص;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_دلیلونه->سرچینه=replay;
                    p_دلیلونه->سرچینه_حد=replay-cnv->preFromULength;
                    p_دلیلونه->flush=خطأ;
                    إذا((سرچینه_شاخص+=cnv->preFromULength)<0) {
                        سرچینه_شاخص=-1;
                    }

                    cnv->preFromULength=0;
                } وإلا {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_سرچینه==تش_اشاره);
                    *err=U_دننی_پروګرام_ERROR;
                }
            }

            /* update pointers */
            s=p_دلیلونه->سرچینه;
            t=p_دلیلونه->موخه;

            إذا(U_SUCCESS(*err)) {
                إذا(s<p_دلیلونه->سرچینه_حد) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    اكسر;
                } وإلا إذا(real_سرچینه!=تش_اشاره) {
                    /* switch back from replaying to the real source and continue */
                    p_دلیلونه->سرچینه=real_سرچینه;
                    p_دلیلونه->سرچینه_حد=real_سرچینه_حد;
                    p_دلیلونه->flush=realFlush;
                    سرچینه_شاخص=real_سرچینه_شاخص;

                    real_سرچینه=تش_اشاره;
                    اكسر;
                } وإلا إذا(p_دلیلونه->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_وموندل;
                    calledCallback=خطأ; /* new error condition */
                } وإلا {
                    /* input consumed */
                    إذا(p_دلیلونه->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        إذا(!converter_Saw_پای_ټکی_د_ننوت) {
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
                UError_کوډ e;

                إذا( calledCallback ||
                    (e=*err)==U_بفر_ډکېدنه_ERROR ||
                    (e!=U_INVALID_CHAR_وموندل &&
                     e!=U_ILLEGAL_CHAR_وموندل &&
                     e!=U_TRUNCATED_CHAR_وموندل)
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
                    إذا(real_سرچینه!=تش_اشاره) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_دلیلونه->سرچینه_حد - p_دلیلونه->سرچینه);
                        إذا(length>0) {
                            u_memcpy(cnv->preFromU, p_دلیلونه->سرچینه, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_دلیلونه->سرچینه=real_سرچینه;
                        p_دلیلونه->سرچینه_حد=real_سرچینه_حد;
                        p_دلیلونه->flush=realFlush;
                    }

                    أرجع;
                }
            }

            /* callback handling */
            {
                UChar32 کوډ_ټکی;

                /* get and write the code point */
                کوډ_ټکی=cnv->fromUChar32;
                error_ننوت_اوږدتیا=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_بفر, error_ننوت_اوږدتیا, کوډ_ټکی);
                cnv->invalid_UChar_اوږدتیا = static_cast<int8_t>(error_ننوت_اوږدتیا);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_دلیلونه,
                    cnv->invalid_UChar_بفر, error_ننوت_اوږدتیا, کوډ_ټکی,
                    *err==U_INVALID_CHAR_وموندل ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
                    err);
            }

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=ریښتیا;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
ساكن UBool
ucnv_output_ډکېدنه_From_Unicode(UConverter *cnv,
                               حرف **موخه, ثابت حرف *موخه_حد,
                               int32_t **pOffsets,
                               UError_کوډ *err) {
    int32_t *offsets;
    حرف *ډکېدنه, *t;
    int32_t i, length;

    t=*موخه;
    إذا(pOffsets!=تش_اشاره) {
        offsets=*pOffsets;
    } وإلا {
        offsets=تش_اشاره;
    }

    ډکېدنه = reinterpret_cast<حرف*>(cnv->char_Error_بفر);
    length=cnv->char_Error_بفر_اوږدتیا;
    i=0;
    طالما(i<length) {
        إذا(t==موخه_حد) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            افعل {
                ډکېدنه[j++]=ډکېدنه[i++];
            } طالما(i<length);

            cnv->char_Error_بفر_اوږدتیا = static_cast<int8_t>(j);
            *موخه=t;
            إذا(offsets!=تش_اشاره) {
                *pOffsets=offsets;
            }
            *err=U_بفر_ډکېدنه_ERROR;
            أرجع ریښتیا;
        }

        /* copy the overflow contents to the target */
        *t++=ډکېدنه[i++];
        إذا(offsets!=تش_اشاره) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_بفر_اوږدتیا=0;
    *موخه=t;
    إذا(offsets!=تش_اشاره) {
        *pOffsets=offsets;
    }
    أرجع خطأ;
}

U_CAPI فراغ U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 حرف **موخه, ثابت حرف *موخه_حد,
                 ثابت char16_t **سرچینه, ثابت char16_t *سرچینه_حد,
                 int32_t *offsets,
                 UBool flush,
                 UError_کوډ *err) {
    UConverter_From_Unicode_دلیلونه دلیلونه_2;
    ثابت char16_t *s;
    حرف *t;

    /* check parameters */
    إذا(err==تش_اشاره || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(cnv==تش_اشاره || موخه==تش_اشاره || سرچینه==تش_اشاره) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    s=*سرچینه;
    t=*موخه;

    إذا ((ثابت فراغ *)U_MAX_PTR(سرچینه_حد) == (ثابت فراغ *)سرچینه_حد) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        سرچینه_حد = (ثابت char16_t *)(((ثابت حرف *)سرچینه_حد) - 1);
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
    إذا (سرچینه_حد<s || موخه_حد<t ||
        ((size_t)(سرچینه_حد-s)>(size_t)0x3fffffff && سرچینه_حد>s) ||
        ((size_t)(موخه_حد-t)>(size_t)0x7fffffff && موخه_حد>t) ||
        (((ثابت حرف *)سرچینه_حد-(ثابت حرف *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    
    /* output the target overflow buffer */
    إذا( cnv->char_Error_بفر_اوږدتیا>0 &&
        ucnv_output_ډکېدنه_From_Unicode(cnv, موخه, موخه_حد, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        أرجع;
    }
    /* *target may have moved, therefore stop using t */

    إذا(!flush && s==سرچینه_حد && cnv->preFromULength>=0) {
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
    دلیلونه_2.converter=cnv;
    دلیلونه_2.flush=flush;
    دلیلونه_2.offsets=offsets;
    دلیلونه_2.سرچینه=s;
    دلیلونه_2.سرچینه_حد=سرچینه_حد;
    دلیلونه_2.موخه=*موخه;
    دلیلونه_2.موخه_حد=موخه_حد;
    دلیلونه_2.size=حجم(دلیلونه_2);

    _fromUnicodeWithCallback(&دلیلونه_2, err);

    *سرچینه=دلیلونه_2.سرچینه;
    *موخه=دلیلونه_2.موخه;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

ساكن فراغ
منزل_Unicode_With_Callback(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه, UError_کوډ *err) {
    UConverter_منزل_Unicode منزل_Unicode;
    UConverter *cnv;
    ثابت حرف *s;
    char16_t *t;
    int32_t *offsets;
    int32_t سرچینه_شاخص;
    int32_t error_ننوت_اوږدتیا;
    UBool converter_Saw_پای_ټکی_د_ننوت, calledCallback;

    /* variables for m:n conversion */
    حرف replay[UCNV_EXT_MAX_بایټونه];
    ثابت حرف *real_سرچینه, *real_سرچینه_حد;
    int32_t real_سرچینه_شاخص;
    UBool realFlush;

    cnv=p_دلیلونه->converter;
    s=p_دلیلونه->سرچینه;
    t=p_دلیلونه->موخه;
    offsets=p_دلیلونه->offsets;

    /* get the converter implementation function */
    سرچینه_شاخص=0;
    إذا(offsets==تش_اشاره) {
        منزل_Unicode=cnv->shared_معلومات->impl->منزل_Unicode;
    } وإلا {
        منزل_Unicode=cnv->shared_معلومات->impl->منزل_Unicode_With_Offsets;
        إذا(منزل_Unicode==تش_اشاره) {
            /* there is no WithOffsets implementation */
            منزل_Unicode=cnv->shared_معلومات->impl->منزل_Unicode;
            /* we will write -1 for each offset */
            سرچینه_شاخص=-1;
        }
    }

    إذا(cnv->pre_منزل_ULength>=0) {
        /* normal mode */
        real_سرچینه=تش_اشاره;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_سرچینه_حد=تش_اشاره;
        realFlush=خطأ;
        real_سرچینه_شاخص=0;
    } وإلا {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        إذا (!p_دلیلونه->سرچینه) {
            *err=U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        real_سرچینه=p_دلیلونه->سرچینه;
        real_سرچینه_حد=p_دلیلونه->سرچینه_حد;
        realFlush=p_دلیلونه->flush;
        real_سرچینه_شاخص=سرچینه_شاخص;

        uprv_memcpy(replay, cnv->pre_منزل_U, -cnv->pre_منزل_ULength);
        p_دلیلونه->سرچینه=replay;
        p_دلیلونه->سرچینه_حد=replay-cnv->pre_منزل_ULength;
        p_دلیلونه->flush=خطأ;
        سرچینه_شاخص=-1;

        cnv->pre_منزل_ULength=0;
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
            منزل_Unicode(p_دلیلونه, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_پای_ټکی_د_ننوت=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_دلیلونه->flush && p_دلیلونه->سرچینه==p_دلیلونه->سرچینه_حد &&
                        cnv->منزل_ULength==0);
        } وإلا {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_پای_ټکی_د_ننوت=خطأ;
        }

        /* no callback called yet for this iteration */
        calledCallback=خطأ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_ننوت_اوږدتیا=0;

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
            إذا(offsets!=تش_اشاره) {
                int32_t length = static_cast<int32_t>(p_دلیلونه->موخه - t);
                إذا(length>0) {
                    _updateOffsets(offsets, length, سرچینه_شاخص, error_ننوت_اوږدتیا);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_دلیلونه->offsets=offsets+=length;
                }

                إذا(سرچینه_شاخص>=0) {
                    سرچینه_شاخص += static_cast<int32_t>(p_دلیلونه->سرچینه - s);
                }
            }

            إذا(cnv->pre_منزل_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                إذا(real_سرچینه==تش_اشاره) {
                    real_سرچینه=p_دلیلونه->سرچینه;
                    real_سرچینه_حد=p_دلیلونه->سرچینه_حد;
                    realFlush=p_دلیلونه->flush;
                    real_سرچینه_شاخص=سرچینه_شاخص;

                    uprv_memcpy(replay, cnv->pre_منزل_U, -cnv->pre_منزل_ULength);
                    p_دلیلونه->سرچینه=replay;
                    p_دلیلونه->سرچینه_حد=replay-cnv->pre_منزل_ULength;
                    p_دلیلونه->flush=خطأ;
                    إذا((سرچینه_شاخص+=cnv->pre_منزل_ULength)<0) {
                        سرچینه_شاخص=-1;
                    }

                    cnv->pre_منزل_ULength=0;
                } وإلا {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_سرچینه==تش_اشاره);
                    *err=U_دننی_پروګرام_ERROR;
                }
            }

            /* update pointers */
            s=p_دلیلونه->سرچینه;
            t=p_دلیلونه->موخه;

            إذا(U_SUCCESS(*err)) {
                إذا(s<p_دلیلونه->سرچینه_حد) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    اكسر;
                } وإلا إذا(real_سرچینه!=تش_اشاره) {
                    /* switch back from replaying to the real source and continue */
                    p_دلیلونه->سرچینه=real_سرچینه;
                    p_دلیلونه->سرچینه_حد=real_سرچینه_حد;
                    p_دلیلونه->flush=realFlush;
                    سرچینه_شاخص=real_سرچینه_شاخص;

                    real_سرچینه=تش_اشاره;
                    اكسر;
                } وإلا إذا(p_دلیلونه->flush && cnv->منزل_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_وموندل;
                    calledCallback=خطأ; /* new error condition */
                } وإلا {
                    /* input consumed */
                    إذا(p_دلیلونه->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        إذا(!converter_Saw_پای_ټکی_د_ننوت) {
                            اكسر;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_منزل_UNICODE, خطأ);
                    }

                    /* done successfully */
                    أرجع;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_کوډ e;

                إذا( calledCallback ||
                    (e=*err)==U_بفر_ډکېدنه_ERROR ||
                    (e!=U_INVALID_CHAR_وموندل &&
                     e!=U_ILLEGAL_CHAR_وموندل &&
                     e!=U_TRUNCATED_CHAR_وموندل &&
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
                    إذا(real_سرچینه!=تش_اشاره) {
                        int32_t length;

                        U_ASSERT(cnv->pre_منزل_ULength==0);

                        length = static_cast<int32_t>(p_دلیلونه->سرچینه_حد - p_دلیلونه->سرچینه);
                        إذا(length>0) {
                            uprv_memcpy(cnv->pre_منزل_U, p_دلیلونه->سرچینه, length);
                            cnv->pre_منزل_ULength = static_cast<int8_t>(-length);
                        }

                        p_دلیلونه->سرچینه=real_سرچینه;
                        p_دلیلونه->سرچینه_حد=real_سرچینه_حد;
                        p_دلیلونه->flush=realFlush;
                    }

                    أرجع;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_ننوت_اوږدتیا=cnv->invalid_Char_اوږدتیا=cnv->منزل_ULength;
            إذا(error_ننوت_اوږدتیا>0) {
                uprv_memcpy(cnv->invalid_Char_بفر, cnv->منزل_UBytes, error_ننوت_اوږدتیا);
            }

            /* set the converter state to deal with the next character */
            cnv->منزل_ULength=0;

            /* call the callback function */
            إذا(cnv->منزل_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_وموندل) {
                cnv->منزل_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->منزل_UContext, p_دلیلونه,
                cnv->invalid_Char_بفر, error_ننوت_اوږدتیا,
                cnv->منزل_UCallback_Reason,
                err);
            cnv->منزل_UCallback_Reason = UCNV_ILLEGAL; /* reset to default value */

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=ریښتیا;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
ساكن UBool
ucnv_output_ډکېدنه_منزل_Unicode(UConverter *cnv,
                             char16_t **موخه, ثابت char16_t *موخه_حد,
                             int32_t **pOffsets,
                             UError_کوډ *err) {
    int32_t *offsets;
    char16_t *ډکېدنه, *t;
    int32_t i, length;

    t=*موخه;
    إذا(pOffsets!=تش_اشاره) {
        offsets=*pOffsets;
    } وإلا {
        offsets=تش_اشاره;
    }

    ډکېدنه=cnv->UChar_Error_بفر;
    length=cnv->UChar_Error_بفر_اوږدتیا;
    i=0;
    طالما(i<length) {
        إذا(t==موخه_حد) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            افعل {
                ډکېدنه[j++]=ډکېدنه[i++];
            } طالما(i<length);

            cnv->UChar_Error_بفر_اوږدتیا = static_cast<int8_t>(j);
            *موخه=t;
            إذا(offsets!=تش_اشاره) {
                *pOffsets=offsets;
            }
            *err=U_بفر_ډکېدنه_ERROR;
            أرجع ریښتیا;
        }

        /* copy the overflow contents to the target */
        *t++=ډکېدنه[i++];
        إذا(offsets!=تش_اشاره) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_بفر_اوږدتیا=0;
    *موخه=t;
    إذا(offsets!=تش_اشاره) {
        *pOffsets=offsets;
    }
    أرجع خطأ;
}

U_CAPI فراغ U_EXPORT2
ucnv_منزل_Unicode(UConverter *cnv,
               char16_t **موخه, ثابت char16_t *موخه_حد,
               ثابت حرف **سرچینه, ثابت حرف *سرچینه_حد,
               int32_t *offsets,
               UBool flush,
               UError_کوډ *err) {
    UConverter_منزل_Unicode_دلیلونه دلیلونه_2;
    ثابت حرف *s;
    char16_t *t;

    /* check parameters */
    إذا(err==تش_اشاره || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(cnv==تش_اشاره || موخه==تش_اشاره || سرچینه==تش_اشاره) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    s=*سرچینه;
    t=*موخه;

    إذا ((ثابت فراغ *)U_MAX_PTR(موخه_حد) == (ثابت فراغ *)موخه_حد) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        موخه_حد = (ثابت char16_t *)(((ثابت حرف *)موخه_حد) - 1);
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
    إذا (سرچینه_حد<s || موخه_حد<t ||
        ((size_t)(سرچینه_حد-s)>(size_t)0x7fffffff && سرچینه_حد>s) ||
        ((size_t)(موخه_حد-t)>(size_t)0x3fffffff && موخه_حد>t) ||
        (((ثابت حرف *)موخه_حد-(ثابت حرف *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    
    /* output the target overflow buffer */
    إذا( cnv->UChar_Error_بفر_اوږدتیا>0 &&
        ucnv_output_ډکېدنه_منزل_Unicode(cnv, موخه, موخه_حد, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        أرجع;
    }
    /* *target may have moved, therefore stop using t */

    إذا(!flush && s==سرچینه_حد && cnv->pre_منزل_ULength>=0) {
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
    دلیلونه_2.converter=cnv;
    دلیلونه_2.flush=flush;
    دلیلونه_2.offsets=offsets;
    دلیلونه_2.سرچینه=s;
    دلیلونه_2.سرچینه_حد=سرچینه_حد;
    دلیلونه_2.موخه=*موخه;
    دلیلونه_2.موخه_حد=موخه_حد;
    دلیلونه_2.size=حجم(دلیلونه_2);

    منزل_Unicode_With_Callback(&دلیلونه_2, err);

    *سرچینه=دلیلونه_2.سرچینه;
    *موخه=دلیلونه_2.موخه;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                حرف *dest, int32_t dest_ظرفیت,
                ثابت char16_t *src, int32_t src_اوږدتیا,
                UError_کوډ *p_Error_کوډ) {
    ثابت char16_t *src_حد;
    حرف *originalDest, *dest_حد;
    int32_t dest_اوږدتیا;

    /* check arguments */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    إذا( cnv==تش_اشاره ||
        dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==تش_اشاره) ||
        src_اوږدتیا<-1 || (src_اوږدتیا!=0 && src==تش_اشاره)
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    إذا(src_اوږدتیا==-1) {
        src_اوږدتیا=u_strlen(src);
    }
    إذا(src_اوږدتیا>0) {
        src_حد=src+src_اوږدتیا;
        dest_ظرفیت=pin_ظرفیت(dest, dest_ظرفیت);
        dest_حد=dest+dest_ظرفیت;

        /* perform the conversion */
        UError_کوډ بفر_حالت = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_حد, &src, src_حد, تش_اشاره, ریښتیا, &بفر_حالت);
        dest_اوږدتیا=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        إذا(بفر_حالت==U_بفر_ډکېدنه_ERROR) {
            حرف بفر[1024];

            dest_حد=بفر+حجم(بفر);
            افعل {
                dest=بفر;
                بفر_حالت=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_حد, &src, src_حد, تش_اشاره, ریښتیا, &بفر_حالت);
                dest_اوږدتیا+=(int32_t)(dest-بفر);
            } طالما(بفر_حالت==U_بفر_ډکېدنه_ERROR);
        }
        إذا (U_FAILURE(بفر_حالت)) {
            *p_Error_کوډ = بفر_حالت;
        }
    } وإلا {
        dest_اوږدتیا=0;
    }

    أرجع u_terminateChars(originalDest, dest_ظرفیت, dest_اوږدتیا, p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_ظرفیت,
              ثابت حرف *src, int32_t src_اوږدتیا,
              UError_کوډ *p_Error_کوډ) {
    ثابت حرف *src_حد;
    char16_t *originalDest, *dest_حد;
    int32_t dest_اوږدتیا;

    /* check arguments */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    إذا( cnv==تش_اشاره ||
        dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==تش_اشاره) ||
        src_اوږدتیا<-1 || (src_اوږدتیا!=0 && src==تش_اشاره))
    {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* initialize */
    ucnv_reset_منزل_Unicode(cnv);
    originalDest=dest;
    إذا(src_اوږدتیا==-1) {
        src_اوږدتیا=(int32_t)uprv_strlen(src);
    }
    إذا(src_اوږدتیا>0) {
        src_حد=src+src_اوږدتیا;
        dest_ظرفیت=pin_ظرفیت(dest, dest_ظرفیت);
        dest_حد=dest+dest_ظرفیت;

        /* perform the conversion */
        UError_کوډ بفر_حالت = U_ZERO_ERROR;
        ucnv_منزل_Unicode(cnv, &dest, dest_حد, &src, src_حد, تش_اشاره, ریښتیا, &بفر_حالت);
        dest_اوږدتیا=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        إذا(بفر_حالت==U_بفر_ډکېدنه_ERROR)
        {
            char16_t بفر[1024];

            dest_حد=بفر+UPRV_LENGTHOF(بفر);
            افعل {
                dest=بفر;
                بفر_حالت=U_ZERO_ERROR;
                ucnv_منزل_Unicode(cnv, &dest, dest_حد, &src, src_حد, تش_اشاره, ریښتیا, &بفر_حالت);
                dest_اوږدتیا+=(int32_t)(dest-بفر);
            }
            طالما(بفر_حالت==U_بفر_ډکېدنه_ERROR);
        }
        إذا (U_FAILURE(بفر_حالت)) {
            *p_Error_کوډ = بفر_حالت;
        }
    } وإلا {
        dest_اوږدتیا=0;
    }

    أرجع u_terminateUChars(originalDest, dest_ظرفیت, dest_اوږدتیا, p_Error_کوډ);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_بل_UChar(UConverter *cnv,
                  ثابت حرف **سرچینه, ثابت حرف *سرچینه_حد,
                  UError_کوډ *err) {
    UConverter_منزل_Unicode_دلیلونه دلیلونه_2;
    char16_t بفر[U16_MAX_اوږدتیا];
    ثابت حرف *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    إذا(err==تش_اشاره || U_FAILURE(*err)) {
        أرجع 0xffff;
    }

    إذا(cnv==تش_اشاره || سرچینه==تش_اشاره) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع 0xffff;
    }

    s=*سرچینه;
    إذا(سرچینه_حد<s) {
        *err=U_ILLEGAL_دلیل_ERROR;
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
    إذا(((size_t)(سرچینه_حد-s)>(size_t)0x7fffffff && سرچینه_حد>s)) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    إذا(cnv->UChar_Error_بفر_اوږدتیا>0) {
        char16_t *ډکېدنه;

        ډکېدنه=cnv->UChar_Error_بفر;
        i=0;
        length=cnv->UChar_Error_بفر_اوږدتیا;
        U16_بل(ډکېدنه, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        إذا((cnv->UChar_Error_بفر_اوږدتیا=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_بفر, cnv->UChar_Error_بفر+i,
                         cnv->UChar_Error_بفر_اوږدتیا*U_SIZEOF_UCHAR);
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
    دلیلونه_2.converter=cnv;
    دلیلونه_2.flush=ریښتیا;
    دلیلونه_2.offsets=تش_اشاره;
    دلیلونه_2.سرچینه=s;
    دلیلونه_2.سرچینه_حد=سرچینه_حد;
    دلیلونه_2.موخه=بفر;
    دلیلونه_2.موخه_حد=بفر+1;
    دلیلونه_2.size=حجم(دلیلونه_2);

    إذا(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        إذا(cnv->منزل_ULength==0 && cnv->shared_معلومات->impl->get_بل_UChar!=تش_اشاره) {
            c=cnv->shared_معلومات->impl->get_بل_UChar(&دلیلونه_2, err);
            *سرچینه=s=دلیلونه_2.سرچینه;
            إذا(*err==U_شاخص_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_منزل_UNICODE, خطأ);
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
        منزل_Unicode_With_Callback(&دلیلونه_2, err);

        إذا(*err==U_بفر_ډکېدنه_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(دلیلونه_2.موخه-بفر);
    } وإلا {
        /* write the lead surrogate from the overflow buffer */
        بفر[0]=(char16_t)c;
        دلیلونه_2.موخه=بفر+1;
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
        c=بفر[0];
        i=1;
        إذا(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } وإلا {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            إذا(cnv->UChar_Error_بفر_اوږدتیا>0) {
                /* got overflow output from the conversion */
                إذا(U16_IS_TRAIL(c2=cnv->UChar_Error_بفر[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    إذا((--cnv->UChar_Error_بفر_اوږدتیا)>0) {
                        uprv_memmove(cnv->UChar_Error_بفر, cnv->UChar_Error_بفر+1,
                                     cnv->UChar_Error_بفر_اوږدتیا*U_SIZEOF_UCHAR);
                    }
                } وإلا {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } وإلا إذا(دلیلونه_2.سرچینه<سرچینه_حد) {
                /* convert once more, to buffer[1] */
                دلیلونه_2.موخه_حد=بفر+2;
                منزل_Unicode_With_Callback(&دلیلونه_2, err);
                إذا(*err==U_بفر_ډکېدنه_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(دلیلونه_2.موخه-بفر);
                إذا(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=بفر[1])) {
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
        إذا((length=cnv->UChar_Error_بفر_اوږدتیا)>0) {
            uprv_memmove(cnv->UChar_Error_بفر+delta, cnv->UChar_Error_بفر,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_بفر_اوږدتیا=(int8_t)(length+delta);

        cnv->UChar_Error_بفر[0]=بفر[i++];
        إذا(delta>1) {
            cnv->UChar_Error_بفر[1]=بفر[i];
        }
    }

    *سرچینه=دلیلونه_2.سرچینه;
    أرجع c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI فراغ U_EXPORT2
ucnv_convertEx(UConverter *موخه_Cnv, UConverter *سرچینه_Cnv,
               حرف **موخه, ثابت حرف *موخه_حد,
               ثابت حرف **سرچینه, ثابت حرف *سرچینه_حد,
               char16_t *محور_پیل, char16_t **محور_سرچینه,
               char16_t **محور_موخه, ثابت char16_t *محور_حد,
               UBool reset, UBool flush,
               UError_کوډ *p_Error_کوډ) {
    char16_t محور_بفر[CHUNK_اندازه];
    ثابت char16_t *my_محور_سرچینه;
    char16_t *my_محور_موخه;
    ثابت حرف *s;
    حرف *t;

    UConverter_منزل_Unicode_دلیلونه منزل_UArgs;
    UConverter_From_Unicode_دلیلونه fromUArgs;
    UConverterConvert convert;

    /* error checking */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }

    إذا( موخه_Cnv==تش_اشاره || سرچینه_Cnv==تش_اشاره ||
        سرچینه==تش_اشاره || *سرچینه==تش_اشاره ||
        موخه==تش_اشاره || *موخه==تش_اشاره || موخه_حد==تش_اشاره
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    s=*سرچینه;
    t=*موخه;
    إذا((سرچینه_حد!=تش_اشاره && سرچینه_حد<s) || موخه_حد<t) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    إذا(
        (سرچینه_حد!=تش_اشاره && ((size_t)(سرچینه_حد-s)>(size_t)0x7fffffff && سرچینه_حد>s)) ||
        ((size_t)(موخه_حد-t)>(size_t)0x7fffffff && موخه_حد>t)
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    
    إذا(محور_پیل==تش_اشاره) {
        إذا(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }

        /* use the stack pivot buffer */
        my_محور_سرچینه=my_محور_موخه=محور_پیل=محور_بفر;
        محور_سرچینه=(char16_t **)&my_محور_سرچینه;
        محور_موخه=&my_محور_موخه;
        محور_حد=محور_بفر+CHUNK_اندازه;
    } وإلا إذا(  محور_پیل>=محور_حد ||
                محور_سرچینه==تش_اشاره || *محور_سرچینه==تش_اشاره ||
                محور_موخه==تش_اشاره || *محور_موخه==تش_اشاره ||
                محور_حد==تش_اشاره
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا(سرچینه_حد==تش_اشاره) {
        /* get limit of single-byte-NUL-terminated source string */
        سرچینه_حد=uprv_strchr(*سرچینه, 0);
    }

    إذا(reset) {
        ucnv_reset_منزل_Unicode(سرچینه_Cnv);
        ucnv_resetFromUnicode(موخه_Cnv);
        *محور_سرچینه=*محور_موخه=محور_پیل;
    } وإلا إذا(موخه_Cnv->char_Error_بفر_اوږدتیا>0) {
        /* output the targetCnv overflow buffer */
        إذا(ucnv_output_ډکېدنه_From_Unicode(موخه_Cnv, موخه, موخه_حد, تش_اشاره, p_Error_کوډ)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            أرجع;
        }
        /* *target has moved, therefore stop using t */

        إذا( !flush &&
            موخه_Cnv->preFromULength>=0 && *محور_سرچینه==*محور_موخه &&
            سرچینه_Cnv->UChar_Error_بفر_اوږدتیا==0 && سرچینه_Cnv->pre_منزل_ULength>=0 && s==سرچینه_حد
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            أرجع;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    إذا( سرچینه_Cnv->shared_معلومات->static_معلومات->conversionType==UCNV_UTF8 &&
        موخه_Cnv->shared_معلومات->impl->fromUTF8!=تش_اشاره
    ) {
        convert=موخه_Cnv->shared_معلومات->impl->fromUTF8;
    } وإلا إذا( موخه_Cnv->shared_معلومات->static_معلومات->conversionType==UCNV_UTF8 &&
               سرچینه_Cnv->shared_معلومات->impl->منزل_UTF8!=تش_اشاره
    ) {
        convert=سرچینه_Cnv->shared_معلومات->impl->منزل_UTF8;
    } وإلا {
        convert=تش_اشاره;
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
    إذا(convert!=تش_اشاره && (محور_حد-محور_پیل)>32) {
        محور_حد=محور_پیل+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=موخه_Cnv;
    fromUArgs.flush=خطأ;
    fromUArgs.offsets=تش_اشاره;
    fromUArgs.موخه=*موخه;
    fromUArgs.موخه_حد=موخه_حد;
    fromUArgs.size=حجم(fromUArgs);

    منزل_UArgs.converter=سرچینه_Cnv;
    منزل_UArgs.flush=flush;
    منزل_UArgs.offsets=تش_اشاره;
    منزل_UArgs.سرچینه=s;
    منزل_UArgs.سرچینه_حد=سرچینه_حد;
    منزل_UArgs.موخه_حد=محور_حد;
    منزل_UArgs.size=حجم(منزل_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=تش_اشاره;

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
        إذا( *محور_سرچینه<*محور_موخه ||
            U_FAILURE(*p_Error_کوډ) ||
            موخه_Cnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.سرچینه=*محور_سرچینه;
            fromUArgs.سرچینه_حد=*محور_موخه;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_کوډ);
            إذا(U_FAILURE(*p_Error_کوډ)) {
                /* target overflow, or conversion error */
                *محور_سرچینه=(char16_t *)fromUArgs.سرچینه;
                اكسر;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *محور_سرچینه=*محور_موخه=محور_پیل;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        إذا(سرچینه_Cnv->UChar_Error_بفر_اوږدتیا>0) {
            إذا(ucnv_output_ډکېدنه_منزل_Unicode(سرچینه_Cnv, محور_موخه, محور_حد, تش_اشاره, p_Error_کوډ)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_کوډ=U_ZERO_ERROR;
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
        إذا( منزل_UArgs.سرچینه==سرچینه_حد &&
            سرچینه_Cnv->pre_منزل_ULength>=0 && سرچینه_Cnv->منزل_ULength==0 &&
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
        إذا(convert!=تش_اشاره && موخه_Cnv->preFromUFirstCP<0 && سرچینه_Cnv->pre_منزل_ULength==0) {
            إذا(*p_Error_کوډ==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_کوډ=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &منزل_UArgs, p_Error_کوډ);
            إذا(*p_Error_کوډ==U_بفر_ډکېدنه_ERROR) {
                اكسر;
            } وإلا إذا(U_FAILURE(*p_Error_کوډ)) {
                إذا(سرچینه_Cnv->منزل_ULength>0) {
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
                    *محور_سرچینه=*محور_موخه=محور_پیل+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    استمر;
                }
            } وإلا إذا(*p_Error_کوډ==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_کوډ=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } وإلا إذا(flush && سرچینه_Cnv->منزل_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_کوډ=U_TRUNCATED_CHAR_وموندل;
            } وإلا {
                /* input consumed */
                إذا(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(سرچینه_Cnv, UCNV_RESET_منزل_UNICODE, خطأ);
                    _reset(موخه_Cnv, UCNV_RESET_FROM_UNICODE, خطأ);
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
        منزل_UArgs.موخه=محور_پیل; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        منزل_Unicode_With_Callback(&منزل_UArgs, p_Error_کوډ);
        *محور_موخه=منزل_UArgs.موخه;
        إذا(*p_Error_کوډ==U_بفر_ډکېدنه_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_کوډ=U_ZERO_ERROR;
        } وإلا إذا(U_FAILURE(*p_Error_کوډ) || (!flush && *محور_موخه==محور_پیل)) {
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
        إذا( flush && منزل_UArgs.سرچینه==سرچینه_حد &&
            سرچینه_Cnv->pre_منزل_ULength>=0 &&
            سرچینه_Cnv->UChar_Error_بفر_اوږدتیا==0
        ) {
            fromUArgs.flush=ریښتیا;
        }
    }

    /*
     * The conversion loop is exited when one of the following is true:
     * - the entire source text has been converted successfully to the target buffer
     * - a target buffer overflow occurred
     * - a conversion error occurred
     */

    *سرچینه=منزل_UArgs.سرچینه;
    *موخه=fromUArgs.موخه;

    /* terminate the target buffer if possible */
    إذا(flush && U_SUCCESS(*p_Error_کوډ)) {
        إذا(*موخه!=موخه_حد) {
            **موخه=0;
            إذا(*p_Error_کوډ==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_کوډ=U_ZERO_ERROR;
            }
        } وإلا {
            *p_Error_کوډ=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
ساكن int32_t
ucnv_دننی_Convert(UConverter *بهر_Converter, UConverter *دننه_Converter,
                     حرف *موخه, int32_t موخه_ظرفیت,
                     ثابت حرف *سرچینه, int32_t سرچینه_اوږدتیا,
                     UError_کوډ *p_Error_کوډ) {
    char16_t محور_بفر[CHUNK_اندازه];
    char16_t *محور, *pivot2;

    حرف *my_موخه;
    ثابت حرف *سرچینه_حد;
    ثابت حرف *موخه_حد;
    int32_t موخه_اوږدتیا=0;

    /* set up */
    إذا(سرچینه_اوږدتیا<0) {
        سرچینه_حد=uprv_strchr(سرچینه, 0);
    } وإلا {
        سرچینه_حد=سرچینه+سرچینه_اوږدتیا;
    }

    /* if there is no input data, we're done */
    إذا(سرچینه==سرچینه_حد) {
        أرجع u_terminateChars(موخه, موخه_ظرفیت, 0, p_Error_کوډ);
    }

    محور=pivot2=محور_بفر;
    my_موخه=موخه;
    موخه_اوږدتیا=0;

    إذا(موخه_ظرفیت>0) {
        /* perform real conversion */
        موخه_حد=موخه+موخه_ظرفیت;
        ucnv_convertEx(بهر_Converter, دننه_Converter,
                       &my_موخه, موخه_حد,
                       &سرچینه, سرچینه_حد,
                       محور_بفر, &محور, &pivot2, محور_بفر+CHUNK_اندازه,
                       خطأ,
                       ریښتیا,
                       p_Error_کوډ);
        موخه_اوږدتیا = static_cast<int32_t>(my_موخه - موخه);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    إذا(*p_Error_کوډ==U_بفر_ډکېدنه_ERROR || موخه_ظرفیت==0)
    {
        حرف موخه_بفر[CHUNK_اندازه];

        موخه_حد=موخه_بفر+CHUNK_اندازه;
        افعل {
            *p_Error_کوډ=U_ZERO_ERROR;
            my_موخه=موخه_بفر;
            ucnv_convertEx(بهر_Converter, دننه_Converter,
                           &my_موخه, موخه_حد,
                           &سرچینه, سرچینه_حد,
                           محور_بفر, &محور, &pivot2, محور_بفر+CHUNK_اندازه,
                           خطأ,
                           ریښتیا,
                           p_Error_کوډ);
            موخه_اوږدتیا += static_cast<int32_t>(my_موخه - موخه_بفر);
        } طالما(*p_Error_کوډ==U_بفر_ډکېدنه_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        أرجع u_terminateChars(موخه, موخه_ظرفیت, موخه_اوږدتیا, p_Error_کوډ);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    أرجع موخه_اوږدتیا;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ثابت حرف *منزل_Converter_Name, ثابت حرف *fromConverterName,
             حرف *موخه, int32_t موخه_ظرفیت,
             ثابت حرف *سرچینه, int32_t سرچینه_اوږدتیا,
             UError_کوډ *p_Error_کوډ) {
    UConverter in, out; /* stack-allocated */
    UConverter *دننه_Converter, *بهر_Converter;
    int32_t موخه_اوږدتیا;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    إذا( سرچینه==تش_اشاره || سرچینه_اوږدتیا<-1 ||
        موخه_ظرفیت<0 || (موخه_ظرفیت>0 && موخه==تش_اشاره)
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* if there is no input data, we're done */
    إذا(سرچینه_اوږدتیا==0 || (سرچینه_اوږدتیا<0 && *سرچینه==0)) {
        أرجع u_terminateChars(موخه, موخه_ظرفیت, 0, p_Error_کوډ);
    }

    /* create the converters */
    دننه_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    بهر_Converter=ucnv_createConverter(&out, منزل_Converter_Name, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        ucnv_close(دننه_Converter);
        أرجع 0;
    }

    موخه_اوږدتیا=ucnv_دننی_Convert(بهر_Converter, دننه_Converter,
                                      موخه, موخه_ظرفیت,
                                      سرچینه, سرچینه_اوږدتیا,
                                      p_Error_کوډ);

    ucnv_close(دننه_Converter);
    ucnv_close(بهر_Converter);

    أرجع موخه_اوږدتیا;
}

/* @internal */
ساكن int32_t
ucnv_convertAlgorithmic(UBool convert_منزل_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        حرف *موخه, int32_t موخه_ظرفیت,
                        ثابت حرف *سرچینه, int32_t سرچینه_اوږدتیا,
                        UError_کوډ *p_Error_کوډ) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *منزل, *from;
    int32_t موخه_اوږدتیا;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    إذا( cnv==تش_اشاره || سرچینه==تش_اشاره || سرچینه_اوږدتیا<-1 ||
        موخه_ظرفیت<0 || (موخه_ظرفیت>0 && موخه==تش_اشاره)
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* if there is no input data, we're done */
    إذا(سرچینه_اوږدتیا==0 || (سرچینه_اوږدتیا<0 && *سرچینه==0)) {
        أرجع u_terminateChars(موخه, موخه_ظرفیت, 0, p_Error_کوډ);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* reset the other converter */
    إذا(convert_منزل_Algorithmic) {
        /* cnv->Unicode->algo */
        ucnv_reset_منزل_Unicode(cnv);
        منزل=algoConverter;
        from=cnv;
    } وإلا {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        منزل=cnv;
    }

    موخه_اوږدتیا=ucnv_دننی_Convert(منزل, from,
                                      موخه, موخه_ظرفیت,
                                      سرچینه, سرچینه_اوږدتیا,
                                      p_Error_کوډ);

    ucnv_close(algoConverter);

    أرجع موخه_اوږدتیا;
}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   حرف *موخه, int32_t موخه_ظرفیت,
                   ثابت حرف *سرچینه, int32_t سرچینه_اوږدتیا,
                   UError_کوډ *p_Error_کوډ) {
    أرجع ucnv_convertAlgorithmic(ریښتیا, algorithmicType, cnv,
                                   موخه, موخه_ظرفیت,
                                   سرچینه, سرچینه_اوږدتیا,
                                   p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     حرف *موخه, int32_t موخه_ظرفیت,
                     ثابت حرف *سرچینه, int32_t سرچینه_اوږدتیا,
                     UError_کوډ *p_Error_کوډ) UPRV_NO_SANITIZE_UNDEFINED {

    إذا(algorithmicType<0 || UCNV_شمېره_د_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    أرجع ucnv_convertAlgorithmic(خطأ, algorithmicType, cnv,
                                   موخه, موخه_ظرفیت,
                                   سرچینه, سرچینه_اوږدتیا,
                                   p_Error_کوډ);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ثابت UConverter* converter)
{
    int8_t type = converter->shared_معلومات->static_معلومات->conversionType;
#إذا !UCONFIG_NO_LEGACY_CONVERSION
    إذا(type == UCNV_MBCS) {
        أرجع ucnv_MBCSGetType(converter);
    }
#د_که_پای
    أرجع (UConverterType)type;
}

U_CAPI فراغ  U_EXPORT2
ucnv_getStarters(ثابت UConverter* converter, 
                 UBool starters[256],
                 UError_کوډ* err)
{
    إذا (err == تش_اشاره || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(converter->shared_معلومات->impl->getStarters != تش_اشاره) {
        converter->shared_معلومات->impl->getStarters(converter, starters, err);
    } وإلا {
        *err = U_ILLEGAL_دلیل_ERROR;
    }
}

ساكن ثابت UAmbiguousConverter *ucnv_getAmbiguous(ثابت UConverter *cnv)
{
    UError_کوډ error_کوډ;
    ثابت حرف *name;
    int32_t i;

    إذا(cnv==تش_اشاره) {
        أرجع تش_اشاره;
    }

    error_کوډ=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }

    لكل(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        إذا(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            أرجع ambiguousConverters+i;
        }
    }

    أرجع تش_اشاره;
}

U_CAPI فراغ  U_EXPORT2
ucnv_fixFileSeparator(ثابت UConverter *cnv, 
                      char16_t* سرچینه,
                      int32_t سرچینه_اوږدتیا) {
    ثابت UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    إذا(cnv==تش_اشاره || سرچینه==تش_اشاره || سرچینه_اوږدتیا<=0 || (a=ucnv_getAmbiguous(cnv))==تش_اشاره)
    {
        أرجع;
    }

    variant5c=a->variant5c;
    لكل(i=0; i<سرچینه_اوږدتیا; ++i) {
        إذا(سرچینه[i]==variant5c) {
            سرچینه[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ثابت UConverter *cnv) {
    أرجع ucnv_getAmbiguous(cnv)!=تش_اشاره;
}

U_CAPI فراغ  U_EXPORT2
ucnv_سېټ_Fallback(UConverter *cnv, UBool کاروي_Fallback)
{
    cnv->useFallback = کاروي_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_کاروي_Fallback(ثابت UConverter *cnv)
{
    أرجع cnv->useFallback;
}

U_CAPI فراغ  U_EXPORT2
ucnv_getInvalidChars (ثابت UConverter * converter,
                      حرف *err_بایټونه,
                      int8_t * len,
                      UError_کوډ * err)
{
    إذا (err == تش_اشاره || U_FAILURE(*err))
    {
        أرجع;
    }
    إذا (len == تش_اشاره || err_بایټونه == تش_اشاره || converter == تش_اشاره)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (*len < converter->invalid_Char_اوږدتیا)
    {
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    إذا ((*len = converter->invalid_Char_اوږدتیا) > 0)
    {
        uprv_memcpy (err_بایټونه, converter->invalid_Char_بفر, *len);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_getInvalidUChars (ثابت UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_کوډ * err)
{
    إذا (err == تش_اشاره || U_FAILURE(*err))
    {
        أرجع;
    }
    إذا (len == تش_اشاره || errChars == تش_اشاره || converter == تش_اشاره)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (*len < converter->invalid_UChar_اوږدتیا)
    {
        *err = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    إذا ((*len = converter->invalid_UChar_اوږدتیا) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_بفر, *len);
    }
}

#تعریف_کړه SIG_MAX_LEN 5

U_CAPI ثابت حرف* U_EXPORT2
ucnv_detectUnicodeSignature( ثابت حرف* سرچینه,
                             int32_t سرچینه_اوږدتیا,
                             int32_t* signature_اوږدتیا,
                             UError_کوډ* p_Error_کوډ) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    حرف پیل[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    صحيح i = 0;

    إذا((p_Error_کوډ==تش_اشاره) || U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }
    
    إذا(سرچینه == تش_اشاره || سرچینه_اوږدتیا < -1){
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    إذا(signature_اوږدتیا == تش_اشاره) {
        signature_اوږدتیا = &dummy;
    }

    إذا(سرچینه_اوږدتیا==-1){
        سرچینه_اوږدتیا=(int32_t)uprv_strlen(سرچینه);
    }

    
    طالما(i<سرچینه_اوږدتیا&& i<SIG_MAX_LEN){
        پیل[i]=سرچینه[i];
        i++;
    }

    إذا(پیل[0] == '\xFE' && پیل[1] == '\xFF') {
        *signature_اوږدتیا=2;
        أرجع  "UTF-16BE";
    } وإلا إذا(پیل[0] == '\xFF' && پیل[1] == '\xFE') {
        إذا(پیل[2] == '\x00' && پیل[3] =='\x00') {
            *signature_اوږدتیا=4;
            أرجع "UTF-32LE";
        } وإلا {
            *signature_اوږدتیا=2;
            أرجع  "UTF-16LE";
        }
    } وإلا إذا(پیل[0] == '\xEF' && پیل[1] == '\xBB' && پیل[2] == '\xBF') {
        *signature_اوږدتیا=3;
        أرجع  "UTF-8";
    } وإلا إذا(پیل[0] == '\x00' && پیل[1] == '\x00' && 
              پیل[2] == '\xFE' && پیل[3]=='\xFF') {
        *signature_اوږدتیا=4;
        أرجع  "UTF-32BE";
    } وإلا إذا(پیل[0] == '\x0E' && پیل[1] == '\xFE' && پیل[2] == '\xFF') {
        *signature_اوږدتیا=3;
        أرجع "SCSU";
    } وإلا إذا(پیل[0] == '\xFB' && پیل[1] == '\xEE' && پیل[2] == '\x28') {
        *signature_اوږدتیا=3;
        أرجع "BOCU-1";
    } وإلا إذا(پیل[0] == '\x2B' && پیل[1] == '\x2F' && پیل[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        إذا(پیل[3] == '\x38' && پیل[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_اوږدتیا=5;
            أرجع "UTF-7";
        } وإلا إذا(پیل[3] == '\x38' || پیل[3] == '\x39' || پیل[3] == '\x2B' || پیل[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_اوږدتیا=4;
            أرجع "UTF-7";
        }
    }وإلا إذا(پیل[0]=='\xDD' && پیل[1]== '\x73'&& پیل[2]=='\x66' && پیل[3]=='\x73'){
        *signature_اوږدتیا=4;
        أرجع "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_اوږدتیا=0;
    أرجع تش_اشاره;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ثابت UConverter* cnv, UError_کوډ* حالت)
{
    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع -1;
    }
    إذا(cnv == تش_اشاره){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع -1;
    }

    إذا(cnv->preFromUFirstCP >= 0){
        أرجع U16_اوږدتیا(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }وإلا إذا(cnv->preFromULength < 0){
        أرجع -cnv->preFromULength ;
    }وإلا إذا(cnv->fromUChar32 > 0){
        أرجع 1;
    }
    أرجع 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_UCount_Pending(ثابت UConverter* cnv, UError_کوډ* حالت){

    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع -1;
    }
    إذا(cnv == تش_اشاره){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع -1;
    }

    إذا(cnv->pre_منزل_ULength > 0){
        أرجع cnv->pre_منزل_ULength ;
    }وإلا إذا(cnv->pre_منزل_ULength < 0){
        أرجع -cnv->pre_منزل_ULength;
    }وإلا إذا(cnv->منزل_ULength > 0){
        أرجع cnv->منزل_ULength;
    }
    أرجع 0;
}

U_CAPI UBool U_EXPORT2
ucnv_is_Fixed_پلنوالی(UConverter *cnv, UError_کوډ *حالت){
    إذا (U_FAILURE(*حالت)) {
        أرجع خطأ;
    }

    إذا (cnv == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }

    بدل (ucnv_getType(cnv)) {
        حالة UCNV_SBCS:
        حالة UCNV_DBCS:
        حالة UCNV_UTF32_BigEndian:
        حالة UCNV_UTF32_LittleEndian:
        حالة UCNV_UTF32:
        حالة UCNV_US_ASCII:
            أرجع ریښتیا;
        افتراضي:
            أرجع خطأ;
    }
}
#د_که_پای

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
