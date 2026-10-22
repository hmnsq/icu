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

#यदि !UCONFIG_NO_CONVERSION

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
#परिभाषय CHUNK_आकार 1024

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
                       UError_संहिता * err)
{
    UConverter *r;

    यदि (err == शून्यसूचकः || U_FAILURE (*err)) {
        प्रत्यागम शून्यसूचकः;
    }

    r =  ucnv_createConverter(शून्यसूचकः, name, err);
    प्रत्यागम r;
}

U_CAPI UConverter* U_EXPORT2 
ucnv_openPackage   (ध्रुव वर्ण *packageName, ध्रुव वर्ण *converterName, UError_संहिता * err)
{
    प्रत्यागम ucnv_createConverterFromPackage(packageName, converterName,  err);
}

/*Extracts the char16_t* to a char* and calls through createConverter */
U_CAPI UConverter*   U_EXPORT2
ucnv_openU (ध्रुव char16_t * name,
                         UError_संहिता * err)
{
    वर्ण asciiName[UCNV_MAX_CONVERTER_NAME_दीर्घता];

    यदि (err == शून्यसूचकः || U_FAILURE(*err))
        प्रत्यागम शून्यसूचकः;
    यदि (name == शून्यसूचकः)
        प्रत्यागम ucnv_open (शून्यसूचकः, err);
    यदि (u_strlen(name) >= UCNV_MAX_CONVERTER_NAME_दीर्घता)
    {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
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
                UError_संहिता * err)
{
    वर्ण myName[UCNV_MAX_CONVERTER_NAME_दीर्घता];
    int32_t myNameLen;

    यदि (err == शून्यसूचकः || U_FAILURE (*err))
        प्रत्यागम शून्यसूचकः;

    /* ucnv_copyPlatformString could return "ibm-" or "cp" */
    myNameLen = ucnv_copyPlatformString(myName, platform);
    T_CString_integer_गन्तव्यम्_String(myName + myNameLen, codepage, 10);

    प्रत्यागम ucnv_createConverter(शून्यसूचकः, myName, err);
}

/* Creating a temporary stack-based object that can be used in one thread, 
and created from a converter that is shared across threads.
*/

U_CAPI UConverter* U_EXPORT2
ucnv_safeClone(ध्रुव UConverter* cnv,
               शून्य* राशिः_बफर U_LIFETIME_BOUND,
               int32_t* p_बफर_आकार,
               UError_संहिता* स्थितिः)
{
    UConverter *localConverter, *allocatedConverter;
    int32_t राशिः_बफर_आकार;
    int32_t बफर_आकार_Needed;
    UError_संहिता cbErr;
    UConverter_गन्तव्यम्_Unicode_तर्काः गन्तव्यम्_UArgs = {
        परिमाण(UConverter_गन्तव्यम्_Unicode_तर्काः),
            सत्य,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः
    };
    UConverter_From_Unicode_तर्काः fromUArgs = {
        परिमाण(UConverter_From_Unicode_तर्काः),
            सत्य,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः
    };

    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_CLONE);

    यदि (स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        UTRACE_EXIT_स्थितिः(स्थितिः? *स्थितिः: U_ILLEGAL_तर्कः_ERROR);
        प्रत्यागम शून्यसूचकः;
    }

    यदि (cnv == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        UTRACE_EXIT_स्थितिः(*स्थितिः);
        प्रत्यागम शून्यसूचकः;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "clone converter %s at %p into stackBuffer %p",
                                    ucnv_getName(cnv, स्थितिः), cnv, राशिः_बफर);

    यदि (cnv->shared_दत्त->impl->safeClone != शून्यसूचकः) {
        /* call the custom safeClone function for sizing */
        बफर_आकार_Needed = 0;
        cnv->shared_दत्त->impl->safeClone(cnv, शून्यसूचकः, &बफर_आकार_Needed, स्थितिः);
        यदि (U_FAILURE(*स्थितिः)) {
            UTRACE_EXIT_स्थितिः(*स्थितिः);
            प्रत्यागम शून्यसूचकः;
        }
    }
    अन्यथा
    {
        /* inherent sizing */
        बफर_आकार_Needed = परिमाण(UConverter);
    }

    यदि (p_बफर_आकार == शून्यसूचकः) {
        राशिः_बफर_आकार = 1;
        p_बफर_आकार = &राशिः_बफर_आकार;
    } अन्यथा {
        राशिः_बफर_आकार = *p_बफर_आकार;
        यदि (राशिः_बफर_आकार <= 0){ /* 'preflighting' request - set needed size into *pBufferSize */
            *p_बफर_आकार = बफर_आकार_Needed;
            UTRACE_EXIT_मानम्(बफर_आकार_Needed);
            प्रत्यागम शून्यसूचकः;
        }
    }

    /* Adjust (if necessary) the stackBuffer pointer to be aligned correctly for a UConverter.
     * TODO(Jira ICU-20736) Redo this using std::align() once g++4.9 compatibility is no longer needed.
     */
    यदि (राशिः_बफर) {
        uintptr_t p = reinterpret_cast<uintptr_t>(राशिः_बफर);
        uintptr_t aligned_p = (p + alignof(UConverter) - 1) & ~(alignof(UConverter) - 1);
        ptrdiff_t pointerAdjustment = aligned_p - p;
        यदि (बफर_आकार_Needed + pointerAdjustment <= राशिः_बफर_आकार) {
            राशिः_बफर = reinterpret_cast<शून्य *>(aligned_p);
            राशिः_बफर_आकार -= static_cast<int32_t>(pointerAdjustment);
        } अन्यथा {
            /* prevent using the stack buffer but keep the size > 0 so that we do not just preflight */
            राशिः_बफर_आकार = 1;
        }
    }

    /* Now, see if we must allocate any memory */
    यदि (राशिः_बफर_आकार < बफर_आकार_Needed || राशिः_बफर == शून्यसूचकः)
    {
        /* allocate one here...*/
        localConverter = allocatedConverter = (UConverter *) uprv_malloc (बफर_आकार_Needed);

        यदि(localConverter == शून्यसूचकः) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            UTRACE_EXIT_स्थितिः(*स्थितिः);
            प्रत्यागम शून्यसूचकः;
        }
        // If pBufferSize was nullptr as the input, pBufferSize is set to &stackBufferSize in this function.
        यदि (p_बफर_आकार != &राशिः_बफर_आकार) {
            *स्थितिः = U_SAFECLONE_ALLOCATED_WARNING;
        }

        /* record the fact that memory was allocated */
        *p_बफर_आकार = बफर_आकार_Needed;
    } अन्यथा {
        /* just use the stack buffer */
        localConverter = (UConverter*) राशिः_बफर;
        allocatedConverter = शून्यसूचकः;
    }

    uprv_memset(localConverter, 0, बफर_आकार_Needed);

    /* Copy initial state */
    uprv_memcpy(localConverter, cnv, परिमाण(UConverter));
    localConverter->isCopyLocal = localConverter->isExtraLocal = असत्य;

    /* copy the substitution string */
    यदि (cnv->subChars == (uint8_t *)cnv->subUChars) {
        localConverter->subChars = (uint8_t *)localConverter->subUChars;
    } अन्यथा {
        localConverter->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_बफर_दीर्घता * U_SIZEOF_UCHAR);
        यदि (localConverter->subChars == शून्यसूचकः) {
            uprv_free(allocatedConverter);
            UTRACE_EXIT_स्थितिः(*स्थितिः);
            प्रत्यागम शून्यसूचकः;
        }
        uprv_memcpy(localConverter->subChars, cnv->subChars, UCNV_ERROR_बफर_दीर्घता * U_SIZEOF_UCHAR);
    }

    /* now either call the safeclone fcn or not */
    यदि (cnv->shared_दत्त->impl->safeClone != शून्यसूचकः) {
        /* call the custom safeClone function */
        localConverter = cnv->shared_दत्त->impl->safeClone(cnv, localConverter, p_बफर_आकार, स्थितिः);
    }

    यदि(localConverter==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        यदि (allocatedConverter != शून्यसूचकः && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars) {
            uprv_free(allocatedConverter->subChars);
        }
        uprv_free(allocatedConverter);
        UTRACE_EXIT_स्थितिः(*स्थितिः);
        प्रत्यागम शून्यसूचकः;
    }

    /* increment refcount of shared data if needed */
    यदि (cnv->shared_दत्त->isReferenceCounted) {
        ucnv_increment_Ref_गणनम्(cnv->shared_दत्त);
    }

    यदि(localConverter == (UConverter*)राशिः_बफर) {
        /* we're using user provided data - set to not destroy */
        localConverter->isCopyLocal = सत्य;
    }

    /* allow callback functions to handle any memory allocation */
    गन्तव्यम्_UArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    cnv->fromCharErrorBehaviour(cnv->गन्तव्यम्_UContext, &गन्तव्यम्_UArgs, शून्यसूचकः, 0, UCNV_CLONE, &cbErr);
    cbErr = U_ZERO_ERROR;
    cnv->fromUCharErrorBehaviour(cnv->fromUContext, &fromUArgs, शून्यसूचकः, 0, 0, UCNV_CLONE, &cbErr);

    UTRACE_EXIT_PTR_स्थितिः(localConverter, *स्थितिः);
    प्रत्यागम localConverter;
}

U_CAPI UConverter* U_EXPORT2
ucnv_clone(ध्रुव UConverter* cnv, UError_संहिता *स्थितिः)
{
    प्रत्यागम ucnv_safeClone(cnv, शून्यसूचकः, शून्यसूचकः, स्थितिः);
}

/*Decreases the reference counter in the shared immutable section of the object
 *and frees the mutable part*/

