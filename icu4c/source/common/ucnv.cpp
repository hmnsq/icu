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
#تعریف_کرو CHUNK_سائز 1024

قسمتعریف ساخت UAmbiguousConverter {
    ثابت حرف *name;
    ثابت char16_t variant5c;
} UAmbiguousConverter;

ساکن ثابت UAmbiguousConverter ambiguousConverters[]={
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
                       UError_رمز * err)
{
    UConverter *r;

    اگر (err == خالی_اشاریہ || U_FAILURE (*err)) {
        واپس خالی_اشاریہ;
    }

    r =  ucnv_createConverter(خالی_اشاریہ, name, err);
    واپس r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ثابت حرف *packageName, ثابت حرف *converterName, UError_رمز * err)
{
    واپس ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ثابت char16_t * name,
                         UError_رمز * err)
{
    حرف asciiName[UCNV_MAX_CONVERTER_NAME_لمبائی];

    اگر (err == خالی_اشاریہ || U_FAILURE(*err))
        واپس خالی_اشاریہ;
    اگر (name == خالی_اشاریہ)
        واپس ucnv_open (خالی_اشاریہ, err);
    اگر (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_لمبائی)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    واپس ucnv_open(u_austrcpy(asciiName, name), err);
}

/* Copy the string that is represented by the UConverterPlatform enum
 * @param platformString An output buffer
 * @param platform An enum representing a platform
 * @return the length of the copied string.
 */
ساکن int32_t
ucnv_copyPlatformString(حرف *platformString, UConverterPlatform pltfrm)
{
    انتخاب (pltfrm)
    {
    حالت UCNV_IBM:
        uprv_strcpy(platformString, "ibm-");
        واپس 4;
    حالت UCNV_UNKNOWN:
        توڑو;
    }

    /* default to empty string */
    *platformString = 0;
    واپس 0;
}

/*Assumes a $platform-#codepage.$CONVERTER_FILE_EXTENSION scheme and calls
 *through createConverter*/
