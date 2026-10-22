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
#وصف_ڪريو CHUNK_سائيز 1024

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
                       UError_ڪوڊ * err)
{
    UConverter *r;

    إذا (err == خالي_اشارو || U_FAILURE (*err)) {
        أرجع خالي_اشارو;
    }

    r =  ucnv_createConverter(خالي_اشارو, name, err);
    أرجع r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ثابت حرف *packageName, ثابت حرف *converterName, UError_ڪوڊ * err)
{
    أرجع ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ثابت char16_t * name,
                         UError_ڪوڊ * err)
{
    حرف asciiName[UCNV_MAX_CONVERTER_NAME_ڊيگهائي];

    إذا (err == خالي_اشارو || U_FAILURE(*err))
        أرجع خالي_اشارو;
    إذا (name == خالي_اشارو)
        أرجع ucnv_open (خالي_اشارو, err);
    إذا (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_ڊيگهائي)
    {
        *err = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
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
                UError_ڪوڊ * err)
{
    حرف myName[UCNV_MAX_CONVERTER_NAME_ڊيگهائي];
    int32_t myNameLen;

    إذا (err == خالي_اشارو || U_FAILURE (*err))
        أرجع خالي_اشارو;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_منزل_String(myName + myNameLen, codepage, 10);

    أرجع ucnv_createConverter(خالي_اشارو, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_محفوظ_Clone(ثابت UConverter* cnv,
               فراغ* ڍير_بفر U_LIFETIME_BOUND,
               int32_t* p_بفر_سائيز,
               UError_ڪوڊ* حالت)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t ڍير_بفر_سائيز;
    int32_t بفر_سائيز_Needed;
    UError_ڪوڊ cbErr;
    UConverter_منزل_Unicode_دليلون منزل_UArgs = {
        حجم(UConverter_منزل_Unicode_دليلون),
            سچ,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو
    };
    UConverter_From_Unicode_دليلون fromUArgs = {
        حجم(UConverter_From_Unicode_دليلون),
            سچ,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو
    };

    UTRACE_داخلا_OC(UTRACE_UCNV_CLONE);

    إذا (حالت == خالي_اشارو || U_FAILURE(*حالت)){
        UTRACE_EXIT_حالت(حالت? *حالت: U_ILLEGAL_دليل_ERROR);
        أرجع خالي_اشارو;
    }

    إذا (cnv == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        UTRACE_EXIT_حالت(*حالت);
        أرجع خالي_اشارو;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, حالت), cnv, ڍير_بفر);

    إذا (cnv->shared_معلومات->impl->محفوظ_Clone != خالي_اشارو) {
        /* call the custom safeClone function for sizing */
        بفر_سائيز_Needed = 0;
        cnv->shared_معلومات->impl->محفوظ_Clone(cnv, خالي_اشارو, &بفر_سائيز_Needed, حالت);
        إذا (U_FAILURE(*حالت)) {
            UTRACE_EXIT_حالت(*حالت);
            أرجع خالي_اشارو;
        }
    }
    وإلا
    {
        /* inherent sizing */
        بفر_سائيز_Needed = حجم(UConverter);
    }

    إذا (p_بفر_سائيز == خالي_اشارو) {
        ڍير_بفر_سائيز = 1;
        p_بفر_سائيز = &ڍير_بفر_سائيز;
    } وإلا {
        ڍير_بفر_سائيز = *p_بفر_سائيز;
        إذا (ڍير_بفر_سائيز <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_بفر_سائيز = بفر_سائيز_Needed;
            UTRACE_EXIT_قدر(بفر_سائيز_Needed);
            أرجع خالي_اشارو;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    إذا (ڍير_بفر) {
        uintptr_t p = reinterpret_cast<uintptr_t>(ڍير_بفر);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t اشارو_Adjustment = aligned_p - p;
        إذا (بفر_سائيز_Needed + اشارو_Adjustment <= ڍير_بفر_سائيز) {
            ڍير_بفر = reinterpret_cast<فراغ *>(aligned_p);
            ڍير_بفر_سائيز -= static_cast<int32_t>(اشارو_Adjustment);
        } وإلا {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            ڍير_بفر_سائيز = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    إذا (ڍير_بفر_سائيز < بفر_سائيز_Needed || ڍير_بفر == خالي_اشارو)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (بفر_سائيز_Needed);

        إذا(localConverter == خالي_اشارو) {
            *حالت = U_يادگيري_ورهاست_ERROR;
            UTRACE_EXIT_حالت(*حالت);
            أرجع خالي_اشارو;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        إذا (p_بفر_سائيز != &ڍير_بفر_سائيز) {
            *حالت = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_بفر_سائيز = بفر_سائيز_Needed;
    } وإلا {
        /* just use the stack buffer */
        localConverter = (UConverter*) ڍير_بفر;
        allocatedConverter = خالي_اشارو;
    }

    uprv_memset(localConverter, 0, بفر_سائيز_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, حجم(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = خطأ;

    /* copy the substitution string */
    إذا (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } وإلا {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_بفر_ڊيگهائي * U_SIZEOF_UCHAR);
        إذا (localConverter->subChars == خالي_اشارو) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_حالت(*حالت);
            أرجع خالي_اشارو;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_بفر_ڊيگهائي * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    إذا (cnv->shared_معلومات->impl->محفوظ_Clone != خالي_اشارو) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_معلومات->impl->محفوظ_Clone(cnv, localConverter, p_بفر_سائيز, حالت);
    }

    إذا(localConverter==خالي_اشارو || U_FAILURE(*حالت)) {
        إذا (allocatedConverter != خالي_اشارو && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_حالت(*حالت);
        أرجع خالي_اشارو;
    }

    /* increment refcount of shared data if needed */
    إذا (cnv->shared_معلومات->isReferenceCounted) {
        ucnv_increment_Ref_ڳڻپ(cnv->shared_معلومات);
    }

    إذا(localConverter == (UConverter*)ڍير_بفر) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = سچ;
    }

    /* allow callback functions to handle any memory allocation */
    منزل_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->منزل_UContext, &منزل_UArgs, خالي_اشارو, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, خالي_اشارو, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_حالت(localConverter, *حالت);
    أرجع localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ثابت UConverter* cnv, UError_ڪوڊ *حالت)
{
    أرجع ucnv_محفوظ_Clone(cnv, خالي_اشارو, خالي_اشارو, حالت);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI فراغ  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;

    UTRACE_داخلا_OC(UTRACE_UCNV_CLOSE);

    إذا (converter == خالي_اشارو)
    {
        UTRACE_EXIT();
        أرجع;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_ڪوڊ), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    إذا (converter->fromCharErrorBehaviour != UCNV_منزل_U_DEFAULT_CALLBACK) {
        UConverter_منزل_Unicode_دليلون منزل_UArgs = {
            حجم(UConverter_منزل_Unicode_دليلون),
                سچ,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو
        };

        منزل_UArgs.converter = converter;
        error_ڪوڊ = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->منزل_UContext, &منزل_UArgs, خالي_اشارو, 0, UCNV_CLOSE, &error_ڪوڊ);
    }
    إذا (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_دليلون fromUArgs = {
            حجم(UConverter_From_Unicode_دليلون),
                سچ,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو
        };
        fromUArgs.converter = converter;
        error_ڪوڊ = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, خالي_اشارو, 0, 0, UCNV_CLOSE, &error_ڪوڊ);
    }

    إذا (converter->shared_معلومات->impl->close != خالي_اشارو) {
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
ucnv_get_موجود_Name (int32_t n)
{
    إذا (0 <= n && n <= 0xffff) {
        UError_ڪوڊ err = U_ZERO_ERROR;
        ثابت حرف *name = ucnv_bld_get_موجود_Converter((uint16_t)n, &err);
        إذا (U_SUCCESS(err)) {
            أرجع name;
        }
    }
    أرجع خالي_اشارو;
}

U_CAPI int32_t   U_EXPORT2
ucnv_ڳڻپ_موجود ()
{
    UError_ڪوڊ err = U_ZERO_ERROR;
    أرجع ucnv_bld_ڳڻپ_موجود_Converters(&err);
}

U_CAPI فراغ    U_EXPORT2
ucnv_getSubstChars (ثابت UConverter * converter,
                    حرف *mySubChar,
                    int8_t * len,
                    UError_ڪوڊ * err)
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
        *err = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI فراغ    U_EXPORT2
ucnv_سيٽ_Subst_Chars (UConverter * converter,
                    ثابت حرف *mySubChar,
                    int8_t len,
                    UError_ڪوڊ * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    إذا ((len > converter->shared_معلومات->static_معلومات->max_بائيٽون_في_Char)
     || (len < converter->shared_معلومات->static_معلومات->min_بائيٽون_في_Char))
    {
        *err = U_ILLEGAL_دليل_ERROR;
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
ucnv_سيٽ_Subst_String(UConverter *cnv,
                    ثابت char16_t *s,
                    int32_t length,
                    UError_ڪوڊ *err) {
    alignas(UConverter) حرف clone_بفر[U_CNV_SAFECLONE_BUFFERSIZE];
    حرف chars[UCNV_ERROR_بفر_ڊيگهائي];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_سائيز, length8;

    /* Let the following functions check all arguments. */
    clone_سائيز = حجم(clone_بفر);
    clone = ucnv_محفوظ_Clone(cnv, clone_بفر, &clone_سائيز, err);
    ucnv_سيٽ_From_UCall_پٺ(clone, UCNV_FROM_U_CALLBACK_STOP, خالي_اشارو, خالي_اشارو, خالي_اشارو, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)حجم(chars), s, length, err);
    ucnv_close(clone);
    إذا (U_FAILURE(*err)) {
        أرجع;
    }

    إذا (cnv->shared_معلومات->impl->writeSub == خالي_اشارو
#إذا !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_معلومات->static_معلومات->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#جيڪڏهن_پورو
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
        إذا (length > UCNV_ERROR_بفر_ڊيگهائي) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_بفر_حد_کان_مٿي_ERROR;
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
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_بفر_ڊيگهائي * U_SIZEOF_UCHAR);
            إذا (cnv->subChars == خالي_اشارو) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_بفر_ڊيگهائي * U_SIZEOF_UCHAR);
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
    إذا(converter == خالي_اشارو) {
        أرجع;
    }

    إذا(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_ڪوڊ error_ڪوڊ;

        إذا(choice<=UCNV_RESET_منزل_UNICODE && converter->fromCharErrorBehaviour != UCNV_منزل_U_DEFAULT_CALLBACK) {
            UConverter_منزل_Unicode_دليلون منزل_UArgs = {
                حجم(UConverter_منزل_Unicode_دليلون),
                سچ,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو
            };
            منزل_UArgs.converter = converter;
            error_ڪوڊ = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->منزل_UContext, &منزل_UArgs, خالي_اشارو, 0, UCNV_RESET, &error_ڪوڊ);
        }
        إذا(choice!=UCNV_RESET_منزل_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_دليلون fromUArgs = {
                حجم(UConverter_From_Unicode_دليلون),
                سچ,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو,
                خالي_اشارو
            };
            fromUArgs.converter = converter;
            error_ڪوڊ = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, خالي_اشارو, 0, 0, UCNV_RESET, &error_ڪوڊ);
        }
    }

    /* now reset the converter itself */
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        converter->منزل_Unicode_حالت = converter->shared_معلومات->منزل_Unicode_حالت;
        converter->گهڻائي = 0;
        converter->منزل_ULength = 0;
        converter->invalid_Char_ڊيگهائي = converter->UChar_Error_بفر_ڊيگهائي = 0;
        converter->pre_منزل_ULength = 0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        converter->from_Unicode_حالت = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_ڊيگهائي = converter->char_Error_بفر_ڊيگهائي = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    إذا (converter->shared_معلومات->impl->reset != خالي_اشارو) {
        /* call the custom reset function */
        converter->shared_معلومات->impl->reset(converter, choice);
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
ucnv_get_Max_Char_سائيز (ثابت UConverter * converter)
{
    أرجع converter->max_بائيٽون_في_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_سائيز (ثابت UConverter * converter)
{
    أرجع converter->shared_معلومات->static_معلومات->min_بائيٽون_في_Char;
}

U_CAPI ثابت حرف* U_EXPORT2
ucnv_getName(ثابت UConverter* converter U_LIFETIME_BOUND, UError_ڪوڊ* err)
{
    إذا (U_FAILURE (*err))
        أرجع خالي_اشارو;
    إذا(converter->shared_معلومات->impl->getName){
        ثابت حرف* عارضي= converter->shared_معلومات->impl->getName(converter);
        إذا(عارضي)
            أرجع عارضي;
    }
    أرجع converter->shared_معلومات->static_معلومات->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ثابت UConverter * converter,
              UError_ڪوڊ * err)
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
                                      UError_ڪوڊ * err)
{
    إذا (U_FAILURE (*err))
        أرجع UCNV_UNKNOWN;

    أرجع (UConverterPlatform)converter->shared_معلومات->static_معلومات->platform;
}

U_CAPI فراغ U_EXPORT2
    ucnv_get_منزل_UCall_پٺ (ثابت UConverter * converter,
                         UConverter_منزل_UCallback *action,
                         ثابت فراغ **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->منزل_UContext;
}

U_CAPI فراغ U_EXPORT2
    ucnv_get_From_UCall_پٺ (ثابت UConverter * converter,
                           UConverterFromUCallback *action,
                           ثابت فراغ **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI فراغ    U_EXPORT2
ucnv_سيٽ_منزل_UCall_پٺ (UConverter * converter,
                            UConverter_منزل_UCallback newAction,
                            ثابت فراغ* newContext,
                            UConverter_منزل_UCallback *oldAction,
                            ثابت فراغ** oldContext,
                            UError_ڪوڊ * err)
{
    إذا (U_FAILURE (*err))
        أرجع;
    إذا (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    إذا (oldContext) *oldContext = converter->منزل_UContext;
    converter->منزل_UContext = newContext;
}

U_CAPI فراغ  U_EXPORT2
ucnv_سيٽ_From_UCall_پٺ (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ثابت فراغ* newContext,
                            UConverterFromUCallback *oldAction,
                            ثابت فراغ** oldContext,
                            UError_ڪوڊ * err)
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
               int32_t ذريعو_اشاريو, int32_t error_داخلا_ڊيگهائي) {
    int32_t *حد_بندي;
    int32_t delta, وٿي;

    إذا(ذريعو_اشاريو>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=ذريعو_اشاريو-error_داخلا_ڊيگهائي;
    } وإلا {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    حد_بندي=offsets+length;
    إذا(delta==0) {
        /* most common case, nothing to do */
    } وإلا إذا(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        طالما(offsets<حد_بندي) {
            وٿي=*offsets;
            إذا(وٿي>=0) {
                *offsets=وٿي+delta;
            }
            ++offsets;
        }
    } وإلا /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        طالما(offsets<حد_بندي) {
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
_fromUnicodeWithCallback(UConverter_From_Unicode_دليلون *p_دليلون, UError_ڪوڊ *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ثابت char16_t *s;
    حرف *t;
    int32_t *offsets;
    int32_t ذريعو_اشاريو;
    int32_t error_داخلا_ڊيگهائي;
    UBool converter_Saw_پڇاڙي_جو_داخلا, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ثابت char16_t *real_ذريعو, *real_ذريعو_حد_بندي;
    int32_t real_ذريعو_اشاريو;
    UBool realFlush;

    cnv=p_دليلون->converter;
    s=p_دليلون->ذريعو;
    t=p_دليلون->هدف;
    offsets=p_دليلون->offsets;

    /* get the converter implementation function */
    ذريعو_اشاريو=0;
    إذا(offsets==خالي_اشارو) {
        fromUnicode=cnv->shared_معلومات->impl->fromUnicode;
    } وإلا {
        fromUnicode=cnv->shared_معلومات->impl->fromUnicodeWithOffsets;
        إذا(fromUnicode==خالي_اشارو) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_معلومات->impl->fromUnicode;
            /* we will write -1 for each offset */
            ذريعو_اشاريو=-1;
        }
    }

    إذا(cnv->preFromULength>=0) {
        /* normal mode */
        real_ذريعو=خالي_اشارو;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ذريعو_حد_بندي=خالي_اشارو;
        realFlush=خطأ;
        real_ذريعو_اشاريو=0;
    } وإلا {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        إذا (!p_دليلون->ذريعو) {
            *err=U_ILLEGAL_دليل_ERROR;
            أرجع;
        }
        real_ذريعو=p_دليلون->ذريعو;
        real_ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
        realFlush=p_دليلون->flush;
        real_ذريعو_اشاريو=ذريعو_اشاريو;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_دليلون->ذريعو=replay;
        p_دليلون->ذريعو_حد_بندي=replay-cnv->preFromULength;
        p_دليلون->flush=خطأ;
        ذريعو_اشاريو=-1;

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
            fromUnicode(p_دليلون, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_پڇاڙي_جو_داخلا=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_دليلون->flush && p_دليلون->ذريعو==p_دليلون->ذريعو_حد_بندي &&
                        cnv->fromUChar32==0);
        } وإلا {
            /* handle error from ucnv_convertEx() */
            converter_Saw_پڇاڙي_جو_داخلا=خطأ;
        }

        /* no callback called yet for this iteration */
        calledCallback=خطأ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_داخلا_ڊيگهائي=0;

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
            إذا(offsets!=خالي_اشارو) {
                int32_t length = static_cast<int32_t>(p_دليلون->هدف - t);
                إذا(length>0) {
                    _updateOffsets(offsets, length, ذريعو_اشاريو, error_داخلا_ڊيگهائي);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_دليلون->offsets=offsets+=length;
                }

                إذا(ذريعو_اشاريو>=0) {
                    ذريعو_اشاريو += static_cast<int32_t>(p_دليلون->ذريعو - s);
                }
            }

            إذا(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                إذا(real_ذريعو==خالي_اشارو) {
                    real_ذريعو=p_دليلون->ذريعو;
                    real_ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
                    realFlush=p_دليلون->flush;
                    real_ذريعو_اشاريو=ذريعو_اشاريو;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_دليلون->ذريعو=replay;
                    p_دليلون->ذريعو_حد_بندي=replay-cnv->preFromULength;
                    p_دليلون->flush=خطأ;
                    إذا((ذريعو_اشاريو+=cnv->preFromULength)<0) {
                        ذريعو_اشاريو=-1;
                    }

                    cnv->preFromULength=0;
                } وإلا {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ذريعو==خالي_اشارو);
                    *err=U_اندروني_پروگرام_ERROR;
                }
            }

            /* update pointers */
            s=p_دليلون->ذريعو;
            t=p_دليلون->هدف;

            إذا(U_SUCCESS(*err)) {
                إذا(s<p_دليلون->ذريعو_حد_بندي) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    اكسر;
                } وإلا إذا(real_ذريعو!=خالي_اشارو) {
                    /* switch back from replaying to the real source and continue */
                    p_دليلون->ذريعو=real_ذريعو;
                    p_دليلون->ذريعو_حد_بندي=real_ذريعو_حد_بندي;
                    p_دليلون->flush=realFlush;
                    ذريعو_اشاريو=real_ذريعو_اشاريو;

                    real_ذريعو=خالي_اشارو;
                    اكسر;
                } وإلا إذا(p_دليلون->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_مليو;
                    calledCallback=خطأ; /* new error condition */
                } وإلا {
                    /* input consumed */
                    إذا(p_دليلون->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        إذا(!converter_Saw_پڇاڙي_جو_داخلا) {
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
                UError_ڪوڊ e;

                إذا( calledCallback ||
                    (e=*err)==U_بفر_حد_کان_مٿي_ERROR ||
                    (e!=U_INVALID_CHAR_مليو &&
                     e!=U_ILLEGAL_CHAR_مليو &&
                     e!=U_TRUNCATED_CHAR_مليو)
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
                    إذا(real_ذريعو!=خالي_اشارو) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_دليلون->ذريعو_حد_بندي - p_دليلون->ذريعو);
                        إذا(length>0) {
                            u_memcpy(cnv->preFromU, p_دليلون->ذريعو, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_دليلون->ذريعو=real_ذريعو;
                        p_دليلون->ذريعو_حد_بندي=real_ذريعو_حد_بندي;
                        p_دليلون->flush=realFlush;
                    }

                    أرجع;
                }
            }

            /* callback handling */
            {
                UChar32 ڪوڊ_نقطو;

                /* get and write the code point */
                ڪوڊ_نقطو=cnv->fromUChar32;
                error_داخلا_ڊيگهائي=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_بفر, error_داخلا_ڊيگهائي, ڪوڊ_نقطو);
                cnv->invalid_UChar_ڊيگهائي = static_cast<int8_t>(error_داخلا_ڊيگهائي);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_دليلون,
                    cnv->invalid_UChar_بفر, error_داخلا_ڊيگهائي, ڪوڊ_نقطو,
                    *err==U_INVALID_CHAR_مليو ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
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
ucnv_output_حد_کان_مٿي_From_Unicode(UConverter *cnv,
                               حرف **هدف, ثابت حرف *هدف_حد_بندي,
                               int32_t **pOffsets,
                               UError_ڪوڊ *err) {
    int32_t *offsets;
    حرف *حد_کان_مٿي, *t;
    int32_t i, length;

    t=*هدف;
    إذا(pOffsets!=خالي_اشارو) {
        offsets=*pOffsets;
    } وإلا {
        offsets=خالي_اشارو;
    }

    حد_کان_مٿي = reinterpret_cast<حرف*>(cnv->char_Error_بفر);
    length=cnv->char_Error_بفر_ڊيگهائي;
    i=0;
    طالما(i<length) {
        إذا(t==هدف_حد_بندي) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            افعل {
                حد_کان_مٿي[j++]=حد_کان_مٿي[i++];
            } طالما(i<length);

            cnv->char_Error_بفر_ڊيگهائي = static_cast<int8_t>(j);
            *هدف=t;
            إذا(offsets!=خالي_اشارو) {
                *pOffsets=offsets;
            }
            *err=U_بفر_حد_کان_مٿي_ERROR;
            أرجع سچ;
        }

        /* copy the overflow contents to the target */
        *t++=حد_کان_مٿي[i++];
        إذا(offsets!=خالي_اشارو) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_بفر_ڊيگهائي=0;
    *هدف=t;
    إذا(offsets!=خالي_اشارو) {
        *pOffsets=offsets;
    }
    أرجع خطأ;
}

U_CAPI فراغ U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 حرف **هدف, ثابت حرف *هدف_حد_بندي,
                 ثابت char16_t **ذريعو, ثابت char16_t *ذريعو_حد_بندي,
                 int32_t *offsets,
                 UBool flush,
                 UError_ڪوڊ *err) {
    UConverter_From_Unicode_دليلون دليلون_2;
    ثابت char16_t *s;
    حرف *t;

    /* check parameters */
    إذا(err==خالي_اشارو || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(cnv==خالي_اشارو || هدف==خالي_اشارو || ذريعو==خالي_اشارو) {
        *err=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    s=*ذريعو;
    t=*هدف;

    إذا ((ثابت فراغ *)U_MAX_PTR(ذريعو_حد_بندي) == (ثابت فراغ *)ذريعو_حد_بندي) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        ذريعو_حد_بندي = (ثابت char16_t *)(((ثابت حرف *)ذريعو_حد_بندي) - 1);
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
    إذا (ذريعو_حد_بندي<s || هدف_حد_بندي<t ||
        ((size_t)(ذريعو_حد_بندي-s)>(size_t)0x3fffffff && ذريعو_حد_بندي>s) ||
        ((size_t)(هدف_حد_بندي-t)>(size_t)0x7fffffff && هدف_حد_بندي>t) ||
        (((ثابت حرف *)ذريعو_حد_بندي-(ثابت حرف *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    
    /* output the target overflow buffer */
    إذا( cnv->char_Error_بفر_ڊيگهائي>0 &&
        ucnv_output_حد_کان_مٿي_From_Unicode(cnv, هدف, هدف_حد_بندي, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        أرجع;
    }
    /* *target may have moved, therefore stop using t */

    إذا(!flush && s==ذريعو_حد_بندي && cnv->preFromULength>=0) {
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
    دليلون_2.converter=cnv;
    دليلون_2.flush=flush;
    دليلون_2.offsets=offsets;
    دليلون_2.ذريعو=s;
    دليلون_2.ذريعو_حد_بندي=ذريعو_حد_بندي;
    دليلون_2.هدف=*هدف;
    دليلون_2.هدف_حد_بندي=هدف_حد_بندي;
    دليلون_2.size=حجم(دليلون_2);

    _fromUnicodeWithCallback(&دليلون_2, err);

    *ذريعو=دليلون_2.ذريعو;
    *هدف=دليلون_2.هدف;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

ساكن فراغ
منزل_Unicode_With_Callback(UConverter_منزل_Unicode_دليلون *p_دليلون, UError_ڪوڊ *err) {
    UConverter_منزل_Unicode منزل_Unicode;
    UConverter *cnv;
    ثابت حرف *s;
    char16_t *t;
    int32_t *offsets;
    int32_t ذريعو_اشاريو;
    int32_t error_داخلا_ڊيگهائي;
    UBool converter_Saw_پڇاڙي_جو_داخلا, calledCallback;

    /* variables for m:n conversion */
    حرف replay[UCNV_EXT_MAX_بائيٽون];
    ثابت حرف *real_ذريعو, *real_ذريعو_حد_بندي;
    int32_t real_ذريعو_اشاريو;
    UBool realFlush;

    cnv=p_دليلون->converter;
    s=p_دليلون->ذريعو;
    t=p_دليلون->هدف;
    offsets=p_دليلون->offsets;

    /* get the converter implementation function */
    ذريعو_اشاريو=0;
    إذا(offsets==خالي_اشارو) {
        منزل_Unicode=cnv->shared_معلومات->impl->منزل_Unicode;
    } وإلا {
        منزل_Unicode=cnv->shared_معلومات->impl->منزل_Unicode_With_Offsets;
        إذا(منزل_Unicode==خالي_اشارو) {
            /* there is no WithOffsets implementation */
            منزل_Unicode=cnv->shared_معلومات->impl->منزل_Unicode;
            /* we will write -1 for each offset */
            ذريعو_اشاريو=-1;
        }
    }

    إذا(cnv->pre_منزل_ULength>=0) {
        /* normal mode */
        real_ذريعو=خالي_اشارو;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ذريعو_حد_بندي=خالي_اشارو;
        realFlush=خطأ;
        real_ذريعو_اشاريو=0;
    } وإلا {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        إذا (!p_دليلون->ذريعو) {
            *err=U_ILLEGAL_دليل_ERROR;
            أرجع;
        }
        real_ذريعو=p_دليلون->ذريعو;
        real_ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
        realFlush=p_دليلون->flush;
        real_ذريعو_اشاريو=ذريعو_اشاريو;

        uprv_memcpy(replay, cnv->pre_منزل_U, -cnv->pre_منزل_ULength);
        p_دليلون->ذريعو=replay;
        p_دليلون->ذريعو_حد_بندي=replay-cnv->pre_منزل_ULength;
        p_دليلون->flush=خطأ;
        ذريعو_اشاريو=-1;

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
            منزل_Unicode(p_دليلون, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_پڇاڙي_جو_داخلا=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_دليلون->flush && p_دليلون->ذريعو==p_دليلون->ذريعو_حد_بندي &&
                        cnv->منزل_ULength==0);
        } وإلا {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_پڇاڙي_جو_داخلا=خطأ;
        }

        /* no callback called yet for this iteration */
        calledCallback=خطأ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_داخلا_ڊيگهائي=0;

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
            إذا(offsets!=خالي_اشارو) {
                int32_t length = static_cast<int32_t>(p_دليلون->هدف - t);
                إذا(length>0) {
                    _updateOffsets(offsets, length, ذريعو_اشاريو, error_داخلا_ڊيگهائي);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_دليلون->offsets=offsets+=length;
                }

                إذا(ذريعو_اشاريو>=0) {
                    ذريعو_اشاريو += static_cast<int32_t>(p_دليلون->ذريعو - s);
                }
            }

            إذا(cnv->pre_منزل_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                إذا(real_ذريعو==خالي_اشارو) {
                    real_ذريعو=p_دليلون->ذريعو;
                    real_ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
                    realFlush=p_دليلون->flush;
                    real_ذريعو_اشاريو=ذريعو_اشاريو;

                    uprv_memcpy(replay, cnv->pre_منزل_U, -cnv->pre_منزل_ULength);
                    p_دليلون->ذريعو=replay;
                    p_دليلون->ذريعو_حد_بندي=replay-cnv->pre_منزل_ULength;
                    p_دليلون->flush=خطأ;
                    إذا((ذريعو_اشاريو+=cnv->pre_منزل_ULength)<0) {
                        ذريعو_اشاريو=-1;
                    }

                    cnv->pre_منزل_ULength=0;
                } وإلا {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ذريعو==خالي_اشارو);
                    *err=U_اندروني_پروگرام_ERROR;
                }
            }

            /* update pointers */
            s=p_دليلون->ذريعو;
            t=p_دليلون->هدف;

            إذا(U_SUCCESS(*err)) {
                إذا(s<p_دليلون->ذريعو_حد_بندي) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    اكسر;
                } وإلا إذا(real_ذريعو!=خالي_اشارو) {
                    /* switch back from replaying to the real source and continue */
                    p_دليلون->ذريعو=real_ذريعو;
                    p_دليلون->ذريعو_حد_بندي=real_ذريعو_حد_بندي;
                    p_دليلون->flush=realFlush;
                    ذريعو_اشاريو=real_ذريعو_اشاريو;

                    real_ذريعو=خالي_اشارو;
                    اكسر;
                } وإلا إذا(p_دليلون->flush && cnv->منزل_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_مليو;
                    calledCallback=خطأ; /* new error condition */
                } وإلا {
                    /* input consumed */
                    إذا(p_دليلون->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        إذا(!converter_Saw_پڇاڙي_جو_داخلا) {
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
                UError_ڪوڊ e;

                إذا( calledCallback ||
                    (e=*err)==U_بفر_حد_کان_مٿي_ERROR ||
                    (e!=U_INVALID_CHAR_مليو &&
                     e!=U_ILLEGAL_CHAR_مليو &&
                     e!=U_TRUNCATED_CHAR_مليو &&
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
                    إذا(real_ذريعو!=خالي_اشارو) {
                        int32_t length;

                        U_ASSERT(cnv->pre_منزل_ULength==0);

                        length = static_cast<int32_t>(p_دليلون->ذريعو_حد_بندي - p_دليلون->ذريعو);
                        إذا(length>0) {
                            uprv_memcpy(cnv->pre_منزل_U, p_دليلون->ذريعو, length);
                            cnv->pre_منزل_ULength = static_cast<int8_t>(-length);
                        }

                        p_دليلون->ذريعو=real_ذريعو;
                        p_دليلون->ذريعو_حد_بندي=real_ذريعو_حد_بندي;
                        p_دليلون->flush=realFlush;
                    }

                    أرجع;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_داخلا_ڊيگهائي=cnv->invalid_Char_ڊيگهائي=cnv->منزل_ULength;
            إذا(error_داخلا_ڊيگهائي>0) {
                uprv_memcpy(cnv->invalid_Char_بفر, cnv->منزل_UBytes, error_داخلا_ڊيگهائي);
            }

            /* set the converter state to deal with the next character */
            cnv->منزل_ULength=0;

            /* call the callback function */
            إذا(cnv->منزل_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_مليو) {
                cnv->منزل_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->منزل_UContext, p_دليلون,
                cnv->invalid_Char_بفر, error_داخلا_ڊيگهائي,
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
ucnv_output_حد_کان_مٿي_منزل_Unicode(UConverter *cnv,
                             char16_t **هدف, ثابت char16_t *هدف_حد_بندي,
                             int32_t **pOffsets,
                             UError_ڪوڊ *err) {
    int32_t *offsets;
    char16_t *حد_کان_مٿي, *t;
    int32_t i, length;

    t=*هدف;
    إذا(pOffsets!=خالي_اشارو) {
        offsets=*pOffsets;
    } وإلا {
        offsets=خالي_اشارو;
    }

    حد_کان_مٿي=cnv->UChar_Error_بفر;
    length=cnv->UChar_Error_بفر_ڊيگهائي;
    i=0;
    طالما(i<length) {
        إذا(t==هدف_حد_بندي) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            افعل {
                حد_کان_مٿي[j++]=حد_کان_مٿي[i++];
            } طالما(i<length);

            cnv->UChar_Error_بفر_ڊيگهائي = static_cast<int8_t>(j);
            *هدف=t;
            إذا(offsets!=خالي_اشارو) {
                *pOffsets=offsets;
            }
            *err=U_بفر_حد_کان_مٿي_ERROR;
            أرجع سچ;
        }

        /* copy the overflow contents to the target */
        *t++=حد_کان_مٿي[i++];
        إذا(offsets!=خالي_اشارو) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_بفر_ڊيگهائي=0;
    *هدف=t;
    إذا(offsets!=خالي_اشارو) {
        *pOffsets=offsets;
    }
    أرجع خطأ;
}

U_CAPI فراغ U_EXPORT2
ucnv_منزل_Unicode(UConverter *cnv,
               char16_t **هدف, ثابت char16_t *هدف_حد_بندي,
               ثابت حرف **ذريعو, ثابت حرف *ذريعو_حد_بندي,
               int32_t *offsets,
               UBool flush,
               UError_ڪوڊ *err) {
    UConverter_منزل_Unicode_دليلون دليلون_2;
    ثابت حرف *s;
    char16_t *t;

    /* check parameters */
    إذا(err==خالي_اشارو || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(cnv==خالي_اشارو || هدف==خالي_اشارو || ذريعو==خالي_اشارو) {
        *err=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    s=*ذريعو;
    t=*هدف;

    إذا ((ثابت فراغ *)U_MAX_PTR(هدف_حد_بندي) == (ثابت فراغ *)هدف_حد_بندي) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        هدف_حد_بندي = (ثابت char16_t *)(((ثابت حرف *)هدف_حد_بندي) - 1);
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
    إذا (ذريعو_حد_بندي<s || هدف_حد_بندي<t ||
        ((size_t)(ذريعو_حد_بندي-s)>(size_t)0x7fffffff && ذريعو_حد_بندي>s) ||
        ((size_t)(هدف_حد_بندي-t)>(size_t)0x3fffffff && هدف_حد_بندي>t) ||
        (((ثابت حرف *)هدف_حد_بندي-(ثابت حرف *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    
    /* output the target overflow buffer */
    إذا( cnv->UChar_Error_بفر_ڊيگهائي>0 &&
        ucnv_output_حد_کان_مٿي_منزل_Unicode(cnv, هدف, هدف_حد_بندي, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        أرجع;
    }
    /* *target may have moved, therefore stop using t */

    إذا(!flush && s==ذريعو_حد_بندي && cnv->pre_منزل_ULength>=0) {
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
    دليلون_2.converter=cnv;
    دليلون_2.flush=flush;
    دليلون_2.offsets=offsets;
    دليلون_2.ذريعو=s;
    دليلون_2.ذريعو_حد_بندي=ذريعو_حد_بندي;
    دليلون_2.هدف=*هدف;
    دليلون_2.هدف_حد_بندي=هدف_حد_بندي;
    دليلون_2.size=حجم(دليلون_2);

    منزل_Unicode_With_Callback(&دليلون_2, err);

    *ذريعو=دليلون_2.ذريعو;
    *هدف=دليلون_2.هدف;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                حرف *dest, int32_t dest_صلاحيت,
                ثابت char16_t *src, int32_t src_ڊيگهائي,
                UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت char16_t *src_حد_بندي;
    حرف *originalDest, *dest_حد_بندي;
    int32_t dest_ڊيگهائي;

    /* check arguments */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    إذا( cnv==خالي_اشارو ||
        dest_صلاحيت<0 || (dest_صلاحيت>0 && dest==خالي_اشارو) ||
        src_ڊيگهائي<-1 || (src_ڊيگهائي!=0 && src==خالي_اشارو)
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    إذا(src_ڊيگهائي==-1) {
        src_ڊيگهائي=u_strlen(src);
    }
    إذا(src_ڊيگهائي>0) {
        src_حد_بندي=src+src_ڊيگهائي;
        dest_صلاحيت=pin_صلاحيت(dest, dest_صلاحيت);
        dest_حد_بندي=dest+dest_صلاحيت;

        /* perform the conversion */
        UError_ڪوڊ بفر_حالت = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_حد_بندي, &src, src_حد_بندي, خالي_اشارو, سچ, &بفر_حالت);
        dest_ڊيگهائي=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        إذا(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR) {
            حرف بفر[1024];

            dest_حد_بندي=بفر+حجم(بفر);
            افعل {
                dest=بفر;
                بفر_حالت=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_حد_بندي, &src, src_حد_بندي, خالي_اشارو, سچ, &بفر_حالت);
                dest_ڊيگهائي+=(int32_t)(dest-بفر);
            } طالما(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR);
        }
        إذا (U_FAILURE(بفر_حالت)) {
            *p_Error_ڪوڊ = بفر_حالت;
        }
    } وإلا {
        dest_ڊيگهائي=0;
    }

    أرجع u_terminateChars(originalDest, dest_صلاحيت, dest_ڊيگهائي, p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_صلاحيت,
              ثابت حرف *src, int32_t src_ڊيگهائي,
              UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت حرف *src_حد_بندي;
    char16_t *originalDest, *dest_حد_بندي;
    int32_t dest_ڊيگهائي;

    /* check arguments */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    إذا( cnv==خالي_اشارو ||
        dest_صلاحيت<0 || (dest_صلاحيت>0 && dest==خالي_اشارو) ||
        src_ڊيگهائي<-1 || (src_ڊيگهائي!=0 && src==خالي_اشارو))
    {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* initialize */
    ucnv_reset_منزل_Unicode(cnv);
    originalDest=dest;
    إذا(src_ڊيگهائي==-1) {
        src_ڊيگهائي=(int32_t)uprv_strlen(src);
    }
    إذا(src_ڊيگهائي>0) {
        src_حد_بندي=src+src_ڊيگهائي;
        dest_صلاحيت=pin_صلاحيت(dest, dest_صلاحيت);
        dest_حد_بندي=dest+dest_صلاحيت;

        /* perform the conversion */
        UError_ڪوڊ بفر_حالت = U_ZERO_ERROR;
        ucnv_منزل_Unicode(cnv, &dest, dest_حد_بندي, &src, src_حد_بندي, خالي_اشارو, سچ, &بفر_حالت);
        dest_ڊيگهائي=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        إذا(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR)
        {
            char16_t بفر[1024];

            dest_حد_بندي=بفر+UPRV_LENGTHOF(بفر);
            افعل {
                dest=بفر;
                بفر_حالت=U_ZERO_ERROR;
                ucnv_منزل_Unicode(cnv, &dest, dest_حد_بندي, &src, src_حد_بندي, خالي_اشارو, سچ, &بفر_حالت);
                dest_ڊيگهائي+=(int32_t)(dest-بفر);
            }
            طالما(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR);
        }
        إذا (U_FAILURE(بفر_حالت)) {
            *p_Error_ڪوڊ = بفر_حالت;
        }
    } وإلا {
        dest_ڊيگهائي=0;
    }

    أرجع u_terminateUChars(originalDest, dest_صلاحيت, dest_ڊيگهائي, p_Error_ڪوڊ);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_ايندڙ_UChar(UConverter *cnv,
                  ثابت حرف **ذريعو, ثابت حرف *ذريعو_حد_بندي,
                  UError_ڪوڊ *err) {
    UConverter_منزل_Unicode_دليلون دليلون_2;
    char16_t بفر[U16_MAX_ڊيگهائي];
    ثابت حرف *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    إذا(err==خالي_اشارو || U_FAILURE(*err)) {
        أرجع 0xffff;
    }

    إذا(cnv==خالي_اشارو || ذريعو==خالي_اشارو) {
        *err=U_ILLEGAL_دليل_ERROR;
        أرجع 0xffff;
    }

    s=*ذريعو;
    إذا(ذريعو_حد_بندي<s) {
        *err=U_ILLEGAL_دليل_ERROR;
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
    إذا(((size_t)(ذريعو_حد_بندي-s)>(size_t)0x7fffffff && ذريعو_حد_بندي>s)) {
        *err=U_ILLEGAL_دليل_ERROR;
        أرجع 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    إذا(cnv->UChar_Error_بفر_ڊيگهائي>0) {
        char16_t *حد_کان_مٿي;

        حد_کان_مٿي=cnv->UChar_Error_بفر;
        i=0;
        length=cnv->UChar_Error_بفر_ڊيگهائي;
        U16_ايندڙ(حد_کان_مٿي, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        إذا((cnv->UChar_Error_بفر_ڊيگهائي=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_بفر, cnv->UChar_Error_بفر+i,
                         cnv->UChar_Error_بفر_ڊيگهائي*U_SIZEOF_UCHAR);
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
    دليلون_2.converter=cnv;
    دليلون_2.flush=سچ;
    دليلون_2.offsets=خالي_اشارو;
    دليلون_2.ذريعو=s;
    دليلون_2.ذريعو_حد_بندي=ذريعو_حد_بندي;
    دليلون_2.هدف=بفر;
    دليلون_2.هدف_حد_بندي=بفر+1;
    دليلون_2.size=حجم(دليلون_2);

    إذا(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        إذا(cnv->منزل_ULength==0 && cnv->shared_معلومات->impl->get_ايندڙ_UChar!=خالي_اشارو) {
            c=cnv->shared_معلومات->impl->get_ايندڙ_UChar(&دليلون_2, err);
            *ذريعو=s=دليلون_2.ذريعو;
            إذا(*err==U_اشاريو_OUTOFBOUNDS_ERROR) {
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
        منزل_Unicode_With_Callback(&دليلون_2, err);

        إذا(*err==U_بفر_حد_کان_مٿي_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(دليلون_2.هدف-بفر);
    } وإلا {
        /* write the lead surrogate from the overflow buffer */
        بفر[0]=(char16_t)c;
        دليلون_2.هدف=بفر+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    إذا(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } وإلا إذا(length==0) {
        /* no input or only state changes */
        *err=U_اشاريو_OUTOFBOUNDS_ERROR;
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

            إذا(cnv->UChar_Error_بفر_ڊيگهائي>0) {
                /* got overflow output from the conversion */
                إذا(U16_IS_TRAIL(c2=cnv->UChar_Error_بفر[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    إذا((--cnv->UChar_Error_بفر_ڊيگهائي)>0) {
                        uprv_memmove(cnv->UChar_Error_بفر, cnv->UChar_Error_بفر+1,
                                     cnv->UChar_Error_بفر_ڊيگهائي*U_SIZEOF_UCHAR);
                    }
                } وإلا {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } وإلا إذا(دليلون_2.ذريعو<ذريعو_حد_بندي) {
                /* convert once more, to buffer[1] */
                دليلون_2.هدف_حد_بندي=بفر+2;
                منزل_Unicode_With_Callback(&دليلون_2, err);
                إذا(*err==U_بفر_حد_کان_مٿي_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(دليلون_2.هدف-بفر);
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
        إذا((length=cnv->UChar_Error_بفر_ڊيگهائي)>0) {
            uprv_memmove(cnv->UChar_Error_بفر+delta, cnv->UChar_Error_بفر,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_بفر_ڊيگهائي=(int8_t)(length+delta);

        cnv->UChar_Error_بفر[0]=بفر[i++];
        إذا(delta>1) {
            cnv->UChar_Error_بفر[1]=بفر[i];
        }
    }

    *ذريعو=دليلون_2.ذريعو;
    أرجع c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI فراغ U_EXPORT2
ucnv_convertEx(UConverter *هدف_Cnv, UConverter *ذريعو_Cnv,
               حرف **هدف, ثابت حرف *هدف_حد_بندي,
               ثابت حرف **ذريعو, ثابت حرف *ذريعو_حد_بندي,
               char16_t *محور_شروعات, char16_t **محور_ذريعو,
               char16_t **محور_هدف, ثابت char16_t *محور_حد_بندي,
               UBool reset, UBool flush,
               UError_ڪوڊ *p_Error_ڪوڊ) {
    char16_t محور_بفر[CHUNK_سائيز];
    ثابت char16_t *my_محور_ذريعو;
    char16_t *my_محور_هدف;
    ثابت حرف *s;
    حرف *t;

    UConverter_منزل_Unicode_دليلون منزل_UArgs;
    UConverter_From_Unicode_دليلون fromUArgs;
    UConverterConvert convert;

    /* error checking */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }

    إذا( هدف_Cnv==خالي_اشارو || ذريعو_Cnv==خالي_اشارو ||
        ذريعو==خالي_اشارو || *ذريعو==خالي_اشارو ||
        هدف==خالي_اشارو || *هدف==خالي_اشارو || هدف_حد_بندي==خالي_اشارو
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    s=*ذريعو;
    t=*هدف;
    إذا((ذريعو_حد_بندي!=خالي_اشارو && ذريعو_حد_بندي<s) || هدف_حد_بندي<t) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    إذا(
        (ذريعو_حد_بندي!=خالي_اشارو && ((size_t)(ذريعو_حد_بندي-s)>(size_t)0x7fffffff && ذريعو_حد_بندي>s)) ||
        ((size_t)(هدف_حد_بندي-t)>(size_t)0x7fffffff && هدف_حد_بندي>t)
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    
    إذا(محور_شروعات==خالي_اشارو) {
        إذا(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
            أرجع;
        }

        /* use the stack pivot buffer */
        my_محور_ذريعو=my_محور_هدف=محور_شروعات=محور_بفر;
        محور_ذريعو=(char16_t **)&my_محور_ذريعو;
        محور_هدف=&my_محور_هدف;
        محور_حد_بندي=محور_بفر+CHUNK_سائيز;
    } وإلا إذا(  محور_شروعات>=محور_حد_بندي ||
                محور_ذريعو==خالي_اشارو || *محور_ذريعو==خالي_اشارو ||
                محور_هدف==خالي_اشارو || *محور_هدف==خالي_اشارو ||
                محور_حد_بندي==خالي_اشارو
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    إذا(ذريعو_حد_بندي==خالي_اشارو) {
        /* get limit of single-byte-NUL-terminated source string */
        ذريعو_حد_بندي=uprv_strchr(*ذريعو, 0);
    }

    إذا(reset) {
        ucnv_reset_منزل_Unicode(ذريعو_Cnv);
        ucnv_resetFromUnicode(هدف_Cnv);
        *محور_ذريعو=*محور_هدف=محور_شروعات;
    } وإلا إذا(هدف_Cnv->char_Error_بفر_ڊيگهائي>0) {
        /* output the targetCnv overflow buffer */
        إذا(ucnv_output_حد_کان_مٿي_From_Unicode(هدف_Cnv, هدف, هدف_حد_بندي, خالي_اشارو, p_Error_ڪوڊ)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            أرجع;
        }
        /* *target has moved, therefore stop using t */

        إذا( !flush &&
            هدف_Cnv->preFromULength>=0 && *محور_ذريعو==*محور_هدف &&
            ذريعو_Cnv->UChar_Error_بفر_ڊيگهائي==0 && ذريعو_Cnv->pre_منزل_ULength>=0 && s==ذريعو_حد_بندي
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            أرجع;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    إذا( ذريعو_Cnv->shared_معلومات->static_معلومات->conversionType==UCNV_UTF8 &&
        هدف_Cnv->shared_معلومات->impl->fromUTF8!=خالي_اشارو
    ) {
        convert=هدف_Cnv->shared_معلومات->impl->fromUTF8;
    } وإلا إذا( هدف_Cnv->shared_معلومات->static_معلومات->conversionType==UCNV_UTF8 &&
               ذريعو_Cnv->shared_معلومات->impl->منزل_UTF8!=خالي_اشارو
    ) {
        convert=ذريعو_Cnv->shared_معلومات->impl->منزل_UTF8;
    } وإلا {
        convert=خالي_اشارو;
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
    إذا(convert!=خالي_اشارو && (محور_حد_بندي-محور_شروعات)>32) {
        محور_حد_بندي=محور_شروعات+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=هدف_Cnv;
    fromUArgs.flush=خطأ;
    fromUArgs.offsets=خالي_اشارو;
    fromUArgs.هدف=*هدف;
    fromUArgs.هدف_حد_بندي=هدف_حد_بندي;
    fromUArgs.size=حجم(fromUArgs);

    منزل_UArgs.converter=ذريعو_Cnv;
    منزل_UArgs.flush=flush;
    منزل_UArgs.offsets=خالي_اشارو;
    منزل_UArgs.ذريعو=s;
    منزل_UArgs.ذريعو_حد_بندي=ذريعو_حد_بندي;
    منزل_UArgs.هدف_حد_بندي=محور_حد_بندي;
    منزل_UArgs.size=حجم(منزل_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=خالي_اشارو;

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
        إذا( *محور_ذريعو<*محور_هدف ||
            U_FAILURE(*p_Error_ڪوڊ) ||
            هدف_Cnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.ذريعو=*محور_ذريعو;
            fromUArgs.ذريعو_حد_بندي=*محور_هدف;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_ڪوڊ);
            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                /* target overflow, or conversion error */
                *محور_ذريعو=(char16_t *)fromUArgs.ذريعو;
                اكسر;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *محور_ذريعو=*محور_هدف=محور_شروعات;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        إذا(ذريعو_Cnv->UChar_Error_بفر_ڊيگهائي>0) {
            إذا(ucnv_output_حد_کان_مٿي_منزل_Unicode(ذريعو_Cnv, محور_هدف, محور_حد_بندي, خالي_اشارو, p_Error_ڪوڊ)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_ڪوڊ=U_ZERO_ERROR;
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
        إذا( منزل_UArgs.ذريعو==ذريعو_حد_بندي &&
            ذريعو_Cnv->pre_منزل_ULength>=0 && ذريعو_Cnv->منزل_ULength==0 &&
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
        إذا(convert!=خالي_اشارو && هدف_Cnv->preFromUFirstCP<0 && ذريعو_Cnv->pre_منزل_ULength==0) {
            إذا(*p_Error_ڪوڊ==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_ڪوڊ=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &منزل_UArgs, p_Error_ڪوڊ);
            إذا(*p_Error_ڪوڊ==U_بفر_حد_کان_مٿي_ERROR) {
                اكسر;
            } وإلا إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                إذا(ذريعو_Cnv->منزل_ULength>0) {
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
                    *محور_ذريعو=*محور_هدف=محور_شروعات+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    استمر;
                }
            } وإلا إذا(*p_Error_ڪوڊ==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_ڪوڊ=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } وإلا إذا(flush && ذريعو_Cnv->منزل_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_ڪوڊ=U_TRUNCATED_CHAR_مليو;
            } وإلا {
                /* input consumed */
                إذا(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(ذريعو_Cnv, UCNV_RESET_منزل_UNICODE, خطأ);
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
        منزل_UArgs.هدف=محور_شروعات; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        منزل_Unicode_With_Callback(&منزل_UArgs, p_Error_ڪوڊ);
        *محور_هدف=منزل_UArgs.هدف;
        إذا(*p_Error_ڪوڊ==U_بفر_حد_کان_مٿي_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_ڪوڊ=U_ZERO_ERROR;
        } وإلا إذا(U_FAILURE(*p_Error_ڪوڊ) || (!flush && *محور_هدف==محور_شروعات)) {
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
        إذا( flush && منزل_UArgs.ذريعو==ذريعو_حد_بندي &&
            ذريعو_Cnv->pre_منزل_ULength>=0 &&
            ذريعو_Cnv->UChar_Error_بفر_ڊيگهائي==0
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

    *ذريعو=منزل_UArgs.ذريعو;
    *هدف=fromUArgs.هدف;

    /* terminate the target buffer if possible */
    إذا(flush && U_SUCCESS(*p_Error_ڪوڊ)) {
        إذا(*هدف!=هدف_حد_بندي) {
            **هدف=0;
            إذا(*p_Error_ڪوڊ==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_ڪوڊ=U_ZERO_ERROR;
            }
        } وإلا {
            *p_Error_ڪوڊ=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
ساكن int32_t
ucnv_اندروني_Convert(UConverter *ٻاهريون_Converter, UConverter *اندريون_Converter,
                     حرف *هدف, int32_t هدف_صلاحيت,
                     ثابت حرف *ذريعو, int32_t ذريعو_ڊيگهائي,
                     UError_ڪوڊ *p_Error_ڪوڊ) {
    char16_t محور_بفر[CHUNK_سائيز];
    char16_t *محور, *pivot2;

    حرف *my_هدف;
    ثابت حرف *ذريعو_حد_بندي;
    ثابت حرف *هدف_حد_بندي;
    int32_t هدف_ڊيگهائي=0;

    /* set up */
    إذا(ذريعو_ڊيگهائي<0) {
        ذريعو_حد_بندي=uprv_strchr(ذريعو, 0);
    } وإلا {
        ذريعو_حد_بندي=ذريعو+ذريعو_ڊيگهائي;
    }

    /* if there is no input data, we're done */
    إذا(ذريعو==ذريعو_حد_بندي) {
        أرجع u_terminateChars(هدف, هدف_صلاحيت, 0, p_Error_ڪوڊ);
    }

    محور=pivot2=محور_بفر;
    my_هدف=هدف;
    هدف_ڊيگهائي=0;

    إذا(هدف_صلاحيت>0) {
        /* perform real conversion */
        هدف_حد_بندي=هدف+هدف_صلاحيت;
        ucnv_convertEx(ٻاهريون_Converter, اندريون_Converter,
                       &my_هدف, هدف_حد_بندي,
                       &ذريعو, ذريعو_حد_بندي,
                       محور_بفر, &محور, &pivot2, محور_بفر+CHUNK_سائيز,
                       خطأ,
                       سچ,
                       p_Error_ڪوڊ);
        هدف_ڊيگهائي = static_cast<int32_t>(my_هدف - هدف);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    إذا(*p_Error_ڪوڊ==U_بفر_حد_کان_مٿي_ERROR || هدف_صلاحيت==0)
    {
        حرف هدف_بفر[CHUNK_سائيز];

        هدف_حد_بندي=هدف_بفر+CHUNK_سائيز;
        افعل {
            *p_Error_ڪوڊ=U_ZERO_ERROR;
            my_هدف=هدف_بفر;
            ucnv_convertEx(ٻاهريون_Converter, اندريون_Converter,
                           &my_هدف, هدف_حد_بندي,
                           &ذريعو, ذريعو_حد_بندي,
                           محور_بفر, &محور, &pivot2, محور_بفر+CHUNK_سائيز,
                           خطأ,
                           سچ,
                           p_Error_ڪوڊ);
            هدف_ڊيگهائي += static_cast<int32_t>(my_هدف - هدف_بفر);
        } طالما(*p_Error_ڪوڊ==U_بفر_حد_کان_مٿي_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        أرجع u_terminateChars(هدف, هدف_صلاحيت, هدف_ڊيگهائي, p_Error_ڪوڊ);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    أرجع هدف_ڊيگهائي;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ثابت حرف *منزل_Converter_Name, ثابت حرف *fromConverterName,
             حرف *هدف, int32_t هدف_صلاحيت,
             ثابت حرف *ذريعو, int32_t ذريعو_ڊيگهائي,
             UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter in, out; /* stack-allocated */
    UConverter *اندريون_Converter, *ٻاهريون_Converter;
    int32_t هدف_ڊيگهائي;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    إذا( ذريعو==خالي_اشارو || ذريعو_ڊيگهائي<-1 ||
        هدف_صلاحيت<0 || (هدف_صلاحيت>0 && هدف==خالي_اشارو)
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* if there is no input data, we're done */
    إذا(ذريعو_ڊيگهائي==0 || (ذريعو_ڊيگهائي<0 && *ذريعو==0)) {
        أرجع u_terminateChars(هدف, هدف_صلاحيت, 0, p_Error_ڪوڊ);
    }

    /* create the converters */
    اندريون_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    ٻاهريون_Converter=ucnv_createConverter(&out, منزل_Converter_Name, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        ucnv_close(اندريون_Converter);
        أرجع 0;
    }

    هدف_ڊيگهائي=ucnv_اندروني_Convert(ٻاهريون_Converter, اندريون_Converter,
                                      هدف, هدف_صلاحيت,
                                      ذريعو, ذريعو_ڊيگهائي,
                                      p_Error_ڪوڊ);

    ucnv_close(اندريون_Converter);
    ucnv_close(ٻاهريون_Converter);

    أرجع هدف_ڊيگهائي;
}

/* @internal */
ساكن int32_t
ucnv_convertAlgorithmic(UBool convert_منزل_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        حرف *هدف, int32_t هدف_صلاحيت,
                        ثابت حرف *ذريعو, int32_t ذريعو_ڊيگهائي,
                        UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *منزل, *from;
    int32_t هدف_ڊيگهائي;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    إذا( cnv==خالي_اشارو || ذريعو==خالي_اشارو || ذريعو_ڊيگهائي<-1 ||
        هدف_صلاحيت<0 || (هدف_صلاحيت>0 && هدف==خالي_اشارو)
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* if there is no input data, we're done */
    إذا(ذريعو_ڊيگهائي==0 || (ذريعو_ڊيگهائي<0 && *ذريعو==0)) {
        أرجع u_terminateChars(هدف, هدف_صلاحيت, 0, p_Error_ڪوڊ);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
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

    هدف_ڊيگهائي=ucnv_اندروني_Convert(منزل, from,
                                      هدف, هدف_صلاحيت,
                                      ذريعو, ذريعو_ڊيگهائي,
                                      p_Error_ڪوڊ);

    ucnv_close(algoConverter);

    أرجع هدف_ڊيگهائي;
}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   حرف *هدف, int32_t هدف_صلاحيت,
                   ثابت حرف *ذريعو, int32_t ذريعو_ڊيگهائي,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع ucnv_convertAlgorithmic(سچ, algorithmicType, cnv,
                                   هدف, هدف_صلاحيت,
                                   ذريعو, ذريعو_ڊيگهائي,
                                   p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     حرف *هدف, int32_t هدف_صلاحيت,
                     ثابت حرف *ذريعو, int32_t ذريعو_ڊيگهائي,
                     UError_ڪوڊ *p_Error_ڪوڊ) UPRV_NO_SANITIZE_UNDEFINED {

    إذا(algorithmicType<0 || UCNV_عدد_جو_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    أرجع ucnv_convertAlgorithmic(خطأ, algorithmicType, cnv,
                                   هدف, هدف_صلاحيت,
                                   ذريعو, ذريعو_ڊيگهائي,
                                   p_Error_ڪوڊ);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ثابت UConverter* converter)
{
    int8_t type = converter->shared_معلومات->static_معلومات->conversionType;
#إذا !UCONFIG_NO_LEGACY_CONVERSION
    إذا(type == UCNV_MBCS) {
        أرجع ucnv_MBCSGetType(converter);
    }
#جيڪڏهن_پورو
    أرجع (UConverterType)type;
}

U_CAPI فراغ  U_EXPORT2
ucnv_getStarters(ثابت UConverter* converter, 
                 UBool starters[256],
                 UError_ڪوڊ* err)
{
    إذا (err == خالي_اشارو || U_FAILURE(*err)) {
        أرجع;
    }

    إذا(converter->shared_معلومات->impl->getStarters != خالي_اشارو) {
        converter->shared_معلومات->impl->getStarters(converter, starters, err);
    } وإلا {
        *err = U_ILLEGAL_دليل_ERROR;
    }
}

ساكن ثابت UAmbiguousConverter *ucnv_getAmbiguous(ثابت UConverter *cnv)
{
    UError_ڪوڊ error_ڪوڊ;
    ثابت حرف *name;
    int32_t i;

    إذا(cnv==خالي_اشارو) {
        أرجع خالي_اشارو;
    }

    error_ڪوڊ=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    لكل(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        إذا(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            أرجع ambiguousConverters+i;
        }
    }

    أرجع خالي_اشارو;
}

U_CAPI فراغ  U_EXPORT2
ucnv_fixFileSeparator(ثابت UConverter *cnv, 
                      char16_t* ذريعو,
                      int32_t ذريعو_ڊيگهائي) {
    ثابت UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    إذا(cnv==خالي_اشارو || ذريعو==خالي_اشارو || ذريعو_ڊيگهائي<=0 || (a=ucnv_getAmbiguous(cnv))==خالي_اشارو)
    {
        أرجع;
    }

    variant5c=a->variant5c;
    لكل(i=0; i<ذريعو_ڊيگهائي; ++i) {
        إذا(ذريعو[i]==variant5c) {
            ذريعو[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ثابت UConverter *cnv) {
    أرجع ucnv_getAmbiguous(cnv)!=خالي_اشارو;
}

U_CAPI فراغ  U_EXPORT2
ucnv_سيٽ_Fallback(UConverter *cnv, UBool استعمال_ڪري_Fallback)
{
    cnv->useFallback = استعمال_ڪري_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_استعمال_ڪري_Fallback(ثابت UConverter *cnv)
{
    أرجع cnv->useFallback;
}

U_CAPI فراغ  U_EXPORT2
ucnv_getInvalidChars (ثابت UConverter * converter,
                      حرف *err_بائيٽون,
                      int8_t * len,
                      UError_ڪوڊ * err)
{
    إذا (err == خالي_اشارو || U_FAILURE(*err))
    {
        أرجع;
    }
    إذا (len == خالي_اشارو || err_بائيٽون == خالي_اشارو || converter == خالي_اشارو)
    {
        *err = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا (*len < converter->invalid_Char_ڊيگهائي)
    {
        *err = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    إذا ((*len = converter->invalid_Char_ڊيگهائي) > 0)
    {
        uprv_memcpy (err_بائيٽون, converter->invalid_Char_بفر, *len);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_getInvalidUChars (ثابت UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_ڪوڊ * err)
{
    إذا (err == خالي_اشارو || U_FAILURE(*err))
    {
        أرجع;
    }
    إذا (len == خالي_اشارو || errChars == خالي_اشارو || converter == خالي_اشارو)
    {
        *err = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا (*len < converter->invalid_UChar_ڊيگهائي)
    {
        *err = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    إذا ((*len = converter->invalid_UChar_ڊيگهائي) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_بفر, *len);
    }
}

#وصف_ڪريو SIG_MAX_LEN 5

U_CAPI ثابت حرف* U_EXPORT2
ucnv_detectUnicodeSignature( ثابت حرف* ذريعو,
                             int32_t ذريعو_ڊيگهائي,
                             int32_t* signature_ڊيگهائي,
                             UError_ڪوڊ* p_Error_ڪوڊ) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    حرف شروعات[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    صحيح i = 0;

    إذا((p_Error_ڪوڊ==خالي_اشارو) || U_FAILURE(*p_Error_ڪوڊ)){
        أرجع خالي_اشارو;
    }
    
    إذا(ذريعو == خالي_اشارو || ذريعو_ڊيگهائي < -1){
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    إذا(signature_ڊيگهائي == خالي_اشارو) {
        signature_ڊيگهائي = &dummy;
    }

    إذا(ذريعو_ڊيگهائي==-1){
        ذريعو_ڊيگهائي=(int32_t)uprv_strlen(ذريعو);
    }

    
    طالما(i<ذريعو_ڊيگهائي&& i<SIG_MAX_LEN){
        شروعات[i]=ذريعو[i];
        i++;
    }

    إذا(شروعات[0] == '\xFE' && شروعات[1] == '\xFF') {
        *signature_ڊيگهائي=2;
        أرجع  "UTF-16BE";
    } وإلا إذا(شروعات[0] == '\xFF' && شروعات[1] == '\xFE') {
        إذا(شروعات[2] == '\x00' && شروعات[3] =='\x00') {
            *signature_ڊيگهائي=4;
            أرجع "UTF-32LE";
        } وإلا {
            *signature_ڊيگهائي=2;
            أرجع  "UTF-16LE";
        }
    } وإلا إذا(شروعات[0] == '\xEF' && شروعات[1] == '\xBB' && شروعات[2] == '\xBF') {
        *signature_ڊيگهائي=3;
        أرجع  "UTF-8";
    } وإلا إذا(شروعات[0] == '\x00' && شروعات[1] == '\x00' && 
              شروعات[2] == '\xFE' && شروعات[3]=='\xFF') {
        *signature_ڊيگهائي=4;
        أرجع  "UTF-32BE";
    } وإلا إذا(شروعات[0] == '\x0E' && شروعات[1] == '\xFE' && شروعات[2] == '\xFF') {
        *signature_ڊيگهائي=3;
        أرجع "SCSU";
    } وإلا إذا(شروعات[0] == '\xFB' && شروعات[1] == '\xEE' && شروعات[2] == '\x28') {
        *signature_ڊيگهائي=3;
        أرجع "BOCU-1";
    } وإلا إذا(شروعات[0] == '\x2B' && شروعات[1] == '\x2F' && شروعات[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        إذا(شروعات[3] == '\x38' && شروعات[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_ڊيگهائي=5;
            أرجع "UTF-7";
        } وإلا إذا(شروعات[3] == '\x38' || شروعات[3] == '\x39' || شروعات[3] == '\x2B' || شروعات[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_ڊيگهائي=4;
            أرجع "UTF-7";
        }
    }وإلا إذا(شروعات[0]=='\xDD' && شروعات[1]== '\x73'&& شروعات[2]=='\x66' && شروعات[3]=='\x73'){
        *signature_ڊيگهائي=4;
        أرجع "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_ڊيگهائي=0;
    أرجع خالي_اشارو;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ثابت UConverter* cnv, UError_ڪوڊ* حالت)
{
    إذا(حالت == خالي_اشارو || U_FAILURE(*حالت)){
        أرجع -1;
    }
    إذا(cnv == خالي_اشارو){
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع -1;
    }

    إذا(cnv->preFromUFirstCP >= 0){
        أرجع U16_ڊيگهائي(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }وإلا إذا(cnv->preFromULength < 0){
        أرجع -cnv->preFromULength ;
    }وإلا إذا(cnv->fromUChar32 > 0){
        أرجع 1;
    }
    أرجع 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_منزل_UCount_Pending(ثابت UConverter* cnv, UError_ڪوڊ* حالت){

    إذا(حالت == خالي_اشارو || U_FAILURE(*حالت)){
        أرجع -1;
    }
    إذا(cnv == خالي_اشارو){
        *حالت = U_ILLEGAL_دليل_ERROR;
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
ucnv_is_Fixed_ويڪر(UConverter *cnv, UError_ڪوڊ *حالت){
    إذا (U_FAILURE(*حالت)) {
        أرجع خطأ;
    }

    إذا (cnv == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
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
#جيڪڏهن_پورو

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