U_CAPI शून्य  U_EXPORT2
ucnv_close (UConverter * converter)
{
    UError_संहिता error_संहिता = U_ZERO_ERROR;

    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_CLOSE);

    यदि (converter == शून्यसूचकः)
    {
        UTRACE_EXIT();
        प्रत्यागम;
    }

    UTRACE_DATA3(UTRACE_OPEN_CLOSE, "close converter %s at %p, isCopyLocal=%b",
        ucnv_getName(converter, &error_संहिता), converter, converter->isCopyLocal);

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    यदि (converter->fromCharErrorBehaviour != UCNV_गन्तव्यम्_U_DEFAULT_CALLBACK) {
        UConverter_गन्तव्यम्_Unicode_तर्काः गन्तव्यम्_UArgs = {
            परिमाण(UConverter_गन्तव्यम्_Unicode_तर्काः),
                सत्य,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः
        };

        गन्तव्यम्_UArgs.converter = converter;
        error_संहिता = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour(converter->गन्तव्यम्_UContext, &गन्तव्यम्_UArgs, शून्यसूचकः, 0, UCNV_CLOSE, &error_संहिता);
    }
    यदि (converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
        UConverter_From_Unicode_तर्काः fromUArgs = {
            परिमाण(UConverter_From_Unicode_तर्काः),
                सत्य,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः
        };
        fromUArgs.converter = converter;
        error_संहिता = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, शून्यसूचकः, 0, 0, UCNV_CLOSE, &error_संहिता);
    }

    यदि (converter->shared_दत्त->impl->close != शून्यसूचकः) {
        converter->shared_दत्त->impl->close(converter);
    }

    यदि (converter->subChars != (uint8_t *)converter->subUChars) {
        uprv_free(converter->subChars);
    }

    यदि (converter->shared_दत्त->isReferenceCounted) {
        ucnv_unload_Shared_दत्त_If_Ready(converter->shared_दत्त);
    }

    यदि(!converter->isCopyLocal){
        uprv_free(converter);
    }

    UTRACE_EXIT();
}

/*returns a single Name from the list, will return nullptr if out of bounds
 */
U_CAPI ध्रुव वर्ण*   U_EXPORT2
ucnv_get_उपलब्धम्_Name (int32_t n)
{
    यदि (0 <= n && n <= 0xffff) {
        UError_संहिता err = U_ZERO_ERROR;
        ध्रुव वर्ण *name = ucnv_bld_get_उपलब्धम्_Converter((uint16_t)n, &err);
        यदि (U_SUCCESS(err)) {
            प्रत्यागम name;
        }
    }
    प्रत्यागम शून्यसूचकः;
}

U_CAPI int32_t   U_EXPORT2
ucnv_गणनम्_उपलब्धम् ()
{
    UError_संहिता err = U_ZERO_ERROR;
    प्रत्यागम ucnv_bld_गणनम्_उपलब्धम्_Converters(&err);
}

U_CAPI शून्य    U_EXPORT2
ucnv_getSubstChars (ध्रुव UConverter * converter,
                    वर्ण *mySubChar,
                    int8_t * len,
                    UError_संहिता * err)
{
    यदि (U_FAILURE (*err))
        प्रत्यागम;

    यदि (converter->subCharLen <= 0) {
        /* Unicode string or empty string from ucnv_setSubstString(). */
        *len = 0;
        प्रत्यागम;
    }

    यदि (*len < converter->subCharLen) /*not enough space in subChars */
    {
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }

    uprv_memcpy (mySubChar, converter->subChars, converter->subCharLen);   /*fills in the subchars */
    *len = converter->subCharLen; /*store # of bytes copied to buffer */
}

U_CAPI शून्य    U_EXPORT2
ucnv_समुच्चयः_Subst_Chars (UConverter * converter,
                    ध्रुव वर्ण *mySubChar,
                    int8_t len,
                    UError_संहिता * err)
{
    यदि (U_FAILURE (*err))
        प्रत्यागम;
    
    /*Makes sure that the subChar is within the codepages char length boundaries */
    यदि ((len > converter->shared_दत्त->static_दत्त->max_अष्टकानि_प्रत्येकम्_Char)
     || (len < converter->shared_दत्त->static_दत्त->min_अष्टकानि_प्रत्येकम्_Char))
    {
        *err = U_ILLEGAL_तर्कः_ERROR;
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
ucnv_समुच्चयः_Subst_String(UConverter *cnv,
                    ध्रुव char16_t *s,
                    int32_t length,
                    UError_संहिता *err) {
    alignas(UConverter) वर्ण clone_बफर[U_CNV_SAFECLONE_BUFFERSIZE];
    वर्ण chars[UCNV_ERROR_बफर_दीर्घता];

    UConverter *clone;
    uint8_t *subChars;
    int32_t clone_आकार, length8;

    /* Let the following functions check all arguments. */
    clone_आकार = परिमाण(clone_बफर);
    clone = ucnv_safeClone(cnv, clone_बफर, &clone_आकार, err);
    ucnv_समुच्चयः_From_UCall_पृष्ठभागः(clone, UCNV_FROM_U_CALLBACK_STOP, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, err);
    length8 = ucnv_fromUChars(clone, chars, (int32_t)परिमाण(chars), s, length, err);
    ucnv_close(clone);
    यदि (U_FAILURE(*err)) {
        प्रत्यागम;
    }

    यदि (cnv->shared_दत्त->impl->writeSub == शून्यसूचकः
#यदि !UCONFIG_NO_LEGACY_CONVERSION
        || (cnv->shared_दत्त->static_दत्त->conversionType == UCNV_MBCS &&
         ucnv_MBCSGetType(cnv) != UCNV_EBCDIC_STATEFUL)
#यदि_अन्तः
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
        यदि (length > UCNV_ERROR_बफर_दीर्घता) {
            /*
             * Should not occur. The converter should output at least one byte
             * per char16_t, which means that ucnv_fromUChars() should catch all
             * overflows.
             */
            *err = U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम;
        }
        subChars = (uint8_t *)s;
        यदि (length < 0) {
            length = u_strlen(s);
        }
        length8 = length * U_SIZEOF_UCHAR;
    }

    /*
     * For storing the substitution string, select either the small buffer inside
     * UConverter or allocate a subChars buffer.
     */
    यदि (length8 > UCNV_MAX_SUBCHAR_LEN) {
        /* Use a separate buffer for the string. Outside UConverter to not make it too large. */
        यदि (cnv->subChars == (uint8_t *)cnv->subUChars) {
            /* Allocate a new buffer for the string. */
            cnv->subChars = (uint8_t *)uprv_malloc(UCNV_ERROR_बफर_दीर्घता * U_SIZEOF_UCHAR);
            यदि (cnv->subChars == शून्यसूचकः) {
                cnv->subChars = (uint8_t *)cnv->subUChars;
                *err = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            uprv_memset(cnv->subChars, 0, UCNV_ERROR_बफर_दीर्घता * U_SIZEOF_UCHAR);
        }
    }

    /* Copy the substitution string into the UConverter or its subChars buffer. */
    यदि (length8 == 0) {
        cnv->subCharLen = 0;
    } अन्यथा {
        uprv_memcpy(cnv->subChars, subChars, length8);
        यदि (subChars == (uint8_t *)chars) {
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
स्थिर शून्य _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    यदि(converter == शून्यसूचकः) {
        प्रत्यागम;
    }

    यदि(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UError_संहिता error_संहिता;

        यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE && converter->fromCharErrorBehaviour != UCNV_गन्तव्यम्_U_DEFAULT_CALLBACK) {
            UConverter_गन्तव्यम्_Unicode_तर्काः गन्तव्यम्_UArgs = {
                परिमाण(UConverter_गन्तव्यम्_Unicode_तर्काः),
                सत्य,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः
            };
            गन्तव्यम्_UArgs.converter = converter;
            error_संहिता = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->गन्तव्यम्_UContext, &गन्तव्यम्_UArgs, शून्यसूचकः, 0, UCNV_RESET, &error_संहिता);
        }
        यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverter_From_Unicode_तर्काः fromUArgs = {
                परिमाण(UConverter_From_Unicode_तर्काः),
                सत्य,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः,
                शून्यसूचकः
            };
            fromUArgs.converter = converter;
            error_संहिता = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, शून्यसूचकः, 0, 0, UCNV_RESET, &error_संहिता);
        }
    }

    /* now reset the converter itself */
    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        converter->गन्तव्यम्_Unicode_स्थितिः = converter->shared_दत्त->गन्तव्यम्_Unicode_स्थितिः;
        converter->बहुलकम् = 0;
        converter->गन्तव्यम्_ULength = 0;
        converter->invalid_Char_दीर्घता = converter->UChar_Error_बफर_दीर्घता = 0;
        converter->pre_गन्तव्यम्_ULength = 0;
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
        converter->from_Unicode_स्थितिः = 0;
        converter->fromUChar32 = 0;
        converter->invalid_UChar_दीर्घता = converter->char_Error_बफर_दीर्घता = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    यदि (converter->shared_दत्त->impl->reset != शून्यसूचकः) {
        /* call the custom reset function */
        converter->shared_दत्त->impl->reset(converter, choice);
    }
}

U_CAPI शून्य  U_EXPORT2
ucnv_reset(UConverter *converter)
{
    _reset(converter, UCNV_RESET_BOTH, सत्य);
}

U_CAPI शून्य  U_EXPORT2
ucnv_reset_गन्तव्यम्_Unicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_गन्तव्यम्_UNICODE, सत्य);
}

U_CAPI शून्य  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, सत्य);
}

U_CAPI int8_t   U_EXPORT2
ucnv_get_Max_Char_आकार (ध्रुव UConverter * converter)
{
    प्रत्यागम converter->max_अष्टकानि_प्रत्येकम्_UChar;
}


