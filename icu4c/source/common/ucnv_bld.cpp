// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 ********************************************************************
 * COPYRIGHT:
 * Copyright (c) 1996-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************
 *
 *  ucnv_bld.cpp:
 *
 *  Defines functions that are used in the creation/initialization/deletion
 *  of converters and related structures.
 *  uses uconv_io.h routines to access disk information
 *  is used by ucnv.h to implement public API create/delete/flushCache routines
 * Modification History:
 *
 *   Date        Name        Description
 *
 *   06/20/2000  helena      OS/400 port changes; mostly typecast.
 *   06/29/2000  helena      Major rewrite of the callback interface.
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_io.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnvmbcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_ext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_cnv.h"


#ਜੇ 0
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
ਬਾਹਰੀ ਖਾਲੀ UCNV_DEBUG_LOG(ਅੱਖਰ *what, ਅੱਖਰ *who, ਖਾਲੀ *p, ਪੂਰਨ l);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#ਨਹੀਂਤਾਂ
# define UCNV_DEBUG_LOG(x,y,z)
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ * ਅਚਲ
converter_ਅੰਕੜੇ[UCNV_ਸੰਖਿਆ_ਦਾ_SUPPORTED_CONVERTER_TYPES]={
    ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ,

#ਜੇ UCONFIG_NO_LEGACY_CONVERSION
    ਖਾਲੀ_ਸੂਚਕ,
#ਨਹੀਂਤਾਂ
    &_MBCSData,
#ਜੇ_ਅੰਤ

    &Latin1_ਅੰਕੜੇ,
    &UTF8_ਅੰਕੜੇ, &_UTF16BEData, &_UTF16LEData,
#ਜੇ UCONFIG_ONLY_HTML_CONVERSION
    ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ,
#ਨਹੀਂਤਾਂ
    &_UTF32BEData, &_UTF32LEData,
#ਜੇ_ਅੰਤ
    ਖਾਲੀ_ਸੂਚਕ,

#ਜੇ UCONFIG_NO_LEGACY_CONVERSION
    ਖਾਲੀ_ਸੂਚਕ,
#ਨਹੀਂਤਾਂ
    &ISO2022_ਅੰਕੜੇ,
#ਜੇ_ਅੰਤ

#ਜੇ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
#ਨਹੀਂਤਾਂ
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#ਜੇ_ਅੰਤ

#ਜੇ UCONFIG_ONLY_HTML_CONVERSION
    ਖਾਲੀ_ਸੂਚਕ,
#ਨਹੀਂਤਾਂ
    &_SCSUData,
#ਜੇ_ਅੰਤ


#ਜੇ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    ਖਾਲੀ_ਸੂਚਕ,
#ਨਹੀਂਤਾਂ
    &_ISCIIData,
#ਜੇ_ਅੰਤ

    &_ASCIIData,
#ਜੇ UCONFIG_ONLY_HTML_CONVERSION
    ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &UTF16_ਅੰਕੜੇ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ,
#ਨਹੀਂਤਾਂ
    &UTF7_ਅੰਕੜੇ, &Bocu1_ਅੰਕੜੇ, &UTF16_ਅੰਕੜੇ, &UTF32_ਅੰਕੜੇ, &CESU8_ਅੰਕੜੇ, &_IMAPData,
#ਜੇ_ਅੰਤ

#ਜੇ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    ਖਾਲੀ_ਸੂਚਕ,
#ਨਹੀਂਤਾਂ
    &Compound_ਪਾਠ_ਅੰਕੜੇ
#ਜੇ_ਅੰਤ
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
ਸਥਿਰ ਬਣਤਰ {
  ਅਚਲ ਅੱਖਰ *name;
  ਅਚਲ UConverterType type;
} ਅਚਲ cnvNameType[] = {
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#ਜੇ_ਅੰਤ
#ਜੇ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#ਜੇ_ਅੰਤ
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#ਜੇ_ਅੰਤ
#ਜੇ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#ਜੇ_ਅੰਤ
#ਜੇ !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#ਜੇ_ਅੰਤ
  { "iso88591", UCNV_LATIN_1 },
#ਜੇ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "lmbcs1", UCNV_LMBCS_1 },
  { "lmbcs11",UCNV_LMBCS_11 },
  { "lmbcs16",UCNV_LMBCS_16 },
  { "lmbcs17",UCNV_LMBCS_17 },
  { "lmbcs18",UCNV_LMBCS_18 },
  { "lmbcs19",UCNV_LMBCS_19 },
  { "lmbcs2", UCNV_LMBCS_2 },
  { "lmbcs3", UCNV_LMBCS_3 },
  { "lmbcs4", UCNV_LMBCS_4 },
  { "lmbcs5", UCNV_LMBCS_5 },
  { "lmbcs6", UCNV_LMBCS_6 },
  { "lmbcs8", UCNV_LMBCS_8 },
#ਜੇ_ਅੰਤ
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#ਜੇ_ਅੰਤ
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#ਜੇ U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#ਨਹੀਂਤਾਂ
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#ਜੇ_ਅੰਤ
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#ਜੇ U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#ਨਹੀਂਤਾਂ
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#ਜੇ_ਅੰਤ
  { "utf8", UCNV_UTF8 },
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_ਪਾਠ}
#ਜੇ_ਅੰਤ
};


/*initializes some global variables */
ਸਥਿਰ UHashtable *SHARED_ਅੰਕੜੇ_HASHTABLE = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ਸਥਿਰ ਅਚਲ ਅੱਖਰ **g_ਉਪਲਬਧ_Converters = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ uint16_t g_ਉਪਲਬਧ_Converter_ਗਿਣਤੀ = 0;
ਸਥਿਰ icu::UInitOnce g_ਉਪਲਬਧ_Converters_Init_Once {};

#ਜੇ !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ਸਥਿਰ ਅੱਖਰ g_Default_Converter_Name_ਬਫਰ[UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ + 1]; /* +1 for nullptr */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *gDefaultConverterName = ਖਾਲੀ_ਸੂਚਕ;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ਸਥਿਰ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *g_Default_Algorithmic_Shared_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ਸਥਿਰ UBool g_Default_Converter_ਸ਼ਾਮਲ_ਹੈ_Option;

