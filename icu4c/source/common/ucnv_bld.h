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

#اگر_غیر_تعریف_شدہ UCNV_BLD_H
#تعریف_کرو UCNV_BLD_H

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/ucnv_err.h"
#شامل "unicode/utf16.h"
#شامل "ucnv_cnv.h"
#شامل "ucnvmbcs.h"
#شامل "ucnv_ext.h"
#شامل "udataswp.h"

/* size of the overflow buffers in UConverter, enough for escaping callbacks */
#تعریف_کرو UCNV_ERROR_بفر_لمبائی 32

/* at most 4 bytes per substitution character (part of .cnv file format! see UConverterStaticData) */
#تعریف_کرو UCNV_MAX_SUBCHAR_LEN 4

/* at most 8 bytes per character in toUBytes[] (UTF-8 uses up to 6) */
#تعریف_کرو UCNV_MAX_CHAR_LEN 8

/* converter options bits */
#تعریف_کرو UCNV_OPTION_نسخہ     0xf
#تعریف_کرو UCNV_OPTION_تبادلہ_LFNL   0x10

#تعریف_کرو UCNV_GET_نسخہ(cnv) ((cnv)->options&UCNV_OPTION_نسخہ)

U_CDECL_BEGIN /* We must declare the following as 'extern "C"' so that if ucnv
                 itself is compiled under C++, the linkage of the funcptrs will
                 work.
              */

اتحاد UConverter_جدول {
    UConverterMBCSTable mbcs;
};

قسمتعریف اتحاد UConverter_جدول UConverter_جدول;

ساخت UConverterImpl;
قسمتعریف ساخت UConverterImpl UConverterImpl;

/** values for the unicodeMask */
#تعریف_کرو UCNV_HAS_SUPPLEMENTARY 1
#تعریف_کرو UCNV_HAS_SURROGATES    2

قسمتعریف ساخت UConverter_Static_ڈیٹا {   /* +offset: size */
    uint32_t struct_سائز;                /* +0: 4 Size of this structure */

    حرف name
      [UCNV_MAX_CONVERTER_NAME_لمبائی]; /* +4: 60  internal name of the converter- invariant chars */

    int32_t codepage;               /* +64: 4 codepage # (now IBM-$codepage) */

    int8_t platform;                /* +68: 1 platform of the converter (only IBM now) */
    int8_t conversionType;          /* +69: 1 conversion type */

    int8_t min_بائٹیں_فی_Char;         /* +70: 1 Minimum # bytes per char in this codepage */
    int8_t max_بائٹیں_فی_Char;         /* +71: 1 Maximum # bytes output per UChar in this codepage */

    uint8_t subChar[UCNV_MAX_SUBCHAR_LEN]; /* +72: 4  [note:  4 and 8 byte boundary] */
    int8_t subCharLen;              /* +76: 1 */

    uint8_t has_مقصد_Unicode_Fallback;   /* +77: 1 UBool needs to be changed to UBool to be consistent across platform */
    uint8_t hasFromUnicodeFallback; /* +78: 1 */
    uint8_t unicode_نقاب;            /* +79: 1  bit 0: has supplementary  bit 1: has single surrogates */
    uint8_t subChar1;               /* +80: 1  single-byte substitution character for IBM MBCS (0 if none) */
    uint8_t reserved[19];           /* +81: 19 to round out the structure */
                                    /* total size: 100 */
} UConverter_Static_ڈیٹا;

/*
 * Defines the UConverterSharedData struct,
 * the immutable, shared part of UConverter.
 */
