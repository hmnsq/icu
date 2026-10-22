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
#تعریف_کرو CHUNK_ناپ 1024

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
                       UError_کوڈ * err)
{
    UConverter *r;

    إذا (err == خالی_سوچک || U_FAILURE (*err)) {
        أرجع خالی_سوچک;
    }

    r =  ucnv_createConverter(خالی_سوچک, name, err);
    أرجع r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ثابت حرف *packageName, ثابت حرف *converterName, UError_کوڈ * err)
{
    أرجع ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ثابت char16_t * name,
                         UError_کوڈ * err)
{
    حرف asciiName[UCNV_MAX_CONVERTER_NAME_لمائی];

    إذا (err == خالی_سوچک || U_FAILURE(*err))
        أرجع خالی_سوچک;
    إذا (name == خالی_سوچک)
        أرجع ucnv_open (خالی_سوچک, err);
    إذا (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_لمائی)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
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
                UError_کوڈ * err)
{
    حرف myName[UCNV_MAX_CONVERTER_NAME_لمائی];
    int32_t myNameLen;

    إذا (err == خالی_سوچک || U_FAILURE (*err))
        أرجع خالی_سوچک;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_منزل_String(myName + myNameLen, codepage, 10);

    أرجع ucnv_createConverter(خالی_سوچک, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_محفوظ_Clone(ثابت UConverter* cnv,
               فراغ* ڈھیر_بفر U_LIFETIME_BOUND,
               int32_t* p_بفر_ناپ,
               UError_کوڈ* حالت)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t ڈھیر_بفر_ناپ;
    int32_t بفر_ناپ_Needed;
    UError_کوڈ cbErr;
    UConverter_منزل_Unicode_دلیلاں منزل_UArgs = {
        حجم(UConverter_منزل_Unicode_دلیلاں),
            سچ,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک
    };
    UConverter_From_Unicode_دلیلاں fromUArgs = {
        حجم(UConverter_From_Unicode_دلیلاں),
            سچ,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک
    };

    UTRACE_اندراج_OC(UTRACE_UCNV_CLONE);

    إذا (حالت == خالی_سوچک || U_FAILURE(*حالت)){
        UTRACE_EXIT_حالت(حالت? *حالت: U_ILLEGAL_دلیل_ERROR);
        أرجع خالی_سوچک;
    }

    إذا (cnv == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(*حالت);
        أرجع خالی_سوچک;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, حالت), cnv, ڈھیر_بفر);

    إذا (cnv->shared_انکڑے->impl->محفوظ_Clone != خالی_سوچک) {
        /* call the custom safeClone function for sizing */
        بفر_ناپ_Needed = 0;
        cnv->shared_انکڑے->impl->محفوظ_Clone(cnv, خالی_سوچک, &بفر_ناپ_Needed, حالت);
        إذا (U_FAILURE(*حالت)) {
            UTRACE_EXIT_حالت(*حالت);
            أرجع خالی_سوچک;
        }
    }
    وإلا
    {
        /* inherent sizing */
        بفر_ناپ_Needed = حجم(UConverter);
    }

    إذا (p_بفر_ناپ == خالی_سوچک) {
        ڈھیر_بفر_ناپ = 1;
        p_بفر_ناپ = &ڈھیر_بفر_ناپ;
    } وإلا {
        ڈھیر_بفر_ناپ = *p_بفر_ناپ;
        إذا (ڈھیر_بفر_ناپ <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_بفر_ناپ = بفر_ناپ_Needed;
            UTRACE_EXIT_قدر(بفر_ناپ_Needed);
            أرجع خالی_سوچک;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    إذا (ڈھیر_بفر) {
        uintptr_t p = reinterpret_cast<uintptr_t>(ڈھیر_بفر);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t اشارہ_گر_Adjustment = aligned_p - p;
        إذا (بفر_ناپ_Needed + اشارہ_گر_Adjustment <= ڈھیر_بفر_ناپ) {
            ڈھیر_بفر = reinterpret_cast<فراغ *>(aligned_p);
            ڈھیر_بفر_ناپ -= static_cast<int32_t>(اشارہ_گر_Adjustment);
        } وإلا {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            ڈھیر_بفر_ناپ = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    إذا (ڈھیر_بفر_ناپ < بفر_ناپ_Needed || ڈھیر_بفر == خالی_سوچک)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (بفر_ناپ_Needed);

        إذا(localConverter == خالی_سوچک) {
            *حالت = U_حافظہ_ونڈ_ERROR;
            UTRACE_EXIT_حالت(*حالت);
            أرجع خالی_سوچک;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        إذا (p_بفر_ناپ != &ڈھیر_بفر_ناپ) {
            *حالت = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_بفر_ناپ = بفر_ناپ_Needed;
    } وإلا {
        /* just use the stack buffer */
        localConverter = (UConverter*) ڈھیر_بفر;
        allocatedConverter = خالی_سوچک;
    }

    uprv_memset(localConverter, 0, بفر_ناپ_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, حجم(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = خطأ;

    /* copy the substitution string */
    إذا (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } وإلا {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_بفر_لمائی * U_SIZEOF_UCHAR);
        إذا (localConverter->subChars == خالی_سوچک) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_حالت(*حالت);
            أرجع خالی_سوچک;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_بفر_لمائی * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    إذا (cnv->shared_انکڑے->impl->محفوظ_Clone != خالی_سوچک) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_انکڑے->impl->محفوظ_Clone(cnv, localConverter, p_بفر_ناپ, حالت);
    }

    إذا(localConverter==خالی_سوچک || U_FAILURE(*حالت)) {
        إذا (allocatedConverter != خالی_سوچک && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_حالت(*حالت);
        أرجع خالی_سوچک;
    }

    /* increment refcount of shared data if needed */
    إذا (cnv->shared_انکڑے->isReferenceCounted) {
        ucnv_increment_Ref_گنتی(cnv->shared_انکڑے);
    }

    إذا(localConverter == (UConverter*)ڈھیر_بفر) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = سچ;
    }

    /* allow callback functions to handle any memory allocation */
    منزل_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->منزل_UContext, &منزل_UArgs, خالی_سوچک, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, خالی_سوچک, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_حالت(localConverter, *حالت);
    أرجع localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ثابت UConverter* cnv, UError_کوڈ *حالت)
{
    أرجع ucnv_محفوظ_Clone(cnv, خالی_سوچک, خالی_سوچک, حالت);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI فراغ  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;

    UTRACE_اندراج_OC(UTRACE_UCNV_CLOSE);

    إذا (converter == خالی_سوچک)
    {
        UTRACE_EXIT();
        أرجع;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_کوڈ), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    إذا (converter->fromCharErrorBehaviour != UCNV_منزل_U_DEFAULT_CALLBACK) {
        UConverter_منزل_Unicode_دلیلاں منزل_UArgs = {
            حجم(UConverter_منزل_Unicode_دلیلاں),
                سچ,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک
        };

        منزل_UArgs.converter = converter;
        error_کوڈ = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->منزل_UContext, &منزل_UArgs, خالی_سوچک, 0, UCNV_CLOSE, &error_کوڈ);
    }
    إذا (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_دلیلاں fromUArgs = {
            حجم(UConverter_From_Unicode_دلیلاں),
                سچ,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک
        };
        fromUArgs.converter = converter;
        error_کوڈ = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, خالی_سوچک, 0, 0, UCNV_CLOSE, &error_کوڈ);
    }

    إذا (converter->shared_انکڑے->impl->close != خالی_سوچک) {
        converter->shared_انکڑے->impl->close(converter);
    }

    إذا (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    إذا (converter->shared_انکڑے->isReferenceCounted) {
        ucnv_unload_Shared_انکڑے_If_Ready(converter->shared_انکڑے);
    }

    إذا(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ثابت حرف*   U_EXPORT2
ucnv_get_دستیاب_Name (int32_t n)
{
    إذا (0 <= n && n <= 0xffff) {
        UError_کوڈ err = U_ZERO_ERROR;
        ثابت حرف *name = ucnv_bld_get_دستیاب_Converter((uint16_t)n, &err);
        إذا (U_SUCCESS(err)) {
            أرجع name;
        }
    }
    أرجع خالی_سوچک;
}

U_CAPI int32_t   U_EXPORT2
ucnv_گنتی_دستیاب ()
{
    UError_کوڈ err = U_ZERO_ERROR;
    أرجع ucnv_bld_گنتی_دستیاب_Converters(&err);
}

U_CAPI فراغ    U_EXPORT2
ucnv_getSubstChars (ثابت UConverter * converter,
                    حرف *mySubChar,
                    int8_t * len,
                    UError_کوڈ * err)
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
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI فراغ    U_EXPORT2
ucnv_سیٹ_Subst_Chars (UConverter * converter,
                    ثابت حرف *mySubChar,
                    int8_t len,
                    UError_کوڈ * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    إذا ((len > converter->shared_انکڑے->static_انکڑے->max_بائٹاں_فی_Char)
     || (len < converter->shared_انکڑے->static_انکڑے->min_بائٹاں_فی_Char))
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
ucnv_سیٹ_Subst_String(UConverter *cnv,
                    ثابت char16_t *s,
                    int32_t length,
                    UError_کوڈ *err) {
    alignas(UConverter) حرف clone_بفر[U_CNV_SAFECLONE_BUFFERSIZE];
    حرف chars[UCNV_ERROR_بفر_لمائی];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_ناپ, length8;

    /* Let the following functions check all arguments. */
    clone_ناپ = حجم(clone_بفر);
    clone = ucnv_محفوظ_Clone(cnv, clone_بفر, &clone_ناپ, err);
    ucnv_سیٹ_From_UCall_پٹھ(clone, UCNV_FROM_U_CALLBACK_STOP, خالی_سوچک, خالی_سوچک, خالی_سوچک, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)حجم(chars), s, length, err);
    ucnv_close(clone);
    إذا (U_FAILURE(*err)) {
        أرجع;
    }

    إذا (cnv->shared_انکڑے->impl->writeSub == خالی_سوچک
#إذا !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_انکڑے->static_انکڑے->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#جے_انت
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
        إذا (length > UCNV_ERROR_بفر_لمائی) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_بفر_حد_توں_ودھ_ERROR;
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
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_بفر_لمائی * U_SIZEOF_UCHAR);
            إذا (cnv->subChars == خالی_سوچک) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_بفر_لمائی * U_SIZEOF_UCHAR);
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
    إذا(converter == خالی_سوچک) {
        أرجع;
    }

    إذا(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_کوڈ error_کوڈ;

        إذا(choice<=UCNV_RESET_منزل_UNICODE && converter->fromCharErrorBehaviour != UCNV_منزل_U_DEFAULT_CALLBACK) {
            UConverter_منزل_Unicode_دلیلاں منزل_UArgs = {
                حجم(UConverter_منزل_Unicode_دلیلاں),
                سچ,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک
            };
            منزل_UArgs.converter = converter;
            error_کوڈ = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->منزل_UContext, &منزل_UArgs, خالی_سوچک, 0, UCNV_RESET, &error_کوڈ);
        }
        إذا(choice!=UCNV_RESET_منزل_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_دلیلاں fromUArgs = {
                حجم(UConverter_From_Unicode_دلیلاں),
                سچ,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک,
                خالی_سوچک
            };
            fromUArgs.converter = converter;
            error_کوڈ = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, خالی_سوچک, 0, 0, UCNV_RESET, &error_کوڈ);
        }
    }

    /* now reset the converter itself */
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        converter->منزل_Unicode_حالت = converter->shared_انکڑے->منزل_Unicode_حالت;
        converter->منوال = 0;
        converter->منزل_ULength = 0;
        converter->invalid_Char_لمائی = converter->UChar_Error_بفر_لمائی = 0;
        converter->pre_منزل_ULength = 0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        converter->from_Unicode_حالت = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_لمائی = converter->char_Error_بفر_لمائی = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    إذا (converter->shared_انکڑے->impl->reset != خالی_سوچک) {
        /* call the custom reset function */
        converter->shared_انکڑے->impl->reset(converter, choice);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, سچ);
}

U_CAPI فراغ  U_EXPORT2
ucnv_reset_منزل_Unicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_منزل_UNICODE, سچ);
}

U_CAPI فراغ  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, سچ);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_ناپ (ثابت UConverter * converter)
{
    أرجع converter->max_بائٹاں_فی_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_ناپ (ثابت UConverter * converter)
{
    أرجع converter->shared_انکڑے->static_انکڑے->min_بائٹاں_فی_Char;
}

U_CAPI ثابت حرف* U_EXPORT2
ucnv_getName(ثابت UConverter* converter U_LIFETIME_BOUND, UError_کوڈ* err)
{
    إذا (U_FAILURE (*err))
        أرجع خالی_سوچک;
    إذا(converter->shared_انکڑے->impl->getName){
        ثابت حرف* عارضی= converter->shared_انکڑے->impl->getName(converter);
        إذا(عارضی)
            أرجع عارضی;
    }
    أرجع converter->shared_انکڑے->static_انکڑے->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ثابت UConverter * converter,
              UError_کوڈ * err)
{
    int32_t ccsid;
    إذا (U_FAILURE (*err))
        أرجع -1;

    ccsid = converter->shared_انکڑے->static_انکڑے->codepage;
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
                                      UError_کوڈ * err)
{
    إذا (U_FAILURE (*err))
        أرجع UCNV_UNKNOWN;

    أرجع (UConverterPlatform)converter->shared_انکڑے->static_انکڑے->platform;
}

U_CAPI فراغ U_EXPORT2
    ucnv_get_منزل_UCall_پٹھ (ثابت UConverter * converter,
                         UConverter_منزل_UCallback *action,
                         ثابت فراغ **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->منزل_UContext;
}

U_CAPI فراغ U_EXPORT2
    ucnv_get_From_UCall_پٹھ (ثابت UConverter * converter,
                           UConverterFromUCallback *action,
                           ثابت فراغ **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI فراغ    U_EXPORT2
ucnv_سیٹ_منزل_UCall_پٹھ (UConverter * converter,
                            UConverter_منزل_UCallback newAction,
                            ثابت فراغ* newContext,
                            UConverter_منزل_UCallback *oldAction,
                            ثابت فراغ** oldContext,
                            UError_کوڈ * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    إذا (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    إذا (oldContext) *oldContext = converter->منزل_UContext;
    converter->منزل_UContext = newContext;
}

U_CAPI فراغ  U_EXPORT2
ucnv_سیٹ_From_UCall_پٹھ (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ثابت فراغ* newContext,
                            UConverterFromUCallback *oldAction,
                            ثابت فراغ** oldContext,
                            UError_کوڈ * err)
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
               int32_t سروت_اشاریہ, int32_t error_داخلہ_لمائی) {
    int32_t *حد_بندی;
    int32_t delta, وتھ;

    إذا(سروت_اشاریہ>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=سروت_اشاریہ-error_داخلہ_لمائی;
    } وإلا {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    حد_بندی=offsets+length;
    إذا(delta==0) {
        /* most common case, nothing to do */
    } وإلا إذا(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        طالما(offsets<حد_بندی) {
            وتھ=*offsets;
            إذا(وتھ>=0) {
                *offsets=وتھ+delta;
            }
            ++offsets;
        }
    } وإلا /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        طالما(offsets<حد_بندی) {
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
_fromUnicodeWithCallback(UConverter_From_Unicode_دلیلاں *p_دلیلاں, UError_کوڈ *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ثابت char16_t *s;
    حرف *t;
    int32_t *offsets;
    int32_t سروت_اشاریہ;
    int32_t error_داخلہ_لمائی;
    UBool converter_Saw_سرا_دا_داخلہ, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ثابت char16_t *real_سروت, *real_سروت_حد_بندی;
    int32_t real_سروت_اشاریہ;
    UBool realFlush;

    cnv=p_دلیلاں->converter;
    s=p_دلیلاں->سروت;
    t=p_دلیلاں->نشانہ;
    offsets=p_دلیلاں->offsets;

    /* get the converter implementation function */
    سروت_اشاریہ=0;
    إذا(offsets==خالی_سوچک) {
        fromUnicode=cnv->shared_انکڑے->impl->fromUnicode;
    } وإلا {
        fromUnicode=cnv->shared_انکڑے->impl->fromUnicodeWithOffsets;
        إذا(fromUnicode==خالی_سوچک) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_انکڑے->impl->fromUnicode;
            /* we will write -1 for each offset */
            سروت_اشاریہ=-1;
        }
    }

    إذا(cnv->preFromULength>=0) {
        /* normal mode */
        real_سروت=خالی_سوچک;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_سروت_حد_بندی=خالی_سوچک;
        realFlush=خطأ;
        real_سروت_اشاریہ=0;
    } وإلا {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        إذا (!p_دلیلاں->سروت) {
            *err=U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        real_سروت=p_دلیلاں->سروت;
        real_سروت_حد_بندی=p_دلیلاں->سروت_حد_بندی;
        realFlush=p_دلیلاں->flush;
        real_سروت_اشاریہ=سروت_اشاریہ;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_دلیلاں->سروت=replay;
        p_دلیلاں->سروت_حد_بندی=replay-cnv->preFromULength;
        p_دلیلاں->flush=خطأ;
        سروت_اشاریہ=-1;

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
            fromUnicode(p_دلیلاں, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_سرا_دا_داخلہ=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_دلیلاں->flush && p_دلیلاں->سروت==p_دلیلاں->سروت_حد_بندی &&
                        cnv->fromUChar32==0);
        } وإلا {
            /* handle error from ucnv_convertEx() */
            converter_Saw_سرا_دا_داخلہ=خطأ;
        }

        /* no callback called yet for this iteration */
        calledCallback=خطأ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_داخلہ_لمائی=0;

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
            إذا(offsets!=خالی_سوچک) {
                int32_t length = static_cast<int32_t>(p_دلیلاں->نشانہ - t);
                إذا(length>0) {
                    _updateOffsets(offsets, length, سروت_اشاریہ, error_داخلہ_لمائی);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_دلیلاں->offsets=offsets+=length;
                }

                إذا(سروت_اشاریہ>=0) {
                    سروت_اشاریہ += static_cast<int32_t>(p_دلیلاں->سروت - s);
                }
            }

            إذا(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                إذا(real_سروت==خالی_سوچک) {
                    real_سروت=p_دلیلاں->سروت;
                    real_سروت_حد_بندی=p_دلیلاں->سروت_حد_بندی;
                    realFlush=p_دلیلاں->flush;
                    real_سروت_اشاریہ=سروت_اشاریہ;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_دلیلاں->سروت=replay;
                    p_دلیلاں->سروت_حد_بندی=replay-cnv->preFromULength;
                    p_دلیلاں->flush=خطأ;
                    إذا((سروت_اشاریہ+=cnv->preFromULength)<0) {
                        سروت_اشاریہ=-1;
                    }

                    cnv->preFromULength=0;
                } وإلا {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_سروت==خالی_سوچک);
                    *err=U_اندرونی_پروگرام_ERROR;
                }
            }

            /* update pointers */
            s=p_دلیلاں->سروت;
            t=p_دلیلاں->نشانہ;

            إذا(U_SUCCESS(*err)) {
                إذا(s<p_دلیلاں->سروت_حد_بندی) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    اكسر;
                } وإلا إذا(real_سروت!=خالی_سوچک) {
                    /* switch back from replaying to the real source and continue */
                    p_دلیلاں->سروت=real_سروت;
                    p_دلیلاں->سروت_حد_بندی=real_سروت_حد_بندی;
                    p_دلیلاں->flush=realFlush;
                    سروت_اشاریہ=real_سروت_اشاریہ;

                    real_سروت=خالی_سوچک;
                    اكسر;
                } وإلا إذا(p_دلیلاں->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_لبھیا;
                    calledCallback=خطأ; /* new error condition */
                } وإلا {
                    /* input consumed */
                    إذا(p_دلیلاں->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        إذا(!converter_Saw_سرا_دا_داخلہ) {
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
                UError_کوڈ e;

                إذا( calledCallback ||
                    (e=*err)==U_بفر_حد_توں_ودھ_ERROR ||
                    (e!=U_INVALID_CHAR_لبھیا &&
                     e!=U_ILLEGAL_CHAR_لبھیا &&
                     e!=U_TRUNCATED_CHAR_لبھیا)
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
                    إذا(real_سروت!=خالی_سوچک) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_دلیلاں->سروت_حد_بندی - p_دلیلاں->سروت);
                        إذا(length>0) {
                            u_memcpy(cnv->preFromU, p_دلیلاں->سروت, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_دلیلاں->سروت=real_سروت;
                        p_دلیلاں->سروت_حد_بندی=real_سروت_حد_بندی;
                        p_دلیلاں->flush=realFlush;
                    }

                    أرجع;
                }
            }

            /* callback handling */
            {
                UChar32 کوڈ_نقطہ;

                /* get and write the code point */
                کوڈ_نقطہ=cnv->fromUChar32;
                error_داخلہ_لمائی=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_بفر, error_داخلہ_لمائی, کوڈ_نقطہ);
                cnv->invalid_UChar_لمائی = static_cast<int8_t>(error_داخلہ_لمائی);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_دلیلاں,
                    cnv->invalid_UChar_بفر, error_داخلہ_لمائی, کوڈ_نقطہ,
                    *err==U_INVALID_CHAR_لبھیا ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
                    err);
            }

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=سچ;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
ساكن UBool
ucnv_output_حد_توں_ودھ_From_Unicode(UConverter *cnv,
                               حرف **نشانہ, ثابت حرف *نشانہ_حد_بندی,
                               int32_t **pOffsets,
                               UError_کوڈ *err) {
    int32_t *offsets;
    حرف *حد_توں_ودھ, *t;
    int32_t i, length;

    t=*نشانہ;
    إذا(pOffsets!=خالی_سوچک) {
        offsets=*pOffsets;
    } وإلا {
        offsets=خالی_سوچک;
    }

    حد_توں_ودھ = reinterpret_cast<حرف*>(cnv->char_Error_بفر);
    length=cnv->char_Error_بفر_لمائی;
    i=0;
    طالما(i<length) {
        إذا(t==نشانہ_حد_بندی) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            افعل {
                حد_توں_ودھ[j++]=حد_توں_ودھ[i++];
            } طالما(i<length);

            cnv->char_Error_بفر_لمائی = static_cast<int8_t>(j);
            *نشانہ=t;
            إذا(offsets!=خالی_سوچک) {
                *pOffsets=offsets;
            }
            *err=U_بفر_حد_توں_ودھ_ERROR;
            أرجع سچ;
        }

        /* copy the overflow contents to the target */
        *t++=حد_توں_ودھ[i++];
        إذا(offsets!=خالی_سوچک) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_بفر_لمائی=0;
    *نشانہ=t;
    إذا(offsets!=خالی_سوچک) {
        *pOffsets=offsets;
    }
    أرجع خطأ;
}

U_CAPI فراغ U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 حرف **نشانہ, ثابت حرف *نشانہ_حد_بندی,
                 ثابت char16_t **سروت, ثابت char16_t *سروت_حد_بندی,
                 int32_t *offsets,
                 UBool flush,
                 UError_کوڈ *err) {
    UConverter_From_Unicode_دلیلاں دلیلاں_2;
    ثابت char16_t *s;
    حرف *t;

    /* check parameters */
    إذا(err==خالی_سوچک || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(cnv==خالی_سوچک || نشانہ==خالی_سوچک || سروت==خالی_سوچک) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    s=*سروت;
    t=*نشانہ;

    إذا ((ثابت فراغ *)U_MAX_PTR(سروت_حد_بندی) == (ثابت فراغ *)سروت_حد_بندی) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        سروت_حد_بندی = (ثابت char16_t *)(((ثابت حرف *)سروت_حد_بندی) - 1);
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
    إذا (سروت_حد_بندی<s || نشانہ_حد_بندی<t ||
        ((size_t)(سروت_حد_بندی-s)>(size_t)0x3fffffff && سروت_حد_بندی>s) ||
        ((size_t)(نشانہ_حد_بندی-t)>(size_t)0x7fffffff && نشانہ_حد_بندی>t) ||
        (((ثابت حرف *)سروت_حد_بندی-(ثابت حرف *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    
    /* output the target overflow buffer */
    إذا( cnv->char_Error_بفر_لمائی>0 &&
        ucnv_output_حد_توں_ودھ_From_Unicode(cnv, نشانہ, نشانہ_حد_بندی, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        أرجع;
    }
    /* *target may have moved, therefore stop using t */

    إذا(!flush && s==سروت_حد_بندی && cnv->preFromULength>=0) {
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
    دلیلاں_2.converter=cnv;
    دلیلاں_2.flush=flush;
    دلیلاں_2.offsets=offsets;
    دلیلاں_2.سروت=s;
    دلیلاں_2.سروت_حد_بندی=سروت_حد_بندی;
    دلیلاں_2.نشانہ=*نشانہ;
    دلیلاں_2.نشانہ_حد_بندی=نشانہ_حد_بندی;
    دلیلاں_2.size=حجم(دلیلاں_2);

    _fromUnicodeWithCallback(&دلیلاں_2, err);

    *سروت=دلیلاں_2.سروت;
    *نشانہ=دلیلاں_2.نشانہ;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

ساكن فراغ
منزل_Unicode_With_Callback(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں, UError_کوڈ *err) {
    UConverter_منزل_Unicode منزل_Unicode;
    UConverter *cnv;
    ثابت حرف *s;
    char16_t *t;
    int32_t *offsets;
    int32_t سروت_اشاریہ;
    int32_t error_داخلہ_لمائی;
    UBool converter_Saw_سرا_دا_داخلہ, calledCallback;

    /* variables for m:n conversion */
    حرف replay[UCNV_EXT_MAX_بائٹاں];
    ثابت حرف *real_سروت, *real_سروت_حد_بندی;
    int32_t real_سروت_اشاریہ;
    UBool realFlush;

    cnv=p_دلیلاں->converter;
    s=p_دلیلاں->سروت;
    t=p_دلیلاں->نشانہ;
    offsets=p_دلیلاں->offsets;

    /* get the converter implementation function */
    سروت_اشاریہ=0;
    إذا(offsets==خالی_سوچک) {
        منزل_Unicode=cnv->shared_انکڑے->impl->منزل_Unicode;
    } وإلا {
        منزل_Unicode=cnv->shared_انکڑے->impl->منزل_Unicode_With_Offsets;
        إذا(منزل_Unicode==خالی_سوچک) {
            /* there is no WithOffsets implementation */
            منزل_Unicode=cnv->shared_انکڑے->impl->منزل_Unicode;
            /* we will write -1 for each offset */
            سروت_اشاریہ=-1;
        }
    }

    إذا(cnv->pre_منزل_ULength>=0) {
        /* normal mode */
        real_سروت=خالی_سوچک;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_سروت_حد_بندی=خالی_سوچک;
        realFlush=خطأ;
        real_سروت_اشاریہ=0;
    } وإلا {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        إذا (!p_دلیلاں->سروت) {
            *err=U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        real_سروت=p_دلیلاں->سروت;
        real_سروت_حد_بندی=p_دلیلاں->سروت_حد_بندی;
        realFlush=p_دلیلاں->flush;
        real_سروت_اشاریہ=سروت_اشاریہ;

        uprv_memcpy(replay, cnv->pre_منزل_U, -cnv->pre_منزل_ULength);
        p_دلیلاں->سروت=replay;
        p_دلیلاں->سروت_حد_بندی=replay-cnv->pre_منزل_ULength;
        p_دلیلاں->flush=خطأ;
        سروت_اشاریہ=-1;

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
            منزل_Unicode(p_دلیلاں, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_سرا_دا_داخلہ=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_دلیلاں->flush && p_دلیلاں->سروت==p_دلیلاں->سروت_حد_بندی &&
                        cnv->منزل_ULength==0);
        } وإلا {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_سرا_دا_داخلہ=خطأ;
        }

        /* no callback called yet for this iteration */
        calledCallback=خطأ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_داخلہ_لمائی=0;

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
            إذا(offsets!=خالی_سوچک) {
                int32_t length = static_cast<int32_t>(p_دلیلاں->نشانہ - t);
                إذا(length>0) {
                    _updateOffsets(offsets, length, سروت_اشاریہ, error_داخلہ_لمائی);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_دلیلاں->offsets=offsets+=length;
                }

                إذا(سروت_اشاریہ>=0) {
                    سروت_اشاریہ += static_cast<int32_t>(p_دلیلاں->سروت - s);
                }
            }

            إذا(cnv->pre_منزل_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                إذا(real_سروت==خالی_سوچک) {
                    real_سروت=p_دلیلاں->سروت;
                    real_سروت_حد_بندی=p_دلیلاں->سروت_حد_بندی;
                    realFlush=p_دلیلاں->flush;
                    real_سروت_اشاریہ=سروت_اشاریہ;

                    uprv_memcpy(replay, cnv->pre_منزل_U, -cnv->pre_منزل_ULength);
                    p_دلیلاں->سروت=replay;
                    p_دلیلاں->سروت_حد_بندی=replay-cnv->pre_منزل_ULength;
                    p_دلیلاں->flush=خطأ;
                    إذا((سروت_اشاریہ+=cnv->pre_منزل_ULength)<0) {
                        سروت_اشاریہ=-1;
                    }

                    cnv->pre_منزل_ULength=0;
                } وإلا {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_سروت==خالی_سوچک);
                    *err=U_اندرونی_پروگرام_ERROR;
                }
            }

            /* update pointers */
            s=p_دلیلاں->سروت;
            t=p_دلیلاں->نشانہ;

            إذا(U_SUCCESS(*err)) {
                إذا(s<p_دلیلاں->سروت_حد_بندی) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    اكسر;
                } وإلا إذا(real_سروت!=خالی_سوچک) {
                    /* switch back from replaying to the real source and continue */
                    p_دلیلاں->سروت=real_سروت;
                    p_دلیلاں->سروت_حد_بندی=real_سروت_حد_بندی;
                    p_دلیلاں->flush=realFlush;
                    سروت_اشاریہ=real_سروت_اشاریہ;

                    real_سروت=خالی_سوچک;
                    اكسر;
                } وإلا إذا(p_دلیلاں->flush && cnv->منزل_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_لبھیا;
                    calledCallback=خطأ; /* new error condition */
                } وإلا {
                    /* input consumed */
                    إذا(p_دلیلاں->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        إذا(!converter_Saw_سرا_دا_داخلہ) {
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
                UError_کوڈ e;

                إذا( calledCallback ||
                    (e=*err)==U_بفر_حد_توں_ودھ_ERROR ||
                    (e!=U_INVALID_CHAR_لبھیا &&
                     e!=U_ILLEGAL_CHAR_لبھیا &&
                     e!=U_TRUNCATED_CHAR_لبھیا &&
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
                    إذا(real_سروت!=خالی_سوچک) {
                        int32_t length;

                        U_ASSERT(cnv->pre_منزل_ULength==0);

                        length = static_cast<int32_t>(p_دلیلاں->سروت_حد_بندی - p_دلیلاں->سروت);
                        إذا(length>0) {
                            uprv_memcpy(cnv->pre_منزل_U, p_دلیلاں->سروت, length);
                            cnv->pre_منزل_ULength = static_cast<int8_t>(-length);
                        }

                        p_دلیلاں->سروت=real_سروت;
                        p_دلیلاں->سروت_حد_بندی=real_سروت_حد_بندی;
                        p_دلیلاں->flush=realFlush;
                    }

                    أرجع;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_داخلہ_لمائی=cnv->invalid_Char_لمائی=cnv->منزل_ULength;
            إذا(error_داخلہ_لمائی>0) {
                uprv_memcpy(cnv->invalid_Char_بفر, cnv->منزل_UBytes, error_داخلہ_لمائی);
            }

            /* set the converter state to deal with the next character */
            cnv->منزل_ULength=0;

            /* call the callback function */
            إذا(cnv->منزل_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_لبھیا) {
                cnv->منزل_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->منزل_UContext, p_دلیلاں,
                cnv->invalid_Char_بفر, error_داخلہ_لمائی,
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
            calledCallback=سچ;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
ساكن UBool
ucnv_output_حد_توں_ودھ_منزل_Unicode(UConverter *cnv,
                             char16_t **نشانہ, ثابت char16_t *نشانہ_حد_بندی,
                             int32_t **pOffsets,
                             UError_کوڈ *err) {
    int32_t *offsets;
    char16_t *حد_توں_ودھ, *t;
    int32_t i, length;

    t=*نشانہ;
    إذا(pOffsets!=خالی_سوچک) {
        offsets=*pOffsets;
    } وإلا {
        offsets=خالی_سوچک;
    }

    حد_توں_ودھ=cnv->UChar_Error_بفر;
    length=cnv->UChar_Error_بفر_لمائی;
    i=0;
    طالما(i<length) {
        إذا(t==نشانہ_حد_بندی) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            افعل {
                حد_توں_ودھ[j++]=حد_توں_ودھ[i++];
            } طالما(i<length);

            cnv->UChar_Error_بفر_لمائی = static_cast<int8_t>(j);
            *نشانہ=t;
            إذا(offsets!=خالی_سوچک) {
                *pOffsets=offsets;
            }
            *err=U_بفر_حد_توں_ودھ_ERROR;
            أرجع سچ;
        }

        /* copy the overflow contents to the target */
        *t++=حد_توں_ودھ[i++];
        إذا(offsets!=خالی_سوچک) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_بفر_لمائی=0;
    *نشانہ=t;
    إذا(offsets!=خالی_سوچک) {
        *pOffsets=offsets;
    }
    أرجع خطأ;
}

U_CAPI فراغ U_EXPORT2
ucnv_منزل_Unicode(UConverter *cnv,
               char16_t **نشانہ, ثابت char16_t *نشانہ_حد_بندی,
               ثابت حرف **سروت, ثابت حرف *سروت_حد_بندی,
               int32_t *offsets,
               UBool flush,
               UError_کوڈ *err) {
    UConverter_منزل_Unicode_دلیلاں دلیلاں_2;
    ثابت حرف *s;
    char16_t *t;

    /* check parameters */
    إذا(err==خالی_سوچک || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(cnv==خالی_سوچک || نشانہ==خالی_سوچک || سروت==خالی_سوچک) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    s=*سروت;
    t=*نشانہ;

    إذا ((ثابت فراغ *)U_MAX_PTR(نشانہ_حد_بندی) == (ثابت فراغ *)نشانہ_حد_بندی) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        نشانہ_حد_بندی = (ثابت char16_t *)(((ثابت حرف *)نشانہ_حد_بندی) - 1);
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
    إذا (سروت_حد_بندی<s || نشانہ_حد_بندی<t ||
        ((size_t)(سروت_حد_بندی-s)>(size_t)0x7fffffff && سروت_حد_بندی>s) ||
        ((size_t)(نشانہ_حد_بندی-t)>(size_t)0x3fffffff && نشانہ_حد_بندی>t) ||
        (((ثابت حرف *)نشانہ_حد_بندی-(ثابت حرف *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    
    /* output the target overflow buffer */
    إذا( cnv->UChar_Error_بفر_لمائی>0 &&
        ucnv_output_حد_توں_ودھ_منزل_Unicode(cnv, نشانہ, نشانہ_حد_بندی, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        أرجع;
    }
    /* *target may have moved, therefore stop using t */

    إذا(!flush && s==سروت_حد_بندی && cnv->pre_منزل_ULength>=0) {
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
    دلیلاں_2.converter=cnv;
    دلیلاں_2.flush=flush;
    دلیلاں_2.offsets=offsets;
    دلیلاں_2.سروت=s;
    دلیلاں_2.سروت_حد_بندی=سروت_حد_بندی;
    دلیلاں_2.نشانہ=*نشانہ;
    دلیلاں_2.نشانہ_حد_بندی=نشانہ_حد_بندی;
    دلیلاں_2.size=حجم(دلیلاں_2);

    منزل_Unicode_With_Callback(&دلیلاں_2, err);

    *سروت=دلیلاں_2.سروت;
    *نشانہ=دلیلاں_2.نشانہ;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                حرف *dest, int32_t dest_سمرتھا,
                ثابت char16_t *src, int32_t src_لمائی,
                UError_کوڈ *p_Error_کوڈ) {
    ثابت char16_t *src_حد_بندی;
    حرف *originalDest, *dest_حد_بندی;
    int32_t dest_لمائی;

    /* check arguments */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    إذا( cnv==خالی_سوچک ||
        dest_سمرتھا<0 || (dest_سمرتھا>0 && dest==خالی_سوچک) ||
        src_لمائی<-1 || (src_لمائی!=0 && src==خالی_سوچک)
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    إذا(src_لمائی==-1) {
        src_لمائی=u_strlen(src);
    }
    إذا(src_لمائی>0) {
        src_حد_بندی=src+src_لمائی;
        dest_سمرتھا=pin_سمرتھا(dest, dest_سمرتھا);
        dest_حد_بندی=dest+dest_سمرتھا;

        /* perform the conversion */
        UError_کوڈ بفر_حالت = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_سوچک, سچ, &بفر_حالت);
        dest_لمائی=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        إذا(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR) {
            حرف بفر[1024];

            dest_حد_بندی=بفر+حجم(بفر);
            افعل {
                dest=بفر;
                بفر_حالت=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_سوچک, سچ, &بفر_حالت);
                dest_لمائی+=(int32_t)(dest-بفر);
            } طالما(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR);
        }
        إذا (U_FAILURE(بفر_حالت)) {
            *p_Error_کوڈ = بفر_حالت;
        }
    } وإلا {
        dest_لمائی=0;
    }

    أرجع u_terminateChars(originalDest, dest_سمرتھا, dest_لمائی, p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_سمرتھا,
              ثابت حرف *src, int32_t src_لمائی,
              UError_کوڈ *p_Error_کوڈ) {
    ثابت حرف *src_حد_بندی;
    char16_t *originalDest, *dest_حد_بندی;
    int32_t dest_لمائی;

    /* check arguments */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    إذا( cnv==خالی_سوچک ||
        dest_سمرتھا<0 || (dest_سمرتھا>0 && dest==خالی_سوچک) ||
        src_لمائی<-1 || (src_لمائی!=0 && src==خالی_سوچک))
    {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* initialize */
    ucnv_reset_منزل_Unicode(cnv);
    originalDest=dest;
    إذا(src_لمائی==-1) {
        src_لمائی=(int32_t)uprv_strlen(src);
    }
    إذا(src_لمائی>0) {
        src_حد_بندی=src+src_لمائی;
        dest_سمرتھا=pin_سمرتھا(dest, dest_سمرتھا);
        dest_حد_بندی=dest+dest_سمرتھا;

        /* perform the conversion */
        UError_کوڈ بفر_حالت = U_ZERO_ERROR;
        ucnv_منزل_Unicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_سوچک, سچ, &بفر_حالت);
        dest_لمائی=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        إذا(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR)
        {
            char16_t بفر[1024];

            dest_حد_بندی=بفر+UPRV_LENGTHOF(بفر);
            افعل {
                dest=بفر;
                بفر_حالت=U_ZERO_ERROR;
                ucnv_منزل_Unicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_سوچک, سچ, &بفر_حالت);
                dest_لمائی+=(int32_t)(dest-بفر);
            }
            طالما(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR);
        }
        إذا (U_FAILURE(بفر_حالت)) {
            *p_Error_کوڈ = بفر_حالت;
        }
    } وإلا {
        dest_لمائی=0;
    }

    أرجع u_terminateUChars(originalDest, dest_سمرتھا, dest_لمائی, p_Error_کوڈ);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_اگلا_UChar(UConverter *cnv,
                  ثابت حرف **سروت, ثابت حرف *سروت_حد_بندی,
                  UError_کوڈ *err) {
    UConverter_منزل_Unicode_دلیلاں دلیلاں_2;
    char16_t بفر[U16_MAX_لمائی];
    ثابت حرف *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    إذا(err==خالی_سوچک || U_FAILURE(*err)) {
        أرجع 0xffff;
    }

    إذا(cnv==خالی_سوچک || سروت==خالی_سوچک) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع 0xffff;
    }

    s=*سروت;
    إذا(سروت_حد_بندی<s) {
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
    إذا(((size_t)(سروت_حد_بندی-s)>(size_t)0x7fffffff && سروت_حد_بندی>s)) {
        *err=U_ILLEGAL_دلیل_ERROR;
        أرجع 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    إذا(cnv->UChar_Error_بفر_لمائی>0) {
        char16_t *حد_توں_ودھ;

        حد_توں_ودھ=cnv->UChar_Error_بفر;
        i=0;
        length=cnv->UChar_Error_بفر_لمائی;
        U16_اگلا(حد_توں_ودھ, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        إذا((cnv->UChar_Error_بفر_لمائی=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_بفر, cnv->UChar_Error_بفر+i,
                         cnv->UChar_Error_بفر_لمائی*U_SIZEOF_UCHAR);
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
    دلیلاں_2.converter=cnv;
    دلیلاں_2.flush=سچ;
    دلیلاں_2.offsets=خالی_سوچک;
    دلیلاں_2.سروت=s;
    دلیلاں_2.سروت_حد_بندی=سروت_حد_بندی;
    دلیلاں_2.نشانہ=بفر;
    دلیلاں_2.نشانہ_حد_بندی=بفر+1;
    دلیلاں_2.size=حجم(دلیلاں_2);

    إذا(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        إذا(cnv->منزل_ULength==0 && cnv->shared_انکڑے->impl->get_اگلا_UChar!=خالی_سوچک) {
            c=cnv->shared_انکڑے->impl->get_اگلا_UChar(&دلیلاں_2, err);
            *سروت=s=دلیلاں_2.سروت;
            إذا(*err==U_اشاریہ_OUTOFBOUNDS_ERROR) {
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
        منزل_Unicode_With_Callback(&دلیلاں_2, err);

        إذا(*err==U_بفر_حد_توں_ودھ_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(دلیلاں_2.نشانہ-بفر);
    } وإلا {
        /* write the lead surrogate from the overflow buffer */
        بفر[0]=(char16_t)c;
        دلیلاں_2.نشانہ=بفر+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    إذا(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } وإلا إذا(length==0) {
        /* no input or only state changes */
        *err=U_اشاریہ_OUTOFBOUNDS_ERROR;
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

            إذا(cnv->UChar_Error_بفر_لمائی>0) {
                /* got overflow output from the conversion */
                إذا(U16_IS_TRAIL(c2=cnv->UChar_Error_بفر[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    إذا((--cnv->UChar_Error_بفر_لمائی)>0) {
                        uprv_memmove(cnv->UChar_Error_بفر, cnv->UChar_Error_بفر+1,
                                     cnv->UChar_Error_بفر_لمائی*U_SIZEOF_UCHAR);
                    }
                } وإلا {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } وإلا إذا(دلیلاں_2.سروت<سروت_حد_بندی) {
                /* convert once more, to buffer[1] */
                دلیلاں_2.نشانہ_حد_بندی=بفر+2;
                منزل_Unicode_With_Callback(&دلیلاں_2, err);
                إذا(*err==U_بفر_حد_توں_ودھ_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(دلیلاں_2.نشانہ-بفر);
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
        إذا((length=cnv->UChar_Error_بفر_لمائی)>0) {
            uprv_memmove(cnv->UChar_Error_بفر+delta, cnv->UChar_Error_بفر,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_بفر_لمائی=(int8_t)(length+delta);

        cnv->UChar_Error_بفر[0]=بفر[i++];
        إذا(delta>1) {
            cnv->UChar_Error_بفر[1]=بفر[i];
        }
    }

    *سروت=دلیلاں_2.سروت;
    أرجع c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI فراغ U_EXPORT2
ucnv_convertEx(UConverter *نشانہ_Cnv, UConverter *سروت_Cnv,
               حرف **نشانہ, ثابت حرف *نشانہ_حد_بندی,
               ثابت حرف **سروت, ثابت حرف *سروت_حد_بندی,
               char16_t *دھرا_شروع, char16_t **دھرا_سروت,
               char16_t **دھرا_نشانہ, ثابت char16_t *دھرا_حد_بندی,
               UBool reset, UBool flush,
               UError_کوڈ *p_Error_کوڈ) {
    char16_t دھرا_بفر[CHUNK_ناپ];
    ثابت char16_t *my_دھرا_سروت;
    char16_t *my_دھرا_نشانہ;
    ثابت حرف *s;
    حرف *t;

    UConverter_منزل_Unicode_دلیلاں منزل_UArgs;
    UConverter_From_Unicode_دلیلاں fromUArgs;
    UConverterConvert convert;

    /* error checking */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }

    إذا( نشانہ_Cnv==خالی_سوچک || سروت_Cnv==خالی_سوچک ||
        سروت==خالی_سوچک || *سروت==خالی_سوچک ||
        نشانہ==خالی_سوچک || *نشانہ==خالی_سوچک || نشانہ_حد_بندی==خالی_سوچک
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    s=*سروت;
    t=*نشانہ;
    إذا((سروت_حد_بندی!=خالی_سوچک && سروت_حد_بندی<s) || نشانہ_حد_بندی<t) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    إذا(
        (سروت_حد_بندی!=خالی_سوچک && ((size_t)(سروت_حد_بندی-s)>(size_t)0x7fffffff && سروت_حد_بندی>s)) ||
        ((size_t)(نشانہ_حد_بندی-t)>(size_t)0x7fffffff && نشانہ_حد_بندی>t)
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    
    إذا(دھرا_شروع==خالی_سوچک) {
        إذا(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }

        /* use the stack pivot buffer */
        my_دھرا_سروت=my_دھرا_نشانہ=دھرا_شروع=دھرا_بفر;
        دھرا_سروت=(char16_t **)&my_دھرا_سروت;
        دھرا_نشانہ=&my_دھرا_نشانہ;
        دھرا_حد_بندی=دھرا_بفر+CHUNK_ناپ;
    } وإلا إذا(  دھرا_شروع>=دھرا_حد_بندی ||
                دھرا_سروت==خالی_سوچک || *دھرا_سروت==خالی_سوچک ||
                دھرا_نشانہ==خالی_سوچک || *دھرا_نشانہ==خالی_سوچک ||
                دھرا_حد_بندی==خالی_سوچک
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا(سروت_حد_بندی==خالی_سوچک) {
        /* get limit of single-byte-NUL-terminated source string */
        سروت_حد_بندی=uprv_strchr(*سروت, 0);
    }

    إذا(reset) {
        ucnv_reset_منزل_Unicode(سروت_Cnv);
        ucnv_resetFromUnicode(نشانہ_Cnv);
        *دھرا_سروت=*دھرا_نشانہ=دھرا_شروع;
    } وإلا إذا(نشانہ_Cnv->char_Error_بفر_لمائی>0) {
        /* output the targetCnv overflow buffer */
        إذا(ucnv_output_حد_توں_ودھ_From_Unicode(نشانہ_Cnv, نشانہ, نشانہ_حد_بندی, خالی_سوچک, p_Error_کوڈ)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            أرجع;
        }
        /* *target has moved, therefore stop using t */

        إذا( !flush &&
            نشانہ_Cnv->preFromULength>=0 && *دھرا_سروت==*دھرا_نشانہ &&
            سروت_Cnv->UChar_Error_بفر_لمائی==0 && سروت_Cnv->pre_منزل_ULength>=0 && s==سروت_حد_بندی
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            أرجع;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    إذا( سروت_Cnv->shared_انکڑے->static_انکڑے->conversionType==UCNV_UTF8 &&
        نشانہ_Cnv->shared_انکڑے->impl->fromUTF8!=خالی_سوچک
    ) {
        convert=نشانہ_Cnv->shared_انکڑے->impl->fromUTF8;
    } وإلا إذا( نشانہ_Cnv->shared_انکڑے->static_انکڑے->conversionType==UCNV_UTF8 &&
               سروت_Cnv->shared_انکڑے->impl->منزل_UTF8!=خالی_سوچک
    ) {
        convert=سروت_Cnv->shared_انکڑے->impl->منزل_UTF8;
    } وإلا {
        convert=خالی_سوچک;
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
    إذا(convert!=خالی_سوچک && (دھرا_حد_بندی-دھرا_شروع)>32) {
        دھرا_حد_بندی=دھرا_شروع+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=نشانہ_Cnv;
    fromUArgs.flush=خطأ;
    fromUArgs.offsets=خالی_سوچک;
    fromUArgs.نشانہ=*نشانہ;
    fromUArgs.نشانہ_حد_بندی=نشانہ_حد_بندی;
    fromUArgs.size=حجم(fromUArgs);

    منزل_UArgs.converter=سروت_Cnv;
    منزل_UArgs.flush=flush;
    منزل_UArgs.offsets=خالی_سوچک;
    منزل_UArgs.سروت=s;
    منزل_UArgs.سروت_حد_بندی=سروت_حد_بندی;
    منزل_UArgs.نشانہ_حد_بندی=دھرا_حد_بندی;
    منزل_UArgs.size=حجم(منزل_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=خالی_سوچک;

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
        إذا( *دھرا_سروت<*دھرا_نشانہ ||
            U_FAILURE(*p_Error_کوڈ) ||
            نشانہ_Cnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.سروت=*دھرا_سروت;
            fromUArgs.سروت_حد_بندی=*دھرا_نشانہ;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_کوڈ);
            إذا(U_FAILURE(*p_Error_کوڈ)) {
                /* target overflow, or conversion error */
                *دھرا_سروت=(char16_t *)fromUArgs.سروت;
                اكسر;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *دھرا_سروت=*دھرا_نشانہ=دھرا_شروع;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        إذا(سروت_Cnv->UChar_Error_بفر_لمائی>0) {
            إذا(ucnv_output_حد_توں_ودھ_منزل_Unicode(سروت_Cnv, دھرا_نشانہ, دھرا_حد_بندی, خالی_سوچک, p_Error_کوڈ)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_کوڈ=U_ZERO_ERROR;
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
        إذا( منزل_UArgs.سروت==سروت_حد_بندی &&
            سروت_Cnv->pre_منزل_ULength>=0 && سروت_Cnv->منزل_ULength==0 &&
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
        إذا(convert!=خالی_سوچک && نشانہ_Cnv->preFromUFirstCP<0 && سروت_Cnv->pre_منزل_ULength==0) {
            إذا(*p_Error_کوڈ==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_کوڈ=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &منزل_UArgs, p_Error_کوڈ);
            إذا(*p_Error_کوڈ==U_بفر_حد_توں_ودھ_ERROR) {
                اكسر;
            } وإلا إذا(U_FAILURE(*p_Error_کوڈ)) {
                إذا(سروت_Cnv->منزل_ULength>0) {
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
                    *دھرا_سروت=*دھرا_نشانہ=دھرا_شروع+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    استمر;
                }
            } وإلا إذا(*p_Error_کوڈ==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_کوڈ=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } وإلا إذا(flush && سروت_Cnv->منزل_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_کوڈ=U_TRUNCATED_CHAR_لبھیا;
            } وإلا {
                /* input consumed */
                إذا(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(سروت_Cnv, UCNV_RESET_منزل_UNICODE, خطأ);
                    _reset(نشانہ_Cnv, UCNV_RESET_FROM_UNICODE, خطأ);
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
        منزل_UArgs.نشانہ=دھرا_شروع; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        منزل_Unicode_With_Callback(&منزل_UArgs, p_Error_کوڈ);
        *دھرا_نشانہ=منزل_UArgs.نشانہ;
        إذا(*p_Error_کوڈ==U_بفر_حد_توں_ودھ_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_کوڈ=U_ZERO_ERROR;
        } وإلا إذا(U_FAILURE(*p_Error_کوڈ) || (!flush && *دھرا_نشانہ==دھرا_شروع)) {
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
        إذا( flush && منزل_UArgs.سروت==سروت_حد_بندی &&
            سروت_Cnv->pre_منزل_ULength>=0 &&
            سروت_Cnv->UChar_Error_بفر_لمائی==0
        ) {
            fromUArgs.flush=سچ;
        }
    }

    /*
     * The conversion loop is exited when one of the following is true:
     * - the entire source text has been converted successfully to the target buffer
     * - a target buffer overflow occurred
     * - a conversion error occurred
     */

    *سروت=منزل_UArgs.سروت;
    *نشانہ=fromUArgs.نشانہ;

    /* terminate the target buffer if possible */
    إذا(flush && U_SUCCESS(*p_Error_کوڈ)) {
        إذا(*نشانہ!=نشانہ_حد_بندی) {
            **نشانہ=0;
            إذا(*p_Error_کوڈ==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_کوڈ=U_ZERO_ERROR;
            }
        } وإلا {
            *p_Error_کوڈ=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
ساكن int32_t
ucnv_اندرونی_Convert(UConverter *باہرلا_Converter, UConverter *وچ_Converter,
                     حرف *نشانہ, int32_t نشانہ_سمرتھا,
                     ثابت حرف *سروت, int32_t سروت_لمائی,
                     UError_کوڈ *p_Error_کوڈ) {
    char16_t دھرا_بفر[CHUNK_ناپ];
    char16_t *دھرا, *pivot2;

    حرف *my_نشانہ;
    ثابت حرف *سروت_حد_بندی;
    ثابت حرف *نشانہ_حد_بندی;
    int32_t نشانہ_لمائی=0;

    /* set up */
    إذا(سروت_لمائی<0) {
        سروت_حد_بندی=uprv_strchr(سروت, 0);
    } وإلا {
        سروت_حد_بندی=سروت+سروت_لمائی;
    }

    /* if there is no input data, we're done */
    إذا(سروت==سروت_حد_بندی) {
        أرجع u_terminateChars(نشانہ, نشانہ_سمرتھا, 0, p_Error_کوڈ);
    }

    دھرا=pivot2=دھرا_بفر;
    my_نشانہ=نشانہ;
    نشانہ_لمائی=0;

    إذا(نشانہ_سمرتھا>0) {
        /* perform real conversion */
        نشانہ_حد_بندی=نشانہ+نشانہ_سمرتھا;
        ucnv_convertEx(باہرلا_Converter, وچ_Converter,
                       &my_نشانہ, نشانہ_حد_بندی,
                       &سروت, سروت_حد_بندی,
                       دھرا_بفر, &دھرا, &pivot2, دھرا_بفر+CHUNK_ناپ,
                       خطأ,
                       سچ,
                       p_Error_کوڈ);
        نشانہ_لمائی = static_cast<int32_t>(my_نشانہ - نشانہ);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    إذا(*p_Error_کوڈ==U_بفر_حد_توں_ودھ_ERROR || نشانہ_سمرتھا==0)
    {
        حرف نشانہ_بفر[CHUNK_ناپ];

        نشانہ_حد_بندی=نشانہ_بفر+CHUNK_ناپ;
        افعل {
            *p_Error_کوڈ=U_ZERO_ERROR;
            my_نشانہ=نشانہ_بفر;
            ucnv_convertEx(باہرلا_Converter, وچ_Converter,
                           &my_نشانہ, نشانہ_حد_بندی,
                           &سروت, سروت_حد_بندی,
                           دھرا_بفر, &دھرا, &pivot2, دھرا_بفر+CHUNK_ناپ,
                           خطأ,
                           سچ,
                           p_Error_کوڈ);
            نشانہ_لمائی += static_cast<int32_t>(my_نشانہ - نشانہ_بفر);
        } طالما(*p_Error_کوڈ==U_بفر_حد_توں_ودھ_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        أرجع u_terminateChars(نشانہ, نشانہ_سمرتھا, نشانہ_لمائی, p_Error_کوڈ);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    أرجع نشانہ_لمائی;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ثابت حرف *منزل_Converter_Name, ثابت حرف *fromConverterName,
             حرف *نشانہ, int32_t نشانہ_سمرتھا,
             ثابت حرف *سروت, int32_t سروت_لمائی,
             UError_کوڈ *p_Error_کوڈ) {
    UConverter in, out; /* stack-allocated */
    UConverter *وچ_Converter, *باہرلا_Converter;
    int32_t نشانہ_لمائی;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    إذا( سروت==خالی_سوچک || سروت_لمائی<-1 ||
        نشانہ_سمرتھا<0 || (نشانہ_سمرتھا>0 && نشانہ==خالی_سوچک)
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* if there is no input data, we're done */
    إذا(سروت_لمائی==0 || (سروت_لمائی<0 && *سروت==0)) {
        أرجع u_terminateChars(نشانہ, نشانہ_سمرتھا, 0, p_Error_کوڈ);
    }

    /* create the converters */
    وچ_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    باہرلا_Converter=ucnv_createConverter(&out, منزل_Converter_Name, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        ucnv_close(وچ_Converter);
        أرجع 0;
    }

    نشانہ_لمائی=ucnv_اندرونی_Convert(باہرلا_Converter, وچ_Converter,
                                      نشانہ, نشانہ_سمرتھا,
                                      سروت, سروت_لمائی,
                                      p_Error_کوڈ);

    ucnv_close(وچ_Converter);
    ucnv_close(باہرلا_Converter);

    أرجع نشانہ_لمائی;
}

/* @internal */
ساكن int32_t
ucnv_convertAlgorithmic(UBool convert_منزل_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        حرف *نشانہ, int32_t نشانہ_سمرتھا,
                        ثابت حرف *سروت, int32_t سروت_لمائی,
                        UError_کوڈ *p_Error_کوڈ) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *منزل, *from;
    int32_t نشانہ_لمائی;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    إذا( cnv==خالی_سوچک || سروت==خالی_سوچک || سروت_لمائی<-1 ||
        نشانہ_سمرتھا<0 || (نشانہ_سمرتھا>0 && نشانہ==خالی_سوچک)
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* if there is no input data, we're done */
    إذا(سروت_لمائی==0 || (سروت_لمائی<0 && *سروت==0)) {
        أرجع u_terminateChars(نشانہ, نشانہ_سمرتھا, 0, p_Error_کوڈ);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
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

    نشانہ_لمائی=ucnv_اندرونی_Convert(منزل, from,
                                      نشانہ, نشانہ_سمرتھا,
                                      سروت, سروت_لمائی,
                                      p_Error_کوڈ);

    ucnv_close(algoConverter);

    أرجع نشانہ_لمائی;
}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   حرف *نشانہ, int32_t نشانہ_سمرتھا,
                   ثابت حرف *سروت, int32_t سروت_لمائی,
                   UError_کوڈ *p_Error_کوڈ) {
    أرجع ucnv_convertAlgorithmic(سچ, algorithmicType, cnv,
                                   نشانہ, نشانہ_سمرتھا,
                                   سروت, سروت_لمائی,
                                   p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     حرف *نشانہ, int32_t نشانہ_سمرتھا,
                     ثابت حرف *سروت, int32_t سروت_لمائی,
                     UError_کوڈ *p_Error_کوڈ) UPRV_NO_SANITIZE_UNDEFINED {

    إذا(algorithmicType<0 || UCNV_نمبر_دا_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    أرجع ucnv_convertAlgorithmic(خطأ, algorithmicType, cnv,
                                   نشانہ, نشانہ_سمرتھا,
                                   سروت, سروت_لمائی,
                                   p_Error_کوڈ);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ثابت UConverter* converter)
{
    int8_t type = converter->shared_انکڑے->static_انکڑے->conversionType;
#إذا !UCONFIG_NO_LEGACY_CONVERSION
    إذا(type == UCNV_MBCS) {
        أرجع ucnv_MBCSGetType(converter);
    }
#جے_انت
    أرجع (UConverterType)type;
}

U_CAPI فراغ  U_EXPORT2
ucnv_getStarters(ثابت UConverter* converter, 
                 UBool starters[256],
                 UError_کوڈ* err)
{
    إذا (err == خالی_سوچک || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(converter->shared_انکڑے->impl->getStarters != خالی_سوچک) {
        converter->shared_انکڑے->impl->getStarters(converter, starters, err);
    } وإلا {
        *err = U_ILLEGAL_دلیل_ERROR;
    }
}

ساكن ثابت UAmbiguousConverter *ucnv_getAmbiguous(ثابت UConverter *cnv)
{
    UError_کوڈ error_کوڈ;
    ثابت حرف *name;
    int32_t i;

    إذا(cnv==خالی_سوچک) {
        أرجع خالی_سوچک;
    }

    error_کوڈ=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    لكل(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        إذا(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            أرجع ambiguousConverters+i;
        }
    }

    أرجع خالی_سوچک;
}

U_CAPI فراغ  U_EXPORT2
ucnv_fixFileSeparator(ثابت UConverter *cnv, 
                      char16_t* سروت,
                      int32_t سروت_لمائی) {
    ثابت UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    إذا(cnv==خالی_سوچک || سروت==خالی_سوچک || سروت_لمائی<=0 || (a=ucnv_getAmbiguous(cnv))==خالی_سوچک)
    {
        أرجع;
    }

    variant5c=a->variant5c;
    لكل(i=0; i<سروت_لمائی; ++i) {
        إذا(سروت[i]==variant5c) {
            سروت[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ثابت UConverter *cnv) {
    أرجع ucnv_getAmbiguous(cnv)!=خالی_سوچک;
}

U_CAPI فراغ  U_EXPORT2
ucnv_سیٹ_Fallback(UConverter *cnv, UBool ورتدا_Fallback)
{
    cnv->useFallback = ورتدا_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_ورتدا_Fallback(ثابت UConverter *cnv)
{
    أرجع cnv->useFallback;
}

U_CAPI فراغ  U_EXPORT2
ucnv_getInvalidChars (ثابت UConverter * converter,
                      حرف *err_بائٹاں,
                      int8_t * len,
                      UError_کوڈ * err)
{
    إذا (err == خالی_سوچک || U_FAILURE(*err))
    {
        أرجع;
    }
    إذا (len == خالی_سوچک || err_بائٹاں == خالی_سوچک || converter == خالی_سوچک)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (*len < converter->invalid_Char_لمائی)
    {
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    إذا ((*len = converter->invalid_Char_لمائی) > 0)
    {
        uprv_memcpy (err_بائٹاں, converter->invalid_Char_بفر, *len);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_getInvalidUChars (ثابت UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_کوڈ * err)
{
    إذا (err == خالی_سوچک || U_FAILURE(*err))
    {
        أرجع;
    }
    إذا (len == خالی_سوچک || errChars == خالی_سوچک || converter == خالی_سوچک)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (*len < converter->invalid_UChar_لمائی)
    {
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    إذا ((*len = converter->invalid_UChar_لمائی) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_بفر, *len);
    }
}

#تعریف_کرو SIG_MAX_LEN 5

U_CAPI ثابت حرف* U_EXPORT2
ucnv_detectUnicodeSignature( ثابت حرف* سروت,
                             int32_t سروت_لمائی,
                             int32_t* signature_لمائی,
                             UError_کوڈ* p_Error_کوڈ) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    حرف شروع[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    صحيح i = 0;

    إذا((p_Error_کوڈ==خالی_سوچک) || U_FAILURE(*p_Error_کوڈ)){
        أرجع خالی_سوچک;
    }
    
    إذا(سروت == خالی_سوچک || سروت_لمائی < -1){
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    إذا(signature_لمائی == خالی_سوچک) {
        signature_لمائی = &dummy;
    }

    إذا(سروت_لمائی==-1){
        سروت_لمائی=(int32_t)uprv_strlen(سروت);
    }

    
    طالما(i<سروت_لمائی&& i<SIG_MAX_LEN){
        شروع[i]=سروت[i];
        i++;
    }

    إذا(شروع[0] == '\xFE' && شروع[1] == '\xFF') {
        *signature_لمائی=2;
        أرجع  "UTF-16BE";
    } وإلا إذا(شروع[0] == '\xFF' && شروع[1] == '\xFE') {
        إذا(شروع[2] == '\x00' && شروع[3] =='\x00') {
            *signature_لمائی=4;
            أرجع "UTF-32LE";
        } وإلا {
            *signature_لمائی=2;
            أرجع  "UTF-16LE";
        }
    } وإلا إذا(شروع[0] == '\xEF' && شروع[1] == '\xBB' && شروع[2] == '\xBF') {
        *signature_لمائی=3;
        أرجع  "UTF-8";
    } وإلا إذا(شروع[0] == '\x00' && شروع[1] == '\x00' && 
              شروع[2] == '\xFE' && شروع[3]=='\xFF') {
        *signature_لمائی=4;
        أرجع  "UTF-32BE";
    } وإلا إذا(شروع[0] == '\x0E' && شروع[1] == '\xFE' && شروع[2] == '\xFF') {
        *signature_لمائی=3;
        أرجع "SCSU";
    } وإلا إذا(شروع[0] == '\xFB' && شروع[1] == '\xEE' && شروع[2] == '\x28') {
        *signature_لمائی=3;
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
            *signature_لمائی=5;
            أرجع "UTF-7";
        } وإلا إذا(شروع[3] == '\x38' || شروع[3] == '\x39' || شروع[3] == '\x2B' || شروع[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_لمائی=4;
            أرجع "UTF-7";
        }
    }وإلا إذا(شروع[0]=='\xDD' && شروع[1]== '\x73'&& شروع[2]=='\x66' && شروع[3]=='\x73'){
        *signature_لمائی=4;
        أرجع "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_لمائی=0;
    أرجع خالی_سوچک;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ثابت UConverter* cnv, UError_کوڈ* حالت)
{
    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع -1;
    }
    إذا(cnv == خالی_سوچک){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع -1;
    }

    إذا(cnv->preFromUFirstCP >= 0){
        أرجع U16_لمائی(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }وإلا إذا(cnv->preFromULength < 0){
        أرجع -cnv->preFromULength ;
    }وإلا إذا(cnv->fromUChar32 > 0){
        أرجع 1;
    }
    أرجع 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_UCount_Pending(ثابت UConverter* cnv, UError_کوڈ* حالت){

    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع -1;
    }
    إذا(cnv == خالی_سوچک){
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
ucnv_is_Fixed_چوڑائی(UConverter *cnv, UError_کوڈ *حالت){
    إذا (U_FAILURE(*حالت)) {
        أرجع خطأ;
    }

    إذا (cnv == خالی_سوچک) {
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
            أرجع سچ;
        افتراضي:
            أرجع خطأ;
    }
}
#جے_انت

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
