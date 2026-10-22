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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"
#ಅನ್ತರ್ಭಾವ "ucnv_io.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnvmbcs.h"
#ಅನ್ತರ್ಭಾವ "ucnv_ext.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_imp.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "ustr_cnv.h"


#ಆದರೆ 0
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಬಾಹ್ಯ ಶೂನ್ಯ UCNV_DEBUG_LOG(ಅಕ್ಷರ *what, ಅಕ್ಷರ *who, ಶೂನ್ಯ *p, ಪೂರ್ಣಾಂಕ l);
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#ಇಲ್ಲದಿದ್ದರೆ
# define UCNV_DEBUG_LOG(x,y,z)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ
converter_ದತ್ತಾಂಶ[UCNV_ಸಂಖ್ಯೆ_ದ_SUPPORTED_CONVERTER_TYPES]={
    ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ,

#ಆದರೆ UCONFIG_NO_LEGACY_CONVERSION
    ಶೂನ್ಯಸೂಚಕ,
#ಇಲ್ಲದಿದ್ದರೆ
    &_MBCSData,
#ಆದರೆ_ಅಂತ್ಯ

    &Latin1_ದತ್ತಾಂಶ,
    &UTF8_ದತ್ತಾಂಶ, &_UTF16BEData, &_UTF16LEData,
#ಆದರೆ UCONFIG_ONLY_HTML_CONVERSION
    ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ,
#ಇಲ್ಲದಿದ್ದರೆ
    &_UTF32BEData, &_UTF32LEData,
#ಆದರೆ_ಅಂತ್ಯ
    ಶೂನ್ಯಸೂಚಕ,

#ಆದರೆ UCONFIG_NO_LEGACY_CONVERSION
    ಶೂನ್ಯಸೂಚಕ,
#ಇಲ್ಲದಿದ್ದರೆ
    &ISO2022_ದತ್ತಾಂಶ,
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
#ಇಲ್ಲದಿದ್ದರೆ
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UCONFIG_ONLY_HTML_CONVERSION
    ಶೂನ್ಯಸೂಚಕ,
#ಇಲ್ಲದಿದ್ದರೆ
    &_SCSUData,
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    ಶೂನ್ಯಸೂಚಕ,
#ಇಲ್ಲದಿದ್ದರೆ
    &_ISCIIData,
#ಆದರೆ_ಅಂತ್ಯ

    &_ASCIIData,
#ಆದರೆ UCONFIG_ONLY_HTML_CONVERSION
    ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &UTF16_ದತ್ತಾಂಶ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ,
#ಇಲ್ಲದಿದ್ದರೆ
    &UTF7_ದತ್ತಾಂಶ, &Bocu1_ದತ್ತಾಂಶ, &UTF16_ದತ್ತಾಂಶ, &UTF32_ದತ್ತಾಂಶ, &CESU8_ದತ್ತಾಂಶ, &_IMAPData,
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    ಶೂನ್ಯಸೂಚಕ,
#ಇಲ್ಲದಿದ್ದರೆ
    &Compound_ಪಠ್ಯ_ದತ್ತಾಂಶ
#ಆದರೆ_ಅಂತ್ಯ
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
ಸ್ಥಿರ ರಚನೆ {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  ಸ್ಥಿರಾಂಕ UConverterType type;
} ಸ್ಥಿರಾಂಕ cnvNameType[] = {
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#ಆದರೆ_ಅಂತ್ಯ
  { "iso88591", UCNV_LATIN_1 },
#ಆದರೆ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#ಆದರೆ_ಅಂತ್ಯ
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#ಆದರೆ U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#ಇಲ್ಲದಿದ್ದರೆ
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#ಆದರೆ U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#ಇಲ್ಲದಿದ್ದರೆ
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#ಆದರೆ_ಅಂತ್ಯ
  { "utf8", UCNV_UTF8 },
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_ಪಠ್ಯ}
#ಆದರೆ_ಅಂತ್ಯ
};


/*initializes some global variables */
ಸ್ಥಿರ UHashtable *SHARED_ದತ್ತಾಂಶ_HASHTABLE = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **g_ಲಭ್ಯ_Converters = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ uint16_t g_ಲಭ್ಯ_Converter_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ icu::UInitOnce g_ಲಭ್ಯ_Converters_Init_Once {};

#ಆದರೆ !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ಸ್ಥಿರ ಅಕ್ಷರ g_Default_Converter_Name_ಬಫರ್[UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ + 1]; /* +1 for nullptr */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *gDefaultConverterName = ಶೂನ್ಯಸೂಚಕ;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *g_Default_Algorithmic_Shared_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ಸ್ಥಿರ UBool g_Default_Converter_ಒಳಗೊಂಡಿದೆ_Option;