ساخت UConverter_Shared_ڈیٹا {
    uint32_t struct_سائز;            /* Size of this structure */
    uint32_t reference_شمار;      /* used to count number of clients, unused for static/immutable SharedData */

    ثابت خالی *ڈیٹا_حافظہ;         /* from udata_openChoice() - for cleanup */

    ثابت UConverter_Static_ڈیٹا *static_ڈیٹا; /* pointer to the static (non changing) data. */

    UBool                shared_ڈیٹا_Cached;   /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    /** If false, then referenceCounter is not used. Must not change after initialization. */
    UBool isReferenceCounted;

    ثابت UConverterImpl *impl;     /* vtable-style struct of mostly function pointers */

    /*initial values of some members of the mutable part of object */
    uint32_t مقصد_Unicode_حالت;

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
#تعریف_کرو UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(p_Static_ڈیٹا, pImpl) \
    { \
        حجم(UConverter_Shared_ڈیٹا), ~((uint32_t)0), \
        تہی, p_Static_ڈیٹا, false, false, pImpl, \
        0, UCNV_MBCS_جدول_INITIALIZER \
    }

/* Defines a UConverter, the lightweight mutable part the user sees */

ساخت UConverter {
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_fromUnicode call
     */
    خالی (U_EXPORT2 *fromUCharErrorBehaviour) (ثابت خالی *context,
                                     UConverter_From_Unicode_دلائل *دلائل_2,
                                     ثابت UChar *رمز_Units,
                                     int32_t لمبائی,
                                     UChar32 رمز_نقطہ,
                                     UConverterCallbackReason reason,
                                     UError_رمز *);
    /*
     * Error function pointer called when conversion issues
     * occur during a ucnv_toUnicode call
     */
    خالی (U_EXPORT2 *fromCharErrorBehaviour) (ثابت خالی *context,
                                    UConverter_مقصد_Unicode_دلائل *دلائل_2,
                                    ثابت حرف *رمز_Units,
                                    int32_t لمبائی,
                                    UConverterCallbackReason reason,
                                    UError_رمز *);

    /*
     * Pointer to additional data that depends on the converter type.
     * Used by ISO 2022, SCSU, GB 18030 converters, possibly more.
     */
    خالی *extraInfo;

    ثابت خالی *fromUContext;
    ثابت خالی *مقصد_UContext;

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

    UConverter_Shared_ڈیٹا *shared_ڈیٹا;   /* Pointer to the shared immutable part of the converter object */

    uint32_t options; /* options flags from UConverterOpen, may contain additional bits */

    UBool shared_ڈیٹا_Is_Cached;  /* true:  shared data is in cache, don't destroy on ucnv_close() if 0 ref.  false: shared data isn't in the cache, do attempt to clean it up if the ref is 0 */
    UBool isCopyLocal;  /* true if UConverter is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */
    UBool isExtraLocal; /* true if extraInfo is not owned and not released in ucnv_close() (stack-allocated, safeClone(), etc.) */

    UBool  useFallback;
    int8_t مقصد_ULength;                   /* number of bytes in toUBytes */
    uint8_t مقصد_UBytes[UCNV_MAX_CHAR_LEN-1];/* more "toU status"; keeps the bytes of the current character */
    uint32_t مقصد_Unicode_حالت;           /* Used to internalize stream status information */
    int32_t منوال;
    uint32_t from_Unicode_حالت;

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
    int8_t max_بائٹیں_فی_UChar;

    int8_t subCharLen;                  /* length of the codepage specific character sequence */
    int8_t invalid_Char_لمبائی;
    int8_t char_Error_بفر_لمبائی;       /* number of valid bytes in charErrorBuffer */

    int8_t invalid_UChar_لمبائی;
    int8_t UChar_Error_بفر_لمبائی;      /* number of valid UChars in charErrorBuffer */

    uint8_t subChar1;                                   /* single-byte substitution character if different from subChar */
    UBool useSubChar1;
    حرف invalid_Char_بفر[UCNV_MAX_CHAR_LEN];          /* bytes from last error/callback situation */
    uint8_t char_Error_بفر[UCNV_ERROR_بفر_لمبائی];  /* codepage output from Error functions */
    UChar subUChars[UCNV_MAX_SUBCHAR_LEN/U_SIZEOF_UCHAR]; /* see subChars documentation */

    UChar invalid_UChar_بفر[U16_MAX_لمبائی];           /* UChars from last error/callback situation */
    UChar UChar_Error_بفر[UCNV_ERROR_بفر_لمبائی];   /* unicode output from Error functions */

    /* fields for conversion extension */

    /* store previous UChars/chars to continue partial matches */
    UChar32 preFromUFirstCP;                /* >=0: partial match */
    UChar preFromU[UCNV_EXT_MAX_UCHARS];
    حرف pre_مقصد_U[UCNV_EXT_MAX_بائٹیں];
    int8_t preFromULength, pre_مقصد_ULength;    /* negative: replay */
    int8_t pre_مقصد_UFirst_لمبائی;               /* length of first character */

    /* new fields for ICU 4.0 */
    UConverterCallbackReason مقصد_UCallback_Reason; /* (*fromCharErrorBehaviour) reason, set when error is detected */
};

U_CDECL_انتہا /* end of UConverter */

#تعریف_کرو CONVERTER_FILE_EXTENSION ".cnv"


/**
 * Return the number of all converter names.
 * @param pErrorCode The error code
 * @return the number of all converter names
 */
U_CFUNC uint16_t
ucnv_bld_گنتی_دستیاب_Converters(UError_رمز *p_Error_رمز);

/**
 * Return the (n)th converter name in mixed case, or NULL
 * if there is none (typically, if the data cannot be loaded).
 * 0<=index<ucnv_io_countAvailableConverters().
 * @param n The number specifies which converter name to get
 * @param pErrorCode The error code
 * @return the (n)th converter name in mixed case, or NULL if there is none.
 */
U_CFUNC ثابت حرف *
ucnv_bld_get_دستیاب_Converter(uint16_t n, UError_رمز *p_Error_رمز);

/**
 * Load a non-algorithmic converter.
 * If pkg==NULL, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI UConverter_Shared_ڈیٹا *
ucnv_بھرو(UConverter_بھرو_دلائل *p_دلائل, UError_رمز *err);

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI خالی
ucnv_unload(UConverter_Shared_ڈیٹا *shared_ڈیٹا);

/**
 * Swap ICU .cnv conversion tables. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_تبادلہ(ثابت UDataSwapper *ds,
          ثابت خالی *اندر_ڈیٹا, int32_t لمبائی, خالی *برآمد_ڈیٹا,
          UError_رمز *p_Error_رمز);

U_CAPI خالی U_EXPORT2
ucnv_enableCleanup(خالی);

#اگر_ختم

#اگر_ختم /* _UCNV_BLD */
