// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2015 International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
*
*  ucnv_bld.h:
*  Contains internal data structure definitions
* Created by Bertrand A. Damiba
*
*   Change history:
*
*   06/29/2000  helena      Major rewrite of the callback APIs.
*/

#यदि_अपरिभाषितम् UCNV_BLD_H
#परिभाषय UCNV_BLD_H

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnvmbcs.h"
#अन्तर्भाव "ucnv_ext.h"
#अन्तर्भाव "udataswp.h"

/* size of the overflow buffers in UConverter, enough for escaping callbacks */
#परिभाषय UCNV_ERROR_बफर_दीर्घता 32

/* at most 4 bytes per substitution character (part of .cnv file format! see UConverterStaticData) */
#परिभाषय UCNV_MAX_SUBCHAR_LEN 4

/* at most 8 bytes per character in toUBytes[] (UTF-8 uses up to 6) */
#परिभाषय UCNV_MAX_CHAR_LEN 8

/* converter options bits */
#परिभाषय UCNV_OPTION_संस्करणम्     0xf
#परिभाषय UCNV_OPTION_SWAP_LFNL   0x10

#परिभाषय UCNV_GET_संस्करणम्(cnv) ((cnv)->options&UCNV_OPTION_संस्करणम्)

U_CDECL_BEGIN /* We must declare the following as 'extern "C"' so that if ucnv
                 itself is compiled under C++, the linkage of the funcptrs will
                 work.
              */

सङ्घ UConverter_सारणी {
    UConverterMBCSTable mbcs;
};

प्रकारनिर्देश सङ्घ UConverter_सारणी UConverter_सारणी;

संरचना UConverterImpl;
प्रकारनिर्देश संरचना UConverterImpl UConverterImpl;

/** values for the unicodeMask */
#परिभाषय UCNV_HAS_SUPPLEMENTARY 1
#परिभाषय UCNV_HAS_SURROGATES    2

प्रकारनिर्देश संरचना UConverter_Static_दत्त {   /* +offset: size */
    uint32_t struct_आकार;                /* +0: 4 Size of this structure */

    वर्ण name
      [UCNV_MAX_CONVERTER_NAME_दीर्घता]; /* +4: 60  internal name of the converter- invariant chars */

    int32_t codepage;               /* +64: 4 codepage # (now IBM-$codepage) */

    int8_t platform;                /* +68: 1 platform of the converter (only IBM now) */
    int8_t conversionType;          /* +69: 1 conversion type */

    int8_t min_अष्टकानि_प्रत्येकम्_Char;         /* +70: 1 Minimum # bytes per char in this codepage */
    int8_t max_अष्टकानि_प्रत्येकम्_Char;         /* +71: 1 Maximum # bytes output per UChar in this codepage */

    uint8_t subChar[UCNV_MAX_SUBCHAR_LEN]; /* +72: 4  [note:  4 and 8 byte boundary] */
    int8_t subCharLen;              /* +76: 1 */

    uint8_t has_गन्तव्यम्_Unicode_Fallback;   /* +77: 1 UBool needs to be changed to UBool to be consistent across platform */
    uint8_t hasFromUnicodeFallback; /* +78: 1 */
    uint8_t unicode_आवरणम्;            /* +79: 1  bit 0: has supplementary  bit 1: has single surrogates */
    uint8_t subChar1;               /* +80: 1  single-byte substitution character for IBM MBCS (0 if none) */
    uint8_t reserved[19];           /* +81: 19 to round out the structure */
                                    /* total size: 100 */
} UConverter_Static_दत्त;

/*
 * Defines the UConverterSharedData struct,
 * the immutable, shared part of UConverter.
 */
