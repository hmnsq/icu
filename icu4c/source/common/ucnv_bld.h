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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UCNV_BLD_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_BLD_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_err.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnvmbcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_ext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"

/* size of the overflow buffers in UConverter, enough for escaping callbacks */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ 32

/* at most 4 bytes per substitution character (part of .cnv file format! see UConverterStaticData) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_MAX_SUBCHAR_LEN 4

/* at most 8 bytes per character in toUBytes[] (UTF-8 uses up to 6) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_MAX_CHAR_LEN 8

/* converter options bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_OPTION_ਸੰਸਕਰਣ     0xf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL   0x10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_GET_ਸੰਸਕਰਣ(cnv) ((cnv)->options&UCNV_OPTION_ਸੰਸਕਰਣ)

U_CDECL_BEGIN /* We must declare the following as 'extern "C"' so that if ucnv
                 itself is compiled under C++, the linkage of the funcptrs will
                 work.
              */

ਸੰਘ UConverter_ਸਾਰਣੀ {
    UConverterMBCSTable mbcs;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ UConverter_ਸਾਰਣੀ UConverter_ਸਾਰਣੀ;

ਬਣਤਰ UConverterImpl;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UConverterImpl UConverterImpl;

/** values for the unicodeMask */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_HAS_SUPPLEMENTARY 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_HAS_SURROGATES    2

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UConverter_Static_ਅੰਕੜੇ {   /* +offset: size */
    uint32_t struct_ਮਾਪ;                /* +0: 4 Size of this structure */

    ਅੱਖਰ name
      [UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ]; /* +4: 60  internal name of the converter- invariant chars */

    int32_t codepage;               /* +64: 4 codepage # (now IBM-$codepage) */

    int8_t platform;                /* +68: 1 platform of the converter (only IBM now) */
    int8_t conversionType;          /* +69: 1 conversion type */

    int8_t min_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char;         /* +70: 1 Minimum # bytes per char in this codepage */
    int8_t max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char;         /* +71: 1 Maximum # bytes output per UChar in this codepage */

    uint8_t subChar[UCNV_MAX_SUBCHAR_LEN]; /* +72: 4  [note:  4 and 8 byte boundary] */
    int8_t subCharLen;              /* +76: 1 */

    uint8_t has_ਮੰਜ਼ਿਲ_Unicode_Fallback;   /* +77: 1 UBool needs to be changed to UBool to be consistent across platform */
    uint8_t hasFromUnicodeFallback; /* +78: 1 */
    uint8_t unicode_ਨਕਾਬ;            /* +79: 1  bit 0: has supplementary  bit 1: has single surrogates */
    uint8_t subChar1;               /* +80: 1  single-byte substitution character for IBM MBCS (0 if none) */
    uint8_t reserved[19];           /* +81: 19 to round out the structure */
                                    /* total size: 100 */
} UConverter_Static_ਅੰਕੜੇ;

/*
 * Defines the UConverterSharedData struct,
 * the immutable, shared part of UConverter.
 */
ਬਣਤਰ UConverter_Shared_ਅੰਕੜੇ {
    uint32_t struct_ਮਾਪ;            /* Size of this structure */
    uint32_t referenceCounter;      /* used to count number of clients, unused for static/immutable SharedData */

    ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ;         /* from udata_openChoice() - for cleanup */

    ਅਚਲ UConverter_Static_ਅੰਕੜੇ *static_ਅੰਕੜੇ; /* pointer to the static (non changing) data. */

    UBool                shared_ਅੰਕੜੇ_Cached;   /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    /** If false, then referenceCounter is not used. Must not change after initialization. */
    UBool isReferenceCounted;

    ਅਚਲ UConverterImpl *impl;     /* vtable-style struct of mostly function pointers */

    /*initial values of some members of the mutable part of object */
    uint32_t ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(p_Static_ਅੰਕੜੇ, pImpl) \
    { \
        ਆਕਾਰ(UConverter_Shared_ਅੰਕੜੇ), ~((uint32_t)0), \
        ਨਿਰਾ, p_Static_ਅੰਕੜੇ, false, false, pImpl, \
        0, UCNV_MBCS_ਸਾਰਣੀ_INITIALIZER \
    }

/* Defines a UConverter, the lightweight mutable part the user sees */

ਬਣਤਰ UConverter {
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_fromUnicode call
     */
    ਖਾਲੀ (U_EXPORT2 *fromUCharErrorBehaviour) (ਅਚਲ ਖਾਲੀ *context,
                                     UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                     ਅਚਲ UChar *ਕੋਡ_Units,
                                     int32_t ਲੰਮਾਈ,
                                     UChar32 ਕੋਡ_ਬਿੰਦੂ,
                                     UConverterCallbackReason reason,
                                     UError_ਕੋਡ *);
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_toUnicode call
     */
    ਖਾਲੀ (U_EXPORT2 *fromCharErrorBehaviour) (ਅਚਲ ਖਾਲੀ *context,
                                    UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                    ਅਚਲ ਅੱਖਰ *ਕੋਡ_Units,
                                    int32_t ਲੰਮਾਈ,
                                    UConverterCallbackReason reason,
                                    UError_ਕੋਡ *);

    /*
     * Pointer to additional data that depends on the converter type.
     * Used by ISO 2022, SCSU, GB 18030 converters, possibly more.
     */
    ਖਾਲੀ *extraInfo;

    ਅਚਲ ਖਾਲੀ *fromUContext;
    ਅਚਲ ਖਾਲੀ *ਮੰਜ਼ਿਲ_UContext;

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

    UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ;   /* Pointer to the shared immutable part of the converter object */

    uint32_t options; /* options flags from UConverterOpen, may contain additional bits */

    UBool shared_ਅੰਕੜੇ_Is_Cached;  /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    UBool isCopyLocal;  /* true if UConverter is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */
    UBool isExtraLocal; /* true if extraInfo is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */

    UBool  useFallback;
    int8_t ਮੰਜ਼ਿਲ_ULength;                   /* number of bytes in toUBytes */
    uint8_t ਮੰਜ਼ਿਲ_UBytes[UCNV_MAX_CHAR_LEN-1];/* more "toU status"; keeps the bytes of the current character */
    uint32_t ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;           /* Used to internalize stream status information */
    int32_t ਬਹੁਲਕ;
    uint32_t from_Unicode_ਸਥਿਤੀ;

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
    int8_t max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar;

    int8_t subCharLen;                  /* length of the codepage specific character sequence */
    int8_t invalid_Char_ਲੰਮਾਈ;
    int8_t char_Error_ਬਫਰ_ਲੰਮਾਈ;       /* number of valid bytes in charErrorBuffer */

    int8_t invalid_UChar_ਲੰਮਾਈ;
    int8_t UChar_Error_ਬਫਰ_ਲੰਮਾਈ;      /* number of valid UChars in charErrorBuffer */

    uint8_t subChar1;                                   /* single-byte substitution character if different from subChar */
    UBool useSubChar1;
    ਅੱਖਰ invalid_Char_ਬਫਰ[UCNV_MAX_CHAR_LEN];          /* bytes from last error/callback situation */
    uint8_t char_Error_ਬਫਰ[UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ];  /* codepage output from Error functions */
    UChar subUChars[UCNV_MAX_SUBCHAR_LEN/U_SIZEOF_UCHAR]; /* see subChars documentation */

    UChar invalid_UChar_ਬਫਰ[U16_MAX_ਲੰਮਾਈ];           /* UChars from last error/callback situation */
    UChar UChar_Error_ਬਫਰ[UCNV_ERROR_ਬਫਰ_ਲੰਮਾਈ];   /* unicode output from Error functions */

    /* fields for conversion extension */

    /* store previous UChars/chars to continue partial matches */
    UChar32 preFromUFirstCP;                /* >=0: partial match */
    UChar preFromU[UCNV_EXT_MAX_UCHARS];
    ਅੱਖਰ pre_ਮੰਜ਼ਿਲ_U[UCNV_EXT_MAX_ਬਾਈਟਾਂ];
    int8_t preFromULength, pre_ਮੰਜ਼ਿਲ_ULength;    /* negative: replay */
    int8_t pre_ਮੰਜ਼ਿਲ_UFirst_ਲੰਮਾਈ;               /* length of first character */

    /* new fields for ICU 4.0 */
    UConverterCallbackReason ਮੰਜ਼ਿਲ_UCallback_Reason; /* (*fromCharErrorBehaviour) reason, set when error is detected */
};

U_CDECL_ਸਿਰਾ /* end of UConverter */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONVERTER_FILE_EXTENSION ".cnv"


/**
 * Return the number of all converter names.
 * @param pErrorCode The error code
 * @return the number of all converter names
 */
U_CFUNC uint16_t
ucnv_bld_ਗਿਣਤੀ_ਉਪਲਬਧ_Converters(UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Return the (n)th converter name in mixed case, or NULL
 * if there is none (typically, if the data cannot be loaded).
 * 0<=index<ucnv_io_countAvailableConverters().
 * @param n The number specifies which converter name to get
 * @param pErrorCode The error code
 * @return the (n)th converter name in mixed case, or NULL if there is none.
 */
U_CFUNC ਅਚਲ ਅੱਖਰ *
ucnv_bld_get_ਉਪਲਬਧ_Converter(uint16_t n, UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Load a non-algorithmic converter.
 * If pkg==NULL, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI UConverter_Shared_ਅੰਕੜੇ *
ucnv_ਲੱਦੋ(UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *err);

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI ਖਾਲੀ
ucnv_unload(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ);

/**
 * Swap ICU .cnv conversion tables. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
          ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
          UError_ਕੋਡ *p_Error_ਕੋਡ);

U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_enableCleanup(ਖਾਲੀ);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* _UCNV_BLD */