#ਜੇ_ਅੰਤ  /* !U_CHARSET_IS_UTF8 */

ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਅੰਕੜੇ_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ਸਥਿਰ ਖਾਲੀ
ucnv_flush_ਉਪਲਬਧ_Converter_Cache() {
    g_ਉਪਲਬਧ_Converter_ਗਿਣਤੀ = 0;
    ਜੇ (g_ਉਪਲਬਧ_Converters) {
        uprv_free(const_cast<ਅੱਖਰ**>(g_ਉਪਲਬਧ_Converters));
        g_ਉਪਲਬਧ_Converters = ਖਾਲੀ_ਸੂਚਕ;
    }
    g_ਉਪਲਬਧ_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ਸਥਿਰ UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    ਜੇ (SHARED_ਅੰਕੜੇ_HASHTABLE != ਖਾਲੀ_ਸੂਚਕ && uhash_ਗਿਣਤੀ(SHARED_ਅੰਕੜੇ_HASHTABLE) == 0) {
        uhash_close(SHARED_ਅੰਕੜੇ_HASHTABLE);
        SHARED_ਅੰਕੜੇ_HASHTABLE = ਖਾਲੀ_ਸੂਚਕ;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_ਉਪਲਬਧ_Converter_Cache();

#ਜੇ !U_CHARSET_IS_UTF8
    gDefaultConverterName = ਖਾਲੀ_ਸੂਚਕ;
    g_Default_Converter_Name_ਬਫਰ[0] = 0;
    g_Default_Converter_ਸ਼ਾਮਲ_ਹੈ_Option = ਝੂਠ;
    g_Default_Algorithmic_Shared_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ (SHARED_ਅੰਕੜੇ_HASHTABLE == ਖਾਲੀ_ਸੂਚਕ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

ਸਥਿਰ UBool U_CALLCONV
isCnvAcceptable(ਖਾਲੀ * /*context*/,
                ਅਚਲ ਅੱਖਰ * /*type*/, ਅਚਲ ਅੱਖਰ * /*name*/,
                ਅਚਲ UDataInfo *pInfo) {
    ਵਾਪਸ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->ਅੰਕੜੇ_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x6e &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x76 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x74 &&
        pInfo->format_ਸੰਸਕਰਣ[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
ਸਥਿਰ UConverter_Shared_ਅੰਕੜੇ*
ucnv_ਅੰਕੜੇ_un_Flatten_Clone(UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ, UError_ਕੋਡ *ਸਥਿਤੀ)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ਅਚਲ uint8_t* raw = static_cast<ਅਚਲ uint8_t*>(udata_get_ਸਮ੍ਰਿਤੀ(p_ਅੰਕੜੇ));
    ਅਚਲ UConverter_Static_ਅੰਕੜੇ* ਸਰੋਤ = reinterpret_cast<ਅਚਲ UConverter_Static_ਅੰਕੜੇ*>(raw);
    UConverter_Shared_ਅੰਕੜੇ *ਅੰਕੜੇ;
    UConverterType type = static_cast<UConverterType>(ਸਰੋਤ->conversionType);

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ))
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (static_cast<uint16_t>(type) >= UCNV_ਸੰਖਿਆ_ਦਾ_SUPPORTED_CONVERTER_TYPES ||
        converter_ਅੰਕੜੇ[type] == ਖਾਲੀ_ਸੂਚਕ ||
        !converter_ਅੰਕੜੇ[type]->isReferenceCounted ||
        converter_ਅੰਕੜੇ[type]->referenceCounter != 1 ||
        ਸਰੋਤ->struct_ਮਾਪ != ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ))
    {
        *ਸਥਿਤੀ = U_INVALID_ਸਾਰਣੀ_FORMAT;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਅੰਕੜੇ = static_cast<UConverter_Shared_ਅੰਕੜੇ*>(uprv_malloc(ਆਕਾਰ(UConverter_Shared_ਅੰਕੜੇ)));
    ਜੇ(ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(ਅੰਕੜੇ, converter_ਅੰਕੜੇ[type], ਆਕਾਰ(UConverter_Shared_ਅੰਕੜੇ));

    ਅੰਕੜੇ->static_ਅੰਕੜੇ = ਸਰੋਤ;

    ਅੰਕੜੇ->shared_ਅੰਕੜੇ_Cached = ਝੂਠ;

    /* fill in fields from the loaded data */
    ਅੰਕੜੇ->ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ = (ਖਾਲੀ*)p_ਅੰਕੜੇ; /* for future use */

    ਜੇ(ਅੰਕੜੇ->impl->load != ਖਾਲੀ_ਸੂਚਕ) {
        ਅੰਕੜੇ->impl->load(ਅੰਕੜੇ, p_ਦਲੀਲਾਂ, raw + ਸਰੋਤ->struct_ਮਾਪ, ਸਥਿਤੀ);
        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
            uprv_free(ਅੰਕੜੇ);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }
    ਵਾਪਸ ਅੰਕੜੇ;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ਸਥਿਰ UConverter_Shared_ਅੰਕੜੇ *createConverterFromFile(UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ * err)
{
    UData_ਸਮ੍ਰਿਤੀ *ਅੰਕੜੇ;
    UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ;

    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_ਲੱਦੋ);

    ਜੇ (U_FAILURE (*err)) {
        UTRACE_EXIT_ਸਥਿਤੀ(*err);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_ਦਲੀਲਾਂ->name, p_ਦਲੀਲਾਂ->pkg);

    ਅੰਕੜੇ = udata_openChoice(p_ਦਲੀਲਾਂ->pkg, ਅੰਕੜੇ_TYPE, p_ਦਲੀਲਾਂ->name, isCnvAcceptable, ਖਾਲੀ_ਸੂਚਕ, err);
    ਜੇ(U_FAILURE(*err))
    {
        UTRACE_EXIT_ਸਥਿਤੀ(*err);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    shared_ਅੰਕੜੇ = ucnv_ਅੰਕੜੇ_un_Flatten_Clone(p_ਦਲੀਲਾਂ, ਅੰਕੜੇ, err);
    ਜੇ(U_FAILURE(*err))
    {
        udata_close(ਅੰਕੜੇ);
        UTRACE_EXIT_ਸਥਿਤੀ(*err);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_ਸਥਿਤੀ(shared_ਅੰਕੜੇ, *err);
    ਵਾਪਸ shared_ਅੰਕੜੇ;
}

/*returns a converter type from a string
 */
ਸਥਿਰ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *
getAlgorithmicTypeFromName(ਅਚਲ ਅੱਖਰ *realName)
{
    uint32_t mid, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;
    uint32_t ਆਖਰੀ_Mid;
    ਪੂਰਨ ਨਤੀਜਾ;
    ਅੱਖਰ strippedName[UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    ਸ਼ੁਰੂ = 0;
    ਹੱਦਬੰਦੀ = UPRV_LENGTHOF(cnvNameType);
    mid = ਹੱਦਬੰਦੀ;
    ਆਖਰੀ_Mid = UINT32_MAX;

    ਲਈ (;;) {
        mid = (ਸ਼ੁਰੂ + ਹੱਦਬੰਦੀ) / 2;
        ਜੇ (ਆਖਰੀ_Mid == mid) {   /* Have we moved? */
            ਰੋਕੋ;  /* We haven't moved, and it wasn't found. */
        }
        ਆਖਰੀ_Mid = mid;
        ਨਤੀਜਾ = uprv_strcmp(strippedName, cnvNameType[mid].name);

        ਜੇ (ਨਤੀਜਾ < 0) {
            ਹੱਦਬੰਦੀ = mid;
        } ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ > 0) {
            ਸ਼ੁਰੂ = mid;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ converter_ਅੰਕੜੇ[cnvNameType[mid].type];
        }
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_CACHE_ਲੱਦੋ_ਗੁਣਕ 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
ਸਥਿਰ ਖਾਲੀ
ucnv_share_Converter_ਅੰਕੜੇ(UConverter_Shared_ਅੰਕੜੇ * ਅੰਕੜੇ)
{
    UError_ਕੋਡ err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    ਜੇ (SHARED_ਅੰਕੜੇ_HASHTABLE == ਖਾਲੀ_ਸੂਚਕ)
    {
        SHARED_ਅੰਕੜੇ_HASHTABLE = uhash_open_ਮਾਪ(uhash_hashChars, uhash_compareChars, ਖਾਲੀ_ਸੂਚਕ,
                            ucnv_io_ਗਿਣਤੀ_ਗਿਆਤ_Converters(&err)*UCNV_CACHE_ਲੱਦੋ_ਗੁਣਕ,
                            &err);
        ucnv_enableCleanup();

        ਜੇ (U_FAILURE(err))
            ਵਾਪਸ;
    }

    /* ### check to see if the element is not already there! */

    /*
    sanity =   ucnv_getSharedConverterData (data->staticData->name);
    if(sanity != nullptr)
    {
    UCNV_DEBUG_LOG("put:overwrite!",data->staticData->name,sanity);
    }
    UCNV_DEBUG_LOG("put:chk",data->staticData->name,sanity);
    */

    /* Mark it shared */
    ਅੰਕੜੇ->shared_ਅੰਕੜੇ_Cached = ਸੱਚ;

    uhash_put(SHARED_ਅੰਕੜੇ_HASHTABLE,
            (ਖਾਲੀ*) ਅੰਕੜੇ->static_ਅੰਕੜੇ->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            ਅੰਕੜੇ,
            &err);
    UCNV_DEBUG_LOG("put", ਅੰਕੜੇ->static_ਅੰਕੜੇ->name,ਅੰਕੜੇ);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
ਸਥਿਰ UConverter_Shared_ਅੰਕੜੇ *
ucnv_get_Shared_Converter_ਅੰਕੜੇ(ਅਚਲ ਅੱਖਰ *name)
{
    /*special case when no Table has yet been created we return nullptr */
    ਜੇ (SHARED_ਅੰਕੜੇ_HASHTABLE == ਖਾਲੀ_ਸੂਚਕ)
    {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਨਹੀਂਤਾਂ
    {
        UConverter_Shared_ਅੰਕੜੇ *rc;

        rc = static_cast<UConverter_Shared_ਅੰਕੜੇ*>(uhash_get(SHARED_ਅੰਕੜੇ_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        ਵਾਪਸ rc;
    }
}

/*frees the string of memory blocks associates with a sharedConverter
 *if and only if the referenceCounter == 0
 */
/* Deletes (frees) the Shared data it's passed. first it checks the referenceCounter to
 * see if anyone is using it, if not it frees all the memory stemming from sharedConverterData and
 * returns true,
 * otherwise returns false
 * @param sharedConverterData The shared data
 * @return if not it frees all the memory stemming from sharedConverterData and
 * returns true, otherwise returns false
 */
ਸਥਿਰ UBool
ucnv_delete_Shared_Converter_ਅੰਕੜੇ(UConverter_Shared_ਅੰਕੜੇ * dead_Shared_ਅੰਕੜੇ)
{
    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name, dead_Shared_ਅੰਕੜੇ);

    ਜੇ (dead_Shared_ਅੰਕੜੇ->referenceCounter > 0) {
        UTRACE_EXIT_ਮੁੱਲ((int32_t)ਝੂਠ);
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ (dead_Shared_ਅੰਕੜੇ->impl->unload != ਖਾਲੀ_ਸੂਚਕ) {
        dead_Shared_ਅੰਕੜੇ->impl->unload(dead_Shared_ਅੰਕੜੇ);
    }

    ਜੇ(dead_Shared_ਅੰਕੜੇ->ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ != ਖਾਲੀ_ਸੂਚਕ)
    {
        UData_ਸਮ੍ਰਿਤੀ *ਅੰਕੜੇ = (UData_ਸਮ੍ਰਿਤੀ*)dead_Shared_ਅੰਕੜੇ->ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ;
        udata_close(ਅੰਕੜੇ);
    }

    uprv_free(dead_Shared_ਅੰਕੜੇ);

    UTRACE_EXIT_ਮੁੱਲ((int32_t)ਸੱਚ);
    ਵਾਪਸ ਸੱਚ;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_ਅੰਕੜੇ *
ucnv_ਲੱਦੋ(UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *err) {
    UConverter_Shared_ਅੰਕੜੇ *my_Shared_Converter_ਅੰਕੜੇ;

    ਜੇ(err == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*err)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(p_ਦਲੀਲਾਂ->pkg != ਖਾਲੀ_ਸੂਚਕ && *p_ਦਲੀਲਾਂ->pkg != 0) {
        /* application-provided converters are not currently cached */
        ਵਾਪਸ createConverterFromFile(p_ਦਲੀਲਾਂ, err);
    }

    my_Shared_Converter_ਅੰਕੜੇ = ucnv_get_Shared_Converter_ਅੰਕੜੇ(p_ਦਲੀਲਾਂ->name);
    ਜੇ (my_Shared_Converter_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_ਅੰਕੜੇ = createConverterFromFile(p_ਦਲੀਲਾਂ, err);
        ਜੇ (U_FAILURE (*err) || (my_Shared_Converter_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ))
        {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਨਹੀਂਤਾਂ ਜੇ (!p_ਦਲੀਲਾਂ->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_ਅੰਕੜੇ(my_Shared_Converter_ਅੰਕੜੇ);
        }
    }
    ਨਹੀਂਤਾਂ
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_ਅੰਕੜੇ->referenceCounter++;
    }

    ਵਾਪਸ my_Shared_Converter_ਅੰਕੜੇ;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI ਖਾਲੀ
ucnv_unload(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ) {
    ਜੇ(shared_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (shared_ਅੰਕੜੇ->referenceCounter > 0) {
            shared_ਅੰਕੜੇ->referenceCounter--;
        }

        ਜੇ((shared_ਅੰਕੜੇ->referenceCounter <= 0)&&(shared_ਅੰਕੜੇ->shared_ਅੰਕੜੇ_Cached == ਝੂਠ)) {
            ucnv_delete_Shared_Converter_ਅੰਕੜੇ(shared_ਅੰਕੜੇ);
        }
    }
}

U_CFUNC ਖਾਲੀ
ucnv_unload_Shared_ਅੰਕੜੇ_If_Ready(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ)
{
    ਜੇ(shared_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ && shared_ਅੰਕੜੇ->isReferenceCounted) {
        umtx_ਤਾਲਾ(&cnvCacheMutex);
        ucnv_unload(shared_ਅੰਕੜੇ);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC ਖਾਲੀ
ucnv_increment_Ref_ਗਿਣਤੀ(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ)
{
    ਜੇ(shared_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ && shared_ਅੰਕੜੇ->isReferenceCounted) {
        umtx_ਤਾਲਾ(&cnvCacheMutex);
        shared_ਅੰਕੜੇ->referenceCounter++;
        umtx_unlock(&cnvCacheMutex);
    }
}

/*
 * *pPieces must be initialized.
 * The name without options will be copied to pPieces->cnvName.
 * The locale and options will be copied to pPieces only if present in inName,
 * otherwise the existing values in pPieces remain.
 * *pArgs will be set to the pPieces values.
 */
ਸਥਿਰ ਖਾਲੀ
parseConverterOptions(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                      UError_ਕੋਡ *err)
{
    ਅੱਖਰ *cnvName = pPieces->cnvName;
    ਅੱਖਰ c;
    int32_t len = 0;

    p_ਦਲੀਲਾਂ->name=ਵਿੱਚ_Name;
    p_ਦਲੀਲਾਂ->locale=pPieces->locale;
    p_ਦਲੀਲਾਂ->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    ਜਦੋਂ((c=*ਵਿੱਚ_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        ਜੇ (++len>=UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ) {
            *err = U_ILLEGAL_ਦਲੀਲ_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            ਵਾਪਸ;
        }
        *cnvName++=c;
        ਵਿੱਚ_Name++;
    }
    *cnvName=0;
    p_ਦਲੀਲਾਂ->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    ਜਦੋਂ((c=*ਵਿੱਚ_Name)!=0) {
        ਜੇ(c==UCNV_OPTION_SEP_CHAR) {
            ++ਵਿੱਚ_Name;
        }

        /* inName is behind an option separator */
        ਜੇ(uprv_strncmp(ਵਿੱਚ_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            ਅੱਖਰ *dest=pPieces->locale;

            /* copy the locale option value */
            ਵਿੱਚ_Name+=7;
            len=0;
            ਜਦੋਂ((c=*ਵਿੱਚ_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++ਵਿੱਚ_Name;

                ਜੇ(++len>=ULOC_FULLNAME_ਸਮਰੱਥਾ) {
                    *err=U_ILLEGAL_ਦਲੀਲ_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    ਵਾਪਸ;
                }

                *dest++=c;
            }
            *dest=0;
        } ਨਹੀਂਤਾਂ ਜੇ(uprv_strncmp(ਵਿੱਚ_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            ਵਿੱਚ_Name+=8;
            c=*ਵਿੱਚ_Name;
            ਜੇ(c==0) {
                p_ਦਲੀਲਾਂ->options=(pPieces->options&=~UCNV_OPTION_ਸੰਸਕਰਣ);
                ਵਾਪਸ;
            } ਨਹੀਂਤਾਂ ਜੇ (static_cast<uint8_t>(c - '0') < 10) {
                p_ਦਲੀਲਾਂ->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_ਸੰਸਕਰਣ) | static_cast<uint32_t>(c - '0');
                ++ਵਿੱਚ_Name;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(uprv_strncmp(ਵਿੱਚ_Name, "swaplfnl", 8)==0) {
            ਵਿੱਚ_Name+=8;
            p_ਦਲੀਲਾਂ->options=(pPieces->options|=UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } ਨਹੀਂਤਾਂ {
            /* ignore any other options until we define some */
            ਜਦੋਂ(((c = *ਵਿੱਚ_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            ਜੇ(c==0) {
                ਵਾਪਸ;
            }
        }
    }
}

/*Logic determines if the converter is Algorithmic AND/OR cached
 *depending on that:
 * -we either go to get data from disk and cache it (Data=true, Cached=false)
 * -Get it from a Hashtable (Data=X, Cached=true)
 * -Call dataConverter initializer (Data=true, Cached=true)
 * -Call AlgorithmicConverter initializer (Data=false, Cached=true)
 */
U_CFUNC UConverter_Shared_ਅੰਕੜੇ *
ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ(ਅਚਲ ਅੱਖਰ *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                    UError_ਕੋਡ * err) {
    UConverterNamePieces ਢੇਰ_Pieces;
    UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਢੇਰ_ਦਲੀਲਾਂ;
    UConverter_Shared_ਅੰਕੜੇ *my_Shared_Converter_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ ਅੰਦਰੂਨੀ_Error_ਕੋਡ = U_ZERO_ERROR;
    UBool mayContainOption = ਸੱਚ;
    UBool checkForAlgorithmic = ਸੱਚ;

    ਜੇ (U_FAILURE (*err)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(pPieces == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(p_ਦਲੀਲਾਂ != ਖਾਲੀ_ਸੂਚਕ) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        pPieces = &ਢੇਰ_Pieces;
    }
    ਜੇ(p_ਦਲੀਲਾਂ == ਖਾਲੀ_ਸੂਚਕ) {
        uprv_memset(&ਢੇਰ_ਦਲੀਲਾਂ, 0, ਆਕਾਰ(ਢੇਰ_ਦਲੀਲਾਂ));
        ਢੇਰ_ਦਲੀਲਾਂ.size = (int32_t)ਆਕਾਰ(ਢੇਰ_ਦਲੀਲਾਂ);
        p_ਦਲੀਲਾਂ = &ਢੇਰ_ਦਲੀਲਾਂ;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_ਦਲੀਲਾਂ->name = converterName;
    p_ਦਲੀਲਾਂ->locale = pPieces->locale;
    p_ਦਲੀਲਾਂ->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    ਜੇ (converterName == ਖਾਲੀ_ਸੂਚਕ) {
#ਜੇ U_CHARSET_IS_UTF8
        p_ਦਲੀਲਾਂ->name = "UTF-8";
        ਵਾਪਸ (UConverter_Shared_ਅੰਕੜੇ *)converter_ਅੰਕੜੇ[UCNV_UTF8];
#ਨਹੀਂਤਾਂ
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_ਦਲੀਲਾਂ->name = ucnv_getDefaultName();
        ਜੇ (p_ਦਲੀਲਾਂ->name == ਖਾਲੀ_ਸੂਚਕ) {
            *err = U_MISSING_RESOURCE_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        my_Shared_Converter_ਅੰਕੜੇ = (UConverter_Shared_ਅੰਕੜੇ *)g_Default_Algorithmic_Shared_ਅੰਕੜੇ;
        checkForAlgorithmic = ਝੂਠ;
        mayContainOption = g_Default_Converter_ਸ਼ਾਮਲ_ਹੈ_Option;
        /* the default converter name is already canonical */
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_ਦਲੀਲਾਂ->name = "UTF-8";
        ਵਾਪਸ (UConverter_Shared_ਅੰਕੜੇ *)converter_ਅੰਕੜੇ[UCNV_UTF8];
    }
    ਨਹੀਂਤਾਂ {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_ਦਲੀਲਾਂ, err);
        ਜੇ (U_FAILURE(*err)) {
            /* Very bad name used. */
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        /* get the canonical converter name */
        p_ਦਲੀਲਾਂ->name = ucnv_io_getConverterName(p_ਦਲੀਲਾਂ->name, &mayContainOption, &ਅੰਦਰੂਨੀ_Error_ਕੋਡ);
        ਜੇ (U_FAILURE(ਅੰਦਰੂਨੀ_Error_ਕੋਡ) || p_ਦਲੀਲਾਂ->name == ਖਾਲੀ_ਸੂਚਕ) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_ਦਲੀਲਾਂ->name = pPieces->cnvName;
        } ਨਹੀਂਤਾਂ ਜੇ (ਅੰਦਰੂਨੀ_Error_ਕੋਡ == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    ਜੇ(mayContainOption && p_ਦਲੀਲਾਂ->name != pPieces->cnvName) {
        parseConverterOptions(p_ਦਲੀਲਾਂ->name, pPieces, p_ਦਲੀਲਾਂ, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    ਜੇ (checkForAlgorithmic) {
        my_Shared_Converter_ਅੰਕੜੇ = (UConverter_Shared_ਅੰਕੜੇ *)getAlgorithmicTypeFromName(p_ਦਲੀਲਾਂ->name);
    }
    ਜੇ (my_Shared_Converter_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_ਦਲੀਲਾਂ->nestedLoads=1;
        p_ਦਲੀਲਾਂ->pkg=ਖਾਲੀ_ਸੂਚਕ;

        umtx_ਤਾਲਾ(&cnvCacheMutex);
        my_Shared_Converter_ਅੰਕੜੇ = ucnv_ਲੱਦੋ(p_ਦਲੀਲਾਂ, err);
        umtx_unlock(&cnvCacheMutex);
        ਜੇ (U_FAILURE (*err) || (my_Shared_Converter_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ))
        {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    ਵਾਪਸ my_Shared_Converter_ਅੰਕੜੇ;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ਅਚਲ ਅੱਖਰ* converterName,
                     UError_ਕੋਡ* err)
{
    UConverterNamePieces ਢੇਰ_Pieces;
    UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਢੇਰ_ਦਲੀਲਾਂ=UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER;
    UConverter_Shared_ਅੰਕੜੇ *my_Shared_Converter_ਅੰਕੜੇ;

    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_OPEN);

    ਜੇ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_ਅੰਕੜੇ = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ(converterName, &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, err);

        myUConverter = ucnv_create_Converter_From_Shared_ਅੰਕੜੇ(
            myUConverter, my_Shared_Converter_ਅੰਕੜੇ,
            &ਢੇਰ_ਦਲੀਲਾਂ,
            err);

        ਜੇ(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_ਸਥਿਤੀ(myUConverter, *err);
            ਵਾਪਸ myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_ਸਥਿਤੀ(*err);
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CFUNC UBool
ucnv_canCreateConverter(ਅਚਲ ਅੱਖਰ *converterName, UError_ਕੋਡ *err) {
    UConverter myUConverter;
    UConverterNamePieces ਢੇਰ_Pieces;
    UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਢੇਰ_ਦਲੀਲਾਂ=UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER;
    UConverter_Shared_ਅੰਕੜੇ *my_Shared_Converter_ਅੰਕੜੇ;

    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_OPEN);

    ਜੇ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ਢੇਰ_ਦਲੀਲਾਂ.onlyTestIsLoadable=ਸੱਚ;
        my_Shared_Converter_ਅੰਕੜੇ = ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ(converterName, &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, err);
        ucnv_create_Converter_From_Shared_ਅੰਕੜੇ(
            &myUConverter, my_Shared_Converter_ਅੰਕੜੇ,
            &ਢੇਰ_ਦਲੀਲਾਂ,
            err);
        ucnv_unload_Shared_ਅੰਕੜੇ_If_Ready(my_Shared_Converter_ਅੰਕੜੇ);
    }

    UTRACE_EXIT_ਸਥਿਤੀ(*err);
    ਵਾਪਸ U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ਅਚਲ ਅੱਖਰ* locale,
                                uint32_t options,
                                UError_ਕੋਡ* err) {
    UConverter *cnv;
    ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ;
    UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਢੇਰ_ਦਲੀਲਾਂ=UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER;

    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    ਜੇ(type<0 || UCNV_ਸੰਖਿਆ_ਦਾ_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        UTRACE_EXIT_ਸਥਿਤੀ(U_ILLEGAL_ਦਲੀਲ_ERROR);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    shared_ਅੰਕੜੇ = converter_ਅੰਕੜੇ[type];
    ਜੇ(shared_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ || shared_ਅੰਕੜੇ->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        UTRACE_EXIT_ਸਥਿਤੀ(U_ILLEGAL_ਦਲੀਲ_ERROR);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਢੇਰ_ਦਲੀਲਾਂ.name = "";
    ਢੇਰ_ਦਲੀਲਾਂ.options = options;
    ਢੇਰ_ਦਲੀਲਾਂ.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_ਅੰਕੜੇ(
            myUConverter, const_cast<UConverter_Shared_ਅੰਕੜੇ*>(shared_ਅੰਕੜੇ),
            &ਢੇਰ_ਦਲੀਲਾਂ, err);

    UTRACE_EXIT_PTR_ਸਥਿਤੀ(cnv, *err);
    ਵਾਪਸ cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ਅਚਲ ਅੱਖਰ *packageName, ਅਚਲ ਅੱਖਰ *converterName, UError_ਕੋਡ * err)
{
    UConverter *myUConverter;
    UConverter_Shared_ਅੰਕੜੇ *my_Shared_Converter_ਅੰਕੜੇ;
    UConverterNamePieces ਢੇਰ_Pieces;
    UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਢੇਰ_ਦਲੀਲਾਂ=UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER;

    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_OPEN_PACKAGE);

    ਜੇ(U_FAILURE(*err)) {
        UTRACE_EXIT_ਸਥਿਤੀ(*err);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ਢੇਰ_Pieces.cnvName[0] = 0;
    ਢੇਰ_Pieces.locale[0] = 0;
    ਢੇਰ_Pieces.options = 0;
    parseConverterOptions(converterName, &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, err);
    ਜੇ (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_ਸਥਿਤੀ(*err);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਢੇਰ_ਦਲੀਲਾਂ.nestedLoads=1;
    ਢੇਰ_ਦਲੀਲਾਂ.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_ਅੰਕੜੇ = createConverterFromFile(&ਢੇਰ_ਦਲੀਲਾਂ, err);

    ਜੇ (U_FAILURE(*err)) {
        UTRACE_EXIT_ਸਥਿਤੀ(*err);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_ਅੰਕੜੇ(ਖਾਲੀ_ਸੂਚਕ, my_Shared_Converter_ਅੰਕੜੇ, &ਢੇਰ_ਦਲੀਲਾਂ, err);

    ਜੇ (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_ਸਥਿਤੀ(*err);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UTRACE_EXIT_PTR_ਸਥਿਤੀ(myUConverter, *err);
    ਵਾਪਸ myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_ਅੰਕੜੇ(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_ਅੰਕੜੇ* my_Shared_Converter_ਅੰਕੜੇ,
                                   UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ* p_ਦਲੀਲਾਂ,
                                   UError_ਕੋਡ* err)
{
    UBool isCopyLocal;

    ਜੇ(U_FAILURE(*err)) {
        ucnv_unload_Shared_ਅੰਕੜੇ_If_Ready(my_Shared_Converter_ਅੰਕੜੇ);
        ਵਾਪਸ myUConverter;
    }
    ਜੇ(myUConverter == ਖਾਲੀ_ਸੂਚਕ)
    {
        myUConverter = (UConverter *) uprv_malloc (ਆਕਾਰ (UConverter));
        ਜੇ(myUConverter == ਖਾਲੀ_ਸੂਚਕ)
        {
            *err = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ucnv_unload_Shared_ਅੰਕੜੇ_If_Ready(my_Shared_Converter_ਅੰਕੜੇ);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        isCopyLocal = ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
        isCopyLocal = ਸੱਚ;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, ਆਕਾਰ(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_ਅੰਕੜੇ = my_Shared_Converter_ਅੰਕੜੇ;
    myUConverter->options = p_ਦਲੀਲਾਂ->options;
    ਜੇ(!p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_ਮੰਜ਼ਿਲ_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = my_Shared_Converter_ਅੰਕੜੇ->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
        myUConverter->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar = my_Shared_Converter_ਅੰਕੜੇ->static_ਅੰਕੜੇ->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char;
        myUConverter->subChar1 = my_Shared_Converter_ਅੰਕੜੇ->static_ਅੰਕੜੇ->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_ਅੰਕੜੇ->static_ਅੰਕੜੇ->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_ਅੰਕੜੇ->static_ਅੰਕੜੇ->subChar, myUConverter->subCharLen);
        myUConverter->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    ਜੇ(my_Shared_Converter_ਅੰਕੜੇ->impl->open != ਖਾਲੀ_ਸੂਚਕ) {
        my_Shared_Converter_ਅੰਕੜੇ->impl->open(myUConverter, p_ਦਲੀਲਾਂ, err);
        ਜੇ(U_FAILURE(*err) && !p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    ਵਾਪਸ myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_ਅੰਕੜੇ *my_Shared_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t pos;
    int32_t ਸਾਰਣੀ_Deleted_ਅੰਕ = 0;
    ਅਚਲ UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, ਬਚਿਆ;

    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    ਜੇ (SHARED_ਅੰਕੜੇ_HASHTABLE == ਖਾਲੀ_ਸੂਚਕ) {
        UTRACE_EXIT_ਮੁੱਲ((int32_t)0);
        ਵਾਪਸ 0;
    }

    /*creates an enumeration to iterate through every element in the
    * table
    *
    * Synchronization:  holding cnvCacheMutex will prevent any other thread from
    *                   accessing or modifying the hash table during the iteration.
    *                   The reference count of an entry may be decremented by
    *                   ucnv_close while the iteration is in process, but this is
    *                   benign.  It can't be incremented (in ucnv_createConverter())
    *                   because the sequence of looking up in the cache + incrementing
    *                   is protected by cnvCacheMutex.
    */
    umtx_ਤਾਲਾ(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    ਕਰੋ {
        ਬਚਿਆ = 0;
        pos = UHASH_ਪਹਿਲਾ;
        ਜਦੋਂ ((e = uhash_ਅਗਲਾ_Element (SHARED_ਅੰਕੜੇ_HASHTABLE, &pos)) != ਖਾਲੀ_ਸੂਚਕ)
        {
            my_Shared_ਅੰਕੜੇ = (UConverter_Shared_ਅੰਕੜੇ *) e->value.ਸੂਚਕ;
            /*deletes only if reference counter == 0 */
            ਜੇ (my_Shared_ਅੰਕੜੇ->referenceCounter == 0)
            {
                ਸਾਰਣੀ_Deleted_ਅੰਕ++;

                UCNV_DEBUG_LOG("del",my_Shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name,my_Shared_ਅੰਕੜੇ);

                uhash_removeElement(SHARED_ਅੰਕੜੇ_HASHTABLE, e);
                my_Shared_ਅੰਕੜੇ->shared_ਅੰਕੜੇ_Cached = ਝੂਠ;
                ucnv_delete_Shared_Converter_ਅੰਕੜੇ (my_Shared_ਅੰਕੜੇ);
            } ਨਹੀਂਤਾਂ {
                ++ਬਚਿਆ;
            }
        }
    } ਜਦੋਂ(++i == 1 && ਬਚਿਆ > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", ਬਚਿਆ);

    UTRACE_EXIT_ਮੁੱਲ(ਸਾਰਣੀ_Deleted_ਅੰਕ);
    ਵਾਪਸ ਸਾਰਣੀ_Deleted_ਅੰਕ;
}

/* available converters list --------------------------------------------------- */

ਸਥਿਰ ਖਾਲੀ U_CALLCONV init_ਉਪਲਬਧ_Converters_List(UError_ਕੋਡ &err_ਕੋਡ) {
    U_ASSERT(g_ਉਪਲਬਧ_Converter_ਗਿਣਤੀ == 0);
    U_ASSERT(g_ਉਪਲਬਧ_Converters == ਖਾਲੀ_ਸੂਚਕ);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_ਨਾਂ(&err_ਕੋਡ);
    int32_t all_Converter_ਗਿਣਤੀ = uenum_ਗਿਣਤੀ(allConvEnum, &err_ਕੋਡ);
    ਜੇ (U_FAILURE(err_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_ਉਪਲਬਧ_Converters = static_cast<ਅਚਲ ਅੱਖਰ**>(uprv_malloc(all_Converter_ਗਿਣਤੀ * ਆਕਾਰ(ਅੱਖਰ*)));
    ਜੇ (!g_ਉਪਲਬਧ_Converters) {
        err_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_ਕੋਡ local_ਸਥਿਤੀ = U_ZERO_ERROR;
    UConverter ਆਰਜ਼ੀ_Converter;
    ucnv_close(ucnv_createConverter(&ਆਰਜ਼ੀ_Converter, ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));

    g_ਉਪਲਬਧ_Converter_ਗਿਣਤੀ = 0;

    ਲਈ (int32_t idx = 0; idx < all_Converter_ਗਿਣਤੀ; idx++) {
        local_ਸਥਿਤੀ = U_ZERO_ERROR;
        ਅਚਲ ਅੱਖਰ *converterName = uenum_ਅਗਲਾ(allConvEnum, ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ);
        ਜੇ (ucnv_canCreateConverter(converterName, &local_ਸਥਿਤੀ)) {
            g_ਉਪਲਬਧ_Converters[g_ਉਪਲਬਧ_Converter_ਗਿਣਤੀ++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ਸਥਿਰ UBool have_ਉਪਲਬਧ_Converter_List(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    umtx_initOnce(g_ਉਪਲਬਧ_Converters_Init_Once, &init_ਉਪਲਬਧ_Converters_List, *p_Error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(*p_Error_ਕੋਡ);
}

U_CFUNC uint16_t
ucnv_bld_ਗਿਣਤੀ_ਉਪਲਬਧ_Converters(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (have_ਉਪਲਬਧ_Converter_List(p_Error_ਕੋਡ)) {
        ਵਾਪਸ g_ਉਪਲਬਧ_Converter_ਗਿਣਤੀ;
    }
    ਵਾਪਸ 0;
}

U_CFUNC ਅਚਲ ਅੱਖਰ *
ucnv_bld_get_ਉਪਲਬਧ_Converter(uint16_t n, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (have_ਉਪਲਬਧ_Converter_List(p_Error_ਕੋਡ)) {
        ਜੇ (n < g_ਉਪਲਬਧ_Converter_ਗਿਣਤੀ) {
            ਵਾਪਸ g_ਉਪਲਬਧ_Converters[n];
        }
        *p_Error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

/* default converter name --------------------------------------------------- */

#ਜੇ !U_CHARSET_IS_UTF8
/*
Copy the canonical converter name.
ucnv_getDefaultName must be thread safe, which can call this function.

ucnv_setDefaultName calls this function and it doesn't have to be
thread safe because there is no reliable/safe way to reset the
converter in use in all threads. If you did reset the converter, you
would not be sure that retrieving a default converter for one string
would be the same type of default converter for a successive string.
Since the name is a returned via ucnv_getDefaultName without copying,
you shouldn't be modifying or deleting the string from a separate thread.
*/
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
ਅੰਦਰੂਨੀ_ਸੈੱਟ_Name(ਅਚਲ ਅੱਖਰ *name, UError_ਕੋਡ *ਸਥਿਤੀ) {
    UConverterNamePieces ਢੇਰ_Pieces;
    UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਢੇਰ_ਦਲੀਲਾਂ=UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool ਸ਼ਾਮਲ_ਹੈ_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != ਖਾਲੀ_ਸੂਚਕ);
    ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *algorithmic_Shared_ਅੰਕੜੇ;

    ਢੇਰ_ਦਲੀਲਾਂ.name = name;
    ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Option) {
        ਢੇਰ_Pieces.cnvName[0] = 0;
        ਢੇਰ_Pieces.locale[0] = 0;
        ਢੇਰ_Pieces.options = 0;
        parseConverterOptions(name, &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, ਸਥਿਤੀ);
        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ;
        }
    }
    algorithmic_Shared_ਅੰਕੜੇ = getAlgorithmicTypeFromName(ਢੇਰ_ਦਲੀਲਾਂ.name);

    umtx_ਤਾਲਾ(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_ਅੰਕੜੇ = algorithmic_Shared_ਅੰਕੜੇ;
    g_Default_Converter_ਸ਼ਾਮਲ_ਹੈ_Option = ਸ਼ਾਮਲ_ਹੈ_Option;
    uprv_memcpy(g_Default_Converter_Name_ਬਫਰ, name, length);
    g_Default_Converter_Name_ਬਫਰ[length]=0;

    /* gDefaultConverterName MUST be the last global var set by this function.  */
    /*    It is the variable checked in ucnv_getDefaultName() to see if initialization is required. */
    //    But there is nothing here preventing that from being reordered, either by the compiler
    //             or hardware. I'm adding the mutex to ucnv_getDefaultName for now. UMTX_CHECK is not enough.
    //             -- Andy
    gDefaultConverterName = g_Default_Converter_Name_ਬਫਰ;

    ucnv_enableCleanup();

    umtx_unlock(&cnvCacheMutex);
}
#ਜੇ_ਅੰਤ

/*
 * In order to be really thread-safe, the get function would have to take
 * a buffer parameter and copy the current string inside a mutex block.
 * This implementation only tries to be really thread-safe while
 * setting the name.
 * It assumes that setting a pointer is atomic.
 */

U_CAPI ਅਚਲ ਅੱਖਰ*  U_EXPORT2
ucnv_getDefaultName() {
#ਜੇ U_CHARSET_IS_UTF8
    ਵਾਪਸ "UTF-8";
#ਨਹੀਂਤਾਂ
    /* local variable to be thread-safe */
    ਅਚਲ ਅੱਖਰ *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex ਤਾਲਾ(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    ਜੇ(name==ਖਾਲੀ_ਸੂਚਕ) {
        UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
        UConverter *cnv = ਖਾਲੀ_ਸੂਚਕ;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        ਜੇ(name != ਖਾਲੀ_ਸੂਚਕ) {
            cnv = ucnv_open(name, &error_ਕੋਡ);
            ਜੇ(U_SUCCESS(error_ਕੋਡ) && cnv != ਖਾਲੀ_ਸੂਚਕ) {
                name = ucnv_getName(cnv, &error_ਕੋਡ);
            }
        }

        ਜੇ(name == ਖਾਲੀ_ਸੂਚਕ || name[0] == 0
            || U_FAILURE(error_ਕੋਡ) || cnv == ਖਾਲੀ_ਸੂਚਕ
            || uprv_strlen(name)>=ਆਕਾਰ(g_Default_Converter_Name_ਬਫਰ))
        {
            /* Panic time, let's use a fallback. */
#ਜੇ (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#ਜਾਂਜੇ U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_ਅਦਲਾ_ਬਦਲੀ_LFNL_OPTION_STRING;
#ਨਹੀਂਤਾਂ
            name = "ibm-37_P100-1995";
#ਜੇ_ਅੰਤ
        }

        ਅੰਦਰੂਨੀ_ਸੈੱਟ_Name(name, &error_ਕੋਡ);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    ਵਾਪਸ name;
#ਜੇ_ਅੰਤ
}

#ਜੇ U_CHARSET_IS_UTF8
U_CAPI ਖਾਲੀ U_EXPORT2 ucnv_ਸੈੱਟ_Default_Name(ਅਚਲ ਅੱਖਰ *) {}
#ਨਹੀਂਤਾਂ
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_ਸੈੱਟ_Default_Name(ਅਚਲ ਅੱਖਰ *converterName) {
    ਜੇ(converterName==ਖਾਲੀ_ਸੂਚਕ) {
        /* reset to the default codepage */
        gDefaultConverterName=ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
        UConverter *cnv = ਖਾਲੀ_ਸੂਚਕ;
        ਅਚਲ ਅੱਖਰ *name = ਖਾਲੀ_ਸੂਚਕ;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ) && cnv != ਖਾਲੀ_ਸੂਚਕ) {
            name = ucnv_getName(cnv, &error_ਕੋਡ);
        }

        ਜੇ(U_SUCCESS(error_ਕੋਡ) && name!=ਖਾਲੀ_ਸੂਚਕ) {
            ਅੰਦਰੂਨੀ_ਸੈੱਟ_Name(name, &error_ਕੋਡ);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#ਜੇ_ਅੰਤ

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#ਜੇ !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
          ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UDataInfo *pInfo;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ;

    ਅਚਲ uint8_t *ਵਿੱਚ_ਬਾਈਟਾਂ;
    uint8_t *ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    uint32_t ਵਿੱਥ, count, static_ਅੰਕੜੇ_ਮਾਪ;
    int32_t size;

    ਅਚਲ UConverter_Static_ਅੰਕੜੇ *ਵਿੱਚ_Static_ਅੰਕੜੇ;
    UConverter_Static_ਅੰਕੜੇ *ਬਾਹਰਲਾ_Static_ਅੰਕੜੇ;

    ਅਚਲ _MBCSHeader *ਵਿੱਚ_MBCSHeader;
    _MBCSHeader *ਬਾਹਰਲਾ_MBCSHeader;
    _MBCSHeader mbcs_ਸਿਰਲੇਖ;
    uint32_t mbcs_ਸਿਰਲੇਖ_ਲੰਮਾਈ;
    UBool noFromU=ਝੂਠ;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;

    ਅਚਲ int32_t *ਵਿੱਚ_Ext_Indexes;
    int32_t ext_ਵਿੱਥ;

    /* udata_swapDataHeader checks the arguments */
    ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* check data format and format version */
    pInfo=(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    ਜੇ(!(
        pInfo->ਅੰਕੜੇ_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x6e &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x76 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x74 &&
        pInfo->format_ਸੰਸਕਰਣ[0]==6 &&
        pInfo->format_ਸੰਸਕਰਣ[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                         pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3],
                         pInfo->format_ਸੰਸਕਰਣ[0], pInfo->format_ਸੰਸਕਰਣ[1]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_ਬਾਈਟਾਂ=(ਅਚਲ uint8_t *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    ਬਾਹਰਲਾ_ਬਾਈਟਾਂ=(ਬਾਹਰਲਾ_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) ? ਖਾਲੀ_ਸੂਚਕ : (uint8_t *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    ਵਿੱਚ_Static_ਅੰਕੜੇ=(ਅਚਲ UConverter_Static_ਅੰਕੜੇ *)ਵਿੱਚ_ਬਾਈਟਾਂ;
    ਬਾਹਰਲਾ_Static_ਅੰਕੜੇ=(UConverter_Static_ਅੰਕੜੇ *)ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    ਜੇ(length<0) {
        static_ਅੰਕੜੇ_ਮਾਪ=ds->readUInt32(ਵਿੱਚ_Static_ਅੰਕੜੇ->struct_ਮਾਪ);
    } ਨਹੀਂਤਾਂ {
        length-=ਸਿਰਲੇਖ_ਮਾਪ;
        ਜੇ( length<(int32_t)ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ) ||
            (uint32_t)length<(static_ਅੰਕੜੇ_ਮਾਪ=ds->readUInt32(ਵਿੱਚ_Static_ਅੰਕੜੇ->struct_ਮਾਪ))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
    }

    ਜੇ(length>=0) {
        /* swap the static data */
        ਜੇ(ਵਿੱਚ_Static_ਅੰਕੜੇ!=ਬਾਹਰਲਾ_Static_ਅੰਕੜੇ) {
            uprv_memcpy(ਬਾਹਰਲਾ_Static_ਅੰਕੜੇ, ਵਿੱਚ_Static_ਅੰਕੜੇ, static_ਅੰਕੜੇ_ਮਾਪ);
        }

        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, &ਵਿੱਚ_Static_ਅੰਕੜੇ->struct_ਮਾਪ, 4,
                           &ਬਾਹਰਲਾ_Static_ਅੰਕੜੇ->struct_ਮਾਪ, p_Error_ਕੋਡ);
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, &ਵਿੱਚ_Static_ਅੰਕੜੇ->codepage, 4,
                           &ਬਾਹਰਲਾ_Static_ਅੰਕੜੇ->codepage, p_Error_ਕੋਡ);

        ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, ਵਿੱਚ_Static_ਅੰਕੜੇ->name, (int32_t)uprv_strlen(ਵਿੱਚ_Static_ਅੰਕੜੇ->name),
                            ਬਾਹਰਲਾ_Static_ਅੰਕੜੇ->name, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            ਵਾਪਸ 0;
        }
    }

    ਵਿੱਚ_ਬਾਈਟਾਂ+=static_ਅੰਕੜੇ_ਮਾਪ;
    ਜੇ (ਬਾਹਰਲਾ_ਬਾਈਟਾਂ != ਖਾਲੀ_ਸੂਚਕ) ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+=static_ਅੰਕੜੇ_ਮਾਪ;
    ਜੇ(length>=0) {
        length-=(int32_t)static_ਅੰਕੜੇ_ਮਾਪ;
    }

    /* check for supported conversionType values */
    ਜੇ(ਵਿੱਚ_Static_ਅੰਕੜੇ->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        ਵਿੱਚ_MBCSHeader=(ਅਚਲ _MBCSHeader *)ਵਿੱਚ_ਬਾਈਟਾਂ;
        ਬਾਹਰਲਾ_MBCSHeader=(_MBCSHeader *)ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

        ਜੇ(0<=length && length<(int32_t)ਆਕਾਰ(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
        ਜੇ(ਵਿੱਚ_MBCSHeader->ਸੰਸਕਰਣ[0]==4 && ਵਿੱਚ_MBCSHeader->ਸੰਸਕਰਣ[1]>=1) {
            mbcs_ਸਿਰਲੇਖ_ਲੰਮਾਈ=MBCS_ਸਿਰਲੇਖ_V4_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ(ਵਿੱਚ_MBCSHeader->ਸੰਸਕਰਣ[0]==5 && ਵਿੱਚ_MBCSHeader->ਸੰਸਕਰਣ[1]>=3 &&
                  ((mbcs_ਸਿਰਲੇਖ.options=ds->readUInt32(ਵਿੱਚ_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_ਨਕਾਬ)==0
        ) {
            mbcs_ਸਿਰਲੇਖ_ਲੰਮਾਈ=mbcs_ਸਿਰਲੇਖ.options&MBCS_OPT_ਲੰਮਾਈ_ਨਕਾਬ;
            noFromU = (mbcs_ਸਿਰਲੇਖ.options & MBCS_OPT_NO_FROM_U) != 0;
        } ਨਹੀਂਤਾਂ {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             ਵਿੱਚ_MBCSHeader->ਸੰਸਕਰਣ[0], ਵਿੱਚ_MBCSHeader->ਸੰਸਕਰਣ[1]);
            *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
            ਵਾਪਸ 0;
        }

        uprv_memcpy(mbcs_ਸਿਰਲੇਖ.ਸੰਸਕਰਣ, ਵਿੱਚ_MBCSHeader->ਸੰਸਕਰਣ, 4);
        mbcs_ਸਿਰਲੇਖ.ਗਿਣਤੀ_ਹਾਲਤਾਂ=         ds->readUInt32(ਵਿੱਚ_MBCSHeader->ਗਿਣਤੀ_ਹਾਲਤਾਂ);
        mbcs_ਸਿਰਲੇਖ.ਗਿਣਤੀ_ਮੰਜ਼ਿਲ_UFallbacks=   ds->readUInt32(ਵਿੱਚ_MBCSHeader->ਗਿਣਤੀ_ਮੰਜ਼ਿਲ_UFallbacks);
        mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_ਮੰਜ਼ਿਲ_UCode_Units=  ds->readUInt32(ਵਿੱਚ_MBCSHeader->ਵਿੱਥ_ਮੰਜ਼ਿਲ_UCode_Units);
        mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_From_UTable=    ds->readUInt32(ਵਿੱਚ_MBCSHeader->ਵਿੱਥ_From_UTable);
        mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_From_UBytes=    ds->readUInt32(ਵਿੱਚ_MBCSHeader->ਵਿੱਥ_From_UBytes);
        mbcs_ਸਿਰਲੇਖ.flags=               ds->readUInt32(ਵਿੱਚ_MBCSHeader->flags);
        mbcs_ਸਿਰਲੇਖ.from_UBytes_ਲੰਮਾਈ=    ds->readUInt32(ਵਿੱਚ_MBCSHeader->from_UBytes_ਲੰਮਾਈ);
        /* mbcsHeader.options have been read above */

        ext_ਵਿੱਥ=(int32_t)(mbcs_ਸਿਰਲੇਖ.flags>>8);
        outputType=(uint8_t)mbcs_ਸਿਰਲੇਖ.flags;
        ਜੇ(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
            ਵਾਪਸ 0;
        }

        /* make sure that the output type is known */
        ਚੋਣ(outputType) {
        ਹਾਲਤ MBCS_OUTPUT_1:
        ਹਾਲਤ MBCS_OUTPUT_2:
        ਹਾਲਤ MBCS_OUTPUT_3:
        ਹਾਲਤ MBCS_OUTPUT_4:
        ਹਾਲਤ MBCS_OUTPUT_3_EUC:
        ਹਾਲਤ MBCS_OUTPUT_4_EUC:
        ਹਾਲਤ MBCS_OUTPUT_2_SISO:
        ਹਾਲਤ MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            ਰੋਕੋ;
        ਮੂਲ:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
            ਵਾਪਸ 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcs_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=0;
        ਜੇ( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_ਸਿਰਲੇਖ.ਸੰਸਕਰਣ[1]>=3 && (maxFastUChar=mbcs_ਸਿਰਲੇਖ.ਸੰਸਕਰਣ[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        ਜੇ(ext_ਵਿੱਥ==0) {
            size=(int32_t)(mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_From_UBytes+mbcs_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
            ਜੇ(!noFromU) {
                size+=(int32_t)mbcs_ਸਿਰਲੇਖ.from_UBytes_ਲੰਮਾਈ;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            ਵਿੱਚ_Ext_Indexes=ਖਾਲੀ_ਸੂਚਕ;
        } ਨਹੀਂਤਾਂ {
            /* there is extension data after the base data, see ucnv_ext.h */
            ਜੇ(length>=0 && length<(ext_ਵਿੱਥ+UCNV_EXT_INDEXES_MIN_ਲੰਮਾਈ*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }

            ਵਿੱਚ_Ext_Indexes=(ਅਚਲ int32_t *)(ਵਿੱਚ_ਬਾਈਟਾਂ+ext_ਵਿੱਥ);
            size=ext_ਵਿੱਥ+udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_ਮਾਪ]);
        }

        ਜੇ(length>=0) {
            ਜੇ(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }

            /* copy the data for inaccessible bytes */
            ਜੇ(ਵਿੱਚ_ਬਾਈਟਾਂ!=ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
                uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_ਸਿਰਲੇਖ_ਲੰਮਾਈ*4;
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, &ਵਿੱਚ_MBCSHeader->ਗਿਣਤੀ_ਹਾਲਤਾਂ, count-4,
                               &ਬਾਹਰਲਾ_MBCSHeader->ਗਿਣਤੀ_ਹਾਲਤਾਂ, p_Error_ਕੋਡ);

            ਜੇ(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_ਅਧਾਰ_Name=(ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਬਾਈਟਾਂ+count;
                ਅੱਖਰ *ਬਾਹਰਲਾ_ਅਧਾਰ_Name=(ਅੱਖਰ *)ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+count;
                ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, ਵਿੱਚ_ਅਧਾਰ_Name, (int32_t)uprv_strlen(ਵਿੱਚ_ਅਧਾਰ_Name),
                                    ਬਾਹਰਲਾ_ਅਧਾਰ_Name, p_Error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                ਵਿੱਥ=count;
                count=mbcs_ਸਿਰਲੇਖ.ਗਿਣਤੀ_ਹਾਲਤਾਂ*1024;
                ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                   ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* swap the toUFallbacks[] */
                ਵਿੱਥ+=count;
                count=mbcs_ਸਿਰਲੇਖ.ਗਿਣਤੀ_ਮੰਜ਼ਿਲ_UFallbacks*8;
                ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                   ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* swap the unicodeCodeUnits[] */
                ਵਿੱਥ=mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_ਮੰਜ਼ਿਲ_UCode_Units;
                count=mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_From_UTable-ਵਿੱਥ;
                ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                   ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* offset to the stage 1 table, independent of the outputType */
                ਵਿੱਥ=mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_From_UTable;

                ਜੇ(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_From_UBytes-ਵਿੱਥ)+mbcs_ਸਿਰਲੇਖ.from_UBytes_ਲੰਮਾਈ;
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                       ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    ਜੇ(ਵਿੱਚ_Static_ਅੰਕੜੇ->unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } ਨਹੀਂਤਾਂ {
                        count=0x40*2; /* only BMP */
                    }
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                       ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                    /* stage 2 table: uint32_t[] */
                    ਵਿੱਥ+=count;
                    count=mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_From_UBytes-ਵਿੱਥ;
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                       ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    ਵਿੱਥ=mbcs_ਸਿਰਲੇਖ.ਵਿੱਥ_From_UBytes;
                    count= noFromU ? 0 : mbcs_ਸਿਰਲੇਖ.from_UBytes_ਲੰਮਾਈ;
                    ਚੋਣ(outputType) {
                    ਹਾਲਤ MBCS_OUTPUT_2:
                    ਹਾਲਤ MBCS_OUTPUT_3_EUC:
                    ਹਾਲਤ MBCS_OUTPUT_2_SISO:
                        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                           ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
                        ਰੋਕੋ;
                    ਹਾਲਤ MBCS_OUTPUT_4:
                        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                           ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
                        ਰੋਕੋ;
                    ਮੂਲ:
                        /* just uint8_t[], nothing to swap */
                        ਰੋਕੋ;
                    }

                    ਜੇ(mbcs_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ!=0) {
                        ਵਿੱਥ+=count;
                        count=mbcs_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
                        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, (int32_t)count,
                                           ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
                    }
                }
            }

            ਜੇ(ext_ਵਿੱਥ!=0) {
                /* swap the extension data */
                ਵਿੱਚ_ਬਾਈਟਾਂ+=ext_ਵਿੱਥ;
                ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+=ext_ਵਿੱਥ;

                /* swap toUTable[] */
                ਵਿੱਥ=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_ਮੰਜ਼ਿਲ_U_ਸੂਚਕਾਂਕ]);
                length=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_ਮੰਜ਼ਿਲ_U_ਲੰਮਾਈ]);
                ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, length*4, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* swap toUUChars[] */
                ਵਿੱਥ=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_ਮੰਜ਼ਿਲ_U_UCHARS_ਸੂਚਕਾਂਕ]);
                length=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_ਮੰਜ਼ਿਲ_U_UCHARS_ਲੰਮਾਈ]);
                ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, length*2, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* swap fromUTableUChars[] */
                ਵਿੱਥ=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_ਸੂਚਕਾਂਕ]);
                length=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_ਲੰਮਾਈ]);
                ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, length*2, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* swap fromUTableValues[] */
                ਵਿੱਥ=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ]);
                /* same length as for fromUTableUChars[] */
                ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, length*4, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                ਵਿੱਥ=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_ਸੂਚਕਾਂਕ]);
                length=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_ਲੰਮਾਈ]);
                ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, length*2, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* swap fromUStage3[] */
                ਵਿੱਥ=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_ਸੂਚਕਾਂਕ]);
                length=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_ਲੰਮਾਈ]);
                ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, length*2, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* swap fromUStage3b[] */
                ਵਿੱਥ=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_ਸੂਚਕਾਂਕ]);
                length=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_ਲੰਮਾਈ]);
                ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, length*4, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);

                /* swap indexes[] */
                length=udata_readInt32(ds, ਵਿੱਚ_Ext_Indexes[UCNV_EXT_INDEXES_ਲੰਮਾਈ]);
                ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ, length*4, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, p_Error_ਕੋਡ);
            }
        }
    } ਨਹੀਂਤਾਂ {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         ਵਿੱਚ_Static_ਅੰਕੜੇ->conversionType);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ+(int32_t)static_ਅੰਕੜੇ_ਮਾਪ+size;
}

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#ਜੇ_ਅੰਤ