U_CAPI int8_t   U_EXPORT2
ucnv_get_Min_Char_आकार (ध्रुव UConverter * converter)
{
    प्रत्यागम converter->shared_दत्त->static_दत्त->min_अष्टकानि_प्रत्येकम्_Char;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ucnv_getName(ध्रुव UConverter* converter U_LIFETIME_BOUND, UError_संहिता* err)
{
    यदि (U_FAILURE (*err))
        प्रत्यागम शून्यसूचकः;
    यदि(converter->shared_दत्त->impl->getName){
        ध्रुव वर्ण* अस्थायी= converter->shared_दत्त->impl->getName(converter);
        यदि(अस्थायी)
            प्रत्यागम अस्थायी;
    }
    प्रत्यागम converter->shared_दत्त->static_दत्त->name;
}

U_CAPI int32_t U_EXPORT2
ucnv_getCCSID(ध्रुव UConverter * converter,
              UError_संहिता * err)
{
    int32_t ccsid;
    यदि (U_FAILURE (*err))
        प्रत्यागम -1;

    ccsid = converter->shared_दत्त->static_दत्त->codepage;
    यदि (ccsid == 0) {
        /* Rare case. This is for cases like gb18030,
        which doesn't have an IBM canonical name, but does have an IBM alias. */
        ध्रुव वर्ण *standardName = ucnv_getStandardName(ucnv_getName(converter, err), "IBM", err);
        यदि (U_SUCCESS(*err) && standardName) {
            ध्रुव वर्ण *ccsidStr = uprv_strchr(standardName, '-');
            यदि (ccsidStr) {
                ccsid = (int32_t)atol(ccsidStr+1);  /* +1 to skip '-' */
            }
        }
    }
    प्रत्यागम ccsid;
}


U_CAPI UConverterPlatform   U_EXPORT2
ucnv_getPlatform (ध्रुव UConverter * converter,
                                      UError_संहिता * err)
{
    यदि (U_FAILURE (*err))
        प्रत्यागम UCNV_UNKNOWN;

    प्रत्यागम (UConverterPlatform)converter->shared_दत्त->static_दत्त->platform;
}

U_CAPI शून्य U_EXPORT2
    ucnv_get_गन्तव्यम्_UCall_पृष्ठभागः (ध्रुव UConverter * converter,
                         UConverter_गन्तव्यम्_UCallback *action,
                         ध्रुव शून्य **context)
{
    *action = converter->fromCharErrorBehaviour;
    *context = converter->गन्तव्यम्_UContext;
}

U_CAPI शून्य U_EXPORT2
    ucnv_get_From_UCall_पृष्ठभागः (ध्रुव UConverter * converter,
                           UConverterFromUCallback *action,
                           ध्रुव शून्य **context)
{
    *action = converter->fromUCharErrorBehaviour;
    *context = converter->fromUContext;
}

U_CAPI शून्य    U_EXPORT2
ucnv_समुच्चयः_गन्तव्यम्_UCall_पृष्ठभागः (UConverter * converter,
                            UConverter_गन्तव्यम्_UCallback newAction,
                            ध्रुव शून्य* newContext,
                            UConverter_गन्तव्यम्_UCallback *oldAction,
                            ध्रुव शून्य** oldContext,
                            UError_संहिता * err)
{
    यदि (U_FAILURE (*err))
        प्रत्यागम;
    यदि (oldAction) *oldAction = converter->fromCharErrorBehaviour;
    converter->fromCharErrorBehaviour = newAction;
    यदि (oldContext) *oldContext = converter->गन्तव्यम्_UContext;
    converter->गन्तव्यम्_UContext = newContext;
}

U_CAPI शून्य  U_EXPORT2
ucnv_समुच्चयः_From_UCall_पृष्ठभागः (UConverter * converter,
                            UConverterFromUCallback newAction,
                            ध्रुव शून्य* newContext,
                            UConverterFromUCallback *oldAction,
                            ध्रुव शून्य** oldContext,
                            UError_संहिता * err)
{
    यदि (U_FAILURE (*err))
        प्रत्यागम;
    यदि (oldAction) *oldAction = converter->fromUCharErrorBehaviour;
    converter->fromUCharErrorBehaviour = newAction;
    यदि (oldContext) *oldContext = converter->fromUContext;
    converter->fromUContext = newContext;
}

स्थिर शून्य
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t source_सूचकांक, int32_t error_निवेशः_दीर्घता) {
    int32_t *सीमा;
    int32_t delta, विस्थापनम्;

    यदि(source_सूचकांक>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=source_सूचकांक-error_निवेशः_दीर्घता;
    } अन्यथा {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    सीमा=offsets+length;
    यदि(delta==0) {
        /* most common case, nothing to do */
    } अन्यथा यदि(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        यावत्(offsets<सीमा) {
            विस्थापनम्=*offsets;
            यदि(विस्थापनम्>=0) {
                *offsets=विस्थापनम्+delta;
            }
            ++offsets;
        }
    } अन्यथा /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        यावत्(offsets<सीमा) {
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
_fromUnicodeWithCallback(UConverter_From_Unicode_तर्काः *p_तर्काः, UError_संहिता *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    ध्रुव char16_t *s;
    वर्ण *t;
    int32_t *offsets;
    int32_t source_सूचकांक;
    int32_t error_निवेशः_दीर्घता;
    UBool converter_Saw_अन्तः_सम्बन्धि_निवेशः, calledCallback;

    /* variables for m:n conversion */
    char16_t replay[UCNV_EXT_MAX_UCHARS];
    ध्रुव char16_t *realSource, *real_Source_सीमा;
    int32_t real_Source_सूचकांक;
    UBool realFlush;

    cnv=p_तर्काः->converter;
    s=p_तर्काः->source;
    t=p_तर्काः->target;
    offsets=p_तर्काः->offsets;

    /* get the converter implementation function */
    source_सूचकांक=0;
    यदि(offsets==शून्यसूचकः) {
        fromUnicode=cnv->shared_दत्त->impl->fromUnicode;
    } अन्यथा {
        fromUnicode=cnv->shared_दत्त->impl->fromUnicodeWithOffsets;
        यदि(fromUnicode==शून्यसूचकः) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->shared_दत्त->impl->fromUnicode;
            /* we will write -1 for each offset */
            source_सूचकांक=-1;
        }
    }

    यदि(cnv->preFromULength>=0) {
        /* normal mode */
        realSource=शून्यसूचकः;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_Source_सीमा=शून्यसूचकः;
        realFlush=असत्य;
        real_Source_सूचकांक=0;
    } अन्यथा {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        यदि (!p_तर्काः->source) {
            *err=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
        realSource=p_तर्काः->source;
        real_Source_सीमा=p_तर्काः->source_सीमा;
        realFlush=p_तर्काः->flush;
        real_Source_सूचकांक=source_सूचकांक;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        p_तर्काः->source=replay;
        p_तर्काः->source_सीमा=replay-cnv->preFromULength;
        p_तर्काः->flush=असत्य;
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
    प्रति(;;) {
        यदि(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(p_तर्काः, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_अन्तः_सम्बन्धि_निवेशः=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_तर्काः->flush && p_तर्काः->source==p_तर्काः->source_सीमा &&
                        cnv->fromUChar32==0);
        } अन्यथा {
            /* handle error from ucnv_convertEx() */
            converter_Saw_अन्तः_सम्बन्धि_निवेशः=असत्य;
        }

        /* no callback called yet for this iteration */
        calledCallback=असत्य;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_निवेशः_दीर्घता=0;

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
            यदि(offsets!=शून्यसूचकः) {
                int32_t length = static_cast<int32_t>(p_तर्काः->target - t);
                यदि(length>0) {
                    _updateOffsets(offsets, length, source_सूचकांक, error_निवेशः_दीर्घता);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_तर्काः->offsets=offsets+=length;
                }

                यदि(source_सूचकांक>=0) {
                    source_सूचकांक += static_cast<int32_t>(p_तर्काः->source - s);
                }
            }

            यदि(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                यदि(realSource==शून्यसूचकः) {
                    realSource=p_तर्काः->source;
                    real_Source_सीमा=p_तर्काः->source_सीमा;
                    realFlush=p_तर्काः->flush;
                    real_Source_सूचकांक=source_सूचकांक;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    p_तर्काः->source=replay;
                    p_तर्काः->source_सीमा=replay-cnv->preFromULength;
                    p_तर्काः->flush=असत्य;
                    यदि((source_सूचकांक+=cnv->preFromULength)<0) {
                        source_सूचकांक=-1;
                    }

                    cnv->preFromULength=0;
                } अन्यथा {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==शून्यसूचकः);
                    *err=U_आन्तरिकम्_क्रमादेशः_ERROR;
                }
            }

            /* update pointers */
            s=p_तर्काः->source;
            t=p_तर्काः->target;

            यदि(U_SUCCESS(*err)) {
                यदि(s<p_तर्काः->source_सीमा) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    भङ्ग;
                } अन्यथा यदि(realSource!=शून्यसूचकः) {
                    /* switch back from replaying to the real source and continue */
                    p_तर्काः->source=realSource;
                    p_तर्काः->source_सीमा=real_Source_सीमा;
                    p_तर्काः->flush=realFlush;
                    source_सूचकांक=real_Source_सूचकांक;

                    realSource=शून्यसूचकः;
                    भङ्ग;
                } अन्यथा यदि(p_तर्काः->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_प्राप्तम्;
                    calledCallback=असत्य; /* new error condition */
                } अन्यथा {
                    /* input consumed */
                    यदि(p_तर्काः->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        यदि(!converter_Saw_अन्तः_सम्बन्धि_निवेशः) {
                            भङ्ग;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, असत्य);
                    }

                    /* done successfully */
                    प्रत्यागम;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_संहिता e;

                यदि( calledCallback ||
                    (e=*err)==U_बफर_अतिप्रवाहः_ERROR ||
                    (e!=U_INVALID_CHAR_प्राप्तम् &&
                     e!=U_ILLEGAL_CHAR_प्राप्तम् &&
                     e!=U_TRUNCATED_CHAR_प्राप्तम्)
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
                    यदि(realSource!=शून्यसूचकः) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length = static_cast<int32_t>(p_तर्काः->source_सीमा - p_तर्काः->source);
                        यदि(length>0) {
                            u_memcpy(cnv->preFromU, p_तर्काः->source, length);
                            cnv->preFromULength = static_cast<int8_t>(-length);
                        }

                        p_तर्काः->source=realSource;
                        p_तर्काः->source_सीमा=real_Source_सीमा;
                        p_तर्काः->flush=realFlush;
                    }

                    प्रत्यागम;
                }
            }

            /* callback handling */
            {
                UChar32 संहिता_Point;

                /* get and write the code point */
                संहिता_Point=cnv->fromUChar32;
                error_निवेशः_दीर्घता=0;
                U16_APPEND_UNSAFE(cnv->invalid_UChar_बफर, error_निवेशः_दीर्घता, संहिता_Point);
                cnv->invalid_UChar_दीर्घता = static_cast<int8_t>(error_निवेशः_दीर्घता);

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, p_तर्काः,
                    cnv->invalid_UChar_बफर, error_निवेशः_दीर्घता, संहिता_Point,
                    *err==U_INVALID_CHAR_प्राप्तम् ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
                    err);
            }

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=सत्य;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return true if overflow
 */
स्थिर UBool
ucnv_output_अतिप्रवाहः_From_Unicode(UConverter *cnv,
                               वर्ण **target, ध्रुव वर्ण *target_सीमा,
                               int32_t **pOffsets,
                               UError_संहिता *err) {
    int32_t *offsets;
    वर्ण *अतिप्रवाहः, *t;
    int32_t i, length;

    t=*target;
    यदि(pOffsets!=शून्यसूचकः) {
        offsets=*pOffsets;
    } अन्यथा {
        offsets=शून्यसूचकः;
    }

    अतिप्रवाहः = reinterpret_cast<वर्ण*>(cnv->char_Error_बफर);
    length=cnv->char_Error_बफर_दीर्घता;
    i=0;
    यावत्(i<length) {
        यदि(t==target_सीमा) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            कुरु {
                अतिप्रवाहः[j++]=अतिप्रवाहः[i++];
            } यावत्(i<length);

            cnv->char_Error_बफर_दीर्घता = static_cast<int8_t>(j);
            *target=t;
            यदि(offsets!=शून्यसूचकः) {
                *pOffsets=offsets;
            }
            *err=U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम सत्य;
        }

        /* copy the overflow contents to the target */
        *t++=अतिप्रवाहः[i++];
        यदि(offsets!=शून्यसूचकः) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->char_Error_बफर_दीर्घता=0;
    *target=t;
    यदि(offsets!=शून्यसूचकः) {
        *pOffsets=offsets;
    }
    प्रत्यागम असत्य;
}

U_CAPI शून्य U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 वर्ण **target, ध्रुव वर्ण *target_सीमा,
                 ध्रुव char16_t **source, ध्रुव char16_t *source_सीमा,
                 int32_t *offsets,
                 UBool flush,
                 UError_संहिता *err) {
    UConverter_From_Unicode_तर्काः तर्काः_2;
    ध्रुव char16_t *s;
    वर्ण *t;

    /* check parameters */
    यदि(err==शून्यसूचकः || U_FAILURE(*err)) {
        प्रत्यागम;
    }

    यदि(cnv==शून्यसूचकः || target==शून्यसूचकः || source==शून्यसूचकः) {
        *err=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    s=*source;
    t=*target;

    यदि ((ध्रुव शून्य *)U_MAX_PTR(source_सीमा) == (ध्रुव शून्य *)source_सीमा) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        source_सीमा = (ध्रुव char16_t *)(((ध्रुव वर्ण *)source_सीमा) - 1);
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
    यदि (source_सीमा<s || target_सीमा<t ||
        ((size_t)(source_सीमा-s)>(size_t)0x3fffffff && source_सीमा>s) ||
        ((size_t)(target_सीमा-t)>(size_t)0x7fffffff && target_सीमा>t) ||
        (((ध्रुव वर्ण *)source_सीमा-(ध्रुव वर्ण *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    
    /* output the target overflow buffer */
    यदि( cnv->char_Error_बफर_दीर्घता>0 &&
        ucnv_output_अतिप्रवाहः_From_Unicode(cnv, target, target_सीमा, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        प्रत्यागम;
    }
    /* *target may have moved, therefore stop using t */

    यदि(!flush && s==source_सीमा && cnv->preFromULength>=0) {
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
    तर्काः_2.converter=cnv;
    तर्काः_2.flush=flush;
    तर्काः_2.offsets=offsets;
    तर्काः_2.source=s;
    तर्काः_2.source_सीमा=source_सीमा;
    तर्काः_2.target=*target;
    तर्काः_2.target_सीमा=target_सीमा;
    तर्काः_2.size=परिमाण(तर्काः_2);

    _fromUnicodeWithCallback(&तर्काः_2, err);

    *source=तर्काः_2.source;
    *target=तर्काः_2.target;
}

/* ucnv_toUnicode() --------------------------------------------------------- */

स्थिर शून्य
गन्तव्यम्_Unicode_With_Callback(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः, UError_संहिता *err) {
    UConverter_गन्तव्यम्_Unicode गन्तव्यम्_Unicode;
    UConverter *cnv;
    ध्रुव वर्ण *s;
    char16_t *t;
    int32_t *offsets;
    int32_t source_सूचकांक;
    int32_t error_निवेशः_दीर्घता;
    UBool converter_Saw_अन्तः_सम्बन्धि_निवेशः, calledCallback;

    /* variables for m:n conversion */
    वर्ण replay[UCNV_EXT_MAX_अष्टकानि];
    ध्रुव वर्ण *realSource, *real_Source_सीमा;
    int32_t real_Source_सूचकांक;
    UBool realFlush;

    cnv=p_तर्काः->converter;
    s=p_तर्काः->source;
    t=p_तर्काः->target;
    offsets=p_तर्काः->offsets;

    /* get the converter implementation function */
    source_सूचकांक=0;
    यदि(offsets==शून्यसूचकः) {
        गन्तव्यम्_Unicode=cnv->shared_दत्त->impl->गन्तव्यम्_Unicode;
    } अन्यथा {
        गन्तव्यम्_Unicode=cnv->shared_दत्त->impl->गन्तव्यम्_Unicode_With_Offsets;
        यदि(गन्तव्यम्_Unicode==शून्यसूचकः) {
            /* there is no WithOffsets implementation */
            गन्तव्यम्_Unicode=cnv->shared_दत्त->impl->गन्तव्यम्_Unicode;
            /* we will write -1 for each offset */
            source_सूचकांक=-1;
        }
    }

    यदि(cnv->pre_गन्तव्यम्_ULength>=0) {
        /* normal mode */
        realSource=शून्यसूचकः;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        real_Source_सीमा=शून्यसूचकः;
        realFlush=असत्य;
        real_Source_सूचकांक=0;
    } अन्यथा {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        यदि (!p_तर्काः->source) {
            *err=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
        realSource=p_तर्काः->source;
        real_Source_सीमा=p_तर्काः->source_सीमा;
        realFlush=p_तर्काः->flush;
        real_Source_सूचकांक=source_सूचकांक;

        uprv_memcpy(replay, cnv->pre_गन्तव्यम्_U, -cnv->pre_गन्तव्यम्_ULength);
        p_तर्काः->source=replay;
        p_तर्काः->source_सीमा=replay-cnv->pre_गन्तव्यम्_ULength;
        p_तर्काः->flush=असत्य;
        source_सूचकांक=-1;

        cnv->pre_गन्तव्यम्_ULength=0;
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
        यदि(U_SUCCESS(*err)) {
            /* convert */
            गन्तव्यम्_Unicode(p_तर्काः, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converter_Saw_अन्तः_सम्बन्धि_निवेशः=
                static_cast<UBool>(U_SUCCESS(*err) &&
                        p_तर्काः->flush && p_तर्काः->source==p_तर्काः->source_सीमा &&
                        cnv->गन्तव्यम्_ULength==0);
        } अन्यथा {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converter_Saw_अन्तः_सम्बन्धि_निवेशः=असत्य;
        }

        /* no callback called yet for this iteration */
        calledCallback=असत्य;

        /* no sourceIndex adjustment for conversion, only for callback output */
        error_निवेशः_दीर्घता=0;

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
            यदि(offsets!=शून्यसूचकः) {
                int32_t length = static_cast<int32_t>(p_तर्काः->target - t);
                यदि(length>0) {
                    _updateOffsets(offsets, length, source_सूचकांक, error_निवेशः_दीर्घता);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    p_तर्काः->offsets=offsets+=length;
                }

                यदि(source_सूचकांक>=0) {
                    source_सूचकांक += static_cast<int32_t>(p_तर्काः->source - s);
                }
            }

            यदि(cnv->pre_गन्तव्यम्_ULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                यदि(realSource==शून्यसूचकः) {
                    realSource=p_तर्काः->source;
                    real_Source_सीमा=p_तर्काः->source_सीमा;
                    realFlush=p_तर्काः->flush;
                    real_Source_सूचकांक=source_सूचकांक;

                    uprv_memcpy(replay, cnv->pre_गन्तव्यम्_U, -cnv->pre_गन्तव्यम्_ULength);
                    p_तर्काः->source=replay;
                    p_तर्काः->source_सीमा=replay-cnv->pre_गन्तव्यम्_ULength;
                    p_तर्काः->flush=असत्य;
                    यदि((source_सूचकांक+=cnv->pre_गन्तव्यम्_ULength)<0) {
                        source_सूचकांक=-1;
                    }

                    cnv->pre_गन्तव्यम्_ULength=0;
                } अन्यथा {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==शून्यसूचकः);
                    *err=U_आन्तरिकम्_क्रमादेशः_ERROR;
                }
            }

            /* update pointers */
            s=p_तर्काः->source;
            t=p_तर्काः->target;

            यदि(U_SUCCESS(*err)) {
                यदि(s<p_तर्काः->source_सीमा) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    भङ्ग;
                } अन्यथा यदि(realSource!=शून्यसूचकः) {
                    /* switch back from replaying to the real source and continue */
                    p_तर्काः->source=realSource;
                    p_तर्काः->source_सीमा=real_Source_सीमा;
                    p_तर्काः->flush=realFlush;
                    source_सूचकांक=real_Source_सूचकांक;

                    realSource=शून्यसूचकः;
                    भङ्ग;
                } अन्यथा यदि(p_तर्काः->flush && cnv->गन्तव्यम्_ULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_प्राप्तम्;
                    calledCallback=असत्य; /* new error condition */
                } अन्यथा {
                    /* input consumed */
                    यदि(p_तर्काः->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        यदि(!converter_Saw_अन्तः_सम्बन्धि_निवेशः) {
                            भङ्ग;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_गन्तव्यम्_UNICODE, असत्य);
                    }

                    /* done successfully */
                    प्रत्यागम;
                }
            }

            /* U_FAILURE(*err) */
            {
                UError_संहिता e;

                यदि( calledCallback ||
                    (e=*err)==U_बफर_अतिप्रवाहः_ERROR ||
                    (e!=U_INVALID_CHAR_प्राप्तम् &&
                     e!=U_ILLEGAL_CHAR_प्राप्तम् &&
                     e!=U_TRUNCATED_CHAR_प्राप्तम् &&
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
                    यदि(realSource!=शून्यसूचकः) {
                        int32_t length;

                        U_ASSERT(cnv->pre_गन्तव्यम्_ULength==0);

                        length = static_cast<int32_t>(p_तर्काः->source_सीमा - p_तर्काः->source);
                        यदि(length>0) {
                            uprv_memcpy(cnv->pre_गन्तव्यम्_U, p_तर्काः->source, length);
                            cnv->pre_गन्तव्यम्_ULength = static_cast<int8_t>(-length);
                        }

                        p_तर्काः->source=realSource;
                        p_तर्काः->source_सीमा=real_Source_सीमा;
                        p_तर्काः->flush=realFlush;
                    }

                    प्रत्यागम;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            error_निवेशः_दीर्घता=cnv->invalid_Char_दीर्घता=cnv->गन्तव्यम्_ULength;
            यदि(error_निवेशः_दीर्घता>0) {
                uprv_memcpy(cnv->invalid_Char_बफर, cnv->गन्तव्यम्_UBytes, error_निवेशः_दीर्घता);
            }

            /* set the converter state to deal with the next character */
            cnv->गन्तव्यम्_ULength=0;

            /* call the callback function */
            यदि(cnv->गन्तव्यम्_UCallback_Reason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_प्राप्तम्) {
                cnv->गन्तव्यम्_UCallback_Reason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->गन्तव्यम्_UContext, p_तर्काः,
                cnv->invalid_Char_बफर, error_निवेशः_दीर्घता,
                cnv->गन्तव्यम्_UCallback_Reason,
                err);
            cnv->गन्तव्यम्_UCallback_Reason = UCNV_ILLEGAL; /* reset to default value */

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=सत्य;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return true if overflow
 */
स्थिर UBool
ucnv_output_अतिप्रवाहः_गन्तव्यम्_Unicode(UConverter *cnv,
                             char16_t **target, ध्रुव char16_t *target_सीमा,
                             int32_t **pOffsets,
                             UError_संहिता *err) {
    int32_t *offsets;
    char16_t *अतिप्रवाहः, *t;
    int32_t i, length;

    t=*target;
    यदि(pOffsets!=शून्यसूचकः) {
        offsets=*pOffsets;
    } अन्यथा {
        offsets=शून्यसूचकः;
    }

    अतिप्रवाहः=cnv->UChar_Error_बफर;
    length=cnv->UChar_Error_बफर_दीर्घता;
    i=0;
    यावत्(i<length) {
        यदि(t==target_सीमा) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            कुरु {
                अतिप्रवाहः[j++]=अतिप्रवाहः[i++];
            } यावत्(i<length);

            cnv->UChar_Error_बफर_दीर्घता = static_cast<int8_t>(j);
            *target=t;
            यदि(offsets!=शून्यसूचकः) {
                *pOffsets=offsets;
            }
            *err=U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम सत्य;
        }

        /* copy the overflow contents to the target */
        *t++=अतिप्रवाहः[i++];
        यदि(offsets!=शून्यसूचकः) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UChar_Error_बफर_दीर्घता=0;
    *target=t;
    यदि(offsets!=शून्यसूचकः) {
        *pOffsets=offsets;
    }
    प्रत्यागम असत्य;
}

U_CAPI शून्य U_EXPORT2
ucnv_गन्तव्यम्_Unicode(UConverter *cnv,
               char16_t **target, ध्रुव char16_t *target_सीमा,
               ध्रुव वर्ण **source, ध्रुव वर्ण *source_सीमा,
               int32_t *offsets,
               UBool flush,
               UError_संहिता *err) {
    UConverter_गन्तव्यम्_Unicode_तर्काः तर्काः_2;
    ध्रुव वर्ण *s;
    char16_t *t;

    /* check parameters */
    यदि(err==शून्यसूचकः || U_FAILURE(*err)) {
        प्रत्यागम;
    }

    यदि(cnv==शून्यसूचकः || target==शून्यसूचकः || source==शून्यसूचकः) {
        *err=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    s=*source;
    t=*target;

    यदि ((ध्रुव शून्य *)U_MAX_PTR(target_सीमा) == (ध्रुव शून्य *)target_सीमा) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a char16_t * boundary.
        This also prevents the next argument check from failing.
        */
        target_सीमा = (ध्रुव char16_t *)(((ध्रुव वर्ण *)target_सीमा) - 1);
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
    यदि (source_सीमा<s || target_सीमा<t ||
        ((size_t)(source_सीमा-s)>(size_t)0x7fffffff && source_सीमा>s) ||
        ((size_t)(target_सीमा-t)>(size_t)0x3fffffff && target_सीमा>t) ||
        (((ध्रुव वर्ण *)target_सीमा-(ध्रुव वर्ण *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    
    /* output the target overflow buffer */
    यदि( cnv->UChar_Error_बफर_दीर्घता>0 &&
        ucnv_output_अतिप्रवाहः_गन्तव्यम्_Unicode(cnv, target, target_सीमा, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        प्रत्यागम;
    }
    /* *target may have moved, therefore stop using t */

    यदि(!flush && s==source_सीमा && cnv->pre_गन्तव्यम्_ULength>=0) {
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
    तर्काः_2.converter=cnv;
    तर्काः_2.flush=flush;
    तर्काः_2.offsets=offsets;
    तर्काः_2.source=s;
    तर्काः_2.source_सीमा=source_सीमा;
    तर्काः_2.target=*target;
    तर्काः_2.target_सीमा=target_सीमा;
    तर्काः_2.size=परिमाण(तर्काः_2);

    गन्तव्यम्_Unicode_With_Callback(&तर्काः_2, err);

    *source=तर्काः_2.source;
    *target=तर्काः_2.target;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                वर्ण *dest, int32_t dest_क्षमता,
                ध्रुव char16_t *src, int32_t src_दीर्घता,
                UError_संहिता *p_Error_संहिता) {
    ध्रुव char16_t *src_सीमा;
    वर्ण *originalDest, *dest_सीमा;
    int32_t dest_दीर्घता;

    /* check arguments */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि( cnv==शून्यसूचकः ||
        dest_क्षमता<0 || (dest_क्षमता>0 && dest==शून्यसूचकः) ||
        src_दीर्घता<-1 || (src_दीर्घता!=0 && src==शून्यसूचकः)
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    यदि(src_दीर्घता==-1) {
        src_दीर्घता=u_strlen(src);
    }
    यदि(src_दीर्घता>0) {
        src_सीमा=src+src_दीर्घता;
        dest_क्षमता=pin_क्षमता(dest, dest_क्षमता);
        dest_सीमा=dest+dest_क्षमता;

        /* perform the conversion */
        UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
        ucnv_fromUnicode(cnv, &dest, dest_सीमा, &src, src_सीमा, शून्यसूचकः, सत्य, &बफर_स्थितिः);
        dest_दीर्घता=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        यदि(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR) {
            वर्ण बफर[1024];

            dest_सीमा=बफर+परिमाण(बफर);
            कुरु {
                dest=बफर;
                बफर_स्थितिः=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, dest_सीमा, &src, src_सीमा, शून्यसूचकः, सत्य, &बफर_स्थितिः);
                dest_दीर्घता+=(int32_t)(dest-बफर);
            } यावत्(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR);
        }
        यदि (U_FAILURE(बफर_स्थितिः)) {
            *p_Error_संहिता = बफर_स्थितिः;
        }
    } अन्यथा {
        dest_दीर्घता=0;
    }

    प्रत्यागम u_terminateChars(originalDest, dest_क्षमता, dest_दीर्घता, p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
ucnv_गन्तव्यम्_UChars(UConverter *cnv,
              char16_t *dest, int32_t dest_क्षमता,
              ध्रुव वर्ण *src, int32_t src_दीर्घता,
              UError_संहिता *p_Error_संहिता) {
    ध्रुव वर्ण *src_सीमा;
    char16_t *originalDest, *dest_सीमा;
    int32_t dest_दीर्घता;

    /* check arguments */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि( cnv==शून्यसूचकः ||
        dest_क्षमता<0 || (dest_क्षमता>0 && dest==शून्यसूचकः) ||
        src_दीर्घता<-1 || (src_दीर्घता!=0 && src==शून्यसूचकः))
    {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* initialize */
    ucnv_reset_गन्तव्यम्_Unicode(cnv);
    originalDest=dest;
    यदि(src_दीर्घता==-1) {
        src_दीर्घता=(int32_t)uprv_strlen(src);
    }
    यदि(src_दीर्घता>0) {
        src_सीमा=src+src_दीर्घता;
        dest_क्षमता=pin_क्षमता(dest, dest_क्षमता);
        dest_सीमा=dest+dest_क्षमता;

        /* perform the conversion */
        UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
        ucnv_गन्तव्यम्_Unicode(cnv, &dest, dest_सीमा, &src, src_सीमा, शून्यसूचकः, सत्य, &बफर_स्थितिः);
        dest_दीर्घता=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        यदि(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR)
        {
            char16_t बफर[1024];

            dest_सीमा=बफर+UPRV_LENGTHOF(बफर);
            कुरु {
                dest=बफर;
                बफर_स्थितिः=U_ZERO_ERROR;
                ucnv_गन्तव्यम्_Unicode(cnv, &dest, dest_सीमा, &src, src_सीमा, शून्यसूचकः, सत्य, &बफर_स्थितिः);
                dest_दीर्घता+=(int32_t)(dest-बफर);
            }
            यावत्(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR);
        }
        यदि (U_FAILURE(बफर_स्थितिः)) {
            *p_Error_संहिता = बफर_स्थितिः;
        }
    } अन्यथा {
        dest_दीर्घता=0;
    }

    प्रत्यागम u_terminateUChars(originalDest, dest_क्षमता, dest_दीर्घता, p_Error_संहिता);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_get_अग्र_UChar(UConverter *cnv,
                  ध्रुव वर्ण **source, ध्रुव वर्ण *source_सीमा,
                  UError_संहिता *err) {
    UConverter_गन्तव्यम्_Unicode_तर्काः तर्काः_2;
    char16_t बफर[U16_MAX_दीर्घता];
    ध्रुव वर्ण *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    यदि(err==शून्यसूचकः || U_FAILURE(*err)) {
        प्रत्यागम 0xffff;
    }

    यदि(cnv==शून्यसूचकः || source==शून्यसूचकः) {
        *err=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0xffff;
    }

    s=*source;
    यदि(source_सीमा<s) {
        *err=U_ILLEGAL_तर्कः_ERROR;
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
    यदि(((size_t)(source_सीमा-s)>(size_t)0x7fffffff && source_सीमा>s)) {
        *err=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    यदि(cnv->UChar_Error_बफर_दीर्घता>0) {
        char16_t *अतिप्रवाहः;

        अतिप्रवाहः=cnv->UChar_Error_बफर;
        i=0;
        length=cnv->UChar_Error_बफर_दीर्घता;
        U16_अग्र(अतिप्रवाहः, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        यदि((cnv->UChar_Error_बफर_दीर्घता=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UChar_Error_बफर, cnv->UChar_Error_बफर+i,
                         cnv->UChar_Error_बफर_दीर्घता*U_SIZEOF_UCHAR);
        }

        यदि(!U16_IS_LEAD(c) || i<length) {
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
    तर्काः_2.converter=cnv;
    तर्काः_2.flush=सत्य;
    तर्काः_2.offsets=शून्यसूचकः;
    तर्काः_2.source=s;
    तर्काः_2.source_सीमा=source_सीमा;
    तर्काः_2.target=बफर;
    तर्काः_2.target_सीमा=बफर+1;
    तर्काः_2.size=परिमाण(तर्काः_2);

    यदि(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        यदि(cnv->गन्तव्यम्_ULength==0 && cnv->shared_दत्त->impl->get_अग्र_UChar!=शून्यसूचकः) {
            c=cnv->shared_दत्त->impl->get_अग्र_UChar(&तर्काः_2, err);
            *source=s=तर्काः_2.source;
            यदि(*err==U_सूचकांक_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_गन्तव्यम्_UNICODE, असत्य);
                प्रत्यागम 0xffff; /* no output */
            } अन्यथा यदि(U_SUCCESS(*err) && c>=0) {
                प्रत्यागम c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one char16_t in buffer[0], or handle getNextUChar() errors */
        गन्तव्यम्_Unicode_With_Callback(&तर्काः_2, err);

        यदि(*err==U_बफर_अतिप्रवाहः_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(तर्काः_2.target-बफर);
    } अन्यथा {
        /* write the lead surrogate from the overflow buffer */
        बफर[0]=(char16_t)c;
        तर्काः_2.target=बफर+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    यदि(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } अन्यथा यदि(length==0) {
        /* no input or only state changes */
        *err=U_सूचकांक_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } अन्यथा {
        c=बफर[0];
        i=1;
        यदि(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } अन्यथा {
            /* got a lead surrogate, see if a trail surrogate follows */
            char16_t c2;

            यदि(cnv->UChar_Error_बफर_दीर्घता>0) {
                /* got overflow output from the conversion */
                यदि(U16_IS_TRAIL(c2=cnv->UChar_Error_बफर[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    यदि((--cnv->UChar_Error_बफर_दीर्घता)>0) {
                        uprv_memmove(cnv->UChar_Error_बफर, cnv->UChar_Error_बफर+1,
                                     cnv->UChar_Error_बफर_दीर्घता*U_SIZEOF_UCHAR);
                    }
                } अन्यथा {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } अन्यथा यदि(तर्काः_2.source<source_सीमा) {
                /* convert once more, to buffer[1] */
                तर्काः_2.target_सीमा=बफर+2;
                गन्तव्यम्_Unicode_With_Callback(&तर्काः_2, err);
                यदि(*err==U_बफर_अतिप्रवाहः_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(तर्काः_2.target-बफर);
                यदि(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=बफर[1])) {
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
    यदि(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        यदि((length=cnv->UChar_Error_बफर_दीर्घता)>0) {
            uprv_memmove(cnv->UChar_Error_बफर+delta, cnv->UChar_Error_बफर,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UChar_Error_बफर_दीर्घता=(int8_t)(length+delta);

        cnv->UChar_Error_बफर[0]=बफर[i++];
        यदि(delta>1) {
            cnv->UChar_Error_बफर[1]=बफर[i];
        }
    }

    *source=तर्काः_2.source;
    प्रत्यागम c;
}

/* ucnv_convert() and siblings ---------------------------------------------- */

U_CAPI शून्य U_EXPORT2
ucnv_convertEx(UConverter *targetCnv, UConverter *sourceCnv,
               वर्ण **target, ध्रुव वर्ण *target_सीमा,
               ध्रुव वर्ण **source, ध्रुव वर्ण *source_सीमा,
               char16_t *pivotStart, char16_t **pivotSource,
               char16_t **pivotTarget, ध्रुव char16_t *pivot_सीमा,
               UBool reset, UBool flush,
               UError_संहिता *p_Error_संहिता) {
    char16_t pivot_बफर[CHUNK_आकार];
    ध्रुव char16_t *myPivotSource;
    char16_t *myPivotTarget;
    ध्रुव वर्ण *s;
    वर्ण *t;

    UConverter_गन्तव्यम्_Unicode_तर्काः गन्तव्यम्_UArgs;
    UConverter_From_Unicode_तर्काः fromUArgs;
    UConverterConvert convert;

    /* error checking */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }

    यदि( targetCnv==शून्यसूचकः || sourceCnv==शून्यसूचकः ||
        source==शून्यसूचकः || *source==शून्यसूचकः ||
        target==शून्यसूचकः || *target==शून्यसूचकः || target_सीमा==शून्यसूचकः
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    s=*source;
    t=*target;
    यदि((source_सीमा!=शून्यसूचकः && source_सीमा<s) || target_सीमा<t) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t. See ucnv_toUnicode() for a more detailed comment.
     */
    यदि(
        (source_सीमा!=शून्यसूचकः && ((size_t)(source_सीमा-s)>(size_t)0x7fffffff && source_सीमा>s)) ||
        ((size_t)(target_सीमा-t)>(size_t)0x7fffffff && target_सीमा>t)
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    
    यदि(pivotStart==शून्यसूचकः) {
        यदि(!flush) {
            /* streaming conversion requires an explicit pivot buffer */
            *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }

        /* use the stack pivot buffer */
        myPivotSource=myPivotTarget=pivotStart=pivot_बफर;
        pivotSource=(char16_t **)&myPivotSource;
        pivotTarget=&myPivotTarget;
        pivot_सीमा=pivot_बफर+CHUNK_आकार;
    } अन्यथा यदि(  pivotStart>=pivot_सीमा ||
                pivotSource==शून्यसूचकः || *pivotSource==शून्यसूचकः ||
                pivotTarget==शून्यसूचकः || *pivotTarget==शून्यसूचकः ||
                pivot_सीमा==शून्यसूचकः
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    यदि(source_सीमा==शून्यसूचकः) {
        /* get limit of single-byte-NUL-terminated source string */
        source_सीमा=uprv_strchr(*source, 0);
    }

    यदि(reset) {
        ucnv_reset_गन्तव्यम्_Unicode(sourceCnv);
        ucnv_resetFromUnicode(targetCnv);
        *pivotSource=*pivotTarget=pivotStart;
    } अन्यथा यदि(targetCnv->char_Error_बफर_दीर्घता>0) {
        /* output the targetCnv overflow buffer */
        यदि(ucnv_output_अतिप्रवाहः_From_Unicode(targetCnv, target, target_सीमा, शून्यसूचकः, p_Error_संहिता)) {
            /* U_BUFFER_OVERFLOW_ERROR */
            प्रत्यागम;
        }
        /* *target has moved, therefore stop using t */

        यदि( !flush &&
            targetCnv->preFromULength>=0 && *pivotSource==*pivotTarget &&
            sourceCnv->UChar_Error_बफर_दीर्घता==0 && sourceCnv->pre_गन्तव्यम्_ULength>=0 && s==source_सीमा
        ) {
            /* the fromUnicode overflow buffer is emptied and there is no new input: we are done */
            प्रत्यागम;
        }
    }

    /* Is direct-UTF-8 conversion available? */
    यदि( sourceCnv->shared_दत्त->static_दत्त->conversionType==UCNV_UTF8 &&
        targetCnv->shared_दत्त->impl->fromUTF8!=शून्यसूचकः
    ) {
        convert=targetCnv->shared_दत्त->impl->fromUTF8;
    } अन्यथा यदि( targetCnv->shared_दत्त->static_दत्त->conversionType==UCNV_UTF8 &&
               sourceCnv->shared_दत्त->impl->गन्तव्यम्_UTF8!=शून्यसूचकः
    ) {
        convert=sourceCnv->shared_दत्त->impl->गन्तव्यम्_UTF8;
    } अन्यथा {
        convert=शून्यसूचकः;
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
    यदि(convert!=शून्यसूचकः && (pivot_सीमा-pivotStart)>32) {
        pivot_सीमा=pivotStart+32;
    }

    /* prepare the converter arguments */
    fromUArgs.converter=targetCnv;
    fromUArgs.flush=असत्य;
    fromUArgs.offsets=शून्यसूचकः;
    fromUArgs.target=*target;
    fromUArgs.target_सीमा=target_सीमा;
    fromUArgs.size=परिमाण(fromUArgs);

    गन्तव्यम्_UArgs.converter=sourceCnv;
    गन्तव्यम्_UArgs.flush=flush;
    गन्तव्यम्_UArgs.offsets=शून्यसूचकः;
    गन्तव्यम्_UArgs.source=s;
    गन्तव्यम्_UArgs.source_सीमा=source_सीमा;
    गन्तव्यम्_UArgs.target_सीमा=pivot_सीमा;
    गन्तव्यम्_UArgs.size=परिमाण(गन्तव्यम्_UArgs);

    /*
     * TODO: Consider separating this function into two functions,
     * extracting exactly the conversion loop,
     * for readability and to reduce the set of visible variables.
     *
     * Otherwise stop using s and t from here on.
     */
    s=t=शून्यसूचकः;

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
        यदि( *pivotSource<*pivotTarget ||
            U_FAILURE(*p_Error_संहिता) ||
            targetCnv->preFromULength<0 ||
            fromUArgs.flush
        ) {
            fromUArgs.source=*pivotSource;
            fromUArgs.source_सीमा=*pivotTarget;
            _fromUnicodeWithCallback(&fromUArgs, p_Error_संहिता);
            यदि(U_FAILURE(*p_Error_संहिता)) {
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
        यदि(sourceCnv->UChar_Error_बफर_दीर्घता>0) {
            यदि(ucnv_output_अतिप्रवाहः_गन्तव्यम्_Unicode(sourceCnv, pivotTarget, pivot_सीमा, शून्यसूचकः, p_Error_संहिता)) {
                /* U_BUFFER_OVERFLOW_ERROR */
                *p_Error_संहिता=U_ZERO_ERROR;
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
        यदि( गन्तव्यम्_UArgs.source==source_सीमा &&
            sourceCnv->pre_गन्तव्यम्_ULength>=0 && sourceCnv->गन्तव्यम्_ULength==0 &&
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
        यदि(convert!=शून्यसूचकः && targetCnv->preFromUFirstCP<0 && sourceCnv->pre_गन्तव्यम्_ULength==0) {
            यदि(*p_Error_संहिता==U_USING_DEFAULT_WARNING) {
                /* remove a warning that may be set by this function */
                *p_Error_संहिता=U_ZERO_ERROR;
            }
            convert(&fromUArgs, &गन्तव्यम्_UArgs, p_Error_संहिता);
            यदि(*p_Error_संहिता==U_बफर_अतिप्रवाहः_ERROR) {
                भङ्ग;
            } अन्यथा यदि(U_FAILURE(*p_Error_संहिता)) {
                यदि(sourceCnv->गन्तव्यम्_ULength>0) {
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
                    अनुवर्त;
                }
            } अन्यथा यदि(*p_Error_संहिता==U_USING_DEFAULT_WARNING) {
                /*
                 * No error, but the implementation requested to temporarily
                 * fall back to pivoting.
                 */
                *p_Error_संहिता=U_ZERO_ERROR;
            /*
             * The following else branches are almost identical to the end-of-input
             * handling in _toUnicodeWithCallback().
             * Avoid calling it just for the end of input.
             */
            } अन्यथा यदि(flush && sourceCnv->गन्तव्यम्_ULength>0) { /* flush==toUArgs.flush */
                /*
                 * the entire input stream is consumed
                 * and there is a partial, truncated input sequence left
                 */

                /* inject an error and continue with callback handling */
                *p_Error_संहिता=U_TRUNCATED_CHAR_प्राप्तम्;
            } अन्यथा {
                /* input consumed */
                यदि(flush) {
                    /* reset the converters without calling the callback functions */
                    _reset(sourceCnv, UCNV_RESET_गन्तव्यम्_UNICODE, असत्य);
                    _reset(targetCnv, UCNV_RESET_FROM_UNICODE, असत्य);
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
        गन्तव्यम्_UArgs.target=pivotStart; /* ==*pivotTarget */
        /* toUArgs.targetLimit=pivotLimit; already set before the loop */
        गन्तव्यम्_Unicode_With_Callback(&गन्तव्यम्_UArgs, p_Error_संहिता);
        *pivotTarget=गन्तव्यम्_UArgs.target;
        यदि(*p_Error_संहिता==U_बफर_अतिप्रवाहः_ERROR) {
            /* pivot overflow: continue with the conversion loop */
            *p_Error_संहिता=U_ZERO_ERROR;
        } अन्यथा यदि(U_FAILURE(*p_Error_संहिता) || (!flush && *pivotTarget==pivotStart)) {
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
        यदि( flush && गन्तव्यम्_UArgs.source==source_सीमा &&
            sourceCnv->pre_गन्तव्यम्_ULength>=0 &&
            sourceCnv->UChar_Error_बफर_दीर्घता==0
        ) {
            fromUArgs.flush=सत्य;
        }
    }

    /*
     * The conversion loop is exited when one of the following is true:
     * - the entire source text has been converted successfully to the target buffer
     * - a target buffer overflow occurred
     * - a conversion error occurred
     */

    *source=गन्तव्यम्_UArgs.source;
    *target=fromUArgs.target;

    /* terminate the target buffer if possible */
    यदि(flush && U_SUCCESS(*p_Error_संहिता)) {
        यदि(*target!=target_सीमा) {
            **target=0;
            यदि(*p_Error_संहिता==U_STRING_NOT_TERMINATED_WARNING) {
                *p_Error_संहिता=U_ZERO_ERROR;
            }
        } अन्यथा {
            *p_Error_संहिता=U_STRING_NOT_TERMINATED_WARNING;
        }
    }
}

/* internal implementation of ucnv_convert() etc. with preflighting */
स्थिर int32_t
ucnv_आन्तरिकम्_Convert(UConverter *बहिर्गमनम्_Converter, UConverter *अन्तर्_Converter,
                     वर्ण *target, int32_t target_क्षमता,
                     ध्रुव वर्ण *source, int32_t source_दीर्घता,
                     UError_संहिता *p_Error_संहिता) {
    char16_t pivot_बफर[CHUNK_आकार];
    char16_t *pivot, *pivot2;

    वर्ण *myTarget;
    ध्रुव वर्ण *source_सीमा;
    ध्रुव वर्ण *target_सीमा;
    int32_t target_दीर्घता=0;

    /* set up */
    यदि(source_दीर्घता<0) {
        source_सीमा=uprv_strchr(source, 0);
    } अन्यथा {
        source_सीमा=source+source_दीर्घता;
    }

    /* if there is no input data, we're done */
    यदि(source==source_सीमा) {
        प्रत्यागम u_terminateChars(target, target_क्षमता, 0, p_Error_संहिता);
    }

    pivot=pivot2=pivot_बफर;
    myTarget=target;
    target_दीर्घता=0;

    यदि(target_क्षमता>0) {
        /* perform real conversion */
        target_सीमा=target+target_क्षमता;
        ucnv_convertEx(बहिर्गमनम्_Converter, अन्तर्_Converter,
                       &myTarget, target_सीमा,
                       &source, source_सीमा,
                       pivot_बफर, &pivot, &pivot2, pivot_बफर+CHUNK_आकार,
                       असत्य,
                       सत्य,
                       p_Error_संहिता);
        target_दीर्घता = static_cast<int32_t>(myTarget - target);
    }

    /*
     * If the output buffer is exhausted (or we are only "preflighting"), we need to stop writing
     * to it but continue the conversion in order to store in targetCapacity
     * the number of bytes that was required.
     */
    यदि(*p_Error_संहिता==U_बफर_अतिप्रवाहः_ERROR || target_क्षमता==0)
    {
        वर्ण target_बफर[CHUNK_आकार];

        target_सीमा=target_बफर+CHUNK_आकार;
        कुरु {
            *p_Error_संहिता=U_ZERO_ERROR;
            myTarget=target_बफर;
            ucnv_convertEx(बहिर्गमनम्_Converter, अन्तर्_Converter,
                           &myTarget, target_सीमा,
                           &source, source_सीमा,
                           pivot_बफर, &pivot, &pivot2, pivot_बफर+CHUNK_आकार,
                           असत्य,
                           सत्य,
                           p_Error_संहिता);
            target_दीर्घता += static_cast<int32_t>(myTarget - target_बफर);
        } यावत्(*p_Error_संहिता==U_बफर_अतिप्रवाहः_ERROR);

        /* done with preflighting, set warnings and errors as appropriate */
        प्रत्यागम u_terminateChars(target, target_क्षमता, target_दीर्घता, p_Error_संहिता);
    }

    /* no need to call u_terminateChars() because ucnv_convertEx() took care of that */
    प्रत्यागम target_दीर्घता;
}

U_CAPI int32_t U_EXPORT2
ucnv_convert(ध्रुव वर्ण *गन्तव्यम्_Converter_Name, ध्रुव वर्ण *fromConverterName,
             वर्ण *target, int32_t target_क्षमता,
             ध्रुव वर्ण *source, int32_t source_दीर्घता,
             UError_संहिता *p_Error_संहिता) {
    UConverter in, out; /* stack-allocated */
    UConverter *अन्तर्_Converter, *बहिर्गमनम्_Converter;
    int32_t target_दीर्घता;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि( source==शून्यसूचकः || source_दीर्घता<-1 ||
        target_क्षमता<0 || (target_क्षमता>0 && target==शून्यसूचकः)
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* if there is no input data, we're done */
    यदि(source_दीर्घता==0 || (source_दीर्घता<0 && *source==0)) {
        प्रत्यागम u_terminateChars(target, target_क्षमता, 0, p_Error_संहिता);
    }

    /* create the converters */
    अन्तर्_Converter=ucnv_createConverter(&in, fromConverterName, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    बहिर्गमनम्_Converter=ucnv_createConverter(&out, गन्तव्यम्_Converter_Name, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        ucnv_close(अन्तर्_Converter);
        प्रत्यागम 0;
    }

    target_दीर्घता=ucnv_आन्तरिकम्_Convert(बहिर्गमनम्_Converter, अन्तर्_Converter,
                                      target, target_क्षमता,
                                      source, source_दीर्घता,
                                      p_Error_संहिता);

    ucnv_close(अन्तर्_Converter);
    ucnv_close(बहिर्गमनम्_Converter);

    प्रत्यागम target_दीर्घता;
}

/* @internal */
स्थिर int32_t
ucnv_convertAlgorithmic(UBool convert_गन्तव्यम्_Algorithmic,
                        UConverterType algorithmicType,
                        UConverter *cnv,
                        वर्ण *target, int32_t target_क्षमता,
                        ध्रुव वर्ण *source, int32_t source_दीर्घता,
                        UError_संहिता *p_Error_संहिता) {
    UConverter algoConverterStatic; /* stack-allocated */
    UConverter *algoConverter, *गन्तव्यम्, *from;
    int32_t target_दीर्घता;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि( cnv==शून्यसूचकः || source==शून्यसूचकः || source_दीर्घता<-1 ||
        target_क्षमता<0 || (target_क्षमता>0 && target==शून्यसूचकः)
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* if there is no input data, we're done */
    यदि(source_दीर्घता==0 || (source_दीर्घता<0 && *source==0)) {
        प्रत्यागम u_terminateChars(target, target_क्षमता, 0, p_Error_संहिता);
    }

    /* create the algorithmic converter */
    algoConverter=ucnv_createAlgorithmicConverter(&algoConverterStatic, algorithmicType,
                                                  "", 0, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* reset the other converter */
    यदि(convert_गन्तव्यम्_Algorithmic) {
        /* cnv->Unicode->algo */
        ucnv_reset_गन्तव्यम्_Unicode(cnv);
        गन्तव्यम्=algoConverter;
        from=cnv;
    } अन्यथा {
        /* algo->Unicode->cnv */
        ucnv_resetFromUnicode(cnv);
        from=algoConverter;
        गन्तव्यम्=cnv;
    }

    target_दीर्घता=ucnv_आन्तरिकम्_Convert(गन्तव्यम्, from,
                                      target, target_क्षमता,
                                      source, source_दीर्घता,
                                      p_Error_संहिता);

    ucnv_close(algoConverter);

    प्रत्यागम target_दीर्घता;
}

U_CAPI int32_t U_EXPORT2
ucnv_गन्तव्यम्_Algorithmic(UConverterType algorithmicType,
                   UConverter *cnv,
                   वर्ण *target, int32_t target_क्षमता,
                   ध्रुव वर्ण *source, int32_t source_दीर्घता,
                   UError_संहिता *p_Error_संहिता) {
    प्रत्यागम ucnv_convertAlgorithmic(सत्य, algorithmicType, cnv,
                                   target, target_क्षमता,
                                   source, source_दीर्घता,
                                   p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
ucnv_fromAlgorithmic(UConverter *cnv,
                     UConverterType algorithmicType,
                     वर्ण *target, int32_t target_क्षमता,
                     ध्रुव वर्ण *source, int32_t source_दीर्घता,
                     UError_संहिता *p_Error_संहिता) UPRV_NO_SANITIZE_UNDEFINED {

    यदि(algorithmicType<0 || UCNV_सङ्ख्या_सम्बन्धि_SUPPORTED_CONVERTER_TYPES<=algorithmicType) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    प्रत्यागम ucnv_convertAlgorithmic(असत्य, algorithmicType, cnv,
                                   target, target_क्षमता,
                                   source, source_दीर्घता,
                                   p_Error_संहिता);
}

U_CAPI UConverterType  U_EXPORT2
ucnv_getType(ध्रुव UConverter* converter)
{
    int8_t type = converter->shared_दत्त->static_दत्त->conversionType;
#यदि !UCONFIG_NO_LEGACY_CONVERSION
    यदि(type == UCNV_MBCS) {
        प्रत्यागम ucnv_MBCSGetType(converter);
    }
#यदि_अन्तः
    प्रत्यागम (UConverterType)type;
}

U_CAPI शून्य  U_EXPORT2
ucnv_getStarters(ध्रुव UConverter* converter, 
                 UBool starters[256],
                 UError_संहिता* err)
{
    यदि (err == शून्यसूचकः || U_FAILURE(*err)) {
        प्रत्यागम;
    }

    यदि(converter->shared_दत्त->impl->getStarters != शून्यसूचकः) {
        converter->shared_दत्त->impl->getStarters(converter, starters, err);
    } अन्यथा {
        *err = U_ILLEGAL_तर्कः_ERROR;
    }
}

स्थिर ध्रुव UAmbiguousConverter *ucnv_getAmbiguous(ध्रुव UConverter *cnv)
{
    UError_संहिता error_संहिता;
    ध्रुव वर्ण *name;
    int32_t i;

    यदि(cnv==शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }

    error_संहिता=U_ZERO_ERROR;
    name=ucnv_getName(cnv, &error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    प्रति(i=0; i<UPRV_LENGTHOF(ambiguousConverters); ++i)
    {
        यदि(0==uprv_strcmp(name, ambiguousConverters[i].name))
        {
            प्रत्यागम ambiguousConverters+i;
        }
    }

    प्रत्यागम शून्यसूचकः;
}

U_CAPI शून्य  U_EXPORT2
ucnv_fixFileSeparator(ध्रुव UConverter *cnv, 
                      char16_t* source,
                      int32_t source_दीर्घता) {
    ध्रुव UAmbiguousConverter *a;
    int32_t i;
    char16_t variant5c;

    यदि(cnv==शून्यसूचकः || source==शून्यसूचकः || source_दीर्घता<=0 || (a=ucnv_getAmbiguous(cnv))==शून्यसूचकः)
    {
        प्रत्यागम;
    }

    variant5c=a->variant5c;
    प्रति(i=0; i<source_दीर्घता; ++i) {
        यदि(source[i]==variant5c) {
            source[i]=0x5c;
        }
    }
}

U_CAPI UBool  U_EXPORT2
ucnv_isAmbiguous(ध्रुव UConverter *cnv) {
    प्रत्यागम ucnv_getAmbiguous(cnv)!=शून्यसूचकः;
}

U_CAPI शून्य  U_EXPORT2
ucnv_समुच्चयः_Fallback(UConverter *cnv, UBool उपयुङ्क्ते_Fallback)
{
    cnv->useFallback = उपयुङ्क्ते_Fallback;
}

U_CAPI UBool  U_EXPORT2
ucnv_उपयुङ्क्ते_Fallback(ध्रुव UConverter *cnv)
{
    प्रत्यागम cnv->useFallback;
}

U_CAPI शून्य  U_EXPORT2
ucnv_getInvalidChars (ध्रुव UConverter * converter,
                      वर्ण *err_अष्टकानि,
                      int8_t * len,
                      UError_संहिता * err)
{
    यदि (err == शून्यसूचकः || U_FAILURE(*err))
    {
        प्रत्यागम;
    }
    यदि (len == शून्यसूचकः || err_अष्टकानि == शून्यसूचकः || converter == शून्यसूचकः)
    {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि (*len < converter->invalid_Char_दीर्घता)
    {
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }
    यदि ((*len = converter->invalid_Char_दीर्घता) > 0)
    {
        uprv_memcpy (err_अष्टकानि, converter->invalid_Char_बफर, *len);
    }
}

U_CAPI शून्य  U_EXPORT2
ucnv_getInvalidUChars (ध्रुव UConverter * converter,
                       char16_t *errChars,
                       int8_t * len,
                       UError_संहिता * err)
{
    यदि (err == शून्यसूचकः || U_FAILURE(*err))
    {
        प्रत्यागम;
    }
    यदि (len == शून्यसूचकः || errChars == शून्यसूचकः || converter == शून्यसूचकः)
    {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि (*len < converter->invalid_UChar_दीर्घता)
    {
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }
    यदि ((*len = converter->invalid_UChar_दीर्घता) > 0)
    {
        u_memcpy (errChars, converter->invalid_UChar_बफर, *len);
    }
}

#परिभाषय SIG_MAX_LEN 5

U_CAPI ध्रुव वर्ण* U_EXPORT2
ucnv_detectUnicodeSignature( ध्रुव वर्ण* source,
                             int32_t source_दीर्घता,
                             int32_t* signature_दीर्घता,
                             UError_संहिता* p_Error_संहिता) {
    int32_t dummy;

    /* initial 0xa5 bytes: make sure that if we read <SIG_MAX_LEN
     * bytes we don't misdetect something 
     */
    वर्ण start[SIG_MAX_LEN]={ '\xa5', '\xa5', '\xa5', '\xa5', '\xa5' };
    पूर्णाङ्क i = 0;

    यदि((p_Error_संहिता==शून्यसूचकः) || U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }
    
    यदि(source == शून्यसूचकः || source_दीर्घता < -1){
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(signature_दीर्घता == शून्यसूचकः) {
        signature_दीर्घता = &dummy;
    }

    यदि(source_दीर्घता==-1){
        source_दीर्घता=(int32_t)uprv_strlen(source);
    }

    
    यावत्(i<source_दीर्घता&& i<SIG_MAX_LEN){
        start[i]=source[i];
        i++;
    }

    यदि(start[0] == '\xFE' && start[1] == '\xFF') {
        *signature_दीर्घता=2;
        प्रत्यागम  "UTF-16BE";
    } अन्यथा यदि(start[0] == '\xFF' && start[1] == '\xFE') {
        यदि(start[2] == '\x00' && start[3] =='\x00') {
            *signature_दीर्घता=4;
            प्रत्यागम "UTF-32LE";
        } अन्यथा {
            *signature_दीर्घता=2;
            प्रत्यागम  "UTF-16LE";
        }
    } अन्यथा यदि(start[0] == '\xEF' && start[1] == '\xBB' && start[2] == '\xBF') {
        *signature_दीर्घता=3;
        प्रत्यागम  "UTF-8";
    } अन्यथा यदि(start[0] == '\x00' && start[1] == '\x00' && 
              start[2] == '\xFE' && start[3]=='\xFF') {
        *signature_दीर्घता=4;
        प्रत्यागम  "UTF-32BE";
    } अन्यथा यदि(start[0] == '\x0E' && start[1] == '\xFE' && start[2] == '\xFF') {
        *signature_दीर्घता=3;
        प्रत्यागम "SCSU";
    } अन्यथा यदि(start[0] == '\xFB' && start[1] == '\xEE' && start[2] == '\x28') {
        *signature_दीर्घता=3;
        प्रत्यागम "BOCU-1";
    } अन्यथा यदि(start[0] == '\x2B' && start[1] == '\x2F' && start[2] == '\x76') {
        /*
         * UTF-7: Initial U+FEFF is encoded as +/v8  or  +/v9  or  +/v+  or  +/v/
         * depending on the second UTF-16 code unit.
         * Detect the entire, closed Unicode mode sequence +/v8- for only U+FEFF
         * if it occurs.
         *
         * So far we have +/v
         */
        यदि(start[3] == '\x38' && start[4] == '\x2D') {
            /* 5 bytes +/v8- */
            *signature_दीर्घता=5;
            प्रत्यागम "UTF-7";
        } अन्यथा यदि(start[3] == '\x38' || start[3] == '\x39' || start[3] == '\x2B' || start[3] == '\x2F') {
            /* 4 bytes +/v8  or  +/v9  or  +/v+  or  +/v/ */
            *signature_दीर्घता=4;
            प्रत्यागम "UTF-7";
        }
    }अन्यथा यदि(start[0]=='\xDD' && start[1]== '\x73'&& start[2]=='\x66' && start[3]=='\x73'){
        *signature_दीर्घता=4;
        प्रत्यागम "UTF-EBCDIC";
    }


    /* no known Unicode signature byte sequence recognized */
    *signature_दीर्घता=0;
    प्रत्यागम शून्यसूचकः;
}

U_CAPI int32_t U_EXPORT2
ucnv_fromUCountPending(ध्रुव UConverter* cnv, UError_संहिता* स्थितिः)
{
    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम -1;
    }
    यदि(cnv == शून्यसूचकः){
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम -1;
    }

    यदि(cnv->preFromUFirstCP >= 0){
        प्रत्यागम U16_दीर्घता(cnv->preFromUFirstCP)+cnv->preFromULength ;
    }अन्यथा यदि(cnv->preFromULength < 0){
        प्रत्यागम -cnv->preFromULength ;
    }अन्यथा यदि(cnv->fromUChar32 > 0){
        प्रत्यागम 1;
    }
    प्रत्यागम 0; 

}

U_CAPI int32_t U_EXPORT2
ucnv_गन्तव्यम्_UCount_Pending(ध्रुव UConverter* cnv, UError_संहिता* स्थितिः){

    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम -1;
    }
    यदि(cnv == शून्यसूचकः){
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम -1;
    }

    यदि(cnv->pre_गन्तव्यम्_ULength > 0){
        प्रत्यागम cnv->pre_गन्तव्यम्_ULength ;
    }अन्यथा यदि(cnv->pre_गन्तव्यम्_ULength < 0){
        प्रत्यागम -cnv->pre_गन्तव्यम्_ULength;
    }अन्यथा यदि(cnv->गन्तव्यम्_ULength > 0){
        प्रत्यागम cnv->गन्तव्यम्_ULength;
    }
    प्रत्यागम 0;
}

U_CAPI UBool U_EXPORT2
ucnv_is_Fixed_विस्तारः(UConverter *cnv, UError_संहिता *स्थितिः){
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम असत्य;
    }

    यदि (cnv == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }

    विकल्प (ucnv_getType(cnv)) {
        पक्ष UCNV_SBCS:
        पक्ष UCNV_DBCS:
        पक्ष UCNV_UTF32_BigEndian:
        पक्ष UCNV_UTF32_LittleEndian:
        पक्ष UCNV_UTF32:
        पक्ष UCNV_US_ASCII:
            प्रत्यागम सत्य;
        स्वभाव:
            प्रत्यागम असत्य;
    }
}
#यदि_अन्तः

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
