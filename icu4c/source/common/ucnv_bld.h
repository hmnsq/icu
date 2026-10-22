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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UCNV_BLD_H
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_BLD_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnvmbcs.h"
#ಅನ್ತರ್ಭಾವ "ucnv_ext.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"

/* size of the overflow buffers in UConverter, enough for escaping callbacks */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ 32

/* at most 4 bytes per substitution character (part of .cnv file format! see UConverterStaticData) */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_MAX_SUBCHAR_LEN 4

/* at most 8 bytes per character in toUBytes[] (UTF-8 uses up to 6) */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_MAX_CHAR_LEN 8

/* converter options bits */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_OPTION_ಆವೃತ್ತಿ     0xf
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_OPTION_ಅದಲುಬದಲು_LFNL   0x10

#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_GET_ಆವೃತ್ತಿ(cnv) ((cnv)->options&UCNV_OPTION_ಆವೃತ್ತಿ)

U_CDECL_BEGIN /* We must declare the following as 'extern "C"' so that if ucnv
                 itself is compiled under C++, the linkage of the funcptrs will
                 work.
              */

ಒಕ್ಕೂಟ UConverter_ಕೋಷ್ಟಕ {
    UConverterMBCSTable mbcs;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ UConverter_ಕೋಷ್ಟಕ UConverter_ಕೋಷ್ಟಕ;

ರಚನೆ UConverterImpl;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UConverterImpl UConverterImpl;

/** values for the unicodeMask */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_HAS_SUPPLEMENTARY 1
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_HAS_SURROGATES    2

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UConverter_Static_ದತ್ತಾಂಶ {   /* +offset: size */
    uint32_t struct_ಗಾತ್ರ;                /* +0: 4 Size of this structure */

    ಅಕ್ಷರ name
      [UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ]; /* +4: 60  internal name of the converter- invariant chars */

    int32_t codepage;               /* +64: 4 codepage # (now IBM-$codepage) */

    int8_t platform;                /* +68: 1 platform of the converter (only IBM now) */
    int8_t conversionType;          /* +69: 1 conversion type */

    int8_t min_ಬೈಟುಗಳು_ಪ್ರತಿ_Char;         /* +70: 1 Minimum # bytes per char in this codepage */
    int8_t max_ಬೈಟುಗಳು_ಪ್ರತಿ_Char;         /* +71: 1 Maximum # bytes output per UChar in this codepage */

    uint8_t subChar[UCNV_MAX_SUBCHAR_LEN]; /* +72: 4  [note:  4 and 8 byte boundary] */
    int8_t subCharLen;              /* +76: 1 */

    uint8_t has_ಗಮ್ಯ_Unicode_Fallback;   /* +77: 1 UBool needs to be changed to UBool to be consistent across platform */
    uint8_t hasFromUnicodeFallback; /* +78: 1 */
    uint8_t unicode_ಮುಖವಾಡ;            /* +79: 1  bit 0: has supplementary  bit 1: has single surrogates */
    uint8_t subChar1;               /* +80: 1  single-byte substitution character for IBM MBCS (0 if none) */
    uint8_t reserved[19];           /* +81: 19 to round out the structure */
                                    /* total size: 100 */
} UConverter_Static_ದತ್ತಾಂಶ;

/*
 * Defines the UConverterSharedData struct,
 * the immutable, shared part of UConverter.
 */
ರಚನೆ UConverter_Shared_ದತ್ತಾಂಶ {
    uint32_t struct_ಗಾತ್ರ;            /* Size of this structure */
    uint32_t referenceCounter;      /* used to count number of clients, unused for static/immutable SharedData */

    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_ಸ್ಮರಣೆ;         /* from udata_openChoice() - for cleanup */

    ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ *static_ದತ್ತಾಂಶ; /* pointer to the static (non changing) data. */

    UBool                shared_ದತ್ತಾಂಶ_Cached;   /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    /** If false, then referenceCounter is not used. Must not change after initialization. */
    UBool isReferenceCounted;

    ಸ್ಥಿರಾಂಕ UConverterImpl *impl;     /* vtable-style struct of mostly function pointers */

    /*initial values of some members of the mutable part of object */
    uint32_t ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;

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
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(p_Static_ದತ್ತಾಂಶ, pImpl) \
    { \
        ಗಾತ್ರ(UConverter_Shared_ದತ್ತಾಂಶ), ~((uint32_t)0), \
        ಖಾಲಿ, p_Static_ದತ್ತಾಂಶ, false, false, pImpl, \
        0, UCNV_MBCS_ಕೋಷ್ಟಕ_INITIALIZER \
    }

/* Defines a UConverter, the lightweight mutable part the user sees */

ರಚನೆ UConverter {
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_fromUnicode call
     */
    ಶೂನ್ಯ (U_EXPORT2 *fromUCharErrorBehaviour) (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                                     UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                     ಸ್ಥಿರಾಂಕ UChar *ಸಂಕೇತ_Units,
                                     int32_t ಉದ್ದಳತೆ,
                                     UChar32 ಸಂಕೇತ_ಬಿಂದು,
                                     UConverterCallbackReason reason,
                                     UError_ಸಂಕೇತ *);
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_toUnicode call
     */
    ಶೂನ್ಯ (U_EXPORT2 *fromCharErrorBehaviour) (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                                    UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂಕೇತ_Units,
                                    int32_t ಉದ್ದಳತೆ,
                                    UConverterCallbackReason reason,
                                    UError_ಸಂಕೇತ *);

    /*
     * Pointer to additional data that depends on the converter type.
     * Used by ISO 2022, SCSU, GB 18030 converters, possibly more.
     */
    ಶೂನ್ಯ *extraInfo;

    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fromUContext;
    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಗಮ್ಯ_UContext;

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

    UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ;   /* Pointer to the shared immutable part of the converter object */

    uint32_t options; /* options flags from UConverterOpen, may contain additional bits */

    UBool shared_ದತ್ತಾಂಶ_Is_Cached;  /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    UBool isCopyLocal;  /* true if UConverter is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */
    UBool isExtraLocal; /* true if extraInfo is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */

    UBool  useFallback;
    int8_t ಗಮ್ಯ_ULength;                   /* number of bytes in toUBytes */
    uint8_t ಗಮ್ಯ_UBytes[UCNV_MAX_CHAR_LEN-1];/* more "toU status"; keeps the bytes of the current character */
    uint32_t ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;           /* Used to internalize stream status information */
    int32_t ಬಹುಲಕ;
    uint32_t from_Unicode_ಸ್ಥಿತಿ;

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
    int8_t max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar;

    int8_t subCharLen;                  /* length of the codepage specific character sequence */
    int8_t invalid_Char_ಉದ್ದಳತೆ;
    int8_t char_Error_ಬಫರ್_ಉದ್ದಳತೆ;       /* number of valid bytes in charErrorBuffer */

    int8_t invalid_UChar_ಉದ್ದಳತೆ;
    int8_t UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ;      /* number of valid UChars in charErrorBuffer */

    uint8_t subChar1;                                   /* single-byte substitution character if different from subChar */
    UBool useSubChar1;
    ಅಕ್ಷರ invalid_Char_ಬಫರ್[UCNV_MAX_CHAR_LEN];          /* bytes from last error/callback situation */
    uint8_t char_Error_ಬಫರ್[UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ];  /* codepage output from Error functions */
    UChar subUChars[UCNV_MAX_SUBCHAR_LEN/U_SIZEOF_UCHAR]; /* see subChars documentation */

    UChar invalid_UChar_ಬಫರ್[U16_MAX_ಉದ್ದಳತೆ];           /* UChars from last error/callback situation */
    UChar UChar_Error_ಬಫರ್[UCNV_ERROR_ಬಫರ್_ಉದ್ದಳತೆ];   /* unicode output from Error functions */

    /* fields for conversion extension */

    /* store previous UChars/chars to continue partial matches */
    UChar32 preFromUFirstCP;                /* >=0: partial match */
    UChar preFromU[UCNV_EXT_MAX_UCHARS];
    ಅಕ್ಷರ pre_ಗಮ್ಯ_U[UCNV_EXT_MAX_ಬೈಟುಗಳು];
    int8_t preFromULength, pre_ಗಮ್ಯ_ULength;    /* negative: replay */
    int8_t pre_ಗಮ್ಯ_UFirst_ಉದ್ದಳತೆ;               /* length of first character */

    /* new fields for ICU 4.0 */
    UConverterCallbackReason ಗಮ್ಯ_UCallback_Reason; /* (*fromCharErrorBehaviour) reason, set when error is detected */
};

U_CDECL_ಕೊನೆ /* end of UConverter */

#ವ್ಯಾಖ್ಯಾನಿಸು CONVERTER_FILE_EXTENSION ".cnv"


/**
 * Return the number of all converter names.
 * @param pErrorCode The error code
 * @return the number of all converter names
 */
U_CFUNC uint16_t
ucnv_bld_ಎಣಿಕೆ_ಲಭ್ಯ_Converters(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * Return the (n)th converter name in mixed case, or NULL
 * if there is none (typically, if the data cannot be loaded).
 * 0<=index<ucnv_io_countAvailableConverters().
 * @param n The number specifies which converter name to get
 * @param pErrorCode The error code
 * @return the (n)th converter name in mixed case, or NULL if there is none.
 */
U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
ucnv_bld_get_ಲಭ್ಯ_Converter(uint16_t n, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * Load a non-algorithmic converter.
 * If pkg==NULL, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI UConverter_Shared_ದತ್ತಾಂಶ *
ucnv_ತುಂಬು(UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *err);

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI ಶೂನ್ಯ
ucnv_unload(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ);

/**
 * Swap ICU .cnv conversion tables. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t ಉದ್ದಳತೆ, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_enableCleanup(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* _UCNV_BLD */