संरचना UConverter_Shared_दत्त {
    uint32_t struct_आकार;            /* Size of this structure */
    uint32_t reference_गणक;      /* used to count number of clients, unused for static/immutable SharedData */

    ध्रुव शून्य *दत्त_स्मृतिः;         /* from udata_openChoice() - for cleanup */

    ध्रुव UConverter_Static_दत्त *static_दत्त; /* pointer to the static (non changing) data. */

    UBool                shared_दत्त_Cached;   /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    /** If false, then referenceCounter is not used. Must not change after initialization. */
    UBool isReferenceCounted;

    ध्रुव UConverterImpl *impl;     /* vtable-style struct of mostly function pointers */

    /*initial values of some members of the mutable part of object */
    uint32_t गन्तव्यम्_Unicode_स्थितिः;

    /*
     * Shared data structures currently come in two flavors:
     * - readonly for built-in algorithmic converters
     * - allocated for MBCS, with a pointer to an allocated UConverterTable
     *   which always has a UConverterMBCSTable
     *
     * To eliminate one allocation, I am making the UConverterMBCSTable
     * a member of the shared data.
     *
     * markus 2003-nov-07
     */
    UConverterMBCSTable mbcs;
};

/** UConverterSharedData initializer for static, non-reference-counted converters. */
#परिभाषय UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(p_Static_दत्त, pImpl) \
    { \
        परिमाण(UConverter_Shared_दत्त), ~((uint32_t)0), \
        रिक्त, p_Static_दत्त, false, false, pImpl, \
        0, UCNV_MBCS_सारणी_INITIALIZER \
    }

/* Defines a UConverter, the lightweight mutable part the user sees */

संरचना UConverter {
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_fromUnicode call
     */
    शून्य (U_EXPORT2 *fromUCharErrorBehaviour) (ध्रुव शून्य *context,
                                     UConverter_From_Unicode_तर्काः *तर्काः_2,
                                     ध्रुव UChar *संहिता_Units,
                                     int32_t दीर्घता,
                                     UChar32 संहिता_Point,
                                     UConverterCallbackReason reason,
                                     UError_संहिता *);
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_toUnicode call
     */
    शून्य (U_EXPORT2 *fromCharErrorBehaviour) (ध्रुव शून्य *context,
                                    UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                                    ध्रुव वर्ण *संहिता_Units,
                                    int32_t दीर्घता,
                                    UConverterCallbackReason reason,
                                    UError_संहिता *);

    /*
     * Pointer to additional data that depends on the converter type.
     * Used by ISO 2022, SCSU, GB 18030 converters, possibly more.
     */
    शून्य *extraInfo;

    ध्रुव शून्य *fromUContext;
    ध्रुव शून्य *गन्तव्यम्_UContext;

    /*
     * Pointer to charset bytes for substitution string if subCharLen>0,
     * or pointer to Unicode string (UChar *) if subCharLen<0.
     * subCharLen==0 is equivalent to using a skip callback.
     * If the pointer is !=subUChars then it is allocated with
     * UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR bytes.
     * The subUChars field is declared as UChar[] not uint8_t[] to
     * guarantee alignment for UChars.
     */
    uint8_t *subChars;

    UConverter_Shared_दत्त *shared_दत्त;   /* Pointer to the shared immutable part of the converter object */

    uint32_t options; /* options flags from UConverterOpen, may contain additional bits */

    UBool shared_दत्त_Is_Cached;  /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    UBool isCopyLocal;  /* true if UConverter is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */
    UBool isExtraLocal; /* true if extraInfo is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */

    UBool  useFallback;
    int8_t गन्तव्यम्_ULength;                   /* number of bytes in toUBytes */
    uint8_t गन्तव्यम्_UBytes[UCNV_MAX_CHAR_LEN-1];/* more "toU status"; keeps the bytes of the current character */
    uint32_t गन्तव्यम्_Unicode_स्थितिः;           /* Used to internalize stream status information */
    int32_t बहुलकम्;
    uint32_t from_Unicode_स्थितिः;

    /*
     * More fromUnicode() status. Serves 3 purposes:
     * - keeps a lead surrogate between buffers (similar to toUBytes[])
     * - keeps a lead surrogate at the end of the stream,
     *   which the framework handles as truncated input
     * - if the fromUnicode() implementation returns to the framework
     *   (ucnv.c ucnv_fromUnicode()), then the framework calls the callback
     *   for this code point
     */
    UChar32 fromUChar32;

    /*
     * value for ucnv_getMaxCharSize()
     *
     * usually simply copied from the static data, but ucnvmbcs.c modifies
     * the value depending on the converter type and options
     */
    int8_t max_अष्टकानि_प्रत्येकम्_UChar;

    int8_t subCharLen;                  /* length of the codepage specific character sequence */
    int8_t invalid_Char_दीर्घता;
    int8_t char_Error_बफर_दीर्घता;       /* number of valid bytes in charErrorBuffer */

    int8_t invalid_UChar_दीर्घता;
    int8_t UChar_Error_बफर_दीर्घता;      /* number of valid UChars in charErrorBuffer */

    uint8_t subChar1;                                   /* single-byte substitution character if different from subChar */
    UBool useSubChar1;
    वर्ण invalid_Char_बफर[UCNV_MAX_CHAR_LEN];          /* bytes from last error/callback situation */
    uint8_t char_Error_बफर[UCNV_ERROR_बफर_दीर्घता];  /* codepage output from Error functions */
    UChar subUChars[UCNV_MAX_SUBCHAR_LEN/U_SIZEOF_UCHAR]; /* see subChars documentation */

    UChar invalid_UChar_बफर[U16_MAX_दीर्घता];           /* UChars from last error/callback situation */
    UChar UChar_Error_बफर[UCNV_ERROR_बफर_दीर्घता];   /* unicode output from Error functions */

    /* fields for conversion extension */

    /* store previous UChars/chars to continue partial matches */
    UChar32 preFromUFirstCP;                /* >=0: partial match */
    UChar preFromU[UCNV_EXT_MAX_UCHARS];
    वर्ण pre_गन्तव्यम्_U[UCNV_EXT_MAX_अष्टकानि];
    int8_t preFromULength, pre_गन्तव्यम्_ULength;    /* negative: replay */
    int8_t pre_गन्तव्यम्_UFirst_दीर्घता;               /* length of first character */

    /* new fields for ICU 4.0 */
    UConverterCallbackReason गन्तव्यम्_UCallback_Reason; /* (*fromCharErrorBehaviour) reason, set when error is detected */
};

