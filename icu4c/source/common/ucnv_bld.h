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

#నిర్వచితం_కాకపోతే UCNV_BLD_H
#నిర్వచించు UCNV_BLD_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ucnv_err.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ucnvmbcs.h"
#అన్తర్భావ "ucnv_ext.h"
#అన్తర్భావ "udataswp.h"

/* size of the overflow buffers in UConverter, enough for escaping callbacks */
#నిర్వచించు UCNV_ERROR_బఫరు_నిడివి 32

/* at most 4 bytes per substitution character (part of .cnv file format! see UConverterStaticData) */
#నిర్వచించు UCNV_MAX_SUBCHAR_LEN 4

/* at most 8 bytes per character in toUBytes[] (UTF-8 uses up to 6) */
#నిర్వచించు UCNV_MAX_CHAR_LEN 8

/* converter options bits */
#నిర్వచించు UCNV_OPTION_సంస్కరణ     0xf
#నిర్వచించు UCNV_OPTION_మార్పిడి_LFNL   0x10

#నిర్వచించు UCNV_GET_సంస్కరణ(cnv) ((cnv)->options&UCNV_OPTION_సంస్కరణ)

U_CDECL_BEGIN /* We must declare the following as 'extern "C"' so that if ucnv
                 itself is compiled under C++, the linkage of the funcptrs will
                 work.
              */

సంఘం UConverter_పట్టిక {
    UConverterMBCSTable mbcs;
};

రకనిర్వచనం సంఘం UConverter_పట్టిక UConverter_పట్టిక;

నిర్మాణం UConverterImpl;
రకనిర్వచనం నిర్మాణం UConverterImpl UConverterImpl;

/** values for the unicodeMask */
#నిర్వచించు UCNV_HAS_SUPPLEMENTARY 1
#నిర్వచించు UCNV_HAS_SURROGATES    2

రకనిర్వచనం నిర్మాణం UConverter_Static_దత్తాంశం {   /* +offset: size */
    uint32_t struct_కొలత;                /* +0: 4 Size of this structure */

    అక్షరం name
      [UCNV_MAX_CONVERTER_NAME_నిడివి]; /* +4: 60  internal name of the converter- invariant chars */

    int32_t codepage;               /* +64: 4 codepage # (now IBM-$codepage) */

    int8_t platform;                /* +68: 1 platform of the converter (only IBM now) */
    int8_t conversionType;          /* +69: 1 conversion type */

    int8_t min_బైట్లు_చొప్పున_Char;         /* +70: 1 Minimum # bytes per char in this codepage */
    int8_t max_బైట్లు_చొప్పున_Char;         /* +71: 1 Maximum # bytes output per UChar in this codepage */

    uint8_t subChar[UCNV_MAX_SUBCHAR_LEN]; /* +72: 4  [note:  4 and 8 byte boundary] */
    int8_t subCharLen;              /* +76: 1 */

    uint8_t has_గమ్యం_Unicode_Fallback;   /* +77: 1 UBool needs to be changed to UBool to be consistent across platform */
    uint8_t hasFromUnicodeFallback; /* +78: 1 */
    uint8_t unicode_ముసుగు;            /* +79: 1  bit 0: has supplementary  bit 1: has single surrogates */
    uint8_t subChar1;               /* +80: 1  single-byte substitution character for IBM MBCS (0 if none) */
    uint8_t reserved[19];           /* +81: 19 to round out the structure */
                                    /* total size: 100 */
} UConverter_Static_దత్తాంశం;

/*
 * Defines the UConverterSharedData struct,
 * the immutable, shared part of UConverter.
 */