U_CAPI UConverter*   U_EXPORT2
ucnv_openCCSID (int32_t codepage,
                UConverterPlatform platform,
                UError_رمز * err)
{
    حرف myName[UCNV_MAX_CONVERTER_NAME_لمبائی];
    int32_t myNameLen;

    اگر (err == خالی_اشاریہ || U_FAILURE (*err))
        واپس خالی_اشاریہ;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_مقصد_String(myName + myNameLen, codepage, 10);

    واپس ucnv_createConverter(خالی_اشاریہ, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_محفوظ_Clone(ثابت UConverter* cnv,
               خالی* ڈھیر_بفر U_LIFETIME_BOUND,
               int32_t* p_بفر_سائز,
               UError_رمز* حالت_2)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t ڈھیر_بفر_سائز;
    int32_t بفر_سائز_Needed;
    UError_رمز cbErr;
    UConverter_مقصد_Unicode_دلائل مقصد_UArgs = {
        حجم(UConverter_مقصد_Unicode_دلائل),
            سچ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ
    };
    UConverter_From_Unicode_دلائل fromUArgs = {
        حجم(UConverter_From_Unicode_دلائل),
            سچ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ
    };

    UTRACE_اندراج_OC(UTRACE_UCNV_CLONE);

    اگر (حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        UTRACE_EXIT_حالت(حالت_2? *حالت_2: U_ILLEGAL_دلیل_ERROR);
        واپس خالی_اشاریہ;
    }

    اگر (cnv == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(*حالت_2);
        واپس خالی_اشاریہ;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, حالت_2), cnv, ڈھیر_بفر);

    اگر (cnv->shared_ڈیٹا->impl->محفوظ_Clone != خالی_اشاریہ) {
        /* call the custom safeClone function for sizing */
        بفر_سائز_Needed = 0;
        cnv->shared_ڈیٹا->impl->محفوظ_Clone(cnv, خالی_اشاریہ, &بفر_سائز_Needed, حالت_2);
        اگر (U_FAILURE(*حالت_2)) {
            UTRACE_EXIT_حالت(*حالت_2);
            واپس خالی_اشاریہ;
        }
    }
    ورنہ
    {
        /* inherent sizing */
        بفر_سائز_Needed = حجم(UConverter);
    }

    اگر (p_بفر_سائز == خالی_اشاریہ) {
        ڈھیر_بفر_سائز = 1;
        p_بفر_سائز = &ڈھیر_بفر_سائز;
    } ورنہ {
        ڈھیر_بفر_سائز = *p_بفر_سائز;
        اگر (ڈھیر_بفر_سائز <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_بفر_سائز = بفر_سائز_Needed;
            UTRACE_EXIT_قدر(بفر_سائز_Needed);
            واپس خالی_اشاریہ;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    اگر (ڈھیر_بفر) {
        uintptr_t p = reinterpret_cast<uintptr_t>(ڈھیر_بفر);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t اشارہ_گر_Adjustment = aligned_p - p;
        اگر (بفر_سائز_Needed + اشارہ_گر_Adjustment <= ڈھیر_بفر_سائز) {
            ڈھیر_بفر = reinterpret_cast<خالی *>(aligned_p);
            ڈھیر_بفر_سائز -= static_cast<int32_t>(اشارہ_گر_Adjustment);
        } ورنہ {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            ڈھیر_بفر_سائز = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    اگر (ڈھیر_بفر_سائز < بفر_سائز_Needed || ڈھیر_بفر == خالی_اشاریہ)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (بفر_سائز_Needed);

        اگر(localConverter == خالی_اشاریہ) {
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            UTRACE_EXIT_حالت(*حالت_2);
            واپس خالی_اشاریہ;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        اگر (p_بفر_سائز != &ڈھیر_بفر_سائز) {
            *حالت_2 = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_بفر_سائز = بفر_سائز_Needed;
    } ورنہ {
        /* just use the stack buffer */
        localConverter = (UConverter*) ڈھیر_بفر;
        allocatedConverter = خالی_اشاریہ;
    }

    uprv_memset(localConverter, 0, بفر_سائز_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, حجم(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = جھوٹ;

    /* copy the substitution string */
    اگر (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } ورنہ {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_بفر_لمبائی * U_SIZEOF_UCHAR);
        اگر (localConverter->subChars == خالی_اشاریہ) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_حالت(*حالت_2);
            واپس خالی_اشاریہ;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_بفر_لمبائی * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    اگر (cnv->shared_ڈیٹا->impl->محفوظ_Clone != خالی_اشاریہ) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_ڈیٹا->impl->محفوظ_Clone(cnv, localConverter, p_بفر_سائز, حالت_2);
    }

    اگر(localConverter==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        اگر (allocatedConverter != خالی_اشاریہ && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_حالت(*حالت_2);
        واپس خالی_اشاریہ;
    }

    /* increment refcount of shared data if needed */
    اگر (cnv->shared_ڈیٹا->isReferenceCounted) {
        ucnv_increment_Ref_گنتی(cnv->shared_ڈیٹا);
    }

    اگر(localConverter == (UConverter*)ڈھیر_بفر) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = سچ;
    }

    /* allow callback functions to handle any memory allocation */
    مقصد_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->مقصد_UContext, &مقصد_UArgs, خالی_اشاریہ, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, خالی_اشاریہ, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_حالت(localConverter, *حالت_2);
    واپس localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ثابت UConverter* cnv, UError_رمز *حالت_2)
{
    واپس ucnv_محفوظ_Clone(cnv, خالی_اشاریہ, خالی_اشاریہ, حالت_2);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI خالی  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_رمز error_رمز = U_ZERO_ERROR;

    UTRACE_اندراج_OC(UTRACE_UCNV_CLOSE);

    اگر (converter == خالی_اشاریہ)
    {
        UTRACE_EXIT();
        واپس;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_رمز), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    اگر (converter->fromCharErrorBehaviour != UCNV_مقصد_U_DEFAULT_CALLBACK) {
        UConverter_مقصد_Unicode_دلائل مقصد_UArgs = {
            حجم(UConverter_مقصد_Unicode_دلائل),
                سچ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ
        };

        مقصد_UArgs.converter = converter;
        error_رمز = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->مقصد_UContext, &مقصد_UArgs, خالی_اشاریہ, 0, UCNV_CLOSE, &error_رمز);
    }
    اگر (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_دلائل fromUArgs = {
            حجم(UConverter_From_Unicode_دلائل),
                سچ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ
        };
        fromUArgs.converter = converter;
        error_رمز = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, خالی_اشاریہ, 0, 0, UCNV_CLOSE, &error_رمز);
    }

    اگر (converter->shared_ڈیٹا->impl->close != خالی_اشاریہ) {
        converter->shared_ڈیٹا->impl->close(converter);
    }

    اگر (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    اگر (converter->shared_ڈیٹا->isReferenceCounted) {
        ucnv_unload_Shared_ڈیٹا_If_Ready(converter->shared_ڈیٹا);
    }

    اگر(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ثابت حرف*   U_EXPORT2
ucnv_get_دستیاب_Name (int32_t n)
{
    اگر (0 <= n && n <= 0xffff) {
        UError_رمز err = U_ZERO_ERROR;
        ثابت حرف *name = ucnv_bld_get_دستیاب_Converter((uint16_t)n, &err);
        اگر (U_SUCCESS(err)) {
            واپس name;
        }
    }
    واپس خالی_اشاریہ;
}

U_CAPI int32_t   U_EXPORT2
ucnv_گنتی_دستیاب ()
{
    UError_رمز err = U_ZERO_ERROR;
    واپس ucnv_bld_گنتی_دستیاب_Converters(&err);
}

U_CAPI خالی    U_EXPORT2
ucnv_getSubstChars (ثابت UConverter * converter,
                    حرف *mySubChar,
                    int8_t * len,
                    UError_رمز * err)
{
    اگر (U_FAILURE (*err))
        واپس;

    اگر (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        واپس;
    }

    اگر (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI خالی    U_EXPORT2
ucnv_سیٹ_Subst_Chars (UConverter * converter,
                    ثابت حرف *mySubChar,
                    int8_t len,
                    UError_رمز * err)
{
    اگر (U_FAILURE (*err))
        واپس;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    اگر ((len > converter->shared_ڈیٹا->static_ڈیٹا->max_بائٹیں_فی_Char)
     || (len < converter->shared_ڈیٹا->static_ڈیٹا->min_بائٹیں_فی_Char))
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس;
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

U_CAPI خالی U_EXPORT2
ucnv_سیٹ_Subst_String(UConverter *cnv,
                    ثابت char16_t *s,
                    int32_t length,
                    UError_رمز *err) {
    alignas(UConverter) حرف clone_بفر[U_CNV_SAFECLONE_BUFFERSIZE];
    حرف chars[UCNV_ERROR_بفر_لمبائی];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_سائز, length8;

    /* Let the following functions check all arguments. */
    clone_سائز = حجم(clone_بفر);
    clone = ucnv_محفوظ_Clone(cnv, clone_بفر, &clone_سائز, err);
    ucnv_سیٹ_From_UCall_عقب(clone, UCNV_FROM_U_CALLBACK_STOP, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)حجم(chars), s, length, err);
    ucnv_close(clone);
    اگر (U_FAILURE(*err)) {
        واپس;
    }

    اگر (cnv->shared_ڈیٹا->impl->writeSub == خالی_اشاریہ
#اگر !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_ڈیٹا->static_ڈیٹا->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#اگر_ختم
    ) {
        /* The converter is not stateful. Store the charset bytes as a fixed string. */
        subChars = (uint8_t *)chars;
    } ورنہ {
        /*
         * The converter has a non-default writeSub() function, indicating
         * that it is stateful.
         * Store the Unicode string for on-the-fly conversion for correct
         * state handling.
         */
        اگر (length > UCNV_ERROR_بفر_لمبائی) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_بفر_حد_سے_تجاوز_ERROR;
            واپس;
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
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_بفر_لمبائی * U_SIZEOF_UCHAR);
            اگر (cnv->subChars == خالی_اشاریہ) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_بفر_لمبائی * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    اگر (length8 == 0) {
        cnv->subCharLen = 0;
    } ورنہ {
        uprv_memcpy(cnv->subChars, subChars, length8);
        اگر (subChars == (uint8_t *)chars) {
            cnv->subCharLen = (int8_t)length8;
        } ورنہ /* subChars == s */ {
            cnv->subCharLen = (int8_t)-length;
        }
    }

    /* See comment in ucnv_setSubstChars(). */
    cnv->subChar1 = 0;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
ساکن خالی _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    اگر(converter == خالی_اشاریہ) {
        واپس;
    }

    اگر(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_رمز error_رمز;

        اگر(choice<=UCNV_RESET_مقصد_UNICODE && converter->fromCharErrorBehaviour != UCNV_مقصد_U_DEFAULT_CALLBACK) {
            UConverter_مقصد_Unicode_دلائل مقصد_UArgs = {
                حجم(UConverter_مقصد_Unicode_دلائل),
                سچ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ
            };
            مقصد_UArgs.converter = converter;
            error_رمز = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->مقصد_UContext, &مقصد_UArgs, خالی_اشاریہ, 0, UCNV_RESET, &error_رمز);
        }
        اگر(choice!=UCNV_RESET_مقصد_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_دلائل fromUArgs = {
                حجم(UConverter_From_Unicode_دلائل),
                سچ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ,
                خالی_اشاریہ
            };
            fromUArgs.converter = converter;
            error_رمز = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, خالی_اشاریہ, 0, 0, UCNV_RESET, &error_رمز);
        }
    }

    /* now reset the converter itself */
    اگر(choice<=UCNV_RESET_مقصد_UNICODE) {
        converter->مقصد_Unicode_حالت = converter->shared_ڈیٹا->مقصد_Unicode_حالت;
        converter->منوال = 0;
        converter->مقصد_ULength = 0;
        converter->invalid_Char_لمبائی = converter->UChar_Error_بفر_لمبائی = 0;
        converter->pre_مقصد_ULength = 0;
    }
    اگر(choice!=UCNV_RESET_مقصد_UNICODE) {
        converter->from_Unicode_حالت = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_لمبائی = converter->char_Error_بفر_لمبائی = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    اگر (converter->shared_ڈیٹا->impl->reset != خالی_اشاریہ) {
        /* call the custom reset function */
        converter->shared_ڈیٹا->impl->reset(converter, choice);
    }
}

U_CAPI خالی  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, سچ);
}

U_CAPI خالی  U_EXPORT2
ucnv_reset_مقصد_Unicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_مقصد_UNICODE, سچ);
}

U_CAPI خالی  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, سچ);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_سائز (ثابت UConverter * converter)
{
    واپس converter->max_بائٹیں_فی_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_سائز (ثابت UConverter * converter)
{
    واپس converter->shared_ڈیٹا->static_ڈیٹا->min_بائٹیں_فی_Char;
}

U_CAPI ثابت حرف* U_EXPORT2
ucnv_getName(ثابت UConverter* converter U_LIFETIME_BOUND, UError_رمز* err)
{
    اگر (U_FAILURE (*err))
        واپس خالی_اشاریہ;
    اگر(converter->shared_ڈیٹا->impl->getName){
        ثابت حرف* عارضی= converter->shared_ڈیٹا->impl->getName(converter);
        اگر(عارضی)
            واپس عارضی;
    }
    واپس converter->shared_ڈیٹا->static_ڈیٹا->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ثابت UConverter * converter,
              UError_رمز * err)
{
    int32_t ccsid;
    اگر (U_FAILURE (*err))
        واپس -1;

    ccsid = converter->shared_ڈیٹا->static_ڈیٹا->codepage;
    اگر (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        ثابت حرف *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        اگر (U_SUCCESS(*err) && standardName) {
            ثابت حرف *ccsidStr = uprv_strchr(standardName, '-');
            اگر (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    واپس ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (ثابت UConverter * converter,
                                      UError_رمز * err)
{
    اگر (U_FAILURE (*err))
        واپس UCNV_UNKNOWN;

    واپس (UConverterPlatform)converter->shared_ڈیٹا->static_ڈیٹا->platform;
}

U_CAPI خالی U_EXPORT2
    ucnv_get_مقصد_UCall_عقب (ثابت UConverter * converter,
                         UConverter_مقصد_UCallback *action,
                         ثابت خالی **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->مقصد_UContext;
}

U_CAPI خالی U_EXPORT2
    ucnv_get_From_UCall_عقب (ثابت UConverter * converter,
                           UConverterFromUCallback *action,
                           ثابت خالی **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI خالی    U_EXPORT2
ucnv_سیٹ_مقصد_UCall_عقب (UConverter * converter,
                            UConverter_مقصد_UCallback newAction,
                            ثابت خالی* newContext,
                            UConverter_مقصد_UCallback *oldAction,
                            ثابت خالی** oldContext,
                            UError_رمز * err)
{
    اگر (U_FAILURE (*err))
        واپس;
    اگر (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    اگر (oldContext) *oldContext = converter->مقصد_UContext;
    converter->مقصد_UContext = newContext;
}

U_CAPI خالی  U_EXPORT2
ucnv_سیٹ_From_UCall_عقب (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ثابت خالی* newContext,
                            UConverterFromUCallback *oldAction,
                            ثابت خالی** oldContext,
                            UError_رمز * err)
{
    اگر (U_FAILURE (*err))
        واپس;
    اگر (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    اگر (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

ساکن خالی
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t ماخذ_اشاریہ, int32_t error_داخلہ_لمبائی) {
    int32_t *حد_بندی;
    int32_t delta, انحراف;

    اگر(ماخذ_اشاریہ>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=ماخذ_اشاریہ-error_داخلہ_لمبائی;
    } ورنہ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    حد_بندی=offsets+length;
    اگر(delta==0) {
        /* most common case, nothing to do */
    } ورنہ اگر(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        جب(offsets<حد_بندی) {
            انحراف=*offsets;
            اگر(انحراف>=0) {
                *offsets=انحراف+delta;
            }
            ++offsets;
        }
    } ورنہ /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        جب(offsets<حد_بندی) {
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

ساکن خالی
_fromUnicodeWithCallback(UConverter_From_Unicode_دلائل *p_دلائل, UError_رمز *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ثابت char16_t *s;
    حرف *t;
    int32_t *offsets;
    int32_t ماخذ_اشاریہ;
    int32_t error_داخلہ_لمبائی;
    UBool converter_Saw_انتہا_کا_داخلہ, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ثابت char16_t *real_ماخذ, *real_ماخذ_حد_بندی;
    int32_t real_ماخذ_اشاریہ;
    UBool realFlush;

    cnv=p_دلائل->converter;
    s=p_دلائل->ماخذ;
    t=p_دلائل->ہدف;
    offsets=p_دلائل->offsets;

    /* get the converter implementation function */
    ماخذ_اشاریہ=0;
    اگر(offsets==خالی_اشاریہ) {
        fromUnicode=cnv->shared_ڈیٹا->impl->fromUnicode;
    } ورنہ {
        fromUnicode=cnv->shared_ڈیٹا->impl->fromUnicodeWithOffsets;
        اگر(fromUnicode==خالی_اشاریہ) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_ڈیٹا->impl->fromUnicode;
            /* we will write -1 for each offset */
            ماخذ_اشاریہ=-1;
        }
    }

    اگر(cnv->preFromULength>=0) {
        /* normal mode */
        real_ماخذ=خالی_اشاریہ;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ماخذ_حد_بندی=خالی_اشاریہ;
        realFlush=جھوٹ;
        real_ماخذ_اشاریہ=0;
    } ورنہ {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        اگر (!p_دلائل->ماخذ) {
            *err=U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
        real_ماخذ=p_دلائل->ماخذ;
        real_ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
        realFlush=p_دلائل->flush;
        real_ماخذ_اشاریہ=ماخذ_اشاریہ;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_دلائل->ماخذ=replay;
        p_دلائل->ماخذ_حد_بندی=replay-cnv->preFromULength;
        p_دلائل->flush=جھوٹ;
        ماخذ_اشاریہ=-1;

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
    برائے(;;) {
        اگر(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(p_دلائل, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_انتہا_کا_داخلہ=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_دلائل->flush && p_دلائل->ماخذ==p_دلائل->ماخذ_حد_بندی &&
                        cnv->fromUChar32==0);
        } ورنہ {
            /* handle error from ucnv_convertEx() */
            converter_Saw_انتہا_کا_داخلہ=جھوٹ;
        }

        /* no callback called yet for this iteration */
        calledCallback=جھوٹ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_داخلہ_لمبائی=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        برائے(;;) {
            /* update offsets if we write any */
            اگر(offsets!=خالی_اشاریہ) {
                int32_t length = static_cast<int32_t>(p_دلائل->ہدف - t);
                اگر(length>0) {
                    _updateOffsets(offsets, length, ماخذ_اشاریہ, error_داخلہ_لمبائی);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_دلائل->offsets=offsets+=length;
                }

                اگر(ماخذ_اشاریہ>=0) {
                    ماخذ_اشاریہ += static_cast<int32_t>(p_دلائل->ماخذ - s);
                }
            }

            اگر(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                اگر(real_ماخذ==خالی_اشاریہ) {
                    real_ماخذ=p_دلائل->ماخذ;
                    real_ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
                    realFlush=p_دلائل->flush;
                    real_ماخذ_اشاریہ=ماخذ_اشاریہ;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_دلائل->ماخذ=replay;
                    p_دلائل->ماخذ_حد_بندی=replay-cnv->preFromULength;
                    p_دلائل->flush=جھوٹ;
                    اگر((ماخذ_اشاریہ+=cnv->preFromULength)<0) {
                        ماخذ_اشاریہ=-1;
                    }

                    cnv->preFromULength=0;
                } ورنہ {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ماخذ==خالی_اشاریہ);
                    *err=U_اندرونی_پروگرام_ERROR;
                }
            }

            /* update pointers */
            s=p_دلائل->ماخذ;
            t=p_دلائل->ہدف;

            اگر(U_SUCCESS(*err)) {
                اگر(s<p_دلائل->ماخذ_حد_بندی) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    توڑو;
                } ورنہ اگر(real_ماخذ!=خالی_اشاریہ) {
                    /* switch back from replaying to the real source and continue */
                    p_دلائل->ماخذ=real_ماخذ;
                    p_دلائل->ماخذ_حد_بندی=real_ماخذ_حد_بندی;
                    p_دلائل->flush=realFlush;
                    ماخذ_اشاریہ=real_ماخذ_اشاریہ;

                    real_ماخذ=خالی_اشاریہ;
                    توڑو;
                } ورنہ اگر(p_دلائل->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_ملا;
                    calledCallback=جھوٹ; /* new error condition */
                } ورنہ {
                    /* input consumed */
                    اگر(p_دلائل->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        اگر(!converter_Saw_انتہا_کا_داخلہ) {
                            توڑو;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, جھوٹ);
                    }

                    /* done successfully */
                    واپس;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_رمز e;

                اگر( calledCallback ||
                    (e=*err)==U_بفر_حد_سے_تجاوز_ERROR ||
                    (e!=U_INVALID_CHAR_ملا &&
                     e!=U_ILLEGAL_CHAR_ملا &&
                     e!=U_TRUNCATED_CHAR_ملا)
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
                    اگر(real_ماخذ!=خالی_اشاریہ) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_دلائل->ماخذ_حد_بندی - p_دلائل->ماخذ);
                        اگر(length>0) {
                            u_memcpy(cnv->preFromU, p_دلائل->ماخذ, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_دلائل->ماخذ=real_ماخذ;
                        p_دلائل->ماخذ_حد_بندی=real_ماخذ_حد_بندی;
                        p_دلائل->flush=realFlush;
                    }

                    واپس;
                }
            }

            /* callback handling */
            {
                UChar32 رمز_نقطہ;

                /* get and write the code point */
                رمز_نقطہ=cnv->fromUChar32;
                error_داخلہ_لمبائی=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_بفر, error_داخلہ_لمبائی, رمز_نقطہ);
                cnv->invalid_UChar_لمبائی = static_cast<int8_t>(error_داخلہ_لمبائی);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_دلائل,
                    cnv->invalid_UChar_بفر, error_داخلہ_لمبائی, رمز_نقطہ,
                    *err==U_INVALID_CHAR_ملا ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
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
ساکن UBool
ucnv_output_حد_سے_تجاوز_From_Unicode(UConverter *cnv,
                               حرف **ہدف, ثابت حرف *ہدف_حد_بندی,
                               int32_t **pOffsets,
                               UError_رمز *err) {
    int32_t *offsets;
    حرف *حد_سے_تجاوز, *t;
    int32_t i, length;

    t=*ہدف;
    اگر(pOffsets!=خالی_اشاریہ) {
        offsets=*pOffsets;
    } ورنہ {
        offsets=خالی_اشاریہ;
    }

    حد_سے_تجاوز = reinterpret_cast<حرف*>(cnv->char_Error_بفر);
    length=cnv->char_Error_بفر_لمبائی;
    i=0;
    جب(i<length) {
        اگر(t==ہدف_حد_بندی) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            کرو {
                حد_سے_تجاوز[j++]=حد_سے_تجاوز[i++];
            } جب(i<length);

            cnv->char_Error_بفر_لمبائی = static_cast<int8_t>(j);
            *ہدف=t;
            اگر(offsets!=خالی_اشاریہ) {
                *pOffsets=offsets;
            }
            *err=U_بفر_حد_سے_تجاوز_ERROR;
            واپس سچ;
        }

        /* copy the overflow contents to the target */
        *t++=حد_سے_تجاوز[i++];
        اگر(offsets!=خالی_اشاریہ) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_بفر_لمبائی=0;
    *ہدف=t;
    اگر(offsets!=خالی_اشاریہ) {
        *pOffsets=offsets;
    }
    واپس جھوٹ;
}

U_CAPI خالی U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 حرف **ہدف, ثابت حرف *ہدف_حد_بندی,
                 ثابت char16_t **ماخذ, ثابت char16_t *ماخذ_حد_بندی,
                 int32_t *offsets,
                 UBool flush,
                 UError_رمز *err) {
    UConverter_From_Unicode_دلائل دلائل_2;
    ثابت char16_t *s;
    حرف *t;

    /* check parameters */
    اگر(err==خالی_اشاریہ || U_FAILURE(*err)) {
        واپس;
    }

    اگر(cnv==خالی_اشاریہ || ہدف==خالی_اشاریہ || ماخذ==خالی_اشاریہ) {
        *err=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    s=*ماخذ;
    t=*ہدف;

    اگر ((ثابت خالی *)U_MAX_PTR(ماخذ_حد_بندی) == (ثابت خالی *)ماخذ_حد_بندی) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        ماخذ_حد_بندی = (ثابت char16_t *)(((ثابت حرف *)ماخذ_حد_بندی) - 1);
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
    اگر (ماخذ_حد_بندی<s || ہدف_حد_بندی<t ||
        ((size_t)(ماخذ_حد_بندی-s)>(size_t)0x3fffffff && ماخذ_حد_بندی>s) ||
        ((size_t)(ہدف_حد_بندی-t)>(size_t)0x7fffffff && ہدف_حد_بندی>t) ||
        (((ثابت حرف *)ماخذ_حد_بندی-(ثابت حرف *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    
    /* output the target overflow buffer */
    اگر( cnv->char_Error_بفر_لمبائی>0 &&
        ucnv_output_حد_سے_تجاوز_From_Unicode(cnv, ہدف, ہدف_حد_بندی, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        واپس;
    }
    /* *target may have moved, therefore stop using t */

    اگر(!flush && s==ماخذ_حد_بندی && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        واپس;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    دلائل_2.converter=cnv;
    دلائل_2.flush=flush;
    دلائل_2.offsets=offsets;
    دلائل_2.ماخذ=s;
    دلائل_2.ماخذ_حد_بندی=ماخذ_حد_بندی;
    دلائل_2.ہدف=*ہدف;
    دلائل_2.ہدف_حد_بندی=ہدف_حد_بندی;
    دلائل_2.size=حجم(دلائل_2);

    _fromUnicodeWithCallback(&دلائل_2, err);

    *ماخذ=دلائل_2.ماخذ;
    *ہدف=دلائل_2.ہدف;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

ساکن خالی
مقصد_Unicode_With_Callback(UConverter_مقصد_Unicode_دلائل *p_دلائل, UError_رمز *err) {
    UConverter_مقصد_Unicode مقصد_Unicode;
    UConverter *cnv;
    ثابت حرف *s;
    char16_t *t;
    int32_t *offsets;
    int32_t ماخذ_اشاریہ;
    int32_t error_داخلہ_لمبائی;
    UBool converter_Saw_انتہا_کا_داخلہ, calledCallback;

    /* variables for m:n conversion */
    حرف replay[UCNV_EXT_MAX_بائٹیں];
    ثابت حرف *real_ماخذ, *real_ماخذ_حد_بندی;
    int32_t real_ماخذ_اشاریہ;
    UBool realFlush;

    cnv=p_دلائل->converter;
    s=p_دلائل->ماخذ;
    t=p_دلائل->ہدف;
    offsets=p_دلائل->offsets;

    /* get the converter implementation function */
    ماخذ_اشاریہ=0;
    اگر(offsets==خالی_اشاریہ) {
        مقصد_Unicode=cnv->shared_ڈیٹا->impl->مقصد_Unicode;
    } ورنہ {
        مقصد_Unicode=cnv->shared_ڈیٹا->impl->مقصد_Unicode_With_Offsets;
        اگر(مقصد_Unicode==خالی_اشاریہ) {
            /* there is no WithOffsets implementation */
            مقصد_Unicode=cnv->shared_ڈیٹا->impl->مقصد_Unicode;
            /* we will write -1 for each offset */
            ماخذ_اشاریہ=-1;
        }
    }

    اگر(cnv->pre_مقصد_ULength>=0) {
        /* normal mode */
        real_ماخذ=خالی_اشاریہ;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_ماخذ_حد_بندی=خالی_اشاریہ;
        realFlush=جھوٹ;
        real_ماخذ_اشاریہ=0;
    } ورنہ {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        اگر (!p_دلائل->ماخذ) {
            *err=U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
        real_ماخذ=p_دلائل->ماخذ;
        real_ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
        realFlush=p_دلائل->flush;
        real_ماخذ_اشاریہ=ماخذ_اشاریہ;

        uprv_memcpy(replay, cnv->pre_مقصد_U, -cnv->pre_مقصد_ULength);
        p_دلائل->ماخذ=replay;
        p_دلائل->ماخذ_حد_بندی=replay-cnv->pre_مقصد_ULength;
        p_دلائل->flush=جھوٹ;
        ماخذ_اشاریہ=-1;

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
    برائے(;;) {
        اگر(U_SUCCESS(*err)) {
            /* convert */
            مقصد_Unicode(p_دلائل, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_انتہا_کا_داخلہ=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_دلائل->flush && p_دلائل->ماخذ==p_دلائل->ماخذ_حد_بندی &&
                        cnv->مقصد_ULength==0);
        } ورنہ {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_انتہا_کا_داخلہ=جھوٹ;
        }

        /* no callback called yet for this iteration */
        calledCallback=جھوٹ;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_داخلہ_لمبائی=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        برائے(;;) {
            /* update offsets if we write any */
            اگر(offsets!=خالی_اشاریہ) {
                int32_t length = static_cast<int32_t>(p_دلائل->ہدف - t);
                اگر(length>0) {
                    _updateOffsets(offsets, length, ماخذ_اشاریہ, error_داخلہ_لمبائی);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_دلائل->offsets=offsets+=length;
                }

                اگر(ماخذ_اشاریہ>=0) {
                    ماخذ_اشاریہ += static_cast<int32_t>(p_دلائل->ماخذ - s);
                }
            }

            اگر(cnv->pre_مقصد_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                اگر(real_ماخذ==خالی_اشاریہ) {
                    real_ماخذ=p_دلائل->ماخذ;
                    real_ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
                    realFlush=p_دلائل->flush;
                    real_ماخذ_اشاریہ=ماخذ_اشاریہ;

                    uprv_memcpy(replay, cnv->pre_مقصد_U, -cnv->pre_مقصد_ULength);
                    p_دلائل->ماخذ=replay;
                    p_دلائل->ماخذ_حد_بندی=replay-cnv->pre_مقصد_ULength;
                    p_دلائل->flush=جھوٹ;
                    اگر((ماخذ_اشاریہ+=cnv->pre_مقصد_ULength)<0) {
                        ماخذ_اشاریہ=-1;
                    }

                    cnv->pre_مقصد_ULength=0;
                } ورنہ {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(real_ماخذ==خالی_اشاریہ);
                    *err=U_اندرونی_پروگرام_ERROR;
                }
            }

            /* update pointers */
            s=p_دلائل->ماخذ;
            t=p_دلائل->ہدف;

            اگر(U_SUCCESS(*err)) {
                اگر(s<p_دلائل->ماخذ_حد_بندی) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    توڑو;
                } ورنہ اگر(real_ماخذ!=خالی_اشاریہ) {
                    /* switch back from replaying to the real source and continue */
                    p_دلائل->ماخذ=real_ماخذ;
                    p_دلائل->ماخذ_حد_بندی=real_ماخذ_حد_بندی;
                    p_دلائل->flush=realFlush;
                    ماخذ_اشاریہ=real_ماخذ_اشاریہ;

                    real_ماخذ=خالی_اشاریہ;
                    توڑو;
                } ورنہ اگر(p_دلائل->flush && cnv->مقصد_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_ملا;
                    calledCallback=جھوٹ; /* new error condition */
                } ورنہ {
                    /* input consumed */
                    اگر(p_دلائل->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        اگر(!converter_Saw_انتہا_کا_داخلہ) {
                            توڑو;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_مقصد_UNICODE, جھوٹ);
                    }

                    /* done successfully */
                    واپس;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_رمز e;

                اگر( calledCallback ||
                    (e=*err)==U_بفر_حد_سے_تجاوز_ERROR ||
                    (e!=U_INVALID_CHAR_ملا &&
                     e!=U_ILLEGAL_CHAR_ملا &&
                     e!=U_TRUNCATED_CHAR_ملا &&
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
                    اگر(real_ماخذ!=خالی_اشاریہ) {
                        int32_t length;

                        U_ASSERT(cnv->pre_مقصد_ULength==0);

                        length = static_cast<int32_t>(p_دلائل->ماخذ_حد_بندی - p_دلائل->ماخذ);
                        اگر(length>0) {
                            uprv_memcpy(cnv->pre_مقصد_U, p_دلائل->ماخذ, length);
                            cnv->pre_مقصد_ULength = static_cast<int8_t>(-length);
                        }

                        p_دلائل->ماخذ=real_ماخذ;
                        p_دلائل->ماخذ_حد_بندی=real_ماخذ_حد_بندی;
                        p_دلائل->flush=realFlush;
                    }

                    واپس;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_داخلہ_لمبائی=cnv->invalid_Char_لمبائی=cnv->مقصد_ULength;
            اگر(error_داخلہ_لمبائی>0) {
                uprv_memcpy(cnv->invalid_Char_بفر, cnv->مقصد_UBytes, error_داخلہ_لمبائی);
            }

            /* set the converter state to deal with the next character */
            cnv->مقصد_ULength=0;

            /* call the callback function */
            اگر(cnv->مقصد_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_ملا) {
                cnv->مقصد_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->مقصد_UContext, p_دلائل,
                cnv->invalid_Char_بفر, error_داخلہ_لمبائی,
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
            calledCallback=سچ;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
ساکن UBool
ucnv_output_حد_سے_تجاوز_مقصد_Unicode(UConverter *cnv,
                             char16_t **ہدف, ثابت char16_t *ہدف_حد_بندی,
                             int32_t **pOffsets,
                             UError_رمز *err) {
    int32_t *offsets;
    char16_t *حد_سے_تجاوز, *t;
    int32_t i, length;

    t=*ہدف;
    اگر(pOffsets!=خالی_اشاریہ) {
        offsets=*pOffsets;
    } ورنہ {
        offsets=خالی_اشاریہ;
    }

    حد_سے_تجاوز=cnv->UChar_Error_بفر;
    length=cnv->UChar_Error_بفر_لمبائی;
    i=0;
    جب(i<length) {
        اگر(t==ہدف_حد_بندی) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            کرو {
                حد_سے_تجاوز[j++]=حد_سے_تجاوز[i++];
            } جب(i<length);

            cnv->UChar_Error_بفر_لمبائی = static_cast<int8_t>(j);
            *ہدف=t;
            اگر(offsets!=خالی_اشاریہ) {
                *pOffsets=offsets;
            }
            *err=U_بفر_حد_سے_تجاوز_ERROR;
            واپس سچ;
        }

        /* copy the overflow contents to the target */
        *t++=حد_سے_تجاوز[i++];
        اگر(offsets!=خالی_اشاریہ) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_بفر_لمبائی=0;
    *ہدف=t;
    اگر(offsets!=خالی_اشاریہ) {
        *pOffsets=offsets;
    }
    واپس جھوٹ;
}

U_CAPI خالی U_EXPORT2
ucnv_مقصد_Unicode(UConverter *cnv,
               char16_t **ہدف, ثابت char16_t *ہدف_حد_بندی,
               ثابت حرف **ماخذ, ثابت حرف *ماخذ_حد_بندی,
               int32_t *offsets,
               UBool flush,
               UError_رمز *err) {
    UConverter_مقصد_Unicode_دلائل دلائل_2;
    ثابت حرف *s;
    char16_t *t;

    /* check parameters */
    اگر(err==خالی_اشاریہ || U_FAILURE(*err)) {
        واپس;
    }

    اگر(cnv==خالی_اشاریہ || ہدف==خالی_اشاریہ || ماخذ==خالی_اشاریہ) {
        *err=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    s=*ماخذ;
    t=*ہدف;

    اگر ((ثابت خالی *)U_MAX_PTR(ہدف_حد_بندی) == (ثابت خالی *)ہدف_حد_بندی) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        ہدف_حد_بندی = (ثابت char16_t *)(((ثابت حرف *)ہدف_حد_بندی) - 1);
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
    اگر (ماخذ_حد_بندی<s || ہدف_حد_بندی<t ||
        ((size_t)(ماخذ_حد_بندی-s)>(size_t)0x7fffffff && ماخذ_حد_بندی>s) ||
        ((size_t)(ہدف_حد_بندی-t)>(size_t)0x3fffffff && ہدف_حد_بندی>t) ||
        (((ثابت حرف *)ہدف_حد_بندی-(ثابت حرف *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    
    /* output the target overflow buffer */
    اگر( cnv->UChar_Error_بفر_لمبائی>0 &&
        ucnv_output_حد_سے_تجاوز_مقصد_Unicode(cnv, ہدف, ہدف_حد_بندی, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        واپس;
    }
    /* *target may have moved, therefore stop using t */

    اگر(!flush && s==ماخذ_حد_بندی && cnv->pre_مقصد_ULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        واپس;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    دلائل_2.converter=cnv;
    دلائل_2.flush=flush;
    دلائل_2.offsets=offsets;
    دلائل_2.ماخذ=s;
    دلائل_2.ماخذ_حد_بندی=ماخذ_حد_بندی;
    دلائل_2.ہدف=*ہدف;
    دلائل_2.ہدف_حد_بندی=ہدف_حد_بندی;
    دلائل_2.size=حجم(دلائل_2);

    مقصد_Unicode_With_Callback(&دلائل_2, err);

    *ماخذ=دلائل_2.ماخذ;
    *ہدف=دلائل_2.ہدف;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                حرف *dest, int32_t dest_استعداد,
                ثابت char16_t *src, int32_t src_لمبائی,
                UError_رمز *p_Error_رمز) {
    ثابت char16_t *src_حد_بندی;
    حرف *originalDest, *dest_حد_بندی;
    int32_t dest_لمبائی;

    /* check arguments */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر( cnv==خالی_اشاریہ ||
        dest_استعداد<0 || (dest_استعداد>0 && dest==خالی_اشاریہ) ||
        src_لمبائی<-1 || (src_لمبائی!=0 && src==خالی_اشاریہ)
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    اگر(src_لمبائی==-1) {
        src_لمبائی=u_strlen(src);
    }
    اگر(src_لمبائی>0) {
        src_حد_بندی=src+src_لمبائی;
        dest_استعداد=pin_استعداد(dest, dest_استعداد);
        dest_حد_بندی=dest+dest_استعداد;

        /* perform the conversion */
        UError_رمز بفر_حالت = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_اشاریہ, سچ, &بفر_حالت);
        dest_لمبائی=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        اگر(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR) {
            حرف بفر[1024];

            dest_حد_بندی=بفر+حجم(بفر);
            کرو {
                dest=بفر;
                بفر_حالت=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_اشاریہ, سچ, &بفر_حالت);
                dest_لمبائی+=(int32_t)(dest-بفر);
            } جب(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR);
        }
        اگر (U_FAILURE(بفر_حالت)) {
            *p_Error_رمز = بفر_حالت;
        }
    } ورنہ {
        dest_لمبائی=0;
    }

    واپس u_terminateChars(originalDest, dest_استعداد, dest_لمبائی, p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
ucnv_مقصد_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_استعداد,
              ثابت حرف *src, int32_t src_لمبائی,
              UError_رمز *p_Error_رمز) {
    ثابت حرف *src_حد_بندی;
    char16_t *originalDest, *dest_حد_بندی;
    int32_t dest_لمبائی;

    /* check arguments */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر( cnv==خالی_اشاریہ ||
        dest_استعداد<0 || (dest_استعداد>0 && dest==خالی_اشاریہ) ||
        src_لمبائی<-1 || (src_لمبائی!=0 && src==خالی_اشاریہ))
    {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* initialize */
    ucnv_reset_مقصد_Unicode(cnv);
    originalDest=dest;
    اگر(src_لمبائی==-1) {
        src_لمبائی=(int32_t)uprv_strlen(src);
    }
    اگر(src_لمبائی>0) {
        src_حد_بندی=src+src_لمبائی;
        dest_استعداد=pin_استعداد(dest, dest_استعداد);
        dest_حد_بندی=dest+dest_استعداد;

        /* perform the conversion */
        UError_رمز بفر_حالت = U_ZERO_ERROR;
        ucnv_مقصد_Unicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_اشاریہ, سچ, &بفر_حالت);
        dest_لمبائی=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        اگر(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR)
        {
            char16_t بفر[1024];

            dest_حد_بندی=بفر+UPRV_LENGTHOF(بفر);
            کرو {
                dest=بفر;
                بفر_حالت=U_ZERO_ERROR;
                ucnv_مقصد_Unicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_اشاریہ, سچ, &بفر_حالت);
                dest_لمبائی+=(int32_t)(dest-بفر);
            }
            جب(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR);
        }
        اگر (U_FAILURE(بفر_حالت)) {
            *p_Error_رمز = بفر_حالت;
        }
    } ورنہ {
        dest_لمبائی=0;
    }

    واپس u_terminateUChars(originalDest, dest_استعداد, dest_لمبائی, p_Error_رمز);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_اگلا_UChar(UConverter *cnv,
                  ثابت حرف **ماخذ, ثابت حرف *ماخذ_حد_بندی,
                  UError_رمز *err) {
    UConverter_مقصد_Unicode_دلائل دلائل_2;
    char16_t بفر[U16_MAX_لمبائی];
    ثابت حرف *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    اگر(err==خالی_اشاریہ || U_FAILURE(*err)) {
        واپس 0xffff;
    }

    اگر(cnv==خالی_اشاریہ || ماخذ==خالی_اشاریہ) {
        *err=U_ILLEGAL_دلیل_ERROR;
        واپس 0xffff;
    }

    s=*ماخذ;
    اگر(ماخذ_حد_بندی<s) {
        *err=U_ILLEGAL_دلیل_ERROR;
        واپس 0xffff;
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
    اگر(((size_t)(ماخذ_حد_بندی-s)>(size_t)0x7fffffff && ماخذ_حد_بندی>s)) {
        *err=U_ILLEGAL_دلیل_ERROR;
        واپس 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    اگر(cnv->UChar_Error_بفر_لمبائی>0) {
        char16_t *حد_سے_تجاوز;

        حد_سے_تجاوز=cnv->UChar_Error_بفر;
        i=0;
        length=cnv->UChar_Error_بفر_لمبائی;
        U16_اگلا(حد_سے_تجاوز, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        اگر((cnv->UChar_Error_بفر_لمبائی=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_بفر, cnv->UChar_Error_بفر+i,
                         cnv->UChar_Error_بفر_لمبائی*U_SIZEOF_UCHAR);
        }

        اگر(!U16_IS_LEAD(c) || i<length) {
            واپس c;
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
    دلائل_2.converter=cnv;
    دلائل_2.flush=سچ;
    دلائل_2.offsets=خالی_اشاریہ;
    دلائل_2.ماخذ=s;
    دلائل_2.ماخذ_حد_بندی=ماخذ_حد_بندی;
    دلائل_2.ہدف=بفر;
    دلائل_2.ہدف_حد_بندی=بفر+1;
    دلائل_2.size=حجم(دلائل_2);

    اگر(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        اگر(cnv->مقصد_ULength==0 && cnv->shared_ڈیٹا->impl->get_اگلا_UChar!=خالی_اشاریہ) {
            c=cnv->shared_ڈیٹا->impl->get_اگلا_UChar(&دلائل_2, err);
            *ماخذ=s=دلائل_2.ماخذ;
            اگر(*err==U_اشاریہ_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_مقصد_UNICODE, جھوٹ);
                واپس 0xffff; /* no output */
            } ورنہ اگر(U_SUCCESS(*err) && c>=0) {
                واپس c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        مقصد_Unicode_With_Callback(&دلائل_2, err);

        اگر(*err==U_بفر_حد_سے_تجاوز_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(دلائل_2.ہدف-بفر);
    } ورنہ {
        /* write the lead surrogate from the overflow buffer */
        بفر[0]=(char16_t)c;
        دلائل_2.ہدف=بفر+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    اگر(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } ورنہ اگر(length==0) {
        /* no input or only state changes */
        *err=U_اشاریہ_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } ورنہ {
        c=بفر[0];
        i=1;
        اگر(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } ورنہ {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            اگر(cnv->UChar_Error_بفر_لمبائی>0) {
                /* got overflow output from the conversion */
                اگر(U16_IS_TRAIL(c2=cnv->UChar_Error_بفر[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    اگر((--cnv->UChar_Error_بفر_لمبائی)>0) {
                        uprv_memmove(cnv->UChar_Error_بفر, cnv->UChar_Error_بفر+1,
                                     cnv->UChar_Error_بفر_لمبائی*U_SIZEOF_UCHAR);
                    }
                } ورنہ {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } ورنہ اگر(دلائل_2.ماخذ<ماخذ_حد_بندی) {
                /* convert once more, to buffer[1] */
                دلائل_2.ہدف_حد_بندی=بفر+2;
                مقصد_Unicode_With_Callback(&دلائل_2, err);
                اگر(*err==U_بفر_حد_سے_تجاوز_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(دلائل_2.ہدف-بفر);
                اگر(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=بفر[1])) {
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
        اگر((length=cnv->UChar_Error_بفر_لمبائی)>0) {
            uprv_memmove(cnv->UChar_Error_بفر+delta, cnv->UChar_Error_بفر,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_بفر_لمبائی=(int8_t)(length+delta);

        cnv->UChar_Error_بفر[0]=بفر[i++];
        اگر(delta>1) {
            cnv->UChar_Error_بفر[1]=بفر[i];
        }
    }

    *ماخذ=دلائل_2.ماخذ;
    واپس c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI خالی U_EXPORT2
ucnv_convertEx(UConverter *ہدف_Cnv, UConverter *ماخذ_Cnv,
               حرف **ہدف, ثابت حرف *ہدف_حد_بندی,
               ثابت حرف **ماخذ, ثابت حرف *ماخذ_حد_بندی,
               char16_t *محور_آغاز, char16_t **محور_ماخذ,
               char16_t **محور_ہدف, ثابت char16_t *محور_حد_بندی,
               UBool reset, UBool flush,
               UError_رمز *p_Error_رمز) {
    char16_t محور_بفر[CHUNK_سائز];
    ثابت char16_t *my_محور_ماخذ;
    char16_t *my_محور_ہدف;
    ثابت حرف *s;
    حرف *t;

    UConverter_مقصد_Unicode_دلائل مقصد_UArgs;
    UConverter_From_Unicode_دلائل fromUArgs;
    UConverterConvert convert;

    /* error checking */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس;
    }

    اگر( ہدف_Cnv==خالی_اشاریہ || ماخذ_Cnv==خالی_اشاریہ ||
        ماخذ==خالی_اشاریہ || *ماخذ==خالی_اشاریہ ||
        ہدف==خالی_اشاریہ || *ہدف==خالی_اشاریہ || ہدف_حد_بندی==خالی_اشاریہ
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    s=*ماخذ;
    t=*ہدف;
    اگر((ماخذ_حد_بندی!=خالی_اشاریہ && ماخذ_حد_بندی<s) || ہدف_حد_بندی<t) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    اگر(
        (ماخذ_حد_بندی!=خالی_اشاریہ && ((size_t)(ماخذ_حد_بندی-s)>(size_t)0x7fffffff && ماخذ_حد_بندی>s)) ||
        ((size_t)(ہدف_حد_بندی-t)>(size_t)0x7fffffff && ہدف_حد_بندی>t)
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    
    اگر(محور_آغاز==خالی_اشاریہ) {
        اگر(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
            واپس;
        }

        /* use the stack pivot buffer */
        my_محور_ماخذ=my_محور_ہدف=محور_آغاز=محور_بفر;
        محور_ماخذ=(char16_t **)&my_محور_ماخذ;
        محور_ہدف=&my_محور_ہدف;
        محور_حد_بندی=محور_بفر+CHUNK_سائز;
    } ورنہ اگر(  محور_آغاز>=محور_حد_بندی ||
                محور_ماخذ==خالی_اشاریہ || *محور_ماخذ==خالی_اشاریہ ||
                محور_ہدف==خالی_اشاریہ || *محور_ہدف==خالی_اشاریہ ||
                محور_حد_بندی==خالی_اشاریہ
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    اگر(ماخذ_حد_بندی==خالی_اشاریہ) {
        /* get limit of single-byte-NUL-terminated source string */
        ماخذ_حد_بندی=uprv_strchr(*ماخذ, 0);
    }

    اگر(reset) {
        ucnv_reset_مقصد_Unicode(ماخذ_Cnv);
        ucnv_resetFromUnicode(ہدف_Cnv);
        *محور_ماخذ=*محور_ہدف=محور_آغاز;
    } ورنہ اگر(ہدف_Cnv->char_Error_بفر_لمبائی>0) {
        /* output the targetCnv overflow buffer */
        اگر(ucnv_output_حد_سے_تجاوز_From_Unicode(ہدف_Cnv, ہدف, ہدف_حد_بندی, خالی_اشاریہ, p_Error_رمز)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            واپس;
        }
        /* *target has moved, therefore stop using t */

        اگر( !flush &&
            ہدف_Cnv->preFromULength>=0 && *محور_ماخذ==*محور_ہدف &&
            ماخذ_Cnv->UChar_Error_بفر_لمبائی==0 && ماخذ_Cnv->pre_مقصد_ULength>=0 && s==ماخذ_حد_بندی
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            واپس;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    اگر( ماخذ_Cnv->shared_ڈیٹا->static_ڈیٹا->conversionType==UCNV_UTF8 &&
        ہدف_Cnv->shared_ڈیٹا->impl->fromUTF8!=خالی_اشاریہ
    ) {
        convert=ہدف_Cnv->shared_ڈیٹا->impl->fromUTF8;
    } ورنہ اگر( ہدف_Cnv->shared_ڈیٹا->static_ڈیٹا->conversionType==UCNV_UTF8 &&
               ماخذ_Cnv->shared_ڈیٹا->impl->مقصد_UTF8!=خالی_اشاریہ
    ) {
        convert=ماخذ_Cnv->shared_ڈیٹا->impl->مقصد_UTF8;
    } ورنہ {
        convert=خالی_اشاریہ;
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
    اگر(convert!=خالی_اشاریہ && (محور_حد_بندی-محور_آغاز)>32) {
        محور_حد_بندی=محور_آغاز+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=ہدف_Cnv;
    fromUArgs.flush=جھوٹ;
    fromUArgs.offsets=خالی_اشاریہ;
    fromUArgs.ہدف=*ہدف;
    fromUArgs.ہدف_حد_بندی=ہدف_حد_بندی;
    fromUArgs.size=حجم(fromUArgs);

    مقصد_UArgs.converter=ماخذ_Cnv;
    مقصد_UArgs.flush=flush;
    مقصد_UArgs.offsets=خالی_اشاریہ;
    مقصد_UArgs.ماخذ=s;
    مقصد_UArgs.ماخذ_حد_بندی=ماخذ_حد_بندی;
    مقصد_UArgs.ہدف_حد_بندی=محور_حد_بندی;
    مقصد_UArgs.size=حجم(مقصد_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=خالی_اشاریہ;

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
    برائے(;;) {
        /*
         * if(pivot not empty or error or replay or flush fromUnicode) {
         *   fromUnicode(pivot -> target);
         * }
         *
         * For pivoting conversion; and for direct conversion for
         * error callback handling and flushing the replay buffer.
         */
        اگر( *محور_ماخذ<*محور_ہدف ||
            U_FAILURE(*p_Error_رمز) ||
            ہدف_Cnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.ماخذ=*محور_ماخذ;
            fromUArgs.ماخذ_حد_بندی=*محور_ہدف;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_رمز);
            اگر(U_FAILURE(*p_Error_رمز)) {
                /* target overflow, or conversion error */
                *محور_ماخذ=(char16_t *)fromUArgs.ماخذ;
                توڑو;
            }

            /*
             * _fromUnicodeWithCallback() must have consumed the pivot contents
             * (*pivotSource==*pivotTarget) since it returned with U_SUCCESS()
             */
        }

        /* The pivot buffer is empty; reset it so we start at pivotStart. */
        *محور_ماخذ=*محور_ہدف=محور_آغاز;

        /*
         * if(sourceCnv overflow buffer not empty) {
         *     move(sourceCnv overflow buffer -> pivot);
         *     continue;
         * }
         */
        /* output the sourceCnv overflow buffer */
        اگر(ماخذ_Cnv->UChar_Error_بفر_لمبائی>0) {
            اگر(ucnv_output_حد_سے_تجاوز_مقصد_Unicode(ماخذ_Cnv, محور_ہدف, محور_حد_بندی, خالی_اشاریہ, p_Error_رمز)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_رمز=U_ZERO_ERROR;
            }
            جاری;
        }

        /*
         * check for end of input and break if done
         *
         * Checking both flush and fromUArgs.flush ensures that the converters
         * have been called with the flush flag set if the ucnv_convertEx()
         * caller set it.
         */
        اگر( مقصد_UArgs.ماخذ==ماخذ_حد_بندی &&
            ماخذ_Cnv->pre_مقصد_ULength>=0 && ماخذ_Cnv->مقصد_ULength==0 &&
            (!flush || fromUArgs.flush)
        ) {
            /* done successfully */
            توڑو;
        }

        /*
         * use direct conversion if available
         * but not if continuing a partial match
         * or flushing the toUnicode replay buffer
         */
        اگر(convert!=خالی_اشاریہ && ہدف_Cnv->preFromUFirstCP<0 && ماخذ_Cnv->pre_مقصد_ULength==0) {
            اگر(*p_Error_رمز==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_رمز=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &مقصد_UArgs, p_Error_رمز);
            اگر(*p_Error_رمز==U_بفر_حد_سے_تجاوز_ERROR) {
                توڑو;
            } ورنہ اگر(U_FAILURE(*p_Error_رمز)) {
                اگر(ماخذ_Cnv->مقصد_ULength>0) {
                    /*
                     * Fall through to calling _toUnicodeWithCallback()
                     * for callback handling.
                     *
                     * The pivot buffer will be reset with
                     *   *pivotSource=*pivotTarget=pivotStart;
                     * which indicates a toUnicode error to the caller
                     * (*pivotSource==pivotStart shows no pivot UChars consumed).
                     */
                } ورنہ {
                    /*
                     * Indicate a fromUnicode error to the caller
                     * (*pivotSource>pivotStart shows some pivot UChars consumed).
                     */
                    *محور_ماخذ=*محور_ہدف=محور_آغاز+1;
                    /*
                     * Loop around to calling _fromUnicodeWithCallbacks()
                     * for callback handling.
                     */
                    جاری;
                }
            } ورنہ اگر(*p_Error_رمز==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_رمز=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } ورنہ اگر(flush && ماخذ_Cnv->مقصد_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_رمز=U_TRUNCATED_CHAR_ملا;
            } ورنہ {
                /* input consumed */
                اگر(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(ماخذ_Cnv, UCNV_RESET_مقصد_UNICODE, جھوٹ);
                    _reset(ہدف_Cnv, UCNV_RESET_FROM_UNICODE, جھوٹ);
                }

                /* done successfully */
                توڑو;
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
        مقصد_UArgs.ہدف=محور_آغاز; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        مقصد_Unicode_With_Callback(&مقصد_UArgs, p_Error_رمز);
        *محور_ہدف=مقصد_UArgs.ہدف;
        اگر(*p_Error_رمز==U_بفر_حد_سے_تجاوز_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_رمز=U_ZERO_ERROR;
        } ورنہ اگر(U_FAILURE(*p_Error_رمز) || (!flush && *محور_ہدف==محور_آغاز)) {
            /* conversion error, or there was nothing left to convert */
            توڑو;
        }
        /*
         * else:
         * _toUnicodeWithCallback() wrote into the pivot buffer,
         * continue with fromUnicode conversion.
         *
         * Set the fromUnicode flush flag if we flush and if toUnicode has
         * processed the end of the input.
         */
        اگر( flush && مقصد_UArgs.ماخذ==ماخذ_حد_بندی &&
            ماخذ_Cnv->pre_مقصد_ULength>=0 &&
            ماخذ_Cnv->UChar_Error_بفر_لمبائی==0
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

    *ماخذ=مقصد_UArgs.ماخذ;
    *ہدف=fromUArgs.ہدف;

    /* terminate the target buffer if possible */
    اگر(flush && U_SUCCESS(*p_Error_رمز)) {
        اگر(*ہدف!=ہدف_حد_بندی) {
            **ہدف=0;
            اگر(*p_Error_رمز==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_رمز=U_ZERO_ERROR;
            }
        } ورنہ {
            *p_Error_رمز=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
ساکن int32_t
ucnv_اندرونی_Convert(UConverter *برآمد_Converter, UConverter *اندر_Converter,
                     حرف *ہدف, int32_t ہدف_استعداد,
                     ثابت حرف *ماخذ, int32_t ماخذ_لمبائی,
                     UError_رمز *p_Error_رمز) {
    char16_t محور_بفر[CHUNK_سائز];
    char16_t *محور, *pivot2;

    حرف *my_ہدف;
    ثابت حرف *ماخذ_حد_بندی;
    ثابت حرف *ہدف_حد_بندی;
    int32_t ہدف_لمبائی=0;

    /* set up */
    اگر(ماخذ_لمبائی<0) {
        ماخذ_حد_بندی=uprv_strchr(ماخذ, 0);
    } ورنہ {
        ماخذ_حد_بندی=ماخذ+ماخذ_لمبائی;
    }

    /* if there is no input data, we're done */
    اگر(ماخذ==ماخذ_حد_بندی) {
        واپس u_terminateChars(ہدف, ہدف_استعداد, 0, p_Error_رمز);
    }

    محور=pivot2=محور_بفر;
    my_ہدف=ہدف;
    ہدف_لمبائی=0;

    اگر(ہدف_استعداد>0) {
        /* perform real conversion */
        ہدف_حد_بندی=ہدف+ہدف_استعداد;
        ucnv_convertEx(برآمد_Converter, اندر_Converter,
                       &my_ہدف, ہدف_حد_بندی,
                       &ماخذ, ماخذ_حد_بندی,
                       محور_بفر, &محور, &pivot2, محور_بفر+CHUNK_سائز,
                       جھوٹ,
                       سچ,
                       p_Error_رمز);
        ہدف_لمبائی = static_cast<int32_t>(my_ہدف - ہدف);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    اگر(*p_Error_رمز==U_بفر_حد_سے_تجاوز_ERROR || ہدف_استعداد==0)
    {
        حرف ہدف_بفر[CHUNK_سائز];

        ہدف_حد_بندی=ہدف_بفر+CHUNK_سائز;
        کرو {
            *p_Error_رمز=U_ZERO_ERROR;
            my_ہدف=ہدف_بفر;
            ucnv_convertEx(برآمد_Converter, اندر_Converter,
                           &my_ہدف, ہدف_حد_بندی,
                           &ماخذ, ماخذ_حد_بندی,
                           محور_بفر, &محور, &pivot2, محور_بفر+CHUNK_سائز,
                           جھوٹ,
                           سچ,
                           p_Error_رمز);
            ہدف_لمبائی += static_cast<int32_t>(my_ہدف - ہدف_بفر);
        } جب(*p_Error_رمز==U_بفر_حد_سے_تجاوز_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        واپس u_terminateChars(ہدف, ہدف_استعداد, ہدف_لمبائی, p_Error_رمز);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    واپس ہدف_لمبائی;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ثابت حرف *مقصد_Converter_Name, ثابت حرف *fromConverterName,
             حرف *ہدف, int32_t ہدف_استعداد,
             ثابت حرف *ماخذ, int32_t ماخذ_لمبائی,
             UError_رمز *p_Error_رمز) {
    UConverter in, out; /* stack-allocated */
    UConverter *اندر_Converter, *برآمد_Converter;
    int32_t ہدف_لمبائی;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر( ماخذ==خالی_اشاریہ || ماخذ_لمبائی<-1 ||
        ہدف_استعداد<0 || (ہدف_استعداد>0 && ہدف==خالی_اشاریہ)
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* if there is no input data, we're done */
    اگر(ماخذ_لمبائی==0 || (ماخذ_لمبائی<0 && *ماخذ==0)) {
        واپس u_terminateChars(ہدف, ہدف_استعداد, 0, p_Error_رمز);
    }

    /* create the converters */
    اندر_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    برآمد_Converter=ucnv_createConverter(&out, مقصد_Converter_Name, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        ucnv_close(اندر_Converter);
        واپس 0;
    }

    ہدف_لمبائی=ucnv_اندرونی_Convert(برآمد_Converter, اندر_Converter,
                                      ہدف, ہدف_استعداد,
                                      ماخذ, ماخذ_لمبائی,
                                      p_Error_رمز);

    ucnv_close(اندر_Converter);
    ucnv_close(برآمد_Converter);

    واپس ہدف_لمبائی;
}

/* @internal */
ساکن int32_t
ucnv_convertAlgorithmic(UBool convert_مقصد_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        حرف *ہدف, int32_t ہدف_استعداد,
                        ثابت حرف *ماخذ, int32_t ماخذ_لمبائی,
                        UError_رمز *p_Error_رمز) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *مقصد, *from;
    int32_t ہدف_لمبائی;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر( cnv==خالی_اشاریہ || ماخذ==خالی_اشاریہ || ماخذ_لمبائی<-1 ||
        ہدف_استعداد<0 || (ہدف_استعداد>0 && ہدف==خالی_اشاریہ)
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* if there is no input data, we're done */
    اگر(ماخذ_لمبائی==0 || (ماخذ_لمبائی<0 && *ماخذ==0)) {
        واپس u_terminateChars(ہدف, ہدف_استعداد, 0, p_Error_رمز);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* reset the other converter */
    اگر(convert_مقصد_Algorithmic) {
        /* cnv->Unicode->algo */
        ucnv_reset_مقصد_Unicode(cnv);
        مقصد=algoConverter;
        from=cnv;
    } ورنہ {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        مقصد=cnv;
    }

    ہدف_لمبائی=ucnv_اندرونی_Convert(مقصد, from,
                                      ہدف, ہدف_استعداد,
                                      ماخذ, ماخذ_لمبائی,
                                      p_Error_رمز);

    ucnv_close(algoConverter);

    واپس ہدف_لمبائی;
}

U_CAPI int32_t U_EXPORT2
ucnv_مقصد_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   حرف *ہدف, int32_t ہدف_استعداد,
                   ثابت حرف *ماخذ, int32_t ماخذ_لمبائی,
                   UError_رمز *p_Error_رمز) {
    واپس ucnv_convertAlgorithmic(سچ, algorithmicType, cnv,
                                   ہدف, ہدف_استعداد,
                                   ماخذ, ماخذ_لمبائی,
                                   p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     حرف *ہدف, int32_t ہدف_استعداد,
                     ثابت حرف *ماخذ, int32_t ماخذ_لمبائی,
                     UError_رمز *p_Error_رمز) UPRV_NO_SANITIZE_UNDEFINED {

    اگر(algorithmicType<0 || UCNV_نمبر_کا_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    واپس ucnv_convertAlgorithmic(جھوٹ, algorithmicType, cnv,
                                   ہدف, ہدف_استعداد,
                                   ماخذ, ماخذ_لمبائی,
                                   p_Error_رمز);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ثابت UConverter* converter)
{
    int8_t type = converter->shared_ڈیٹا->static_ڈیٹا->conversionType;
#اگر !UCONFIG_NO_LEGACY_CONVERSION
    اگر(type == UCNV_MBCS) {
        واپس ucnv_MBCSGetType(converter);
    }
#اگر_ختم
    واپس (UConverterType)type;
}

U_CAPI خالی  U_EXPORT2
ucnv_getStarters(ثابت UConverter* converter, 
                 UBool starters[256],
                 UError_رمز* err)
{
    اگر (err == خالی_اشاریہ || U_FAILURE(*err)) {
        واپس;
    }

    اگر(converter->shared_ڈیٹا->impl->getStarters != خالی_اشاریہ) {
        converter->shared_ڈیٹا->impl->getStarters(converter, starters, err);
    } ورنہ {
        *err = U_ILLEGAL_دلیل_ERROR;
    }
}

ساکن ثابت UAmbiguousConverter *ucnv_getAmbiguous(ثابت UConverter *cnv)
{
    UError_رمز error_رمز;
    ثابت حرف *name;
    int32_t i;

    اگر(cnv==خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }

    error_رمز=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }

    برائے(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        اگر(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            واپس ambiguousConverters+i;
        }
    }

    واپس خالی_اشاریہ;
}

U_CAPI خالی  U_EXPORT2
ucnv_fixFileSeparator(ثابت UConverter *cnv, 
                      char16_t* ماخذ,
                      int32_t ماخذ_لمبائی) {
    ثابت UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    اگر(cnv==خالی_اشاریہ || ماخذ==خالی_اشاریہ || ماخذ_لمبائی<=0 || (a=ucnv_getAmbiguous(cnv))==خالی_اشاریہ)
    {
        واپس;
    }

    variant5c=a->variant5c;
    برائے(i=0; i<ماخذ_لمبائی; ++i) {
        اگر(ماخذ[i]==variant5c) {
            ماخذ[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ثابت UConverter *cnv) {
    واپس ucnv_getAmbiguous(cnv)!=خالی_اشاریہ;
}

U_CAPI خالی  U_EXPORT2
ucnv_سیٹ_Fallback(UConverter *cnv, UBool استعمال_کرتا_Fallback)
{
    cnv->useFallback = استعمال_کرتا_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_استعمال_کرتا_Fallback(ثابت UConverter *cnv)
{
    واپس cnv->useFallback;
}

U_CAPI خالی  U_EXPORT2
ucnv_getInvalidChars (ثابت UConverter * converter,
                      حرف *err_بائٹیں,
                      int8_t * len,
                      UError_رمز * err)
{
    اگر (err == خالی_اشاریہ || U_FAILURE(*err))
    {
        واپس;
    }
    اگر (len == خالی_اشاریہ || err_بائٹیں == خالی_اشاریہ || converter == خالی_اشاریہ)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر (*len < converter->invalid_Char_لمبائی)
    {
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس;
    }
    اگر ((*len = converter->invalid_Char_لمبائی) > 0)
    {
        uprv_memcpy (err_بائٹیں, converter->invalid_Char_بفر, *len);
    }
}

U_CAPI خالی  U_EXPORT2
ucnv_getInvalidUChars (ثابت UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_رمز * err)
{
    اگر (err == خالی_اشاریہ || U_FAILURE(*err))
    {
        واپس;
    }
    اگر (len == خالی_اشاریہ || errChars == خالی_اشاریہ || converter == خالی_اشاریہ)
    {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر (*len < converter->invalid_UChar_لمبائی)
    {
        *err = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس;
    }
    اگر ((*len = converter->invalid_UChar_لمبائی) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_بفر, *len);
    }
}

#تعریف_کرو SIG_MAX_LEN 5

U_CAPI ثابت حرف* U_EXPORT2
ucnv_detectUnicodeSignature( ثابت حرف* ماخذ,
                             int32_t ماخذ_لمبائی,
                             int32_t* signature_لمبائی,
                             UError_رمز* p_Error_رمز) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    حرف آغاز[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    صحیح i = 0;

    اگر((p_Error_رمز==خالی_اشاریہ) || U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }
    
    اگر(ماخذ == خالی_اشاریہ || ماخذ_لمبائی < -1){
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(signature_لمبائی == خالی_اشاریہ) {
        signature_لمبائی = &dummy;
    }

    اگر(ماخذ_لمبائی==-1){
        ماخذ_لمبائی=(int32_t)uprv_strlen(ماخذ);
    }

    
    جب(i<ماخذ_لمبائی&& i<SIG_MAX_LEN){
        آغاز[i]=ماخذ[i];
        i++;
    }

    اگر(آغاز[0] == '\xFE' && آغاز[1] == '\xFF') {
        *signature_لمبائی=2;
        واپس  "UTF-16BE";
    } ورنہ اگر(آغاز[0] == '\xFF' && آغاز[1] == '\xFE') {
        اگر(آغاز[2] == '\x00' && آغاز[3] =='\x00') {
            *signature_لمبائی=4;
            واپس "UTF-32LE";
        } ورنہ {
            *signature_لمبائی=2;
            واپس  "UTF-16LE";
        }
    } ورنہ اگر(آغاز[0] == '\xEF' && آغاز[1] == '\xBB' && آغاز[2] == '\xBF') {
        *signature_لمبائی=3;
        واپس  "UTF-8";
    } ورنہ اگر(آغاز[0] == '\x00' && آغاز[1] == '\x00' && 
              آغاز[2] == '\xFE' && آغاز[3]=='\xFF') {
        *signature_لمبائی=4;
        واپس  "UTF-32BE";
    } ورنہ اگر(آغاز[0] == '\x0E' && آغاز[1] == '\xFE' && آغاز[2] == '\xFF') {
        *signature_لمبائی=3;
        واپس "SCSU";
    } ورنہ اگر(آغاز[0] == '\xFB' && آغاز[1] == '\xEE' && آغاز[2] == '\x28') {
        *signature_لمبائی=3;
        واپس "BOCU-1";
    } ورنہ اگر(آغاز[0] == '\x2B' && آغاز[1] == '\x2F' && آغاز[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        اگر(آغاز[3] == '\x38' && آغاز[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_لمبائی=5;
            واپس "UTF-7";
        } ورنہ اگر(آغاز[3] == '\x38' || آغاز[3] == '\x39' || آغاز[3] == '\x2B' || آغاز[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_لمبائی=4;
            واپس "UTF-7";
        }
    }ورنہ اگر(آغاز[0]=='\xDD' && آغاز[1]== '\x73'&& آغاز[2]=='\x66' && آغاز[3]=='\x73'){
        *signature_لمبائی=4;
        واپس "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_لمبائی=0;
    واپس خالی_اشاریہ;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ثابت UConverter* cnv, UError_رمز* حالت_2)
{
    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس -1;
    }
    اگر(cnv == خالی_اشاریہ){
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس -1;
    }

    اگر(cnv->preFromUFirstCP >= 0){
        واپس U16_لمبائی(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }ورنہ اگر(cnv->preFromULength < 0){
        واپس -cnv->preFromULength ;
    }ورنہ اگر(cnv->fromUChar32 > 0){
        واپس 1;
    }
    واپس 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_مقصد_UCount_Pending(ثابت UConverter* cnv, UError_رمز* حالت_2){

    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس -1;
    }
    اگر(cnv == خالی_اشاریہ){
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس -1;
    }

    اگر(cnv->pre_مقصد_ULength > 0){
        واپس cnv->pre_مقصد_ULength ;
    }ورنہ اگر(cnv->pre_مقصد_ULength < 0){
        واپس -cnv->pre_مقصد_ULength;
    }ورنہ اگر(cnv->مقصد_ULength > 0){
        واپس cnv->مقصد_ULength;
    }
    واپس 0;
}

U_CAPI UBool U_EXPORT2
ucnv_is_Fixed_چوڑائی(UConverter *cnv, UError_رمز *حالت_2){
    اگر (U_FAILURE(*حالت_2)) {
        واپس جھوٹ;
    }

    اگر (cnv == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }

    انتخاب (ucnv_getType(cnv)) {
        حالت UCNV_SBCS:
        حالت UCNV_DBCS:
        حالت UCNV_UTF32_BigEndian:
        حالت UCNV_UTF32_LittleEndian:
        حالت UCNV_UTF32:
        حالت UCNV_US_ASCII:
            واپس سچ;
        طےشدہ:
            واپس جھوٹ;
    }
}
#اگر_ختم

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