#ಆದರೆ_ಅಂತ್ಯ  /* !U_CHARSET_IS_UTF8 */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ದತ್ತಾಂಶ_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_flush_ಲಭ್ಯ_Converter_Cache() {
    g_ಲಭ್ಯ_Converter_ಎಣಿಕೆ = 0;
    ಆದರೆ (g_ಲಭ್ಯ_Converters) {
        uprv_free(const_cast<ಅಕ್ಷರ**>(g_ಲಭ್ಯ_Converters));
        g_ಲಭ್ಯ_Converters = ಶೂನ್ಯಸೂಚಕ;
    }
    g_ಲಭ್ಯ_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ಸ್ಥಿರ UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    ಆದರೆ (SHARED_ದತ್ತಾಂಶ_HASHTABLE != ಶೂನ್ಯಸೂಚಕ && uhash_ಎಣಿಕೆ(SHARED_ದತ್ತಾಂಶ_HASHTABLE) == 0) {
        uhash_close(SHARED_ದತ್ತಾಂಶ_HASHTABLE);
        SHARED_ದತ್ತಾಂಶ_HASHTABLE = ಶೂನ್ಯಸೂಚಕ;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_ಲಭ್ಯ_Converter_Cache();

#ಆದರೆ !U_CHARSET_IS_UTF8
    gDefaultConverterName = ಶೂನ್ಯಸೂಚಕ;
    g_Default_Converter_Name_ಬಫರ್[0] = 0;
    g_Default_Converter_ಒಳಗೊಂಡಿದೆ_Option = ಸುಳ್ಳು;
    g_Default_Algorithmic_Shared_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ (SHARED_ದತ್ತಾಂಶ_HASHTABLE == ಶೂನ್ಯಸೂಚಕ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

ಸ್ಥಿರ UBool U_CALLCONV
isCnvAcceptable(ಶೂನ್ಯ * /*context*/,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*type*/, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*name*/,
                ಸ್ಥಿರಾಂಕ UDataInfo *pInfo) {
    ಹಿಂತಿರುಗಿ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x6e &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x76 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x74 &&
        pInfo->format_ಆವೃತ್ತಿ[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
ಸ್ಥಿರ UConverter_Shared_ದತ್ತಾಂಶ*
ucnv_ದತ್ತಾಂಶ_un_Flatten_Clone(UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ಸ್ಥಿರಾಂಕ uint8_t* raw = static_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(udata_get_ಸ್ಮರಣೆ(p_ದತ್ತಾಂಶ));
    ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ* ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ*>(raw);
    UConverter_Shared_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;
    UConverterType type = static_cast<UConverterType>(ಮೂಲ->conversionType);

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2))
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (static_cast<uint16_t>(type) >= UCNV_ಸಂಖ್ಯೆ_ದ_SUPPORTED_CONVERTER_TYPES ||
        converter_ದತ್ತಾಂಶ[type] == ಶೂನ್ಯಸೂಚಕ ||
        !converter_ದತ್ತಾಂಶ[type]->isReferenceCounted ||
        converter_ದತ್ತಾಂಶ[type]->referenceCounter != 1 ||
        ಮೂಲ->struct_ಗಾತ್ರ != ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ))
    {
        *ಸ್ಥಿತಿ_2 = U_INVALID_ಕೋಷ್ಟಕ_FORMAT;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ದತ್ತಾಂಶ = static_cast<UConverter_Shared_ದತ್ತಾಂಶ*>(uprv_malloc(ಗಾತ್ರ(UConverter_Shared_ದತ್ತಾಂಶ)));
    ಆದರೆ(ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(ದತ್ತಾಂಶ, converter_ದತ್ತಾಂಶ[type], ಗಾತ್ರ(UConverter_Shared_ದತ್ತಾಂಶ));

    ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ = ಮೂಲ;

    ದತ್ತಾಂಶ->shared_ದತ್ತಾಂಶ_Cached = ಸುಳ್ಳು;

    /* fill in fields from the loaded data */
    ದತ್ತಾಂಶ->ದತ್ತಾಂಶ_ಸ್ಮರಣೆ = (ಶೂನ್ಯ*)p_ದತ್ತಾಂಶ; /* for future use */

    ಆದರೆ(ದತ್ತಾಂಶ->impl->load != ಶೂನ್ಯಸೂಚಕ) {
        ದತ್ತಾಂಶ->impl->load(ದತ್ತಾಂಶ, p_ವಾದಗಳು, raw + ಮೂಲ->struct_ಗಾತ್ರ, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            uprv_free(ದತ್ತಾಂಶ);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ಸ್ಥಿರ UConverter_Shared_ದತ್ತಾಂಶ *createConverterFromFile(UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ * err)
{
    UData_ಸ್ಮರಣೆ *ದತ್ತಾಂಶ;
    UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ;

    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_ತುಂಬು);

    ಆದರೆ (U_FAILURE (*err)) {
        UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_ವಾದಗಳು->name, p_ವಾದಗಳು->pkg);

    ದತ್ತಾಂಶ = udata_openChoice(p_ವಾದಗಳು->pkg, ದತ್ತಾಂಶ_TYPE, p_ವಾದಗಳು->name, isCnvAcceptable, ಶೂನ್ಯಸೂಚಕ, err);
    ಆದರೆ(U_FAILURE(*err))
    {
        UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    shared_ದತ್ತಾಂಶ = ucnv_ದತ್ತಾಂಶ_un_Flatten_Clone(p_ವಾದಗಳು, ದತ್ತಾಂಶ, err);
    ಆದರೆ(U_FAILURE(*err))
    {
        udata_close(ದತ್ತಾಂಶ);
        UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_ಸ್ಥಿತಿ(shared_ದತ್ತಾಂಶ, *err);
    ಹಿಂತಿರುಗಿ shared_ದತ್ತಾಂಶ;
}

/*returns a converter type from a string
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *
getAlgorithmicTypeFromName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *realName)
{
    uint32_t mid, ಆರಂಭ, ಎಲ್ಲೆ;
    uint32_t ಕೊನೆಯ_Mid;
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;
    ಅಕ್ಷರ strippedName[UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    ಆರಂಭ = 0;
    ಎಲ್ಲೆ = UPRV_LENGTHOF(cnvNameType);
    mid = ಎಲ್ಲೆ;
    ಕೊನೆಯ_Mid = UINT32_MAX;

    ಗಾಗಿ (;;) {
        mid = (ಆರಂಭ + ಎಲ್ಲೆ) / 2;
        ಆದರೆ (ಕೊನೆಯ_Mid == mid) {   /* Have we moved? */
            ನಿಲ್ಲಿಸು;  /* We haven't moved, and it wasn't found. */
        }
        ಕೊನೆಯ_Mid = mid;
        ಫಲಿತಾಂಶ = uprv_strcmp(strippedName, cnvNameType[mid].name);

        ಆದರೆ (ಫಲಿತಾಂಶ < 0) {
            ಎಲ್ಲೆ = mid;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ > 0) {
            ಆರಂಭ = mid;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ converter_ದತ್ತಾಂಶ[cnvNameType[mid].type];
        }
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_CACHE_ತುಂಬು_ಗುಣಕ 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_share_Converter_ದತ್ತಾಂಶ(UConverter_Shared_ದತ್ತಾಂಶ * ದತ್ತಾಂಶ)
{
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    ಆದರೆ (SHARED_ದತ್ತಾಂಶ_HASHTABLE == ಶೂನ್ಯಸೂಚಕ)
    {
        SHARED_ದತ್ತಾಂಶ_HASHTABLE = uhash_open_ಗಾತ್ರ(uhash_hashChars, uhash_compareChars, ಶೂನ್ಯಸೂಚಕ,
                            ucnv_io_ಎಣಿಕೆ_ತಿಳಿದ_Converters(&err)*UCNV_CACHE_ತುಂಬು_ಗುಣಕ,
                            &err);
        ucnv_enableCleanup();

        ಆದರೆ (U_FAILURE(err))
            ಹಿಂತಿರುಗಿ;
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
    ದತ್ತಾಂಶ->shared_ದತ್ತಾಂಶ_Cached = ನಿಜ;

    uhash_put(SHARED_ದತ್ತಾಂಶ_HASHTABLE,
            (ಶೂನ್ಯ*) ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            ದತ್ತಾಂಶ,
            &err);
    UCNV_DEBUG_LOG("put", ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name,ದತ್ತಾಂಶ);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
ಸ್ಥಿರ UConverter_Shared_ದತ್ತಾಂಶ *
ucnv_get_Shared_Converter_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
    /*special case when no Table has yet been created we return nullptr */
    ಆದರೆ (SHARED_ದತ್ತಾಂಶ_HASHTABLE == ಶೂನ್ಯಸೂಚಕ)
    {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        UConverter_Shared_ದತ್ತಾಂಶ *rc;

        rc = static_cast<UConverter_Shared_ದತ್ತಾಂಶ*>(uhash_get(SHARED_ದತ್ತಾಂಶ_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        ಹಿಂತಿರುಗಿ rc;
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
ಸ್ಥಿರ UBool
ucnv_delete_Shared_Converter_ದತ್ತಾಂಶ(UConverter_Shared_ದತ್ತಾಂಶ * dead_Shared_ದತ್ತಾಂಶ)
{
    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name, dead_Shared_ದತ್ತಾಂಶ);

    ಆದರೆ (dead_Shared_ದತ್ತಾಂಶ->referenceCounter > 0) {
        UTRACE_EXIT_ಮೌಲ್ಯ((int32_t)ಸುಳ್ಳು);
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ (dead_Shared_ದತ್ತಾಂಶ->impl->unload != ಶೂನ್ಯಸೂಚಕ) {
        dead_Shared_ದತ್ತಾಂಶ->impl->unload(dead_Shared_ದತ್ತಾಂಶ);
    }

    ಆದರೆ(dead_Shared_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ_ಸ್ಮರಣೆ != ಶೂನ್ಯಸೂಚಕ)
    {
        UData_ಸ್ಮರಣೆ *ದತ್ತಾಂಶ = (UData_ಸ್ಮರಣೆ*)dead_Shared_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ_ಸ್ಮರಣೆ;
        udata_close(ದತ್ತಾಂಶ);
    }

    uprv_free(dead_Shared_ದತ್ತಾಂಶ);

    UTRACE_EXIT_ಮೌಲ್ಯ((int32_t)ನಿಜ);
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_ದತ್ತಾಂಶ *
ucnv_ತುಂಬು(UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *err) {
    UConverter_Shared_ದತ್ತಾಂಶ *my_Shared_Converter_ದತ್ತಾಂಶ;

    ಆದರೆ(err == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(p_ವಾದಗಳು->pkg != ಶೂನ್ಯಸೂಚಕ && *p_ವಾದಗಳು->pkg != 0) {
        /* application-provided converters are not currently cached */
        ಹಿಂತಿರುಗಿ createConverterFromFile(p_ವಾದಗಳು, err);
    }

    my_Shared_Converter_ದತ್ತಾಂಶ = ucnv_get_Shared_Converter_ದತ್ತಾಂಶ(p_ವಾದಗಳು->name);
    ಆದರೆ (my_Shared_Converter_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_ದತ್ತಾಂಶ = createConverterFromFile(p_ವಾದಗಳು, err);
        ಆದರೆ (U_FAILURE (*err) || (my_Shared_Converter_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ))
        {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!p_ವಾದಗಳು->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_ದತ್ತಾಂಶ(my_Shared_Converter_ದತ್ತಾಂಶ);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_ದತ್ತಾಂಶ->referenceCounter++;
    }

    ಹಿಂತಿರುಗಿ my_Shared_Converter_ದತ್ತಾಂಶ;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI ಶೂನ್ಯ
ucnv_unload(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ) {
    ಆದರೆ(shared_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (shared_ದತ್ತಾಂಶ->referenceCounter > 0) {
            shared_ದತ್ತಾಂಶ->referenceCounter--;
        }

        ಆದರೆ((shared_ದತ್ತಾಂಶ->referenceCounter <= 0)&&(shared_ದತ್ತಾಂಶ->shared_ದತ್ತಾಂಶ_Cached == ಸುಳ್ಳು)) {
            ucnv_delete_Shared_Converter_ದತ್ತಾಂಶ(shared_ದತ್ತಾಂಶ);
        }
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_unload_Shared_ದತ್ತಾಂಶ_If_Ready(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ)
{
    ಆದರೆ(shared_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ && shared_ದತ್ತಾಂಶ->isReferenceCounted) {
        umtx_ಬೀಗ(&cnvCacheMutex);
        ucnv_unload(shared_ದತ್ತಾಂಶ);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_increment_Ref_ಎಣಿಕೆ(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ)
{
    ಆದರೆ(shared_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ && shared_ದತ್ತಾಂಶ->isReferenceCounted) {
        umtx_ಬೀಗ(&cnvCacheMutex);
        shared_ದತ್ತಾಂಶ->referenceCounter++;
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
ಸ್ಥಿರ ಶೂನ್ಯ
parseConverterOptions(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
                      UError_ಸಂಕೇತ *err)
{
    ಅಕ್ಷರ *cnvName = pPieces->cnvName;
    ಅಕ್ಷರ c;
    int32_t len = 0;

    p_ವಾದಗಳು->name=ಒಳ_Name;
    p_ವಾದಗಳು->locale=pPieces->locale;
    p_ವಾದಗಳು->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    ಆಗ((c=*ಒಳ_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        ಆದರೆ (++len>=UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ) {
            *err = U_ILLEGAL_ವಾದ_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            ಹಿಂತಿರುಗಿ;
        }
        *cnvName++=c;
        ಒಳ_Name++;
    }
    *cnvName=0;
    p_ವಾದಗಳು->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    ಆಗ((c=*ಒಳ_Name)!=0) {
        ಆದರೆ(c==UCNV_OPTION_SEP_CHAR) {
            ++ಒಳ_Name;
        }

        /* inName is behind an option separator */
        ಆದರೆ(uprv_strncmp(ಒಳ_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            ಅಕ್ಷರ *dest=pPieces->locale;

            /* copy the locale option value */
            ಒಳ_Name+=7;
            len=0;
            ಆಗ((c=*ಒಳ_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++ಒಳ_Name;

                ಆದರೆ(++len>=ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ) {
                    *err=U_ILLEGAL_ವಾದ_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    ಹಿಂತಿರುಗಿ;
                }

                *dest++=c;
            }
            *dest=0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(uprv_strncmp(ಒಳ_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            ಒಳ_Name+=8;
            c=*ಒಳ_Name;
            ಆದರೆ(c==0) {
                p_ವಾದಗಳು->options=(pPieces->options&=~UCNV_OPTION_ಆವೃತ್ತಿ);
                ಹಿಂತಿರುಗಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (static_cast<uint8_t>(c - '0') < 10) {
                p_ವಾದಗಳು->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_ಆವೃತ್ತಿ) | static_cast<uint32_t>(c - '0');
                ++ಒಳ_Name;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(uprv_strncmp(ಒಳ_Name, "swaplfnl", 8)==0) {
            ಒಳ_Name+=8;
            p_ವಾದಗಳು->options=(pPieces->options|=UCNV_OPTION_ಅದಲುಬದಲು_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* ignore any other options until we define some */
            ಆಗ(((c = *ಒಳ_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            ಆದರೆ(c==0) {
                ಹಿಂತಿರುಗಿ;
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
U_CFUNC UConverter_Shared_ದತ್ತಾಂಶ *
ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
                    UError_ಸಂಕೇತ * err) {
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverter_ತುಂಬು_ವಾದಗಳು ರಾಶಿ_ವಾದಗಳು;
    UConverter_Shared_ದತ್ತಾಂಶ *my_Shared_Converter_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ ಆಂತರಿಕ_Error_ಸಂಕೇತ = U_ZERO_ERROR;
    UBool mayContainOption = ನಿಜ;
    UBool checkForAlgorithmic = ನಿಜ;

    ಆದರೆ (U_FAILURE (*err)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(pPieces == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(p_ವಾದಗಳು != ಶೂನ್ಯಸೂಚಕ) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        pPieces = &ರಾಶಿ_Pieces;
    }
    ಆದರೆ(p_ವಾದಗಳು == ಶೂನ್ಯಸೂಚಕ) {
        uprv_memset(&ರಾಶಿ_ವಾದಗಳು, 0, ಗಾತ್ರ(ರಾಶಿ_ವಾದಗಳು));
        ರಾಶಿ_ವಾದಗಳು.size = (int32_t)ಗಾತ್ರ(ರಾಶಿ_ವಾದಗಳು);
        p_ವಾದಗಳು = &ರಾಶಿ_ವಾದಗಳು;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_ವಾದಗಳು->name = converterName;
    p_ವಾದಗಳು->locale = pPieces->locale;
    p_ವಾದಗಳು->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    ಆದರೆ (converterName == ಶೂನ್ಯಸೂಚಕ) {
#ಆದರೆ U_CHARSET_IS_UTF8
        p_ವಾದಗಳು->name = "UTF-8";
        ಹಿಂತಿರುಗಿ (UConverter_Shared_ದತ್ತಾಂಶ *)converter_ದತ್ತಾಂಶ[UCNV_UTF8];
#ಇಲ್ಲದಿದ್ದರೆ
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_ವಾದಗಳು->name = ucnv_getDefaultName();
        ಆದರೆ (p_ವಾದಗಳು->name == ಶೂನ್ಯಸೂಚಕ) {
            *err = U_MISSING_RESOURCE_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        my_Shared_Converter_ದತ್ತಾಂಶ = (UConverter_Shared_ದತ್ತಾಂಶ *)g_Default_Algorithmic_Shared_ದತ್ತಾಂಶ;
        checkForAlgorithmic = ಸುಳ್ಳು;
        mayContainOption = g_Default_Converter_ಒಳಗೊಂಡಿದೆ_Option;
        /* the default converter name is already canonical */
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_ವಾದಗಳು->name = "UTF-8";
        ಹಿಂತಿರುಗಿ (UConverter_Shared_ದತ್ತಾಂಶ *)converter_ದತ್ತಾಂಶ[UCNV_UTF8];
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_ವಾದಗಳು, err);
        ಆದರೆ (U_FAILURE(*err)) {
            /* Very bad name used. */
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        /* get the canonical converter name */
        p_ವಾದಗಳು->name = ucnv_io_getConverterName(p_ವಾದಗಳು->name, &mayContainOption, &ಆಂತರಿಕ_Error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(ಆಂತರಿಕ_Error_ಸಂಕೇತ) || p_ವಾದಗಳು->name == ಶೂನ್ಯಸೂಚಕ) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_ವಾದಗಳು->name = pPieces->cnvName;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆಂತರಿಕ_Error_ಸಂಕೇತ == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    ಆದರೆ(mayContainOption && p_ವಾದಗಳು->name != pPieces->cnvName) {
        parseConverterOptions(p_ವಾದಗಳು->name, pPieces, p_ವಾದಗಳು, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    ಆದರೆ (checkForAlgorithmic) {
        my_Shared_Converter_ದತ್ತಾಂಶ = (UConverter_Shared_ದತ್ತಾಂಶ *)getAlgorithmicTypeFromName(p_ವಾದಗಳು->name);
    }
    ಆದರೆ (my_Shared_Converter_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_ವಾದಗಳು->nestedLoads=1;
        p_ವಾದಗಳು->pkg=ಶೂನ್ಯಸೂಚಕ;

        umtx_ಬೀಗ(&cnvCacheMutex);
        my_Shared_Converter_ದತ್ತಾಂಶ = ucnv_ತುಂಬು(p_ವಾದಗಳು, err);
        umtx_unlock(&cnvCacheMutex);
        ಆದರೆ (U_FAILURE (*err) || (my_Shared_Converter_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ))
        {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }

    ಹಿಂತಿರುಗಿ my_Shared_Converter_ದತ್ತಾಂಶ;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* converterName,
                     UError_ಸಂಕೇತ* err)
{
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverter_ತುಂಬು_ವಾದಗಳು ರಾಶಿ_ವಾದಗಳು=UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER;
    UConverter_Shared_ದತ್ತಾಂಶ *my_Shared_Converter_ದತ್ತಾಂಶ;

    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_OPEN);

    ಆದರೆ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_ದತ್ತಾಂಶ = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ(converterName, &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, err);

        myUConverter = ucnv_create_Converter_From_Shared_ದತ್ತಾಂಶ(
            myUConverter, my_Shared_Converter_ದತ್ತಾಂಶ,
            &ರಾಶಿ_ವಾದಗಳು,
            err);

        ಆದರೆ(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_ಸ್ಥಿತಿ(myUConverter, *err);
            ಹಿಂತಿರುಗಿ myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CFUNC UBool
ucnv_canCreateConverter(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName, UError_ಸಂಕೇತ *err) {
    UConverter myUConverter;
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverter_ತುಂಬು_ವಾದಗಳು ರಾಶಿ_ವಾದಗಳು=UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER;
    UConverter_Shared_ದತ್ತಾಂಶ *my_Shared_Converter_ದತ್ತಾಂಶ;

    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_OPEN);

    ಆದರೆ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ರಾಶಿ_ವಾದಗಳು.onlyTestIsLoadable=ನಿಜ;
        my_Shared_Converter_ದತ್ತಾಂಶ = ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ(converterName, &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, err);
        ucnv_create_Converter_From_Shared_ದತ್ತಾಂಶ(
            &myUConverter, my_Shared_Converter_ದತ್ತಾಂಶ,
            &ರಾಶಿ_ವಾದಗಳು,
            err);
        ucnv_unload_Shared_ದತ್ತಾಂಶ_If_Ready(my_Shared_Converter_ದತ್ತಾಂಶ);
    }

    UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
    ಹಿಂತಿರುಗಿ U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
                                uint32_t options,
                                UError_ಸಂಕೇತ* err) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ;
    UConverter_ತುಂಬು_ವಾದಗಳು ರಾಶಿ_ವಾದಗಳು=UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER;

    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    ಆದರೆ(type<0 || UCNV_ಸಂಖ್ಯೆ_ದ_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        UTRACE_EXIT_ಸ್ಥಿತಿ(U_ILLEGAL_ವಾದ_ERROR);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    shared_ದತ್ತಾಂಶ = converter_ದತ್ತಾಂಶ[type];
    ಆದರೆ(shared_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ || shared_ದತ್ತಾಂಶ->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ವಾದ_ERROR;
        UTRACE_EXIT_ಸ್ಥಿತಿ(U_ILLEGAL_ವಾದ_ERROR);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ರಾಶಿ_ವಾದಗಳು.name = "";
    ರಾಶಿ_ವಾದಗಳು.options = options;
    ರಾಶಿ_ವಾದಗಳು.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_ದತ್ತಾಂಶ(
            myUConverter, const_cast<UConverter_Shared_ದತ್ತಾಂಶ*>(shared_ದತ್ತಾಂಶ),
            &ರಾಶಿ_ವಾದಗಳು, err);

    UTRACE_EXIT_PTR_ಸ್ಥಿತಿ(cnv, *err);
    ಹಿಂತಿರುಗಿ cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName, UError_ಸಂಕೇತ * err)
{
    UConverter *myUConverter;
    UConverter_Shared_ದತ್ತಾಂಶ *my_Shared_Converter_ದತ್ತಾಂಶ;
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverter_ತುಂಬು_ವಾದಗಳು ರಾಶಿ_ವಾದಗಳು=UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER;

    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_OPEN_PACKAGE);

    ಆದರೆ(U_FAILURE(*err)) {
        UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ರಾಶಿ_Pieces.cnvName[0] = 0;
    ರಾಶಿ_Pieces.locale[0] = 0;
    ರಾಶಿ_Pieces.options = 0;
    parseConverterOptions(converterName, &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, err);
    ಆದರೆ (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ರಾಶಿ_ವಾದಗಳು.nestedLoads=1;
    ರಾಶಿ_ವಾದಗಳು.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_ದತ್ತಾಂಶ = createConverterFromFile(&ರಾಶಿ_ವಾದಗಳು, err);

    ಆದರೆ (U_FAILURE(*err)) {
        UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_ದತ್ತಾಂಶ(ಶೂನ್ಯಸೂಚಕ, my_Shared_Converter_ದತ್ತಾಂಶ, &ರಾಶಿ_ವಾದಗಳು, err);

    ಆದರೆ (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_ಸ್ಥಿತಿ(*err);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UTRACE_EXIT_PTR_ಸ್ಥಿತಿ(myUConverter, *err);
    ಹಿಂತಿರುಗಿ myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_ದತ್ತಾಂಶ(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_ದತ್ತಾಂಶ* my_Shared_Converter_ದತ್ತಾಂಶ,
                                   UConverter_ತುಂಬು_ವಾದಗಳು* p_ವಾದಗಳು,
                                   UError_ಸಂಕೇತ* err)
{
    UBool isCopyLocal;

    ಆದರೆ(U_FAILURE(*err)) {
        ucnv_unload_Shared_ದತ್ತಾಂಶ_If_Ready(my_Shared_Converter_ದತ್ತಾಂಶ);
        ಹಿಂತಿರುಗಿ myUConverter;
    }
    ಆದರೆ(myUConverter == ಶೂನ್ಯಸೂಚಕ)
    {
        myUConverter = (UConverter *) uprv_malloc (ಗಾತ್ರ (UConverter));
        ಆದರೆ(myUConverter == ಶೂನ್ಯಸೂಚಕ)
        {
            *err = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ucnv_unload_Shared_ದತ್ತಾಂಶ_If_Ready(my_Shared_Converter_ದತ್ತಾಂಶ);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        isCopyLocal = ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        isCopyLocal = ನಿಜ;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, ಗಾತ್ರ(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_ದತ್ತಾಂಶ = my_Shared_Converter_ದತ್ತಾಂಶ;
    myUConverter->options = p_ವಾದಗಳು->options;
    ಆದರೆ(!p_ವಾದಗಳು->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_ಗಮ್ಯ_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = my_Shared_Converter_ದತ್ತಾಂಶ->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
        myUConverter->max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar = my_Shared_Converter_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->max_ಬೈಟುಗಳು_ಪ್ರತಿ_Char;
        myUConverter->subChar1 = my_Shared_Converter_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->subChar, myUConverter->subCharLen);
        myUConverter->ಗಮ್ಯ_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    ಆದರೆ(my_Shared_Converter_ದತ್ತಾಂಶ->impl->open != ಶೂನ್ಯಸೂಚಕ) {
        my_Shared_Converter_ದತ್ತಾಂಶ->impl->open(myUConverter, p_ವಾದಗಳು, err);
        ಆದರೆ(U_FAILURE(*err) && !p_ವಾದಗಳು->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }

    ಹಿಂತಿರುಗಿ myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_ದತ್ತಾಂಶ *my_Shared_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    int32_t pos;
    int32_t ಕೋಷ್ಟಕ_Deleted_ಅಂಕೆ = 0;
    ಸ್ಥಿರಾಂಕ UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, ಉಳಿದ;

    UTRACE_ನಮೂದು_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    ಆದರೆ (SHARED_ದತ್ತಾಂಶ_HASHTABLE == ಶೂನ್ಯಸೂಚಕ) {
        UTRACE_EXIT_ಮೌಲ್ಯ((int32_t)0);
        ಹಿಂತಿರುಗಿ 0;
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
    umtx_ಬೀಗ(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    ಮಾಡು {
        ಉಳಿದ = 0;
        pos = UHASH_ಮೊದಲ;
        ಆಗ ((e = uhash_ಮುಂದಿನ_Element (SHARED_ದತ್ತಾಂಶ_HASHTABLE, &pos)) != ಶೂನ್ಯಸೂಚಕ)
        {
            my_Shared_ದತ್ತಾಂಶ = (UConverter_Shared_ದತ್ತಾಂಶ *) e->value.ಸೂಚಕ;
            /*deletes only if reference counter == 0 */
            ಆದರೆ (my_Shared_ದತ್ತಾಂಶ->referenceCounter == 0)
            {
                ಕೋಷ್ಟಕ_Deleted_ಅಂಕೆ++;

                UCNV_DEBUG_LOG("del",my_Shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name,my_Shared_ದತ್ತಾಂಶ);

                uhash_removeElement(SHARED_ದತ್ತಾಂಶ_HASHTABLE, e);
                my_Shared_ದತ್ತಾಂಶ->shared_ದತ್ತಾಂಶ_Cached = ಸುಳ್ಳು;
                ucnv_delete_Shared_Converter_ದತ್ತಾಂಶ (my_Shared_ದತ್ತಾಂಶ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ++ಉಳಿದ;
            }
        }
    } ಆಗ(++i == 1 && ಉಳಿದ > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", ಉಳಿದ);

    UTRACE_EXIT_ಮೌಲ್ಯ(ಕೋಷ್ಟಕ_Deleted_ಅಂಕೆ);
    ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ_Deleted_ಅಂಕೆ;
}

/* available converters list --------------------------------------------------- */

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV init_ಲಭ್ಯ_Converters_List(UError_ಸಂಕೇತ &err_ಸಂಕೇತ) {
    U_ASSERT(g_ಲಭ್ಯ_Converter_ಎಣಿಕೆ == 0);
    U_ASSERT(g_ಲಭ್ಯ_Converters == ಶೂನ್ಯಸೂಚಕ);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_ಹೆಸರುಗಳು(&err_ಸಂಕೇತ);
    int32_t all_Converter_ಎಣಿಕೆ = uenum_ಎಣಿಕೆ(allConvEnum, &err_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(err_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_ಲಭ್ಯ_Converters = static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**>(uprv_malloc(all_Converter_ಎಣಿಕೆ * ಗಾತ್ರ(ಅಕ್ಷರ*)));
    ಆದರೆ (!g_ಲಭ್ಯ_Converters) {
        err_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_ಸಂಕೇತ local_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    UConverter ತಾತ್ಕಾಲಿಕ_Converter;
    ucnv_close(ucnv_createConverter(&ತಾತ್ಕಾಲಿಕ_Converter, ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));

    g_ಲಭ್ಯ_Converter_ಎಣಿಕೆ = 0;

    ಗಾಗಿ (int32_t idx = 0; idx < all_Converter_ಎಣಿಕೆ; idx++) {
        local_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName = uenum_ಮುಂದಿನ(allConvEnum, ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ);
        ಆದರೆ (ucnv_canCreateConverter(converterName, &local_ಸ್ಥಿತಿ)) {
            g_ಲಭ್ಯ_Converters[g_ಲಭ್ಯ_Converter_ಎಣಿಕೆ++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ಸ್ಥಿರ UBool have_ಲಭ್ಯ_Converter_List(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    umtx_initOnce(g_ಲಭ್ಯ_Converters_Init_Once, &init_ಲಭ್ಯ_Converters_List, *p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(*p_Error_ಸಂಕೇತ);
}

U_CFUNC uint16_t
ucnv_bld_ಎಣಿಕೆ_ಲಭ್ಯ_Converters(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (have_ಲಭ್ಯ_Converter_List(p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ g_ಲಭ್ಯ_Converter_ಎಣಿಕೆ;
    }
    ಹಿಂತಿರುಗಿ 0;
}

U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
ucnv_bld_get_ಲಭ್ಯ_Converter(uint16_t n, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (have_ಲಭ್ಯ_Converter_List(p_Error_ಸಂಕೇತ)) {
        ಆದರೆ (n < g_ಲಭ್ಯ_Converter_ಎಣಿಕೆ) {
            ಹಿಂತಿರುಗಿ g_ಲಭ್ಯ_Converters[n];
        }
        *p_Error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

/* default converter name --------------------------------------------------- */

#ಆದರೆ !U_CHARSET_IS_UTF8
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ಆಂತರಿಕ_ಗಣ_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverter_ತುಂಬು_ವಾದಗಳು ರಾಶಿ_ವಾದಗಳು=UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool ಒಳಗೊಂಡಿದೆ_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != ಶೂನ್ಯಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *algorithmic_Shared_ದತ್ತಾಂಶ;

    ರಾಶಿ_ವಾದಗಳು.name = name;
    ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Option) {
        ರಾಶಿ_Pieces.cnvName[0] = 0;
        ರಾಶಿ_Pieces.locale[0] = 0;
        ರಾಶಿ_Pieces.options = 0;
        parseConverterOptions(name, &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ;
        }
    }
    algorithmic_Shared_ದತ್ತಾಂಶ = getAlgorithmicTypeFromName(ರಾಶಿ_ವಾದಗಳು.name);

    umtx_ಬೀಗ(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_ದತ್ತಾಂಶ = algorithmic_Shared_ದತ್ತಾಂಶ;
    g_Default_Converter_ಒಳಗೊಂಡಿದೆ_Option = ಒಳಗೊಂಡಿದೆ_Option;
    uprv_memcpy(g_Default_Converter_Name_ಬಫರ್, name, length);
    g_Default_Converter_Name_ಬಫರ್[length]=0;

    /* gDefaultConverterName MUST be the last global var set by this function.  */
    /*    It is the variable checked in ucnv_getDefaultName() to see if initialization is required. */
    //    But there is nothing here preventing that from being reordered, either by the compiler
    //             or hardware. I'm adding the mutex to ucnv_getDefaultName for now. UMTX_CHECK is not enough.
    //             -- Andy
    gDefaultConverterName = g_Default_Converter_Name_ಬಫರ್;

    ucnv_enableCleanup();

    umtx_unlock(&cnvCacheMutex);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * In order to be really thread-safe, the get function would have to take
 * a buffer parameter and copy the current string inside a mutex block.
 * This implementation only tries to be really thread-safe while
 * setting the name.
 * It assumes that setting a pointer is atomic.
 */

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*  U_EXPORT2
ucnv_getDefaultName() {
#ಆದರೆ U_CHARSET_IS_UTF8
    ಹಿಂತಿರುಗಿ "UTF-8";
#ಇಲ್ಲದಿದ್ದರೆ
    /* local variable to be thread-safe */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex ಬೀಗ(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    ಆದರೆ(name==ಶೂನ್ಯಸೂಚಕ) {
        UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
        UConverter *cnv = ಶೂನ್ಯಸೂಚಕ;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        ಆದರೆ(name != ಶೂನ್ಯಸೂಚಕ) {
            cnv = ucnv_open(name, &error_ಸಂಕೇತ);
            ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && cnv != ಶೂನ್ಯಸೂಚಕ) {
                name = ucnv_getName(cnv, &error_ಸಂಕೇತ);
            }
        }

        ಆದರೆ(name == ಶೂನ್ಯಸೂಚಕ || name[0] == 0
            || U_FAILURE(error_ಸಂಕೇತ) || cnv == ಶೂನ್ಯಸೂಚಕ
            || uprv_strlen(name)>=ಗಾತ್ರ(g_Default_Converter_Name_ಬಫರ್))
        {
            /* Panic time, let's use a fallback. */
#ಆದರೆ (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_ಅದಲುಬದಲು_LFNL_OPTION_STRING;
#ಇಲ್ಲದಿದ್ದರೆ
            name = "ibm-37_P100-1995";
#ಆದರೆ_ಅಂತ್ಯ
        }

        ಆಂತರಿಕ_ಗಣ_Name(name, &error_ಸಂಕೇತ);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    ಹಿಂತಿರುಗಿ name;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ U_CHARSET_IS_UTF8
U_CAPI ಶೂನ್ಯ U_EXPORT2 ucnv_ಗಣ_Default_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) {}
#ಇಲ್ಲದಿದ್ದರೆ
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_ಗಣ_Default_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName) {
    ಆದರೆ(converterName==ಶೂನ್ಯಸೂಚಕ) {
        /* reset to the default codepage */
        gDefaultConverterName=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
        UConverter *cnv = ಶೂನ್ಯಸೂಚಕ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = ಶೂನ್ಯಸೂಚಕ;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && cnv != ಶೂನ್ಯಸೂಚಕ) {
            name = ucnv_getName(cnv, &error_ಸಂಕೇತ);
        }

        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && name!=ಶೂನ್ಯಸೂಚಕ) {
            ಆಂತರಿಕ_ಗಣ_Name(name, &error_ಸಂಕೇತ);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#ಆದರೆ !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_ಬೈಟುಗಳು;
    uint8_t *ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    uint32_t ಸ್ಥಾನಾಂತರ, count, static_ದತ್ತಾಂಶ_ಗಾತ್ರ;
    int32_t size;

    ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ *ಒಳ_Static_ದತ್ತಾಂಶ;
    UConverter_Static_ದತ್ತಾಂಶ *ಹೊರಹೋಗು_Static_ದತ್ತಾಂಶ;

    ಸ್ಥಿರಾಂಕ _MBCSHeader *ಒಳ_MBCSHeader;
    _MBCSHeader *ಹೊರಹೋಗು_MBCSHeader;
    _MBCSHeader mbcs_ಶೀರ್ಷಿಕೆ;
    uint32_t mbcs_ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ;
    UBool noFromU=ಸುಳ್ಳು;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;

    ಸ್ಥಿರಾಂಕ int32_t *ಒಳ_Ext_Indexes;
    int32_t ext_ಸ್ಥಾನಾಂತರ;

    /* udata_swapDataHeader checks the arguments */
    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check data format and format version */
    pInfo=(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    ಆದರೆ(!(
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x6e &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x76 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x74 &&
        pInfo->format_ಆವೃತ್ತಿ[0]==6 &&
        pInfo->format_ಆವೃತ್ತಿ[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                         pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3],
                         pInfo->format_ಆವೃತ್ತಿ[0], pInfo->format_ಆವೃತ್ತಿ[1]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_ಬೈಟುಗಳು=(ಸ್ಥಿರಾಂಕ uint8_t *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಹೊರಹೋಗು_ಬೈಟುಗಳು=(ಹೊರಹೋಗು_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) ? ಶೂನ್ಯಸೂಚಕ : (uint8_t *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    ಒಳ_Static_ದತ್ತಾಂಶ=(ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ *)ಒಳ_ಬೈಟುಗಳು;
    ಹೊರಹೋಗು_Static_ದತ್ತಾಂಶ=(UConverter_Static_ದತ್ತಾಂಶ *)ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    ಆದರೆ(length<0) {
        static_ದತ್ತಾಂಶ_ಗಾತ್ರ=ds->readUInt32(ಒಳ_Static_ದತ್ತಾಂಶ->struct_ಗಾತ್ರ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        length-=ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
        ಆದರೆ( length<(int32_t)ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ) ||
            (uint32_t)length<(static_ದತ್ತಾಂಶ_ಗಾತ್ರ=ds->readUInt32(ಒಳ_Static_ದತ್ತಾಂಶ->struct_ಗಾತ್ರ))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    ಆದರೆ(length>=0) {
        /* swap the static data */
        ಆದರೆ(ಒಳ_Static_ದತ್ತಾಂಶ!=ಹೊರಹೋಗು_Static_ದತ್ತಾಂಶ) {
            uprv_memcpy(ಹೊರಹೋಗು_Static_ದತ್ತಾಂಶ, ಒಳ_Static_ದತ್ತಾಂಶ, static_ದತ್ತಾಂಶ_ಗಾತ್ರ);
        }

        ds->ಅದಲುಬದಲು_Array32(ds, &ಒಳ_Static_ದತ್ತಾಂಶ->struct_ಗಾತ್ರ, 4,
                           &ಹೊರಹೋಗು_Static_ದತ್ತಾಂಶ->struct_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);
        ds->ಅದಲುಬದಲು_Array32(ds, &ಒಳ_Static_ದತ್ತಾಂಶ->codepage, 4,
                           &ಹೊರಹೋಗು_Static_ದತ್ತಾಂಶ->codepage, p_Error_ಸಂಕೇತ);

        ds->ಅದಲುಬದಲು_Inv_Chars(ds, ಒಳ_Static_ದತ್ತಾಂಶ->name, (int32_t)uprv_strlen(ಒಳ_Static_ದತ್ತಾಂಶ->name),
                            ಹೊರಹೋಗು_Static_ದತ್ತಾಂಶ->name, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    ಒಳ_ಬೈಟುಗಳು+=static_ದತ್ತಾಂಶ_ಗಾತ್ರ;
    ಆದರೆ (ಹೊರಹೋಗು_ಬೈಟುಗಳು != ಶೂನ್ಯಸೂಚಕ) ಹೊರಹೋಗು_ಬೈಟುಗಳು+=static_ದತ್ತಾಂಶ_ಗಾತ್ರ;
    ಆದರೆ(length>=0) {
        length-=(int32_t)static_ದತ್ತಾಂಶ_ಗಾತ್ರ;
    }

    /* check for supported conversionType values */
    ಆದರೆ(ಒಳ_Static_ದತ್ತಾಂಶ->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        ಒಳ_MBCSHeader=(ಸ್ಥಿರಾಂಕ _MBCSHeader *)ಒಳ_ಬೈಟುಗಳು;
        ಹೊರಹೋಗು_MBCSHeader=(_MBCSHeader *)ಹೊರಹೋಗು_ಬೈಟುಗಳು;

        ಆದರೆ(0<=length && length<(int32_t)ಗಾತ್ರ(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ(ಒಳ_MBCSHeader->ಆವೃತ್ತಿ[0]==4 && ಒಳ_MBCSHeader->ಆವೃತ್ತಿ[1]>=1) {
            mbcs_ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ=MBCS_ಶೀರ್ಷಿಕೆ_V4_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಒಳ_MBCSHeader->ಆವೃತ್ತಿ[0]==5 && ಒಳ_MBCSHeader->ಆವೃತ್ತಿ[1]>=3 &&
                  ((mbcs_ಶೀರ್ಷಿಕೆ.options=ds->readUInt32(ಒಳ_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_ಮುಖವಾಡ)==0
        ) {
            mbcs_ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ=mbcs_ಶೀರ್ಷಿಕೆ.options&MBCS_OPT_ಉದ್ದಳತೆ_ಮುಖವಾಡ;
            noFromU = (mbcs_ಶೀರ್ಷಿಕೆ.options & MBCS_OPT_NO_FROM_U) != 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             ಒಳ_MBCSHeader->ಆವೃತ್ತಿ[0], ಒಳ_MBCSHeader->ಆವೃತ್ತಿ[1]);
            *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        uprv_memcpy(mbcs_ಶೀರ್ಷಿಕೆ.ಆವೃತ್ತಿ, ಒಳ_MBCSHeader->ಆವೃತ್ತಿ, 4);
        mbcs_ಶೀರ್ಷಿಕೆ.ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು=         ds->readUInt32(ಒಳ_MBCSHeader->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು);
        mbcs_ಶೀರ್ಷಿಕೆ.ಎಣಿಕೆ_ಗಮ್ಯ_UFallbacks=   ds->readUInt32(ಒಳ_MBCSHeader->ಎಣಿಕೆ_ಗಮ್ಯ_UFallbacks);
        mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_UCode_Units=  ds->readUInt32(ಒಳ_MBCSHeader->ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_UCode_Units);
        mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_From_UTable=    ds->readUInt32(ಒಳ_MBCSHeader->ಸ್ಥಾನಾಂತರ_From_UTable);
        mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_From_UBytes=    ds->readUInt32(ಒಳ_MBCSHeader->ಸ್ಥಾನಾಂತರ_From_UBytes);
        mbcs_ಶೀರ್ಷಿಕೆ.flags=               ds->readUInt32(ಒಳ_MBCSHeader->flags);
        mbcs_ಶೀರ್ಷಿಕೆ.from_UBytes_ಉದ್ದಳತೆ=    ds->readUInt32(ಒಳ_MBCSHeader->from_UBytes_ಉದ್ದಳತೆ);
        /* mbcsHeader.options have been read above */

        ext_ಸ್ಥಾನಾಂತರ=(int32_t)(mbcs_ಶೀರ್ಷಿಕೆ.flags>>8);
        outputType=(uint8_t)mbcs_ಶೀರ್ಷಿಕೆ.flags;
        ಆದರೆ(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        /* make sure that the output type is known */
        ಆಯ್ಕೆ(outputType) {
        ಸ್ಥಿತಿ MBCS_OUTPUT_1:
        ಸ್ಥಿತಿ MBCS_OUTPUT_2:
        ಸ್ಥಿತಿ MBCS_OUTPUT_3:
        ಸ್ಥಿತಿ MBCS_OUTPUT_4:
        ಸ್ಥಿತಿ MBCS_OUTPUT_3_EUC:
        ಸ್ಥಿತಿ MBCS_OUTPUT_4_EUC:
        ಸ್ಥಿತಿ MBCS_OUTPUT_2_SISO:
        ಸ್ಥಿತಿ MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcs_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=0;
        ಆದರೆ( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_ಶೀರ್ಷಿಕೆ.ಆವೃತ್ತಿ[1]>=3 && (maxFastUChar=mbcs_ಶೀರ್ಷಿಕೆ.ಆವೃತ್ತಿ[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        ಆದರೆ(ext_ಸ್ಥಾನಾಂತರ==0) {
            size=(int32_t)(mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_From_UBytes+mbcs_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
            ಆದರೆ(!noFromU) {
                size+=(int32_t)mbcs_ಶೀರ್ಷಿಕೆ.from_UBytes_ಉದ್ದಳತೆ;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            ಒಳ_Ext_Indexes=ಶೂನ್ಯಸೂಚಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* there is extension data after the base data, see ucnv_ext.h */
            ಆದರೆ(length>=0 && length<(ext_ಸ್ಥಾನಾಂತರ+UCNV_EXT_INDEXES_MIN_ಉದ್ದಳತೆ*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }

            ಒಳ_Ext_Indexes=(ಸ್ಥಿರಾಂಕ int32_t *)(ಒಳ_ಬೈಟುಗಳು+ext_ಸ್ಥಾನಾಂತರ);
            size=ext_ಸ್ಥಾನಾಂತರ+udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_ಗಾತ್ರ]);
        }

        ಆದರೆ(length>=0) {
            ಆದರೆ(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }

            /* copy the data for inaccessible bytes */
            ಆದರೆ(ಒಳ_ಬೈಟುಗಳು!=ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
                uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ*4;
            ds->ಅದಲುಬದಲು_Array32(ds, &ಒಳ_MBCSHeader->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು, count-4,
                               &ಹೊರಹೋಗು_MBCSHeader->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು, p_Error_ಸಂಕೇತ);

            ಆದರೆ(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_ತಳ_Name=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ಬೈಟುಗಳು+count;
                ಅಕ್ಷರ *ಹೊರಹೋಗು_ತಳ_Name=(ಅಕ್ಷರ *)ಹೊರಹೋಗು_ಬೈಟುಗಳು+count;
                ds->ಅದಲುಬದಲು_Inv_Chars(ds, ಒಳ_ತಳ_Name, (int32_t)uprv_strlen(ಒಳ_ತಳ_Name),
                                    ಹೊರಹೋಗು_ತಳ_Name, p_Error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                ಸ್ಥಾನಾಂತರ=count;
                count=mbcs_ಶೀರ್ಷಿಕೆ.ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು*1024;
                ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                   ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* swap the toUFallbacks[] */
                ಸ್ಥಾನಾಂತರ+=count;
                count=mbcs_ಶೀರ್ಷಿಕೆ.ಎಣಿಕೆ_ಗಮ್ಯ_UFallbacks*8;
                ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                   ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* swap the unicodeCodeUnits[] */
                ಸ್ಥಾನಾಂತರ=mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_UCode_Units;
                count=mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_From_UTable-ಸ್ಥಾನಾಂತರ;
                ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                   ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* offset to the stage 1 table, independent of the outputType */
                ಸ್ಥಾನಾಂತರ=mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_From_UTable;

                ಆದರೆ(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_From_UBytes-ಸ್ಥಾನಾಂತರ)+mbcs_ಶೀರ್ಷಿಕೆ.from_UBytes_ಉದ್ದಳತೆ;
                    ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                       ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    ಆದರೆ(ಒಳ_Static_ದತ್ತಾಂಶ->unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        count=0x40*2; /* only BMP */
                    }
                    ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                       ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                    /* stage 2 table: uint32_t[] */
                    ಸ್ಥಾನಾಂತರ+=count;
                    count=mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_From_UBytes-ಸ್ಥಾನಾಂತರ;
                    ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                       ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    ಸ್ಥಾನಾಂತರ=mbcs_ಶೀರ್ಷಿಕೆ.ಸ್ಥಾನಾಂತರ_From_UBytes;
                    count= noFromU ? 0 : mbcs_ಶೀರ್ಷಿಕೆ.from_UBytes_ಉದ್ದಳತೆ;
                    ಆಯ್ಕೆ(outputType) {
                    ಸ್ಥಿತಿ MBCS_OUTPUT_2:
                    ಸ್ಥಿತಿ MBCS_OUTPUT_3_EUC:
                    ಸ್ಥಿತಿ MBCS_OUTPUT_2_SISO:
                        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                           ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ MBCS_OUTPUT_4:
                        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                           ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
                        ನಿಲ್ಲಿಸು;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* just uint8_t[], nothing to swap */
                        ನಿಲ್ಲಿಸು;
                    }

                    ಆದರೆ(mbcs_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ!=0) {
                        ಸ್ಥಾನಾಂತರ+=count;
                        count=mbcs_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
                        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, (int32_t)count,
                                           ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
                    }
                }
            }

            ಆದರೆ(ext_ಸ್ಥಾನಾಂತರ!=0) {
                /* swap the extension data */
                ಒಳ_ಬೈಟುಗಳು+=ext_ಸ್ಥಾನಾಂತರ;
                ಹೊರಹೋಗು_ಬೈಟುಗಳು+=ext_ಸ್ಥಾನಾಂತರ;

                /* swap toUTable[] */
                ಸ್ಥಾನಾಂತರ=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_ಗಮ್ಯ_U_ಸೂಚ್ಯಂಕ]);
                length=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_ಗಮ್ಯ_U_ಉದ್ದಳತೆ]);
                ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, length*4, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* swap toUUChars[] */
                ಸ್ಥಾನಾಂತರ=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_ಗಮ್ಯ_U_UCHARS_ಸೂಚ್ಯಂಕ]);
                length=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_ಗಮ್ಯ_U_UCHARS_ಉದ್ದಳತೆ]);
                ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, length*2, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* swap fromUTableUChars[] */
                ಸ್ಥಾನಾಂತರ=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_ಸೂಚ್ಯಂಕ]);
                length=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_ಉದ್ದಳತೆ]);
                ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, length*2, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* swap fromUTableValues[] */
                ಸ್ಥಾನಾಂತರ=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ]);
                /* same length as for fromUTableUChars[] */
                ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, length*4, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                ಸ್ಥಾನಾಂತರ=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_ಸೂಚ್ಯಂಕ]);
                length=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_ಉದ್ದಳತೆ]);
                ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, length*2, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* swap fromUStage3[] */
                ಸ್ಥಾನಾಂತರ=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_ಸೂಚ್ಯಂಕ]);
                length=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_ಉದ್ದಳತೆ]);
                ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, length*2, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* swap fromUStage3b[] */
                ಸ್ಥಾನಾಂತರ=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_ಸೂಚ್ಯಂಕ]);
                length=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_ಉದ್ದಳತೆ]);
                ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, length*4, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

                /* swap indexes[] */
                length=udata_readInt32(ds, ಒಳ_Ext_Indexes[UCNV_EXT_INDEXES_ಉದ್ದಳತೆ]);
                ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು, length*4, ಹೊರಹೋಗು_ಬೈಟುಗಳು, p_Error_ಸಂಕೇತ);
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         ಒಳ_Static_ದತ್ತಾಂಶ->conversionType);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+(int32_t)static_ದತ್ತಾಂಶ_ಗಾತ್ರ+size;
}

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#ಆದರೆ_ಅಂತ್ಯ