U_CDECL_अन्तः /* end of UConverter */

#परिभाषय CONVERTER_FILE_EXTENSION ".cnv"


/**
 * Return the number of all converter names.
 * @param pErrorCode The error code
 * @return the number of all converter names
 */
U_CFUNC uint16_t
ucnv_bld_गणनम्_उपलब्धम्_Converters(UError_संहिता *p_Error_संहिता);

/**
 * Return the (n)th converter name in mixed case, or NULL
 * if there is none (typically, if the data cannot be loaded).
 * 0<=index<ucnv_io_countAvailableConverters().
 * @param n The number specifies which converter name to get
 * @param pErrorCode The error code
 * @return the (n)th converter name in mixed case, or NULL if there is none.
 */
U_CFUNC ध्रुव वर्ण *
ucnv_bld_get_उपलब्धम्_Converter(uint16_t n, UError_संहिता *p_Error_संहिता);

/**
 * Load a non-algorithmic converter.
 * If pkg==NULL, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI UConverter_Shared_दत्त *
ucnv_आरोपय(UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *err);

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI शून्य
ucnv_unload(UConverter_Shared_दत्त *shared_दत्त);

/**
 * Swap ICU .cnv conversion tables. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_swap(ध्रुव UDataSwapper *ds,
          ध्रुव शून्य *अन्तर्_दत्त, int32_t दीर्घता, शून्य *बहिर्गमनम्_दत्त,
          UError_संहिता *p_Error_संहिता);

U_CAPI शून्य U_EXPORT2
ucnv_enableCleanup(शून्य);

#यदि_अन्तः

#यदि_अन्तः /* _UCNV_BLD */