నిర్మాణం UConverter_Shared_దత్తాంశం {
    uint32_t struct_కొలత;            /* Size of this structure */
    uint32_t referenceCounter;      /* used to count number of clients, unused for static/immutable SharedData */

    స్థిరరాశి శూన్య *దత్తాంశం_జ్ఞాపకం;         /* from udata_openChoice() - for cleanup */

    స్థిరరాశి UConverter_Static_దత్తాంశం *static_దత్తాంశం; /* pointer to the static (non changing) data. */

    UBool                shared_దత్తాంశం_Cached;   /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    /** If false, then referenceCounter is not used. Must not change after initialization. */
    UBool isReferenceCounted;

    స్థిరరాశి UConverterImpl *impl;     /* vtable-style struct of mostly function pointers */

    /*initial values of some members of the mutable part of object */
    uint32_t గమ్యం_Unicode_స్థితి;

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
#నిర్వచించు UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(p_Static_దత్తాంశం, pImpl) \
    { \
        పరిమాణం(UConverter_Shared_దత్తాంశం), ~((uint32_t)0), \
        ఖాళీ, p_Static_దత్తాంశం, false, false, pImpl, \
        0, UCNV_MBCS_పట్టిక_INITIALIZER \
    }

/* Defines a UConverter, the lightweight mutable part the user sees */

నిర్మాణం UConverter {
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_fromUnicode call
     */
    శూన్య (U_EXPORT2 *fromUCharErrorBehaviour) (స్థిరరాశి శూన్య *context,
                                     UConverter_From_Unicode_వాదనలు *వాదనలు_2,
                                     స్థిరరాశి UChar *సంకేతం_Units,
                                     int32_t నిడివి,
                                     UChar32 సంకేతం_బిందువు,
                                     UConverterCallbackReason reason,
                                     UError_సంకేతం *);
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_toUnicode call
     */
    శూన్య (U_EXPORT2 *fromCharErrorBehaviour) (స్థిరరాశి శూన్య *context,
                                    UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                                    స్థిరరాశి అక్షరం *సంకేతం_Units,
                                    int32_t నిడివి,
                                    UConverterCallbackReason reason,
                                    UError_సంకేతం *);

    /*
     * Pointer to additional data that depends on the converter type.
     * Used by ISO 2022, SCSU, GB 18030 converters, possibly more.
     */
    శూన్య *extraInfo;

    స్థిరరాశి శూన్య *fromUContext;
    స్థిరరాశి శూన్య *గమ్యం_UContext;

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

    UConverter_Shared_దత్తాంశం *shared_దత్తాంశం;   /* Pointer to the shared immutable part of the converter object */

    uint32_t options; /* options flags from UConverterOpen, may contain additional bits */

    UBool shared_దత్తాంశం_Is_Cached;  /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    UBool isCopyLocal;  /* true if UConverter is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */
    UBool isExtraLocal; /* true if extraInfo is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */

    UBool  useFallback;
    int8_t గమ్యం_ULength;                   /* number of bytes in toUBytes */
    uint8_t గమ్యం_UBytes[UCNV_MAX_CHAR_LEN-1];/* more "toU status"; keeps the bytes of the current character */
    uint32_t గమ్యం_Unicode_స్థితి;           /* Used to internalize stream status information */
    int32_t బహుళకం;
    uint32_t from_Unicode_స్థితి;

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
    int8_t max_బైట్లు_చొప్పున_UChar;

    int8_t subCharLen;                  /* length of the codepage specific character sequence */
    int8_t invalid_Char_నిడివి;
    int8_t char_Error_బఫరు_నిడివి;       /* number of valid bytes in charErrorBuffer */

    int8_t invalid_UChar_నిడివి;
    int8_t UChar_Error_బఫరు_నిడివి;      /* number of valid UChars in charErrorBuffer */

    uint8_t subChar1;                                   /* single-byte substitution character if different from subChar */
    UBool useSubChar1;
    అక్షరం invalid_Char_బఫరు[UCNV_MAX_CHAR_LEN];          /* bytes from last error/callback situation */
    uint8_t char_Error_బఫరు[UCNV_ERROR_బఫరు_నిడివి];  /* codepage output from Error functions */
    UChar subUChars[UCNV_MAX_SUBCHAR_LEN/U_SIZEOF_UCHAR]; /* see subChars documentation */

    UChar invalid_UChar_బఫరు[U16_MAX_నిడివి];           /* UChars from last error/callback situation */
    UChar UChar_Error_బఫరు[UCNV_ERROR_బఫరు_నిడివి];   /* unicode output from Error functions */

    /* fields for conversion extension */

    /* store previous UChars/chars to continue partial matches */
    UChar32 preFromUFirstCP;                /* >=0: partial match */
    UChar preFromU[UCNV_EXT_MAX_UCHARS];
    అక్షరం pre_గమ్యం_U[UCNV_EXT_MAX_బైట్లు];
    int8_t preFromULength, pre_గమ్యం_ULength;    /* negative: replay */
    int8_t pre_గమ్యం_UFirst_నిడివి;               /* length of first character */

    /* new fields for ICU 4.0 */
    UConverterCallbackReason గమ్యం_UCallback_Reason; /* (*fromCharErrorBehaviour) reason, set when error is detected */
};

U_CDECL_చివర /* end of UConverter */

#నిర్వచించు CONVERTER_FILE_EXTENSION ".cnv"


/**
 * Return the number of all converter names.
 * @param pErrorCode The error code
 * @return the number of all converter names
 */
U_CFUNC uint16_t
ucnv_bld_లెక్క_అందుబాటు_Converters(UError_సంకేతం *p_Error_సంకేతం);

/**
 * Return the (n)th converter name in mixed case, or NULL
 * if there is none (typically, if the data cannot be loaded).
 * 0<=index<ucnv_io_countAvailableConverters().
 * @param n The number specifies which converter name to get
 * @param pErrorCode The error code
 * @return the (n)th converter name in mixed case, or NULL if there is none.
 */
U_CFUNC స్థిరరాశి అక్షరం *
ucnv_bld_get_అందుబాటు_Converter(uint16_t n, UError_సంకేతం *p_Error_సంకేతం);

/**
 * Load a non-algorithmic converter.
 * If pkg==NULL, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI UConverter_Shared_దత్తాంశం *
ucnv_ఎక్కించు(UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *err);

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI శూన్య
ucnv_unload(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం);

/**
 * Swap ICU .cnv conversion tables. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
          స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t నిడివి, శూన్య *బహిర్గతం_దత్తాంశం,
          UError_సంకేతం *p_Error_సంకేతం);

U_CAPI శూన్య U_EXPORT2
ucnv_enableCleanup(శూన్య);

#అయితే_ముగింపు

#అయితే_ముగింపు /* _UCNV_BLD */
